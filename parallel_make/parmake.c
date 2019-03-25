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

bool isCyclic(char* target) {
    if (!visited) {
      visited = shallow_set_create();
    }
    int flag_run = 0;
    if (set_contains(visited, target)) {
        // Part2: edit global var.: sum, failed
        set_destroy(visited);
        visited = NULL;
        return true;
    } else {
      set_add(visited, target);
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
        // Part2: check status only after iteration
        if (isCyclic(neighbor)) {
          vector_destroy(neighbors);
          return true;
        }
      }
      // Part2: if failed return true; wait here until sum = size(neighbors) 
      vector_destroy(neighbors);
    }
    // Part2: put all below in threads -- global var.: sum, failed
    rule_t *rule = (rule_t *) graph_get_vertex_value(g, target);
    if(!(rule->state) && flag_run) {
      rule->state = 1;
      for (size_t i = 0; i < vector_size(rule->commands); i++) {
        if (system(vector_get(rule->commands, i)) != 0) {
          set_destroy(visited);
          visited = NULL;
          return true;
        }
      }
    }
    set_destroy(visited);
    visited = NULL;
    return false;
}
int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    g = parser_parse_makefile(makefile, targets);
    // printf("pass here\n");
    vector *goals;
    goals = graph_neighbors(g, "");
    for (size_t i = 0; i < vector_size(goals); i++) {
      char *goal = vector_get(goals, i);
      if (isCyclic(goal)) {
        print_cycle_failure(goal);
        // Part2: edit global var.: failed = 0
      }
    }
    vector_destroy(goals);
    graph_destroy(g);
    return 0;
}
