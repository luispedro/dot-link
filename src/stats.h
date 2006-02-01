#ifndef LPC_STATS_H1083933319_INCLUDE_GUARD_
#define LPC_STATS_H1083933319_INCLUDE_GUARD_

#include <iostream>
#include <string>


#ifndef NOSTATS
#define DECL( function_decl ) function_decl ;
#else
#define DECL( function_decl ) inline function_decl { }
#endif

namespace stats {


DECL( void did_match( bool res ) )
DECL( void print() )
DECL( void print( std::ostream& out ) )
DECL( void print_python() )
DECL( void print_python( std::ostream& out ) )

DECL( void average( std::string, unsigned ) )
DECL( void average( std::string, double ) )
DECL( void proportion( std::string, bool ) )
DECL( void count( std::string name, unsigned current, unsigned limit ) )
DECL( void setfile( std::ostream& out ) )
DECL( void printtime( const char* ) )
DECL( void count_one( const char* ) )

struct accumulator_type {
		accumulator_type() : value_( 0 ) { }
		
		void add( unsigned d ) { value_ += d; }
		void reset() { value_ = 0; }
		unsigned value() const { return value_; }

		protected: 
		unsigned value_;
};

#ifndef NOSTATS
accumulator_type& accumulator( std::string );
#else
inline
accumulator_type& accumulator( std::string ) {
	static accumulator_type acc;
	return acc;
}
#endif
std::ostream& file();

}



#endif /* LPC_STATS_H1083933319_INCLUDE_GUARD_ */
