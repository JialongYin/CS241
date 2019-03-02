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
int test1 = 0;
int test2 = 0;
drm_t *drm1 = NULL;
drm_t *drm2 = NULL;
size_t num_threads = 2;

void *myfunc1(void *tid) {
  if (drm_wait(drm1, tid))
    test1++;
  else printf("deadlock!!!\n");
  printf("1 first\n");
  if (drm_wait(drm2, tid)) {
    for (int i = 0; i < 10000; i++) {
      test2++;
    }
  } else printf("deadlock!!!\n");

  if (drm_post(drm2, tid) == 0)
    printf("error!!!\n");

  if (drm_post(drm1, tid) == 0)
    printf("error!!!\n");
  return NULL;
}
void *myfunc2(void *tid) {
  printf("2 first\n");
  if (drm_wait(drm2, tid)) {
    for (int i = 0; i < 1000000; i++) {
      test2++;
    }
  } else printf("deadlock!!!\n");
  
  if (drm_wait(drm1, tid))
    test1++;
  else printf("deadlock!!!\n");

  if (drm_post(drm1, tid) == 0)
    printf("error!!!\n");

  if (drm_post(drm2, tid) == 0)
    printf("error!!!\n");

  return NULL;
}
int main() {
    drm1 = drm_init();
    drm2 = drm_init();
    pthread_t tid[num_threads];
    for (size_t i = 0; i < num_threads; i++) {
      if (i==0) {
        pthread_create(tid+i, NULL, myfunc1, tid);
      } else {
        pthread_create(tid+i, NULL, myfunc2, tid+1);
      }

      if (i == num_threads-1) {
        for (size_t j = 0; j < num_threads; j++) {
          pthread_join(tid[j], NULL);
        }
      }
    }
    printf("test1: %d\n", test1);
    printf("test2: %d\n", test2);
    drm_destroy(drm1);
    drm_destroy(drm2);
    pthread_exit(NULL);
    return 0;
}
