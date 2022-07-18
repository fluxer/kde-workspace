/***************************************************************************
 *   Copyright 2022 by Chaz Peloquin <chazpelo2@gmail.com>                 *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef SANGRIA_H
#define SANGRIA_H

#include <QtGui/qaction.h>
#include <QtCore/qprocess.h>
#include <QtGui/qgraphicsview.h>
#include <KMenu>
#include <KFileItem>
#include <KDirLister>

#include <Plasma/Applet>

#include <QAction>

class KCModuleProxy;
class KDialog;
class KFilePlacesModel;
class EmptyGraphicsItem;

namespace Plasma
{
    class IconWidget;
}

class SangriaMenu : public Plasma::Applet
{
    Q_OBJECT
    public:
        SangriaMenu(QObject *parent, const QVariantList &args);
        virtual QList<QAction*> contextualActions();
        ~SangriaMenu();

        void init();
        void constraintsEvent(Plasma::Constraints constraints);

    protected:
        void createMenu();
        void updateIcon();
        
        QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const;

    protected Q_SLOTS:
        void startLogout();
        void logout();
        
    protected slots:
        void popup();
        void open();

    private slots:
        void iconSizeChanged(int group);

    private:
        void adjustToolBackerGeometry();
        Plasma::IconWidget* m_icon;
        QList<QAction*> actions;
        KDirLister *m_dirLister;
        KMenu m_menu;
        EmptyGraphicsItem *m_toolBacker;
        QAction *m_emptyAction;
        QWeakPointer<KDialog> m_confirmEmptyDialog;
        int m_count;
        bool m_showText;
        KFilePlacesModel *m_places;
        KCModuleProxy *m_proxy;
        QProcess *m_emptyProcess;
};

K_EXPORT_PLASMA_APPLET(sangriamenu, SangriaMenu)

#endif
