/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

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

#ifndef KDIRSHAREPLUGIN_H
#define KDIRSHAREPLUGIN_H

#include <QDBusInterface>
#include <kpropertiesdialog.h>

#include "ui_kdirshareplugin.h"

class KDirSharePlugin : public KPropertiesDialogPlugin
{
    Q_OBJECT
public:
    KDirSharePlugin(QObject *parent, const QList<QVariant> &args);
    ~KDirSharePlugin();

    void applyChanges() final;

private Q_SLOTS:
    void slotShare(const bool value);
    void slotRandomPort(const bool value);
    void slotPortMin(const int value);
    void slotPortMax(const int value);
    void slotAuthorization(const bool value);
    void slotUserEdited(const QString &value);
    void slotPasswordEdited(const QString &value);

private:
    Ui_KDirShareUI m_ui;
    QDBusInterface m_kdirshareiface;
    QString m_url;
};

#endif // KDIRSHAREPLUGIN_H
