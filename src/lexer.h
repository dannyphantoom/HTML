#ifndef LEXER_H_CURSOR_AI
#define LEXER_H_CURSOR_AI

// Forward declare structs that html_lexer needs for its signature, if possible,
// or include the necessary headers that define them.
// tokens.h defines Tokens, Token, int8, etc.
#include "tokens.h" // For Tokens type, and int8 (via tokens.h -> HTML.h)

/**
 * @brief Parses a null-terminated HTML source string into a sequence of tokens.
 * 
 * @param html_source_text The HTML content to lex.
 * @return Tokens A Tokens struct containing the list of parsed tokens.
 *         The caller is responsible for freeing the returned Tokens struct using destroytokens().
 */
Tokens html_lexer(const int8* html_source_text);

#endif /* LEXER_H_CURSOR_AI */ 