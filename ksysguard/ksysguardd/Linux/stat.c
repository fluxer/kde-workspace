/*
	KSysGuard, the KDE System Guard
	
	Copyright (c) 1999, 2000 Chris Schlaeger <cs@kde.org>
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of version 2 of the GNU General Public
	License as published by the Free Software Foundation.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*
 * stat.c is used to read from /proc/[pid]/stat
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Command.h"
#include "ksysguardd.h"

#include "stat.h"

typedef struct {
	/* A CPU can be loaded with user processes, reniced processes and
	* system processes. Unused processing time is called idle load.
	* These variable store the percentage of each load type. */
	float userLoad;
	float niceLoad;
	float sysLoad;
	float idleLoad;
	float waitLoad;
	
	/* To calculate the loads we need to remember the tick values for each
	* load type. */
	unsigned long userTicks;
	unsigned long niceTicks;
	unsigned long sysTicks;
	unsigned long idleTicks;
	unsigned long waitTicks;
} CPULoadInfo;

static int StatDirty = 0;

/* We have observed deviations of up to 5% in the accuracy of the timer
* interrupts. So we try to measure the interrupt interval and use this
* value to calculate timing dependant values. */
static float StatTimeInterval = 0;
static struct timeval lastSampling;
static struct timeval currSampling;
static struct SensorModul* StatSM;

static CPULoadInfo CPULoad;
static CPULoadInfo* SMPLoad = 0;
static unsigned CPUCount = 0;
static unsigned long PageIn = 0;
static unsigned long OldPageIn = 0;
static unsigned long PageOut = 0;
static unsigned long OldPageOut = 0;
static unsigned long Ctxt = 0;
static unsigned long OldCtxt = 0;
static unsigned int NumOfInts = 0;
static unsigned long* OldIntr = 0;
static unsigned long* Intr = 0;

static void updateCPULoad( const char* line, CPULoadInfo* load );
static void processStat( void );

/**
 * updateCPULoad
 *
 * Parses the total cpu status line from /proc/stat
 */
static void updateCPULoad( const char* line, CPULoadInfo* load ) {
	unsigned long currUserTicks, currSysTicks, currNiceTicks;
	unsigned long currIdleTicks, currWaitTicks, totalTicks;
	
	if(sscanf( line, "%*s %lu %lu %lu %lu %lu", &currUserTicks, &currNiceTicks,
		&currSysTicks, &currIdleTicks, &currWaitTicks ) != 5) {
        return;
    }
	
	totalTicks = ( currUserTicks - load->userTicks ) +
		( currSysTicks - load->sysTicks ) +
		( currNiceTicks - load->niceTicks ) +
		( currIdleTicks - load->idleTicks ) +
		( currWaitTicks - load->waitTicks );
	
	if ( totalTicks > 10 ) {
		load->userLoad = ( 100.0 * ( currUserTicks - load->userTicks ) ) / totalTicks;
		load->sysLoad = ( 100.0 * ( currSysTicks - load->sysTicks ) ) / totalTicks;
		load->niceLoad = ( 100.0 * ( currNiceTicks - load->niceTicks ) ) / totalTicks;
		load->idleLoad = ( 100.0 * ( currIdleTicks - load->idleTicks ) ) / totalTicks;
		load->waitLoad = ( 100.0 * ( currWaitTicks - load->waitTicks ) ) / totalTicks;
	}
	else
		load->userLoad = load->sysLoad = load->niceLoad = load->idleLoad = load->waitLoad = 0.0;
		
	load->userTicks = currUserTicks;
	load->sysTicks = currSysTicks;
	load->niceTicks = currNiceTicks;
	load->idleTicks = currIdleTicks;
	load->waitTicks = currWaitTicks;
}

static void processStat( void ) {
	char format[ 32 ];
	char tagFormat[ 16 ];
	char buf[ 1024 ];
	char tag[ 32 ];
	
	sprintf( format, "%%%d[^\n]\n", (int)sizeof( buf ) - 1 );
	sprintf( tagFormat, "%%%ds", (int)sizeof( tag ) - 1 );

	gettimeofday( &currSampling, 0 );
	StatDirty = 0;

    FILE *stat = fopen("/proc/stat", "r");
    if(!stat) {
		print_error( "Cannot open file \'/proc/stat\'!\n"
				"The kernel needs to be compiled with support\n"
				"for /proc file system enabled!\n" );
		return;
    }

	while ( fscanf( stat, format, buf ) == 1 ) {
		buf[ sizeof( buf ) - 1 ] = '\0';
		sscanf( buf, tagFormat, tag );
		
		if ( strcmp( "cpu", tag ) == 0 ) {
			/* Total CPU load */
			updateCPULoad( buf, &CPULoad );
		}
		else if ( strncmp( "cpu", tag, 3 ) == 0 ) {
			/* Load for each SMP CPU */
			int id;
			sscanf( tag + 3, "%d", &id );
			updateCPULoad( buf, &SMPLoad[ id ]  );
		}
		else if ( strcmp( "page", tag ) == 0 ) {
			unsigned long v1, v2;
			sscanf( buf + 5, "%lu %lu", &v1, &v2 );
			PageIn = v1 - OldPageIn;
			OldPageIn = v1;
			PageOut = v2 - OldPageOut;
			OldPageOut = v2;
		}
		else if ( strcmp( "intr", tag ) == 0 ) {
			unsigned int i = 0;
			char* p = buf + 5;
			
			for ( i = 0; i < NumOfInts; i++ ) {
				unsigned long val;
			
				sscanf( p, "%lu", &val );
				Intr[ i ] = val - OldIntr[ i ];
				OldIntr[ i ] = val;
				while ( *p && *p != ' ' )
					p++;
				while ( *p && *p == ' ' )
					p++;
			}
		} else if ( strcmp( "ctxt", tag ) == 0 ) {
			unsigned long val;
			
			sscanf( buf + 5, "%lu", &val );
			Ctxt = val - OldCtxt;
			OldCtxt = val;
		}
	}
    fclose(stat);
	
	/* Read Linux 2.5.x /proc/vmstat */
    stat = fopen("/proc/vmstat", "r");
    if(stat) {
        while ( fscanf( stat, format, buf ) == 1 ) {
            buf[ sizeof( buf ) - 1 ] = '\0';
            sscanf( buf, tagFormat, tag );

            if ( strcmp( "pgpgin", tag ) == 0 ) {
                unsigned long v1;
                sscanf( buf + 7, "%lu", &v1 );
                PageIn = v1 - OldPageIn;
                OldPageIn = v1;
            }
            else if ( strcmp( "pgpgout", tag ) == 0 ) {
                unsigned long v1;
                sscanf( buf + 7, "%lu", &v1 );
                PageOut = v1 - OldPageOut;
                OldPageOut = v1;
            }
        }
        fclose(stat);
    }
	
	/* save exact time interval between this and the last read of /proc/stat */
	StatTimeInterval = currSampling.tv_sec - lastSampling.tv_sec +
			   ( currSampling.tv_usec - lastSampling.tv_usec ) / 1000000.0;
	lastSampling = currSampling;
}

/*
================================ public part =================================
*/

void initStat( struct SensorModul* sm ) {
	/* The CPU load is calculated from the values in /proc/stat. The cpu
	* entry contains 7 counters. These counters count the number of ticks
	* the system has spend on user processes, system processes, nice
	* processes, idle and IO-wait time, hard and soft interrupts.
	*
	* SMP systems will have cpu1 to cpuN lines right after the cpu info. The
	* format is identical to cpu and reports the information for each cpu.
	* Linux kernels <= 2.0 do not provide this information!
	*
	* The /proc/stat file looks like this:
	*
	* cpu  <user> <nice> <system> <idling> <waiting> <hardinterrupt> <softinterrupt>
	* disk 7797 0 0 0
	* disk_rio 6889 0 0 0
	* disk_wio 908 0 0 0
	* disk_rblk 13775 0 0 0
	* disk_wblk 1816 0 0 0
	* page 27575 1330
	* swap 1 0
	* intr 50444 38672 2557 0 0 0 0 2 0 2 0 0 3 1429 1 7778 0
	* ctxt 54155
	* btime 917379184
	* processes 347 
	*
	* Linux kernel >= 2.4.0 have one or more disk_io: lines instead of
	* the disk_* lines.
	*
	* Linux kernel >= 2.6.x(?) have disk I/O stats in /proc/diskstats
	* and no disk relevant lines are found in /proc/stat
	*/
	
	char format[ 32 ];
	char tagFormat[ 16 ];
	char buf[ 1024 ];
	char tag[ 32 ];
	
	StatSM = sm;
	
	sprintf( format, "%%%d[^\n]\n", (int)sizeof( buf ) - 1 );
	sprintf( tagFormat, "%%%ds", (int)sizeof( tag ) - 1 );

    FILE *stat = fopen("/proc/stat", "r");
    if(!stat) {
		print_error( "Cannot open file \'/proc/stat\'!\n"
				"The kernel needs to be compiled with support\n"
				"for /proc file system enabled!\n" );
		return;
    }
	while ( fscanf( stat, format, buf ) == 1 ) {
		buf[ sizeof( buf ) - 1 ] = '\0';
		sscanf( buf, tagFormat, tag );
		
		if ( strcmp( "cpu", tag ) == 0 ) {
			/* Total CPU load */
			registerMonitor( "cpu/system/user", "float", printCPUUser, printCPUUserInfo, StatSM );
			registerMonitor( "cpu/system/nice", "float", printCPUNice, printCPUNiceInfo, StatSM );
			registerMonitor( "cpu/system/sys", "float", printCPUSys, printCPUSysInfo, StatSM );
			registerMonitor( "cpu/system/TotalLoad", "float", printCPUTotalLoad, printCPUTotalLoadInfo, StatSM );
			registerMonitor( "cpu/system/idle", "float", printCPUIdle, printCPUIdleInfo, StatSM );
			registerMonitor( "cpu/system/wait", "float", printCPUWait, printCPUWaitInfo, StatSM );

			/* Monitor names changed from kde3 => kde4. Remain compatible with legacy requests when possible. */
			registerLegacyMonitor( "cpu/user", "float", printCPUUser, printCPUUserInfo, StatSM );
			registerLegacyMonitor( "cpu/nice", "float", printCPUNice, printCPUNiceInfo, StatSM );
			registerLegacyMonitor( "cpu/sys", "float", printCPUSys, printCPUSysInfo, StatSM );
			registerLegacyMonitor( "cpu/TotalLoad", "float", printCPUTotalLoad, printCPUTotalLoadInfo, StatSM );
			registerLegacyMonitor( "cpu/idle", "float", printCPUIdle, printCPUIdleInfo, StatSM );
			registerLegacyMonitor( "cpu/wait", "float", printCPUWait, printCPUWaitInfo, StatSM );
		}
		else if ( strncmp( "cpu", tag, 3 ) == 0 ) {
			char cmdName[ 24 ];
			/* Load for each SMP CPU */
			int id;
			
			sscanf( tag + 3, "%d", &id );
			CPUCount++;
			sprintf( cmdName, "cpu/cpu%d/user", id );
			registerMonitor( cmdName, "float", printCPUxUser, printCPUxUserInfo, StatSM );
			sprintf( cmdName, "cpu/cpu%d/nice", id );
			registerMonitor( cmdName, "float", printCPUxNice, printCPUxNiceInfo, StatSM );
			sprintf( cmdName, "cpu/cpu%d/sys", id );
			registerMonitor( cmdName, "float", printCPUxSys, printCPUxSysInfo, StatSM );
			sprintf( cmdName, "cpu/cpu%d/TotalLoad", id );
			registerMonitor( cmdName, "float", printCPUxTotalLoad, printCPUxTotalLoadInfo, StatSM );
			sprintf( cmdName, "cpu/cpu%d/idle", id );
			registerMonitor( cmdName, "float", printCPUxIdle, printCPUxIdleInfo, StatSM );
			sprintf( cmdName, "cpu/cpu%d/wait", id );
			registerMonitor( cmdName, "float", printCPUxWait, printCPUxWaitInfo, StatSM );
		}
		else if ( strcmp( "page", tag ) == 0 ) {
			sscanf( buf + 5, "%lu %lu", &OldPageIn, &OldPageOut );
			registerMonitor( "cpu/pageIn", "float", printPageIn, printPageInInfo, StatSM );
			registerMonitor( "cpu/pageOut", "float", printPageOut, printPageOutInfo, StatSM );
		}
		else if ( strcmp( "intr", tag ) == 0 ) {
			unsigned int i;
			char cmdName[ 32 ];
			char* p = buf + 5;
			
			/* Count the number of listed values in the intr line. */
			NumOfInts = 0;
			while ( *p )
				if ( *p++ == ' ' )
					NumOfInts++;
			
			/* It looks like anything above 24 is always 0. So let's just
			* ignore this for the time being. */
			if ( NumOfInts > 25 )
				NumOfInts = 25;
			OldIntr = (unsigned long*)malloc( NumOfInts * sizeof( unsigned long ) );
			Intr = (unsigned long*)malloc( NumOfInts * sizeof( unsigned long ) );
			i = 0;
			p = buf + 5;
			for ( i = 0; p && i < NumOfInts; i++ ) {
				sscanf( p, "%lu", &OldIntr[ i ] );
				while ( *p && *p != ' ' )
					p++;
				while ( *p && *p == ' ' )
					p++;
				sprintf( cmdName, "cpu/interrupts/int%02d", i );
				registerMonitor( cmdName, "float", printInterruptx, printInterruptxInfo, StatSM );
			}
		}
		else if ( strcmp( "ctxt", tag ) == 0 ) {
			sscanf( buf + 5, "%lu", &OldCtxt );
			registerMonitor( "cpu/context", "float", printCtxt, printCtxtInfo, StatSM );
		}
	}
    fclose(stat);

	stat = fopen("/proc/vmstat", "r");
    if(!stat) {
		print_error( "Cannot open file \'/proc/vmstat\'\n");
    } else {
        while ( fscanf( stat, format, buf ) == 1 ) {
            buf[ sizeof( buf ) - 1 ] = '\0';
            sscanf( buf, tagFormat, tag );

            if ( strcmp( "pgpgin", tag ) == 0 ) {
                sscanf( buf + 7, "%lu", &OldPageIn );
                registerMonitor( "cpu/pageIn", "float", printPageIn, printPageInInfo, StatSM );
            }
            else if ( strcmp( "pgpgout", tag ) == 0 ) {
                sscanf( buf + 7, "%lu", &OldPageOut );
                registerMonitor( "cpu/pageOut", "float", printPageOut, printPageOutInfo, StatSM );
            }
        }
        fclose(stat);
    }
	if ( CPUCount > 0 )
		SMPLoad = (CPULoadInfo*)calloc( CPUCount, sizeof( CPULoadInfo ) );
	
	/* Call processStat to eliminate initial peek values. */
	processStat();
}
	
void exitStat( void ) {
	free( SMPLoad );
	SMPLoad = 0;
	
	free( OldIntr );
	OldIntr = 0;
	
	free( Intr );
	Intr = 0;
	
	removeMonitor("cpu/system/user");
	removeMonitor("cpu/system/nice");
	removeMonitor("cpu/system/sys");
	removeMonitor("cpu/system/idle");
	
	/* Todo: Dynamically registered monitors (per cpu, per disk) are not removed yet) */
	
	/* These were registered as legacy monitors */
	removeMonitor("cpu/user");
	removeMonitor("cpu/nice");
	removeMonitor("cpu/sys");
	removeMonitor("cpu/idle");
}

int updateStat( void )
{
    StatDirty = 1;
    return 0;
}

void printCPUUser( const char* cmd ) {
	(void)cmd;
	
	if ( StatDirty )
		processStat();
	
	output( "%f\n", CPULoad.userLoad );
}

void printCPUUserInfo( const char* cmd ) {
	(void)cmd;

	output( "CPU User Load\t0\t100\t%%\n" );
}

void printCPUNice( const char* cmd ) {
	(void)cmd;
	
	if ( StatDirty )
		processStat();
	
	output( "%f\n", CPULoad.niceLoad );
}

void printCPUNiceInfo( const char* cmd ) {
	(void)cmd;

	output( "CPU Nice Load\t0\t100\t%%\n" );
}

void printCPUSys( const char* cmd ) {
	(void)cmd;
	
	if ( StatDirty )
		processStat();
	
	output( "%f\n", CPULoad.sysLoad );
}

void printCPUSysInfo( const char* cmd ) {
	(void)cmd;

	output( "CPU System Load\t0\t100\t%%\n" );
}

void printCPUTotalLoad( const char* cmd ) {
	(void)cmd;
	
	if ( StatDirty )
		processStat();
	
	output( "%f\n", CPULoad.userLoad + CPULoad.sysLoad + CPULoad.niceLoad + CPULoad.waitLoad );
}

void printCPUTotalLoadInfo( const char* cmd ) {
	(void)cmd;

	output( "CPU Total Load\t0\t100\t%%\n" );
}

void printCPUIdle( const char* cmd ) {
	(void)cmd;
	
	if ( StatDirty )
		processStat();
	
	output( "%f\n", CPULoad.idleLoad );
}

void printCPUIdleInfo( const char* cmd ) {
	(void)cmd;

	output( "CPU Idle Load\t0\t100\t%%\n" );
}

void printCPUWait( const char* cmd )
{
	(void)cmd;

	if ( StatDirty )
		processStat();

	output( "%f\n", CPULoad.waitLoad );
}

void printCPUWaitInfo( const char* cmd )
{
	(void)cmd;
	output( "CPU Wait Load\t0\t100\t%%\n" );
}

void printCPUxUser( const char* cmd ) {
	int id;
	
	if ( StatDirty )
		processStat();
	
	sscanf( cmd + 7, "%d", &id );
	output( "%f\n", SMPLoad[ id ].userLoad );
}

void printCPUxUserInfo( const char* cmd ) {
	int id;

	sscanf( cmd + 7, "%d", &id );
	output( "CPU %d User Load\t0\t100\t%%\n", id+1 );
}

void printCPUxNice( const char* cmd ) {
	int id;
	
	if ( StatDirty )
		processStat();
	
	sscanf( cmd + 7, "%d", &id );
	output( "%f\n", SMPLoad[ id ].niceLoad );
}

void printCPUxNiceInfo( const char* cmd ) {
	int id;
	
	sscanf( cmd + 7, "%d", &id );
	output( "CPU %d Nice Load\t0\t100\t%%\n", id+1 );
}

void printCPUxSys( const char* cmd ) {
	int id;
	
	if ( StatDirty )
		processStat();
	
	sscanf( cmd + 7, "%d", &id );
	output( "%f\n", SMPLoad[ id ].sysLoad );
}

void printCPUxSysInfo( const char* cmd ) {
	int id;
	
	sscanf( cmd + 7, "%d", &id );
	output( "CPU %d System Load\t0\t100\t%%\n", id+1 );
}

void printCPUxTotalLoad( const char* cmd ) {
	int id;
	
	if ( StatDirty )
		processStat();
	
	sscanf( cmd + 7, "%d", &id );
	output( "%f\n", SMPLoad[ id ].userLoad + SMPLoad[ id ].sysLoad + SMPLoad[ id ].niceLoad + SMPLoad[ id ].waitLoad );
}

void printCPUxTotalLoadInfo( const char* cmd ) {
	int id;
	
	sscanf( cmd + 7, "%d", &id );
	output( "CPU %d\t0\t100\t%%\n", id+1 );
}

void printCPUxIdle( const char* cmd ) {
	int id;
	
	if ( StatDirty )
		processStat();
	
	sscanf( cmd + 7, "%d", &id );
	output( "%f\n", SMPLoad[ id ].idleLoad );
}

void printCPUxIdleInfo( const char* cmd ) {
	int id;
	
	sscanf( cmd + 7, "%d", &id );
	output( "CPU %d Idle Load\t0\t100\t%%\n", id+1 );
}

void printCPUxWait( const char* cmd )
{
	int id;

	if ( StatDirty )
		processStat();

	sscanf( cmd + 7, "%d", &id );
	output( "%f\n", SMPLoad[ id ].waitLoad );
}

void printCPUxWaitInfo( const char* cmd )
{
	int id;

	sscanf( cmd + 7, "%d", &id );
	output( "CPU %d Wait Load\t0\t100\t%%\n", id+1 );
}

void printPageIn( const char* cmd ) {
	(void)cmd;
	
	if ( StatDirty )
		processStat();
	
	output( "%f\n", (float)( PageIn / StatTimeInterval ) );
}

void printPageInInfo( const char* cmd ) {
	(void)cmd;

	output( "Paged in Pages\t0\t0\t1/s\n" );
}

void printPageOut( const char* cmd ) {
	(void)cmd;
	
	if ( StatDirty )
		processStat();
	
	output( "%f\n", (float)( PageOut / StatTimeInterval ) );
}

void printPageOutInfo( const char* cmd ) {
	(void)cmd;

	output( "Paged out Pages\t0\t0\t1/s\n" );
}

void printInterruptx( const char* cmd ) {
	int id;
	
	if ( StatDirty )
		processStat();
	
	sscanf( cmd + strlen( "cpu/interrupts/int" ), "%d", &id );
	output( "%f\n", (float)( Intr[ id ] / StatTimeInterval ) );
}

void printInterruptxInfo( const char* cmd ) {
	int id;
	
	sscanf( cmd + strlen( "cpu/interrupt/int" ), "%d", &id );
	output( "Interrupt %d\t0\t0\t1/s\n", id );
}

void printCtxt( const char* cmd ) {
	(void)cmd;
	
	if ( StatDirty )
		processStat();
	
	output( "%f\n", (float)( Ctxt / StatTimeInterval ) );
}

void printCtxtInfo( const char* cmd ) {
	(void)cmd;

	output( "Context switches\t0\t0\t1/s\n" );
}
