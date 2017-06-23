//
// Simple string parser
//

#ifndef PARSEH
#define PARSEH

enum tokentype {ASTRING, ANUMBER, OTHER,ENDOFSTRING, BAD, SPACE, LPAR, RPAR, COMMA, SEMICOLON};

// Prototype
tokentype NextToken (char *&CurPos, char *buf);

#endif
