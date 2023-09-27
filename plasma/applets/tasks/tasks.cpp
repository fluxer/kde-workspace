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
#include "taskmanager/taskmanager.h"

#include <Plasma/Svg>
#include <Plasma/FrameSvg>
#include <Plasma/SvgWidget>
#include <Plasma/Theme>
#include <Plasma/ToolTipManager>
#include <KWindowSystem>
#include <KIcon>
#include <KDebug>

// standard issue margin/spacing
static const int s_spacing = 6;
// TODO: this should be configurable
static const int s_maximumtasksize = 200;

// TODO: these task lookups are sub-optimal
static bool kIsTaskActive(const WId taskwindow)
{
    foreach (TaskManager::Task *task, TaskManager::TaskManager::self()->tasks()) {
        if (task->window() == taskwindow) {
            return task->isActive();
        }
    }
    return false;
}

static bool kDoesTaskDemandAttention(const WId taskwindow)
{
    foreach (TaskManager::Task *task, TaskManager::TaskManager::self()->tasks()) {
        if (task->window() == taskwindow) {
            return task->demandsAttention();
        }
    }
    return false;
}

static QString kTaskName(const WId taskwindow)
{
    foreach (TaskManager::Task *task, TaskManager::TaskManager::self()->tasks()) {
        if (task->window() == taskwindow) {
            return task->name();
        }
    }
    return QString::number(taskwindow);
}

static QPixmap kTaskPixmap(const WId taskwindow, const int width, const int height)
{
    foreach (TaskManager::Task *task, TaskManager::TaskManager::self()->tasks()) {
        if (task->window() == taskwindow) {
            return task->icon(width, height);
        }
    }
    return QPixmap();
}

static QString kElementPrefixForTask(const WId taskwindow, const bool hovered)
{
    if (hovered) {
        return QString::fromLatin1("hover");
    } else if (kIsTaskActive(taskwindow)) {
        return QString::fromLatin1("focus");
    } else if (kDoesTaskDemandAttention(taskwindow)) {
        return QString::fromLatin1("attention");
    }
    return QString::fromLatin1("normal");
}

class TasksSvg : public Plasma::SvgWidget
{
    Q_OBJECT
public:
    TasksSvg(const WId taskwindow, QGraphicsItem *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) final;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) final;

private Q_SLOTS:
    void slotClicked(const Qt::MouseButton button);
    void slotUpdateSvgAndToolTip();
    void slotWindowChanged(::TaskManager::Task *task);

private:
    void updateToolTip();

    WId m_taskwindow;
    bool m_hovered;
    Plasma::FrameSvg* m_framesvg;
};

TasksSvg::TasksSvg(const WId taskwindow, QGraphicsItem *parent)
    : Plasma::SvgWidget(parent),
    m_taskwindow(taskwindow),
    m_hovered(false),
    m_framesvg(nullptr)
{
    slotUpdateSvgAndToolTip();
    setAcceptHoverEvents(true);
    connect(
        this, SIGNAL(clicked(Qt::MouseButton)),
        this, SLOT(slotClicked(Qt::MouseButton))
    );
    connect(
        TaskManager::TaskManager::self(), SIGNAL(windowChanged(::TaskManager::Task*,::TaskManager::TaskChanges)),
        this, SLOT(slotWindowChanged(::TaskManager::Task*))
    );
    connect(
        Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()),
        this, SLOT(slotUpdateSvgAndToolTip())
    );
}


void TasksSvg::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setFont(KGlobalSettings::taskbarFont());
    const QRectF brect = boundingRect();
    const QSizeF brectsize = brect.size();
    m_framesvg->setElementPrefix(kElementPrefixForTask(m_taskwindow, m_hovered));
    m_framesvg->resizeFrame(brectsize);
    m_framesvg->paintFrame(painter, brect);
    // TODO: both can be optional
    const int spacingoffset = (s_spacing * 2);
    const int iconsize = qRound(qMin(brectsize.width(), brectsize.height()));
    // TODO: pixmap effect based on task state
    QPixmap iconpixmap = kTaskPixmap(m_taskwindow, iconsize, iconsize);
    if (!iconpixmap.isNull()) {
        iconpixmap = iconpixmap.scaled(
            iconsize - spacingoffset,
            iconsize - spacingoffset,
            Qt::KeepAspectRatio, Qt::SmoothTransformation
        );
    }
    if (!iconpixmap.isNull()) {
        painter->drawPixmap(QPoint(s_spacing, s_spacing), iconpixmap);
    }
    QFontMetrics framefontmetrics(painter->font());
    const QPoint textpoint = QPoint(
        iconpixmap.width() + spacingoffset,
        (brectsize.height() / 2) + (framefontmetrics.height() / 2) - (s_spacing / 2)
    );
    painter->drawText(
        textpoint,
        framefontmetrics.elidedText(
            kTaskName(m_taskwindow),
            Qt::ElideRight,
            brectsize.width() - textpoint.x() - s_spacing
        )
    );
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

void TasksSvg::updateToolTip()
{
    Plasma::ToolTipContent plasmatooltip;
    plasmatooltip.setMainText(QString::fromLatin1("<center>%1</center>").arg(kTaskName(m_taskwindow)));
    plasmatooltip.setWindowToPreview(m_taskwindow);
    Plasma::ToolTipManager::self()->setContent(this, plasmatooltip);
}

void TasksSvg::slotClicked(const Qt::MouseButton button)
{
    // TODO: context menu on right-click
    if (button == Qt::LeftButton) {
        foreach (TaskManager::Task *task, TaskManager::TaskManager::self()->tasks()) {
            if (task->window() == m_taskwindow) {
                task->activateRaiseOrIconify();
                break;
            }
        }
    }
}

void TasksSvg::slotUpdateSvgAndToolTip()
{
    if (m_framesvg) {
        delete m_framesvg;
    }
    m_framesvg = new Plasma::FrameSvg(this);
    m_framesvg->setImagePath("widgets/tasks");
    setSvg(m_framesvg);
    updateToolTip();
}

void TasksSvg::slotWindowChanged(::TaskManager::Task *task)
{
    if (task->window() == m_taskwindow) {
        updateToolTip();
        update();
    }
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
}

void TasksApplet::init()
{
    slotUpdateLayout();
    connect(
        TaskManager::TaskManager::self(), SIGNAL(taskAdded(::TaskManager::Task*)),
        this, SLOT(slotUpdateLayout())
    );
    connect(
        TaskManager::TaskManager::self(), SIGNAL(taskRemoved(::TaskManager::Task*)),
        this, SLOT(slotUpdateLayout())
    );
}

void TasksApplet::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::SizeConstraint || constraints & Plasma::FormFactorConstraint) {
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
                break;
            }
        }
        updateSizes();
    }
}

void TasksApplet::updateSizes()
{
    QMutexLocker locker(&m_mutex);
    QSizeF maximumtasksize;
    if (m_layout->orientation() == Qt::Horizontal) {
        maximumtasksize = QSizeF(s_maximumtasksize, QWIDGETSIZE_MAX);
    } else {
        maximumtasksize = QSizeF(QWIDGETSIZE_MAX, s_maximumtasksize);
    }
    foreach (TasksSvg* taskssvg, m_taskssvgs) {
        taskssvg->setMaximumSize(maximumtasksize);
    }
}

void TasksApplet::slotUpdateLayout()
{
    // TODO: this should be incremental and animated
    QMutexLocker locker(&m_mutex);
    foreach (TasksSvg* taskssvg, m_taskssvgs) {
        m_layout->removeItem(taskssvg);
    }
    qDeleteAll(m_taskssvgs);
    m_taskssvgs.clear();
    if (m_spacer) {
        m_layout->removeItem(m_spacer);
    }
    adjustSize();
    foreach (TaskManager::Task *task, TaskManager::TaskManager::self()->tasks()) {
        TasksSvg* taskssvg = new TasksSvg(task->window(), this);
        m_layout->addItem(taskssvg);
        m_taskssvgs.append(taskssvg);
    }
    // TODO: once the taskbar is nearly full show arrow for for items that shall be available via
    // menu-like widget
    if (!m_spacer) {
        m_spacer = new QGraphicsWidget(this);
        m_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_spacer->setMinimumSize(1, 1);
    }
    m_layout->addItem(m_spacer);
}

#include "moc_tasks.cpp"
#include "tasks.moc"
