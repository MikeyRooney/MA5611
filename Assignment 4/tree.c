#include <stdlib.h>
#include <stdio.h>

#include "tree.h"

struct binary_tree *init_tree(){
	struct binary_tree *b = calloc(1, sizeof(struct binary_tree));
	return b;
}

static void free_nodes_recursive(struct node *cur){
	if(cur->left != NULL && cur->right != NULL){
		free(cur);
		return;
	}
	if(cur->left != NULL){
		free_nodes_recursive(cur->left);
	}
	if(cur->right != NULL){
		free_nodes_recursive(cur->right);
	}
}

void free_tree(struct binary_tree *tree){
	if(tree->root != NULL){
		free_nodes_recursive(tree->root);
	}
	free(tree);
}

struct node *find_node(struct binary_tree *tree, int key){
	struct node *cur = tree->root;
	while(cur != NULL){
		if(cur->key == key){
			return cur;
		} else if(key > cur->key){
			cur = cur->right;
		} else if(key < cur->key){
			cur = cur ->left;
		}
	}
	return NULL; // not found
}

static struct node *find_minimum_in_subtree(struct node *cur){
	while(1){
		if(cur->left == NULL){
			return cur;
		}
		cur = cur->left;
	}
}

static void delete_root_node(struct binary_tree *tree, struct node *cur){
	if(cur->left == NULL && cur->right == NULL){ // root is only node
		tree->root = NULL;
	} else if(cur->left != NULL && cur->right == NULL){ // root only has left child node
		tree->root = cur->left;
		cur->left->parent = NULL;
	} else if(cur->left == NULL && cur->right != NULL){ // root only has right child node
		tree->root = cur->right;
		cur->right->parent = NULL;
	} else { // root has left and right child nodes
		// basically swapping the cur node with the minimum node in the subtree
		// because of this we don't actually delete the cur node, but we do
		// delete the minimum node.
		struct node *min = find_minimum_in_subtree(tree->root->right);
		int key = min->key;
		delete_key(tree, key);
		tree->root->key = key;
		return; // return so the node isn't freed
	}
	tree->total_size--;
	free(cur);
}

static void delete_non_root_node(struct binary_tree *tree, struct node *cur){
	if(cur->left == NULL && cur->right == NULL){ // is leaf - no child nodes
		if(cur->parent->left == cur){
			cur->parent->left = NULL;
		} else {
			cur->parent->right = NULL;
		}
	} else if(cur->left != NULL && cur->right == NULL){ // only has a child left node
		if(cur->parent->left == cur){
			cur->parent->left = cur->left;
		} else {
			cur->parent->right = cur->left;
		}
	} else if(cur->left == NULL && cur->right != NULL){ // only has a child right node
		if(cur->parent->left == cur){
			cur->parent->left = cur->right;
		} else {
			cur->parent->right = cur->right;
		}
	} else { // has both left and right child nodes
		// basically swapping the cur node with the minimum node in the subtree
		// because of this we don't actually delete the cur node, but we do
		// delete the minimum node.
		struct node *min = find_minimum_in_subtree(cur->right);
		int key = min->key;
		delete_key(tree, key);
		cur->key = key;
		return; // return so the node isn't freed
	}
	tree->total_size--;
	free(cur);
}

void delete_key(struct binary_tree *tree, int key){
	struct node *cur = find_node(tree, key);
	if(cur == NULL){
		return;
	}
	if(cur == tree->root){
		delete_root_node(tree, cur);
	} else {
		delete_non_root_node(tree, cur);
	}
}

void insert_key(struct binary_tree *tree, int key){
	tree->total_size++;
	if(tree->root == NULL){
		tree->root = calloc(1, sizeof(struct node));
		tree->root->key = key;
		return;
	}
	struct node *cur = tree->root;
	while(1){
		if(key > cur->key){
			if(cur->right == NULL){
				struct node *r = calloc(1, sizeof(struct node));
				r->key = key;
				r->parent = cur;
				cur->right = r;
				return;
			} else {
				cur = cur->right;
			}
		} else {
			if(cur->left == NULL){
				struct node *l = calloc(1, sizeof(struct node));
				l->key = key;
				l->parent = cur;
				cur->left = l;
				return;
			} else {
				cur = cur->left;
			}
		}
	}
}

// Used for deletion in the parallel version.
// Simply returns a random key from the tree to be deleted.
int get_random_key_from_tree(struct binary_tree *tree){
	struct node *cur = tree->root;
	while(1){
		int r = rand() % 3;
		if(r == 0){
			return cur->key;
		}
		if(cur->left != NULL && cur->right != NULL){
			r = rand() % 2;
			if(r == 0){
				cur = cur->left;
			} else {
				cur = cur->right;
			}
		} else if(cur->left != NULL){
			cur = cur->left;
		} else if(cur->right != NULL){
			cur = cur->right;
		} else {
			return cur->key;
		}
	}
}

// Prints tree left to right
// Pass root node to print entire tree
static void print_tree_recursive(struct node *cur){
	if(cur->left != NULL){
		print_tree_recursive(cur->left);
	}
	printf("%d, ", cur->key);
	if(cur->right != NULL){
		print_tree_recursive(cur->right);
	}
}

void print_tree(struct binary_tree *tree){
	if(tree->root == NULL){
		printf("Tree is empty\n");
		return;
	}
	printf("Tree contains in order:\n");
	print_tree_recursive(tree->root);
	printf("\n");
}

int find_height_recursive(struct node *cur){
	if(cur == NULL){
		return 0;
	}
	int left_depth = find_height_recursive(cur->left);
	int right_depth = find_height_recursive(cur->right);
	if(left_depth > right_depth){
		return 1 + left_depth;
	} else {
		return 1 + right_depth;
	}
}

int is_tree_balanced(struct binary_tree *tree){
	if(tree->root == NULL){
		return 1;
	}
	int left_depth = find_height_recursive(tree->root->left);
	int right_depth = find_height_recursive(tree->root->right);
	printf("Left depth: %d\n", left_depth);
	printf("Right depth: %d\n", right_depth);
	if(abs(right_depth - left_depth) > 1){
		return 0;
	}
	return 1;
	
}

// We traverse the tree and each node puts its key into the correct location in
// the passed array.
// When finished the array will contain all keys in the tree and will be sorted.
// Index should be 0 on the first call.
static int insert_node_key_recursive(int *keys, int index, struct node *cur){
	if(cur->left != NULL){
		index = insert_node_key_recursive(keys, index, cur->left);
	}
	keys[index] = cur->key;
	index = index + 1;
	if(cur->right != NULL){
		index = insert_node_key_recursive(keys, index, cur->right);
	}
	return index;
}

// Note that the loops start at i = 1 and access the keys at [halfway +/- i].
// This allows keys to be inserted in an optimal order.
static void insert_keys_balanced(struct binary_tree *balanced, int size, int *keys){
	int halfway = size / 2;
	if(size % 2 == 0){
		insert_key(balanced, keys[halfway]);
		int i;
		for(i = 1; i < halfway; i++){
			insert_key(balanced, keys[halfway + i]);
			insert_key(balanced, keys[halfway - i]);
		}
		insert_key(balanced, keys[0]);
	} else {
		insert_key(balanced, keys[halfway]);
		int i;
		for(i = 1; i <= halfway; i++){
			insert_key(balanced, keys[halfway + i]);
			insert_key(balanced, keys[halfway - i]);
		}
	}
}

// First gets all keys into a sorted array.
// Then it inserts these keys into a new tree, starting from the middle and 
// going out towards the start and end of the array.
// This insertion order means the new tree will be balanced.
struct binary_tree *balance_tree(struct binary_tree *tree){
	int *keys = calloc(tree->total_size, sizeof(int));
	insert_node_key_recursive(keys, 0, tree->root);
	struct binary_tree *balanced = init_tree();
	insert_keys_balanced(balanced, tree->total_size, keys);
	free(keys);
	free_tree(tree);
	return balanced;
}

