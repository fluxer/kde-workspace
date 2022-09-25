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
        || !parameters.contains("fstype")) {
        return KAuthorization::HelperError;
    }

    const QString device = parameters.value("device").toString();
    const QString mountpoint = parameters.value("mountpoint").toString();
    const QString fstype = parameters.value("fstype").toString();
    // qDebug() << Q_FUNC_INFO << device << mountpoint << fstype;

#ifdef Q_OS_LINUX
    // NOTE: if the filesystem type is not listed in /proc/filesystems then mount() will fail
    bool isknownfs = false;
    const QByteArray fstypebytes = fstype.toLocal8Bit();
    QFile filesystemsfile(QString::fromLatin1("/proc/filesystems"));
    if (filesystemsfile.open(QFile::ReadOnly)) {
        const QByteArray filesystemmatch = QByteArray(" ") + fstypebytes;
        while (!filesystemsfile.atEnd()) {
            const QByteArray filesystemsline = filesystemsfile.readLine().trimmed();
            if (filesystemsline.endsWith(filesystemmatch)) {
                isknownfs = true;
                break;
            }
        }
    }
    if (isknownfs) {
        const QByteArray devicebytes = device.toLocal8Bit();
        const QByteArray mountpointbytes = mountpoint.toLocal8Bit();
        const int mountresult = ::mount(
            devicebytes.constData(), mountpointbytes.constData(), fstypebytes.constData(),
            0, NULL
        );
        if (mountresult == 0) {
            return KAuthorization::NoError;
        }
        const int savederrno = errno;
        kWarning() << qt_error_string(savederrno);
        return KAuthorization::HelperError;
    }
#endif // Q_OS_LINUX

    const QStringList mountargs = QStringList() << device << mountpoint;
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
    const QByteArray mountpointbytes = mountpoint.toLocal8Bit();;
    const int umountresult = ::umount2(mountpointbytes.constData(), MNT_DETACH);
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
