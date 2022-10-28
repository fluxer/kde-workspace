/*  This file is part of the KDE project
    
    Copyright (C) 2007 John Tapsell <tapsell@kde.org>

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

#ifndef LSOFWIDGET_H_
#define LSOFWIDGET_H_

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtGui/QTreeWidget>
#include <kapplication.h>
#include <kdemacros.h>

struct KLsofWidgetPrivate;

class KDE_EXPORT KLsofWidget : public QTreeWidget
{
    Q_OBJECT
    Q_PROPERTY(qlonglong pid READ pid WRITE setPid)
public:
    KLsofWidget(QWidget *parent = NULL);
    ~KLsofWidget();

    bool update();

private Q_SLOTS:
    /* For QProcess *process */
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    qlonglong pid() const;
    void setPid(qlonglong pid);
private:
    KLsofWidgetPrivate* const d;
};

#endif // LSOFWIDGET_H_
