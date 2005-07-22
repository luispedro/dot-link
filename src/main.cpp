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


/**************************************************************************************************/
/**************************************************************************************************/

/*The program entry point. Handles all command line arguments. 
This function calls suffix tree interface functions in order to 
create, search and delete a suffix tree out of a string or a 
file given as input. 

  Input : Number of command-line parameters, an array of command-line parameters.
  
  Output: Returns 0 to the operating system.*/


/**************************************************************************************************/
int main(int argc, char* argv[])
{
	SUFFIX_TREE* tree;
	unsigned char command, *str = NULL, freestr = 0;
	DBL_WORD i,len = 0;
	/*If less then 3 arguments - print a proper message and exit the program.*/
	if(argc < 3) PrintUsage();
		
	/*'t' means selfTest request.*/
	if(argv[1][0] == 't')
		command = argv[1][1];
	else
	{
		if(argc < 4)
			PrintUsage();
		command = argv[1][0];
	}

	switch(command)
	{
	/*'s' means an immediate string*/
	case 's':
		str = (unsigned char*)argv[2];
		len = strlen((const char*)str);
		break;
	/*'s' means a file*/
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
			  str = (unsigned char*)malloc(len*sizeof(unsigned char));
			  if(str == 0)
			  {
				  printf("\nOut of memory.\n");
				  exit(0);
			  }
			  fread(str, sizeof(unsigned char), len, file);
			  /*When freestr = 1 it means that a temporary string has been allocated and therefor 
			    must be deleted afterwards.*/
			  freestr = 1;
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
	ST_AddDotLinks( tree );
	full.stop();
	dots.stop();
	
	/*If 'p' was included in the command-line arguments - print the tree.*/
	if((argc == 5 && argv[4][0] == 'p') || (argv[1][0] == 't' && argc == 4 && argv[3][0] == 'p'))
		ST_PrintTree(tree);

	if(argv[1][0] == 't')
		ST_SelfTest(tree);
	else
	{
		i = ST_FindSubstringWithErrors(tree, (unsigned char*)(argv[3]), strlen(argv[3]));
		if(i == ST_ERROR)
			printf("\nResults:      String is not a substring.\n\n");
		else
			std::cout << boost::format( "\nResults:      Substring exists in position %s.\n\n" ) % i;

		std::cout << boost::format( "String size: %s \n" ) % tree->length;
		std::cout << boost::format( "Nodes without dot links: %s \n" ) % ST_CountNodes( tree, false );
		std::cout << boost::format( "Nodes with dot links: %s \n" ) % ST_CountNodes( tree, true );
	}

	/*Delete the temporary string.*/
	if(freestr == 1)
		free(str);

	ST_DeleteTree(tree);
	return 0;
}
