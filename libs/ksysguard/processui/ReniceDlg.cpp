/*
    KSysGuard, the KDE System Guard

	Copyright (c) 1999 Chris Schlaeger <cs@kde.org>
	Copyright (c) 2007 John Tapsell <tapsell@kde.org>

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

#include <klocale.h>
#include <kdebug.h>

#include "moc_ReniceDlg.cpp"
#include <QListWidget>
#include <QButtonGroup>
#include "ui_ReniceDlgUi.h"
#include "processcore/process.h"

ReniceDlg::ReniceDlg(QWidget* parent, const QStringList& processes, int currentCpuPrio, int currentCpuSched)
    : KDialog( parent )
{
    setObjectName( "Renice Dialog" );
    setModal( true );
    setCaption( i18n("Set Priority") );
    setButtons( Ok | Cancel );
    previous_cpuscheduler = 0;

    connect( this, SIGNAL(okClicked()), SLOT(slotOk()) );

    QWidget *widget = new QWidget(this);
    setMainWidget(widget);
    ui = new Ui_ReniceDlgUi();
    ui->setupUi(widget);
    ui->listWidget->insertItems(0, processes);

    cpuScheduler = new QButtonGroup(this);
    cpuScheduler->addButton(ui->radioNormal, (int)KSysGuard::Process::Other);
#ifndef Q_OS_SOLARIS
    cpuScheduler->addButton(ui->radioBatch, (int)KSysGuard::Process::Batch);
#else
    cpuScheduler->addButton(ui->radioBatch, (int)KSysGuard::Process::Interactive);
    ui->radioBatch->setText( i18nc("Scheduler", "Interactive") );
#endif
    cpuScheduler->addButton(ui->radioFIFO, (int)KSysGuard::Process::Fifo);
    cpuScheduler->addButton(ui->radioRR, (int)KSysGuard::Process::RoundRobin);
    if(currentCpuSched >= 0) { //negative means none of these
        QAbstractButton *sched = cpuScheduler->button(currentCpuSched);
        if(sched) {
            sched->setChecked(true); //Check the current scheduler
            previous_cpuscheduler = currentCpuSched;
        }
    }
    cpuScheduler->setExclusive(true);

    setSliderRange(); //Update the slider ranges before trying to set their current values
    ui->sliderCPU->setValue(currentCpuPrio);

    ui->imgCPU->setPixmap( KIcon("cpu").pixmap(128, 128) );

    newCPUPriority = 40;

    connect(cpuScheduler, SIGNAL(buttonClicked(int)), this, SLOT(cpuSchedulerChanged(int)));
    connect(ui->sliderCPU, SIGNAL(valueChanged(int)), this, SLOT(cpuSliderChanged(int)));

    updateUi();
}

void ReniceDlg::cpuSchedulerChanged(int value) {
    if(value != previous_cpuscheduler) {
        if( (value == (int)KSysGuard::Process::Other || value == KSysGuard::Process::Batch) &&
            (previous_cpuscheduler == (int)KSysGuard::Process::Fifo || previous_cpuscheduler == (int)KSysGuard::Process::RoundRobin)) {
            int slider = -ui->sliderCPU->value() * 2 / 5 + 20;
            setSliderRange();
            ui->sliderCPU->setValue( slider );
        } else if( (previous_cpuscheduler == (int)KSysGuard::Process::Other || previous_cpuscheduler == KSysGuard::Process::Batch) &&
            (value == (int)KSysGuard::Process::Fifo || value == (int)KSysGuard::Process::RoundRobin)) {
            int slider = (-ui->sliderCPU->value() + 20) * 5 / 2;
            setSliderRange();
            ui->sliderCPU->setValue( slider );
        }
    }
    previous_cpuscheduler = value;
    updateUi();
}

void ReniceDlg::cpuSliderChanged(int value) {
    ui->sliderCPU->setToolTip(QString::number(value));
}

void ReniceDlg::updateUi() {
    bool cpuPrioEnabled = ( cpuScheduler->checkedId() != -1);

    ui->sliderCPU->setEnabled(cpuPrioEnabled);
    ui->lblCpuLow->setEnabled(cpuPrioEnabled);
    ui->lblCpuHigh->setEnabled(cpuPrioEnabled);

    setSliderRange();
    cpuSliderChanged(ui->sliderCPU->value());
}

void ReniceDlg::setSliderRange() {
    if(cpuScheduler->checkedId() == (int)KSysGuard::Process::Other || cpuScheduler->checkedId() == (int)KSysGuard::Process::Batch || cpuScheduler->checkedId() == (int)KSysGuard::Process::Interactive) {
        //The slider is setting the priority, so goes from 19 to -20.  We cannot actually do this with a slider, so instead we go from -19 to 20, and negate later
        if(ui->sliderCPU->value() > 20) {
            ui->sliderCPU->setValue(20);
        }
        ui->sliderCPU->setInvertedAppearance(true);
        ui->sliderCPU->setMinimum(-19);
        ui->sliderCPU->setMaximum(20);
        ui->sliderCPU->setTickInterval(5);
    } else {
        if(ui->sliderCPU->value() < 1) {
            ui->sliderCPU->setValue(1);
        }
        ui->sliderCPU->setInvertedAppearance(false);
        ui->sliderCPU->setMinimum(1);
        ui->sliderCPU->setMaximum(99);
        ui->sliderCPU->setTickInterval(12);
    }
}

void ReniceDlg::slotOk()
{
    newCPUPriority = ui->sliderCPU->value();
    newCPUSched = cpuScheduler->checkedId();
}
