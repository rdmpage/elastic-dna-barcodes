//
// (c) 1993, Roderic D. M. Page
//
// Filename:
//
// Author:       	Rod Page
//
// Date:            25 August 1993
//
// Class:
//
// Inheritance:
//
// Purpose:		Primitives for tree drawing
//
// Modifications:	12 Oct 1993 Code for Vibrant added
//
//

#ifndef PLOTH
#define PLOTH

/*---Specify GUI API to use---*/

// wxWindows C++ GUI library
#ifdef wx_msw
#include <wx.h>

// Define point and rectangle data structures

typedef struct rdpPoint {
	float x;
	float y;
};

typedef struct rdpRect {
	float left;
	float top;
	float width;
	float height;
};

#endif


// VCLASS library
#ifdef VCLASS
#include "vport.h"
#endif


// Tree styles
#if defined(VIBRANT) || defined(VCLASS)
#define TS_LEFT     			0x0001
#define TS_RIGHT				0x0002
#define TS_SLANT				0x0004
#define TS_ANGLE				0x0008
#define TS_CLADOGRAM			0x0010
#define TS_PHYLOGRAM			0x0020
#define TS_LABELINTERNALS		0x0040
#define TS_HIGHLIGHT			0x0080
#define TS_LABELEDGES	   		0x0100
#define TS_SAMESCALE			0x0200
#define TS_SHOWNAME				0x0400
#define TS_RADIAL				0x0800
#define TS_ROTATE_LABELS 		0x1000

// 3/4/97
#define ROTATE_LABELS

//clw
#define TS_USEINTERNALLABEL 	0x0080

#define TS_DEFAULT		TS_LEFT | TS_SLANT | TS_CLADOGRAM
#else
#define CLADOGRAM 1
#define PHYLOGRAM 2
#endif


typedef struct  {
	int 		count;
	int 		leaves;
	float		maxheight;
#ifdef VCLASS
	VRect			r;
	int 			lasty;
	// 10/2/97
	float			nodegap;
	float			leafgap;

	VFontPtr		labelfont;
	VFontPtr		edgefont;
	unsigned int 	style;
	double			scale;
	double			leafangle;
#endif

	// 21/4/97
#ifdef ROTATE_LABELS
	VRect			treeExtent;
	VRect			labelExtent;
#endif


}treeplot;

#endif

