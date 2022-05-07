/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

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

#include "kfirewallconfig.h"

#include <QJsonDocument>
#include <QFile>
#include <QComboBox>
#include <QSpinBox>
#include <kstandarddirs.h>
#include <kuser.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kauthaction.h>
#include <kaboutdata.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kdebug.h>

#include <limits.h>

K_PLUGIN_FACTORY(KCMFirewallFactory, registerPlugin<KCMFirewall>();)
K_EXPORT_PLUGIN(KCMFirewallFactory("kcmfirewallconfig", "kcm_firewallconfig"))

KCMFirewall::KCMFirewall(QWidget* parent, const QVariantList& args)
    : KCModule(KCMFirewallFactory::componentData(), parent),
    m_kfirewallconfigpath(KStandardDirs::locateLocal("data", "kfirewall.json"))
{
    Q_UNUSED(args);

    setQuickHelp(i18n("<h1>KFirewall</h1> This module allows you to change KDE firewall options."));

    setupUi(this);

    const QString iptablesexe = KStandardDirs::findRootExe("iptables-restore");
    messagewidget->setMessageType(KMessageWidget::Error);
    messagewidget->setCloseButtonVisible(false);
    if (iptablesexe.isEmpty()) {
        rulestable->setEnabled(false);
        addbutton->setEnabled(false);
        removebutton->setEnabled(false);
        messagewidget->show();
        messagewidget->setText(i18n("iptables is not available"));
    } else {
        messagewidget->hide();
    }

    KAboutData *about = new KAboutData(
        I18N_NOOP("kcmkfirewall"), 0,
        ki18n("KDE Firewall Module"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("Copyright 2022, Ivailo Monev <email>xakepa10@gmail.com</email>")
    );
    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData(about);

    setNeedsAuthorization(true);

    load();

    connect(
        rulestable, SIGNAL(itemChanged(QTableWidgetItem*)),
        this, SLOT(slotItemChanged(QTableWidgetItem*))
    );
    connect(
        rulestable->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(slotSelectionChanged(QItemSelection,QItemSelection))
    );

    rulestable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

    connect(
        addbutton, SIGNAL(pressed()),
        this, SLOT(slotAddRule())
    );
    connect(
        removebutton, SIGNAL(pressed()),
        this, SLOT(slotRemoveRule())
    );
}

KCMFirewall::~KCMFirewall()
{
}

void KCMFirewall::load()
{
    rulestable->clearContents();
    rulestable->setRowCount(0);

    QFile kfirewallfile(m_kfirewallconfigpath);
    if (!kfirewallfile.open(QFile::ReadOnly)) {
        // may not exist yet
        kDebug() << "Could not open config for reading" << kfirewallfile.errorString();
        emit changed(false);
        return;
    }

    const QByteArray kfirewalljsondata = kfirewallfile.readAll();
    QJsonDocument kfirewalljsondocument = QJsonDocument::fromJson(kfirewalljsondata);
    if (!kfirewalljsondata.isEmpty() && kfirewalljsondocument.isNull()) {
        KMessageBox::error(this, i18n("Could create JSON document: %1", kfirewalljsondocument.errorString()));
        return;
    }
    const QVariantMap kfirewallsettingsmap = kfirewalljsondocument.toVariant().toMap();
    // qDebug() << Q_FUNC_INFO << kfirewallsettingsmap;

    int counter = 0;
    foreach(const QString &key, kfirewallsettingsmap.keys()) {
        addRuleRow();
        const QVariantMap rowsettingsmap = kfirewallsettingsmap.value(key).toMap();
        const QString trafficvalue = rowsettingsmap.value(QString::fromLatin1("traffic")).toString();
        const QString addressvalue = rowsettingsmap.value(QString::fromLatin1("address")).toString();
        const uint portvalue = rowsettingsmap.value(QString::fromLatin1("port")).toUInt();
        const QString actionvalue = rowsettingsmap.value(QString::fromLatin1("action")).toString();

        int trafficindex = 0;
        if (trafficvalue == QLatin1String("inbound")) {
            trafficindex = 0;
        } else if (trafficvalue == QLatin1String("outbound")) {
            trafficindex = 1;
        } else {
            kWarning() << "Invalid traffic value";
        }
        QComboBox* tabletrafficwidget = qobject_cast<QComboBox*>(rulestable->cellWidget(counter, 0));
        tabletrafficwidget->setCurrentIndex(trafficindex);

        QTableWidgetItem* tableaddresswidget = rulestable->item(counter, 1);
        tableaddresswidget->setText(addressvalue);

        if (portvalue > INT_MAX) {
            kWarning() << "Port value outside range" << portvalue;
        }
        QSpinBox* tableportwidget = qobject_cast<QSpinBox*>(rulestable->cellWidget(counter, 2));
        tableportwidget->setValue(portvalue);

        int actionindex = 0;
        if (actionvalue == QLatin1String("accept")) {
            actionindex = 0;
        } else if (actionvalue == QLatin1String("reject")) {
            actionindex = 1;
        } else {
            kWarning() << "Invalid action value" << actionvalue;
        }
        QComboBox* tableactionwidget = qobject_cast<QComboBox*>(rulestable->cellWidget(counter, 3));
        tableactionwidget->setCurrentIndex(actionindex);

        counter++;
    }

    emit changed(false);
}

void KCMFirewall::save()
{
    QVariantMap kfirewallsettingsmap;
    const QVariant uservalue = QVariant(KUser().loginName());
    for (int i = 0; i < rulestable->rowCount(); i++) {
        QVariant trafficvalue;
        const QComboBox* tabletrafficwidget = qobject_cast<QComboBox*>(rulestable->cellWidget(i, 0));
        switch (tabletrafficwidget->currentIndex()) {
            case 0: {
                trafficvalue = QVariant(QByteArray("inbound"));
                break;
            }
            case 1: {
                trafficvalue = QVariant(QByteArray("outbound"));
                break;
            }
            default: {
                Q_ASSERT(false);
                break;
            }
        }
        // qDebug() << Q_FUNC_INFO << trafficvalue;

        const QTableWidgetItem* tableaddresswidget = rulestable->item(i, 1);
        const QVariant addressvalue = QVariant(tableaddresswidget->text());
        // qDebug() << Q_FUNC_INFO << addressvalue;

        const QSpinBox* tableportwidget = qobject_cast<QSpinBox*>(rulestable->cellWidget(i, 2));
        const QVariant portvalue = QVariant(tableportwidget->value());
        // qDebug() << Q_FUNC_INFO << portvalue;

        if (addressvalue.toString().isEmpty() && tableportwidget->value() <= 0) {
            KMessageBox::error(this, i18n("Either address or port must be specified"));
            return;
        }

        QVariant actionvalue;
        const QComboBox* tableactionwidget = qobject_cast<QComboBox*>(rulestable->cellWidget(i, 3));
        switch (tableactionwidget->currentIndex()) {
            case 0: {
                actionvalue = QVariant(QByteArray("accept"));
                break;
            }
            case 1: {
                actionvalue = QVariant(QByteArray("reject"));
                break;
            }
            default: {
                Q_ASSERT(false);
                break;
            }
        }
        // qDebug() << Q_FUNC_INFO << actionvalue;

        QVariantMap rowsettingsmap;
        rowsettingsmap.insert(QString::fromLatin1("user"), uservalue);
        rowsettingsmap.insert(QString::fromLatin1("traffic"), trafficvalue);
        rowsettingsmap.insert(QString::fromLatin1("address"), addressvalue);
        rowsettingsmap.insert(QString::fromLatin1("port"), portvalue);
        rowsettingsmap.insert(QString::fromLatin1("action"), actionvalue);
        kfirewallsettingsmap.insert(QString::number(i), rowsettingsmap);
    }
    // qDebug() << Q_FUNC_INFO << kfirewallsettingsmap;

    QJsonDocument kfirewalljsondocument = QJsonDocument::fromVariant(kfirewallsettingsmap);
    if (!kfirewallsettingsmap.isEmpty() && kfirewalljsondocument.isNull()) {
        KMessageBox::error(this, i18n("Could create JSON document: %1", kfirewalljsondocument.errorString()));
        return;
    }
    const QByteArray kfirewalljsondata = kfirewalljsondocument.toJson();

    QFile kfirewallfile(m_kfirewallconfigpath);
    if (!kfirewallfile.open(QFile::WriteOnly)) {
        KMessageBox::error(this, i18n("Could not open config for writing: %1", kfirewallfile.errorString()));
        return;
    }
    if (kfirewallfile.write(kfirewalljsondata) != kfirewalljsondata.size()) {
        KMessageBox::error(this, i18n("Could not write config: %1", kfirewallfile.errorString()));
        return;
    }

    emit changed(false);
}

void KCMFirewall::defaults()
{
    rulestable->clearContents();
    rulestable->setRowCount(0);
    emit changed(true);
}

void KCMFirewall::addRuleRow()
{
    const int rulesrowcount = rulestable->rowCount();
    rulestable->setRowCount(rulesrowcount + 1);

    QComboBox* tabletrafficwidget = new QComboBox();
    tabletrafficwidget->addItem(i18n("Inbound"));
    tabletrafficwidget->addItem(i18n("Outbound"));
    connect(
        tabletrafficwidget, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotTrafficChanged(int))
    );
    rulestable->setCellWidget(rulesrowcount, 0, tabletrafficwidget);

    QTableWidgetItem* tableaddresswidget = new QTableWidgetItem();
    tableaddresswidget->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    rulestable->setItem(rulesrowcount, 1, tableaddresswidget);

    QSpinBox* tableportwidget = new QSpinBox();
    tableportwidget->setRange(0, INT_MAX);
    tableportwidget->setValue(0);
    connect(
        tableportwidget, SIGNAL(valueChanged(int)),
        this, SLOT(slotPortChanged(int))
    );
    rulestable->setCellWidget(rulesrowcount, 2, tableportwidget);

    QComboBox* tabletactionwidget = new QComboBox();
    tabletactionwidget->addItem(i18n("Accept"));
    tabletactionwidget->addItem(i18n("Reject"));
    connect(
        tabletactionwidget, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotActionChanged(int))
    );
    rulestable->setCellWidget(rulesrowcount, 3, tabletactionwidget);

    rulestable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
}

void KCMFirewall::slotAddRule()
{
    addRuleRow();
    emit changed(true);
}

void KCMFirewall::slotRemoveRule()
{
    QList<int> removedrows;
    foreach (const QModelIndex &modelindex, rulestable->selectionModel()->selectedIndexes()) {
        const int modelindexrow = modelindex.row();
        // qDebug() << Q_FUNC_INFO << modelindexrow;
        if (removedrows.contains(modelindexrow)) {
            continue;
        }
        rulestable->removeRow(modelindexrow);
        removedrows.append(modelindexrow);
    }
    emit changed(true);
}

void KCMFirewall::slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    removebutton->setEnabled(!selected.isEmpty());
}

void KCMFirewall::slotItemChanged(QTableWidgetItem* tablewidget)
{
    Q_UNUSED(tablewidget);
    emit changed(true);
}

void KCMFirewall::slotTrafficChanged(const int value)
{
    Q_UNUSED(value);
    emit changed(true);
}

void KCMFirewall::slotPortChanged(const int value)
{
    Q_UNUSED(value);
    emit changed(true);
}

void KCMFirewall::slotActionChanged(const int value)
{
    Q_UNUSED(value);
    emit changed(true);
}

#include "moc_kfirewallconfig.cpp"
