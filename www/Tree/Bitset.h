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
*        File: bitset.h                                                    *
*	 Purpose: bitmask sets                                                *
*     History: 15 June 1993                                                *
*		      3 May 1994 operators added               				*
* 	                                                                      *
***************************************************************************/
#ifndef BITSET
#define BITSET

#include <iostream.h>
#include "CpVars.h"


#define MAXELEMENTS 5000
#define BITSPERINT 16
#define rdmpBLOCK	(MAXELEMENTS / BITSPERINT + 1)

enum SetRelations {rdmpIDENTITY, rdmpDISJOINT, rdmpSUBSET, rdmpSUPERSET, rdmpOVERLAPPING};


//Set implemented as a bit set
class Bitset
{
public:
	Bitset();
	void 		AddToSet (int i); 					//Add i to the set
	void 		AddSetToSet (const Bitset &b); 		//Add the set b

	int  		Cardinality() const; 				//Return the number of elements

	void 		DeleteFromSet (int i);

	void 		FullSet (int n);

	int  		GetFirstElement();
	int  		GetIthElement (int i);
	int  		GetNextElement();

	void 		Intersection (const Bitset &b, Bitset &intersection);
	BooleaN 	IsDisjoint (const Bitset &b);
	BooleaN		IsElement (const int &i);
	BooleaN		IsMore ();
	BooleaN 	IsNull ();
	BooleaN 	IsSame (const Bitset &b);
	BooleaN 	IsSubset (const Bitset &b);
	BooleaN 	IsSuperset (const Bitset &b);

	void		SetDifference (const Bitset &b, Bitset &complement);

	int			WriteSet (ostream &f);
	void 		NullSet();
	void 		ParseSet (char *s);				//Fill set with elements listed in s
	SetRelations Relationship (Bitset &b);		// relationship w.r.t b
	void 		Union (const Bitset &b, Bitset &setunion);

	// Operators
	// add, etc
	void 		operator+= (const int i);		// add i
	void 		operator+= (const Bitset &b);
	void 		operator-= (const int i);		// delete i

	Bitset		operator+ (const Bitset &b);	// union
	Bitset		operator- (const Bitset &b);	// set difference
	Bitset		operator* (const Bitset &b);	// intersection
	int			operator[] (const int i);    	// access elements

	  // relations
	int 		operator== (const Bitset &b); 	// identity
	int 		operator<= (const Bitset &b);	// subset of b
	int 		operator>= (const Bitset &b); 	// superset of b

	void 		Dump ();						//Dump the set

	void 		LongToSet (const long l);

	// 23/7/97
	int 		Hash (int HashSize);
	void 		WriteSetAsStar (ostream &s, int max_value);
	// 24/7/97
	int			IsCompatible (Bitset &b);
	int			operator|| (Bitset &b); 		// Compatible
	void 		Write (ostream &s);
	void		operator*= (const Bitset &b);	// Keep elements in common with b
	void		operator-= (const Bitset &b);	// Keep elements not in b
   
private:
	unsigned short	set[rdmpBLOCK];				//The bit set itself
	int			CurrentElement;
	int			Count;
	int			numElements;
};
typedef Bitset *BitsetPtr;

#endif


