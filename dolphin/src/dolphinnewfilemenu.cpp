/***************************************************************************
 *   Copyright (C) 2006 by Peter Penz                                      *
 *   peter.penz@gmx.at                                                     *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "dolphinnewfilemenu.h"

#include "views/dolphinnewfilemenuobserver.h"

#include <KActionCollection>
#include <KIO/Job>

DolphinNewFileMenu::DolphinNewFileMenu(KActionCollection* collection, QObject* parent) :
    KNewFileMenu(collection, "new_menu", parent)
{
    DolphinNewFileMenuObserver::instance().attach(this);
}

DolphinNewFileMenu::~DolphinNewFileMenu()
{
    DolphinNewFileMenuObserver::instance().detach(this);
}

void DolphinNewFileMenu::slotResult(KJob* job)
{
    if (job->error()) {
        emit errorMessage(job->errorString());
    } else {
        KNewFileMenu::slotResult(job);
    }
}

#include "moc_dolphinnewfilemenu.cpp"
