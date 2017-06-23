// Base tree class

#include <ctype.h>
#include <iomanip.h>
#include <string.h>
#include <math.h>
#include <fstream.h>
#include <iostream.h>

#ifdef Macintosh
	#include "VString.h"
#endif

#define SCALE

#include "BaseTree.h"

#define ROOT_CHAR 219
#define epsilon 0.001
#define pi 3.141592


//------------------------------------------------------------------------------
// Constructor
BaseTree::BaseTree()
{
	Root		= NULL;
	Leaves    	= 0;
	Internals 	= 0;
	Name		= NULL;
	Error		= errOK;
	SetFlag(TF_ROOTED, True); // tree is rooted by default
    SetFlag(TF_VALID, False);
}

/* Traverse tree in postorder deleting nodes by calling DeleteNode */
void BaseTree::deletetraverse (BaseNodePtr p)
{
	if (p)
	{
		deletetraverse (p->GetChild());
		deletetraverse (p->GetSibling());
		DeleteNode (p);
	}
}

//------------------------------------------------------------------------------
// destructor
BaseTree::~BaseTree()
{
	DeleteTree();
}


//a

//b
//------------------------------------------------------------------------------
void BaseTree::buildtraverse (BaseNodePtr p)
{
	if (p)
	{
		p->ClearCluster ();
		p->SetWeight (0);
      p->SetDegree (0);
		buildtraverse (p->GetChild ());
		buildtraverse (p->GetSibling ());
		if (p->IsLeaf())
		{
			p->AddToCluster (p->GetLeafNumber ());
			p->SetWeight (1);
               Nodes[p->GetLabelNumber () - 1] = p;
		}
		else
		{
			p->SetLabelNumber (count + 1);
			Nodes[count++] = p;
			 }
		if (p != Root)
		{
			p->GetAnc()->AddClusterToCluster (p);
			p->GetAnc()->AddWeight (p->GetWeight());
			p->GetAnc()->IncrementDegree();
		}
	}
}

//c

//------------------------------------------------------------------------------
// Free any nodes in the node list and node stack.
// Called by tree reading classes if they encounter an error.
void BaseTree::CleanUp ()
{
	// Clear up the node list
	int count = 0;
     int j = 0;

	while ((count < Leaves) && (j < MAXLEAVES))
	{
		while (!Nodes[j]) j++;
		DeleteNode(Nodes[j]);
		count++;
      j++;
	}

	if (St.EmptyStack()) DeleteNode (Root);
	else
	  {
		// Clear up the stack;
		while (!St.EmptyStack())
		{
			DeleteNode (St.TopOfStack());
			St.Pop();
		}
	}

     // Ensure root is NULL 
     Root = NULL;

}

//------------------------------------------------------------------------------
// Traverse tree inorder outputting a text-based dendrogram to cout
//
void BaseTree::compresstraverse (BaseNodePtr p)
{
	if (p)
	{
		BaseNode *q = p->GetAnc();
		int 	start = Leaves - q->GetWeight();
		int 	stop = Leaves - p->GetWeight();
		char	symbol;

          // Draw line between p and its ancestor
		int i;
        for (i = start + 1; i <= stop; i++) 
        	Line[i] = (char)HBAR;

        // Find appropriate symbol for node
		if (p == q->GetChild())
		{
			if (q == Root) symbol = (char)ROOT;
			else symbol = (char)DOWN;
		}
		else
		{
          	// p is a sibling
			if (p->GetSibling()) symbol = (char)SIB;
			else symbol = (char)BOT;

		    // For siblings we need to fill in any vertical branches
            // below (i.e. to the left) of p. Such lines only exist
			// if an ancestor of p's immediate ancestor has siblings.
		    while ((q) && (q != Root))
		    {
				if (q->GetSibling())
				{
					Line[Leaves - q->GetAnc()->GetWeight()] = (char)VBAR;
				}
				q = q->GetAnc();
			}
		}
		Line[start] = symbol;

          // If p is a leaf then we output the line buffer
		if (p->IsLeaf())
		{
          	Line[Leaves] = '\0';
			*treeStream << Line << " " << p->GetLabel () << EOLN;
			int j = MAXLEAVES + MAXNAMELENGTH;
               for (i = 0; i < j; i++) Line[i] = ' '; 
		}

        // Traverse the rest of the tree
		compresstraverse (p->GetChild());
		compresstraverse (p->GetSibling());
	}
}


//d

//------------------------------------------------------------------------------
// Free the memory allocated to the tree
void BaseTree::DeleteTree ()
{
	deletetraverse (Root);
	if (Name) delete Name;
}

//------------------------------------------------------------------------------
// Draw the tree to a text device, default is uncompressed format

//------------------------------------------------------------------------------
void BaseTree::Draw (ostream &f, BOOL compress)
{
	treeStream = &f;

	if (Root)
	{
		// Allocate memory for line buffer
		Line = new char[MAXLEAVES + MAXNAMELENGTH + 1];
      for (int i = 0; i < MAXLEAVES + MAXNAMELENGTH; i++)
      	Line[i] = ' ';
		Line[MAXLEAVES + MAXNAMELENGTH] = '\0';

      	// Display tree name
      	if (IsRooted ())
      		f << "TREE ";
      	else f << "UTREE ";
      	f << Name << " =" << EOLN;

        // Draw tree
		if (compress)
      		compresstraverse (Root->GetChild());
		else
      	{
/*      	if (IsFlag (TF_HAS_EDGES))
         	{
         		getHeights (Root);
         		tp.scale = 60 / tp.maxheight;
         	}
         	else
         	{    */
         		tp.scale = 1.0;
         		getNodeHeights (Root);
 //       	}
	      	drawAsTextTraverse (Root);
      	}

      	// Free up memory
      	delete Line;
	}
	else f << "(No tree)" << EOLN;
}



//------------------------------------------------------------------------------
//e

//------------------------------------------------------------------------------
//f

//------------------------------------------------------------------------------
//i

//------------------------------------------------------------------------------
// Check that tree is completely read, return 0 is OK
int BaseTree::IsFinished ()
{
	if (!St.EmptyStack()) return (errStackNotEmpty);
     // Need to check for full set of leaves here
	return (errOK);
}


//------------------------------------------------------------------------------
// Make CurNode a leaf, called by tree reading code
void BaseTree::MakeCurNodeALeaf (int i)
{
	Leaves++;
	CurNode->SetLeaf();
	CurNode->SetWeight(1);
	CurNode->SetLabelNumber(i);
     CurNode->SetLeafNumber(i);
	Nodes[i - 1] = CurNode;
} 

//m

//------------------------------------------------------------------------------
// Make a child of CurNode and make it CurNode
int BaseTree::MakeChild ()
{
	BaseNodePtr	q;

	if (!St.Push (CurNode)) return (errStackOverFlow);
	else
	{
		q = NewNode();     	
		if (!q) return (errNoMemory);
		else
		{
			CurNode->SetChild(q);
			q->SetAnc(CurNode);
			CurNode->IncrementDegree();
			CurNode = q;
			Internals++;
			return (errOK);
		}
	}
}

//------------------------------------------------------------------------------
// Create a new node and make it the root of the tree, and set CureNode=Root.
// Returns 0 if successfull
// Called by tree reader
int BaseTree::MakeRoot ()
{
	St.Clear();
	CurNode = NewNode();
	if (!CurNode) return (errNoMemory);
	else
	{
		Root = CurNode;
		return (errOK);
	}
}

//------------------------------------------------------------------------------
// Make a sibling of CurNode whose ancestor is the top of the node stack,
// and make CurNode the new node.
// Returns 0 if successful, otherwise an error code. 
int	BaseTree::MakeSibling ()
{
	BaseNodePtr	q;
     BaseNodePtr	ancestor;

	if (St.EmptyStack()) return (errMissingLPar);
	else
	{
     	ancestor = St.TopOfStack();
		q = NewNode();
		if (!q) return (errNoMemory);
		else
		{
			CurNode->SetSibling(q);
			q->SetAnc(ancestor);
			ancestor->AddWeight(CurNode->GetWeight());
			ancestor->IncrementDegree();
			CurNode = q;
     	     return (errOK);
		}
     }
}

//n



//------------------------------------------------------------------------------
// Pop a node off the stack and make it the current node.
// Return 0 if successful
int BaseTree::PopNode ()
{
	BaseNodePtr q;

	if (St.EmptyStack()) return (errStackEmpty);
	else
	{
		q = St.TopOfStack();
		q->AddWeight (CurNode->GetWeight());
		CurNode = q;
		St.Pop();
		return (errOK);
	}
}

//s

//------------------------------------------------------------------------------
//Set the tree's name to aname
void BaseTree::SetName (const char *aname)
{
	if (aname != NULL)
     {
		Name = new char[strlen (aname) + 1];
		*Name = '\0';
		strcpy (Name, aname);
	}
}

//------------------------------------------------------------------------------
void BaseTree::traverse (BaseNodePtr p)
{

	if (p)
	{
		if (p->IsFlag (NF_LEAF))
		{
			// Glasgow
			if (!IsFlag (TF_TRANSLATE) && (p->GetLabel() != NULL))
			{
				char buf[MAXNAMELENGTH + 1]; // rdp
                strcpy (buf, p->GetLabel ());
				*treeStream << OutputString (buf);
				count += strlen (buf);
           	}
			else 
			{
				*treeStream << p->GetLabelNumber();
				count += 2;
			}

			if (IsFlag (TF_HAS_EDGES))
			{
				*treeStream << ':' << p->GetEdgeLength ();
				count++;
			}
		}
		else
		{
			*treeStream << "(";
			count++;
       	}
       	
       	if (count >= 50)
       	{
       		*treeStream << EOLN;
       		if (p != Root)
       			*treeStream <<"\t\t";
       		count = 0;
       	}
       	
       	
		traverse (p->GetChild());
		if (p->GetSibling())
		{
			*treeStream << ",";
			count++;
		}
		else
		{
			if (p != Root)
			{
				*treeStream << ")";
				// 29/3/96
				if (p->GetAnc()->GetLabel() != NULL && IsFlag (TF_INTERNAL_LABELS))
				{
					char buf[MAXNAMELENGTH + 1]; // rdp 11/1/96
					strcpy (buf, p->GetAnc()->GetLabel ());
					*treeStream << '\'' << buf << '\''; // 10/9/96 enclose in ''
					count += strlen (buf);
				}
				if (IsFlag (TF_HAS_EDGES) && (p->GetAnc () != Root))
				{
					*treeStream << ':' << p->GetAnc()->GetEdgeLength ();
					count += 6;
          	    }
			}
		}
		traverse (p->GetSibling());
	}

}

//------------------------------------------------------------------------------
void BaseTree::WriteNEXUS (ostream &f)
{
	count = 0;
	
	// 5 Nov 1994. Ensure edge lengths are correctly output
	f.setf (ios::fixed, ios::floatfield);
	treeStream = &f;

	
	if (IsFlag (TF_ROOTED)) f << "\tTREE ";
	else f << "\tUTREE ";

	if (IsFlag (TF_DEFAULT)) f << "* ";

	f << Name << "= ";

	traverse (Root);

	f << ";" << EOLN;
}



//u

//------------------------------------------------------------------------------
// Updates these fields of the tree's nodes:
//   Cluster
//   Weight
//	Degree
//	LabelNumber for internals (always n+1...m) where n = original leaves in tree
//
// Updates Nodes[n..m-1] so that internals are accessible by their LabelNumber - 1
//
// Clusters are built using LeafNumber field to ensures that for any
// tree with n leaves the clusters are subsets of {1,...,n}, whether or not
// leaves have been pruned.
//
// Sets TF_VALID flag so we know everything is updated
void BaseTree::Update ()
{
	if (!IsFlag (TF_VALID))
	{
     	count = Leaves;
		buildtraverse (Root);
		SetFlag (TF_VALID, True);
     }
}


//------------------------------------------------------------------------------
void BaseTree::calc (BaseNodePtr p)
{
	if (p)
	{
		calc (p->GetChild());
		p->Calc (tp);
		calc (p->GetSibling());
	}
}

//------------------------------------------------------------------------------
// Visit nodes in preorder for radial trees.
void BaseTree::radialCalc (BaseNodePtr p)
{
	if (p)
	{
		if (p != Root) 
			p->RadialCalc (tp);
		radialCalc (p->GetChild());
		radialCalc (p->GetSibling());
	}
}

//------------------------------------------------------------------------------
// Compute nodal heights based on path length from root, and store maximum
// value in tp.maxheight. Used by drawing routines.
void BaseTree::getHeights(BaseNodePtr p)
{
	if (p)
	{
		if (p != Root)
			p->SetHeight (p->GetAnc()->GetHeight() + p->GetEdgeLength());
		if (p->GetHeight() > tp.maxheight)
			tp.maxheight = p->GetHeight();
		getHeights (p->GetChild());
		getHeights (p->GetSibling());
	}
}

//------------------------------------------------------------------------------
// Compute node heights in range 0 to n-1, used when drawing angled cladograms
void BaseTree::getNodeHeights(BaseNodePtr p)
{
	if (p)
	{
		p->SetHeight (Leaves - p->GetWeight ());
		if (p->GetHeight() > tp.maxheight)
			tp.maxheight = p->GetHeight();
		getNodeHeights (p->GetChild());
		getNodeHeights (p->GetSibling());
	}
}

//------------------------------------------------------------------------------
void BaseTree::getLeafAngles (BaseNodePtr p)
{
	if (p)
	{
		if (p->IsLeaf ())
		{
			p->SetHeight (tp.count * tp.leafangle);
			tp.count++;
//			*treeStream << tp.count << " " << p->GetHeight() << EOLN;
		}
		getLeafAngles (p->GetChild());
		// Visit ancestor
		if (p->GetSibling() == NULL)
		{
			BaseNodePtr anc = p->GetAnc ();
			if (anc)
			{
				anc->SetHeight ((anc->GetChild()->GetHeight() + p->GetHeight()) / 2);
//				*treeStream <<  "   " << anc->GetHeight() << EOLN;
			}
		}
		getLeafAngles (p->GetSibling());
	}
}


//--------------------------------------------------------------------------
// Draw the tree
void BaseTree::drawTraverse (BaseNodePtr p)
{
	if (p)
	{
		drawTraverse (p->GetChild());
		p->Draw (tp);
		drawTraverse(p->GetSibling());
	}
}

//--------------------------------------------------------------------------
// Draw the tree  within the specified rectangle
// using the desired style (CLADOGRAM is the default)
void BaseTree::Draw (VRect r, VFontPtr font, unsigned int style, VFontPtr edgeFont)

{
	tp.r.SetRect (r);
	
	// 10/1/97 Inset to allow for the font so that top and bottom
	// labels are not clipped
	Port.SelectAFont (font);
	tp.r.Inset (0, Port.FontHeight () / 2);

	// Allow space for the tree's name
	if (style & TS_SHOWNAME)
	{
		tp.r.SetTop (tp.r.GetTop() + Port.FontHeight() * 2);
	}
	
	// 10/2/97
	// Allow space for scale bar
	int scalebar_space = Port.FontHeight() * 2;
	if (IsFlag(TF_HAS_EDGES) && (style & TS_PHYLOGRAM))
	{
		tp.r.SetBottom (tp.r.GetBottom() - scalebar_space);
	}
	

	tp.count   = 0;
	tp.leaves  = Leaves;
	tp.lasty   = 0;
	// 10/2/97
	float w = r.GetRight() - r.GetLeft();
	float l = Leaves - 1;
	tp.nodegap = w/l;

//	tp.nodegap = (tp.r.GetRight() - tp.r.GetLeft()) / (Leaves - 1);
#ifdef SCALE
	//10/297
	float h = tp.r.GetBottom() - tp.r.GetTop();
//	if (IsFlag(TF_HAS_EDGES) && (style & TS_PHYLOGRAM))
//		tp.leafgap = h / (l + 1);
//	else
		tp.leafgap = h/l;

//	if (IsFlag(TF_HAS_EDGES) && (style & TS_PHYLOGRAM))
//		tp.leafgap	= (tp.r.GetBottom() - tp.r.GetTop()) / (Leaves);
//	else
//		tp.leafgap	= (tp.r.GetBottom() - tp.r.GetTop()) / (Leaves - 1);
#else
	tp.leafgap	= (tp.r.GetBottom() - tp.r.GetTop()) / (Leaves - 1);
#endif


/*	// 12 July 1996
	if (tp.nodegap < 1 || tp.leafgap < 1)
	{
		Port.FrameRect (r);
		char buf[32];
		strcpy (buf, "Tree is too large to display");
		Port.DrawText (r, buf, strlen (buf), 'c', FALSE);
		return;
  	}
*/

	// 16/4/96
	if (edgeFont)
		tp.edgefont = edgeFont;
	else
		tp.edgefont = font;
		
	// 21/1/97
	// Draw tree name (if desired)
	if (style & TS_SHOWNAME)
	{
		Port.SelectAFont (tp.edgefont);
		VRect lr;
		lr.Load (tp.r.GetLeft(), tp.r.GetTop() - 2 * Port.FontHeight(),
		tp.r.GetRight(), tp.r.GetTop() - Port.FontHeight ());
		Port.DrawString (lr, Name, 'l', FALSE);
	}

	tp.maxheight   = 0.0;
	tp.style		= style;
	tp.labelfont	= font;
	if ((style & TS_PHYLOGRAM) == TS_PHYLOGRAM)
	{
		// Ensure this is a sensible thing to do...
         if (IsFlag (TF_HAS_EDGES))
		{
			Root->SetHeight (0.0);
			getHeights (Root);
#ifdef SCALE
			// Draw scale bar using "log" scale
			float m = log10 (tp.maxheight);
			int i = (int) m - 1;
			float bar = pow (10.0, i);
			int scalebar = ((bar / tp.maxheight) * (tp.r.GetRight() - tp.r.GetLeft()));

			PoinT pt1;
			PoinT pt2;
			if (tp.style & TS_LEFT)
			{
				pt1.x = tp.r.GetLeft();
				pt1.y = tp.r.GetBottom() + scalebar_space;
				pt2.x = pt1.x + scalebar;
				pt2.y = pt1.y;
			}
			else
			{
				pt2.x = tp.r.GetRight ();
				pt2.y = tp.r.GetBottom () + scalebar_space;
				pt1.x = pt2.x - scalebar;
				pt1.y = pt2.y;
			}
			
			// 10/1/97
			// Store current pen and draw a scale bar
#ifdef Macintosh
			int w = Port.GetCurrentPenWidth ();
#else
			float w = Port.GetCurrentPenWidth ();
			Technology t = Port.GetDisplayType ();
			if (t != vDISPLAY)
				w = w / Port.PixelsPerPt (t);
#endif
			Port.WidePen (1);
			Port.DrawLine (pt1, pt2);
			Port.WidePen (w);
			
//			Port.DrawLine (pt1, pt2);

			// Label scale
			char buf[20];
			if (i >= 0)
			{
				sprintf (buf, "%d", int (bar));
			}
			else
			{
				int j = abs (i);
				sprintf (buf, "%.*f", j, bar);
			}
			// Ensure enough space for label
			Port.SelectAFont (tp.edgefont);
			int l = Port.TextWidth (buf, strlen (buf)) + Port.MaxCharWidth();;
			if (l > scalebar)
			{
				if (tp.style & TS_LEFT)
					pt2.x = pt1.x + l;
				else
					pt1.x = pt2.x - l;
			}
			VRect r;
			r.SetLeft(pt1.x);
			r.SetRight (pt2.x);
			r.SetBottom(pt1.y);
			r.SetTop (r.GetBottom() - Port.FontHeight ());
			r.Offset (0, -2);
			Port.DrawText (r, buf, strlen (buf), 'c', FALSE);
#endif
		}
		else
		{
			// as we don't have edge lengths change style to cladogram
			// and if user has requested TS_ANGLE compute node heights
               // based on node weights
			tp.style &= ~TS_PHYLOGRAM;
			tp.style |= TS_CLADOGRAM;
			if (tp.style & TS_ANGLE)
            {
				Root->SetHeight (0.0);
				getNodeHeights (Root);
            }
		}

	}
	else
	{
		// If user wants just angled cladogram, ensure we draw one!
		if ( (style & TS_ANGLE )  == TS_ANGLE) 
		{
			Root->SetHeight (0.0);
			getNodeHeights (Root);
		}
	}
	
	// 11/1/97
	// Bug in drawing angled trees!
	// The x coordinate of internal nodes is a multiple of tp.nodegap, which
	// is obtained by dividing the width of tp.r by (Leaves - 1). If there is
	// a remainder rem, then the x-coordinate of the leaves will be nodegap + r
	// from the last node in the tree. Hence we inset the left marging of
	// tp.r by rem. This fixes the crossing of edges sometimes seen on big
	// cladograms.
/*	if (style & TS_SLANT)
	{
		int d = tp.r.GetWidth () - tp.nodegap * (Leaves - 1);
		tp.r.SetLeft (tp.r.GetLeft () + d);
	}
*/

     // Compute coordinates then draw
    calc (Root);
	drawTraverse (Root);
}

//------------------------------------------------------------------------------
// Return least upper bound of b in tree
BaseNodePtr BaseTree::LUB (Bitset &b)
{
	BaseNodePtr p = Root;
	BaseNodePtr q = p;
     BooleaN found = False;

	while (q && !found)
	{
		switch (q->ClusterRelationship (b))
		{
			case rdmpIDENTITY:
               	// we've found it exactly
				p = q;
				found = True;
				break;
			case rdmpSUBSET:
				// q's ancestor was lub
				p = q->GetAnc ();
                    found = True; 
				break;
			case rdmpDISJOINT:
               	// look at sibling
				p = q;
				q = q->GetSibling ();
				break;
			case rdmpSUPERSET:
               	// q is too big, go left up tree
				p = q;
				q = q->GetChild();
				break;
			case rdmpOVERLAPPING:
				// q's ancestor was lub
				p = q->GetAnc ();
                    found = True; 
				break;
		}
	}
     return (p);
}




void BaseTree::clearNode (BaseNodePtr p, int f)
{
	if (p)
	{
		clearNode (p->GetChild(), f);
		p->SetFlag (f, False);
		clearNode (p->GetSibling(), f);
	}
}

void BaseTree::ClearFlags (int f)
{
	clearNode (Root, f);
}


//------------------------------------------------------------------------------
void BaseTree::ultratrav (BaseNodePtr p)
{
	if (p && aflag)
	{
		if (p->IsLeaf ())
		{
//      	*treeStream << p->GetLabel() << ' ' << setprecision(6) << p->GetHeight() << ' ' << tp.maxheight << endl;

			if ( fabs (p->GetHeight () - tp.maxheight) > epsilon )
				aflag = False;
          }
	if (aflag) ultratrav (p->GetChild ());
     if (aflag) ultratrav (p->GetSibling ());
	}
}


//------------------------------------------------------------------------------
// True if tree is ultrametric
BooleaN BaseTree::IsUltrametric ()
{

//	treeStream = new ofstream ("ultra.out");

//	*treeStream << Name << endl;

	aflag = True;
	Root->SetHeight (0.0);
   tp.maxheight = 0;
	getHeights (Root);
	ultratrav (Root);

//	delete treeStream;

	return (aflag);
}


//------------------------------------------------------------------------------
float BaseTree::GetMaxHeight ()
{
	tp.maxheight = 0.0;
	Root->SetHeight(0.0);
	getHeights (Root);
	return tp.maxheight;
}


//------------------------------------------------------------------------------
void BaseTree::MakeRootBinary ()
{
	if (!Root->IsBinary())
	{
		BaseNodePtr SplitNode = NewNode();
		if (SplitNode)
		{
			Internals++;
			SplitNode->SetChild (Root->GetChild()->GetSibling());
			Root->SetChildSibling (SplitNode);
			SplitNode->SetAnc (Root);
			Root->SetDegree (2);
			BaseNodePtr q = SplitNode->GetChild();
			while (q)
			{
				q->SetAnc (SplitNode);
				SplitNode->IncrementDegree();
				q = q->GetSibling();
			}
		}
	}
}

//------------------------------------------------------------------------------
// Mark nodes on path between p and Root inclusive
void BaseTree::MarkPath (BaseNodePtr p)
{
	BaseNodePtr q = p;
	while (q != NULL)
	{
		q->SetFlag (NF_MARKED, True);
		q = q->GetAnc ();
	}
}


//------------------------------------------------------------------------------
BaseNodePtr BaseTree::FirstDescendant (BaseNodePtr p)
{
	CurNode = p->GetChild ();
	return CurNode;
}

//------------------------------------------------------------------------------
BaseNodePtr BaseTree::NextDescendant ()
{
	CurNode = CurNode->GetSibling ();
	return CurNode;
}

//------------------------------------------------------------------------------
// Return next desc of p that is not on the path OG..Root.
BaseNodePtr BaseTree::nextNonOGDesc ()
{
	BaseNodePtr q = NextDescendant ();

	BooleaN done = False;
	while (!done)
	{
		if (q == NULL)
			done = True;
		if (!done)
			done = (BooleaN)(!q->IsFlag (NF_MARKED));
		if (!done)
			q = NextDescendant ();
	}
	return q;
}

//------------------------------------------------------------------------------
void BaseTree::listOtherDesc (BaseNodePtr p)
{
	BaseNodePtr q = FirstDescendant (p);
	if (q->IsFlag (NF_MARKED))
		q = nextNonOGDesc ();

	// Add desc
	if (p != Root)
	{
		AddThere->SetChild (q);
		q->SetAnc (AddThere);
	}
	else
	{
		// descendant of root must be siblings of AddThere
		AddThere->SetSibling (q);
		q->SetAnc (AddThere->GetAnc ());
	}
	q->GetAnc()->IncrementDegree();
	AddThere = q;

	// Go through remaining desc
	q = nextNonOGDesc ();
	while (q != NULL)
	{
		AddThere->SetSibling (q);
		q->SetAnc (AddThere->GetAnc ());
		q->GetAnc()->IncrementDegree ();
		AddThere = q;
		q = nextNonOGDesc ();
	}
	AddThere->SetSibling (NULL);
}

//------------------------------------------------------------------------------
void BaseTree::ReRoot (BaseNodePtr OG_PTR)
{
	if ((OG_PTR == NULL) || (OG_PTR == Root)) return; // Ensure

	SetFlag (TF_ROOTED, True);

	if (OG_PTR->GetAnc () == Root) return;  // No need to reroot


	MarkPath (OG_PTR);

	BaseNodePtr IG_PTR = NewNode ();
	IG_PTR->SetAnc (OG_PTR->GetAnc());

	AddThere = IG_PTR;
	BaseNodePtr q = OG_PTR->GetAnc ();
	// adjust edge length
	if (q)
	{	
		// Bug fix 21/12/95
		AddThere->SetEdgeLength (0); // for now
		// Assign bootstrap value!
		AddThere->SetLabel (NULL);

	}
	while (q != NULL)
	{
		listOtherDesc (q);

		BaseNodePtr t = q;
		q = q->GetAnc ();
		if ((q != NULL) && (q != Root))
		{
			BaseNodePtr p = NewNode ();
			AddThere->SetSibling (p);
			p->SetAnc (AddThere->GetAnc ());

			// Bug fix 21/12/95
//			p->SetEdgeLength (q->GetEdgeLength ());
			p->SetEdgeLength (t->GetEdgeLength ());
			p->SetLabel (t->GetLabel ());

			p->GetAnc ()->IncrementDegree ();
			AddThere = p;
		}
	}
	OG_PTR->GetAnc()->SetChild (OG_PTR);
	OG_PTR->SetSibling (IG_PTR);
	Root = OG_PTR->GetAnc ();

	q = Root->GetAnc();
	while (q != NULL)
	{
		BaseNodePtr p = q;
		q = q->GetAnc ();
		delete p;
	}

	Root->SetAnc (NULL);
	Root->SetSibling (NULL);


	SetFlag (TF_VALID, False);
	Update ();

	Root->SetFlag (NF_MARKED, False);
	OG_PTR->SetFlag (NF_MARKED, False);
}

//------------------------------------------------------------------------------
/*	  Returns pointer to outgroup node in OG_PTR. If outgroup is
	  paraphyletic on tree then return ptr to outgroup's complement.

	  An outgroup is a partition O:O'of the set [1,..,leaves].
	  In order to be valid either O or O' must be a cluster
	  in the tree.
*/
//
// @mfunc BaseNodePtr | Tree | CanRootWith | Returns outgroup node corresponding to outgroup.
//
// @parm Bitset | outgroup | Outgroup.
//
BaseNodePtr BaseTree::CanRootWith (Bitset &outgroup)
{
	BaseNodePtr OG_PTR = NULL;
	if (outgroup.IsNull ()) return NULL; // ensure
	if (outgroup.Cardinality () == 1)
	{
		OG_PTR = Nodes[outgroup[1] - 1]; // root with leaf
	}
	else
	{
		BaseNodePtr q = LUB (outgroup);
		if (q == Root)
		{
			// get LUB of complement of outgroup
			Bitset f;
			f.FullSet (Leaves);
			// set difference!
			Bitset c = f - outgroup;
			q = LUB (c);
			if (q->ClusterRelationship (c) == rdmpIDENTITY)
			{
				OG_PTR = q;
			}
		}
		else
		{
			if (q->ClusterRelationship (outgroup) == rdmpIDENTITY)
			{
				OG_PTR = q;
			}
		}
	}
	return OG_PTR;
}

//------------------------------------------------------------------------------
void BaseTree::getRadialNodeHeights(BaseNodePtr p)
{
	if (p)
	{
		if (p != Root)
		{
			BaseNodePtr a = p->GetAnc ();
			if ( (a == Root) && (p == Root->GetChild ()))
			{
				p->SetHeight (0.0);
				p->SetEdgeLength (0.0);
			}
			else
			{
				p->SetHeight (Leaves - p->GetWeight ());
				p->SetEdgeLength (1.0); // for radial trees
			}
			
		}
		if (p->GetHeight() > tp.maxheight)
			tp.maxheight = p->GetHeight();
		getRadialNodeHeights (p->GetChild());
		getRadialNodeHeights (p->GetSibling());
	}
}



//------------------------------------------------------------------------------
// Draw the tree
void BaseTree::radialDrawTraverse (BaseNodePtr p)
{
	if (p)
	{
		radialDrawTraverse (p->GetChild());
		p->RadialDraw (tp);
		radialDrawTraverse(p->GetSibling());
	}
}

//------------------------------------------------------------------------------
void BaseTree::centroidTraverse (BaseNodePtr p)
{
	if (p != NULL)
	{
		centroidTraverse (p->GetChild ());
		if (CurNode) return;
		if (!p->IsLeaf ())
		{
			if (p->GetWeight () >= avalue)
			{
				CurNode = p;
				aflag = (BooleaN) ((p->GetWeight () == avalue) && (Leaves % 2 == 0));
				return;
			}
		}
		centroidTraverse (p->GetSibling ());
		if (CurNode) return;
	}
}
			
//------------------------------------------------------------------------------
//
// @mfunc void | Tree | Centroid | Find the centroid of an unrooted tree.
//
// @parm BaseNodePtr | centre | The centroid.
// @parm BooleaN | edge | True if centroid is an edge.
//
void BaseTree::GetCentroid (BaseNodePtr &centre, BooleaN &edge)
{
	CurNode = NULL;
	avalue = Leaves / 2;
	centroidTraverse (Root);
	centre = CurNode;
	edge = aflag;
}

#ifdef DAYLIGHT

// Much of this code is a direct steal from Dave Swofford's unroot.c code which
// implements his version of Joe Felsenstein's "maximum daylight algorithm for drawing
// unrooted trees.

//#define DEBUG_DAYLIGHT

PoinT focus;
ofstream f;
BaseNodePtr stopNode, focusNode;
int offset;
float amin, amax;
float maxRotation;
float rotation;
float xchange, ychange;
float xoffset, yoffset;

float ttop, tleft, tright, tbottom, scale_real;
float labelleft, labelright;

void SetFocalNode (BaseNodePtr p, BaseNodePtr root);
void traverseFocus (BaseNodePtr p);
float GetDaylight(BaseNodePtr p, BaseNodePtr root);
void RotateWedges(BaseNodePtr p, float skySeg);
void PolarToRect(BaseNodePtr root);
void GetCanopyAbove (BaseNodePtr p);
float GetCanopyBelow(BaseNodePtr p, BaseNodePtr root);
void CalcRelRotation(BaseNodePtr q, float m);
void traverseCanopyAbove (BaseNodePtr p);
void traverseCanopyBelow (BaseNodePtr p);
void traverseRotate (BaseNodePtr q);
void traversePolar (BaseNodePtr p);
void PolarToRect(BaseNodePtr root);
void traverseDeviceTree (BaseNodePtr p);
void traverseCentreTree (BaseNodePtr p);
void traverseTreeExtent (BaseNodePtr p);


//------------------------------------------------------------------------------
void traverseFocus (BaseNodePtr p)
{
	if (p)
    {
        // translate rectangular coordinate so that p is at (0,0) ...
        p->x -= xchange;
        p->y -= ychange;
        // ... then calculate corresponding polar coordinate
        p->SetRadius (sqrt (p->x*p->x + p->y*p->y));
        float a = 0.0;
        if (p->GetRadius () > epsilon)
        {
            a = acos (p->x/p->GetRadius());
            if (p->y < 0.0)
                a = 2 * M_PI - a;
        }
        p->SetAngle (a);
        traverseFocus (p->GetChild());
        traverseFocus (p->GetSibling());
    }
}


//------------------------------------------------------------------------------
//	Set up a new "focal node" -- we will distribute the "daylight" evenly around the wedges it
//	determines.  On input, rectangular coordinates are available.  We translate so that the new
//	coordinate system locates the focal node at (0,0).  A polar coordinate system with the focal
//	node located at the origin is then defined.
void SetFocalNode (BaseNodePtr p, BaseNodePtr root)
{
	p->GetXY (focus);
    xchange = p->x;
    ychange = p->y;
    traverseFocus (root);
}

//------------------------------------------------------------------------------
// Calculate rotation angle of vector leading to node q relative to vector leading from focal node
//	to root of current subtree.
void CalcRelRotation(BaseNodePtr q, float m)
{
	float		a_rel, qa;
	/* calculate the relative rotation, updating min and max accordingly */
	qa = q->GetAngle();
	a_rel = qa - m;
	/* allow for crossing 2PI<=>0 boundary */
	if (m > M_PI)
		{
		if ((qa < m) && (qa < m - M_PI))
			a_rel += 2*M_PI;
		}
	else
		{
		if ((qa > m) && (qa > m + M_PI))
			a_rel -= 2*M_PI;
		}
	if (a_rel < amin)
		{
		amin = a_rel;
		}
	else if (a_rel > amax)
		{
		amax = a_rel;
		}
}

//------------------------------------------------------------------------------
void traverseCanopyAbove (BaseNodePtr p)
{
	if (p)
    {
		CalcRelRotation (p, stopNode->GetAngle());
		traverseCanopyAbove (p->GetChild());
        if (p != stopNode)
	        traverseCanopyAbove (p->GetSibling());
    }
}


//------------------------------------------------------------------------------
void GetCanopyAbove (BaseNodePtr p)
{
	stopNode = p;
    traverseCanopyAbove (p);
    p->SetALeft (p->GetAngle() + amin);
 	p->SetARight (p->GetAngle() + amax);
#ifdef DEBUG_DAYLIGHT
	Bitset b;
    p->GetCluster(b);
	f << "\tCanopy above ";
    b.Write (f);
    f << "= " << p->GetALeft() * 180 / M_PI
    	 << "° <-> " << p->GetARight() * 180 / M_PI << "°\n";
#endif
}

//------------------------------------------------------------------------------
void traverseCanopyBelow (BaseNodePtr p)
{
	if (p)
    {
    	if (p != stopNode)
			CalcRelRotation (p, stopNode->GetAnc()->GetAngle());
		if (p != stopNode)
        	traverseCanopyBelow (p->GetChild());
        traverseCanopyBelow (p->GetSibling());
    }
}


//------------------------------------------------------------------------------
float GetCanopyBelow(BaseNodePtr p, BaseNodePtr root)
{
	stopNode = p;
	amin = amax = 0.0;
    BaseNodePtr s = p->GetAnc();
    s->SetALeft(0.0);
	traverseCanopyBelow (root);
	s->SetALeft(s->GetAngle() + amin);
	s->SetARight (s->GetAngle() + amax);
#ifdef DEBUG_DAYLIGHT
	f << "\tCanopy below = " << p->GetALeft() * 180 / M_PI
    	 << "° <-> " << p->GetARight() * 180 / M_PI << "°\n";
#endif
	return amax - amin;
}

//------------------------------------------------------------------------------
// Find the total amount of "daylight" between all subtrees connected to internal node 'p'.
float GetDaylight(BaseNodePtr p, BaseNodePtr root)
{
	int n = 0;
	float sky = 2 * M_PI;

    BaseNodePtr q = p->GetChild();
    while (q)
    {
		amin = amax = 0.0;
		GetCanopyAbove(q);
		sky -= (amax - amin);
        q = q->GetSibling ();
        n++;
    }
#ifdef DEBUG_DAYLIGHT
	f << "\tsky above = " << sky ;
#endif
   if (p != root)
    {
    	float s = GetCanopyBelow(p, root);
#ifdef DEBUG_DAYLIGHT
		f << "\tsky below = " << s;
#endif
    	sky -= s;
		n++;
	}
#ifdef DEBUG_DAYLIGHT
	f << '\n';
#endif
	return sky / n;
}

//------------------------------------------------------------------------------
void traverseRotate (BaseNodePtr q)
{
	if (q)
    {
    	float a = q->GetAngle();
        q->SetAngle (a + rotation);
        if (q->IsLeaf())
        {
        	q->SetALeft (q->GetAngle());
            q->SetARight(q->GetAngle());
        }
        else
        {
        	q->SetALeft (q->GetALeft() + rotation);
            q->SetARight (q->GetARight() + rotation);
        }
#ifdef DEBUG_DAYLIGHT
 //	f << '\t' << "Wedgeq->GetALeft() << " <-> " << q->GetARight() << '\n';
#endif
        traverseRotate (q->GetChild());
        if (q != stopNode)
        	traverseRotate (q->GetSibling());
    }
}


//------------------------------------------------------------------------------
//	Rotate the (usually three) wedges defined by an internal node so that the amount of daylight
//	is equal (to 'skySeg') between each pair of wedges.
void RotateWedges(BaseNodePtr p, float skySeg)
{
	BaseNodePtr	q;
	float	prev_aright, diff1, diff2, diff;

#ifdef DEBUG_DAYLIGHT
	f << "\nRotating wedges:\n";
#endif

	q = p->GetChild();
    if (p->GetAnc() != NULL)
		prev_aright = p->GetAnc()->GetARight();
	else
    {
		prev_aright = q->GetARight();
		q = q->GetSibling();
    }
    while (q)
    {
		// figure out how much to rotate subtree so that daylight between wedges will be equal
        float a = q->GetAngle();
        float aleft = q->GetALeft();
        float aright = q->GetARight();
		if (prev_aright > 2*M_PI)
			prev_aright -= 2*M_PI;
		rotation = prev_aright + skySeg - aleft;
#		if defined(CHECK_WEDGE_ROTATION)
			Pprintf("   rotation for node %d => %f\n", q->index, rotation);
#		endif
		/* keep track of maximum rotation for convergence */
		diff1 = fabs(2 * M_PI - rotation);			/* NOTE: THINK C 7.0.4 (and later?) has a */
		diff2 = fabs(rotation);						/* code generation bug that prevents use  */
		diff = (diff1 < diff2) ? diff1 : diff2;		/* of just two "diff" variables here      */
		if (diff > maxRotation)
			maxRotation = diff;
		// rotate entire subtree by this amount
        stopNode = q;
        traverseRotate (q);
		prev_aright = q->GetARight();
        q = q->GetSibling ();
	}
#	if defined(CHECK_WEDGE_ROTATION)
		Pputs("  rotated wedges:\n");
		if (!ROOT(p))
			{
			qu = p->anc->plotData->u.unrooted;
			Pprintf("%6d: (%f,%f,%f)\n", Dex(p->anc), qu->aleft, qu->a, qu->aright);
			}
		for (q = p->left; q != NULL; q = q->right)
			{
			qu = q->plotData->u.unrooted;
			Pprintf("%6d: (%f,%f,%f)\n", Dex(q), qu->aleft, qu->a, qu->aright);
			}
#	endif
}
//------------------------------------------------------------------------------
void traversePolar (BaseNodePtr p)
{
	if (p)
    {
    	float r = p->GetRadius();
        float a = p->GetAngle();
        PoinT pt;
        p->x = r * cos(a) + xoffset;
        p->y = r * sin(a) + yoffset;
        pt.x = p->x;
        pt.y = p->y;
        p->SetXY (pt);
        traversePolar (p->GetChild());
        if (p != stopNode)
        	traversePolar (p->GetSibling ());
    }
}

//------------------------------------------------------------------------------
//	Get rectangular coordinates for all nodes corresponding to the current polar coordinates.
void PolarToRect(BaseNodePtr root)
{
    stopNode = root;
    traversePolar (root);
}


//------------------------------------------------------------------------------
void BaseTree::traverseDaylight (BaseNodePtr p)
{
	if (p)
    {
    	if (!p->IsLeaf())
        {
            SetFocalNode (p, Root);
#ifdef DEBUG_DAYLIGHT
			f << "Focal node = ";
			p->cluster.Write (f);
            f << '\n';
#endif
            float daylightSeg = 0.0;
            daylightSeg = GetDaylight(p, Root);
            if (daylightSeg > 0.0)
            {
                RotateWedges(p, daylightSeg);
                PolarToRect(Root);
            }
        }
        traverseDaylight (p->GetChild ());
        traverseDaylight (p->GetSibling());
    }
}


//------------------------------------------------------------------------------
void traverseTreeExtent (BaseNodePtr p)
{
	if (p)
    {
    	tleft = MIN (tleft, p->x);
        tright = MAX (tright, p->x);
        ttop = MIN (ttop, p->y);
        tbottom = MAX (tbottom, p->y);
        if (p->IsLeaf())
        {
        	int w =  Port.TextWidth (p->GetLabel(), strlen (p->GetLabel()));
        	if (p->x < p->GetAnc()->x)
        		labelleft = MIN (labelleft, p->x - w);
			else
        		labelright = MAX (labelright, p->x + w);
        }
        traverseTreeExtent (p->GetChild());
       	traverseTreeExtent (p->GetSibling ());
    }
}

//------------------------------------------------------------------------------
void traverseCentreTree (BaseNodePtr p)
{
	if (p)
    {
    	p->x -= xoffset;
        p->y -= yoffset;
		// to do..scale
        p->x *= scale_real;
        p->y *= scale_real;
        traverseCentreTree (p->GetChild());
       	traverseCentreTree (p->GetSibling ());
    }
}

//------------------------------------------------------------------------------
void traverseDeviceTree (BaseNodePtr p)
{
	if (p)
    {
    	PoinT pt;
        pt.x = p->x + xoffset;
        pt.y = p->y + yoffset;
        p->SetXY (pt);
		traverseDeviceTree (p->GetChild());
       	traverseDeviceTree (p->GetSibling ());
    }
}



// Test of Joe and Dave's daylight approach
void BaseTree::RadialDraw (VRect r, VFontPtr font, unsigned int style, VFontPtr EdgeFont)
{
/*	// To make a nicer picture, root the tree at it's centre of gravity.
	BaseNodePtr p;
    int i;
	BooleaN isEdge;
	GetCentroid (p, isEdge);

    // Only reroot and make binary if necessary
    if ((p != Root) && (p->GetAnc() != Root))
    {
		MakeRootBinary ();
		ReRoot (p);
    }
*/

	// To make a nicer picture, root the tree at it's centre of gravity.
	BaseNodePtr p;
	int i;
	BooleaN isEdge;
	GetCentroid (p, isEdge);

	// Only reroot and make binary if necessary
    if ((p != Root) && (p->GetAnc() != Root))
	{
		MakeRootBinary ();
		ReRoot (p);
	}

	// Test
	if (Root->GetDegreeOf () == 2)
	{
		BaseNodePtr toDelete = Root->GetChild();
		BaseNodePtr toKeep = toDelete->GetSibling();
		BaseNodePtr tmp;
		if (toDelete->IsLeaf())
		{
			tmp = toKeep;
			toKeep = toDelete;
			toDelete = tmp;
		}
		// Preserve edge length
		toKeep->SetEdgeLength (toKeep->GetEdgeLength() + toDelete->GetEdgeLength());

		tmp = toDelete->GetChild();
		while (tmp)
		{
			tmp->SetAnc (Root);
			tmp = tmp->GetSibling();
		}
		if (toDelete->IsTheChild())
		{
			toDelete->GetChild()->GetRightMostSibling ()->SetSibling(toKeep);
			Root->SetChild (toDelete->GetChild());
		}
		else
		{
			toKeep->SetSibling (toDelete->GetChild());
			Root->SetChild (toKeep);
		}

		delete tmp;
	}


	tp.r.SetRect (r);
	tp.count	= 0;
	tp.leaves   = Leaves;
	tp.lasty 	= 0;
	tp.nodegap  = (r.GetRight() - r.GetLeft()) / (Leaves - 1);
	tp.leafgap	= (r.GetBottom() - r.GetTop()) / (Leaves - 1);
	tp.maxheight= 0.0;
	tp.style	= style;
	tp.labelfont= font;

	// 16/4/96
	if (EdgeFont)
		tp.edgefont = EdgeFont;
	else
		tp.edgefont = font;

	// Square the rectange for drawing
	int w = tp.r.GetWidth ();
	int h = tp.r.GetHeight ();
	int off;
	if (w > h)
	{
		off = (w - h) / 2;
		tp.r.SetRight (tp.r.GetLeft () + h);
		tp.r.Offset (off, 0);
	}
	else
	{
		off = (h - w) / 2;
		tp.r.SetBottom (tp.r.GetTop () + w);
		tp.r.Offset (0, off);
	}

	// Position the root at the centre of the circle
	PoinT pt;
	pt.x = tp.r.GetLeft () + tp.r.GetWidth () /2;
	pt.y = tp.r.GetTop () + tp.r.GetHeight () /2;
	Root->SetXY (pt);
    Root->x = pt.x;
    Root->y = pt.y;


	// Get angles of leaves
	tp.leafangle = 2 * pi / Leaves;
	tp.count = 0;

	// Ensure we have sensible edge lengths, and get maximum height of tree
	Root->SetHeight (0.0);
    if (IsFlag (TF_HAS_EDGES))
	{
		// Use actual branch lengths
		getHeights (Root);
	}
	else
	{
		// Use edge lengths of unit length
		getRadialNodeHeights (Root);
    }

	// Compute scale factor
	tp.scale = (tp.r.GetWidth () / 2) / tp.maxheight;


	// Now compute angles for nodes (overwrites height field)
	tp.count = 0;
	getLeafAngles (Root);

	 // Compute coordinates then draw
	radialCalc (Root);


#ifdef DEBUG_DAYLIGHT
	f.open ("daylight.txt");
#endif

	maxRotation = 0.0;

    // Dummy variables used to relocate tree in final call to PolarToRect
    xoffset = 0.0;
    yoffset = 0.0;

    // Number of smoothing passes we make
    int maxPasses = 5; //1;
    for (int pass = 0; pass < maxPasses; pass++)
    {
    	traverseDaylight (Root);
        if (maxRotation < 0.1)
        	break;
    }
    // We now need to ensure tree drawing fills the available space as much as possible.
    VRect treeRect;
    treeRect.SetRect (r);
	Port.SelectAFont (tp.labelfont);
    // Get extent of the rectangle the tree occupies
    tleft = 1000.0;
    tright = 0.0;
    ttop = 1000.0;
    tbottom = 0.0;
    labelleft = 1000.0;
    labelright = 0.0;
	traverseTreeExtent (Root);
    // Allow for space occupied by labels
    int fontHeight = Port.FontHeight();
    h = treeRect.GetHeight () - 2 * fontHeight;
    treeRect.SetLeft (treeRect.GetLeft() + (tleft - labelleft));
    treeRect.SetRight (treeRect.GetRight() - (labelright - tright));
    w = treeRect.GetWidth();

    float w_real, h_real;
    w_real = tright - tleft;
    h_real = tbottom - ttop;
   	scale_real = MIN ((float) w / w_real, (float) h / h_real);
    // Centre tree in middle of it's rectangle, and rescale
    xoffset = tleft + w_real / 2;
    yoffset = ttop + h_real / 2;
    traverseCentreTree (Root);
    // Convert to device coordinates
    xoffset = r.GetLeft() + w/2  + (tleft - labelleft);
    yoffset = r.GetTop() + h/2 + fontHeight;
    traverseDeviceTree (Root);

/*
	// Prepare to draw the tree in device coordinates
	SetFocalNode (Root, Root);
    xoffset = 200.0;
    yoffset = 200.0;
    PolarToRect (Root);    */

#ifdef DEBUG_DAYLIGHT
	f.close();
#endif


	// Draw completed tree
	radialDrawTraverse (Root);
}
#else


//------------------------------------------------------------------------------
//
// @mfunc void | Tree | RadialDraw | Draw a radial tree.
//
// @parm VRect | r | Rectangle tree will be drawn in.
// @parm VFontPtr | font | Font ot be used for labels.
// @parm unsigned int | style | Flags determining style of tree.
//
void BaseTree::RadialDraw (VRect r, VFontPtr font, unsigned int style, VFontPtr edgeFont)
{
	// To make a nicer picture, root the tree at it's centre of gravity.
	BaseNodePtr p;
	BooleaN isEdge;
	GetCentroid (p, isEdge);

	MakeRootBinary ();
	ReRoot (p);

	tp.r.SetRect (r);
	tp.count	= 0;
	tp.leaves   = Leaves;
	tp.lasty 	= 0;
	tp.nodegap  = (r.GetRight() - r.GetLeft()) / (Leaves - 1);
	tp.leafgap	= (r.GetBottom() - r.GetTop()) / (Leaves - 1);
	tp.edgefont = font;
	tp.maxheight   = 0.0;
	tp.style		= style;
	tp.labelfont	= font;
	// 16/4/96
	if (edgeFont)
		tp.edgefont = edgeFont;
	else
		tp.edgefont = font;

	
	// Square the rectange for drawing
	int w = tp.r.GetWidth ();
	int h = tp.r.GetHeight ();
	int off;
	if (w > h)
	{
		off = (w - h) / 2;
		tp.r.SetRight (tp.r.GetLeft () + h);
		tp.r.Offset (off, 0);
	}
	else
	{
		off = (h - w) / 2;
		tp.r.SetBottom (tp.r.GetTop () + w);
		tp.r.Offset (0, off);
	}

	// Position the root at the centre of the circle
	PoinT pt;
	pt.x = tp.r.GetLeft () + tp.r.GetWidth () /2;
	pt.y = tp.r.GetTop () + tp.r.GetHeight () /2;
	Root->SetXY (pt);
	
	// Get angles of leaves
	tp.leafangle = 2 * pi / Leaves;
	tp.count = 0;
	
	// Ensure we have sensible edge lengths, and get maximum height of tree
	Root->SetHeight (0.0);
    if (IsFlag (TF_HAS_EDGES))
	{
		// Use actual branch lengths
		getHeights (Root);
	}
	else
	{		
		// Use edge lengths of unit length
		getRadialNodeHeights (Root);
    }

	// Compute scale factor
	tp.scale = (tp.r.GetWidth () / 2) / tp.maxheight;
	
	
	// Now compute angles for nodes (overwrites height field)
	tp.count = 0;
	getLeafAngles (Root);	

    // Compute coordinates then draw
	radialCalc (Root);
	
	// Actual drawing might be quite small due to angles of branches, so
	// find most distant leaf from centre and rescale accordingly
	int i;
	float maxdist = 0.0;
	PoinT ptRoot;
	Root->GetXY (ptRoot);
	for (i = 0; i < Leaves; i++)
	{
		PoinT ptLeaf;
		Nodes[i]->GetXY (ptLeaf);

		// with 16 bit integers we have an overflow problem
		float a = ptLeaf.x - ptRoot.x;
		float b = ptLeaf.y - ptRoot.y;
		float c = a * a + b * b; 
		float hypotenuse = sqrt (c);
		if (hypotenuse > maxdist)
			maxdist = hypotenuse;
	}
	// Now, rescale tree
	float rescale = (tp.r.GetWidth () / 2) / maxdist;
	tp.scale = tp.scale * rescale;
	radialCalc (Root);

	// Lastly, can we place tree in a more pleasing position in the rect?
	// Find the greatest extent of the tree and reposition it.
	int maxLeft   = r.GetRight ();
	int maxRight  = r.GetLeft ();
	int maxTop    = r.GetBottom();
	int maxBottom = r.GetTop ();
	BaseNodePtr nodeLeft = NULL;
	BaseNodePtr nodeRight = NULL;
	for (i = 0; i < Leaves; i++)
	{
		PoinT ptLeaf;
		Nodes[i]->GetXY (ptLeaf);
		if (ptLeaf.x < maxLeft)
		{
			maxLeft = ptLeaf.x;
			nodeLeft = Nodes[i];
		}
		if (ptLeaf.x > maxRight)
		{
			maxRight = ptLeaf.x;
			nodeRight = Nodes[i];
		}
		maxTop = MIN (maxTop, ptLeaf.y);
		maxBottom = MAX (maxBottom, ptLeaf.y);
	}
	// Adjust x dimensions to account for the size of labels.	
	Port.SelectAFont (font);
	maxLeft -= Port.TextWidth (nodeLeft->GetLabel(), strlen (nodeLeft->GetLabel()));
	maxRight += Port.TextWidth (nodeRight->GetLabel(), strlen (nodeRight->GetLabel()));

	// Move the image, if necessary
	int centre_x = r.GetLeft () + (r.GetRight() - r.GetLeft ()) / 2;
	int centre_y = r.GetTop() + (r.GetBottom() - r.GetTop ()) / 2;
	int centre_tx = maxLeft + (maxRight - maxLeft) / 2;
	int centre_ty = maxTop + (maxBottom - maxTop) / 2;
	tp.r.Offset (-(centre_tx - centre_x), -(centre_ty - centre_y));
	// Relocate the root and recalculate position of tree
	pt.x = tp.r.GetLeft () + tp.r.GetWidth () /2;
	pt.y = tp.r.GetTop () + tp.r.GetHeight () /2;
	Root->SetXY (pt);
	radialCalc (Root);

#ifdef SCALE
	// Draw a scale bar to show amount of evolutionary change
	if (IsFlag (TF_HAS_EDGES))
   {
		float maxXYdist = 0.0; // max distance in device coordinates
   	float maxdist = 0.0;   // actual evolutionary distance
		for (i = 0; i < Leaves; i++)
		{
			BaseNodePtr p = Nodes[i];
			float d = 0.0;
         float dxy = 0.0;
			while (p != Root)
			{
         	// Distance to root in device units
				// Use floats as 16 bit integers overflow.
				PoinT ptAnc;
				PoinT ppt;
            p->GetXY (ppt);
            p->GetAnc()->GetXY (ptAnc);
				float a = ppt.x - ptAnc.x;
				float b = ppt.y - ptAnc.y;
				float c = a*a + b*b;
				dxy = dxy + sqrt (c);
            // Distance to root in evolutionary units
				d = d + p->GetEdgeLength ();

				p = p->GetAnc();
			}
			if (d > maxdist)
				maxdist = d;
			if (dxy > maxXYdist)
				maxXYdist = dxy;         	
		}
      // Use log scale
		float m = log10 (maxdist);
		int i = (int) m - 1;
		float bar = pow (10, i);
		int scalebar = (bar / maxdist) * maxXYdist;

      // Draw scale bar
		PoinT pt1;
		pt1.x = r.GetLeft();
		pt1.y = r.GetBottom();
		PoinT pt2;
		pt2.x = pt1.x + scalebar;
		pt2.y = pt1.y;
		Port.DrawLine (pt1, pt2);

		// Label scale
		char buf[20];
		if (i >= 0)
		{
			sprintf (buf, "%d", int (bar));
		}
		else
		{
     	int j = abs (i);
			sprintf (buf, "%.*f", j, bar);
		}
		// Ensure enough space for label
		Port.SelectAFont (tp.edgefont); // 16/4/96
		int l = Port.TextWidth (buf, strlen (buf)) + Port.MaxCharWidth();;
		if (l > scalebar)
      	pt2.x = pt1.x + l;
		VRect r;
		r.SetLeft(pt1.x);
		r.SetRight (pt2.x);
		r.SetBottom(pt1.y);
		r.SetTop (r.GetBottom() - Port.FontHeight ());
		r.Offset (0, -2);
   	Port.DrawText (r, buf, strlen (buf), 'c', FALSE);
 	}
#endif

	// Draw completed tree
	radialDrawTraverse (Root);
}
#endif

//------------------------------------------------------------------------------
// Create a star tree with n leaves
void BaseTree::StarTree (int n)
{
	Leaves = n;
	Internals = 1;

	Root = NewNode();
	Root->SetWeight (n);
	Root->SetDegree (n);
	Root->cluster.FullSet (n);

	CurNode = NewNode();
	Nodes[0] = CurNode;
	CurNode->SetLeaf();
	CurNode->SetLeafNumber(1);
	CurNode->SetLabelNumber(1);
	CurNode->cluster += 1;

	Root->child = CurNode;
	CurNode->anc = Root;

	// Remaining leaves
	for (int i = 1; i < n; i++)
	{
		BaseNodePtr q = NewNode ();
		Nodes[i] = q;
		q->SetLeaf();
		q->SetLeafNumber(i+1);
		q->SetLabelNumber(i+1);
		q->cluster += (i + 1);
		q->anc = Root;
		CurNode->sib = q;
		CurNode = q;
	}
}



//------------------------------------------------------------------------------
void BaseTree::checkSiblings (BaseNodePtr q, Bitset &b)
{
	BaseNodePtr r = q;
	BaseNodePtr s = q->sib;
	BaseNodePtr t = q->anc;

	while (s)
	{
		if (s->cluster <= b)
		{
			s->anc = q->anc;
			r = s;
			s = s->sib;
		}
		else
		{
			t->sib = s;
			r->sib = s->sib;
			t = s;
			t->sib = NULL;
			s = r->sib;
		}
	}
}


//------------------------------------------------------------------------------
int BaseTree::AddCluster (Bitset &b, float e, char *slabel, long ilabel)
{
	Error = 0;
	Bitset s;
	s.NullSet();
	s += b;
	int n = s.Cardinality();
	if (n > 1 && n < Root->cluster.Cardinality())
	{
		// non trivial set
		BaseNodePtr p = Root;
		BaseNodePtr q = p->child;
		BooleaN Finished = False;
		SetRelations r;

		// locate insertion point
		while (q && !Finished)
		{
			r = s.Relationship (q->cluster);
			switch (r)
			{
				case rdmpIDENTITY:
					Finished = True;
					Error = 1;
					break;

				case rdmpSUBSET:
					p = q;
					q = q->child;
					break;

				case rdmpDISJOINT:
					p = q;
					q = q->sib;
					break;

				case rdmpSUPERSET:
					Finished = True;
					break;

				case rdmpOVERLAPPING:
					Finished = True;
					Error = 2;
					break;

				default:
					break;
			}
		}

		if (Error == 0)
		{
			// Create node
			BaseNodePtr nunode = NewNode ();
			Internals++;
			nunode->weight = n;
			nunode->cluster += s;
			if (slabel)
				nunode->SetLabel (slabel);
			else if (ilabel > -1)
			{
				char buf[32];
				sprintf (buf, "%d", ilabel);
				nunode->SetLabel (buf);
			}
			if (e != -1.0)
				nunode->SetEdgeLength (e);
			

			// Insert node in tree
			switch (r)
			{
				case rdmpSUBSET:
					p->child =  nunode;
					nunode->anc = p;
					break;

				case rdmpDISJOINT:
					p->sib = nunode;
					nunode->anc = p->anc;
					break;

				case rdmpSUPERSET:
					if (q == p->child)
					{
						p->child = nunode;
						nunode->child = q;
						nunode->anc = p;
						q->anc = nunode;
					}
					else
					{
						p->sib = nunode;
						nunode->child = q;
						nunode->anc = p->anc;
						q->anc = nunode;
					}
					checkSiblings (q, nunode->cluster);
				}
			nunode->GetDegreeOf ();
			nunode->anc->degree = nunode->degree - nunode->degree - 1;
		}
	}
	return Error;
}


// Home 4/5/97



//------------------------------------------------------------------------------
void BaseTree::DrawInteriorEdge (BaseNodePtr p)
{
	BaseNodePtr r = p->GetAnc ();
	int stop = r->GetHeight() * tp.scale;
	if (p->IsTheChild ())
	{
		// Visiting ancestor for the first time, so draw the
		// end symbol
		if (r == Root)
		{
			if (IsRooted ())
				Line[stop] = TEE;   // «
			else
				Line[stop] = VBAR;  // Ò
		}
		else
		{
			Line[stop] = TEE;      // «
		}


		// Draw branch itself
		if (r != Root)
		{
			// Line
			int start = r->GetAnc()->GetHeight() * tp.scale;
			for (int i = start + 1; i < stop; i++)
			{
				Line[i] = HBAR; // €
			}
			// Start symbol
			if (start == stop)
				Line[start] = VBAR;     // Ò
			else if (r->IsTheChild ())
				Line[start] = LEFT;     // ò
			else if (r->GetSibling ())
				Line[start] = SIB;      // Ì
			else Line[start] = RIGHT;  // Ë

			//
			Fill_In_Ancestors (r);
		}
	}
	else
	{
		// Just draw nodes below
		Line[stop] = VBAR;
		Fill_In_Ancestors (p->GetSibling());
	}

	// Output the line
	Line[stop + 1] = '\0';

	*treeStream << Line;

	// Draw internal label, if present
	if (r->GetLabel())
		*treeStream  << r->GetLabel();
	 *treeStream  << EOLN;

	// Clear the line for the next pass
	int j = MAXLEAVES + MAXNAMELENGTH;
	for (int i = 0; i < j; i++)
		Line[i] = ' ';
}


//------------------------------------------------------------------------------
void BaseTree::DrawPendantEdge (BaseNodePtr p)
{
		BaseNode *q = p->GetAnc();
		int start = q->GetHeight() * tp.scale;
		int stop = p->GetHeight() * tp.scale;
		char	symbol;

		// Draw line between p and its ancestor
		int i;
		for (i = start + 1; i <= stop; i++)
			Line[i] = (char)HBAR;      // €

		// Find appropriate symbol for link to ancestor
		if (p == q->GetChild())
		{
			symbol = (char)LEFT;       // ò
		}
		else
		{
			// p is a sibling
			if (p->GetSibling())
				symbol = (char)SIB;     // Ì
			else symbol = (char)RIGHT; // Ë
		}
		Line[start] = symbol;

		// Fill in ancestors
		Fill_In_Ancestors (p);

		// Terminate line
		Line[stop + 1] = '\0';



		// Output line and taxon name
		*treeStream << Line << " " << p->GetLabel () << EOLN;

		// Clear the line for the next pass
		int j = MAXLEAVES + MAXNAMELENGTH;
		for (i = 0; i < j; i++)
			Line[i] = ' ';
}



//------------------------------------------------------------------------------
/*
	{ Put a VBAR symbol ("Ò") into buffer where needed to
	  represent branches of tree under p. Two cases:

			.....  Ancestor
			€€€€>  Sibling
			<----  Child

		  1. r is a sibling of the child of q, and q has a sibling, or

					x€€€€sib€€€>r
					 ò    .    .
					  \   .   .
						\  .  .
				  child\ . .|
						  \.. |
							q€€|€€€sib€€€>
							 \ |
							  \| <- this branch passes under r
								\


		  2. r is q's child and q is a sibling of another node.

						  r
							ò
							 \
							 |\
							 | \
				  x€€€sib€|€>q
							 | .
							 |.<- this branch passes under r
							 .
							.

	} */

//------------------------------------------------------------------------------
void BaseTree::Fill_In_Ancestors (BaseNodePtr p)
{

	BaseNodePtr q = p->GetAnc ();
	BaseNodePtr r = p;
	int count = 0;
	while (q != Root)
	{
		if (
			(q->GetSibling () && !(r->IsTheChild ()))
			|| (!(q->IsTheChild ()) && r->IsTheChild())
			)
		{
//		Line[(int)(q->GetAnc()->GetHeight() * tp.scale)] = VBAR;

			if (r ==p && q->GetHeight() == q->GetAnc()->GetHeight())
				Line[(int)(q->GetAnc()->GetHeight() * tp.scale)] = SIB;
			else
			{
/*				if (count == 0)
				{
					if ((q->GetHeight() - 1) == q->GetAnc()->GetHeight())
						Line[(int)(q->GetAnc()->GetHeight() * tp.scale)] = '&';
					else
					Line[(int)(q->GetAnc()->GetHeight() * tp.scale)] = '*';
				}
				else*/
					Line[(int)(q->GetAnc()->GetHeight() * tp.scale)] = VBAR;
//				count++;
			}

		}
		r = q;
		q = q->GetAnc ();
	}
}





//------------------------------------------------------------------------------
void BaseTree::drawAsTextTraverse (BaseNodePtr p)
{
	if (p)
	{
		drawAsTextTraverse (p->GetChild ());
		if (p->IsLeaf ())
			DrawPendantEdge (p);
		if (p->GetSibling ())
			DrawInteriorEdge (p);
		drawAsTextTraverse (p->GetSibling ());
	}
}

