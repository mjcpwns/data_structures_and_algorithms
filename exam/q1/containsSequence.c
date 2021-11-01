// containsSequence.c ... implementation of containsSequence function

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

/*
    You will submit only this one file.

    Implement the function "containsSequence" below. Read the exam paper
    for a detailed specification and description of your task.

    - DO NOT modify the code in any other files except for debugging
      purposes.
    - If you wish, you can add static variables and/or helper functions
      to this file.
    - DO NOT add a "main" function to this file.
*/

int containsSequence(List seq1, List seq2) {
    // Empty sequence is always contained within another sequence
    if (seq2->first == NULL) return 1;
    // seq1 is NULL so seq2 is not in seq1
    if (seq1->first == NULL) return 0;

    Node curr1 = seq1->first;
    Node curr2 = seq2->first;
    
    while (curr1 != NULL) {
      if (curr2->value == curr1->value) {
        curr2 = curr2->next;
      }
      curr1 = curr1->next;
      // seq1 is traversed completely so it exists in seq2
      if (curr2 == NULL) return 1;
    }

    return 0;
}

