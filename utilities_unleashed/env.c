/**
 * Utilities Unleashed Lab
 * CS 241 - Spring 2019
 Partner: yichiz3, jialong2
 */
 #include <stdio.h>
 #include <stdint.h>
 #include <unistd.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <string.h>
 #include <ctype.h>
 #include "format.h"

int main(int argc, char *argv[]) {
  if (argc < 4) {
    print_env_usage();
  }

  pid_t pid = fork();
  if (pid < 0) { // fork failure
      print_fork_failed();
  } else if (pid > 0) { // I am the parent
      int status;
      waitpid(pid, &status, 0);
      return 0;
  } else {
        int i = 1;
        while ( argv[i] ) {
          // check env or exec
          if (strcmp(argv[i], "--")) {
                char *key = strtok(argv[i], "=");
                char *value = strtok(NULL, "");
                if (value == NULL) print_env_usage();
                // check key value are only number letter underscore
                char *ptr_k = key;
                while (*ptr_k) {
                    if ( !(isalpha(*ptr_k) || isdigit(*ptr_k) || *ptr_k == '_') ) {
                        print_env_usage();
                    }
                    ptr_k++;
                }
                // check value or reference
                if (value[0] != '%') {
                    char *ptr_v = value;
                    while (*ptr_v) {
                        if ( !(isalpha(*ptr_v) || isdigit(*ptr_v) || *ptr_v == '_') ) {
                            print_env_usage();
                        }
                        ptr_v++;
                    }
                } else {
                    value = getenv(value+1);
                    if (!value) {
                        print_environment_change_failed();
                    }
                }
                if (setenv(key, value, 1) < 0)
                    print_environment_change_failed();
          } else {
                    execvp(argv[i+1], argv+i+1);
                    print_exec_failed();
          }
          i++;
        }
        print_env_usage();
  }
  return 0;
}
