/**
 * Vector Lab
 * CS 241 - Spring 2019
 */

#include "sstring.h"

int main(int argc, char *argv[]) {
    // TODO create some tests
    sstring *sstr = cstr_to_sstring("hello");
    char *cstr = sstring_to_cstr(sstr);
    printf("str : %s\n", cstr);
    sstring_destroy(sstr);
    free(cstr);

    sstring *sstr1 = cstr_to_sstring("hello world!");
    sstring *sstr2 = cstr_to_sstring("my friend");
    int num = sstring_append(sstr1, sstr2);
    printf("num : %d\n", num);
    sstring_destroy(sstr1);
    sstring_destroy(sstr2);

    sstring *sstr_split = cstr_to_sstring("This is a sentence.");
    vector *v = sstring_split(sstr_split, ' ');
    for (size_t i = 0; i < vector_size(v); i++) {
      char *str = (char *) vector_get(v, i);
      printf("str : %s\n", str);
    }
    sstring_destroy(sstr_split);
    vector_destroy(v);

    sstring *sstr_slice = cstr_to_sstring("This is a sentence.");
    char *cstr_slice = sstring_slice(sstr_slice, 3, 7);
    printf("%s\n", cstr_slice);
    sstring_destroy(sstr_slice);
    free(cstr_slice);

    sstring *replace_me = cstr_to_sstring("This is a {} day, {}!");
    sstring_substitute(replace_me, 18, "{}", "friend");
    char *cstr_sbs = sstring_to_cstr(replace_me); // == "This is a {} day, friend!"
    printf("str : %s\n", cstr_sbs);
    sstring_destroy(replace_me);
    free(cstr_sbs);

    return 0;
}
