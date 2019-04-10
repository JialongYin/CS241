/**
 * Mad Mad Access Patterns
 * CS 241 - Spring 2019
 */
#include "tree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/
BinaryTreeNode *wordBinarySearch(uint32_t offset, FILE *file, char *word) {
  if (offset == 0) return NULL;
  fseek(file, offset, SEEK_SET);
  BinaryTreeNode *node = calloc(1,sizeof(BinaryTreeNode)+10);
  fread(node, sizeof(BinaryTreeNode)+10, 1, file);
  if (strcmp(word, node->word) == 0) {
    return node;
  }
  BinaryTreeNode *word_node;
  if ((word_node = wordBinarySearch(node->left_child, file, word))) {
    free(node);
    return word_node;
  }
  if ((word_node = wordBinarySearch(node->right_child, file, word))) {
    free(node);
    return word_node;
  }
  free(node);
  return NULL;
}
int main(int argc, char **argv) {
  if (argc < 3) {
    printArgumentUsage();
    exit(1);
  }
  char *file_name = argv[1];
  FILE *file = fopen(file_name, "r");
  if (file == 0) {
    openFail(file_name);
    exit(2);
  }
  char root[BINTREE_ROOT_NODE_OFFSET];
  fread(root, 1, BINTREE_ROOT_NODE_OFFSET, file);
  if (strcmp(root, BINTREE_HEADER_STRING)) {
    formatFail(file_name);
    exit(2);
  }
  for (int i = 2; i < argc; i++) {
    BinaryTreeNode *word_node = wordBinarySearch(BINTREE_ROOT_NODE_OFFSET, file, argv[i]);
    if (word_node) {
      printFound(word_node->word, word_node->count, word_node->price);
      free(word_node);
    } else {
      printNotFound(argv[i]);
    }
  }
  fclose(file);
  return 0;
}
