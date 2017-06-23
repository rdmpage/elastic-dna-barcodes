// $Log: Scan.cpp,v $
// Revision 1.9  2001/09/03 11:37:09  rdmp1c
// Fixed minor bug that caused TreeView to not store the echoed
// comments in a NEXUS file
//
// Revision 1.8  2001/09/03 11:12:18  rdmp1c
// Fixed mixed bug in ParseNumber caused by labels having
// a combination of numbers and letters.
//
// Revision 1.7  2001/01/28 22:43:13  rdmp1c
// ParseNumber now uses finite state machine to recognise

// numbers (fixes bug introduced by trying to fix TreeView).
//
// Revision 1.6  2000/08/07 15:52:58  rdmp1c
// Logging fixed for Mac version (#ifdef so Mac version stores
// \r when echoing comments)
//
// Revision 1.5  2000/08/07 15:42:59  rdmp1c
// Restored output of [! comments to log window for GeneTree
//
// Revision 1.4  2000/06/17 01:52:08  rdmp1c
// NDE and TreeView versions got out of sync, leading to a bug in

// ParseNumber. This code fixes this.
//
//

/***************************************************************************
*                                                                          *
*  	COMPONENT C++ translation                                             *
*                                                                          *
*  	(c) 1993, Roderic D. M. Page                                          *
*      All rights reserved                                                 *
*                                                                          *
*                                                                          *
*  	Compiler:  Borland Turbo C++ for Windows 3.1                          *
*  	Platform:  Microsoft Windows 3.1                                      *
*                                                                          *
*        File: scan.cpp                                                    *
*	 Purpose: a simple lexical scanner                                    *
*     History: 15 June 1993                                                *
* 	          27 Oct 1993 C++ streams skip CR so when echoing comments    *
* 	                      in Windows there are no line breaks. Fixed      *
* 	                      by inserting CR is stored string.               *
* 	                                                                      *
***************************************************************************/
/*

15 Jun 1993 Written


*/

#include <ctype.h>
#include "Scan.h"

#ifdef VCLASS
	#include "VText.h"
	extern VTextWindowPtr Log; // Global log window
#endif


Scanner::~Scanner ()
{
	if (lpszComment)
     {
#ifdef VIBRANT
		MemFree (lpszComment);
#else
		delete lpszComment;
#endif
	}
}

//------------------------------------------------------------------------------
// Return a pointer to the comments (if any) and rest lpszComment to
// NULL. Calling code has responsibility for disposing of comments.
char *Scanner::GetComments ()
{
	if (!lpszComment) return NULL;
	else
	{
		char *temp = lpszComment;
		lpszComment = NULL;
		return (temp);
	}
}


//------------------------------------------------------------------------------
// Return the next token in the input stream, ttNULL (=0) if
// unsuccessful. The returnspace parameter is optional (default = 0)
tokentypes Scanner::GetToken (int returnspace)
{
#ifdef NEWSCAN
	token = ttNULL;
	while ((token == ttNULL) && (s->get (ch)))
	{
		if (strchr (NEXUSWhiteSpace, ch))
		{
			// white space
           	pos++;
#ifdef Windows
			// 28/12/97
            // Windows C++ disk streams have only \n at the end of the line so we
            // need to increment the pos counter to include the \r\n that is actually
            // present in the disk file. However, but a strstream derived from an
            // edit window will have \r\n so we decrement the counter to avoid
            // double counting the \r.
			if (ch == '\n')
            	pos++;
			if (ch == '\r')
           		pos--;
#endif
			if (returnspace && (ch == ' ')) token = ttSPACE;
		}
		else
		{
			// We don't have white space
			if (strchr (NEXUSPunctuation, ch))
			{
            	pos++;
				// classify punctuation token
				switch (ch)
				{
					case '[': ParseComment (); break;
					case '\'':
						if (ParseString ())
							token = ttIDENTIFIER;
						else token = ttBAD;
						break;
					case '(':
						token = ttOPENPAR;
						break;
					case ')':
						token = ttCLOSEPAR;
						break;
					case '=':
						token = ttEQUALS;
						break;
					case ';':
						token = ttSEMICOLON;
						break;
					case ',':
						token = ttCOMMA;
						break;
					case '*':
						token = ttASTERIX;
						break;
					case ':':
						token = ttCOLON;
						break;
					case '-':
						token = ttMINUS;
						break;
					case '"':
						token = ttDOUBLEQUOTE;
						break;
                   case '/':
                    	token = ttBACKSLASH;
                        break;
					default:
						token = ttOTHER;
						break;
				}
			}
			else
			{
#ifdef NOHASH
				// A token (identifier or number)
				if (isdigit (ch))
				{
					token = ParseNumber ();
				}
				else
				{
					if (ParseToken ())
					{
						token = ttIDENTIFIER;
					}
					else token = ttBAD;
				}
#else
				// a token, could be identifier or a number or a symbol (e.g. #)
				if (ch == '#')
					token = ttHASH;
                else if (ch == '.')
                	token = ttPERIOD;
				else
				{
					if (isdigit (ch))
					{
						if (!ParseNumber ()) token = ttBAD;
					}
					else
					{
						if (ParseToken ())
						{
							token = ttIDENTIFIER;
						}
						else token = ttBAD;
					}
				}
#endif
			}
		}
	}
	// 9 July 1996
	if (token != ttIDENTIFIER && token != ttNUMBER)
	{
		// Store the single character token
		buf[0] = ch;
		buf[1] = '\0';
	}

	SaveToken (buf); // save token for use in error messages

	return token;
#else

	token = ttNULL;
	while ((token == ttNULL) && (s->get (ch)))
	{

		// First check for letters
		if (isalpha (ch))
		{
			if (ParseToken ())
				token = ttIDENTIFIER;
			else token = ttBAD;
		}
		// then for numbers
		else
		{
			if (isdigit (ch))
			{
				if (ParseNumber ())
					token = ttNUMBER;
				else token = ttBAD;
			}
			// and lastly single character tokens and strings
			else
			{
           	    // Update position in file
			    pos++;

				switch (ch)
				{
					case ' ':
						// Only return space if user asks for it
						if (returnspace) token = ttSPACE;
						break;
					case '\t':break; // eat tabs
					case '\n':break; // eat newline
					case '\r':break; // eat newline
					case '[': ParseComment (); break;
					case '\'':
						if (ParseString ())
							token = ttIDENTIFIER;
						else token = ttBAD;
						break;
					case '(':
						token = ttOPENPAR;
						break;
					case ')':
						token = ttCLOSEPAR;
						break;
					case '=':
						token = ttEQUALS;
						break;
					case '#':
						token = ttHASH;
						break;
					case ';':
						token = ttSEMICOLON;
						break;
					case ',':
						token = ttCOMMA;
						break;
					case '*':
						token = ttASTERIX;
						break;
					case ':':
						token = ttCOLON;
						break;
					case '-':
						token = ttMINUS;
						break;
 					default:
						token = ttOTHER;
						break;

				}
			}
		}
	}
	SaveToken (buf); // save token for use in error messages
	return (token);

#endif
}

//------------------------------------------------------------------------------
// Parse a comment
int Scanner::ParseComment ()
{
	int echo = 0;
	if (!s->get (ch)) return (0);
	else
	{
    	pos++;
 //		if (ch == ']') return (1); // allow for []
 //		else echo = (ch == '!');   // check for output flag
 		switch (ch)
        {
        	case ']':
            	return 1;
                break;

            case '!':
            	echo = 1;
                break;

            case '\\':
                s->get(ch);
                switch (ch)
                {
                	case 'i':
                    	curFormat |= italicFace;
                        break;
                	case 'b':
                    	curFormat |= boldFace;
                        break;
                	case 'u':
                    	curFormat |= underlineFace;
                        break;
                	case 'p':
                    	curFormat = plainFace;
                        break;
                }
                s->get (ch);
                if (ch != ']')
                	return 0;
                else
                	return 1;

                break;

            default:
            	break;
        }
	}
	// Store the comments in a buffer for later access
	int count = 0;

	if (echo)
	{
		lpszComment = new char[COMMENT_BUFFER_SIZE];
		lpszComment[0] = '\0';
	}

	while ((s->get (ch)) && (ch != ']'))
	{
#ifdef Macintosh
		if (ch == '\n')
			ch = '\r';
#endif
#ifdef Windows
		// 27 Oct 1993
		// Windows expects lines to be terminated with CR/LF,
		// but C++ streams return only LF from text files so
		// insert CR before LF.
#ifndef NDE
		if (echo && (ch == '\n') && (count < (COMMENT_BUFFER_SIZE) - 3))
		{
			// Glasgow: but if we do this the comments have extra lines
			// when written to disk file
			// 20/1/97
			strcat (lpszComment, "\r");
			count++;
            pos++;
 		}
#endif
#endif
		if (echo && count < COMMENT_BUFFER_SIZE - 2)
		{
#if defined (NDE) || defined (GENETREE) || defined (TREEVIEW)
#ifdef Windows
			if (ch != '\r')
#endif
            {
                char thechar[2];
                thechar[0] = ch;
                thechar[1] = '\0';
                strcat (lpszComment, thechar);
                count++;
            }
#endif            
    	}

	    // Update position in file
    	pos++;

	}
    pos++; // Count the ']'

#ifndef VCLASS
	if (echo)
		cout << lpszComment << "\n";
#endif
#ifdef VCLASS
#ifndef NDE
	if (Log && echo)
		*Log << lpszComment << EOLN;
#endif
#endif

     return (1);
}

//------------------------------------------------------------------------------
//Parse a string enclosed in single quotes, return 0 if unsuccessful.
//String is stored in buf
int Scanner::ParseString ()
{
	int done = 0;
	bufpos 	= 0;
	buf[0] 	= '\0';
	while ((!done) && (s->get(ch)) && (bufpos < BUFFER_SIZE))
	{
		if (ch=='\'')
		{
			// Check if next character is '
			s->get(ch);
			done = (ch != '\'');
		}
		if (!done && ch != '\n' && ch != '\r') // 11/1/96 Handle word-wrapped emailed files from RDP
			buf[bufpos++] = ch;
	}
	buf[bufpos] = '\0';
	s->putback(ch);
	return (done);
}

//------------------------------------------------------------------------------
// Parse a token
int Scanner::ParseToken ()
{
#ifdef NEWSCAN
	// Handle NEXUS tokens better
	bufpos = 0;

	buf[bufpos] = ch;
    format[bufpos] = curFormat;
    bufpos++;

	while ((s->get(ch))
		&& (strchr (NEXUSWhiteSpace, ch) == NULL)
		&& (strchr (NEXUSPunctuation, ch)== NULL)
		&& (bufpos < BUFFER_SIZE))
	{
		buf[bufpos] = ch;
        format[bufpos] = curFormat;
        bufpos++;
	}
	buf[bufpos] = '\0';
	s->putback (ch);

    // Update position in file
    pos += bufpos;

    // 7 Jan 1999 Per de Place Bjorn
    // Check for unterminated quote. If punctuation that ends this token
    // is a single quote ' then we have an error as this token should
    // have started with a quote.
    if (ch == '\'')
    	return 0;


	return (1);
#else
	bufpos = 0;
	buf[bufpos++] = ch;
	while ((s->get(ch)) &&
		((isalpha (ch)) || (isdigit(ch)) || (ch == '_') || (ch == '.'))
		&& (bufpos < BUFFER_SIZE))
	{
		buf[bufpos++] = ch;
	}
	buf[bufpos] = '\0';
	s->putback (ch);
	return (1);
#endif
}

//------------------------------------------------------------------------------
//Parse a positive number (real or integer)
tokentypes Scanner::ParseNumber ()
{
#if 1
	tokentypes result = ttBAD;
	
	// New code that parses numbers (including exponents) and checks for syntax
    // errors. Added 28 Jan 2001.
	enum {
		start		= 0x0001, // 0
		sign		= 0x0002, // 1
        digit		= 0x0004, // 2
		fraction	= 0x0008, // 3
		expsymbol	= 0x0010, // 4
		expsign		= 0x0020, // 5
        exponent 	= 0x0040, // 6
        bad			= 0x0080,
        done		= 0x0100
    } state;

    state = start;

	bufpos = 0;

	while ((strchr (NEXUSWhiteSpace, ch) == NULL)
		&& !(strchr (NEXUSPunctuation, ch) && (ch != '-'))
		&& (bufpos < BUFFER_SIZE)
        && (state != bad)
        && (state != done))
    {
    	if (isdigit (ch))
        {
        	switch (state)
            {
            	case start:
                case sign:
                	state = digit;
                    break;
                case expsymbol:
                case expsign:
                	state = exponent;
                    break;
                default:
                	break;
            }
        }
        else if ((ch == '-') || (ch == '+'))
        {
        	switch (state)
            {
            	case start:
                	state = sign;		// sign of number
                    break;
                case digit:
                	state = done;		// minus sign is punctuation, such as 6-10
                	break;
                case expsymbol:
                	state = expsign;	// sign of exponent
                    break;
                default:
                	state = bad;		// syntax error
                    break;
            }
        }
        else if ((ch == '.') && (state == digit))
        	state = fraction;
        else if (((ch == 'E') || (ch == 'e')) && (state & (digit | fraction)))
        	state = expsymbol;
        else
        	state = bad;

        if ((state != bad) && (state != done))
        {
			buf[bufpos++] = ch;
            s->get(ch);
        }
    }

    // Update position in file
    pos += bufpos;

    int isNumber =  state & (digit | fraction | exponent | done);
    if (isNumber)
    {
    	buf[bufpos] = '\0';

		token = ttNUMBER;
		result = token;

		if (strchr (NEXUSPunctuation, ch))
        {
        	s->putback (ch);
        }
    }
    else
    {
    	// Not a number but a token that starts with a number
    	do {
    		if (ch == '_')
    			ch = ' ';
    		buf[bufpos++] = ch;
    		s->get(ch);
    	} while ((strchr (NEXUSWhiteSpace, ch) == NULL) && (strchr (NEXUSPunctuation, ch)== NULL));
    	buf[bufpos] = '\0';
  		if (strchr (NEXUSPunctuation, ch))
        {
        	s->putback (ch);
        }
    	result = ttIDENTIFIER;

    	
    }

	return  result;

#else


#ifdef NEWSCAN
	// Handle NEXUS tokens better
	bufpos = 0;
	buf[bufpos++] = ch;
	while (s->get(ch)
		&& (strchr (NEXUSWhiteSpace, ch) == NULL)
		&& (strchr (NEXUSPunctuation, ch) == NULL || (ch == '-'))
		&& (bufpos < BUFFER_SIZE))
	{
		buf[bufpos++] = ch;
	}
	buf[bufpos] = '\0';

	if (!s->good()) return 0;

	s->putback (ch);

	// Is it a number?
	double value;
	char *endptr;
	value = strtod (buf, &endptr);
	if (*endptr == '\0' || endptr == NULL)
		token = ttNUMBER;
	else token = ttIDENTIFIER;

    // Update position in file
    pos += bufpos;

	return 1;
#else
	int numpoints = 0;
	bufpos = 0;
	buf[bufpos++] = ch;
	while ((s->get(ch)) && ((isdigit(ch)) || (ch == '.') || (ch == 'e')
		|| (ch == 'E') || (ch == '-'))
		&& (numpoints < 2))
	{
		buf[bufpos++] = ch;
		if (ch == '.') numpoints++;
	}
	buf[bufpos] = '\0';
	s->putback (ch);
	return (numpoints < 2);
#endif

#endif
}


//--------------------------------------------------------------------------
void Scanner::Reset ()
{
	if (lpszComment)
	{
		delete lpszComment;
		lpszComment = NULL;
	}
	Error = errOK;
#if (__MWERKS__)
	s->rdbuf()->pubseekoff(streamoff(0), ios::beg, ios::in);
#else	
  	s->seekg (0);
#endif /* __MWERKS */
	pos = 0;
}

//------------------------------------------------------------------------------
// 23/4/96 Convert the buffer to a number.
double Scanner::BufferToNumber ()
{
	double value = 0.0;
	char *endptr;
	value = strtod (buf, &endptr);
	if (*endptr == '\0' || endptr == NULL)
		Error = errOK;
	else
		Error = errBadNumber;
	return value;
}



