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
#include "kworkspace/ktaskmanager.h"

#include <QGraphicsSceneContextMenuEvent>
#include <Plasma/Svg>
#include <Plasma/FrameSvg>
#include <Plasma/SvgWidget>
#include <Plasma/Theme>
#include <Plasma/ToolTipManager>
#include <KWindowSystem>
#include <KIcon>
#include <KIconLoader>
#include <KDebug>

// standard issue margin/spacing
static const int s_spacing = 6;
// TODO: this should be configurable
static const int s_maximumtasksize = 200;

static QPixmap kTaskPixmap(const KTaskManager::Task &task, const int size)
{
    return KIcon(task.icon).pixmap(size);
}

static QString kElementPrefixForTask(const KTaskManager::Task &task, const bool hovered)
{
    if (hovered) {
        return QString::fromLatin1("hover");
    } else if (KTaskManager::self()->isActive(task)) {
        return QString::fromLatin1("focus");
    } else if (KTaskManager::self()->demandsAttention(task)) {
        return QString::fromLatin1("attention");
    }
    return QString::fromLatin1("normal");
}

class TasksSvg : public Plasma::SvgWidget
{
    Q_OBJECT
public:
    TasksSvg(const KTaskManager::Task &task, QGraphicsItem *parent = nullptr);

    void setOrientation(const Qt::Orientation orientation);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const final;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) final;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) final;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) final;

private Q_SLOTS:
    void slotClicked(const Qt::MouseButton button);
    void slotUpdateSvgAndToolTip();
    void slotTaskChanged(const KTaskManager::Task &task);

private:
    void updateToolTip();

    KTaskManager::Task m_task;
    bool m_hovered;
    Qt::Orientation m_orientation;
    Plasma::FrameSvg* m_framesvg;
};

TasksSvg::TasksSvg(const KTaskManager::Task &task, QGraphicsItem *parent)
    : Plasma::SvgWidget(parent),
    m_task(task),
    m_hovered(false),
    m_orientation(Qt::Horizontal),
    m_framesvg(nullptr)
{
    slotUpdateSvgAndToolTip();
    setAcceptHoverEvents(true);
    connect(
        this, SIGNAL(clicked(Qt::MouseButton)),
        this, SLOT(slotClicked(Qt::MouseButton))
    );
    connect(
        KTaskManager::self(), SIGNAL(taskChanged(KTaskManager::Task)),
        this, SLOT(slotTaskChanged(KTaskManager::Task))
    );
    connect(
        Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()),
        this, SLOT(slotUpdateSvgAndToolTip())
    );
}

void TasksSvg::setOrientation(const Qt::Orientation orientation)
{
    m_orientation = orientation;
}

void TasksSvg::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setFont(KGlobalSettings::taskbarFont());
    const QRectF brect = boundingRect();
    const QSizeF brectsize = brect.size();
    m_framesvg->setElementPrefix(kElementPrefixForTask(m_task, m_hovered));
    m_framesvg->resizeFrame(brectsize);
    m_framesvg->paintFrame(painter, brect);
    const int spacingoffset = (s_spacing * 2);
    const int iconsize = qRound(qMin(brectsize.width(), brectsize.height()));
    // TODO: pixmap effect based on task state
    QPixmap iconpixmap = kTaskPixmap(m_task, iconsize);
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
}

QSizeF TasksSvg::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    if (which == Qt::PreferredSize) {
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

void TasksSvg::updateToolTip()
{
    Plasma::ToolTipContent plasmatooltip;
    plasmatooltip.setMainText(QString::fromLatin1("<center>%1</center>").arg(m_task.name));
    plasmatooltip.setWindowToPreview(m_task.window);
    Plasma::ToolTipManager::self()->setContent(this, plasmatooltip);
}

void TasksSvg::slotClicked(const Qt::MouseButton button)
{
    if (button == Qt::LeftButton) {
        KTaskManager::self()->activateRaiseOrIconify(m_task);
    }
}

void TasksSvg::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu* taskmenu = KTaskManager::menuForTask(m_task, nullptr);
    if (!taskmenu) {
        Plasma::SvgWidget::contextMenuEvent(event);
        return;
    }
    event->accept();
    taskmenu->exec(QCursor::pos());
    taskmenu->deleteLater();
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

void TasksSvg::slotTaskChanged(const KTaskManager::Task &task)
{
    if (task.id == m_task.id) {
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
        KTaskManager::self(), SIGNAL(taskAdded(KTaskManager::Task)),
        this, SLOT(slotUpdateLayout())
    );
    connect(
        KTaskManager::self(), SIGNAL(taskRemoved(KTaskManager::Task)),
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
        updateOrientation();
    }
}

void TasksApplet::updateOrientation()
{
    QMutexLocker locker(&m_mutex);
    foreach (TasksSvg* taskssvg, m_taskssvgs) {
        taskssvg->setOrientation(m_layout->orientation());
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
    foreach (const KTaskManager::Task &task, KTaskManager::self()->tasks()) {
        TasksSvg* taskssvg = new TasksSvg(task, this);
        taskssvg->setOrientation(m_layout->orientation());
        m_layout->addItem(taskssvg);
        m_taskssvgs.append(taskssvg);
    }
    // TODO: once the taskbar is nearly full show arrow for items that shall be available via
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
