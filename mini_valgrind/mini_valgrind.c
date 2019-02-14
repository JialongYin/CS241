/**
 * Mini Valgrind Lab
 * CS 241 - Spring 2019
 partner: jialong2, qishanz2
 */

#include "mini_valgrind.h"
#include <stdio.h>
#include <string.h>

meta_data* head;
size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;
void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    if (request_size == 0) return NULL;
    void* ptr = malloc(sizeof(meta_data) + request_size);
    if (!ptr) return NULL;

    meta_data* new_meta = (meta_data*)ptr;
    new_meta->request_size = request_size;
    new_meta->filename = filename;
    new_meta->instruction = instruction;
    new_meta->next = head;

    head = new_meta;
    total_memory_requested += request_size;
    return ptr + sizeof(meta_data);
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
    if (num_elements == 0 || element_size == 0) {
      return NULL;
    }

    size_t request_size = num_elements * element_size;
    void* ptr = mini_malloc(request_size, filename, instruction);
    if (!ptr) return NULL;

    memset(ptr, 0, request_size);
    return ptr;
}

void *mini_realloc(void *ptr, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
    if (!ptr) return mini_malloc(request_size, filename, instruction);
    if (request_size == 0) {
      mini_free(ptr);
      return NULL;
    }

    meta_data* prev_p = NULL;
    meta_data* meta_p = head;
    while (meta_p) {
      void* memory_ptr = (void*)meta_p + sizeof(meta_data);
      if (memory_ptr == ptr) {
        break;
      }
      prev_p = meta_p;
      meta_p = meta_p->next;
    }

    if (!meta_p) {
      invalid_addresses++;
      return NULL;
    }

    meta_data* new_p = realloc(meta_p, request_size + sizeof(meta_data));
    if (!new_p) return NULL;

    size_t old_request_size = meta_p->request_size;

    new_p->request_size = request_size;
    new_p->filename = filename;
    new_p->instruction = instruction;

    if (new_p != meta_p) {
      prev_p->next = new_p;
      new_p->next = meta_p->next;
      free(meta_p);
    }

    if (old_request_size < request_size) {
      total_memory_requested += request_size - old_request_size;
    } else if (old_request_size > request_size) {
      total_memory_freed += old_request_size - request_size;
    }
    return new_p + 1;
}

void mini_free(void *ptr) {
    // your code here
    if (!ptr) return;

    meta_data* meta_p = head;
    meta_data* prev_p = NULL;
    while(meta_p) {
      void* memory_ptr = (void*)meta_p + sizeof(meta_data);
      if (memory_ptr == ptr) {
        break;
      }
      prev_p = meta_p;
      meta_p = meta_p->next;
    }

    if (!meta_p) {
      invalid_addresses++;
      return;
    }

    if (prev_p) {
      prev_p->next = meta_p->next;
    } else {
      head = meta_p->next;
    }
    total_memory_freed += meta_p->request_size;

    free(meta_p);
}
