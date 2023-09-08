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

#include <signal.h>

// NOTE: keep in sync with:
// kdelibs/kdeui/util/kcrash.cpp

static const QStringList s_kcrashfilters = QStringList()
    << QString::fromLatin1("*.kcrash");

static QString kSignalDescription(const int signal)
{
    switch (signal) {
        // for reference:
        // https://man7.org/linux/man-pages/man7/signal.7.html
        case SIGSEGV: {
            return i18n("Invalid memory reference");
        }
        case SIGBUS: {
            return i18n("Bus error (bad memory access)");
        }
        case SIGFPE: {
            return i18n("Floating-point exception");
        }
        case SIGILL: {
            return i18n("Illegal Instruction");
        }
        case SIGABRT: {
            return i18n("Abort signal from <i>abort<i>");
        }
    }
    return i18n("Unknown");
}

static QString kFixURL(const QString &url)
{
    KUrl kurl(url);
    const QString kurlprotocol = kurl.protocol();
    if (kurlprotocol.isEmpty() || kurlprotocol == QLatin1String("mailto")) {
        kurl.setScheme("mailto");
    }
    return kurl.url();
}

KCrashDialog::KCrashDialog(const KCrashDetails &kcrashdetails, QWidget *parent)
    : KDialog(parent),
    m_widget(nullptr),
    m_layout(nullptr),
    m_pixmap(nullptr),
    m_label(nullptr),
    m_backtrace(nullptr)
{
    setWindowIcon(KIcon("tools-report-bug"));
    // do not include the application name in the title
    setWindowTitle(
        i18nc(
            "@title:window", "Crash Details for %1 (%2)",
            kcrashdetails.kcrashappname, kcrashdetails.kcrashapppid
        )
    );
    setButtons(KDialog::Ok | KDialog::Close);
    setDefaultButton(KDialog::Ok);
    setButtonText(KDialog::Ok, i18nc("@action:button", "Report"));
    m_reporturl = kFixURL(kcrashdetails.kcrashbugreporturl);
    if (m_reporturl.startsWith(QLatin1String("mailto"))) {
        setButtonIcon(KDialog::Ok, KIcon("internet-mail"));
    } else {
        setButtonIcon(KDialog::Ok, KIcon("internet-web-browser"));
    }

    m_widget = new QWidget(this);
    m_layout = new QGridLayout(m_widget);

    // const int dialogiconsize = KIconLoader::global()->currentSize(KIconLoader::Dialog);
    m_pixmap = new KPixmapWidget(m_widget);
    m_pixmap->setPixmap(KIcon(kcrashdetails.kcrashappicon).pixmap(64));
    m_layout->addWidget(m_pixmap, 0, 0);
    m_label = new QLabel(m_widget);
    m_label->setWordWrap(false);
    m_label->setOpenExternalLinks(true);
    m_label->setText(
        i18n(
            "Reason: %1<br/>Bug address: <a href=\"%2\">%3</a><br/> Homepage: <a href=\"%4\">%4</a>",
            kSignalDescription(kcrashdetails.kcrashsignal),
            kFixURL(kcrashdetails.kcrashbugaddress), kcrashdetails.kcrashbugaddress,
            kFixURL(kcrashdetails.kcrashhomepage)
        )
    );
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_layout->addWidget(m_label, 0, 1);

    m_backtrace = new KTextEdit(m_widget);
    m_backtrace->setReadOnly(true);
    m_backtrace->setLineWrapMode(QTextEdit::NoWrap);
    m_backtrace->setText(
        QString::fromLatin1(
            kcrashdetails.kcrashbacktrace.constData(),
            kcrashdetails.kcrashbacktrace.size()
        )
    );
    m_layout->addWidget(m_backtrace, 1, 0, 1, 2);

    setMainWidget(m_widget);

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
            kcrashdetails.kcrashsignal = kcrashsignal;
            kcrashdetails.kcrashappname = kcrashappname;
            kcrashdetails.kcrashapppid = kcrashdata["pid"];
            kcrashdetails.kcrashappicon = kcrashdata["programicon"];
            kcrashdetails.kcrashbugaddress = kcrashdata["bugaddress"];
            kcrashdetails.kcrashhomepage = kcrashdata["homepage"];
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
