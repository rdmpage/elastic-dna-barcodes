// $Log: Scan.h,v $
// Revision 1.8  2001/09/03 11:12:19  rdmp1c
// Fixed mixed bug in ParseNumber caused by labels having
// a combination of numbers and letters.
//
// Revision 1.7  2000/11/27 14:45:02  rdmp1c
// Added "std::" to streampos declaration (CW6)
//
// Revision 1.6  2000/08/23 14:17:39  rdmp1c
// Changed flag to detected which version of MSL we are
// compiling for to handle version 4.1.05 (I downloaded this
// update for CW Pro 4). Versions of MSL earlier than 4.1.05
// have different stream headers.
//
// Revision 1.5  1999/10/23 13:08:37  mac
// Changed a version number check: was if (__MSW__ >= 0x4000)
// but is now checking against 0x5000.  Shouldn't affect the
// compilation on anything but the most recent CodeWarrior IDE.
//
// Revision 1.4  1999/10/19 09:03:31  mac
// inserted this:
// 		#if (__MSL__ >= 0x4000)	// MAC added this 10/8/99
// 			return (streamoff) p;	//
// 		#else								//
// 			return p.offset ();
// 		#endif							//
// on lines 100-104
//
// Revision 1.3  1999/10/14 10:29:29  rdmp1c
// Merged with NDE versions
//
//

//
// (c) 1993, Roderic D. M. Page
//
// Filename:		scan.h
//
// Author:       	Rod Page
//
// Date:            15 June 1993
//
// Class:           Scanner
//
// Inheritance:	
//
// Purpose:		Simple lexical scanner
//
// Modifications:	
//
//

#ifndef SCANH
#define SCANH

#include <iostream.h>
typedef istream *StreamPtr;

#include "Error.h"

// 23/4/98
#define plainFace 		0x00
#define boldFace 		0x01
#define italicFace 		0x02
#define underlineFace	0x04

#define FORMAT_BUFFER	100

// 1/3/96 Deal with NEXUS tokens properly.
#define NEWSCAN
#ifdef NEWSCAN
	#include <stdlib.h>
#if 1
	// Syst Bio paper on standard
	#define NEXUSPunctuation "()[]{}/\\,;:=*'\"`+-"
#else
	// PAUP manual
	#define NEXUSPunctuation "\"'*(){}[]/,;:="
#endif
	#define NEXUSWhiteSpace "\n\r\t "
	#define NOHASH
#endif


#define BUFFER_SIZE 2048

// A 1024 byte buffer to store comments in file
#define COMMENT_BUFFER_SIZE 2048


// Standard token types
enum tokentypes {ttNULL, ttIDENTIFIER, ttHASH, ttNUMBER, ttSEMICOLON,
		ttOPENPAR, ttCLOSEPAR, ttEQUALS, ttSPACE, ttCOMMA, ttASTERIX,
		ttCOLON, ttOTHER, ttBAD, ttMINUS, ttDOUBLEQUOTE, ttPERIOD, ttBACKSLASH};

// A simple lexical scanner
class Scanner
{
public:
	Scanner ()
	{
		Error = errOK;
		lpszComment = NULL;
        pos = 0;
        curFormat = plainFace;
	};
	Scanner (StreamPtr astream)
	{
		Error = errOK;
		lpszComment = NULL;
		s = astream;
        pos = 0;
        curFormat = plainFace;
	};
	~Scanner ();
	void		SetStream (StreamPtr astream) {s = astream;};
	char		*GetBuffer () { return (buf); };
	tokentypes	GetCurrentToken() { return (token); };
	int			GetError () { return (Error); };
	StreamPtr	GetStream () { return (s); };
	tokentypes 	GetToken (int returnspace = 0);
	char		*GetComments ();
#if (__MWERKS__)
	long 		GetPosition ()
	{
		std::streampos p = s->rdbuf()->pubseekoff(0, ios::cur, ios::in);
		#if (__MSL__ >= 0x4011)	// This is in MSL 4.1.05 and above
			return (streamoff) p;	//
		#else								//
			return p.offset ();
		#endif							//
	}
#else
//	long		GetPosition () { return s->tellg(); };
	long		GetPosition () { return pos; };
#endif /* __MWERKS__ */

	void		Reset ();
	// 23/4/97
	double 		BufferToNumber ();

    void		IncrementPos (long p) { pos += p; };

    int			GetCharacterTextFormat (int i) { return format[i]; };
protected:
	char		buf[BUFFER_SIZE];
	int			bufpos;
	char 		ch;
	int			Error;
	StreamPtr	s;
	tokentypes 	token;
	int 		ParseComment();
	tokentypes	ParseNumber();
	int 		ParseString();
	int 		ParseToken();
	char		*lpszComment;
    long		pos;

    short		format[FORMAT_BUFFER];
    int			curFormat;
};

#endif /* SCANH */


