//
// (c) 1993, Roderic D. M. Page
//
// Filename:		bintree.h
//
// Author:       	Rod Page
//
// Date:            23 July 1993
//
// Class:			BinaryTree
//
// Inheritance:	Tree->BinaryTree
//
// Purpose:		binary tree
//
// Modifications:
//
//

#include "bintree.h"

////////////////////////////////////////////////////////////////////////////
// Add node beneath below. Doesn't update clusters, etc 	
void BinaryTree::AddNodeBelow (Node *node, Node *below)
{

	NodePtr ancestor = node->GetAnc();

	if (ancestor->GetChild() != NULL)
	{
     	// node is left desc
     	ancestor->SetChildSibling(below);
		ancestor->SetSibling(below->GetSibling());
		below->SetSibling(NULL);
	}
	else
	{
     	// node is right desc
		ancestor->SetChild(below);
		ancestor->SetSibling(below->GetSibling());
		below->SetSibling(node);
     }
     	
	if (below->GetAnc() == NULL) Root = ancestor;
	else
	{
		if (below == below->GetAnc()->GetChild())
          {
			below->GetAnc()->SetChild(ancestor);
          }
		else
		{
			below->GetAnc()->SetChildSibling(ancestor);
		}
	}
	ancestor->SetAnc (below->GetAnc());
	below->SetAnc (ancestor);

	// Invalidate tree so we know to update it if necessary
	SetFlag (TF_VALID, False);
}

////////////////////////////////////////////////////////////////////////////
// Allocate memory for n leaves and m = n-1 internals and create tree (1,2).
// Nodes m..m + n are ancestors of nodes 1..n, and leaves 2..n are all
// children  
void BinaryTree::TwoLeafTree (int n)
{
	int m = 2 * n - 1;

     // Allocate memory
     int i;
	for (i = 0; i < m; i++) Nodes[i] = NewNode();

     // Set up leaves 3..n
     m = n;
	for (i = 2; i < n; i++)
	{
     	int a = m + i - 1;
		Nodes[i]->SetAnc(Nodes[a]);
		Nodes[a]->SetChild(Nodes[i]);
		Nodes[i]->SetLeaf();
		Nodes[i]->SetLeafNumber(i + 1);
		Nodes[i]->SetLabelNumber(i + 1);
	}

	// Create tree (1,2)
	Nodes[0]->SetLeaf();
	Nodes[0]->SetLeafNumber(1);
	Nodes[0]->SetLabelNumber(1);
	Nodes[1]->SetLeaf();
	Nodes[1]->SetLeafNumber(2);
	Nodes[1]->SetLabelNumber(2);
	Nodes[m]->SetChild(Nodes[0]);
	Nodes[0]->SetSibling(Nodes[1]);
	Nodes[0]->SetAnc(Nodes[m]);
	Nodes[1]->SetAnc(Nodes[m]);
	Root 	= Nodes[m];
	Leaves 	= 2;
	Internals = 1;

	// Invalidate tree so we know to update it if necessary
	SetFlag (TF_VALID, False);
}
////////////////////////////////////////////////////////////////////////////
void BinaryTree::ThreeLeafTree (int n)
{
	TwoLeafTree (n);
	AddNodeBelow (Nodes[2], Nodes[1]);
	Leaves++;
	Internals++;
	SetFlag (TF_ROOTED, False);
}

////////////////////////////////////////////////////////////////////////////
// Generate a random binary tree from the set of all possible rooted
// or unrooted binary trees 
void BinaryTree::Random (BooleaN rooted, Uniform &U, int n)
{
	int start;
	int there;

	if (rooted)
     {
		TwoLeafTree(n);
		start = 2;
	}
	else
	{
		ThreeLeafTree(n);
		start = 3;
     }

	int i;
	for (i = start; i < n; i++)
	{
		if (rooted)
		{
			there = (int) U.RandomInteger (0, Leaves + Internals - 1);
			// Adjust if internal edge
			if (there >= i)
			{
				there = there - i + n;
			}
		}
		else
		{
			there = (int) U.RandomInteger (1, Leaves + Internals - 2);
			// Adjust if internal edge
			if (there >= i) there = there - i + n + 1;
		}
		AddNodeBelow (Nodes[i], Nodes[there]);

		Leaves++;
		Internals++;
	}
	SetFlag (TF_VALID, False);

}



////////////////////////////////////////////////////////////////////////////
// Generate a random binary dendrogram (Markovian tree).
void BinaryTree::Markovian (Uniform &U, int n)
{
	TwoLeafTree(n);
	for (int i = 2; i < n; i++)
	{
     	// Find next leaf to "speciate"
		int there = (int) U.RandomInteger (0, i - 1);
		AddNodeBelow (Nodes[i], Nodes[there]);
		Leaves++;
		Internals++;
	}
	Permutation p (n, U.GetSeed ());
	p.Permute (); // 10/2/97 Forgot this!
	PermuteLabels (p);
}

// 16/5/96
//-----------------------------RemoveNode-----------------------------------
/*
   {
     Remove node Node and its ancestor from tree and return the
	  node (p) below which Node's anc was removed.
     Preserve the Node^.anc = NodeAnc relationship, and
     the direction of the descendant. Two results:

	  Left desc               Right desc
     ---------               ----------

     Node                         Node
      \.     p                p    .
       \.   /        and      \  .
        \. /                   \.
      NodeAnc--||            NodeAnc--||

   }
*/
NodePtr BinaryTree::RemoveNode (NodePtr Node)
{
	NodePtr NodeAnc = Node->GetAnc();
	NodePtr p;
	if (Node->IsTheChild())
		p = Node->GetSibling();
	else
	{
		p = NodeAnc->GetChild();
		NodeAnc->SetChild (NULL);
	}
	NodePtr q = NodeAnc->GetAnc();
	if (q == NULL)
	{
		p->SetSibling (NULL);
		Root = p;
	}
	else
	{
		if (NodeAnc->IsTheChild())
			q->SetChild (p);
		else
			q->GetChild()->SetSibling (p);
	}

	p->SetSibling (NodeAnc->GetSibling());
	p->SetAnc (q);

	NodeAnc->SetSibling (NULL);

	return p;
}

void BinaryTree::resolveTraverse (NodePtr p)
{
	if (p)
    {
 		if (!p->IsLeaf())
        {
        	int d = p->GetDegreeOf();
        	if (d > 2)
            {
            	// Resolve polytomy semi-randomly

                // Get array of descendants
                NodePtr descendant[MAXLEAVES]; 
                int i = 0;
                descendant[0] = p->GetChild();
                while (descendant[i]->GetSibling())
                {
                	descendant[i+1] = descendant[i]->GetSibling();
                    i++;
                }
                descendant[1]->SetSibling (NULL);




                // Resolve
                for (int j = 2; j < d; j++)
                {
					int there = (int) u->RandomInteger (0, j - 1);
                    NodePtr a = NewNode();
                    a->SetChild(descendant[j]);
                    descendant[j]->SetAnc(a);
					AddNodeBelow (descendant[j], descendant[there]);
                    Internals++;
                }
            }
        }
        resolveTraverse (p->GetChild());
        resolveTraverse (p->GetSibling());
    }
}

// Randomly resolve any polytomies in a tree
void BinaryTree::RandomlyResolve (Uniform &U)
{
	if (Internals < Leaves - 1)
    {
    	u = &U;
 		resolveTraverse (Root);
	}
    SetFlag (TF_VALID, False);
    Update ();
}


void BinaryTree::DefaultComb (int n)
{
	TwoLeafTree (n);

    for (int i = 2; i < n; i++)
    {
		AddNodeBelow (Nodes[i], Nodes[i-1]);
		Leaves++;
		Internals++;
	}
}
