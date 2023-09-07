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

#ifndef KEYBOARDLAYOUTDIALOG_H
#define KEYBOARDLAYOUTDIALOG_H

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <kdialog.h>
#include <kkeyboardlayout.h>

class KCMKeyboardLayoutDialog : public KDialog
{
    Q_OBJECT
public:
    KCMKeyboardLayoutDialog(const QList<KKeyboardType> &filter, QWidget *parent);
    ~KCMKeyboardLayoutDialog();

    KKeyboardType keyboardType() const;
    void setKeyboardType(const KKeyboardType &layout);

private Q_SLOTS:
    void slotLayoutIndexChanged(const int index);
    void slotVariantIndexChanged(const int index);

private:
    bool filterLayout(const QByteArray &layout, const QByteArray &variant) const;

    QList<KKeyboardType> m_filter;
    KKeyboardType m_keyboardtype;
    QWidget* m_widget;
    QGridLayout* m_layout;
    QLabel* m_layoutslabel;
    QComboBox* m_layoutsbox;
    QLabel* m_variantslabel;
    QComboBox* m_variantsbox;
};

#endif // KEYBOARDLAYOUTDIALOG_H
