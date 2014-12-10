//-----------------------------------------------------------------------------
//
// Screen savers for KDE
//
// Copyright (c)  Martin R. Jones 1999
//

#ifndef __DEMOWIN_H__
#define __DEMOWIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <QKeyEvent>
#include <QWidget>

//----------------------------------------------------------------------------

class DemoWindow : public QWidget
{
    Q_OBJECT
public:
    DemoWindow() : QWidget()
    {
        setFixedSize(600, 420);
    }

protected:
    virtual void keyPressEvent(QKeyEvent *e)
    {
        if (e->text() == QLatin1String( "q" ))
        {
            kapp->quit();
        }
    }
};

#endif

