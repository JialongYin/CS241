/**
 * Savvy Scheduler
 * CS 241 - Spring 2019
 */
#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"
typedef struct _job_info {
    int id;
    double arrival;
    double running_time;
    double priority;
    double remaining_time;
    double start_time;
    double start;
    /* Add whatever other bookkeeping you need into this struct. */
} job_info;

priqueue_t pqueue;
scheme_t pqueue_scheme;
comparer_t comparision_func;
double turnaround;
double total_waiting;
double total_response;
size_t n;

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any set up code you may need here
    turnaround = 0;
    total_waiting = 0;
    total_response = 0;
    n = 0;
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
  job *j_a = a;
  job *j_b = b;
  job_info *info_a = j_a->metadata;
  job_info *info_b = j_b->metadata;
  if (info_a->arrival < info_b->arrival)
    return -1;
  else if (info_a->arrival > info_b->arrival)
    return 1;
  else
    return 0;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
  job *j_a = a;
  job *j_b = b;
  job_info *info_a = j_a->metadata;
  job_info *info_b = j_b->metadata;
  if (info_a->priority < info_b->priority)
    return -1;
  else if (info_a->priority > info_b->priority)
    return 1;
  else
    return break_tie(a, b);
}

int comparer_psrtf(const void *a, const void *b) {
  job *j_a = a;
  job *j_b = b;
  job_info *info_a = j_a->metadata;
  job_info *info_b = j_b->metadata;
  if (info_a->remaining_time < info_b->remaining_time)
    return -1;
  else if (info_a->remaining_time > info_b->remaining_time)
    return 1;
  else
    return break_tie(a, b);
}

int comparer_rr(const void *a, const void *b) {
    return 0;
}

int comparer_sjf(const void *a, const void *b) {
  job *j_a = a;
  job *j_b = b;
  job_info *info_a = j_a->metadata;
  job_info *info_b = j_b->metadata;
  if (info_a->running_time < info_b->running_time)
    return -1;
  else if (info_a->running_time > info_b->running_time)
    return 1;
  else
    return break_tie(a, b);
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    // TODO complete me!
    job_info *j_info = calloc(1,sizeof(job_info));
    j_info->id = job_number;
    j_info->arrival = time;
    j_info->running_time = sched_data->running_time;
    j_info->priority = sched_data->priority;
    j_info->remaining_time = sched_data->running_time;
    j_info->start_time = time;
    j_info->start = -1;
    newjob->metadata = j_info;
    priqueue_offer(&pqueue, newjob);
    n++;
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO complete me!
    job *j = priqueue_peek(&pqueue);
    if (!job_evicted && !j)
      return NULL;
    job_info *j_info = j->metadata;
    if (j_info->start < 0)
      j_info->start = time;
    if (!job_evicted && j) {
      // edit j: start_time = time
      j_info->start_time = time;
      return j;
    }
    if (pqueue_scheme == PPRI) {
      if (j != job_evicted) {
        return j;
      } else {
        return job_evicted;
      }
    } else if (pqueue_scheme == PSRTF) {
      if (j != job_evicted) {
        // edit job_evicted: remaining_time -= time - start_time
        // edit j: start_time = time
        job_info *je_info = job_evicted->metadata;
        je_info->remaining_time -= time - je_info->start_time;
        j_info->start_time = time;
        return j;
      } else {
        // edit job_evicted: remaining time -= time - start_time and start_time = time
        job_info *je_info = job_evicted->metadata;
        je_info->remaining_time -= time - je_info->start_time;
        je_info->start_time = time;
        return job_evicted;
      }
    } else if (pqueue_scheme == RR) {
      priqueue_offer(&pqueue, j);
      priqueue_poll(&pqueue);
      return priqueue_peek(&pqueue);
    } else {
      return job_evicted;
    }
}

void scheduler_job_finished(job *job_done, double time) {
    // TODO complete me!
    job_info *j_info = job_done->metadata;
    turnaround += time - j_info->arrival;
    total_waiting += time - j_info->arrival - j_info->running_time;
    total_response += j_info->start - j_info->arrival;
    free(j_info);
    priqueue_poll(&pqueue);
}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO complete me!
    return total_waiting/n;
}

double scheduler_average_turnaround_time() {
    // TODO complete me!
    return turnaround/n;
}

double scheduler_average_response_time() {
    // TODO complete me!
    return total_response/n;
}

void scheduler_show_queue() {
    // Implement this if you need it!
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
