/**
 * Shell Lab
 * CS 241 - Spring 2019
 */
 
#pragma once
#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>

/**
 * This library will handle all the formatting
 * for your shell.
 *
 * Please use this to ensure your formatting
 * matches what the autograder expects.
 */

#define STATUS_RUNNING "Running"
#define STATUS_STOPPED "Stopped"

/**
 * Struct with information about a process
 *
 * pid      process id
 * nthreads number of threads for process
 * vsize    size of program (virtual) memory in kb
 * state    process state/status
 * start_str process start time (in HH:MM format)
 * time_str process execution time (in M:SS format)
 * command  command process is executing
 */
typedef struct process_info {
    int pid;
    long int nthreads;
    unsigned long int vsize;
    char state;
    char *start_str;
    char *time_str;
    char *command;
} process_info;

/**
 * Print the correct usage on invalid command line arguments.
 */
void print_usage();

/**
 * Print a command and newline.
 */
void print_command(const char *command);

/**
 * Print when the user specifies a non-existent script file.
 */
void print_script_file_error();

/**
 * Print the shell prompt with current directory `directory` and current
 * process id `pid`.
 */
void print_prompt(const char *directory, pid_t pid);

/**
 * Print when the user tries to change directories but `path` does not exist.
 */
void print_no_directory(const char *path);

/**
 * Print when executing an external command with a process id `pid`.
 */
void print_command_executed(pid_t pid);

/**
 * Print when fork fails.
 */
void print_fork_failed();

/**
 * Print when exec fails.
 */
void print_exec_failed(const char *command);

/**
 * Print when wait fails.
 */
void print_wait_failed();

/**
 * Print when setpgid fails.
 */
void print_setpgid_failed();

/**
 * Print when use of built-in command is invalid,
 * such as `kill` without pid.
 */
void print_invalid_command(const char *command);

/**
 * Print header for process information.
 */
void print_process_info_header();

/**
 * Print information about a process.
 */
void print_process_info(process_info *pinfo);

/**
 * Converts a time structure containing calendar date/time to
 * a string in HH:MM format and stores the result in buffer.
 *
 * Returns number of bytes placed in buffer.
 */
size_t time_struct_to_string(char *buffer, size_t buffer_len,
                             struct tm *tm_info);

/**
 * Converts execution time to a string in M:SS format and stores
 * the result in buffer.
 *
 * Returns the number of bytes that *should* have been written to buffer.
 * May return more bytes than written if buffer size is too small (can't
 * write more bytes than size of buffer). Same return value as snprintf.
 */
int execution_time_to_string(char *buffer, size_t buffer_len, size_t minutes,
                             size_t seconds);

/**
 * Print header for file descriptor information.
 */
void print_process_fd_info_header();

/**
 * Print information about a file descriptor (for a process).
 *
 * fd_no        file descriptor number
 * file_pos     file descriptor position (in specified process)
 * realpath     real path to file
 */
void print_process_fd_info(size_t fd_no, size_t file_pos, char *realpath);

/**
 * Print when an action on pid is requested, but a process with that
 * pid does not exist, such as `kill 123424235`
 */
void print_no_process_found(int pid);

/**
 * Print when a process was successfully killed.
 *
 * pid      process id of killed process
 * command  command process was running
 */
void print_killed_process(int pid, char *command);

/**
 * Print when process with pid has been stopped.
 */
void print_stopped_process(int pid, char *command);

/**
 * Print a single line of history. Both `index` and `command` are the same as
 * in the history log.
 */
void print_history_line(size_t index, const char *command);

/**
 * Print when the user specifies and out-of-bounds history command.
 */
void print_invalid_index();

/**
 * Print when the history search fails (no command has the prefix).
 */
void print_no_history_match();

/**
* Print when the user specifies a non-existent history file.
*/
void print_history_file_error();

/**
 * Returns the full path a file named 'filename' in the current directory.
 * The path is stored on the heap, so it is up the caller to free it.
 */
char *get_full_path(char *filename);
