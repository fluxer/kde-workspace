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

#ifndef KEYBOARDOPTIONSDIALOG_H
#define KEYBOARDOPTIONSDIALOG_H

#include <QGridLayout>
#include <QTreeWidget>
#include <kdialog.h>
#include <kmessagewidget.h>

class KCMKeyboardOptionsDialog : public KDialog
{
    Q_OBJECT
public:
    KCMKeyboardOptionsDialog(QWidget *parent);
    ~KCMKeyboardOptionsDialog();

    QByteArray options() const;
    void setOptions(const QByteArray &options);

private Q_SLOTS:
    void slotItemChanged(QTreeWidgetItem*,int);

private:
    QWidget* m_widget;
    QGridLayout* m_layout;
    KMessageWidget* m_optionswidget;
    QTreeWidget* m_optionstree;
    QList<QByteArray> m_options;
    QString m_enabledi18n;
    QString m_disabledi18n;
};

#endif // KEYBOARDOPTIONSDIALOG_H
