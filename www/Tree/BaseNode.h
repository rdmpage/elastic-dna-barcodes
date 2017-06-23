// Base node in a tree

#ifndef BASENODEH
#define BASENODEH

#include <stdlib.h>
#include <stdio.h>
#include <iomanip.h>
#include <string.h>

#include "Bitset.h"
#include "Flags.h"
#include "Cpvars.h"
#include "Plot.h"

#ifdef VCLASS
	#include "VPort.h"
#endif

// Node flags
#define NF_LEAF 	0x001	// node is a leaf
#define NF_MARKED	0x4000	// node is marked

class BaseTree; // forward declaration of Tree

// Node in a n-tree
class BaseNode : public flags
{
friend BaseTree; // Tree has access to Node
public:
	BaseNode();
	virtual ~BaseNode() { if (label) delete label;};

	virtual void		AddClusterToCluster (BaseNode *p) { cluster.AddSetToSet (p->cluster); };
	virtual void		AddCluster (Bitset &b) { cluster.AddSetToSet (b); };
	virtual void		AddToCluster (int i) { cluster.AddToSet (i); };
	virtual void		AddWeight (int i) { weight += i; };

	virtual void		ClearCluster () { cluster.NullSet(); };
	virtual int			ClusterCardinality () { return (cluster.Cardinality ()); };
	virtual SetRelations ClusterRelationship (Bitset &b) { return (cluster.Relationship (b)); };

	virtual void		DeleteFromCluster (int i) { cluster.DeleteFromSet (i); };

	virtual int 		GetDegree() { return (degree); };
	virtual float		GetEdgeLength () { return (EdgeLength); };
	virtual float		GetHeight () { return (height); };
	virtual int			GetIthElement (int i) { return (cluster.GetIthElement (i)); };
	virtual int 		GetIndex() { return (index); };
	virtual char		*GetLabel() { return (label); }
	virtual int 		GetLabelNumber() { return (LabelNumber); };
	virtual int 		GetLeafNumber() { return (LeafNumber); };
	virtual int 		GetWeight() { return (weight); };
	virtual void		GetXY (PoinT &pt) { LoadPt (&pt, xy.x, xy.y); };
	virtual void		GetLabelXY (PoinT &pt) { LoadPt (&pt, labelxy.x, labelxy.y); };
	virtual BaseNode	*GetAnc() { return (anc); };
	virtual BaseNode	*GetChild() { return (child); };
	virtual int			GetFirstClusterElement() { return (cluster.GetFirstElement());};
	virtual int			GetNextClusterElement() { return (cluster.GetNextElement());};
	virtual BaseNode	*GetRight () { return (right); };
//	virtual BaseNode	*GetRightMost ();
	virtual BaseNode	*GetRightMostSibling();
	virtual BaseNode	*GetSibling() { return (sib); };
//	int			GetValue () { return (value); };

	virtual void		IncrementDegree () { degree++; };
//	virtual void		IncrementValue () { value++; };
	virtual BooleaN 	IsALeftDescendantOf (BaseNode *q);
	virtual BooleaN 	IsAnAncestorOf (BaseNode *p);
	virtual int			IsBinary () { return (degree == 2); }
	virtual BooleaN		IsTheChild () { return BooleaN (anc->child == this); }
	virtual int			IsLeaf (){ return (IsFlag (NF_LEAF)); };
	virtual int			IsMoreClusterElements() { return (cluster.IsMore()); };

	virtual BaseNode	*MRCA (BaseNode *p);

	virtual void		SetAnc (BaseNode *n){ anc = n; };
	virtual void		SetChild (BaseNode *n){ child = n; };
	virtual void		SetChildSibling (BaseNode *n) { child->sib = n; };
	virtual void		SetDegree (int i) {degree = i; };
	virtual void		SetEdgeLength (float l) { EdgeLength = l;};
	virtual void 		SetHeight (float h) { height = h;};
	virtual void 		SetIndex (int i) { index = i; };
	virtual void		SetLabel (const char *alabel);
	virtual void 		SetLabelNumber (int i) { LabelNumber = i; };
	virtual void		SetLeaf() { SetFlag (NF_LEAF, True); };
	virtual void 		SetLeafNumber (int i) { LeafNumber = i; };
	virtual void		SetRight (BaseNode *n) { right = n; };
//	virtual void 		SetRightMost (BaseNode *p);
	virtual void		SetSibling (BaseNode *n){ sib = n; };
//	virtual void		SetValue (int i) { value = i; };
	virtual void 		SetWeight (int i) { weight = i; };
	virtual void		SetXY (const PoinT &pt) { LoadPt (&xy, pt.x, pt.y); };

	virtual void 		Calc (treeplot &tp);
	virtual void 		Draw (treeplot &tp);
	virtual void 		RadialCalc (treeplot &tp);
	virtual void 		RadialDraw (treeplot &tp);

   	virtual void		GetCluster (Bitset &b) { b.NullSet(); b.AddSetToSet (cluster); };

	// 11/9/96
   	virtual int 		GetDegreeOf ();
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
    

protected:
	int 		index;
	int 		weight;
	int			degree;
	int			LeafNumber;
	int			LabelNumber;
	char		*label;
	float		EdgeLength;
	float		height;
	BaseNode 	*child;
	BaseNode	*sib;
	BaseNode	*anc;
	BaseNode	*right;
	Bitset		cluster;

//	int			value;		// general purpose value

	PoinT		xy;			// xy coordinates of node
	PoinT		labelxy;	// xy coordinates of end of leaf label

	float		angle;
    float		radius;
    float		aleft;
    float		aright;

};

// Pointer to a node
typedef BaseNode *BaseNodePtr;


#endif

