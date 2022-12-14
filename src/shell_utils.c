#include <stdio.h>

#include <string.h>
#include <unistd.h>

#include "shell_utils.h"

#define BUFFER_SIZE 4096

// --------

typedef enum {
	MODE_WHITESPACE,
	MODE_NORMAL,
	MODE_QUOTED,
} ESCAPING_MODE;

// --------

char* get_next_token(char** s) {

	ESCAPING_MODE mode = MODE_WHITESPACE;
	char* token_start = *s;
	char current_char;

	// sanitize input:
	if(s == NULL || *s == NULL) {
		return NULL;
	}

	while(1) {

		current_char = **s;

		// if reached end of string, ensure all further calls on `s` will return NULL:
		if(current_char == '\0') {
			*s = NULL;
		}
		else {
			(*s)++;
		}

		switch(mode) {
			case MODE_WHITESPACE:
				switch(current_char) {
					case '\0':
						return NULL;
					case '"':
						mode = MODE_QUOTED;
						// fallthrough
					case '\t':
						// fallthrough
					case '\n':
						// fallthrough
					case '\r':
						// fallthrough
					case ' ':
						token_start = *s;
						break;
					default:
						mode = MODE_NORMAL;
						break;
				}
				break;
			case MODE_NORMAL:
				switch(current_char) {
					case '\0':
						return token_start;
					case '\t':
						// fallthrough
					case '\n':
						// fallthrough
					case '\r':
						// fallthrough
					case ' ':
						*((*s)-1) = '\0';
						return token_start;
						break;
					default:
						break;
				}
				break;
			case MODE_QUOTED:
				switch(current_char) {
					case '\0': // shouldn't happen; could inform user about this
						return NULL; // discard the incomplete quote sequence
					case '"':
						*((*s)-1) = '\0';
						return token_start;
					default:
						break;
				}
				break;
			default: // doesn't happen
				return NULL;
		}
	}
}


char** get_next_command(char*** argv) {

	char** argv_start = *argv;
	char* current_arg;

	// sanitize input:
	if(argv == NULL || *argv == NULL || **argv == NULL) {
		return NULL;
	}

	while(1) {

		current_arg = **argv;

		// if reached end of argv, ensure all further calls on `argv` will return NULL:
		if(current_arg == NULL) {
			*argv = NULL;
			return argv_start;
		}
		else {
			(*argv)++;
		}

		switch(strcmp(current_arg, "|")) {
			case 0:
				*((*argv)-1) = NULL;
				return argv_start;
			default:
				break;
		}
	}
}

void flush_pipe(int pipe_read_fd) {

	char buffer[BUFFER_SIZE];
	ssize_t bytes_read = 0;

	do {
		memset(buffer, 0, BUFFER_SIZE);
		bytes_read = read(pipe_read_fd, buffer, BUFFER_SIZE);

		if(bytes_read <= 0) {
			if(bytes_read < 0) { perror("read"); return; }
			break;
		}
		fputs(buffer, stdout);

	} while(bytes_read != 0);

	return;
}


