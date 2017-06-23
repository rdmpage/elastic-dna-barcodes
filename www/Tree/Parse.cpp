//
//Simple string parser
//

#include <iostream.h>
#include <string.h>
#include <ctype.h>
#include "parse.h"

//Return the next token in a string starting from CurPos
tokentype NextToken (char *&CurPos, char *buf)
{
	char ch;
	int i;
	tokentype t;

	ch = *CurPos;
	if (isalpha (ch))
	{
		i = 0;
		while (isalpha (ch)) //read alphabetic token
		{
			buf[i++] = ch;
			CurPos++;
			ch = *CurPos;
		}
          buf[i] = '\0'; //null-terminate the token
		t = ASTRING; //classify the token
	}
	else
	{
		if (isdigit(ch))
		{
			i = 0;
			while (isdigit (ch)) //read numeric token
			{
				buf[i++] = ch;
				CurPos++;
				ch = *CurPos;
			}
	          buf[i] = '\0'; //null-terminate the token
			t = ANUMBER; //classify the token
		}
		else
		{
			if (ch=='\0')
			{
				t = ENDOFSTRING;
			}
			else
               {
				buf[0] = ch;
				buf[1] = '\0';
				CurPos++;
				switch (ch)
				{
					case '(': t = LPAR; 	break;
					case ')': t = RPAR; 	break;
					case ' ': t = SPACE; 	break;
					case ',': t = COMMA; 	break;
					case ';': t = SEMICOLON; break;
					default:  t = BAD; 		break;
	               }
			}
		}
	}
	return (t);
}

