/*
 *   Copyright 2008,2010 Davide Bettio <davide.bettio@kdemail.net>
 *   Copyright 2009 John Layt <john@layt.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "calendartable.h"

//Qt
#include <QtCore/qdatetime.h>
#include <QtCore/qlist.h>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QWidget>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qstyleoption.h>

//KDECore
#include <KGlobal>
#include <KDateTime>
#include <KDebug>
#include <KConfigDialog>
#include <KConfigGroup>

//Plasma
#include <Plasma/Svg>
#include <Plasma/Theme>
#include <Plasma/DataEngine>
#include <Plasma/DataEngineManager>

#include "ui_calendarConfig.h"

#include <cmath>

namespace Plasma
{

static const int DISPLAYED_WEEKS = 6;

class CalendarCellBorder
{
public:
    CalendarCellBorder(int c, int w, int d, CalendarTable::CellTypes t, QDate dt)
        : cell(c),
          week(w),
          weekDay(d),
          type(t),
          date(dt)
    {
    }

    int cell;
    int week;
    int weekDay;
    CalendarTable::CellTypes type;
    QDate date;
};

class CalendarTablePrivate
{
    public:
        CalendarTablePrivate(CalendarTable *calTable, const QDate &initialDate = QDate::currentDate())
            : q(calTable),
              calendarType(-1),
              calendar(0),
              automaticUpdates(true),
              opacity(0.5),
              hoverWeekRow(-1),
              hoverWeekdayColumn(-1)
        {
            svg = new Svg();
            svg->setImagePath("widgets/calendar");
            svg->setContainsMultipleImages(true);

            setDate(initialDate);
        }

        ~CalendarTablePrivate()
        {
            // Delete the old calendar first if it's not the global calendar
            if (calendar != KGlobal::locale()->calendar()) {
                delete calendar;
            }

            delete svg;
        }

        void setCalendar(const KCalendarSystem *newCalendar)
        {
            // If not the global calendar, delete the old calendar first
            if (calendar != KGlobal::locale()->calendar()) {
                delete calendar;
            }

            calendar = newCalendar;

            if (calendar == KGlobal::locale()->calendar()) {
                calendarType = -1;
                QObject::connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), q, SLOT(settingsChanged(int)));
            } else {
                calendarType = q->calendar()->calendarSystem();
                QObject::disconnect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), q, SLOT(settingsChanged(int)));
            }

            // Force date update to refresh cached date componants then update display
            setDate(selectedDate);
            updateHoveredPainting(QPointF());
            q->update();
        }

        void setDate(const QDate &setDate)
        {
            //q->calendar() cannot be used because d is still not assigned
            const KCalendarSystem *cal = calendar ? calendar : KGlobal::locale()->calendar();
            selectedDate = setDate;
            selectedMonth = cal->month(setDate);
            selectedYear = cal->year(setDate);
            weekDayFirstOfSelectedMonth = weekDayFirstOfMonth(setDate);
            daysInWeek = cal->daysInWeek(setDate);
            daysInSelectedMonth = cal->daysInMonth(setDate);
            daysShownInPrevMonth = (weekDayFirstOfSelectedMonth - cal->weekStartDay() + daysInWeek) % daysInWeek;
            // make sure at least one day of the previous month is visible.
            // 1 = minimum number of days to show, increase if more days should be forced visible:
            if (daysShownInPrevMonth < 1) {
                daysShownInPrevMonth += daysInWeek;
            }
            viewStartDate = dateFromRowColumn(0, 0);
            viewEndDate = dateFromRowColumn(DISPLAYED_WEEKS - 1, daysInWeek - 1);
        }

        //Returns the x co-ordinate of a given column to LTR order, column is 0 to (daysInWeek-1)
        //This version does not adjust for RTL, so should not be used directly for drawing
        int columnToX(int column)
        {
            return q->boundingRect().x() +
                   centeringSpace +
                   weekBarSpace +
                   cellW +
                   ((cellW + cellSpace) * column);
        }

        //Returns the y co-ordinate for given row, row is 0 to (DISPLAYED_WEEKS - 1)
        int rowToY(int row)
        {
            return (int) q->boundingRect().y() +
                         headerHeight +
                         headerSpace +
                         ((cellH + cellSpace) * row);
        }

        //Returns the absolute LTR column for a given x co-ordinate, -1 if outside table
        int xToColumn(qreal x)
        {
            if (x >= columnToX(0) && x < columnToX(daysInWeek)) {
                return ((x - centeringSpace) / (cellW + cellSpace)) - 1;
            }
            return -1;
        }

        //Returns the absolute row for a given y co-ordinate, -1 if outside table
        int yToRow(qreal y)
        {
            if (y >= rowToY(0) && y < rowToY(DISPLAYED_WEEKS)) {
                return (y - headerHeight - headerSpace) / (cellH + cellSpace);
            }
            return -1;
        }

        //Convert between column and weekdayColumn depending on LTR or RTL mode
        //Note the same calculation used in both directions
        int adjustColumn(int column)
        {
            if (column >= 0 && column < daysInWeek) {
                if (q->layoutDirection() == Qt::RightToLeft) {
                    return daysInWeek - column - 1;
                } else {
                    return column;
                }
            }
            return -1;
        }

        //Given an x y point in the table return the cell date.
        //Note can be an invalid date in the calendar system
        QDate dateFromPoint(QPointF point)
        {
            if (point.isNull()) {
                return QDate();
            }

            int column = xToColumn(point.x());
            int row = yToRow(point.y());

            if (column < 0 || column >= daysInWeek || row < 0 || row >= DISPLAYED_WEEKS) {
                return QDate();
            }

            return dateFromRowColumn(row, adjustColumn(column));
        }

        //Given a date in the currently selected month, return the position in the table as a
        //row and column. Note no direction is assumed
        void rowColumnFromDate(const QDate &cellDate, int &weekRow, int &weekdayColumn)
        {
            int offset = q->calendar()->day(cellDate) + daysShownInPrevMonth - 1;
            weekRow = offset / daysInWeek;
            weekdayColumn = offset % daysInWeek;
        }

        //Given a position in the table as a 0-indexed row and column, return the cell date.  Makes
        //no assumption about direction.  Date returned can be an invalid date in the calendar
        //system, or simply invalid.
        QDate dateFromRowColumn(int weekRow, int weekdayColumn)
        {
            QDate cellDate;
            //q->calendar() cannot be used because d is still not assigned
            const KCalendarSystem *cal = calendar ? calendar : KGlobal::locale()->calendar();

            //starting from the first of the month, which is known to always be valid, add/subtract
            //number of days to get to the required cell
            if (cal->setDate(cellDate, selectedYear, selectedMonth, 1)) {
                cellDate = cal->addDays(cellDate, (weekRow * daysInWeek) + weekdayColumn - daysShownInPrevMonth);
            }

            return cellDate;
        }

        void updateHoveredPainting(const QPointF &hoverPoint)
        {
            QRectF oldHoverRect = hoverRect;
            hoverRect = QRectF();
            hoverWeekdayColumn = -1;
            hoverWeekRow = -1;

            if (!hoverPoint.isNull()) {
                int column = xToColumn(hoverPoint.x());
                int row = yToRow(hoverPoint.y());

                if (column >= 0 && column < daysInWeek && row >= 0 && row < DISPLAYED_WEEKS) {
                    hoverRect = QRectF(columnToX(column) - glowRadius,
                                       rowToY(row) - glowRadius,
                                       cellW + glowRadius * 2,
                                       cellH + glowRadius * 2).adjusted(-2,-2,2,2);
                    hoverWeekdayColumn = adjustColumn(column);
                    hoverWeekRow = row;
                }
            }

            // now update what is needed, and only what is needed!
            if (hoverRect != oldHoverRect) {
                if (oldHoverRect.isValid()) {
                    q->update(oldHoverRect);
                }

                if (hoverRect.isValid()) {
                    q->update(hoverRect);
                }

                if (hoverWeekRow >= 0 && hoverWeekdayColumn >= 0) {
                    const QDate date = dateFromRowColumn(hoverWeekRow, hoverWeekdayColumn);
                    emit q->dateHovered(date);
                } else {
                    emit q->dateHovered(QDate());
                }
            }
        }

        // calculate weekday number of first day of this month, this is the anchor for all calculations
        int weekDayFirstOfMonth(const QDate &cellDate)
        {
            //q->calendar() cannot be used because d is still not assigned
            const KCalendarSystem *cal = calendar ? calendar : KGlobal::locale()->calendar();
            Q_UNUSED(cellDate);
            QDate firstDayOfMonth;
            int weekday = -1;
            if ( cal->setDate(firstDayOfMonth, selectedYear, selectedMonth, 1)) {
                weekday = cal->dayOfWeek(firstDayOfMonth);
            }
            return weekday;
        }

        void settingsChanged(int category);

        CalendarTable *q;
        int calendarType;
        const KCalendarSystem *calendar;

        QDate selectedDate;
        QDate currentDate;
        int selectedMonth;
        int selectedYear;
        int weekDayFirstOfSelectedMonth;
        int daysInWeek;
        int daysInSelectedMonth;
        int daysShownInPrevMonth;
        QDate viewStartDate;
        QDate viewEndDate;

        bool automaticUpdates;

        QPointF lastSeenMousePos;

        Ui::calendarConfig calendarConfigUi;

        Plasma::Svg *svg;
        float opacity; //transparency for the inactive text
        QRectF hoverRect;
        int hoverWeekRow;
        int hoverWeekdayColumn;
        int centeringSpace;
        int cellW;
        int cellH;
        int cellSpace;
        int headerHeight;
        int headerSpace;
        int weekBarSpace;
        int glowRadius;

        QFont weekDayFont;
        QFont dateFontBold;
        QFont dateFont;
};

CalendarTable::CalendarTable(const QDate &date, QGraphicsWidget *parent)
    : QGraphicsWidget(parent), d(new CalendarTablePrivate(this, date))
{
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

CalendarTable::CalendarTable(QGraphicsWidget *parent)
    : QGraphicsWidget(parent), d(new CalendarTablePrivate(this))
{
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

CalendarTable::~CalendarTable()
{
    delete d;
}

void CalendarTable::setCalendar(int newCalendarType)
{
    if (newCalendarType == d->calendarType) {
        return;
    }

    if (newCalendarType == -1) {
        d->setCalendar(KGlobal::locale()->calendar());
    } else {
        d->setCalendar(KCalendarSystem::create(static_cast<KLocale::CalendarSystem>(newCalendarType)));
    }

    // Signal out date change so any dependents will update as well
    emit dateChanged(date(), date());
    emit dateChanged(date());
}

void CalendarTable::setCalendar(const KCalendarSystem *newCalendar)
{
    if (newCalendar == d->calendar) {
        return;
    }

    d->setCalendar(newCalendar);

    // Signal out date change so any dependents will update as well
    emit dateChanged(date(), date());
    emit dateChanged(date());
}

const KCalendarSystem *CalendarTable::calendar() const
{
    if (d->calendar) {
        return d->calendar;
    } else {
        return KGlobal::locale()->calendar();
    }
}

void CalendarTable::setDate(const QDate &newDate)
{
    // New date must be valid in the current calendar system
    if (!calendar()->isValid(newDate)) {
        return;
    }

    // If new date is the same as old date don't actually need to do anything
    if (newDate == date()) {
        return;
    }

    int oldYear = d->selectedYear;
    int oldMonth = d->selectedMonth;
    QDate oldDate = date();

    // now change the date
    d->setDate(newDate);

    d->updateHoveredPainting(d->lastSeenMousePos);
    update(); //FIXME: we shouldn't need this update here, but something in Qt is broken (but with plasmoidviewer everything work)

    if (oldYear == d->selectedYear && oldMonth == d->selectedMonth) {
        // only update the old and the new areas
        int row, column;
        d->rowColumnFromDate(oldDate, row, column);
        update(cellX(column) - d->glowRadius, cellY(row) - d->glowRadius,
               d->cellW + d->glowRadius * 2, d->cellH + d->glowRadius * 2);

        d->rowColumnFromDate(newDate, row, column);
        update(cellX(column) - d->glowRadius, cellY(row) - d->glowRadius,
               d->cellW + d->glowRadius * 2, d->cellH + d->glowRadius * 2);
    }

    emit dateChanged(newDate, oldDate);
    emit dateChanged(newDate);
}

const QDate& CalendarTable::date() const
{
    return d->selectedDate;
}

void CalendarTable::setAutomaticUpdateEnabled(bool enabled)
{
    d->automaticUpdates = enabled;
}

bool CalendarTable::isAutomaticUpdateEnabled() const
{
    return d->automaticUpdates;
}

void CalendarTable::setCurrentDate(const QDate &date)
{
    d->currentDate = date;
}

const QDate& CalendarTable::currentDate() const
{
    return d->currentDate;
}

QDate CalendarTable::startDate() const
{
    return d->viewStartDate;
}

QDate CalendarTable::endDate() const
{
    return d->viewEndDate;
}

void CalendarTablePrivate::settingsChanged(int category)
{
    if (category != KGlobalSettings::SETTINGS_LOCALE) {
        return;
    }

    calendar = KGlobal::locale()->calendar();
    q->update();
}

void CalendarTable::dataUpdated(const QString &source, const Plasma::DataEngine::Data &data)
{
    Q_UNUSED(source)
    Q_UNUSED(data)
    update();
}

void CalendarTable::applyConfiguration(KConfigGroup cg)
{
    // convert pre 4.11 value if needed
    const QVariant oldCalendarTypeValue = cg.readEntry("calendarType", QVariant());

    if (oldCalendarTypeValue.type() == QVariant::String) {
        const QString oldCalendarType = oldCalendarTypeValue.toString();
        int newCalendarType = -1;

        if (oldCalendarType == "gregorian") {
            newCalendarType = KLocale::GregorianCalendar;
        } else if (oldCalendarType == "coptic") {
            newCalendarType = KLocale::CopticCalendar;
        } else if (oldCalendarType == "ethiopian") {
            newCalendarType = KLocale::EthiopianCalendar;
        } else if (oldCalendarType == "hebrew") {
            newCalendarType = KLocale::HebrewCalendar;
        } else if (oldCalendarType == "hijri") {
            newCalendarType = KLocale::IslamicCivilCalendar;
        } else if (oldCalendarType == "indian-national") {
            newCalendarType = KLocale::IndianNationalCalendar;
        } else if (oldCalendarType == "jalali") {
            newCalendarType = KLocale::JalaliCalendar;
        } else if (oldCalendarType == "japanese") {
            newCalendarType = KLocale::JapaneseCalendar;
        } else if (oldCalendarType == "julian") {
            newCalendarType = KLocale::JulianCalendar;
        } else if (oldCalendarType == "minguo") {
            newCalendarType = KLocale::MinguoCalendar;
        } else if (oldCalendarType == "thai") {
            newCalendarType = KLocale::ThaiCalendar;
        }

        cg.writeEntry("calendarType", newCalendarType);
    }

    setCalendar(cg.readEntry("calendarType", -1));
}

void CalendarTable::writeConfiguration(KConfigGroup cg)
{
    cg.writeEntry("calendarType", d->calendarType);
}

void CalendarTable::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *calendarConfigWidget = new QWidget();
    d->calendarConfigUi.setupUi(calendarConfigWidget);
    parent->addPage(calendarConfigWidget, i18n("Calendar"), "view-pim-calendar");

    const QList<KLocale::CalendarSystem> calendars = KCalendarSystem::calendarSystemsList();
    d->calendarConfigUi.calendarComboBox->addItem( i18n("Local"), QVariant( -1 ) );
    for (int  i = 0; i < calendars.count(); ++i) {
        d->calendarConfigUi.calendarComboBox->addItem( KCalendarSystem::calendarLabel( calendars.at(i) ), QVariant( calendars.at(i) ) );
    }
    d->calendarConfigUi.calendarComboBox->setCurrentIndex( d->calendarConfigUi.calendarComboBox->findData( QVariant( d->calendarType ) ) );

    connect(d->calendarConfigUi.calendarComboBox, SIGNAL(activated(int)), parent, SLOT(settingsModified()));
}

void CalendarTable::configAccepted(KConfigGroup cg)
{
    setCalendar(d->calendarConfigUi.calendarComboBox->itemData(d->calendarConfigUi.calendarComboBox->currentIndex()).toInt());

    writeConfiguration(cg);
}

//Returns the x co-ordinate for drawing the day cell on the widget given the weekday column
//Note weekdayColumn is 0 to (daysInWeek -1) and is not a real weekDay number (i.e. NOT Monday=1).
//Adjusts automatically for RTL mode, so don't use to calculate absolute positions
int CalendarTable::cellX(int weekdayColumn)
{
    return d->columnToX(d->adjustColumn(weekdayColumn));
}

//Returns the y co-ordinate for drawing the day cell on the widget given the weekRow
//weekRow is 0 to (DISPLAYED_WEEKS - 1)
int CalendarTable::cellY(int weekRow)
{
    return d->rowToY(weekRow);
}

void CalendarTable::wheelEvent(QGraphicsSceneWheelEvent * event)
{
    if (event->delta() < 0) {
        setDate(calendar()->addMonths(date(), 1));
    } else if (event->delta() > 0) {
        setDate(calendar()->addMonths(date(), -1));
    }
}

void CalendarTable::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    d->lastSeenMousePos = event->pos();

    event->accept();
    QDate date = d->dateFromPoint(event->pos());
    setDate(date);
    emit dateSelected(date);
}

void CalendarTable::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    mousePressEvent(event);
}

void CalendarTable::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    d->lastSeenMousePos = event->pos();

    emit tableClicked();
}

void CalendarTable::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    d->lastSeenMousePos = event->pos();

    d->updateHoveredPainting(event->pos());
}

void CalendarTable::resizeEvent(QGraphicsSceneResizeEvent * event)
{
    Q_UNUSED(event);

    QRectF r = contentsRect();
    int numCols = d->daysInWeek + 1;
    int rectSizeH = int(r.height() / (DISPLAYED_WEEKS + 1));
    int rectSizeW = int(r.width() / numCols);

    //Using integers to help to keep things aligned to the grid
    //kDebug() << r.width() << rectSize;
    d->cellSpace = qMax(1, qMin(4, qMin(rectSizeH, rectSizeW) / 20));
    d->headerSpace = d->cellSpace * 2;
    d->weekBarSpace = d->cellSpace * 2 + 1;
    d->cellH = rectSizeH - d->cellSpace;
    d->cellW = rectSizeW - d->cellSpace;
    d->glowRadius = d->cellW * .1;
    d->headerHeight = (int) (d->cellH / 1.5);
    d->centeringSpace = qMax(0, int((r.width() - (rectSizeW * numCols) - (d->cellSpace * (numCols -1))) / 2));

    // Relative to the cell size
    const qreal weekSize = .75;
    const qreal dateSize = .8;

    d->weekDayFont = Theme::defaultTheme()->font(Plasma::Theme::DefaultFont);
    d->weekDayFont.setPixelSize(d->cellH * weekSize);

    d->dateFontBold = Theme::defaultTheme()->font(Plasma::Theme::DefaultFont);
    d->dateFontBold.setPixelSize(d->cellH * dateSize);
    d->dateFontBold.setBold(true);

    d->dateFont = Theme::defaultTheme()->font(Plasma::Theme::DefaultFont);

    QFontMetrics fm(d->weekDayFont);

    int width = 0;
    for (int i = 0; i < d->daysInWeek; i++) {
        const QString name = calendar()->weekDayName(i, KCalendarSystem::ShortDayName);
        width = qMax(width, fm.width(name));
    }

    if (width > d->cellW * weekSize) {
        d->weekDayFont.setPixelSize(d->weekDayFont.pixelSize() * ((d->cellW * weekSize) / width));
    }

    fm = QFontMetrics(d->dateFontBold);
    width = 0;
    for (int i = 10; i <= 52; i++) {
        width = qMax(width, fm.width(QString::number(i)));
    }

    if (width > d->cellW * dateSize) {
        d->dateFontBold.setPixelSize(d->dateFontBold.pixelSize() * ((d->cellW * dateSize) / width));
    }

    d->dateFont.setPixelSize(d->dateFontBold.pixelSize());
}

void CalendarTable::paintCell(QPainter *p, int cell, int weekRow, int weekdayColumn, CellTypes type, const QDate &cellDate)
{
    Q_UNUSED(cell);

    QString cellSuffix = type & NotInCurrentMonth ? "inactive" : "active";
    QRectF cellArea = QRectF(cellX(weekdayColumn), cellY(weekRow), d->cellW, d->cellH);

    d->svg->paint(p, cellArea, cellSuffix); // draw background

    QColor numberColor = Theme::defaultTheme()->color(Plasma::Theme::TextColor);
    if (type & NotInCurrentMonth || type & InvalidDate) {
        p->setOpacity(d->opacity);
    }

    p->setPen(numberColor);
    p->setFont(d->dateFont);
    if (!(type & InvalidDate)) {
        p->drawText(cellArea, Qt::AlignCenter, calendar()->formatDate(cellDate, KLocale::Day, KLocale::ShortNumber), &cellArea); //draw number
    }
    p->setOpacity(1.0);
}

void CalendarTable::paintBorder(QPainter *p, int cell, int weekRow, int weekdayColumn, CellTypes type, const QDate &cellDate)
{
    Q_UNUSED(cell);
    Q_UNUSED(cellDate);

    if (type & Hovered) {
        d->svg->paint(p, QRect(cellX(weekdayColumn), cellY(weekRow), d->cellW, d->cellH), "hoverHighlight");
    }

    QString elementId;

    if (type & Today) {
        elementId = "today";
    } else if (type & Selected) {
        elementId = "selected";
    } else {
        return;
    }

    d->svg->paint(p, QRectF(cellX(weekdayColumn) - 1, cellY(weekRow) - 1, d->cellW + 1, d->cellH + 2), elementId);
}

void CalendarTable::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    // Draw weeks numbers column and day header
    QRectF r = boundingRect();
    d->svg->paint(p, QRectF(r.x() + d->centeringSpace, cellY(0), d->cellW,
                  cellY(DISPLAYED_WEEKS) - cellY(0) - d->cellSpace),  "weeksColumn");
    d->svg->paint(p, QRectF(r.x() + d->centeringSpace, r.y(),
                  cellX(d->daysInWeek) - r.x() - d->cellSpace - d->centeringSpace, d->headerHeight), "weekDayHeader");

    QList<CalendarCellBorder> borders;
    QList<CalendarCellBorder> hovers;
    if (d->automaticUpdates) {
        d->currentDate = QDate::currentDate();
    }

    //weekRow and weekDaycolumn of table are 0 indexed and are not equivalent to weekday or week
    //numbers.  In LTR mode we count/paint row and column from top-left corner, in RTL mode we
    //count/paint from top-right corner, but we don't need to know as cellX() calculates the actual
    //painting position for us depending on the mode.
    for (int weekRow = 0; weekRow < DISPLAYED_WEEKS; weekRow++) {
        for (int weekdayColumn = 0; weekdayColumn < d->daysInWeek; weekdayColumn++) {

            int x = cellX(weekdayColumn);
            int y = cellY(weekRow);

            QRectF cellRect(x, y, d->cellW, d->cellH);
            if (!cellRect.intersects(option->exposedRect)) {
                continue;
            }

            QDate cellDate = d->dateFromRowColumn(weekRow, weekdayColumn);
            CalendarTable::CellTypes type(CalendarTable::NoType);
            // get cell info
            const int cellDay = calendar()->day(cellDate);

            // check what kind of cell we are
            if (calendar()->month(cellDate) != d->selectedMonth) {
                type |= CalendarTable::NotInCurrentMonth;
            }

            if (!calendar()->isValid(cellDate)) {
                type |= CalendarTable::InvalidDate;
            }

            if (cellDate == d->currentDate) {
                type |= CalendarTable::Today;
            }

            if (cellDate == date()) {
                type |= CalendarTable::Selected;
            }

            if (type != CalendarTable::NoType && type != CalendarTable::NotInCurrentMonth) {
                borders.append(CalendarCellBorder(cellDay, weekRow, weekdayColumn, type, cellDate));
            }

            if (weekRow == d->hoverWeekRow && weekdayColumn == d->hoverWeekdayColumn) {
                type |= CalendarTable::Hovered;
                hovers.append(CalendarCellBorder(cellDay, weekRow, weekdayColumn, type, cellDate));
            }

            paintCell(p, cellDay, weekRow, weekdayColumn, type, cellDate);

            // FIXME: modify svg to allow for a wider week number cell
            // a temporary workaround is to paint only one week number (weekString) when the cell is small
            // and both week numbers (accurateWeekString) when there is enough room
            if (weekdayColumn == 0) {
                QRectF cellRect(r.x() + d->centeringSpace, y, d->cellW, d->cellH);
                p->setPen(Theme::defaultTheme()->color(Plasma::Theme::TextColor));
                p->setFont(d->dateFont);
                p->setOpacity(d->opacity);
                QString weekString;
                QString accurateWeekString;
                if (calendar()->isValid(cellDate)) {
                    weekString = calendar()->formatDate(cellDate, KLocale::Week, KLocale::LongNumber);
                    accurateWeekString = weekString;
                    if (calendar()->dayOfWeek(cellDate) != Qt::Monday) {
                        QDate nextWeekDate = calendar()->addDays(cellDate, d->daysInWeek);
                        if (calendar()->isValid(nextWeekDate)) {
                            if (layoutDirection() == Qt::RightToLeft) {
                                accurateWeekString.prepend("/").prepend(calendar()->formatDate(nextWeekDate, KLocale::Week, KLocale::LongNumber));
                            } else {
                                accurateWeekString.append("/").append(calendar()->formatDate(nextWeekDate, KLocale::Week, KLocale::LongNumber));
                            }
                        }
                        // ensure that weekString is the week number that has the most amout of days in the row
                        QDate middleWeekDate = calendar()->addDays(cellDate, floor(static_cast<float>(d->daysInWeek / 2)));
                        if (calendar()->isValid(middleWeekDate)) {
                            QString middleWeekString = calendar()->formatDate(middleWeekDate, KLocale::Week, KLocale::LongNumber);
                            if (weekString != middleWeekString) {
                                weekString = middleWeekString;
                            }
                        }
                    }
                }
                QFontMetrics fontMetrics(d->dateFont);
                if (fontMetrics.width(accurateWeekString) > d->cellW) {
                    p->drawText(cellRect, Qt::AlignCenter, weekString); //draw number
                } else {
                    p->drawText(cellRect, Qt::AlignCenter, accurateWeekString); //draw number
                }
                p->setOpacity(1.0);
            }
        }
    }

    // Draw days
    if (option->exposedRect.intersects(QRect(r.x(), r.y(), r.width(), d->headerHeight))) {
        p->setPen(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
        int weekStartDay = calendar()->weekStartDay();
        for (int i = 0; i < d->daysInWeek; i++){
            int weekDay = ((i + weekStartDay - 1) % d->daysInWeek) + 1;
            QString dayName = calendar()->weekDayName(weekDay, KCalendarSystem::ShortDayName);
            p->setFont(d->weekDayFont);
            p->drawText(QRectF(cellX(i), r.y(), d->cellW, d->headerHeight),
                        Qt::AlignCenter | Qt::AlignVCenter, dayName);
        }
    }

    // Draw hovers
    foreach (const CalendarCellBorder &border, hovers) {
        p->save();
        paintBorder(p, border.cell, border.week, border.weekDay, border.type, border.date);
        p->restore();
    }

    // Draw borders
    foreach (const CalendarCellBorder &border, borders) {
        p->save();
        paintBorder(p, border.cell, border.week, border.weekDay, border.type, border.date);
        p->restore();
    }
}

} //namespace Plasma

#include "moc_calendartable.cpp"
