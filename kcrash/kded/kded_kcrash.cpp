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
#include <kurl.h>
#include <kcrash.h>
#include <kdebug.h>

#include <QDir>
#include <QFileInfo>

// NOTE: keep in sync with:
// kdelibs/kdeui/util/kcrash.cpp

static const QStringList s_kcrashfilters = QStringList()
    << QString::fromLatin1("*.kcrash");

KCrashDialog::KCrashDialog(const KCrashDetails &kcrashdetails, QWidget *parent)
    : KDialog(parent),
    m_mainvbox(nullptr)
{
    setWindowIcon(KIcon("tools-report-bug"));
    // do not include the application name in the title
    setWindowTitle(
        KDialog::makeStandardCaption(
            i18nc(
                "@title:window", "Crash Details for %1 (%2)",
                kcrashdetails.kcrashappname, kcrashdetails.kcrashapppid
            ),
            nullptr, KDialog::NoCaptionFlags
        )
    );
    setButtons(KDialog::Ok | KDialog::Close);
    setDefaultButton(KDialog::Ok);
    setButtonText(KDialog::Ok, i18nc("@action:button", "Report"));
    KUrl bugreporturl(kcrashdetails.kcrashbugreporturl);
    const QString bugreporturlprotocol = bugreporturl.protocol();
    if (bugreporturlprotocol.isEmpty() || bugreporturlprotocol == QLatin1String("mailto")) {
        setButtonIcon(KDialog::Ok, KIcon("internet-mail"));
        bugreporturl.setScheme("mailto");
    } else {
        setButtonIcon(KDialog::Ok, KIcon("internet-web-browser"));
    }
    m_reporturl = bugreporturl.url();

    m_mainvbox = new KVBox(this);
    setMainWidget(m_mainvbox);

    m_backtrace = new KTextEdit(m_mainvbox);
    m_backtrace->setReadOnly(true);
    m_backtrace->setLineWrapMode(QTextEdit::NoWrap);
    m_backtrace->setText(
        QString::fromLatin1(
            kcrashdetails.kcrashbacktrace.constData(),
            kcrashdetails.kcrashbacktrace.size()
        )
    );

    KConfigGroup kconfiggroup(KGlobal::config(), "KCrashDialog");
    restoreDialogSize(kconfiggroup);
}

KCrashDialog::~KCrashDialog()
{
    KConfigGroup kconfiggroup(KGlobal::config(), "KCrashDialog");
    saveDialogSize(kconfiggroup);
    KGlobal::config()->sync();
}

QString KCrashDialog::reportUrl() const
{
    return m_reporturl;
}


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
    QMutableMapIterator<KNotification*,KCrashDetails> iter(m_notifications);
    while (iter.hasNext()) {
        iter.next();
        KNotification* knotification = iter.key();
        disconnect(knotification, 0, this, 0);
        knotification->close();
        iter.remove();
    }

    kDebug() << "Closing dialogs" << m_dialogs.size();
    qDeleteAll(m_dialogs);
    m_dialogs.clear();
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
        QMap<QByteArray,QString> kcrashdata;
        int kcrashsignal = 0;
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
        QString kcrashappname = kcrashdata["programname"];
        if (kcrashappname.isEmpty()) {
            kcrashappname = kcrashdata["appname"];
        }
        if (kcrashflags & KCrash::Notify) {
            QString bugreporturl = kcrashdata["bugaddress"];
            // special case for Katana applications
            if (bugreporturl == QLatin1String(KDE_BUG_REPORT_EMAIL)) {
                bugreporturl = QString::fromLatin1(KDE_BUG_REPORT_URL);
            }

            KCrashDetails kcrashdetails;
            kcrashdetails.kcrashappname = kcrashappname;
            kcrashdetails.kcrashapppid = kcrashdata["pid"];
            kcrashdetails.kcrashbacktrace = kcrashbacktrace;
            kcrashdetails.kcrashbugreporturl = bugreporturl;

            kDebug() << "Sending notification for" << kcrashfilepath;
            // NOTE: when the notification is closed/deleted the actions become non-operational
            KNotification* knotification = new KNotification(this);
            knotification->setEventID("kcrash/Crash");
            knotification->setFlags(KNotification::Persistent);
            knotification->setTitle(i18n("%1 crashed", kcrashappname));
            knotification->setText(i18n("To view details about the crash and report it click on the details button."));
            knotification->setActions(QStringList() << i18n("Details"));
            m_notifications.insert(knotification, kcrashdetails);
            connect(knotification, SIGNAL(closed()), this, SLOT(slotClosed()));
            connect(knotification, SIGNAL(action1Activated()), this, SLOT(slotDetails()));
            knotification->send();
        }
        if (kcrashflags & KCrash::Log) {
            // NOTE: this goes to the system log by default which the user may not have access to
            kError() << kcrashappname << "crashed" << kcrashbacktrace;
        }
        if (kcrashflags & KCrash::AutoRestart) {
            const QString kcrashdisplay = kcrashdata["display"];
            const QString kcrashapppath = kcrashdata["apppath"];
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
    m_notifications.remove(knotification);
}

void KCrashModule::slotDetails()
{
    KNotification* knotification = qobject_cast<KNotification*>(sender());
    kDebug() << "Notification details will be shown" << knotification;
    const KCrashDetails kcrashdetails = m_notifications.value(knotification);
    knotification->close();
    KCrashDialog* kcrashdialog = new KCrashDialog(kcrashdetails);
    m_dialogs.append(kcrashdialog);
    connect(kcrashdialog, SIGNAL(finished(int)), this, SLOT(slotDialogFinished(int)));
    kcrashdialog->show();
}

void KCrashModule::slotDialogFinished(const int result)
{
    kDebug() << "Notification details result" << result;

    KCrashDialog* kcrashdetails = qobject_cast<KCrashDialog*>(sender());
    const QString kcrashreporturl = kcrashdetails->reportUrl();
    m_dialogs.removeAll(kcrashdetails);
    kcrashdetails->deleteLater();

    if (result == QDialog::Accepted) {
        if (kcrashreporturl.startsWith(QLatin1String("mailto:"))) {
            KToolInvocation::invokeMailer(kcrashreporturl, QString::fromLatin1("Crash report"));
        } else {
            KToolInvocation::invokeBrowser(kcrashreporturl);
        }
    }
}

#include "moc_kded_kcrash.cpp"
