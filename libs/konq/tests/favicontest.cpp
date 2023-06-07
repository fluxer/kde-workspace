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

#include "favicontest.h"

#include <qtest_kde.h>
#include <kio/job.h>
#include <kconfiggroup.h>
#include <kio/netaccess.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <QImageReader>
#include <QElapsedTimer>
#include <QEventLoop>

QTEST_KDEMAIN(FavIconTest, NoGUI)

// D-Bus and QEventLoop don't get along
// #define USE_EVENT_LOOP

static const char s_hostUrl[] = "https://www.google.com/";
static const char s_icoPath[] = KDESRCDIR "designer.ico";
static const int s_waitTime = 20000; // in ms

enum CheckStatus { Unknown, Yes, No };
CheckStatus s_networkAccess = CheckStatus::Unknown;
static bool checkNetworkAccess()
{
    if (s_networkAccess == CheckStatus::Unknown) {
        QElapsedTimer networkTimer;
        networkTimer.start();
        KIO::Job* job = KIO::get(KUrl(s_hostUrl), KIO::NoReload, KIO::HideProgressInfo);
        if (KIO::NetAccess::synchronousRun(job, nullptr)) {
            s_networkAccess = Yes;
            qDebug("Network access OK. Download time %lld", networkTimer.elapsed());
        } else {
            qWarning("%s", qPrintable(KIO::NetAccess::lastErrorString()));
            s_networkAccess = CheckStatus::No;
        }
    }
    return s_networkAccess == CheckStatus::Yes;
}

CheckStatus s_icoReadable = CheckStatus::Unknown;
static bool checkICOReadable()
{
    if (s_icoReadable == CheckStatus::Unknown) {
        QFile icofile(s_icoPath);
        icofile.open(QFile::ReadOnly);
        QImageReader icoimagereader(&icofile);
        if (icoimagereader.canRead()) {
            s_icoReadable = Yes;
            qDebug("ICO is readable");
        } else {
            qWarning("%s", qPrintable(icoimagereader.errorString()));
            s_icoReadable = CheckStatus::No;
        }
    }
    return s_icoReadable == CheckStatus::Yes;
}

static void cleanCache()
{
    QDir faviconsdir(KGlobal::dirs()->saveLocation("cache", QString::fromLatin1("favicons/")));
    // qDebug() << Q_FUNC_INFO << faviconsdir.absolutePath();
    foreach (const QFileInfo &favinfo, faviconsdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
        qDebug() << "Removing cached icon" << favinfo.filePath();
        QFile::remove(favinfo.filePath());
    }
}

FavIconTest::FavIconTest()
    : QObject(),
    m_iconChanged(false),
    m_favIconModule("org.kde.kded", "/modules/favicons", QDBusConnection::sessionBus())
{
    connect(
        &m_favIconModule, SIGNAL(iconChanged(QString,QString)),
        this, SLOT(slotIconChanged(QString,QString))
    );
}

void FavIconTest::initTestCase()
{
}

void FavIconTest::testIconForURL_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("icon");

    QTest::newRow("https://www.google.com")
        << QString::fromLatin1("https://www.google.com") << QString::fromLatin1("favicons/www.google.com");
    QTest::newRow("https://www.ibm.com/foo?bar=baz")
        << QString::fromLatin1("https://www.ibm.com") << QString::fromLatin1("favicons/www.ibm.com");
    QTest::newRow("https://www.wpoven.com/") // NOTE: favicon.png
        << QString::fromLatin1("https://www.wpoven.com/") << QString::fromLatin1("favicons/www.wpoven.com");
    QTest::newRow("https://140.82.121.3/")
        << QString::fromLatin1("https://140.82.121.3/") << QString::fromLatin1("favicons/140.82.121.3");
}

void FavIconTest::testIconForURL()
{
    QFETCH(QString, url);
    QFETCH(QString, icon);

    if (!checkICOReadable()) {
        QSKIP("ico not readable", SkipAll);
    }

    if (!checkNetworkAccess()) {
        QSKIP("no network access", SkipAll);
    }

    cleanCache();

    const KUrl favUrl(url);
    QString favicon = KMimeType::favIconForUrl(favUrl);
    QCOMPARE(favicon, QString());

#if USE_EVENT_LOOP
    QEventLoop eventLoop;
    connect(&m_favIconModule, SIGNAL(iconChanged(QString,QString)), &eventLoop, SLOT(quit()));
    QSignalSpy spy(&m_favIconModule, SIGNAL(iconChanged(QString,QString)));
    QVERIFY(spy.isValid());
    QCOMPARE(spy.count(), 0);
    m_favIconModule.downloadUrlIcon(url);
    qDebug("called downloadHostIcon, waiting");
    if (spy.count() < 1) {
        QTimer::singleShot(s_waitTime, &eventLoop, SLOT(quit()));
        eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }
    QCOMPARE(spy.count(), 1);
#else
    m_iconChanged = false;
    m_favIconModule.downloadUrlIcon(url);
    qDebug("called downloadHostIcon, waiting");
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    while (!m_iconChanged && elapsedTimer.elapsed() < s_waitTime) {
        QTest::qWait(400);
    }
    QVERIFY(m_iconChanged);
#endif

    favicon = KMimeType::favIconForUrl(favUrl);
    QCOMPARE(favicon, icon);
}

void FavIconTest::slotIconChanged(const QString &url, const QString &iconName)
{
    qDebug() << url << iconName;
    m_iconChanged = true;
    m_url = url;
    m_iconName = iconName;
}

#include "moc_favicontest.cpp"
