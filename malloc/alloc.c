/**
 * Malloc Lab
 * CS 241 - Spring 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

typedef struct _metadata_entry_t {
    void *ptr;
    size_t size;
    int free;
    struct _metadata_entry_t *next;
    struct _metadata_entry_t *prev;
} metadata_entry_t;

static metadata_entry_t *head = NULL;
static size_t total_memory_requested = 0;
static size_t total_memory_sbrk = 0;
void coalesceBlock(metadata_entry_t *p);
int splitBlock(size_t size, metadata_entry_t *entry);

void *calloc(size_t num, size_t size) {
    // implement calloc!
    size_t total = num * size;
    void *result = malloc(total);
    if (!result) return NULL;
    memset(result, 0, total);
    return result;
}

void *malloc(size_t size) {
    // implement malloc!
    if (size == 0) return NULL; // handle extreme case: size == 0
    metadata_entry_t *p = head;
    metadata_entry_t *chosen = NULL;
    if (total_memory_sbrk-total_memory_requested >= size) {
      while(p) {
        if (p->free && p->size >= size) {
          // if (splitBlock(size, p)) {
          //   total_memory_requested += sizeof(metadata_entry_t); // consider metadata when coalescing
          // }
          chosen = p; // first fit allocation
          break;
        }
        p = p->next;
      }
    }
    if (chosen) {
      chosen->free = 0;
    } else {
      size_t sbrk_size = 1*size; // sbrk double of size as requested
      chosen = sbrk(sizeof(metadata_entry_t)+sbrk_size);
      if (chosen == (void *)-1)
        return NULL;
      chosen->ptr = chosen + 1;
      chosen->size = sbrk_size;
      chosen->free = 0;
      chosen->next = head;
      if (head) {
        chosen->prev = head->prev;
        head->prev = chosen;
      } else {
        chosen->prev = NULL;
      }
      head = chosen;
      total_memory_sbrk += chosen->size;
    }
    total_memory_requested += chosen->size;
    return chosen->ptr;
}
void coalesceNext(metadata_entry_t *p) {
    p->size += p->next->size+sizeof(metadata_entry_t);
    p->next = p->next->next;
    p->next->prev = p;
}
void coalesceBlock(metadata_entry_t *p) {
  if (p->next && p->next->free == 1) {
    coalesceNext(p);
    total_memory_requested -= sizeof(metadata_entry_t);
  }
  if (p->prev && p->prev->free == 1) {
    p->prev->size += p->size+sizeof(metadata_entry_t);
    p->prev->next = p->next;
    if (p->next) {
      p->next->prev = p->prev;
    }
    p = p->prev;
    total_memory_requested -= sizeof(metadata_entry_t);
  }
}
void free(void *ptr) {
    // implement free!
    if (!ptr) return;
    metadata_entry_t *p = (metadata_entry_t *)ptr - 1;
    assert(p->free == 0);
    p->free = 1;
    total_memory_requested -= p->size;
    coalesceBlock(p);
    // printf("free size: %zu\n", p->size);
    return;
}
int splitBlock(size_t size, metadata_entry_t *entry) {
  if (entry->size >= 2*size && (entry->size-size) >= 1024) {
    size_t redu_size = size*2; // entry->size/2: split block at the middle of the previous  // size*2: split block double of new size
    metadata_entry_t *new_entry = entry->ptr + redu_size;
    // printf("splitBlock segfault start\n");
    // printf("entry->ptr: %p\n", entry->ptr);
    // printf("new_entry: %p\n", new_entry);
    // printf("entry->ptr end: %p\n", entry->ptr+entry->size);
    new_entry->ptr = (new_entry + 1);
    new_entry->free = 1;
    new_entry->size = entry->size - redu_size - sizeof(metadata_entry_t);
    new_entry->next = entry->next;
    // printf("splitBlock segfault end\n");
    if (entry->next) {
      entry->next->prev = new_entry;
    }
    new_entry->prev = entry;
    entry->size = redu_size;
    entry->next = new_entry;
    return 1;
  }
  return 0;
}
void *realloc(void *ptr, size_t size) {
    // implement realloc!
    if (!ptr) return malloc(size);
    metadata_entry_t *entry = ((metadata_entry_t *)ptr) - 1;
    assert(entry->ptr == ptr);
    assert(entry->free == 0);
    if (!size) free(ptr);
    // printf("realloc segfault start\n");
    if (splitBlock(size, entry)) {
      total_memory_requested -= entry->next->size;
    }
    // printf("realloc segfault end\n");
    if (entry->size >= size) {
      return ptr;
    } else if (entry->next && entry->next->free && entry->size+entry->next->size+sizeof(metadata_entry_t) >= size) {
      total_memory_requested += entry->next->size;
      coalesceNext(entry);
      return entry->ptr;
    }
    void *new_ptr = malloc(size);
    memcpy(new_ptr, ptr, entry->size);
    free(ptr);
    return new_ptr;
}
