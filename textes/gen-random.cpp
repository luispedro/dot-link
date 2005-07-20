#include <iostream>
#include <cstring>
#include <cstdlib>

void usage( char* argv[] ) {
	std::cerr << "usage: " << argv[ 0 ] << " Nchar [MaxAlph]\n";
}

int main( int argc, char** argv ) {
	const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVXZabcdefghijklmnopqrstuvxz1234567890";

	std::srand( 1023 );

	if ( argc < 2 ) {
		usage( argv );
		return 1;
	}

	int n = std::atoi( argv[ 1 ] );
	int alph = ( argv[ 2 ] ? std::atoi( argv[ 2 ] ) : std::strlen( alphabet ) );

	if ( n <= 0 || alph <= 0 ) {
		usage( argv );
		return 1;
	}

	for ( int i = 0; i != n; ++i ) {
		std::cout << alphabet[ rand() % alph ];
	}
	std::cout << std::endl;
	return 0;
}
