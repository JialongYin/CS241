/**
 * Nonstop Networking
 * CS 241 - Spring 2019
 */
#include "common.h"
int print_any_err(size_t bytes_rd, size_t size) {
  if (bytes_rd == 0 && bytes_rd != size) {
    print_connection_closed();
    return 1;
  } else if (bytes_rd < size) {
    print_too_little_data();
    return 1;
  } else if (bytes_rd > size) {
    print_received_too_much_data();
    return 1;
  }
  return 0;
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
        perror("read_from_socket()");
        return -1;
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
        return -1;
      }
      bytes_write += ret;
    }
    return bytes_write;
}
