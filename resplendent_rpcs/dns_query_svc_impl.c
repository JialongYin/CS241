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
    return NULL;
}

void create_response(query *argp, char *ipv4_address, response *res) {
    // Your code here
    // As before there are comments in the header file
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
