/*  This file is part of the KDE project
    Copyright (C) 2023 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "pager.h"
#include "kworkspace/ktaskmanager.h"

#include <QX11Info>
#include <Plasma/Dialog>
#include <Plasma/Animation>
#include <Plasma/IconWidget>
#include <Plasma/Svg>
#include <Plasma/FrameSvg>
#include <Plasma/SvgWidget>
#include <Plasma/Theme>
#include <Plasma/ToolTipManager>
#include <KCModuleInfo>
#include <KWindowSystem>
#include <KIcon>
#include <KIconLoader>
#include <KDebug>
#include <netwm.h>

// standard issue margin/spacing
static const int s_spacing = 4;

static QString kElementPrefixForDesktop(const int desktop)
{
    if (KWindowSystem::currentDesktop() == desktop) {
        return QString::fromLatin1("active");
    }
    return QString::fromLatin1("normal");
}

static QString kElementForLocation(const Plasma::Location location)
{
    switch (location) {
        case Plasma::Location::TopEdge: {
            return QString::fromLatin1("down-arrow");
        }
        case Plasma::Location::BottomEdge: {
            return QString::fromLatin1("up-arrow");
        }
        case Plasma::Location::LeftEdge: {
            return QString::fromLatin1("right-arrow");
        }
        case Plasma::Location::RightEdge: {
            return QString::fromLatin1("left-arrow");
        }
    }
    return QString::fromLatin1("up-arrow");
}

static bool kHandleMouseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        NETRootInfo netrootinfo(QX11Info::display(), NET::WM2ShowingDesktop | NET::Supported);
        if (!netrootinfo.isSupported(NET::WM2ShowingDesktop)) {
            kWarning() << "NET::WM2ShowingDesktop is not supported";
            return false;
        }
        // that is how the showdesktop does it - it tracks the state internally
        static bool s_didshow = false;
        if (netrootinfo.showingDesktop() || s_didshow) {
            s_didshow = false;
            netrootinfo.setShowingDesktop(false);
        } else {
            s_didshow = true;
            netrootinfo.setShowingDesktop(true);
        }
        return true;
    }
    return false;
}

class PagerDialog : public Plasma::Dialog
{
    Q_OBJECT
public:
    explicit PagerDialog(QWidget *parent = nullptr);

    void updateTasks(const QList<KTaskManager::Task> &tasks);

private:
    QGraphicsScene* m_scene;
    QGraphicsWidget* m_widget;
    QGraphicsLinearLayout* m_layout;
    QList<KTaskManager::Task> m_tasks;
};

PagerDialog::PagerDialog(QWidget *parent)
    : Plasma::Dialog(parent, Qt::Dialog | Qt::WindowStaysOnTopHint),
    m_scene(nullptr),
    m_widget(nullptr),
    m_layout(nullptr)
{
    m_scene = new QGraphicsScene(this);
    m_widget = new QGraphicsWidget();

    m_layout = new QGraphicsLinearLayout(m_widget);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(s_spacing);

    m_widget->setLayout(m_layout);

    m_scene->addItem(m_widget);
    setGraphicsWidget(m_widget);
}

void PagerDialog::updateTasks(const QList<KTaskManager::Task> &tasks)
{
    // TODO:
    m_tasks = tasks;
}


class PagerIcon : public Plasma::IconWidget
{
    Q_OBJECT
public:
    explicit PagerIcon(const KTaskManager::Task &task, QGraphicsItem *parent);

    QByteArray taskID() const;
    void animatedShow();
    void animatedRemove();
    void updateIconAndToolTip();

private Q_SLOTS:
    void slotClicked();
    void slotWindowPreviewActivated(const WId window);

private:
    KTaskManager::Task m_task;
};

PagerIcon::PagerIcon(const KTaskManager::Task &task, QGraphicsItem *parent)
    : Plasma::IconWidget(parent),
    m_task(task)
{
    updateIconAndToolTip();
    connect(
        this, SIGNAL(clicked()),
        this, SLOT(slotClicked())
    );
    connect(
        Plasma::ToolTipManager::self(), SIGNAL(windowPreviewActivated(WId,Qt::MouseButtons,Qt::KeyboardModifiers,QPoint)),
        this, SLOT(slotWindowPreviewActivated(WId))
    );
}

QByteArray PagerIcon::taskID() const
{
    return m_task.id;
}

void PagerIcon::animatedShow()
{
    Plasma::Animation *animation = Plasma::Animator::create(Plasma::Animator::FadeAnimation);
    Q_ASSERT(animation != nullptr);
    animation->setTargetWidget(this);
    animation->setProperty("startOpacity", 0.0);
    animation->setProperty("targetOpacity", 1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void PagerIcon::animatedRemove()
{
    Plasma::Animation *animation = Plasma::Animator::create(Plasma::Animator::FadeAnimation);
    Q_ASSERT(animation != nullptr);
    connect(animation, SIGNAL(finished()), this, SLOT(deleteLater()));
    animation->setTargetWidget(this);
    animation->setProperty("startOpacity", 1.0);
    animation->setProperty("targetOpacity", 0.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void PagerIcon::updateIconAndToolTip()
{
    setIcon(m_task.icon);
    Plasma::ToolTipContent plasmatooltip;
    plasmatooltip.setMainText(QString::fromLatin1("<center>%1</center>").arg(m_task.name));
    plasmatooltip.setWindowToPreview(m_task.window);
    plasmatooltip.setClickable(true);
    Plasma::ToolTipManager::self()->setContent(this, plasmatooltip);
}

void PagerIcon::slotClicked()
{
    KTaskManager::self()->activateRaiseOrIconify(m_task);
}

void PagerIcon::slotWindowPreviewActivated(const WId window)
{
    // manually hide the tooltip first
    Plasma::ToolTipManager::self()->hide(this);
    KWindowSystem::activateWindow(window);
    KWindowSystem::raiseWindow(window);
}


class PagerSvg : public Plasma::SvgWidget
{
    Q_OBJECT
public:
    PagerSvg(const int desktop, const Qt::Orientation orientation, QGraphicsItem *parent = nullptr);

    void setup(const Qt::Orientation orientation, const Plasma::Location location);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;
    // handled here too
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) final;
    void resizeEvent(QGraphicsSceneResizeEvent *event) final;

private Q_SLOTS:
    void slotClicked(const Qt::MouseButton button);
    void slotUpdate();
    void slotUpdateSvg();
    void slotTaskAdded(const KTaskManager::Task &task);
    void slotTaskChanged(const KTaskManager::Task &task);
    void slotTaskRemoved(const KTaskManager::Task &task);
    void slotCheckSpace();
    void slotShowDialog();

private:
    QMutex m_mutex;
    int m_desktop;
    Plasma::FrameSvg* m_framesvg;
    QGraphicsLinearLayout* m_layout;
    QList<PagerIcon*> m_pagericons;
    QGraphicsWidget* m_spacer;
    Plasma::IconWidget* m_pagericon;
    PagerDialog* m_pagerdialog;
};

PagerSvg::PagerSvg(const int desktop, const Qt::Orientation orientation, QGraphicsItem *parent)
    : Plasma::SvgWidget(parent),
    m_desktop(desktop),
    m_framesvg(nullptr),
    m_layout(nullptr),
    m_spacer(nullptr),
    m_pagericon(nullptr),
    m_pagerdialog(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout = new QGraphicsLinearLayout(orientation, this);
    m_layout->setSpacing(0);

    slotUpdateSvg();
    setAcceptHoverEvents(true);

    foreach (const KTaskManager::Task &task, KTaskManager::self()->tasks()) {
        slotTaskAdded(task);
    }

    connect(
        this, SIGNAL(clicked(Qt::MouseButton)),
        this, SLOT(slotClicked(Qt::MouseButton))
    );
    connect(
        KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)),
        this, SLOT(slotUpdate())
    );
    connect(
        KTaskManager::self(), SIGNAL(taskAdded(KTaskManager::Task)),
        this, SLOT(slotTaskAdded(KTaskManager::Task))
    );
    connect(
        KTaskManager::self(), SIGNAL(taskChanged(KTaskManager::Task)),
        this, SLOT(slotTaskChanged(KTaskManager::Task))
    );
    connect(
        KTaskManager::self(), SIGNAL(taskRemoved(KTaskManager::Task)),
        this, SLOT(slotTaskRemoved(KTaskManager::Task))
    );
    connect(
        Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()),
        this, SLOT(slotUpdateSvg())
    );
}

void PagerSvg::setup(const Qt::Orientation orientation, const Plasma::Location location)
{
    m_layout->setOrientation(orientation);
    if (m_pagericon) {
        m_pagericon->setSvg("widgets/arrows", kElementForLocation(location));
    }
}

void PagerSvg::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing);
    const QRectF brect = boundingRect();
    m_framesvg->setElementPrefix(kElementPrefixForDesktop(m_desktop));
    m_framesvg->resizeFrame(brect.size());
    m_framesvg->paintFrame(painter, brect);
}

void PagerSvg::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (kHandleMouseEvent(event)) {
        event->accept();
        return;
    }
    Plasma::SvgWidget::mouseReleaseEvent(event);
}

void PagerSvg::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Plasma::SvgWidget::resizeEvent(event);
    slotCheckSpace();
}

void PagerSvg::slotClicked(const Qt::MouseButton button)
{
    if (button == Qt::LeftButton) {
        KWindowSystem::setCurrentDesktop(m_desktop);
    }
}

void PagerSvg::slotUpdate()
{
    update();
}

void PagerSvg::slotUpdateSvg()
{
    if (m_framesvg) {
        delete m_framesvg;
    }
    m_framesvg = new Plasma::FrameSvg(this);
    m_framesvg->setImagePath("widgets/pager");
    setSvg(m_framesvg);
    if (m_pagericon) {
        PagerApplet* pagerapplet = qobject_cast<PagerApplet*>(parentObject());
        Q_ASSERT(pagerapplet != nullptr);
        m_pagericon->setSvg("widgets/arrows", kElementForLocation(pagerapplet->location()));
    }
}

void PagerSvg::slotTaskAdded(const KTaskManager::Task &task)
{
    QMutexLocker locker(&m_mutex);
    if (m_spacer) {
        m_layout->removeItem(m_spacer);
    }
    const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(task.window, NET::WMDesktop);
    if (kwindowinfo.isOnDesktop(m_desktop)) {
        PagerIcon* pagericon = new PagerIcon(task, this);
        connect(
            pagericon, SIGNAL(destroyed()),
            this, SLOT(slotCheckSpace())
        );
        m_pagericons.append(pagericon);
        m_layout->addItem(pagericon);
        pagericon->animatedShow();
    }
    if (!m_spacer) {
        m_spacer = new QGraphicsWidget(this);
        m_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_spacer->setMinimumSize(0, 0);
    }
    m_layout->addItem(m_spacer);
    if (!m_pagericon) {
        PagerApplet* pagerapplet = qobject_cast<PagerApplet*>(parentObject());
        Q_ASSERT(pagerapplet != nullptr);
        m_pagericon = new Plasma::IconWidget(this);
        m_pagericon->setSvg("widgets/arrows", kElementForLocation(pagerapplet->location()));
        connect(
            m_pagericon, SIGNAL(clicked()),
            this, SLOT(slotShowDialog())
        );
    }
    m_layout->addItem(m_pagericon);
    locker.unlock();
    slotCheckSpace();
}

void PagerSvg::slotTaskChanged(const KTaskManager::Task &task)
{
    // special case for tasks moved from one virtual desktop to another, the check is done via
    // KWindowInfo::isOnDesktop() because tasks may be shown on all desktops
    const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(task.window, NET::WMDesktop);
    if (!kwindowinfo.isOnDesktop(m_desktop)) {
        slotTaskRemoved(task);
    } else {
        QMutexLocker locker(&m_mutex);
        PagerIcon* foundpagericon = nullptr;
        foreach (PagerIcon* pagericon, m_pagericons) {
            if (pagericon->taskID() == task.id) {
                foundpagericon = pagericon;
                break;
            }
        }
        locker.unlock();
        if (!foundpagericon) {
            slotTaskAdded(task);
        } else {
            foundpagericon->updateIconAndToolTip();
        }
    }
}

void PagerSvg::slotTaskRemoved(const KTaskManager::Task &task)
{
    QMutexLocker locker(&m_mutex);
    foreach (PagerIcon* pagericon, m_pagericons) {
        if (pagericon->taskID() == task.id) {
            m_pagericons.removeAll(pagericon);
            pagericon->animatedRemove();
            break;
        }
    }
}

void PagerSvg::slotCheckSpace()
{
    // qDebug() << Q_FUNC_INFO << m_spacer->size() << size();
    if (!m_spacer) {
        return;
    }
    const QSizeF spacersize = m_spacer->size();
    bool notenoughspace = false;
    QMutexLocker locker(&m_mutex);
    if (m_layout->orientation() == Qt::Horizontal) {
        notenoughspace = (m_pagericons.size() > 0 && spacersize.width() <= m_pagericons.first()->size().width());
    } else {
        notenoughspace = (m_pagericons.size() > 0 && spacersize.height() <= m_pagericons.first()->size().height());
    }
    if (notenoughspace) {
        if (!m_pagerdialog) {
            m_pagerdialog = new PagerDialog();
        }
        // TODO: m_pagerdialog->updateTasks();
        m_pagericon->setVisible(true);
    } else {
        m_pagericon->setVisible(false);
    }
}

void PagerSvg::slotShowDialog()
{
    Q_ASSERT(m_pagerdialog != nullptr);
    m_pagerdialog->show();
}


PagerApplet::PagerApplet(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_layout(nullptr),
    m_adddesktopaction(nullptr),
    m_removedesktopaction(nullptr),
    m_spacer(nullptr),
    m_kcmdesktopproxy(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_pager");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout = new QGraphicsLinearLayout(Qt::Horizontal, this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(s_spacing);

    // early setup to get proper initial size
    slotUpdateLayout();
    adjustSize();
}

void PagerApplet::init()
{
    connect(
        KWindowSystem::self(), SIGNAL(numberOfDesktopsChanged(int)),
        this, SLOT(slotUpdateLayout())
    );
}

void PagerApplet::createConfigurationInterface(KConfigDialog *parent)
{
    m_kcmdesktopproxy = new KCModuleProxy("desktop");
    parent->addPage(
        m_kcmdesktopproxy, m_kcmdesktopproxy->moduleInfo().moduleName(),
        m_kcmdesktopproxy->moduleInfo().icon()
    );

    connect(parent, SIGNAL(applyClicked()), this, SLOT(slotConfigAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(slotConfigAccepted()));
    connect(m_kcmdesktopproxy, SIGNAL(changed(bool)), parent, SLOT(settingsModified()));
}

QList<QAction*> PagerApplet::contextualActions()
{
    return m_actions;
}

void PagerApplet::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    const int currentdesktop = KWindowSystem::currentDesktop();
    if (event->delta() < 0) {
        KWindowSystem::setCurrentDesktop(currentdesktop + 1);
    } else {
        KWindowSystem::setCurrentDesktop(currentdesktop - 1);
    }
}

void PagerApplet::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (kHandleMouseEvent(event)) {
        event->accept();
        return;
    }
    Plasma::Applet::mouseReleaseEvent(event);
}

void PagerApplet::constraintsEvent(Plasma::Constraints constraints)
{
    // perfect size finder
    // qDebug() << Q_FUNC_INFO << size();
    if (constraints & Plasma::SizeConstraint || constraints & Plasma::FormFactorConstraint) {
        switch (formFactor()) {
            case Plasma::FormFactor::Horizontal: {
                m_layout->setOrientation(Qt::Horizontal);
                updatePagers();
                return;
            }
            case Plasma::FormFactor::Vertical: {
                m_layout->setOrientation(Qt::Vertical);
                updatePagers();
                return;
            }
            default: {
                break;
            }
        }

        const QSizeF appletsize = size();
        if (appletsize.width() >= appletsize.height()) {
            m_layout->setOrientation(Qt::Horizontal);
        } else {
            m_layout->setOrientation(Qt::Vertical);
        }
        updatePagers();
    }
}

void PagerApplet::updatePagers()
{
    QMutexLocker locker(&m_mutex);
    // NOTE: if the preferred size is not set the pager widgets will expand and shrink in a very
    // weird way when task icon is added or removed (sometimes expanding, sometimes shrinking)
    // because someone tried to make layouts and policies algorithms smart
    QSizeF dividedappletsize = size();
    if (m_layout->orientation() == Qt::Horizontal) {
        dividedappletsize.setWidth((dividedappletsize.width() / m_pagersvgs.size()));
    } else {
        dividedappletsize.setHeight((dividedappletsize.height() / m_pagersvgs.size()));
    }
    foreach (PagerSvg* pagersvg, m_pagersvgs) {
        pagersvg->setup(m_layout->orientation(), location());
        if (m_layout->orientation() == Qt::Horizontal) {
            pagersvg->setPreferredWidth(dividedappletsize.width());
        } else {
            pagersvg->setPreferredHeight(dividedappletsize.height());
        }
    }
}

void PagerApplet::slotUpdateLayout()
{
    QMutexLocker locker(&m_mutex);
    foreach (PagerSvg* pagersvg, m_pagersvgs) {
        m_layout->removeItem(pagersvg);
    }
    qDeleteAll(m_pagersvgs);
    m_pagersvgs.clear();

    const int numberofdesktops = KWindowSystem::numberOfDesktops();
    const Qt::Orientation orientation = m_layout->orientation();
    for (int i = 0; i < numberofdesktops; i++) {
        PagerSvg* pagersvg = new PagerSvg(i + 1, orientation, this);
        m_layout->addItem(pagersvg);
        m_pagersvgs.append(pagersvg);
    }

    m_actions.clear();
    if (!m_adddesktopaction) {
        m_adddesktopaction = new QAction(
            KIcon("list-add"), i18n("&Add Virtual Desktop"),
            this
        );
    }
    m_actions.append(m_adddesktopaction);
    connect(
        m_adddesktopaction, SIGNAL(triggered(bool)),
        this , SLOT(slotAddDesktop())
    );
    if (numberofdesktops > 1) {
        if (!m_removedesktopaction) {
            m_removedesktopaction = new QAction(
                KIcon("list-remove"), i18n("&Remove Last Virtual Desktop"),
                this
            );
        }
        m_actions.append(m_removedesktopaction);
        connect(
            m_removedesktopaction, SIGNAL(triggered(bool)),
            this , SLOT(slotRemoveDesktop())
        );
    }
}

void PagerApplet::slotAddDesktop()
{
    NETRootInfo netrootinfo(QX11Info::display(), NET::NumberOfDesktops);
    netrootinfo.setNumberOfDesktops(netrootinfo.numberOfDesktops() + 1);
}

void PagerApplet::slotRemoveDesktop()
{
    NETRootInfo netrootinfo(QX11Info::display(), NET::NumberOfDesktops);
    const int numberofdesktops = netrootinfo.numberOfDesktops();
    if (numberofdesktops > 1) {
        netrootinfo.setNumberOfDesktops(netrootinfo.numberOfDesktops() - 1);
    } else {
        kWarning() << "there is only one desktop";
    }
}

void PagerApplet::slotConfigAccepted()
{
    slotUpdateLayout();
    m_kcmdesktopproxy->save();
    emit configNeedsSaving();
}

#include "moc_pager.cpp"
#include "pager.moc"
