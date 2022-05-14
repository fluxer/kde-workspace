/*  This file is part of the Kate project.
 *
 *  Copyright (C) 2016 Ivailo Monev <xakepa10@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */ 

#include "ui_kateprojectnew.h"
#include "kateprojectnew.h"

#include <KMessageBox>
#include <KLocale>
#include <QDialog>

KateProjectNew::KateProjectNew(QWidget *parent)
  : QDialog(parent)
{
    m_ui = new Ui_KateProjectNew();
    m_ui->setupUi(this);

    connect(m_ui->projectPath, SIGNAL(textChanged(QString)), this, SLOT(slotPath(QString)));
    connect(m_ui->okButton, SIGNAL(clicked()), this, SLOT(slotOk()));
    connect(m_ui->cancelButton, SIGNAL(clicked()), this, SLOT(slotCancel()));
}

KateProjectNew::~KateProjectNew()
{
    delete m_ui;
}

QByteArray KateProjectNew::getPathType(QString path)
{
    QDir gitdir(path + QLatin1String("/.git"));
    QDir hgdir(path + QLatin1String("/.hg"));
    QDir svndir(path + QLatin1String("/.svn"));
    if (gitdir.exists()) {
        return "git";
    } else if (hgdir.exists()) {
        return "hg";
    } else if (svndir.exists()) {
        return "svn";
    }
    return "unknown";
}

void KateProjectNew::slotPath(QString path)
{
    // path can be empty on close
    if (!path.isEmpty()) {
      m_ui->okButton->setEnabled(true);
      if (getPathType(path) == "unknown") {
          m_ui->okButton->setEnabled(false);
          KMessageBox::error(this, i18n("Path is not managed with Version Control System"), i18n("Invalid project path"));
      } else {
          QDir projectdir(path);
          m_ui->projectName->setText(projectdir.dirName());
      }
    }
}

void KateProjectNew::slotOk()
{
    QFile kateproject(m_ui->projectPath->text() + QLatin1String("/.kateproject"));

    if (kateproject.exists()) {
        int result = KMessageBox::questionYesNo(this,
          i18n("Project already exists, overwrite?"), i18n("Project exists"));
        if (result !=  KMessageBox::Yes) {
            return;
        }
    }

    // not very elegant but it will do
    QByteArray projectdata("{\n\t\"name\": \"#PROJECTNAME#\" ,\n\t\"files\": [ { \"#PROJECTTYPE#\": 1 } ]\n}");
    projectdata.replace("#PROJECTNAME#", m_ui->projectName->text().toUtf8());
    projectdata.replace("#PROJECTTYPE#", getPathType(m_ui->projectPath->text()));

    kateproject.open(QIODevice::WriteOnly);
    int byteswritten = kateproject.write(projectdata);
    if (byteswritten <= 0) {
        KMessageBox::error(this, i18n("Could not write project file"), i18n("Project creation failed"));
    }
    kateproject.close();
    emit projectCreated(kateproject.fileName());

    slotCancel();
}

void KateProjectNew::slotCancel()
{
    m_ui->projectName->clear();
    m_ui->projectPath->clear();
    hide();
}

#include "moc_kateprojectnew.cpp"
