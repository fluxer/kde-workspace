//-----------------------------------------------------------------------------
//
// kpolygon - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//

#ifndef POLYGON_H
#define POLYGON_H

#include <qtimer.h>
#include <QVector>
#include <QList>

#include <kdialog.h>
#include <kscreensaver.h>
#include <krandomsequence.h>

class QPolygon;

class kPolygonSaver : public KScreenSaver
{
	Q_OBJECT
public:
	kPolygonSaver( WId id );
	virtual ~kPolygonSaver();

	void setPolygon( int len, int ver );
	void setSpeed( int spd );

private:
	void readSettings();
	void initialisePolygons();
	void moveVertices();
	void initialiseColor();
	void nextColor();

protected:
	void paintEvent(QPaintEvent *event);

protected:
	QTimer		timer;
	bool		cleared;
	int			numLines;
	int			numVertices;
	int			speed;
	QColor		colors[64];
    int         currentColor;
	QList<QPolygon> polygons;
	QVector<QPoint> directions;
	KRandomSequence rnd;
};

class kPolygonSetup : public KDialog
{
	Q_OBJECT
public:
	kPolygonSetup( QWidget *parent = 0 );
    ~kPolygonSetup();

protected:
	void readSettings();

private slots:
	void slotLength( int );
	void slotVertices( int );
	void slotSpeed( int );
	void slotOk();
	void slotHelp();

private:
	QWidget *preview;
	kPolygonSaver *saver;

	int length;
	int vertices;
	int speed;
};

#endif

