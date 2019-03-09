/**
 * Ideal Indirection Lab
 * CS 241 - Spring 2019
 */
 
#pragma once
#include "types.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    addr32 start;
    addr32 end;
    addr32 permissions;
    bool grows_down;
} vm_segmentation;

typedef enum segments {
    STACK,
    MMAP,
    HEAP,
    BSS,
    DATA,
    TEXT,
    TESTING, // this segment is made up for testing purposes
    NUM_SEGMENTS
} segments;

typedef struct { vm_segmentation segments[NUM_SEGMENTS]; } vm_segmentations;

typedef enum permissions { READ = 0x1, WRITE = 0x2, EXEC = 0x4 } permissions;

void grow_segment(vm_segmentations *segmentations, segments segment,
                  size_t num_pages);
void shrink_segment(vm_segmentations *segmentations, segments segment,
                    size_t num_pages);
bool address_in_segmentations(vm_segmentations *segmentations, addr32 address);
vm_segmentation *find_segment(vm_segmentations *segmentations, addr32 address);
