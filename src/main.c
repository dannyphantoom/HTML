#include <stdio.h> // For printf
#include "HTML.h"   // For stringlen, int8
#include "tokens.h" // For Tokens, showtokens, destroytokens
#include "lexer.h"  // For html_lexer

// main function is now the only content of this file.
// All other function definitions have been moved to utils.c or lexer.c

int main(int argc, char **argv){
    (void)argc; // Suppress unused parameter warning
    (void)argv; // Suppress unused parameter warning

    const int8 *html_input = (const int8 *)"<html><body class=\"test\"><h1>Hello <br/> World!</h1><p>Another node.</p></body></html>";

    // stringlen is declared in HTML.h, defined in utils.c
    if (html_input == NULL || stringlen((int8*)html_input) == 0) { 
        printf("HTML input string is NULL or empty.\n");
        return 1; 
    }

    printf("Input HTML: \"%s\"\n", html_input);

    // html_lexer is declared in lexer.h, defined in lexer.c
    Tokens lexed_tokens = html_lexer(html_input);

    // showtokens is declared in tokens.h, defined in utils.c
    printf("Lexed Tokens (%d):\n'%s'\n", lexed_tokens.length, showtokens(lexed_tokens));

    // destroytokens is a macro in tokens.h
    destroytokens(lexed_tokens); 
    
    return 0;
}