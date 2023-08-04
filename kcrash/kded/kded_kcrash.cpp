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

#include "kded_kcrash.h"
#include <kpluginfactory.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kglobalsettings.h>
#include <ktoolinvocation.h>
#include <kcrash.h>
#include <kdebug.h>

#include <QDir>
#include <QFileInfo>

// NOTE: keep in sync with:
// kdelibs/kdeui/util/kcrash.cpp

static const QStringList s_kcrashfilters = QStringList()
    << QString::fromLatin1("*.kcrash");

K_PLUGIN_FACTORY(KCrashModuleFactory, registerPlugin<KCrashModule>();)
K_EXPORT_PLUGIN(KCrashModuleFactory("kcrash"))

KCrashModule::KCrashModule(QObject *parent, const QList<QVariant> &args)
    : KDEDModule(parent),
    m_dirwatch(nullptr)
{
    Q_UNUSED(args);

    m_kcrashpath = KGlobal::dirs()->saveLocation("tmp", "kcrash/");
    m_dirwatch = new KDirWatch(this);
    m_dirwatch->addDir(m_kcrashpath);
    connect(
        m_dirwatch, SIGNAL(dirty(QString)),
        this, SLOT(slotDirty(QString))
    );
}

KCrashModule::~KCrashModule()
{
    delete m_dirwatch;
    kDebug() << "Closing notifications" << m_notifications.size();
    QMutableListIterator<KNotification*> iter(m_notifications);
    while (iter.hasNext()) {
        KNotification* knotification = iter.next();
        disconnect(knotification, 0, this, 0);
        knotification->close();
        iter.remove();
    }
}

void KCrashModule::slotDirty(const QString &path)
{
    Q_UNUSED(path);
    QDir kcrashdir(m_kcrashpath);
    foreach (const QFileInfo &fileinfo, kcrashdir.entryInfoList(s_kcrashfilters, QDir::Files)) {
        const QString kcrashfilepath = fileinfo.absoluteFilePath();
        QFile kcrashfile(kcrashfilepath);
        if (!kcrashfile.open(QFile::ReadOnly)) {
            kWarning() << "Could not open" << kcrashfilepath;
            continue;
        }

        kDebug() << "Reading" << kcrashfilepath;
        QVariantMap kcrashdata;
        int kcrashsignal;
        QByteArray kcrashbacktrace;
        {
            QDataStream crashstream(&kcrashfile);
            crashstream >> kcrashdata;
            crashstream >> kcrashsignal;
            crashstream >> kcrashbacktrace;
        }
        if (!QFile::remove(kcrashfilepath)) {
            kWarning() << "Could not remove" << kcrashfilepath;
        }

        const KCrash::CrashFlags kcrashflags = static_cast<KCrash::CrashFlags>(
            kcrashdata["flags"].toInt()
        );
        if (kcrashflags & KCrash::DrKonqi) {
            QString kcrashappname = kcrashdata["programname"].toString();
            if (kcrashappname.isEmpty()) {
                kcrashappname = kcrashdata["appname"].toString();
            }
            kDebug() << "Sending notification for" << kcrashfilepath;
            // NOTE: when the notification is closed/deleted the actions become non-operational
            KNotification* knotification = new KNotification("Crash", KNotification::Persistent, this);
            knotification->setComponentData(KComponentData("kcrash"));
            knotification->setTitle(i18n("%1 crashed", kcrashappname));
            knotification->setText(i18n("For details about the crash look into the system log."));
            knotification->setActions(QStringList() << i18n("Report"));
            m_notifications.append(knotification);
            connect(knotification, SIGNAL(closed()), this, SLOT(slotClosed()));
            connect(knotification, SIGNAL(action1Activated()), this, SLOT(slotReport()));
            knotification->sendEvent();
        }
        if (kcrashflags & KCrash::AutoRestart) {
            const QString kcrashdisplay = kcrashdata["display"].toString();
            const QString kcrashapppath = kcrashdata["apppath"].toString();
            QStringList kcrashargs;
            if (!kcrashdisplay.isEmpty()) {
                kcrashargs.append(QString::fromLatin1("--display"));
                kcrashargs.append(kcrashdisplay);
            }
            kDebug() << "Restarting" << kcrashfilepath << kcrashapppath << kcrashargs;
            KToolInvocation::kdeinitExec(kcrashapppath, kcrashargs);
        }
    }
}

void KCrashModule::slotClosed()
{
    KNotification* knotification = qobject_cast<KNotification*>(sender());
    kDebug() << "Notification closed" << knotification;
    m_notifications.removeAll(knotification);
}

void KCrashModule::slotReport()
{
    KNotification* knotification = qobject_cast<KNotification*>(sender());
    knotification->close();
    const QString kcrashreporturl = KDE_BUG_REPORT_URL;
    KToolInvocation::invokeBrowser(kcrashreporturl);
}

#include "moc_kded_kcrash.cpp"
