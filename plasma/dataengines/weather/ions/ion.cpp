/*****************************************************************************
 * Copyright (C) 2007-2009 by Shawn Starr <shawn.starr@rogers.com>           *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#include "ion.h"
#include "moc_ion.cpp"

#include <KDebug>

class IonInterface::Private
{
public:
    Private(IonInterface *i)
            : ion(i),
            initialized(false) {}

    IonInterface *ion;
    bool initialized;
};

IonInterface::IonInterface(QObject *parent, const QVariantList &args)
        : Plasma::DataEngine(parent, args),
        d(new Private(this))
{
}

IonInterface::~IonInterface()
{
    delete d;
}

/**
 * If the ion is not initialized just set the initial data source up even if it's empty, we'll retry once the initialization is done
 */
bool IonInterface::sourceRequestEvent(const QString &source)
{
    kDebug() << "sourceRequested(): " << source;

    // init anyway the data as it's going to be used
    // sooner or later (doesnt depend upon initialization
    // this will avoid problems if updateIonSource() fails for any reason
    // but later it's able to retrieve the data
    setData(source, Plasma::DataEngine::Data());

    // if initialized, then we can try to grab the data
    if (d->initialized) {
        return updateIonSource(source);
    }

    return true;
}

/**
 * Update the ion's datasource. Triggered when a Plasma::DataEngine::connectSource() timeout occurs.
 */
bool IonInterface::updateSourceEvent(const QString& source)
{
    kDebug() << "updateSource(" << source << ")";
    if (d->initialized) {
        kDebug() << "Calling updateIonSource(" << source << ")";
        return updateIonSource(source);
    }

    return false;
}

/**
 * Set the ion to make sure it is ready to get real data.
 */
void IonInterface::setInitialized(bool initialized)
{
    d->initialized = initialized;

    if (d->initialized) {
        updateAllSources();
    }
}

/**
 * Return wind direction svg element to display in applet when given a wind direction.
 */
QString IonInterface::getWindDirectionIcon(const QMap<QString, WindDirections> &windDirList, const QString& windDirection) const
{
    // NOTE: the direction strings have to match the elements in:
    // kde-extraapps/kdeplasma-addons/applets/weatherstation/wind_arrows.svgz
    // that means only no-op translation here!
    const char* noop_directions[]={
        I18N_NOOP("N"),
        I18N_NOOP("NNE"),
        I18N_NOOP("NE"),
        I18N_NOOP("ENE"),
        I18N_NOOP("E"),
        I18N_NOOP("SSE"),
        I18N_NOOP("SE"),
        I18N_NOOP("ESE"),
        I18N_NOOP("S"),
        I18N_NOOP("NNW"),
        I18N_NOOP("NW"),
        I18N_NOOP("WNW"),
        I18N_NOOP("W"),
        I18N_NOOP("SSW"),
        I18N_NOOP("SW"),
        I18N_NOOP("WSW"),
        I18N_NOOP("N/A")
    };
    Q_UNUSED(noop_directions);

    switch (windDirList[windDirection.toLower()]) {
    case N:
        return QString::fromLatin1("N");
    case NNE:
        return QString::fromLatin1("NNE");
    case NE:
        return QString::fromLatin1("NE");
    case ENE:
        return QString::fromLatin1("ENE");
    case E:
        return QString::fromLatin1("E");
    case SSE:
        return QString::fromLatin1("SSE");
    case SE:
        return QString::fromLatin1("SE");
    case ESE:
        return QString::fromLatin1("ESE");
    case S:
        return QString::fromLatin1("S");
    case NNW:
        return QString::fromLatin1("NNW");
    case NW:
        return QString::fromLatin1("NW");
    case WNW:
        return QString::fromLatin1("WNW");
    case W:
        return QString::fromLatin1("W");
    case SSW:
        return QString::fromLatin1("SSW");
    case SW:
        return QString::fromLatin1("SW");
    case WSW:
        return QString::fromLatin1("WSW");
    case VR:
        return QString::fromLatin1("N/A"); // For now, we'll make a variable wind icon later on
    }

    // No icon available, use 'X'
    return QString::fromLatin1("N/A");
}

/**
 * Return weather icon to display in an applet when given a condition.
 */
QString IonInterface::getWeatherIcon(ConditionIcons condition) const
{
    switch (condition) {
    case ClearDay:
        return "weather-clear";
    case FewCloudsDay:
        return "weather-few-clouds";
    case PartlyCloudyDay:
        return "weather-clouds";
    case Overcast:
        return "weather-many-clouds";
    case Rain:
        return "weather-showers";
    case LightRain:
        return "weather-showers-scattered";
    case Showers:
        return "weather-showers-scattered";
    case ChanceShowersDay:
        return "weather-showers-scattered-day";
    case ChanceShowersNight:
        return "weather-showers-scattered-night";
    case ChanceSnowDay:
        return "weather-snow-scattered-day";
    case ChanceSnowNight:
        return "weather-snow-scattered-night";
    case Thunderstorm:
        return "weather-storm";
    case Hail:
        return "weather-hail";
    case Snow:
        return "weather-snow";
    case LightSnow:
        return "weather-snow-scattered";
    case Flurries:
        return "weather-snow-scattered";
    case RainSnow:
        return "weather-snow-rain";
    case FewCloudsNight:
        return "weather-few-clouds-night";
    case PartlyCloudyNight:
        return "weather-clouds-night";
    case ClearNight:
        return "weather-clear-night";
    case Mist:
        return "weather-mist";
    case Haze:
        return "weather-mist";
    case FreezingRain:
        return "weather-freezing-rain";
    case FreezingDrizzle:
        return "weather-freezing-rain";
    case ChanceThunderstormDay:
        return "weather-storm-day";
    case ChanceThunderstormNight:
        return "weather-storm-night";
    case NotAvailable:
        return "weather-none-available";
    }
    return "weather-none-available";
}

/**
 * Return weather icon to display in an applet when given a condition.
 */
QString IonInterface::getWeatherIcon(const QMap<QString, ConditionIcons> &conditionList, const QString& condition) const
{
    return getWeatherIcon(conditionList[condition.toLower()]);
}
