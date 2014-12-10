//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 2002
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef KXSXML_H
#define KXSXML_H

#include "kxsconfig.h"
#include <QXmlDefaultHandler>
#include <QList>
#include <QStack>

class QWidget;
class KXSXmlHandler;

class KXSXml
{
public:
    KXSXml( QWidget *p );

    bool parse( const QString &filename );
    QList<KXSConfigItem*> items() const;
    QString name() const;
    QString description() const;

private:
    QWidget *parent;
    KXSXmlHandler *handler;
};

class KXSXmlHandler : public QXmlDefaultHandler
{
public:
    KXSXmlHandler( QWidget *p );

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
	    const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );
    bool characters( const QString & );

    QList<KXSConfigItem*> items() const { return mConfigItemList; }
    const QString &name() const { return mName; }
    const QString &description() const { return mDesc; }

private:
    QWidget *parent;
    KXSSelectItem *selItem;
    bool inDesc;
    QString mName;
    QString mDesc;
    QList<KXSConfigItem*> mConfigItemList;
    QStack<QWidget*> mParentStack;
};

#endif // KXSXML_H

