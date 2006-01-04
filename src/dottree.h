#include <memory>
#include <iostream>
#include <string>
#include <cassert>

namespace dottree {
typedef unsigned int uint;
struct node {
	public:
		node(unsigned h, unsigned sd):
			head_(h),
			sdepth_(sd),
			parent_(0),
			children_(0),
			next_(0),
			suffixlink_(0)
		{}
		void add_child(node* c) {
			c->parent_ = this;
			c->next_ = children_;
			children_ = c;
		}
		void remove_child(node* c) {
			if (c == children_) children_ = c->next_;
			else {
				node* bef = children_;
				while (bef && bef->next_ != c) bef = bef->next_;
				if (bef) {
					bef->next_ = c->next_;
				}
			}
			c->parent_ = 0;
		}
		void print(const char*, std::ostream& out, std::string prefix, bool recurs = true) const;
		
		node* children() const { return children_;}
		node* children(node* c) { std::swap(c,children_); return c; }

		node* next() const { return next_;}
		node* next(node* c) { std::swap(c,next_); return c; }

		node* parent() const { return parent_; }
		node* parent(node* p) { std::swap(p,parent_); return p; }

		unsigned head() const { return head_; }
		unsigned head(unsigned h) { std::swap(h, head_); return h; }

		node* suffixlink() const { return suffixlink_;}
		node* suffixlink(node* s) { std::swap(s, suffixlink_); return s; }

		unsigned sdepth() const { return sdepth_; }
		unsigned sdepth(unsigned s) { std::swap(s, sdepth_); return s; }

		unsigned start() const { return head() + parent_->sdepth(); }
		unsigned length() const { assert(parent_); return sdepth_ - parent_->sdepth(); }
	private:
		friend class tree;
		unsigned head_;
		unsigned sdepth_;
		node* parent_;
		node* children_;
		node* next_;
		node* suffixlink_;
};

struct position {
	public:
		position(node* n, unsigned o):
			node_(n),
			offset_(o)
		{}
		unsigned offset() const { return offset_; }
		node* curnode() const { return node_; }
		bool at_end() const { return offset() == curnode()->length(); }
	private:
		node* node_;
		unsigned offset_;
};

struct tree {
	public:
		tree(const char* str, unsigned len, char dollar, char dot):
			string_(str),
			lenght_(len),
			dollar_(dollar),
			dot_(dot)
		{}
		const char* string() const { return string_; }
		node* root() { return root_; }
		unsigned length() const { return lenght_; }
		char at(unsigned idx) const { assert(idx < length()); return string()[idx]; }

		void print(std::ostream& out) const {
			root_->print(string_, out, "");
		}

		node* child(node* n, char ch) {
			node* cur = n->children_;
			while (cur && string_[cur->start()] != ch) cur = cur->next_;
			return cur;
		}

		/**
		 * Advances pos by ch
		 *
		 * Returns true if possible (there is a letter match)
		 * If there is no match, returns false and pos is unchanged
		 */
		bool descend(dottree::position& pos, char ch) const;

		int match(const char* ch) const;
		const char* string_;
		unsigned lenght_;
		node* root_;
		char dollar_;
		char dot_;
};

std::auto_ptr<tree> build_tree(const char* str, char dollar = '$', char dot = '.');
}
