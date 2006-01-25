#include "dottree.h"
#include "timer.h"
#include "stats.h"
#include "add_dottree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <exception>
#include <boost/format.hpp>

void usage()
{
	printf(	"usage: suffixtree k filename searchstring\n" );
	exit(0);
}

typedef unsigned (*search_func)(dottree::tree*, const char*, unsigned,const char*);

void find_substring( dottree::tree* tree,
		const char* string,
	       	unsigned k,
		search_func search,
		const char* name,
		CummulativeTimer* m = 0 ) {
	Timer match( "match_edit" );
	if ( m ) m->start();
	int res = search(tree,string, k, (m ? m->name(): ""));
	match.stop();
	if ( m ) m->stop();

	if(res < 0)
		std::cout << boost::format("\nResults: String[-%s-] is not an %s substring.\n\n") % string %name;
	else
		std::cout << boost::format( "\nResults: String[-%s-] exists %s times (%s).\n\n" ) % string % res % name;
}
void find_substring( dottree::tree* tree,
		const char* string,
	       	unsigned k,
		CummulativeTimer* m1 = 0,
	        CummulativeTimer* m2 = 0) {
	find_substring(tree, string,k, edit_search, "edit", m1);
	find_substring(tree, string,k, hamming_search, "hamming", m2);
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
	if(argc < 4) usage();
	int k = atoi( argv[ 1 ] );
	char* str = read_file( argv[ 2 ] );
	
	try { 
		Timer full( "full-tree-construction" );
		Timer dots( "add-dot-links" );
		full.start();
		tree = dottree::build_tree(str);
		std::cout << boost::format("Nodes 0-order: %s\n") % dottree::node::allocated_nodes();
		dots.start();
		add_dotlinks(tree.get(),k);
		full.stop();
		dots.stop();
		//tree->dfs(new dottree::print_leafs);
	} catch ( const std::exception& e ) {
		std::cerr << "Error [construction]: (exception): " << e.what() << std::endl;
	}
	
	
	try {
		if ( strcmp( argv[ 3 ], "-" ) ) {
			find_substring( tree.get(), argv[ 3 ], k );
		} else {
			std::string tmp;
			CummulativeTimer* cumm1 = 0;
			CummulativeTimer* cumm2 = 0;
			while (std::getline(std::cin, tmp,'\0')) {
				if (tmp.substr(0, 5) == "timer") {
					delete cumm1;
					delete cumm2;
					cumm1 = new CummulativeTimer((tmp.substr(6,std::string::npos) + ".subst").c_str());
					cumm2 = new CummulativeTimer((tmp.substr(6,std::string::npos) + ".edit").c_str());
				} else find_substring(tree.get(), tmp.c_str(), k, cumm1, cumm2);
			}
		}
	} catch ( const std::exception& e ) {
		std::cerr << "Error [matching]: (exception): " << e.what() << std::endl;
	}
	try {
		/*std::cout << boost::format( "Nodes without dot links: %s \n" ) % ST_CountNodes( tree, false );
		std::cout << boost::format( "Nodes with dot links: %s \n" ) % ST_CountNodes( tree, true );
*/
		stats::print();
		std::cout << boost::format("Nodes Allocated: %s\n") % dottree::node::allocated_nodes();
		std::cout << boost::format("sizeof(node): %s\n") % sizeof(dottree::node);
		std::cout << boost::format("sizeof(node_wsl): %s\n") % sizeof(dottree::node_wsl);
		std::cout << boost::format( "String size: %s \n" ) % ( tree->length() - 1 );

		free(str);
	} catch ( const std::exception& e ) {
		std::cerr << "Error [statting]: (exception): " << e.what() << std::endl;
	}
	return 0;
}

