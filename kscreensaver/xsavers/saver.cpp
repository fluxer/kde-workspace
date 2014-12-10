#include <kapplication.h>
#include <kglobal.h>
#include <k3process.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <X11/Xlib.h>
#include <QX11Info>

#include "saver.h"
#include "saver.moc"

//-----------------------------------------------------------------------------
kScreenSaver::kScreenSaver(Drawable drawable) : QObject()
{
	Window root;
	int ai;
	unsigned int au;

	mDrawable = drawable;
	mGc = XCreateGC(QX11Info::display(), mDrawable, 0, 0);
	XGetGeometry(QX11Info::display(), mDrawable, &root, &ai, &ai,
		&mWidth, &mHeight, &au, &au); 
}

kScreenSaver::~kScreenSaver()
{
	XFreeGC(QX11Info::display(), mGc);
}

//-----------------------------------------------------------------------------


