/*  This file is part of the KDE project
    Copyright (C) 2023 Ivailo Monev <xakepa10@gmail.com>

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

#ifndef KCRASH_KDED_H
#define KCRASH_KDED_H

#include "kdedmodule.h"

#include <kdirwatch.h>
#include <knotification.h>
#include <kdialog.h>
#include <kvbox.h>
#include <ktextedit.h>

struct KCrashDetails
{
    QString kcrashappname;
    QString kcrashapppid;
    QByteArray kcrashbacktrace;
    QString kcrashbugreporturl;
};


class KCrashDialog : public KDialog
{
    Q_OBJECT
public:
    KCrashDialog(const KCrashDetails &kcrashdetails, QWidget *parent = nullptr);
    ~KCrashDialog();

    QString reportUrl() const;

private:
    QString m_reporturl;
    KVBox* m_mainvbox;
    KTextEdit* m_backtrace;
};


class KCrashModule: public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kcrash")

public:
    KCrashModule(QObject *parent, const QList<QVariant> &args);
    ~KCrashModule();

private Q_SLOTS:
    void slotDirty(const QString &path);
    void slotClosed();
    void slotDetails();
    void slotDialogFinished(const int result);

private:
    QString m_kcrashpath;
    KDirWatch *m_dirwatch;
    QMap<KNotification*,KCrashDetails> m_notifications;
    QList<KCrashDialog*> m_dialogs;
};

#endif // KCRASH_KDED_H
