
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

        output = camelCaser("");
        if ( output[0] != 0 ) return 0;
        destroy(output);
	/*
	output = camelCaser("   ");
        if ( output[0] != 0 ) return 0;
        destroy(output);
	*/
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
	/*
        //Testcase3:
        char str3[] = " \n &A2c 3e5 123   iS a WORD? No! = @ disapeear\t ";
        output = camelCaser(str3);
        if ( strcmp(output[0], "")) return 0;
        if ( strcmp(output[1], "a2c3E5123IsAWord")) return 0;
        if ( strcmp(output[2], "no")) return 0;
        if ( strcmp(output[3], "")) return 0;
        if ( strcmp(output[4], "")) return 0;
        destroy(output);
	*/
        //Testcase4:
        char str4[] = "@.!";// "", NULL
        output = camelCaser(str4);
        if ( strcmp(output[0], "")) return 0;
        if ( strcmp(output[1], "")) return 0;
        if ( strcmp(output[2], "")) return 0;
        destroy(output);
        //printf("testcase4 pass\n");
	
        //Testcase5:
        char str5[] = " \a   \b \n     \t     ";// "", NULL
        output = camelCaser(str5);
        if (output[0] != NULL) return 0;
        //if ( strcmp(output[0], "")) return 0;
        //if ( strcmp(output[1], "")) return 0;
        //if ( strcmp(output[1], "")) return 0;
        destroy(output);
	
	//Testcase6:
        char str6[] = "123@.12d!";// "", NULL
        output = camelCaser(str6);
        if ( strcmp(output[0], "123")) return 0;
        if ( strcmp(output[1], "")) return 0;
        if ( strcmp(output[2], "12d")) return 0;
        destroy(output);
	
	//Testcase7:
        char str7[] = "@1.2!";// "", NULL
        output = camelCaser(str7);
        if ( strcmp(output[0], "")) return 0;
        if ( strcmp(output[1], "1")) return 0;
        if ( strcmp(output[2], "2")) return 0;
        destroy(output);
	
	//Testcase8:
        char str8[] = "@";// "", NULL
        output = camelCaser(str8);
        if ( strcmp(output[0], "")) return 0;
        destroy(output);
	/*
	//Testcase9:
	char str9[] = "The Heisenbug is an incredible creature. Facenovel servers get their power from its indeterminism. Code smell can be ignored with INCREDIBLE use of air freshener. God objects are the new religion.";// "", NULL
        output = camelCaser(str9);
        if ( strcmp(output[0], "theHeisenbugIsAnIncredibleCreature")) return 0;
        if ( strcmp(output[1], "facenovelServersGetTheirPowerFromItsIndeterminism")) return 0;
        if ( strcmp(output[2], "codeSmellCanBeIgnoredWithIncredibleUseOfAirFreshener")) return 0;
        if ( strcmp(output[3], "godObjectsAreTheNewReligion")) return 0;
        destroy(output);
	*/
	//Testcase10: 
        char str10[] = "rgd";// NULL
        output = camelCaser(str10);
        if ( output[0] != NULL ) return 0;
        /*
	//Testcase11:
        char str11[] = "a@";// "", NULL
        output = camelCaser(str11);
        if ( strcmp(output[0], "a")) return 0;
        destroy(output);
	*/
	/*
	//Testcase12: 
        char str12[] = "r";// NULL
        output = camelCaser(str12);
        if ( output[0] != NULL ) return 0;
       */
	/*
	//Testcase14:
        char str14[] = "   @ .  ! ";// "", NULL
        output = camelCaser(str14);
        if ( strcmp(output[0], "")) return 0;
        if ( strcmp(output[1], "")) return 0;
        if ( strcmp(output[2], "")) return 0;
        destroy(output);
	*/
 	/*
	//Testcase15: 
        char str15[] = "1";
        output = camelCaser(str15);
        if ( output[0] != NULL ) return 0;
	*/
 	//Testcase16: 
	const char* str16="\a\bas\a\bas, f62 and \a y,";
    	char* str5_0="\a\bas\a\bas";
    	char* str5_1="f62And\aY";
    	char** output5=camelCaser(str16);
    	//printf("%s\n",output5[0]);
    	//printf("%s\n",output5[1]);
    	if(strcmp(output5[0],str5_0)!=0){return 0;}
    	if(strcmp(output5[1],str5_1)!=0){return 0;}
    	destroy(output5);

	//Testcase17:
	const char* str17=" qwr \v \af, food  ";
        char* str6_0="qwr\aF";
        char** output6=camelCaser(str17);
        //printf("%s\n",output6[0]);
        if(strcmp(output6[0],str6_0)!=0){return 0;}
        destroy(output6);
	
	//Testcase18:
	const char* str18="76\v9 IQ\\T : M\bE q78pE\nm Gu\ni?";
        char* str10_0="769Iq";
        char* str10_1="t";
        char* str10_2="m\beQ78peMGuI";
        char** output10=camelCaser(str18);
        if(strcmp(output10[0],str10_0)!=0){return 0;}
        if(strcmp(output10[1],str10_1)!=0){return 0;}
        if(strcmp(output10[2],str10_2)!=0){return 0;}
        destroy(output10);
	
        return 1;
}

