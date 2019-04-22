/**
 * Resplendent RPCs
 * CS 241 - Spring 2019
 */
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *check_cache_for_address(char *hostname, char *cache_file_name) {
    FILE *file = fopen(cache_file_name, "r");
    if (!file) {
        return NULL;
    }

    size_t hostlen = strlen(hostname);
    ssize_t nread;
    size_t len = 0;
    int found = 0;
    char *line = NULL;
    char *ret = NULL;
    while ((nread = getline(&line, &len, file)) != -1 && ret == NULL) {
        if (nread == 0) {
            continue;
        }
        char *newline = strchr(line, '\n');
        if (newline) {
            *newline = '\0';
        }
        char *delim = strchr(line, ';');
        if (!delim) {
            continue;
        }
        *delim = '\0';
        char *host = line;
        char *ip = delim + 1;
        if (strcmp(hostname, line) == 0) {
            ret = strdup(ip);
        }
    }
    free(line);
    fclose(file);
    return ret;
}
