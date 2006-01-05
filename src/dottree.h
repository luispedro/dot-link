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
			children_(0),
			next_(0),
			suffixlink_(0)
		{}
		void add_child(const char*,node* c);
		void remove_child(node* c) {
			if (c == children_) children_ = c->next_;
			else {
				node* bef = children_;
				while (bef && bef->next_ != c) bef = bef->next_;
				if (bef) {
					bef->next_ = c->next_;
				}
			}
		}
		void print(const char*, const node*, std::ostream& out, std::string prefix, bool recurs = true) const;
		
		node* children() const { return children_;}
		node* children(node* c) { std::swap(c,children_); return c; }

		node* next() const { return next_;}
		node* next(node* c) { std::swap(c,next_); return c; }


		unsigned head() const { return head_; }
		unsigned head(unsigned h) { std::swap(h, head_); return h; }

		node* suffixlink() const { return suffixlink_;}
		node* suffixlink(node* s) { std::swap(s, suffixlink_); return s; }

		bool is_root() const { return suffixlink_ == this; }
		bool is_leaf() const { return !children_; }
		
		unsigned sdepth() const { return sdepth_; }
		unsigned sdepth(unsigned s) { std::swap(s, sdepth_); return s; }

		unsigned start(const node* par) const {
			assert(par);
			assert(par->is_root() || par->is_child(this));
			return head() + par->sdepth();
		}
		unsigned length(const node* par) const {
			assert(par);
			assert(par->is_root() || par->is_child(this));
			return sdepth_ - par->sdepth();
		}
	private:
		bool is_child(const node* other) const {
			node* cur = children_;
			while (cur && cur != other) cur = cur->next_;
			return cur == other;
		}
		friend class tree;
		unsigned head_;
		unsigned sdepth_;
		node* children_;
		node* next_;
		node* suffixlink_;
};

class node_visitor {
	public: 
		virtual ~node_visitor() { }
		virtual void visit_node(node*) = 0;
		virtual void finished() { }
};

class print_leafs : public node_visitor {
	public:
		void visit_node(node* c) { if (c->is_leaf()) std::cout << c->head()<< std::endl; }
		void finished() { delete this; }
};

struct position {
	public:
		position(node* p, node* n, unsigned o):
			parent_(p),
			node_(n),
			offset_(o)
		{}
		unsigned offset() const { return offset_; }
		node* parent() const { return parent_; }
		node* curnode() const { return node_; }
		bool at_end() const { return offset() == curnode()->length(parent_); }
		bool at_leaf() const { return !node_->children(); }
	private:
		node* parent_;
		node* node_;
		unsigned offset_;
};

struct tree {
	public:
		tree(const char* str, unsigned len, char dollar, char dot):
			string_(str),
			length_(len),
			dollar_(dollar)
		{
			leafs = new unsigned[length_];
			std::memset(leafs,0,sizeof(unsigned) * length_);
		}
		~tree() {
			delete [] leafs;
		}
		const char* string() const { return string_; }
		node* root() { return root_; }
		unsigned length() const { return length_; }
		char at(unsigned idx) const { assert(idx < length()); return string()[idx]; }

		void print(std::ostream& out) const {
			root_->print(string_, root_, out, "");
		}

		node* child(node* n, char ch) {
			node* cur = n->children_;
			while (cur && string_[cur->start(n)] != ch) cur = cur->next_;
			return cur;
		}

		void dfs(node_visitor* visitor) const { dfs(root_, visitor); visitor->finished(); }

		/**
		 * Advances pos by ch
		 *
		 * Returns true if possible (there is a letter match)
		 * If there is no match, returns false and pos is unchanged
		 */
		bool descend(dottree::position& pos, char ch) const;

		int match(const char* ch) const;
		const char* string_;
		unsigned length_;
		node* root_;
		char dollar_;
		unsigned* leafs;
	private:
		void dfs(node*, node_visitor* ) const;
};

std::auto_ptr<tree> build_tree(const char* str, char dollar = '$', char dot = '.');
}
