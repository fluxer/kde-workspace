/*******************************************************************
* drkonqidialog.cpp
* Copyright 2009    Dario Andres Rodriguez <andresbajotierra@gmail.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************/

#include "drkonqidialog.h"

#include <KMenu>
#include <KIcon>
#include <KStandardDirs>
#include <KLocale>
#include <KTabWidget>
#include <KDebug>
#include <KCmdLineArgs>
#include <KToolInvocation>
#include <kglobalsettings.h>

#include "drkonqi.h"
#include "backtracegenerator.h"
#include "backtracewidget.h"
#include "parser/backtraceparser.h"
#include "crashedapplication.h"
#include "debuggermanager.h"
#include "debuggerlaunchers.h"
#include "drkonqi_globals.h"
#include "systeminformation.h"

DrKonqiDialog::DrKonqiDialog(QWidget * parent) :
        KDialog(parent),
        m_backtraceWidget(0)
{
    KGlobal::ref();
    setAttribute(Qt::WA_DeleteOnClose, true);

    //Setting dialog title and icon
    setCaption(DrKonqi::crashedApplication()->name());
    setWindowIcon(KIcon("tools-report-bug"));

    m_tabWidget = new KTabWidget(this);
    setMainWidget(m_tabWidget);

    buildIntroWidget();
    m_tabWidget->addTab(m_introWidget, i18nc("@title:tab general information", "&General"));

    m_backtraceWidget = new BacktraceWidget(DrKonqi::debuggerManager()->backtraceGenerator(), this);
    m_backtraceWidget->setMinimumSize(QSize(575, 240));
    m_tabWidget->addTab(m_backtraceWidget, i18nc("@title:tab", "&Developer Information"));

    buildDialogButtons();

    setMinimumSize(QSize(640,320));
    resize(minimumSize());
    KConfigGroup config(KGlobal::config(), "General");
    restoreDialogSize(config);

    m_backtraceWidget->generateBacktrace();
}

DrKonqiDialog::~DrKonqiDialog()
{
    KConfigGroup config(KGlobal::config(), "General");
    saveDialogSize(config);

    KGlobal::deref();
}

void DrKonqiDialog::buildIntroWidget()
{
    const CrashedApplication *crashedApp = DrKonqi::crashedApplication();

    m_introWidget = new QWidget(this);
    ui.setupUi(m_introWidget);

    ui.titleLabel->setText(i18nc("@info", "<para>We are sorry, <application>%1</application> "
                                               "closed unexpectedly.</para>", crashedApp->name()));

    QString reportMessage;
    if (!crashedApp->bugReportAddress().isEmpty()) {
        if (crashedApp->fakeExecutableBaseName() == QLatin1String("drkonqi")) { //Handle own crashes
            reportMessage = i18nc("@info", "<para>As the Crash Handler itself has failed, the "
                                           "automatic reporting process is disabled to reduce the "
                                           "risks of failing again.<nl /><nl />"
                                           "Please, <link url='%1'>manually report</link> this error "
                                           "to the KDE bug tracking system. Do not forget to include "
                                           "the backtrace from the <interface>Developer Information</interface> "
                                           "tab.</para>",
                                           QLatin1String(BUG_REPORT_URL));
        } else if (KCmdLineArgs::parsedArgs()->isSet("safer")) {
            reportMessage = i18nc("@info", "<para>The reporting assistant is disabled because "
                                           "the crash handler dialog was started in safe mode."
                                           "<nl />You can manually report this bug to %1 "
                                           "(including the backtrace from the "
                                           "<interface>Developer Information</interface> "
                                           "tab.)</para>", crashedApp->bugReportAddress());
        } else {
            reportMessage = i18nc("@info", "<para>You can help us improve KDE Software by reporting "
                                          "this error.<nl /><link url='%1'>Learn "
                                          "more about bug reporting.</link></para><para><note>It is "
                                          "safe to close this dialog if you do not want to report "
                                          "this bug.</note></para>",
                                          QLatin1String(ABOUT_BUG_REPORTING_URL)
                                          );
        }
    } else {
        reportMessage = i18nc("@info", "<para>You cannot report this error, because "
                                        "<application>%1</application> does not provide a bug reporting "
                                        "address.</para>",
                                        crashedApp->name()
                                        );
    }
    ui.infoLabel->setText(reportMessage);

    ui.iconLabel->setPixmap(
                        QPixmap(KStandardDirs::locate("appdata", QLatin1String("pics/crash.png"))));

    ui.detailsTitleLabel->setText(QString("<strong>%1</strong>").arg(i18nc("@label","Details:")));

    ui.detailsLabel->setText(i18nc("@info Note the time information is divided into date and time parts",
                                            "<para>Executable: <application>%1"
                                            "</application> PID: <numid>%2</numid> Signal: %3 (%4) "
                                            "Time: %5 %6</para>",
                                             crashedApp->fakeExecutableBaseName(),
                                             crashedApp->pid(),
                                             crashedApp->signalName(),
                                             crashedApp->signalNumber(),
                                             KGlobal::locale()->formatDate(crashedApp->datetime().date(), KLocale::ShortDate),

                                             KGlobal::locale()->formatTime(crashedApp->datetime().time(), true)
                                             ));
}

void DrKonqiDialog::buildDialogButtons()
{
    const CrashedApplication *crashedApp = DrKonqi::crashedApplication();

    //Set kdialog buttons
    setButtons(KDialog::User3 | KDialog::User2 | KDialog::User1 | KDialog::Close);

    //Report bug button
    setButtonGuiItem(KDialog::User1, KGuiItem2(i18nc("@action:button", "Report &Bug"),
                                               KIcon("tools-report-bug"),
                                               i18nc("@info:tooltip",
                                                     "Starts the bug report assistant.")));

    bool enableReportAssistant = !crashedApp->bugReportAddress().isEmpty() &&
                                 crashedApp->fakeExecutableBaseName() != QLatin1String("drkonqi") &&
                                 !KCmdLineArgs::parsedArgs()->isSet("safer");
    enableButton(KDialog::User1, enableReportAssistant);
    connect(this, SIGNAL(user1Clicked()), this, SLOT(startBugReportAssistant()));

    //Default debugger button and menu (only for developer mode)
    DebuggerManager *debuggerManager = DrKonqi::debuggerManager();
    setButtonGuiItem(KDialog::User2, KGuiItem2(i18nc("@action:button this is the debug menu button "
                                               "label which contains the debugging applications",
                                               "&Debug"), KIcon("applications-development"),
                                               i18nc("@info:tooltip", "Starts a program to debug "
                                                     "the crashed application.")));
    showButton(KDialog::User2, debuggerManager->showExternalDebuggers());

    m_debugMenu = new KMenu(this);
    setButtonMenu(KDialog::User2, m_debugMenu);

    QList<AbstractDebuggerLauncher*> debuggers = debuggerManager->availableExternalDebuggers();
    foreach(AbstractDebuggerLauncher *launcher, debuggers) {
        addDebugger(launcher);
    }

    connect(debuggerManager, SIGNAL(externalDebuggerAdded(AbstractDebuggerLauncher*)),
            SLOT(addDebugger(AbstractDebuggerLauncher*)));
    connect(debuggerManager, SIGNAL(externalDebuggerRemoved(AbstractDebuggerLauncher*)),
            SLOT(removeDebugger(AbstractDebuggerLauncher*)));
    connect(debuggerManager, SIGNAL(debuggerRunning(bool)), SLOT(enableDebugMenu(bool)));

    //Restart application button
    setButtonGuiItem(KDialog::User3, KGuiItem2(i18nc("@action:button", "&Restart Application"),
                                               KIcon("system-reboot"),
                                               i18nc("@info:tooltip", "Use this button to restart "
                                                     "the crashed application.")));
    enableButton(KDialog::User3, !crashedApp->hasBeenRestarted() &&
                                 crashedApp->fakeExecutableBaseName() != QLatin1String("drkonqi"));
    connect(this, SIGNAL(user3Clicked()), crashedApp, SLOT(restart()));
    connect(crashedApp, SIGNAL(restarted(bool)), this, SLOT(applicationRestarted(bool)));

    //Close button
    QString tooltipText = i18nc("@info:tooltip",
                                "Close this dialog (you will lose the crash information.)");
    setButtonToolTip(KDialog::Close, tooltipText);
    setButtonWhatsThis(KDialog::Close, tooltipText);
    setDefaultButton(KDialog::Close);
    setButtonFocus(KDialog::Close);
}

void DrKonqiDialog::addDebugger(AbstractDebuggerLauncher *launcher)
{
    QAction *action = new QAction(KIcon("applications-development"),
                                  i18nc("@action:inmenu 1 is the debugger name",
                                        "Debug in <application>%1</application>",
                                        launcher->name()), m_debugMenu);
    m_debugMenu->addAction(action);
    connect(action, SIGNAL(triggered()), launcher, SLOT(start()));
    m_debugMenuActions.insert(launcher, action);
}

void DrKonqiDialog::removeDebugger(AbstractDebuggerLauncher *launcher)
{
    QAction *action = m_debugMenuActions.take(launcher);
    if ( action ) {
        m_debugMenu->removeAction(action);
        action->deleteLater();
    } else {
        kError() << "Invalid launcher";
    }
}

void DrKonqiDialog::enableDebugMenu(bool debuggerRunning)
{
    enableButton(KDialog::User2, !debuggerRunning);
}

void DrKonqiDialog::startBugReportAssistant()
{
    const CrashedApplication *crashedApp = DrKonqi::crashedApplication();
    QString appReportAddress = crashedApp->bugReportAddress();
    SystemInformation *sysinfo = new SystemInformation(this);
    QString backtrace = DrKonqi::debuggerManager()->backtraceGenerator()->parser()->parsedBacktrace();
    QString query;
    // KDE applications use the email address by default
    if (appReportAddress == BUG_REPORT_EMAIL) {
        // black magic - report is done in Markdown syntax with new lines preserved
        // but invokeBrowser() can call external browser and at this point KDE
        // application can not be trused not to crash again (konqueror, rekonq, etc.).
        query = QString("%1/new").arg(BUG_REPORT_URL);
        query.append(QString("?title=%1 %2 %3").arg(crashedApp->name(), crashedApp->version(),
            crashedApp->signalName()));
        query.append(QString("&body=%1\nOS: %2\nRelease: %3\nKDE: %4\nKatie: %5\n%6").arg(
            QUrl::toPercentEncoding("## Platform"), sysinfo->system(),
            sysinfo->release(), sysinfo->kdeVersion(), sysinfo->qtVersion(),
            QUrl::toPercentEncoding("## Backtrace\nPlease, copy-paste it from the DrKonqi window\n")));
    } else {
        query = QString(appReportAddress);
    }

    KToolInvocation::invokeBrowser(query);
}

void DrKonqiDialog::applicationRestarted(bool success)
{
    enableButton(KDialog::User3, !success);
}
