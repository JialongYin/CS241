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
int external_command(char *buffer) {
  if (!strncmp(buffer,"cd",2)) {
      int sucess = chdir(buffer+3);
      if (sucess < 0) {
        print_no_directory(buffer+3);
        return 1;
      } else return 0;
  } else{
        fflush(stdout);
        pid_t pid = fork();
        if (pid < 0) { // fork failure
            print_fork_failed();
            exit(1);
        } else if (pid > 0) { // I am the parent
            // printf("Parent pid : %d\n", getpid());
            process *prcss = process_create(buffer, pid);
            vector_push_back(process_l, prcss);
            // printf("background pid : %d\n", pid);
            // background & foreground
            if (buffer[strlen(buffer)-1] == '&') {
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
                    if (!WIFEXITED(status)) return 1; // child not exit successfully
                } else {
                  print_wait_failed();
                  exit(1);
                }
            }
        } else { // I am the child
            // handle & suffix
            if (buffer[strlen(buffer)-1] == '&')
                buffer[strlen(buffer)-1] = '\0';
            // buffer to char *command[]
            vector *command_v = sstring_split(cstr_to_sstring(buffer), ' ');
            char *command[vector_size(command_v)+1];
            for (size_t i = 0; i < vector_size(command_v); i++) {
              command[i] = (char *) vector_get(command_v, i);
            }
            if (!strcmp(command[vector_size(command_v)-1], ""))
              command[vector_size(command_v)-1] = NULL;
            else
              command[vector_size(command_v)] = NULL;
            // printf("Child pid : %d\n", getpid());
            print_command_executed(getpid());
            execvp(command[0], command);
            print_exec_failed(command[0]);
            exit(1);
        }
      }
      return 0;
}
void process_l_destroy() {
    for (size_t i = 0; i < vector_size(process_l); i++) {
      process *prcss = (process *) vector_get(process_l, i);
      free(prcss->command);
      free(prcss);
    }
    vector_destroy(process_l);
}
void killAll(){
  for (size_t i = 0; i < vector_size(process_l); i++) {
      process *prcss = (process *) vector_get(process_l, i);
      kill(prcss->pid, SIGKILL);
  }
  process_l_destroy();
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
    // load history
    FILE *file_h;
    if (hit_name) {
        file_h = fopen(hit_name, "r");
        // if (!file_h) {
        //     print_history_file_error();
        //     exit(1);
        // }
        if (file_h){
            char *buffer_h = NULL;
            size_t size_h = 0;
            ssize_t bytes_read_h;
            while (1) {
              bytes_read_h = getline(&buffer_h,&size_h, file_h);
              if (bytes_read_h == -1) break;
              if (bytes_read_h>0 && buffer_h[bytes_read_h-1] == '\n') {
                buffer_h[bytes_read_h-1] = '\0';
                vector_push_back(history, buffer_h);
              }
            }
            free(buffer_h);
            fclose(file_h);
        }
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
      char *full_path = get_full_path("./");
      print_prompt(full_path, getpid());
      free(full_path);
      // command input
      bytes_read = getline(&buffer,&size, file);
      if (bytes_read == -1){
        killAll(); break;
      }
      if (bytes_read>0 && buffer[bytes_read-1] == '\n') {
        buffer[bytes_read-1] = '\0';
        if (file != stdin) print_command(buffer);
      }
      // int command();
      // built-in command
      if (!strcmp(buffer,"!history")) {
          for (size_t i = 0; i < vector_size(history); i++) {
            print_history_line(i, (char *) vector_get(history, i));
          }
      } else if (buffer[0] == '#') {
          size_t num,num_read;
          num_read = sscanf(buffer+1, "%zu", &num);
          if (!num_read || num > vector_size(history)-1) {
            print_invalid_index();
          } else {
            char *his_cmd = (char *)vector_get(history, num);
            print_command(his_cmd);
            vector_push_back(history, his_cmd);
            external_command(his_cmd);
          }
      } else if (buffer[0] == '!') {
          for (int i = vector_size(history) - 1; i >= 0 ; i--) {
            char *his_cmd = (char *)vector_get(history, i);
            if (buffer[1] == '\0' || !strncmp(buffer+1, his_cmd, strlen(buffer+1))) {
              print_command(his_cmd);
              vector_push_back(history, his_cmd);
              external_command(his_cmd);
              break;
            }
            if (i == 0) print_no_history_match();
          }
      } else if (!strcmp(buffer,"exit")) {
          killAll(); break;
      } else {
          // logical operators
          int op_flag = 0;
          sstring *sstr = cstr_to_sstring(buffer);
          vector *parse = sstring_split(sstr, ' ');
          for (size_t i = 0; i < vector_size(parse); i++) {
              char *op = (char *) vector_get(parse, i);
              if (!strcmp(op, "&&")) {
                  char *buffer1 = strtok(buffer, "&");
                  buffer1[strlen(buffer1)-1] = '\0';
                  char *buffer2 = strtok(NULL, "");
                  buffer2 = buffer2+2;
                  vector_push_back(history, buffer1);
                  if(!external_command(buffer1)) {
                    vector_push_back(history, buffer2);
                    external_command(buffer2);
                  }
                  op_flag = 1;
              } else if (!strcmp(op, "||")) {
                  char *buffer1 = strtok(buffer, "|");
                  buffer1[strlen(buffer1)-1] = '\0';
                  char *buffer2 = strtok(NULL, "");
                  buffer2 = buffer2+2;
                  vector_push_back(history, buffer1);
                  if(external_command(buffer1)) {
                    vector_push_back(history, buffer2);
                    external_command(buffer2);
                  }
                  op_flag = 1;
              } else if (op[strlen(op)-1] == ';') {
                  char *buffer1 = strtok(buffer, ";");
                  char *buffer2 = strtok(NULL, "");
                  buffer2 = buffer2+1;
                  vector_push_back(history, buffer1);
                  external_command(buffer1);
                  vector_push_back(history, buffer2);
                  external_command(buffer2);
                  op_flag = 1;
              }
          }
          vector_destroy(parse);
          sstring_destroy(sstr);
          if (!op_flag) {
            // built-in cd && external command
            vector_push_back(history, buffer);
            external_command(buffer);
          }

        }
    }
    free(buffer);
    if (cmd_name) {
      fclose(file);
    }
    // write to history
    if (hit_name) {
      FILE* f = fopen(get_full_path(hit_name), "w");
      VECTOR_FOR_EACH(history, line, {
        fprintf(f, "%s\n", (char *)line);
      });
      fclose(f);
    }
    vector_destroy(history);
    return 0;
}
