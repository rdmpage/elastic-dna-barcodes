// Base tree class

#ifndef BASETREEH
#define BASETREEH


#include "cpvars.h"
#include "bitset.h"
#include "cputils.h"
#include "error.h"
#include "BaseNode.h"
#include "plot.h"
#include "BaseStack.h"
#include "flags.h"

#ifndef EOLN
	#define EOLN '\n'
#endif

#define TF_ROOTED 	     	0x0001 // tree is rooted
#define TF_DEFAULT	     	0x0002 // tree is the default tree
#define TF_VALID    		0x0004 // tree's fields are valid
#define TF_HAS_EDGES     	0x0008 // tree has edge lengths
#define TF_HAS_LABELS		0x0010 // tree's leaves are labelled
#define TF_ULTRAMETRIC		0x0020 // tree is ultrametric
#define TF_TRANSLATE		0x0040 // don't use labels in tree description
#define TF_INTERNAL_LABELS  0x0080

#define DAYLIGHT

// tree object
class BaseTree : public flags
{
public:
	BaseTree();
	virtual	~BaseTree();

	void		CleanUp ();
	void 		ClearFlags (int f);

	void		DeleteNode(BaseNode *anode) { delete anode; };
	void 		DeleteTree ();
	void		Draw (ostream &f, BOOL compress = FALSE);

#ifdef VCLASS
	virtual void Draw (VRect r, VFontPtr font, unsigned int style = TS_DEFAULT, VFontPtr edgeFont = NULL);
#endif

	BaseNodePtr	GetCurNode () { return (CurNode); };
	int		    GetError () { return (Error); };
	int		    GetInternals () { return (Internals); };
	BaseNodePtr	GetIthNode (int i) { return (Nodes[i]); };
	float		GetMaxHeight ();
	int		    GetLeaves () { return (Leaves); };
	int		    GetNodes () { return (Leaves + Internals); };
	char		*GetName () { return (Name); };
	BaseNodePtr	GetRoot () { return (Root); };

	void		IncrementInternals () { Internals++; };
	void		IncrementLeaves () { Leaves++; };
	void		Invalidate () { SetFlag (TF_VALID, False); };
	BooleaN	    IsBinary () { return (BooleaN) (Leaves == (Internals + 1)); };
	int 		IsFinished ();
	BooleaN		IsRooted () { return (BooleaN) IsFlag (TF_ROOTED); };
	BooleaN 	IsUltrametric ();

	BaseNodePtr LUB (Bitset &b);

	int 		MakeChild ();
	void 		MakeCurNodeALeaf (int i);
	int 		MakeRoot ();
	void 		MakeRootBinary ();
	int		    MakeSibling ();

	virtual 	BaseNodePtr NewNode() { return new BaseNode(); };

	int 		PopNode ();


	void		SetInternals (int i) { Internals = i; };
	void		SetLeaves (int i) { Leaves = i; };
	void		SetName (const char *aname);
	void		SetRoot (BaseNodePtr p) { Root = p; };


	void		Update ();


	void 		WriteNEXUS (ostream &f); // 11/9/96 changed from VOStream to ostream

// Glasgow
	void 		ReRoot (BaseNodePtr OG_PTR);
	void 		MarkPath (BaseNodePtr p);
	BaseNodePtr CanRootWith (Bitset &outgroup);
	void 		RadialDraw (VRect r, VFontPtr font, unsigned int style, VFontPtr edgeFont = NULL);
	void 		GetCentroid (BaseNodePtr &centre, BooleaN &edge);

	// 11/7/96
	// Code for consensus trees
	virtual void StarTree (int n);
   	virtual int  AddCluster (Bitset &b, float e = -1.0, char *slabel = NULL, long ilabel= -1);

protected:
	int			Error;
	int			Leaves;
	char		*Line;		//line buffer for tree drawing
	int 		Internals;
	char		*Name;
	BaseNodePtr		CurNode;
	BaseNodePtr		Root;
	BaseNodePtr		Nodes[MAXNODES];
	BaseNodeStack	St;

	treeplot 	tp;
	void 		calc (BaseNodePtr p);
	void 		getHeights(BaseNodePtr p);
	void 		getNodeHeights(BaseNodePtr p);
	void 		getLeafAngles (BaseNodePtr p);
	void 		drawTraverse (BaseNodePtr p);
	void 		radialCalc (BaseNodePtr p);

	void 		buildtraverse (BaseNodePtr p);
	void 		compresstraverse (BaseNodePtr p);
	void 		clearNode (BaseNodePtr p, int f);
	void 		deletetraverse (BaseNodePtr p);
	void 		dumptraverse (BaseNodePtr p);
	void 		traverse (BaseNodePtr p);

	void 		ultratrav (BaseNodePtr p);

	int			count;
	int			avalue;
	ostream		*treeStream;

	BooleaN		aflag;

	void 		flagTrav (BaseNodePtr p);

	// Glasgow
	BaseNodePtr FirstDescendant (BaseNodePtr p);
	BaseNodePtr NextDescendant ();
	BaseNodePtr nextNonOGDesc ();
	void 		listOtherDesc (BaseNodePtr p);
	BaseNodePtr	AddThere;
	void 		radialDrawTraverse (BaseNodePtr p);
	void 		centroidTraverse (BaseNodePtr p);
	void		getRadialNodeHeights (BaseNodePtr p);	
	// 11/7/96
	void checkSiblings (BaseNodePtr q, Bitset &b);

	//Home 4/5/97
	virtual void DrawInteriorEdge (BaseNodePtr p);
	virtual void DrawPendantEdge (BaseNodePtr p);
	virtual void Fill_In_Ancestors (BaseNodePtr p);
	virtual void drawAsTextTraverse (BaseNodePtr p);

#ifdef DAYLIGHT
    virtual void 		traverseDaylight (BaseNodePtr p);

#endif



};

typedef BaseTree *BaseTreePtr;

#endif

