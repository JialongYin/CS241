/**
 * Deadlock Demolition Lab
 * CS 241 - Spring 2019
 */

#include "graph.h"
#include "libdrm.h"
#include "set.h"
#include <pthread.h>

struct drm_t {pthread_mutex_t m;};
graph *g = NULL;
set *visited = NULL;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
bool isCyclic(void* node) {
    if (!visited) {
      visited = shallow_set_create();
    }
    if (set_contains(visited, node)) {
        visited = NULL;
        return true;
    } else {
      set_add(visited, node);
      vector *neighbors = graph_neighbors(g, node);
      for (size_t i = 0; i < vector_size(neighbors); i++) {
        if (isCyclic(vector_get(neighbors, i))) {
          return true;
        }
      }
    }
    visited = NULL;
    return false;
}
drm_t *drm_init() {
    /* Your code here */
    drm_t *drm = malloc(sizeof(drm_t));
    pthread_mutex_init(&drm->m, NULL);

    pthread_mutex_lock(&m);
    if (!g) {
      g = shallow_graph_create();
    }
    graph_add_vertex(g, drm);
    pthread_mutex_unlock(&m);

    return drm;
}

int drm_post(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    if (pthread_mutex_unlock(&drm->m)) return 0;

    pthread_mutex_lock(&m);
    graph_remove_edge(g, drm, thread_id);
    pthread_mutex_unlock(&m);

    return 1;
}

int drm_wait(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */

    pthread_mutex_lock(&m);
    graph_add_vertex(g, thread_id);
    graph_add_edge(g, thread_id, drm);
    if (isCyclic(thread_id)) {
      graph_remove_edge(g, thread_id, drm);
      pthread_mutex_unlock(&m);

      return 0;
    } else {
      graph_remove_edge(g, thread_id, drm);
      graph_add_edge(g, drm, thread_id);
      pthread_mutex_unlock(&m);

      pthread_mutex_lock(&drm->m);

      return 1;
    }
}

void drm_destroy(drm_t *drm) {
    /* Your code here */

    pthread_mutex_lock(&m);
    graph_remove_vertex(g, drm);
    pthread_mutex_unlock(&m);

    pthread_mutex_destroy(&drm->m);
    free(drm);
    return;
}
