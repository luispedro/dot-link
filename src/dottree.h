#include <memory>
namespace dotted_tree {
typedef unsigned int uint;
struct node {
	public:
		node* parent() const { return parent_; }
		node* child( char c ) const;
		void add_child( node* c ) { c->next_ = children_; children_ = c; }
	private:
		node* parent_;
		unsigned head_;
		unsigned lenght_;
		node* children_;
		node* next_;
		node* suffixlink_;
};

struct point {
	public:
		point( node* n, unsigned o ):
			node_( n ),
			offset_( o )
		{}
		unsigned offset() const { return offset_; }
		node* node() const { return node_; }
	private:
		node* node_;
		unsigned offset_;
};

struct tree {
	public:
	private:
		const char* string_;
		unsigned lenght_;
		node* root_;
		char dollar_;
		char dot_;
};

std::auto_ptr<tree> build_tree( const char* str, char dollar = '$', char dot = '.' );
}
