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

#ifndef KEYBOARDCONFIG_H
#define KEYBOARDCONFIG_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QTreeWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <knuminput.h>
#include <kvbox.h>
#include <kcmodule.h>

/**
 * Control KDE keyboard
 *
 * @author Ivailo Monev (xakepa10@gmail.com)
 */
class KCMKeyboard : public KCModule
{
    Q_OBJECT
public:
    KCMKeyboard(QWidget *parent, const QVariantList &args);

    // KCModule reimplementations
public Q_SLOTS:
    void load() final;
    void save() final;
    void defaults() final;

private Q_SLOTS:
    void slotEmitChanged();
    void slotItemSelectionChanged();
    void slotAddPressed();
    void slotEditPressed();
    void slotRemovePressed();
    void slotUpPressed();
    void slotDownPressed();
    void slotAdvancedPressed();

private:
    QByteArray m_layoutmodel;
    QByteArray m_layoutoptions;
    QVBoxLayout* m_layout;

    QGroupBox* m_repeatgroup;
    QLabel* m_repeatdelaylabel;
    KIntNumInput* m_repeatdelayinput;
    QLabel* m_repeatratelabel;
    KIntNumInput* m_repeatrateinput;

    QGroupBox* m_layoutsgroup;
    QLabel* m_layoutsmodellabel;
    QComboBox* m_layoutsmodelbox;
    QTreeWidget* m_layoutstree;
    KHBox* m_layoutbuttonsbox;
    QPushButton* m_layoutsaddbutton;
    QPushButton* m_layoutseditbutton;
    QPushButton* m_layoutsremovebutton;
    QPushButton* m_layoutsupbutton;
    QPushButton* m_layoutsdownbutton;
    QSpacerItem* m_layoutsbuttonsspacer;
    QPushButton* m_layoutsavdancedbutton;
};

#endif // KEYBOARDCONFIG_H
