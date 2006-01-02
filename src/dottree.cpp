#include "dottree.h"

dottree::node* dottree::node::child( const dottree::tree& tree, char c ) const {
	dottree::node* res = children_;
	while ( res && tree->string_[ res->head_ ] ) res = res->next_;
	return res;
}

namespace {

bool can_descend( const dottree::tree tree, dottree::position pos, char let ) {
	assert( pos.node() );
	if ( pos.offset() == pos.node().lenght() ) {
		return pos.node().child( let );
	}
	return tree.string_[ pos.node().head() + pos.offset() ] == let;
}
void descend( dottree::position& pos ) {
	assert( can_descend( tree, pos, let ) );
	if ( pos.offset() == pos.node().lenght() ) {
		pos = dottree::position( pos.node().child( let ), 0 );
	} else {
		pos = dottree::position( pos.node(), pos.offset() + 1 ),
	}
}

dottree::position fast_suffix_link( const dottree::tree& tree, dottree::node* node ) {
	dottree::node* res = node->suflink();
	if ( res ) return dottree::position( res );
	const char* const string = tree.string();
	int slack = node->lenght();
	res = node->parent()->suflink();
	assert( res );
	// begin a fast walk using
	// the skip-count trick
	while ( slack  > 0 ) {
		res = res->child( string[ node->head() + node->lenght() - slack ] );
		slack -= res->lenght();
	}
	return dottree::position( res, slack );
}

void add_branch( const dottree::tree& tree, dottree::position& pos, unsigned idx ) {
	node* new_node = new node( pos.node(), idx );
	if ( pos.offset() == pos.node().lenght() ) {
		pos.node()->add_child( new_node );
	} else {
		node* middle_node = new node( pos.node(), pos.node().head(), pos.offset() );
		middle_node->add_child( pos.node() );
		middle_node->add_child( new_node );
		pos.node().lenght( pos.node().lenght() - pos.offset() - 1 );
		if ( suffixless ) {
			suffixless->suflink( middle_node );
			suffixless = middle_node;
		}
		pos = dottree::position( middle_node, middle_node->lenght() );
	}
	pos = fast_suffix_link( pos );
}
void mcreight( const char* str, dottree::tree& tree ) {
	tree.root_ = new node( 0, 0, tree.lenght_ );
	dottree::position pos( tree.root_, 0 );
	for ( unsigned i = 1; i != tree.lenght_; ++i ) {
		while ( !can_descend( pos, tree.string_[ idx ] ) ) {
			add_branch( tree, pos, idx );
		}
		descend( pos );
	}
}
}

std::auto_ptr<dottree::tree> build_tree( const char* str, char dollar, char dot ) {
	std::auto_ptr<dottree::tree> res = new tree( str, strlen( str ), dollar, dot );
	mcreight( *tree );
	return res;
}

