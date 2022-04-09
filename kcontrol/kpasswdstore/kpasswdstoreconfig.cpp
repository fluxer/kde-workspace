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

#include "kpasswdstoreconfig.h"

#include <kdebug.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

static const QByteArray kpasswdstore_cookie = QByteArray("user");
static const int kpasswdstore_passretries = 3;
static const qint64 kpasswdstore_passtimeout = 2; // minutes

K_PLUGIN_FACTORY(KCMPasswdStoreFactory, registerPlugin<KCMPasswdStore>();)
K_EXPORT_PLUGIN(KCMPasswdStoreFactory("kcmpasswdstoreconfig", "kcm_passwdstoreconfig"))

KCMPasswdStore::KCMPasswdStore(QWidget* parent, const QVariantList& args)
    : KCModule(KCMPasswdStoreFactory::componentData(), parent)
{
    Q_UNUSED(args);

    setQuickHelp(i18n("<h1>KPasswdStore</h1> This module allows you to change KDE password store preferences."));

    setupUi(this);

    KAboutData *about = new KAboutData(
        I18N_NOOP("KCMPasswdStore"), 0,
        ki18n("KDE Password Store Module"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("Copyright 2022, Ivailo Monev <email>xakepa10@gmail.com</email>")
    );
    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData(about);

    connect(cookiebox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotCookieChanged(QString)));
    connect(retriesinput, SIGNAL(valueChanged(int)), this, SLOT(slotRetriesChanged(int)));
    connect(timeoutinput, SIGNAL(valueChanged(int)), this, SLOT(slotTimeoutChanged(int)));

    load();
}

KCMPasswdStore::~KCMPasswdStore()
{
}

void KCMPasswdStore::load()
{
    KConfig kconfig("kpasswdstorerc", KConfig::SimpleConfig);
    KConfigGroup kconfiggroup = kconfig.group("KPasswdStore");
    const QByteArray cookietype = kconfiggroup.readEntry("Cookie", kpasswdstore_cookie).toLower();
    for (int i = 0; i < cookiebox->count(); i++) {
        if (cookiebox->itemText(i).toLower() == cookietype) {
            cookiebox->setCurrentIndex(i);
            break;
        }
    }
    retriesinput->setValue(kconfiggroup.readEntry("Retries", kpasswdstore_passretries));
    timeoutinput->setValue(kconfiggroup.readEntry("Timeout", kpasswdstore_passtimeout));
    emit changed(false);
}

void KCMPasswdStore::save()
{
    KConfig kconfig("kpasswdstorerc", KConfig::SimpleConfig);
    KConfigGroup kconfiggroup = kconfig.group("KPasswdStore");
    kconfiggroup.writeEntry("Cookie", cookiebox->currentText().toLatin1());
    kconfiggroup.writeEntry("Retries", retriesinput->value());
    kconfiggroup.writeEntry("Timeout", timeoutinput->value());
    emit changed(false);
}

void KCMPasswdStore::defaults()
{
    for (int i = 0; i < cookiebox->count(); i++) {
        if (cookiebox->itemText(i).toLower() == kpasswdstore_cookie) {
            cookiebox->setCurrentIndex(i);
            break;
        }
    }
    retriesinput->setValue(kpasswdstore_passretries);
    timeoutinput->setValue(kpasswdstore_passtimeout);
    emit changed(true);
}

void KCMPasswdStore::slotCookieChanged(const QString &style)
{
    Q_UNUSED(style);
    emit changed(true);
}

void KCMPasswdStore::slotRetriesChanged(const int retries)
{
    Q_UNUSED(retries);
    emit changed(true);
}

void KCMPasswdStore::slotTimeoutChanged(const int timeout)
{
    Q_UNUSED(timeout);
    emit changed(true);
}


#include "moc_kpasswdstoreconfig.cpp"
