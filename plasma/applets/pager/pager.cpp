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

#include <QX11Info>
#include <Plasma/Svg>
#include <Plasma/FrameSvg>
#include <Plasma/SvgWidget>
#include <Plasma/Theme>
#include <Plasma/PaintUtils>
#include <KWindowSystem>
#include <KIcon>
#include <KIconLoader>
#include <KDebug>
#include <netwm.h>

// standard issue margin/spacing
static const int s_spacing = 4;
// the applet may be hidden unless there are two virtual desktops
static const QSizeF s_preferredsize = QSizeF(0, 0);
static PagerApplet::PagerMode s_defaultpagermode = PagerApplet::ShowNumber;

static QString kElementForDesktop(const int desktop, const bool hovered)
{
    if (hovered) {
        return QString::fromLatin1("hover");
    } else if (KWindowSystem::currentDesktop() == desktop) {
        return QString::fromLatin1("active");
    }
    return QString::fromLatin1("normal");
}

static QFont kGetFont(const int height)
{
    QFont font = KGlobalSettings::smallestReadableFont();
    font.setBold(true);
    return font;
}

class PagerSvg : public Plasma::SvgWidget
{
    Q_OBJECT
public:
    PagerSvg(const int desktop, QGraphicsItem *parent = nullptr);

    void setup(const PagerApplet::PagerMode pagermode);

private Q_SLOTS:
    void slotClicked(const Qt::MouseButton button);
    void slotUpdate();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) final;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) final;

private Q_SLOTS:
    void slotUpdateSvg();

private:
    int m_desktop;
    bool m_hovered;
    Plasma::FrameSvg* m_framesvg;
    PagerApplet::PagerMode m_pagermode;
};

PagerSvg::PagerSvg(const int desktop, QGraphicsItem *parent)
    : Plasma::SvgWidget(parent),
    m_desktop(desktop),
    m_hovered(false),
    m_framesvg(nullptr),
    m_pagermode(s_defaultpagermode)
{
    // TODO: tooltip, maybe drag-n-drop
    slotUpdateSvg();
    setAcceptHoverEvents(true);
    connect(
        this, SIGNAL(clicked(Qt::MouseButton)),
        this, SLOT(slotClicked(Qt::MouseButton))
    );
    connect(
        KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)),
        this, SLOT(slotUpdate())
    );
    connect(
        KWindowSystem::self(), SIGNAL(desktopNamesChanged()),
        this, SLOT(slotUpdate())
    );
    connect(
        Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()),
        this, SLOT(slotUpdateSvg())
    );
}

void PagerSvg::setup(const PagerApplet::PagerMode pagermode)
{
    m_pagermode = pagermode;
    update();
}

void PagerSvg::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const QRectF brect = boundingRect();
    m_framesvg->setElementPrefix(kElementForDesktop(m_desktop, m_hovered));
    m_framesvg->resizeFrame(brect.size());
    m_framesvg->paintFrame(painter, brect);
    switch (m_pagermode) {
        // TODO: different font size for panels
        case PagerApplet::ShowNumber: {
            painter->save();
            painter->setFont(kGetFont(brect.height()));
            painter->drawText(brect.toRect(), QString::number(m_desktop), QTextOption(Qt::AlignCenter));
            painter->restore();
            break;
        }
        case PagerApplet::ShowName: {
            painter->save();
            painter->setFont(kGetFont(brect.height()));
            painter->drawText(brect.toRect(), KWindowSystem::desktopName(m_desktop), QTextOption(Qt::AlignCenter));
            painter->restore();
            break;
        }
    }
}

void PagerSvg::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_hovered = true;
    update();
}

void PagerSvg::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_hovered = false;
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


PagerApplet::PagerApplet(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_layout(nullptr),
    m_adddesktopaction(nullptr),
    m_removedesktopaction(nullptr),
    m_pagermode(s_defaultpagermode)
{
    // TODO: mouse wheel events
    KGlobal::locale()->insertCatalog("plasma_applet_pager");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    setPreferredSize(s_preferredsize);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout = new QGraphicsLinearLayout(Qt::Horizontal, this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(s_spacing);
    
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
    // TODO:
}

QList<QAction*> PagerApplet::contextualActions()
{
    return m_actions;
}

// NOTE: keep in sync with:
// plasma/applets/lockout/lockout.cpp
void PagerApplet::updateSizes()
{
    QMutexLocker locker(&m_mutex);
    const int pagersvgssize = m_pagersvgs.size();
    QSizeF preferredsize = s_preferredsize;
    foreach (PagerSvg *pagersvg, m_pagersvgs) {
        preferredsize += pagersvg->preferredSize();
    }
    locker.unlock();

    int iconsize = 0;
    switch (formFactor()) {
        case Plasma::FormFactor::Horizontal:
        case Plasma::FormFactor::Vertical: {
            // panel
            iconsize = KIconLoader::global()->currentSize(KIconLoader::Panel);
            break;
        }
        default: {
            // desktop-like
            iconsize = (KIconLoader::global()->currentSize(KIconLoader::Desktop) * 2);
            break;
        }
    }
    // the width is based on the number of desktops
    setMinimumSize(iconsize * pagersvgssize, iconsize);
    emit sizeHintChanged(Qt::MinimumSize);

    setPreferredSize(preferredsize);
    switch (m_layout->orientation()) {
        case Qt::Horizontal: {
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            break;
        }
        case Qt::Vertical: {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            break;
        }
    }
    emit sizeHintChanged(Qt::PreferredSize);
}

void PagerApplet::constraintsEvent(Plasma::Constraints constraints)
{
    // perfect size finder
    // qDebug() << Q_FUNC_INFO << size();
    if (constraints & Plasma::SizeConstraint || constraints & Plasma::FormFactorConstraint) {
        switch (formFactor()) {
            case Plasma::FormFactor::Horizontal: {
                m_layout->setOrientation(Qt::Horizontal);
                m_layout->setSpacing(0);
                updateSizes();
                return;
            }
            case Plasma::FormFactor::Vertical: {
                m_layout->setOrientation(Qt::Vertical);
                m_layout->setSpacing(0);
                updateSizes();
                return;
            }
            default: {
                m_layout->setSpacing(s_spacing);
                break;
            }
        }

        const QSizeF appletsize = size();
        if (appletsize.width() >= appletsize.height()) {
            m_layout->setOrientation(Qt::Horizontal);
        } else {
            m_layout->setOrientation(Qt::Vertical);
        }
        updateSizes();
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
    for (int i = 0; i < numberofdesktops; i++) {
        PagerSvg* pagersvg = new PagerSvg(i + 1, this);
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
    locker.unlock();
    updateSizes();
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

#include "moc_pager.cpp"
#include "pager.moc"
