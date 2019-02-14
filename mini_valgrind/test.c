/**
 * Mini Valgrind Lab
 * CS 241 - Spring 2019
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    // Your tests here using malloc and free
    void *p1 = malloc(30);
    p1 = realloc(p1, 500);
    free(p1);
    return 0;
}
