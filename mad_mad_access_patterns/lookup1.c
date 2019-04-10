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
int wordBinarySearch(uint32_t offset, FILE *file, char *word) {
  if (offset == 0) return 0;
  fseek(file, offset, SEEK_SET);
  BinaryTreeNode node;
  fread(&node, sizeof(BinaryTreeNode), 1, file);
  fseek(file, offset+sizeof(BinaryTreeNode), SEEK_SET);
  char word_node[10];
  fread(word_node, 10, 1, file);
  if (strcmp(word, word_node) == 0) {
    printFound(word_node, node.count, node.price);
    return 1;
  } else if (strcmp(word, word_node) < 0) {
    if (wordBinarySearch(node.left_child, file, word)) return 1;
  } else  {
    if (wordBinarySearch(node.right_child, file, word)) return 1;
  }
  return 0;
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
    int find = wordBinarySearch(BINTREE_ROOT_NODE_OFFSET, file, argv[i]);
    if (!find) {
      printNotFound(argv[i]);
    }
  }
  fclose(file);
  return 0;
}
