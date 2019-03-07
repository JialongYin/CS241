/**
 * Password Cracker Lab
 * CS 241 - Spring 2019
 */

#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

queue *q = NULL;
int numRecovered = 0;
int numFailed = 0;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
typedef struct _task_t {
  char *username;
  char *pwd_hash;
  char *pwd_k;
} task_t;
void task_destroy(task_t *task) {
  free(task->username);
  free(task->pwd_hash);
  free(task->pwd_k);
  free(task);
}
void *myfunc(void *i) {
  struct crypt_data cdata;
  cdata.initialized = 0;
  int threadId = (long) i;
  task_t *task = NULL;
  while ((task = queue_pull(q))) {
    v1_print_thread_start(threadId, task->username);
    double start_time = getThreadCPUTime();
    int hashCount = 0;
    int success = 0;
    char *pwd = calloc(8+1, sizeof(char));
    strcpy(pwd, task->pwd_k);
    setStringPosition(pwd+getPrefixLength(pwd), 0);
    char *hashed = NULL;
    while (1) {
      hashed = crypt_r(pwd, "xx", &cdata);
      hashCount++;
      if (!strcmp(hashed, task->pwd_hash)) {
        double timeElapsed = getThreadCPUTime() - start_time;
        v1_print_thread_result(threadId, task->username, pwd, hashCount, timeElapsed, 0);
        success = 1;
        pthread_mutex_lock(&m1);
        numRecovered++;
        pthread_mutex_unlock(&m1);
        break;
      }
      incrementString(pwd);
      if (strncmp(pwd, task->pwd_k, getPrefixLength(task->pwd_k)))
        break;
    }
    free(pwd);
    if (!success) {
      double timeElapsed = getThreadCPUTime() - start_time;
      v1_print_thread_result(threadId, task->username, NULL, hashCount, timeElapsed, 1);
      pthread_mutex_lock(&m2);
      numFailed++;
      pthread_mutex_unlock(&m2);
    }
    task_destroy(task);
  }
  queue_push(q, NULL);
  return NULL;
}
int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    q = queue_create(-1);
    char *buffer = NULL;
    size_t size = 0;
    ssize_t bytes_read;
    while (1) {
      bytes_read = getline(&buffer,&size, stdin);
      if (bytes_read == -1){
        break;
      }
      if (bytes_read>0 && buffer[bytes_read-1] == '\n') {
        buffer[bytes_read-1] = '\0';
      }
      task_t *task = calloc(1, sizeof(task_t));
      char *username = strtok(buffer, " ");
      char *pwd_hash = strtok(NULL, " ");
      char *pwd_k = strtok(NULL, " ");
      task->username = calloc(strlen(username)+1, sizeof(char));
      strcpy(task->username, username);
      task->pwd_hash = calloc(strlen(pwd_hash)+1, sizeof(char));
      strcpy(task->pwd_hash, pwd_hash);
      task->pwd_k = calloc(strlen(pwd_k)+1, sizeof(char));
      strcpy(task->pwd_k, pwd_k);
      queue_push(q, task);
    }
    queue_push(q, NULL);
    free(buffer);

    pthread_t tid[thread_count];
    for (size_t i = 0; i < thread_count; i++) {
      pthread_create(tid+i, NULL, myfunc, (void *)(i+1));
      if (i == thread_count-1) {
        for (size_t j = 0; j < thread_count; j++) {
          pthread_join(tid[j], NULL);
        }
      }
    }
    queue_destroy(q);
    v1_print_summary(numRecovered, numFailed);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
