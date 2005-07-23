#include "stats.h"

#include <fstream>
#include <map>
#include <vector>
#include <numeric>

#define STATS

namespace {
	struct proportioninfo {
			unsigned total;
			unsigned positive;
	};
	proportioninfo matches;

	struct accumulator_type_open : stats::accumulator_type {
			unsigned value() const { return value_; }
	};


	std::map<std::string, std::vector<unsigned> > counters;
	std::map<std::string, proportioninfo> proportions;
	std::map<std::string, std::vector<unsigned> > averages;
	std::map<std::string, std::vector<double> > averagesf;
	std::map<std::string, accumulator_type_open> accumulators;
}

void stats::did_match( bool res ) {
	++matches.total;
	if ( res ) ++ matches.positive;
}

void stats::proportion( std::string name, bool v ) {
		proportioninfo& p = proportions[ name ];
		++p.total;
		if ( v ) ++p.positive;
}

stats::accumulator_type& stats::accumulator( std::string name ) {
		return accumulators[ name ];
}

void stats::count( std::string name, unsigned current, unsigned limit ) {
	std::vector<unsigned>& c = counters[ name ];
	c.resize( limit + 1 );
	++c[ std::min<unsigned>( current, limit ) ];
}

void stats::count_one( const char* name ) {
	accumulator( name ).add( 1 );
}

void stats::print() {
	stats::print( stats::file() );
}

void stats::print( std::ostream& out ) {
	out << "Total matches: "<< matches.total
		<< ", of which " << matches.positive
		<< " ( " << double( matches.positive ) / matches.total
		<< " ) were positive\n";
	for ( std::map< std::string, proportioninfo>::const_iterator f = proportions.begin(), e = proportions.end();
			f != e;
			++f ) {
		out << "Proportion [ " << f->first << " ] = "
			<< double( f->second.positive ) / f->second.total 
			<< " ( " << f->second.positive << " of " << f->second.total << " ).\n";
	}
	for ( std::map<std::string, std::vector<unsigned> >::const_iterator f = averages.begin(), e = averages.end();
					f != e;
					++f ) {
			out << "Average [ " << f->first << " ] = "
					<< std::accumulate( f->second.begin(), f->second.end(), 0.0 )
						/ std::max<unsigned>( f->second.size(), 1 )
					<< ", on " << f->second.size() << " samples.\n";
	}
	for ( std::map<std::string, std::vector<double> >::const_iterator f = averagesf.begin(), e = averagesf.end();
					f != e;
					++f ) {
			out << "Average [ " << f->first << " ] = "
					<< std::accumulate( f->second.begin(), f->second.end(), 0.0 ) 
						/ std::max<unsigned>( f->second.size(), 1 )
					<< ", on " << f->second.size() << " samples.\n";
	}

	for ( std::map<std::string, accumulator_type_open>::const_iterator f = accumulators.begin(), e = accumulators.end();
					f != e;
					++f ) {
			out << "Accumulator {"  << f->first << "} : " << f->second.value() << std::endl;
	}

	for ( std::map<std::string, std::vector<unsigned> >::const_iterator f = counters.begin(), e = counters.end();
					f != e;
					++f ) {
			unsigned cumm = 0;
			unsigned total = std::accumulate( f->second.begin(), f->second.end(), 0 );
			for ( std::vector<unsigned>::size_type i = 0, len = f->second.size(); i != len; ++i ) {
					cumm += f->second[ i ];
					float perc = double( cumm ) / total;
					out << " count {" << f->first << "} [ " << i << " ] " <<  " = "
							<< f->second[ i ] << " cumm = " << cumm << " ( " << perc << "% ) " << std::endl;
			}
	}

}


namespace {
	std::ostream* outfile = 0;
	bool closefile = true;
	void close_outfile() { if ( closefile ) delete outfile; }
}

void stats::setfile( std::ostream& out ) {
	outfile = &out;
	closefile = false;
}

void stats::average( std::string name, unsigned v ) {
		averages[ name ].push_back( v );
}

void stats::average( std::string name, double v ) {
		averagesf[ name ].push_back( v );
}

std::ostream& stats::file() {
	return std::cout;
}

void stats::printtime( const char* msg ) {
	time_t now = time( 0 );
	stats::file() << ( msg ? msg : "" )
		<< ctime( &now ) << '\n';
}



