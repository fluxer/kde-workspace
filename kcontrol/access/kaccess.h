#ifndef __K_ACCESS_H__
#define __K_ACCESS_H__

#include <QWidget>
#include <QColor>
#include <QLabel>
#include <QEvent>

#include <KUniqueApplication>
#include <KUrl>
#include <QTextStream> // so that it can define Status
#include <KMediaPlayer>

#include <X11/Xlib.h>
#define explicit int_explicit        // avoid compiler name clash in XKBlib.h
#include <X11/XKBlib.h>
#undef explicit

class KDialog;
class KComboBox;

class KAccessApp : public KUniqueApplication
{
  Q_OBJECT

public:

  explicit KAccessApp(bool allowStyles=true);

  bool x11EventFilter(XEvent *event);

  int newInstance();

  void setXkbOpcode(int opcode);

protected:

  void readSettings();

  void xkbStateNotify();
  void xkbBellNotify(XkbBellNotifyEvent *event);
  void xkbControlsNotify(XkbControlsNotifyEvent *event);


private Q_SLOTS:

  void activeWindowChanged(WId wid);
  void notifyChanges();
  void applyChanges();
  void yesClicked();
  void noClicked();
  void dialogClosed();


private:
   void  createDialogContents();
   void  initMasks();

  int xkb_opcode;
  unsigned int features;
  unsigned int requestedFeatures;

  bool    _systemBell, _artsBell, _visibleBell, _visibleBellInvert;
  QColor  _visibleBellColor;
  int     _visibleBellPause;

  bool    _gestures, _gestureConfirmation;
  bool    _kNotifyModifiers, _kNotifyAccessX;

  QWidget *overlay;

  KAudioPlayer *_player;
  QString _currentPlayerSource;

  WId _activeWindow;

  KDialog *dialog;
  QLabel *featuresLabel;
  KComboBox *showModeCombobox;

  int keys[8];
  int state;
};


class VisualBell : public QWidget
{
  Q_OBJECT

public:

  VisualBell(int pause)
    : QWidget(( QWidget* )0, Qt::X11BypassWindowManagerHint), _pause(pause)
    {}


protected:

  void paintEvent(QPaintEvent *);


private:

  int _pause;

};




#endif
