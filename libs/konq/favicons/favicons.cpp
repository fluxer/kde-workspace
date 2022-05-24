/* This file is part of the KDE project
   Copyright (C) 2001 Malte Starostik <malte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "favicons.h"
#include "favicons_adaptor.h"

#include <kicontheme.h>
#include <kconfig.h>
#include <klocale.h>
#include <kde_file.h>
#include <kstandarddirs.h>
#include <kio/job.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QImage>
#include <QImageReader>

K_PLUGIN_FACTORY(FavIconsFactory, registerPlugin<FavIconsModule>();)
K_EXPORT_PLUGIN(FavIconsFactory("favicons"))

static QString portForUrl(const KUrl& url)
{
    if (url.port() > 0) {
        return (QString(QLatin1Char('_')) + QString::number(url.port()));
    }
    return QString();
}

static QString simplifyURL(const KUrl &url)
{
    // splat any = in the URL so it can be safely used as a config key
    QString result = url.host() + portForUrl(url) + url.path();
    for (int i = 0; i < result.length(); ++i)
        if (result[i] == QLatin1Char('='))
            result[i] = QLatin1Char('_');
    return result;
}

static QString iconNameFromURL(const KUrl &iconURL)
{
    if (iconURL.path() == QLatin1String("/favicon.ico"))
       return iconURL.host() + portForUrl(iconURL);

    QString result = simplifyURL(iconURL);
    // splat / so it can be safely used as a file name
    for (int i = 0; i < result.length(); ++i) {
        if (result[i] == QLatin1Char('/')) {
            result[i] = QLatin1Char('_');
        }
    }

    QString ext = result.right(4);
    if (ext == QLatin1String(".ico") || ext == QLatin1String(".png") || ext == QLatin1String(".xpm"))
        result.remove(result.length() - 4, 4);

    return result;
}

static QString removeSlash(QString result)
{
    for (unsigned int i = result.length() - 1; i > 0; --i) {
        if (result[i] != QLatin1Char('/')) {
            result.truncate(i + 1);
            break;
        }
    }
    return result;
}

static QString faviconsCacheDir()
{
    QString faviconsDir = KGlobal::dirs()->saveLocation("cache", QLatin1String("favicons/"));
    faviconsDir.truncate(faviconsDir.length() - 9); // Strip off "favicons/"
    return faviconsDir;
}

struct FavIconsDownloadInfo
{
    QString hostOrURL;
    bool isHost;
    QByteArray iconData;
};
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(FavIconsDownloadInfo, Q_PRIMITIVE_TYPE);
QT_END_NAMESPACE

static QString makeIconName(const FavIconsDownloadInfo& download, const KUrl& iconURL)
{
    QString iconName (QLatin1String("favicons/"));
    iconName += (download.isHost ? download.hostOrURL : iconNameFromURL(iconURL));
    return iconName;
}

struct FavIconsModulePrivate
{
    FavIconsModulePrivate() : config(nullptr) { }
    ~FavIconsModulePrivate() { delete config; }

    QMap<KJob *, FavIconsDownloadInfo> downloads;
    KUrl::List failedDownloads;
    KConfig *config;
    KIO::MetaData metaData;
};

FavIconsModule::FavIconsModule(QObject* parent, const QList<QVariant>&)
    : KDEDModule(parent)
{
    // create our favicons folder so that KIconLoader knows about it
    d = new FavIconsModulePrivate;
    d->metaData.insert(QLatin1String("cache"), "reload");
    d->metaData.insert(QLatin1String("no-www-auth"), QLatin1String("true"));
    d->config = new KConfig(KStandardDirs::locateLocal("data", QLatin1String("konqueror/faviconrc")));

    new FavIconsAdaptor( this );
}

FavIconsModule::~FavIconsModule()
{
    delete d;
}

QString FavIconsModule::iconForUrl(const KUrl &url)
{
    if (url.host().isEmpty())
        return QString();

    // kDebug() << url;

    const QString simplifiedURL = removeSlash(simplifyURL(url));
    QString icon = d->config->group(QString()).readEntry(simplifiedURL, QString());

    if (!icon.isEmpty())
        icon = iconNameFromURL(KUrl(icon));
    else
        icon = url.host();

    icon = QLatin1String("favicons/") + icon;

    kDebug() << "URL:" << url << "ICON:" << icon;

    if (QFile::exists(faviconsCacheDir() + icon + QLatin1String(".png")))
        return icon;

    return QString();
}

bool FavIconsModule::isIconOld(const QString &icon)
{
    QFileInfo iconInfo(icon);
    const QDateTime iconLastModified = iconInfo.lastModified();
    if (!iconInfo.isFile() || !iconLastModified.isValid()) {
        // kDebug() << "isIconOld" << icon << "yes, no such file";
        return true; // Trigger a new download on error
    }

    // kDebug() << "isIconOld" << icon << "?";
    const QDateTime currentTime = QDateTime::currentDateTime();
    return ((currentTime.toTime_t() - iconLastModified.toTime_t()) > 604800); // arbitrary value (one week)
}

void FavIconsModule::setIconForUrl(const KUrl &url, const KUrl &iconURL)
{
    // kDebug() << url << iconURL;
    const QString iconName = QLatin1String("favicons/") + iconNameFromURL(iconURL);
    const QString iconFile = faviconsCacheDir() + iconName + QLatin1String(".png");

    if (!isIconOld(iconFile)) {
        // kDebug() << "emit iconChanged" << false << url << iconName;
        emit iconChanged(false, url.url(), iconName);
        return;
    }

    startDownload(url.url(), false, iconURL);
}

void FavIconsModule::downloadHostIcon(const KUrl &url)
{
    // kDebug() << url;
    const QString iconFile = faviconsCacheDir() + QLatin1String("favicons/") + url.host() + QLatin1String(".png");
    if (!isIconOld(iconFile)) {
        // kDebug() << "not old -> doing nothing";
        return;
    }
    startDownload(url.host(), true, KUrl(url, QLatin1String("/favicon.ico")));
}

void FavIconsModule::forceDownloadHostIcon(const KUrl &url)
{
    // kDebug() << url;
    KUrl iconURL = KUrl(url, QLatin1String("/favicon.ico"));
    d->failedDownloads.removeAll(iconURL); // force a download to happen
    startDownload(url.host(), true, iconURL);
}

void FavIconsModule::startDownload(const QString &hostOrURL, bool isHost, const KUrl &iconURL)
{
    if (d->failedDownloads.contains(iconURL)) {
        // kDebug() << iconURL << "already in failedDownloads, emitting error";
        emit error(isHost, hostOrURL, i18n("No favicon found"));
        return;
    }

    // kDebug() << iconURL;
    FavIconsDownloadInfo download;
    download.hostOrURL = hostOrURL;
    download.isHost = isHost;
    KIO::Job *job = KIO::get(iconURL, KIO::NoReload, KIO::HideProgressInfo);
    job->addMetaData(d->metaData);
    d->downloads.insert(job, download);
    connect(job, SIGNAL(infoMessage(KJob*,QString,QString)), SLOT(slotInfoMessage(KJob*,QString)));
    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(slotData(KIO::Job*,QByteArray)));
    connect(job, SIGNAL(result(KJob*)), SLOT(slotResult(KJob*)));
}

void FavIconsModule::slotData(KIO::Job *job, const QByteArray &data)
{
    KIO::TransferJob* tjob = static_cast<KIO::TransferJob*>(job);
    FavIconsDownloadInfo &download = d->downloads[job];
    unsigned int oldSize = download.iconData.size();
    // Size limit. Stop downloading if the file is huge.
    // Testcase (as of june 2008, at least): http://planet-soc.com/favicon.ico, 136K and strange format.
    if (oldSize > 500000U) {
        kWarning() << "Favicon too big, aborting download of" << tjob->url();
        const KUrl iconURL = tjob->url();
        d->failedDownloads.append(iconURL);
        d->downloads.remove(job);
        job->kill();
        return;
    }
    download.iconData.resize(oldSize + data.size());
    memcpy(download.iconData.data() + oldSize, data.data(), data.size());
}

void FavIconsModule::slotResult(KJob *job)
{
    KIO::TransferJob* tjob = static_cast<KIO::TransferJob*>(job);
    FavIconsDownloadInfo download = d->downloads[job];
    d->downloads.remove(job);
    const KUrl iconURL = tjob->url();
    QString iconName;
    QString errorMessage;
    if (!job->error()) {
        QBuffer buffer(&download.iconData);
        buffer.open(QIODevice::ReadOnly);
        QImageReader ir(&buffer);
        QSize desired(16,16);
        if (ir.canRead()) {
            ir.setScaledSize(desired);
            const QImage img = ir.read();
            if (!img.isNull()) {
                iconName = makeIconName(download, iconURL);
                const QString localPath = faviconsCacheDir() + iconName + QLatin1String(".png");
                if (!img.save(localPath, "PNG")) {
                    iconName.clear();
                    errorMessage = i18n("Error saving image to %1", localPath);
                    kWarning() << "Error saving image to" << localPath;
                } else {
                    if (!download.isHost) {
                        d->config->group(QString()).writeEntry(removeSlash(download.hostOrURL), iconURL.url());
                    }
                }
            } else {
                errorMessage = i18n("Image reader returned null image");
                kWarning() << "Image reader returned null image" << ir.errorString();
            }
        } else {
            errorMessage = i18n("Image reader cannot read the data");
            kWarning() << "Image reader cannot read the data" << ir.errorString();
        }
    } else {
        errorMessage = job->errorString();
        kWarning() << "Job error" << job->errorString();
    }
    if (iconName.isEmpty()) {
        // kDebug() << "adding" << iconURL << "to failed downloads";
        d->failedDownloads.append(iconURL);
        emit error(download.isHost, download.hostOrURL, errorMessage);
    } else {
        // kDebug() << "emit iconChanged" << download.isHost << download.hostOrURL << iconName;
        emit iconChanged(download.isHost, download.hostOrURL, iconName);
    }
}

void FavIconsModule::slotInfoMessage(KJob *job, const QString &msg)
{
    emit infoMessage(static_cast<KIO::TransferJob *>( job )->url().url(), msg);
}

#include "moc_favicons.cpp"
