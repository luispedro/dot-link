#include "dottree.h"
#include <iostream>
using dottree::node;
using dottree::nodep_or_idx;

node* make_dot_node() {
	node* res = new node(uint(-1),uint(-1));
	std::cout << "New Dot NODE: " << res << std::endl;
	return res;
}

bool has_dot_link(node* n) {
	return n->children().valid() && n->children().is_ptr() && n->children().as_ptr()->head() == unsigned(-1);
}

node* copy_simple(dottree::tree* t, nodep_or_idx n) {
	if (n.is_ptr()) return new dottree::node(n.as_ptr()->head(),n.as_ptr()->sdepth());
	return new dottree::node(n.as_index(), t->length() - n.as_index());
}

node* copy_recursive(dottree::tree* t, nodep_or_idx n, char cond) {
	if (n.is_null()) return 0;
	if (n.is_int() || !n.as_ptr()->children()) {
		if (!cond || t->at(t->head(n)) == cond) return copy_simple(t,n);
		return 0;
	}

	dottree::node* children = copy_recursive(t, n.as_ptr()->children(),cond);
	if (!children) return 0;
	dottree::node* res = copy_simple(t,n);
	res->next(copy_recursive(t,t->next(n),cond));
	return res;
}

void add_dotlink(dottree::tree* t, node* n) {
	std::cout << "Adding dot link at " << n << std::endl;
	assert(n);
	if (n->head() == unsigned(-1)) return; // I am a dot node....
	if (n == t->root()) return;
	if (has_dot_link(n)) return;
	add_dotlink(t,n->suffixlink());
	node* dot = make_dot_node();
	dot->children(copy_recursive(t,n,t->at(n->head())));
	dot->next(n->children(dot));
}
class dot_linker : public dottree::node_visitor{
	public:
		dot_linker(dottree::tree * t): t(t) { }
		void finished() { delete this; }
		void visit_node(dottree::position p) { if (!p.is_leaf()) add_dotlink(t,p.curnode().as_ptr()); }
		void visit_leaf(unsigned) { }
	private:
		dottree::tree* t;
};

void add_dotlinks(dottree::tree *t, unsigned ) {
	node* root_dot = make_dot_node();
	root_dot->children(copy_recursive(t,t->root(), false));
	root_dot->next(t->root()->children(root_dot));
	t->dfs(new dot_linker(t));
}

