/**
 * Extreme Edge Cases Lab
 * CS 241 - Spring 2019
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @param  destroy      A pointer to the function that destroys camelCaser
 * output.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Return 1 if the passed in function works properly; 0 if it doesn't.
	
	char **output;
	
	output = camelCaser(NULL); 
	if ( output ) return 0;
	destroy(output);

	//Testcase1: 
	char str1[] = " A2c 3e5 123   iS a WORD? No! disapeear "; 
	output = camelCaser(str1);
	if ( strcmp(output[0], "a2c3E5123IsAWord")) return 0;
	if ( strcmp(output[1], "no")) return 0;
	destroy(output);
	
	//Testcase2: 
	char str2[] = " \n \0 . b";// NULL
	output = camelCaser(str2);
	if ( output[0] != NULL ) return 0;
	//printf("testcase2 pass\n");
	
	//Testcase3:
	char str3[] = " &A2c 3e5 123   iS a WORD? No! = @ disapeear "; 
        output = camelCaser(str3);
	if ( strcmp(output[0], "")) return 0;
        if ( strcmp(output[1], "a2c3E5123IsAWord")) return 0;
        if ( strcmp(output[2], "no")) return 0;
	if ( strcmp(output[3], "")) return 0;
	if ( strcmp(output[4], "")) return 0;
        destroy(output);
	
	//Testcase4:
	char str4[] = "   .@   !   ";// "", NULL
        output = camelCaser(str4);
        if ( strcmp(output[0], "")) return 0;
	if ( strcmp(output[1], "")) return 0;
	if ( strcmp(output[1], "")) return 0;
	destroy(output);
	//printf("testcase4 pass\n");

	//Testcase5:
	char str5[] = "  \a\b  \n ";// "", NULL
        output = camelCaser(str5);
	if (output[0] != NULL) return 0;
        //if ( strcmp(output[0], "")) return 0;
        //if ( strcmp(output[1], "")) return 0;
        //if ( strcmp(output[1], "")) return 0;
        destroy(output);
	return 1;
}
