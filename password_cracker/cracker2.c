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

queue *q = NULL;
typedef struct _task_t {
  char *username;
  char *pwd_hash;
  char *pwd_k;
} task_t;
task_t *task_g = NULL;
size_t thread_count_g = 0;
pthread_barrier_t mybarrier;
pthread_cond_t cv;
pthread_mutex_t m;
int flag_done = 0;
int start_work = 0;
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
  while (1) {
    pthread_mutex_lock(&m);
    while (!start_work) {
      pthread_cond_wait(&cv, &m);
    }
    pthread_mutex_unlock(&m);
    if (flag_done) break;
    long start_index = 0;
    long count = 0;
    int known_letter = getPrefixLength(task_g->pwd_k);
    getSubrange(8-known_letter, thread_count_g, threadId, &start_index, &count);
    char *pwd = calloc(8+1, sizeof(char));
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
        password = calloc(8+1, sizeof(char));
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
    free(pwd);
    if (!found) {
      pthread_mutex_lock(&m_h);
      v2_print_thread_result(threadId, hashCount, 2);
      hashSum += hashCount;
      pthread_mutex_unlock(&m_h);
    }
    pthread_barrier_wait(&mybarrier);
    pthread_barrier_wait(&mybarrier);
  }
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

    pthread_cond_init(&cv, NULL);
    pthread_mutex_init(&m, NULL);
    pthread_mutex_init(&m_h, NULL);
    thread_count_g = thread_count;
    pthread_t tid[thread_count];
    for (size_t i = 0; i < thread_count; i++) {
      pthread_create(tid+i, NULL, myfunc, (void *)(i+1));
    }
    pthread_barrier_init(&mybarrier, NULL, thread_count+1);
    while ((task_g = queue_pull(q))) {
      double start_time = getTime();
      double start_cpu_time = getCPUTime();
      v2_print_start_user(task_g->username);
      start_work = 1;
      pthread_cond_broadcast(&cv);
      pthread_barrier_wait(&mybarrier);
      int result;
      if (found) result = 0; else result = 1;
      double elapsed = getTime() - start_time;
      double totalCPUTime = getCPUTime() - start_cpu_time;
      v2_print_summary(task_g->username, password, hashSum, elapsed, totalCPUTime, result);
      if (found) {
        free(password);
        password = NULL;
      }
      start_work = 0;
      found = 0;
      hashSum = 0;
      task_destroy(task_g);
      pthread_barrier_wait(&mybarrier);
    }
    flag_done = 1;
    start_work = 1;
    pthread_cond_broadcast(&cv);

    for (size_t j = 0; j < thread_count; j++) {
      pthread_join(tid[j], NULL);
    }
    queue_destroy(q);
    pthread_mutex_destroy(&m);
    pthread_mutex_destroy(&m_h);
    pthread_cond_destroy(&cv);
    pthread_barrier_destroy(&mybarrier);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
