//-----------------------------------------------------------------------------
//
// kvm screensaver
//

#ifndef KVM_H
#define KVM_H

#include <qtimer.h>

#include <kdialog.h>
#include <kscreensaver.h>

extern "C" {
#include "vm.h"
#include "vm_random.h"
}

#define	THREAD_MAX_STACK_SIZE	10
#define	MAX_THREADS_NUM		20

#define	MAX_REFRESH_TIMEOUT	40

typedef struct {
  QWidget *w;
  int grid_width, grid_height;
  int grid_margin_x;
  int grid_margin_y;
  int char_width, char_height;
  bool insert_top_p, insert_bottom_p;
  int density;
  struct tvm_pool*	pool;
  char*	modified;
  int	show_threads;

  QPixmap images;
  int image_width, image_height;
  int nglyphs;

} m_state;


class kVmSaver : public KScreenSaver
{
	Q_OBJECT
public:
	kVmSaver( WId id );
	virtual ~kVmSaver();

	void setSpeed( int spd );
	void setRefreshTimeout( const int refreshTimeout );

protected:
	void readSettings();
        int getRandom( const int max_value );
        void modifyArea( const int op );
        void paintEvent(QPaintEvent *event);

protected:
	QTimer      timer;

        bool        cleared;
	int         speed;
	m_state*    pool_state;
        int	refreshStep;
        int	refreshTimeout;
};


class kVmSetup : public KDialog
{
	Q_OBJECT
public:
	kVmSetup( QWidget *parent = NULL );
    ~kVmSetup();
protected:
	void readSettings();

private slots:
	void slotSpeed( int );
	void slotRefreshTimeout( int num );
	void slotOk();
	void slotHelp();

private:
	QWidget *preview;
	kVmSaver *saver;

	int speed;
	int refreshTimeout;
};


#endif

