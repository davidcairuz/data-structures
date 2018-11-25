/*
	David Cairuz | (ICMC/USP) - 2018
	25/11/2018

	A simple and powerful code of an AVL tree.
	
	This code's purpose is teaching and that's the reason 
	why it has so many comments.

	Heavily based on:
	https://github.com/Vernalhav/T1-Alg/blob/master/avl.c
	https://gist.github.com/tonious/1377768
	https://www.youtube.com/watch?v=g4y2h70D6Nk
*/

#include <stdio.h>
#include <stdlib.h>

#define max(a, b) ((a > b) ? a : b)

/*
	If you want to change the type of item in the AVL
	the only place you need to change is here.
*/
typedef int ITEM;

struct node {
	struct node *left;
	struct node *right;
	int height;
	ITEM value;
};

struct avl {
	struct node *root;
};

typedef struct node NODE;
typedef struct avl AVL;

AVL* create_avl() {
	AVL *tree = malloc(sizeof(AVL));

	if(tree == NULL) return tree;

	tree->root = NULL;
	return tree;
}

/*
	Creates a node with determined value.
	Used in the insertion of an item in the tree. 
	When a NULL node is found, it becomes the node 
	created by this function
*/
NODE* initialize_node(ITEM value) {
	NODE *node = malloc(sizeof(NODE));

	if(node == NULL) return node;

	node->value = value;
	node->left = NULL;
	node->right = NULL;

	return node;
}

/*
	Gets the root of the tree.
*/
NODE* get_root(AVL* tree) {
	return tree->root;
}

/* 
	Returns a node's height or -1 if it's NULL.
	It's useful to say taht the height of a NULL node is -1 
	for porpuses of updating its height when necessary. 
*/
int node_height(NODE *node) {
	return (node == NULL) ? -1 : node->height;
}

/*
	Updates a node's height using the formula:
	height(node) = max(left child, right child) + 1
*/
void height_update(NODE *node) {
	int height = max(node_height(node->left), node_height(node->right));
	node->height = height + 1;
}

/*
	Calculates the balance factor of a node using the formula:
	balance(node) = height(left child) - height(right child)
*/
int balance_factor(NODE *node) {
	return node_height(node->left) - node_height(node->right);
}

/*
	Rotates a node to the left when necessary.
	This function is called when the balance factor
	of a node is out of the range [-1, 1] and is negative.
	Also updates the heights of 'a' and 'b'.
*/
NODE* left_rotate(NODE* a) {
	NODE* b = a->right;
	a->right = b->left;
	b->left = a;

	height_update(a);
	height_update(b);

	return b;
}

/*
	Rotates a node to the right when necessary.
	This function is called when the balance factor
	of a node is out of the range [-1, 1] and is positive.
	Also updates the heights of 'a' and 'b'.
*/
NODE* right_rotate(NODE *a) {
	NODE *b = a->left;
	a->left = b-> right;
	b->right = a;

	height_update(a);
	height_update(b);

	return b;
}

/*
	Makes a double rotation when necessary.
	A left/right rotation is needed when the balance factor
	of a node has the opposite sign of its left child.
*/
NODE* left_right_rotate(NODE *a){
	a->left = left_rotate(a->left);
	return right_rotate(a);
}

/*
	Makes a double rotation when necessary.
	A right/left rotation is needed when the balance factor
	of a node has the opposite sign of its right child.
*/
NODE* right_left_rotate(NODE *a){
	a->right = right_rotate(a->right);
	return left_rotate(a);
}

/*
	Follows the already mentioned rules to perform the correct
	rotations on node 'a'.
*/
NODE* rebalance(NODE *a) {
	int balance = balance_factor(a);

	if(balance >= 0) {
		return balance_factor(a->left) < 0 ? a = left_right_rotate(a) : right_rotate(a);

	} else if(balance < 0) {
		return balance_factor(a->right) > 0 ? a = right_left_rotate(a) : left_rotate(a);
	}
}

/*
	Finds the correct place for the item to be in the AVL and inserts it there.
	While leaving the recursion, it updates the height of each node and
	rebalances the tree when needed.

	The '(balance * balance) > 2' part is used to check if the balance factor is
	in the range [-1, 1].
*/
NODE* insert_avl_node(NODE* node, ITEM value) {
	if(node == NULL) node = initialize_node(value);
	else if(value < node->value) node->left = insert_avl_node(node->left, value);
	else if(value > node->value) node->right = insert_avl_node(node->right, value);
	else if(value == node->value) {
		printf("Invalid value.\n");
		return node;
	}

	height_update(node);
	int balance = balance_factor(node);
	if(balance * balance > 2) return rebalance(node);
	return node;
}

/*
	Used to start insertion.
*/
void insert_avl(AVL* tree, ITEM value) {
	tree->root = insert_avl_node(tree->root, value);
}

/*
	Used to find the smallest value of a subtree.
*/
ITEM smallest_in_subtree(NODE* node) {
	while(node->left != NULL) {
		node = node->left;
	}

	return node->value;
}

/*
	Used to remove a node from the AVL.
	The idea behind removal is not simple enough
	to explain here.

	However, the idea used in this code is the one
	in this video: https://www.youtube.com/watch?v=g4y2h70D6Nk
	by William Fiset.
*/
NODE* remove_avl_node(NODE *node, ITEM key) {
	if(node == NULL) return node;

	if(key < node->value) {
		node->left = remove_avl_node(node->left,key);

	} else if (key > node->value) {
		node->right = remove_avl_node(node->right, key);

	} else if (key == node->value) {

		if(node->right == NULL) {
			NODE* aux = node->left;
			free(node);
			node = aux;

		} else if(node->left == NULL) {
			NODE*aux = node->right;
			free(node);
			node = aux;

		} else if(node->right != NULL && node->left != NULL) {
			ITEM new_value = smallest_in_subtree(node->right);
			node-> value = new_value;
			node->right = remove_avl_node(node->right, new_value);
		}
	}

	if (node == NULL) return node;
	
	height_update(node);
	int balance = balance_factor(node);
	if (balance * balance > 2) return rebalance(node);
	return node;
}

/*
	Used to start removal.
*/
void remove_avl(AVL* tree, ITEM value) {
	tree->root = remove_avl_node(tree->root, value);
}

/*
	Used to print the tree.
	Copied this function from Victor Vernalha's project T1-Alg on Github.
	The link to his project is in the header of this file since my code
	was heavily based on his.
*/
void avl_print_debug(NODE *node, int depth){
	if (node == NULL) {
		putchar('\n');
		return;
	}
	int i;
	for (i = 0; i < depth; i++) putchar('\t');
	printf("[%d]\n", node->value);
	avl_print_debug(node->left, depth + 1);
	avl_print_debug(node->right, depth + 1);
}

/*
	Performs an in-order traversal on the tree
	and prints the result.
*/
void avl_print_sorted(NODE* node) {
	if (node == NULL) return;
	avl_print_sorted(node->left);
	printf("%d ", node->value);
	avl_print_sorted(node->right);
}

/*
	Recursively frees the AVL tree.
*/
void free_avl(NODE* node) {
	if(node == NULL) return;
	free_avl(node->left);
	free_avl(node->right);
	free(node);
}

/*
	You can test the AVL using the main function below.
*/
int main() {	
	AVL* tree = create_avl();
	int op;
	int value;

	printf("[1] Insert item\n");
	printf("[2] Remove item\n");
	printf("[3] Print tree\n");
	printf("[4] Print sorted\n");
	printf("[0] Exit\n\n");

	while(scanf("%d", &op), op != 0) {

		if(op == 1) {
			printf("Insert item: ");
			scanf("%d", &value);
			insert_avl(tree, value);

		} else if(op == 2) {
			printf("Remove item: ");
			scanf("%d", &value);
			remove_avl(tree, value);

		} else if(op == 3) {
			printf("Printing tree...\n");
			avl_print_debug(get_root(tree), 0);

		} else if(op == 4) {
			printf("Printing sorted items...\n")
			avl_print_sorted(get_root(tree));
			printf('\n');
		}
	}

	free_avl(get_root(tree));
	return 0;
}
