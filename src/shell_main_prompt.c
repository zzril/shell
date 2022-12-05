#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

#include "shell_subprocess.h"
#include "shell_utils.h"

// --------

#define SHELL_PROMPT_STREAM stderr

#define INITIAL_NUM_TOKENS 8

// --------

typedef struct {
	char* input_buffer;
	size_t input_buffer_len;
	size_t input_len;
	char** argument_vector;
	int argument_count;

} Resources;

// --------

Resources shell_resources = {
	NULL,
	0,
	0,
	NULL,
	0,
};

// --------

static void shell_free_resources() {
	free(shell_resources.input_buffer);
	free(shell_resources.argument_vector);
	return;
}

static void shell_exit(int exit_code) {
	shell_free_resources();
	exit(exit_code);
}

static void shell_fail() {
	shell_exit(EXIT_FAILURE);
}

static void shell_fail_msg(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	shell_fail();
}

static void shell_fail_errno_msg(const char* function_name) {
	perror(function_name);
	shell_fail();
}

static void shell_display_prompt() {
	fputs("$ ", SHELL_PROMPT_STREAM);
	return;
}

static size_t shell_getline() {

	errno = 0;

	if(getline(&(shell_resources.input_buffer), &(shell_resources.input_buffer_len), stdin) < 0) {
		if(errno != 0) {
			shell_fail_errno_msg("getline");
		}
		else { // end of file
			shell_exit(EXIT_SUCCESS);
		}
	}

	shell_resources.input_len = strlen(shell_resources.input_buffer);

	return shell_resources.input_len;

}

static int shell_create_argv() {

	// free old argv:
	free(shell_resources.argument_vector);

	size_t current_buffer_size = INITIAL_NUM_TOKENS;
	shell_resources.argument_vector = calloc(current_buffer_size, sizeof(char*));
	if(shell_resources.argument_vector == NULL) { shell_fail_errno_msg("calloc"); }

	char* current_argument = NULL;
	char* rest_of_string = shell_resources.input_buffer;

	shell_resources.argument_count = -1;

	do {
		current_argument = get_next_token(&rest_of_string);

		shell_resources.argument_count += 1;
		if(shell_resources.argument_count < 0) { shell_fail_msg("shell_create_argv: too many tokens"); }

		// if buffer lacks space, try re-allocating:
		if((size_t) shell_resources.argument_count >= current_buffer_size) {
			// (put the `*2` into the 2nd argument, so no overflow can happen there)
			char** argument_vector_new = reallocarray(shell_resources.argument_vector, current_buffer_size, 2 * sizeof(char*));
			if(argument_vector_new == NULL) { shell_fail_errno_msg("reallocarray"); }
			shell_resources.argument_vector = argument_vector_new;
			current_buffer_size *= 2; // no need to check for overflow here
		}

		// append new argument:
		shell_resources.argument_vector[shell_resources.argument_count] = current_argument;

	} while(current_argument != NULL);

	char** args = shell_resources.argument_vector;

	return (args != NULL && args[0] != NULL && args[0][0] != '\0');
}

static void shell_builtins_handle_exit() {
	int status_code = EXIT_SUCCESS;
	if(shell_resources.argument_count > 1) {
		status_code = atoi(shell_resources.argument_vector[1]);
	}
	shell_exit(status_code);
}

static void shell_check_builtins() {

	// check "exit":
	if(strcmp(shell_resources.argument_vector[0], "exit") == 0) {
		shell_builtins_handle_exit();
	}

	return;
}

static void shell_handle_input() {
	if(shell_create_argv()) {
		shell_check_builtins();
		shell_run_commandline(shell_resources.argument_vector);
	}
	return;
}

// --------

int main() {

	int shell_running = 1;

	while(shell_running) {

		shell_display_prompt();

		shell_getline();

		shell_handle_input();
	}

	// never reached
	shell_fail_msg("main: control flow unexpectedly reached end of function");
}


