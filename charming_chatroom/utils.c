/**
 * Charming Chatroom
 * CS 241 - Spring 2019
 partner; jialong2, qishanz2
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    // Your code here
    // convert from host to network
    size_t nsize = htonl(size);
    return write_all_to_socket(socket, (char*)&nsize, MESSAGE_SIZE_DIGITS);
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    size_t bytes_recd = 0;
    while (bytes_recd < count) {
      ssize_t ret = read(socket, buffer + bytes_recd, count - bytes_recd);
      // if socket disconnected
      if (ret == 0) {
        return 0;
      }
      // continue if signal interrupt
      if (ret == -1 && errno == EINTR) {
        continue;
      }
      // return -1 on failure
      if (ret == -1) {
        return -1;
      }
      bytes_recd += ret;
    }
    // bytes_recd is 0 if socket is disconnected?
    return bytes_recd;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here
    size_t bytes_write = 0;
    while (bytes_write < count) {
      ssize_t ret = write(socket, buffer + bytes_write, count - bytes_write);
      // if socket disconnected
      if (ret == 0) {
        return 0;
      }
      // continue if signal interrupt
      if (ret == -1 && errno == EINTR) {
        continue;
      }
      // return -1 on failure
      if (ret == -1) {
        return -1;
      }
      bytes_write += ret;
    }

    return bytes_write;
}
