//
// (c) 1993, Roderic D. M. Page
//
// Filename:		labels.h
//
// Author:       	Rod Page
//
// Date:            15 June 1993
//
// Class:			Labels
//
// Inheritance:
//
// Purpose:		leaf label storage and lookup
//
// Modifications:	10 July	Reads and Writes NEXUS TAXLABELS and
//						TRANSLATE commands.
//

#ifndef LABELSH
#define LABELSH

#define LABELS_DEBUG

#include <iostream.h>
#include <ctype.h>
#include <string.h>

#include "CpVars.h"
#include "Error.h"
#include "Scan.h"
#include "CpUtils.h"
#ifdef UseMikesBBitset
 #include "BasicBitset.h"
#else
 #include "Bitset.h"
#endif

#ifdef VCLASS
	#ifdef Windows
		#include "VDefs.h"
    #endif
	#include "VPort.h"
#endif

#define HASHTABLESIZE MAXLEAVES

typedef char 	*LabelList[MAXLEAVES];
typedef int 	LabelCode[HASHTABLESIZE];

class Labels
{
public:
	Labels();
	Labels (const Labels &L);
	virtual ~Labels();
	int 		AddLabel (char *s,  int Index);
	int 		AddTransLabel (char *s, int Index);
	int 		Encode (char *s, int Index);
	char		*Filestring (int i);
	char		*GetLabel (int i, BooleaN pretty = True);
	int 		GetLabelIndex (char *s);
	int			GetMaxLabelLength () { return (MaxLabelLength); };
	int			GetStored () { return (Stored); };
	int			IsLabelsRead () { return (LabelsRead); };
	int			IsTranslate () { return (Translate); };
	int 		LocateLabel (char *alabel);
	int 		LocateTransLabel (char *alabel);
	int 		ReadLabels (Scanner &s, int n);
	int			ReadTranslateTable (Scanner &s, BooleaN importing = False);
	void		SetLabelsRead () { LabelsRead = True; };
	void		SetTranslate () { Translate = True; };
	int 		WriteLabels (ostream &f);
	int 		WriteTranslateTable (ostream &f);
#ifdef LABELS_DEBUG
	void 		dump ();
#endif
#ifdef VCLASS
	int 		GetPortLabelLength ();
#endif

	void 		MakeAlphaLabels (int n);
	
	// 19/8/97
#ifdef UseMikesBBitset
	virtual int WriteTranslateTableSubset (ostream &f, BBitset &subset);
	virtual int WriteLabelsSubset (ostream &f, BBitset &subset);
#else
	virtual int WriteTranslateTableSubset (ostream &f, Bitset &subset);
	virtual int WriteLabelsSubset (ostream &f, Bitset &subset);
#endif

    virtual void ReplaceLabel (int i, char *s);

private:
	int			MaxLabelLength;
	int 		Stored;
	BooleaN		Translate;
	BooleaN		LabelsRead;
	char		WorkString[MAXNAMELENGTH * 2 + 1];
	LabelList	LeafLabels;
	LabelCode	LabelCodes;
	LabelList	TransLabels;
	LabelCode	TransCodes;
	LabelCode	Translation;

	int 		addname (LabelList L, LabelCode H, int &Index);
	int 		decode (char *s);
	int 		hashcode ();
	int 		locatename (const LabelList L, const LabelCode H);
	void 		storestring(char *astring);
};

typedef Labels *LabelsPtr;

#endif  


