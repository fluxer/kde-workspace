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

#include "kded_kdirshare.h"

#include <QThread>
#include <QCoreApplication>
#include <klocale.h>
#include <kconfiggroup.h>
#include <knotification.h>
#include <kpluginfactory.h>
#include <kdebug.h>

class KDirShareThread : public QThread
{
    Q_OBJECT
public:
    KDirShareThread(QObject *parent = nullptr);
    ~KDirShareThread();

    QString serve(const QString &dirpath, const quint16 portmin, const quint16 portmax);
    QString directory() const;
    quint16 portMin() const;
    quint16 portMax() const;

Q_SIGNALS:
    void unblock();
    void serveError(const QString &error);

private Q_SLOTS:
    void slotUnblock();
    void slotServeError(const QString &error);

protected:
    void run() final;

private:
    KDirShareImpl* m_kdirshareimpl;
    bool m_starting;
    QString m_directory;
    quint16 m_portmin;
    quint16 m_portmax;
    QString m_error;
};

KDirShareThread::KDirShareThread(QObject *parent)
    : QThread(parent),
    m_kdirshareimpl(new KDirShareImpl(this)),
    m_starting(false),
    m_portmin(0),
    m_portmax(0)
{
    connect(
        this, SIGNAL(unblock()),
        this, SLOT(slotUnblock())
    );
    connect(
        this, SIGNAL(serveError(QString)),
        this, SLOT(slotServeError(QString))
    );
}

KDirShareThread::~KDirShareThread()
{
    if (m_kdirshareimpl) {
        m_kdirshareimpl->stop();
        m_kdirshareimpl->deleteLater();
    }
}

QString KDirShareThread::directory() const
{
    return m_directory;
}

quint16 KDirShareThread::portMin() const
{
    return m_portmin;
}

quint16 KDirShareThread::portMax() const
{
    return m_portmax;
}

void KDirShareThread::run()
{
    if (!m_kdirshareimpl->setDirectory(m_directory)) {
        emit serveError(i18n("Directory does not exist: %1", m_directory));
        emit unblock();
        return;
    }
    if (!m_kdirshareimpl->serve(QHostAddress(QHostAddress::Any), m_portmin, m_portmax)) {
        emit serveError(i18n("Could not serve: %1", m_kdirshareimpl->errorString()));
        emit unblock();
        return;
    }
    if (!m_kdirshareimpl->publish()) {
        m_kdirshareimpl->stop();
        emit serveError(i18n("Could not publish service: %1", m_kdirshareimpl->publishError()));
        emit unblock();
        return;
    }
    emit unblock();
}

QString KDirShareThread::serve(const QString &dirpath, const quint16 portmin, const quint16 portmax)
{
    // qDebug() << Q_FUNC_INFO << dirpath << portmin << portmax;
    m_directory = dirpath;
    m_portmin = portmin;
    m_portmax = portmax;
    m_starting = true;
    m_error.clear();
    start();
    while (m_starting) {
        QCoreApplication::processEvents();
    }
    // qDebug() << Q_FUNC_INFO << m_error;
    return m_error;
}

void KDirShareThread::slotUnblock()
{
    m_starting = false;
}

void KDirShareThread::slotServeError(const QString &error)
{
    m_error = error;
}


static QByteArray getDirShareKey(const KDirShareThread *kdirsharethread)
{
    return kdirsharethread->directory().toLocal8Bit().toHex();
};

K_PLUGIN_FACTORY(KDirShareModuleFactory, registerPlugin<KDirShareModule>();)
K_EXPORT_PLUGIN(KDirShareModuleFactory("kdirshare"))

KDirShareModule::KDirShareModule(QObject *parent, const QList<QVariant>&)
    : KDEDModule(parent)
{
    bool shareerror = false;
    KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
    foreach (const QString &kdirsharekey, kdirshareconfig.groupList()) {
        // qDebug() << Q_FUNC_INFO << kdirsharekey;
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        const QString kdirsharedirpath = kdirsharegroup.readEntry("dirpath", QString());
        if (kdirsharedirpath.isEmpty()) {
            continue;
        }
        const uint kdirshareportmin = kdirsharegroup.readEntry("portmin", uint(s_kdirshareportmin));
        const uint kdirshareportmax = kdirsharegroup.readEntry("portmax", uint(s_kdirshareportmax));
        // qDebug() << Q_FUNC_INFO << kdirsharekey << kdirsharedirpath << kdirshareportmin << kdirshareportmax;
        const QString kdirshareerror = share(
            kdirsharedirpath,
            quint16(kdirshareportmin), quint16(kdirshareportmax)
        );
        if (!kdirshareerror.isEmpty()) {
            kWarning() << kdirshareerror;
            shareerror = true;
        }
    }

    if (shareerror) {
        KNotification *knotification = new KNotification("ShareError");
        knotification->setComponentData(KComponentData("kdirshare"));
        knotification->setTitle(i18n("Directory share"));
        knotification->setText(i18n("Unable to share one or more directories"));
        knotification->sendEvent();
    }
}

KDirShareModule::~KDirShareModule()
{
    KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
    foreach (const KDirShareThread *kdirsharethread, m_dirshares) {
        const QByteArray kdirsharekey = getDirShareKey(kdirsharethread);
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        // qDebug() << Q_FUNC_INFO << kdirsharekey << kdirsharethread->directory() << kdirsharethread->portMin() << kdirsharethread->portMax();
        kdirsharegroup.writeEntry("dirpath", kdirsharethread->directory());
        kdirsharegroup.writeEntry("portmin", uint(kdirsharethread->portMin()));
        kdirsharegroup.writeEntry("portmax", uint(kdirsharethread->portMax()));
    }
    qDeleteAll(m_dirshares);
    m_dirshares.clear();
}

QString KDirShareModule::share(const QString &dirpath, const uint portmin, const uint portmax)
{
    if (isShared(dirpath)) {
        const QString unshareerror = unshare(dirpath);
        if (!unshareerror.isEmpty()) {
            return unshareerror;
        }
    }

    KDirShareThread *kdirsharethread = new KDirShareThread(this);
    // qDebug() << Q_FUNC_INFO << dirpath << portmin << portmax;
    const QString serveerror = kdirsharethread->serve(dirpath, portmin, portmax);
    if (!serveerror.isEmpty()) {
        delete kdirsharethread;
        return serveerror;
    }
    m_dirshares.append(kdirsharethread);
    return QString();
}

QString KDirShareModule::unshare(const QString &dirpath)
{
    foreach (KDirShareThread *kdirsharethread, m_dirshares) {
        if (kdirsharethread->directory() == dirpath) {
            KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
            const QByteArray kdirsharekey = getDirShareKey(kdirsharethread);
            KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
            kdirsharegroup.writeEntry("dirpath", QString());
            kdirsharethread->terminate();
            delete kdirsharethread;
            m_dirshares.removeAll(kdirsharethread);
            return QString();
        }
    }
    return i18n("Invalid directory share: %1", dirpath);
}

bool KDirShareModule::isShared(const QString &dirpath) const
{
    foreach (const KDirShareThread *kdirsharethread, m_dirshares) {
        if (kdirsharethread->directory() == dirpath) {
            return true;
        }
    }
    return false;
}

quint16 KDirShareModule::getPortMin(const QString &dirpath) const
{
    foreach (const KDirShareThread *kdirsharethread, m_dirshares) {
        if (kdirsharethread->directory() == dirpath) {
            return kdirsharethread->portMin();
        }
    }
    return s_kdirshareportmin;
}

quint16 KDirShareModule::getPortMax(const QString &dirpath) const
{
    foreach (const KDirShareThread *kdirsharethread, m_dirshares) {
        if (kdirsharethread->directory() == dirpath) {
            return kdirsharethread->portMax();
        }
    }
    return s_kdirshareportmax;
}

#include "kded_kdirshare.moc"
