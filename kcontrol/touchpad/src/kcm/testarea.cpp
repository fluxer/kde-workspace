/*
 * Copyright (C) 2013 Alexander Mezin <mezin.alexander@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "testarea.h"

#include <QtGui/qevent.h>
#include <QtGui/qevent.h>
#include <QScopedPointer>
#include <QStandardItemModel>

#include <KLocale>
#include <KIcon>
#include <Plasma/Theme>

TestArea::TestArea(QWidget *parent) : QWidget(parent)
{
    m_ui.setupUi(this);

    QStandardItemModel *model = new QStandardItemModel(m_ui.listView);
    m_ui.listView->setModel(model);
    QScopedPointer<QStandardItem> item(
                new QStandardItem(KIcon("folder"), i18n("Drag me")));
    model->appendRow(item.data());
    item.take();

    QString wallpaper = Plasma::Theme::defaultTheme()->wallpaperPath();
    static const QString stylesheet("background-image: url(%1)");
    m_ui.scrollAreaWidgetContents->setStyleSheet(stylesheet.arg(wallpaper));
}

void TestArea::enterEvent(QEvent *e)
{
    Q_EMIT enter();
    QWidget::enterEvent(e);
}

void TestArea::leaveEvent(QEvent *e)
{
    Q_EMIT leave();
    QWidget::leaveEvent(e);
}
