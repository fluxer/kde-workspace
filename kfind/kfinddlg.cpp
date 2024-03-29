/*******************************************************************
* kfinddlg.cpp
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

#include "kfinddlg.h"
#include "moc_kfinddlg.cpp"

#include <QLayout>
#include <QPushButton>

#include <klocale.h>
#include <kglobal.h>
#include <kguiitem.h>
#include <kstatusbar.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kaboutapplicationdialog.h>
#include <kstandarddirs.h>
#include <khelpmenu.h>
#include <kmenu.h>

#include "kftabdlg.h"
#include "kquery.h"
#include "kfindtreeview.h"

KfindDlg::KfindDlg(const KUrl & url, QWidget *parent)
  : KDialog( parent )
{
  setButtons( User1 | User2 | User3 | Close | Help );
  setDefaultButton( User3 );
  setModal( true );

  setButtonGuiItem( User3, KStandardGuiItem::find());
  setButtonGuiItem( User2, KStandardGuiItem::stop() );
  setButtonGuiItem( User1, KStandardGuiItem::saveAs() );

  QWidget::setWindowTitle( i18nc("@title:window", "Find Files/Folders" ) );
  setButtonsOrientation(Qt::Vertical);

  enableButton(User3, true); // Enable "Find"
  enableButton(User2, false); // Disable "Stop"
  enableButton(User1, false); // Disable "Save As..."

  isResultReported = false;

  QFrame *frame = new QFrame;
  setMainWidget( frame );

  // create tabwidget
  tabWidget = new KfindTabWidget( frame );
  tabWidget->setURL( url );
  tabWidget->setFocus();

  // prepare window for find results
  win = new KFindTreeView(frame, this);

  mStatusBar = new KStatusBar(frame);
  mStatusBar->insertItem("AMiddleLengthText...", 0);
  setStatusMsg( i18nc("the application is currently idle, there is no active search", "Idle.") );
  mStatusBar->setItemAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
  mStatusBar->insertPermanentItem(QString(), 1, 1);
  mStatusBar->setItemAlignment(1, Qt::AlignRight | Qt::AlignVCenter);

  QVBoxLayout *vBox = new QVBoxLayout(frame);
  vBox->addWidget(tabWidget, 0);
  vBox->addWidget(win, 1);
  vBox->addWidget(mStatusBar, 0);

  connect(tabWidget, SIGNAL(startSearch()),
          this, SLOT(startSearch()));
  connect(this, SIGNAL(user3Clicked()),
          this, SLOT(startSearch()));
  connect(this, SIGNAL(user2Clicked()),
	  this, SLOT(stopSearch()));
  connect(this, SIGNAL(user1Clicked()),
	  win, SLOT(saveResults()));

  connect( this, SIGNAL(closeClicked()), this, SLOT(finishAndClose()) );

  connect(win ,SIGNAL(resultSelected(bool)),
	  this,SIGNAL(resultSelected(bool)));

  query = new KQuery(frame);
    connect(query, SIGNAL(result(int)), SLOT(slotResult(int)));
    connect(query, SIGNAL(foundFileList(QList<QPair<KFileItem,QString> >)), SLOT(addFiles(QList<QPair<KFileItem,QString> >)));

  KHelpMenu *helpMenu = new KHelpMenu(this, KGlobal::mainComponent().aboutData(), true);
  setButtonMenu( Help, helpMenu->menu() );
}

KfindDlg::~KfindDlg()
{
  stopSearch();
}

void KfindDlg::finishAndClose()
{
  //Stop the current search and closes the dialog
  stopSearch();
  close();
}

void KfindDlg::setProgressMsg(const QString &msg)
{
   mStatusBar->changeItem(msg, 1);
}

void KfindDlg::setStatusMsg(const QString &msg)
{
   mStatusBar->changeItem(msg, 0);
}


void KfindDlg::startSearch()
{
  tabWidget->setQuery(query);

  isResultReported = false;

  // Reset count - use the same i18n as below
  setProgressMsg(i18np("one file found", "%1 files found", 0));

  emit resultSelected(false);
  emit haveResults(false);

  enableButton(User3, false); // Disable "Find"
  enableButton(User2, true); // Enable "Stop"
  enableButton(User1, false); // Disable "Save As..."

  win->beginSearch(query->url());
  tabWidget->beginSearch();

  setStatusMsg(i18n("Searching..."));
  query->start();
}

void KfindDlg::stopSearch()
{
  query->kill();
}

void KfindDlg::newSearch()
{
  // WABA: Not used any longer?
  stopSearch();

  tabWidget->setDefaults();

  emit haveResults(false);
  emit resultSelected(false);

  setFocus();
}

void KfindDlg::slotResult(int errorCode)
{
  if (errorCode == 0)
    setStatusMsg( i18nc("the application is currently idle, there is no active search", "Idle.") );
  else if (errorCode == KIO::ERR_USER_CANCELED)
    setStatusMsg(i18n("Canceled."));
  else if (errorCode == KIO::ERR_MALFORMED_URL)
  {
     setStatusMsg(i18n("Error."));
     KMessageBox::sorry( this, i18n("Please specify an absolute path in the \"Look in\" box."));
  }
  else if (errorCode == KIO::ERR_DOES_NOT_EXIST)
  {
     setStatusMsg(i18n("Error."));
     KMessageBox::sorry( this, i18n("Could not find the specified folder."));
  }
  else
  {
     kDebug()<<"KIO error code: "<<errorCode;
     setStatusMsg(i18n("Error."));
  };

  enableButton(User3, true); // Enable "Find"
  enableButton(User2, false); // Disable "Stop"
  enableButton(User1, true); // Enable "Save As..."

  win->endSearch();
  tabWidget->endSearch();
  setFocus();

}

void KfindDlg::addFiles( const QList< QPair<KFileItem,QString> > & pairs)
{
  win->insertItems( pairs );

  if (!isResultReported)
  {
    emit haveResults(true);
    isResultReported = true;
  }

  QString str = i18np("one file found", "%1 files found", win->itemCount());
  setProgressMsg( str );
}

void KfindDlg::setFocus()
{
  tabWidget->setFocus();
}

void KfindDlg::copySelection()
{
  win->copySelection();
}

void  KfindDlg::about ()
{
  KAboutApplicationDialog dlg(0, this);
  dlg.exec ();
}

QStringList KfindDlg::getAllSubdirs(QDir d)
{
  QStringList dirs;
  QStringList subdirs;

  d.setFilter( QDir::Dirs );
  dirs = d.entryList();

  for(QStringList::const_iterator it = dirs.constBegin(); it != dirs.constEnd(); ++it)
  {
    if((*it==".")||(*it==".."))
      continue;
    subdirs.append(d.path()+'/'+*it);
    subdirs+=getAllSubdirs(QString(d.path()+QLatin1Char('/')+*it));
  }
  return subdirs;
}
