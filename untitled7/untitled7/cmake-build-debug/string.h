//
// Created by Admin on 12/10/2021.
//

typedef struct
{
    char* str;		// misto pro dany retezec ukonceny znakem '\0'
    int length;		// skutecna delka retezce
    int allocSize;	// velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

int getNextToken(string *attr);


char *strGetStr(string *s);
int strGetLength(string *s);
