/*****************************************************************
ksmserver - the KDE session management server

Copyright 2000 Matthias Ettrich <ettrich@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef SHUTDOWNDLG_H
#define SHUTDOWNDLG_H

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QEventLoop>
#include <QTimer>
#include <Plasma/Dialog>
#include <Plasma/Label>
#include <Plasma/Separator>
#include <Plasma/IconWidget>
#include <Plasma/PushButton>
#include <kworkspace/kworkspace.h>

// The methods that make the desktop gray if compositor is active.
class KSMShutdownFeedback
{
public:
    static void start();
    static void stop();
};

// The confirmation dialog
class KSMShutdownDlg : public Plasma::Dialog
{
    Q_OBJECT
public:
    static bool confirmShutdown(bool maysd, bool choose, KWorkSpace::ShutdownType &sdtype);

public Q_SLOTS:
    void slotLogout();
    void slotHalt();
    void slotReboot();
    void slotOk();
    void slotCancel();
    void slotTimeout();

protected:
    // Plasma::Dialog reimplementations
    void hideEvent(QHideEvent *event) final;
    bool eventFilter(QObject *watched, QEvent *event) final;

private:
    KSMShutdownDlg(QWidget *parent, bool maysd, bool choose, KWorkSpace::ShutdownType sdtype);
    ~KSMShutdownDlg();

    bool execDialog();
    void interrupt();

    QGraphicsScene* m_scene;
    QGraphicsWidget* m_widget;
    QGraphicsGridLayout* m_layout;
    Plasma::Label* m_titlelabel;
    Plasma::Separator* m_separator;
    Plasma::IconWidget* m_logoutwidget;
    Plasma::IconWidget* m_rebootwidget;
    Plasma::IconWidget* m_haltwidget;
    Plasma::PushButton* m_okbutton;
    Plasma::PushButton* m_cancelbutton;
    QEventLoop* m_eventloop;
    QTimer* m_timer;
    int m_second;
    KWorkSpace::ShutdownType m_shutdownType;
};

#endif // SHUTDOWNDLG_H
