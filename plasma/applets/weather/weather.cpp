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

#include "weather.h"

#include <QTimer>
#include <QJsonDocument>
#include <QGridLayout>
#include <QLabel>
#include <QGraphicsGridLayout>
#include <Plasma/Frame>
#include <Plasma/IconWidget>
#include <Plasma/ToolTipManager>
#include <KSystemTimeZones>
#include <KIO/StoredTransferJob>
#include <KIO/Job>
#include <KIcon>
#include <KDebug>

static const QSizeF s_minimumsize = QSizeF(290, 140);
// for reference:
// http://www.geoplugin.com/quickstart
// alternatively:
// https://location.services.mozilla.com/v1/geolocate?key=b9255e08-838f-4d4e-b270-bec2cc89719b
// https://location.services.mozilla.com/v1/country?key=b9255e08-838f-4d4e-b270-bec2cc89719b
static const QString s_geourl = QString::fromLatin1("http://www.geoplugin.net/json.gp");
// for reference:
// https://api.met.no/weatherapi/locationforecast/2.0/documentation
// https://api.met.no/doc/ForecastJSON
static const QString s_weatherurl = QString::fromLatin1("https://api.met.no/weatherapi/locationforecast/2.0/compact?lat=%1&lon=%2");
static const QString s_defaultweathericon = QString::fromLatin1("weather-none-available");
static const KTemperature::KTempUnit s_defaulttempunit = KTemperature::Celsius;

static bool kNightTime(const QDateTime &dt)
{
    const int month = dt.date().month();
    const int hour = dt.time().hour();
    if (month <= 3 || month >= 9) {
        return (hour >= 19 || hour <= 6);
    }
    return (hour >= 20 || hour <= 5);
}

static void kResetIconWidget(Plasma::IconWidget *iconwidget)
{
    iconwidget->setIcon(KIcon(s_defaultweathericon));
    iconwidget->setText(i18n("N/A"));
    iconwidget->setToolTip(QString());
}

static Plasma::IconWidget* kMakeIconWidget(QGraphicsWidget *parent, const Qt::Orientation orientation)
{
    const int desktopiconsize = KIconLoader::global()->currentSize(KIconLoader::Desktop);
    const QSizeF desktopiconsizef = QSizeF(desktopiconsize, desktopiconsize);
    Plasma::IconWidget* result = new Plasma::IconWidget(parent);
    result->setAcceptHoverEvents(false);
    result->setAcceptedMouseButtons(Qt::NoButton);
    result->setOrientation(orientation);
    result->setPreferredIconSize(desktopiconsizef);
    kResetIconWidget(result);
    return result;
}

static QString kTemperatureDisplayString(const QString &temperature,
                                         const KTemperature::KTempUnit fromunit,
                                         const KTemperature::KTempUnit tounit)
{
    if (temperature.isEmpty()) {
        return i18n("N/A");
    }
    KTemperature ktemperature(temperature.toDouble(), fromunit);
    if (fromunit == tounit) {
        return ktemperature.toString();
    }
    return KTemperature(ktemperature.convertTo(tounit), tounit).toString();
}

static QIcon kDisplayIcon(const QString &icon, const bool isnighttime)
{
    if (icon.isEmpty()) {
        return KIcon(s_defaultweathericon);
    } else if (icon == QLatin1String("mist")) {
        return KIcon("weather-mist");
    } else if (icon == QLatin1String("cloudy")) {
        return KIcon("weather-many-clouds");
    } else if (icon.startsWith(QLatin1String("clearsky_"))) {
        return KIcon(isnighttime ? "weather-clear-night" : "weather-clear");
    } else if (icon.startsWith(QLatin1String("partlycloudy_")) || icon.startsWith(QLatin1String("fair_"))) {
        return KIcon(isnighttime ? "weather-few-clouds-night" : "weather-few-clouds");
    } else if (icon.contains(QLatin1String("lightrain"))) {
        return KIcon(isnighttime ? "weather-showers-scattered-night" : "weather-showers-scattered");
    } else if (icon.contains(QLatin1String("rain"))) {
        return KIcon(isnighttime ? "weather-showers-night" : "weather-showers");
    } else if (icon.contains(QLatin1String("sleet"))) {
        return KIcon("weather-hail");
    } else if (icon.contains(QLatin1String("lightsnow"))) {
        return KIcon("weather-snow-scattered");
    } else if (icon.contains(QLatin1String("snow"))) {
        return KIcon("weather-snow");
    }
    kWarning() << "unhandled weather icon" << icon;
    return KIcon(s_defaultweathericon);
}

static QString kDisplayCondition(const QString &icon, const bool isnighttime)
{
    // TODO:
    return icon;
}

static QString kDisplayZoneName(const QString &ktimezonename)
{
    const QByteArray ktimezonenamebytes = ktimezonename.toUtf8();
    QString result = i18n(ktimezonenamebytes.constData());
    // replace underscore with space like KTimeZoneWidget
    result = result.replace(QLatin1Char('_'), QLatin1Char(' '));
    return result;
}

class KWeatherData
{
public:
    KWeatherData();
    explicit KWeatherData(const KTemperature::KTempUnit tempunit);

    KTemperature::KTempUnit tempunit;
    QString daytemperature;
    QString dayicon;
    QString nighttemperature;
    QString nighticon;
};
QDebug operator<<(QDebug s, const KWeatherData &kweatherdata)
{
    s.nospace() << "KWeatherData("
        << kweatherdata.tempunit << ",\n"
        << kweatherdata.daytemperature << ", " << kweatherdata.dayicon << ",\n"
        << kweatherdata.nighttemperature << ", " << kweatherdata.nighticon << ",\n"
        << ")";
    return s.space();
}

static void kUpdateIconWidget(Plasma::IconWidget *iconwidget, const KWeatherData &weatherdata,
                              const bool isnighttime, const KTemperature::KTempUnit tempunit)
{
    iconwidget->setText(
        kTemperatureDisplayString(
            isnighttime ? weatherdata.nighttemperature : weatherdata.daytemperature,
            weatherdata.tempunit, tempunit
        )
    );
    iconwidget->setIcon(
        kDisplayIcon(
            isnighttime ? weatherdata.nighticon : weatherdata.dayicon,
            isnighttime
        )
    );
    iconwidget->setToolTip(
        kDisplayCondition(
            isnighttime ? weatherdata.nighticon : weatherdata.dayicon,
            isnighttime
        )
    );
}

KWeatherData::KWeatherData()
    : tempunit(s_defaulttempunit)
{
}

KWeatherData::KWeatherData(const KTemperature::KTempUnit _tempunit)
    : tempunit(_tempunit)
{
}

class WeatherWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    WeatherWidget(WeatherApplet *weather);

    void setup(const QString &source, const float latitude, const float longitude, const KTemperature::KTempUnit tempunit);

private Q_SLOTS:
    void slotGeoResult(KJob *kjob);
    void slotWeatherResult(KJob *kjob);
    void slotUpdateWidgets();

private:
    void startGeoJob();
    void startWeatherJob(const QString &source, const float latitude, const float longitude);

    WeatherApplet* m_weather;
    QGraphicsLinearLayout* m_layout;
    KTemperature::KTempUnit m_tempunit;
    KIO::StoredTransferJob* m_geojob;
    KIO::StoredTransferJob* m_weatherjob;
    QVector<KWeatherData> m_weatherdata;
    QTimer* m_timer;
    Plasma::Frame* m_forecastframe;
    QGraphicsGridLayout* m_forecastlayout;
    Plasma::IconWidget* m_day0iconwidget;
    Plasma::IconWidget* m_night0iconwidget;
    Plasma::IconWidget* m_day1iconwidget;
    Plasma::IconWidget* m_night1iconwidget;
    Plasma::IconWidget* m_day2iconwidget;
    Plasma::IconWidget* m_night2iconwidget;
    Plasma::IconWidget* m_day3iconwidget;
    Plasma::IconWidget* m_night3iconwidget;
    Plasma::IconWidget* m_day4iconwidget;
    Plasma::IconWidget* m_night4iconwidget;
};

WeatherWidget::WeatherWidget(WeatherApplet* weather)
    : QGraphicsWidget(weather),
    m_weather(weather),
    m_layout(nullptr),
    m_tempunit(s_defaulttempunit),
    m_geojob(nullptr),
    m_weatherjob(nullptr),
    m_timer(nullptr),
    m_forecastframe(nullptr),
    m_forecastlayout(nullptr),
    m_day0iconwidget(nullptr),
    m_night0iconwidget(nullptr),
    m_day1iconwidget(nullptr),
    m_night1iconwidget(nullptr),
    m_day2iconwidget(nullptr),
    m_night2iconwidget(nullptr),
    m_day3iconwidget(nullptr),
    m_night3iconwidget(nullptr),
    m_day4iconwidget(nullptr),
    m_night4iconwidget(nullptr)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setMinimumSize(s_minimumsize);

    m_layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    setLayout(m_layout);

    m_forecastframe = new Plasma::Frame(this);
    m_forecastframe->setFrameShadow(Plasma::Frame::Sunken);
    m_forecastlayout = new QGraphicsGridLayout(m_forecastframe);
    m_forecastframe->setLayout(m_forecastlayout);
    m_day0iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_day0iconwidget, 0, 0);
    m_night0iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_night0iconwidget, 1, 0);
    m_day1iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_day1iconwidget, 0, 1);
    m_night1iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_night1iconwidget, 1, 1);
    m_day2iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_day2iconwidget, 0, 2);
    m_night2iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_night2iconwidget, 1, 2);
    m_day3iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_day3iconwidget, 0, 3);
    m_night3iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_night3iconwidget, 1, 3);
    m_day4iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_day4iconwidget, 0, 4);
    m_night4iconwidget = kMakeIconWidget(m_forecastframe, Qt::Vertical);
    m_forecastlayout->addItem(m_night4iconwidget, 1, 4);
    m_layout->addItem(m_forecastframe);

    m_timer = new QTimer(this);
    m_timer->setInterval(60000); // 1min
    connect(
        m_timer, SIGNAL(timeout()),
        this, SLOT(slotUpdateWidgets())
    );
}

void WeatherWidget::setup(const QString &source, const float latitude, const float longitude, const KTemperature::KTempUnit tempunit)
{
    m_timer->stop();
    m_tempunit = tempunit;
    m_weatherdata.clear();
    m_weatherdata.reserve(5);
    m_weatherdata.fill(KWeatherData(s_defaulttempunit), 5);
    m_forecastframe->setText(i18n("Unknown"));
    kResetIconWidget(m_day0iconwidget);
    kResetIconWidget(m_night0iconwidget);
    kResetIconWidget(m_day1iconwidget);
    kResetIconWidget(m_night1iconwidget);
    kResetIconWidget(m_day2iconwidget);
    kResetIconWidget(m_night2iconwidget);
    kResetIconWidget(m_day3iconwidget);
    kResetIconWidget(m_night3iconwidget);
    kResetIconWidget(m_day4iconwidget);
    kResetIconWidget(m_night4iconwidget);

    m_weather->setBusy(true);
    if (latitude == KTimeZone::UNKNOWN || longitude == KTimeZone::UNKNOWN) {
        startGeoJob();
        return;
    }

    startWeatherJob(source, latitude, longitude);
}

void WeatherWidget::startGeoJob()
{
    Q_ASSERT(m_geojob == nullptr);
    const KUrl geojoburl = s_geourl;
    kDebug() << "Starting geo job for" << geojoburl;
    m_geojob = KIO::storedGet(geojoburl, KIO::NoReload, KIO::HideProgressInfo);
    m_geojob->setAutoDelete(false);
    connect(
        m_geojob, SIGNAL(result(KJob*)),
        this, SLOT(slotGeoResult(KJob*))
    );
}

void WeatherWidget::startWeatherJob(const QString &source, const float latitude, const float longitude)
{
    Q_ASSERT(m_weatherjob == nullptr);
    m_forecastframe->setText(source.isEmpty() ? i18n("Unknown") : source);
    const KUrl weatherjoburl = s_weatherurl.arg(
        QString::number(latitude),
        QString::number(longitude)
    );
    kDebug() << "Starting weather job for" << weatherjoburl;
    m_weatherjob = KIO::storedGet(weatherjoburl, KIO::NoReload, KIO::HideProgressInfo);
    m_weatherjob->setAutoDelete(false);
    connect(
        m_weatherjob, SIGNAL(result(KJob*)),
        this, SLOT(slotWeatherResult(KJob*))
    );
}

void WeatherWidget::slotGeoResult(KJob *kjob)
{
    m_timer->stop();
    // the fallback is to local timezone coordinates
    const KTimeZone ktimezone = KSystemTimeZones::local();
    if (kjob->error() != KJob::NoError) {
        kWarning() << "geo job error" << kjob->errorString();
        kjob->deleteLater();
        startWeatherJob(ktimezone.name(), ktimezone.latitude(), ktimezone.longitude());
        return;
    }
    kDebug() << "geo job completed" << m_geojob->url();
    const QByteArray geodata = m_geojob->data();
    kjob->deleteLater();
    const QJsonDocument geojson = QJsonDocument::fromJson(geodata);
    // qDebug() << Q_FUNC_INFO << geodata;
    if (geojson.isNull()) {
        kWarning() << "null geo json document" << geojson.errorString();
        startWeatherJob(ktimezone.name(), ktimezone.latitude(), ktimezone.longitude());
        return;
    }
    const QVariantMap geomap = geojson.toVariant().toMap();
    if (geomap.isEmpty()) {
        kWarning() << "null weather map";
        return;
    }
    const QString georegion = geomap.value("geoplugin_regionName").toString();
    const QString geocountry = geomap.value("geoplugin_countryName").toString();
    const float geolatitude = geomap.value("geoplugin_latitude").toFloat();
    const float geolongitude = geomap.value("geoplugin_longitude").toFloat();
    const QString weathersource = QString::fromLatin1("%1 (%2)").arg(georegion, geocountry);
    startWeatherJob(weathersource, geolatitude, geolongitude);
}

void WeatherWidget::slotWeatherResult(KJob *kjob)
{
    if (kjob->error() != KJob::NoError) {
        kWarning() << "weather job error" << kjob->errorString();
        kjob->deleteLater();
        m_weather->setBusy(false);
        return;
    }
    kDebug() << "weather job completed" << m_weatherjob->url();
    const QByteArray weatherdata = m_weatherjob->data();
    kjob->deleteLater();
    const QJsonDocument weatherjson = QJsonDocument::fromJson(weatherdata);
    // qDebug() << Q_FUNC_INFO << weatherdata;
    if (weatherjson.isNull()) {
        kWarning() << "null weather json document" << weatherjson.errorString();
        m_weather->setBusy(false);
        return;
    }
    const QVariantMap weathermap = weatherjson.toVariant().toMap();
    if (weathermap.isEmpty()) {
        kWarning() << "null weather map";
        m_weather->setBusy(false);
        return;
    }
    const QVariantMap weatherpropertiesmap = weathermap.value("properties").toMap();
    if (weatherpropertiesmap.isEmpty()) {
        kWarning() << "null weather properties map";
        m_weather->setBusy(false);
        return;
    }
    const QVariantList weathertimeserieslist = weatherpropertiesmap.value("timeseries").toList();
    if (weathertimeserieslist.isEmpty()) {
        kWarning() << "null weather timeseries list";
        m_weather->setBusy(false);
        return;
    }

    KTemperature::KTempUnit tempunit = s_defaulttempunit;
    const QString weathertemperatureunit = weatherpropertiesmap.value("air_temperature").toString();
    if (!weathertemperatureunit.isEmpty()) {
        tempunit = KTemperature(0.0, weathertemperatureunit).unitEnum();
        if (tempunit == KTemperature::Invalid) {
            kWarning() << "invalid weather temperature unit" << weathertemperatureunit;
            tempunit = s_defaulttempunit;
        }
    }
    m_weatherdata.fill(KWeatherData(tempunit), 5);

    const QDateTime utcnow = QDateTime::currentDateTimeUtc();
    const QDate utc0 = utcnow.date();
    const QDate utc1 = utc0.addDays(1);
    const QDate utc2 = utc0.addDays(2);
    const QDate utc3 = utc0.addDays(3);
    const QDate utc4 = utc0.addDays(4);
    foreach (const QVariant &weathertimevariant, weathertimeserieslist) {
        const QVariantMap weathertimemap = weathertimevariant.toMap();
        const QString weathertimestring = weathertimemap.value("time").toString();
        const QDateTime weatherdatetime = QDateTime::fromString(weathertimestring, Qt::ISODate).toUTC();
        if (weatherdatetime.isNull()) {
            kWarning() << "invalid weather time" << weathertimestring;
            continue;
        }

        const QVariantMap weatherdatamap = weathertimemap.value("data").toMap();
        if (weatherdatamap.isEmpty()) {
            kWarning() << "invalid weather data" << weathertimestring;
            continue;
        }

        const QDate weatherdate = weatherdatetime.date();
        int weatherdataindex = -1;
        if (weatherdate == utc0) {
            weatherdataindex = 0;
        } else if (weatherdate == utc1) {
            weatherdataindex = 1;
        } else if (weatherdate == utc2) {
            weatherdataindex = 2;
        } else if (weatherdate == utc3) {
            weatherdataindex = 3;
        } else if (weatherdate == utc4) {
            weatherdataindex = 4;
        }
        // qDebug() << Q_FUNC_INFO << weatherdataindex;
        if (weatherdataindex != -1) {
            kDebug() << "found weather data for day number" << weatherdataindex;
            if (kNightTime(weatherdatetime)) {
                kDebug() << "found weather data for night" << weatherdataindex;
                if (m_weatherdata[weatherdataindex].nighttemperature.isEmpty()) {
                    m_weatherdata[weatherdataindex].nighttemperature = weatherdatamap.value("instant").toMap().value("details").toMap().value("air_temperature").toString();
                }
                if (m_weatherdata[weatherdataindex].nighticon.isEmpty()) {
                    m_weatherdata[weatherdataindex].nighticon = weatherdatamap.value("next_12_hours").toMap().value("summary").toMap().value("symbol_code").toString();
                }
            } else {
                kDebug() << "found weather data for day" << weatherdataindex;
                if (m_weatherdata[weatherdataindex].daytemperature.isEmpty()) {
                    m_weatherdata[weatherdataindex].daytemperature = weatherdatamap.value("instant").toMap().value("details").toMap().value("air_temperature").toString();
                }
                if (m_weatherdata[weatherdataindex].dayicon.isEmpty()) {
                    m_weatherdata[weatherdataindex].dayicon = weatherdatamap.value("next_12_hours").toMap().value("summary").toMap().value("symbol_code").toString();
                }
            }
        }
        // qDebug() << Q_FUNC_INFO << weatherdatetime;
    }
    // qDebug() << Q_FUNC_INFO << m_weatherdata;
    m_weather->setBusy(false);
    slotUpdateWidgets();
    m_timer->start();
}

void WeatherWidget::slotUpdateWidgets()
{
    // TODO: day change detection
    const QDateTime utcnow = QDateTime::currentDateTimeUtc();
    const bool isnighttime = kNightTime(utcnow);
    // qDebug() << Q_FUNC_INFO << utcnow << isnighttime;
    kUpdateIconWidget(m_day0iconwidget, m_weatherdata[0], false, m_tempunit);
    kUpdateIconWidget(m_night0iconwidget, m_weatherdata[0], true, m_tempunit);
    kUpdateIconWidget(m_day1iconwidget, m_weatherdata[1], false, m_tempunit);
    kUpdateIconWidget(m_night1iconwidget, m_weatherdata[1], true, m_tempunit);
    kUpdateIconWidget(m_day2iconwidget, m_weatherdata[2], false, m_tempunit);
    kUpdateIconWidget(m_night2iconwidget, m_weatherdata[2], true, m_tempunit);
    kUpdateIconWidget(m_day3iconwidget, m_weatherdata[3], false, m_tempunit);
    kUpdateIconWidget(m_night3iconwidget, m_weatherdata[3], true, m_tempunit);
    kUpdateIconWidget(m_day4iconwidget, m_weatherdata[4], false, m_tempunit);
    kUpdateIconWidget(m_night4iconwidget, m_weatherdata[4], true, m_tempunit);
    m_weather->setPopupIcon(kDisplayIcon(isnighttime ? m_weatherdata[0].nighticon : m_weatherdata[0].dayicon, isnighttime));
}

WeatherApplet::WeatherApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_weatherwidget(nullptr),
    m_tempunit(s_defaulttempunit),
    m_tempunitbox(nullptr),
    m_locationbox(nullptr),
    m_latitude(KTimeZone::UNKNOWN),
    m_latitudeinput(nullptr),
    m_longitude(KTimeZone::UNKNOWN),
    m_longitudeinput(nullptr),
    m_spacer(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_weather");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    setPopupIcon(s_defaultweathericon);

    m_weatherwidget = new WeatherWidget(this);
}

WeatherApplet::~WeatherApplet()
{
    delete m_weatherwidget;
}

void WeatherApplet::init()
{
    KConfigGroup configgroup = config();
    m_tempunit = static_cast<KTemperature::KTempUnit>(configgroup.readEntry("weatherTempUnit", static_cast<int>(s_defaulttempunit)));
    m_location = configgroup.readEntry("weatherLocation", QString());
    m_latitude = configgroup.readEntry("weatherLatitude", KTimeZone::UNKNOWN);
    m_longitude = configgroup.readEntry("weatherLongitude", KTimeZone::UNKNOWN);
    QString source;
    if (!m_location.isEmpty()) {
        source = kDisplayZoneName(m_location);
    } else if (m_latitude != KTimeZone::UNKNOWN && m_longitude != KTimeZone::UNKNOWN) {
        source = i18n("Custom");
    }
    m_weatherwidget->setup(source, m_latitude, m_longitude, m_tempunit);
}

void WeatherApplet::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget* widget = new QWidget();
    QGridLayout* widgetlayout = new QGridLayout(widget);
    QLabel* tempunitlabel = new QLabel(widget);
    tempunitlabel->setText(i18n("Temperature unit:"));
    widgetlayout->addWidget(tempunitlabel, 0, 0);
    m_tempunitbox = new QComboBox(widget);
    for (int i = 0; i < KTemperature::UnitCount; i++) {
        KTemperature::KTempUnit unit = static_cast<KTemperature::KTempUnit>(i);
        m_tempunitbox->addItem(KTemperature::unitDescription(unit), unit);
    }
    m_tempunitbox->setCurrentIndex(static_cast<int>(m_tempunit));
    widgetlayout->addWidget(m_tempunitbox, 0, 1);
    QLabel* locationlabel = new QLabel(widget);
    locationlabel->setText(i18n("Location:"));
    widgetlayout->addWidget(locationlabel, 1, 0);
    m_locationbox = new QComboBox(widget);
    QMap<QString,QString> sortedzones;
    foreach (const KTimeZone &ktimezone, KSystemTimeZones::zones()) {
        const QString ktimezonename = ktimezone.name();
        if (ktimezonename == QLatin1String("UTC")) {
            continue;
        }
        sortedzones.insert(kDisplayZoneName(ktimezonename), ktimezonename);
    }
    m_locationbox->addItem(i18n("Automatic"));
    m_locationbox->addItem(i18n("Custom"));
    QMapIterator<QString,QString> sortedzonesiter(sortedzones);
    while (sortedzonesiter.hasNext()) {
        sortedzonesiter.next();
        m_locationbox->addItem(sortedzonesiter.key(), sortedzonesiter.value());
    }
    widgetlayout->addWidget(m_locationbox, 1, 1, 1, 1);
    m_latitudeinput = new KDoubleNumInput(widget);
    m_latitudeinput->setSliderEnabled(true);
    m_latitudeinput->setLabel(i18n("Latitude:"));
    m_latitudeinput->setValue(m_latitude);
    widgetlayout->addWidget(m_latitudeinput, 2, 0, 1, 2);
    m_longitudeinput = new KDoubleNumInput(widget);
    m_longitudeinput->setSliderEnabled(true);
    m_longitudeinput->setLabel(i18n("Longitude:"));
    m_longitudeinput->setValue(m_longitude);
    widgetlayout->addWidget(m_longitudeinput, 3, 0, 1, 2);
    m_spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    widgetlayout->addItem(m_spacer, 4, 0, 1, 2);
    widget->setLayout(widgetlayout);
    parent->addPage(widget, i18n("Weather"), "weather-clear");

    if (!m_location.isEmpty()) {
        const int locationindex = m_locationbox->findData(m_location);
        // first is automatic, then custom and then timezones
        if (locationindex < 2) {
            kWarning() << "could not find location index" << m_location;
        } else {
            m_locationbox->setCurrentIndex(locationindex);
        }
    } else if (m_latitude != KTimeZone::UNKNOWN && m_longitude != KTimeZone::UNKNOWN) {
        // custom location
        m_locationbox->setCurrentIndex(1);
    }
    slotCheckLocation();

    connect(parent, SIGNAL(applyClicked()), this, SLOT(slotConfigAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(slotConfigAccepted()));
    connect(m_tempunitbox, SIGNAL(currentIndexChanged(int)), parent, SLOT(settingsModified()));
    connect(m_locationbox, SIGNAL(currentIndexChanged(int)), parent, SLOT(settingsModified()));
    connect(m_locationbox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCheckLocation()));
    connect(m_latitudeinput, SIGNAL(valueChanged(double)), parent, SLOT(settingsModified()));
    connect(m_longitudeinput, SIGNAL(valueChanged(double)), parent, SLOT(settingsModified()));
}

QGraphicsWidget* WeatherApplet::graphicsWidget()
{
    return m_weatherwidget;
}

void WeatherApplet::slotCheckLocation()
{
    const int locationindex = m_locationbox->currentIndex();
    if (locationindex == 1){
        m_latitudeinput->setEnabled(true);
        m_latitudeinput->setRange(-90.0, 90.0);
        m_latitudeinput->setValue(0.0);
        m_longitudeinput->setEnabled(true);
        m_longitudeinput->setRange(-180.0, 180.0);
        m_longitudeinput->setValue(0.0);
    } else {
        m_latitudeinput->setEnabled(false);
        m_latitudeinput->setRange(KTimeZone::UNKNOWN, KTimeZone::UNKNOWN);
        m_latitudeinput->setValue(KTimeZone::UNKNOWN);
        m_longitudeinput->setEnabled(false);
        m_longitudeinput->setRange(KTimeZone::UNKNOWN, KTimeZone::UNKNOWN);
        m_longitudeinput->setValue(KTimeZone::UNKNOWN);
    }
}

void WeatherApplet::slotConfigAccepted()
{
    Q_ASSERT(m_tempunitbox);
    Q_ASSERT(m_locationbox);
    Q_ASSERT(m_latitudeinput);
    Q_ASSERT(m_longitudeinput);
    const int tempindex = m_tempunitbox->currentIndex();
    m_tempunit = static_cast<KTemperature::KTempUnit>(tempindex);
    const int locationindex = m_locationbox->currentIndex();
    QString source;
    // the location is saved only when it is timezone
    m_location.clear();
    if (locationindex == 0) {
        source = i18n("Unknown");
        m_latitude = KTimeZone::UNKNOWN;
        m_longitude = KTimeZone::UNKNOWN;
    } else if (locationindex == 1) {
        source = i18n("Custom");
        m_latitude = m_latitudeinput->value();
        m_longitude = m_longitudeinput->value();
    } else {
        bool foundtimezone = false;
        const QString ktimezonename = m_locationbox->itemData(locationindex).toString();
        foreach (const KTimeZone &ktimezone, KSystemTimeZones::zones()) {
            if (ktimezone.name() != ktimezonename) {
                continue;
            }
            foundtimezone = true;
            source = m_locationbox->itemText(locationindex);
            m_location = ktimezone.name();
            m_latitude = ktimezone.latitude();
            m_longitude = ktimezone.longitude();
        }
        if (!foundtimezone) {
            kWarning() << "could not find timezone" << ktimezonename;
        }
    }
    KConfigGroup configgroup = config();
    configgroup.writeEntry("weatherTempUnit", tempindex);
    configgroup.writeEntry("weatherLocation", m_location);
    configgroup.writeEntry("weatherLatitude", m_latitude);
    configgroup.writeEntry("weatherLongitude", m_longitude);
    m_weatherwidget->setup(source, m_latitude, m_longitude, m_tempunit);
    emit configNeedsSaving();
}

K_EXPORT_PLASMA_APPLET(weather, WeatherApplet)

#include "moc_weather.cpp"
#include "weather.moc"
