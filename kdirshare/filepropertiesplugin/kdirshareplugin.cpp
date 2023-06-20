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

#include "kdirshareplugin.h"
#include "kdirshare.h"

#include <QDBusReply>
#include <QFileInfo>
#include <kvbox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

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
            m_ui.authgroup->setEnabled(false);
            m_ui.serverlabel->setVisible(false);
        } else {
            m_ui.sharebox->setChecked(kdirsharereply.value());
            m_ui.portgroup->setEnabled(kdirsharereply.value());
            m_ui.authgroup->setEnabled(kdirsharereply.value());
            m_ui.serverlabel->setVisible(true);
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

        QDBusReply<QString> kdirsharereply3 = m_kdirshareiface.call("getUser", m_url);
        if (!kdirsharereply3.isValid()) {
            kWarning() << "Invalid kdirshare module reply for getUser()";
            m_ui.useredit->setText(QString());
        } else {
            m_ui.useredit->setText(kdirsharereply3.value());
        }
        if (!m_ui.useredit->text().isEmpty()) {
            kdirsharereply3 = m_kdirshareiface.call("getPassword", m_url);
            if (!kdirsharereply3.isValid()) {
                kWarning() << "Invalid kdirshare module reply for getPassword()";
                m_ui.passwordedit->setText(QString());
            } else {
                m_ui.passwordedit->setText(kdirsharereply3.value());
            }
        }
        if (!m_ui.useredit->text().isEmpty()) {
            m_ui.authbox->setChecked(true);
        }
        m_ui.useredit->setEnabled(m_ui.authbox->isChecked());
        m_ui.passwordedit->setEnabled(m_ui.authbox->isChecked());

        updateServerLabel();
    } else {
        kWarning() << "kdirshare module interface is not valid";
        m_ui.sharebox->setEnabled(false);
        m_ui.portgroup->setEnabled(false);
        m_ui.authgroup->setEnabled(false);
    }

    connect(m_ui.sharebox, SIGNAL(toggled(bool)), this, SLOT(slotShare(bool)));
    connect(m_ui.randombox, SIGNAL(toggled(bool)), this, SLOT(slotRandomPort(bool)));
    connect(m_ui.portmininput, SIGNAL(valueChanged(int)), this, SLOT(slotPortMin(int)));
    connect(m_ui.portmaxinput, SIGNAL(valueChanged(int)), this, SLOT(slotPortMax(int)));
    connect(m_ui.authbox, SIGNAL(toggled(bool)), this, SLOT(slotAuthorization(bool)));
    connect(m_ui.useredit, SIGNAL(textEdited(QString)), this, SLOT(slotUserEdited(QString)));
    connect(m_ui.passwordedit, SIGNAL(textEdited(QString)), this, SLOT(slotPasswordEdited(QString)));
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
            if (m_ui.authbox->isChecked() && (m_ui.useredit->text().isEmpty() || m_ui.passwordedit->text().isEmpty())) {
                KMessageBox::error(nullptr, i18n("User and password cannot be empty"));
                properties->abortApplying();
                return;
            }

            kdirsharereply = m_kdirshareiface.call("share",
                m_url,
                uint(m_ui.portmininput->value()), uint(m_ui.portmaxinput->value()),
                m_ui.useredit->text(), m_ui.passwordedit->text()
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
    m_ui.authgroup->setEnabled(value);
    updateServerLabel();
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

void KDirSharePlugin::slotAuthorization(const bool value)
{
    // qDebug() << Q_FUNC_INFO << value;
    m_ui.useredit->setEnabled(value);
    m_ui.passwordedit->setEnabled(value);
    if (!value) {
        m_ui.useredit->clear();
        m_ui.passwordedit->clear();
    }
    emit changed();
}

void KDirSharePlugin::slotUserEdited(const QString &value)
{
    // qDebug() << Q_FUNC_INFO << value;
    emit changed();
}

void KDirSharePlugin::slotPasswordEdited(const QString &value)
{
    // qDebug() << Q_FUNC_INFO << value;
    emit changed();
}

void KDirSharePlugin::updateServerLabel()
{
    QDBusReply<QString> kdirsharereply = m_kdirshareiface.call("getAddress", m_url);
    if (!kdirsharereply.isValid()) {
        kWarning() << "Invalid kdirshare module reply for getAddress()";
        m_ui.serverlabel->setText(QString());
    } else {
        const QString kdirshareaddress = kdirsharereply.value();
        m_ui.serverlabel->setText(i18n("<html>The directory can be accessed at <a href=\"%1\">%1</a>.</html>", kdirshareaddress));
    }
}

#include "moc_kdirshareplugin.cpp"
