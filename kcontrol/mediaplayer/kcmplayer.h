/*  This file is part of the KDE libraries
    Copyright (C) 2016 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KCMPLAYER_H
#define KCMPLAYER_H

#include <kcmodule.h>
#include <kmediaplayer.h>
#include <ksettings.h>

QT_BEGIN_NAMESPACE
class Ui_KCMPlayer;
QT_END_NAMESPACE

class KCMPlayer : public KCModule
{
    Q_OBJECT

public:
    KCMPlayer(QWidget *parent, const QVariantList &arguments);
    ~KCMPlayer();

public slots:
    void defaults();
    void save();
    void load();

    void setGlobalOutput(QString output);
    void setGlobalVolume(int volume);
    void setGlobalMute(int mute);

    void setApplicationSettings(QString application);
    void setApplicationOutput(QString output);
    void setApplicationVolume(int volume);
    void setApplicationMute(int mute);

private:
    Ui_KCMPlayer *m_ui;
    KSettings *m_settings;
    QString m_application;
};

#endif // KCMPLAYER_H
