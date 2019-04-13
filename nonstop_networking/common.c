/**
 * Nonstop Networking
 * CS 241 - Spring 2019
 */
#include "common.h"

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
    // handle print_connection_closed if bytes_recd == 0
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
