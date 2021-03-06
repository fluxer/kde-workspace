/* Slide Show Screen Saver
 *  (C) 1999 Stefan Taferner <taferner@kde.org>
 *  (C) 2001 Martin R. Jones <mjones@kde.org>
 *  (C) 2003 Chris Howells <howells@kde.org>
 *  (C) 2003 Sven Leiber <s.leiber@web.de>
 *
 * This code is under GPL
 *
 * 2001/03/04 Converted to libkscreensaver by Martin R. Jones.
 */


#include <QDir>
#include <QColor>
#include <QLabel>
#include <QLayout>
#include <QFile>
#include <QFileInfo>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDesktopWidget>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPolygon>


#include <kconfig.h>
#include <kglobal.h>
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <kaboutdata.h>
#include <kaboutapplicationdialog.h>
#include <kdebug.h>
#include <krandom.h>

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#ifdef HAVE_KEXIV2
#include <libkexiv2/kexiv2.h>
#endif

#include "slideshow.h"
#include "moc_slideshow.cpp"


#define SLIDESHOW_VERSION "2.3.0"
static const char version[] = SLIDESHOW_VERSION;
static const char description[] = I18N_NOOP("KSlideshow");

static KAboutData* s_aboutData = 0;

// libkscreensaver interface
class KSlideShowSaverInterface : public KScreenSaverInterface
{
public:
    virtual KAboutData* aboutData() {
        return s_aboutData;
    }

    virtual KScreenSaver* create( WId id )
    {
        return new kSlideShowSaver( id );
    }

    virtual QDialog* setup()
    {
        return new kSlideShowSetup();
    }
};

int main( int argc, char *argv[] )
{
    s_aboutData = new KAboutData( "kslideshow.kss", "klock", ki18n("SlideShow"),
                                  version, ki18n(description), KAboutData::License_GPL,
                                  ki18n("(c) 1999-2003, The KDE Team") );
    s_aboutData->addAuthor(ki18n("Stefan Taferner"), KLocalizedString(), "taferner@kde.org");
    s_aboutData->addAuthor(ki18n("Chris Howells"), KLocalizedString(), "howells@kde.org");
    s_aboutData->addAuthor(ki18n("Sven Leiber"), KLocalizedString(), "s.leiber@web.de");

    KSlideShowSaverInterface kss;
    return kScreenSaverMain( argc, argv, kss );
}


//=============================================================================
//  Class kSlideShowSaver
//=============================================================================
kSlideShowSaver::kSlideShowSaver( WId id ): KScreenSaver(id)
{
  mEffect = NULL;
  mNumEffects = 0;
  mIntArray = NULL;
  readConfig();
  registerEffects();

  initNextScreen();

  mFileIdx = 0;

  mEffectRunning = false;

  mTimer.setSingleShot(true);
  connect(&mTimer, SIGNAL(timeout()), SLOT(update()));
  mTimer.start(10);

  QDesktopWidget *d = QApplication::desktop();
  if(geometry() == d->geometry() && d->screenCount() > 1)
  {
    for(int i = 0; i < d->screenCount(); ++i)
    {
      QRect s = d->screenGeometry(i);
      mGeoList.append(mScreenGeo(s.width(), s.height(), s.topLeft().x(), s.topLeft().y()));
    }
  }
  else
  {
    mGeoList.append(mScreenGeo(width(), height(), 0, 0));
  }
  setAttribute(Qt::WA_NoSystemBackground);
  createNextScreen();
  show();
}



//----------------------------------------------------------------------------
kSlideShowSaver::~kSlideShowSaver()
{
  delete [] mIntArray;
  delete [] mEffectList;
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::initNextScreen()
{
  int w, h;

  w = width();
  h = height();
  mNextScreen = QPixmap(w, h);
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::readConfig()
{
  KConfigGroup config(KGlobal::config(), "Settings");
  mShowRandom = config.readEntry("ShowRandom", true);
  mZoomImages = config.readEntry("ZoomImages", false);
  mPrintName = config.readEntry("PrintName", true);
  mPrintPath = config.readEntry("PrintPath", false);
  mDirectory = config.readPathEntry("Directory", KGlobal::dirs()->findDirs("wallpaper", QLatin1String( "" )).last());
  mDelay = config.readEntry("Delay", 10) * 1000;
  mSubdirectory = config.readEntry("SubDirectory", false);
  mRandomPosition = config.readEntry("RandomPosition", false);
  mEffectsEnabled = config.readEntry("EffectsEnabled", true);

  loadDirectory();
}


//----------------------------------------------------------------------------
void kSlideShowSaver::registerEffects()
{
  if (!mEffectsEnabled)
  {
    mEffectList = 0;
    return;
  }

  int i = 0;

  mEffectList = new EffectMethod[64];
//  mEffectList[i++] = &kSlideShowSaver::effectChessboard;
  mEffectList[i++] = &kSlideShowSaver::effectMultiCircleOut;
  mEffectList[i++] = &kSlideShowSaver::effectSpiralIn;
  mEffectList[i++] = &kSlideShowSaver::effectSweep;
//  mEffectList[i++] = &kSlideShowSaver::effectMeltdown;
  mEffectList[i++] = &kSlideShowSaver::effectCircleOut;
  mEffectList[i++] = &kSlideShowSaver::effectBlobs;
//  mEffectList[i++] = &kSlideShowSaver::effectHorizLines;
//  mEffectList[i++] = &kSlideShowSaver::effectVertLines;
//  mEffectList[i++] = &kSlideShowSaver::effectRandom;
  mEffectList[i++] = &kSlideShowSaver::effectGrowing;
  mEffectList[i++] = &kSlideShowSaver::effectIncomingEdges;

  mNumEffects = i;
  // mNumEffects = 1;  //...for testing
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectMultiCircleOut(bool aInit)
{
  int x, y, i;
  double alpha;
  static QPolygon pa(4);

  if (aInit)
  {
    mw = width();
    mh = height();
    mx = mw;
    my = mh>>1;
    pa.setPoint(0, mw>>1, mh>>1);
    pa.setPoint(3, mw>>1, mh>>1);
    mfy = sqrt((double)mw*mw + mh*mh) / 2;
    mi  = KRandom::random()%15 + 2;
    mfd = M_PI*2/mi;
    mAlpha = mfd;
    mwait = 10 * mi;
    mfx = M_PI/32;  // divisor must be powers of 8
  }

  if (mAlpha < 0)
  {
    showNextScreen();
    return -1;
  }

  QPainter p(this);
  QBrush brush;
  brush.setTexture(mNextScreen);
  p.setBrush(brush);
  p.setPen(Qt::NoPen);

  for (alpha=mAlpha, i=mi; i>=0; i--, alpha+=mfd)
  {
    x = (mw>>1) + (int)(mfy * cos(-alpha));
    y = (mh>>1) + (int)(mfy * sin(-alpha));

    mx = (mw>>1) + (int)(mfy * cos(-alpha + mfx));
    my = (mh>>1) + (int)(mfy * sin(-alpha + mfx));

    pa.setPoint(1, x, y);
    pa.setPoint(2, mx, my);

    p.drawPolygon(pa);
  }
  mAlpha -= mfx;

  return mwait;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectSpiralIn(bool aInit)
{
  if (aInit)
  {
    mw = width();
    mh = height();
    mix = mw / 8;
    miy = mh / 8;
    mx0 = 0;
    mx1 = mw - mix;
    my0 = miy;
    my1 = mh - miy;
    mdx = mix;
    mdy = 0;
    mi = 0;
    mj = 16 * 16;
    mx = 0;
    my = 0;
  }

  if (mi==0 && mx0>=mx1)
  {
    showNextScreen();
    return -1;
  }

  if (mi==0 && mx>=mx1) // switch to: down on right side
  {
    mi = 1;
    mdx = 0;
    mdy = miy;
    mx1 -= mix;
  }
  else if (mi==1 && my>=my1) // switch to: right to left on bottom side
  {
    mi = 2;
    mdx = -mix;
    mdy = 0;
    my1 -= miy;
  }
  else if (mi==2 && mx<=mx0) // switch to: up on left side
  {
    mi = 3;
    mdx = 0;
    mdy = -miy;
    mx0 += mix;
  }
  else if (mi==3 && my<=my0) // switch to: left to right on top side
  {
    mi = 0;
    mdx = mix;
    mdy = 0;
    my0 += miy;
  }

  QPainter p(this);
  p.drawPixmap(mx, my, mNextScreen, mx, my, mix, miy);

  mx += mdx;
  my += mdy;
  mj--;

  return 8;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectMeltdown(bool aInit)
{
  int i, x, y;
  bool done;

  if (aInit)
  {
    delete [] mIntArray;
    mw = width();
    mh = height();
    mdx = 4;
    mdy = 16;
    mix = mw / mdx;
    mIntArray = new int[mix];
    for (i=mix-1; i>=0; i--)
      mIntArray[i] = 0;
  }

  done = true;

  QPainter p(this);
  for (i=0,x=0; i<mix; i++,x+=mdx)
  {
    y = mIntArray[i];
    if (y >= mh) continue;
    done = false;
    if ((KRandom::random()&15) < 6) continue;
    p.drawPixmap(x, y, mNextScreen, x, y, mdx, mdy);
    mIntArray[i] += mdy;
  }

  if (done)
  {
    delete [] mIntArray;
    mIntArray = NULL;
    return -1;
  }

  return 15;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectCircleOut(bool aInit)
{
  int x, y;
  static QPolygon pa(4);

  if (aInit)
  {
    mw = width();
    mh = height();
    mx = mw;
    my = mh>>1;
    mAlpha = 2*M_PI;
    pa.setPoint(0, mw>>1, mh>>1);
    pa.setPoint(3, mw>>1, mh>>1);
    mfx = M_PI/16;  // divisor must be powers of 8
    mfy = sqrt((double)mw*mw + mh*mh) / 2;
  }

  if (mAlpha < 0)
  {
    showNextScreen();
    return -1;
  }

  x = mx;
  y = my;
  mx = (mw>>1) + (int)(mfy * cos(mAlpha));
  my = (mh>>1) + (int)(mfy * sin(mAlpha));
  mAlpha -= mfx;

  pa.setPoint(1, x, y);
  pa.setPoint(2, mx, my);

  QBrush brush;
  brush.setTexture(mNextScreen);
  QPainter p(this);
  p.setBrush(brush);
  p.setPen(Qt::NoPen);

  p.drawPolygon(pa);

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectSweep(bool aInit)
{
  int w, h, x, y, i;

  if (aInit)
  {
    // subtype: 0=sweep right to left, 1=sweep left to right
    //          2=sweep bottom to top, 3=sweep top to bottom
    mSubType = KRandom::random() % 4;
    mw  = width();
    mh  = height();
    mdx = (mSubType==1 ? 16 : -16);
    mdy = (mSubType==3 ? 16 : -16);
    mx  = (mSubType==1 ? 0 : mw);
    my  = (mSubType==3 ? 0 : mh);
  }

  QPainter p(this);
  if (mSubType==0 || mSubType==1)
  {
    // horizontal sweep
    if ((mSubType==0 && mx < -64) ||
	(mSubType==1 && mx > mw+64))
    {
       return -1;
    }
    for (w=2,i=4,x=mx; i>0; i--, w<<=1, x-=mdx)
    {
      p.drawPixmap(x, 0, mNextScreen, x, 0, w, mh);
    }
    mx += mdx;
  }
  else
  {
    // vertical sweep
    if ((mSubType==2 && my < -64) ||
	(mSubType==3 && my > mh+64))
    {
      return -1;
    }
    for (h=2,i=4,y=my; i>0; i--, h<<=1, y-=mdy)
    {
      p.drawPixmap(0, y, mNextScreen, 0, y, mw, h);
    }
    my += mdy;
  }

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectBlobs(bool aInit)
{
  int r;

  if (aInit)
  {
    mAlpha = M_PI * 2;
    mw = width();
    mh = height();
    mi = 150;
  }

  if (mi <= 0)
  {
    showNextScreen();
    return -1;
  }

  r = (KRandom::random() % 100) + 100;
  mx = KRandom::random() % (mw + r);
  my = KRandom::random() % (mh + r);

  QBrush brush;
  brush.setTexture(mNextScreen);
  QPainter p(this);
  p.setBrush(brush);
  p.setPen(Qt::NoPen);
  p.drawEllipse(mx-r, my-r, r, r);

  mi--;

  return 10;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectRandom(bool /*aInit*/)
{
  int x, y, i, w, h, fact, sz;

  fact = (KRandom::random() % 3) + 1;

  w = width() >> fact;
  h = height() >> fact;
  sz = 1 << fact;

  QPainter p(this);
  for (i = (w*h)<<1; i > 0; i--)
  {
    x = (KRandom::random() % w) << fact;
    y = (KRandom::random() % h) << fact;
    p.drawPixmap(x, y, mNextScreen, x, y, sz, sz);
  }
  p.end();
  showNextScreen();

  return -1;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectGrowing(bool aInit)
{
  if (aInit)
  {
    mw = width();
    mh = height();
    mx = mw >> 1;
    my = mh >> 1;
    mi = 0;
    mfx = mx / 100.0;
    mfy = my / 100.0;
  }

  mx = (mw>>1) - (int)(mi * mfx);
  my = (mh>>1) - (int)(mi * mfy);
  mi++;

  if (mx<0 || my<0)
  {
    showNextScreen();
    return -1;
  }

  if((mw - (mx<<1) == 0) && (mh - (my<<1) == 0))
    return 1;

  QPainter p(this);
  p.drawPixmap(mx, my, mNextScreen, mx, my, mw - (mx<<1), mh - (my<<1));

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectChessboard(bool aInit)
{
  int y;

  if (aInit)
  {
    mw  = width();
    mh  = height();
    mdx = 8;         // width of one tile
    mdy = 8;         // height of one tile
    mj  = (mw+mdx-1)/mdx; // number of tiles
    mx  = mj*mdx;    // shrinking x-offset from screen border
    mix = 0;         // growing x-offset from screen border
    miy = 0;         // 0 or mdy for growing tiling effect
    my  = mj&1 ? 0 : mdy; // 0 or mdy for shrinking tiling effect
    mwait = 800 / mj; // timeout between effects
  }

  if (mix >= mw)
  {
    showNextScreen();
    return -1;
  }

  mix += mdx;
  mx  -= mdx;
  miy = miy ? 0 : mdy;
  my  = my ? 0 : mdy;

  QPainter p(this);
  for (y=0; y<mw; y+=(mdy<<1))
  {
    p.drawPixmap(mix, y+miy, mNextScreen, mix, y+miy, mdx, mdy);
    p.drawPixmap(mx, y+my, mNextScreen, mx, y+my, mdx, mdy);
  }

  return mwait;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectIncomingEdges(bool aInit)
{
  int x1, y1;

  if (aInit)
  {
    mw = width();
    mh = height();
    mix = mw >> 1;
    miy = mh >> 1;
    mfx = mix / 100.0;
    mfy = miy / 100.0;
    mi = 0;
    mSubType = KRandom::random() & 1;
  }

  mx = (int)(mfx * mi);
  my = (int)(mfy * mi);

  if (mx>mix || my>miy)
  {
    showNextScreen();
    return -1;
  }

  x1 = mw - mx;
  y1 = mh - my;
  mi++;

  if((mx==0) && (my==0))
    return 1; // otherwise drawPixmap draws the bottom-right of mNextScreen

  QPainter p(this);
  if (mSubType)
  {
    // moving image edges
    p.drawPixmap(0, 0, mNextScreen, mix-mx, miy-my, mx, my);
    p.drawPixmap(x1, 0, mNextScreen, mix, miy-my, mx, my);
    p.drawPixmap(0, y1, mNextScreen, mix-mx, miy, mx, my);
    p.drawPixmap(x1, y1, mNextScreen, mix, miy, mx, my);
  }
  else
  {
    // fixed image edges
    p.drawPixmap(0, 0, mNextScreen,  0,  0, mx, my);
    p.drawPixmap(x1, 0, mNextScreen, x1,  0, mx, my);
    p.drawPixmap(0, y1, mNextScreen,  0, y1, mx, my);
    p.drawPixmap(x1, y1, mNextScreen, x1, y1, mx, my);
  }
  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectHorizLines(bool aInit)
{
  static int iyPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
  int y;

  if (aInit)
  {
    mw = width();
    mh = height();
    mi = 0;
  }

  if (iyPos[mi] < 0) return -1;

  QPainter p(this);
  for (y=iyPos[mi]; y<mh; y+=8)
  {
    p.drawPixmap(0, y, mNextScreen, 0, y, mw, 1);
  }

  mi++;
  if (iyPos[mi] >= 0) return 160;
  return -1;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectVertLines(bool aInit)
{
  static int ixPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
  int x;

  if (aInit)
  {
    mw = width();
    mh = height();
    mi = 0;
  }

  if (ixPos[mi] < 0) return -1;

  QPainter p(this);
  for (x=ixPos[mi]; x<mw; x+=8)
  {
    p.drawPixmap(x, 0, mNextScreen, x, 0, 1, mh);
  }

  mi++;
  if (ixPos[mi] >= 0) return 160;
  return -1;
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::restart()
{
  mEffectRunning = false;
  mEffect = NULL;
  update();
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::paintEvent(QPaintEvent *)
{
  int tmout = -1;
  int i;

  if (mEffectRunning)
  {
    tmout = (this->*mEffect)(false);
  }
  else
  {
    loadNextImage();
    createNextScreen();
    if (mNumEffects == 0)
    {
      showNextScreen();
      tmout = -1;
    }
    else
    {
      if (mNumEffects > 1) i = KRandom::random() % mNumEffects;
      else i = 0;

      mEffect = mEffectList[i];
      mEffectRunning = true;
      tmout = (this->*mEffect)(true);
    }
  }
  if (tmout <= 0)
  {
    tmout = mDelay;
    mEffectRunning = false;
  }
  mTimer.start(tmout);
}


//----------------------------------------------------------------------------
void kSlideShowSaver::showNextScreen()
{
  QPainter p(this);
  p.drawPixmap(0, 0, mNextScreen, 0, 0, mNextScreen.width(), mNextScreen.height());
}


//----------------------------------------------------------------------------
void kSlideShowSaver::createNextScreen()
{
  QPainter p;
  int ww, wh, iw, ih, x, y;
  double fx, fy;

  if (mNextScreen.size() != size())
    mNextScreen = QPixmap(size());

  mNextScreen.fill(Qt::black);

  p.begin(&mNextScreen);

  foreach( const mScreenGeo& geo, mGeoList )
  {
    loadNextImage();

    iw = mImage.width();
    ih = mImage.height();
    ww = geo.mW;
    wh = geo.mH;

    if (mFileList.isEmpty())
    {
      p.setPen(QColor("white"));
      p.drawText(20 + (KRandom::random() % (ww>>1)), 20 + (KRandom::random() % (wh>>1)),
	         i18n("No images found"));
    }
    else
    {
      if (mZoomImages)
      {
        fx = (double)ww / iw;
        fy = (double)wh / ih;
        if (fx > fy) fx = fy;
        if (fx > 2) fx = 2;
        iw = (int)(iw * fx);
        ih = (int)(ih * fx);
        QImage scaledImg = mImage.scaled(iw, ih,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        x = ((ww - iw) >> 1) + geo.mXorg;
        y = ((wh - ih) >> 1) + geo.mYorg;

        p.drawImage(x, y, scaledImg);
      }
      else
      {
        if(iw >= ww || ih >= wh)
        {
          fx = (double)ww / iw;
	  fy = (double)wh / ih;
	  if (fx > fy) fx = fy;
	  if (fx > 2) fx = 2;
	  iw = (int)(iw * fx);
	  ih = (int)(ih * fx);
          QImage scaledImg = mImage.scaled(iw, ih,
              Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

          x = ((ww - iw) >> 1) + geo.mXorg;
          y = ((wh - ih) >> 1) + geo.mYorg;

	  p.drawImage(x, y, scaledImg);
        }
        else
        {
          if(mRandomPosition)
          {
            x = (KRandom::random() % (ww - iw)) + geo.mXorg;
            y = (KRandom::random() % (wh - ih)) + geo.mYorg;
          }
          else
          {
            x = ((ww - iw) >> 1) + geo.mXorg;
            y = ((wh - ih) >> 1) + geo.mYorg;
          }

          // bitBlt(&mNextScreen, x, y, &mImage, 0, 0, iw, ih);
          p.drawImage(x, y, mImage);
        }
      }

      if (mPrintName)
      {
        p.setPen(QColor("black"));
        for (x=9; x<=11; x++)
	  for (y=21; y>=19; y--)
            p.drawText(x + geo.mXorg, wh-y+geo.mYorg, mImageName);
        p.setPen(QColor("white"));
        p.drawText(10 + geo.mXorg, wh-20 + geo.mYorg, mImageName);
      }
    }
  }
  p.end();
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadNextImage()
{
  QString fname;
  int num;

nexttry:

  num = mFileList.count();
  if (num <= 0) //no files in the directory
  {
    return;
  }

  if (mShowRandom)
  {
    mFileIdx = KRandom::random() % num;
    fname = mFileList.takeAt(mFileIdx);
    if (num == 1) //we're about to run out of images
    {
      mFileList = mRandomList;
    }
  }
  else
  {
    if (mFileIdx >= num) mFileIdx = 0;
    fname = mFileList[mFileIdx];
  }

  if (!mImage.load(fname))
  {
    kDebug() << "Failed to load image " << fname;
    mFileList.removeAll(fname);
    mRandomList.removeAll(fname);
    goto nexttry;
  }
  mFileIdx++;

#ifdef HAVE_KEXIV2
  KExiv2Iface::KExiv2 exiv(fname);
  exiv.rotateExifQImage(mImage, exiv.getImageOrientation());
#endif

  mImageName = mPrintPath ? fname : QFileInfo(fname).completeBaseName();
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadDirectory()
{
  mFileIdx = 0;
  mFileList.clear();
  traverseDirectory(mDirectory);
  mRandomList = mFileList;
}

void kSlideShowSaver::traverseDirectory(const QString &dirName)
{
  QDir dir(dirName);
  if (!dir.exists())
  {
     return ;
  }
  dir.setFilter(QDir::Dirs | QDir::Files);

  QFileInfoList fileinfolist = dir.entryInfoList();
  QFileInfoList::Iterator it = fileinfolist.begin();
  while ( it != fileinfolist.end())
  {
     if (it->fileName() == QLatin1String( "." ) || it->fileName() == QLatin1String( ".." ))
     {
        ++it;
        continue;
     }
     if (it->isDir() && it->isReadable() && mSubdirectory)
     {
        traverseDirectory(it->filePath());
     }
     else
     {
        if (!it->isDir())
        {
           mFileList.append(it->filePath());
        }
     }
     ++it;
  }
}


//=============================================================================
//  Class kSlideShowSetup
//=============================================================================
kSlideShowSetup::kSlideShowSetup(QWidget *aParent)
  : KDialog(aParent)
{
  setCaption(i18n( "Setup Slide Show Screen Saver" ));
  setButtons(Ok|Cancel|Help);
  setDefaultButton(Ok);
  setModal(true);
  setButtonText( Help, i18n( "A&bout" ) );

  QWidget *main = new QWidget(this);
  setMainWidget(main);
  cfg = new SlideShowCfg(main);

  cfg->mPreview->setFixedSize(220, 170);
  cfg->mPreview->show();    // otherwise saver does not get correct size
  mSaver = new kSlideShowSaver(cfg->mPreview->winId());

  cfg->mDirChooser->setMode(KFile::Directory | KFile::ExistingOnly);
  connect(cfg->mDirChooser, SIGNAL(returnPressed(QString)),
      SLOT(slotDirSelected(QString)));
  connect(cfg->mDirChooser, SIGNAL(urlSelected(KUrl)),
      SLOT(slotDirSelected(KUrl)));
  connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
  connect(this,SIGNAL(helpClicked()),this,SLOT(slotHelp()));
  readSettings();
}

kSlideShowSetup::~kSlideShowSetup()
{
    delete mSaver;
}

//-----------------------------------------------------------------------------
void kSlideShowSetup::readSettings()
{
    KConfigGroup config( KGlobal::config(), "Settings");

    cfg->mCbxRandom->setChecked(config.readEntry("ShowRandom", true));
    cfg->mCbxZoom->setChecked(config.readEntry("ZoomImages", false));
    cfg->mCbxShowName->setChecked(config.readEntry("PrintName", true));
    cfg->mCbxShowPath->setChecked(config.readEntry("PrintPath", false));
    cfg->mDelay->setValue(config.readEntry("Delay", 20));
    cfg->mDelay->setSuffix(ki18np(" second", " seconds"));
    cfg->mDirChooser->setUrl(KUrl(config.readPathEntry("Directory", QString())));
    cfg->mCbxSubdirectory->setChecked(config.readEntry("SubDirectory", false));
    cfg->mCbxRandomPosition->setChecked(config.readEntry("RandomPosition", false));
    //cfg->mCbxEffectsEnabled->setChecked(config.readEntry("EffectsEnabled", true));
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::writeSettings()
{
  KConfigGroup config( KGlobal::config(), "Settings");

  config.writeEntry("ShowRandom", cfg->mCbxRandom->isChecked());
  config.writeEntry("ZoomImages", cfg->mCbxZoom->isChecked());
  config.writeEntry("PrintName",  cfg->mCbxShowName->isChecked());
  config.writeEntry("PrintPath",  cfg->mCbxShowPath->isChecked());
  config.writeEntry("Delay", cfg->mDelay->value());
  config.writePathEntry("Directory", cfg->mDirChooser->url().path());
  config.writeEntry("SubDirectory", cfg->mCbxSubdirectory->isChecked());
  config.writeEntry("RandomPosition", cfg->mCbxRandomPosition->isChecked());
  //config.writeEntry("EffectsEnabled", cfg->mCbxEffectsEnabled->isChecked());

  config.sync();

  if (mSaver)
  {
    mSaver->readConfig();
    mSaver->restart();
  }
}


//-----------------------------------------------------------------------------

void kSlideShowSetup::slotDirSelected(const KUrl &)
{
      writeSettings();
}

void kSlideShowSetup::slotDirSelected(const QString &)
{
  writeSettings();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotOk()
{
  writeSettings();
  accept();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotHelp()
{
  KAboutApplicationDialog mAbout(s_aboutData, this);
  mAbout.exec();
}
