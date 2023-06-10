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

#ifndef _FAVICONS_H_
#define _FAVICONS_H_

#include <kdedmodule.h>
#include <kjob.h>

class FavIconsModulePrivate;

/**
 * FavIconsModule implements a KDED Module that downloads shortcut icons for
 * URLs and stores them on disk for later use.
 */
class FavIconsModule : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.FavIcon")

public:
    FavIconsModule(QObject* parent, const QList<QVariant> &args);
    ~FavIconsModule();

public Q_SLOTS:
    /**
     * Looks up an icon name for a given URL. This function does not
     * initiate any download. If no icon for the URL or its host has
     * been downloaded yet, QString() is returned.
     *
     * @param url the URL for which the icon is queried
     * @return the icon name suitable to pass to @ref KIconLoader or
     *         QString() if no icon for this URL was found.
     */
    QString iconForUrl(const QString &url);

    /**
     * Downloads the icon for a given host if it was not downloaded before
     * or the download was too long ago. If the download finishes
     * successfully, the iconChanged() D-Bus signal is emitted.
     *
     * @param url any URL on the host for which the icon is to be downloaded
     */
    void downloadUrlIcon(const QString &url);

    /**
     * Downloads the icon for a given host, even if we tried very recently.
     * Not recommended in the general case; only useful for explicit "update favicon"
     * actions from the user.
     *
     * If the download finishes successfully, the iconChanged() D-Bus signal is emitted.
     *
     * @param url any URL on the host for which the icon is to be downloaded
     */
    void forceDownloadUrlIcon(const QString &url);

Q_SIGNALS: // D-Bus signals
    /**
     * Emitted once a new icon is available for a URL (after download request).
     * Empty @p iconName means an error occured. The @p url is same as the one
     * passed to @p downloadUrlIcon() or @p forceDownloadUrlIcon()
     */
    void iconChanged(QString url, QString iconName);

private Q_SLOTS:
    void slotFinished(KJob *kjob);

private:
    void startJob(const QString &url, const QString &faviconUrl, const QString &iconFile);
    void downloadSuccess(const QString &url);
    void downloadError(const QString &url, const QString &faviconUrl, const QString &iconFile);

private:
    FavIconsModulePrivate *d;
};

#endif // _FAVICONS_H_
