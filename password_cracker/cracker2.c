/**
 * Password Cracker Lab
 * CS 241 - Spring 2019
 */

#include "cracker2.h"
#include "format.h"
#include "utils.h"

// queue *q = NULL;
// pthread_barrier_t mybarrier;
// typedef struct _task_t {
//   char *username;
//   char *pwd_hash;
//   char *pwd_k;
// } task_t;
// task_t *task_g = NULL;
// int flag_done = 0;
// int start_work = 0;
// void *myfunc(void *i) {
//   struct crypt_data cdata;
//   cdata.initialized = 0;
//   int threadId = (long) i;
//   while (!flag_done) {
//     v2_print_thread_start(threadId, task_g->username,
//             long offset,char *startPassword);
//     pthread_barrier_wait(&mybarrier);
//     start_work = 0;
//     pthread_barrier_wait(&mybarrier);
//   }
//   return NULL;
// }
int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    // q = queue_create(-1);
    // char *buffer = NULL;
    // size_t size = 0;
    // ssize_t bytes_read;
    // while (1) {
    //   bytes_read = getline(&buffer,&size, stdin);
    //   if (bytes_read == -1){
    //     break;
    //   }
    //   if (bytes_read>0 && buffer[bytes_read-1] == '\n') {
    //     buffer[bytes_read-1] = '\0';
    //   }
    //   task_t *task = calloc(1, sizeof(task_t));
    //   char *username = strtok(buffer, " ");
    //   char *pwd_hash = strtok(NULL, " ");
    //   char *pwd_k = strtok(NULL, " ");
    //   task->username = calloc(strlen(username)+1, sizeof(char));
    //   strcpy(task->username, username);
    //   task->pwd_hash = calloc(strlen(pwd_hash)+1, sizeof(char));
    //   strcpy(task->pwd_hash, pwd_hash);
    //   task->pwd_k = calloc(strlen(pwd_k)+1, sizeof(char));
    //   strcpy(task->pwd_k, pwd_k);
    //   queue_push(q, task);
    // }
    // queue_push(q, NULL);
    // free(buffer);
    //
    // pthread_barrier_init(&mybarrier, NULL, thread_count+1);
    // pthread_t tid[thread_count];
    // for (size_t i = 0; i < thread_count; i++) {
    //   pthread_create(tid+i, NULL, myfunc, (void *)(i+1));
    // }
    // while ((task_g = queue_pull(q))) {
    //   v2_print_start_user(task_g->username);
    //   // threads start working
    //   start_work = 1;
    //
    //   pthread_barrier_wait(&mybarrier);
    //   // print
    //   pthread_barrier_wait(&mybarrier);
    // }
    // flag_done = 1;
    //
    // for (size_t j = 0; j < thread_count; j++) {
    //   pthread_join(tid[j], NULL);
    // }
    // queue_destroy(q);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
