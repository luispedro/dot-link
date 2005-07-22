#include "timer.h"
#include "stats.h"

#include <sys/times.h> //  clock
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <cassert>
#include <iostream>

namespace {
	long Clk_Tck = 0;
}



Timer::Timer( const char* name )
	:id( name ) {
		start();
	}

Timer::~Timer() {
	if ( begin ) stop();
}

void Timer::stop() {
	struct tms tmp;
	times( &tmp );
	clock_t now = tmp.tms_utime;
	stats::file() << "timer[" << id << "]:\t " <<  (now - begin )/ Clk_Tck << std::endl;
	begin = 0;
}
void Timer::start() {
	struct tms tmp;
	if ( times( &tmp ) != clock_t( -1 ) ) {
		begin = tmp.tms_utime;
		if ( !Clk_Tck ) Clk_Tck = sysconf( _SC_CLK_TCK );
	} else {
		std::cerr << "Error: " << strerror( errno ) << '\n';
		exit( 1 );
	}
}

AvgRealTimer::AvgRealTimer( const char* name )
	:id( name ) {
	start();
}

AvgRealTimer::~AvgRealTimer() {
	if ( begin ) stop();
}

void AvgRealTimer::start() {
	time( &begin );
}

void AvgRealTimer::stop() {
	time_t now = time( 0 );
	stats::average( std::string( "timer-avg+" + id ), double( now - begin )/double( Clk_Tck ) );
	begin = 0;
}

AvgTimer::AvgTimer( const char* name )
	:id( name ) {
	start();
}

AvgTimer::~AvgTimer() {
	if ( begin ) stop();
}

void AvgTimer::start() {
	struct tms tmp;
	times( &tmp );
	begin = tmp.tms_utime;
}

void AvgTimer::stop() {
	struct tms tmp;
	times( &tmp );
	clock_t now = tmp.tms_utime;
	stats::average( std::string( "timer-avg+" + id ), double( now - begin )/double( Clk_Tck ) );
	begin = 0;
}



CummulativeTimer::CummulativeTimer( const char* name )
	:id( name ) {
		start();
	}

CummulativeTimer::~CummulativeTimer() {
	if ( begin ) stop();
}

void CummulativeTimer::stop() {
	struct tms tmp;
	times( &tmp );
	clock_t now = tmp.tms_utime;
	stats::accumulator( std::string( "timer[" ) + id +  "] " ).add(now - begin );
	begin = 0;
}
void CummulativeTimer::start() {
	struct tms tmp;
	times( &tmp );
	begin = tmp.tms_utime;
	if ( !Clk_Tck ) Clk_Tck = sysconf( _SC_CLK_TCK );
}


RealTimer::RealTimer( const char* id )
	:id( id ) {
		start();
	}

RealTimer::~RealTimer() {
	if ( begin ) stop();
}

void RealTimer::start() {
	stats::printtime( "START" );
	time( &begin );
}
void RealTimer::stop() {
	time_t now = time( 0 );
	stats::printtime( "STOP" );
	stats::file() << "real-time[" << id << "]: " << ( now - begin ) << '\n';
	begin = 0;
}

