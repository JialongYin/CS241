/**
 * Utilities Unleashed Lab
 * CS 241 - Spring 2019
 */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "format.h"

#define BILLION  1000000000L

int main(int argc, char *argv[]) {
  if (argc < 2) {
    print_time_usage();
  }

  pid_t pid = fork();
  if (pid < 0) { // fork failure
    print_fork_failed();
  } else if (pid > 0) { // I am the parent
    struct timespec start, end;
    double duration;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int status;
    pid_t result_pid = waitpid(pid, &status, 0);

    clock_gettime(CLOCK_MONOTONIC, &end);

    if (result_pid != -1 && WIFEXITED(status)) {
      duration = (end.tv_sec - start.tv_sec) + (double) (end.tv_nsec - start.tv_nsec) / (double) BILLION;
      display_results(argv, duration);
    }
  } else { // I am the child
    execvp(argv[1], argv+1);
    print_exec_failed();
  }

  return 0;
}
