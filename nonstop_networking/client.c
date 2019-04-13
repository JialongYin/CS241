/**
 * Nonstop Networking
 * CS 241 - Spring 2019
 */
#include "common.h"
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
static const size_t MESSAGE_SIZE_DIGITS = 8;
char **parse_args(int argc, char **argv);
verb check_args(char **args);
int connect_to_server(char **args);
void write_cmd(char **args);
ssize_t write_to_socket(int socket, const char *buffer, size_t count);
void read_response(char **args, int socket);
ssize_t read_from_socket(int socket, char *buffer, size_t count);
static int serverSocket;
int main(int argc, char **argv) {
    // Good luck!
    char **args = parse_args(argc, argv);
    check_args(args); // {host, port, method, remote, local, NULL}
    serverSocket = connect_to_server(args);
    write_cmd(args);
    if (shutdown(serverSocket, SHUT_WR) != 0) {
        perror("shutdown()");
    }
    read_response(args, serverSocket);
    shutdown(serverSocket, SHUT_RD);
    close(serverSocket);
    free(args);
}
void read_response(char **args, int socket) {
  char *buffer = calloc(1, strlen("OK\n")+1);
  size_t bytes_rd = read_from_socket(socket, buffer, strlen("OK\n"));
  if (strcmp(buffer, "OK\n") == 0) {
    fprintf(stdout, "%s", buffer);
    // OK response content
    char *command = args[2];
    if (strcmp(command, "DELETE") == 0 || strcmp(command, "PUT") == 0) {
      print_success();
    } else if (strcmp(command, "GET") == 0) {
      unsigned long long size;
      read_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
      char *buffer_f = calloc(1, size);
      read_from_socket(socket, buffer_f, size);
      fprintf(stdout, "%llu%s", size, buffer_f);
      // handle open file
    }
    return;
  }
  buffer = realloc(buffer, strlen("ERROR\n")+1);
  read_from_socket(socket, buffer+bytes_rd, strlen("ERROR\n")-bytes_rd);
  if (strcmp(buffer, "ERROR") == 0) {
    fprintf(stdout, "%s", buffer);
    char *err = calloc(1, 20);
    read_from_socket(socket, err, 20);
    print_error_message(err);
  } else {
    print_invalid_response();
  }
}
ssize_t read_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    size_t bytes_recd = 0;
    while (bytes_recd < count) {
      ssize_t ret = read(socket, buffer + bytes_recd, count - bytes_recd);
      // if socket disconnected
      if (ret == 0) {
        break;
      }
      // continue if signal interrupt
      if (ret == -1 && errno == EINTR) {
        continue;
      }
      // return -1 on failure
      if (ret == -1) {
        print_invalid_response();
        exit(1);
      }
      bytes_recd += ret;
    }
    if (bytes_recd == 0) {
      print_connection_closed();
      exit(1);
    }
    return bytes_recd;
}
ssize_t write_to_socket(int socket, const char *buffer, size_t count) {
    size_t bytes_write = 0;
    while (bytes_write < count) {
      ssize_t ret = write(socket, buffer + bytes_write, count - bytes_write);
      // if socket disconnected
      if (ret == 0) {
        break;
      }
      // continue if signal interrupt
      if (ret == -1 && errno == EINTR) {
        continue;
      }
      // return -1 on failure
      if (ret == -1) {
        fprintf(stderr, "write fail\n");
        exit(1);
      }
      bytes_write += ret;
    }
    if (bytes_write < count) {
      print_connection_closed();
      exit(1);
    }
    return bytes_write;
}
void write_cmd(char **args) {
  // code here
  char *command = args[2];
  char *msg;
  if (strcmp(command, "LIST") == 0) {
    msg = calloc(1, strlen(command)+2);
    sprintf(msg, "%s\n", command);
  } else {
    msg = calloc(1, strlen(command)+strlen(args[3])+3);
    sprintf(msg, "%s %s\n", command, args[3]);
  }
  ssize_t len = strlen(msg) + 1;
  LOG("msg: %s\n", msg);
  write_to_socket(serverSocket, msg, len);
  free(msg);
  if (strcmp(command, "PUT") == 0) {
    // read file and get size
    struct stat buf;
    if(stat(args[4], &buf) == -1)
      exit(1);
    unsigned long long size = buf.st_size;
    LOG("size of file 1: %llu\n", size);
    write_to_socket(serverSocket, (char*)&size, MESSAGE_SIZE_DIGITS);
    FILE *local = fopen(args[4], "r");
    if (!local) {
      fprintf(stderr, "local file open fail\n");
      exit(1);
    }
    size_t size_hd = size > 1024 ? 1024 : size;
    char *buffer = calloc(1, size_hd+1);
    fread(buffer, 1, size_hd, local);
    write_to_socket(serverSocket, buffer, size_hd);
    fclose(local);
  }
}
int connect_to_server(char **args) {
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int s = getaddrinfo(args[0], args[1], &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(1);
  }
  int sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (sock_fd == -1) {
    perror(NULL);
    exit(1);
  }
  int ok = connect(sock_fd, result->ai_addr, result->ai_addrlen);
  if (ok == -1) {
    perror(NULL);
    exit(1);
  }
  freeaddrinfo(result);
  return sock_fd;
}
/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
