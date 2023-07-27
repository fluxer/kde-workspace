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

#include "kcmlocale.h"

#include <KAboutData>
#include <KLocale>
#include <KConfigGroup>
#include <KMessageBox>
#include <KBuildSycocaProgressDialog>
#include <KGlobalSettings>
#include <KPluginFactory>
#include <KDebug>

K_PLUGIN_FACTORY(KCMLocaleFactory, registerPlugin<KCMLocale>();)
K_EXPORT_PLUGIN(KCMLocaleFactory("kcmlocale"))

static const Qt::Alignment s_labelsalignment = (Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
// NOTE: keep in sync with:
// kdelibs/kdecore/localization/klocale.cpp
static const QString s_defaultlanguage = KLocale::defaultLanguage();
static const int s_defaultbinary = static_cast<int>(KLocale::IECBinaryDialect);

KCMLocale::KCMLocale(QWidget *parent, const QVariantList &args)
    : KCModule(KCMLocaleFactory::componentData(), parent, args),
    m_layout(nullptr),
    m_languagelabel(nullptr),
    m_languagebox(nullptr),
    m_binarylabel(nullptr),
    m_binarybox(nullptr),
    m_measurelabel(nullptr),
    m_measurebox(nullptr),
    m_dateshortlabel(nullptr),
    m_dateshortedit(nullptr),
    m_datelonglabel(nullptr),
    m_datelongedit(nullptr),
    m_datenarrowlabel(nullptr),
    m_datenarrowedit(nullptr),
    m_timeshortlabel(nullptr),
    m_timeshortedit(nullptr),
    m_timelonglabel(nullptr),
    m_timelongedit(nullptr),
    m_timenarrowlabel(nullptr),
    m_timenarrowedit(nullptr),
    m_datetimeshortlabel(nullptr),
    m_datetimeshortedit(nullptr),
    m_datetimelonglabel(nullptr),
    m_datetimelongedit(nullptr),
    m_datetimenarrowlabel(nullptr),
    m_datetimenarrowedit(nullptr),
    m_spacer(nullptr)
{
    KAboutData *about = new KAboutData(
        "kcmlocale", 0, ki18n("Localization options for KDE applications"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("Copyright 2023 Ivailo Monev")
    );

    about->addAuthor(ki18n("Ivailo Monev"), ki18n("Maintainer"), "xakepa10@gmail.com");

    setAboutData(about);

    m_layout = new QGridLayout(this);

    m_languagelabel = new QLabel(this);
    m_languagelabel->setText(i18n("Language:"));
    m_layout->addWidget(m_languagelabel, 0, 0, s_labelsalignment);
    m_languagebox = new KComboBox(this);
    foreach (const QString &language, KLocale::allLanguagesList()) {
        QString languagelang;
        QString languagecntry;
        QString languagemod;
        QString languagechar;
        KLocale::splitLocale(language, languagelang, languagecntry, languagemod, languagechar);
        if (languagecntry.isEmpty()) {
            const QString languagetext = KGlobal::locale()->languageCodeToName(language);
            m_languagebox->addItem(languagetext, language);
        } else {
            const QString languagetext = QString::fromLatin1("%1 - %2").arg(
                KGlobal::locale()->languageCodeToName(languagelang),
                KGlobal::locale()->countryCodeToName(languagecntry)
            );
            m_languagebox->addItem(languagetext, language);
        }
    }
    const QString languagehelp = i18n(
        "<p>This is the list of languages KDE Workspace can use for converting "
        "date, time and numbers. Changing the language will also change the "
        "translations language, however each application can specify additional "
        "translation languages."
        "</p>"
    );
    m_languagebox->setToolTip(languagehelp);
    m_languagebox->setWhatsThis(languagehelp);
    connect(m_languagebox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLanguageChanged(int)));
    m_layout->addWidget(m_languagebox, 0, 1);

    m_binarylabel = new QLabel(this);
    m_binarylabel->setText(i18n("Byte size units:"));
    m_layout->addWidget(m_binarylabel, 1, 0, s_labelsalignment);
    m_binarybox = new KComboBox(this);
    Q_ASSERT(int(KLocale::LastBinaryDialect) == int(KLocale::MetricBinaryDialect));
    m_binarybox->addItem(
        i18nc("Unit of binary measurement", "IEC Units (KiB, MiB, etc)"),
        static_cast<int>(KLocale::IECBinaryDialect)
    );
    m_binarybox->addItem(
        i18nc("Unit of binary measurement", "JEDEC Units (KB, MB, etc)"),
        static_cast<int>(KLocale::JEDECBinaryDialect)
    );
    m_binarybox->addItem(
        i18nc("Unit of binary measurement", "Metric Units (kB, MB, etc)"),
        static_cast<int>(KLocale::MetricBinaryDialect)
    );
    const QString binaryhelp = i18n(
        "<p>This changes the units used by most KDE programs to display "
        "numbers counted in bytes. Traditionally \"kilobytes\" meant units "
        "of 1024, instead of the metric 1000, for most (but not all) byte "
        "sizes."
        "<ul>"
        "<li>To reduce confusion you can use the recently standardized IEC "
         "units which are always in multiples of 1024.</li>"
        "<li>You can also select metric, which is always in units of 1000.</li>"
        "<li>Selecting JEDEC restores the older-style units used in KDE 3.5 "
        "and some other operating systems.</li>"
        "</ul>"
        "</p>"
    );
    m_binarybox->setToolTip(binaryhelp);
    m_binarybox->setWhatsThis(binaryhelp);
    connect(m_binarybox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBinaryChanged(int)));
    m_layout->addWidget(m_binarybox, 1, 1);

    m_measurelabel = new QLabel(this);
    m_measurelabel->setText(i18n("Measurement system:"));
    m_layout->addWidget(m_measurelabel, 2, 0, s_labelsalignment);
    m_measurebox = new KComboBox(this);
    Q_ASSERT(int(QLocale::MetricSystem) == 0);
    Q_ASSERT(int(QLocale::UKSystem) == 2);
    m_measurebox->addItem(
        i18n("Metric System"),
        static_cast<int>(QLocale::MetricSystem)
    );
    m_measurebox->addItem(
        i18n("Imperial System"),
        static_cast<int>(QLocale::ImperialSystem)
    );
    m_measurebox->addItem(
        i18n("UK System"),
        static_cast<int>(QLocale::UKSystem)
    );
    const QString measurehelp = i18n("<p>Here you can define the measurement system to use.</p>");
    m_measurebox->setToolTip(measurehelp);
    m_measurebox->setWhatsThis(measurehelp);
    connect(m_measurebox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMeasureChanged(int)));
    m_layout->addWidget(m_measurebox, 2, 1);

    // TODO: tooltips and whatsthis
    // TODO: validate date and time format, user usually has no clue what to enter and invalid
    // format will result in all sorts of bad stuff
    int groupsalignment = 0;
    QGroupBox* dategroup = new QGroupBox(this);
    QFontMetrics groupsmetrics = QFontMetrics(dategroup->font());
    dategroup->setTitle(i18n("Date format"));
    QGridLayout* datelayout = new QGridLayout(dategroup);
    dategroup->setLayout(datelayout);
    m_dateshortlabel = new QLabel(dategroup);
    m_dateshortlabel->setText(i18n("Short date:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_dateshortlabel->text()));
    datelayout->addWidget(m_dateshortlabel, 0, 0, s_labelsalignment);
    m_dateshortedit = new KLineEdit(dategroup);
    connect(m_dateshortedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    datelayout->addWidget(m_dateshortedit, 0, 1);
    m_datelonglabel = new QLabel(dategroup);
    m_datelonglabel->setText(i18n("Long date:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_datelonglabel->text()));
    datelayout->addWidget(m_datelonglabel, 1, 0, s_labelsalignment);
    m_datelongedit = new KLineEdit(dategroup);
    connect(m_datelongedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    datelayout->addWidget(m_datelongedit, 1, 1);
    m_datenarrowlabel = new QLabel(dategroup);
    m_datenarrowlabel->setText(i18n("Narrow date:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_datenarrowlabel->text()));
    datelayout->addWidget(m_datenarrowlabel, 2, 0, s_labelsalignment);
    m_datenarrowedit = new KLineEdit(dategroup);
    connect(m_datenarrowedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    datelayout->addWidget(m_datenarrowedit, 2, 1);
    m_layout->addWidget(dategroup, 3, 0, 1, 2);

    QGroupBox* timegroup = new QGroupBox(this);
    timegroup->setTitle(i18n("Time format"));
    QGridLayout* timelayout = new QGridLayout(timegroup);
    timegroup->setLayout(timelayout);
    m_timeshortlabel = new QLabel(timegroup);
    m_timeshortlabel->setText(i18n("Short time:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_timeshortlabel->text()));
    timelayout->addWidget(m_timeshortlabel, 0, 0, s_labelsalignment);
    m_timeshortedit = new KLineEdit(timegroup);
    connect(m_timeshortedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    timelayout->addWidget(m_timeshortedit, 0, 1);
    m_timelonglabel = new QLabel(timegroup);
    m_timelonglabel->setText(i18n("Long time:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_timelonglabel->text()));
    timelayout->addWidget(m_timelonglabel, 1, 0, s_labelsalignment);
    m_timelongedit = new KLineEdit(timegroup);
    connect(m_timelongedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    timelayout->addWidget(m_timelongedit, 1, 1);
    m_timenarrowlabel = new QLabel(timegroup);
    m_timenarrowlabel->setText(i18n("Narrow time:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_timenarrowlabel->text()));
    timelayout->addWidget(m_timenarrowlabel, 2, 0, s_labelsalignment);
    m_timenarrowedit = new KLineEdit(timegroup);
    connect(m_timenarrowedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    timelayout->addWidget(m_timenarrowedit, 2, 1);
    m_layout->addWidget(timegroup, 4, 0, 1, 2);

    QGroupBox* datetimegroup = new QGroupBox(this);
    datetimegroup->setTitle(i18n("Date and time format"));
    QGridLayout* datetimelayout = new QGridLayout(datetimegroup);
    datetimegroup->setLayout(datetimelayout);
    m_datetimeshortlabel = new QLabel(datetimegroup);
    m_datetimeshortlabel->setText(i18n("Short date and time:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_datetimeshortlabel->text()));
    datetimelayout->addWidget(m_datetimeshortlabel, 0, 0, s_labelsalignment);
    m_datetimeshortedit = new KLineEdit(datetimegroup);
    connect(m_datetimeshortedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    datetimelayout->addWidget(m_datetimeshortedit, 0, 1);
    m_datetimelonglabel = new QLabel(datetimegroup);
    m_datetimelonglabel->setText(i18n("Long date and time:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_datetimelonglabel->text()));
    datetimelayout->addWidget(m_datetimelonglabel, 1, 0, s_labelsalignment);
    m_datetimelongedit = new KLineEdit(datetimegroup);
    connect(m_datetimelongedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    datetimelayout->addWidget(m_datetimelongedit, 1, 1);
    m_datetimenarrowlabel = new QLabel(datetimegroup);
    m_datetimenarrowlabel->setText(i18n("Narrow date and time:"));
    groupsalignment = qMax(groupsalignment, groupsmetrics.width(m_datetimenarrowlabel->text()));
    datetimelayout->addWidget(m_datetimenarrowlabel, 2, 0, s_labelsalignment);
    m_datetimenarrowedit = new KLineEdit(datetimegroup);
    connect(m_datetimenarrowedit, SIGNAL(textChanged(QString)), this, SLOT(slotDateOrTimeChanged(QString)));
    datetimelayout->addWidget(m_datetimenarrowedit, 2, 1);
    m_layout->addWidget(datetimegroup, 5, 0, 1, 2);

    datelayout->setColumnMinimumWidth(0, groupsalignment);
    timelayout->setColumnMinimumWidth(0, groupsalignment);
    datetimelayout->setColumnMinimumWidth(0, groupsalignment);

    m_spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout->addItem(m_spacer, 6, 1, 2);

    setLayout(m_layout);
}

KCMLocale::~KCMLocale()
{
}

void KCMLocale::load()
{
    KConfig localeconfig = KConfig("kdeglobals", KConfig::FullConfig);
    KConfigGroup localegroup = localeconfig.group("Locale");

    const QString localelanguage = localegroup.readEntry("Language", s_defaultlanguage);
    const int languageindex = m_languagebox->findData(localelanguage);
    if (languageindex >= 0) {
        m_languagebox->setCurrentIndex(languageindex);
        emit changed(true);
    } else {
        kWarning() << "Could not find the language index for" << localelanguage;
    }

    const int localebinary = localegroup.readEntry("BinaryUnitDialect", s_defaultbinary);
    const int binaryindex = m_binarybox->findData(localebinary);
    if (binaryindex >= 0) {
        m_binarybox->setCurrentIndex(binaryindex);
        emit changed(true);
    } else {
        kWarning() << "Could not find the binary index for" << localebinary;
    }

    // NOTE: KLocale defaults to what QLocale returns
    const QLocale locale(localelanguage);
    const int localemeasure = localegroup.readEntry("MeasurementSystem", int(locale.measurementSystem()));
    const int measureindex = m_measurebox->findData(localemeasure);
    if (measureindex >= 0) {
        m_measurebox->setCurrentIndex(measureindex);
        emit changed(true);
    } else {
        kWarning() << "Could not find the measure index for" << localemeasure;
    }

    const QString localedateshort = localegroup.readEntry("ShortDateFormat", locale.dateFormat(QLocale::ShortFormat));
    m_dateshortedit->setText(localedateshort);
    const QString localedatelong = localegroup.readEntry("LongDateFormat", locale.dateFormat(QLocale::LongFormat));
    m_datelongedit->setText(localedatelong);
    const QString localedatenarrow = localegroup.readEntry("NarrowDateFormat", locale.dateFormat(QLocale::NarrowFormat));
    m_datenarrowedit->setText(localedatenarrow);

    const QString localetimeshort = localegroup.readEntry("ShortTimeFormat", locale.timeFormat(QLocale::ShortFormat));
    m_timeshortedit->setText(localetimeshort);
    const QString localetimelong = localegroup.readEntry("LongTimeFormat", locale.timeFormat(QLocale::LongFormat));
    m_timelongedit->setText(localetimelong);
    const QString localetimenarrow = localegroup.readEntry("NarrowTimeFormat", locale.timeFormat(QLocale::NarrowFormat));
    m_timenarrowedit->setText(localetimenarrow);

    const QString localedatetimeshort = localegroup.readEntry("ShortDateTimeFormat", locale.dateTimeFormat(QLocale::ShortFormat));
    m_datetimeshortedit->setText(localedatetimeshort);
    const QString localedatetimelong = localegroup.readEntry("LongDateTimeFormat", locale.dateTimeFormat(QLocale::LongFormat));
    m_datetimelongedit->setText(localedatetimelong);
    const QString localedatetimenarrow = localegroup.readEntry("NarrowDateTimeFormat", locale.dateTimeFormat(QLocale::NarrowFormat));
    m_datetimenarrowedit->setText(localedatetimenarrow);

    emit changed(false);
}

void KCMLocale::save()
{
    KConfig localeconfig = KConfig("kdeglobals", KConfig::FullConfig);
    KConfigGroup localegroup = localeconfig.group("Locale");
    const QString localelanguage = m_languagebox->itemData(m_languagebox->currentIndex()).toString();
    localegroup.writeEntry("Language", localelanguage);
    const int localebinary = m_binarybox->itemData(m_binarybox->currentIndex()).toInt();
    localegroup.writeEntry("BinaryUnitDialect", localebinary);
    const int localemeasure = m_measurebox->itemData(m_measurebox->currentIndex()).toInt();
    localegroup.writeEntry("MeasurementSystem", localemeasure);
    // TODO: when the format is same as the default do not write it to the config (it may change)
    localegroup.writeEntry("ShortDateFormat", m_dateshortedit->text());
    localegroup.writeEntry("LongDateFormat", m_datelongedit->text());
    localegroup.writeEntry("NarrowDateFormat", m_datenarrowedit->text());
    localegroup.writeEntry("ShortTimeFormat", m_timeshortedit->text());
    localegroup.writeEntry("LongTimeFormat", m_timelongedit->text());
    localegroup.writeEntry("NarrowTimeFormat", m_timenarrowedit->text());
    localegroup.writeEntry("ShortDateTimeFormat", m_datetimeshortedit->text());
    localegroup.writeEntry("LongDateTimeFormat", m_datetimelongedit->text());
    localegroup.writeEntry("NarrowDateTimeFormat", m_datetimenarrowedit->text());
    localegroup.sync();
    emit changed(false);

    KMessageBox::information(this,
        i18n(
            "Changed language settings apply only to "
            "newly started applications.\nTo change the "
            "language of all programs, you will have to "
            "logout first."
        ),
        i18n("Applying Language Settings"),
        QLatin1String("LanguageChangesApplyOnlyToNewlyStartedPrograms")
    );

    KBuildSycocaProgressDialog::rebuildKSycoca(this);
    KGlobalSettings::self()->emitChange(KGlobalSettings::SettingsChanged, KGlobalSettings::SETTINGS_LOCALE);
}

void KCMLocale::defaults()
{
    const int languageindex = m_languagebox->findData(s_defaultlanguage);
    if (languageindex >= 0) {
        m_languagebox->setCurrentIndex(languageindex);
        emit changed(true);
    } else {
        kWarning() << "Could not find the language index for" << s_defaultlanguage;
    }

    const int binaryindex = m_binarybox->findData(s_defaultbinary);
    if (binaryindex >= 0) {
        m_binarybox->setCurrentIndex(binaryindex);
        emit changed(true);
    } else {
        kWarning() << "Could not find the binary index for" << s_defaultbinary;
    }

    loadLocaleSettings();
}

QString KCMLocale::quickHelp() const
{
    return i18n(
        "<h1>Country/Region & Language</h1>\n"
        "<p>Here you can set your localization settings such as language, "
        "numeric formats, date and time formats, etc.  Choosing a country "
        "will load a set of default formats which you can then change to "
        "your personal preferences.  These personal preferences will remain "
        "set even if you change the country.  The reset buttons allow you "
        "to easily see where you have personal settings and to restore "
        "those items to the country's default value.</p>"
    );
}

void KCMLocale::loadLocaleSettings()
{
    const QString localelanguage = m_languagebox->itemData(m_languagebox->currentIndex()).toString();
    const QLocale locale(localelanguage);
    m_measurebox->setCurrentIndex(int(locale.measurementSystem()));
    m_dateshortedit->setText(locale.dateFormat(QLocale::ShortFormat));
    m_datelongedit->setText(locale.dateFormat(QLocale::LongFormat));
    m_datenarrowedit->setText(locale.dateFormat(QLocale::NarrowFormat));
    m_timeshortedit->setText(locale.timeFormat(QLocale::ShortFormat));
    m_timelongedit->setText(locale.timeFormat(QLocale::LongFormat));
    m_timenarrowedit->setText(locale.timeFormat(QLocale::NarrowFormat));
    m_datetimeshortedit->setText(locale.dateTimeFormat(QLocale::ShortFormat));
    m_datetimelongedit->setText(locale.dateTimeFormat(QLocale::LongFormat));
    m_datetimenarrowedit->setText(locale.dateTimeFormat(QLocale::NarrowFormat));
}

void KCMLocale::slotLanguageChanged(const int index)
{
    Q_UNUSED(index);
    loadLocaleSettings();
    emit changed(true);
}

void KCMLocale::slotBinaryChanged(const int index)
{
    Q_UNUSED(index);
    emit changed(true);
}

void KCMLocale::slotMeasureChanged(const int index)
{
    Q_UNUSED(index);
    emit changed(true);
}

void KCMLocale::slotDateOrTimeChanged(const QString &text)
{
    Q_UNUSED(text);
    emit changed(true);
}

#include "moc_kcmlocale.cpp"
