#ifndef TOKENS_H_CURSOR_AI
#define TOKENS_H_CURSOR_AI

// It's generally better for tokens.h to not depend on HTML.h directly if possible,
// or to only depend on very basic types from it that could be forward-declared.
// However, given the current structure where HTML.h defines int8, int16 etc.,
// and tokens.h uses them, this include is needed.
// HTML.h should now have its own include guard to prevent recursion issues.
#include "HTML.h" 

/* Token Types 
    - text
    - tagstart
    - tagend
    - selfclosed
*/

enum e_tag{
    html = 1,
    body = 2,
    b = 3,
    br = 4
};
typedef enum e_tag Tag;

/**
 * @brief Represents a single HTML attribute (name-value pair).
 */
struct s_attribute {
    int8 *name;  /**< Dynamically allocated attribute name. */
    int8 *value; /**< Dynamically allocated attribute value. */
    struct s_attribute *next; /**< Pointer to the next attribute in a linked list. */
};
typedef struct s_attribute Attribute;

struct s_tagstart{
    Tag type;
    int8 value[64]; // Tag name, e.g., "div", "p" - consider making this dynamic if tag names can be longer
    Attribute *attributes; /**< Head of linked list of attributes, NULL if no attributes. */
};
typedef struct s_tagstart TagStart;

struct s_tagend{
    Tag type;
    int8 value[64]; // Keep consistent with TagStart for now
};
typedef struct s_tagend TagEnd;

struct s_selfclosed{
    Tag type;
    int8 value[64]; // Tag name - consider making this dynamic
    Attribute *attributes; /**< Head of linked list of attributes, NULL if no attributes. */
};
typedef struct s_selfclosed SelfClosed;

struct s_texttoken{
    // Tag type; // This was commented out as text tokens don't typically have a HTML tag type like 'p' or 'div'
    int8 value[];
}; 
typedef struct s_texttoken Text;

enum e_tokentype{
    text = 1,
    tagstart = 2,
    tagend = 3,
    selfclosed = 4,
};
typedef enum e_tokentype TokenType;


struct s_token{
    TokenType type;
    union{
        Text *texttoken;
        TagStart *start;
        TagEnd *end;
        SelfClosed *self;
    }contents;
};
typedef struct s_token Token;

struct s_tokens{
    int16 length;
    Token *ts; // Pointer to an array of Token structs
};
typedef struct s_tokens Tokens;

/**
 * @brief Helper function to free a linked list of attributes.
 * @param attr Pointer to the head of the attribute list.
 */
static inline void free_attribute_list(Attribute *attr) {
    Attribute *current = attr;
    Attribute *next_attr;
    while (current != NULL) {
        next_attr = current->next;
        free(current->name);  // Free dynamically allocated name
        free(current->value); // Free dynamically allocated value
        free(current);        // Free the attribute node itself
        current = next_attr;
    }
}

/**
 * @brief Macro to free the dynamically allocated content of a Token.
 *        This should be called before freeing the Token itself if the Token is also heap-allocated,
 *        or before the array containing the Token is freed.
 * @param t_ptr Pointer to the Token whose contents are to be freed.
 *          It frees the specific structure pointed to by the active member of the `contents` union.
 * 
 * @warning This macro assumes that the correct field in the `contents` union is non-NULL
 *          and that other fields are NULL or their freeing is handled elsewhere if they could co-exist.
 *          The `mktoken` function needs to ensure only one content pointer is set and others are NULL.
 */
#define destroytoken_content(t_ptr) do { \
    if ((t_ptr) == NULL) break; \
    switch ((t_ptr)->type) { \
        case text: if((t_ptr)->contents.texttoken) { free((t_ptr)->contents.texttoken); (t_ptr)->contents.texttoken = NULL; } break; \
        case tagstart: \
            if ((t_ptr)->contents.start != NULL) { \
                free_attribute_list((t_ptr)->contents.start->attributes); \
                (t_ptr)->contents.start->attributes = NULL; \
                free((t_ptr)->contents.start); \
                (t_ptr)->contents.start = NULL; \
            } \
            break; \
        case tagend: if((t_ptr)->contents.end) { free((t_ptr)->contents.end); (t_ptr)->contents.end = NULL; } break; \
        case selfclosed: \
            if ((t_ptr)->contents.self != NULL) { \
                free_attribute_list((t_ptr)->contents.self->attributes); \
                (t_ptr)->contents.self->attributes = NULL; \
                free((t_ptr)->contents.self); \
                (t_ptr)->contents.self = NULL; \
            } \
            break; \
        default: break; \
    } \
} while(false)

/**
 * @brief Macro to destroy a Tokens structure.
 * 
 * This iterates through the tokens, freeing the content of each token using `destroytoken_content`,
 * and then frees the array of tokens (`ts.ts`) itself.
 * @param ts_struct The Tokens structure (passed by value) to destroy.
 * 
 * @warning This assumes `ts_struct.ts` is a dynamically allocated array of `Token` structs,
 *          and each `Token`\'s `contents` point to dynamically allocated memory.
 */
#define destroytokens(ts_struct) do {\
    if ((ts_struct).ts != NULL) { \
        for(int16 _idx = 0; _idx < (ts_struct).length; _idx++) { \
            destroytoken_content(&((ts_struct).ts[_idx])); \
        } \
        free((ts_struct).ts); \
        (ts_struct).ts = NULL; \
        (ts_struct).length = 0; \
    } \
} while(false)

// Function declarations for functions defined in HTML.c but used with Token/Tokens types
// Or for functions in a dedicated tokens.c file if you create one.

int8 *showtoken(Token); // Assumes Token is defined above
int8 *showtokens(Tokens); // Assumes Tokens is defined above

/*Constructors*/
Token *mktoken(TokenType type, int8 * value); // Assumes Token and TokenType are defined
// Tokens *mktokens(); // Declaration for a potential constructor for Tokens struct - not yet implemented

/**
 * @brief Destroys a single token, including its dynamically allocated contents 
 *        and the token structure itself if it was dynamically allocated.
 * 
 * This function is intended to be used for Token instances that were created by `mktoken`
 * and are managed individually (not as part of a `Tokens` array where only content needs freeing per element).
 * 
 * @param t Pointer to the Token to be destroyed. Assumes 't' was allocated by mktoken
 *          or is otherwise a heap-allocated Token whose contents also need freeing.
 */
void destroytoken_fully(Token* t); // Assumes Token is defined

#endif /* TOKENS_H_CURSOR_AI */