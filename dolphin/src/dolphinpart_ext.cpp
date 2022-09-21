/* This file is part of the KDE project
 * Copyright (c) 2012 Dawit Alemayehu <adawit@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "dolphinpart_ext.h"

#include "dolphinpart.h"
#include "views/dolphinview.h"

#include <QVariant>

#include <KFileItemList>


DolphinPartBrowserExtension::DolphinPartBrowserExtension(DolphinPart* part)
    :KParts::BrowserExtension( part )
    ,m_part(part)
{

}

void DolphinPartBrowserExtension::restoreState(QDataStream &stream)
{
    KParts::BrowserExtension::restoreState(stream);
    m_part->view()->restoreState(stream);
}

void DolphinPartBrowserExtension::saveState(QDataStream &stream)
{
    KParts::BrowserExtension::saveState(stream);
    m_part->view()->saveState(stream);
}

void DolphinPartBrowserExtension::cut()
{
    m_part->view()->cutSelectedItems();
}

void DolphinPartBrowserExtension::copy()
{
    m_part->view()->copySelectedItems();
}

void DolphinPartBrowserExtension::paste()
{
    m_part->view()->paste();
}

void DolphinPartBrowserExtension::pasteTo(const KUrl&)
{
    m_part->view()->pasteIntoFolder();
}

void DolphinPartBrowserExtension::reparseConfiguration()
{
    m_part->view()->readSettings();
}

#include "moc_dolphinpart_ext.cpp"
