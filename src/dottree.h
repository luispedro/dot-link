namespace dotted_tree {
typedef unsigned int uint;
struct node {
	node* parent_;
	unsigned head_;
	unsigned depth_;
	node* sons_;
	node* next_;
	node* suffixlink_;
};

struct tree {
	const char* string_;
	unsigned lenght_;
	node* root_;
};

}
