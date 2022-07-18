/***************************************************************************
 *   Copyright 2022 by Chaz Peloquin <chazpelo2@gmail.com>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "sangria.h"

//QT
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QAction>
#include <QPainter>
#include <QGraphicsView>
#include <QTimer>
#include <QtCore/qsharedpointer.h>

//KDE
#include <KCModuleProxy>
#include <KConfigDialog>
#include <KDebug>
#include <KFilePlacesModel>
#include <KGlobalSettings>
#include <KIconLoader>
#include <KMessageBox>
#include <KLocale>
#include <KNotification>
#include <QProcess>
#include <KRun>
#include <KSharedConfig>
#include <KStandardDirs>
#include <KUrl>
#include <KWindowSystem>
#include <KAuthorized>
#include <kworkspace/kworkspace.h>

#include <KIO/CopyJob>
#include <KIO/JobUiDelegate>

//Plasma
#include <Plasma/Animation>
#include <Plasma/Applet>
#include <Plasma/FrameSvg>
#include <Plasma/PaintUtils>
#include <Plasma/Theme>
#include <Plasma/ToolTipContent>
#include <Plasma/ItemBackground>
#include <Plasma/IconWidget>
#include <Plasma/Containment>
#include <Plasma/ToolTipManager>

SangriaMenu::SangriaMenu(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
      m_icon(0),
      m_dirLister(0),
      m_emptyAction(0),
      m_count(0),
      m_showText(false),
      m_places(0),
      m_proxy(0),
      m_emptyProcess(0)
{
    setHasConfigurationInterface(true);
    setAspectRatioMode(Plasma::ConstrainedSquare);

    m_icon = new Plasma::IconWidget(KIcon("kde"),QString(),this);
    m_icon->setNumDisplayLines(2);
    m_icon->setDrawBackground(true);
    setBackgroundHints(NoBackground);

    resize(m_icon->sizeFromIconSize(IconSize(KIconLoader::Desktop)));
    createMenu();
}

SangriaMenu::~SangriaMenu()
{
}

void SangriaMenu::init()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addItem(m_icon);

    setAcceptDrops(true);
    installSceneEventFilter(m_icon);

    connect(m_icon, SIGNAL(activated()), this, SLOT(popup()));
    connect(m_icon, SIGNAL(clicked()), this, SLOT(open()));
    connect(KGlobalSettings::self(), SIGNAL(iconChanged(int)),
            this, SLOT(iconSizeChanged(int)));
}

void SangriaMenu::createMenu()
{
    QAction* shutdown = new QAction(SmallIcon("system-shutdown"), i18n("Leave..."), this);
    connect(shutdown, SIGNAL(triggered(bool)), this, SLOT(startLogout()));

    m_menu.addAction(shutdown);

    //add the menu as an action icon
    QAction* menu = new QAction(SmallIcon("arrow-up-double"),i18n("&Menu"), this);
    connect(menu, SIGNAL(triggered(bool)), this , SLOT(popup()));
    m_icon->addIconAction(menu);

    connect(&m_menu, SIGNAL(aboutToHide()), m_icon, SLOT(setUnpressed()));
}

void SangriaMenu::popup()
{
    if (m_menu.isVisible()) {
        m_menu.hide();
        return;
    }
    m_menu.popup(popupPosition(m_menu.sizeHint()));
    m_icon->setPressed();
}

void SangriaMenu::updateIcon()
{
    Plasma::ToolTipContent data;
    data.setMainText(i18n("Melon Menu"));

    m_icon->setIcon("kde");

    m_icon->update();

    data.setImage(m_icon->icon().pixmap(IconSize(KIconLoader::Desktop)));
}

void SangriaMenu::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::FormFactorConstraint) {
        disconnect(m_icon, SIGNAL(activated()), this, SLOT(open()));
        disconnect(m_icon, SIGNAL(clicked()), this, SLOT(open()));

        if (formFactor() == Plasma::Planar ||
            formFactor() == Plasma::MediaCenter) {

            connect(m_icon, SIGNAL(activated()), this, SLOT(open()));

            m_icon->setText(i18n("Melon Menu"));
            m_showText = true;
            m_icon->setDrawBackground(true);
            //Adding an arbitrary width to make room for a larger count of items
            setMinimumSize(m_icon->sizeFromIconSize(IconSize(KIconLoader::Desktop))+=QSizeF(20,0));
        } else {
            //in a panel the icon always behaves like a button
            connect(m_icon, SIGNAL(clicked()), this, SLOT(open()));

            m_icon->setText(QString());
            m_icon->setInfoText(QString());
            m_showText = false;
            m_icon->setDrawBackground(false);

            setMinimumSize(m_icon->sizeFromIconSize(IconSize(KIconLoader::Small)));
        }
        updateIcon();
    }
}

void SangriaMenu::open()
{
    setFocus();
}

QList<QAction*> SangriaMenu::contextualActions()
{
    return actions;
}

QSizeF SangriaMenu::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const
{
    if (which == Qt::PreferredSize) {
        int iconSize = 0;

        switch (formFactor()) {
            case Plasma::Planar:
            case Plasma::MediaCenter:
                iconSize = IconSize(KIconLoader::Desktop);
                break;

            case Plasma::Horizontal:
            case Plasma::Vertical:
                iconSize = IconSize(KIconLoader::Panel);
                break;
        }

        return QSizeF(iconSize, iconSize);
    }

    return Plasma::Applet::sizeHint(which, constraint);
}

void SangriaMenu::iconSizeChanged(int group)
{
    if (group == KIconLoader::Desktop || group == KIconLoader::Panel) {
        updateGeometry();
    }
}

void SangriaMenu::startLogout()
{
    QTimer::singleShot(10, this, SLOT(logout()));
}

void SangriaMenu::logout()
{
    if (!KAuthorized::authorizeKAction("logout")) {
        return;
    }

    KWorkSpace::requestShutDown();
}

#include "moc_sangria.cpp"