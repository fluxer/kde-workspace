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
            kWarning() << "Invalid kdirshare module reply";
            m_ui.sharebox->setChecked(false);
        } else {
            m_ui.sharebox->setChecked(kdirsharereply.value());
        }
    } else {
        kWarning() << "kdirshare module interface is not valid";
        m_ui.sharebox->setEnabled(false);
    }

    connect(m_ui.sharebox, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
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
            kdirsharereply = m_kdirshareiface.call("share", m_url);
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

#include "moc_kdirshareplugin.cpp"
