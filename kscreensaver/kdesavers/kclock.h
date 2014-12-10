// kclock - Clock screen saver for KDE
//
// Copyright (c) 2003, 2006, 2007, 2008 Melchior FRANZ <mfranz # kde : org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef KCLOCK_H
#define KCLOCK_H

#include <QTimer>
#include <QPainter>

#include <KDialog>
#include <kscreensaver.h>


class KClockSaver;


class KClockSetup : public KDialog {
	Q_OBJECT
public:
	KClockSetup(QWidget *parent = 0);
	~KClockSetup();

private slots:
	void slotOk();
	void slotHelp();

	void slotBgndColor(const QColor &);
	void slotScaleColor(const QColor &);
	void slotHourColor(const QColor &);
	void slotMinColor(const QColor &);
	void slotSecColor(const QColor &);
	void slotSliderMoved(int);
	void slotKeepCenteredChanged(int);

private:
	void readSettings();
	KClockSaver *_saver;

	QColor _bgndColor;
	QColor _scaleColor;
	QColor _hourColor;
	QColor _minColor;
	QColor _secColor;

	int _size;
	bool _keepCentered;
};



class ClockPainter : public QPainter {
public:
	ClockPainter(QPaintDevice *device, int diameter);
	void drawTick(double angle, double from, double to, double width, const QColor &, bool shadow = false);
	void drawDisc(double width, const QColor &, bool shadow = false);
	void drawHand(double angle, double length, double width, const QColor &, bool disc = true);
	void drawScale(const QColor &);
};



class KClockSaver : public KScreenSaver {
	Q_OBJECT
public:
	KClockSaver(WId id);
	inline void setBgndColor(const QColor &c) { _second = -1; setPalette(QPalette(_bgndColor = c)); }
	inline void setScaleColor(const QColor &c) { _second = -1; _scaleColor = c; }
	inline void setHourColor(const QColor &c) { _second = -1; _hourColor = c; }
	inline void setMinColor(const QColor &c) { _second = -1; _minColor = c; }
	inline void setSecColor(const QColor &c) { _second = -1; _secColor = c; }
	void setKeepCentered(bool b);
	void resizeClock(int size);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);

private slots:
	void slotTimeout();

private:
	void readSettings();

	QTimer _timer;
	QColor _bgndColor;
	QColor _scaleColor;
	QColor _hourColor;
	QColor _minColor;
	QColor _secColor;
	bool _keepCentered;
	int _size;

	int _x;
	int _y;
	int _xstep;
	int _ystep;
	int _diameter;
	int _hour;
	int _minute;
	int _second;
};

#endif
