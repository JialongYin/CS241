/**
 * Extreme Edge Cases Lab
 * CS 241 - Spring 2019
 */
 
#include "camelCaser.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char **stc_gen(const char *input_str){	
	const char *start = input_str;
	int count = 0;
	while( *input_str ){
        	if(ispunct(*input_str)) count++;
		input_str++;	   
	}
	input_str = start;
	//printf("count : %d\n", count);
	char **input_sl = (char **) calloc(count+1, sizeof(char *));	
	int num = 0;
	unsigned i = 0;
	const char *ptr = start;
	while( *input_str ){
		 
                if(ispunct(*input_str)){
			char *input_s = (char *) calloc(num+1, sizeof(char));//no punct string
			strncpy(input_s, ptr, num);
			ptr = input_str+1;
			//printf("num : %d\n", num);
			//printf("input_str: %s\n", input_str);
			//printf("input_s : %s\n", input_s);
			input_sl[i] = input_s;		
			i++;
			num = -1;		
		}
		num++;
                input_str++;
        }
	input_str = start;
	return input_sl;
}
char **output_gen(char **input_sl){
	//printf("output_gen start\n");
	char **start = input_sl;
	int count = 0;
	while ( *input_sl ) {
		//printf("input_s : %s\n", *input_sl);
		count++;
		input_sl++;
	}
	input_sl = start;
	//printf("count : %d\n", count);
	char **output_sl = (char **) calloc(count+1, sizeof(char *));
	char **start_o = output_sl;
	int i = 0;
	while ( *input_sl ) {
		char *stc =  *input_sl;
		int num = 0;
		while ( *stc ) {
			if ( !isspace(*stc) ){
				num++;	 
			}
			stc++;
		}
		//printf("num : %d\n", num);
		char *output_s = (char *) calloc(num+1, sizeof(char));
		output_sl[i] = output_s;
		output_sl++;
		input_sl++;
	}
	output_sl = start_o;
	input_sl = start;
	while ( *input_sl ) {
                char *stc = *input_sl;
		char *output_s = *output_sl;
		char *start_os = output_s;
                int pre_space = 1;
		int i = 0;
                while ( *stc ) {
                        if ( !isspace(*stc) ){
                                if ( pre_space ) {
					if ( isalpha(*stc) ) {
						*stc = toupper(*stc); 
						pre_space = 0; //1st alpha
					}
					output_s[i] = *stc;			
				} 
				else{
					if ( isalpha(*stc) ) *stc = tolower(*stc);
					output_s[i] = *stc;
				}
				i++;
                        }
			else {
				pre_space = 1;
			}
                        stc++;
                }
		if ( start_os && *start_os ) {
			//printf("before seg : %s\n", start_os); 
			while ( !isalpha(*start_os) ) start_os++; 
		}
		if ( start_os && *start_os ) *start_os = tolower(*start_os);
                input_sl++;
		output_sl++;
        }
	input_sl = start;
	output_sl = start_o;
	return start_o;
}

char **camel_caser(const char *input_str) {
	if (input_str == NULL) return NULL;
	//printf("input_str : %s\n", input_str);
	char **input_sl = stc_gen(input_str);
	//char **start = input_sl;
	//while ( *input_sl ) printf( "%s\n", *input_sl++ );
	//input_sl = start;
	char **output_sl = output_gen(input_sl);
	destroy(input_sl);
	//printf("destroy input_sl works well\n");
	return output_sl;
}

void destroy(char **result) {
	if ( result == NULL ) return;
	char **ptr = result;
	while ( *ptr ) {
		free(*ptr);
		ptr++;
	}
	free(*ptr);
	free(result);
	return;
}
