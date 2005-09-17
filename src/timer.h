#ifndef LPC_TIMER_H1122058556_INCLUDE_GUARD_
#define LPC_TIMER_H1122058556_INCLUDE_GUARD_

#include <time.h>
#include <string>
#include <fstream>

class Timer {
	public:
		explicit Timer( const char* name );
		~Timer();
		void start();
		void stop();

	private:
		const std::string id;
		clock_t begin;
};

class CummulativeTimer {
	public:
		explicit CummulativeTimer( const char* name );
		~CummulativeTimer();
		void start();
		void stop();
		const char* name() const { return id.c_str(); }

	private:
		const std::string id;
		clock_t begin;
};

class RealTimer {
	public:
		explicit RealTimer( const char* name );
		~RealTimer();
		void start();
		void stop();
	private:
		const std::string id;
		time_t begin;
};

class AvgRealTimer {
	public:
		explicit AvgRealTimer( const char* name );
		~AvgRealTimer();
	private:
		void start();
		void stop();
		const std::string id;
		time_t begin;
};
class AvgTimer {
	public:
		explicit AvgTimer( const char* name );
		~AvgTimer();
	private:
		void start();
		void stop();
		const std::string id;
		clock_t begin;
};



#endif /* LPC_TIMER_H1122058556_INCLUDE_GUARD_ */
