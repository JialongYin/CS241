/**
 * Mad Mad Access Patterns
 * CS 241 - Spring 2019
 Partner: jialong2, yichiz3, xy6
 */
#include "tree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/
BinaryTreeNode *wordBinarySearch(uint32_t offset, char *addr, char *word) {
  if (offset == 0) return NULL;
  BinaryTreeNode *node = (BinaryTreeNode *) (addr+offset);
  BinaryTreeNode *word_node;
  if (strcmp(word, node->word) == 0) {
    return node;
  } else if (strcmp(word, node->word) < 0) {
    if ((word_node = wordBinarySearch(node->left_child, addr, word))) {
      return word_node;
    }
  } else {
    if ((word_node = wordBinarySearch(node->right_child, addr, word))) {
      return word_node;
    }
  }
  return NULL;
}
int main(int argc, char **argv) {
  if (argc < 3) {
    printArgumentUsage();
    exit(1);
  }
  char *file_name = argv[1];
  int fd = open(file_name, O_RDONLY);
  if (fd < 0) {
    openFail(file_name);
    exit(2);
  }
  struct stat filestat;
  if(fstat(fd, &filestat) !=0) {
    openFail(file_name);
    exit(2);
  }
  char* addr = mmap(NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == (void *)-1) {
    mmapFail(file_name);
    exit(2);
  }
  if (strncmp(addr, BINTREE_HEADER_STRING, BINTREE_ROOT_NODE_OFFSET)) {
    formatFail(file_name);
    exit(2);
  }
  for (int i = 2; i < argc; i++) {
    BinaryTreeNode *word_node = wordBinarySearch(BINTREE_ROOT_NODE_OFFSET, addr, argv[i]);
    if (word_node) {
      printFound(word_node->word, word_node->count, word_node->price);
    } else {
      printNotFound(argv[i]);
    }
  }
  close(fd);
  return 0;
}
