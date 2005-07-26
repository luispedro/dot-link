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

printf(	"USAGE: SuffixTree <cmd> <string | filename> [<search string>] [p]\n\n"
"examples: immidiate string with printing:   SuffixTree s  mystring trin p\n"
"          string from file:                 SuffixTree f  mytree.txt substring\n"
"          immidate with self testing:       SuffixTree ts mystring trin\n"
"          file with self testing:           SuffixTree tf mytree.txt substring\n\n");
 
printf("<cmd> - Command. These commands are available:\n"
"             s - build a suffix tree based on an immediate string, meanning\n"
"                 the actual string follows the command.\n"
"             f - build a suffix tree based on a file, meanning the\n"
"                 full path file name follows the command.\n");
printf("             ts - same as the s command but includes a self-test\n"
"                  as well (See Testing Guide).\n"
"             tf - same as the f command but includes a self-test\n"
"                  as well (See Testing Guide).\n");
printf("<string | filename> - the string that follows command s or the file name that\n"
"                       follows the command f (see <cmd>).\n"
"<search string> - the string to search after the construction is done.\n"
"[p] - optional - print the tree. Printing is useful when dealing with\n"
"                 small trees, while printing a large tree\n"
"                 could take a long time.\n\n");
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


int main(int argc, char* argv[])
{
	SUFFIX_TREE* tree;
	char* str;
	bool freestr;
	DBL_WORD len = 0;
	if(argc < 4) PrintUsage();

	switch ( argv[ 1 ][ 0 ] )
	{
	/*'s' means an immediate string*/
	case 's':
		str = argv[ 2 ];
		len = strlen(str);
		break;
	/*'f' means a file*/
	case 'f': {
			  FILE* file = fopen((const char*)argv[ 2 ],"r");
			  /*Check for validity of the file.*/
			  if(!file)
			  {
				  printf("can't open file '%s'.\n", argv[ 2 ] );
				  return(1);
			  }
			  /*Calculate the file length in bytes. This will be the length of the source string.*/
			  fseek(file, 0, SEEK_END);
			  len = ftell(file);
			  fseek(file, 0, SEEK_SET);
			  str = (char*)malloc(len*sizeof(char));
			  if(str == 0)
			  {
				  printf("\nOut of memory.\n");
				  exit(0);
			  }
			  fread(str, sizeof(unsigned char), len, file);
			  freestr = true;
			  fclose(file );
		  }
		break;
	default:
		PrintUsage();
	}

	
	Timer full( "full-tree-construction" );
	Timer dots( "add-dot-links" );
	full.start();
	tree = ST_CreateTree(str,len);
	dots.start();
	ST_AddDotLinks( tree, 2 );
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

	if(freestr) free(str);
	ST_DeleteTree(tree);
	return 0;
}
