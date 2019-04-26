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
char **parse_args(int argc, char **argv);
verb check_args(char **args);

int connect_to_server(char **args);
void write_cmd(char **args, int socket, verb method);
void read_response(char **args, int socket, verb method);

int main(int argc, char **argv) {
    // Good luck!
    char **args = parse_args(argc, argv);
    verb method = check_args(args); // {host, port, method, remote, local, NULL}

    int serverSocket = connect_to_server(args);
    write_cmd(args, serverSocket, method);
    if (shutdown(serverSocket, SHUT_WR) != 0) {
        perror("shutdown()");
    }
    read_response(args, serverSocket, method);
    shutdown(serverSocket, SHUT_RD);
    close(serverSocket);
    free(args);
}
void read_response(char **args, int socket, verb method) {
  char *buffer = calloc(1,strlen("OK\n")+1);
  size_t bytes_rd = read_from_socket(socket, buffer, strlen("OK\n"));
  if (strcmp(buffer, "OK\n") == 0) {
    fprintf(stdout, "%s", buffer);
    if (method == DELETE || method == PUT) {
      print_success();
    } else if (method == GET) {
      FILE *local = fopen(args[4], "a+");
      if (!local) {
        perror(NULL);
        exit(1);
      }
      size_t size;
      read_from_socket(socket, (char *)&size, sizeof(size_t));
      // fprintf(stdout, "%zu", size);
      size_t bytes_read = 0;
      while (bytes_read < size+5) {
        size_t size_hd = (size+5-bytes_read) > 1024 ? 1024 : (size+5-bytes_read);
        char buffer_f[1024+1] = {0};
        size_t rc = read_from_socket(socket, buffer_f, size_hd);
        fwrite(buffer_f, 1, rc, local);
        // fprintf(stdout, "%s", buffer_f);
        bytes_read += rc;
        if (rc == 0)
          break;
      }
      if (print_any_err(bytes_read, size)) exit(1);
      fclose(local);
    } else if (method == LIST) {
      size_t size;
      read_from_socket(socket, (char *)&size, sizeof(size_t));
      char buffer_f[size+5+1];
      memset(buffer_f, 0, size+5+1);
      bytes_rd = read_from_socket(socket, buffer_f, size+5);
      if (print_any_err(bytes_rd, size)) exit(1);
      fprintf(stdout, "%zu%s", size, buffer_f);
    }
  } else {
    buffer = realloc(buffer, strlen("ERROR\n")+1);
    read_from_socket(socket, buffer+bytes_rd, strlen("ERROR\n")-bytes_rd);
    if (strcmp(buffer, "ERROR\n") == 0) {
      fprintf(stdout, "%s", buffer);
      char err[20] = {0};
      if (!read_from_socket(socket, err, 20))
        print_connection_closed();
      print_error_message(err);
    } else {
      print_invalid_response();
    }
  }
  free(buffer);
}

void write_cmd(char **args, int socket, verb method) {
  char *msg;
  if (method == LIST) {
    msg = calloc(1, strlen(args[2])+2);
    sprintf(msg, "%s\n", args[2]);
  } else {
    msg = calloc(1, strlen(args[2])+strlen(args[3])+3);
    sprintf(msg, "%s %s\n", args[2], args[3]);
  }
  ssize_t len = strlen(msg);
  if (write_to_socket(socket, msg, len) < len) {
    print_connection_closed();
    exit(1);
  }
  free(msg);

  if (method == PUT) {
    struct stat buf;
    if(stat(args[4], &buf) == -1)
      exit(1);
    size_t size = buf.st_size;
    write_to_socket(socket, (char*)&size, sizeof(size_t));
    // LOG("client write_cmd size:%zu", size);
    FILE *local = fopen(args[4], "r");
    if (!local) {
      fprintf(stdout, "local file open fail\n");
      exit(1);
    }
    size_t bytes_write = 0;
    while (bytes_write < size) {
      ssize_t size_hd = (size-bytes_write) > 1024 ? 1024 : (size-bytes_write);
      char buffer[size_hd+1];
      fread(buffer, 1, size_hd, local);
      if (write_to_socket(socket, buffer, size_hd) < size_hd) {
        print_connection_closed();
        exit(1);
      }
      bytes_write += size_hd;
    }
    // LOG("client write_cmd bytes_write:%zu", bytes_write);
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
