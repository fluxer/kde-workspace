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

#include "tasks.h"

#include <Plasma/Animation>
#include <Plasma/Svg>
#include <Plasma/FrameSvg>
#include <Plasma/SvgWidget>
#include <Plasma/Theme>
#include <Plasma/ToolTipManager>
#include <KWindowSystem>
#include <KIcon>
#include <KIconLoader>
#include <KIconEffect>
#include <KDebug>

// standard issue margin/spacing
static const int s_spacing = 6;

static QString kElementPrefixForTask(const bool hovered, const bool isactive, const bool demandsattention)
{
    if (hovered) {
        return QString::fromLatin1("hover");
    } else if (isactive) {
        return QString::fromLatin1("focus");
    } else if (demandsattention) {
        return QString::fromLatin1("attention");
    }
    return QString::fromLatin1("normal");
}

class TasksSvg : public Plasma::SvgWidget
{
    Q_OBJECT
public:
    TasksSvg(const WId task, QGraphicsItem *parent = nullptr);

    WId task() const;
    void setup(const bool inpanel);
    void animatedShow();
    void animatedRemove();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const final;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) final;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) final;

private Q_SLOTS:
    void slotClicked(const Qt::MouseButton button);
    void slotWindowPreviewActivated(const WId task);
    void slotUpdate();
    void slotUpdateSvg();
    void slotTaskChanged(const WId task);

private:
    void updatePixmapAndToolTip();

    WId m_task;
    bool m_hovered;
    bool m_inpanel;
    Plasma::FrameSvg* m_framesvg;
    bool m_updatetask;
    QPixmap m_pixmap;
};

TasksSvg::TasksSvg(const WId task, QGraphicsItem *parent)
    : Plasma::SvgWidget(parent),
    m_task(task),
    m_hovered(false),
    m_inpanel(true),
    m_framesvg(nullptr),
    m_updatetask(true)
{
    updatePixmapAndToolTip();
    slotUpdateSvg();
    setAcceptHoverEvents(true);
    connect(
        this, SIGNAL(clicked(Qt::MouseButton)),
        this, SLOT(slotClicked(Qt::MouseButton))
    );
    connect(
        Plasma::ToolTipManager::self(), SIGNAL(windowPreviewActivated(WId,Qt::MouseButtons,Qt::KeyboardModifiers,QPoint)),
        this, SLOT(slotWindowPreviewActivated(WId))
    );
    connect(
        KTaskManager::self(), SIGNAL(taskChanged(WId)),
        this, SLOT(slotTaskChanged(WId))
    );
    connect(
        KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)),
        this, SLOT(slotUpdate())
    );
    connect(
        Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()),
        this, SLOT(slotUpdateSvg())
    );
}

WId TasksSvg::task() const
{
    return m_task;
}

void TasksSvg::setup(const bool inpanel)
{
    m_inpanel = inpanel;
}

void TasksSvg::animatedShow()
{
    show();
    Plasma::Animation *animation = Plasma::Animator::create(Plasma::Animator::FadeAnimation);
    Q_ASSERT(animation != nullptr);
    animation->setTargetWidget(this);
    animation->setProperty("startOpacity", 0.0);
    animation->setProperty("targetOpacity", 1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void TasksSvg::animatedRemove()
{
    // during task removal (when the window is no more) updating the pixmap will not get a valid
    // icon for example
    m_updatetask = false;
    Plasma::Animation *animation = Plasma::Animator::create(Plasma::Animator::FadeAnimation);
    Q_ASSERT(animation != nullptr);
    connect(animation, SIGNAL(finished()), this, SLOT(deleteLater()));
    animation->setTargetWidget(this);
    animation->setProperty("startOpacity", 1.0);
    animation->setProperty("targetOpacity", 0.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void TasksSvg::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const bool isactive = KTaskManager::isActive(m_task);
    const bool demandsattention = KTaskManager::demandsAttention(m_task);
    painter->setRenderHint(QPainter::Antialiasing);
    const QRectF brect = boundingRect();
    const QSizeF brectsize = brect.size();
    m_framesvg->setElementPrefix(kElementPrefixForTask(m_hovered, isactive, demandsattention));
    m_framesvg->resizeFrame(brectsize);
    m_framesvg->paintFrame(painter, brect);
    const int spacingx2 = (s_spacing * 2);
    const int iconsize = qRound(qMin(brectsize.width(), brectsize.height()));
    QPixmap iconpixmap = m_pixmap;
    if (!iconpixmap.isNull()) {
        iconpixmap = iconpixmap.scaled(
            iconsize - spacingx2,
            iconsize - spacingx2,
            Qt::KeepAspectRatio, Qt::SmoothTransformation
        );
    }
    // gray-out unless the task is active or demands attention
    if (!iconpixmap.isNull() && !isactive && !demandsattention) {
        iconpixmap = KIconEffect::apply(iconpixmap, KIconEffect::ToGray, 0.5, QColor(), QColor(), true);
    }
    if (!iconpixmap.isNull()) {
        painter->drawPixmap(QPoint(s_spacing, s_spacing), iconpixmap);
    }
}

QSizeF TasksSvg::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    if (which == Qt::PreferredSize || (!m_inpanel && which == Qt::MinimumSize)) {
        const int paneliconsize = KIconLoader::global()->currentSize(KIconLoader::Panel);
        return QSizeF(paneliconsize, paneliconsize);
    }
    return Plasma::SvgWidget::sizeHint(which, constraint);
}

void TasksSvg::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_hovered = true;
    update();
}

void TasksSvg::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_hovered = false;
    update();
}

void TasksSvg::updatePixmapAndToolTip()
{
    if (!m_updatetask) {
        return;
    }
    m_pixmap = KWindowSystem::icon(m_task);
    const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(m_task, NET::WMVisibleName);
    Plasma::ToolTipContent plasmatooltip;
    plasmatooltip.setMainText(QString::fromLatin1("<center>%1</center>").arg(kwindowinfo.visibleName()));
    plasmatooltip.setWindowToPreview(m_task);
    plasmatooltip.setClickable(true);
    Plasma::ToolTipManager::self()->setContent(this, plasmatooltip);
}

void TasksSvg::slotClicked(const Qt::MouseButton button)
{
    if (button == Qt::LeftButton) {
        KTaskManager::activateRaiseOrIconify(m_task);
    }
}

void TasksSvg::slotWindowPreviewActivated(const WId window)
{
    // manually hide the tooltip first
    Plasma::ToolTipManager::self()->hide(this);
    KWindowSystem::activateWindow(window);
    KWindowSystem::raiseWindow(window);
}

void TasksSvg::slotTaskChanged(const WId task)
{
    if (task == m_task) {
        updatePixmapAndToolTip();
        update();
    }
}

void TasksSvg::slotUpdate()
{
    update();
}

void TasksSvg::slotUpdateSvg()
{
    if (m_framesvg) {
        delete m_framesvg;
    }
    m_framesvg = new Plasma::FrameSvg(this);
    m_framesvg->setImagePath("widgets/tasks");
    setSvg(m_framesvg);
}


TasksApplet::TasksApplet(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_layout(nullptr),
    m_spacer(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_tasks");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout = new QGraphicsLinearLayout(Qt::Horizontal, this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_spacer = new QGraphicsWidget(this);
    m_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_spacer->setMinimumSize(0, 0);
    m_layout->addItem(m_spacer);
}

void TasksApplet::init()
{
    foreach (const WId task, KTaskManager::self()->tasks()) {
        slotTaskAdded(task);
    }

    connect(
        KTaskManager::self(), SIGNAL(taskAdded(WId)),
        this, SLOT(slotTaskAdded(WId))
    );
    connect(
        KTaskManager::self(), SIGNAL(taskRemoved(WId)),
        this, SLOT(slotTaskRemoved(WId))
    );
    connect(
        KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)),
        this, SLOT(slotCurrentDesktopChanged(int))
    );
}

void TasksApplet::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::FormFactorConstraint) {
        bool inpanel = true;
        switch (formFactor()) {
            case Plasma::FormFactor::Horizontal: {
                m_layout->setOrientation(Qt::Horizontal);
                break;
            }
            case Plasma::FormFactor::Vertical: {
                m_layout->setOrientation(Qt::Vertical);
                break;
            }
            default: {
                m_layout->setOrientation(Qt::Horizontal);
                inpanel = false;
                break;
            }
        }
        QMutexLocker locker(&m_mutex);
        foreach (TasksSvg* taskssvg, m_taskssvgs) {
            taskssvg->setup(inpanel);
        }
    }
}

void TasksApplet::slotTaskAdded(const WId task)
{
    QMutexLocker locker(&m_mutex);
    m_layout->removeItem(m_spacer);
    const bool inpanel = (
        formFactor() == Plasma::FormFactor::Horizontal
        || formFactor() == Plasma::FormFactor::Vertical
    );
    TasksSvg* taskssvg = new TasksSvg(task, this);
    taskssvg->setup(inpanel);
    m_layout->addItem(taskssvg);
    m_taskssvgs.append(taskssvg);
    const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(taskssvg->task(), NET::WMDesktop);
    if (kwindowinfo.isOnDesktop(KWindowSystem::currentDesktop())) {
        taskssvg->animatedShow();
    } else {
        taskssvg->hide();
    }
    // TODO: once the taskbar is nearly full show arrow for items that shall be available via
    // menu-like widget
    m_layout->addItem(m_spacer);
}

void TasksApplet::slotTaskRemoved(const WId task)
{
    QMutexLocker locker(&m_mutex);
    QMutableListIterator<TasksSvg*> iter(m_taskssvgs);
    while (iter.hasNext()) {
        TasksSvg* taskssvg = iter.next();
        if (taskssvg->task() == task) {
            iter.remove();
            taskssvg->animatedRemove();
            break;
        }
    }
}

void TasksApplet::slotCurrentDesktopChanged(const int desktop)
{
    // special case for tasks moved from one virtual desktop to another
    QMutexLocker locker(&m_mutex);
    foreach (TasksSvg* taskssvg, m_taskssvgs) {
        const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(taskssvg->task(), NET::WMDesktop);
        if (!kwindowinfo.isOnDesktop(desktop)) {
            taskssvg->hide();
        } else {
            taskssvg->animatedShow();
        }
    }
}

#include "moc_tasks.cpp"
#include "tasks.moc"
