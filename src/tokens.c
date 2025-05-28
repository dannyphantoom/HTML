#include "HTML.h"
#include "tokens.h"
#include <stdio.h>  // For snprintf in showtoken
#include <stdlib.h> // For malloc, free
#include <string.h> // For bzero (or strings.h), strlen potentially if not using custom
#include <assert.h>

// Definitions from HTML.h related functions
int16 stringlen(int8 *str) {
    int16 n;
    int8 *p;
    assert(str);
    for (p = str, n=0; *p; p++, n++);
    return n;
}

void stringcopy(int8 *dst, const int8 *src, int16 size) {
    int16 n;
    int8 *d;
    const int8 *s;
    assert(src && dst && size);
    for (d=dst, s=src, n=size; n; d++, s++, n--)
        *d = *s;
    return;
}

String *mkstring(const int8 *str) {
    String *p;
    int16 n, 실제size;
    assert(str);
    n = stringlen((int8*)str); // Current stringlen expects non-const
    assert(n > 0); // As per original logic; html_lexer handles empty source separately
    실제size = sizeof(struct s_string) + n;
    p = (String *)malloc(실제size);
    assert(p);
    bzero($1 p, 실제size);
    p->length = n;
    stringcopy(p->data, str, n);
    p->cur = p->data;
    return p;
}

String *scopy(String *s) {
    String *p;
    int16 실제size;
    assert(s && s->length);
    실제size = sizeof(struct s_string) + s->length;
    p = (String *)malloc(실제size);
    assert(p);
    bzero($1 p, 실제size);
    p->length = s->length;
    stringcopy(p->data, s->cur, s->length);
    p->cur = p->data;
    return p;
}

Tuple get(String *s) {
    String *new_str;
    int8 c;
    assert(s);
    if (!s->length) {
        Tuple err = {0}; return err;
    }
    c = *s->cur;
    new_str = scopy(s);
    if (!new_str) {
        Tuple err = {0}; return err;
    }
    new_str->cur++;
    new_str->length--;
    Tuple ret = { .s = new_str, .c = c };
    sdestroy(s); // Original string is destroyed
    return ret;
}

// Definitions from tokens.h related functions
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
            textContent->value[value_len] = '\0';
            newToken->contents.texttoken = textContent;
            break;
        }
        case tagstart: {
            TagStart *startContent = (TagStart *)malloc(sizeof(TagStart) + value_len + 1);
            assert(startContent != NULL);
            bzero(startContent, sizeof(TagStart) + value_len + 1);
            startContent->attributes = NULL; // Initialize attributes
            stringcopy(startContent->value, value, value_len);
            startContent->value[value_len] = '\0';
            newToken->contents.start = startContent;
            break;
        }
        case tagend: {
            TagEnd *endContent = (TagEnd *)malloc(sizeof(TagEnd) + value_len + 1);
            assert(endContent != NULL);
            bzero(endContent, sizeof(TagEnd) + value_len + 1);
            stringcopy(endContent->value, value, value_len);
            endContent->value[value_len] = '\0';
            newToken->contents.end = endContent;
            break;
        }
        case selfclosed: {
            SelfClosed *selfContent = (SelfClosed *)malloc(sizeof(SelfClosed) + value_len + 1);
            assert(selfContent != NULL);
            bzero(selfContent, sizeof(SelfClosed) + value_len + 1);
            selfContent->attributes = NULL; // Initialize attributes
            stringcopy(selfContent->value, value, value_len);
            selfContent->value[value_len] = '\0';
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

void destroytoken_fully(Token* t){
    if (t == NULL) return;
    destroytoken_content(t); // This macro is in tokens.h
    free(t);
}

int8 *showtoken(Token t){
    static int8 tmp[256];
    assert(t.type > 0);
    bzero($1 tmp, sizeof(tmp));
    switch(t.type){
        case text: snprintf($c tmp,255,"%s",t.contents.texttoken->value); break;
        case tagstart: {
            int offset = snprintf($c tmp,255,"<%s",t.contents.start->value);
            Attribute *attr = t.contents.start->attributes;
            while(attr != NULL && offset < 250) { 
                offset += snprintf($c tmp + offset, 255 - offset, " %s=\"%s\"", attr->name, attr->value);
                attr = attr->next;
            }
            if (offset < 255) snprintf($c tmp + offset, 255 - offset, ">");
            break;
        }
        case tagend: snprintf($c tmp,255,"</%s>",t.contents.end->value); break;
        case selfclosed: {
            int offset = snprintf($c tmp,255,"<%s",t.contents.self->value);
            Attribute *attr = t.contents.self->attributes;
            while(attr != NULL && offset < 250) { 
                offset += snprintf($c tmp + offset, 255 - offset, " %s=\"%s\"", attr->name, attr->value);
                attr = attr->next;
            }
            if (offset < 253) snprintf($c tmp + offset, 255 - offset, "/>");
            break;
        }
        default: break;
    }
    return tmp;
}

int8 *showtokens(Tokens ts){
    int8 *p,*cur;
    static int8 buf[20480]; // Large static buffer, be cautious
    int16 n, total,i;
    Token *t_ptr;
    total = 0;
    cur = buf;
    bzero(buf, sizeof(buf));
    for(i = 0, t_ptr = ts.ts; i < ts.length; i++, t_ptr++){
        p = showtoken(*t_ptr);
        if(!p) break;
        if(!(*p)) continue;
        n = stringlen(p);
        total += n;
        if(total >= sizeof(buf) -1 ) break; // -1 for null terminator safety
        stringcopy(cur,p,n);
        cur += n;
    }
    *cur = '\0'; // Ensure final buffer is null-terminated
    return buf;
} 