#ifndef TOKEN_H

#include <stdio.h>;

#define OK 0;
#define ERROR 1;

typedef enum {
	S_start, 					//initial
	S_commentOrDiv, 			// first occurence of /, we need to decide if it's comment or divison
	S_commentBlock,
	S_commentBlockStar,			
	S_commentLine,  			// beggining of block comment /*
	S_lessOrLessEqual,			// < or <=
	S_greaterOrGreaterEqual,	// > or >=
	S_assignOrEqual,			// = or ==
	S_or,						// ||
	S_and,						// &&	
	S_string,
	S_stringBackslash,
	S_int,
	S_double1,
	S_double2,
	S_doubleExp,
	S_doubleExp2,
	S_doubleExp3,
} T_state;

void set_file(FILE *f);
int get_token(string *attr);

#define TOKEN_H