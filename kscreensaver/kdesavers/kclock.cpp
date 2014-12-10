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

#include <QCheckBox>
#include <QColor>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

#include <KColorButton>
#include <KConfig>
#include <KGlobal>
#include <KHBox>
#include <KLocale>
#include <KMessageBox>

#include "kclock.h"
#include "kclock.moc"


const int COLOR_BUTTON_WIDTH = 80;
const int TIMER_INTERVAL = 100;
const int MAX_CLOCK_SIZE = 10;
const unsigned int DEFAULT_CLOCK_SIZE = 8;
const bool DEFAULT_KEEP_CENTERED = false;



class KClockSaverInterface : public KScreenSaverInterface {
public:
	virtual KAboutData *aboutData() {
		return new KAboutData("kclock.kss", "klock", ki18n("Clock"), "2.0", ki18n("Clock"));
	}

	virtual KScreenSaver *create(WId id) {
		return new KClockSaver(id);
	}

	virtual QDialog *setup() {
		return new KClockSetup();
	}
};


int main(int argc, char *argv[])
{
	KClockSaverInterface kss;
	return kScreenSaverMain(argc, argv, kss);
}


//-----------------------------------------------------------------------------


KClockSetup::KClockSetup(QWidget *parent) :
	KDialog(parent),
	_saver(0)
{
	setCaption(i18n("Setup Clock Screen Saver"));
	setModal(true);
	setButtons(Ok|Cancel|Help);
	setDefaultButton(Ok);

	readSettings();

	setButtonText(Help, i18n("A&bout"));
	QWidget *main = new QWidget(this);
	setMainWidget(main);

	QVBoxLayout *top = new QVBoxLayout(main);

	QHBoxLayout *hbox = new QHBoxLayout;
	top->addLayout(hbox);


	QGroupBox *colgroup = new QGroupBox(i18n("Colors"), main);
	QGridLayout *grid = new QGridLayout();

	QLabel *label;
	KColorButton *colorButton;

	label = new QLabel(i18n("&Hour-hand:"));
	colorButton = new KColorButton(_hourColor);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(QColor)),
			SLOT(slotHourColor(QColor)));
	grid->addWidget(label, 1, 1);
	grid->addWidget(colorButton, 1, 2);

	label = new QLabel(i18n("&Minute-hand:"));
	colorButton = new KColorButton(_minColor);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(QColor)),
			SLOT(slotMinColor(QColor)));
	grid->addWidget(label, 2, 1);
	grid->addWidget(colorButton, 2, 2);

	label = new QLabel(i18n("&Second-hand:"));
	colorButton = new KColorButton(_secColor);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(QColor)),
			SLOT(slotSecColor(QColor)));
	grid->addWidget(label, 3, 1);
	grid->addWidget(colorButton, 3, 2);

	label = new QLabel(i18n("Scal&e:"));
	colorButton = new KColorButton(_scaleColor);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(QColor)),
			SLOT(slotScaleColor(QColor)));
	grid->addWidget(label, 4, 1);
	grid->addWidget(colorButton, 4, 2);

	label = new QLabel(i18n("&Background:"));
	colorButton = new KColorButton(_bgndColor);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(QColor)),
			SLOT(slotBgndColor(QColor)));
	grid->addWidget(label, 5, 1);
	grid->addWidget(colorButton, 5, 2);

	hbox->addWidget(colgroup);
	colgroup->setLayout(grid);


	QWidget *_preview = new QWidget(main);
	_preview->setFixedSize(220, 165);
	_preview->show();
	_saver = new KClockSaver(_preview->winId());
	hbox->addWidget(_preview);

	label = new QLabel(i18n("Si&ze:"), main);
	top->addWidget(label);
	QSlider *qs = new QSlider(Qt::Horizontal);
	label->setBuddy(qs);
	qs->setRange(0, MAX_CLOCK_SIZE);
	qs->setSliderPosition(_size);
	qs->setTickInterval(1);
	qs->setTickPosition(QSlider::TicksBelow);
	connect(qs, SIGNAL(valueChanged(int)), this, SLOT(slotSliderMoved(int)));
	top->addWidget(qs);

	KHBox *qsscale = new KHBox(main);
	label = new QLabel(i18n("Small"), qsscale);
	label->setAlignment(Qt::AlignLeading);
	label = new QLabel(i18n("Medium"), qsscale);
	label->setAlignment(Qt::AlignHCenter);
	label = new QLabel(i18n("Big"), qsscale);
	label->setAlignment(Qt::AlignTrailing);
	top->addWidget(qsscale);

	QCheckBox *keepCentered = new QCheckBox(i18n("&Keep clock centered"), main);
	keepCentered->setChecked(_keepCentered);
	connect(keepCentered, SIGNAL(stateChanged(int)), SLOT(slotKeepCenteredChanged(int)));
	top->addWidget(keepCentered);
	top->addStretch();

	connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
	connect(this, SIGNAL(helpClicked()), this, SLOT(slotHelp()));
}


KClockSetup::~KClockSetup()
{
	delete _saver;
}


void KClockSetup::readSettings()
{
	KConfigGroup settings(KGlobal::config(), "Settings");

	_keepCentered = settings.readEntry("KeepCentered", DEFAULT_KEEP_CENTERED);
	_size = settings.readEntry("Size", DEFAULT_CLOCK_SIZE);
	if (_size > MAX_CLOCK_SIZE)
		_size = MAX_CLOCK_SIZE;

	KConfigGroup colors(KGlobal::config(), "Colors");
	QColor c = Qt::black;
	_bgndColor = colors.readEntry("Background", c);

	c = Qt::white;
	_scaleColor = colors.readEntry("Scale", c);
	_hourColor = colors.readEntry("HourHand", c);
	_minColor = colors.readEntry("MinuteHand", c);

	c = Qt::red;
	_secColor = colors.readEntry("SecondHand", c);

	if (_saver) {
		_saver->setBgndColor(_bgndColor);
		_saver->setScaleColor(_scaleColor);
		_saver->setHourColor(_hourColor);
		_saver->setMinColor(_minColor);
		_saver->setSecColor(_secColor);
	}
}


void KClockSetup::slotOk()
{
	KConfigGroup settings(KGlobal::config(), "Settings");
	settings.writeEntry("Size", _size);
	settings.writeEntry("KeepCentered", _keepCentered);
	settings.sync();

	KConfigGroup colors(KGlobal::config(), "Colors");
	colors.writeEntry("Background", _bgndColor);
	colors.writeEntry("Scale", _scaleColor);
	colors.writeEntry("HourHand", _hourColor);
	colors.writeEntry("MinuteHand", _minColor);
	colors.writeEntry("SecondHand", _secColor);
	colors.sync();
	accept();
}


void KClockSetup::slotHelp()
{
	KMessageBox::about(this, QLatin1String("<qt>") + i18n(
			"Clock Screen Saver<br>"
			"Version 2.0<br>"
			"<nobr>Melchior FRANZ (c) 2003, 2006, 2007</nobr>") +
			QLatin1String("<br><a href=\"mailto:mfranz@kde.org\">mfranz@kde.org</a>"
			"</qt>"), QString(), KMessageBox::AllowLink);
}


void KClockSetup::slotBgndColor(const QColor &color)
{
	_bgndColor = color;
	if (_saver)
		_saver->setBgndColor(_bgndColor);
}


void KClockSetup::slotScaleColor(const QColor &color)
{
	_scaleColor = color;
	if (_saver)
		_saver->setScaleColor(_scaleColor);
}


void KClockSetup::slotHourColor(const QColor &color)
{
	_hourColor = color;
	if (_saver)
		_saver->setHourColor(_hourColor);
}


void KClockSetup::slotMinColor(const QColor &color)
{
	_minColor = color;
	if (_saver)
		_saver->setMinColor(_minColor);
}


void KClockSetup::slotSecColor(const QColor &color)
{
	_secColor = color;
	if (_saver)
		_saver->setSecColor(_secColor);
}


void KClockSetup::slotSliderMoved(int v)
{
	if (_saver)
		_saver->resizeClock(_size = v);
}


void KClockSetup::slotKeepCenteredChanged(int c)
{
	if (_saver)
		_saver->setKeepCentered(_keepCentered = c);
}


//-----------------------------------------------------------------------------


ClockPainter::ClockPainter(QPaintDevice *device, int diameter) :
	QPainter(device)
{
	setRenderHint(QPainter::Antialiasing);
	translate(diameter / 2.0, diameter / 2.0);
	scale(diameter / 2000.0, -diameter / 2000.0);
	setPen(Qt::NoPen);
}


void ClockPainter::drawTick(double angle, double from, double to, double width, const QColor &color, bool shadow)
{
	save();
	rotate(90.0 - angle);

	if (shadow) {
		width += 1.0;
		setBrush(QColor(100, 100, 100));
	} else {
		setBrush(color);
	}
	drawRect(QRectF(from, -width / 2.0, to - from, width));
	restore();
}


void ClockPainter::drawDisc(double width, const QColor &color, bool shadow)
{
	if (shadow) {
		width += 1.0;
		setBrush(QColor(100, 100, 100));
	} else {
		setBrush(color);
	}
	drawEllipse(QRectF(-width, -width, 2.0 * width, 2.0 * width));
}


void ClockPainter::drawHand(double angle, double length, double width, const QColor &color, bool disc)
{
	if (disc)
		drawDisc(width * 1.3, color, true);
	drawTick(angle, 0.0, length, width, color, true);

	if (disc)
		drawDisc(width * 1.3, color, false);
	drawTick(angle, 0.0, length, width, color, false);

}


void ClockPainter::drawScale(const QColor &color)
{
	for (int i = 0; i < 360; i += 6)
		if (i % 30)
			drawTick(i, 920.0, 980.0, 15.0, color);
		else
			drawTick(i, 825.0, 980.0, 40.0, color);
}



//-----------------------------------------------------------------------------



KClockSaver::KClockSaver(WId id) :
	KScreenSaver(id),
	_timer(this),
	_xstep(1),
	_ystep(-1),
	_hour(-1),
	_minute(-1),
	_second(-1)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setMinimumSize(50, 50);
	readSettings();
	resizeClock(_size);

	QPalette p = palette();
	p.setColor(backgroundRole(), _bgndColor);
	setPalette(p);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
	show();
}


void KClockSaver::readSettings()
{
	KConfigGroup settings(KGlobal::config(), "Settings");
	_keepCentered = settings.readEntry("KeepCentered", DEFAULT_KEEP_CENTERED);
	_size = settings.readEntry("Size", DEFAULT_CLOCK_SIZE);
	if (_size > MAX_CLOCK_SIZE)
		_size = MAX_CLOCK_SIZE;

	KConfigGroup colors(KGlobal::config(), "Colors");
	QColor c = Qt::black;
	setBgndColor(colors.readEntry("Background", c));

	c = Qt::white;
	setScaleColor(colors.readEntry("Scale", c));
	setHourColor(colors.readEntry("HourHand", c));
	setMinColor(colors.readEntry("MinuteHand", c));

	c = Qt::red;
	setSecColor(colors.readEntry("SecondHand", c));
}


void KClockSaver::setKeepCentered(bool b)
{
	_keepCentered = b;
	if (b) {
		_x = (width() - _diameter) / 2;
		_y = (height() - _diameter) / 2;
	}
	update();
}


void KClockSaver::resizeClock(int size)
{
	_size = size;
	_diameter = qMin(width(), height()) * (_size + 4) / 14;
	_x = (width() - _diameter) / 2;
	_y = (height() - _diameter) / 2;
	update();
}


void KClockSaver::resizeEvent(QResizeEvent *)
{
	resizeClock(_size);
}


void KClockSaver::showEvent(QShowEvent *)
{
	_second = -1;
	slotTimeout();
	_timer.start(TIMER_INTERVAL);
}


void KClockSaver::hideEvent(QHideEvent *)
{
	_timer.stop();
}


void KClockSaver::slotTimeout()
{
	QTime t = QTime::currentTime();
	int s = t.second();
	if (s == _second)
		return;

	_second = _secColor != _bgndColor ? s : 0;
	_hour = t.hour();
	_minute = t.minute();

	if (!_keepCentered) {
		int i;
		_x += _xstep;
		if (_x <= 0)
			_x = 0, _xstep = 1;
		else if (_x >= (i = width() - _diameter))
			_x = i, _xstep = -1;

		_y += _ystep;
		if (_y <= 0)
			_y = 0, _ystep = 1;
		else if (_y >= (i = height() - _diameter))
			_y = i, _ystep = -1;
	}
	update();
}


void KClockSaver::paintEvent(QPaintEvent *)
{
	double hour_angle = _hour * 30.0 + _minute * .5 + _second / 120.0;
	double minute_angle = _minute * 6.0 + _second * .1;
	double second_angle = _second * 6.0;

	QImage clock(_diameter, _diameter, QImage::Format_RGB32);
	ClockPainter c(&clock, _diameter);
	c.fillRect(-1000, -1000, 2000, 2000, _bgndColor);

	if (_scaleColor != _bgndColor)
		c.drawScale(_scaleColor);
	if (_hourColor != _bgndColor)
		c.drawHand(hour_angle, 600.0, 55.0, _hourColor, false);
	if (_minColor != _bgndColor)
		c.drawHand(minute_angle, 900.0, 40.0, _minColor);
	if (_secColor != _bgndColor)
		c.drawHand(second_angle, 900.0, 30.0, _secColor);

	QPainter p(this);
	p.drawImage(_x, _y, clock);
	p.eraseRect(0, 0, _x, height());                                        // left ver
	p.eraseRect(_x + _diameter, 0, width(), height());                      // right ver
	p.eraseRect(_x, 0, _diameter, _y);                                      // top hor
	p.eraseRect(_x, _y + _diameter, _diameter, height() - _y - _diameter);  // bottom hor
}


