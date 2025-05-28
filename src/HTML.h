#ifndef HTML_H_CURSOR_AI
#define HTML_H_CURSOR_AI

/* hyperbird.h */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
// #include <birchutils.h> // This might be a custom library, ensure it's available or remove if not used

// Define basic integer types first, as they are used by structs in this file and in tokens.h
typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

// Now include tokens.h, which might use the typedefs above (int16)
// and defines Token/Tokens needed by html_lexer declaration later in this file.
#include "tokens.h" 

// Struct definitions that use the above typedefs
struct s_string {
    int16 length;
    int8 *cur;
    int8 data[];
};
typedef struct s_string String;

struct s_tuple {
    String *s; // Uses String, which uses int16/int8
    int8 c;
};
typedef struct s_tuple Tuple;

// Macro definitions
#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $c (char *)
#define $i (int)

#define sdestroy(s)     free(s) /**< Macro to free memory allocated for a String. */

// Function declarations

/**
 * @brief Creates a new String from a null-terminated C-style string.
 * 
 * @param str The input C-style string. Should be null-terminated.
 * @return String* Pointer to the newly created String, or NULL on failure.
 */
String *mkstring(const int8*);

String *scopy(String*);
Tuple get(String*);
int16 stringlen(int8*);

/**
 * @brief Copies a specified number of bytes from a source string to a destination string.
 * 
 * @param dst Pointer to the destination buffer.
 * @param src Pointer to the source string (will only be read from).
 * @param size The number of bytes to copy.
 */
void stringcopy(int8*, const int8*, int16);

// Declare html_lexer to return Tokens, which is defined in tokens.h
struct s_tokens html_lexer(const int8* html_content);

#endif /* HTML_H_CURSOR_AI */