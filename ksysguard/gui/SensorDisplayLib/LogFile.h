/*
    KSysGuard, the KDE System Guard

	Copyright (c) 2001 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef LOGFILE_H
#define LOGFILE_H

#define MAXLINES 500

#include <QListWidget>
#include <QtXml/qdom.h>
#include <QtCore/qcoreevent.h>

#include <SensorDisplay.h>

QT_BEGIN_NAMESPACE
class Ui_LogFileSettings;
QT_END_NAMESPACE

class LogFile : public KSGRD::SensorDisplay
{
	Q_OBJECT
public:
	LogFile(QWidget *parent, const QString& title, SharedSettings *workSheetSettings);
	~LogFile(void);

	bool addSensor(const QString& hostName, const QString& sensorName,
				   const QString& sensorType, const QString& sensorDescr);
	void answerReceived(int id, const QList<QByteArray>& answer);

	bool restoreSettings(QDomElement& element);
	bool saveSettings(QDomDocument& doc, QDomElement& element);

	void updateMonitor(void);

	void configureSettings(void);

	virtual void timerTick()
	{
		updateMonitor();
	}

	virtual bool hasSettingsDialog() const
	{
		return true;
	}

public Q_SLOTS:
	void applySettings();
	void applyStyle();

	void settingsAddRule();
	void settingsDeleteRule();
	void settingsChangeRule();
	void settingsRuleListSelected(int index);
	void settingsRuleTextChanged();

private:
	Ui_LogFileSettings* lfs;
	QListWidget* monitor;
	QStringList filterRules;

	unsigned long logFileID;
};

#endif // LOGFILE_H
