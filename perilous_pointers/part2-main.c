/**
 * Perilous Pointers Lab
 * CS 241 - Spring 2019
 */
 
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {

	first_step(81);

	int value_2 = 132;
	second_step(&value_2);
	
	int v = 8942;	
	int *ptr = &v;
	int **value_3 = &ptr;
	double_step(value_3);
	
	char value_4[10];
	for (int i = 0; i < 10; i++) value_4[i] = 0; 
	int number = 15;
	value_4[5] = (char) number;
	strange_step(value_4);
	
	char *value_5 = "012";
	empty_step(value_5);
	
	char *s2 = "012u";
	char *s = s2;
	two_step(s, s2);

	char *first_7 = "0123456";
	char *second_7 = first_7 + 2;
	char *third_7 = second_7 + 2;
	three_step(first_7, second_7, third_7);

	char first_8[] = " abcdefg";
        char second_8[20];
	second_8[2] = (char)((int)first_8[1] + 8);
        char third_8[20];
	third_8[3] = (char)((int)second_8[2] + 8);
	step_step_step(first_8, second_8, third_8);

	int b = 40;
	char a = 40;
	it_may_be_odd(&a, b);

	char str[] = " ,CS241";
	tok_step(str);

	char blue[10];
	for (int i = 0; i < 10; i++) blue[i] = 0;
	blue[0] = 1; blue[1] = 2; blue[2] = 0; blue[3] = 0;
	the_end(blue, blue);

	return 0;
}
