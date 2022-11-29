/*  This file is part of the KDE libraries
    Copyright (C) 2016 Ivailo Monev <xakepa10@gmail.com>

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

#include <KIcon>
#include <KLocale>
#include <KDebug>
#include <KAction>
#include <KStandardShortcut>
#include <KStandardAction>
#include <KActionCollection>
#include <KCMultiDialog>
#include <KInputDialog>
#include <KFileDialog>
#include <KToolBar>
#include <KStatusBar>
#include <KConfigGroup>
#include <QMessageBox>
#include <QApplication>
#include <QMenuBar>

#include "kmediawindow.h"

KMediaWindow::KMediaWindow(QWidget *parent, Qt::WindowFlags flags)
    : KXmlGuiWindow(parent, flags)
{
    m_config = new KConfig("kmediaplayerrc", KConfig::SimpleConfig);

    m_player = new KMediaWidget(this, KMediaWidget::AllOptions);
    m_player->player()->setPlayerID("kmediaplayer");

    setCentralWidget(m_player);

    KAction *action = actionCollection()->addAction("file_open_path", this, SLOT(slotOpenPath()));
    action->setText(i18n("Open"));
    action->setIcon(KIcon("document-open"));
    action->setShortcut(KStandardShortcut::open());
    action->setWhatsThis(i18n("Open a path."));

    action = actionCollection()->addAction("file_open_url", this, SLOT(slotOpenURL()));
    action->setText(i18n("Open URL"));
    action->setIcon(KIcon("document-open-remote"));
    action->setWhatsThis(i18n("Open a URL."));

    action = actionCollection()->addAction("file_close", this, SLOT(slotClosePath()));
    action->setText(i18n("Close"));
    action->setIcon(KIcon("document-close"));
    action->setShortcut(KStandardShortcut::close());
    action->setWhatsThis(i18n("Close the current path/URL."));

    action = actionCollection()->addAction("file_quit", this, SLOT(slotQuit()));
    action->setText(i18n("Quit"));
    action->setIcon(KIcon("application-exit"));
    action->setShortcut(KStandardShortcut::quit());
    action->setWhatsThis(i18n("Close the application."));

    action = actionCollection()->addAction("player_fullscreen", this, SLOT(slotFullscreen()));
    action->setText(i18n("Fullscreen"));
    action->setIcon(KIcon("view-fullscreen"));
    action->setShortcut(KStandardShortcut::fullScreen());
    action->setWhatsThis(i18n("Set the player view to fullscreen/non-fullscreen"));

    action = actionCollection()->addAction("settings_player", this, SLOT(slotConfigure()));
    action->setText(i18n("Configure KMediaPlayer"));
    action->setIcon(KIcon("preferences-desktop-sound"));
    action->setWhatsThis(i18n("Configure KMediaPlayer and applications that use it."));

    m_recentfiles = new KRecentFilesAction(KIcon("document-open-recent"), "Open recent", this);
    m_recentfiles->setShortcut(KStandardShortcut::shortcut(KStandardShortcut::OpenRecent));
    m_recentfiles->setWhatsThis(i18n("Open recently opened files."));
    connect(m_recentfiles, SIGNAL(urlSelected(KUrl)), this, SLOT(slotOpenURL(KUrl)));
    actionCollection()->addAction("file_open_recent", m_recentfiles);

    KConfigGroup firstrungroup(m_config, "KMediaPlayer");
    const bool firstrun = firstrungroup.readEntry("firstrun", true);
    if (firstrun) {
        // no toolbar unless explicitly enabled
        toolBar()->setVisible(false);
        // also set a decent window size
        resize(640, 480);
    }

    connect(m_player, SIGNAL(controlsHidden(bool)), this, SLOT(slotHideBars(bool)));
    m_menu = new QMenu();
    m_menu->addAction(KIcon("show-menu"), i18n("Show/hide menubar"), this, SLOT(slotMenubar()));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotMenu(QPoint)));

    setupGUI(KXmlGuiWindow::Keys | KXmlGuiWindow::StatusBar | KXmlGuiWindow::Save | KXmlGuiWindow::Create);
    setAutoSaveSettings();

    KConfigGroup recentfilesgroup(m_config, "RecentFiles");
    m_recentfiles->loadEntries(recentfilesgroup);

    setMouseTracking(true);
    qApp->installEventFilter(this);
}

KMediaWindow::~KMediaWindow()
{
    slotHideBars(true);
    disconnect(m_player, SIGNAL(controlsHidden(bool)), this, SLOT(slotHideBars(bool)));
    saveAutoSaveSettings();

    KConfigGroup recentfilesgroup(m_config, "RecentFiles");
    m_recentfiles->saveEntries(recentfilesgroup);
    KConfigGroup firstrungroup(m_config, "KMediaPlayer");
    firstrungroup.writeEntry("firstrun", false);

    m_player->deleteLater();
    m_recentfiles->deleteLater();
    m_menu->deleteLater();
    delete m_config;
}

void KMediaWindow::showEvent(QShowEvent *event)
{
    KXmlGuiWindow::showEvent(event);
    m_menuvisible = menuBar()->isVisible();
    m_statusvisible = statusBar()->isVisible();
}

bool KMediaWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::KeyPress) {
        m_player->resetControlsTimer();
    }
    return KXmlGuiWindow::eventFilter(object, event);
}

void KMediaWindow::slotHideBars(bool visible)
{
    if (!visible) {
        m_menuvisible = menuBar()->isVisible();
        m_statusvisible = statusBar()->isVisible();
        menuBar()->setVisible(false);
        statusBar()->setVisible(false);
    } else {
        menuBar()->setVisible(m_menuvisible);
        statusBar()->setVisible(m_statusvisible);
    }
}

void KMediaWindow::slotOpenPath()
{
    const QString path = KFileDialog::getOpenFileName(KUrl(), QString(), this, i18n("Select paths"));
    if (!path.isEmpty()) {
        if (!m_player->player()->isPathSupported(path)) {
            QMessageBox::warning(this, i18n("Invalid path"),
                i18n("The path is invalid:\n%1", path));
        } else {
            m_player->open(path);
            m_recentfiles->addUrl(KUrl(path));
        }
    }
}

void KMediaWindow::slotOpenURL()
{
    bool dummy;
    QString protocols = m_player->player()->protocols().join(", ");
    KUrl url = KInputDialog::getText(i18n("Input URL"),
        i18n("Supported protocols are:\n\n%1", protocols),
        QString(), &dummy, this);
    if (!url.isEmpty()) {
        QString urlstring = url.prettyUrl();
        if (!m_player->player()->isPathSupported(urlstring)) {
            QMessageBox::warning(this, i18n("Invalid URL"),
                i18n("Invalid URL:\n%1", urlstring));
        } else {
            m_player->open(urlstring);
            m_recentfiles->addUrl(url);
        }
    }
}

void KMediaWindow::slotOpenURL(KUrl url)
{
    m_player->open(url.prettyUrl());
    m_recentfiles->addUrl(url);
}

void KMediaWindow::slotClosePath()
{
    m_player->player()->stop();
    statusBar()->showMessage("");
}

void KMediaWindow::slotFullscreen()
{
    m_player->setFullscreen();
}

void KMediaWindow::slotConfigure()
{
    KCMultiDialog kcmdialg(this);
    kcmdialg.addModule("kcmplayer");
    kcmdialg.exec();
}

void KMediaWindow::slotMenubar()
{
    menuBar()->setVisible(!menuBar()->isVisible());
    m_menuvisible = menuBar()->isVisible();
}

void KMediaWindow::slotMenu(QPoint position)
{
    // it is bogus, just ignore it
    Q_UNUSED(position);
    m_menu->exec(QCursor::pos());
}

void KMediaWindow::slotQuit()
{
    KMediaWindow::close();
    qApp->quit();
}
