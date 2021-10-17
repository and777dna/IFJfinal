#include <stdio.h>
#include "main.h"
#include "string.h"
#include "ctype.h"
#include "malloc.h"


#define FILE_ERROR    -1


#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla

#define STR_ERROR   1
#define STR_SUCCESS 0

FILE* source;

int strInit(string *s)
// funkce vytvori novy retezec
{
    if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
        return STR_ERROR;
    s->str[0] = '\0';
    s->length = 0;
    s->allocSize = STR_LEN_INC;
    return STR_SUCCESS;
}

void strFree(string *s)
// funkce uvolni retezec z pameti
{
    free(s->str);
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
    s->str[0] = '\0';
    s->length = 0;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
    if (s1->length + 1 >= s1->allocSize)
    {
        // pamet nestaci, je potreba provest realokaci
        if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
            return STR_ERROR;
        s1->allocSize = s1->length + STR_LEN_INC;
    }
    s1->str[s1->length] = c;
    s1->length++;
    s1->str[s1->length] = '\0';
    return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2)
// prekopiruje retezec s2 do s1
{
    int newLength = s2->length;
    if (newLength >= s1->allocSize)
    {
        // pamet nestaci, je potreba provest realokaci
        if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
            return STR_ERROR;
        s1->allocSize = newLength + 1;
    }
    strcpy(s1->str, s2->str);
    s1->length = newLength;
    return STR_SUCCESS;
}

int strCmpString(string *s1, string *s2)
// porovna oba retezce a vrati vysledek
{
    return strcmp(s1->str, s2->str);
}

int strCmpConstStr(string *s1, char* s2)
// porovna nas retezec s konstantnim retezcem
{
    return strcmp(s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
    return s->str;
}

int strGetLength(string *s)
// vrati delku daneho retezce
{
    return s->length;
}

int getNextToken(string *attr){
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
                else if (c == ':') return COLUMN;
                else if (c == '#') return HASH;
                else if (c == '.') state = 12;
                else if (c == '<') state = 13;
                else if (c == '>') state = 14;
                else if (c == '=') state = 15;
                else if (c == '~') state = 16;
                else if (c == '"') {state = 17; return PARANTACIES;}
                else if (isdigit(c)) state = 18;
                else if (c == '(') return LEFT_BRACKET;
                else if (c == ')') return RIGHT_BRACKET;
                else if (c == ',') return COMMA;

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
                else if (isspace(c)){state = 0; return DEC;}
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
                if ((c >= 'A' && c <= 'Z') && c != ')') {return LEX_ERROR;}
                    // identifikator nebo klicove slovo
                else if (isalnum(c) || c == '.')
                    // identifikator pokracuje
                    strAddChar(attr, c);
                else
                    // konec identifikatoru
                {
                    ungetc(c, source); // POZOR! Je potreba vratit posledni nacteny znak

                    // kontrola, zda se nejedna o klicove slovo
                    if (strCmpConstStr(attr, "do") == 0) {state = 0; return DO;}
                    else if (strCmpConstStr(attr, "else") == 0) {state = 0; return ELSE;}
                    else if (strCmpConstStr(attr, "end") == 0) {state = 0; return END;}
                    else if (strCmpConstStr(attr, "function") == 0) {state = 0; return FUNCTION;}
                    else if (strCmpConstStr(attr, "global") == 0) {state = 0; return GLOBAL;}
                    else if (strCmpConstStr(attr, "if") == 0) {state = 0; return IF;}
                    else if (strCmpConstStr(attr, "integer") == 0) {state = 0; return INTEGER;}
                    else if (strCmpConstStr(attr, "local") == 0) {state = 0; return LOCAL;}
                    else if (strCmpConstStr(attr, "nil") == 0) {state = 0; return NIL;}
                    else if (strCmpConstStr(attr, "number") == 0) {state = 0; return NUMBER;}
                    else if (strCmpConstStr(attr, "require") == 0) {state = 0; return REQUIRE;}
                    else if (strCmpConstStr(attr, "return") == 0) {state = 0; return RETURN;}
                    else if (strCmpConstStr(attr, "string") == 0) {state = 0; return STRING;}
                    else if (strCmpConstStr(attr, "then") == 0) {state = 0; return THEN;}
                    else if (strCmpConstStr(attr, "while") == 0) {state = 0; return WHILE;}
                    else
                    {state = 0; return ID;}          // jednalo se skutecne o identifikator
                }
                break;

            case 8:
                // pokracovani operatoru +
                if (isspace(c)) {state = 0; return INC;}
                else if (c == '(' || c == ',' || c == ')'){
            ungetc(c, source);
            state = 0;
            return INC;
        }
        else
            return LEX_ERROR;
                break;

            case 9:
                // pokracovani operatoru *
                if (isspace(c)) {state = 0; return MULTIPLY;}
                else if (c == '(' || c == ',' || c == ')'){
            ungetc(c, source);
            state = 0;
            return MULTIPLY;
        }
        else
            return LEX_ERROR;
                break;

            case 10:
                // pokracovani operatoru /
                if (c == '/') state = 11;
                else if (isspace(c)) {state = 0; return DIV;}
                else if (c == '(' || c == ',' || c == ')'){
            ungetc(c, source);
            state = 0;
            return DIV;
        }
        else
            return LEX_ERROR;
                break;

            case 11:
                // pokracovani operatoru //
                if (isspace(c)) {state = 0; return MOD;}
                else if (c == '(' || c == ',' || c == ')'){
            ungetc(c, source);
            state = 0;
            return MOD;
        }
        else
            return LEX_ERROR;
                break;

            case 12:
                // pokracovani operatoru ..
                if (c == '.') {state = 0; return DOTDOT;}
                else
                return LEX_ERROR;
                break;

            case 13:
                // pokracovani operatoru </<=
                if (c == '=') {state = 0; return LESSOREQUAL;}
                else if (isspace(c)) return LESS;
                else if (c == '(' || c == ',' || c == ')'){
                    ungetc(c, source);
                    state = 0;
                    return LESS;
                }
                else
                    return LEX_ERROR;
                break;

            case 14:
                // pokracovani operatoru >/>=
                if (c == '=') {state = 0; return MOREOREQUAL;}
                else if (isspace(c)) return MORE;
                else if (c == '(' || c == ',' || c == ')'){
                    ungetc(c, source);
                    state = 0;
                    return MORE;
                }
                else
                    return LEX_ERROR;
                break;

            case 15:
                // pokracovani operatoru ==
                if (c == '=') {state = 0; return EQUAL;}
                else if (isspace(c)) return ASSIGNED;
                else if (c == '(' || c == ',' || c == ')'){
                    ungetc(c, source);
                    state = 0;
                    return ASSIGNED;
                }
                else
                    return LEX_ERROR;
                break;

            case 16:
                // pokracovani operatoru ~=
                if (c == '=') {state = 0; return NOTEQUAL;}
                else
                    return LEX_ERROR;
                break;

            case 17:
                // pokracovani operatoru ""
                if (c == '\"') {state = 0; return PARANTACIES;}
                else if (c == ')' || c == ',' || c == ')'){
                    ungetc(c, source);
                    state = 0;
                    return RETEZEC;
                }
                else
                    strAddChar(attr, c);
                break;

            case 18:
                if (isspace(c)) return INT;
                else if (c == '.') state = 19;
                else if (c == ')' || c == ',' || c == ')'){
            ungetc(c, source);
            state = 0;
            return INT;
        }
        else if (!isdigit(c))
            return LEX_ERROR;
                strAddChar(attr, c);
                break;

            case 19:
                if (c == 'e' || c == 'E'){
                    state = 20;
                }
                else if (c == ')' || c == ',' || c == ')'){
                    ungetc(c, source);
                    state = 0;
                    return FLOAT;
                }
                else if (!isspace(c) && isdigit(c) && c != 'e' && c != 'E'){
                    return LEX_ERROR;
                }
                else if (isspace(c)){state = 0; return FLOAT;}
                strAddChar(attr, c);
                break;

            case 20:
                if (!isspace(c) && !isdigit(c) && c != '+' && c != '-' && c != ')'){
                    return LEX_ERROR;
                }
                else if (c == ')' || c == ',' || c == ')'){
                    ungetc(c, source);
                    state = 0;
                    return FLOAT;
                }
                else if (isspace(c)){state = 0; return FLOAT;}
                strAddChar(attr, c);
                break;

        }
    }
}

int main(int argc, char** argv){

    if ((source = fopen("C:\\Users\\Admin\\Desktop\\untitled7\\ifj21.tl", "r")) == NULL)
    {
        printf("Soubor se nepodarilo otevrit\n");
        return FILE_ERROR;
    }else{
        printf("Soubor se podarilo otevrit\n");
    }
//    if (argc == 1)
//    {
//        printf("Neni zadan vstupni soubor\n");
//        return FILE_ERROR;
//    }

    string *token;
    strInit(token);
    int a;

    for(int i = 0; i < 30; i++){
        a = getNextToken(token);
        printf("%d  :  %s\n",a, *token);
        //printf("%s\n",*token);
    }
    free(token);

    return 0;
}
