// /**
//  * Nonstop Networking
//  * CS 241 - Spring 2019
//  */
// #include <stdio.h>
// int main(int argc, char **argv) {
//     // good luck!
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "vector.h"
#include "format.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>

static char* port;
static struct addrinfo* result;
static vector* file_vec;
static size_t file_name_bytes;
static char* temp_dir;

void termination_handler(int signum) {
	if (signum == SIGINT) {
		size_t i;
		for (i = 0; i < vector_size(file_vec); i++) {
			char* this_str = vector_get(file_vec, i);
			char file_to_del[1024]; memset(file_to_del, 0, 1024);
			strcat(file_to_del, "./");
			strcat(file_to_del, this_str);
			remove(file_to_del);
		}
		char tp[1024]; getcwd(tp, 1024);
		chdir("./..");
		getcwd(tp, 1024);
		//char file_to_del[1024]; memset(file_to_del, 0, 1024);
		//fprintf(stderr, "UP\n");
		if (rmdir(tp) < 0) {
			fprintf(stderr ,"rmdir fail\n");
		}
		vector_destroy(file_vec);
		freeaddrinfo(result);
		exit(0);
	}

}


void change_size_to_array(unsigned char* arr, size_t size) {
	//printf("size is %lx\n", size);
	int i;
	for (i = 0; i < 8; i++) {
		arr[i] = 0xFF & size;
		size = size >> 8;
		//fprintf(stderr, "%hhx\n",arr[i]);
	}
}

size_t change_arr_to_size(char* start_of_size) {
	unsigned char* cp = (unsigned char*)start_of_size;
	size_t size = 0;
	for (int i = 7; i >= 0; i--) {
		//fprintf(stderr, "%x\n",start_of_size[i]  );
		size_t a = cp[i] << (8 * i);
		size += a;
	}
	return size;
}

void accept_connections(struct epoll_event *e,int epoll_fd) {
	while(1) {
		struct sockaddr_in new_addr;
		socklen_t new_len = sizeof(new_addr);
		int new_fd = accept(e->data.fd, (struct sockaddr*) &new_addr, &new_len);

		if(new_fd == -1) {
			// All pending connections handled
			if(errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			else {
				perror("accept");
				exit(1);
			}
		}

		char* connected_ip = inet_ntoa(new_addr.sin_addr);
		//int port = ntohs(new_addr.sin_port);
        printf("Accepted Connection %s port %s\n", connected_ip, port);

        int flags = fcntl(new_fd, F_GETFL, 0);
        fcntl(new_fd, F_SETFL, flags | O_NONBLOCK);

        //Connection to epoll
        struct epoll_event event;
        event.data.fd = new_fd;
        event.events = EPOLLIN | EPOLLET;
        if(epoll_ctl (epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1) {
        	perror("accept epoll_ctl");
        	exit(1);
        }
	}
}
/*
0 is err input
1 is get
2 is put
3 is list
4 is delete
*/
int fenxi_verb(char* str) {
	if (strstr(str, "GET") == str) return 1;
	if (strstr(str, "PUT") == str) return 2;
	if (strstr(str, "LIST") == str) return 3;
	if (strstr(str, "DELETE") == str) return 4;
	return 0;
}

/*
0 is bad req
1 is get
2 is put
3 is list
4 is delete
10 is err_bad_file_size
100 is err_no_such_file
*/

static char buff[1024*1000];

void handle_data(struct epoll_event *e) {
	ssize_t count;
	//char buff[4096];
	size_t total_bytes_read = 0;
	int finish = 0;
	int full = 0;
	int op = 0;
	char* buff_ptr = buff;

	while(1) {
		//int err_sav = errno;
		count = read(e->data.fd, buff_ptr, 1024*250 - total_bytes_read);
		//fprintf(stderr, "first read is %ld\n", count);
		if (count > 0) {
			total_bytes_read += count;
			buff_ptr = buff_ptr + count;
			if (count >= 1024) {
				full = 1;
				break;
			}
		}
		/*if (count > 0) {
			total_bytes_read += count;
			if (count == 4096) {
				// fprintf(stderr, "First buffer is full\n");
				full = 1;
				break;
			} else {
 				buff_ptr = buff_ptr + count;
			}
		} */
		// fprintf(stderr, "count now is %ld\n", count);

		if(count == -1) {
			if(errno != EAGAIN) {
			//err exit
				perror("read error");
				printf("Closed connection on descriptor %d with error\n", e->data.fd);
				//close(e->data.fd);
				break;
			}
			//errno = err_sav;
		} else if( count == 0) {
			//clean exit
			finish = 1;
			// printf("Closed connection on descriptor %d\n", e->data.fd);
			//close(e->data.fd);
			//errno = err_sav;
			break;
		}
	}

	if (total_bytes_read == 5) {
		buff[4] = 0;
		if (strcmp(buff, "LIST") == 0) {
			op = 3;
		} else {
			op = 0;
		}
 	} else {
 		op = fenxi_verb(buff);
 	}


	if (op == 0) {
		print_invalid_response();
		dprintf(e->data.fd, "ERROR\n%s\n", err_bad_request);
	} else if (op == 3) {
		//LIST
		//fprintf(stderr, "IN op3\n");

		unsigned char size_arr[8];
		change_size_to_array(size_arr, file_name_bytes + vector_size(file_vec) - 1);
		dprintf(e->data.fd, "OK\n");
		write(e->data.fd, size_arr, 8);
		for (size_t i = 0; i < vector_size(file_vec); i++) {
			char* this_str = vector_get(file_vec, i);
			if (i == vector_size(file_vec) - 1) {
				// final element, no \n
				write(e->data.fd, this_str, strlen(this_str));
			} else {
				write(e->data.fd, this_str, strlen(this_str));
				char t = '\n';
				write(e->data.fd, &t, 1);
			}
		}
		close(e->data.fd);
	} else if (op == 1) {
		// get
		char* blank = strstr(buff, " ");
		char* huanhang = strstr(buff, "\n");
		char file_name[1024]; memset(file_name, 0, 1024);
		*huanhang = 0;
		strcpy(file_name, blank+ 1 );
		*huanhang = '\n';
		int find_file = 0;
		for (size_t i = 0; i < vector_size(file_vec); i++) {
			if (strcmp(file_name, vector_get(file_vec, i)) == 0) {
				find_file = 1;
				break;
			}
		}
		if (!find_file) {
			dprintf(e->data.fd, "ERROR\n%s\n", err_no_such_file);
		} else {
			int local_fd = open(file_name, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
			dprintf(e->data.fd, "OK\n");
			struct stat st;
			stat(file_name, &st);
			size_t size = st.st_size;
			unsigned char size_arr[8];
			change_size_to_array(size_arr, size);
			write(e->data.fd,size_arr, 8);
			char* file_arr = mmap(NULL, size, PROT_READ, MAP_FILE | MAP_SHARED, local_fd, 0);
			write(e->data.fd, file_arr, size);
			close(local_fd);
		}

		close(e->data.fd);
	} else if (op == 2) {
		// put
		char* huanhang = strstr(buff, "\n");
		char* start_of_size = huanhang + 1;
		for (int i = 0; i < 8; i++) {
			//fprintf(stderr, "%hhx\n", start_of_size[i] );
		}
		size_t size_of_file_uploaded = change_arr_to_size(start_of_size);
		fprintf(stderr, "size of uploaded file is %zu\n",size_of_file_uploaded);

		char file_name[1024]; memset(file_name, 0, 1024);
		char* blank = strstr(buff, " ");
		*huanhang = 0;
		strcpy(file_name, blank + 1);
		*huanhang = '\n'; // get file name;
		if (file_name_bytes == 0) {
			// no file uploaded
			char template[] = "XXXXXX";
			temp_dir = mkdtemp(template);
			print_temp_directory(temp_dir);
			chdir(temp_dir);
			//fprintf(stderr, "now change dir %s\n", temp_dir);
		}

		int local_fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
		char* content_start = start_of_size + 8;
	//	fprintf(stderr, "total_bytes_read now is %zu\n", total_bytes_read);
		ssize_t real_size_file_now = total_bytes_read - (content_start - buff);
		ssize_t wr_bytes = real_size_file_now;
		while (1) {
			if (wr_bytes <= 0) break;
			if (wr_bytes <= 1024) {
				lseek(local_fd, 0, SEEK_END);
				write(local_fd, content_start, wr_bytes);
				//write(1, content_start, wr_bytes);
				wr_bytes = 0;
			} else {
				lseek(local_fd, 0, SEEK_END);
				write(local_fd, content_start, 1024);
				//write(1, content_start, 1024);
				content_start += 1024;
				wr_bytes -= 1024;
			}
		}
		if (full) {
			//fprintf(stderr, "is full here\n");
			while (1) {
				char loc_buff[4096 * 100];
				int tp = read(e->data.fd, loc_buff, 4096 * 100);
				if (tp > 0) {
					lseek(local_fd, 0, SEEK_END);
					write(local_fd, loc_buff, tp);
					real_size_file_now += tp;
				} else if (tp == 0) {
					break;
				}
			}
		}
		// fprintf(stderr, "real_size_file_now is %ld\n",real_size_file_now);
		if ((size_t)real_size_file_now > size_of_file_uploaded) {
			print_received_too_much_data();
			dprintf(e->data.fd,"ERROR\n%s\n",err_bad_file_size);
		} else if ((size_t)real_size_file_now < size_of_file_uploaded) {
			print_too_little_data();
			dprintf(e->data.fd,"ERROR\n%s\n",err_bad_file_size);
		} else {
			dprintf(e->data.fd,"OK\n");
		}
		vector_push_back(file_vec, file_name);
		file_name_bytes += strlen(file_name);
		close(local_fd);
		close(e->data.fd);
	} else if (op == 4) {
		//delete
		char* blank = strstr(buff, " ");
		char* huanhang = strstr(buff, "\n");
		char file_name[1024]; memset(file_name, 0, 1024);
		*huanhang = 0;
		strcpy(file_name, blank + 1);
		*huanhang = '\n';
		//clear vector
		size_t i, success = 0;
		for (i = 0; i < vector_size(file_vec); i++) {
			char* this_str = vector_get(file_vec, i);
			if (strcmp(this_str, file_name) == 0) {
				vector_erase(file_vec, i);
				file_name_bytes -= strlen(file_name);
				char file_to_del[1024]; memset(file_to_del, 0, 1024);
				strcat(file_to_del, "./");
				strcat(file_to_del, file_name);
				int rm = unlink(file_to_del);
				if (rm < 0) fprintf(stderr, "rm failed\n");
				success = 1;
				break;
			}
		}
		if (!success) {
			// cannot find
			dprintf(e->data.fd, "ERROR\n%s\n", err_no_such_file);
		} else {
			dprintf(e->data.fd, "OK\n");
		}
		close(e->data.fd);
	}

}



int main(int argc, char **argv) {
    // good luck!
    if (argc != 2) exit(1);
	port = argv[1];
	file_vec = string_vector_create(); // free

	struct sigaction psa;
	psa.sa_handler = termination_handler;
	sigaction(SIGINT, &psa, NULL);

	int s;
	//struct epoll_event *events;

    // Create the socket as a nonblocking socket
    int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, port , &hints, &result); // free
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    int optval = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if ( bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0 )
    {
        perror("bind()");
        exit(1);
    }

    if ( listen(sock_fd, 10) != 0 )
    {
        perror("listen()");
        exit(1);
    }

    struct sockaddr_in sin;
    socklen_t socklen = sizeof(sin);
    if (getsockname(sock_fd, (struct sockaddr *)&sin, &socklen) == -1)
      perror("getsockname");
    else
      printf("Listening on port number %d\n", ntohs(sin.sin_port));

	//setup epoll
	int epoll_fd = epoll_create(1);
	if(epoll_fd == -1)
    {
        perror("epoll_create()");
        exit(1);
    }

	struct epoll_event event;
	event.data.fd = sock_fd;
	event.events = EPOLLIN | EPOLLET;

	//Add the sever socket to the epoll
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event))
	{
        perror("epoll_ctl()");
        exit(1);
	}

	// Event loop
	while(1) {
		struct epoll_event new_event;

		if(epoll_wait(epoll_fd, &new_event, 1, -1) > 0)
		{
			//Probably check for errors

			// New Connection Ready
			if(sock_fd == new_event.data.fd)
				accept_connections(&new_event, epoll_fd);
			else
				handle_data(&new_event);
		}
	}
    return 0;
}
