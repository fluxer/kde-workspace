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

#ifndef KDEPASSWDSTORE_H
#define KDEPASSWDSTORE_H

#include <QComboBox>
#include <kcmodule.h>
#include <knuminput.h>
#include <klineedit.h>
#include <kpushbutton.h>

/**
 * Control KPasswdStore preferences of KDE applications
 *
 * @author Ivailo Monev (xakepa10@gmail.com)
 */
class KCMPasswdStore : public KCModule
{
    Q_OBJECT

public:
    KCMPasswdStore(QWidget* parent, const QVariantList&);
    ~KCMPasswdStore();

    // KCModule reimplementations
public Q_SLOTS:
    void load() final;
    void save() final;
    void defaults() final;

private Q_SLOTS:
    void slotCookieChanged(const QString &style);
    void slotRetriesChanged(const int retries);
    void slotTimeoutChanged(const int timeout);
    void slotStorePressed();

private:
    QComboBox* m_cookiebox;
    KIntNumInput* m_retriesinput;
    KIntNumInput* m_timeoutinput;
    QComboBox* m_storesbox;
    KLineEdit* m_useredit;
    KLineEdit* m_passedit;
    KPushButton* m_storebutton;
};

#endif // KDEPASSWDSTORE_H
