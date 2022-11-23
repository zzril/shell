#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "shell_subprocess.h"
#include "shell_utils.h"

// --------

#define FD_STDIN 0
#define FD_STDOUT 1

// --------

static void exec_process(char* const argv[]) {
	execvp(argv[0], argv);
	perror("execve");
	exit(EXIT_FAILURE);
}

// --------

int shell_run_single_command(char* const argv[], int in_fd, int* out_fd) {

	int wstatus;
	int pipe_fds[2];

	// sanitize in_fd:
	if(in_fd < 0) {
		fputs("shell_run_single_command: invalid in_fd\n", stderr);
		return -1;
	}

	// mark *out_fd as invalid in case of any error:
	*out_fd = -1;

	// create pipe:
	if(pipe(pipe_fds) < 0) {
		perror("pipe");
		return -1;
	}
	int pipe_read_fd = pipe_fds[0];
	int pipe_write_fd = pipe_fds[1];

	// fork:
	pid_t subprocess_pid = fork();

	switch(subprocess_pid) {

		case 0: // child

			close(pipe_read_fd);

			if(dup2(in_fd, FD_STDIN) < 0 || dup2(pipe_write_fd, FD_STDOUT) < 0) {
				perror("dup2");
				close(pipe_write_fd);
				exit(EXIT_FAILURE);
			}

			exec_process(argv);

		case -1: // error
			perror("fork");
			close(pipe_read_fd);
			close(pipe_write_fd);
			return -1;

		default: // parent

			close(pipe_write_fd);
			*out_fd = pipe_read_fd;

			if(waitpid(subprocess_pid, &wstatus, 0) < 0) {
				perror("waitpid");
				return -1;
			}
			if(!WIFEXITED(wstatus)) {
				return -1;
			}

			return WEXITSTATUS(wstatus);
	}
}

int shell_run_commandline(char** argv) {

	char** current_argv = NULL;
	char** rest_of_command_line = argv;

	int exit_code = 0;

	int next_process_input = FD_STDIN;
	int last_process_output;

	while(1) {

		current_argv = get_next_command(&rest_of_command_line);
		if(current_argv == NULL) { break; }

		exit_code = shell_run_single_command(current_argv, next_process_input, &last_process_output);
		next_process_input = last_process_output;

		if(exit_code != 0) {
			break;
		}
	}

	if(exit_code == 0) {
		flush_pipe(last_process_output);
	}

	return exit_code;
}


