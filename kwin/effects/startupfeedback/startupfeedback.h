/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

 Copyright (C) 2010 Martin Gräßlin <mgraesslin@kde.org>

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

#ifndef KWIN_STARTUPFEEDBACK_H
#define KWIN_STARTUPFEEDBACK_H
#include <QObject>
#include <kwineffects.h>
#include <KStartupInfo>

class KSelectionOwner;
namespace KWin
{

class StartupFeedbackEffect
    : public Effect
{
    Q_OBJECT
public:
    StartupFeedbackEffect();
    virtual ~StartupFeedbackEffect();

    virtual void reconfigure(ReconfigureFlags flags);
    virtual bool isActive() const;

private Q_SLOTS:
    void gotNewStartup(const KStartupInfoId& id, const KStartupInfoData& data);
    void gotRemoveStartup(const KStartupInfoId& id, const KStartupInfoData& data);
    void gotStartupChange(const KStartupInfoId& id, const KStartupInfoData& data);

private:
    enum FeedbackType {
        NoFeedback = 0,
        PassiveFeedback = 1
    };
    void start(const QString& icon);
    void stop();

    KStartupInfo* m_startupInfo;
    KSelectionOwner* m_selection;
    KStartupInfoId m_currentStartup;
    QMap< KStartupInfoId, QString > m_startups; // QString == pixmap
    bool m_active;
    FeedbackType m_type;
};
} // namespace

#endif
