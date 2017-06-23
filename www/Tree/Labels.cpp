//
// (c) 1993, Roderic D. M. Page
//
// Filename:		labels.cpp
//
// Author:       	Rod Page
//
// Date:            16 June 1993
//
// Class:			Labels
//
// Inheritance:
//
// Purpose:		Leaf label storage
//
// Modifications:
//
//
//
// labels.cpp
//
//
// leaf label storage and lookup.
//
// Note: make zero-offset arrays transparent to user.
//
// 16 June 1993
//
//
#include "Labels.h"

#ifndef max
	#define max(a,b)		(((a) > (b)) ? (a) : (b))
#endif

#define HASHOVERFLOW 	-1
#define PROBELENGTH 	1

const int ALPHABET = 26; 	// letters in alphabet 
const int ASCIIa = 96; 		// ASCII code of character before 'a'
	// 18.10.2000 - MAC added "int" in above since implicit 'int' is no longer supported in C++

//------------------------------------------------------------------------------
// Constructor
Labels::Labels()
{
	int i;

     MaxLabelLength = 0;
	Stored         = 0;
	Translate      = False;
	LabelsRead     = False;

	for (i=0; i<MAXLEAVES; i++)
	{
		LeafLabels[i]  = NULL;
		TransLabels[i] = NULL;
	}
	for (i=0;i<MAXLEAVES;i++)
	{
		LabelCodes[i]  = -1;
		TransCodes[i]  = -1;
		Translation[i] = -1;
	  }
}


//------------------------------------------------------------------------------
// 31/7/97 Copy constructor
Labels::Labels(const Labels &L)
{
	int i;

    MaxLabelLength = L.MaxLabelLength;
	Stored         = L.Stored;
	Translate      = L.Translate;
	LabelsRead     = L.LabelsRead;

	for (i=0; i< Stored; i++)
	{
		LeafLabels[i] = new char[strlen (L.LeafLabels[i]) + 1];
		strcpy (LeafLabels[i], L.LeafLabels[i]);
		
		if (Translate)
		{
			TransLabels[i] = new char[strlen (L.TransLabels[i]) + 1];
			strcpy (TransLabels[i], L.TransLabels[i]);
		}
	}
	for (i=0;i < Stored;i++)
	{
		LabelCodes[i]  = L.LabelCodes[i];
		TransCodes[i]  = L.TransCodes[i];
		Translation[i] = L.Translation[i];
	  }
}



//------------------------------------------------------------------------------
// Free the memory allocated for the labels
Labels::~Labels()
{
	int i;

	for (i=0; i<Stored; i++)
	{
		delete [] LeafLabels[i];	// 25.8.00 MAC -- this should use delete [] since was created with new [].
		if (Translate) delete [] TransLabels[i];	// 25.8.00 MAC -- this should use delete [] since was created with new [].
	}
}

// a

//------------------------------------------------------------------------------
// Add s to list of labels
int Labels::AddLabel (char *s,  int Index)
{
	storestring (s);
	int result = addname (LeafLabels, LabelCodes, Index);
	if (result == 0)
	{
		Stored++;
		MaxLabelLength = MAX (MaxLabelLength, strlen (WorkString));
	 }
	return (result);
}

//------------------------------------------------------------------------------
// Add s to translation table
int Labels::AddTransLabel (char *s, int Index)
{
	storestring (s);
	int result = addname (TransLabels, TransCodes, Index);
	return (result);
}


//------------------------------------------------------------------------------
// Add WorkString to L, return 0 if successful. If label already exists
// returns the index of that label; if hash table overflow returns -1.
int Labels::addname (LabelList L, LabelCode H, int &Index)
{
	int hcode;
	int originalhcode;
	int code = -2;

	hcode = hashcode ();
	originalhcode = hcode;
	while (code < -1)
	{
		if (H[hcode] == -1)
		{
			// bucket is vacant, so label is new
			L[Index-1] = new char[strlen (WorkString) + 1];
			strcpy (L[Index-1], WorkString);
			H[hcode] = Index-1;
			code = 0;
		}
		else
		{
          	// bucket is occupied
			if (strcmp (L[H[hcode]], WorkString))
			{
				// and is different, resolve collision by linear probing
				hcode += PROBELENGTH;
				if (hcode == HASHTABLESIZE) hcode = 0;
				if (hcode == originalhcode) code = HASHOVERFLOW;
			}
			else
			{
				// bucket is the same, so we've a duplicate label
				// This next line causes a UAE in Windows. If duplicate label is also
				// the first label, then code will be 0, which is the
				// same return code for a success (!)
// 				code = H[hcode]; // Found name, return it's index

				// Hence, increment the code by one to ensure we
				// never return 0 (14/12/95)
				code = H[hcode] + 1; // Found name, return it's index + 1
				SaveToken (WorkString);
			}
		}
	}
	return (code);
}

//d

//------------------------------------------------------------------------------
// Given the translation token s, return the index of the
// corresponding label
int Labels::decode (char *s)
{
	int result = LocateTransLabel (s); // find translation label
	if (result > -1) return Translation[result]; 
	else return result;
}

//------------------------------------------------------------------------------
// Debugging
#ifdef LABELS_DEBUG
void Labels::dump ()
{
	int i;

	cout << "Labels:\n";
     cout << "-------\n";
	cout << "Stored=" << Stored << '\n';
//     cout << "storedstring=" << WorkString << '\n';
	for (i=0;i<Stored;i++)
	{
		cout << i << '\t' << LeafLabels[i] << '\t' << TransLabels[i] << endl;
	}
}
#endif

//e

//------------------------------------------------------------------------------
// Encode label s by storing its index in the translation table
int Labels::Encode (char *s, int Index)
{
	int result = LocateLabel (s); // Check label exists
//     cout << "result=" << result << '\n';
	if (result > -1) Translation[Index-1] = result; // store its index
	return (result);
}



// g

//------------------------------------------------------------------------------
// ith label. Default (pretty = True) is to remove underscores
// Label i
char *Labels::GetLabel (int i, BooleaN pretty)
{
	if (!LeafLabels[i - 1]) return NULL;
	if (pretty)
	{
		// Replace underscores with spaces so looks pretty
		strcpy (WorkString, LeafLabels[i - 1]);
		char *p;
		p = WorkString;
		while (*p)
		{
			if (*p == '_') *p = ' ';
			p++;
		}
          return (WorkString);
	}
     // return the label as is
     else return (LeafLabels[i-1]);
}

//------------------------------------------------------------------------------
// Return the (zero-offset adjusted) index of the label s, performing
// any translation if necessary. Returns 0 if unsuccessful.
int Labels::GetLabelIndex (char *s)
{
	int result;

	if (Translate) result = decode (s);
	else result = LocateLabel (s);
	return (result+1);
}

// h
//------------------------------------------------------------------------------
int Labels::hashcode ()
{
	long hcode = 0;  // 11/1/96
	int j = 0;

	while (WorkString[j])
	{
		hcode += ((j+1) * int (WorkString[j]));
		j++;
	}
	long result = hcode % HASHTABLESIZE; // 11/1/96 Avoid UAE
	return ( (int) result);
}

//l

//------------------------------------------------------------------------------
// Look up the label
// Returns index of label (zero-offset) if successful
int Labels::LocateLabel (char *alabel)
{
	storestring (alabel);
#ifdef __MWERKS__
	return (locatename ((const char **) LeafLabels, LabelCodes));
	// 18.10.2000 MAC: added explicit type casting to (const char **)
#else
	return (locatename (LeafLabels, LabelCodes));
#endif
}

//------------------------------------------------------------------------------
// Look up the translate label
int Labels::LocateTransLabel (char *alabel)
{
	storestring (alabel);
#ifdef __MWERKS__
	return (locatename ((const char **) TransLabels, TransCodes));
	// 18.10.2000 MAC: added explicit type casting to (const char **)
#else
    	return (locatename (TransLabels, TransCodes));
#endif

    }

//------------------------------------------------------------------------------
// Look up WorkString in L using hash table H.
// Returns index (zero-offset) of WorkString in L if successful, otherwise
// returns -1.
int Labels::locatename (const LabelList L, const LabelCode H)
{
	int hcode;
     int originalhcode;
	int code = -2;

	hcode = hashcode ();
     originalhcode = hcode;
	while (code < -1)
	{
		if (H[hcode] == -1)
		{
		   code = -1; // Cell empty, name doesn't exist
		}
		else
		{
			if (strcmp (L[H[hcode]], WorkString))
			{
				// Cell is different, resolve collision by linear probing
				hcode += PROBELENGTH;
				if (hcode == HASHTABLESIZE)
					hcode = 0;
				if (hcode == originalhcode)
					code = -1;
			}
			else
			{
				code = H[hcode]; // Found name
			}
		}
	}
	return (code);
}

// s 

//------------------------------------------------------------------------------
void Labels::storestring(char *astring)
{
	strncpy (WorkString, astring, MAXNAMELENGTH);
}

////////////////////////////////////////////////////////////////////////////
// Read NEXUS TAXLABELS command.
// Return error code. 
int Labels::ReadLabels (Scanner &s, int n)
{
	int count = 0;
     int result;
	int error = errOK;

	while ((count < n) && (error == errOK))
	{
		tokentypes t = s.GetToken();
		if ((t == ttIDENTIFIER) || (t == ttNUMBER))
		{
          	count++;
			result = AddLabel (s.GetBuffer(), count);
			switch (result)
			{
				case 0:
					break;
				case HASHOVERFLOW:
					error = errHashTableOverflow;
                         break;
				default:
					error = errDuplicateLabel;
			}
          }
		else error = errSyntax;
	}
	if (error == errOK)
	{
		if (count != n) error = errTooFewLabels;
		else
		{
			if (s.GetToken() == ttSEMICOLON) LabelsRead = True;
			else error = errSemicolon;
          }
	}
	return (error);
}

//------------------------------------------------------------------------------
// Read NEXUS TRANSLATE command. By default checks that labels being coded
// exist, otherwise adds labels. This allows importing of PAUP tree files
// with no TAXA block.
// Return error code.
int Labels::ReadTranslateTable (Scanner &s, BooleaN importing)
{
	int 		count = 0;
	int 		result;
	int 		error = errOK;
	BooleaN		done = False;
	tokentypes 	t;

	while (!done && (error == errOK))
	{
		// Get the code
		t = s.GetToken();
		if ((t == ttIDENTIFIER) || (t == ttNUMBER))
		{
			count++;
			result = AddTransLabel (s.GetBuffer(), count);
			switch (result)
			{
				case 0:
					break;
				case HASHOVERFLOW:
					error = errHashTableOverflow;
					break;
				default:
					error = errDuplicateLabel;
			}
		}
		else error = errSyntax;

		// Get the corresponding label
		if (error == errOK)
		{
			t = s.GetToken ();      // 1/3/96
			if ((t == ttIDENTIFIER) || (t == ttNUMBER))
			{
				if ((importing) && !LabelsRead)
				// Importing a PAUP treefile so store the labels.
				{
					result = AddLabel (s.GetBuffer(), count);
					switch (result)
					{
						case 0:
							// 9/9/96
							// Encode
							Translation[count - 1] = count - 1;
							break;
						case HASHOVERFLOW:
							error = errHashTableOverflow;
							break;
						default:
							error = errDuplicateLabel;
					}
				}
				else
				// Not importing so just encode labels
				{
					if (Encode (s.GetBuffer(), count) < 0)
					{
						error = errUnknownLabel;
					}
				}
			}
			else error = errSyntax;

			// Get either comma or semicolon
			if (error == errOK)
			{
				if (importing)
				// We don't now how many leaves to expect
				{
					switch (s.GetToken())
					{
						case ttCOMMA:
							break;
						case ttSEMICOLON:
							done = True;
							break;
						default:
							error = errSyntax;
					}
				}
				else
				// Token depends on how many leaves we've read
				{
					switch (s.GetToken())
					{
						case ttCOMMA:
							if (count == Stored) error = errSemicolon;
										 break;
						case ttSEMICOLON:
							if (count != Stored) error = errTooFewLabels;
                                   else done = True;
							break;
						default:
							if (count < Stored) error = errComma;
               	               else error = errSemicolon;
					}
				}
			}
		}
	}
	if (error == errOK)
	{
		if (importing)
		{
			// Use labels from translation table
			if (!LabelsRead) 
				LabelsRead = True;
         	Translate = True; // 9/9/96
		}
		else
		{
			Translate = True;
			// If not importing then we known how many labels there are
			if (count != Stored)
				error = errTooFewLabels;
		}
	}
	return (error);
}

//------------------------------------------------------------------------------
// Write labels as a TAXLABELS command
// Nonzero if success.
int Labels::WriteLabels (ostream &f)
{
	f << "\tTAXLABELS" << EOLN;
	int i = 0;
	while ((i < Stored) && f.good ())
	{
		i++;
		char buf[128];  //11/1/96 rdp
		strcpy (buf, Filestring (i));
		f << "\t\t" << buf << EOLN;
	}
	f << "\t\t;" << EOLN;
	return (f.good());
}

//------------------------------------------------------------------------------
// Write translation as a TRANSLATE command
// Nonzero if success.
int Labels::WriteTranslateTable (ostream &f)
{
	f << "\tTRANSLATE" << EOLN;
	int i = 0;
	while ((i < Stored) && f.good())
	{
		f << "\t\t";
		if (TransLabels[i] != NULL) // 29/3/96 NULL labels now checked for!
			f << TransLabels[i];
		else
			f << (i + 1);
		f << '\t';
		i++;
		f << Filestring(i);
		if (i < Stored)
			f << ',';
		f << EOLN;
	}
	f << "\t\t;" << EOLN;
	return (f.good()); 
}

//------------------------------------------------------------------------------
// If label contains single quote(s) double quotes them and encloses whole
// in quotes, or returns original label.
// Used when writing NEXUS files
char *Labels::Filestring(int i)
{
	strcpy (WorkString, LeafLabels[i-1]);
	return (OutputString (WorkString));
}
/*

	  int 		j = 0;
	char 	*p = LeafLabels[i - 1];
	BooleaN 	enclose = False;

	if (isalpha (*p))
	{
		// First character is a letter, check the rest
          p++;
		while (*p && !enclose)
		{
			if (!isalnum (*p) && (*p != '_') && (*p != '.'))
				enclose = True;
			else p++;
          }
          	
	}
	else
	{
     	// First character is not a letter, so enclose string
		enclose = True;
     }
	if (!enclose) return LeafLabels [i - 1];

     //Enclose
	WorkString[0] = '\'';
     j = 1;

	// Insert single quotes where needed      
	while (*p)
	{
		if (*p == '\'') WorkString[j++] = '\'';
          WorkString[j++] = *p;
		p++;
	}

     // Append last single quote
     WorkString[j++] = '\'';
	WorkString[j] = '\0';
	return (WorkString);
}
*/


#ifdef VCLASS
//------------------------------------------------------------------------------
// Return length of longest label when drawn on current port using current
// font
int Labels::GetPortLabelLength ()
{
	int score = 0;
	for (int i = 0; i < Stored; i++)
	{
		score = MAX (score, Port.TextWidth (LeafLabels[i], strlen (LeafLabels[i])));
	}
	  return (score);
}
#endif


//NEW 19/6
//------------------------------------------------------------------------------
void Labels::MakeAlphaLabels (int n)
{
	int i = 0;
	int j = 1;
	while (i < n)
	{
		if ((i / ALPHABET != 0) && (i % ALPHABET == 0))
			j ++;
		i++;
		int k = i % ALPHABET;
		if (k == 0)
			k = ALPHABET;
		char ch = (char) (ASCIIa + k);
		char buf[32];
		buf[0] = '\0';
		for (k = 0; k < j; k++)
			buf[k] = ch;
		buf[k] = '\0';
		AddLabel (buf, i);
	}
	LabelsRead = True;
}


//------------------------------------------------------------------------------
// 19/8/97
// Write translation as a TRANSLATE command for a subset of the taxon labels
// Nonzero if success.
#ifdef UseMikesBBitset
 int Labels::WriteTranslateTableSubset (ostream &f, BBitset &subset)
#else
 int Labels::WriteTranslateTableSubset (ostream &f, Bitset &subset)
#endif
{
	f << "\tTRANSLATE" << EOLN;
	int i = 1;
#ifdef UseMikesBBitset
	int n = subset.getCard();
#else
	int n = subset.Cardinality();
#endif
	while ((i <= n) && f.good())
	{
		f << "\t\t";
		int j = subset[i] - 1;
		f << i;
		f << '\t';
		f << Filestring(j + 1);
		if (i < n) 
			f << ',';
		f << EOLN;
		i++;
	}
	f << "\t\t;" << EOLN;
	return (f.good()); 
}

//------------------------------------------------------------------------------
// 19/8/97
// Write labels as a TAXLABELS command for a subset of the taxon labels
// Nonzero if success.
#ifdef UseMikesBBitset
 int Labels::WriteLabelsSubset (ostream &f, BBitset &subset)
#else
 int Labels::WriteLabelsSubset (ostream &f, Bitset &subset)
#endif
{
	f << "\tTAXLABELS" << EOLN;
	int i = 1;
#ifdef UseMikesBBitset
	int n = subset.getCard();
#else
	int n = subset.Cardinality();
#endif
	while ((i <= n) && f.good ())
	{
		int j = subset[i];
		char buf[128];
          strcpy (buf, Filestring (j));
		f << "\t\t" << buf << EOLN;
		i++;
	}
	f << "\t\t;" << EOLN;
	return (f.good()); 
}



//------------------------------------------------------------------------------
void Labels::ReplaceLabel (int i, char *s)  // i is not zero offset
{
	// Firstly clear info relating to previous ith label
    storestring (LeafLabels[i-1]);
	int hcode;
    int originalhcode;
	int code = -2;

	hcode = hashcode ();
    originalhcode = hcode;
	while (code < -1)
	{
		if (LabelCodes[hcode] == -1)
		{
		   code = -1; // Cell empty, name doesn't exist  (shouldn't ever happen)
		}
		else
		{
			if (strcmp (LeafLabels[LabelCodes[hcode]], WorkString))
			{
				// Cell is different, resolve collision by linear probing
				hcode += PROBELENGTH;
				if (hcode == HASHTABLESIZE) hcode = 0;
                    if (hcode == originalhcode) code = -1;
			}
			else
			{
				code = LabelCodes[hcode]; // Found name
			}
		}
	}

    // Remove entry from hash table
    LabelCodes[hcode] = -1;

    // Delete string
	delete [] LeafLabels[i-1];	// 25.8.00 MAC -- this should use delete [] since was created with new [].

    storestring (s);
	addname (LeafLabels, LabelCodes, i);
}

