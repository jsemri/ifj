#include <string.h>
#include <stdlib.h>
#include "string.h"

// allocates space for buffer
char* str_init(T_string *s) {
    s->space = 2;
    s->length = 0;
    return (s->buf = calloc(2, 1));
}

// adds character at the end of string
int str_addchar(T_string *dest, const char c) {
    if (dest->space > dest->length + 1) {
        dest->buf[dest->length] = c;
        dest->buf[dest->length+1] = 0;
        dest->length++;
        return c;
    }
    else {
        // reallocation if needed
        void *ptr = realloc(dest->buf , 2*dest->space);
        if (!ptr)
            return -1;
        dest->buf = ptr;
        dest->space *= 2;
        return str_addchar(dest, c);
    }
}

// adds char* at the end of string
int str_addstr(T_string *dest, const char *src) {
    unsigned len = strlen(src);
    if (dest->space > dest->length + len) {
        strcpy(dest->buf+dest->length, src);
        dest->length += strlen(src);
        return 1;
    }
    else {
        // reallocation if needed
        void *ptr = realloc(dest->buf, dest->space + len);
        if (!ptr)
            return -1;
        dest->buf = ptr;
        dest->space += len;
        strcpy(dest->buf + dest->length, src);
        dest->length += len;
        return 1;
    }
}

// adds string at the end of string
int str_catstr(T_string *dest, const T_string *src) {
    // same string
    if (dest == src) {
        char *buf = calloc(dest->length + 1, 1);
        if (!buf)
            return -1;
        strncpy(buf, dest->buf, dest->length );
        int res = str_addstr(dest, buf);
        free(buf);
        return res;
    }
    return str_addstr(dest, src->buf);
}

// return length of string
unsigned str_length(const T_string *s) {
    return s->length;
}

// compares two strings
int str_cmp(const T_string *s1, const T_string *s2) {
    return strcmp(s1->buf, s2->buf);
}

// copies src to dest
int str_copy(T_string *dest,const T_string *src) {
    if (src != dest) {
        str_clear(dest);
        return str_catstr(dest, src);
    }
    return 1;
}

// clears buffer
void str_clear(T_string *str) {
    memset(str->buf, 0, str->length);
    str->length = 0;
}

// removes string buffer
void str_free(T_string *str) {
    free(str->buf);
    str->buf = NULL;
    str->space = 0;
    str->length = 0;
}

