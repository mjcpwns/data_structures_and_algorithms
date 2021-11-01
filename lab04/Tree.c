
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Record.h"
#include "Tree.h"

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
static Node doTreeInsert(Tree t, Node n, Record rec, bool *res);
static Node newNode(Record rec);
static Node doTreeDelete(Tree t, Node n, Record rec, bool *res);
static Node joinTrees(Node t1, Node t2);
static Record doTreeSearch(Tree t, Node n, Record rec);
static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l);
static Record findGreaterRecord(Tree t, Node n, Record r);
static void doTreeListInOrder(Node n);
static Node rotateLeft(Node n1);
static Node rotateRight(Node n1);
static Record getRecord(Node n);
static int max(int x, int y);
static int height(Node n);
static Record doTreeNext(Tree t, Node n, Record rec);

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

bool TreeInsert(Tree t, Record rec) {
    bool res = false; // if the record was inserted
    t->root = doTreeInsert(t, t->root, rec, &res);
    return res;
}

static Node doTreeInsert(Tree t, Node n, Record rec, bool *ret) {
    if (n == NULL) {
        *ret = true;
        return newNode(rec);
    } 
    int cmp = t->compare(rec, getRecord(n));
    if (cmp == 0) {
        *ret = false;
    } else {
        if (cmp < 0) {
            n->left = doTreeInsert(t, n->left, rec, ret);
        } else if (cmp > 0) {
            n->right = doTreeInsert(t, n->right, rec, ret);
        }
        *ret = true;
        // Increase height of parent
        n->height = max(height(n->left), height(n->right)) + 1;
        int balance = height(n->left) - height(n->right);
        if (balance > 1) {
            if (t->compare(rec, getRecord(n->left)) > 0) {
                n->left = rotateLeft(n->left);
            }
            return rotateRight(n);  
        } else if (balance < -1) {
            if (t->compare(rec, getRecord(n->right)) < 0) {
                n->right = rotateRight(n->right);
            }
            return rotateLeft(n); 
        }
    }
    return n;
}

static Node rotateLeft(Node n1) {
    if (n1 == NULL || n1->right == NULL) return n1;
    Node n2 = n1->right;
    n1->right = n2->left;
    n2->left = n1;
    return n2;
}

static Node rotateRight(Node n1) {
    if (n1 == NULL || n1->left == NULL) return n1;
    Node n2 = n1->left;
    n1->left = n2->right;
    n2->right = n1;
    return n2;
}
// static Node doTreeInsert(Tree t, Node n, Record rec, bool *res) {
//     if (n == NULL) {
//         *res = true;
//         return newNode(rec);
//     }

//     int cmp = t->compare(rec, n->rec);
//     if (cmp < 0) {
//         n->left = doTreeInsert(t, n->left, rec, res);
//     } else if (cmp > 0) {
//         n->right = doTreeInsert(t, n->right, rec, res);
//     } else {
//         *res = false;
//     }
//     return n;
// }

static Node newNode(Record rec) {
    Node n = malloc(sizeof(*n));
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    n->left = NULL;
    n->right = NULL;
    n->rec = rec;
    n->height = 0;
    return n;
}

////////////////////////////////////////////////////////////////////////

bool TreeDelete(Tree t, Record rec) {
    bool res = false;
    t->root = doTreeDelete(t, t->root, rec, &res);
    return res;
}

static Node doTreeDelete(Tree t, Node n, Record rec, bool *res) {
    if (n == NULL) {
        *res = false;
        return NULL;
    }

    int cmp = t->compare(rec, n->rec);
    if (cmp < 0) {
        n->left = doTreeDelete(t, n->left, rec, res);
    } else if (cmp > 0) {
        n->right = doTreeDelete(t, n->right, rec, res);
    } else {
        *res = true;
        Node l = n->left;
        Node r = n->right;
        free(n);
        n = joinTrees(l, r);
    }

    return n;
}

static Node joinTrees(Node t1, Node t2) {
    if (t1 == NULL) {
        return t2;
    } else if (t2 == NULL) {
        return t1;
    } else {
        Node curr = t2;
        Node prev = NULL;
        while (curr->left != NULL) {
            prev = curr;
            curr = curr->left;
        }

        if (prev != NULL) {
            prev->left = curr->right;
            curr->right = t2;
        }

        curr->left = t1;
        return curr;
    }
}

Record TreeNext(Tree t, Record r) {
    return t ? doTreeNext(t, t->root, r) : NULL;
}


static Record doTreeNext(Tree t, Node n, Record rec) {
    if (n == NULL) return NULL;
    int cmp = t->compare(n->rec, rec);
    Record ret;
    if (cmp > 0) {
        ret = doTreeNext(t, n->left, rec);
    } else if (cmp < 0) {
        return doTreeNext(t, n->right, rec);
    } else {
        ret = n->rec;
    }
    return (ret == NULL) ? n->rec : ret;
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

List TreeSearchBetween(Tree t, Record lower, Record upper) {
    // TODO: Complete this function
    List my_list = ListNew();
    doTreeSearchBetween(t, t->root, lower, upper, my_list);
    
    return my_list;
}

// n - the current node
// l - a list to accumulate results
static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l) {
    // Base Case
    if (t == NULL || n == NULL) {
        return;
    } 
    // Compare record to lower and upper bounds
    int cmp1 = t->compare(n->rec, lower);
    int cmp2 = t->compare(n->rec, upper);

    // if (cmp1 == 0 && cmp2 == 0) {
    //     return ListAppend(l, n->rec);
    // }

    // Checks if current node is within the bound
    if (cmp2 <= 0 && cmp1 >= 0) { // In-order list traversal
        doTreeSearchBetween(t, n->left, lower, upper, l);
        ListAppend(l, n->rec);
        doTreeSearchBetween(t, n->right, lower, upper, l);
        return;
    } else if (cmp2 > 0) { // Node is greater than upper bound
        return doTreeSearchBetween(t, n->left, lower, upper, l);
    } else { // cmp1 < 0 // Node is less than the lower bound
        return doTreeSearchBetween(t, n->right, lower, upper, l);
    } 
}

////////////////////////////////////////////////////////////////////////

void TreeListInOrder(Tree t) {
    doTreeListInOrder(t->root);
}

static void doTreeListInOrder(Node n) {
    if (n != NULL) {
        doTreeListInOrder(n->left);
        RecordShow(n->rec);
        printf("\n");
        doTreeListInOrder(n->right);
    }
}

static int height(Node n) {
    if (n == NULL) return 0;
    return n->height;
}

static Record getRecord(Node n) {
    if (n == NULL) return NULL;
    return n->rec;
}

static int max(int x, int y) {
    return (x > y) ? x : y;
}
