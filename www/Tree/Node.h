/***************************************************************************
*                                                                         
*  	COMPONENT Pascal to C++ translation                                   
*                                                                          
*  	(c) 1993, Roderic D. M. Page                                          
*      All rights reserved                                                 
*                                                                          
*                                                                          
*  	Compiler:  Borland Turbo C++ for Windows 3.1                          
*  	Platform:  Microsoft Windows 3.1                                      
*                                                                          
*        File: node.cpp                                                    
*	 Purpose: node class                                                  
*     History: 15 June 1993                                                
* 	                                                                      
***************************************************************************/
#ifndef NODEH
#define NODEH

#include <stdlib.h>
#include <stdio.h>
#include <iomanip.h>
#include <string.h>


#ifdef UseMikesBBitset
 #include "BasicBitset.h"
#else
 #include "Bitset.h"
#endif
#include "Flags.h"
#include "CpVars.h"
#include "Plot.h"

#ifdef VCLASS
	#include "VPort.h"
#endif

//#define NODE_DEBUG		// local debugging flag
#ifdef NODE_DEBUG
	#include <fstream.h>
#endif

// Node flags
#define NF_LEAF 0x001	// node is a leaf
// Flags for kinds of range
#define NF_WIDESPREAD 	0x0010	// node is widespread
#define NF_LAND	 		0x0020	// node is point of arrival
#define NF_JUMPFROM   	0x0040	// node is point of departure
#define NF_LEFTDUP		0x0080
#define NF_RIGHTDUP		0x0100
#define NF_PRESENT		0x0200
#define NF_BETWEENDUPS	0x0400
#define NF_POSTPONE		0x0800
#define NF_HIGHLIGHT	0x1000
#define NF_NAIL      	0x2000
#define NF_MARKED		0x4000	// node is marked

#ifdef RECONCILE
class Tree; // forward declaration of Tree
#endif

// Node in a n-tree
class Node : public flags
{
// Rod, can you see whether this works for earlier MSL?
#ifdef RECONCILE
	friend class Tree;
#endif
//#ifdef RECONCILE
// #if (__MSL__ < 0x4000)
//	friend Tree; // Tree has access to Node
// #else
//	friend class Tree; // Tree has access to Node
// #endif /* __MSL__ */
//#endif
public:
	Node();
	virtual ~Node() { if (label) delete [] label;}	// 25.8.00 MAC -- this should use delete [] since was created with new [].

#ifdef UseMikesBBitset
	virtual void		AddClusterToCluster (Node *p) { cluster.add(p->cluster); }
	virtual void		AddCluster (BBitset &b) { cluster.add(b); }
	virtual void		AddToCluster (int i) { cluster.add(i); }
#else
	virtual void		AddClusterToCluster (Node *p) { cluster.AddSetToSet (p->cluster); }
	virtual void		AddCluster (Bitset &b) { cluster.AddSetToSet (b); }
	virtual void		AddToCluster (int i) { cluster.AddToSet (i); }
#endif
	virtual void		AddWeight (int i) { weight += i; }

#ifdef UseMikesBBitset
	virtual void		ClearCluster () { cluster.clear(); }
	virtual int			ClusterCardinality () { return (cluster.getCard()); }
	virtual SetRels 	ClusterRelationship (BBitset &b) { return (cluster.rel(b)); }
#else
	virtual void		ClearCluster () { cluster.NullSet(); }
	virtual int			ClusterCardinality () { return (cluster.Cardinality ()); }
	virtual SetRelations ClusterRelationship (Bitset &b) { return (cluster.Relationship (b)); }
#endif

#ifdef UseMikesBBitset
	virtual void		DeleteFromCluster (int i) { cluster.del(i); }
#else
	virtual void		DeleteFromCluster (int i) { cluster.DeleteFromSet (i); }
#endif

#ifdef VCLASS
	virtual void 		DrawLabel (BooleaN left);
#endif

#ifdef NODE_DEBUG
	virtual void		Dump ();
#ifdef VCLASS
	virtual void		dumpxy () { cout << "x= " << xy.x << ", y= " << xy.y; }
#endif
#endif

	virtual int 		GetDegree() { return (degree); }
	virtual float		GetEdgeLength () { return (EdgeLength); }
	virtual float		GetHeight () { return (height); }
#ifdef UseMikesBBitset
	virtual int			GetIthElement (int i) { return (cluster.getIthElement (i)); }
#else
	virtual int			GetIthElement (int i) { return (cluster.GetIthElement (i)); }
#endif
	virtual int 		GetIndex() { return (index); }
	virtual char		*GetLabel() { return (label); }
	virtual int 		GetLabelNumber() { return (LabelNumber); }
	virtual int 		GetLeafNumber() { return (LeafNumber); }
	virtual int 		GetWeight() { return (weight); }
#ifdef VCLASS
	virtual void		GetXY (PoinT &pt) { LoadPt (&pt, xy.x, xy.y); }
	virtual void		GetLabelXY (PoinT &pt) { LoadPt (&pt, labelxy.x, labelxy.y); }
#endif
	virtual Node		*GetAnc() { return (anc); }
	virtual Node		*GetChild() { return (child); }
#ifdef UseMikesBBitset
	virtual int			GetFirstClusterElement() { return (cluster.getFirstElement()); }
	virtual int			GetNextClusterElement() { return (cluster.getNextElement()); }
#else
	virtual int			GetFirstClusterElement() { return (cluster.GetFirstElement()); }
	virtual int			GetNextClusterElement() { return (cluster.GetNextElement()); }
#endif
	virtual Node		*GetRight () { return (right); }
	virtual Node		*GetRightMost ();
	virtual Node		*GetRightMostSibling();
	virtual Node		*GetSibling() { return (sib); }
	virtual int			GetValue () { return (value); }

	virtual void		IncrementDegree () { degree++; }
	virtual void		IncrementValue () { value++; }
	virtual void 		InsertBeforeInRightList (Node *p, Node *before);
	virtual BooleaN 	IsALeftDescendantOf (Node *q);
	virtual BooleaN 	IsAnAncestorOf (Node *p);
	virtual int			IsBinary () { return (degree == 2); }
	virtual BooleaN		IsTheChild () { return BooleaN (anc->child == this); }
	virtual int			IsLeaf (){ return (IsFlag (NF_LEAF)); }
#ifdef UseMikesBBitset
	virtual int			IsMoreClusterElements() { return (cluster.hasMore()); }
#else
	virtual int			IsMoreClusterElements() { return (cluster.IsMore()); }
#endif

	virtual Node		*MRCA (Node *p);

#ifdef VCLASS
	virtual BooleaN 	PtOnNode (PoinT &pt, BooleaN Left = True);
#endif

	virtual void 		RotateDescendants ();

	virtual void		SetAnc (Node *n){ anc = n; }
	virtual void		SetChild (Node *n){ child = n; }
	virtual void		SetChildSibling (Node *n) { child->sib = n; }
	virtual void		SetDegree (int i) {degree = i; }
	virtual void		SetEdgeLength (float l) { EdgeLength = l; }
	virtual void 		SetHeight (float h) { height = h; }
	virtual void 		SetIndex (int i) { index = i; }
	virtual void		SetLabel (const char *alabel);
	virtual void 		SetLabelNumber (int i) { LabelNumber = i; }
	virtual void		SetLeaf() { SetFlag (NF_LEAF, True); }
	virtual void 		SetLeafNumber (int i) { LeafNumber = i; }
	virtual void		SetRight (Node *n) { right = n; }
	virtual void 		SetRightMost (Node *p);
	virtual void		SetSibling (Node *n){ sib = n; }
	virtual void		SetValue (int i) { value = i; }
	virtual void 		SetWeight (int i) { weight = i; }
#ifdef VCLASS
	virtual void		SetXY (const PoinT &pt) { LoadPt (&xy, pt.x, pt.y); }
#endif

	// HostTree GLA
	virtual void 		Calc (treeplot &tp);
	virtual void 		Draw (treeplot &tp);
	// Glasgow
	virtual void 		RadialCalc (treeplot &tp);
	virtual void 		RadialDraw (treeplot &tp);

	virtual void 		Copy (Node* theCopy);
#ifdef UseMikesBBitset
	virtual void		GetCluster (BBitset &b) { b.clear(); b.add(cluster); }
#else
	virtual void		GetCluster (Bitset &b) { b.NullSet(); b.AddSetToSet (cluster); }
#endif

	// 17/6/96
#ifdef NODE_DEBUG
	virtual void		MemDump (ofstream &f);
#endif

	// 11/7/96
	virtual int 		GetDegreeOf ();

	//19/9/96
	virtual Node		*LeftSiblingOf ();

	virtual void 		MarkPathTo (Node *p, BooleaN mark);
#ifdef UseMikesBBitset
	virtual bool	 	IsElementOfCluster (int i) { return cluster.contains(i); }
#else
	virtual BooleaN 	IsElementOfCluster (int i) { return cluster.IsElement (i); }
#endif

    virtual void		SetAngle (float a) { angle = a; };
    virtual void		SetRadius (float r) { radius = r; };
    virtual float		GetAngle () { return angle; };
    virtual float		GetRadius () { return radius; };


    virtual void		SetARight (float a) { aright = a; };
    virtual float		GetARight () { return aright; };
    virtual void		SetALeft (float a) { aleft = a; };
    virtual float		GetALeft () { return aleft; };

    float		x;
    float		y;
    
	//28/1/98 Eulsenstein
	Node		*parent;
	Node		*affiliation;
	Node		*basket;
	int			rank;
	Node		*next;

protected:
	int 		index;
	int 		weight;
	int			degree;
	int			LeafNumber;
	int			LabelNumber;
	char		*label;
	float		EdgeLength;
	float		height;
	Node 		*child;
	Node		*sib;
	Node		*anc;
	Node		*right;
#ifdef UseMikesBBitset
	BBitset		cluster;
#else
	Bitset		cluster;
#endif

	int			value;		// general purpose value

#ifdef VCLASS
	PoinT		xy;			// xy coordinates of node
	PoinT		labelxy;		// xy coordinates of end of leaf label
#endif

	float		angle;
    float		radius;
    float		aleft;
    float		aright;
    


};

// Pointer to a node
typedef Node *NodePtr;


#endif


