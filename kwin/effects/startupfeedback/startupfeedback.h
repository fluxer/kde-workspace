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
#include <QCursor>
#include <kwineffects.h>
#include <KStartupInfo>

namespace KWin
{

class StartupEventNotifier : public QWidget
{
    Q_OBJECT
public:
    StartupEventNotifier(xcb_window_t window);
    ~StartupEventNotifier();

Q_SIGNALS:
    void interrupt();

protected:
    bool x11Event(XEvent *xevent) final;

private:
    xcb_window_t m_window;
};

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

    void start();
    void stop();

private:
    enum FeedbackType {
        NoFeedback = 0,
        PassiveFeedback = 1
    };

    KStartupInfo* m_startupInfo;
    int m_startups;
    StartupEventNotifier* m_notifier;
    FeedbackType m_type;
    QCursor m_cursor;
};
} // namespace

#endif
