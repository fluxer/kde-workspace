/*
 *  Copyright (C) 2009 Marco Martin <notmart@gmail.com>
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
 *
 */
#include "workspaceoptions.h"

#include "ui_mainpage.h"

#include <QDBusInterface>

#include <KDebug>
#include <KAboutData>
#include <KMessageBox>
#include <KPluginFactory>
#include <KRun>
#include <KStandardDirs>
#include <KUrl>
#include <KConfigGroup>

K_PLUGIN_FACTORY(WorkspaceOptionsModuleFactory, registerPlugin<WorkspaceOptionsModule>();)
K_EXPORT_PLUGIN(WorkspaceOptionsModuleFactory("kcmworkspaceoptions"))


WorkspaceOptionsModule::WorkspaceOptionsModule(QWidget *parent, const QVariantList &)
  : KCModule(WorkspaceOptionsModuleFactory::componentData(), parent),
    m_kwinConfig( KSharedConfig::openConfig("kwinrc")),
    m_ownConfig( KSharedConfig::openConfig("workspaceoptionsrc")),
    m_plasmaDesktopAutostart("plasma-desktop"),
    m_krunnerAutostart("krunner"),
    m_currentlyIsDesktop(false),
    m_plasmaFound(false),
    m_ui(new Ui_MainPage())
{
    KAboutData *about =
    new KAboutData("kcmworkspaceoptions", 0, ki18n("Global options for the Plasma Workspace"),
                   0, KLocalizedString(), KAboutData::License_GPL,
                   ki18n("(c) 2009 Marco Martin"));

    about->addAuthor(ki18n("Marco Martin"), ki18n("Maintainer"), "notmart@gmail.com");

    setAboutData(about);

    setButtons(Help|Apply);

    m_plasmaFound = !KStandardDirs::findExe("plasma-desktop").isNull();

    m_ui->setupUi(this);
    // NOTE: the i18n() bellow is using translation from:
    // kdelibs/kdecore/localization/klocale_kde.cpp
    m_ui->themeCacheSize->setSuffix(i18n("%1 MB", QLatin1String("")));

    connect(m_ui->formFactor, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    connect(m_ui->showToolTips, SIGNAL(toggled(bool)), this, SLOT(changed()));
    connect(m_ui->cacheTheme, SIGNAL(toggled(bool)), this, SLOT(changed()));
    connect(m_ui->cacheTheme, SIGNAL(toggled(bool)), this, SLOT(cacheThemeChanged(bool)));
    connect(m_ui->themeCacheSize, SIGNAL(valueChanged(int)), this, SLOT(changed()));

    if (!m_plasmaFound) {
        m_ui->formFactor->setEnabled(false);
        m_ui->cacheTheme->setEnabled(false);
        m_ui->themeCacheSize->setEnabled(false);
    }
}

WorkspaceOptionsModule::~WorkspaceOptionsModule()
{
    delete m_ui;
}

void WorkspaceOptionsModule::save()
{
    {
        KConfig config("plasmarc");
        KConfigGroup cg(&config, "PlasmaToolTips");
        cg.writeEntry("Delay", m_ui->showToolTips->isChecked() ? 0.7 : -1);
        KConfigGroup cg2(&config, "CachePolicies");
        cg2.writeEntry("CacheTheme", m_ui->cacheTheme->isChecked());
        cg2.writeEntry("ThemeCacheKb", m_ui->themeCacheSize->value() * 1024);
    }

    const bool isDesktop = m_ui->formFactor->currentIndex() == 0;

    m_plasmaDesktopAutostart.setAutostarts(true);
    m_plasmaDesktopAutostart.setStartPhase(KAutostart::BaseDesktop);
    m_plasmaDesktopAutostart.setCommand("plasma-desktop");
    m_plasmaDesktopAutostart.setAllowedEnvironments(QStringList()<<"KDE");

    m_krunnerAutostart.setAutostarts(true);
    m_krunnerAutostart.setStartPhase(KAutostart::BaseDesktop);
    m_krunnerAutostart.setCommand("krunner");
    m_krunnerAutostart.setAllowedEnvironments(QStringList()<<"KDE");

    KConfigGroup winCg(m_kwinConfig, "Windows");

    winCg.writeEntry("BorderlessMaximizedWindows", !isDesktop);
    if (!isDesktop) {
        winCg.writeEntry("Placement", "Maximizing");
    } else {
        winCg.writeEntry("Placement", "Smart");
    }
    winCg.sync();

    KConfigGroup ownButtonsCg(m_ownConfig, "TitleBarButtons");
    KConfigGroup ownPresentWindowsCg(m_ownConfig, "Effect-PresentWindows");
    KConfigGroup ownCompositingCg(m_ownConfig, "Compositing");
    KConfigGroup kwinStyleCg(m_kwinConfig, "Style");
    KConfigGroup kwinPresentWindowsCg(m_kwinConfig, "Effect-PresentWindows");
    KConfigGroup kwinCompositingCg(m_kwinConfig, "Compositing");


    QString desktopTitleBarButtonsLeft = ownButtonsCg.readEntry("DesktopLeft", "MS");
    QString desktopTitleBarButtonsRight = ownButtonsCg.readEntry("DesktopRight", "HIAX");

    QString netbookTitleBarButtonsLeft = ownButtonsCg.readEntry("NetbookLeft", "MS");
    QString netbookTitleBarButtonsRight = ownButtonsCg.readEntry("NetbookRight", "HAX");


    int desktopPresentWindowsLayoutMode = 0;
    int netbookPresentWindowsLayoutMode = 1;

    bool desktopUnredirectFullscreen = ownCompositingCg.readEntry("DesktopUnredirectFullscreen", true);
    bool netbookUnredirectFullscreen = ownCompositingCg.readEntry("NetbookUnredirectFullscreen", false);

    if (m_currentlyIsDesktop) {
        //save the user preferences on titlebar buttons
        desktopTitleBarButtonsLeft = kwinStyleCg.readEntry("ButtonsOnLeft", "MS");
        desktopTitleBarButtonsRight = kwinStyleCg.readEntry("ButtonsOnRight", "HIAX");
        ownButtonsCg.writeEntry("DesktopLeft", desktopTitleBarButtonsLeft);
        ownButtonsCg.writeEntry("DesktopRight", desktopTitleBarButtonsRight);

        //Unredirect fullscreen
        desktopUnredirectFullscreen = kwinCompositingCg.readEntry("UnredirectFullscreen", true);
        ownCompositingCg.writeEntry("DesktopUnredirectFullscreen", desktopUnredirectFullscreen);

        //desktop grid effect
        desktopPresentWindowsLayoutMode = kwinPresentWindowsCg.readEntry("LayoutMode", 0);
        ownPresentWindowsCg.writeEntry("DesktopLayoutMode", desktopPresentWindowsLayoutMode);
    } else {
        //save the user preferences on titlebar buttons
        netbookTitleBarButtonsLeft = kwinStyleCg.readEntry("ButtonsOnLeft", "MS");
        netbookTitleBarButtonsRight = kwinStyleCg.readEntry("ButtonsOnRight", "HAX");
        ownButtonsCg.writeEntry("NetbookLeft", netbookTitleBarButtonsLeft);
        ownButtonsCg.writeEntry("NetbookRight", netbookTitleBarButtonsRight);

        //Unredirect fullscreen
        netbookUnredirectFullscreen = kwinCompositingCg.readEntry("UnredirectFullscreen", true);
        ownCompositingCg.writeEntry("NetbookUnredirectFullscreen", netbookUnredirectFullscreen);

        //desktop grid effect
        desktopPresentWindowsLayoutMode = kwinPresentWindowsCg.readEntry("LayoutMode", 0);
        ownPresentWindowsCg.writeEntry("NetbookLayoutMode", desktopPresentWindowsLayoutMode);
    }

    ownButtonsCg.sync();
    ownPresentWindowsCg.sync();
    ownCompositingCg.sync();

    kwinStyleCg.writeEntry("CustomButtonPositions", true);
    if (isDesktop) {
        //kill/enable the minimize button, unless configured differently
        kwinStyleCg.writeEntry("ButtonsOnLeft", desktopTitleBarButtonsLeft);
        kwinStyleCg.writeEntry("ButtonsOnRight", desktopTitleBarButtonsRight);

        // enable unredirect fullscreen, unless configured differently
        kwinCompositingCg.writeEntry("UnredirectFullscreen", desktopUnredirectFullscreen);

        //present windows mode
        kwinPresentWindowsCg.writeEntry("LayoutMode", desktopPresentWindowsLayoutMode);
    } else {
        //kill/enable the minimize button, unless configured differently
        kwinStyleCg.writeEntry("ButtonsOnLeft", netbookTitleBarButtonsLeft);
        kwinStyleCg.writeEntry("ButtonsOnRight", netbookTitleBarButtonsRight);

        // disable unredirect fullscreen, unless configured differently
        kwinCompositingCg.writeEntry("UnredirectFullscreen", netbookUnredirectFullscreen);

        //present windows mode
        kwinPresentWindowsCg.writeEntry("LayoutMode", netbookPresentWindowsLayoutMode);
    }

    kwinStyleCg.sync();
    kwinPresentWindowsCg.sync();
    kwinCompositingCg.sync();

    // Reload KWin.
    QDBusMessage message = QDBusMessage::createSignal( "/KWin", "org.kde.KWin", "reloadConfig" );
    QDBusConnection::sessionBus().send(message);

    m_currentlyIsDesktop = isDesktop;
}

void WorkspaceOptionsModule::load()
{
    if (m_plasmaDesktopAutostart.autostarts()) {
        m_ui->formFactor->setCurrentIndex(0);
    } else {
        m_ui->formFactor->setCurrentIndex(1);
    }

    KConfig config("plasmarc");
    KConfigGroup cg(&config, "PlasmaToolTips");
    m_ui->showToolTips->setChecked(cg.readEntry("Delay", 0.7) > 0);
    KConfigGroup cg2(&config, "CachePolicies");
    m_ui->cacheTheme->setChecked(cg2.readEntry("CacheTheme", true));
    const int themeCacheKb = cg2.readEntry("ThemeCacheKb", 81920);
    m_ui->themeCacheSize->setValue(themeCacheKb / 1024);
}

void WorkspaceOptionsModule::defaults()
{
    m_ui->formFactor->setCurrentIndex(0);
}

void WorkspaceOptionsModule::cacheThemeChanged(bool cacheTheme)
{
    Q_ASSERT(m_plasmaFound);
    m_ui->themeCacheSize->setEnabled(cacheTheme);
}

#include "moc_workspaceoptions.cpp"
