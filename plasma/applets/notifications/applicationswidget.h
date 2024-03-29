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

#ifndef APPLICATIONSWIDGET_H
#define APPLICATIONSWIDGET_H

#include <QMutex>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <Plasma/Label>
#include <Plasma/Frame>
#include <Plasma/IconWidget>
#include <Plasma/PushButton>
#include <Plasma/DataEngine>

class NotificationsWidget;

class ApplicationFrame : public Plasma::Frame
{
    Q_OBJECT
public:
    explicit ApplicationFrame(const QString &name, QGraphicsWidget *parent);

    Plasma::IconWidget* iconwidget;
    Plasma::Label* label;
    Plasma::IconWidget* removewidget;
    Plasma::IconWidget* configurewidget;
    QString name;

    void animateRemove();
};


class ApplicationsWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    ApplicationsWidget(QGraphicsItem *parent, NotificationsWidget *notificationswidget);
    ~ApplicationsWidget();

    int count() const;

Q_SIGNALS:
    int countChanged();
    void ping();

public Q_SLOTS:
    void slotFrameDestroyed();
    void slotRemoveActivated();
    void slotConfigureActivated();
    void slotActionReleased();

private Q_SLOTS:
    void sourceAdded(const QString &name);
    void dataUpdated(const QString &name, const Plasma::DataEngine::Data &data);

private:
    QMutex m_mutex;
    NotificationsWidget *m_notificationswidget;
    QGraphicsLinearLayout* m_layout;
    Plasma::Label* m_label;
    QList<ApplicationFrame*> m_frames;
    Plasma::DataEngine *m_dataengine;
};

#endif // APPLICATIONSWIDGET_H
