#include "dottree.h"

unsigned dottree::node::cur_alloc_;
unsigned dottree::node::max_alloc_;
unsigned dottree::node_wsl::cur_alloc_;
unsigned dottree::node_wsl::max_alloc_;

unsigned dottree::node::allocated_nodes() {
	return max_alloc_ + dottree::node_wsl::allocated_nodes(); 
}

namespace dottree {
std::ostream& operator << ( std::ostream& out, const nodep_or_idx& n ) {
	if (n.is_null()) return out << "null";
	if ( n.is_ptr() ) return out << "ptr(" << n.as_ptr() << ")";
	return out << "idx(" << n.as_index() << ")";
}

std::ostream& operator << (std::ostream& out, const position& p) {
	return out << "[" << p.curnode() << "+" << p.offset() << " (^" << p.parent() << ")]";
}
}

namespace {
class mcreight_builder {
	public:
		mcreight_builder(const char* str, char dollar, bool str_own):
			tree_(0),
			str_(str),
			str_own_(str_own),
			dollar_(dollar),
			suffixless_(0)
		{ }

		std::auto_ptr<dottree::tree> build() {
			tree_ = std::auto_ptr<dottree::tree>(new dottree::tree(str_,strlen(str_) + (dollar_? 0 : 1),dollar_,str_own_));
			tree_->root_ = new dottree::node_wsl(0, 0);
			checked_cast<dottree::node_wsl*>(tree_->root_)->suffixlink(tree_->root_);
			dottree::position pos(tree_->root_,dottree::nodep_or_idx(tree_->root_), 0);
			unsigned sdepth = 0;
			for (unsigned head = 0; head != tree_->length(); ++head) {
				if (sdepth) --sdepth;
				while (tree_->descend(pos, tree_->at(head+sdepth))) ++sdepth;
				// I'm at a breaking point
				add_branch(pos, head, sdepth);
				pos = fast_suffix_link(pos);
				if (pos.at_end() && suffixless_) {
					suffixless_->suffixlink(pos.curnode().as_ptr());
					suffixless_ = 0;
				}
			}
			return tree_;
		}
	private:
		dottree::position fast_suffix_link(dottree::position pos) {
			//std::cout << "\n\n\n\nstarting fast_suffix_link at: " << pos << '\n';
			//tree_->print();
			assert(pos.at_end());
			assert(!pos.at_leaf());
			if (pos.curnode() == tree_->root()) return pos;
			if (tree_->sdepth(pos.curnode()) == 1) {
				//std::cout << "I'm at the root...\n";
				return dottree::position(tree_->root_, dottree::nodep_or_idx(tree_->root_), 0);
			}
			unsigned str_start = tree_->start(pos.curnode(),pos.parent());
			const unsigned target = tree_->sdepth(pos.curnode()) - 1;
			dottree::nodep_or_idx cur = dottree::nodep_or_idx(pos.parent());
			assert(!cur.is_null());
			//std::cout << cur.as_ptr() << " == "  << tree_->root_ << " \n";
			if (cur.as_ptr() == tree_->root_) {
				//std::cout << "cur == tree_->root_\n";
				++str_start;
			} else cur.set(checked_cast<dottree::node_wsl*>(cur.as_ptr())->suffixlink());
			assert(!cur.is_null());

			//std::cout << "Skipping: \"" << std::string(str_,str_start,target - tree_->sdepth(cur)) 
			//		<< "\" (" << int(target - tree_->sdepth(cur)) << ") starting at " << cur << "\n";

			dottree::node* par = 0;
			while (tree_->sdepth(cur) < target ) {
				par = cur.as_ptr();
				cur = tree_->child(cur.as_ptr(),str_[str_start]);
				assert(!cur.is_null());
				str_start += tree_->length(cur,par);
			}
			assert(par);
			return dottree::position(par,cur, target - tree_->sdepth(par));
		}
			
		void add_branch(dottree::position& pos, unsigned head, unsigned sdepth) {
			//std::cout << "add_branch( " << pos << ", " << head << ", " << sdepth << " )\n";
			dottree::nodep_or_idx leaf = tree_->leaf(head);
			dottree::node* parent = pos.parent();
			if (pos.at_end()) {
				tree_->add_child(pos.curnode().as_ptr(),leaf);
			} else {
				dottree::node_wsl* top = new dottree::node_wsl(head, sdepth);
				dottree::nodep_or_idx cur = pos.curnode();
				tree_->add_child(parent,dottree::nodep_or_idx(top));
				//std::cout << "BEFORE REMOVE CHILD\n";
				//tree_->print(std::cout);

				tree_->remove_child(parent,cur);
				//std::cout << "AFTER REMOVE CHILD\n";
				//tree_->print(std::cout);

				tree_->add_child(top,cur);
				tree_->add_child(top,leaf);
				if (suffixless_) {
					suffixless_->suffixlink(top);
					suffixless_ = 0;
				}
				if (top->sdepth() == 1) top->suffixlink(tree_->root_);
				else suffixless_ = top;
				pos = dottree::position(parent, dottree::nodep_or_idx(top), top->length(parent));
			}
			//std::cout << "SOFAR:\n";
			//tree_->print(std::cout);
		}
			
		std::auto_ptr<dottree::tree> tree_;
		const char* str_;
		bool str_own_;
		char dollar_;
		dottree::node_wsl* suffixless_;
};
}

int dottree::tree::match(const char* pat) const {
	assert(pat);
	dottree::position pos = dottree::position(root_,dottree::nodep_or_idx(root_),0);
	while ( *pat && descend(pos,*pat)) ++pat;
//X 	if (*pat) return -1;
	return head(pos.curnode());
}

/**
 * Advances pos by ch
 *
 * Returns true if possible (there is a letter match)
 * If there is no match, returns false and pos is unchanged
 */
bool dottree::tree::descend(dottree::position& pos, char ch) const {
	//std::cout << "descend( " << pos << ", '" << ch << "' )\n";
	if (pos.at_end()) {
		if (pos.at_leaf()) return false;
		//std::cout << "at end!\n";
		dottree::nodep_or_idx next = const_cast<dottree::tree*>(this)->child(pos.curnode().as_ptr(), ch);
		if (!next) return false;
		pos = dottree::position(pos.curnode().as_ptr(),next, 1);
		return true;
	}
	//std::cout << this->at(start(pos.curnode(),pos.parent()) + pos.offset()) << " =?= " << ch << "\n";
	if (this->at(start(pos.curnode(),pos.parent()) + pos.offset()) == ch) {
		pos = dottree::position(pos.parent(),pos.curnode(), pos.offset() + 1);
		return true;
	}
	return false;
}

void dottree::tree::dfs(dottree::node* par, dottree::nodep_or_idx node, dottree::node_visitor* visit) const {
	//std::cout << "dottree::tree::dfs( " << node << ", . )\n";

	visit->visit_node(dottree::position(par,node,0));
	if (node.is_int()) visit->visit_leaf(head(node));
	
	visit->down();
	if (node.is_ptr() && !node.as_ptr()->children_.is_null()) dfs(node.as_ptr(),node.as_ptr()->children_, visit);
	visit->up();

	if (!next(node).is_null()) dfs(par,next(node), visit);
}	

void dottree::tree::add_child(dottree::node* par, dottree::nodep_or_idx n) {
	//std::cout << "add_child( " << par << ", " << n << ")\n";
	//print_leafvector();
	if (!par->children_) {
		//std::cout << "setting: " << par << "->children_ to " << n << " (was null)\n";
		par->children_ = n;
		next(n,nodep_or_idx());
	} else {
		const char ch = string_[start(n,par)];
		dottree::nodep_or_idx prev = par->children_;
		if (at(start(prev,par)) > ch) {
			par->children_ = n;
			//std::cout << "setting: " << par << "->children_ to " << n << " (was " << prev << ")\n";
			next(n,prev);
		} else {
			while (!next(prev).is_null() && string_[start(next(prev), par)] < ch) prev = next(prev);
			//std::cout << "setting: " << n << " between " << prev << " and " << next(prev) << '\n';
			next(n,next(prev));
			next(prev,n);
		}
	}
	//print_leafvector();
}

void dottree::tree::remove_child(dottree::node* par, dottree::nodep_or_idx child) {
	//std::cout << "remove_child( " << par << ", " << child << ")\n";
	//print_leafvector();
	if (child == par->children_) {
		//std::cout << "setting " << par << "->children_ to " <<  next(child) << '\n';
		par->children_ = next(par->children_);
	}
	else {
		dottree::nodep_or_idx bef = par->children_;
		while (!bef.is_null() && next(bef) != child) bef = next(bef);
		if (!bef.is_null()) {
			//std::cout << "setting (" << bef<< ") to " <<  next(next(bef)) << '\n';
			next(bef, next(next(bef)));
			next(child,nodep_or_idx());
		} else {
			//std::cout << child << " not found!\n";
		}
	}
	//print_leafvector();
}


dottree::nodep_or_idx dottree::tree::child(node* n, char ch) {
	//std::cout << "child( " << n << ", \'" << ch << "\' )\n";
	nodep_or_idx cur = n->children_;
	//std::cout << "Starting at: " << cur << " (gotten from " << n << "->children_)\n";
	while (cur.valid() && (head(cur) == dot_node_marker || string_[start(cur,n)] != ch)) {
		//if (head(cur) != dot_node_marker) std::cout << "Looking at " << cur << " (\'" << at(start(cur,n)) << "\')\n";
		//else std::cout << "Skipping dot node\n";
		cur = next(cur);
	}
	//std::cout << "returning " << cur << '\n';
	return cur;
}

dottree::node* dottree::tree::dot_link(nodep_or_idx n) {
	if (children(n).is_null()) return 0;
	if (head(children(n)) == dot_node_marker) return children(n).as_ptr();
	return 0;
}

void dottree::tree::print_leafvector() const {
	for (unsigned i = 0; i != length(); ++i) {
		std::cout << leafs_[i] << ' ';
	}
	std::cout << std::endl;
}


void dottree::print_all::visit_node(dottree::position p) {
	out_ << prefix_
		<< p << ": (" << tree_->head(p) << "-" << tree_->sdepth(p) << ") ";
	if (tree_->head(p) == dot_node_marker) {
		out_ << "[\".\"]";
	} else {
		out_ << "[\"" << std::string(tree_->string(),tree_->start(p),tree_->length(p)) << "\"]";
	}
	//if (!p.at_leaf()) out_ << " suflink: " << p.curnode().as_ptr()->suffixlink();
	out_ << " ==> " << tree_->next(p);
	out_ << std::endl;
}

std::auto_ptr<dottree::tree> dottree::build_tree(const char* orig, char dollar, bool copy_string) {
	const char* fixed;
	if (copy_string) {
		unsigned len = strlen(orig);
		assert(!dollar || !strchr(orig,dollar) || (strchr(orig,dollar) && !*(strchr(orig,dollar)+1)));
		char* copy = static_cast<char*>(malloc(len + 2));
		strcpy(copy,orig);
		if (orig[len-1] != dollar) {
			copy[len] = dollar;
			copy[len + 1] = '\0';
		}

		fixed = copy;
	} else {
		// Either dollar is '\0' or it occurs in the string, but only at the last position
		assert(!dollar || (strchr(orig,dollar) && !*(strchr(orig,dollar)+1)));
		fixed=orig;
	}
	mcreight_builder builder(fixed, dollar,copy_string);
	std::auto_ptr<dottree::tree> res = builder.build();
	//std::cout << "Final Tree:\n";
	//res->print(std::cout);
	return res;
}

