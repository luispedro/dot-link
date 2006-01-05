#include "dottree.h"
#include "timer.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <exception>
#include <boost/format.hpp>

/**************************************************************************************************/

/*Prints all possible command-line arguments and then exits the program.

  Input : None:

  Output: A printout of all the command-line options to the screen.*/


/**************************************************************************************************/
void PrintUsage()
{
	printf(	"usage: suffixtree k filename searchstring\n" );
	exit(0);
}

void find_substring( dottree::tree* tree, const char* string, CummulativeTimer* m = 0 ) {
	Timer match( "match" );
	if ( m ) m->start();
	int res = tree->match(string);
	match.stop();
	if ( m ) m->stop();

	if(res < 0)
		printf("\nResults: String[-%s-] is not a substring.\n\n", string);
	else
		std::cout << boost::format( "\nResults:   Substring exists in position %s.\n\n" ) % res;
}

char* read_file( const char* fname )
{
	char* res = 0;
	FILE* file = fopen( fname, "r" );
	if(!file)
	{
		printf("can't open file '%s'.\n", fname );
		return 0;
	}

	/*Calculate the file length in bytes. This will be the length of the source string.*/
	fseek(file, 0, SEEK_END);
	int len = ftell(file);
	fseek(file, 0, SEEK_SET);
	res = (char*)malloc(len+1);
	if ( !res ) 
	{
		printf("\nOut of memory.\n");
		exit(0);
	}
	fread(res, 1, len, file);
	fclose( file );
	res[len] = '\0';
	return res;
}

int main(int argc, char* argv[])
{
	std::auto_ptr<dottree::tree> tree;
	if(argc < 4) PrintUsage();
	//int k = atoi( argv[ 1 ] );
	char* str = read_file( argv[ 2 ] );
	
	try { 
		Timer full( "full-tree-construction" );
		Timer dots( "add-dot-links" );
		full.start();
		tree = dottree::build_tree(str);
		dots.start();
		tree->dfs(new dottree::print_leafs);
		//ST_AddDotLinks( tree, k );
		full.stop();
		dots.stop();
	} catch ( const std::exception& e ) {
		std::cerr << "Error [construction]: (exception): " << e.what() << std::endl;
	}
	
	
	try {
		if ( strcmp( argv[ 3 ], "-" ) ) {
			find_substring( tree.get(), argv[ 3 ] );
		} else {
			std::string tmp;
			CummulativeTimer* cummul = 0;
			while ( std::getline( std::cin, tmp ) ) {
				if ( tmp.substr( 0, 5 ) == "timer" ) {
					delete cummul;
					cummul = new CummulativeTimer( tmp.substr( 6,std::string::npos ).c_str() );
				} else find_substring( tree.get(), tmp.c_str(), cummul );
			}
		}
	} catch ( const std::exception& e ) {
		std::cerr << "Error [matching]: (exception): " << e.what() << std::endl;
	}
	try {
		std::cout << boost::format( "String size: %s \n" ) % ( tree->length() - 1 );
		/*std::cout << boost::format( "Nodes without dot links: %s \n" ) % ST_CountNodes( tree, false );
		std::cout << boost::format( "Nodes with dot links: %s \n" ) % ST_CountNodes( tree, true );
*/
		stats::print();

		free(str);
	} catch ( const std::exception& e ) {
		std::cerr << "Error [statting]: (exception): " << e.what() << std::endl;
	}
	return 0;
}

