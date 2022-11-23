#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "shell_subprocess.h"

// --------

static void exec_process(char* const argv[]) {
	execvp(argv[0], argv);
	perror("execve");
	exit(EXIT_FAILURE);
}

// --------

int shell_run_commandline(char* const argv[]) {

	int wstatus;
	pid_t subprocess_pid = fork();

	switch(subprocess_pid) {
		case 0: // child
			exec_process(argv);
		case -1: // error
			perror("fork");
			return -1;
		default: // parent
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


