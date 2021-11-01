// isBST.c ... implementation of isBST function

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Tree.h"

/*
    You will submit only this one file.

    Implement the function "isBST" below. Read the exam paper for a
    detailed specification and description of your task.

    - DO NOT modify the code in any other files except for debugging
      purposes.
    - If you wish, you can add static variables and/or helper functions
      to this file.
    - DO NOT add a "main" function to this file.
*/
static Record getMin(Node t) {
    if (t == NULL) return NULL;
    if (t->left == NULL) return t->rec;
    return getMin(t->left);
}

static Record getMax(Node t) {
    if (t == NULL) return NULL;
    if (t->right == NULL) return t->rec;
    return getMax(t->right);
}

// static int isValid(Node t, int (*cmp)(Record, Record)) {
//     if (t == NULL) return 1;

//     if ((t->left != NULL && cmp(t->left->rec, t->rec) > 0) || 
//         (t->right != NULL && cmp(t->right->rec, t->rec) < 0))
//       return 0;

//     return (isValid(t->left, cmp) && isValid(t->right, cmp)) ? 1 : 0;
// }

// int isBST(Tree t) {
//     if (t == NULL) return 1;

//     Record max_left = getMax(t->root->left);
//     Record min_right = getMin(t->root->right);

//     if ((max_left != NULL && t->compare(max_left, t->root->rec) > 0) || 
//         (min_right != NULL && t->compare(min_right, t->root->rec) < 0))
//       return 0;
    
//     return isValid(t->root, t->compare);
// }

static int isValid(Node t, Record min, Record max, int (*cmp)(Record, Record)) {
  if (t == NULL) return 1;
  if ((min != NULL && cmp(t->rec, min) < 0) || (max != NULL && cmp(t->rec, max) > 0)) 
    return 0;

  if (isValid(t->left, min, t->rec, cmp) && isValid(t->right, t->rec, max, cmp))
    return 1;
  return 0;
}

int isBST(Tree t) {
    if (t == NULL) return 1;

    Record min = getMin(t->root->left);
    Record max = getMax(t->root->right);
       
    return isValid(t->root, min, max, t->compare);
}
