#ifndef SHELL_SUBPROCESS_H
#define SHELL_SUBPROCESS_H
// --------

/**
 * Runs the subprocess specified by `argv`.
 * The subprocess will read its input from in_fd and write its output to a newly created pipe.
 * On return, *out_fd will refer to the read end of the created pipe.
 * Returns the exit code of the child process or -1 in case of any shell error.
 */
int shell_run_single_command(char* const argv[], int in_fd, int* out_fd);

/**
 * Like `shell_run_single_command`, except that it does interpret `|`, when appearing as a separate argument, as a pipe.
 */
int shell_run_commandline(char** argv);

// --------
#endif


