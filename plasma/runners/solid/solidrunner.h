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

#ifndef SOLIDRUNNER_H
#define SOLIDRUNNER_H

#include <QAction>
#include <Plasma/AbstractRunner>
#include <Solid/Device>

class SolidRunner : public Plasma::AbstractRunner
{
    Q_OBJECT
public:
    enum SolidMatchType {
        MatchDevice = 0,
        MatchMount = 1,
        MatchUnmount = 2,
        MatchEject = 3,
        MatchUnlock = 4,
        MatchLock = 5
    };

    SolidRunner(QObject *parent, const QVariantList &args);

    void match(Plasma::RunnerContext &context) final;
    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) final;

protected:
    QList<QAction*> actionsForMatch(const Plasma::QueryMatch &match) final;

private:
    QList<Solid::Device> solidDevices(const QString &term) const;
    void addDeviceMatch(const QString &term, Plasma::RunnerContext &context,
                        const Solid::Device &soliddevice, const SolidMatchType solidmatchtype);

    bool m_onlyremovable;
};

K_EXPORT_PLASMA_RUNNER(solid, SolidRunner)

#endif // SOLIDRUNNER_H
