#include <stdio.h>
#include "main.h"
#include "string.h"


#define FILE_ERROR    -1

int main(int argc, char** argv){
    FILE *f;
//    if (argc == 1)
//    {
//        printf("Neni zadan vstupni soubor\n");
//        return FILE_ERROR;
//    }

    if ((f = fopen("C:\\Users\\Admin\\Desktop\\untitled7\\cmake-build-debug\\CMakeFiles\\IFJcode", "r")) == NULL)
    {
        printf("Soubor se nepodarilo otevrit\n");
        return FILE_ERROR;
    }else{
        printf("Soubor se podarilo otevrit\n");
    }

    int getNextToken(string *attr){
        return 0;

// hlavni funkce lexikalniho analyzatoru
        int state = 0;
        int c;
        // vymazeme obsah atributu a v pripade identifikatoru
        // budeme postupne do nej vkladat jeho nazev
        strClear(attr);
        while (1) {
            c = getc(source);               // nacteni dalsiho znaku
            switch (state) {
                case 0:                     // zakladni stav automatu
                    if (isspace(c))         // bila mista - ignorovat
                        state = 0;
                    else if (c == '_'){      // radkovy_komentar
                        strAddChar(attr, c);
                        state = 7;}
                    else if (c == '-')      //blokovy_komentar '--[['
                        state = 2;
                    else if (isalpha(c))    // identifikator nebo klicove slovo
                    {
                        if (c >= 'A' && c <= 'Z') return LEX_ERROR;
                        else {
                        strAddChar(attr, c);
                        state = 7;}
                    } else if (c == '+')        // operator +
                        state = 8;
                    else if (c == '*')          // operator *
                        state = 9;
                    else if (c == '/')          // operator /
                        state = 10;
                    else if (c == '{') return LEFT_VINCULUM;
                    else if (c == '}') return RIGHT_VINCULUM;
                    else if (c == ';') return SEMICOLON;
                    else if (c == '#') return HASH;
                    else if (c == '.') state = 12;
                    else if (c == '<') state = 13;
                    else if (c == '>') state = 14;
                    else if (c == '=') state = 15;
                    else if (c == '~') state = 16;
                    else if (c == '\"') state = 17;
                    else if (isdigit(c)) state = 18;

                    else if (c == EOF) return END_OF_FILE;
                    else
                        return LEX_ERROR;
                    break;

                case 1:
                    // komentar
                    if (c == '\n') state = 0;
                    break;

                case 2:
                    // pokracovani komentaru
                    if (c == '-') state = 3;
                    else if (isspase(c)){state = 0; return DEC;}
                    else
                        return LEX_ERROR;
                    break;

                case 3:
                    // pokracovani komentaru
                    if (c == '[') state = 4;
                    else state = 1;
                    break;

                case 4:
                    // pokracovani komentaru
                    if (c == '[') state = 5;
                    else state = 1;
                    break;

                case 5:
                    // komentar
                    if (c == ']') state = 6;
                        // komentar pokracuje libovolnymi znaky, zustan ve stavu 1,
                        // ale kontroluj, zda neprijde EOF (neukonceny komentar)
                    if (c == EOF) return LEX_ERROR;
                    break;

                case 6:
                    // komentar
                    if (c == ']') state = 0;  // --[[ ]]
                    else state = 5;
                    if (c == EOF) return LEX_ERROR;
                    break;

                case 7:
                    if (c >= 'A' && c <= 'Z') return LEX_ERROR;
                    // identifikator nebo klicove slovo
                    else if (isalnum(c) || c == '.')
                        // identifikator pokracuje
                        strAddChar(attr, c);
                    else
                        // konec identifikatoru
                    {
                        ungetc(c, source); // POZOR! Je potreba vratit posledni nacteny znak

                        // kontrola, zda se nejedna o klicove slovo
                        if (strCmpConstStr(attr, "do") == 0) {state = 0; return DO;}  //закончить потом
                        else if (strCmpConstStr(attr, "else") == 0) return ELSE;
                        else if (strCmpConstStr(attr, "end") == 0) return END;
                        else if (strCmpConstStr(attr, "function") == 0) return FUNCTION;
                        else if (strCmpConstStr(attr, "global") == 0) return GLOBAL;
                        else if (strCmpConstStr(attr, "if") == 0) return IF;
                        else if (strCmpConstStr(attr, "integer") == 0) return INTEGER;
                        else if (strCmpConstStr(attr, "local") == 0) return LOCAL;
                        else if (strCmpConstStr(attr, "nil") == 0) return NIL;
                        else if (strCmpConstStr(attr, "number") == 0) return NUMBER;
                        else if (strCmpConstStr(attr, "require") == 0) return REQUIRE;
                        else if (strCmpConstStr(attr, "return") == 0) return RETURN;
                        else if (strCmpConstStr(attr, "string") == 0) return STRING;
                        else if (strCmpConstStr(attr, "then") == 0) return THEN;
                        else if (strCmpConstStr(attr, "while") == 0) return WHILE;
                        else
                        {state = 0; return ID;}          // jednalo se skutecne o identifikator
                    }
                    break;

                case 8:
                    // pokracovani operatoru +
                    if (isspace(c)) return INC;
                    else
                        return LEX_ERROR;
                    break;

                case 9:
                    // pokracovani operatoru *
                    if (isspace(c)) return MULTIPLY;
                    else
                        return LEX_ERROR;
                    break;

                case 10:
                    // pokracovani operatoru /
                    if (c == '/') state = 11;
                    else if (isspace(c)) return DIV;
                    else
                        return LEX_ERROR;
                    break;

                case 11:
                    // pokracovani operatoru //
                    if (isspace(c)) return MOD;
                    else
                        return LEX_ERROR;
                    break;

                case 12:
                    // pokracovani operatoru ..
                    if (c == '.') return DOTDOT;
                    else
                        return LEX_ERROR;
                    break;

                case 13:
                    // pokracovani operatoru </<=
                    if (c == '=') return LESSOREQUAL;
                    else if (isspace(c)) return LESS;
                    else
                        return LEX_ERROR;
                    break;

                case 14:
                    // pokracovani operatoru >/>=
                    if (c == '=') return MOREOREQUAL;
                    else if (isspace(c)) return MORE;
                    else
                        return LEX_ERROR;
                    break;

                case 15:
                    // pokracovani operatoru ==
                    if (c == '=') return EQUAL;
                    else if (isspace(c)) return ASSIGNED;
                    else
                        return LEX_ERROR;
                    break;

                case 16:
                    // pokracovani operatoru ~=
                    if (c == '=') return NOTEQUAL;
                    else
                        return LEX_ERROR;
                    break;

                case 17:
                    // pokracovani operatoru ""
                    if (c == '\"') state = 0;
                    else
                        strAddChar(attr, c);
                    break;

                case 18:
                    if (isspace(c)) return INT;
                    else if (c == '.') state 19;
                    else if (!isdigit(c))
                        return LEX_ERROR;
                    strAddChar(attr, c)
                    break;

                case 19:
                    if (c == 'e' || c == 'E'){
                        state = 20;
                    }
                    else if (!isspace(c) && isdigit(c) && c != 'e' && c != 'E'){
                        return LEX_ERROR
                    }
                    else if (isspace(c)){state = 0}
                    strAddChar(attr, c)

                case 20:
                    if (!isspace(c) && !isdigit(c) && c != 'e' && c != 'E' && c != '+' && c != '-'){
                        return LEX_ERROR;
                    }
                    else if (isspace(c)){state = 0;}
                    strAddChar(attr, c)
                    break;

            }
        }
    };
    return 0;
}
