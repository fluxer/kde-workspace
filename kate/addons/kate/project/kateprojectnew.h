/*  This file is part of the Kate project.
 *
 *  Copyright (C) 2016 Ivailo Monev <xakepa10@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */ 

#ifndef KATEPROJECTNEW_H
#define KATEPROJECTNEW_H

#include <QWidget>
#include <QDialog>

QT_BEGIN_NAMESPACE
class Ui_KateProjectNew;
QT_END_NAMESPACE

class KateProjectNew: public QDialog
{
    Q_OBJECT
public:
    KateProjectNew(QWidget *parent);
    ~KateProjectNew();

    static QByteArray getPathType(QString path);

Q_SIGNALS:
    void projectCreated(QString path);

private Q_SLOTS:
    void slotPath(QString path);
    void slotOk();
    void slotCancel();

private:
    Ui_KateProjectNew *m_ui;
};

#endif // KATEPROJECTNEW_H
