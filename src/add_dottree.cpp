#include "dottree.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
using dottree::node;
using dottree::nodep_or_idx;
using dottree::dot_node_marker;
namespace {

class gather_leafs : public dottree::node_visitor {
	public:
		explicit gather_leafs(dottree::tree* t, std::vector<unsigned>& results):
			tree_(t),
			res_(results)
		{ }
		void visit_node(dottree::position p) {
			if (p.at_leaf() || p.curnode().as_ptr()->children().is_null()) {
				res_.push_back(tree_->head(p.curnode()));
			}
		}
		void visit_leaf(unsigned h) { }
		void finished() { delete this; }
	private:
		dottree::tree* tree_;
		std::vector<unsigned>& res_;
};
class print_paths: public dottree::node_visitor {
	public:
		explicit print_paths(const dottree::tree* t):
			tree_(t),
			out_(std::cout)
		{}
		void visit_node(dottree::position p) {
			if (tree_->head(p.curnode()) == dot_node_marker) {
				path_.push_back(".");
			} else {
				path_.push_back(std::string(tree_->string(),tree_->start(p), tree_->length(p)));
			}
			if (p.at_leaf() || p.curnode().as_ptr()->children().is_null()) {
				for (unsigned i = 0; i != path_.size(); ++i)
					out_ << path_[i];
				out_ << std::endl;
			}
		}
		void visit_leaf(unsigned h) {
		
		}
		void finished() { delete this; }
		void up() { path_.pop_back(); }
		void down() { }
	private:
		const dottree::tree* tree_;
		std::vector<std::string> path_;
		std::ostream& out_;
};
node* make_dot_node(unsigned sd) {
	node* res = new node(dot_node_marker,sd);
	//std::cout << "New Dot NODE: " << res << std::endl;
	return res;
}

bool has_dot_link(node* n) {
	return n->children().valid() && n->children().is_ptr() && n->children().as_ptr()->head() == dot_node_marker;
}

node* copy_simple(dottree::tree* t, nodep_or_idx n) {
	if (t->head(n) == dot_node_marker) return new dottree::node(t->head(n), t->sdepth(n) + 1);
	return new dottree::node(t->head(n) - 1, t->sdepth(n) + 1);
}

bool fulfils_cond(dottree::tree* t, nodep_or_idx n, char cond) {
	if (t->head(n) == 0 || t->head(n) == dot_node_marker) return false;
	return !cond || t->at(t->head(n) - 1) == cond;
}

node* copy_recursive(dottree::tree* t, const nodep_or_idx n, const char cond) {
	if (n.is_null()) return 0;
	if (n.is_int() || !n.as_ptr()->children()) {
		if (fulfils_cond(t,n,cond)) {
			node* res = copy_simple(t,n);
			res->next(copy_recursive(t,t->next(n),cond));
			return res;
		}
		return copy_recursive(t,t->next(n),cond);
	}

	dottree::node* children = copy_recursive(t, n.as_ptr()->children(),cond);
	if (!children) return copy_recursive(t,t->next(n), cond);
	dottree::node* res;
	if (children->next().is_null() && t->head(n) != dot_node_marker) {
		res = children;
	} else {
		res = copy_simple(t,n);
		res->children(children);
	}
	res->next(copy_recursive(t,t->next(n),cond));
	return res;
}

node* copy_at(dottree::tree* t, dottree::node* n, char cond) {
	//std::cout << "copy_at( . , " << n << " , \'" << cond << "\' )\n";
	//std::cout << "starting copy at: " << t->dot_link(n)->children() << "\n";
	return copy_recursive(t,t->dot_link(n)->children(),cond);
}

void delete_subtree(node* n) {
	//std::cout << "delete_subtree( " << n << " )\n";
	if (!n) return;
	delete_subtree(n->children().as_ptr());
	delete_subtree(n->next().as_ptr());
	delete n;
}

void remove_dot_link(dottree::tree* t, node* n) {
	if (node* dot_link = t->dot_link(n)) {
		n->children(t->next(n->children()));
		dot_link->next(0);
		delete_subtree(dot_link);
	}
	assert(!has_dot_link(n));
}

void remove_dot_links_recur(dottree::tree* t, node* n) {
	remove_dot_link(t,n);
	for (nodep_or_idx cur = n->children(); cur.valid(); cur = t->next(cur)) {
		if (cur.is_ptr()) {
			if (t->head(cur) != dot_node_marker) remove_dot_link(t,cur.as_ptr());
			remove_dot_links_recur(t,cur.as_ptr());
		}
	}
}

void add_dotlink(dottree::tree* t, node* n) {
	//std::cout << "add_dotlink( . , " << n << " )" << std::endl;
	assert(n);
	if (n->head() == dot_node_marker) return; // I am a dot node....
	if (n == t->root()) return;
	if (has_dot_link(n)) return;

	add_dotlink(t,n->suffixlink());
	//std::cout << "Adding dot link at " << n << std::endl;
	node* dot = make_dot_node(t->sdepth(n)+1);
	dot->children(copy_at(t,n->suffixlink(),t->at(n->head())));
	dot->next(n->children(dot));
	//t->dfs(new dottree::print_all(t));
	//t->dfs(new print_paths(t));
}		

void add_dotlink_recur(dottree::tree* t, node* n) {
	for(nodep_or_idx cur = n->children(); cur.valid(); cur = t->next(cur)) {
		if (t->head(cur) != dot_node_marker && cur.is_ptr()) {
			add_dotlink(t,cur.as_ptr());
			add_dotlink_recur(t,cur.as_ptr());
		}
	}
}
struct searcher {
	public:
		searcher(dottree::tree* t, const char* str, bool edit = false):
			tree_(t),
			pat_(str),
			len_(strlen(str)),
			edit_(edit)
		{ }
		unsigned search(dottree::position pos, unsigned start, unsigned k) {
			if (start == len_) return count_leafs(pos);
			if (pos.at_end()) {
				if (k > 0) {
					unsigned here = search(
							dottree::position(
								pos.curnode().as_ptr(),
								tree_->dot_link(pos.curnode()),
								0),
							start+1,
							k - 1);
					if (here != unsigned(-1)) return here;
				}
				nodep_or_idx child = tree_->child(pos.curnode(),pat_[start]);
				if (child.valid()) {
					return search(
							dottree::position(
								pos.curnode().as_ptr(),
								child,
								1),
							start + 1,
							k);
				}
				return unsigned(-1);
			}
			if (edit_ && k > 0) {
				// deletion
				unsigned here = search(pos, start + 1, k - 1);
				if (here != unsigned(-1)) return here; 
				//insertion
				here = search(pos + 1, start, k - 1);
				if (here != unsigned(-1)) return here; 
			}
			if (pat_[start] != tree_->at(pos)) {
				if (k > 0) --k;
				else return unsigned(-1);
			}
			++pos;
			
			return search(pos, start + 1, k);
		}
	private:
		unsigned count_leafs(dottree::position pos) {
			std::vector<unsigned> leafs;
			tree_->dfs(pos, new gather_leafs(tree_,leafs));
			tree_->dfs(pos, new dottree::print_all(tree_));
			return leafs.size();
		}
		dottree::tree* tree_;
		const char* pat_;
		const unsigned len_;
		const bool edit_;
};
} // namespace
	
void add_dotlinks(dottree::tree *t, unsigned k) {
	for (unsigned i = 0; i != k; ++i) {
		node* root = t->root();
		node* root_dot = make_dot_node(1);
		root_dot->children(copy_recursive(t,root->children(), false));
		remove_dot_links_recur(t,root);
		//std::cout << "\n\nREMOVED OLD LINKS:\n";
		//t->dfs(new dottree::print_all(t));
		//t->dfs(new print_paths(t));

		root_dot->next(root->children());
		root->children(root_dot);
		//std::cout << "\n\nADDED ROOT DOT:\n";
		//t->dfs(new dottree::print_all(t));
		//t->dfs(new print_paths(t));
		add_dotlink_recur(t,t->root());
		//std::cout << "\n\nADDED NEW LINKS:\n";
		//t->dfs(new dottree::print_all(t));
		//t->dfs(new print_paths(t));
	}
	//t->dfs(new dottree::print_all(t));
	//t->dfs(new print_paths(t));
}
unsigned hamming_search(dottree::tree* t, const char* str, unsigned k) {
	return searcher(t, str, false).search(t->rootp(), 0, k);
}
unsigned edit_search(dottree::tree* t, const char* str, unsigned k) {
	return searcher(t, str, true).search(t->rootp(), 0, k);
}

