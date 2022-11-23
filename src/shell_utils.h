#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H
// --------

#include <stddef.h>

// --------

/**
 * Returns the next token in `*s`, and increments `*s` so that it points right after the returned token.
 * Returns NULL if `s` contains no more tokens.
 * If `s` or `*s` is NULL, returns NULL without doing anything.
 */
char* get_next_token(char** s);

// --------
#endif


