#ifndef LPC_DOTTREE_H1137158521_INCLUDE_GUARD_
#define LPC_DOTTREE_H1137158521_INCLUDE_GUARD_

#include <memory>
#include <iostream>
#include <string>
#include <cassert>
#include <ext/mt_allocator.h>

namespace dottree {
typedef unsigned int uint;
struct node;
struct tree;

const unsigned dot_node_marker = unsigned(-2345);

struct nodep_or_idx {
	// Use low order bit:
	// 0: ptr
	// 1: int
	//
	public:
		nodep_or_idx() { reset(); }
		nodep_or_idx(node* n) { set(n); }
		nodep_or_idx(const nodep_or_idx& other):
			d(other.d)
		{ }
		explicit nodep_or_idx(unsigned i) { set(i); }
		node* as_ptr() const {
			assert(is_ptr());
			if ( d == 0 ) return 0;
			return reinterpret_cast<node*>(d);
		}
		node* as_nodep() const { return as_ptr(); }
		unsigned as_index() const {
			assert(is_int());
			return d >> 1;
		}
		void reset() { d = 0; assert(is_null()); }
		void set(unsigned v) { d = (v << 1) | 1; assert(is_int()); }
		void set(node* p) { d = reinterpret_cast<unsigned>(p); assert(as_ptr() == p); }

		bool is_ptr() const { return !is_int(); }
		bool is_int() const { return d & 1; }

		bool is_null() const { return !valid(); }
		bool valid() const { return d; }
		bool operator !() const { return is_null(); }
		bool operator == (const nodep_or_idx& other) const {
			return d == other.d;
		}
		bool operator != (const nodep_or_idx& other) const {
			return d != other.d;
		}

		nodep_or_idx operator = (const nodep_or_idx& other) {
			d = other.d;
			return *this;
		}

	private:
		unsigned d;
};
std::ostream& operator << (std::ostream& , const nodep_or_idx&);
struct node {
	public:
		node(unsigned h, unsigned sd):
			head_(h),
			sdepth_(sd)
		{}
		//virtual ~node() { }
		void print(const char*, const node*, std::ostream& out, std::string prefix, bool recurs = true) const;
		
		nodep_or_idx next() const { return next_;}
		nodep_or_idx next(nodep_or_idx c) { std::swap(c,next_); return c; }

		unsigned head() const { return head_; }
		unsigned head(unsigned h) { std::swap(h, head_); return h; }

		nodep_or_idx children(nodep_or_idx c) { std::swap(c,children_); return c; }
		bool is_leaf() const { return children_.is_null(); }
		
		unsigned sdepth() const { return sdepth_; }
		unsigned sdepth(unsigned s) { std::swap(s, sdepth_); return s; }

		unsigned start(const node* par) const {
			assert(par);
			return head() + par->sdepth();
		}
		unsigned length(const node* par) const {
			assert(par);
			if (head() == dot_node_marker) return 0;
			return sdepth_ - par->sdepth();
		}
		void* operator new (size_t s) {
			assert(s == sizeof(node));
			++cur_alloc_;
			if (cur_alloc_ > max_alloc_) {
			       	max_alloc_ = cur_alloc_;
				//if (!(max_alloc_ % 1000)) std::cout << "max_alloc_: " << max_alloc_ << '\n';
			}
			return ::operator new(s);
		}
		void operator delete(void* p) {
			--cur_alloc_;
			::operator delete(p);
		}
		static unsigned allocated_nodes();
	private:
		static unsigned cur_alloc_;
		static unsigned max_alloc_;
		static __gnu_cxx::__mt_alloc<node> alloc_;

		friend class tree;
		unsigned head_;
		unsigned sdepth_;
		nodep_or_idx next_;
		nodep_or_idx children_;
};

struct node_wsl : public node {
	public:
		node_wsl(unsigned h, unsigned sd):
			node(h,sd),
			suffixlink_(0)
		{ }
		node* suffixlink() const { return suffixlink_;}
		node* suffixlink(node* s) { std::swap(s, suffixlink_); return s; }
		void* operator new (size_t s) {
			assert(s == sizeof(node_wsl));
			++cur_alloc_;
			if (cur_alloc_ > max_alloc_) max_alloc_ = cur_alloc_;
			return alloc_.allocate(s);
		}
		void operator delete(void* p) {
			
		}
		static unsigned allocated_nodes() { return max_alloc_; }
	private:
		static unsigned cur_alloc_;
		static unsigned max_alloc_;
		static __gnu_cxx::__mt_alloc<node_wsl> alloc_;
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
		void one_down() { ++offset_; }
		position& operator ++() { one_down(); return *this; }
		position operator + (unsigned int idx) {
			position res = *this;
			res.offset_ += idx;
			return res;
		}
		node* parent() const { return parent_; }
		nodep_or_idx curnode() const { return node_; }

		bool at_end() const {
			return offset() == unsigned(-1) ||
				curnode().is_ptr() && offset() == curnode().as_ptr()->length(parent_);
		}
		bool at_leaf() const { return curnode().is_int(); }
		bool is_leaf() const { return curnode().is_int(); }
		bool is_dotnode() const { return !is_leaf() && curnode().as_ptr()->head() == dot_node_marker; }
	private:
		tree* tree_;
		node* parent_;
		nodep_or_idx node_;
		unsigned offset_;
};

std::ostream& operator << (std::ostream& , const position&);

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
			tree_(t),
			out_(std::cout)
		{}
		void visit_node(position p);
		void visit_leaf(unsigned h) {
		
		}
		void finished() { delete this; }
		void up() { prefix_ = prefix_.substr(0,prefix_.size() - 5); }
		void down() { prefix_+= "    |"; }
	private:
		const tree* tree_;
		std::string prefix_;
		std::ostream& out_;
};


struct tree {
	public:
		tree(const char* str, unsigned len, char dollar):
			string_(str),
			length_(len),
			dollar_(dollar)
		{
			leafs_ = new nodep_or_idx[length_];
			std::memset(leafs_,0,sizeof(leafs_[0]) * length_);
		}
		~tree() {
			delete_subtree(root_);
			free(const_cast<char*>(string_));
			delete [] leafs_;
		}
		const char* string() const { return string_; }
		node* root() { return root_; }
		position rootp() { return position(root_, root_, 0); }
		unsigned length() const { return length_; }
		char at(unsigned idx) const { assert(idx < length()); return string_[idx]; }
		char at(position pos) const { return at(start(pos.curnode(),pos.parent()) + pos.offset()); }

		nodep_or_idx leaf(unsigned h) { return dottree::nodep_or_idx(h); }
		nodep_or_idx new_node();
		void print(std::ostream& out = std::cout) const {
			const tree* tthis = this;
			const_cast<tree*>(tthis)->dfs(new print_all(this));
		}

		void add_child(node* c, nodep_or_idx n);
		void remove_child(node* n, nodep_or_idx child);
		nodep_or_idx child(node* n, char ch);
		nodep_or_idx child(nodep_or_idx n, char ch) {
			if (n.is_ptr()) return child(n.as_ptr(), ch);
			return nodep_or_idx();
		}
		node* dot_link(nodep_or_idx n);

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
			if (head(n) == dot_node_marker) return 0;
			if (n.is_ptr()) return n.as_ptr()->length(par);
			return sdepth(n) - par->sdepth();
		}

		bool is_leaf(nodep_or_idx n) const {
			return head(n) + sdepth(n) == length();
		}

		nodep_or_idx children(nodep_or_idx n) const {
			if (is_leaf(n)) return nodep_or_idx();
			return n.as_ptr()->children_;
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
		void dfs(position p, node_visitor* visitor) const {
			visitor->start();
			if (p.at_end()) {
				dfs(p.curnode().as_ptr(),children(p.curnode()),visitor);
			} else {
				dfs(p.parent(),p.curnode(),visitor);
			}
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

		void delete_subtree(nodep_or_idx n) {
			//std::cout << "delete_subtree( " << n << " )\n";
			if (n.is_null()) return;
			delete_subtree(children(n));
			delete_subtree(next(n));
			if (n.is_ptr()) delete n.as_ptr();
		}

	private:
		void next(nodep_or_idx cur, nodep_or_idx next) const {
			assert(!cur.is_null());
			if (cur.is_ptr()) cur.as_ptr()->next(next);
			else leafs_[cur.as_index()] = next;
		}
		nodep_or_idx* leafs_;
		node* nodes_;
		void dfs(node*, nodep_or_idx, node_visitor* ) const;
		void print_leafvector() const;
};

std::auto_ptr<tree> build_tree(const char* str, char dollar = '$');
}

template <typename T, typename U>
inline T checked_cast(U p) {

	return static_cast<T>(p);
	//return dynamic_cast<T>(p);
}



#endif /* LPC_DOTTREE_H1137158521_INCLUDE_GUARD_ */
