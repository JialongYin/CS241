/**
 * Parallel Make Lab
 * CS 241 - Spring 2019
 */

#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "set.h"
#include "queue.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
graph *g = NULL;
set *visited = NULL;
queue *q = NULL;
int failed = 0;
pthread_cond_t cv;
pthread_mutex_t m;

bool isCyclic(char* target) {
    if (!visited) {
      visited = shallow_set_create();
    }
    if (set_contains(visited, target)) {
        set_destroy(visited);
        visited = NULL;
        return true;
    } else {
      set_add(visited, target);
      vector *neighbors = graph_neighbors(g, target);
      for (size_t i = 0; i < vector_size(neighbors); i++) {
        if (isCyclic(vector_get(neighbors, i))) {
          vector_destroy(neighbors);
          return true;
        }
      }
      vector_destroy(neighbors);
    }
    set_destroy(visited);
    visited = NULL;
    return false;
}
//
bool all_satisfied(vector *neighbors) {
  for (size_t i = 0; i < vector_size(neighbors); i++) {
    char *neighbor = vector_get(neighbors, i);
    rule_t *rule_nbr = (rule_t *) graph_get_vertex_value(g, neighbor);
    if (!(rule_nbr->state)) return false;
  }
  return true;
}
//
bool isFailed(char* target) {
    int flag_run = 0;
    if (access(target, F_OK) < 0) {
      flag_run = 1;
    }
    vector *neighbors = graph_neighbors(g, target);
    for (size_t i = 0; i < vector_size(neighbors); i++) {
      char *neighbor = vector_get(neighbors, i);
      if (!flag_run && access(neighbor, F_OK) < 0) {
        flag_run = 1;
      } else if (!flag_run && access(target, F_OK) == 0 && access(neighbor, F_OK) == 0) {
        struct stat trg;
        stat(target, &trg);
        struct stat nbr;
        stat(neighbor, &nbr);
        if (difftime(trg.st_mtime, nbr.st_mtime) < 0) {
          flag_run = 1;
        }
      }
      if (isFailed(neighbor))
          return true;
    }
    if (vector_size(neighbors)) {
      pthread_mutex_lock(&m);
      while (!all_satisfied(neighbors)) {
        pthread_cond_wait(&cv, &m);
      }
      pthread_mutex_unlock(&m);
    }
    vector_destroy(neighbors);
    if (failed) return true;
    rule_t *rule = (rule_t *) graph_get_vertex_value(g, target);
    if (flag_run && !(rule->state)) {
      queue_push(q, rule);
    } else {
      rule->state = 1;
    }
    return false;
}
void *myfunc(void *ptr) {
  while (1) {
    rule_t *rule = queue_pull(q);
    if (!rule) {
      queue_push(q, NULL);
      break;
    }
    for (size_t i = 0; i < vector_size(rule->commands); i++) {
      if (system(vector_get(rule->commands, i)) != 0) {
        failed = 1;
        break;
      }
    }
    rule->state = 1;
    pthread_cond_signal(&cv);
  }
  return NULL;
}
int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    q = queue_create(-1);
    pthread_t tid[num_threads];
    for (size_t i = 0; i < num_threads; i++) {
      pthread_create(tid+i, NULL, myfunc, NULL);
    }

    pthread_cond_init(&cv, NULL);
    pthread_mutex_init(&m, NULL);
    g = parser_parse_makefile(makefile, targets);
    vector *goals = graph_neighbors(g, "");
    for (size_t i = 0; i < vector_size(goals); i++) {
      char *goal = vector_get(goals, i);
      if (isCyclic(goal)) {
        print_cycle_failure(goal);
      } else {
        if (isFailed(goal))
          failed = 0;
      }
    }
    queue_push(q, NULL);
    for (size_t j = 0; j < num_threads; j++) {
      pthread_join(tid[j], NULL);
    }
    vector_destroy(goals);
    graph_destroy(g);
    queue_destroy(q);
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&cv);
    return 0;
}
