#include "kaccess.h"
#include <kdebug.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

int main(int argc, char * argv[] )
{
  KAboutData about(I18N_NOOP("kaccess"), 0, ki18n("KDE Accessibility Tool"),
                  0, KLocalizedString(), KAboutData::License_GPL,
                  ki18n("(c) 2000, Matthias Hoelzer-Kluepfel"));

  about.addAuthor(ki18n("Matthias Hoelzer-Kluepfel"), ki18n("Author") , "hoelzer@kde.org");

  KCmdLineArgs::init( argc, argv, &about );

  if (!KAccessApp::start())
    return 0;

  // verify the Xlib has matching XKB extension
  int major = XkbMajorVersion;
  int minor = XkbMinorVersion;
  if (!XkbLibraryVersion(&major, &minor))
    {
      kError() << "Xlib XKB extension does not match";
      return 1;
    }
  kDebug() << "Xlib XKB extension major=" << major << " minor=" << minor;

  // we need an application object for QX11Info
  KAccessApp app;

  // verify the X server has matching XKB extension
  // if yes, the XKB extension is initialized
  int opcode_rtrn;
  int error_rtrn;
  int xkb_opcode;
  if (!XkbQueryExtension(QX11Info::display(), &opcode_rtrn, &xkb_opcode, &error_rtrn,
			 &major, &minor))
    {
      kError() << "X server has not matching XKB extension";
      return 1;
    }
  kDebug() << "X server XKB extension major=" << major << " minor=" << minor;

  //Without that, the application dies when the dialog is closed only once.
  app.setQuitOnLastWindowClosed(false);
  
  app.setXkbOpcode(xkb_opcode);
  app.disableSessionManagement();
  return app.exec();
}
