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

#ifndef KGREETERCONFIG_H
#define KGREETERCONFIG_H

#include <QProcess>
#include <kcmodule.h>

#include "ui_kgreeterconfig.h"

/**
 * Control look of KDE greeter
 *
 * @author Ivailo Monev (xakepa10@gmail.com)
 */
class KCMGreeter : public KCModule, public Ui_KGreeterDialog
{
    Q_OBJECT
public:
    // KCModule reimplementations
    KCMGreeter(QWidget* parent, const QVariantList&);
    ~KCMGreeter();

public Q_SLOTS:
    void load() final;
    void save() final;
    void defaults() final;

private Q_SLOTS:
    void slotStyleChanged(const QString &style);
    void slotColorChanged(const QString &color);
    void slotURLChanged(const QString &url);
    void slotURLChanged(const KUrl &url);

    void slotTest();

private:
    void enableTest(const bool enable);
    void killLightDM();

    QString m_lightdmexe;
    Q_PID m_lightdmpid;
};

#endif // KGREETERCONFIG_H
