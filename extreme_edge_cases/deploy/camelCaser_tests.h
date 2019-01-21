/**
 * Extreme Edge Cases Lab
 * CS 241 - Spring 2019
 */
 
#pragma once

#include "camelCaser.h"

int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **));
