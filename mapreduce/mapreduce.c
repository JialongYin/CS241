/**
 * mapreduce Lab
 * CS 241 - Spring 2019
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "utils.h"

#define SPLITTER "./splitter"

/* pointer to fds for mapper  */
int* mappers_fds;
/* fds for reducer */
int reducer_fds[2];

int main(int argc, char **argv) {
    // check usage
    // ./mapreduce <input_file> <output_file> <mapper_executable> <reducer_executable> <mapper_count>
    if (argc != 6) {
      print_usage();
      exit(1);
    }
    // parse args
    char* input_file = argv[1];
    char* output_file = argv[2];
    char* mapper_name = argv[3];
    char* reducer_name = argv[4];
    size_t mapper_count = (size_t)atoi(argv[5]);

    // save child pid
    pid_t splitter_pids[128];
    pid_t mappers_pids[128];
    pid_t reducer_pid;

    // Create an input pipe for each mapper.
    mappers_fds = (int*)malloc(sizeof(int) * mapper_count * 2);
    size_t i = 0;
    for (; i < mapper_count; i++) {
      pipe(&mappers_fds[2 * i]);
    }

    // Create one input pipe for the reducer.
    pipe(reducer_fds);

    // Open the output file.
    // open(pathname, flags, mode)
    // mode must be supplied when O_CREAT or O_TMPFILE is specified in flags
    int output_fd = open(output_file, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);

    // Start a splitter process for each mapper.
    for (i = 0; i < mapper_count; i++) {
      splitter_pids[i] = fork();
      if (splitter_pids[i] == 0) {  // child : splitter
        size_t readend = 2*i;
        size_t writend = 2*i + 1;
        close(mappers_fds[readend]);   // close write end for reducer
        dup2(mappers_fds[writend], 1);      // redirect output file des to stdout

        char str_mapper_count[5];
        sprintf(str_mapper_count, "%zu", mapper_count);
        char str_i[5];
        sprintf(str_i, "%zu", i);
        int execret = execl(SPLITTER, SPLITTER, input_file, str_mapper_count, str_i, NULL);
        # ifdef DEBUG
        perror("");
        # endif
        return execret;
      } else { // parent
        // just continue
      }
    }

    for (i = 0; i < mapper_count; i++) {
       // close write end for every mapper --> not blocking mappers
      size_t writend = 2*i + 1;
      close(mappers_fds[writend]);
    }

    // Start all the mapper processes.
    for (i = 0; i < mapper_count; i++) {
      mappers_pids[i] = fork();
      if (mappers_pids[i] == 0) {  // child : mapper
        size_t readend = 2*i;
        size_t writend = 2*i + 1;
        close(mappers_fds[writend]);   // close write end for reducer
        close(reducer_fds[0]);
        dup2(mappers_fds[readend], 0);      // redirect output file des to stdout
        dup2(reducer_fds[1], 1); // redirect reducer writeend to stdout

        int execret = execl(mapper_name, mapper_name, NULL);
        return execret;
      } else { // parent
        // just continue
      }
    }

    // Start the reducer process.
    reducer_pid = fork();
    if (reducer_pid== 0) { // child!
      close(reducer_fds[1]);   // close write end for reducer
      dup2(reducer_fds[0], 0); // redirect reducer pipe read-end to stdin
      dup2(output_fd, 1);      // redirect output file des to stdout

      int execret = execl(reducer_name, reducer_name, NULL);
      return execret;
    } else { // parent
      // Wait for the reducer to finish.
      close(reducer_fds[0]);
      close(reducer_fds[1]);
    }

    // Print nonzero subprocess exit codes.
    // wait for all splitter
    int status;
    for (i = 0; i < mapper_count; i++) {
      waitpid(splitter_pids[i], &status, 0);
      if (status) {
        print_nonzero_exit_status(SPLITTER, status);
      }
    }

    // wait for all mappers
    for (i = 0; i < mapper_count; i++) {
      waitpid(mappers_pids[i], &status, 0);
      if (status) {
        print_nonzero_exit_status(mapper_name, status);
      }
    }

    waitpid(reducer_pid, &status, 0);
    if (status) {
      print_nonzero_exit_status(reducer_name, status);
    }

    // Count the number of lines in the output file.
    print_num_lines(output_file);

    free(mappers_fds);
    return 0;
}
