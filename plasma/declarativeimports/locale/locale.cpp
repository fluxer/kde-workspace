/* This file is part of the KDE libraries
    Copyright (C) 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
    Copyright (C) 2012 Antonis Tsiapaliokas <kok3rs@gmail.com>

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

//own
#include "locale_p.h"

//KDE
#include <KGlobal>

Locale::Locale(QObject* parent)
    : QObject(parent)
{
    m_locale = KGlobal::locale();
}

Locale::BinaryUnitDialect Locale::binaryUnitDialect() const
{
    return (Locale::BinaryUnitDialect)m_locale->binaryUnitDialect();
}

QString Locale::dateFormat(QLocale::FormatType format) const
{
    return m_locale->dateFormat(format);
}

QString Locale::timeFormat(QLocale::FormatType format) const
{
    return m_locale->timeFormat(format);
}

QString Locale::dateTimeFormat(QLocale::FormatType format) const
{
    return m_locale->dateTimeFormat(format);
}

QLocale::MeasurementSystem Locale::measureSystem() const
{
    return m_locale->measureSystem();
}

QString Locale::formatLong(long num) const
{
    return m_locale->formatLong(num);
}

QString Locale::formatNumber(const QString &numStr, bool round, int precision) const
{
    return m_locale->formatNumber(numStr, round, precision);
}

QString Locale::formatByteSize(double size, int precision,
                               Locale::BinarySizeUnits specificUnit) const
{
 return m_locale->formatByteSize(size, precision, (KLocale::BinarySizeUnits)specificUnit);
}

QString Locale::formatByteSize(double size) const
{
    return m_locale->formatByteSize(size);
}

QString Locale::formatDuration(unsigned long mSec) const
{
    return m_locale->formatDuration(mSec);
}

QString Locale::formatDate(const QDate &date, QLocale::FormatType format) const
{
    return m_locale->formatDate(date, format);
}

QString Locale::formatTime(const QTime &time, QLocale::FormatType format) const
{
    return m_locale->formatTime(time, format);
}

QString Locale::formatDateTime(const QDateTime &dateTime, QLocale::FormatType format) const
{
    return m_locale->formatDateTime(dateTime, format);
}

double Locale::readNumber(const QString &str) const
{
    bool ok = false;
    return m_locale->readNumber(str, &ok);
}

QDate Locale::readDate(const QString &intstr, QLocale::FormatType format) const
{
    return m_locale->readDate(intstr, format);
}

QTime Locale::readTime(const QString &intstr, QLocale::FormatType format) const
{
    return m_locale->readTime(intstr, format);
}

QStringList Locale::languageList() const
{
    return m_locale->languageList();
}

bool Locale::isApplicationTranslatedInto(const QString &lang)
{
    return KLocale::isApplicationTranslatedInto(lang);
}

void Locale::splitLocale(const QString &locale, QString &language, QString &country, QString &modifier,
                         QString &charset)
{
    KLocale::splitLocale(locale, language, country, modifier, charset);
}

QString Locale::language() const
{
    return m_locale->language();
}

QString Locale::translateQt(const char *context, const char *sourceText) const
{
    return m_locale->translateQt(context, sourceText);
}

QString Locale::defaultLanguage()
{
    return KLocale::defaultLanguage();
}

QStringList Locale::allLanguagesList()
{
    return KLocale::allLanguagesList();
}

QStringList Locale::installedLanguages()
{
    return KLocale::installedLanguages();
}

QString Locale::languageCodeToName(const QString &language) const
{
    return m_locale->languageCodeToName(language);
}

QString Locale::countryCodeToName(const QString &country) const
{
    return m_locale->countryCodeToName(country);
}

QString Locale::removeAcceleratorMarker(const QString &label) const
{
    return m_locale->removeAcceleratorMarker(label);
}

void Locale::reparseConfiguration()
{
    m_locale->reparseConfiguration();
}

QLocale Locale::toLocale() const
{
    return m_locale->toLocale();
}
