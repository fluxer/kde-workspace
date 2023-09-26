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
#include <QGridLayout>
#include <QLabel>
#include <Plasma/Svg>
#include <Plasma/FrameSvg>
#include <Plasma/SvgWidget>
#include <Plasma/Theme>
#include <Plasma/ToolTipManager>
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
static const bool s_defaulthidesingle = true;

static QString kElementPrefixForDesktop(const int desktop, const bool hovered)
{
    if (hovered) {
        return QString::fromLatin1("hover");
    } else if (KWindowSystem::currentDesktop() == desktop) {
        return QString::fromLatin1("active");
    }
    return QString::fromLatin1("normal");
}

static QRectF kAdjustRect(const QRectF &rect)
{
    QRectF result = rect;
    result.setWidth(result.width() - (s_spacing * 2));
    return result;
}

static QFont kGetFont()
{
    QFont font = KGlobalSettings::smallestReadableFont();
    font.setBold(true);
    return font;
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

class PagerSvg : public Plasma::SvgWidget
{
    Q_OBJECT
public:
    PagerSvg(const int desktop, const PagerApplet::PagerMode pagermode,QGraphicsItem *parent = nullptr);

    void setup(const PagerApplet::PagerMode pagermode);

private Q_SLOTS:
    void slotClicked(const Qt::MouseButton button);
    void slotUpdate();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint) const final;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) final;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) final;
    // handled here too
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) final;

private Q_SLOTS:
    void slotUpdateSvgAndToolTip();

private:
    int m_desktop;
    bool m_hovered;
    Plasma::FrameSvg* m_framesvg;
    PagerApplet::PagerMode m_pagermode;
};

PagerSvg::PagerSvg(const int desktop, const PagerApplet::PagerMode pagermode, QGraphicsItem *parent)
    : Plasma::SvgWidget(parent),
    m_desktop(desktop),
    m_hovered(false),
    m_framesvg(nullptr),
    m_pagermode(pagermode)
{
    slotUpdateSvgAndToolTip();
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
        this, SLOT(slotUpdateSvgAndToolTip())
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
    m_framesvg->setElementPrefix(kElementPrefixForDesktop(m_desktop, m_hovered));
    m_framesvg->resizeFrame(brect.size());
    m_framesvg->paintFrame(painter, brect);
    switch (m_pagermode) {
        case PagerApplet::ShowNumber: {
            painter->save();
            painter->setFont(kGetFont());
            painter->translate(s_spacing, 0);
            painter->drawText(kAdjustRect(brect.toRect()), QString::number(m_desktop), QTextOption(Qt::AlignCenter));
            painter->restore();
            break;
        }
        case PagerApplet::ShowName: {
            painter->save();
            painter->setFont(kGetFont());
            painter->translate(s_spacing, 0);
            painter->drawText(kAdjustRect(brect.toRect()), KWindowSystem::desktopName(m_desktop), QTextOption(Qt::AlignCenter));
            painter->restore();
            break;
        }
    }
}

QSizeF PagerSvg::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    if (m_pagermode == PagerApplet::ShowName) {
        QSizeF svgwidgethint = Plasma::SvgWidget::sizeHint(which, constraint);
        svgwidgethint.setWidth(svgwidgethint.width() * 2);
        return svgwidgethint;
    }
    return Plasma::SvgWidget::sizeHint(which, constraint);
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

void PagerSvg::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (kHandleMouseEvent(event)) {
        event->accept();
        return;
    }
    Plasma::SvgWidget::mouseReleaseEvent(event);
}

void PagerSvg::slotUpdateSvgAndToolTip()
{
    if (m_framesvg) {
        delete m_framesvg;
    }
    m_framesvg = new Plasma::FrameSvg(this);
    m_framesvg->setImagePath("widgets/pager");
    setSvg(m_framesvg);
    Plasma::ToolTipContent plasmatooltip;
    plasmatooltip.setMainText(QString::fromLatin1("<center>%1</center>").arg(KWindowSystem::desktopName(m_desktop)));
    Plasma::ToolTipManager::self()->setContent(this, plasmatooltip);
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
    m_pagermode(s_defaultpagermode),
    m_hidesingle(s_defaulthidesingle),
    m_pagermodebox(nullptr),
    m_hidesinglebox(nullptr),
    m_spacer(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_pager");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    setPreferredSize(s_preferredsize);
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
    KConfigGroup configgroup = config();
    const PagerApplet::PagerMode oldpagermode = m_pagermode;
    const bool oldhidesingle = m_hidesingle;
    m_pagermode = static_cast<PagerApplet::PagerMode>(configgroup.readEntry("pagerMode", static_cast<int>(s_defaultpagermode)));
    m_hidesingle = configgroup.readEntry("pagerHideSingle", s_defaulthidesingle);

    if (oldpagermode != m_pagermode || oldhidesingle != m_hidesingle) {
        // layout again with the new mode for size hints to apply correct
        slotUpdateLayout();
    }

    connect(
        KWindowSystem::self(), SIGNAL(numberOfDesktopsChanged(int)),
        this, SLOT(slotUpdateLayout())
    );
}

void PagerApplet::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget* widget = new QWidget();
    QGridLayout* widgetlayout = new QGridLayout(widget);
    QLabel* pagermodelabel = new QLabel(widget);
    pagermodelabel->setText(i18n("Text:"));
    widgetlayout->addWidget(pagermodelabel, 0, 0);
    m_pagermodebox = new QComboBox(widget);
    m_pagermodebox->addItem(i18n("Desktop number"), static_cast<int>(PagerApplet::ShowNumber));
    m_pagermodebox->addItem(i18n("Desktop name"), static_cast<int>(PagerApplet::ShowName));
    m_pagermodebox->setCurrentIndex(static_cast<int>(m_pagermode));
    widgetlayout->addWidget(m_pagermodebox, 0, 1);
    m_hidesinglebox = new QCheckBox(widget);
    m_hidesinglebox->setText(i18n("Hide when there is only one desktop"));
    m_hidesinglebox->setChecked(m_hidesingle);
    widgetlayout->addWidget(m_hidesinglebox, 1, 0, 1, 2);
    m_spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    widgetlayout->addItem(m_spacer, 2, 0, 1, 2);
    widget->setLayout(widgetlayout);
    // doesn't look like media visualization but ok.. (that's what the clock applet uses)
    parent->addPage(widget, i18n("Appearance"), "view-media-visualization");

    connect(parent, SIGNAL(applyClicked()), this, SLOT(slotConfigAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(slotConfigAccepted()));
    connect(m_pagermodebox, SIGNAL(currentIndexChanged(int)), parent, SLOT(settingsModified()));
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
    setPreferredSize(preferredsize);
    switch (m_layout->orientation()) {
        case Qt::Horizontal: {
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            setMinimumSize(iconsize * pagersvgssize, iconsize);
            break;
        }
        case Qt::Vertical: {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            setMinimumSize(iconsize, iconsize * pagersvgssize);
            break;
        }
    }
    emit sizeHintChanged(Qt::PreferredSize);
    emit sizeHintChanged(Qt::MinimumSize);
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
    if (m_hidesingle && numberofdesktops == 1) {
        // the applet becomes invisible, can't even configure it and adding it to the desktop looks
        // like nothing happens in such case. that is how the old implementation was doing it but
        // that may change
        adjustSize();
        return;
    }
    for (int i = 0; i < numberofdesktops; i++) {
        PagerSvg* pagersvg = new PagerSvg(i + 1, m_pagermode, this);
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

void PagerApplet::slotConfigAccepted()
{
    Q_ASSERT(m_pagermodebox != nullptr);
    Q_ASSERT(m_hidesinglebox != nullptr);
    const int pagermodeindex = m_pagermodebox->currentIndex();
    m_pagermode = static_cast<PagerApplet::PagerMode>(pagermodeindex);
    m_hidesingle = m_hidesinglebox->isChecked();
    KConfigGroup configgroup = config();
    configgroup.writeEntry("pagerMode", pagermodeindex);
    configgroup.writeEntry("pagerHideSingle", m_hidesingle);
    slotUpdateLayout();
    emit configNeedsSaving();
}

#include "moc_pager.cpp"
#include "pager.moc"
