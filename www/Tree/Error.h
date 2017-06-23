// $Log: Error.h,v $
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
*        File: error.h                                                     *
*	 Purpose: error codes                                                 *
*     History: 15 June 1993                                                *
* 	                                                                      *
***************************************************************************/

#ifndef ERRORH
#define ERRORH

#if defined(VIBRANT) || defined(VCLASS)
#include	<stdio.h>
#else
#include <iostream.h>
#endif

#include <string.h>

#define errOK 				0
#define errSyntax 			1
#define errEnd 				2
#define errMissingLPar 		3
#define errUnbalanced 		4
#define errStackNotEmpty 	5
#define errSemicolon 		6
#define errNoMemory			7
#define errTooManyTrees		8
#define errStackOverFlow 	9
#define errBadNumber		10
#define errStackEmpty		errUnbalanced
#define errExtraPair		11
#define errEquals			12
#define errTreeName			13
#define errBegin			14
#define errBlockName		15
#define errHashTableOverflow 	16
#define errDuplicateLabel	17
#define errTooFewLabels		18
#define errUnknownLabel		19
#define errComma			20
#define errMixedRoot		21
#define errIncorrectCommand	22
#define errNotNEXUS			23
#define errString			24
#define errInvalidNumber	25
#define errNoLabels			26
#define errBadCommand		27
#define errTREESBlock		28
#define errImportFile		29
#define errImportFormat		30
#define errFILETYPE			31
#define errFILE				32
#define errBadImportFile	33
#define errIncorrectBlock 	34
#define errAllBinary	 	35
#define errUnknownFormat 36

// 9 July 1996
#define errENDBLOCK    		37
#define errDIMENSIONS  		38
#define errTAXLABELS   		39
#define errNTAX        		40

// 23/4/97
#define errNCHAR        	41
#define errFORMAT       	42
#define errMATRIX       	43
#define errDMATRIX       	45
#define errTaxonName		44
#define errDoubleQuote		46

// Spectrum
#define errAsymmetricDistanceMatrix 	50
#define errNotAllNonNegativeDistances 	51
#define errTooManyTaxa 					52
#define errInvalidSymbol 				53

// GeneTree
#define errMissingLeaves				59

// NEXUS editor
#define errInvalidCharacterNumber		54
#define errColon						55
#define errPictureFormat				56
#define errPictureEncoding				57
#define errPictureSource				58
// 7 Jan 99
#define errUnterminatedQuote			60


void SaveToken (char *s);

// Return error message
char *GetErrorMsg (int error);

#ifndef VIBRANT
// Simple error message reporting
void ErrorMsg (int error);
#endif


// 10 July 1996
char *GetOffendingToken ();

#endif /* ERRORH */


