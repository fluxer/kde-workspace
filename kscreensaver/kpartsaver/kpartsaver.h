/*
 * Copyright (C) 2001 Stefan Schimanski <1Stein@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KPARTSAVER_H_INCLUDED
#define KPARTSAVER_H_INCLUDED

#include <QDebug>
#include <kscreensaver.h>

#include "ui_configwidget.h"
#include <QList>
#include <KDialog>
class QTimer;
class QLabel;
namespace KParts { class ReadOnlyPart; }

class ConfigWidget : public QWidget, public Ui::ConfigWidget
{
public:
    ConfigWidget( QWidget *parent = 0L ) : QWidget( parent ) {
        setupUi( this );
    }
    virtual ~ConfigWidget() {
        qDebug()<<" delete ConfigWidget";
    }
};


class SaverConfig : public KDialog {
Q_OBJECT

 public:
    SaverConfig( QWidget* parent = 0);
    ~SaverConfig();

 protected slots:
    void apply();
    void add();
    void remove();
    void select();
    void up();
    void down();
    void slotHelp();
private:
    ConfigWidget *cfg;
};


class KPartSaver : public KScreenSaver {
Q_OBJECT

 public:
    KPartSaver( WId id=0 );
    virtual ~KPartSaver();

 public slots:
    void next( bool random );
    void queue( const KUrl &url );
    void timeout();
    void closeUrl();

 protected:
    struct Medium {
	KUrl url;
	bool failed;
        inline bool operator==(const Medium& other) { return url == other.url; }
        inline bool operator!=(const Medium& other) { return !(*this == other); }
    };

    bool openUrl(  const KUrl& url );

    QList<Medium> m_media;
    QTimer *m_timer;
    KParts::ReadOnlyPart *m_part;
    int m_current;

    bool m_single;
    bool m_random;
    int m_delay;
    QStringList m_files;
    QLabel *m_back;
};

#endif
