/* 
 * File:   lexer.h
 * Author: supercoder
 *
 * Created on October 21, 2015, 8:06 AM
 */
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    NONE = 0, IDENT, NUMBER, BEGIN, CALL, CONST, DO, ELSE, END, FOR, IF, ODD, PROCEDURE,
    PROGRAM, THEN,TO, VAR, WHILE, PLUS, MINUS, TIMES, SLASH, EQU,NEQ, LSS, LEQ, GTR,
    GEQ, LPARENT, RPARENT, LBRACK, RBRACK, PERIOD, COMMA, SEMICOLON, ASSIGN,
    PERCENT
} TokenType;

int LENGTH_KEY_CHAR = 15;

TokenType keys[] = {
    BEGIN, CALL, CONST, DO, ELSE, END, FOR, IF, ODD, PROCEDURE, 
    PROGRAM, THEN, TO, VAR, WHILE    
};

char* keyStr[] = {"Begin", "Call", "Const", "Do", "Else", "End", "For", 
     "If", "Odd", "Procedure", "Program", "Then", "To", "Var", "While"};         

int LENGTH_SPECIAL_CHAR = 18;
char specChar[] = {'+', '-', '*', '/', '=', '>', '<', '%', ':', '[', ']', '{', '}', '(', ')', ',', ';', '.'};

int MAX_IDENT_LENGTH = 100;

typedef enum {
    SPACE, IDENTCHAR, DIGIT, SPECIAL_CHAR, NOT_DETECT
} EnumType;

TokenType getToken();

void error(char* msg);

void factor(void);

void term(void);

void expression(void);

void condition(void);

void statement(void);

void block(void);

void program();

