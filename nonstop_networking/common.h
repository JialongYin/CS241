/**
 * Nonstop Networking
 * CS 241 - Spring 2019
 */
#pragma once
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include "format.h"

#define LOG(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0);

typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;

ssize_t write_to_socket(int socket, const char *buffer, size_t count);
ssize_t read_from_socket(int socket, char *buffer, size_t count);
int print_any_err(size_t bytes_rd, size_t size);
