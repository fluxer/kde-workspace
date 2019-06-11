/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef _KDEBUGDIALOG
#define _KDEBUGDIALOG

#include <kcmodule.h>

#include "ui_kdebugconfig.h"

#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>

class KConfig;
class KPushButton;

/**
 * Control debug/warning/error/fatal output of KDE applications
 *
 * This dialog allows control of debugging output for all KDE apps.
 *
 * @author Kalle Dalheimer (kalle@kde.org)
 */
class KCMDebug : public KCModule, public Ui_KDebugDialog
{
    Q_OBJECT

public:
    KCMDebug( QWidget* parent, const QVariantList& );
    ~KCMDebug();

    virtual void load();
    virtual void save();

protected Q_SLOTS:
    void slotDisableAllChanged(const int);
    void slotDebugAreaChanged(QTreeWidgetItem*);
    void slotDestinationChanged();
    void slotAbortFatalChanged();
    void slotApply();

private:
    void showArea(const QString& areaName);
    void readAreas();

    QString mCurrentDebugArea;
    bool mLoaded; // hack to avoid saving before loading

    QMap<QString /*area name*/, QString /*description*/> mAreaMap;

protected:
    KConfig* pConfig;
};

#endif
