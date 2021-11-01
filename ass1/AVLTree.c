
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Record.h"
#include "AVLTree.h"

typedef struct node *Node;
struct node {
    Record rec;
    Node   left;
    Node   right;
    int    height;
};

struct tree {
    Node    root;
    int     (*compare)(Record, Record);
};

////////////////////////////////////////////////////////////////////////
// Auxiliary functions

static void doTreeFree(Node n, bool freeRecords);
static Node newNode(Record rec);
static Node doTreeInsert(Tree t, Node n, Record rec, bool *ret);
static Record doTreeSearch(Tree t, Node n, Record rec);
static Node rotateLeft(Node n1);
static Node rotateRight(Node n1);
static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l);
static Record doTreeNext(Tree t, Node n, Record rec);
static int height(Node n);
static Record getRecord(Node n);
static int max(int x, int y);

////////////////////////////////////////////////////////////////////////

static Node newNode(Record rec) {
    Node n = malloc(sizeof(*n));
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    n->rec = rec;
    n->left = NULL;
    n->right = NULL;
    n->height = 0;
    return n;
}

////////////////////////////////////////////////////////////////////////

Tree TreeNew(int (*compare)(Record, Record)) {
    Tree t = malloc(sizeof(*t));
    if (t == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    t->root = NULL;
    t->compare = compare;
    return t;
}

////////////////////////////////////////////////////////////////////////

void TreeFree(Tree t, bool freeRecords) {
    doTreeFree(t->root, freeRecords);
    free(t);
}

static void doTreeFree(Node n, bool freeRecords) {
    if (n != NULL) {
        doTreeFree(n->left, freeRecords);
        doTreeFree(n->right, freeRecords);
        if (freeRecords) {
            RecordFree(n->rec);
        }
        free(n);
    }
}

////////////////////////////////////////////////////////////////////////

Record TreeSearch(Tree t, Record rec) {
    return doTreeSearch(t, t->root, rec);
}

static Record doTreeSearch(Tree t, Node n, Record rec) {
    if (n == NULL) {
        return NULL;
    }

    int cmp = t->compare(rec, n->rec);
    if (cmp < 0) {
        return doTreeSearch(t, n->left, rec);
    } else if (cmp > 0) {
        return doTreeSearch(t, n->right, rec);
    } else {
        return n->rec;
    }
}


////////////////////////////////////////////////////////////////////////
/* IMPORTANT: 
   Do NOT modify the code above this line. 
   You must not modify the 'node' and 'tree' structures defined above.
   You must not modify the functions defined above.
*/
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

/**
 * Inserts the given record into the AVL tree.
 * Returns true if the record was inserted successfully,  or  false  if
 * there was already a record that compares equal to the given record in
 * the tree (according to the comparison function).
 * Source: Algorithm borrowed from course lecture notes
 */

bool TreeInsert(Tree t, Record rec) {

    bool ret = false;

    t->root = doTreeInsert(t, t->root, rec, &ret);

    return ret;
}

static Node doTreeInsert(Tree t, Node n, Record rec, bool *ret) {

    // Base Case
    if (n == NULL) {
        *ret = true;
        return newNode(rec);
    }

    // Compare current record to record passed in as argument
    int cmp = t->compare(rec, getRecord(n));

    if (cmp == 0) { // No duplicates
        *ret = false;
    } else {
        if (cmp < 0) {
            n->left = doTreeInsert(t, n->left, rec, ret);
        } else if (cmp > 0) {
            n->right = doTreeInsert(t, n->right, rec, ret);
        }
        // *ret = true;
        // Increase height of parent
        n->height = max(height(n->left), height(n->right)) + 1;
        int balance = height(n->left) - height(n->right);
        if (balance > 1) { // Case R and LR
            if (t->compare(rec, getRecord(n->left)) > 0) {
                n->left = rotateLeft(n->left);
            }
            return rotateRight(n);  
        } else if (balance < -1) { // Case RL and L
            if (t->compare(rec, getRecord(n->right)) < 0) {
                n->right = rotateRight(n->right);
            }
            return rotateLeft(n); 
        }
    }

    return n;
}

static Node rotateLeft(Node x) {

    if (x == NULL || x->right == NULL) return x;

    // Tree Left Rotation
    Node y = x->right;
    x->right = y->left;
    y->left = x;

	// Update Heights
	x->height = max(height(x->left), height(x->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

static Node rotateRight(Node y) {

    if (y == NULL || y->left == NULL) return y;

    // Tree Right Rotation
    Node x = y->left;
    y->left = x->right;
    x->right = y;

	// Update Heights
	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

    return x; 
}

////////////////////////////////////////////////////////////////////////

List TreeSearchBetween(Tree t, Record lower, Record upper) {

    List my_list = ListNew();

    doTreeSearchBetween(t, t->root, lower, upper, my_list);

    return my_list;
}

static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l) {

    // Base Case
    if (t == NULL || n == NULL) {
        return;
    } 

    // Compare current record to upper and lower bounds
    int cmp1 = t->compare(getRecord(n), lower);
    int cmp2 = t->compare(getRecord(n), upper);

    if (cmp1 == 0 && cmp2 == 0) {  // Equal to Lower and Upper Bound
        ListAppend(l, getRecord(n));
    } else if (cmp1 == 0) { // Equal to Lower Bound
        ListAppend(l, getRecord(n));
        doTreeSearchBetween(t, n->right, lower, upper, l);
    } else if (cmp2 == 0) {  // Equal to Upper Bound
        doTreeSearchBetween(t, n->left, lower, upper, l);
        ListAppend(l, getRecord(n));
    } else if (cmp2 < 0 && cmp1 > 0) { // In Range
        doTreeSearchBetween(t, n->left, lower, upper, l);
        ListAppend(l, getRecord(n));
        doTreeSearchBetween(t, n->right, lower, upper, l);
    } else if (cmp2 > 0) { // Greater than upper bound
        doTreeSearchBetween(t, n->left, lower, upper, l);
    } else { // Lower than lower bound
        doTreeSearchBetween(t, n->right, lower, upper, l);
    }

    return; 
}

////////////////////////////////////////////////////////////////////////

Record TreeNext(Tree t, Record r) {
    // If tree exists, recurse and do TreeNext
    return t ? doTreeNext(t, t->root, r) : NULL;
}

static Record doTreeNext(Tree t, Node n, Record rec) {

    // Base Case
    if (n == NULL) return NULL;

    // Compare current record to record passed in as argument
    int cmp = t->compare(n->rec, rec);
    Record ret;

    if (cmp > 0) { // Current record is greater -- go left
        ret = doTreeNext(t, n->left, rec);
    } else if (cmp < 0) { // Current record is smaller -- go right
        return doTreeNext(t, n->right, rec);
    } else { // Found record
        ret = n->rec;
    }

    // If ret == NULL (for case L -> R -> NULL), return current record (i.e. L)
    return (ret == NULL) ? n->rec : ret;
}

////////////////////////////////////////////////////////////////////////

static int height(Node n) {
    // For node == NULL, height == -1
    return n ? n->height : -1;
}

static Record getRecord(Node n) {
    // Get record in node (if it exists)
    return n ? n->rec : NULL;
}

static int max(int x, int y) {
    // Return the greater integer
    return (x > y) ? x : y;
}
