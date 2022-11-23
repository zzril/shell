#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H
// --------

#include <stddef.h>

// --------

/**
 * Returns the next token in `*s`, and increments `*s` so that it points right after the returned token.
 * Returns NULL if `*s` contains no more tokens.
 * If `s` or `*s` is NULL, returns NULL without doing anything.
 */
char* get_next_token(char** s);

/**
 * Returns the next command in `*argv`, and increments `*argv` so that it points right after the returned command.
 * Returns NULL if `*argv` contains no more commands.
 * If `argv`, `*argv` or `**argv` is NULL, returns NULL without doing anything.
 */
char** get_next_command(char*** argv);

/**
 * Flushes the pipe referred to by `pipe_read_fd` to stdout.
 */
void flush_pipe(int pipe_read_fd);

// --------
#endif


