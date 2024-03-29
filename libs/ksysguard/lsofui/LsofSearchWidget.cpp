/*
    KSysGuard, the KDE System Guard

	Copyright (c) 1999 Chris Schlaeger <cs@kde.org>
	Copyright (c) 2007 John Tapsell <tapsell@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.


*/

#include <klocale.h>
#include <kdebug.h>

#include "moc_LsofSearchWidget.cpp"
#include "ui_LsofSearchWidget.h"

LsofSearchWidget::LsofSearchWidget(QWidget* parent, int pid)
    : KDialog(parent)
{
    setObjectName("Renice Dialog");
    setModal(true);
    setCaption(i18n("Renice Process"));
    setButtons(KDialog::Close);
    QWidget *widget = new QWidget(this);
    setMainWidget(widget);
    ui = new Ui_LsofSearchWidget();
    ui->setupUi(widget);
    ui->klsofwidget->setPid(pid);
    ktreewidgetsearchline
}

