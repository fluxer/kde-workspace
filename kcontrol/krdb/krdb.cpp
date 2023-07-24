/****************************************************************************
**
** Copyright (C) 2023 by Ivailo Monev <xakepa10@gmail.com>
** This application is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <QProcess>
#include <kdebug.h>

void runRdb()
{
    const int krdbstatus = QProcess::execute(QString::fromLatin1("krdb"));
    if (krdbstatus != 0) {
        kWarning() << "krdb exited abnormally" << krdbstatus;
    }
}

