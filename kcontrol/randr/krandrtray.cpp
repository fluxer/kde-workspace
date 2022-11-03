/*
 * Copyright (c) 2007      Gustavo Pichorim Boiko <gustavo.boiko@kdemail.net>
 * Copyright (c) 2002,2003 Hamish Rodda <rodda@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <config-X11.h>

#include "krandrtray.h"
#include "krandrpassivepopup.h"
#include "moc_krandrtray.cpp"
#include "randrscreen.h"
#include "randroutput.h"
#include "randrmode.h"

#include <QtGui/qevent.h>
#include <QVariant>

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KCMultiDialog>
#include <KComponentData>
#include <KHelpMenu>
#include <KIcon>
#include <KIconLoader>
#include <KLocale>
#include <KMenu>
#include <KWindowSystem>

KRandRSystemTray::KRandRSystemTray(RandRDisplay *dpy, QWidget* parent)
    : KStatusNotifierItem(parent),
    m_help(new KHelpMenu(parent, KGlobal::mainComponent().aboutData(), false, actionCollection())),
    m_display(dpy)
{
    setIconByName("preferences-desktop-display-randr");
    setCategory(Hardware);

    m_menu = new KMenu(associatedWidget());
    setContextMenu(m_menu);
    setStatus(Active);

    //TODO: probably we need an about to show signal
    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(slotPrepareMenu()));
    m_display->refresh();
    updateToolTip();

    OutputMap outputs = m_display->currentScreen()->outputs();
    foreach(RandROutput *output, outputs) {
        connect(output, SIGNAL(outputChanged(RROutput,int)), this, SLOT(slotPrepareMenu()));
        connect(output, SIGNAL(outputChanged(RROutput,int)), this, SLOT(updateToolTip()));
    }
}

KRandRSystemTray::~KRandRSystemTray()
{
    if (m_kcm) {
        delete m_kcm.data();
    }
}

void KRandRSystemTray::activate(const QPoint &pos)
{
    Q_UNUSED(pos)
    slotPrefs();
}

void KRandRSystemTray::slotPrepareMenu()
{
    QAction *action;

    m_menu->clear();

    if (!m_display->isValid()) {
        action = m_menu->addAction(i18n("Required X Extension Not Available"));
        action->setEnabled(false);
    } else  {
        m_screenPopups.clear();
        for (int s = 0; s < m_display->numScreens(); s++)  {
            m_display->setCurrentScreen(s);
            if (s == QX11Info::appScreen()) {
                /*lastIndex = menu->insertItem(i18n("Screen %1").arg(s+1));
                menu->setItemEnabled(lastIndex, false);*/
            } else  {
                KMenu* subMenu = new KMenu(i18n("Screen %1", s+1), m_menu );
                subMenu->setObjectName( QString("screen%1").arg(s+1) );
                m_screenPopups.append(subMenu);
                populateMenu(subMenu);
                action = m_menu->addMenu(subMenu);
                connect(subMenu, SIGNAL(activated(int)), SLOT(slotScreenActivated()));
            }
        }

        m_display->setCurrentScreen(m_display->screenIndexOfWidget(m_menu));
        if (m_display->needsRefresh()) {
            kDebug() << "Configuration dirty, reloading settings...";
            m_display->refresh();
            updateToolTip();
        }

        populateMenu(m_menu);
    }

    m_menu->addSeparator();

    KAction *actPrefs = actionCollection()->addAction( QString() );
    actPrefs->setIcon( KIcon( "configure" ) );
    actPrefs->setText( i18n( "Configure Display..." ) );

    connect( actPrefs, SIGNAL(triggered(bool)), SLOT(slotPrefs()) );
    m_menu->addAction( actPrefs );

    m_menu->addMenu(/*SmallIcon("help-contents"),KStandardGuiItem::help().text(),*/ m_help->menu());
    QAction *quitAction = actionCollection()->action(KStandardAction::name(KStandardAction::Quit));
    m_menu->addAction( quitAction );
}

void KRandRSystemTray::slotScreenActivated()
{
    m_display->setCurrentScreen(m_screenPopups.indexOf(static_cast<KMenu*>(sender())));
}

void KRandRSystemTray::updateToolTip()
{
    const QString icon = "preferences-desktop-display-randr";
    QString title = i18n("Display");
    QString subTitle = i18n("Resize, rotate and configure screens.");
    if (m_display->isValid()) {
        OutputMap outputs = m_display->currentScreen()->outputs();
        if (outputs.count() <= 0) {
            return;
        }

        RandRScreen *screen = m_display->currentScreen();
        Q_ASSERT(screen);

        if (screen->outputsUnified() && screen->connectedCount() > 1) {
            SizeList sizes = screen->unifiedSizes();
            if (!sizes.isEmpty()) {
                const QSize currentSize = screen->rect().size();
                subTitle = i18n("Resolution: %1 x %2",
                                QString::number(currentSize.width()),
                                QString::number(currentSize.height()));
                int rotations = screen->unifiedRotations();
                if (rotations != RandR::Rotate0) {
                    int rotation = RandR::Rotate0;
                    foreach (RandROutput *output, screen->outputs()) {
                        if (output->isActive()) {
                            rotation = output->rotation();
                            break;
                        }
                    }

                    if (rotation != RandR::Rotate0) {
                        subTitle += "<br>" + i18n("Rotation: %1", RandR::rotationName(1 << rotation));
                    }
                }
            }
        } else {
            QString details = "<table>";
            bool first = true;
            foreach(RandROutput *output, outputs) {
                if (output->isConnected()) {
                    if (!output->isActive()) {
                        details += "<tr><td>&nbsp;</td></tr>";
                        details += "<tr><th align=\"left\" colspan=\"2\">" + output->name()
                                + " (" + i18n("Disabled") + ") </th></tr>";
                    } else {
                        if (first) {
                            first = false;
                            details += "<tr><td>&nbsp;</td></tr>";
                        } else {
                            details += "<tr><td>&nbsp;</td></tr>";
                        }
                        details += "<tr><th align=\"left\" colspan=\"2\">" + output->name()
                                + "</th></tr>";

                        QSize currentSize = output->rect().size();
                        if (output->rotation() & (RandR::Rotate90 | RandR::Rotate270)) {
                            currentSize = QSize(currentSize.height(), currentSize.width());
                        }
                        details += "<tr>" + i18n("<td align=\"right\">Resolution: </td><td>%1 x %2</td></tr>",
                                    QString::number(currentSize.width()),
                                    QString::number(currentSize.height()));
                        RateList rates = output->refreshRates();
                        if (rates.count()) {
                            details += "<tr><td align=\"right\">" + i18n("Refresh: ") + "</td><td>"
                                    + ki18n("%1 Hz").subs(output->refreshRate(), 0, 'f', 1).toString()
                                    + "</td></tr>";
                        }

                        int rotations = output->rotations();
                        if (rotations != RandR::Rotate0 && output->rotation() != RandR::Rotate0) {
                            details += "<tr><td align=\"right\">" + i18n("Rotation: ") + "</td><td>"
                                    + RandR::rotationName(1 << output->rotation())
                                    + "</td></tr>";
                        }
                    }
                }
            }

            if (details != "<table>") {
                subTitle = details + "</table>";
            }
        }
    }

    setToolTip(icon, title, subTitle);
}

void KRandRSystemTray::configChanged()
{
    m_display->refresh();
    updateToolTip();
    static bool first = true;

    if (!first) {
        // TODO: display config changed message
        QString message = "Screen config changed";

        KRandrPassivePopup::message(
            i18n("Screen configuration has changed"),
            message, SmallIcon("view-fullscreen"),
            associatedWidget()
        );
    }

    first = false;
}

void KRandRSystemTray::populateMenu(KMenu* menu)
{
    QAction *action;
    QActionGroup *actionGroup;

    OutputMap outputs = m_display->currentScreen()->outputs();
    if (outputs.count() <= 0) {
        return;
    }

    RandRScreen *screen = m_display->currentScreen();
    Q_ASSERT(screen);

    // if the outputs are unified, do not show output-specific size 
    // changing options in the tray.
    if (screen->outputsUnified() && screen->connectedCount() > 1) {
        SizeList sizes = screen->unifiedSizes();
    
        if (sizes.count()) {
            // populate unified sizes
            QSize currentSize;
            currentSize = screen->rect().size();

            menu->addTitle(SmallIcon("view-fullscreen"), i18n("Screen Size"));
            actionGroup = populateSizes(menu, sizes, currentSize);	
            connect(actionGroup, SIGNAL(triggered(QAction*)), screen, SLOT(slotResizeUnified(QAction*)));

            // if the outputs are unified, we can rotate the screen on all outputs
            int rotations = screen->unifiedRotations();
            if (rotations != RandR::Rotate0) {
                menu->addTitle(SmallIcon("view-refresh"), i18n("Orientation"));
                int rotation = RandR::Rotate0;
                foreach(RandROutput *output, screen->outputs()) {
                    if (output->isActive()) {
                        rotation = output->rotation();
                        break;
                    }
                }

                actionGroup = populateRotations(menu, rotations, rotation);
                connect(actionGroup, SIGNAL(triggered(QAction*)), screen, SLOT(slotRotateUnified(QAction*)));
            }
        }
    } else {
        if (screen->connectedCount() != 1) {
            menu->addTitle(SmallIcon("view-fullscreen"), i18n("Outputs"));
        }

        RandROutput *primary = screen->primaryOutput();

        foreach(RandROutput *output, outputs) {
            if (output->isConnected()) {
                KMenu *outputMenu;
                if (screen->connectedCount() == 1) {
                    outputMenu = menu;
                } else {
                    outputMenu = new KMenu(output->name());
                }
                outputMenu->setIcon(SmallIcon(output->icon()));
                outputMenu->addTitle(SmallIcon("view-fullscreen"), i18n("%1 - Screen Size", output->name()));

                QSize currentSize = output->rect().size();

                // if the output is rotated 90 or 270, the returned rect is inverted
                // so we need to invert the size before comparing
                if (output->rotation() & (RandR::Rotate90 | RandR::Rotate270)) {
                    currentSize = QSize(currentSize.height(), currentSize.width());
                }

                actionGroup = populateSizes(outputMenu, output->sizes(), currentSize);
                connect(actionGroup, SIGNAL(triggered(QAction*)), output, SLOT(slotChangeSize(QAction*)));
                
                // if there is only one output active, do not show the disable option
                // this prevents the user from doing wrong things ;)
                kDebug() << "Active outputs: " << screen->activeCount();
                if (screen->activeCount() != 1) {
                    action = outputMenu->addAction(i18n("Disable"));
                    if (!output->crtc()) {
                        QFont font = action->font();
                        font.setBold(true);
                        action->setFont(font);
                    }
                    connect(action, SIGNAL(triggered(bool)), output, SLOT(slotDisable()));
                }

                // Display the rotations
                int rotations = output->rotations();
                // Don't display the rotation options if there is no point (ie. none are supported)
                // XFree86 4.3 does not include rotation support.
                if (rotations != RandR::Rotate0)  {
                    outputMenu->addTitle(SmallIcon("view-refresh"), i18n("Orientation"));
                    actionGroup = populateRotations(outputMenu, rotations, output->rotation());
                    connect(actionGroup, SIGNAL(triggered(QAction*)), output, SLOT(slotChangeRotation(QAction*)));
                }

                // refresh rate
                RateList rates = output->refreshRates();
                if (rates.count()) {
                    outputMenu->addTitle(SmallIcon("chronometer"), i18n("Refresh Rate"));
                    actionGroup = populateRates(outputMenu, rates, output->refreshRate());
                    connect(actionGroup, SIGNAL(triggered(QAction*)), output, SLOT(slotChangeRefreshRate(QAction*)));
                }

                if (screen->connectedCount() != 1) {
                    outputMenu->addSeparator();
                    action = outputMenu->addAction(
                        i18nc("(checkbox) designate this output as the primary output", "Primary output"),
                        output,
                        SLOT(slotSetAsPrimary(bool))
                    );
                    action->setCheckable(true);
                    action->setChecked(primary == output);
                }

                
                if (screen->connectedCount() != 1) {
                    menu->addMenu(outputMenu);
                }
            }
        }
    }
    // if there is more than one output connected, give the option to unify the outputs
    if (screen->connectedCount() != 1 && !screen->unifiedSizes().isEmpty()) {
        menu->addSeparator();
        action = menu->addAction( i18n("Unify Outputs"), screen, SLOT(slotUnifyOutputs(bool)) );
        action->setCheckable(true);
        action->setChecked(screen->outputsUnified());
    }
}

QActionGroup *KRandRSystemTray::populateRotations(KMenu *menu, int rotations, int rotation)
{
    QAction *action;
    QActionGroup *rotateGroup = new QActionGroup(menu);
    rotateGroup->setExclusive(true);

    for (int i = 0; i < 6; i++) {
        if ((1 << i) & rotations) {
            action = menu->addAction(QIcon(RandR::rotationIcon(1 << i, rotation)), RandR::rotationName(1 << i));
            action->setCheckable(true);

            action->setData(1 << i);
            if (rotation & (1 << i)) {
                action->setChecked(true);
            }
            rotateGroup->addAction(action);
        }
    }
    return rotateGroup;
}

QActionGroup *KRandRSystemTray::populateSizes(KMenu *menu, const SizeList &sizes, const QSize &size)
{
    QAction *action;
    QActionGroup *sizeGroup = new QActionGroup(menu);
    sizeGroup->setExclusive(true);
    for (int i = 0; i < sizes.count(); ++i) {
        QSize s = sizes[i];
        action = menu->addAction(QString("%1 x %2").arg(s.width()).arg(s.height()));
        action->setCheckable(true);
        action->setData(s);
        if (s == size) {
            action->setChecked(true);
        }
        sizeGroup->addAction(action);
    }
    return sizeGroup;
}

QActionGroup *KRandRSystemTray::populateRates(KMenu *menu, const RateList &rates, float rate)
{
    QAction *action;
    QActionGroup *rateGroup = new QActionGroup(menu);
    rateGroup->setExclusive(true);

    foreach(float r, rates) {
        action = menu->addAction(ki18n("%1 Hz").subs(r, 0, 'f', 1).toString());
        action->setCheckable(true);
        action->setData(r);
        if (r == rate) {
            action->setChecked(true);
        }
        rateGroup->addAction(action);
    }
    return rateGroup;
}

void KRandRSystemTray::slotPrefs()
{
    if (!m_kcm) {
        KCMultiDialog *kcm = new KCMultiDialog( associatedWidget() );
        kcm->setFaceType( KCMultiDialog::Plain );
        kcm->setWindowTitle( i18n( "Configure Display" ) );
        kcm->addModule( "display" );
        kcm->setAttribute(Qt::WA_DeleteOnClose);
        m_kcm = kcm;
    } else if (KWindowSystem::activeWindow() == m_kcm.data()->winId()) {
        m_kcm.data()->hide();
        return;
    }

    KWindowSystem::setOnDesktop(m_kcm.data()->winId(), KWindowSystem::currentDesktop());
    m_kcm.data()->show();
    m_kcm.data()->raise();
    KWindowSystem::forceActiveWindow(m_kcm.data()->winId());
}
// vim:noet:sts=8:sw=8:
