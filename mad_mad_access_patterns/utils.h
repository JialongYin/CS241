/**
 * Mad Mad Access Patterns
 * CS 241 - Spring 2019
 */
#pragma once

#include <stdint.h>
#include <stdio.h>
void openFail(const char *file_name);
void mmapFail(const char *file_name);
void formatFail(const char *file_name);

void printFound(const char *word, const uint32_t count, const float price);
void printNotFound(const char *word);
void printArgumentUsage();
