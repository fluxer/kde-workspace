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

#include <QGraphicsGridLayout>
#include <Plasma/DataEngineManager>
#include <KRun>
#include <KIconLoader>
#include <KIcon>
#include <KDebug>

JobFrame::JobFrame(const QString &_name, QGraphicsWidget *parent)
    : Plasma::Frame(parent),
    iconwidget(nullptr),
    label(nullptr),
    removewidget(nullptr),
    openwidget(nullptr),
    meter(nullptr),
    name(_name)
{
    JobsWidget* jobswidget = qobject_cast<JobsWidget*>(parent);

    setFrameShadow(Plasma::Frame::Sunken);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QGraphicsGridLayout* framelayout = new QGraphicsGridLayout(this);

    iconwidget = new Plasma::IconWidget(this);
    iconwidget->setAcceptHoverEvents(false);
    iconwidget->setAcceptedMouseButtons(Qt::NoButton);
    iconwidget->setIcon(KIcon("services"));
    const int desktopiconsize = KIconLoader::global()->currentSize(KIconLoader::Desktop);
    const QSizeF desktopiconsizef = QSizeF(desktopiconsize, desktopiconsize);
    iconwidget->setPreferredIconSize(desktopiconsizef);
    iconwidget->setMinimumSize(desktopiconsizef);
    iconwidget->setMaximumSize(desktopiconsizef);
    iconwidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    framelayout->addItem(iconwidget, 0, 0, 2, 1);

    label = new Plasma::Label(this);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    framelayout->addItem(label, 0, 1, 3, 1);

    const int smalliconsize = KIconLoader::global()->currentSize(KIconLoader::Small);
    removewidget = new Plasma::IconWidget(this);
    removewidget->setMaximumIconSize(QSize(smalliconsize, smalliconsize));
    removewidget->setIcon(KIcon("dialog-close"));
    removewidget->setToolTip(i18n("Click to remove this job notification."));
    removewidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    removewidget->setVisible(false);
    connect(
        removewidget, SIGNAL(activated()),
        jobswidget, SLOT(slotRemoveActivated())
    );
    framelayout->addItem(removewidget, 0, 2, 1, 1);

    openwidget = new Plasma::IconWidget(this);
    openwidget->setMaximumIconSize(QSize(smalliconsize, smalliconsize));
    openwidget->setIcon(KIcon("system-file-manager"));
    openwidget->setToolTip(i18n("Click to open the destination of the job."));
    openwidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    openwidget->setVisible(false);
    connect(
        openwidget, SIGNAL(activated()),
        jobswidget, SLOT(slotOpenActivated())
    );
    framelayout->addItem(openwidget, 1, 2, 1, 1);

    meter = new Plasma::Meter(this);
    meter->setMeterType(Plasma::Meter::BarMeterHorizontal);
    meter->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    meter->setMinimum(0);
    meter->setMaximum(100);
    meter->setVisible(false);
    framelayout->addItem(meter, 4, 0, 1, 3);

    setLayout(framelayout);
}


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
    // qDebug() << Q_FUNC_INFO << name;
    QMutexLocker locker(&m_mutex);
    JobFrame* frame = new JobFrame(name, this);
    m_frames.append(frame);
    m_label->setVisible(false);
    m_layout->insertItem(0, frame);
    adjustSize();

    emit countChanged();
    emit ping();
    locker.unlock();
    m_dataengine->connectSource(name, this);
}

void JobsWidget::dataUpdated(const QString &name, const Plasma::DataEngine::Data &data)
{
    // qDebug() << Q_FUNC_INFO << name << data;
#if 0
    // sample data
    "Job 1" QHash(
        ("totalUnit0", QVariant(QString, "bytes") )
        ( "processedUnit0" ,  QVariant(QString, "bytes") )
        ( "totalUnit1" ,  QVariant(QString, "files") )
        ( "processedUnit1" ,  QVariant(QString, "files") )
        ( "processedAmount0" ,  QVariant(qlonglong, 320864256) )
        ( "processedAmount1" ,  QVariant(qlonglong, 1) )
        ( "destUrl" ,  QVariant(QString, "/home/smil3y/Downloads/magnet%3A") )
        ( "killable" ,  QVariant(uint, 1) )
        ( "infoMessage" ,  QVariant(QString, "Копиране") )
        ( "percentage" ,  QVariant(uint, 95) )
        ( "appName" ,  QVariant(QString, "Dolphin") )
        ( "state" ,  QVariant(QString, "running") )
        ( "totalAmount0" ,  QVariant(qlonglong, 336900096) )
        ( "label0" ,  QVariant(QString, "/home/smil3y/Downloads/debian-11.5.0-arm64-netinst.iso") )
        ( "eta" ,  QVariant(qlonglong, 201) )
        ( "totalAmount1" ,  QVariant(qlonglong, 1) )
        ( "label1" ,  QVariant(QString, "/home/smil3y/Downloads/magnet%3A/debian-11.5.0-arm64-netinst.iso") )
        ( "appIconName" ,  QVariant(QString, "system-file-manager") )
        ( "suspendable" ,  QVariant(uint, 2) )
        ( "numericSpeed" ,  QVariant(qlonglong, 79691776) )
        ( "speed" ,  QVariant(QString, "76,0 MiB/s") )
        ( "labelName0" ,  QVariant(QString, "Източник") )
        ( "labelName1" ,  QVariant(QString, "Местоназначение") )
    )
#endif
    QMutexLocker locker(&m_mutex);
    foreach (JobFrame* frame, m_frames) {
        if (frame->name == name) {
            const QString infomessage = data.value("infoMessage").toString();
            frame->setText(infomessage);
            const QString appiconname = data.value("appIconName").toString();
            if (!appiconname.isEmpty()) {
                frame->iconwidget->setIcon(appiconname);
            }
            const QString labelname0 = data.value("labelName0").toString();
            const QString labelname1 = data.value("labelName1").toString();
            if (!labelname0.isEmpty() && !labelname1.isEmpty()) {
                frame->label->setText(
                    i18n(
                        "<p><b>%1:</b> <i>%2</i></p><p><b>%3:</b> <i>%4</i></p>",
                        labelname0, data.value("label0").toString(),
                        labelname1, data.value("label1").toString()
                    )
                );
            } else if (!labelname0.isEmpty()) {
                frame->label->setText(
                    i18n(
                        "<b>%1:</b> <i>%2</i>",
                        labelname0, data.value("label0").toString()
                    )
                );
            } else if (!labelname1.isEmpty()) {
                frame->label->setText(
                    i18n(
                        "<b>%1:</b> <i>%2</i>",
                        labelname1, data.value("label1").toString()
                    )
                );
            }
            const uint percentage = data.value("percentage").toUInt();
            if (percentage > 0) {
                frame->meter->setVisible(true);
                frame->meter->setValue(percentage);
            }
            const QByteArray state = data.value("state").toByteArray();
            if (state == "stopped") {
                frame->removewidget->setVisible(true);
                const QString desturl = data.value("destUrl").toString();
                if (!desturl.isEmpty()) {
                    frame->openwidget->setVisible(true);
                }
            }
            frame->adjustSize();
            adjustSize();
            locker.unlock();
            emit ping();
            break;
        }
    }
}

void JobsWidget::slotRemoveActivated()
{
    QMutexLocker locker(&m_mutex);
    const Plasma::IconWidget* removewidget = qobject_cast<Plasma::IconWidget*>(sender());
    JobFrame* jobframe = qobject_cast<JobFrame*>(removewidget->parentObject());
    Q_ASSERT(jobframe != nullptr);
    QMutableListIterator<JobFrame*> iter(m_frames);
    while (iter.hasNext()) {
        JobFrame* frame = iter.next();
        if (frame == jobframe) {
            m_layout->removeItem(frame);
            frame->deleteLater();
            iter.remove();
            break;
        }
    }
    m_label->setVisible(m_frames.size() <= 0);
    adjustSize();
    locker.unlock();
    emit countChanged();
}

void JobsWidget::slotOpenActivated()
{
    QMutexLocker locker(&m_mutex);
    const Plasma::IconWidget* openwidget = qobject_cast<Plasma::IconWidget*>(sender());
    const QString desturl = openwidget->property("_k_desturl").toString();
    locker.unlock();
    KRun::runUrl(KUrl(desturl), "inode/directory", nullptr);
}

#include "moc_jobswidget.cpp"
