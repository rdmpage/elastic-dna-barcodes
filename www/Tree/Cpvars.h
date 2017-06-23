// $Log: Cpvars.h,v $
// Revision 1.6  2001/03/05 17:03:55  rdmp1c
// Increased MAXLEAVES to 1000 for TreeView to satisfy users
// requests for > 500 leaves. This required some changes to
// TreeBuf.cpp and Profile.cpp to compile under Metrowerks.
// The problem is my code has MAXLEAVES size arrays in the
// Tree class, which is very wasteful of space, and can send
// the local data size over the 32K limit.
//
// Revision 1.5  2000/08/10 09:17:49  rdmp1c
// GeneTree's MAXLEAVES increased to 500
//
// Revision 1.4  1999/11/30 14:34:43  rdmp1c
// Fixed bug with GENETREE ifdef
//
// Revision 1.3  1999/10/14 10:29:29  rdmp1c
// Merged with NDE versions
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
*        File: cpvars.h                                                    *
*	 Purpose: global contants                                             *
*     History: 15 June 1993                                                *
* 	                                                                      *
***************************************************************************/

#ifndef CPVARS
#define CPVARS

// Global constants

#ifdef NDE
	#define MAXLEAVES 	1000
#else
	#ifdef GENETREE
		#define MAXLEAVES	500
	#else
		#define MAXLEAVES 	1000
	#endif               	// maximum leaves in a tree
#endif
#define MAXNODES 		(MAXLEAVES * 2) - 1	// maximum nodes in a tree
#define MAXNAMELENGTH 	128 				// maximum characters in a leaf label

// Boolean type declaration
// 12 Oct 93 Boolean conflicts with Vibrant so use BooleaN as identifier
enum BooleaN {False, True};

// Tree drawing characters
//#define ANSI      // Define the character set to use for text drawings

#ifdef Macintosh
	#ifndef ANSI // PAUP Monaco characters (obtained using FontView)
		#define HBAR  220 // Ñ 
		#define VBAR  221 // Ý
		#define SIB   225 // á
		#define BOT   222 // Þ
		#define ROOT  223 // ß
		#define DOWN  227 // ã // Not quite what we want...!
		
		// Full trees
		#define TEE   224 // à
		#define LEFT  ROOT 
		#define RIGHT BOT 
		
		// Charting characters
		#define BAR_BLOCK 226 // â
	#endif
#endif

#ifdef Windows      // OEM character set
	//Compressed tree
	#define HBAR 196  // Ä
	#define VBAR 179  // ³
	#define SIB  195  // Ã
	#define BOT  192  // À
	#define ROOT 218  // Ú
	#define DOWN 194  // Â

	// Regular tree
	#define TEE  180  // ´
	#define LEFT ROOT
	#define RIGHT BOT

#endif

#ifdef ANSI       // ANSI character set
	#define HBAR 45
	#define VBAR 124
	#define SIB  43
	#define BOT  92
	#define ROOT 43
	#define DOWN 43
#endif


#endif


