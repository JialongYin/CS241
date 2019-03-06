/**
 * Deadlock Demolition Lab
 * CS 241 - Spring 2019
 */

#include "libdrm.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int test1 = 0;
int test2 = 0;
drm_t *drm1 = NULL;
drm_t *drm2 = NULL;
size_t num_threads = 2;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

void *myfunc1(void *tid) {
  if (drm_wait(drm1, tid))
    test1++;
  else printf("deadlock 1!!!\n");
  sleep(1);

  if (drm_wait(drm2, tid)) {
    for (int i = 0; i < 10000; i++) {
      test2++;
    }
  } else printf("deadlock 2!!!\n");

  if (drm_post(drm2, tid) == 0)
    printf("error!!!\n");

  if (drm_post(drm1, tid) == 0)
    printf("error!!!\n");
  return NULL;

  // pthread_mutex_lock(&m1);
  // for (int i = 0; i < 100000000; i++) {
  //   test1++;
  // }
  // sleep(1);
  // printf("1 first\n");
  // pthread_mutex_lock(&m2);
  // for (int i = 0; i < 10000; i++) {
  //   test2++;
  // }
  // pthread_mutex_unlock(&m2);
  // pthread_mutex_unlock(&m1);
  // return NULL;
}
void *myfunc2(void *tid) {
  if (drm_wait(drm2, tid)) {
    for (int i = 0; i < 1000000; i++) {
      test2++;
    }
  } else printf("deadlock 3!!!\n");

  if (drm_wait(drm1, tid))
    test1++;
  else printf("deadlock 4!!!\n");

  if (drm_post(drm1, tid) == 0)
    printf("error!!!\n");

  if (drm_post(drm2, tid) == 0)
    printf("error!!!\n");
  return NULL;

  // printf("2 first\n");
  // pthread_mutex_lock(&m2);
  // for (int i = 0; i < 100000000; i++) {
  //   test2++;
  // }
  // pthread_mutex_lock(&m1);
  // for (int i = 0; i < 10000; i++) {
  //   test1++;
  // }
  // pthread_mutex_unlock(&m1);
  // pthread_mutex_unlock(&m2);
  // return NULL;
}
int main() {
    drm1 = drm_init();
    drm2 = drm_init();
    pthread_t tid[num_threads];
    pthread_create(tid, NULL, myfunc1, tid);
    pthread_create(tid+1, NULL, myfunc2, tid+1);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    printf("test1: %d\n", test1);
    printf("test2: %d\n", test2);
    drm_destroy(drm1);
    drm_destroy(drm2);
    pthread_mutex_destroy(&m1);
    pthread_mutex_destroy(&m2);
    pthread_exit(NULL);
}
