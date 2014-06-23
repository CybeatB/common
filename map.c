/* When I suggested implementing a map in C,
 * they told me "don't do that".
 * So, naturally, I had to try. Here is my
 * attempt. */

#include <string.h>
char* strdup(char*);

/* A key comparator, because sorting keys.
 * Returns 0 if strings are identical.
 * Returns >0 if first parameter is 'greater'.
 * Returns <0 if first parameter is 'less'.
 * Also returns 0 on error, so be careful.
 * If the strings are different lengths, and
 * the longer string is the shorter string
 * with some extra characters appended, the
 * longer string is considered 'greater'.
 * 'Alphabetical Order' is ascending: A < Z.
 * Case-Insensitive. */

int keycmp (char*, char*);

/* Lacking a good hashing function,
 * a hash map was out of the question.
 * Since I've never implemented a binary
 * search tree either, I might as well
 * give that a shot too. This project
 * was never meant to be useful, it's
 * a learning experience. */

struct Mapnode {
	char* key;
	int value;
	int depth;
	struct Mapnode *left;
	struct Mapnode *right;
}

/* The three critical functions:
 * Insert, Retrieve, Remove */

typedef struct Map {
	struct Mapnode *root;
	void (*add)(struct Mapnode*, char*);
	int (*search)(struct Mapnode*, char*);
	int (*remove)(void);
} Map;

/* Returns the value of the specified key.
 * Returns -1 on error, or 0 if the key
 * does not currently exist. */

int map_search(struct Mapnode* target, char* newkey) {
	// base case: no tree left
	if (target == NULL) {
		return 0;
	}

	// have we found our value?
	int compared = keycmp(newkey, target->key);
	if (compared == 0) {
		// yes, return
		return target->value;
	}
	if (compared > 0) {
		// no, it's greater than this one
		return map_search(target->right, newkey);
	}
	if (compared < 0) {
		// no, it's less than this one
		return map_search(target->left, newkey);
	}

	// if this ever happens, panic
	return -1;
}

/* Retrieves and updates the depth of
 * the specified node, helper function
 * for balancing */

int map_get_depth(struct Mapnode* target) {
	if (target == NULL) {
		return -1;
	}
	int ldepth = target->left ? target->left->depth : -1;
	int rdepth = target->right ? target->right->depth : -1;
	target->depth = ldepth >= rdepth ? ldepth+1 : rdepth+1;
	return target->depth;
}

/* The all-important tree balancer.
 * Probably an AVL tree, possibly a
 * Scapegoat tree, doesn't really matter
 * as long as it works. Testing TBC. */

void map_balance(struct Mapnode* target) {
	if (target == NULL) {
		return;
	}
	while (1) {
		// get depths of children
		int ldepth = target->left ? target->left->depth : -1;
		int rdepth = target->right ? target->right->depth : -1;
		int balchk = ldepth - rdepth;
		if (balchk > 1) {
			// balance left
			struct Mapnode oldroot;
			memcpy(&oldroot, target, sizeof(struct Mapnode));
			memcpy(target, target->left, sizeof(struct Mapnode));
			free(oldroot.left);
			if (target->right) {
				if (target->right->right) {
					oldroot.left = malloc(sizeof(struct Mapnode));
					memcpy(oldroot.left, target->right->right, sizeof(struct Mapnode));
					memcpy(target->right->right, oldroot, sizeof(struct Mapnode));
				} else {
					target->right->right = malloc(sizeof(struct Mapnode));
					memcpy(target->right->right, oldroot, sizeof(struct Mapnode));
				}
			} else {
				target->right = malloc(sizeof(struct Mapnode));
				memcpy(target->right, oldroot, sizeof(struct Mapnode));
			}
			map_get_depth(target->right->right);
			map_get_depth(target->right);
			map_get_depth(target);
		} else if (balchk < -1) {
			// balance right
			struct Mapnode oldroot;
			memcpy(&oldroot, target, sizeof(struct Mapnode));
			memcpy(target, target->right, sizeof(struct Mapnode));
			free(oldroot.right);
			if (target->left) {
				if (target->left->left) {
					oldroot.right = malloc(sizeof(struct Mapnode));
					memcpy(oldroot.right, target->left->left, sizeof(struct Mapnode));
					memcpy(target->left->left, oldroot, sizeof(struct Mapnode));
				} else {
					target->left->left = malloc(sizeof(struct Mapnode));
					memcpy(target->left->left, oldroot, sizeof(struct Mapnode));
				}
			} else {
				target->right = malloc(sizeof(struct Mapnode));
				memcpy(target->right, oldroot, sizeof(struct Mapnode));
			}
			map_get_depth(target->right->right);
			map_get_depth(target->right);
			map_get_depth(target);
		} else {
			// balanced
			break;
		}
	}
}

void map_add(struct Mapnode* target, char* newkey) {
	// have we found the node we are searching for?
	int compared = keycmp(newkey, target->key);

	if (compared == 0) {
		// yes, increment
		target->value++;
	}

	if (compared > 0) {
		// no, candidate is greater
		if (target->right) {
			// move down the tree
			map_add(target->right, newkey);
		} else {
			// we need a new node
			target->right = malloc(sizeof(struct Mapnode));
			target->right->key = strdup(newkey);
			target->right->value = 1;
			target->right->depth = 0;
			target->right->left = NULL;
			target->right->right = NULL;
		}
	}

	if (compared < 0) {
		// no, candidate is less
		if (target->less) {
			// move down the tree
			map_add(target->left, newkey);
		} else {
			// we need a new node
			target->left = malloc(sizeof(struct Mapnode));
			target->left->key = strdup(newkey);
			target->left->value = 1;
			target->left->depth = 0;
			target->left->left = NULL;
			target->left->right = NULL;
		}
	}

	map_get_depth(target);
	map_balance(target);
}

/* Remove a node from the map.
 * Deletes the node completely,
 * does not decrement counter. */

struct Mapnode* map_remove(struct Mapnode* target, char* key) {
	// compare the keys
	int compared = keycmp(key, target->key);
	// recursive cases: move down the tree
	if (compared > 0) {
		target->right = map_remove(target->right, key);
		goto end;
	}
	if (compared < 0) {
		target->left = map_remove(target->left, key);
		goto end;
	}

	// base case: remove the node
	if (target->right == NULL) {
		// far-right node
		struct Mapnode* temp = target->left;
		if (temp != NULL) {
			// replace with left child
			memcpy(target, temp, sizeof(struct Mapnode));
		} else {
			// leaf node, delete
			free(target);
			target = NULL;
		}
		free(temp);
		goto end;
	}

	if (target->left == NULL) {
		// far-left node
		struct Mapnode* temp = target->right;
		memcpy(target, temp, sizeof(struct Mapnode));
		free(temp);
		goto end;
	}

	// find rightmost node in subtree
	struct Mapnode* rparent = target;
	struct Mapnode* rmost = target->left;
	while (rmost->right != NULL) {
		rparent = rmost;
		rmost = rmost->right;
	}
	// isolate rightmost node
	rparent->right = rmost->left;
	// remove selected node
	rmost->left = target->left;
	memcpy(rmost, target, sizeof(struct Mapnode));
	free(rmost);

	end:
	map_balance(target);
	return target;
}

/* compares keys, more info at top
 * of file. */

int keycmp(char* str1, char* str2) {
	// yay, ASCII
	const char diff = 'a' - 'A';

	// traverse the strings
	while (*str1 && *str2) {
		// case-insensitive comparison
		char c1 = *str1;
		if (c1 >= 'a') {
			c1 -= diff;
		}
		char c2 = *str2;
		if (c2 >= 'a') {
			c1 -= diff;
		}

		// if different, return
		int result = c2 > c1 ? -1 : (c1 > c2 ? 1 : 0);
		result ? return result : 0;
	}

	return *str1 - *str2
}

/* Duplicates a string, returns a pointer
 * to the duplicate. */

char* strdup(char* source) {
	size_t length = strlen(source)+1;
	char* result = malloc(length * sizeof(char));
	strcpy(result, source);
	return result;
}

/* I'll put tests here. Eventually. */

int main() {
	return 0;
}
