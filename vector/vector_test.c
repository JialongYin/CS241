/**
 * Vector Lab
 * CS 241 - Spring 2019
 */
#include <stdio.h>
#include "vector.h"
int main(int argc, char *argv[]) {
    // Write your test cases here
    vector *v = char_vector_create();
    char *str = "hello";
    char *ptr = str;
    while (*ptr) {
      vector_push_back(v, ptr);
      ptr++;
    }
    for (size_t i = 0; i < 5; i++) {
      char *c = (char *) vector_get(v, i);
      printf("c : %c\n", *c);
    }
    // vector_resize(v, size_t n);
    vector_destroy(v);
    return 0;
}
