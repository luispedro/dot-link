#include "suffix_tree.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
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

void find_substring( SUFFIX_TREE* tree, const char* string ) {
	Timer match( "match" );
	unsigned i = ST_FindSubstringWithErrors(tree, string, strlen(string));
	match.stop();

	if(i == ST_ERROR)
		printf("\nResults: String is not a substring.\n\n");
	else
		std::cout << boost::format( "\nResults:   Substring exists in position %s.\n\n" ) % i;
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
	res = (char*)malloc(len);
	if ( !res ) 
	{
		printf("\nOut of memory.\n");
		exit(0);
	}
	fread(res, 1, len, file);
	fclose( file );
	return res;
}

int main(int argc, char* argv[])
{
	SUFFIX_TREE* tree;
	if(argc < 4) PrintUsage();
	int k = atoi( argv[ 1 ] );
	char* str = read_file( argv[ 2 ] );
	
	Timer full( "full-tree-construction" );
	Timer dots( "add-dot-links" );
	full.start();
	tree = ST_CreateTree(str);
	dots.start();
	ST_AddDotLinks( tree, k );
	full.stop();
	dots.stop();
	
	
	if ( strcmp( argv[ 3 ], "-" ) ) {
		find_substring( tree, argv[ 3 ] );
	} else {
		std::string tmp;
		while ( std::getline( std::cin, tmp ) ) find_substring( tree, tmp.c_str() );
	}
	std::cout << boost::format( "String size: %s \n" ) % ( tree->length - 1 );
	std::cout << boost::format( "Nodes without dot links: %s \n" ) % ST_CountNodes( tree, false );
	std::cout << boost::format( "Nodes with dot links: %s \n" ) % ST_CountNodes( tree, true );

	free(str);
	ST_DeleteTree(tree);
	return 0;
}
