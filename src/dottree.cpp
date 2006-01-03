#include "dottree.h"

namespace {

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
			tree_->root_->parent(tree_->root_);
			dottree::position pos(tree_->root_, 0);
			unsigned advance = 0;
			for (unsigned start = 0; start != tree_->length(); ++start) {
				if (!advance) 
					while (descend(pos, tree_->at(start+advance))) ++advance;
				// I'm at a breaking point
				add_branch(pos, start, advance);
				pos = fast_suffix_link(pos);
				--advance;
			}
			return tree_;
		}
	private:
		/**
		 * Advances pos by ch
		 *
		 * Returns true if possible (there is a letter match)
		 * If there is no match, returns false and pos is unchanged
		 */
		bool descend(dottree::position& pos, char ch) {
			if (pos.at_end()) {
				dottree::node* next = tree_->child(pos.curnode(), ch);
				if (!next) return false;
				pos = dottree::position(next, 0);
				return true;
			}
			if (tree_->at(pos.curnode()->start() + pos.offset()) == ch) {
				pos = dottree::position(pos.curnode(), pos.offset() + 1);
				return true;
			}
			return false;
		}
		dottree::position fast_suffix_link(dottree::position pos) {
			dottree::node* cur = pos.curnode()->parent();
			unsigned len = pos.offset();
			unsigned str_start = pos.curnode()->start();
			while (len > cur->lenght()) 
				cur = tree_->child(cur,str_[str_start + len]);
			return dottree::position(cur, len);
		}
			
		void add_branch(dottree::position pos, unsigned start, unsigned advance) {
			dottree::node* leaf = new dottree::node(start, start + advance);
			if (pos.at_end()) {
				pos.curnode()->add_child(leaf);
			} else {
				dottree::node* middle = new dottree::node(start, pos.curnode()->sdepth() + pos.offset());
				middle->children(pos.curnode()->children(0));
				pos.curnode()->add_child(middle);
				middle->add_child(leaf);
				if (suffixless_) {
					suffixless_->suffixlink(middle);
				}
				suffixless_ = middle;
			}
		}
			
		std::auto_ptr<dottree::tree> tree_;
		const char* str_;
		char dollar_;
		char dot_;
		dottree::node* suffixless_;
};
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
	return builder.build();
}

