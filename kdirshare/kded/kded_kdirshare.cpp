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
#include "kdirshare.h"

#include <QThread>
#include <QCoreApplication>
#include <QTimer>
#include <klocale.h>
#include <kconfiggroup.h>
#include <knotification.h>
#include <kdirnotify.h>
#include <kpluginfactory.h>
#include <kdebug.h>

static quint16 getPort(const quint16 portmin, const quint16 portmax)
{
    if (portmin == portmax) {
        return portmax;
    }
    quint16 portnumber = 0;
    while (portnumber < portmin || portnumber > portmax) {
        portnumber = quint16(qrand());
    }
    return portnumber;
}

static QByteArray getDirShareKey(const QString &kdirsharedirpath)
{
    return kdirsharedirpath.toLocal8Bit().toHex();
};


class KDirShareThread : public QThread
{
    Q_OBJECT
public:
    KDirShareThread(QObject *parent = nullptr);
    ~KDirShareThread();

    QString serve(const QString &dirpath,
                  const quint16 portmin, const quint16 portmax,
                  const QString &username, const QString &password);
    QString directory() const;
    quint16 portMin() const;
    quint16 portMax() const;
    QString user() const;
    QString password() const;

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
    quint16 m_port;
    quint16 m_portmin;
    quint16 m_portmax;
    QString m_user;
    QString m_password;
    QString m_error;
};

KDirShareThread::KDirShareThread(QObject *parent)
    : QThread(parent),
    m_kdirshareimpl(new KDirShareImpl(this)),
    m_starting(false),
    m_port(0),
    m_portmin(s_kdirshareportmin),
    m_portmax(s_kdirshareportmax)
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

QString KDirShareThread::user() const
{
    return m_user;
}

QString KDirShareThread::password() const
{
    return m_password;
}

void KDirShareThread::run()
{
    if (!m_kdirshareimpl->setDirectory(m_directory)) {
        emit serveError(i18n("Directory does not exist: %1", m_directory));
        emit unblock();
        return;
    }
    if (!m_user.isEmpty() && !m_password.isEmpty()) {
        if (!m_kdirshareimpl->setAuthenticate(m_user.toUtf8(), m_password.toUtf8(), i18n("Not authorized"))) {
            emit serveError(i18n("Could not set authentication: %1", m_kdirshareimpl->errorString()));
            emit unblock();
            return;
        }
    }
    if (!m_kdirshareimpl->serve(QHostAddress(QHostAddress::Any), m_port)) {
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

QString KDirShareThread::serve(const QString &dirpath,
                               const quint16 portmin, const quint16 portmax,
                               const QString &username, const QString &password)
{
    // qDebug() << Q_FUNC_INFO << dirpath << port;
    m_directory = dirpath;
    m_port = getPort(portmin, portmax);
    m_portmin = portmin;
    m_portmax = portmax;
    m_user = username;
    m_password = password;
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


K_PLUGIN_FACTORY(KDirShareModuleFactory, registerPlugin<KDirShareModule>();)
K_EXPORT_PLUGIN(KDirShareModuleFactory("kdirshare"))

KDirShareModule::KDirShareModule(QObject *parent, const QList<QVariant>&)
    : KDEDModule(parent)
{
    m_passwdstore.setStoreID("KDirShare");
    // delayed restore without blocking
    QTimer::singleShot(2000, this, SLOT(slotDelayedRestore()));
}

KDirShareModule::~KDirShareModule()
{
    KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
    foreach (const KDirShareThread *kdirsharethread, m_dirshares) {
        const QByteArray kdirsharekey = getDirShareKey(kdirsharethread->directory());
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        // qDebug() << Q_FUNC_INFO << kdirsharekey << kdirsharethread->directory() << kdirsharethread->portMin() << kdirsharethread->portMax();
        kdirsharegroup.writeEntry("dirpath", kdirsharethread->directory());
        kdirsharegroup.writeEntry("portmin", uint(kdirsharethread->portMin()));
        kdirsharegroup.writeEntry("portmax", uint(kdirsharethread->portMax()));
        kdirsharegroup.writeEntry("user", kdirsharethread->user());
    }
    qDeleteAll(m_dirshares);
    m_dirshares.clear();
}

QString KDirShareModule::share(const QString &dirpath,
                               const uint portmin, const uint portmax,
                               const QString &username, const QString &password)
{
    if (isShared(dirpath)) {
        const QString unshareerror = unshare(dirpath);
        if (!unshareerror.isEmpty()) {
            return unshareerror;
        }
    }

    KDirShareThread *kdirsharethread = new KDirShareThread(this);
    // qDebug() << Q_FUNC_INFO << dirpath << portmin << portmax;
    const QString serveerror = kdirsharethread->serve(
        dirpath,
        portmin, portmax,
        username, password
    );
    if (!username.isEmpty() && !password.isEmpty()) {
        m_passwdstore.storePasswd(KPasswdStore::makeKey(dirpath), password);
    }
    if (!serveerror.isEmpty()) {
        delete kdirsharethread;
        return serveerror;
    }
    m_dirshares.append(kdirsharethread);
    org::kde::KDirNotify::emitFilesAdded(QString::fromLatin1("network:/"));
    return QString();
}

QString KDirShareModule::unshare(const QString &dirpath)
{
    foreach (KDirShareThread *kdirsharethread, m_dirshares) {
        if (kdirsharethread->directory() == dirpath) {
            KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
            const QByteArray kdirsharekey = getDirShareKey(kdirsharethread->directory());
            KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
            kdirsharegroup.writeEntry("dirpath", QString());
            kdirsharethread->terminate();
            delete kdirsharethread;
            m_dirshares.removeAll(kdirsharethread);
            org::kde::KDirNotify::emitFilesAdded(QString::fromLatin1("network:/")); // works the same as emitFilesRemoved()
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

QString KDirShareModule::getUser(const QString &dirpath) const
{
    foreach (const KDirShareThread *kdirsharethread, m_dirshares) {
        if (kdirsharethread->directory() == dirpath) {
            return kdirsharethread->user();
        }
    }
    return QString();
}

QString KDirShareModule::getPassword(const QString &dirpath) const
{
    foreach (const KDirShareThread *kdirsharethread, m_dirshares) {
        if (kdirsharethread->directory() == dirpath) {
            return kdirsharethread->password();
        }
    }
    return QString();
}

void KDirShareModule::slotDelayedRestore()
{
    bool requiresauth = false;
    KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
    foreach (const QString &kdirsharekey, kdirshareconfig.groupList()) {
        // qDebug() << Q_FUNC_INFO << kdirsharekey;
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        const QString kdirsharedirpath = kdirsharegroup.readEntry("dirpath", QString());
        if (kdirsharedirpath.isEmpty()) {
            continue;
        }
        const QString kdirshareuser = kdirsharegroup.readEntry("user", QString());
        if (!kdirshareuser.isEmpty()) {
            requiresauth = true;
            break;
        }
    }
    if (requiresauth) {
        if (!m_passwdstore.openStore()) {
            KNotification *knotification = new KNotification("AuthError");
            knotification->setComponentData(KComponentData("kdirshare"));
            knotification->setTitle(i18n("Directory share"));
            knotification->setText(i18n("Authorization is required but could not open password store"));
            knotification->sendEvent();
            return;
        }
    }

    bool shareerror = false;
    foreach (const QString &kdirsharekey, kdirshareconfig.groupList()) {
        // qDebug() << Q_FUNC_INFO << kdirsharekey;
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        const QString kdirsharedirpath = kdirsharegroup.readEntry("dirpath", QString());
        if (kdirsharedirpath.isEmpty()) {
            continue;
        }
        const uint kdirshareportmin = kdirsharegroup.readEntry("portmin", uint(s_kdirshareportmin));
        const uint kdirshareportmax = kdirsharegroup.readEntry("portmax", uint(s_kdirshareportmax));
        const QString kdirshareuser = kdirsharegroup.readEntry("user", QString());
        QString kdirsharepassword;
        if (!kdirshareuser.isEmpty()) {
            kdirsharepassword = m_passwdstore.getPasswd(KPasswdStore::makeKey(kdirsharedirpath));
        }
        // qDebug() << Q_FUNC_INFO << kdirsharekey << kdirsharedirpath << kdirshareportmin << kdirshareportmax;
        const QString kdirshareerror = share(
            kdirsharedirpath,
            quint16(kdirshareportmin), quint16(kdirshareportmax),
            kdirshareuser, kdirsharepassword
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

#include "kded_kdirshare.moc"
