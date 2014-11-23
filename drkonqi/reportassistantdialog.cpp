/*******************************************************************
* reportassistantdialog.cpp
* Copyright 2009,2010    Dario Andres Rodriguez <andresbajotierra@gmail.com>
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

#include "reportassistantdialog.h"

#include <QCloseEvent>

#include <KMessageBox>

#include "drkonqi.h"

#include "parser/backtraceparser.h"
#include "debuggermanager.h"
#include "backtracegenerator.h"

#include "crashedapplication.h"
#include "reportassistantpages_base.h"
#include "reportinterface.h"

ReportAssistantDialog::ReportAssistantDialog(QWidget * parent) :
        KAssistantDialog(parent),
        m_reportInterface(new ReportInterface(this)),
        m_canClose(false)
{
    KGlobal::ref();
    setAttribute(Qt::WA_DeleteOnClose, true);

    //Set window properties
    setWindowTitle(i18nc("@title:window","Crash Reporting Assistant"));
    setWindowIcon(KIcon("tools-report-bug"));

    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),
            this, SLOT(currentPageChanged_slot(KPageWidgetItem*,KPageWidgetItem*)));
    connect(this, SIGNAL(helpClicked()), this, SLOT(showHelp()));

    //Create the assistant pages

    //-Introduction Page
    KConfigGroup group(KGlobal::config(), "ReportAssistant");
    const bool skipIntroduction = group.readEntry("SkipIntroduction", false);

    if (!skipIntroduction) {
        IntroductionPage * m_introduction = new IntroductionPage(this);

        KPageWidgetItem * m_introductionPage = new KPageWidgetItem(m_introduction,
                QLatin1String(PAGE_INTRODUCTION_ID));
        m_pageWidgetMap.insert(QLatin1String(PAGE_INTRODUCTION_ID),m_introductionPage);
        m_introductionPage->setHeader(i18nc("@title","Welcome to the Reporting Assistant"));
        m_introductionPage->setIcon(KIcon("tools-report-bug"));

        addPage(m_introductionPage);
    }

    //-Bug Awareness Page
    BugAwarenessPage * m_awareness = new BugAwarenessPage(this);
    connectSignals(m_awareness);

    KPageWidgetItem * m_awarenessPage = new KPageWidgetItem(m_awareness,
                                                                QLatin1String(PAGE_AWARENESS_ID));
    m_pageWidgetMap.insert(QLatin1String(PAGE_AWARENESS_ID),m_awarenessPage);
    m_awarenessPage->setHeader(i18nc("@title","What do you know about the crash?"));
    m_awarenessPage->setIcon(KIcon("checkbox"));

    //-Crash Information Page
    CrashInformationPage * m_backtrace = new CrashInformationPage(this);
    connectSignals(m_backtrace);

    KPageWidgetItem * m_backtracePage = new KPageWidgetItem(m_backtrace,
                                                        QLatin1String(PAGE_CRASHINFORMATION_ID));
    m_pageWidgetMap.insert(QLatin1String(PAGE_CRASHINFORMATION_ID),m_backtracePage);
    m_backtracePage->setHeader(i18nc("@title","Fetching the Backtrace (Automatic Crash Information)"));
    m_backtracePage->setIcon(KIcon("run-build"));

    //-Results Page
    ConclusionPage * m_conclusions = new ConclusionPage(this);
    connectSignals(m_conclusions);

    KPageWidgetItem * m_conclusionsPage = new KPageWidgetItem(m_conclusions,
                                                                QLatin1String(PAGE_CONCLUSIONS_ID));
    m_pageWidgetMap.insert(QLatin1String(PAGE_CONCLUSIONS_ID),m_conclusionsPage);
    m_conclusionsPage->setHeader(i18nc("@title","Results of the Analyzed Crash Details"));
    m_conclusionsPage->setIcon(KIcon("dialog-information"));
    connect(m_conclusions, SIGNAL(finished(bool)), this, SLOT(assistantFinished(bool)));

    //TODO Remember to keep the pages ordered
    addPage(m_awarenessPage);
    addPage(m_backtracePage);
    addPage(m_conclusionsPage);

    setMinimumSize(QSize(600, 400));
    resize(minimumSize());
}

ReportAssistantDialog::~ReportAssistantDialog()
{
    KGlobal::deref();
}

void ReportAssistantDialog::connectSignals(ReportAssistantPage * page)
{
    //React to the changes in the assistant pages
    connect(page, SIGNAL(completeChanged(ReportAssistantPage*,bool)),
             this, SLOT(completeChanged(ReportAssistantPage*,bool)));
}

void ReportAssistantDialog::currentPageChanged_slot(KPageWidgetItem * current , KPageWidgetItem * before)
{
    //Page changed

    enableButton(KDialog::Cancel, true);
    m_canClose = false;

    //Save data of the previous page
    if (before) {
        ReportAssistantPage* beforePage = dynamic_cast<ReportAssistantPage*>(before->widget());
        beforePage->aboutToHide();
    }

    //Load data of the current(new) page
    if (current) {
        ReportAssistantPage* currentPage = dynamic_cast<ReportAssistantPage*>(current->widget());
        enableNextButton(currentPage->isComplete());
        currentPage->aboutToShow();
    }
}

void ReportAssistantDialog::completeChanged(ReportAssistantPage* page, bool isComplete)
{
    if (page == dynamic_cast<ReportAssistantPage*>(currentPage()->widget())) {
        enableNextButton(isComplete);
    }
}

void ReportAssistantDialog::assistantFinished(bool showBack)
{
    //The assistant finished: allow the user to close the dialog normally

    enableNextButton(false);
    enableButton(KDialog::User3, showBack); //Back button
    enableButton(KDialog::User1, true);
    enableButton(KDialog::Cancel, false);

    m_canClose = true;
}

//Override KAssistantDialog "next" page implementation
void ReportAssistantDialog::next()
{
    //Allow the widget to Ask a question to the user before changing the page
    ReportAssistantPage * page = dynamic_cast<ReportAssistantPage*>(currentPage()->widget());
    if (page) {
        if (!page->showNextPage()) {
            return;
        }
    }

    const QString name = currentPage()->name();

    //If the information the user can provide is not useful, skip the backtrace page
    if (name == QLatin1String(PAGE_AWARENESS_ID))
    {
        //Force save settings in the current page
        page->aboutToHide();

        if (!(m_reportInterface->isBugAwarenessPageDataUseful()))
        {
            setCurrentPage(m_pageWidgetMap.value(QLatin1String(PAGE_CONCLUSIONS_ID)));
            return;
        }
    } else if (name == QLatin1String(PAGE_CRASHINFORMATION_ID)){
        //Force save settings in current page
        page->aboutToHide();
    }

    KAssistantDialog::next();
}

//Override KAssistantDialog "back"(previous) page implementation
//It has to mirror the custom next() implementation
void ReportAssistantDialog::back()
 {
    if (currentPage()->name() == QLatin1String(PAGE_CONCLUSIONS_ID))
    {
        if (!(m_reportInterface->isBugAwarenessPageDataUseful()))
        {
            setCurrentPage(m_pageWidgetMap.value(QLatin1String(PAGE_AWARENESS_ID)));
            return;
        }
    }

    KAssistantDialog::back();
}

void ReportAssistantDialog::enableNextButton(bool enabled)
{
    enableButton(KDialog::User2, enabled);
}

void ReportAssistantDialog::reject()
{
    close();
}

void ReportAssistantDialog::closeEvent(QCloseEvent * event)
{
    //Handle the close event
    if (!m_canClose) {
        //If the assistant didn't finished yet, offer the user the possibilities to
        //Close, Cancel, or Save the bug report and Close"

        KGuiItem closeItem = KStandardGuiItem::close();
        closeItem.setText(i18nc("@action:button", "Close the assistant"));

        KGuiItem keepOpenItem = KStandardGuiItem::cancel();
        keepOpenItem.setText(i18nc("@action:button", "Cancel"));

        BacktraceParser::Usefulness use =
                DrKonqi::debuggerManager()->backtraceGenerator()->parser()->backtraceUsefulness();
        if (use == BacktraceParser::ReallyUseful || use == BacktraceParser::MayBeUseful) {
            //Backtrace is still useful, let the user save it.
            KGuiItem saveBacktraceItem = KStandardGuiItem::save();
            saveBacktraceItem.setText(i18nc("@action:button", "Save information and close"));

            int ret = KMessageBox::questionYesNoCancel(this,
                           i18nc("@info","Do you really want to close the bug reporting assistant? "
                           "<note>The crash information is still valid, so "
                           "you can save the report before closing if you want.</note>"),
                           i18nc("@title:window","Close the Assistant"),
                           closeItem, saveBacktraceItem, keepOpenItem, QString(), KMessageBox::Dangerous);
            if(ret == KMessageBox::Yes)
            {
                event->accept();
            } else if (ret == KMessageBox::No) {
                //Save backtrace and accept event (dialog will be closed)
                DrKonqi::saveReport(reportInterface()->generateReportFullText(false));
                event->accept();
            } else {
                event->ignore();
            }
        } else {
            if (KMessageBox::questionYesNo(this, i18nc("@info","Do you really want to close the bug "
                                                   "reporting assistant?"),
                                       i18nc("@title:window","Close the Assistant"),
                                           closeItem, keepOpenItem, QString(), KMessageBox::Dangerous)
                                        == KMessageBox::Yes) {
                event->accept();
            } else {
                event->ignore();
            }
        }
    } else {
        event->accept();
    }
}
