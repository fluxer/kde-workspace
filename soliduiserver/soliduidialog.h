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

#ifndef SOLIDUIDIALOG_H
#define SOLIDUIDIALOG_H

#include <QGridLayout>
#include <QLabel>
#include <kdialog.h>
#include <kserviceaction.h>
#include <kpixmapwidget.h>
#include <klistwidget.h>
#include <solid/device.h>

class SolidUiDialog : public KDialog
{
    Q_OBJECT
public:
    SolidUiDialog(const Solid::Device &soliddevce,
                  const QList<KServiceAction> &kserviceactions,
                  const bool mount,
                  QWidget *parent = nullptr);
    ~SolidUiDialog();

private Q_SLOTS:
    void slotItemSelectionChanged();
    void slotOkClicked();

private:
    Solid::Device m_soliddevice;
    QList<KServiceAction> m_serviceactions;
    bool m_mount;
    QWidget* m_mainwidget;
    QGridLayout* m_mainlayout;
    KPixmapWidget* m_devicepixmap;
    QLabel* m_devicelabel;
    KListWidget* m_listwidget;
};

#endif // SOLIDUIDIALOG_H
