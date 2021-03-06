/*  This file is part of the KDE project
    Copyright (C) 2000 David Faure <faure@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kserviceselectdlg.h"
#include "moc_kserviceselectdlg.cpp"
#include "kservicelistwidget.h"

#include <klocale.h>
#include <QVBoxLayout>
#include <QLabel>

KServiceSelectDlg::KServiceSelectDlg( const QString& /*serviceType*/, const QString& /*value*/, QWidget *parent )
    : KDialog( parent )
{
    setObjectName( QLatin1String( "serviceSelectDlg" ) );
    setModal( true );
    setCaption( i18n( "Add Service" ) );
    setButtons( Ok | Cancel );

    QWidget *vbox = new QWidget( this );
    QVBoxLayout *layout = new QVBoxLayout( vbox );

    layout->addWidget( new QLabel( i18n( "Select service:" ), vbox ) );
    m_listbox=new KListWidget( vbox );

    // Can't make a KTrader query since we don't have a servicetype to give,
    // we want all services that are not applications.......
    // So we have to do it the slow way
    // ### Why can't we query for KParts/ReadOnlyPart as the servicetype? Should work fine!
    const KService::List allServices = KService::allServices();
    KService::List::const_iterator it(allServices.constBegin());
    for ( ; it != allServices.constEnd() ; ++it )
      if ( (*it)->hasServiceType( "KParts/ReadOnlyPart" ) )
      {
          m_listbox->addItem( new KServiceListItem( (*it), KServiceListWidget::SERVICELIST_SERVICES ) );
      }

    m_listbox->model()->sort(0);
    m_listbox->setMinimumHeight(350);
    m_listbox->setMinimumWidth(400);
    layout->addWidget( m_listbox );
    connect(m_listbox,SIGNAL(itemDoubleClicked(QListWidgetItem*)),SLOT(slotOk()));
    connect( this, SIGNAL(okClicked()), this, SLOT(slotOk()) );
    setMainWidget(vbox);
}

KServiceSelectDlg::~KServiceSelectDlg()
{
}

void KServiceSelectDlg::slotOk()
{
   accept();
}

KService::Ptr KServiceSelectDlg::service()
{
    int selIndex = m_listbox->currentRow();
    KServiceListItem *selItem = static_cast<KServiceListItem *>(m_listbox->item(selIndex));
    return KService::serviceByDesktopPath( selItem->desktopPath );
}
