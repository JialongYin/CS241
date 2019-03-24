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
graph *g = NULL;
set *visited = NULL;

bool isCyclic(char* target) {
    if (!visited) {
      visited = shallow_set_create();
    }
    rule_t *rule = (rule_t *) graph_get_vertex_value(g, target);
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
    // three conditions
    if (access(rule->target, F_OK) < 0 && !(rule->state)) {
      for (size_t i = 0; i < vector_size(rule->commands); i++) {
        if (system(vector_get(rule->commands, i)) != 0) {
          // may remove vertex
          rule->state = 1;
          set_destroy(visited);
          visited = NULL;
          return true;
        }
      }
      rule->state = 1; //
    } else {
      // any dependency not on disk - run cmds
      // newer modification time - run cmds
    }
    set_destroy(visited);
    visited = NULL;
    return false;
}
int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    g = parser_parse_makefile(makefile, targets);
    vector *goals;
    goals = graph_neighbors(g, "");
    for (size_t i = 0; i < vector_size(goals); i++) {
      char *goal = vector_get(goals, i);
      if (isCyclic(goal)) {
        print_cycle_failure(goal);
      }
    }
    vector_destroy(goals);
    graph_destroy(g);
    return 0;
}
