/**
 * Nonstop Networking
 * CS 241 - Spring 2019
 */
#include "common.h"
#include "format.h"
#include "./includes/dictionary.h"
#include "./includes/vector.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX_CLIENTS 100
#define MAX_EVENTS 100
#define HEADER_SIZE 1024
#define FILENAME_SIZE 255
typedef struct client_info {
	int state;
	verb cmd;
	char filename[FILENAME_SIZE];
	char header[HEADER_SIZE];
} client_info;

void close_server();
void cleanup();
void run_server(char *port);
static int epollfd;
static dictionary  *clients_dict;
static char *dir_name;
static vector *file_list;
int main(int argc, char **argv) {
	// good luck!
	signal(SIGPIPE, handle_sigpipe);
	if (argc != 2) {
			print_server_usage();
			exit(1);
	}
	struct sigaction act;
	memset(&act, '\0', sizeof(act));
	act.sa_handler = close_server;
	if (sigaction(SIGINT, &act, NULL) < 0) {
			perror("sigaction");
			exit(1);
	}
	clients_dict = int_to_shallow_dictionary_create();
	char template[] = "XXXXXX";
  dir_name = mkdtemp(template);
  print_temp_directory(dir_name);
	file_list = string_vector_create();
	run_server(argv[1]);
	cleanup();
}
void run_server(char *port) {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;     // passive server
	int s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(1);
	}
	int optval = 1;
	if ( setsockopt(serverSocket, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof(optval)) {
		perror(NULL);
		exit(1);
	}
	if ( bind(serverSocket, result->ai_addr, result->ai_addrlen) != 0 ) {
		perror(NULL);
		exit(1);
	}
	if ( listen(serverSocket, MAX_CLIENTS) != 0 ) { // Start accepting connections. kernel would handle the handshake. We can specify the number of backlog connection.
		perror(NULL);
		exit(1);
	}
	freeaddrinfo(result);
	epollfd = epoll_create(42);
	struct epoll_event ev = {.events = EPOLLIN, .data.fd = serverSocket};
	epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &ev);
	struct epoll_event array[MAX_EVENTS];
	while (!endSession) {
		int num_events = epoll_wait(epollfd, array, MAX_EVENTS, 1000);
		if (num_events == -1) {
			// handle close server, clean up
			exit(1);
		}
		if (num_events == 0) continue;
		for (int i = 0; i < num_events; i++) {
			if (array[i].data.fd == serverSocket) {
				int client_fd = accept(serverSocket, NULL, NULL);
				if (client_fd < 0) {
						perror(NULL);
						exit(1);
				}
				struct epoll_event ev_c = {.events = EPOLLIN, .data.fd = client_fd};
				epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev_c);
				client_info *info = calloc(1, sizeof(client_info));
        dictionary_set(clients_dict, &client_fd, info);
			} else {
				process_client(array[i].data.fd);
			}
		}
	}
}
void *process_client(int client_fd) {
	client_info *info = dictionary_get(clients_dict, &client_fd);
	if (info->state == 0) {
		read_header(client_fd, info);
	} else if (info->state == 1) {
		process_cmd(client_fd, info);
	} else {
		err_handler(client_fd, info);
	}
}
void *err_handler(int client_fd, client_info *info) {
	// write ERROR
	if (info->state == -1) {
		err_bad_request;
	} else if (info->state == -2) {
		err_bad_file_size
	} else if (info->state == -3) {
		err_no_such_file
	}
	// clean up client
}
int state;
verb cmd;
char filename[FILENAME_SIZE];
char header[HEADER_SIZE];
void *process_cmd(int client_fd, client_info *info) {
	if (info->cmd == GET) {

	} else if (info->cmd == PUT) {

	} else if (info->cmd == DELETE) {

	} else if (info->cmd == LIST) {

	}
}
int read_body(int client_fd, client_info *info) {
	int len = strlen(dir_name) + 1 + strlen(info->filename) + 1;
	char path[len] = {0};
	sprintf(path, "%s/%s", g_dir_name, info->filename);
	FILE *remote = fopen(path, "w");
	if (!remote) {
		perror(NULL);
		return 1;
	}
	size_t size;
	read_from_socket(client_fd, (char *)&size, sizeof(size_t));
	size_t bytes_read = 0;
	while (bytes_read < size+5) {
		size_t size_hd = (size+5-bytes_read) > 1024 ? 1024 : (size+5-bytes_read);
		char buffer_f[1024+1] = {0};
		size_t rc = read_from_socket(client_fd, buffer_f, size_hd);
		fwrite(buffer_f, 1, rc, remote);
		bytes_read += rc;
		if (rc == 0)
			break;
	}
	fclose(remote);
	if (print_any_err(bytes_read, size)) {
		remove(path);
		return 1;
	}
	vector_push_back(file_list, info->filename);
	return 0;
}
void epollout_client(int client_fd) {
	struct epoll_event ev_c = {.events = EPOLLOUT, .data.fd = client_fd};
	epoll_ctl(epollfd, EPOLL_CTL_MOD, client_fd, &ev_c);
}
void read_header(int client_fd, client_info *info) {
	read_from_socket(client_fd, info->header, HEADER_SIZE);
	if (strncmp(info->header, "GET", 3)) {
		info->cmd = GET;
		strcpy(info->filename, info->header+4);
		info->filename[strlen(info->filename)-1] = '\0';
	} else if (strncmp(info->header, "PUT", 3)) {
		info->cmd = PUT;
		strcpy(info->filename, info->header+4);
		info->filename[strlen(info->filename)-1] = '\0';
		if (read_body(client_fd, info)) {
			info->state = -2;
			epollout_client(client_fd);
			return;
		}
	} else if (strncmp(info->header, "DELETE", 6)) {
		info->cmd = DELETE;
		strcpy(info->filename, info->header+7);
		info->filename[strlen(info->filename)-1] = '\0';
	} else if (strncmp(info->header, "LIST", 4)) {
		info->cmd = LIST;
	} else {
		print_invalid_response();
		info->state = -1;
		epollout_client(client_fd);
		return;
	}
	info->state = 1;
	epollout_client(client_fd);
}
void close_server() {
}
void cleanup() {
}
