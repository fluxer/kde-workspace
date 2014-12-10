//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 1999
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "kxsconfig.h"
#include <klocale.h>
#include <qxml.h>

//===========================================================================
KXSConfigItem::KXSConfigItem(const QString &name, KConfig &config)
  : mName(name)
{
  KConfigGroup grp = config.group(name);
  mLabel = i18n(grp.readEntry("Label").toUtf8());
}

KXSConfigItem::KXSConfigItem(const QString &name, const QXmlAttributes &attr )
  : mName(name)
{
  mLabel = attr.value(QLatin1String( "_label" ));
}

//===========================================================================
KXSRangeItem::KXSRangeItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config), mInvert(false)
{
  KConfigGroup grp(&config, name);
  mMinimum = grp.readEntry("Minimum",0);
  mMaximum = grp.readEntry("Maximum",0);
  mValue   = grp.readEntry("Value",0);
  mSwitch  = grp.readEntry("Switch",0);
}

KXSRangeItem::KXSRangeItem(const QString &name, const QXmlAttributes &attr )
  : KXSConfigItem(name, attr), mInvert(false)
{
  mMinimum = attr.value(QLatin1String( "low" )).toInt();
  mMaximum = attr.value(QLatin1String( "high" )).toInt();
  mValue   = attr.value(QLatin1String( "default" )).toInt();
  mSwitch  = attr.value(QLatin1String( "arg" ));
  int pos = mSwitch.indexOf( QLatin1Char( '%' ) );
  if (pos >= 0)
    mSwitch.insert(pos+1, QLatin1Char( '1' ));
  if ( attr.value(QLatin1String( "convert" )) == QLatin1String( "invert" ) )
    mInvert = true;
  if (mInvert)
    mValue = mMaximum-(mValue-mMinimum);
}

QString KXSRangeItem::command()
{
  return mSwitch.arg(mInvert?mMaximum-(mValue-mMinimum):mValue);
}

void KXSRangeItem::read(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  if (grp.hasKey("Value"))
      mValue = grp.readEntry("Value",0);
}

void KXSRangeItem::save(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  grp.writeEntry("Value", mValue);
}

//===========================================================================
KXSDoubleRangeItem::KXSDoubleRangeItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config), mInvert(false)
{
 KConfigGroup grp(&config, name);
  mMinimum = grp.readEntry("Minimum",0.0);
  mMaximum = grp.readEntry("Maximum",0.0);
  mValue   = grp.readEntry("Value",0.0);
  mSwitch  = grp.readEntry("Switch");
}

KXSDoubleRangeItem::KXSDoubleRangeItem(const QString &name, const QXmlAttributes &attr)
  : KXSConfigItem(name, attr), mInvert(false)
{
  mMinimum = attr.value(QLatin1String( "low" )).toDouble();
  mMaximum = attr.value(QLatin1String( "high" )).toDouble();
  mValue   = attr.value(QLatin1String( "default" )).toDouble();
  mSwitch  = attr.value(QLatin1String( "arg" ));
  int pos = mSwitch.indexOf( QLatin1Char( '%' ) );
  if (pos >= 0)
    mSwitch.insert(pos+1, QLatin1Char( '1' ));
  if ( attr.value(QLatin1String( "convert" )) == QLatin1String( "invert" ) )
    mInvert = true;
  if (mInvert)
    mValue = mMaximum-(mValue-mMinimum);
}

QString KXSDoubleRangeItem::command()
{
  return mSwitch.arg(mInvert?mMaximum-(mValue-mMinimum):mValue);
}

void KXSDoubleRangeItem::read(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  if (grp.hasKey("Value"))
      mValue = grp.readEntry("Value",0.0);
}

void KXSDoubleRangeItem::save(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  grp.writeEntry("Value", mValue);
}


//===========================================================================
KXSBoolItem::KXSBoolItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  KConfigGroup grp(&config, name);
  mValue = grp.readEntry("Value",0);
  mSwitchOn  = grp.readEntry("SwitchOn");
  mSwitchOff = grp.readEntry("SwitchOff");
}

KXSBoolItem::KXSBoolItem(const QString &name, const QXmlAttributes &attr )
  : KXSConfigItem(name, attr)
{
  mSwitchOn  = attr.value(QLatin1String( "arg-set" ));
  mSwitchOff = attr.value(QLatin1String( "arg-unset" ));
  mValue = mSwitchOn.isEmpty() ? true : false;
}

QString KXSBoolItem::command()
{
  return mValue ? mSwitchOn : mSwitchOff;
}

void KXSBoolItem::read(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  if (grp.hasKey("Value"))
      mValue = grp.readEntry("Value", false);
}

void KXSBoolItem::save(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  grp.writeEntry("Value", mValue);
}

//===========================================================================
KXSSelectItem::KXSSelectItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  KConfigGroup grp(&config, name);
  mOptions = grp.readEntry("Options",QStringList());
  mSwitches = grp.readEntry("Switches",QStringList());
  mValue = grp.readEntry("Value",0);
}

KXSSelectItem::KXSSelectItem(const QString &name, const QXmlAttributes &attr )
  : KXSConfigItem(name, attr), mValue(0)
{
}

void KXSSelectItem::addOption( const QXmlAttributes &attr )
{
    QString opt = attr.value(QLatin1String( "_label" ));
    QString arg = attr.value(QLatin1String( "arg-set" ));
    if ( arg.isEmpty() )
	mValue = mSwitches.count();
    mOptions += opt;
    mSwitches += arg;
}

QString KXSSelectItem::command()
{
  QString tmp = mSwitches.at(mValue);
  return tmp;
}

void KXSSelectItem::read(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  if (grp.hasKey("Value"))
      mValue = grp.readEntry("Value",0);
}

void KXSSelectItem::save(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  grp.writeEntry("Value", mValue);
}


//===========================================================================
KXSStringItem::KXSStringItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  KConfigGroup grp(&config, name);
  mValue = grp.readEntry("Value");
  mSwitch = grp.readEntry("Switch");
  int pos = mSwitch.indexOf( QLatin1Char( '%' ) );
  if (pos >= 0) {
    mSwitch.insert(pos+1, QLatin1String( "\"" ));
    mSwitch.insert(pos, QLatin1String( "\"" ));
  }
}

KXSStringItem::KXSStringItem(const QString &name, const QXmlAttributes &attr )
  : KXSConfigItem(name, attr)
{
  mSwitch = attr.value(QLatin1String( "arg" ));
  int pos = mSwitch.indexOf( QLatin1Char( '%' ) );
  if (pos >= 0) {
    mSwitch.insert(pos+1, QLatin1String( "1\"" ));
    mSwitch.insert(pos, QLatin1String( "\"" ));
  }
}

QString KXSStringItem::command()
{
  if (!mValue.isEmpty())
      return mSwitch.arg(mValue);
  return QLatin1String( "" );
}

void KXSStringItem::read(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  if (grp.hasKey("Value"))
      mValue = grp.readEntry("Value");
}

void KXSStringItem::save(KConfig &config)
{
  KConfigGroup grp = config.group(mName);
  grp.writeEntry("Value", mValue);
}

