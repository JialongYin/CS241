/**
 * Teaching Threads Lab
 * CS 241 - Spring 2019
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

/* You might need a struct for each task ... */
typedef struct _task_t {
  int *start;
  int *end;
  int result;
  reducer callback;
} task_t;
/* You should create a start routine for your threads. */
void *myfunc(void *ptr) {
  task_t* task = (task_t*) ptr;
  for ( int *sub = task->start; sub != task->end; sub++) {
      task->result = task->callback(task->result, *sub);
  }
  pthread_exit(NULL);
}
int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
    size_t sub_len = list_len / num_threads;
    task_t* tasks = calloc(num_threads, sizeof(task_t));
    for (size_t k = 0; k < num_threads; k++) {
      tasks[k].start = list + k*sub_len;
      tasks[k].end = list + (k+1)*sub_len;
      tasks[k].result = base_case;
      tasks[k].callback = reduce_func;
    }
    tasks[num_threads-1].end = list + list_len;
    pthread_t tid[num_threads];
    for (size_t i = 0; i < num_threads; i++) {
      pthread_create(tid+i, NULL, myfunc, tasks+i);
      if (i == num_threads-1) {
        for (size_t j = 0; j < num_threads; j++) {
          pthread_join(tid[j], NULL);
        }
      }
    }
    int result = base_case;
    for ( size_t i = 0; i < num_threads; i++) {
        result = reduce_func(result, tasks[i].result);
    }
    return result;
}
