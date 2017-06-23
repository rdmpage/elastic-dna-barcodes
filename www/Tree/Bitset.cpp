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
*        File: bitset.cpp                                                  *
*	 Purpose: bitmask sets                                                *
*     History:                                                             *
* 	                                                                      *
***************************************************************************/


#include "Bitset.h"
#include <stdlib.h>
#include <string.h>

unsigned short bits[BITSPERINT] =
					{0x0001,0x0002,0x0004,0x0008,
					 0x0010,0x0020,0x0040,0x0080,
 					 0x0100,0x0200,0x0400,0x0800,
					 0x1000,0x2000,0x4000,0x8000};


// Constructor clears the set
Bitset::Bitset()
{
	NullSet();
}

// Clears the set
void Bitset::NullSet()
{
	int i;
	for (i=0;i<rdmpBLOCK;i++)
	{
		set[i] = 0;
	}
}

//Glasgow
void Bitset::FullSet (int n)
{
	for (int i = 1; i <= n; i++)
	{
		AddToSet (i);
	}
}

//Add element i to the set
void Bitset::AddToSet (int i)
{
	int m, j;
	m = i % BITSPERINT;
	j = i / BITSPERINT;
	set[j] = set[j] | bits[m];
}

// Delete element i from the set
void Bitset::DeleteFromSet (int i)
{
	int m, j;
	m = i % BITSPERINT;
	j = i / BITSPERINT;
	set[j] = set[j] & (~bits[m]);
}

//Dump the set to cout
void Bitset::Dump()
{
	int i, j;
	cout << "[ ";
	for (i=0;i<rdmpBLOCK;i++)
	{
		for (j=0;j<BITSPERINT;j++)
		{
			if ((set[i] & bits[j]) == bits[j])
			{
				cout << ((i * BITSPERINT) + j) << " ";
			}
		}
	}
	cout << "]\n";
}
int Bitset::WriteSet (ostream &f)
{
	int i, j;

	for (i=0;i<rdmpBLOCK;i++)
	{
		for (j=0;j<BITSPERINT;j++)
		{
			if ((set[i] & bits[j]) == bits[j])
			{
				f << " " << ((i * BITSPERINT) + j);
			}
		}
	}
     return (f.good());
}


//Add another set to the set
void Bitset::AddSetToSet (const Bitset &b)
{
	int i;
	for (i=0;i<rdmpBLOCK;i++)
	{
		set[i] = set[i] | b.set[i];
	}
}

//Return the number of elements in the set
int Bitset::Cardinality() const
{
	int count = 0;
	int i, j;
	for (i=0;i<rdmpBLOCK;i++)
	{
		if (set[i]!=0)
		{
			for (j=0;j<BITSPERINT;j++)
			{
				if ((set[i] & bits[j])==bits[j])
				{
				 count++;
				}
			}
		}
	}
	return (count);
}

//Fill the set with elements listed in s, e.g. "6 11 13-18" 
void Bitset::ParseSet (char *s)
{
	char *q, *r;
     int i, j, k;

	  NullSet (); // Clear the set

	q = strtok (s, " ");
	while (q)
	{
		r = strchr (q, '-');
		if (r)
		{
			r++;
			i = atoi (q);
			j = atoi (r);
			for (k=i;k<=j;k++)
			{
				AddToSet (k);
               }
		}
		else
		{
          AddToSet (atoi(q));
		}
		q = strtok (NULL, " ");
     }
}

//True if i in set
BooleaN Bitset::IsElement (const int &i)
{
	int m = i % BITSPERINT;
	int j = i / BITSPERINT;
	if ((set[j] & bits[m]) == bits[m]) return (True);
     else return (False);
}

int	Bitset::GetFirstElement()
{
	numElements = Cardinality();
	if (numElements == 0) return (0);
     Count = 0;
	CurrentElement = 0;
     return (GetNextElement());
}

BooleaN Bitset::IsMore ()
{
	return (BooleaN(Count != numElements));
}

int Bitset::GetIthElement (int i)
{
	int count = 0;
	int j = 0;
	while (count != i && j < MAXELEMENTS)
	{
		j++;
		if (IsElement (j)) count++;
	}
     if (count == i) return (j);
	else return (0);
}

int Bitset::GetNextElement()
{
	do
	{
		CurrentElement++;
	} while (!IsElement(CurrentElement));
	Count++;
	return (CurrentElement);
}

BooleaN Bitset::IsSame (const Bitset &b)
{
	BooleaN same = True;
	int i = 0;
	while (same && i < rdmpBLOCK)
	{
		same = (BooleaN)(set[i] == b.set[i]);
		i++;
	}
     return (same);
}

BooleaN Bitset::IsNull ()
{
	return (BooleaN)(Cardinality () == 0);
}

void Bitset::Intersection (const Bitset &b, Bitset &intersection)
{
	for (int i = 0; i < rdmpBLOCK; i++)
	{
		intersection.set[i] = set[i] & b.set[i];
	}
}

void Bitset::Union (const Bitset &b, Bitset &setunion)
{
	for (int i = 0; i < rdmpBLOCK; i++)
	{
		setunion.set[i] = set[i] | b.set[i];
	}
}

void Bitset::SetDifference (const Bitset &b, Bitset &complement)
{
	for (int i = 0; i < rdmpBLOCK; i++)
	{
		complement.set[i] = set[i] & ~( b.set[i]);
	}
}


BooleaN Bitset::IsDisjoint (const Bitset &b)
{
	Bitset i;
	Intersection (b, i);
     return (i.IsNull ());
}

// True if set is subset of b
BooleaN Bitset::IsSubset (const Bitset &b)
{
	Bitset u;
	Union (b, u);
	return (BooleaN)(b.Cardinality () == u.Cardinality ());
}

// True if set is superset of b
BooleaN Bitset::IsSuperset (const Bitset &b)
{
	Bitset u;
	Union (b, u);
	return (BooleaN)(Cardinality () == u.Cardinality ());
}


// Relationship of set relative to b
SetRelations Bitset::Relationship (Bitset &b)
{
	if (IsSame (b)) return (rdmpIDENTITY);		//identity
	if (IsDisjoint (b)) return (rdmpDISJOINT);	// sets are disjoint
	if (IsSubset (b)) return (rdmpSUBSET);		// a subset of b
	if (IsSuperset (b)) return (rdmpSUPERSET);	// a superset of b
	return (rdmpOVERLAPPING);				// sets overlap
}

void Bitset::operator+= (const int i)
{
	AddToSet (i);
}

void Bitset::operator+= (const Bitset &b)
{
	AddSetToSet (b);
}


void Bitset::operator-= (const int i)
{
	DeleteFromSet (i);
}

Bitset Bitset::operator+ (const Bitset &b)
{
	Bitset s;
	Union (b, s);
	return (s);
}

Bitset Bitset::operator- (const Bitset &b)
{
	Bitset s;
	SetDifference (b, s);
	return (s);
}

Bitset Bitset::operator* (const Bitset &b)
{
	Bitset s;
	Intersection (b, s);
	return (s);
}


int	Bitset::operator[] (const int i)
{
	return (GetIthElement (i));
}


int Bitset::operator== (const Bitset &b)
{
	return ( (int) (IsSame (b)));
}

int Bitset::operator<= (const Bitset &b)
{
	return ( (int) (IsSubset (b)));
}

int Bitset::operator>= (const Bitset &b)
{
	return ( (int) (IsSuperset (b)));
}

// 8/5/97 Support for splits
void Bitset::LongToSet (const long l)
{
	long split = l;
	
	NullSet ();
	int i = 0;
	while (split != 0)
	{
		long mask = 1 << i;
		if (split & mask)
		{
			AddToSet (i + 1);
			split &= (~mask);
		}
		i++;
	}
}
			
//22/7/97
int Bitset::Hash (int HashSize)
{
	int hcode = 0;
	for (int i=0;i<rdmpBLOCK;i++)
	{
		hcode += (set[i] % HashSize);
	}
	return hcode % HashSize;
}

void Bitset::WriteSetAsStar (ostream &s, int max_value)
{
	for (int i = 1; i <= max_value; i++)
	{
		if (IsElement (i))
	#ifdef Macintosh
			s << "€";
		else
			s << "§";
	#else	
			s << "*";
		else s << ".";
	#endif
		if (i % 5 == 0)
      	s << " ";
	}
}

//24/7/97
int Bitset::IsCompatible (Bitset &b)
{
	if (Relationship (b) == rdmpOVERLAPPING)
		return 0;
	else
		return 1;
}

int Bitset::operator|| (Bitset &b)
{
	return IsCompatible (b);
}

void Bitset::Write (ostream &s)
{
	int count = 0;
	int start = 1;
	int card = Cardinality ();
	s << "[";
	while (count < card)
	{
		int k = 0;
		while (!IsElement (start))
			start++;
		k++;
		int end = start + 1;
		while (IsElement (end))
		{
			end++;
			k++;
		}

		if (k >= 1)
			s << " " << start;
		if (k > 1)
			s << "-" << (end - 1);

		start = end;

		count += k;
	}
	s << "]";
}

void Bitset::operator*= (const Bitset &b)
{
	for (int i = 0; i < rdmpBLOCK; i++)
	{
		set[i] &= b.set[i];
	}
}


void Bitset::operator-= (const Bitset &b)
{
	for (int i = 0; i < rdmpBLOCK; i++)
	{
		set[i] &= ~( b.set[i]);
	}
}











