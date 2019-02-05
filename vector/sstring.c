/**
 * Vector Lab
 * CS 241 - Spring 2019
 */

#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    // Anything you want
    vector *v;
};

sstring *cstr_to_sstring(const char *input) {
    // your code goes here
    sstring *str_p = calloc(1, sizeof(sstring));
    str_p->v = char_vector_create();
    const char *ptr = input;
    while (*ptr) {
      vector_push_back(str_p->v, (void *) ptr);
      ptr++;
    }
    return str_p;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
    char *cstr = calloc(vector_size(input->v)+1, sizeof(char));
    for (size_t i = 0; i < vector_size(input->v); i++) {
      cstr[i] = *((char *) vector_get(input->v, i));
    }
    return cstr;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
    for (size_t i = 0; i < vector_size(addition->v); i++) {
      vector_push_back(this->v, vector_get(addition->v, i));
    }
    return vector_size(this->v);
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
    vector *str_l = string_vector_create();
    sstring *str_p = calloc(1, sizeof(sstring));
    str_p->v = char_vector_create();
    for (size_t i = 0; i < vector_size(this->v); i++) {
      char *curr = vector_get(this->v, i);
      if (*curr == delimiter) {
        char *cstr = sstring_to_cstr(str_p);
        vector_push_back(str_l, cstr);
        vector_clear(str_p->v);
        free(cstr);
      } else {
        vector_push_back(str_p->v, curr);
      }
    }
    char *cstr_end = sstring_to_cstr(str_p);
    vector_push_back(str_l, cstr_end);
    free(cstr_end);
    sstring_destroy(str_p);
    return str_l;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // your code goes here
    char *ptr_tgt = target;
    int flag_sbs = 1;
    for (size_t i = offset; i < vector_size(this->v); i++) {
      char *curr = vector_get(this->v, i);
      if (*curr == *ptr_tgt && (vector_size(this->v)-i) >= strlen(target)) {
        int j = i;
        while ( *ptr_tgt ) {
          if ( *((char *) vector_get(this->v, j)) != *ptr_tgt ) {
            flag_sbs = 0;
            ptr_tgt = target;
            break;
          }
          j++;
          ptr_tgt++;
        }
        if ( flag_sbs ) {
          for (size_t n = 0; n < strlen(target); n++) {
            vector_erase(this->v, i);
          }
          for (size_t n = 0; n < strlen(substitution); n++) {
            vector_insert(this->v, i, substitution+n);
            i++;
          }
          return 0;
        }
      }
    }
    return -1;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
    char *cstr = calloc((end-start)+1, sizeof(char));
    int j = 0;
    for (int i = start; i < end; i++) {
      cstr[j] = *((char *) vector_get(this->v, i));
      j++;
    }
    cstr[j] = '\0';
    return cstr;
}

void sstring_destroy(sstring *this) {
    // your code goes here
    vector_destroy(this->v);
    free(this);
}
