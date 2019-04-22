/**
 * Resplendent RPCs
 * CS 241 - Spring 2019
 */
#pragma once

/**
 * Given a file at `cache_file_name`, this function
 * Will loop through the file to return an ip address
 * That matches `hostname` given
 * If the line can't be found, the function returns
 * NULL
 */
char *check_cache_for_address(char *hostname, char *cache_file_name);

/**
 * Given a file at `cache_file_name`, this function
 * adds a `hostname` and `ip` to the end of the file
 **/
void add_to_cache(char *hostname, char *ip, char *cache_file);
