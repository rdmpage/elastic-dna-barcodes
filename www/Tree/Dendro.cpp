//--------------------------------------------------------------------------
//
// (c) 1993, Roderic D. M. Page
// Department of Zoology, University of Oxford
// South Parks Road, Oxford OX1 3PS, UK
// rod.page@zoology.oxford.ac.uk
//
// Filename:  		dendro.cpp
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

#include <math.h>
#ifdef UseMikesBBitset
 #include "BasicBitset.h"
#else
 #include "bitset.h"
#endif
#include "dendro.h"


//////////////////////////////////////////////////////////////////////////////
// Allocate memory for vector of cluster heights
void Dendrogram::AllocateHeights ()
{
	if (heights == NULL) heights = new float[Leaves];
	for (int i = 0; i < Leaves; i++) heights[i] = 0.0;
}

// Get horizons between cluster heights
void Dendrogram::GetHorizons ()
{
	if (Horizons) delete [] Horizons;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
	Horizons = new float[Leaves - 1];
	for (int i = 0; i < Leaves - 1; i++)
	{
		Horizons[i] = heights[i + 1] - heights[i];
	}
}

// Randomise horizons to get new cluster heights
void Dendrogram::RandomHeightsFromHorizons ()
{
	Permutation p (Leaves - 1);
	p.Permute ();
   heights[0] = 0.0;
	for (int i = 0; i < Leaves-1; i++)
	{
		heights[i + 1] = heights[i] + Horizons[p.P[i]];
	}
}
   	
	 



//////////////////////////////////////////////////////////////////////////////
// Generate coalescent times for Yule model given lamba.
// Formula based on Hey (1992). The vector heights has coalescent
// events in ascending order with heights[i] being the height above the root of
// the ith event.
void Dendrogram::Yule (float lambda)
{
	AllocateHeights ();
	float t = 0.0;
   heights[0] = 0.0;
	for (int i = 1; i < Leaves; i++)
	{
		t += -( (1 / (lambda * (i + 1))) * (log (U.Random01())) );
		heights[i] = t;
	}
}


// Get ML estimate of lambda from observed cluster heights
float Dendrogram::GetLambda ()
{
	float sum = 0.0;
   float result;

	for (int i = 1; i < Leaves - 1; i++)
	{
     	sum += (heights[i] - heights[i - 1]) * (i + 1); 
	}
   result = (float) (Leaves - 2) / sum;
	return result;
}

// Obtain ranks from input heights
void Dendrogram::GetHeights ()
{
	AllocateHeights ();

	// Calculate heights
	Root->SetHeight (0.0);
	getHeights (Root);
	for (int i = 0; i < Leaves; i++) heights[i] = tp.maxheight;

	rankTraverse (Root);
}


void Dendrogram::rankTraverse (NodePtr p)
{
	if (p)
	{
		if (!p->IsLeaf ())
		{
			float h = p->GetHeight ();
			// insert into list
			int i = 0;
			while (i < Leaves && h > heights[i]) i++;
			if (heights[i] == tp.maxheight) heights[i] = h;
			else
			{
				// insert into list
				for (int j = Leaves - 1; j > i; j--)
				{
					heights[j] = heights[j-1];
            }
				heights[i] = h;
			}
		}
		rankTraverse (p->GetChild ());
		rankTraverse (p->GetSibling ());
	}
}

//////////////////////////////////////////////////////////////////////////////
// Assign heights (and edge lengths) to nodes 
void Dendrogram::LabelWithHeights ()
{
	if (!IsFlag (TF_HAS_RANKS)) LabelWithRanks ();
	heightTraverse (Root);
}

void Dendrogram::heightTraverse (NodePtr p)
{
	if (p)
	{   	
		p->SetHeight (heights[p->GetIndex() - 1]);
		if (p == Root)
		{
			p->SetEdgeLength (0.0);
		}
		else
		{
			p->SetEdgeLength (p->GetHeight () - p->GetAnc()->GetHeight ());
      }
		heightTraverse (p->GetChild());
		heightTraverse (p->GetSibling ());
	}
}

//////////////////////////////////////////////////////////////////////////////
// Randomly assign ranks 1..n-1 to each cluster in the dendrogram such that
// order of heights is preserved.
// Uses cluster field so clears TF_VALID flag to ensure tree is updated
void Dendrogram::LabelWithRanks ()
{
   Update (); // ensure clusters are valid
	Root->DeleteFromCluster (Leaves); // ensure
	labelTraverse (Root);
	SetFlag (TF_VALID, False);
	SetFlag (TF_HAS_RANKS, True);
}

//////////////////////////////////////////////////////////////////////////////
// Randomly label clusters in order of height 1..n-1
// Adapted from old AMNH code RATES.PAS
void Dendrogram::labelTraverse (NodePtr p)
{
	if (p)
	{
		if (p->IsLeaf ())
		{
			p->SetIndex (Leaves);
		}
		else
		{
       	// Get first element in set, then remove it
			int j = p->GetIthElement (1);
			p->SetIndex (j);
//         p->SetLabelNumber (j);
			p->DeleteFromCluster (j);

			// Choose at random from the remaining cluster heights enough
			// heights to label the left subtree. The remainder are used to
			// label the right subtree
#ifdef UseMikesBBitset
			BBitset leftset;
#else
			Bitset leftset;
			leftset.NullSet ();
#endif

			for (int i = 1; i < p->GetChild()->GetWeight(); i++)
			{
           	// Choose an element at random...
				j = p->ClusterCardinality();
				int k = p->GetIthElement (U.RandomInteger (1, j));

             // .. add to left set
#ifdef UseMikesBBitset
				leftset.add(k);
#else
				leftset.AddToSet (k);
#endif
            // ...and delete from cluster so not sampled again
				p->DeleteFromCluster (k);
			}

			p->GetChild()->ClearCluster();
#ifdef UseMikesBBitset
			p->GetChild()->AddCluster(leftset);
#else
			p->GetChild()->AddCluster (leftset);
#endif
			p->GetChild()->GetSibling()->ClearCluster();
			p->GetChild()->GetSibling()->AddClusterToCluster (p);
		}
		if (p == Root) p->SetEdgeLength (0.0);
		else p->SetEdgeLength (p->GetIndex () - p->GetAnc()->GetIndex());

		labelTraverse (p->GetChild());
		labelTraverse (p->GetSibling ());
	}
}




