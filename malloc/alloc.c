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
} metadata_entry_t;

static metadata_entry_t *head = NULL;
static size_t total_memory_requested = 0;
static size_t total_memory_freed = 0;
static size_t total_memory_sbrk = 0;

void *calloc(size_t num, size_t size) {
    // implement calloc!
    size_t total = num * size;
    void *result = malloc(total);
    if (!result) return NULL;
    memset(result, 0, total);
    return result;
}
void splitBlock(size_t size, metadata_entry_t *entry) {
  if (entry->size >= 2*size && (entry->size-size) > 1024) {
    metadata_entry_t *new_entry = (void *)(entry + 1) + entry->size/2;
    new_entry->ptr = new_entry + 1;
    new_entry->free = 1;
    new_entry->size = entry->size - entry->size/2 - sizeof(new_entry);
    new_entry->next = entry->next;
    entry->size = entry->size/2;
    entry->next = new_entry;
    total_memory_requested += sizeof(metadata_entry_t); // consider metadata when coalescing
  }
}
void *malloc(size_t size) {
    // implement malloc!
    if (size == 0) return NULL; // handle extreme case: size == 0
    metadata_entry_t *p = head;
    metadata_entry_t *chosen = NULL;
    if (total_memory_sbrk-(total_memory_requested-total_memory_freed) >= size) {
      while(p) {
        if (p->free && p->size >= size) {
          splitBlock(size, p);
          chosen = p; // first fit allocation
        }
        p = p->next;
      }
    }

    if (chosen) {
      // not include metadata in size after freed
      // not split blocks check necessarity if (chosen->size>>size+*)
      chosen->free = 0;
    } else {
      chosen = sbrk(sizeof(metadata_entry_t)+size);
      if (chosen == (void *)-1)
        return NULL;
      chosen->ptr = chosen + 1;
      chosen->size = size;
      chosen->free = 0;
      chosen->next = head;
      head = chosen;
      total_memory_sbrk += chosen->size;
    }
    total_memory_requested += chosen->size;
    return chosen->ptr;
}

void free(void *ptr) {
    // implement free!
    if (!ptr) return;
    metadata_entry_t *p = (metadata_entry_t *)ptr - 1;
    assert(p->free == 0);
    p->free = 1;
    total_memory_freed += p->size;

    // metadata_entry_t *p = head;
    // metadata_entry_t *prev = NULL;
    // while (p) {
    //   if (p->ptr == ptr) {
    //     // not handle block coalescing
    //     p->free = 1;
    //     total_memory_freed += p->size;
    //     break;
    //   }
    //   prev = p;
    //   p = p->next;
    // }
    return;
}

void *realloc(void *ptr, size_t size) {
    // implement realloc!
    if (!ptr) return malloc(size);
    metadata_entry_t *entry = ((metadata_entry_t *)ptr) - 1;
    assert(entry->ptr == ptr);
    assert(entry->free == 0);
    if (!size) free(ptr);
    splitBlock(size, entry);
    // printf("pass here\n");
    if (entry->size >= size) {
      return ptr;
    }
    void *new_ptr = malloc(size);
    memcpy(new_ptr, ptr, entry->size);
    free(ptr);
    return new_ptr;
}
