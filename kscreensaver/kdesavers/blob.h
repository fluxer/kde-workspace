//-----------------------------------------------------------------------------
//
// kblob - Basic screen saver for KDE
//
// Copyright (c)  Tiaan Wessels, 1997
//

#ifndef BLOB_H
#define BLOB_H

#include <qtimer.h>

#include <kdialog.h>
#include <kscreensaver.h>

class QListWidgetItem;

#define RAMP		64
#define SPEED		10

enum blob_alg {
	ALG_LINEAR,
	ALG_HSINE,
	ALG_CIRB,
	ALG_POLARC,
	ALG_LAST,
	ALG_RANDOM = ALG_LAST };

class KBlobSaver : public KScreenSaver
{
    Q_OBJECT

public:
    KBlobSaver( WId id );
    virtual ~KBlobSaver();

    void setDimension(int d)
	{ dim = d; }
    void setShowlen(time_t s)
	{ showlen = s; }
    void setColorInc(int c)
	{ colorInc = c; }

public slots:
    void setAlgorithm(int pos);

public:
    typedef void (KBlobSaver::*AlgFunc)();
    struct KBSAlg
    {
	QString Name;
	AlgFunc Init;
	AlgFunc NextFrame;
    };
private:

    QTimer	timer;
    uint	colors[RAMP];
    uint	lookup[256];
    int		colorInc;
    int		tx, ty;
    int		dim;
    int		xhalf, yhalf;
    int		alg, newalg, newalgp;
    time_t	showlen, start;
    KBSAlg	Algs[ALG_LAST];
    int		ln_xinc, ln_yinc;
    float	hs_radians, hs_rinc, hs_flip, hs_per;
    float	cb_radians, cb_rinc, cb_sradians, cb_radius, cb_devradinc;
    float	cb_deviate;
    float	pc_angle, pc_radius, pc_inc, pc_crot, pc_div;

    void lnSetup();
    void hsSetup();
    void cbSetup();
    void pcSetup();

    void lnNextFrame();
    void hsNextFrame();
    void cbNextFrame();
    void pcNextFrame();

    void box(int, int);
    void readSettings();

protected:
    void paintEvent(QPaintEvent *event);
};

class QListWidget;
class KIntNumInput;

class KBlobSetup : public KDialog
{
    Q_OBJECT

    int showtime;
    int alg;
    QListWidget  *algs;
    KIntNumInput *stime;

public:
    KBlobSetup( QWidget *parent = NULL );

protected:
    void readSettings();

private slots:
    void slotOk();
    void slotHelp();
    void setAlgorithm(QListWidgetItem* item);
private:
    KBlobSaver *saver;
};

#endif

