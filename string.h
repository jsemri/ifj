/*
 * string.h
 *
 * File contains structure T_string and functions needed for strings.
 *
 */

#ifndef STRING_H
#define STRING_H


typedef struct {
    char *buf;
    unsigned length;  // actual length of string
    unsigned space; // allocated space
} T_string;

// allocates a space for buffer
char *str_init(T_string *s, unsigned space);
// adds character at the end of string
int str_addchar(T_string *dest, char c);
// adds string at the end of string
int str_catstr(T_string *dest, const T_string *src);
// adds char* at the end of string
int str_addstr(T_string *dest, const char *src);
// return length of string
unsigned str_length(const T_string *src);
// compares two strings
int str_cmp(const T_string *s1, const T_string *s2);
// copies src to dest
int str_copy(const T_string *src, T_string *dest);
// clears buffer
void str_clear(T_string *s);
// removes buffer
void str_free(T_string *s);

#endif
