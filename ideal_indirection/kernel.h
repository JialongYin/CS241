/**
 * Ideal Indirection Lab
 * CS 241 - Spring 2019
 */
 
#pragma once
#include "page_table.h"
#include "types.h"
#include <stdbool.h>
#include <stdlib.h>

#define NUM_PHYSICAL_PAGES 250
// 1MB = 250 4kb pages
#define PHYSICAL_MEMORY_SIZE (PAGE_SIZE * NUM_PHYSICAL_PAGES)

/* Memory pool that represents all of physical memory */
extern char physical_memory[PHYSICAL_MEMORY_SIZE]
    __attribute__((aligned(PAGE_SIZE)));

/* Mapping to determine which page_table_entry owns which page. */
extern page_table_entry *entry_mapping[NUM_PHYSICAL_PAGES];

/*
 * Returns the physical address of the next chunk to allocate if possible,
 * these will be in PAGE_SIZE chunks aligned to PAGE_SIZE bytes.
 * Returns NULL if there are no more frames to give out.
 */
addr32 ask_kernel_for_frame(page_table_entry *entry);
void return_frame_to_kernel(void *base_addr);
void remove_swap_file(page_table_entry *entry);
void remove_swap_files();
void write_page_to_disk(page_table_entry *entry);
void read_page_from_disk(page_table_entry *entry);
void return_frame_to_kernel(void *frame_address);
void *get_system_pointer_from_pte(page_table_entry *entry);
void *get_system_pointer_from_pde(page_directory_entry *entry);
void *get_system_pointer_from_address(addr32 address);
