/*
 *   Copyright (C) 2008 Laurent Montel <montel@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "preferencesdlg.h"

#include <QHBoxLayout>
#include <QCheckBox>

#include <KLocale>
#include <KConfigGroup>

PreferencesDialog::PreferencesDialog( QWidget *parent )
    : KPageDialog( parent )
{
    setFaceType( List );
    setButtons( Ok | Cancel );
    setDefaultButton( Ok );

    m_pageMisc = new MiscPage( this );
    KPageWidgetItem *page = new KPageWidgetItem( m_pageMisc , i18n( "General" ) );
    page->setIcon( KIcon( "kmenuedit" ) );
    addPage(page);

    connect( this, SIGNAL(okClicked()), this, SLOT(slotSave()) );
}

void PreferencesDialog::slotSave()
{
    m_pageMisc->saveOptions();
}

MiscPage::MiscPage( QWidget *parent )
    : QWidget( parent )
{
    QVBoxLayout *lay = new QVBoxLayout( this );
    m_showHiddenEntries = new QCheckBox( i18n( "Show hidden entries" ), this );
    lay->addWidget( m_showHiddenEntries );
    lay->addStretch();
    setLayout( lay );

    KConfigGroup group( KGlobal::config(), "General" );
    m_showHiddenEntries->setChecked(  group.readEntry( "ShowHidden", false ) );
}

void MiscPage::saveOptions()
{
    KConfigGroup group( KGlobal::config(), "General" );
    group.writeEntry( "ShowHidden", m_showHiddenEntries->isChecked() );
    group.sync();
}

#include "moc_preferencesdlg.cpp"
