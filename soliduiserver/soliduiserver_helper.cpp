/*  This file is part of the KDE project
    Copyright (C) 2021 Ivailo Monev <xakepa10@gmail.com>

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

#include "soliduiserver_helper.h"

#include <kstandarddirs.h>
#include <kdebug.h>

#include <QTimer>
#include <QDir>
#include <QCoreApplication>

static const int s_waittime = 3000;

SolidUiServerHelper::SolidUiServerHelper(const char* const helper, QObject *parent)
    : KAuthorization(helper, parent),
    m_process(nullptr),
    m_eventloop(nullptr)
 {
    m_process = new QProcess(this);
    m_eventloop = new QEventLoop(this);
    connect(
        m_process, SIGNAL(finished(int)),
        m_eventloop, SLOT(quit())
    );
}

SolidUiServerHelper::~SolidUiServerHelper()
{
    m_process->terminate();
    if (!m_process->waitForFinished(s_waittime)) {
        m_process->kill();
    }
}

int SolidUiServerHelper::cryptopen(const QVariantMap &parameters)
{
    if (!parameters.contains("device") || !parameters.contains("name") || !parameters.contains("password")) {
        return KAuthorization::HelperError;
    }

    m_cryptbin = KStandardDirs::findRootExe("cryptsetup");
    if (m_cryptbin.isEmpty()) {
        kWarning() << "cryptsetup is missing";
        return KAuthorization::HelperError;
    }

    m_parameters = parameters;
    QTimer::singleShot(500, this, SLOT(slotCryptOpen()));
    m_eventloop->exec();

    if (m_process->exitCode() == 0) {
        return KAuthorization::NoError;
    }
    QString crypterror = m_process->readAllStandardError();
    if (crypterror.isEmpty()) {
        crypterror = m_process->readAllStandardOutput();
    }
    kWarning() << crypterror;
    return KAuthorization::HelperError;
}

int SolidUiServerHelper::cryptclose(const QVariantMap &parameters)
{
    if (!parameters.contains("name")) {
        return KAuthorization::HelperError;
    }

    m_cryptbin = KStandardDirs::findRootExe("cryptsetup");
    if (m_cryptbin.isEmpty()) {
        kWarning() << "cryptsetup is missing";
        return KAuthorization::HelperError;
    }

    m_parameters = parameters;
    QTimer::singleShot(500, this, SLOT(slotCryptClose()));
    m_eventloop->exec();

    if (m_process->exitCode() == 0) {
        return KAuthorization::NoError;
    }
    QString crypterror = m_process->readAllStandardError();
    if (crypterror.isEmpty()) {
        crypterror = m_process->readAllStandardOutput();
    }
    kWarning() << crypterror;
    return KAuthorization::HelperError;
}

int SolidUiServerHelper::mount(const QVariantMap &parameters)
{
    if (!parameters.contains("device") || !parameters.contains("mountpoint")
        || !parameters.contains("readonly")) {
        return KAuthorization::HelperError;
    }

    m_parameters = parameters;
    QTimer::singleShot(500, this, SLOT(slotMount()));
    m_eventloop->exec();

    if (m_process->exitCode() == 0) {
        return KAuthorization::NoError;
    }
    QString mounterror = m_process->readAllStandardError();
    if (mounterror.isEmpty()) {
        mounterror = m_process->readAllStandardOutput();
    }
    kWarning() << mounterror;
    return KAuthorization::HelperError;
}

int SolidUiServerHelper::unmount(const QVariantMap &parameters)
{
    if (!parameters.contains("mountpoint")) {
        return KAuthorization::HelperError;
    }

    m_parameters = parameters;
    QTimer::singleShot(500, this, SLOT(slotUnmount()));
    m_eventloop->exec();

    if (m_process->exitCode() == 0) {
        return KAuthorization::NoError;
    }
    QString umounterror = m_process->readAllStandardError();
    if (umounterror.isEmpty()) {
        umounterror = m_process->readAllStandardOutput();
    }
    kWarning() << umounterror;
    return KAuthorization::HelperError;
}

void SolidUiServerHelper::slotCryptOpen()
{
    Q_ASSERT(!m_cryptbin.isEmpty());
    const QString device = m_parameters.value("device").toString();
    const QString name = m_parameters.value("name").toString();
    const QByteArray password = QByteArray::fromHex(m_parameters.value("password").toByteArray());
    const QStringList cryptargs = QStringList() << "--batch-mode" << "--key-file=-" << "open" << device << name;
    m_process->start(m_cryptbin, cryptargs);
    m_process->waitForStarted(s_waittime);
    m_process->write(password);
    m_process->closeWriteChannel();
}

void SolidUiServerHelper::slotCryptClose()
{
    Q_ASSERT(!m_cryptbin.isEmpty());
    const QString name = m_parameters.value("name").toString();
    const QStringList cryptargs = QStringList() << "--batch-mode" << "close" << name;
    m_process->start(m_cryptbin, cryptargs);
}

void SolidUiServerHelper::slotMount()
{
    const QString device = m_parameters.value("device").toString();
    const QString mountpoint = m_parameters.value("mountpoint").toString();
    const bool readonly = m_parameters.value("readonly").toBool();
    // qDebug() << Q_FUNC_INFO << device << mountpoint << readonly;

    QStringList mountargs = QStringList() << device << mountpoint;
    if (readonly) {
#if defined(Q_OS_SOLARIS)
        mountargs << QString::fromLatin1("-oro");
#else
        mountargs << QString::fromLatin1("-r");
#endif
    }
    m_process->start("mount", mountargs);
}

void SolidUiServerHelper::slotUnmount()
{
    const QString mountpoint = m_parameters.value("mountpoint").toString();
    const QStringList umountargs = QStringList() << mountpoint;
    m_process->start("umount", umountargs);
}

K_AUTH_MAIN("org.kde.soliduiserver.mountunmounthelper", SolidUiServerHelper)
