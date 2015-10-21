#include "lexer.h"

char* currentString;
char c;
int i, currentLength = 0;
FILE *f;
int count_line = 1;
TokenType token, result, tokenTemp;
char* filepath;
char message[100];

void error(char* msg) {
    printf("\n");
    printf("Error at line %d: %s \n", count_line, msg);
    fclose(f);
    exit(0);
}

void init() {
    currentLength = 0;
    currentString = (char*) malloc(MAX_IDENT_LENGTH * sizeof (int));
}

void freePointer(char* pointer) {
    if (pointer != NULL) {
        free(pointer);
    }
}

int isIdent(char c) {
    return 1 ? (isalpha(c) || (c == '_')) : 0;
}

int isSpecialChar(char c) {
    int i;
    for (i = 0; i < LENGTH_SPECIAL_CHAR; i++) {
        if (c == specChar[i]) return 1;
    }
    return 0;
}

int isKey(char* s) {
    int i;

    for (i = 0; i < LENGTH_KEY_CHAR; i++) {
        if (strcmp(keyStr[i], s) == 0) return 1;
    }
    return 0;
}

int toKey(char* s) {
    int i;
    for (i = 0; i < LENGTH_KEY_CHAR; i++) {
        if (strcmp(keyStr[i], s) == 0) {
            return keys[i];
        }
    }
    return NONE;
}

char* keyToStr(TokenType key) {
    int i;
    for (i = 0; i < LENGTH_KEY_CHAR; i++) {
        if (key == keys[i]) {
            return keyStr[i];
        }
    }

    return NULL;
}

char* toLower(char* s) {
    char* copy;
    copy = (char*) malloc(MAX_IDENT_LENGTH * sizeof (char));
    int i = 0;
    while (s[i]) {
        copy[i] = tolower(s[i]);
        i++;
    }
    return copy;
}

char* toUpper(char* s) {
    char* copy;
    copy = (char*) malloc(MAX_IDENT_LENGTH * sizeof (char));
    int i = 0;
    while (s[i]) {
        copy[i] = toupper(s[i]);
        i++;
    }
    return copy;
}

void goBack(FILE* f) {
    fseek(f, -1, SEEK_CUR);
}

EnumType detectType(char c) {
    if (isIdent(c)) return IDENTCHAR;
    if (isSpecialChar(c)) return SPECIAL_CHAR;
    if (isdigit(c)) return DIGIT;
    if (isspace(c) || c == ' ') return SPACE;
    return NOT_DETECT;
}

TokenType getToken() {
    // start read a word
    while (!feof(f)) {
        c = fgetc(f);
        switch (detectType(c)) {
            case IDENTCHAR:
                init();
                currentString[0] = c;
                currentLength = 1;

                while (!feof(f)) {
                    c = fgetc(f);
                    if (detectType(c) == IDENTCHAR || detectType(c) == DIGIT) {
                        currentLength++;
                        currentString[currentLength - 1] = c;
                        if (currentLength > MAX_IDENT_LENGTH) {
                            error("IDENT exceed maximum of length");
                            return NONE;
                        }
                    } else {
                        goBack(f);
                        break;
                    }
                }

                if (isKey(currentString)) {
                    printf("KEY: %s \n", currentString);
                    return toKey(currentString);
                } else {
                    printf("IDENT: %s \n", currentString);
                    return IDENT;
                }

                break;

            case SPACE:
                if (c == '\n') {
                    count_line = count_line + 1;
                }
                while ((detectType(c) == SPACE) & (!feof(f))) {
                    c = (char) fgetc(f);
                    if (c == '\n') {
                        count_line = count_line + 1;
                    }
                }
                goBack(f);
                continue;
                break;

            case DIGIT:
                init();
                currentString[0] = c;

                c = fgetc(f);
                while (detectType(c) == DIGIT) {
                    c = (char) fgetc(f);
                    currentLength++;
                    currentString[currentLength] = c;
                }

                goBack(f);
                printf("NUMBER: %s \n", currentString);
                return NUMBER;
                break;

            case SPECIAL_CHAR:
                switch (c) {
                    case '*': result = TIMES;
                        break;
                    case ';': result = SEMICOLON;
                        break;
                    case ',': result = COMMA;
                        break;
                    case '[': result = LBRACK;
                        break;
                    case ']': result = RBRACK;
                        break;
                    case '(': result = LPARENT;
                        break;
                    case ')': result = RPARENT;
                        break;
                    case '.': result = PERIOD;
                        break;

                    case '=': result = EQU;
                        break;

                    case ':':
                        c = fgetc(f);
                        if (c == '=') {
                            result = ASSIGN;
                        } else {
                            error(" Can not define ':' syntax ");
                            result = NONE;
                            goBack(f);
                        }
                        break;

                    case '>':
                        c = fgetc(f);
                        if (c == '=') {
                            result = GEQ;
                        } else {
                            goBack(f);
                            result = GTR;
                        }
                        break;

                    case '+':
                        result = PLUS;
                        break;

                    case '-':
                        result = MINUS;
                        break;

                    case '%':
                        result = PERCENT;
                        break;

                    case '<':
                        c = fgetc(f);
                        if (c == '>') {
                            result = NEQ;
                        } else {
                            goBack(f);
                            result = LSS;
                        }
                        break;

                    case '/':
                        result = SLASH;
                        break;
                }

                return result;
                break;

            case NOT_DETECT:
                sprintf(message, "Can not detect special char: %c", c);
                error(message);
                break;
        }
    }
}

void factor() {
    switch (token) {
        case IDENT:
            token = getToken();
            if (token == LBRACK) {
                token = getToken();
                expression();
                if (token != RBRACK) {
                    error("Expected: ]");
                } else {
                    token = getToken();
                }
            }
            break;

        case NUMBER:
            token = getToken();
            break;

        case LPARENT:
            token = getToken();
            expression();
            if (token != RPARENT) {
                error("Expected: )");
            } else {
                token = getToken();
            }
    }
}

void term() {
    factor();
    while (token == TIMES || token == SLASH) {
        token = getToken();
        factor();
    }
}

void expression() {
    if (token == PLUS || token == MINUS) {
        token = getToken();
    }
    term();
    while (token == PLUS || token == MINUS) {
        token = getToken();
        term();
    }

}

void condition() {
    if (token == ODD) {
        token = getToken();
        expression();
    } else {
        expression();
        if (token == LEQ || token == NEQ || token == LSS ||
                token == EQU || token == GTR || token == GEQ) {
            token = getToken();
            expression();
        } else {
            error("condition: syntax error");
        }
    }
}

void statement() {
    switch (token) {
        case IDENT:
            token = getToken();
            if (token == LBRACK) {
                token = getToken();
                expression();
                if (token != RBRACK) {
                    error("Expected: ]");
                } else {
                    token = getToken();
                }
            }

            if (token == ASSIGN) {
                token = getToken();
                expression();
            } else {
                error("Expected: :=");
            }
            break;

        case CALL:
            token = getToken();
            if (token == IDENT) {
                token = getToken();
                if (token == LPARENT) {

                    tokenTemp = COMMA;
                    while (tokenTemp == COMMA) {
                        token = getToken();
                        expression();
                        tokenTemp = token;
                    }

                    if (token != RPARENT) {
                        error("Expected: )");
                    } else {
                        token = getToken();
                    }
                }
            } else {
                error("Expected: IDENT");
            }
            break;

        case BEGIN:
            token = getToken();
            statement();

            while (token == SEMICOLON) {
                token = getToken();
                statement();
            }

            if (token == END) {
                token = getToken();
            } else {
                error("Expected: END");
            }
            break;

        case IF:
            token = getToken();
            condition();

            if (token == THEN) {
                token = getToken();
                statement();

                if (token == ELSE) {
                    token = getToken();
                    statement();
                }
            } else {
                error("Expected: THEN");
            }
            break;

        case WHILE:
            token = getToken();
            condition();
            if (token == DO) {
                token = getToken();
                statement();
            } else {
                error("Expected: DO");
            }
            break;


        case FOR:
            token = getToken();
            if (token == IDENT) {
                token = getToken();

                if (token == ASSIGN) {
                    token = getToken();
                    expression();

                    if (token == TO) {
                        token = getToken();
                        expression();

                        if (token == DO) {
                            token = getToken();
                            statement();
                        } else {
                            error("Expected: DO");
                        }
                    } else {
                        error("Expected: To");
                    }
                } else {
                    error("Expected: ASSIGN");
                }
            } else {
                error("Expected: IDENT");
            }
            break;
    }
}

void blockBEGIN() {
    token = getToken();
    statement();

    while (token == SEMICOLON) {
        token = getToken();
        statement();
    }

    if (token != END) {
        error("Expected: END");
    } else {
        token = getToken();
    }
}

void blockPROCEDURE() {
    token = getToken();
    if (token == IDENT) {
        token = getToken();

        if (token == LPARENT) {
            tokenTemp = SEMICOLON;

            while (tokenTemp == SEMICOLON) {
                token = getToken();
                if (token == VAR) {
                    // do nothing
                    token = getToken();
                }
                if (token == IDENT) {
                    token = getToken();
                    tokenTemp = token;
                } else {
                    error("Expected: IDENT");
                }
            }

            if (token == RPARENT) {
                token = getToken();
            } else {
                error("Expected: )");
            }
        }

        if (token = SEMICOLON) {
            token = getToken();
            block();
            if (token == SEMICOLON) {
                token = getToken();
                switch (token) {
                    case PROCEDURE: blockPROCEDURE();
                        break;
                    case BEGIN: blockBEGIN();
                        break;
                    default: error("Expected: PROCEDURE OR BEGIN");
                        break;
                }

            } else {
                error("Expected: ;");
            }
        } else {
            error("Expected: ;");
        }
    } else {
        error("Expected: IDENT");
    }
}

void blockVAR() {
    tokenTemp = COMMA;

    while (tokenTemp == COMMA) {
        token = getToken();
        if (token == IDENT) {
            token = getToken();
            if (token == LBRACK) {
                token = getToken();
                if (token == NUMBER) {
                    token = getToken();
                    if (token != RBRACK) {
                        error("Expected: ]");
                    } else {
                        token = getToken();
                    }
                } else {
                    error("Expected: NUMBER");
                }
            }

            tokenTemp = token;
        } else {
            error("Expected: IDENT");
        }
    }

    if (tokenTemp == SEMICOLON) {
        token = getToken();
        switch (token) {
            case PROCEDURE: blockPROCEDURE();
                break;
            case BEGIN: blockBEGIN();
                break;
            default: error("Expected: PROCEDURE, BEGIN");
                break;
        }
    } else {
        error("Expected: ;");
    }
}

void blockCONST() {
    tokenTemp = COMMA;

    while (tokenTemp == COMMA) {
        token = getToken();
        if (token == IDENT) {
            token = getToken();
            if (token == EQU) {
                token = getToken();
                if (token == NUMBER) {
                    token = getToken();
                    tokenTemp = token;
                } else {
                    error("Expected: NUMBER");
                }
            } else {
                error("Expected: = ");
            }
        } else {
            error("Expected: IDENT");
        }
    }

    if (token == SEMICOLON) {
        token = getToken();

        switch (token) {
            case VAR:
                blockVAR();
                break;

            case PROCEDURE:
                blockPROCEDURE();
                break;

            case BEGIN:
                blockBEGIN();
                break;
            default: error("Expected: VAR, PROCEDURE, BEGIN");
                break;
        }
    } else {
        error("Expected: ;");
    }
}

void block() {
    switch (token) {
        case CONST: blockCONST();
            break;
        case VAR: blockVAR();
            break;
        case PROCEDURE: blockPROCEDURE();
            break;
        case BEGIN: blockBEGIN();
            break;
        default: error("Expected: CONST, VAR, PROCEDURE or BEGIN");
            break;
    }
}

void program() {
    if (token == PROGRAM) {
        token = getToken();
        if (token == IDENT) {
            token = getToken();
            if (token == SEMICOLON) {
                token = getToken();
                block();
                if (token == PERIOD) {
                    printf("\n||===========================||");
                    printf("\n||    Parsed succesfully !   ||");
                    printf("\n||===========================||");
                } else {
                    error("Expected: . ");
                }
            } else {
                error("Expected: ;");
            }
        } else {
            error("Expected: Program does not have a name");
        }
    } else {
        error("Expected: Program");
    }

}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Usage: ./lexer  [filepath] \n");
    } else {
        filepath = argv[1];
        init();
        if (!(f = fopen(filepath, "r"))) {
            return -1;
        }
        token = getToken();
        program();
        printf("\n");
        fclose(f);
    }   
}