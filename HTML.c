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