/*
 *   Copyright (C) 2007 by Ivan Cukic <ivan.cukic+kde@gmail.com>
 *   Copyright (C) 2009 by Ana Cecília Martins <anaceciliamb@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "widgetexplorer.h"

#include <QMutex>
#include <QApplication>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include <Plasma/LineEdit>
#include <Plasma/ToolButton>
#include <Plasma/ScrollWidget>
#include <Plasma/Frame>
#include <Plasma/IconWidget>
#include <Plasma/Label>
#include <klineedit.h>
#include <ksycoca.h>
#include <kicon.h>
#include <kdebug.h>

namespace Plasma
{

// hardcoded but ok because the widget is not resizable
static const int s_margin = 4;
static const QSizeF s_appletframesize = QSize(300, 94);
static const QSizeF s_appleticonsize = QSize(80, 80);
static const int s_filterwidth = 305;
static const int s_dragpixmapsize = 64;
// the default Plasma::Corona MIME type, see:
// kdelibs/plasma/corona.cpp
static const QString s_coronamimetype = QString::fromLatin1("text/x-plasmoidservicename");

Qt::Orientation kOrientationForLocation(const Plasma::Location location)
{
    switch (location) {
        case Plasma::Location::LeftEdge:
        case Plasma::Location::RightEdge: {
            return Qt::Vertical;
        }
        default: {
            return Qt::Horizontal;
        }
    }
    Q_UNREACHABLE();
}

class AppletIcon : public Plasma::IconWidget
{
    Q_OBJECT
public:
    AppletIcon(QGraphicsItem *parent, const KPluginInfo &appletInfo);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) final;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) final;

private:
    KPluginInfo m_appletinfo;
    QPointF m_dragstartpos;
};

AppletIcon::AppletIcon(QGraphicsItem *parent, const KPluginInfo &appletInfo)
    : Plasma::IconWidget(parent),
    m_appletinfo(appletInfo)
{
}

void AppletIcon::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_dragstartpos = event->pos();
    // don't propagate
    event->accept();
}

void AppletIcon::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton &&
        (event->pos() - m_dragstartpos).manhattanLength() > KGlobalSettings::dndEventDelay())
    {
        // have to parent it to QWidget*..
        QDrag* drag = new QDrag(qApp->activeWindow());
        QMimeData* mimedata = new QMimeData();
        mimedata->setData(s_coronamimetype, m_appletinfo.pluginName().toUtf8());
        drag->setMimeData(mimedata);
        drag->setPixmap(KIcon(m_appletinfo.icon()).pixmap(s_dragpixmapsize));
        static const int halfdragpixmapsize = (s_dragpixmapsize / 2);
        drag->setHotSpot(QPoint(halfdragpixmapsize, halfdragpixmapsize));
        drag->exec();
    }
}


class AppletFrame : public Plasma::Frame
{
    Q_OBJECT
public:
    AppletFrame(QGraphicsWidget *parent, const KPluginInfo &appletInfo);

    KPluginInfo pluginInfo() const;
    void setRunning(const bool isrunning);

Q_SIGNALS:
    void addApplet(const QString &applet);
    void removeApplet(const QString &applet);

private Q_SLOTS:
    void slotAddApplet();
    void slotRemoveApplet();
    void slotUpdateFonts();

private:
    KPluginInfo m_appletinfo;
    Plasma::Label* m_appletname;
    Plasma::IconWidget* m_appletactive;
    Plasma::Label* m_appletcomment;
};

AppletFrame::AppletFrame(QGraphicsWidget *parent, const KPluginInfo &appletInfo)
    : Plasma::Frame(parent),
    m_appletinfo(appletInfo),
    m_appletname(nullptr),
    m_appletactive(nullptr),
    m_appletcomment(nullptr)
{
    setFrameShadow(Plasma::Frame::Sunken);

    QGraphicsGridLayout* appletLayout = new QGraphicsGridLayout(this);
    AppletIcon* appletIcon = new AppletIcon(this, appletInfo);
    appletIcon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    appletIcon->setMinimumSize(s_appleticonsize);
    appletIcon->setMaximumSize(s_appleticonsize);
    appletIcon->setIcon(appletInfo.icon());
    connect(
        appletIcon, SIGNAL(doubleClicked()),
        this, SLOT(slotAddApplet())
    );
    appletLayout->addItem(appletIcon, 0, 0, 2, 1);

    m_appletname = new Plasma::Label(this);
    m_appletname->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QFont appletNameFont = KGlobalSettings::generalFont();
    appletNameFont.setBold(true);
    m_appletname->setFont(appletNameFont);
    m_appletname->setText(appletInfo.name());
    appletLayout->addItem(m_appletname, 0, 1);

    m_appletactive = new Plasma::IconWidget(this);
    m_appletactive->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_appletactive->setMaximumSize(22, 22);
    m_appletactive->setIcon(KIcon());
    connect(
        m_appletactive, SIGNAL(clicked()),
        this, SLOT(slotRemoveApplet())
    );
    appletLayout->addItem(m_appletactive, 0, 2);

    m_appletcomment = new Plasma::Label(this);
    m_appletcomment->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_appletcomment->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_appletcomment->setFont(KGlobalSettings::smallestReadableFont());
    m_appletcomment->setText(appletInfo.comment());
    appletLayout->addItem(m_appletcomment, 1, 1, 1, 2);

    setLayout(appletLayout);

    connect(
        KGlobalSettings::self(), SIGNAL(kdisplayFontChanged()),
        this, SLOT(slotUpdateFonts())
    );
}

KPluginInfo AppletFrame::pluginInfo() const
{
    return m_appletinfo;
}

void AppletFrame::setRunning(const bool isrunning)
{
    m_appletactive->setIcon(isrunning ? KIcon("edit-delete") : KIcon());
}

void AppletFrame::slotAddApplet()
{
    emit addApplet(m_appletinfo.pluginName());
}

void AppletFrame::slotRemoveApplet()
{
    emit removeApplet(m_appletinfo.pluginName());
}

void AppletFrame::slotUpdateFonts()
{
    QFont appletNameFont = KGlobalSettings::generalFont();
    appletNameFont.setBold(true);
    m_appletname->setFont(appletNameFont);
    m_appletcomment->setFont(KGlobalSettings::smallestReadableFont());
}


class WidgetExplorerPrivate
{
public:
    WidgetExplorerPrivate(WidgetExplorer *w)
        : q(w),
        containment(nullptr),
        mainLayout(nullptr),
        filterEdit(nullptr),
        topSpacer(nullptr),
        categoriesButton(nullptr),
        categoriesMenu(nullptr),
        closeButton(nullptr),
        scrollWidget(nullptr),
        appletsWidget(nullptr),
        appletsLayout(nullptr),
        appletsPlaceholder(nullptr)
    {
    }

    void init(Plasma::Location loc);
    void updateApplets();
    void updateRunningApplets();
    void filterApplets(const QString &text);
    void updateOrientation(const Qt::Orientation orientation);

    void _k_appletAdded(Plasma::Applet *applet);
    void _k_appletRemoved(Plasma::Applet *applet);
    void _k_containmentDestroyed();
    void _k_immutabilityChanged(const Plasma::ImmutabilityType type);
    void _k_textChanged(const QString &text);
    void _k_closePressed();
    void _k_categoriesClicked();
    void _k_menuTriggered(QAction *action);
    void _k_menuAboutToHide();
    void _k_addApplet(const QString &pluginName);
    void _k_removeApplet(const QString &pluginName);
    void _k_databaseChanged(const QStringList &resources);

    Plasma::Location location;
    WidgetExplorer* q;
    Plasma::Containment* containment;

    QMutex mutex;
    QGraphicsGridLayout* mainLayout;
    Plasma::LineEdit* filterEdit;
    Plasma::Label* topSpacer;
    Plasma::ToolButton* categoriesButton;
    QMenu* categoriesMenu;
    Plasma::ToolButton* closeButton;
    Plasma::ScrollWidget* scrollWidget;
    QGraphicsWidget* appletsWidget;
    QGraphicsLinearLayout* appletsLayout;
    QList<AppletFrame*> appletFrames;
    Plasma::Frame* appletsPlaceholder;
    QMap<Plasma::Applet*,QString> runningApplets;
};

void WidgetExplorerPrivate::init(Plasma::Location loc)
{
    q->setFocusPolicy(Qt::StrongFocus);

    const Qt::Orientation orientation = kOrientationForLocation(location);

    location = loc;
    mainLayout = new QGraphicsGridLayout(q);
    mainLayout->setContentsMargins(s_margin, 0, s_margin, 0);
    mainLayout->setSpacing(s_margin);

    filterEdit = new Plasma::LineEdit(q);
    filterEdit->setClickMessage(i18n("Enter search term..."));
    filterEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    QSizeF filterEditMinimumSize = filterEdit->minimumSize();
    filterEditMinimumSize.setWidth(s_filterwidth);
    filterEdit->setMinimumSize(filterEditMinimumSize);
    filterEdit->setMaximumSize(filterEditMinimumSize);
    q->setFocusProxy(filterEdit);
    q->connect(
        filterEdit, SIGNAL(textChanged(QString)),
        q, SLOT(_k_textChanged(QString))
    );
    mainLayout->addItem(filterEdit, 0, 0);

    topSpacer = new Plasma::Label(q);
    topSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    topSpacer->setMinimumSize(1, 1);
    mainLayout->addItem(topSpacer, 0, 1);

    categoriesButton = new Plasma::ToolButton(q);
    categoriesButton->setText(i18n("All Widgets"));
    q->connect(
        categoriesButton, SIGNAL(clicked()),
        q, SLOT(_k_categoriesClicked())
    );
    categoriesMenu = new QMenu();
    q->connect(
        categoriesMenu, SIGNAL(triggered(QAction*)),
        q, SLOT(_k_menuTriggered(QAction*))
    );
    q->connect(
        categoriesMenu, SIGNAL(aboutToHide()),
        q, SLOT(_k_menuAboutToHide())
    );
    mainLayout->addItem(categoriesButton, 0, 2);

    closeButton = new Plasma::ToolButton(q);
    closeButton->setIcon(KIcon("window-close"));
    q->connect(
        closeButton, SIGNAL(pressed()),
        q, SLOT(_k_closePressed())
    );
    mainLayout->addItem(closeButton, 0, 3);

    scrollWidget = new Plasma::ScrollWidget(q);
    scrollWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollWidget->setOverShoot(false);
    scrollWidget->setOverflowBordersVisible(false);
    scrollWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    appletsWidget = new QGraphicsWidget(scrollWidget);
    appletsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    appletsLayout = new QGraphicsLinearLayout(orientation, appletsWidget);
    appletsPlaceholder = new Plasma::Frame(appletsWidget);
    appletsPlaceholder->setFrameShadow(Plasma::Frame::Sunken);
    appletsPlaceholder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGraphicsLinearLayout* appletsPlaceholderLayout = new QGraphicsLinearLayout(Qt::Horizontal, appletsPlaceholder);
    Plasma::Label* appletsPlaceholderLabel = new Plasma::Label(appletsPlaceholder);
    appletsPlaceholderLabel->setAlignment(Qt::AlignCenter);
    appletsPlaceholderLabel->setText(i18n("No applets found"));
    appletsPlaceholderLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    appletsPlaceholderLayout->addItem(appletsPlaceholderLabel);
    appletsPlaceholder->setLayout(appletsPlaceholderLayout);
    appletsLayout->addItem(appletsPlaceholder);
    appletsWidget->setLayout(appletsLayout);
    scrollWidget->setWidget(appletsWidget);
    mainLayout->addItem(scrollWidget, 1, 0, 1, 4);

    updateOrientation(orientation);

    q->setLayout(mainLayout);

    q->connect(
        KSycoca::self(), SIGNAL(databaseChanged(QStringList)),
        q, SLOT(_k_databaseChanged(QStringList))
    );
}

void WidgetExplorerPrivate::updateApplets()
{
    filterEdit->setEnabled(false);

    QMutexLocker locker(&mutex);
    foreach (AppletFrame* appletFrame, appletFrames) {
        appletsLayout->removeItem(appletFrame);
    }
    qDeleteAll(appletFrames);
    appletFrames.clear();

    QStringList appletCategories;
    bool hasapplets = false;
    const QString containmentsCategory = i18n("Containments");
    foreach (const KPluginInfo &appletInfo, Plasma::Applet::listAppletInfo()) {
        const QString appletCategory = appletInfo.category();
        if (appletInfo.property("NoDisplay").toBool() || appletCategory == containmentsCategory) {
            continue;
        }
        if (!appletCategory.isEmpty() && !appletCategories.contains(appletCategory)) {
            appletCategories.append(appletCategory);
        }

        hasapplets = true;
        AppletFrame* appletFrame = new AppletFrame(appletsWidget, appletInfo);
        appletFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        appletFrame->setMinimumSize(s_appletframesize);
        appletFrame->setPreferredSize(s_appletframesize);
        q->connect(
            appletFrame, SIGNAL(addApplet(QString)),
            q, SLOT(_k_addApplet(QString))
        );
        q->connect(
            appletFrame, SIGNAL(removeApplet(QString)),
            q, SLOT(_k_removeApplet(QString))
        );
        appletsLayout->addItem(appletFrame);
        appletFrames.append(appletFrame);
    }

    appletsPlaceholder->setVisible(!hasapplets);

    categoriesMenu->clear();
    categoriesMenu->addAction(i18n("All Widgets"));
    categoriesMenu->addAction(i18n("Running"));
    categoriesMenu->addSeparator();
    foreach (const QString &appletCategory, appletCategories) {
        categoriesMenu->addAction(appletCategory);
    }

    filterEdit->setEnabled(true);
}

void WidgetExplorerPrivate::updateRunningApplets()
{
    QMutexLocker locker(&mutex);
    const QStringList running = runningApplets.values();
    foreach (AppletFrame* appletFrame, appletFrames) {
        appletFrame->setRunning(running.contains(appletFrame->pluginInfo().pluginName()));
    }
}

void WidgetExplorerPrivate::filterApplets(const QString &text)
{
    QMutexLocker locker(&mutex);
    const QString categoriesButtonText = categoriesButton->text();
    const bool allwidgets = (categoriesButtonText == i18n("All Widgets"));
    const bool onlyrunning = (categoriesButtonText == i18n("Running"));
    const QStringList running = runningApplets.values();
    bool hasapplets = false;
    foreach (AppletFrame* appletFrame, appletFrames) {
        appletFrame->setVisible(false);
        const KPluginInfo appletInfo = appletFrame->pluginInfo();
        const QString appletPluginName = appletInfo.pluginName();
        if (onlyrunning && !running.contains(appletPluginName)) {
            continue;
        }
        const QString appletCategory = appletInfo.category();
        if (!onlyrunning && !allwidgets && categoriesButtonText != appletCategory) {
            continue;
        }
        if (!text.isEmpty()) {
            const QString appletName = appletInfo.name();
            const QString appletComment = appletInfo.comment();
            if (appletName.contains(text) || appletPluginName.contains(text) || appletComment.contains(text)) {
                appletFrame->setVisible(true);
                hasapplets = true;
            }
        } else {
            appletFrame->setVisible(true);
            hasapplets = true;
        }
    }
    appletsPlaceholder->setVisible(!hasapplets);
    if (hasapplets) {
        appletsWidget->adjustSize();
    } else {
        appletsWidget->resize(scrollWidget->size());
    }
}

void WidgetExplorerPrivate::updateOrientation(const Qt::Orientation orientation)
{
    appletsLayout->setOrientation(orientation);
    topSpacer->setVisible(false);
    mainLayout->removeItem(topSpacer);
    categoriesButton->setVisible(false);
    mainLayout->removeItem(categoriesButton);
    if (orientation == Qt::Horizontal) {
        filterEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        mainLayout->addItem(topSpacer, 0, 1);
        topSpacer->setVisible(true);
        mainLayout->addItem(categoriesButton, 0, 2);
        categoriesButton->setVisible(true);
    } else {
        filterEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        // TODO: put the categories button bellow the filter and close button
    }
}

void WidgetExplorerPrivate::_k_textChanged(const QString &text)
{
    filterApplets(text);
}

void WidgetExplorerPrivate::_k_categoriesClicked()
{
    categoriesButton->setDown(true);
    categoriesMenu->exec(QCursor::pos());
}

void WidgetExplorerPrivate::_k_menuTriggered(QAction *action)
{
    categoriesButton->setText(action->text());
    categoriesButton->setDown(false);
    filterApplets(filterEdit->text());
}

void WidgetExplorerPrivate::_k_menuAboutToHide()
{
    categoriesButton->setDown(false);
}

void WidgetExplorerPrivate::_k_closePressed()
{
    emit q->closeClicked();
}

void WidgetExplorerPrivate::_k_addApplet(const QString &pluginName)
{
    if (!containment) {
        return;
    }
    containment->addApplet(pluginName);
}

void WidgetExplorerPrivate::_k_removeApplet(const QString &pluginName)
{
    if (!containment) {
        return;
    }
    Plasma::Corona *corona = containment->corona();
    if (!corona) {
        return;
    }
    QList<Containment*> containments = corona->containments();
    foreach (Containment *containment, containments) {
        foreach (Applet *applet, containment->applets()) {
            if (applet->pluginName() == pluginName) {
                applet->destroy();
            }
        }
    }
}

void WidgetExplorerPrivate::_k_databaseChanged(const QStringList &resources)
{
    if (resources.contains("services")) {
        updateApplets();
        filterApplets(filterEdit->text());
    }
}

void WidgetExplorerPrivate::_k_appletAdded(Plasma::Applet *applet)
{
    runningApplets.insert(applet, applet->pluginName());
    updateRunningApplets();
}

void WidgetExplorerPrivate::_k_appletRemoved(Plasma::Applet *applet)
{
    runningApplets.remove(applet);
    updateRunningApplets();
}

void WidgetExplorerPrivate::_k_containmentDestroyed()
{
    q->setContainment(nullptr);
}

void WidgetExplorerPrivate::_k_immutabilityChanged(const Plasma::ImmutabilityType type)
{
    if (type != Plasma::Mutable) {
        emit q->closeClicked();
    }
}

WidgetExplorer::WidgetExplorer(Plasma::Location loc, QGraphicsItem *parent)
    : QGraphicsWidget(parent),
    d(new WidgetExplorerPrivate(this))
{
    d->init(loc);
}

WidgetExplorer::WidgetExplorer(QGraphicsItem *parent)
    : QGraphicsWidget(parent),
    d(new WidgetExplorerPrivate(this))
{
    d->init(Plasma::BottomEdge);
}

WidgetExplorer::~WidgetExplorer()
{
    if (d->containment) {
        d->containment->disconnect(this);
    }
    foreach (AppletFrame* appletFrame, d->appletFrames) {
        d->appletsLayout->removeItem(appletFrame);
    }
    qDeleteAll(d->appletFrames);
    d->appletFrames.clear();
    d->appletsLayout->removeItem(d->appletsPlaceholder);
    delete d->appletsPlaceholder;
    d->mainLayout->removeItem(d->filterEdit);
    delete d->filterEdit;
    d->mainLayout->removeItem(d->topSpacer);
    delete d->topSpacer;
    d->mainLayout->removeItem(d->categoriesButton);
    delete d->categoriesButton;
    delete d->categoriesMenu;
    d->mainLayout->removeItem(d->closeButton);
    delete d->closeButton;
    delete d->appletsWidget;
    delete d;
}

void WidgetExplorer::setLocation(const Plasma::Location loc)
{
    d->location = loc;
    d->updateOrientation(kOrientationForLocation(loc));
    emit locationChanged(loc);
}

Plasma::Location WidgetExplorer::location() const
{
    return d->location;
}

void WidgetExplorer::setContainment(Plasma::Containment *containment)
{
    if (d->containment != containment) {
        if (d->containment) {
            d->containment->disconnect(this);
        }

        d->runningApplets.clear();
        d->containment = containment;

        if (d->containment) {
            connect(
                d->containment, SIGNAL(destroyed(QObject*)),
                this, SLOT(_k_containmentDestroyed())
            );
            connect(
                d->containment, SIGNAL(immutabilityChanged(Plasma::ImmutabilityType)),
                this, SLOT(_k_immutabilityChanged(Plasma::ImmutabilityType))
            );

            setLocation(containment->location());
        }

        if (containment) {
            Plasma::Corona *corona = containment->corona();
            if (corona) {
                QList<Containment*> containments = corona->containments();
                foreach (Containment *containment, containments) {
                    connect(
                        containment, SIGNAL(appletAdded(Plasma::Applet*,QPointF)),
                        this, SLOT(_k_appletAdded(Plasma::Applet*))
                    );
                    connect(
                        containment, SIGNAL(appletRemoved(Plasma::Applet*)),
                        this, SLOT(_k_appletRemoved(Plasma::Applet*))
                    );

                    foreach (Applet *applet, containment->applets()) {
                        d->runningApplets.insert(applet, applet->pluginName());
                    }
                }
            }
        }

        d->updateApplets();
        d->updateRunningApplets();
        d->filterApplets(d->filterEdit->text());
    }
}

Containment* WidgetExplorer::containment() const
{
    return d->containment;
}

void WidgetExplorer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        emit closeClicked();
        event->accept();
        return;
    }
    // if the scroll area or any other widget steals the focus the focus goes back to the filter
    // widget
    if (!d->filterEdit->hasFocus()) {
        d->filterEdit->setFocus();
    }
    QApplication::sendEvent(d->filterEdit->nativeWidget(), event);
}

} // namespace Plasma

#include "moc_widgetexplorer.cpp"
#include "widgetexplorer.moc"
