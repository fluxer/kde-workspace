/*
    KSysGuard, the KDE System Guard

    Copyright (c) 2006-2007 John Tapsell <tapsell@kde.org>

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

#ifndef _ReniceDlg_h_
#define _ReniceDlg_h_

#include <kdialog.h>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
class Ui_ReniceDlgUi;
QT_END_NAMESPACE

/**
 * This class creates and handles a simple dialog to change the scheduling
 * priority of a process.
 */
class ReniceDlg : public KDialog
{
    Q_OBJECT

public:
    /** Let the user specify the new priorities of the @p processes given, using the given current values.
     *  @p currentCpuSched The current Cpu Scheduler of the processes.  Set to -1 to they have different schedulers
     */
    ReniceDlg(QWidget* parent, const QStringList& processes, int currentCpuPrio, int currentCpuSched);
    int newCPUPriority;
    int newCPUSched;

public Q_SLOTS:
    void slotOk();
    void updateUi();
    void cpuSliderChanged(int value);
    void cpuSchedulerChanged(int value);
private:
    void setSliderRange();
    Ui_ReniceDlgUi *ui;
    QButtonGroup *cpuScheduler;
    int previous_cpuscheduler;
};

#endif
