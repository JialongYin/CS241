/**
 * mapreduce Lab
 * CS 241 - Spring 2019
 */

#include "utils.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if (argc != 6) {
      print_usage();
      return 1;
    }
    char *input = argv[1];
    char *output = argv[2];
    char *mapper = argv[3];
    char *reducer = argv[4];
    int count;
    if (1 != sscanf(argv[5], "%d", &count) || count < 1) {
      print_usage();
      return 1;
    }
    // Create an input pipe for each mapper.
    int* fd_m[count];
    for (int i = 0; i < count; i++) {
      fd_m[i] = calloc(2, sizeof(int));
      pipe(fd_m[i]);
    }
    // Create one input pipe for the reducer.
    int fd_r[2];
    pipe(fd_r);
    // Open the output file.
    int fd_o = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
    // Start a splitter process for each mapper.
    pid_t child_l[count];
    for (int i = 0; i < count; i++) {
      pid_t child = fork();
      child_l[i] = child;
      if (child == 0) {
        char idx_str[10];
        sprintf(idx_str, "%d", i);
        close(fd_m[i][0]);
        dup2(fd_m[i][1], 1);
        execl("./splitter", "./splitter", input, argv[5],
                  idx_str, NULL);
        exit(1);
      }
    }

    // Start all the mapper processes.
    pid_t child_m[count];
    for (int i = 0; i < count; i++) {
      close(fd_m[i][1]);
      pid_t child = fork();
      child_m[i] = child;
      if (child == 0) {
        close(fd_r[0]);
        dup2(fd_m[i][0], 0);
        dup2(fd_r[1], 1);
        execl(mapper, mapper, NULL);
        exit(1);
      }
    }

    // Start the reducer process.
    close(fd_r[1]);
    pid_t child = fork();
    if (child == 0) {
      dup2(fd_r[0], 0);
      dup2(fd_o, 1);
      execl(reducer, reducer, NULL);
      exit(1);
    }
    close(fd_r[0]);
    close(fd_o);
    // Wait for the reducer to finish.
    for (int i = 0; i < count; i++) {
      int status;
      waitpid(child_l[i], &status, 0);
    }
    for (int i = 0; i < count; i++) {
      close(fd_m[i][0]);
      int status;
      waitpid(child_m[i], &status, 0);
    }
    int status;
    waitpid(child, &status, 0);
    // Print nonzero subprocess exit codes.
    if (status)
      print_nonzero_exit_status(reducer, status);
    // Count the number of lines in the output file.
    print_num_lines(output);

    for (int i = 0; i < count; i++) {
      free(fd_m[i]);
    }
    return 0;
}
