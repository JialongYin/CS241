/**
 * Resplendent RPCs
 * CS 241 - Spring 2019
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "common.h"
#include "common.h"
#include "dns_query.h"
#include "dns_query_svc_impl.h"

#define CACHE_FILE "cache_files/rpc_server_cache"

char *contact_nameserver(query *argp, char *host, int port) {
    // Your code here
    // Look in the header file for a few more comments
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }
  int optval = 1;
  // Let them reuse
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
  // Get host by name
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  int result = inet_pton(AF_INET, host, &addr.sin_addr);
  ssize_t bytes = sendto(sockfd, argp->host, strlen(argp->host), 0, (struct sockaddr*)&addr, sizeof(addr));
  char *buf = calloc(1, MAX_BYTES_IPV4);
  // printf("host:%s,inet_pton:%d,%u,sendto:%zu\n",argp->host,result,addr.sin_addr.s_addr,bytes);
  struct sockaddr src_addr;
  socklen_t  addrlen = sizeof(src_addr);
  recvfrom(sockfd, buf, MAX_BYTES_IPV4-1, 0, &src_addr, &addrlen);
  // printf("pass contact_nameserver end\n");
  if (!strcmp(buf, "-1.-1.-1.-1")) {
    return NULL;
  } else {
    return buf;
  }
}

void create_response(query *argp, char *ipv4_address, response *res) {
    // Your code here
    // As before there are comments in the header file
    res = calloc(1, sizeof(response));
    res->address = calloc(1, sizeof(host_address));
    res->address->host = calloc(1, strlen(argp->host)+1);
    strcpy(res->address->host, argp->host);
    if (!ipv4_address) {
      res->success = 0;
      res->address->host_ipv4_address = calloc(1, 1);
    } else {
      res->success = 1;
      res->address->host_ipv4_address = calloc(1, strlen(ipv4_address)+1);
      strcpy(res->address->host_ipv4_address, ipv4_address);
    }
}

// Stub code

response *dns_query_1_svc(query *argp, struct svc_req *rqstp) {
    printf("Resolving query...\n");
    // check its cache, 'rpc_server_cache'
    // if it's in cache, return with response object with the ip address
    char *ipv4_address = check_cache_for_address(argp->host, CACHE_FILE);
    if (ipv4_address == NULL) {
        // not in the cache. contact authoritative servers like a recursive dns
        // server
        printf("Domain not found in server's cache. Contacting authoritative "
               "servers...\n");
        char *host = getenv("NAMESERVER_HOST");
        int port = strtol(getenv("NAMESERVER_PORT"), NULL, 10);
        ipv4_address = contact_nameserver(argp, host, port);
    } else {
        // it is in the server's cache; no need to ask the authoritative
        // servers.
        printf("Domain found in server's cache!\n");
    }

    static response res;
    xdr_free(xdr_response, &res); // Frees old memory in the response struct
    create_response(argp, ipv4_address, &res);

    free(ipv4_address);

    return &res;
}
