/***************************************************************************
*                                                                                            
*  	COMPONENT C++ translation                                             
*                                                                          
*  	(c) 1993, Roderic D. M. Page                                          
*      All rights reserved                                                 
*                                                                        
*                                                                          
*  	Compiler:  Borland Turbo C++ for Windows 3.1                          
*  	Platform:  Microsoft Windows 3.1                                    
*                                                                          
*      File: tree.h                                                      
*      Purpose: base tree class        r                                    
*      History: 15 June 1993                                                
* 	                                                                      
***************************************************************************/

// 29/6/94 Compiled with BC++ 4.0

#ifndef TREEH
#define TREEH


#include <fstream.h>
#include "cpvars.h"
#ifdef UseMikesBBitset
 #include "BasicBitset.h"
#else
#include "bitset.h"
#endif
#include "cputils.h"
#include "error.h"
#include "labels.h"
#include "node.h"
#include "parse.h"
#include "perm.h"
#include "plot.h"
#include "stack.h"
#include "flags.h"

#define TF_ROOTED 	     	0x0001 // tree is rooted
#define TF_DEFAULT	     	0x0002 // tree is the default tree
#define TF_VALID				0x0004 // tree's fields are valid
#define TF_HAS_EDGES     	0x0008 // tree has edge lengths
#define TF_HAS_LABELS		0x0010 // tree's leaves are labelled
#define TF_ULTRAMETRIC		0x0020 // tree is ultrametric
#define TF_TRANSLATE			0x0040 // don't use labels in tree description
#define TF_INTERNAL_LABELS 0x0080 // internal nodes are labelled

// 29/12/97 test of daylight algorithm
#define DAYLIGHT


#define newbuf 	// New buffer structure
#ifdef newbuf
// Tree storage
typedef struct  {
	int		leaves;
	int		internals;
	int		*ancfunc;
	float		*edges;
	char		*name;
	char		**nodeLabels; 
}TreeRecord;
typedef TreeRecord *TreeRecPtr;
#endif

// Glasgow
enum LadderType {Ladder_Default, Ladder_Left, Ladder_Right};

#ifdef RECONCILE
class AncestorFunction;
#endif

// tree object
class Tree : public flags
{
#ifdef RECONCILE
friend class AncestorFunction;
#endif
public:
	Tree();
	Tree (Tree &t);    
	virtual	~Tree();

	virtual void		CleanUp ();
	virtual void 		ClearFlags (int f);

	virtual void		DeleteNode(Node *anode) { delete anode; };
	virtual void 		DeleteTree ();
	virtual void		Draw (ostream &f, BOOL compress = FALSE);

#ifdef VCLASS
	virtual void Draw (VRect r, VFontPtr font, unsigned int style = TS_DEFAULT,
					VFontPtr edgeFont = NULL);
#endif

	virtual void		Dump ();

	virtual NodePtr		GetCurNode () { return (CurNode); };
	virtual int			GetError () { return (Error); };
	virtual int			GetInternals () { return (Internals); };
	virtual NodePtr		GetIthNode (int i) { return (Nodes[i]); };
	virtual float		GetMaxHeight ();
	virtual int			GetLeaves () { return (Leaves); };
	virtual int			GetNodes () { return (Leaves + Internals); };
	virtual char		*GetName () { return (Name); };
	virtual NodePtr		GetRoot () { return (Root); };

	virtual void		IncrementInternals () { Internals++; };
	virtual void		IncrementLeaves () { Leaves++; };
	virtual void		Invalidate () { SetFlag (TF_VALID, False); };
	virtual BooleaN		IsBinary () { return (BooleaN) (Leaves == (Internals + 1)); };
	virtual int 		IsFinished ();
	virtual BooleaN		IsRooted () { return (BooleaN) IsFlag (TF_ROOTED); };
	virtual BooleaN 	IsUltrametric ();

	virtual void 		LabelLeaves (Labels &l);
#ifdef UseMikesBBitset
	virtual NodePtr 	LUB(BBitset &b);
#else
	virtual NodePtr 	LUB (Bitset &b);
#endif

	virtual int 		MakeChild ();
	virtual void 		MakeCurNodeALeaf (int i);
	virtual int 		MakeRoot ();
	virtual void 		MakeRootBinary ();
	virtual int			MakeSibling ();
	
	virtual NodePtr 	NewNode() { return new Node(); };

	virtual char 		*Parse (char *TreeDescr);
	virtual void 		PermuteLabels (Permutation &p);
	virtual int 		PopNode ();

#ifdef VCLASS
	virtual NodePtr 	PtOnNode (PoinT &pt, BooleaN left = True);
#endif
	virtual void 		RandomiseBranchLengths ();
	virtual void 		RotateDescendants (NodePtr p);

	virtual void 		SetFlagAbove (NodePtr p, int flag, BooleaN on);
	virtual void		SetInternals (int i) { Internals = i; };
	virtual void		SetLeaves (int i) { Leaves = i; };
	virtual void		SetName (const char *aname);
	virtual void		SetRoot (NodePtr p) { Root = p; };


#ifdef newbuf
	virtual int			TreeFromRecord (TreeRecPtr tr);
	virtual void		TreeToRecord (TreeRecPtr tr);

#else
	virtual int 		StringToTree (unsigned char *s);
	virtual unsigned char *TreeToString ();
#endif

	virtual void		Update ();

	virtual void 		write ();

	virtual void 		WriteNEXUS (ostream &f);
	virtual void 		WriteNEXUS_BPA (ostream &f);
	virtual void 		WriteDANRAN1 (ostream &f);

	// Glasgow
	virtual void 		Ladderise (LadderType Ladder);
	virtual void 		ReRoot (NodePtr OG_PTR);
	virtual void 		MarkPath (NodePtr p);
#ifdef UseMikesBBitset
	virtual NodePtr 	CanRootWith(BBitset &outgroup);
#else
	virtual NodePtr 	CanRootWith(Bitset &outgroup);
#endif
	virtual void 		RadialDraw (VRect r, VFontPtr font, unsigned int style, VFontPtr EdgeFont = NULL);
	virtual void 		GetCentroid (NodePtr &centre, BooleaN &edge);
	// 29/3/96
	virtual void		WriteHennig86 (ostream &f);
	virtual void		WritePHYLIP (ostream &f);

#ifdef RECONCILE
	// 22/4/96
	// Access to the node stack...
	virtual void 		StackClear () { St.Clear (); };
	virtual void 		StackPop () { St.Pop (); };
	virtual void 		StackPush (NodePtr p) { St.Push (p); };
	virtual NodePtr		StackTop () { return St.TopOfStack () ; };
	virtual BooleaN		StackEmpty () { return (BooleaN) St.EmptyStack (); };

	// Copying subtrees...
	virtual NodePtr 	CopyOfSubtree (NodePtr RootedAt);
#endif

	virtual int 		NodesBetween (NodePtr descendant, NodePtr ancestor);
	// 17/6/96
#ifdef NODE_DEBUG
	virtual void 		MemDump (ofstream &f);
#endif

	// 11/7/96
	// Code for consensus trees
	virtual void 		StarTree (int n);
#ifdef UseMikesBBitset
	virtual int 		AddCluster (BBitset &b, char *label = NULL);
#else
	virtual int 		AddCluster (Bitset &b, char *label = NULL);
#endif

	// 19/9/96
	virtual void 		LadderAbove (NodePtr p, LadderType Ladder);
	virtual NodePtr 	RemoveNode (NodePtr Node);
	// 31/10/96
	// GeneTree
#ifdef UseMikesBBitset
	virtual void 		PruneByMarking (BBitset &keep);
#else
	virtual void 		PruneByMarking (Bitset &keep);
#endif
	virtual int 		UnmarkedNodesBetween (NodePtr descendant, NodePtr ancestor);

	//28/7/97
	virtual float 		PathDifference (Tree &t, BooleaN useEdgeLength = False);
	virtual float 		PathLengthBetweenNodes (int i, int j, BooleaN useEdgeLengths = False);
	virtual float 		PathLengthBetweenNodes (NodePtr p, NodePtr q, BooleaN useEdgeLengths = False);
	virtual void 		MarkPathij (NodePtr i, NodePtr j);
	virtual void 		UnMarkPathij (NodePtr i, NodePtr j);
	virtual void 		UnMarkPath (NodePtr p);

	// 29/7/97

	// 18/8/97
#ifdef UseMikesBBitset
	virtual void 		Prune (BBitset &ToBePruned);
#else
	virtual void 		Prune (Bitset &ToBePruned);
#endif
	NodePtr				operator[] (const int i) { return Nodes[i]; }    // Access node
	
	// 27/1/98
	virtual 			void GetNodeDepths ();

    // 7/10/99
    virtual				bool HasInternalLabels ();

	virtual 			void MidpointRoot();    

protected:
	int			Error;
	int			Leaves;
	char		*Line;		//line buffer for tree drawing
	int 		Internals;
	char		*Name;
	NodePtr		CurNode;
	NodePtr		Root;
	NodePtr		Nodes[MAXNODES];
	NodeStack 	St;

	treeplot 	tp;
	virtual void 		calc (NodePtr p);
	virtual void 		getHeights(NodePtr p);
	virtual void 		getNodeHeights(NodePtr p);
	virtual void 		getLeafAngles (NodePtr p);
	virtual void 		drawTraverse (NodePtr p);
	virtual void 		radialCalc (NodePtr p);

	virtual void 		buildtraverse (NodePtr p);
	virtual void 		compresstraverse (NodePtr p);
	virtual void 		clearNode (NodePtr p, int f);
	virtual void 		deletetraverse (NodePtr p);
	virtual void 		dumptraverse (NodePtr p);
	virtual void 		traverse (NodePtr p);

	virtual void 		ultratrav (NodePtr p);

	int			count;
	int			avalue;
	ostream		*treeStream;

#ifdef VCLASS
	PoinT		mousePt;
	virtual void 		findNode (NodePtr p, BooleaN left);
#endif

	BooleaN		aflag;

	virtual void 		flagTrav (NodePtr p);

	// Glasgow
	LadderType 	Laddering;
	virtual void 		SortDescendants (NodePtr node, LadderType Ladder);
	virtual BooleaN 	MustSwap (NodePtr L, NodePtr R, LadderType Ladder);
	virtual void 		ladderTraverse (NodePtr p);
	virtual NodePtr 	FirstDescendant (NodePtr p);
	virtual NodePtr 	NextDescendant ();
	virtual NodePtr 	nextNonOGDesc ();
	virtual void 		listOtherDesc (NodePtr p);
	NodePtr		AddThere;
	virtual void 		radialDrawTraverse (NodePtr p);
	virtual void 		centroidTraverse (NodePtr p);
	virtual void		getRadialNodeHeights (NodePtr p);
	// 29/3/96
	virtual void		Hennig86traverse (NodePtr p);

#ifdef RECONCILE
	virtual void 		copyTraverse (NodePtr p1, NodePtr &p2);
#endif

#ifdef NODE_DEBUG
	virtual void 		memDumptraverse (NodePtr p, ofstream &f);
#endif

	// 11/7/96
#ifdef UseMikesBBitset
	virtual void 		checkSiblings (NodePtr q, BBitset &b);
#else
	virtual void 		checkSiblings (NodePtr q, Bitset &b);
#endif

   // 6/5/97
	virtual void 		DrawInteriorEdge (NodePtr p);
	virtual void 		DrawPendantEdge (NodePtr p);
	virtual void 		Fill_In_Ancestors (NodePtr p);
	virtual void 		drawAsTextTraverse (NodePtr p);

	// 29/12/97
#ifdef DAYLIGHT
    virtual void 		traverseDaylight (NodePtr p);
#endif

	// 27/1/98
	virtual 			void getNodeDepth(NodePtr p);

    // 7/10/99
    virtual				void hasInternalLabelsTraverse (NodePtr p);


};

typedef Tree *TreePtr;

#endif







