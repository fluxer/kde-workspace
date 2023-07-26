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
#ifndef LOCALE_H
#define LOCALE_H

//Qt
#include <QObject>
#include <QString>
#include <QDate>
#include <QTime>
#include <QDateTime>

//KDE
#include <KLocale>

/**
 * \file klocale.h
 */

/**
  *
  * KLocale provides support for country specific stuff like
  * the national language.
  *
  * KLocale supports translating, as well as specifying the format
  * for numbers, time, and date.
  *
  * Use KGlobal::locale() to get pointer to the global KLocale object,
  * containing the applications current locale settings.
  *
  * For example, to format the date May 17, 1995 in the current locale, use:
  *
  * \code
  *   QString date = KGlobal::locale()->formatDate(QDate(1995,5,17));
  * \endcode
  *
  * @author Stephan Kulow <coolo@kde.org>, Preston Brown <pbrown@kde.org>,
  * Hans Petter Bieker <bieker@kde.org>, Lukas Tinkl <lukas.tinkl@suse.cz>
  * @short class for supporting locale settings and national language
  */
class Locale : public QObject
{
    Q_OBJECT

    // enums
    Q_ENUMS(BinarySizeUnits)
    Q_ENUMS(BinaryUnitDialect)

    // properties
    Q_PROPERTY(BinaryUnitDialect binaryUnitDialect READ binaryUnitDialect CONSTANT) //read-only
    Q_PROPERTY(QString language READ language CONSTANT) //read-only
    Q_PROPERTY(QString defaultLanguage READ defaultLanguage CONSTANT)//read-only
    Q_PROPERTY(QStringList languageList READ languageList CONSTANT) //read-only
    Q_PROPERTY(QStringList allLanguagesList READ allLanguagesList CONSTANT) //read-only
    Q_PROPERTY(QStringList installedLanguages READ installedLanguages CONSTANT) //read-only

public:
    Locale(QObject *parent = 0);

    enum BinarySizeUnits {
        /// Auto-choose a unit such that the result is in the range [0, 1000 or 1024)
        DefaultBinaryUnits = -1,

        // The first real unit must be 0 for the current implementation!
        UnitByte = 0,      ///<  B         1 byte
        UnitKiloByte,  ///<  KiB/KB/kB 1024/1000 bytes.
        UnitMegaByte,  ///<  MiB/MB/MB 2^20/10^06 bytes.
        UnitGigaByte,  ///<  GiB/GB/GB 2^30/10^09 bytes.
        UnitTeraByte,  ///<  TiB/TB/TB 2^40/10^12 bytes.
        UnitPetaByte,  ///<  PiB/PB/PB 2^50/10^15 bytes.
        UnitExaByte,   ///<  EiB/EB/EB 2^60/10^18 bytes.
        UnitZettaByte, ///<  ZiB/ZB/ZB 2^70/10^21 bytes.
        UnitYottaByte, ///<  YiB/YB/YB 2^80/10^24 bytes.
        LastBinaryUnit = UnitYottaByte
    };

    enum BinaryUnitDialect {
        IECBinaryDialect = 0,          ///< KDE Default, KiB, MiB, etc. 2^(10*n)
        JEDECBinaryDialect,        ///< KDE 3.5 default, KB, MB, etc. 2^(10*n)
        MetricBinaryDialect,       ///< SI Units, kB, MB, etc. 10^(3*n)
        LastBinaryDialect = MetricBinaryDialect
    };

    BinaryUnitDialect binaryUnitDialect() const;
    QString dateFormat(QLocale::FormatType format) const;
    QString timeFormat(QLocale::FormatType format) const;
    QString dateTimeFormat(QLocale::FormatType format) const;
    QLocale::MeasurementSystem measureSystem() const;

    Q_INVOKABLE QString formatNumber(const QString &numStr, bool round = true, int precision = -1) const;
    Q_INVOKABLE QString formatLong(long num) const;
    Q_INVOKABLE QString formatByteSize(double size) const;
    QString formatByteSize(double size, int precision,
                           BinarySizeUnits specificUnit = Locale::DefaultBinaryUnits) const;

    Q_INVOKABLE QString formatDuration(unsigned long mSec) const;
    Q_INVOKABLE QString formatDate(const QDate &date, QLocale::FormatType format = QLocale::ShortFormat) const;
    Q_INVOKABLE QString formatDateTime(const QDateTime &dateTime, QLocale::FormatType format = QLocale::ShortFormat) const;
    Q_INVOKABLE QString formatTime(const QTime &time, QLocale::FormatType format = QLocale::ShortFormat) const;

    Q_INVOKABLE double readNumber(const QString &numStr) const;
    Q_INVOKABLE QDate readDate(const QString &str) const;
    Q_INVOKABLE QTime readTime(const QString &str) const;

    Q_INVOKABLE QString language() const;

    Q_INVOKABLE QStringList languageList() const;
    Q_INVOKABLE QString translateQt(const char *context, const char *sourceText) const;
    Q_INVOKABLE QString languageCodeToName(const QString &language) const;
    Q_INVOKABLE QString countryCodeToName(const QString &country) const;
    Q_INVOKABLE QStringList allLanguagesList();
    Q_INVOKABLE QStringList installedLanguages();
    Q_INVOKABLE bool isApplicationTranslatedInto(const QString &language);
    Q_INVOKABLE void splitLocale(const QString &locale, QString &language, QString &country,
                                 QString &modifier, QString &charset);
    Q_INVOKABLE QString defaultLanguage();
    Q_INVOKABLE QString removeAcceleratorMarker(const QString &label) const;

    Q_INVOKABLE void reparseConfiguration();

private:
    KLocale *m_locale;
};

Q_DECLARE_METATYPE(QLocale::FormatType)
Q_DECLARE_METATYPE(QLocale::MeasurementSystem)

#endif
