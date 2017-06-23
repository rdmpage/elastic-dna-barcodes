//--------------------------------------------------------------------------
//
// (c) 1993, Roderic D. M. Page
// Department of Zoology, University of Oxford
// South Parks Road, Oxford OX1 3PS, UK
// rod.page@zoology.oxford.ac.uk
//
// Filename:		dendro.h
//
// Author:       	Rod Page
//
// Date:            1993
//
// Class:			Dendrogram
//
// Inheritance:
//
// Purpose:
//
// Modifications:
//
//
//--------------------------------------------------------------------------



#include "node.h"
#include "bintree.h"
#include "random.h"

#define TF_HAS_RANKS	0x020	// Tree's clusters are ranked

class Dendrogram : public BinaryTree
{
public:
	Dendrogram () : BinaryTree ()
	{
		heights = NULL;
		Horizons = NULL;
	};
	~Dendrogram ()
	{
		if (heights != NULL) delete [] heights;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
		if (Horizons) delete [] Horizons;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
	}
	void 	GetHeights ();
   void	GetHorizons ();
	float GetLambda ();
	void 	LabelWithRanks ();
	void 	LabelWithHeights ();
	void 	RandomHeightsFromHorizons ();
	void 	Yule (float lambda);
protected:
//	levelptr	Levels;
	Uniform	U;

	float	*heights;
	float	*Horizons;
	void 	heightTraverse (NodePtr p);
	void 	labelTraverse (NodePtr p);
	void 	rankTraverse (NodePtr p);
   void	AllocateHeights ();
};

