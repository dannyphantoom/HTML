#include "lexer.h"
#include "tokens.h" // For Token, Tokens, Attribute, mktoken, string_duplicate etc.
#include "HTML.h"   // For String, mkstring, sdestroy, int8, int16, stringlen, stringcopy
                    // and also for global functions like destroytoken_fully (though maybe these should move too)

#include <stdio.h>   // For printf in main, not strictly for lexer functions unless debugging
#include <stdlib.h>  // For malloc, realloc, free
#include <string.h>  // For bzero (though often in strings.h, or use memset from string.h)
#include <ctype.h>   // For isspace
#include <assert.h>  // For assert

// Helper function to duplicate a string (like strdup)
// This is static as it's only used within lexer.c
static int8* string_duplicate(const int8* s) {
    if (s == NULL) {
        return NULL;
    }
    int16 len = stringlen((int8*)s); 
    int8* new_s = (int8*)malloc(len + 1); 
    if (new_s == NULL) {
        assert(false && "malloc failed in string_duplicate");
        return NULL; 
    }
    stringcopy(new_s, s, len); 
    new_s[len] = '\0';
    return new_s;
}

// Definition of html_lexer function (copied from HTML.c)
// The return type in HTML.h was manually changed by user to 'struct s_tokens'
// but the actual function here returns 'Tokens'. This should be consistent.
// lexer.h declares it as returning 'Tokens'.
Tokens html_lexer(const int8* html_source_text) {
    Tokens tokens_result;
    tokens_result.length = 0;
    tokens_result.ts = NULL;

    if (html_source_text == NULL || *html_source_text == '\0') {
        return tokens_result;
    }

    String *input_stream = mkstring(html_source_text);
    if (!input_stream) {
        return tokens_result;
    }

    int16 capacity = 10;
    tokens_result.ts = (Token *)malloc(sizeof(Token) * capacity);
    assert(tokens_result.ts != NULL);
    bzero(tokens_result.ts, sizeof(Token) * capacity);

    while (input_stream->length > 0) {
        int8 current_char = *(input_stream->cur);
        Token *new_parsed_token = NULL;
        bool advanced_in_logic = false;

        if (current_char == '<') {
            input_stream->cur++; 
            input_stream->length--;
            advanced_in_logic = true;

            if (input_stream->length == 0) { break; }

            if (*(input_stream->cur) == '/') { 
                input_stream->cur++; 
                input_stream->length--;
                
                int8 tag_name_buffer[64]; 
                bzero(tag_name_buffer, sizeof(tag_name_buffer));
                int16 name_len = 0;
                while (input_stream->length > 0 && *(input_stream->cur) != '>' && name_len < (sizeof(tag_name_buffer) - 1)) {
                    tag_name_buffer[name_len++] = *(input_stream->cur);
                    input_stream->cur++;
                    input_stream->length--;
                }
                tag_name_buffer[name_len] = '\0';

                if (name_len > 0) {
                    new_parsed_token = mktoken(tagend, tag_name_buffer);
                }
                
                if (input_stream->length > 0 && *(input_stream->cur) == '>') { 
                    input_stream->cur++;
                    input_stream->length--;
                } else if (new_parsed_token) {
                    destroytoken_fully(new_parsed_token);
                    new_parsed_token = NULL;
                }

            } else { 
                int8 tag_name_buffer[64]; 
                bzero(tag_name_buffer, sizeof(tag_name_buffer));
                int16 name_len = 0;

                while (input_stream->length > 0 && 
                       *(input_stream->cur) != '>' &&
                       *(input_stream->cur) != '/' && 
                       !isspace((unsigned char)*(input_stream->cur)) && 
                       name_len < (sizeof(tag_name_buffer) - 1)) {
                    tag_name_buffer[name_len++] = *(input_stream->cur);
                    input_stream->cur++;
                    input_stream->length--;
                }
                tag_name_buffer[name_len] = '\0';

                Attribute *attributes_list = NULL;
                Attribute *last_attribute = NULL;

                if (name_len > 0) {
                    while (input_stream->length > 0 && *(input_stream->cur) != '>' && *(input_stream->cur) != '/') {
                        while (input_stream->length > 0 && isspace((unsigned char)*(input_stream->cur))) {
                            input_stream->cur++;
                            input_stream->length--;
                        }

                        if (input_stream->length == 0 || *(input_stream->cur) == '>' || *(input_stream->cur) == '/') {
                            break; 
                        }
                        int8 attr_name_buffer[64];
                        bzero(attr_name_buffer, sizeof(attr_name_buffer));
                        int16 attr_name_len = 0;
                        while (input_stream->length > 0 &&
                               !isspace((unsigned char)*(input_stream->cur)) &&
                               *(input_stream->cur) != '=' && *(input_stream->cur) != '>' && *(input_stream->cur) != '/' &&
                               attr_name_len < (sizeof(attr_name_buffer) - 1)) {
                            attr_name_buffer[attr_name_len++] = *(input_stream->cur);
                            input_stream->cur++;
                            input_stream->length--;
                        }
                        attr_name_buffer[attr_name_len] = '\0';

                        if (attr_name_len == 0) { break; }
                        while (input_stream->length > 0 && isspace((unsigned char)*(input_stream->cur))) {
                            input_stream->cur++; input_stream->length--;
                        }
                        if (input_stream->length > 0 && *(input_stream->cur) == '=') {
                            input_stream->cur++; input_stream->length--;
                            while (input_stream->length > 0 && isspace((unsigned char)*(input_stream->cur))) {
                                input_stream->cur++; input_stream->length--;
                            }

                            int8 attr_value_buffer[256];
                            bzero(attr_value_buffer, sizeof(attr_value_buffer));
                            int16 attr_value_len = 0;
                            char quote_char = 0;
                            if (input_stream->length > 0 && (*(input_stream->cur) == '\"' || *(input_stream->cur) == '\'')) {
                                quote_char = *(input_stream->cur);
                                input_stream->cur++; input_stream->length--;
                                while (input_stream->length > 0 && *(input_stream->cur) != quote_char && attr_value_len < (sizeof(attr_value_buffer) - 1)) {
                                    attr_value_buffer[attr_value_len++] = *(input_stream->cur);
                                    input_stream->cur++; input_stream->length--;
                                }
                                if (input_stream->length > 0 && *(input_stream->cur) == quote_char) {
                                    input_stream->cur++; input_stream->length--;
                                }
                            } else { 
                                while (input_stream->length > 0 && !isspace((unsigned char)*(input_stream->cur)) &&
                                       *(input_stream->cur) != '>' && *(input_stream->cur) != '/' && 
                                       attr_value_len < (sizeof(attr_value_buffer) - 1)) {
                                    attr_value_buffer[attr_value_len++] = *(input_stream->cur);
                                    input_stream->cur++; input_stream->length--;
                                }
                            }
                            attr_value_buffer[attr_value_len] = '\0';
                            
                            Attribute *new_attr = (Attribute*)malloc(sizeof(Attribute)); assert(new_attr != NULL);
                            new_attr->name = string_duplicate(attr_name_buffer);
                            new_attr->value = string_duplicate(attr_value_buffer);
                            new_attr->next = NULL;
                            if (!new_attr->name || !new_attr->value) { free(new_attr->name); free(new_attr->value); free(new_attr);}
                            else { 
                                if (attributes_list == NULL) { attributes_list = new_attr; last_attribute = new_attr; } 
                                else { last_attribute->next = new_attr; last_attribute = new_attr; }
                            }
                        } else { 
                            Attribute *new_attr = (Attribute*)malloc(sizeof(Attribute)); assert(new_attr != NULL);
                            new_attr->name = string_duplicate(attr_name_buffer);
                            new_attr->value = string_duplicate((const int8*)""); 
                            new_attr->next = NULL;
                            if (!new_attr->name || !new_attr->value) { free(new_attr->name); free(new_attr->value); free(new_attr); } 
                            else {
                                if (attributes_list == NULL) { attributes_list = new_attr; last_attribute = new_attr; } 
                                else { last_attribute->next = new_attr; last_attribute = new_attr; }
                            }
                        }
                    } 

                    if (input_stream->length > 0 && *(input_stream->cur) == '/') {
                         if (input_stream->length >= 2 && input_stream->cur[1] == '>') { 
                            new_parsed_token = mktoken(selfclosed, tag_name_buffer);
                            if(new_parsed_token) { new_parsed_token->contents.self->attributes = attributes_list; } else { free_attribute_list(attributes_list); }
                            input_stream->cur += 2; input_stream->length -= 2;
                        } else { 
                            free_attribute_list(attributes_list); attributes_list = NULL; 
                        }
                    } else if (input_stream->length > 0 && *(input_stream->cur) == '>') { 
                        new_parsed_token = mktoken(tagstart, tag_name_buffer);
                        if(new_parsed_token) { new_parsed_token->contents.start->attributes = attributes_list; } else { free_attribute_list(attributes_list); }
                        input_stream->cur++; input_stream->length--;
                    } else { 
                        free_attribute_list(attributes_list); attributes_list = NULL;
                    }
                } else { 
                    if (input_stream->length > 0 && *(input_stream->cur) == '>') { 
                        input_stream->cur++; input_stream->length--;
                    }
                }
            }
        } else { 
            advanced_in_logic = true; 
            int8 text_buffer[1024]; 
            bzero(text_buffer, sizeof(text_buffer));
            int16 text_len = 0;

            while (input_stream->length > 0 && 
                   *(input_stream->cur) != '<' && 
                   text_len < (sizeof(text_buffer) - 1)) {
                text_buffer[text_len++] = *(input_stream->cur);
                input_stream->cur++;
                input_stream->length--;
            }
            text_buffer[text_len] = '\0';

            if (text_len > 0) {
                bool all_whitespace = true;
                for (int16 i = 0; i < text_len; i++) {
                    if (text_buffer[i] != ' ' && text_buffer[i] != '\t' && 
                        text_buffer[i] != '\n' && text_buffer[i] != '\r') {
                        all_whitespace = false;
                        break;
                    }
                }
                if (!all_whitespace) {
                    new_parsed_token = mktoken(text, text_buffer);
                }
            }
        }

        if (new_parsed_token) {
            if (tokens_result.length == capacity) {
                capacity *= 2;
                Token* new_ts_array = (Token*)realloc(tokens_result.ts, sizeof(Token) * capacity);
                assert(new_ts_array != NULL); 
                tokens_result.ts = new_ts_array;
            }
            tokens_result.ts[tokens_result.length] = *new_parsed_token; 
            free(new_parsed_token); 
            new_parsed_token = NULL;
            tokens_result.length++;
        } else if (!advanced_in_logic && input_stream->length > 0) {
            input_stream->cur++;
            input_stream->length--;
        }
    }

    sdestroy(input_stream);
    return tokens_result;
}
