/**
 * Password Cracker Lab
 * CS 241 - Spring 2019
 */

#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

typedef struct _task_t {
  char *username;
  char *pwd_hash;
  char *pwd_k;
} task_t;
task_t *task_g = NULL;
size_t thread_count_g = 0;
pthread_barrier_t mybarrier;
int flag_done = 0;
int found = 0;
char *password = NULL;
pthread_mutex_t m_h;
int hashSum = 0;

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
  char *pwd = calloc(8+1, sizeof(char));
  while (1) {
    pthread_barrier_wait(&mybarrier);
    if (flag_done) break;
    long start_index = 0;
    long count = 0;
    int known_letter = getPrefixLength(task_g->pwd_k);
    getSubrange(8-known_letter, thread_count_g, threadId, &start_index, &count);
    strcpy(pwd, task_g->pwd_k);
    setStringPosition(pwd+known_letter, start_index);
    v2_print_thread_start(threadId, task_g->username, start_index, pwd);
    int hashCount = 0;
    char *hashed = NULL;
    for (long i = 0; i < count; i++) {
      hashed = crypt_r(pwd, "xx", &cdata);
      hashCount++;
      if (!strcmp(hashed, task_g->pwd_hash)) {
        found = 1;
        strcpy(password, pwd);
        pthread_mutex_lock(&m_h);
        v2_print_thread_result(threadId, hashCount, 0);
        hashSum += hashCount;
        pthread_mutex_unlock(&m_h);
        break;
      }
      if (found) {
        pthread_mutex_lock(&m_h);
        v2_print_thread_result(threadId, hashCount, 1);
        hashSum += hashCount;
        pthread_mutex_unlock(&m_h);
        break;
      }
      incrementString(pwd);
    }
    if (!found) {
      pthread_mutex_lock(&m_h);
      v2_print_thread_result(threadId, hashCount, 2);
      hashSum += hashCount;
      pthread_mutex_unlock(&m_h);
    }
    pthread_barrier_wait(&mybarrier);
  }
  free(pwd);
  return NULL;
}
int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    pthread_mutex_init(&m_h, NULL);
    pthread_barrier_init(&mybarrier, NULL, thread_count+1);
    thread_count_g = thread_count;
    pthread_t tid[thread_count];
    for (size_t i = 0; i < thread_count; i++) {
      pthread_create(tid+i, NULL, myfunc, (void *)(i+1));
    }

    task_g = calloc(1, sizeof(task_t));
    task_g->username = calloc(8+1, sizeof(char));
    task_g->pwd_hash = calloc(13+1, sizeof(char));
    task_g->pwd_k = calloc(8+1, sizeof(char));
    password = calloc(8+1, sizeof(char));
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
      char *username = strtok(buffer, " ");
      char *pwd_hash = strtok(NULL, " ");
      char *pwd_k = strtok(NULL, " ");
      strcpy(task_g->username, username);
      strcpy(task_g->pwd_hash, pwd_hash);
      strcpy(task_g->pwd_k, pwd_k);

      double start_time = getTime();
      double start_cpu_time = getCPUTime();
      v2_print_start_user(task_g->username);
      pthread_barrier_wait(&mybarrier);

      pthread_barrier_wait(&mybarrier);
      int result;
      if (found) result = 0; else result = 1;
      double elapsed = getTime() - start_time;
      double totalCPUTime = getCPUTime() - start_cpu_time;
      v2_print_summary(task_g->username, password, hashSum, elapsed, totalCPUTime, result);
      found = 0;
      hashSum = 0;
    }
    flag_done = 1;
    task_destroy(task_g);
    free(password);
    free(buffer);
    pthread_barrier_wait(&mybarrier);

    for (size_t j = 0; j < thread_count; j++) {
      pthread_join(tid[j], NULL);
    }
    pthread_mutex_destroy(&m_h);
    pthread_barrier_destroy(&mybarrier);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
