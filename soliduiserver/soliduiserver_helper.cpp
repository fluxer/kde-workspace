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

#include <QProcess>
#include <QDir>
#include <QCoreApplication>

#ifdef Q_OS_LINUX
#  include <sys/mount.h>
#  include <errno.h>
#endif

SolidUiServerHelper::SolidUiServerHelper(const char* const helper, QObject *parent)
    : KAuthorization(helper, parent)
{
}

int SolidUiServerHelper::cryptopen(const QVariantMap &parameters)
{
    if (!parameters.contains("device") || !parameters.contains("name") || !parameters.contains("password")) {
        return KAuthorization::HelperError;
    }

    const QString cryptbin = KStandardDirs::findRootExe("cryptsetup");
    if (cryptbin.isEmpty()) {
        kWarning() << "cryptsetup is missing";
        return KAuthorization::HelperError;
    }

    const QString device = parameters.value("device").toString();
    const QString name = parameters.value("name").toString();
    const QByteArray password = QByteArray::fromHex(parameters.value("password").toByteArray());
    const QStringList cryptargs = QStringList() << "--batch-mode" << "--key-file=-" << "open" << device << name;
    QProcess cryptproc;
    cryptproc.start(cryptbin, cryptargs);
    cryptproc.waitForStarted();
    cryptproc.write(password);
    cryptproc.closeWriteChannel();
    cryptproc.waitForFinished();

    if (cryptproc.exitCode() == 0) {
        return KAuthorization::NoError;
    }
    QString crypterror = cryptproc.readAllStandardError();
    if (crypterror.isEmpty()) {
        crypterror = cryptproc.readAllStandardOutput();
    }
    kWarning() << crypterror;
    return KAuthorization::HelperError;
}

int SolidUiServerHelper::cryptclose(const QVariantMap &parameters)
{
    if (!parameters.contains("name")) {
        return KAuthorization::HelperError;
    }

    const QString cryptbin = KStandardDirs::findRootExe("cryptsetup");
    if (cryptbin.isEmpty()) {
        kWarning() << "cryptsetup is missing";
        return KAuthorization::HelperError;
    }

    const QString name = parameters.value("name").toString();
    const QStringList cryptargs = QStringList() << "--batch-mode" << "close" << name;
    QProcess cryptproc;
    cryptproc.start(cryptbin, cryptargs);
    cryptproc.waitForStarted();
    cryptproc.waitForFinished();

    if (cryptproc.exitCode() == 0) {
        return KAuthorization::NoError;
    }
    QString crypterror = cryptproc.readAllStandardError();
    if (crypterror.isEmpty()) {
        crypterror = cryptproc.readAllStandardOutput();
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

    const QString device = parameters.value("device").toString();
    const QString mountpoint = parameters.value("mountpoint").toString();
    const bool readonly = parameters.value("readonly").toBool();
    // qDebug() << Q_FUNC_INFO << device << mountpoint << readonly;

    QStringList mountargs = QStringList() << device << mountpoint;
    if (readonly) {
#if defined(Q_OS_SOLARIS)
        mountargs << QString::fromLatin1("-oro");
#else
        mountargs << QString::fromLatin1("-r");
#endif
    }
    QProcess mountproc;
    mountproc.start("mount", mountargs);
    mountproc.waitForStarted();
    mountproc.waitForFinished();

    if (mountproc.exitCode() == 0) {
        return KAuthorization::NoError;
    }
    QString mounterror = mountproc.readAllStandardError();
    if (mounterror.isEmpty()) {
        mounterror = mountproc.readAllStandardOutput();
    }
    kWarning() << mounterror;
    return KAuthorization::HelperError;
}

int SolidUiServerHelper::unmount(const QVariantMap &parameters)
{
    if (!parameters.contains("mountpoint")) {
        return KAuthorization::HelperError;
    }

    const QString mountpoint = parameters.value("mountpoint").toString();

#ifdef Q_OS_LINUX
    const QByteArray mountpointbytes = mountpoint.toLocal8Bit();
    const int umountresult = ::umount2(mountpointbytes.constData(), 0);
    if (umountresult == 0) {
        return KAuthorization::NoError;
    }
    const int savederrno = errno;
    kWarning() << qt_error_string(savederrno);
    return KAuthorization::HelperError;
#else
    const QStringList umountargs = QStringList() << mountpoint;
    QProcess umountproc;
    umountproc.start("umount", umountargs);
    umountproc.waitForStarted();
    umountproc.waitForFinished();

    if (umountproc.exitCode() == 0) {
        return KAuthorization::NoError;
    }
    QString umounterror = umountproc.readAllStandardError();
    if (umounterror.isEmpty()) {
        umounterror = umountproc.readAllStandardOutput();
    }
    kWarning() << umounterror;
    return KAuthorization::HelperError;
#endif // Q_OS_LINUX
}

K_AUTH_MAIN("org.kde.soliduiserver.mountunmounthelper", SolidUiServerHelper)
