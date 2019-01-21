/**
 * Extreme Edge Cases Lab
 * CS 241 - Spring 2019
 */
 
#pragma once

#include "camelCaser.h"

char *unescape(const char *input);
char *escape(const char *input);
void print_camelCaser(char *input);
int diff(char **output, char **expected_output);
int check_output(char *input, char **output);
