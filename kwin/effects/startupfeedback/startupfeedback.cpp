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
#include "startupfeedback.h"
#include "client.h"
// Qt
#include <QCursor>
// KDE
#include <KConfigGroup>
#include <KDebug>
#include <KGlobal>
#include <KIconLoader>
#include <KStandardDirs>
#include <KStartupInfo>

// based on StartupId in KRunner by Lubos Lunak
// Copyright (C) 2001 Lubos Lunak <l.lunak@kde.org>
// Copyright (C) 2019 Ivailo Monev <xakepa10@gmail.com>

namespace KWin
{

StartupFeedbackEffect::StartupFeedbackEffect()
    : m_startupInfo(new KStartupInfo(KStartupInfo::CleanOnCantDetect, this))
    , m_active(false)
    , m_type(PassiveFeedback)
{
    connect(m_startupInfo, SIGNAL(gotNewStartup(KStartupInfoId,KStartupInfoData)), SLOT(gotNewStartup(KStartupInfoId,KStartupInfoData)));
    connect(m_startupInfo, SIGNAL(gotRemoveStartup(KStartupInfoId,KStartupInfoData)), SLOT(gotRemoveStartup(KStartupInfoId,KStartupInfoData)));
    connect(m_startupInfo, SIGNAL(gotStartupChange(KStartupInfoId,KStartupInfoData)), SLOT(gotStartupChange(KStartupInfoId,KStartupInfoData)));
    reconfigure(ReconfigureAll);
}

StartupFeedbackEffect::~StartupFeedbackEffect()
{
}

void StartupFeedbackEffect::reconfigure(Effect::ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    KConfig conf("klaunchrc", KConfig::NoGlobals);
    KConfigGroup c = conf.group("FeedbackStyle");
    const bool busyCursor = c.readEntry("BusyCursor", true);

    c = conf.group("BusyCursorSettings");
    m_startupInfo->setTimeout(c.readEntry("Timeout", 10));
    if (!busyCursor) {
        m_type = NoFeedback;
    } else {
        m_type = PassiveFeedback;
    }
    if (m_active) {
        stop();
        start(m_startups[ m_currentStartup ]);
    }
}

void StartupFeedbackEffect::gotNewStartup(const KStartupInfoId& id, const KStartupInfoData& data)
{
    const QString& icon = data.findIcon();
    m_currentStartup = id;
    m_startups[ id ] = icon;
    start(icon);
}

void StartupFeedbackEffect::gotRemoveStartup(const KStartupInfoId& id, const KStartupInfoData& data)
{
    Q_UNUSED( data )
    m_startups.remove(id);
    if (m_startups.count() == 0) {
        m_currentStartup = KStartupInfoId(); // null
        stop();
        return;
    }
    m_currentStartup = m_startups.begin().key();
    start(m_startups[ m_currentStartup ]);
}

void StartupFeedbackEffect::gotStartupChange(const KStartupInfoId& id, const KStartupInfoData& data)
{
    if (m_currentStartup == id) {
        const QString& icon = data.findIcon();
        if (!icon.isEmpty() && icon != m_startups[ m_currentStartup ]) {
            m_startups[ id ] = icon;
            start(icon);
        }
    }
}

void StartupFeedbackEffect::start(const QString& icon)
{
    Q_UNUSED(icon);
    if (m_type == NoFeedback) {
        return;
    }

    QCursor cursor = QCursor(Qt::WaitCursor);

    xcb_connection_t *c = connection();
    ScopedCPointer<xcb_grab_pointer_reply_t> grabPointer(
        xcb_grab_pointer_reply(
            c,
            xcb_grab_pointer_unchecked(c, false, rootWindow(),
                XCB_EVENT_MASK_NO_EVENT,
                XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_WINDOW_NONE,
                cursor.handle(), XCB_TIME_CURRENT_TIME),
            NULL
        )
    );
    if (grabPointer.isNull() || grabPointer->status != XCB_GRAB_STATUS_SUCCESS) {
        kWarning() << "could not grab pointer";
        m_active = false;
        return;
    }

    m_active = true;
}

void StartupFeedbackEffect::stop()
{
    switch(m_type) {
        case PassiveFeedback:
            if (m_active) {
                xcb_ungrab_pointer(connection(), XCB_TIME_CURRENT_TIME);
                m_active = false;
            }
            break;
        case NoFeedback:
            return; // don't want the full repaint
        default:
            break; // impossible
    }
}

bool StartupFeedbackEffect::isActive() const
{
    return m_active;
}

} // namespace
