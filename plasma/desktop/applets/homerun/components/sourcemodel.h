/*
Copyright 2012 Aurélien Gâteau <agateau@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SOURCEMODEL_H
#define SOURCEMODEL_H

// Local

// Qt
#include <QtCore/qabstractitemmodel.h>

// KDE
#include <KConfigGroup>

namespace Homerun
{
class AbstractSourceRegistry;
}

class SourceModelItem;

/**
 *
 */
class SourceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SourceModel(Homerun::AbstractSourceRegistry *registry, const KConfigGroup &tabGroup, QObject *parent);
    ~SourceModel();

    enum {
        SourceIdRole = Qt::UserRole + 1,
        ModelRole,
        ConfigGroupRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const; // reimp
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const; // reimp

    void reload();

    Q_INVOKABLE void appendSource(const QString &sourceId);
    Q_INVOKABLE void recreateModel(int row);
    Q_INVOKABLE void remove(int row);
    Q_INVOKABLE void move(int from, int to);

private:
    Homerun::AbstractSourceRegistry *m_sourceRegistry;
    KConfigGroup m_tabGroup;
    QList<SourceModelItem *> m_list;

    void writeSourcesEntry();
};

#endif /* SOURCEMODEL_H */
