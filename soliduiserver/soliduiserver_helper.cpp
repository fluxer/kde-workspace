/*  This file is part of the KDE libraries
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

#include <kauthhelpersupport.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <QProcess>
#include <QDir>
#include <QCoreApplication>

KAuth::ActionReply SolidUiServerHelper::cryptopen(QVariantMap parameters)
{
    if (!parameters.contains("device") || !parameters.contains("name") || !parameters.contains("password")) {
        return KAuth::ActionReply::HelperErrorReply;
    }

    const QString cryptbin = KStandardDirs::findRootExe("cryptsetup");
    if (cryptbin.isEmpty()) {
        KAuth::ActionReply errorReply(KAuth::ActionReply::HelperError);
        errorReply.setErrorDescription("cryptsetup is missing");
        errorReply.setErrorCode(1);
        return errorReply;
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
        return KAuth::ActionReply::SuccessReply;
    }
    QString crypterror = cryptproc.readAllStandardError();
    if (crypterror.isEmpty()) {
        crypterror = cryptproc.readAllStandardOutput();
    }
    KAuth::ActionReply errorReply(KAuth::ActionReply::HelperError);
    errorReply.setErrorDescription(crypterror);
    errorReply.setErrorCode(cryptproc.exitCode());
    return errorReply;
}

KAuth::ActionReply SolidUiServerHelper::cryptclose(QVariantMap parameters)
{
    if (!parameters.contains("name")) {
        return KAuth::ActionReply::HelperErrorReply;
    }

    const QString cryptbin = KStandardDirs::findRootExe("cryptsetup");
    if (cryptbin.isEmpty()) {
        KAuth::ActionReply errorReply(KAuth::ActionReply::HelperError);
        errorReply.setErrorDescription("cryptsetup is missing");
        errorReply.setErrorCode(1);
        return errorReply;
    }

    const QString name = parameters.value("name").toString();
    const QStringList cryptargs = QStringList() << "--batch-mode" << "close" << name;
    QProcess cryptproc;
    cryptproc.start(cryptbin, cryptargs);
    cryptproc.waitForStarted();
    cryptproc.waitForFinished();

    if (cryptproc.exitCode() == 0) {
        return KAuth::ActionReply::SuccessReply;
    }
    QString crypterror = cryptproc.readAllStandardError();
    if (crypterror.isEmpty()) {
        crypterror = cryptproc.readAllStandardOutput();
    }
    KAuth::ActionReply errorReply(KAuth::ActionReply::HelperError);
    errorReply.setErrorDescription(crypterror);
    errorReply.setErrorCode(cryptproc.exitCode());
    return errorReply;
}

KAuth::ActionReply SolidUiServerHelper::mount(QVariantMap parameters)
{
    if (!parameters.contains("device") || !parameters.contains("mountpoint")) {
        return KAuth::ActionReply::HelperErrorReply;
    }

    const QString device = parameters.value("device").toString();
    const QString mountpoint = parameters.value("mountpoint").toString();
    const QStringList mountargs = QStringList() << device << mountpoint;
    QProcess mountproc;
    mountproc.start("mount", mountargs);
    mountproc.waitForStarted();
    mountproc.waitForFinished();

    if (mountproc.exitCode() == 0) {
        return KAuth::ActionReply::SuccessReply;
    }
    QString mounterror = mountproc.readAllStandardError();
    if (mounterror.isEmpty()) {
        mounterror = mountproc.readAllStandardOutput();
    }
    KAuth::ActionReply errorReply(KAuth::ActionReply::HelperError);
    errorReply.setErrorDescription(mounterror);
    errorReply.setErrorCode(mountproc.exitCode());
    return errorReply;
}

KAuth::ActionReply SolidUiServerHelper::unmount(QVariantMap parameters)
{
    if (!parameters.contains("mountpoint")) {
        return KAuth::ActionReply::HelperErrorReply;
    }

    const QString mountpoint = parameters.value("mountpoint").toString();
    const QStringList umountargs = QStringList() << mountpoint;
    QProcess umountproc;
    umountproc.start("umount", umountargs);
    umountproc.waitForStarted();
    umountproc.waitForFinished();

    if (umountproc.exitCode() == 0) {
        return KAuth::ActionReply::SuccessReply;
    }
    QString umounterror = umountproc.readAllStandardError();
    if (umounterror.isEmpty()) {
        umounterror = umountproc.readAllStandardOutput();
    }
    KAuth::ActionReply errorReply(KAuth::ActionReply::HelperError);
    errorReply.setErrorDescription(umounterror);
    errorReply.setErrorCode(umountproc.exitCode());
    return errorReply;
}

KDE4_AUTH_HELPER_MAIN("org.kde.soliduiserver.mountunmounthelper", SolidUiServerHelper)
