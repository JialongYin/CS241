/**
 * Shell Lab
 * CS 241 - Spring 2019
 */

#include "format.h"
#include "shell.h"
#include "vector.h"
#include "sstring.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>





typedef struct process {
    char *command;
    pid_t pid;
} process;

static vector *process_l;
process* process_create(const char *buffer, pid_t pid) {
  process *prcss = calloc(1, sizeof(process));
  prcss->command = calloc(strlen(buffer)+1, sizeof(char));
  strcpy(prcss->command, buffer);
  prcss->pid = pid;
  return prcss;
}
void process_destroy(pid_t pid) {
  for (size_t i = 0; i < vector_size(process_l); i++) {
    process *prcss = (process *) vector_get(process_l, i);
    if ( prcss->pid == pid ) {
      free(prcss->command);
      free(prcss);
      vector_erase(process_l, i);
      return;
    }
  }
}
void ctrlcpressed() {
  for (size_t i = 0; i < vector_size(process_l); i++) {
    process *prcss = (process *) vector_get(process_l, i);
    if ( prcss->pid != getpgid(prcss->pid) ){
      kill(prcss->pid, SIGKILL);
      process_destroy(prcss->pid);
    }
  }
}
void backgrndEnd() {
  pid_t pid;
  while ( (pid = waitpid(-1, 0, WNOHANG)) > 0) {
    process_destroy(pid);
    // printf("finished child process: %d\n", pid);
  }
}

int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
    signal(SIGINT, ctrlcpressed);
    signal(SIGCHLD, backgrndEnd);
    process_l = shallow_vector_create();
    vector *history = string_vector_create();
    // -h -f file argv & check argc
    int count = 1;
    int opt;
    char* hit_name = NULL;
    char* cmd_name = NULL;
    while ((opt = getopt(argc, argv, "h:f:")) != -1) {
      switch(opt){
        case 'h': hit_name = optarg; count+=2; break;
        case 'f': cmd_name = optarg; count+=2; break;
      }
    }
    if (argc != count) {
      print_usage();
      exit(1);
    }
    // open file or stdin
    FILE *file;
    if (cmd_name) {
        file = fopen(cmd_name, "r");
        if (!file) {
            print_script_file_error();
            exit(1);
        }
    } else file = stdin;
    // shell loop
    char *buffer = NULL;
    size_t size = 0;
    ssize_t bytes_read;
    while (1) {
      print_prompt(getenv("PATH"), getpid());
      // command input
      bytes_read = getline(&buffer,&size, file);
      // printf("bytes_read : %zd ", bytes_read);
      if (bytes_read == -1) break;
      if (bytes_read>0 && buffer[bytes_read-1] == '\n') {
        buffer[bytes_read-1] = '\0';
        if (file != stdin) print_command(buffer);
      }
      // buffer to char *command[]
      vector *command_v = sstring_split(cstr_to_sstring(buffer), ' ');
      char *command[vector_size(command_v)+1];
      for (size_t i = 0; i < vector_size(command_v); i++) {
        command[i] = (char *) vector_get(command_v, i);
      }
      char *str = command[vector_size(command_v)-1];
      if ( !strcmp(str, "&") ) {
        command[vector_size(command_v)-1] = NULL;
      } else if (str[strlen(str)-1] == '&') {
        str[strlen(str)-1] = '\0';
        command[vector_size(command_v)] = NULL;
      } else {
        command[vector_size(command_v)] = NULL;
      }
      // built-in command
      if (!strcmp(command[0],"cd")) {
        int sucess = chdir(command[1]);
        if (sucess < 0) {
          print_no_directory(command[1]);
        }
      }
      if (!strcmp(command[0],"exit")) return 0;
      vector_push_back(history, buffer);
      // external command
      fflush(stdout);
      pid_t pid = fork();
      if (pid < 0) { // fork failure
          print_fork_failed();
          exit(1);
      } else if (pid > 0) { // I am the parent
          // printf("Parent pid : %d\n", getpid());
          process *prcss = process_create(buffer, pid);
          vector_push_back(process_l, prcss);
          // background & foreground
          if (buffer[bytes_read-2] == '&') {
              if (setpgid(pid, pid) == -1) {
                  print_setpgid_failed();
                  exit(1);
              }
          } else {
              if (setpgid(pid, getpid()) == -1) {
                  print_setpgid_failed();
                  exit(1);
              }
              int status;
              pid_t result_pid = waitpid(pid, &status, 0);
              if (result_pid != -1) {
                  // printf("Already wait : %d\n", result_pid);
                  process_destroy(result_pid);
              } else {
                print_wait_failed();
                exit(1);
              }
          }
      } else { // I am the child
          // printf("Child pid : %d\n", getpid());


          print_command_executed(getpid());
          execvp(command[0], command);
          print_exec_failed(command[0]);
          exit(1);
      }
    }
    free(buffer);
    if (cmd_name) {
      fclose(file);
    }
    return 0;
}
