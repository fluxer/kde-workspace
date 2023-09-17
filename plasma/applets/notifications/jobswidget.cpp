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

#include "jobswidget.h"

#include <Plasma/DataEngineManager>
#include <KDebug>

JobsWidget::JobsWidget(QGraphicsItem *parent, NotificationsWidget *notificationswidget)
    : QGraphicsWidget(parent),
    m_notificationswidget(notificationswidget),
    m_layout(nullptr),
    m_label(nullptr),
    m_dataengine(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_label = new Plasma::Label(this);
    m_label->setText(i18n("No job notifications"));
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout->addItem(m_label);
    setLayout(m_layout);

    m_dataengine = Plasma::DataEngineManager::self()->loadEngine("applicationjobs");
    if (!m_dataengine) {
        kWarning() << "Could not load applicationjobs data engine";
        return;
    }
    connect(
        m_dataengine, SIGNAL(sourceAdded(QString)),
        this, SLOT(sourceAdded(QString))
    );
}

JobsWidget::~JobsWidget()
{
    if (m_dataengine) {
        Plasma::DataEngineManager::self()->unloadEngine("applicationjobs");
    }
}

int JobsWidget::count() const
{
    return m_frames.size();
}

void JobsWidget::sourceAdded(const QString &name)
{
    qDebug() << Q_FUNC_INFO << name;
    m_dataengine->connectSource(name, this);
}

void JobsWidget::dataUpdated(const QString &name, const Plasma::DataEngine::Data &data)
{
    // qDebug() << Q_FUNC_INFO << name << data;
}

#include "moc_jobswidget.cpp"
