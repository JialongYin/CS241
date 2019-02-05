/**
 * Vector Lab
 * CS 241 - Spring 2019
 */

#include "sstring.h"
#include <string.h>

int main(int argc, char *argv[]) {
    // TODO create some tests
    // sstring *sstr = cstr_to_sstring("hello");
    // char *cstr = sstring_to_cstr(sstr);
    // printf("str : %s\n", cstr);
    // sstring_destroy(sstr);
    // free(cstr);
    //
    // sstring *sstr1 = cstr_to_sstring("hello world!");
    // sstring *sstr2 = cstr_to_sstring("my friend");
    // int num = sstring_append(sstr1, sstr2);
    // printf("num : %d\n", num);
    // sstring_destroy(sstr1);
    // sstring_destroy(sstr2);
    //
    // sstring *sstr_split = cstr_to_sstring("This is a sentence.");
    // vector *v = sstring_split(sstr_split, ' ');
    // for (size_t i = 0; i < vector_size(v); i++) {
    //   char *str = (char *) vector_get(v, i);
    //   printf("str : %s\n", str);
    // }
    // sstring_destroy(sstr_split);
    // vector_destroy(v);
    //
    // sstring *sstr_slice = cstr_to_sstring("This is a sentence.");
    // char *cstr_slice = sstring_slice(sstr_slice, 3, 7);
    // printf("%s\n", cstr_slice);
    // sstring_destroy(sstr_slice);
    // free(cstr_slice);

    // * sstring *replace_me = cstr_to_sstring("This is a {} day, {}!");
    // * sstring_substitute(replace_me, 18, "{}", "friend");
    // * sstring_to_cstr(replace_me); // == "This is a {} day, friend!"
    // * sstring_substitute(replace_me, 0, "{}", "good");
    // * sstring_to_cstr(replace_me); // == "This is a good day, friend!"
      // sstring *replace_me = cstr_to_sstring("This is a {} day, {}!");
      // int ret = sstring_substitute(replace_me, 18, "{}", "friend");
      // printf("ret : %d\n", ret);
      // char* res = sstring_to_cstr(replace_me); // == "This is a {} day, friend!"
      // printf("res : %s\n", res);
      // if (!res || ret || strcmp(res, "This is a {} day, friend!")) {
      //     #ifdef DEBUG
      //     printf("ret: %d. res: %s . expected is \"This is a {} day, friend!\" !\n", ret,res);
      //     #endif
      //     if (res) free(res);
      //     sstring_destroy(replace_me);
      //     return 0;
      // }
      //
      // free(res);
      // ret = sstring_substitute(replace_me, 0, "{}", "good");
      // printf("ret : %d\n", ret);
      // res = sstring_to_cstr(replace_me); // == "This is a good day, friend!"
      // printf("res : %s\n", res);
      // if (!res || ret || strcmp(res, "This is a good day, friend!")) {
      //     #ifdef DEBUG
      //     printf("ret: %d. res: %s . expected is \"This is a good day, friend!\" !\n", ret,res);
      //     #endif
      //     if (res) free(res);
      //     sstring_destroy(replace_me);
      //     return 0;
      // }
      //
      // free(res);
      // sstring_destroy(replace_me);


    sstring *replace_me = cstr_to_sstring("{ This is a  {  day,     { }");
    int ret = sstring_substitute(replace_me, 1, "{", "friend");
    char *cstr_sbs1 = sstring_to_cstr(replace_me); // == "This is a {} day, friend!"
    printf("ret1 : %d\n", ret);
    printf("str1 : %s\n", cstr_sbs1);
    ret = sstring_substitute(replace_me, 0, "{", "good");
    char *cstr_sbs2 = sstring_to_cstr(replace_me); // == "This is a good day, friend!"
    printf("ret2 : %d\n", ret);
    printf("str2 : %s\n", cstr_sbs2);
    sstring_destroy(replace_me);
    free(cstr_sbs1);
    free(cstr_sbs2);

    return 0;
}
