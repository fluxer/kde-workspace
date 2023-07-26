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

#include "calendar.h"

//Qt
#include <QtCore/qdatetime.h>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/QGraphicsGridLayout>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QDesktopWidget>

//KDECore
#include <KCalendarWidget>
#include <KDebug>
#include <KGlobal>
#include <KIcon>
#include <KLineEdit>
#include <KLocale>
#include <KConfigDialog>
#include <KConfigGroup>
#include <KNotification>

//Plasma
#include <Plasma/Label>
#include <Plasma/LineEdit>
#include <Plasma/Separator>
#include <Plasma/SpinBox>
#include <Plasma/TextBrowser>
#include <Plasma/ToolButton>
#include <Plasma/CalendarWidget>
#include <Plasma/DataEngine>

#include "wheelytoolbutton.h"

namespace Plasma
{

static const int s_yearWidgetIndex = 3;

class CalendarPrivate
{
    public:
        CalendarPrivate(Calendar *calendar)
            : q(calendar),
              calendarWidget(nullptr),
              layout(nullptr),
              currentDate(QDate::currentDate()),
              automaticUpdates(true)
        {
        }

        void init(const QDate &date = QDate());
        void popupMonthsMenu();
        void updateSize();

        Calendar *q;
        Plasma::CalendarWidget *calendarWidget;
        QGraphicsLinearLayout *layout;
        QDate currentDate;
        bool automaticUpdates;
};

Calendar::Calendar(const QDate &date, QGraphicsWidget *parent)
    : QGraphicsWidget(parent),
      d(new CalendarPrivate(this))
{
    d->init(date);
}

Calendar::Calendar(QGraphicsWidget *parent)
    : QGraphicsWidget(parent),
      d(new CalendarPrivate(this))
{
    d->init();
}

Calendar::~Calendar()
{
   delete d;
}

void CalendarPrivate::init(const QDate &initialDate)
{
    q->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    layout = new QGraphicsLinearLayout(Qt::Horizontal, q);

    calendarWidget = new CalendarWidget(q);
    calendarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QObject::connect(calendarWidget, SIGNAL(clicked(QDate)), q, SLOT(dateUpdated()));
    QObject::connect(calendarWidget, SIGNAL(activated(QDate)), q, SLOT(dateUpdated()));

    layout->addItem(calendarWidget);

    q->setDate(initialDate);
    updateSize();
}

void Calendar::showEvent(QShowEvent * event)
{
    if (d->automaticUpdates) {
        d->currentDate = QDate::currentDate();
    }
    QGraphicsWidget::showEvent(event);
}

void Calendar::focusInEvent(QFocusEvent* event)
{
    Q_UNUSED(event);
    d->calendarWidget->setFocus();
}

void Calendar::setAutomaticUpdateEnabled(bool automatic)
{
    d->automaticUpdates = automatic;
}
 
bool Calendar::isAutomaticUpdateEnabled() const
{
    return d->automaticUpdates;
}

void Calendar::setDate(const QDate &toDate)
{
    // New date must be valid one
    if (!toDate.isValid()) {
        if (!toDate.isNull()) {
            KNotification::beep();
        }
        return;
    }

    // NOTE: this method is called by Plasma::ClockApplet::popupEvent() on show so updating as if
    // show event ocurred
    if (d->automaticUpdates) {
        d->currentDate = QDate::currentDate();
    }

    d->calendarWidget->setSelectedDate(toDate);
}

QDate Calendar::date() const
{
    return d->calendarWidget->selectedDate();
}

void Calendar::setCurrentDate(const QDate &date)
{
    d->currentDate = date;
}

QDate Calendar::currentDate() const
{
    return d->currentDate;
}

void CalendarPrivate::updateSize()
{
    QSize minSize = QSize(300, 250);
    QSize prefSize = calendarWidget ? calendarWidget->size().toSize() : QSize(300, 250);

    q->setMinimumSize(minSize);
    q->setPreferredSize(prefSize);
}

void Calendar::dateUpdated()
{
    emit dateChanged(date());
}

}

#include "moc_calendar.cpp"
