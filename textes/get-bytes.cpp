#include <iostream>
#include <cstdlib>

using namespace std;

void usage( char* argv[] ) {
	cerr << argv[ 0 ] << " nbytes <in >out\n";
}

int main( int argc, char* argv[] ) {
	if ( !argv[ 1 ] || atoi( argv[ 1 ] ) <= 0 ) {
			usage( argv );
			return 1;
	}
	int max = atoi( argv[ 1 ] );
	for ( int i = 0; i != max; ++i ) {
		cout << char( cin.get() );
		if ( !cin ) return 2;
	}
	return 0;
}
