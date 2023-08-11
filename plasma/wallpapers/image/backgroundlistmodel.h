/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef BACKGROUNDLISTMODEL_H
#define BACKGROUNDLISTMODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include <QEventLoop>
#include <QThread>
#include <KDirWatch>
#include <KFileItem>
#include <Plasma/Wallpaper>

class KProgressDialog;

namespace Plasma
{
    class Package;
} // namespace Plasma

class Image;

class BackgroundListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    BackgroundListModel(Image *listener, QObject *parent);
    virtual ~BackgroundListModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Plasma::Package *package(int index) const;

    void reload(const QStringList &selected);
    void addBackground(const QString &path);
    QModelIndex indexOf(const QString &path) const;
    virtual bool contains(const QString &bg) const;

    void setWallpaperSize(const QSize& size);
    void setResizeMethod(Plasma::Wallpaper::ResizeMethod resizeMethod);

protected Q_SLOTS:
    void reload();
    void showPreview(const KFileItem &item, const QPixmap &preview);
    void previewFailed(const KFileItem &item);
    void processPaths(const QStringList &paths);

private:
    QSize bestSize(Plasma::Package *package) const;

    QWeakPointer<Image> m_structureParent;
    QList<Plasma::Package *> m_packages;
    mutable QHash<Plasma::Package *, QSize> m_sizeCache;
    QHash<Plasma::Package *, QPixmap> m_previews;
    QHash<KUrl, QPersistentModelIndex> m_previewJobs;
    KDirWatch m_dirwatch;

    QSize m_size;
    Plasma::Wallpaper::ResizeMethod m_resizeMethod;
    QPixmap m_previewUnavailablePix;
};

class BackgroundFinder : public QThread
{
    Q_OBJECT

public:
    BackgroundFinder(Plasma::Wallpaper *structureParent, const QStringList &p);
    ~BackgroundFinder();

    static QStringList suffixes();

signals:
    void backgroundsFound(const QStringList &paths);

protected:
    void run();

private:
    Plasma::PackageStructure::Ptr m_structure;
    QStringList m_paths;
    QString m_token;
};

#endif // BACKGROUNDLISTMODEL_H
