/* hyperbird.c */
#include "tokens.h"
#include "HTML.h"

int8 *showtokens(Tokens ts){
    int8 *p,*cur;
    static int8 buf[20480];
    int16 n, total,i;
    Token *t;

    total = 0;
    cur = buf;
    bzero(buf, sizeof(buf));
    for(i = ts.length, t=ts.ts;i; i--,t++){
        p = showtoken(*t);
        if(!p)
            break;
        if(!(*p))
            continue;
        n = stringlen(p);
        total += n;
        if(total >= sizeof(buf))
            break;
        stringcopy(cur,p,n);
        cur += n;
    }
    return buf;
}

int8 *showtoken(Token t){
    int *ret;
    static int8 tmp[256];
    assert(t.type > 0);
    ret = tmp;
    bzero($1 tmp, sizeof(tmp));
    switch(t.type){
        case text: snprintf($c tmp,255,"%s",t.contents.texttoken->value); break;
        case tagstart: snprintf($c tmp,255,"<%s>",t.contents.start->value); break;
        case tagend: snprintf($c tmp,255,"</%s>",t.contents.end->value); break;
        case selfclosed: snprintf($c tmp,255,"<%s/>",t.contents.self->value); break;
        default: break;

    }
    return tmp;
}
/*

<body>
 <b>Text</b>
</body>

Lexer

Token       Value
------------------
tagstart    "body"
tagstart    "b"
text        "Text"
tagend      "b"
tagend      "body"

*/

int16 stringlen(int8 *str) {
    int16 n;
    int8 *p;

    assert(str);
    for (p = str, n=0; *p; p++, n++);

    return n;
}

void stringcopy(int8 *dst, int8 *src, int16 size) {
    int16 n;
    int8 *d, *s;

    assert(src && dst && size);
    for (d=dst, s=src, n=size; n; d++, s++, n--)
        *d = *s;
    
    return;
}

String *scopy(String *s) {
    String *p;
    int16 size;

    assert(s && s->length);

    size = sizeof(struct s_string) + s->length;
    p = (String *)malloc($i size);
    assert(p);

    bzero($1 p, size);
    p->length = s->length;
    stringcopy(p->data, s->cur, s->length);
    p->cur = p->data;

    return p;
}

Tuple get(String *s) {
    String *new;
    int8 c;

    assert(s);
    if (!s->length)
        goto fail;

    c = *s->cur;
    new = scopy(s);
    if (!new)
        goto fail;
    
    new->cur++;
    new->length--;

    Tuple ret = {
        .s = new,
        .c = c
    };
    sdestroy(s);

    return ret;

    fail:
        Tuple err = {0};

        return err;

}

String *mkstring(int8 *str) {
    String *p;
    int16 n, size;

    assert(str);
    n = stringlen(str);
    if (n == 0) {
    }
    assert(n > 0);
    size = sizeof(struct s_string) + n;
    p = (String *)malloc($i size);
    assert(p);

    bzero($1 p, size);
    p->length = n;
    stringcopy(p->data, str, n);
    p->cur = p->data;

    return p;
}

void destroytoken_fully(Token* t){
    if (t == NULL) {
        return;
    }
    destroytoken_content(t);
    free(t);
}

Token *mktoken(TokenType type, int8 * value){
    Token *newToken = NULL;
    int16 value_len;

    assert(value != NULL);
    value_len = stringlen(value);

    newToken = (Token *)malloc(sizeof(Token));
    assert(newToken != NULL);
    bzero(newToken, sizeof(Token));

    newToken->type = type;

    switch(type){
        case text: {
            Text *textContent = (Text *)malloc(sizeof(Text) + value_len + 1);
            assert(textContent != NULL);
            bzero(textContent, sizeof(Text) + value_len + 1);
            stringcopy(textContent->value, value, value_len);
            newToken->contents.texttoken = textContent;
            break;
        }
        case tagstart: {
            TagStart *startContent = (TagStart *)malloc(sizeof(TagStart) + value_len + 1);
            assert(startContent != NULL);
            bzero(startContent, sizeof(TagStart) + value_len + 1);
            stringcopy(startContent->value, value, value_len);
            newToken->contents.start = startContent;
            break;
        }
        case tagend: {
            TagEnd *endContent = (TagEnd *)malloc(sizeof(TagEnd) + value_len + 1);
            assert(endContent != NULL);
            bzero(endContent, sizeof(TagEnd) + value_len + 1);
            stringcopy(endContent->value, value, value_len);
            newToken->contents.end = endContent;
            break;
        }
        case selfclosed: {
            SelfClosed *selfContent = (SelfClosed *)malloc(sizeof(SelfClosed) + value_len + 1);
            assert(selfContent != NULL);
            bzero(selfContent, sizeof(SelfClosed) + value_len + 1);
            stringcopy(selfContent->value, value, value_len);
            newToken->contents.self = selfContent;
            break;
        }
        default:
            free(newToken);
            assert(false && "Unknown token type in mktoken");
            return NULL;
    }
    return newToken;
}

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

    // Main parsing loop
    while (input_stream->length > 0) {
        int8 current_char = *(input_stream->cur);
        Token *new_parsed_token = NULL;
        bool advanced_in_logic = false;

        if (current_char == '<') {
            input_stream->cur++; // Consume '<'
            input_stream->length--;
            advanced_in_logic = true;

            if (input_stream->length == 0) { // EOF after '<'
                break; // Malformed
            }

            if (*(input_stream->cur) == '/') { // Potential End Tag
                input_stream->cur++; // Consume '/'
                input_stream->length--;
                
                // ---- START: Placeholder for Full End Tag Logic ----
                int8 tag_name_buffer[64]; 
                bzero(tag_name_buffer, sizeof(tag_name_buffer));
                int16 name_len = 0;
                while (input_stream->length > 0 && *(input_stream->cur) != '>' && name_len < (sizeof(tag_name_buffer) - 1)) {
                    tag_name_buffer[name_len++] = *(input_stream->cur);
                    input_stream->cur++;
                    input_stream->length--;
                }
                tag_name_buffer[name_len] = '\0';

                // if (name_len > 0) {
                // new_parsed_token = mktoken(tagend, tag_name_buffer); // For future
                // }
                
                if (input_stream->length > 0 && *(input_stream->cur) == '>') { // Consume '>'
                    input_stream->cur++;
                    input_stream->length--;
                } else if (new_parsed_token) { // If we made a token but no '>'
                    // destroytoken_fully(new_parsed_token); new_parsed_token = NULL;
                }
                // ---- END: Placeholder for Full End Tag Logic ----

            } else { // Potential Start Tag or Self-Closing Tag
                // ---- START: Logic for Start Tags / Self-Closing Tags ----
                int8 tag_name_buffer[64]; 
                bzero(tag_name_buffer, sizeof(tag_name_buffer));
                int16 name_len = 0;

                // Extract tag name
                while (input_stream->length > 0 && 
                       *(input_stream->cur) != '>' &&
                       *(input_stream->cur) != '/' && 
                       // Add !isspace for attributes later
                       name_len < (sizeof(tag_name_buffer) - 1)) {
                    tag_name_buffer[name_len++] = *(input_stream->cur);
                    input_stream->cur++;
                    input_stream->length--;
                }
                tag_name_buffer[name_len] = '\0';

                if (name_len > 0) {
                    // Check for self-closing: e.g., <br/>
                    if (input_stream->length >= 1 && *(input_stream->cur) == '/') {
                        if (input_stream->length >= 2 && input_stream->cur[1] == '>') { // Check for '/>'
                            new_parsed_token = mktoken(selfclosed, tag_name_buffer);
                            input_stream->cur += 2; // Consume "/>"
                            input_stream->length -= 2;
                        } else {
                             // Malformed: <tag / notfollowedby > e.g. <tag /a> or <tag / EOF
                             // We treat this as an error for now, don't create a token.
                             // The '/' will be consumed by the fallback advance at the end of the loop.
                        }
                    } else if (input_stream->length >= 1 && *(input_stream->cur) == '>') { // Regular start tag: <tag>
                        new_parsed_token = mktoken(tagstart, tag_name_buffer);
                        input_stream->cur++; // Consume '>'
                        input_stream->length--;
                    } else {
                        // Malformed: <tag somethingelse or <tag EOF
                        // No token created. Fallback advance will handle current char.
                    }
                } else { 
                    // Empty tag name, e.g., <>, </>, or < followed by non-alpha if we add that check
                    // Or just '<' at EOF if initial check `input_stream->length == 0` was more complex
                    if (input_stream->length > 0 && *(input_stream->cur) == '>') { // Case of <>
                        input_stream->cur++;
                        input_stream->length--;
                    }
                    // Other cases like '< /' or '< EOF' are tricky, current logic might get stuck
                    // or rely on outer loop's fallback. A robust lexer needs more specific error states here.
                }
                // ---- END: Logic for Start Tags / Self-Closing Tags ----
            }
        } else { // Not starting with '<', so it's Text
            // ---- START: Logic for Text Tokens ----
            advanced_in_logic = true; 
            int8 text_buffer[1024]; // Adjust size as needed
            bzero(text_buffer, sizeof(text_buffer));
            int16 text_len = 0;

            while (input_stream->length > 0 && 
                   *(input_stream->cur) != '<' && // Stop at the next tag
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
            // ---- END: Logic for Text Tokens ----
        }

        // Add new_parsed_token to the array if it was created
        if (new_parsed_token) {
            if (tokens_result.length == capacity) {
                capacity *= 2;
                Token* new_ts_array = (Token*)realloc(tokens_result.ts, sizeof(Token) * capacity);
                assert(new_ts_array != NULL); 
                tokens_result.ts = new_ts_array;
                // bzero(&tokens_result.ts[tokens_result.length], sizeof(Token) * (capacity - tokens_result.length)); // Zero out new part
            }
            tokens_result.ts[tokens_result.length] = *new_parsed_token;
            destroytoken_fully(new_parsed_token);
            tokens_result.length++;
        } else if (!advanced_in_logic && input_stream->length > 0) {
            // Fallback: If no specific logic consumed the character, consume it now
            // to prevent an infinite loop on unhandled characters.
            input_stream->cur++;
            input_stream->length--;
        }
    }

    sdestroy(input_stream);
    return tokens_result;
}

int main(int argc, char **argv){
    (void)argc;
    (void)argv;
    Token *temp_t1, *temp_t2, *temp_t3, *temp_t4, *temp_t5, *temp_t6;
    
    temp_t1 = mktoken(tagstart, "html");
    temp_t2 = mktoken(tagstart, "body");
    temp_t3 = mktoken(text, "Hello World");
    temp_t4 = mktoken(selfclosed, "br");
    temp_t5 = mktoken(tagend, "body");
    temp_t6 = mktoken(tagend, "html");

    assert(temp_t1 && temp_t2 && temp_t3 && temp_t4 && temp_t5 && temp_t6);

    Tokens ts;
    ts.length = 6;
    ts.ts = (Token *)malloc(sizeof(Token) * ts.length);
    assert(ts.ts != NULL);
    bzero(ts.ts, sizeof(Token) * ts.length);

    ts.ts[0] = *temp_t1; destroytoken_fully(temp_t1);
    ts.ts[1] = *temp_t2; destroytoken_fully(temp_t2);
    ts.ts[2] = *temp_t3; destroytoken_fully(temp_t3);
    ts.ts[3] = *temp_t4; destroytoken_fully(temp_t4);
    ts.ts[4] = *temp_t5; destroytoken_fully(temp_t5);
    ts.ts[5] = *temp_t6; destroytoken_fully(temp_t6);
    
    printf("'%s'\n", showtokens(ts));
    
    destroytokens(ts); 
    
    return 0;
}