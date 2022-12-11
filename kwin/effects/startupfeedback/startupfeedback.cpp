/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

 Copyright (C) 2019 Ivailo Monev <xakepa10@gmail.com>

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
#include <KStartupInfo>

namespace KWin
{

StartupFeedbackEffect::StartupFeedbackEffect()
    : m_startupInfo(new KStartupInfo(KStartupInfo::CleanOnCantDetect, this))
    , m_startups(0)
    , m_active(false)
    , m_type(PassiveFeedback)
    , m_cursor(Qt::WaitCursor)
{
    reconfigure(ReconfigureAll);

    connect(m_startupInfo, SIGNAL(gotNewStartup(KStartupInfoId,KStartupInfoData)), SLOT(gotNewStartup(KStartupInfoId,KStartupInfoData)));
    connect(m_startupInfo, SIGNAL(gotRemoveStartup(KStartupInfoId,KStartupInfoData)), SLOT(gotRemoveStartup(KStartupInfoId,KStartupInfoData)));
}

StartupFeedbackEffect::~StartupFeedbackEffect()
{
}

void StartupFeedbackEffect::reconfigure(Effect::ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    const bool oldactive = m_active;
    if (oldactive) {
        stop();
    }

    KConfig conf("klaunchrc", KConfig::NoGlobals);
    KConfigGroup c = conf.group("FeedbackStyle");
    const bool busyCursor = c.readEntry("BusyCursor", true);
    c = conf.group("BusyCursorSettings");
    const int timeout = c.readEntry("Timeout", 10);
    m_startupInfo->setTimeout(timeout);
    if (!busyCursor) {
        m_type = NoFeedback;
    } else {
        m_type = PassiveFeedback;
    }
    if (oldactive) {
        start();
    }
}

void StartupFeedbackEffect::gotNewStartup(const KStartupInfoId& id, const KStartupInfoData& data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);

    m_startups++;
    if (!m_active) {
        start();
    }
}

void StartupFeedbackEffect::gotRemoveStartup(const KStartupInfoId& id, const KStartupInfoData& data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);

    m_startups--;
    if (m_startups <= 0) {
        m_startups = 0;
        stop();
    }
}

void StartupFeedbackEffect::start()
{
    if (m_type == NoFeedback) {
        return;
    }

    xcb_connection_t *c = connection();
    ScopedCPointer<xcb_grab_pointer_reply_t> grabPointer(
        xcb_grab_pointer_reply(
            c,
            xcb_grab_pointer_unchecked(c, false, rootWindow(),
                XCB_EVENT_MASK_NO_EVENT,
                XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_WINDOW_NONE,
                m_cursor.handle(), XCB_TIME_CURRENT_TIME),
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
            return;
        default:
            break; // impossible
    }
}

bool StartupFeedbackEffect::isActive() const
{
    return m_active;
}

} // namespace
