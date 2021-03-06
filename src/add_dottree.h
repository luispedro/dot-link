#ifndef LPC_ADD_DOTTREE_H1137158521_INCLUDE_GUARD_
#define LPC_ADD_DOTTREE_H1137158521_INCLUDE_GUARD_

#include "dottree.h"


void add_dotlinks(dottree::tree* t, unsigned);
unsigned edit_search(dottree::tree* t, const char* str, unsigned k, const char* name = 0);
unsigned hamming_search(dottree::tree* t, const char* str, unsigned k, const char* name = 0);


#endif /* LPC_ADD_DOTTREE_H1137158521_INCLUDE_GUARD_ */
