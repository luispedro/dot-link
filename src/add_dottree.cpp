#include "dottree.h"
#include "stats.h"
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
			if (p.at_leaf() || tree_->children(p.curnode()).is_null()) {
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
			if (tree_->is_dotnode(p.curnode())) {
				path_.push_back(".");
			} else {
				path_.push_back(std::string(tree_->string(),tree_->start(p), tree_->length(p)));
			}
			if (p.at_leaf() || tree_->children(p.curnode()).is_null()) {
				output();
			}
		}
		void visit_leaf(unsigned h) {
		}
		void finished() { delete this; }
		void up() { path_.pop_back(); }
		void down() { }
	private:
		void output() const {		
			out_ << "path: ";
			for (unsigned i = 0; i != path_.size(); ++i)
				out_ << path_[i];
			out_ << std::endl;
		
		}
		const dottree::tree* tree_;
		std::vector<std::string> path_;
		std::ostream& out_;
};
node* make_dot_node(unsigned sd) {
	node* res = new node(dot_node_marker,sd);
	//std::cout << "New Dot NODE: " << res << std::endl;
	return res;
}

bool has_dot_link(dottree::tree* t, node* n) {
	return t->children(n).valid() && t->is_dotnode(t->children(n));
}

node* copy_simple_(dottree::tree* t, nodep_or_idx n, int kdepth) {
	assert(kdepth >= 0);
	if (t->is_dotnode(n)) return new dottree::node(t->head(n), t->sdepth(n) + 1 + kdepth);
	return new dottree::node(t->head(n) - 1 - kdepth, t->sdepth(n) + 1 + kdepth);
}
node* copy_simple(dottree::tree* t, nodep_or_idx n, int kdepth) {
	node* res = copy_simple_(t,n,kdepth);
	std::cout << "copy_simple( ., " << n << ", " << kdepth << " ) => " << res << "[" << t->head(res) << ':' << t->sdepth(res) << "]\n";
	return res;
}

bool fulfils_cond(dottree::tree* t, nodep_or_idx n, char cond,int kdepth) {
	assert(kdepth >= 0);
	if (int(t->head(n)) <= kdepth || t->is_dotnode(n)) return false;
	bool res = (!cond || t->at(t->head(n) - 1 - kdepth) == cond);
	if (t->head(n) > (t->length() - kdepth)) res = false;
	std::cout << "fulfils_cond( . , " << n << ", " << cond << " ); => " << std::boolalpha << res << "\n";
	return res;
}

node* copy_recursive(dottree::tree* t, const nodep_or_idx n, const char cond, int kdepth) {
	std::cout << "copy_recursive( . , " << n << ", " << cond << ", " << kdepth << ");\n";
	if (n.is_null()) return 0;
	if (n.is_int() || !t->children(n)) {
		if (fulfils_cond(t,n,cond,kdepth)) {
			node* res = copy_simple(t,n, kdepth);
			res->next(copy_recursive(t,t->next(n),cond,kdepth));
			return res;
		}
		return copy_recursive(t,t->next(n),cond,kdepth);
	}


	nodep_or_idx children = t->children(n);
	if (children.is_null()) return copy_recursive(t,t->next(n), cond, kdepth);

	dottree::node* childrencopy = 0;
	if (kdepth > 0 || !t->is_dotnode(children)) {
		childrencopy = copy_recursive(t,children,cond,kdepth - int(t->is_dotnode(children)));
	} else {
		childrencopy = copy_recursive(t,t->next(children), cond, kdepth);
	}
	if (!childrencopy) return copy_recursive(t,t->next(n), cond,kdepth);
	dottree::node* res;
	if (childrencopy->next().is_null() && !t->is_dotnode(n)) {
		res = childrencopy;
	} else {
		res = copy_simple(t,n, kdepth);
		res->children(childrencopy);
	}
	res->next(copy_recursive(t,t->next(n),cond,kdepth));
	return res;
}

node* copy_at(dottree::tree* t, dottree::node* n, char cond) {
	//std::cout << "copy_at( . , " << n << " , \'" << cond << "\' )\n";
	//std::cout << "starting copy at: " << t->children(t->dot_link(n)) << "\n";
	return copy_recursive(t,t->children(t->dot_link(n)),cond,0);
}
void remove_dot_link(dottree::tree* t, node* n) {
	if (node* dot_link = t->dot_link(n)) {
		n->children(t->next(t->children(n)));
		dot_link->next(0);
		t->delete_subtree(dot_link);
	}
	assert(!has_dot_link(t,n));
}

void remove_dot_links_recur(dottree::tree* t, node* n) {
	remove_dot_link(t,n);
	for (nodep_or_idx cur = t->children(n); cur.valid(); cur = t->next(cur)) {
		if (cur.is_ptr()) {
			if (!t->is_dotnode(cur)) remove_dot_link(t,cur.as_ptr());
			remove_dot_links_recur(t,cur.as_ptr());
		}
	}
}

void add_dotlink(dottree::tree* t, node* n) {
	//std::cout << "add_dotlink( . , " << n << " )" << std::endl;
	assert(n);
	if (n->is_dotnode()) return; // I am a dot node....
	if (n == t->root()) return;
	if (has_dot_link(t,n)) return;

	add_dotlink(t,checked_cast<dottree::node_wsl*>(n)->suffixlink());
	//std::cout << "Adding dot link at " << n << std::endl;
	node* dot = make_dot_node(t->sdepth(n)+1);
	dot->children(copy_at(t,checked_cast<dottree::node_wsl*>(n)->suffixlink(),t->at(n->head())));
	dot->next(n->children(dot));
	//t->dfs(new dottree::print_all(t));
	//t->dfs(new print_paths(t));
}		

void add_dotlink_recur(dottree::tree* t, node* n) {
	for(nodep_or_idx cur = t->children(n); cur.valid(); cur = t->next(cur)) {
		if (!t->is_dotnode(cur) && cur.is_ptr()) {
			add_dotlink(t,cur.as_ptr());
			add_dotlink_recur(t,cur.as_ptr());
		}
	}
}
struct searcher {
	public:
		searcher(dottree::tree* t, const char* str, bool edit, const char* name = ""):
			tree_(t),
			pat_(str),
			len_(strlen(str)),
			edit_(edit),
			acc_(stats::accumulator(name))
		{ }
		bool search(dottree::position pos, unsigned start, int k) {
			acc_.add(1);
			if (k < 0) return false;
			if (start == len_) return count_leafs(pos);

			//deletion:
			if (edit_ && search(pos, start + 1, k - 1)) return true;

			if (pos.at_end()) {
				// substituition:
				if (search(dottree::position(pos.curnode().as_ptr(),
								tree_->dot_link(pos.curnode()),
								0),
							start+1,
							k - 1)) return true;
				// insertion:
				if (edit_ && search(dottree::position(
								pos.curnode().as_ptr(),
								tree_->dot_link(pos.curnode()),
								0),
							start,
							k - 1)) return true;
				// matching:
				nodep_or_idx child = tree_->child(pos.curnode(),pat_[start]);
				if (child.valid() && search(dottree::position(
								pos.curnode().as_ptr(),
								child,
								1),
							start + 1,
							k)) return true;
				return false;
			}
				//insertion
			if (edit_ && search(pos + 1, start, k - 1)) return true;
			// match or substituition:
			if (pat_[start] != tree_->at(pos)) --k;
			++pos;
			
			return search(pos, start + 1, k);
		}
	private:
		unsigned count_leafs(dottree::position pos) {
			return 1;
			std::vector<unsigned> leafs;
			tree_->dfs(pos, new gather_leafs(tree_,leafs));
			//tree_->dfs(pos, new dottree::print_all(tree_));
			return leafs.size();
		}
		dottree::tree* tree_;
		const char* pat_;
		const unsigned len_;
		const bool edit_;
		stats::accumulator_type& acc_;
};
	
void add_first_level_dotlink(dottree::tree* t, nodep_or_idx p, unsigned k) {
	if (t->is_leaf(p) || t->sdepth(p) != 1) return;
	node* n = p.as_ptr();
	std::cout << "add_first_level_dotlink( . , " << t->head(p) << ", " << k << " )\n";

	node* dot = make_dot_node(t->sdepth(n)+1);
	dot->children(copy_recursive(t,t->children(t->root()),t->at(n->head()),k));
	dot->next(n->children(dot));
	assert(has_dot_link(t,n));
	
}
} // namespace

void add_dotlinks(dottree::tree *t, unsigned k) {
//	assert(k <= 1);
	for (unsigned i = 0; i != k; ++i) {
		node* root = t->root();
		t->dfs(new dottree::print_all(t));
		//t->dfs(new print_paths(t));

		for (nodep_or_idx cur = t->children(root); cur.valid(); cur = t->next(cur)) {
			add_first_level_dotlink(t,cur,i+1);
		}
		for (nodep_or_idx cur = t->children(root); cur.valid(); cur = t->next(cur)) {
			//if (cur.is_ptr()) remove_dot_links_recur(t,cur.as_ptr());
		}
		
		std::cout << "\n\nADDED ROOT DOT:\n";
		t->dfs(new dottree::print_all(t));
		//t->dfs(new print_paths(t));
		add_dotlink_recur(t,t->root());
		std::cout << "\n\nADDED NEW LINKS:\n";
		t->dfs(new dottree::print_all(t));
		//t->dfs(new print_paths(t));
	}
	//t->dfs(new dottree::print_all(t));
	//t->dfs(new print_paths(t));
}
unsigned hamming_search(dottree::tree* t, const char* str, unsigned k, const char* name) {
	return searcher(t, str, false,name).search(t->rootp(), 0, k);
}
unsigned edit_search(dottree::tree* t, const char* str, unsigned k, const char* name) {
	return searcher(t, str, true,name).search(t->rootp(), 0, k);
}

