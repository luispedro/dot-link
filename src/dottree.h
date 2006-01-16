#include <memory>
#include <iostream>
#include <string>
#include <cassert>

namespace dottree {
typedef unsigned int uint;
struct node;
struct tree;

struct nodep_or_idx {
	public:
		nodep_or_idx() { reset(); }
		nodep_or_idx(node* n) { set(n); }
		explicit nodep_or_idx(unsigned i) { set(i); }
		node* as_ptr() const {
			assert(is_ptr());
			if ( d == 0 ) return 0;
			return reinterpret_cast<node*>(d);
		}
		node* as_nodep() const { return as_ptr(); }
		unsigned as_index() const {
			assert(is_int());
			return ~d - 1;
		}
		void reset() { d = 0; assert(is_null()); }
		void set(unsigned v) { d = ~(v+1); assert(is_int()); }
		void set(node* p) { d = reinterpret_cast<unsigned>(p); assert(is_ptr()); }

		bool is_ptr() const { return int(d) >= 0; }
		bool is_int() const { return int(d) < 0; }

		bool is_null() const { return d == 0; }
		bool operator !() const { return is_null(); }
		bool operator == (const nodep_or_idx& other) const {
			return d == other.d;
		}
		bool operator != (const nodep_or_idx& other) const {
			return d != other.d;
		}

	private:
		unsigned d;
};
struct node {
	public:
		node(unsigned h, unsigned sd):
			head_(h),
			sdepth_(sd),
			suffixlink_(0)
		{}
		void print(const char*, const node*, std::ostream& out, std::string prefix, bool recurs = true) const;
		
		nodep_or_idx children() const { return children_;}
		nodep_or_idx children(nodep_or_idx c) { std::swap(c,children_); return c; }

		nodep_or_idx next() const { return next_;}
		nodep_or_idx next(nodep_or_idx c) { std::swap(c,next_); return c; }

		unsigned head() const { return head_; }
		unsigned head(unsigned h) { std::swap(h, head_); return h; }

		node* suffixlink() const { return suffixlink_;}
		node* suffixlink(node* s) { std::swap(s, suffixlink_); return s; }

		bool is_root() const { return suffixlink_ == this; }
		bool is_leaf() const { return children_.is_null(); }
		
		unsigned sdepth() const { return sdepth_; }
		unsigned sdepth(unsigned s) { std::swap(s, sdepth_); return s; }

		unsigned start(const node* par) const {
			assert(par);
			return head() + par->sdepth();
		}
		unsigned length(const node* par) const {
			assert(par);
			return sdepth_ - par->sdepth();
		}
	private:
		friend class tree;
		unsigned head_;
		unsigned sdepth_;
		nodep_or_idx children_;
		nodep_or_idx next_;
		node* suffixlink_;
};

struct position {
	public:
		position(node* p, nodep_or_idx n, unsigned o):
			parent_(p),
			node_(n),
			offset_(o)
		{}
		unsigned offset() const { return offset_; }
		node* parent() const { return parent_; }
		nodep_or_idx curnode() const { return node_; }
		bool at_end() const { return curnode().is_ptr() && offset() == curnode().as_ptr()->length(parent_); }
		bool at_leaf() const { return curnode().is_int(); }
	private:
		node* parent_;
		nodep_or_idx node_;
		unsigned offset_;
};

class node_visitor {
	public: 
		virtual ~node_visitor() { }
		virtual void visit_leaf(unsigned) = 0;
		virtual void visit_node(position) = 0;
		virtual void start() { }
		virtual void down() { }
		virtual void up() { }
		virtual void finished() { }
};

class print_leafs : public node_visitor {
	public:
		void visit_node(position) { }
		void visit_leaf(unsigned h) { std::cout << h << std::endl; }
		void finished() { delete this; }
};
class print_all: public node_visitor {
	public:
		explicit print_all(const tree* t):
			tree_(t)
		{}
		void visit_node(position p);
		void visit_leaf(unsigned h) {
		
		}
		void finished() { delete this; }
		void up() { prefix_ = prefix_.substr(0,prefix_.size() - 4); }
		void down() { prefix_+= "    "; }
	private:
		const tree* tree_;
		std::string prefix_;
};


struct tree {
	public:
		tree(const char* str, unsigned len, char dollar, char dot):
			string_(str),
			length_(len),
			dollar_(dollar)
		{
			leafs_ = new nodep_or_idx[length_];
			std::memset(leafs_,0,sizeof(leafs_[0]) * length_);
		}
		~tree() {
			delete [] leafs_;
		}
		const char* string() const { return string_; }
		node* root() { return root_; }
		unsigned length() const { return length_; }
		char at(unsigned idx) const { assert(idx < length()); return string()[idx]; }

		nodep_or_idx leaf(unsigned h) { return dottree::nodep_or_idx(h); }
		void print(std::ostream& out) const {
			const tree* tthis = this;
			const_cast<tree*>(tthis)->dfs(new print_all(this));
		}

		void add_child(node* c, nodep_or_idx n);
		void remove_child(node* n, nodep_or_idx child);
		nodep_or_idx child(node* n, char ch);

		unsigned sdepth(position p) const { return sdepth(p.curnode()); }
		unsigned sdepth(nodep_or_idx n) const {
			if (n.is_ptr())  return n.as_nodep()->sdepth();
			return length_ - n.as_index();
		}

		unsigned head(position p) const { return head(p.curnode()); }
		unsigned head(nodep_or_idx n) const {
		       if (n.is_ptr()) return n.as_nodep()->head();
		       return n.as_index();
		}

		unsigned start(position p) const { return start(p.curnode(),p.parent()); }
		unsigned start(nodep_or_idx n, node* par) const {
			if (n.is_ptr()) return n.as_ptr()->start(par);
			return n.as_index() + par->sdepth();
		}

		unsigned length(position p) const { return length(p.curnode(),p.parent()); }
		unsigned length(nodep_or_idx n, node* par) const {
			if (n.is_ptr()) return n.as_ptr()->length(par);
			return sdepth(n) - par->sdepth();
		}

		nodep_or_idx next(position p) const { return next(p.curnode()); }
		nodep_or_idx next(nodep_or_idx n) const {
			if (n.is_ptr()) return n.as_nodep()->next();
			return leafs_[n.as_index()];
		}

		nodep_or_idx root() const { return nodep_or_idx(root_); }

		void dfs(node_visitor* visitor) const {
			visitor->start();
			dfs(root_, root(), visitor);
			visitor->finished();
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
		unsigned length_;
		node* root_;
		char dollar_;
	private:
		void next(nodep_or_idx cur, nodep_or_idx next) const {
			assert(!cur.is_null());
			if (cur.is_ptr()) cur.as_ptr()->next(next);
			else leafs_[cur.as_index()] = next;
		}
		nodep_or_idx* leafs_;
		void dfs(node*, nodep_or_idx, node_visitor* ) const;
		void print_leafvector() const;
};

std::auto_ptr<tree> build_tree(const char* str, char dollar = '$', char dot = '.');
}

