#include "dottree.h"

namespace {

std::ostream& operator << (std::ostream& out, const dottree::position& p) {
	return out << "[" << p.curnode() << "+" << p.offset() << " (^" << p.parent() << ")]";
}

class mcreight_builder {
	public:
		mcreight_builder(const char* str, char dollar, char dot):
			tree_(0),
			str_(str),
			suffixless_(0)
		{ }

		std::auto_ptr<dottree::tree> build() {
			tree_ = std::auto_ptr<dottree::tree>(new dottree::tree(str_,strlen(str_),dollar_, dot_));
			tree_->root_ = new dottree::node(0, 0);
			tree_->root_->suffixlink(tree_->root_);
			dottree::position pos(tree_->root_,tree_->root_, 0);
			unsigned sdepth = 0;
			for (unsigned head = 0; head != tree_->length(); ++head) {
				if (sdepth) --sdepth;
				 if (!sdepth)
					while (tree_->descend(pos, tree_->at(head+sdepth))) ++sdepth;
				// I'm at a breaking point
				add_branch(pos, head, sdepth);
				pos = fast_suffix_link(pos);
			}
			return tree_;
		}
	private:
		dottree::position fast_suffix_link(dottree::position pos) {
			std::cout << "starting fast_suffix_link at: " << pos << '\n';
			assert(pos.at_end());
			assert(!pos.at_leaf());
			if (pos.curnode()->suffixlink()) {
				std::cout << "easy...\n";
				return dottree::position(pos.parent()->suffixlink(),pos.curnode()->suffixlink(),0);
			}
			if (pos.curnode()->sdepth() == 1) {
				std::cout << "I'm at the root...\n";
				return dottree::position(tree_->root_, tree_->root_, 0);
			}
			unsigned str_start = pos.curnode()->start(pos.parent());
			unsigned to_skip = pos.offset();
			dottree::node* cur = pos.parent();
			assert(cur);
			std::cout << cur << " == "  << tree_->root_ << " \n";
			if (cur == tree_->root_) {
				std::cout << "cur == tree_->root_\n";
				--to_skip;
				++str_start;
			} else cur = cur->suffixlink();
			assert(cur);
			std::cout << "Skipping: \"" << std::string(str_,str_start,to_skip) 
				<< "\" (" << to_skip << ") starting at " << cur << "\n";
			assert(to_skip);

			dottree::node* par;
			while (1) {
				par = cur;
				cur = tree_->child(cur,str_[str_start]);
				assert(cur);
				std::cout << "STEP( \'" << str_[str_start] << "\' ) => "
					<< cur << " (" << cur->length(par) << ") to_go: " << to_skip << "\n";
				if (to_skip <= cur->length(par)) break;
				to_skip -= cur->length(par);
				str_start += cur->length(par);
			}
			std::cout << "Result: " << dottree::position(par,cur, to_skip) << '\n';
			return dottree::position(par,cur, to_skip);
		}
			
		void add_branch(dottree::position& pos, unsigned head, unsigned advance) {
			std::cout << "add_branch( { " << pos.curnode() << "; " << pos.offset() << " } , "
				<< head << ", " << advance << " )\n";
			dottree::node* leaf = new dottree::node(head, tree_->length() - head);
			dottree::node* parent = pos.parent();
			if (pos.at_end()) {
				pos.curnode()->add_child(str_,leaf);
			} else {
				dottree::node* top = new dottree::node(head, advance);
				dottree::node* cur = pos.curnode();
				parent->add_child(str_,top);
				parent->remove_child(cur);
				top->add_child(str_,cur);
				top->add_child(str_,leaf);
				if (suffixless_) {
					suffixless_->suffixlink(top);
					suffixless_ = 0;
				}
				if (top->sdepth() == 1) top->suffixlink(tree_->root_);
				else suffixless_ = top;
				pos = dottree::position(parent,top, top->length(parent));
			}
			leaf->print(str_,pos.curnode(),std::cout, "built: ", false);
			tree_->root_->print(str_,tree_->root_,std::cout, "tree so far: ");
		}
			
		std::auto_ptr<dottree::tree> tree_;
		const char* str_;
		char dollar_;
		char dot_;
		dottree::node* suffixless_;
};
}

int dottree::tree::match(const char* pat) const {
	assert(pat);
	dottree::position pos = dottree::position(root_,root_,0);
	while ( *pat && descend(pos,*pat)) ++pat;
	if (*pat) return -1;
	return pos.curnode()->head();
}

/**
 * Advances pos by ch
 *
 * Returns true if possible (there is a letter match)
 * If there is no match, returns false and pos is unchanged
 */
bool dottree::tree::descend(dottree::position& pos, char ch) const {
	std::cout << "descend( " << pos << ", '" << ch << "' )\n";
	if (pos.at_end()) {
		std::cout << "at end!\n";
		dottree::node* next = const_cast<dottree::tree*>(this)->child(pos.curnode(), ch);
		if (!next) return false;
		pos = dottree::position(pos.curnode(),next, 1);
		return true;
	}
	if (this->at(pos.curnode()->start(pos.parent()) + pos.offset()) == ch) {
		pos = dottree::position(pos.parent(),pos.curnode(), pos.offset() + 1);
		return true;
	}
	return false;
}

void dottree::node::print(const char* str, const node* par, std::ostream& out, std::string prefix, bool recurs) const {
	out << prefix
		<< this << ": (head: " << head() << "; sdepth: " << sdepth() 
			<< "; " << start(par) << '-' << length(par) << ") \t"
		<< "[\"" << std::string(str,head(),sdepth()-length(par)) 
			<< '_' << std::string(str,start(par),length(par)) << "\"] "
		<< " suffixlink: " << suffixlink() << std::endl;
	if (!recurs) return;
	if (children_) children_->print(str,this,out,prefix + "   ");
	if (next_) next_->print(str,par,out,prefix);
}

void dottree::tree::dfs(dottree::node* node, dottree::node_visitor* visit) const {
	visit->visit_node(node);
	if (node->children_) dfs(node->children_, visit);
	if (node->next_) dfs(node->next_, visit);
}	

void dottree::node::add_child(const char* str, node* c) {
	if (!children_) {
		children_ = c;
		c->next_ = 0;
	} else {
		const char ch = str[c->head() + sdepth()];
		dottree::node* prev = children_;
		if (str[prev->start(this)] > ch) {
			c->next_ = prev;
			children_ = c;
		} else {
			while (prev->next_ && str[prev->next_->start(this)] < ch) prev = prev->next_;
			c->next_ = prev->next_;
			prev->next_ = c;
		}
	}
}

std::auto_ptr<dottree::tree> dottree::build_tree(const char* orig, char dollar, char dot) {
	assert(!strchr(orig,dollar));
	assert(!strchr(orig,dot));
	unsigned len = strlen(orig);

	char* fixed = static_cast<char*>(malloc(len + 2));
	strcpy(fixed,orig);
	fixed[len] = dollar;
	fixed[len + 1] = '\0';
	mcreight_builder builder(fixed, dollar, dot);
	std::auto_ptr<dottree::tree> res = builder.build();
	res->print(std::cout);
	return res;
}

