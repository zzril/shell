#ifndef SHELL_SUBPROCESS_H
#define SHELL_SUBPROCESS_H
// --------

/**
 * Runs the subprocess specified by `argv`.
 * Returns the exit code of the child process or -1 in case of any shell error.
 */
int shell_run_commandline(char* const argv[]);

// --------
#endif


