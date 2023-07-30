/*  This file is part of the KDE libraries
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

#ifndef KCMLOCALE_H
#define KCMLOCALE_H

#include <KCModule>
#include <KMessageWidget>
#include <KComboBox>
#include <KLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

/**
 * @short A KCM to configure locale settings
 */
class KCMLocale : public KCModule
{
    Q_OBJECT
public:
    KCMLocale(QWidget *parent, const QVariantList &args);
    ~KCMLocale();

    void load() final;
    void save() final;
    void defaults() final;
    QString quickHelp() const final;

private Q_SLOTS:
    void slotLanguageChanged(const int index);
    void slotBinaryChanged(const int index);
    void slotMeasureChanged(const int index);
    void slotDateOrTimeChanged(const QString &text);

private:
    void loadLocaleSettings();

    QGridLayout* m_layout;
    KMessageWidget* m_messagewidget;
    QLabel* m_languagelabel;
    KComboBox* m_languagebox;
    QLabel* m_binarylabel;
    KComboBox* m_binarybox;
    QLabel* m_measurelabel;
    KComboBox* m_measurebox;
    QLabel* m_dateshortlabel;
    KLineEdit* m_dateshortedit;
    QLabel* m_datelonglabel;
    KLineEdit* m_datelongedit;
    QLabel* m_datenarrowlabel;
    KLineEdit* m_datenarrowedit;
    QLabel* m_timeshortlabel;
    KLineEdit* m_timeshortedit;
    QLabel* m_timelonglabel;
    KLineEdit* m_timelongedit;
    QLabel* m_timenarrowlabel;
    KLineEdit* m_timenarrowedit;
    QLabel* m_datetimeshortlabel;
    KLineEdit* m_datetimeshortedit;
    QLabel* m_datetimelonglabel;
    KLineEdit* m_datetimelongedit;
    QLabel* m_datetimenarrowlabel;
    KLineEdit* m_datetimenarrowedit;
    QSpacerItem* m_spacer;
};

#endif // KCMLOCALE_H
