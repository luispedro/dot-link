#include <memory>
#include <cassert>

namespace dottree {
typedef unsigned int uint;
struct node {
	public:
		node(unsigned h, unsigned sd):
			head_(h),
			sdepth_(sd),
			children_(0), // These members have good initial values. 
			next_(0)      // The others leave unitinialized to have valgrind catch errors
		{}
		void add_child(node* c) {
			c->parent_ = this;
			c->next_ = children_;
			children_ = c;
		}
		
		node* children() const { return children_;}
		node* children(node* c) { std::swap(c,children_); return c; }

		node* parent() const { return parent_; }
		node* parent(node* p) { std::swap(p,parent_); return p; }

		unsigned head() const { return head_; }
		unsigned head(unsigned h) { std::swap(h, head_); return h; }

		node* suffixlink() { return suffixlink_;}
		node* suffixlink(node* s) { std::swap(s, suffixlink_); return s; }

		unsigned sdepth() const { return sdepth_; }
		unsigned sdepth(unsigned s) { std::swap(s, sdepth_); return s; }

		unsigned start() const { return head() + sdepth(); }
		unsigned lenght() const { assert(parent_); return sdepth_ - parent_->sdepth(); }
	private:
		friend class tree;
		node* parent_;
		unsigned head_;
		unsigned sdepth_;
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
		bool at_end() const { return offset() == curnode()->lenght(); }
	private:
		node* node_;
		unsigned offset_;
};

struct tree {
	public:
		tree(const char* str, char dollar, char dot):
			string_(strdup(str)),
			lenght_(strlen(str)),
			dollar_(dollar),
			dot_(dot)
		{}
		const char* string() const { return string_; }
		node* root() { return root_; }
		unsigned length() const { return lenght_; }
		char at(unsigned idx) const { assert(idx < length()); return string()[idx]; }

		node* child(node* n, char ch) {
			node* cur = n->children_;
			while (cur && string_[cur->start()] != ch) cur = cur->next_;
			return cur;
		}
		const char* string_;
		unsigned lenght_;
		node* root_;
		char dollar_;
		char dot_;
};

std::auto_ptr<tree> build_tree(const char* str, char dollar = '$', char dot = '.');
}
