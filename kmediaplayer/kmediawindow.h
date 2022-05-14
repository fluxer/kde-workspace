/*  This file is part of the KDE libraries
    Copyright (C) 2016 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KMEDIAWINDOW_H
#define KMEDIAWINDOW_H

#include <KXmlGuiWindow>
#include <KMediaWidget>
#include <KRecentFilesAction>
#include <KConfig>
#include <QMenu>

class KMediaWindow: public KXmlGuiWindow
{
    Q_OBJECT
public:
    KMediaWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~KMediaWindow();

public slots:
    void openPath();
    void openURL();
    void openURL(KUrl url);
    void closePath();
    void fullscreen();
    void configure();
    void menubar();
    void menu(QPoint position);
    void quit();

private slots:
    void hideMenuBar(bool hidden);

protected:
    virtual void showEvent(QShowEvent *event);
    virtual bool eventFilter(QObject *object, QEvent *event);

private:
    KConfig *m_config;
    KMediaWidget *m_player;
    KRecentFilesAction *m_recentfiles;
    QMenu *m_menu;
    bool m_menuvisible;
    bool m_toolvisible;
    bool m_statusvisible;
};

#endif // KMEDIAWINDOW_H
