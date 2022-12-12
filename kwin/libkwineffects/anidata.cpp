/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2011 Thomas Lübking <thomas.luebking@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "anidata_p.h"

#include <KDebug>

QDebug operator<<(QDebug dbg, const KWin::AniData &a)
{
    dbg.nospace() << a.debugInfo();
    return dbg.space();
}

using namespace KWin;

AniData::AniData()
{
    attribute = AnimationEffect::Opacity;
    windowType = (NET::WindowTypeMask)0;
    duration = time = meta = startTime = 0;
    waitAtSource = keepAtTarget = false;
}

AniData::AniData(AnimationEffect::Attribute a, uint meta, int ms, const FPx2 &to,
                 QEasingCurve curve, int delay, const FPx2 &from, bool waitAtSource, bool keepAtTarget )
{
    attribute = a;
    this->from = from;
    this->to = to;
    this->curve = curve;
    duration = ms;
    time = 0;
    this->meta = meta;
    this->waitAtSource = waitAtSource;
    this->keepAtTarget = keepAtTarget;
    startTime = AnimationEffect::clock() + delay;
}

AniData::AniData(const AniData &other)
{
    attribute = other.attribute;
    from = other.from;
    to = other.to;
    time = other.time;
    duration = other.duration;
    curve = other.curve;
    windowType = other.windowType;
    meta = other.meta;
    waitAtSource = other.waitAtSource;
    keepAtTarget = other.keepAtTarget;
    startTime = other.startTime;
}

static QString attributeString(KWin::AnimationEffect::Attribute attribute)
{
    switch (attribute) {
    case KWin::AnimationEffect::Opacity:      return "Opacity";
    case KWin::AnimationEffect::Brightness:   return "Brightness";
    case KWin::AnimationEffect::Saturation:   return "Saturation";
    case KWin::AnimationEffect::Scale:        return "Scale";
    case KWin::AnimationEffect::Translation:  return "Translation";
    case KWin::AnimationEffect::Rotation:     return "Rotation";
    case KWin::AnimationEffect::Position:     return "Position";
    case KWin::AnimationEffect::Size:         return "Size";
    case KWin::AnimationEffect::Clip:         return "Clip";
    default:                                  return " ";
    }
}

QString AniData::debugInfo() const
{
    return "Animation: " + attributeString(attribute) + '\n' +
           "     From: " + from.toString() + '\n' +
           "       To: " + to.toString() + '\n' +
           "  Started: " + QString::number(AnimationEffect::clock() - startTime) + "ms ago\n" +
           " Duration: " + QString::number(duration) + "ms\n" +
           "   Passed: " + QString::number(time) + "ms\n" +
           " Applying: " + QString::number(windowType) + '\n';
}
