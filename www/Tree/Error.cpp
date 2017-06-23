// $Log: Error.cpp,v $
// Revision 1.3  1999/10/14 10:29:29  rdmp1c
// Merged with NDE versions
//
//

#include "error.h"

#include <stdio.h>
#include <string.h>

static char OffendingToken[128];

void SaveToken (char *s)
{
	strncpy (OffendingToken, s, sizeof (OffendingToken) - 1); 
}


char *GetErrorMsg (int error)
{
	static char	msg[300];

	switch (error)
	{
		case errSyntax:
			sprintf (msg, "Syntax error: \"%s\"", OffendingToken);
			break;
		case errEnd:
			strcpy (msg, "Unexpected end of file");
			break;
		case errMissingLPar:
			strcpy (msg, "Missing left parenthesis \"(\"");
			break;
		case errUnbalanced:
			strcpy (msg, "Unbalanced tree description");
			break;
		case errStackNotEmpty:
			strcpy (msg, "Stack not empty");
			break;
		case errSemicolon:
			strcpy (msg, "Expecting a semicolon \";\"");
			break;
		case errNoMemory:
			strcpy (msg, "Insufficient memory");
#ifdef Macintosh
			strcat (msg, "\r\r(Try increasing the memory allocated to this application using Finder's \"Get Info\" command).");
#endif
			break;
		case errTooManyTrees:
			strcpy (msg, "Too many trees");
			break;
		case errStackOverFlow:
			strcpy (msg, "Stack overflow");
 			break;
		case errBadNumber:
			strcpy (msg, "Bad number");
			break;
		case errExtraPair:
			strcpy (msg, "Extra pair of parentheses \"()\"");
			break;
		case errEquals:
			strcpy (msg, "Expecting an equals \"=\"");
			break;
		case errTreeName:
			strcpy (msg, "Expecting a tree name");
			break;
		case errBegin:
			strcpy (msg, "Expecting a BEGIN command");
			break;
		case errBlockName:
			strcpy (msg, "Expecting a block name");
			break;
		case errHashTableOverflow:
			strcpy (msg, "Label hash table overflow");
			break;
		case errDuplicateLabel:
			sprintf (msg, "Label \"%s\" already exists", OffendingToken);
			break;
		case errTooFewLabels:
			strcpy (msg, "Too few labels");
			break;
		case errComma:
			strcpy (msg, "Expecting a comma \",\"");
			break;
		case errUnknownLabel:
			sprintf (msg, "Label \"%s\" is unknown", OffendingToken);
			break;
		case errMixedRoot:
			strcpy (msg, "Cannot mix rooted and unrooted trees");
			break;
		case errIncorrectCommand:
			sprintf (msg, "Command \"%s\" used incorrectly", OffendingToken);
			break;
		case errNotNEXUS:
			sprintf (msg, "File \"%s\" is not a NEXUS file", OffendingToken);
			break;
		case errString:
			strcpy (msg, "Expecting a string field");
			break;
		case errInvalidNumber:
			sprintf (msg, "\"%s\" is not a valid number", OffendingToken);
			break;
		case errNoLabels:
			strcpy (msg, "TRANSLATE command requires labels");
			break;
		case errBadCommand:
			sprintf (msg, "Command \"%s\" is unrecognised", OffendingToken);
			break;
		case errTREESBlock:
			strcpy (msg, "Expecting a TREES block");
			break;
		case errImportFile:
			strcpy (msg, "Unable to open import file");
			break;
		case errImportFormat:
			sprintf (msg, "Unrecognised import format \"%s\"", OffendingToken);
			break;
		case errFILETYPE:
			strcpy (msg, "Expecting a FILETYPE command");
			break;
		case errFILE:
			strcpy (msg, "Expecting a FILE command");
			break;
		case errBadImportFile:
			strcpy (msg, "Import file does not match filetype");
			break;
		case errIncorrectBlock:
			sprintf (msg, "Not expecting a \"%s\" block", OffendingToken);
			break;
		case errAllBinary:
			sprintf (msg, "Trees must all be binary (=fully resolved)");
			break;
		case errUnknownFormat:
			sprintf (msg, "File is not a recognised tree file");
			break;
		case errENDBLOCK:
			sprintf (msg, "Expecting an ENDBLOCK command");
			break;
		case errDIMENSIONS:
			sprintf (msg, "Expecting a DIMENSIONS command");
			break;
		case errTAXLABELS:
			sprintf (msg, "Expecting a TAXLABELS command");
			break;
		case errNTAX:
			sprintf (msg, "Expecting a NTAX command");
			break;
		case errNCHAR:
			sprintf (msg, "Expecting a NCHAR command");
			break;
		case errFORMAT:
			sprintf (msg, "Expecting a FORMAT command");
			break;
		case errMATRIX:
			sprintf (msg, "Expecting a MATRIX command");
			break;
		case errTaxonName:
			sprintf (msg, "Expecting a taxon name");
			break;
		case errDMATRIX:
			sprintf (msg, "Expecting a DMATRIX command");
			break;
		case errDoubleQuote:
			sprintf (msg, "Expecting a double quote \"");
			break;
		case errAsymmetricDistanceMatrix:
			sprintf (msg, "Distance matrix is asymmetric");
			break;
		case errNotAllNonNegativeDistances:
			sprintf (msg, "Cannot handle negative distances in distance matrix");
			break;
		case errTooManyTaxa:
			sprintf (msg, "Too many taxa");
			break;		
      case errInvalidSymbol:
			sprintf (msg, "%s<-- invalid symbol", OffendingToken);
			break;		
      case errInvalidCharacterNumber:
			 (msg, "Not a valid character number");
			break;		
      case errColon:
			 (msg, "Expecting a colon \":\"");
			break;
      case errPictureFormat:
      		sprintf (msg, "Picture format \"%s\" is not supported", OffendingToken);
            break;
      case errPictureEncoding:
      		sprintf (msg, "\"%s\" encoding of pictures is not supported", OffendingToken);
            break;
      case errPictureSource:
      		strcpy (msg, "Pictures source must be from file");
            break;
	  case errMissingLeaves:
			sprintf (msg, "Tree description is missing one or more taxa");
			break;
      // 7 Jan 1999 Per de Place Bjorn
      case errUnterminatedQuote:
      		sprintf (msg, "Unterminated quote");
            break;

		default:
			strcpy (msg, "Undocumented error");
			break;
	}
	return (msg);
}

#if !defined(VCLASS)
void ErrorMsg (int error)
{
	cout << "Error " << error << ": " << GetErrorMsg (error) << endl;
}
#endif

// 10 July 1996
char *GetOffendingToken ()
{
	return OffendingToken;
}

