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

#include <QDBusReply>
#include <QFileInfo>
#include <kvbox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

#include "kdirshareplugin.h"

static const quint16 s_kdirshareportmin = 1000;
static const quint16 s_kdirshareportmax = 32000;

K_PLUGIN_FACTORY(KDirSharePluginFactory, registerPlugin<KDirSharePlugin>();)
K_EXPORT_PLUGIN(KDirSharePluginFactory("kdirshareplugin"))

KDirSharePlugin::KDirSharePlugin(QObject *parent, const QList<QVariant> &args)
    : KPropertiesDialogPlugin(qobject_cast<KPropertiesDialog *>(parent)),
    m_kdirshareiface(QString::fromLatin1("org.kde.kded"), QString::fromLatin1("/modules/kdirshare"), QString::fromLatin1("org.kde.kdirshare"))
{
    m_url = properties->kurl().path(KUrl::RemoveTrailingSlash);
    if (m_url.isEmpty()) {
        return;
    }

    QFileInfo pathinfo(m_url);
    if (!pathinfo.permission(QFile::ReadUser) || !pathinfo.isDir()) {
        return;
    }

    KGlobal::locale()->insertCatalog("kdirshareplugin");

    KVBox *kvbox = new KVBox();
    properties->addPage(kvbox, i18n("&Share"));
    properties->setFileSharingPage(kvbox);

    QWidget *uiwidget = new QWidget(kvbox);
    m_ui.setupUi(uiwidget);

    if (m_kdirshareiface.isValid()) {
        QDBusReply<bool> kdirsharereply = m_kdirshareiface.call("isShared", m_url);
        if (!kdirsharereply.isValid()) {
            kWarning() << "Invalid kdirshare module reply for isShared()";
            m_ui.sharebox->setChecked(false);
            m_ui.portgroup->setEnabled(false);
        } else {
            m_ui.sharebox->setChecked(kdirsharereply.value());
            m_ui.portgroup->setEnabled(kdirsharereply.value());
        }

        QDBusReply<quint16> kdirsharereply2 = m_kdirshareiface.call("getPortMin", m_url);
        if (!kdirsharereply2.isValid()) {
            kWarning() << "Invalid kdirshare module reply for getPortMin()";
            m_ui.portmininput->setValue(s_kdirshareportmin);
        } else {
            m_ui.portmininput->setValue(kdirsharereply2.value());
        }
        kdirsharereply2 = m_kdirshareiface.call("getPortMax", m_url);
        if (!kdirsharereply2.isValid()) {
            kWarning() << "Invalid kdirshare module reply for getPortMax()";
            m_ui.portmaxinput->setValue(s_kdirshareportmax);
        } else {
            m_ui.portmaxinput->setValue(kdirsharereply2.value());
        }
        const bool randomport = (m_ui.portmininput->value() != m_ui.portmaxinput->value());
        m_ui.randombox->setChecked(randomport);
        m_ui.portmininput->setVisible(randomport);
    } else {
        kWarning() << "kdirshare module interface is not valid";
        m_ui.sharebox->setEnabled(false);
        m_ui.portgroup->setEnabled(false);
    }

    connect(m_ui.sharebox, SIGNAL(toggled(bool)), this, SLOT(slotShare(bool)));
    connect(m_ui.randombox, SIGNAL(toggled(bool)), this, SLOT(slotRandomPort(bool)));
    connect(m_ui.portmininput, SIGNAL(valueChanged(int)), this, SLOT(slotPortMin(int)));
    connect(m_ui.portmaxinput, SIGNAL(valueChanged(int)), this, SLOT(slotPortMax(int)));
}

KDirSharePlugin::~KDirSharePlugin()
{
}

void KDirSharePlugin::applyChanges()
{
    // qDebug() << Q_FUNC_INFO << m_ui.sharebox->isEnabled() << m_ui.sharebox->isChecked();
    if (m_ui.sharebox->isEnabled()) {
        QDBusReply<QString> kdirsharereply;
        if (m_ui.sharebox->isChecked()) {
            kdirsharereply = m_kdirshareiface.call("share",
                m_url,
                uint(m_ui.portmininput->value()), uint(m_ui.portmaxinput->value())
            );
        } else {
            kdirsharereply = m_kdirshareiface.call("unshare", m_url);
        }
        if (!kdirsharereply.isValid()) {
            KMessageBox::error(nullptr, i18n("Invalid kdirshare module reply"));
        } else {
            const QString kdirshareerror = kdirsharereply.value();
            if (!kdirshareerror.isEmpty()) {
                KMessageBox::error(nullptr, kdirshareerror);
            }
        }
    }
}

void KDirSharePlugin::slotShare(const bool value)
{
    // qDebug() << Q_FUNC_INFO << value;
    m_ui.portgroup->setEnabled(value);
    emit changed();
}

void KDirSharePlugin::slotRandomPort(const bool value)
{
    // qDebug() << Q_FUNC_INFO << value;
    m_ui.portmininput->setVisible(value);
    if (!value) {
        m_ui.portmininput->setValue(m_ui.portmaxinput->value());
    } else {
        m_ui.portmininput->setValue(s_kdirshareportmin);
    }
    emit changed();
}

void KDirSharePlugin::slotPortMin(const int value)
{
    // qDebug() << Q_FUNC_INFO << value;
    Q_UNUSED(value);
    emit changed();
}

void KDirSharePlugin::slotPortMax(const int value)
{
    // qDebug() << Q_FUNC_INFO << value;
    if (!m_ui.portmininput->isVisible()) {
        m_ui.portmininput->setValue(value);
    }
    emit changed();
}

#include "moc_kdirshareplugin.cpp"
