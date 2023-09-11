/*
 *   Copyright (C) 2007 by Ivan Cukic <ivan.cukic+kde@gmail.com>
 *   Copyright (C) 2009 by Ana Cecília Martins <anaceciliamb@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef WIDGETEXPLORER_H
#define WIDGETEXPLORER_H

#include <QGraphicsWidget>
#include <QGraphicsItem>

#include <Plasma/Corona>
#include <Plasma/Containment>

#include "plasmagenericshell_export.h"

namespace Plasma
{

class WidgetExplorerPrivate;

class PLASMAGENERICSHELL_EXPORT WidgetExplorer : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit WidgetExplorer(Plasma::Location loc, QGraphicsItem *parent = nullptr);
    explicit WidgetExplorer(QGraphicsItem *parent = nullptr);
    ~WidgetExplorer();

    /**
     * Sets the containment to add applets to
     */
    void setContainment(Plasma::Containment *containment);
    /**
     * @return the containment to add applets to
     */
    Containment *containment() const;

    void setLocation(const Plasma::Location loc);
    Plasma::Location location() const;

Q_SIGNALS:
    void locationChanged(Plasma::Location loc);
    void closeClicked();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Q_PRIVATE_SLOT(d, void _k_appletAdded(Plasma::Applet*))
    Q_PRIVATE_SLOT(d, void _k_appletRemoved(Plasma::Applet*))
    Q_PRIVATE_SLOT(d, void _k_containmentDestroyed())
    Q_PRIVATE_SLOT(d, void _k_immutabilityChanged(Plasma::ImmutabilityType))
    Q_PRIVATE_SLOT(d, void _k_textChanged(QString))
    Q_PRIVATE_SLOT(d, void _k_closePressed())
    Q_PRIVATE_SLOT(d, void _k_databaseChanged(QStringList))

    WidgetExplorerPrivate * const d;
    friend WidgetExplorerPrivate;
};

} // namespace Plasma

#endif // WIDGETEXPLORER_H
