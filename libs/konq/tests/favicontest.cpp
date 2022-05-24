/* This file is part of KDE
    Copyright (c) 2006 David Faure <faure@kde.org>

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

#include "favicontest.h"

#include <qtest_kde.h>
#include <kio/job.h>
#include <kconfiggroup.h>
#include <kio/netaccess.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <QElapsedTimer>
#include <QEventLoop>

QTEST_KDEMAIN(FavIconTest, NoGUI)

static const char s_hostUrl[] = "https://www.google.com/";
static const int s_waitTime = 20000; // in ms

enum NetworkAccess { Unknown, Yes, No } s_networkAccess = Unknown;
static bool checkNetworkAccess()
{
    if (s_networkAccess == Unknown) {
        QElapsedTimer networkTimer;
        networkTimer.start();
        KIO::Job* job = KIO::get(KUrl(s_hostUrl), KIO::NoReload, KIO::HideProgressInfo);
        if( KIO::NetAccess::synchronousRun(job, 0) ) {
            s_networkAccess = Yes;
            qDebug("Network access OK. Download time %d", networkTimer.elapsed());
        } else {
            qWarning("%s", qPrintable(KIO::NetAccess::lastErrorString()));
            s_networkAccess = No;
        }
    }
    return s_networkAccess == Yes;
}

static void cleanCache()
{
    QDir faviconsdir(KGlobal::dirs()->saveLocation("cache", QString::fromLatin1("favicons/")));
    // qDebug() << Q_FUNC_INFO << faviconsdir.absolutePath();
    foreach (const QFileInfo &favinfo, faviconsdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
        kWarning() << "Removing cached icon" << favinfo.filePath();
        QFile::remove(favinfo.filePath());
    }
}

FavIconTest::FavIconTest()
    : QObject(),
    m_favIconModule("org.kde.kded", "/modules/favicons", QDBusConnection::sessionBus())
{
}

void FavIconTest::initTestCase()
{
}

void FavIconTest::testSetIconForURL_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("icon");
    QTest::addColumn<QString>("result");

    QTest::newRow("https://www.google.com")
        << QString::fromLatin1("https://www.google.com") << QString::fromLatin1("https://www.google.com/favicon.ico")
        << QString::fromLatin1("favicons/www.google.com");
    QTest::newRow("https://www.ibm.com")
        << QString::fromLatin1("https://www.ibm.com") << QString::fromLatin1("https://www.ibm.com/favicon.ico")
        << QString::fromLatin1("favicons/www.ibm.com");
    QTest::newRow("https://github.com/")
        << QString::fromLatin1("https://github.com/") << QString::fromLatin1("https://github.com/favicon.ico")
        << QString::fromLatin1("favicons/github.com");
    QTest::newRow("https://140.82.121.3/")
        << QString::fromLatin1("https://140.82.121.3/") << QString::fromLatin1("https://140.82.121.3/favicon.ico")
        << QString::fromLatin1("favicons/lb-140-82-121-3-fra.github.com");
}

void FavIconTest::testSetIconForURL()
{
    QFETCH(QString, url);
    QFETCH(QString, icon);
    QFETCH(QString, result);

    if (!checkNetworkAccess()) {
        QSKIP("no network access", SkipAll);
    }

    cleanCache();

    QEventLoop eventLoop;
    // The call to connect() triggers qdbus initialization stuff, while QSignalSpy doesn't...
    connect(&m_favIconModule, SIGNAL(iconChanged(bool,QString,QString)), &eventLoop, SLOT(quit()));

    QSignalSpy spy(&m_favIconModule, SIGNAL(iconChanged(bool,QString,QString)));
    QVERIFY(spy.isValid());
    QCOMPARE(spy.count(), 0);

    m_favIconModule.setIconForUrl(url, icon);
    qDebug("called setIconForUrl, waiting");
    if (spy.count() < 1) {
        QTimer::singleShot(s_waitTime, &eventLoop, SLOT(quit()));
        eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toBool(), false);
    QCOMPARE(spy[0][1].toString(), url);
    QCOMPARE(spy[0][2].toString(), result);
}

void FavIconTest::testIconForURL_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("icon");

    QTest::newRow("https://www.google.com")
        << QString::fromLatin1("https://www.google.com") << QString::fromLatin1("favicons/www.google.com");
    QTest::newRow("https://www.ibm.com")
        << QString::fromLatin1("https://www.ibm.com") << QString::fromLatin1("favicons/www.ibm.com");
    QTest::newRow("https://github.com/")
        << QString::fromLatin1("https://github.com/") << QString::fromLatin1("favicons/github.com");
    QTest::newRow("https://140.82.121.3/")
        << QString::fromLatin1("https://140.82.121.3/") << QString::fromLatin1("favicons/140.82.121.3");
}

void FavIconTest::testIconForURL()
{
    QFETCH(QString, url);
    QFETCH(QString, icon);

    if (!checkNetworkAccess()) {
        QSKIP("no network access", SkipAll);
    }

    cleanCache();

    const KUrl favUrl(url);
    QString favicon = KMimeType::favIconForUrl(favUrl);
    QCOMPARE(favicon, QString());

    QEventLoop eventLoop;
    // The call to connect() triggers qdbus initialization stuff, while QSignalSpy doesn't...
    connect(&m_favIconModule, SIGNAL(iconChanged(bool,QString,QString)), &eventLoop, SLOT(quit()));

    QSignalSpy spy(&m_favIconModule, SIGNAL(iconChanged(bool,QString,QString)));
    QVERIFY(spy.isValid());
    QCOMPARE(spy.count(), 0);

    m_favIconModule.downloadHostIcon(url);
    qDebug("called downloadHostIcon, waiting");
    if (spy.count() < 1) {
        QTimer::singleShot(s_waitTime, &eventLoop, SLOT(quit()));
        eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }

    favicon = KMimeType::favIconForUrl(favUrl);
    QCOMPARE(favicon, icon);
}

#include "moc_favicontest.cpp"
