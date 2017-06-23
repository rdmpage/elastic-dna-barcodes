// $Log: Tree.cpp,v $
// Revision 1.12  2002/05/10 16:58:40  rdmp1c
// Ultrametric tree scale added
//
// Revision 1.11  2001/11/26 12:27:23  mac
// Moving towards using Mike's BBitset instead of Rod's Bitset class.
//
// Revision 1.10  2001/10/23 10:03:44  rdmp1c
// Midpoint rooting added
//
// Revision 1.9  2001/09/07 15:37:30  mac
// cosmetic change to tree output.
//
// Revision 1.8  2001/08/08 13:17:00  mac
// cosmetic change to output tree
//
// Revision 1.7  2001/03/27 10:14:16  mac
// Made NodePtr Tree::RemoveNode (NodePtr Node) actually
// return a NodePtr, thus avoiding irritating compiler warnings.
//
// Revision 1.6  2000/09/06 15:58:56  mac
// Fixed delete [] X when X created with new []
//
// Revision 1.5  2000/02/15 16:40:05  rdmp1c
// A bug reported by J. Peter Gogarten
// <gogarten@uconnvm.uconn.edu> and Martyn Kennedy was due
// to Internals not being incremented when the tree is rerooted.
// This would cause the Tree Editor to crash in TreeView 1.6.0 if the user rerooted
// a tree had a basal polytomy.
//
// Revision 1.4  1999/10/08 11:20:26  rdmp1c
// Add editing internal labels for Ziheng Yang
//
// Revision 1.3  1999/05/07 20:18:29  rdmp1c
// Fixed rerooting bug
//
//

//
// (c) 1993, Roderic D. M. Page
//
// Filename:
//
// Author:        	Rod Page
//
// Date:			15 June 1993
//
// Class:           tree
//
// Purpose:		n-tree object
//
// Modifications:	29 June 1993 Draw method added
//
//

#include <ctype.h>
#include <iomanip.h>
#include <string.h>
#include <math.h>
#include <fstream.h>

#ifdef Macintosh
#include "VString.h"
#endif

#define SCALE

#include "Tree.h"



#define ROOT_CHAR 219
#define epsilon 0.001
#define pi 3.141592


//------------------------------------------------------------------------------
// Constructor
Tree::Tree()
{
	Root		= NULL;
	Leaves    	= 0;
	Internals 	= 0;
	Name		= NULL;
	Error		= errOK;
	SetFlag(TF_ROOTED, True); // tree is rooted by default
    SetFlag(TF_VALID, False);
}

//------------------------------------------------------------------------------
// 29/7/97
// Copy constructor
Tree::Tree (Tree &t)
{
	Root      = t.CopyOfSubtree (t.GetRoot());
	Leaves    = t.GetLeaves ();
	Internals = t.GetInternals ();
	if (t.GetName())
	{
		Name = new char[strlen (t.GetName()) + 1];
		strcpy (Name, t.GetName()); // 25/7/97
	}
	SetFlag (TF_ROOTED, t.IsFlag (TF_ROOTED));
}


//------------------------------------------------------------------------------
// Traverse tree in postorder deleting nodes by calling DeleteNode 
void Tree::deletetraverse (NodePtr p)
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
Tree::~Tree()
{
	DeleteTree();
}


//a

//b
//------------------------------------------------------------------------------
void Tree::buildtraverse (NodePtr p)
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
//            Nodes[p->GetLabelNumber () - 1] = p;
			// 18/8/97 Allow for pruned trees
            Nodes[p->GetLeafNumber () - 1] = p;
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
void Tree::CleanUp ()
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
void Tree::compresstraverse (NodePtr p)
{
	if (p)
	{
		Node *q = p->GetAnc();
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
					Line[Leaves - q->GetAnc()->GetWeight()]
						= (char)VBAR;
				}
				q = q->GetAnc();
			}
		}
		Line[start] = symbol;

		// If p is a leaf then we output the line buffer
		if (p->IsLeaf())
		{
      		Line[Leaves] = '\0';
			*treeStream << Line << p->GetLabelNumber() << EOLN;
			int j = MAXLEAVES + MAXNAMELENGTH;
         	for (i = 0; i < j; i++)
         		Line[i] = ' ';
		}

          // Traverse the rest of the tree
		compresstraverse (p->GetChild());
		compresstraverse (p->GetSibling());
	}
}


//d

//------------------------------------------------------------------------------
// Free the memory allocated to the tree
void Tree::DeleteTree ()
{
	deletetraverse (Root);
	if (Name) delete [] Name;
}

//------------------------------------------------------------------------------
// Draw the tree to a text device, default is compressed format
void Tree::Draw (ostream &f, BOOL compress)
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
      f << Name << " = " << EOLN;

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
 //        }
	      drawAsTextTraverse (Root);
      }

      // Free up memory
      delete [] Line;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
	}
	else f << "(No tree)" << EOLN;
}

//------------------------------------------------------------------------------
void Tree::dumptraverse (NodePtr p)
{
	if (p)
	{
//		p->Dump ();
		dumptraverse (p->GetChild ());
		dumptraverse (p->GetSibling ());
	}
}

//------------------------------------------------------------------------------
// Dump the tree to cout for debugging
void Tree::Dump ()
{
	cout << "Tree = " << Name << endl;
	dumptraverse (Root);
}

//e

//f

//i

//------------------------------------------------------------------------------
// Check that tree is completely read, return 0 is OK
int Tree::IsFinished ()
{
	if (!St.EmptyStack()) return (errStackNotEmpty);
     // Need to check for full set of leaves here
	return (errOK);
}


//------------------------------------------------------------------------------
// Make CurNode a leaf, called by tree reading code
void Tree::MakeCurNodeALeaf (int i)
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
int Tree::MakeChild ()
{
	NodePtr	q;

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
int Tree::MakeRoot ()
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
int	Tree::MakeSibling ()
{
	NodePtr	q;
     NodePtr	ancestor;

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



//p

//------------------------------------------------------------------------------
//Parse a nested parenthesis style tree description, returning NULL if
//successful, otherwise return the unprocessed part of the description.
char *Tree::Parse (char *TreeDescr)
{
	enum {stGETNAME, stGETINTERNODE, stNEXTMOVE,
		stFINISHCHILDREN, stQUIT, stACCEPTED} state;
	NodePtr		q;
	tokentype	token;
	char 		*curpos;
	int			count = 1;
	char		buf[16];

     // Initialise tree variables
	Root 	= NULL;
	Leaves 	= 0;
	Internals = 0;
	St.Clear();
	Error 	= errOK;

	// Create first node
	CurNode	= NewNode();
//	CurNode->SetIndex (count);
     Root		= CurNode;

     // Initialise FSA that reads tree
	state = stGETNAME;
	curpos = TreeDescr;
	token = NextToken (curpos, buf);

	while ((state != stQUIT) && (state != stACCEPTED))
	{
		switch (state)
		{
			case stGETNAME:
				switch (token)
                    {
					case SPACE:
						token = NextToken (curpos, buf);
						break;
					case ASTRING:
						Nodes[Leaves++] = CurNode;
						CurNode->SetLeaf ();
						CurNode->SetLabelNumber (Leaves);
						CurNode->SetLeafNumber (Leaves);
						CurNode->SetWeight (1);
						CurNode->SetLabel (buf);
						token = NextToken (curpos, buf);
						state = stGETINTERNODE;
                              break;
					case ANUMBER:
						break;  // ignore for now
					case LPAR:
						state = stNEXTMOVE;
						break;
					case ENDOFSTRING:
						Error = errEnd;
						state = stQUIT;
                              break;
					default:
						Error = errSyntax;
						state = stQUIT;
				}
                    break;

			case stGETINTERNODE:
				switch (token)
				{
					case SPACE:
						token = NextToken (curpos, buf);
						break;
					case COMMA:
						state = stNEXTMOVE;
						break;
					case RPAR:
						state = stNEXTMOVE;
						break;
					case ENDOFSTRING:
						Error = errEnd;
						state = stQUIT;
                              break;
					default:
						Error = errSyntax;
						state = stQUIT;
				}
                    break;

			case stNEXTMOVE:
				switch (token)
				{
					case SPACE:
						token = NextToken (curpos, buf);
						break;
					// The next node encountered will be a sibling
					// of Curnode and a descendant of the node on
                         // the top of the node stack.
					case COMMA:
						count++;
						q = NewNode();
//						q->SetIndex (count);
						CurNode->SetSibling (q);
						if (St.EmptyStack())
						{
							Error = errMissingLPar;
							state = stQUIT;
						}
						else
						{
							q->SetAnc (St.TopOfStack());
							q->AddWeight (CurNode->GetWeight());
							CurNode = q;
							state = stGETNAME;
							token = NextToken (curpos, buf);
						}
						break;
					//The next node will be a child of CurNode, hence
					//we create the node and push CurNode onto the
                         //node stack.
					case LPAR:
						count++;
						Internals++;
						St.Push (CurNode);
						q = NewNode();
//						q->SetIndex (count);
						CurNode->SetChild (q);
						q->SetAnc (CurNode);
						CurNode = q;
						token = NextToken (curpos, buf);
						state = stGETNAME;
						break;
					//We've finished ready the descendants of the node
     	               //at the top of the node stack so pop it off.
					case RPAR:
						if (St.EmptyStack())
						{
							Error = errUnbalanced;
							state = stQUIT;
						}
						else
						{
							q = St.TopOfStack();
							q->AddWeight (CurNode->GetWeight());
							CurNode = q;
                              	St.Pop ();
							state = stFINISHCHILDREN;
							token = NextToken (curpos, buf);
						}
						break;
                         //We should have finished ready the tree
					case SEMICOLON:
						if (St.EmptyStack())
						{
							state = stACCEPTED;
						}
						else
						{
							Error = errStackNotEmpty;
							state = stQUIT;
						}
						break;
					case ENDOFSTRING:
						Error = errEnd;
						state = stQUIT;
                              break;
					default:
						Error = errSyntax;
						state = stQUIT;
				}
                    break;

			case stFINISHCHILDREN:
				switch (token)
				{
					case SPACE:
						token = NextToken (curpos, buf);
						break;
					//We've completed traversing the descendants of the
					//node at the top of the stack, so pop it off.
					case RPAR:
						if (St.EmptyStack())
						{
							Error = errUnbalanced;
							state = stQUIT;
						}
						else
						{
							q = St.TopOfStack();
							q->AddWeight (CurNode->GetWeight());
							CurNode = q;
                              	St.Pop ();
							token = NextToken (curpos, buf);
                              }
						break;
					//The node at the top of the stack still has some
					//descendants.
					case COMMA:
						count++;
						q = NewNode();
//						q->SetIndex (count);
						CurNode->SetSibling (q);
						if (St.EmptyStack())
						{
							Error = errMissingLPar;
							state = stQUIT;
						}
						else
						{
							q->SetAnc (St.TopOfStack());
							q->AddWeight (CurNode->GetWeight());
							CurNode = q;
							state = stGETNAME;
							token = NextToken (curpos, buf);
						}
						break;
					case SEMICOLON:
						state = stNEXTMOVE;
						break;
					default:
						if (St.EmptyStack())
						{
							Error = errSemicolon;
						}
						else
						{
							Error = errSyntax;
						}
                              state = stQUIT;
				}
				break;
		}
	}
	// Handle errors
	if (state == stQUIT)
	{
     	//Clean up to go here
		return (curpos);
	}
	else
	{
		SetFlag (TF_VALID, False);
		Update ();
		return (NULL);
     }
}

//------------------------------------------------------------------------------
// Pop a node off the stack and make it the current node.
// Return 0 if successful
int Tree::PopNode ()
{
	NodePtr q;

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
void Tree::SetName (const char *aname)
{
	if (aname != NULL)
     {
		Name = new char[strlen (aname) + 1];
		*Name = '\0';
		strcpy (Name, aname);
	}
}

#ifdef newbuf
//------------------------------------------------------------------------------
int Tree::TreeFromRecord (TreeRecPtr tr)
{
	int i;
    int j;
	int numnodes;
    Node *p, *q, *r;

	Name		= NULL;	
	Leaves 		= tr->leaves;
	Internals 	= tr->internals;
	numnodes 	= Leaves + Internals;
	Error		= errOK;
	if (tr->edges != NULL) SetFlag (TF_HAS_EDGES, True);
	if (tr->nodeLabels != NULL) SetFlag (TF_INTERNAL_LABELS, True);

	// Allocate memory for tree
	i = 0;
	while ((i < numnodes) && (Error == errOK))
	{
		Nodes[i] = NewNode();
		if (!Nodes[i]) Error = errNoMemory;
		i++;
	}

	if (Error) return (Error);

	// Do leaves
	for (i = 0; i < Leaves; i++)
	{
		Nodes[i]->SetLeaf();
		Nodes[i]->SetLabelNumber (i+1);
		Nodes[i]->SetLeafNumber (i+1);

		if (IsFlag (TF_HAS_EDGES))
		{
			Nodes[i]->SetEdgeLength (tr->edges[i]);
		}

		// Add node
		p = Nodes[i];
		q = Nodes[tr->ancfunc[i]]; // ancestor

		p->SetAnc (q);
		if (q->GetChild())
		{
			r = q->GetChild();
			r = r->GetRightMostSibling();
					r->SetSibling (p);
		}
		else q->SetChild (p);
	}

	//Do internals
	for (i = Leaves; i < numnodes; i++)
	{
		if (IsFlag (TF_HAS_EDGES))
		{
			Nodes[i]->SetEdgeLength (tr->edges[i]);
		}
		if (IsFlag (TF_INTERNAL_LABELS))
		{
			Nodes[i]->SetLabel (tr->nodeLabels[i]);
		}

		j = tr->ancfunc[i]; // ancestor

		if (j < 0) Root = Nodes[i];
		else
		{
			// Add node
			p = Nodes[i];
			q = Nodes[tr->ancfunc[i]];
			p->SetAnc (q);
			if (q->GetChild())
			{
				r = q->GetChild();
				r = r->GetRightMostSibling();
				r->SetSibling (p);
			}
			else q->SetChild (p);
		}
	}

	//Ensure fields are correct
    SetFlag (TF_VALID, False);
	Update();

    return (errOK);
}
#else
//------------------------------------------------------------------------------
// Create the tree described by the ancestor function s
int Tree::StringToTree (unsigned char *s)
{
	int i;
     int j;
	int numnodes;
     Node *p, *q, *r;

	Name		= NULL;	
	Leaves 	= (int)s[0];
	Internals = (int)s[1];
	numnodes 	= Leaves + Internals;
     Error	= errOK;


     // Allocate memory for tree
	i = 0;
	while ((i < numnodes) && (Error == errOK))
     {
		Nodes[i] = NewNode();
		Nodes[i]->SetLabelNumber (i+1);
		if (!Nodes[i]) Error = errNoMemory;
		i++;
	}

	if (Error) return (Error);

	// Do leaves
	for (i = 0; i < Leaves; i++)
	{
		Nodes[i]->SetLeaf();
		Nodes[i]->SetLabelNumber (i+1);
		Nodes[i]->SetLeafNumber (i+1);

		// Add node
		p = Nodes[i];
          j = (int)s[i+2]; //index of ancestor 
		q = Nodes[j-1]; // ancestor

		p->SetAnc (q);
		if (q->GetChild())
		{
			r = q->GetChild();
			r = r->GetRightMostSibling();
               r->SetSibling (p);
		}
          else q->SetChild (p);
	}

	//Do internals
	for (i = Leaves; i < numnodes; i++)
	{
		j = (int)s[i+2]; // ancestor

		if (j == ROOT_CHAR) Root = Nodes[i];
		else
		{
			// Add node
			p = Nodes[i];
			q = Nodes[j-1]; // ancestor
			p->SetAnc (q);
			if (q->GetChild())
			{
				r = q->GetChild();
				r = r->GetRightMostSibling();
          	     r->SetSibling (p);
			}
			else q->SetChild (p);
          }

	}

	//Ensure fields are correct
     SetFlag (TF_VALID, False);
	Update();

     return (0);

}

#endif
//t

//------------------------------------------------------------------------------
void Tree::traverse (NodePtr p)
{

	if (p)
	{
		if (p->IsFlag (NF_LEAF))
		{
			// Glasgow
			if (!IsFlag (TF_TRANSLATE) && (p->GetLabel() != NULL))
			{
				char buf[MAXNAMELENGTH + 1]; // rdp 11/1/96
				strcpy (buf, p->GetLabel ());
				*treeStream << OutputString (buf);
			}
			else
			{
				*treeStream << p->GetLabelNumber();
			}
			if (IsFlag (TF_HAS_EDGES))
			{
				*treeStream << ':' << p->GetEdgeLength ();
			}
		}
		else
		{
			*treeStream << "(";
		}
		traverse (p->GetChild());
		if (p->GetSibling())
		{
			*treeStream << ",";
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
				}
				if (IsFlag (TF_HAS_EDGES) && (p->GetAnc () != Root))
				{
					*treeStream << ':' << p->GetAnc()->GetEdgeLength ();
				}
			}
		}
		traverse (p->GetSibling());
	}
}

#ifdef newbuf
void Tree::TreeToRecord (TreeRecPtr tr)
{
	tr->leaves 	= Leaves;
	tr->internals 	= Internals;

	int	count 	= Leaves;

	for (int i = 0; i < Leaves; i ++)
	{
		NodePtr p = Nodes[i];
		NodePtr q = p->GetAnc ();
		p->SetIndex (i);
		if (IsFlag (TF_HAS_EDGES))
		{
			tr->edges[i] = p->GetEdgeLength ();
		}


		// If ancestor has already been labelled then use that
		// label, otherwise label ancestor
		if (q->GetIndex ())
		{
			tr->ancfunc[i] = q->GetIndex ();
		}
		else
		{
			while ((q) && (!q->GetIndex()))
			{
				// Label ancestor, and any of its unlabelled
				// ancestors
				q->SetIndex (count++);
				tr->ancfunc[p->GetIndex ()] = q->GetIndex ();

				if (IsFlag (TF_HAS_EDGES))
				{
					tr->edges[q->GetIndex()] = q->GetEdgeLength ();
				}

				if (IsFlag (TF_INTERNAL_LABELS))
				{
					// allocate memory for this label
					if (q->GetLabel () != NULL)
					{
						tr->nodeLabels[q->GetIndex()] = new char [strlen (q->GetLabel ()) + 1];
						strcpy (tr->nodeLabels[q->GetIndex()], q->GetLabel ());
					}
				}

				p = q;
				q = q->GetAnc ();
			}
			if (q)
			{
				tr->ancfunc[p->GetIndex ()] = q->GetIndex ();
			}
		}
	}
	tr->ancfunc[Root->GetIndex ()] = -1;
}
#else
//------------------------------------------------------------------------------
// Return a string representation of a tree
// NOTE: Assumes index field of all nodes is 0, and that Nodes list
// is currently valid for leaves
// On returning the Nodes field for both leaves and internals is correct
unsigned char *Tree::TreeToString ()
{
	unsigned char* 	s;
	int		count;
	int 		i;
     Node		*p, *q;

	s = new char[Leaves + Internals + 3];
     s[Leaves + Internals + 2] = '\0';

	if (!s) return (NULL);
     else
	{
		s[0] = (char)Leaves;
		s[1] = (char)Internals;
          count = Leaves + 1;

		for (i = 0; i < Leaves; i ++)
		{
			p = Nodes[i];
			q = p->GetAnc ();

			// if ancestor has already been labelled then use that
               // label, otherwise label ancestor
			if (q->GetIndex ())
			{
				s[i + 2] = (char)q->GetIndex ();
				Nodes[q->GetIndex() - 1] = q;
               }
			else
			{
				while ((q) && (!q->GetIndex()))
				{
					// label ancestor, and any of its unlabelled
					// ancestors

                    	q->SetIndex (count++);
					if (p->IsLeaf()) s[i + 2] = (char)q->GetIndex();
                         else s[p->GetIndex() + 1] = (char)q->GetIndex();
					p = q;
					q = q->GetAnc ();
				}
				if (q)
				{
					s[p->GetIndex() + 1] = (char)q->GetIndex();
					Nodes[q->GetIndex() - 1] = q;
				} 
			}
		}

		//Set root's "ancestor"
		s[Root->GetIndex() + 1] = (unsigned char)ROOT_CHAR;
          // null terminate string
		s[Leaves + Internals + 2] = '\0';
		return (s);
     }
}
#endif

//w

//------------------------------------------------------------------------------
// Write the tree in nested parenthesis form to cout
void Tree::write()
{
	treeStream = &cout;
	*treeStream << "tree " << Name << "=";
	if (Root) {
		traverse (Root);
	}
	else {
		*treeStream << "nil";
	}
	*treeStream << ";" << "\n";
}

void Tree::WriteNEXUS (ostream &f)
{
	// 5 Nov 1994. Ensure edge lengths are correctly output
	f.setf (ios::fixed, ios::floatfield);
	treeStream = &f;


	if (IsFlag (TF_ROOTED)) f << "\tTREE ";
	else f << "\tUTREE ";

	if (IsFlag (TF_DEFAULT)) f << "* ";

	f << Name << " = ";

	traverse (Root);

	f << ";" << EOLN;
}

void Tree::WriteNEXUS_BPA (ostream &f)
{
	treeStream = &f;

	BooleaN hasEdges = (BooleaN)IsFlag (TF_HAS_EDGES);
	SetFlag (TF_HAS_EDGES, False);
	
	f << "\tTREE ";

	if (IsFlag (TF_DEFAULT)) f << "* ";

	f << Name << "= (";

	traverse (Root);

	f << ",ANCESTOR);" << EOLN;
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
void Tree::Update ()
{
	if (!IsFlag (TF_VALID))
	{
     	count = Leaves;
		buildtraverse (Root);
		SetFlag (TF_VALID, True);
     }
}


//------------------------------------------------------------------------------
void Tree::calc (NodePtr p)
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
void Tree::radialCalc (NodePtr p)
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
void Tree::getHeights(NodePtr p)
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
void Tree::getNodeHeights(NodePtr p)
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
void Tree::getLeafAngles (NodePtr p)
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
			NodePtr anc = p->GetAnc ();
			if (anc)
			{
				anc->SetHeight ((anc->GetChild()->GetHeight() + p->GetHeight()) / 2);
//				*treeStream <<  "   " << anc->GetHeight() << EOLN;
			}
		}
		getLeafAngles (p->GetSibling());
	}
}


//------------------------------------------------------------------------------
// Draw the tree
void Tree::drawTraverse (NodePtr p)
{
	if (p)
	{
		drawTraverse (p->GetChild());
		p->Draw (tp);
/*		// RADIAL debug
		PoinT pt;
		p->GetXY (pt);
		*treeStream << p->GetIndex() << '\t' << p->GetEdgeLength() << '\t' << p->GetHeight()
		<< '\t' << "x=" << pt.x << " y=" << pt.y << EOLN;
*/
		drawTraverse(p->GetSibling());
	}
}

//------------------------------------------------------------------------------
// Draw the tree  within the specified rectangle
// using the desired style (CLADOGRAM is the default)
void Tree::Draw (VRect r, VFontPtr font, unsigned int style, VFontPtr edgeFont)
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

	// Allow space for scale bar
	bool isUltra = false;
	int scalebar_space = Port.FontHeight() * 2;
	if (IsFlag(TF_HAS_EDGES) && (style & TS_PHYLOGRAM))
	{
		tp.r.SetBottom (tp.r.GetBottom() - scalebar_space);
		isUltra = (bool)IsUltrametric();
	}

	tp.count	= 0;
	tp.leaves	= Leaves;
	tp.lasty	= 0;
	// 10/2/97
	float w = r.GetRight() - r.GetLeft();
	float l = Leaves - 1;
	tp.nodegap = w/l;
//	tp.nodegap     = (r.GetRight() - r.GetLeft()) / (Leaves - 1);
#ifdef SCALE
	// 10/2/97
	float h = tp.r.GetBottom() - tp.r.GetTop();
//	if (IsFlag(TF_HAS_EDGES) && (style & TS_PHYLOGRAM))
//	{
//		tp.leafgap = h / (l + 1);
//	}
//	else
		tp.leafgap = h/l;

/*	// 10/1/97
	if (IsFlag(TF_HAS_EDGES) && (style & TS_PHYLOGRAM))
		tp.leafgap	= (tp.r.GetBottom() - tp.r.GetTop()) / (Leaves);
	else
		tp.leafgap	= (tp.r.GetBottom() - tp.r.GetTop()) / (Leaves - 1); */
#else
	tp.leafgap	= (tp.r.GetBottom() - tp.r.GetTop()) / (Leaves - 1);
#endif


	// 16/4/96
	if (edgeFont)
		tp.edgefont = edgeFont;
	else
		tp.edgefont = font;

	// 10/1/97
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
			Port.SelectAFont (font);
			float m = log10 (tp.maxheight);
			int i = (int) m - 1;
			float bar = pow ((float)10, (float)i);
			
			// Draw a scale bar the length of the unit
			int scalebar = ((bar / tp.maxheight) * (tp.r.GetRight() - tp.r.GetLeft()));

			
			PoinT pt1;
			PoinT pt2;

			if (isUltra)
			{
				// Draw a scale bar the full length of the tree
				pt1.x = tp.r.GetLeft();
				pt1.y = tp.r.GetBottom() + scalebar_space;
				pt2.x = tp.r.GetRight();
				pt2.y = pt1.y;
				
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
				
				// labels
				char buf[20];
				int j = abs (i);
				sprintf (buf, "%.*f",2, tp.maxheight);
				Port.SelectAFont (tp.edgefont); // 16/4/96
				int l = Port.TextWidth (buf, strlen (buf)) + Port.MaxCharWidth();

				VRect r;
				if (tp.style & TS_LEFT)
				{
					r.SetLeft(pt2.x);
					r.SetRight (pt2.x + Port.MaxCharWidth());
					r.SetTop(pt1.y);
					r.SetBottom (pt1.y + Port.FontHeight ());
					r.Offset (-Port.MaxCharWidth()/2, 0);
					Port.DrawText (r, "0", 1, 'c', FALSE);
					
					r.Offset (-tp.r.GetWidth(), 0);
					r.SetRight(r.GetLeft() + l);
//					r.Offset (-l/2, 0);
					Port.DrawText (r, buf, strlen(buf), 'c', FALSE);
				}
				else
				{
					r.SetLeft(pt1.x);
					r.SetRight (pt1.x + Port.MaxCharWidth());
					r.SetTop(pt1.y);
					r.SetBottom (pt1.y + Port.FontHeight ());
					r.Offset (-Port.MaxCharWidth()/2, 0);
					Port.DrawText (r, "0", 1, 'c', FALSE);
					
					r.Offset (tp.r.GetWidth(), 0);
					r.SetRight(r.GetLeft() + l);
					r.Offset (-l/2, 0);
					Port.DrawText (r, buf, strlen(buf), 'c', FALSE);
				}
		
								
				
				
				
				// Draw ticks
				int num_ticks = tp.maxheight/bar;
				int tick_height = Port.FontHeight()/2;
				if (tp.style & TS_LEFT)
				{
					// Draw from right to left
					for (int i = 0; i <= num_ticks; i++)
					{
						pt1.x = tp.r.GetRight() - scalebar * i;
						pt1.y = tp.r.GetBottom() + scalebar_space - 1;
						pt2.x = pt1.x;
						pt2.y = pt1.y - tick_height;
						Port.DrawLine (pt1, pt2);
					}
				}
				else
				{
					// Draw from left to right
					for (int i = 0; i <= num_ticks; i++)
					{
						pt1.x = tp.r.GetLeft() + scalebar * i;
						pt1.y = tp.r.GetBottom() + scalebar_space - 1;
						pt2.x = pt1.x;
						pt2.y = pt1.y - tick_height;
						Port.DrawLine (pt2, pt1);
					}
				}
				
				Port.WidePen (w);
			}
			else
			{

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
				int l = Port.TextWidth (buf, strlen (buf)) + Port.MaxCharWidth();
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
	#ifdef Macintosh
				r.Offset (0, -2);
	#else
				r.Offset (0, -5);
				Port.SetMode (vMERGE);
	#endif
				Port.DrawText (r, buf, strlen (buf), 'c', FALSE);
	#endif
			}
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
		if ( (style & TS_ANGLE) == TS_ANGLE)
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
// Set leaf labels
void Tree::LabelLeaves (Labels &l)
{
	for (int i = 0; i < Leaves; i++ )
	{
//		if (Nodes[i]) Nodes[i]->SetLabel (l.GetLabel (i + 1));
		// 18/8/97
		if (Nodes[i]) 
			Nodes[i]->SetLabel (l.GetLabel (Nodes[i]->GetLabelNumber ()));
	}
	  SetFlag (TF_HAS_LABELS, True);
}
//------------------------------------------------------------------------------
// Return least upper bound of b in tree
#ifdef UseMikesBBitset
 NodePtr Tree::LUB(BBitset &b)
#else
 NodePtr Tree::LUB (Bitset &b)
#endif
{
	// ensure
#ifdef UseMikesBBitset
	if (!b.nonEmpty())	// it's faster to check that a set is non-empty than empty.
		return NULL;
#else
	if (b.IsNull ())
		return NULL;
#endif
	else
	{
	NodePtr p = Root;
	NodePtr q = p;
	BooleaN found = False;

	while (q && !found)
	{
		switch (q->ClusterRelationship(b))
		{
#ifdef UseMikesBBitset
			case IDENTITY:
               	// we've found it exactly
				p = q;
				found = True;
				break;
			case SUBSET:
				// q's ancestor was lub
				p = q->GetAnc ();
                    found = True; 
				break;
			case DISJOINT:
               	// look at sibling
				p = q;
				q = q->GetSibling ();
				break;
			case SUPERSET:
               	// q is too big, go left up tree
				p = q;
				q = q->GetChild();
				break;
			case OVERLAPPING:
				// q's ancestor was lub
				p = q->GetAnc ();
                    found = True;
				break;
#else
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
#endif
			default:
				break;
		}
	}

	  return (p);
	}
}



#ifdef VCLASS
//------------------------------------------------------------------------------
NodePtr Tree::PtOnNode (PoinT &pt, BooleaN left)
{
	CurNode = NULL;
	mousePt.x = pt.x;
	mousePt.y = pt.y;
	findNode (Root, left);
	return (CurNode);
}

//------------------------------------------------------------------------------
void Tree::findNode (NodePtr p, BooleaN left)
{
	if (p)
	{
		if (p->PtOnNode (mousePt, left)) CurNode = p;
		if (!CurNode) findNode (p->GetChild(), left);
		if (!CurNode) findNode (p->GetSibling(), left);
	}
}
#endif

//------------------------------------------------------------------------------
void Tree::clearNode (NodePtr p, int f)
{
	if (p)
	{
		clearNode (p->GetChild(), f);
		p->SetFlag (f, False);
		clearNode (p->GetSibling(), f);
	}
}

//------------------------------------------------------------------------------
void Tree::ClearFlags (int f)
{
	clearNode (Root, f);
}

//------------------------------------------------------------------------------
void Tree::ultratrav (NodePtr p)
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
BooleaN Tree::IsUltrametric ()
{

	aflag = True;
	Root->SetHeight (0.0);
	tp.maxheight = 0;
	getHeights (Root);
	ultratrav (Root);
	return (aflag);
}

//------------------------------------------------------------------------------
// 14/6/93
// Permute the leaf labels of tree, assumes tree is not pruned.
// Calls Update to rebuild clusters, etc.
void Tree::PermuteLabels (Permutation &p)
{
	for (int i = 0; i < Leaves; i++)
	{
		Nodes[i]->SetLabelNumber (p.P[i] + 1);
		Nodes[i]->SetLeafNumber (p.P[i] + 1);
	}
	SetFlag (TF_VALID, False);
	Update ();
}


//------------------------------------------------------------------------------
// Write tree in additive binary coded format required for Siddall's DANRAN program
void Tree::WriteDANRAN1 (ostream &f)
{
	Line = new char[Leaves + Internals + 11];

	for (int i = 0; i < Leaves; i++)
	{
   	for (int i = 0; i < Leaves + Internals; i++) Line[i] = '0';
		Line[Leaves + Internals] = '\0';

		NodePtr p = Nodes[i];

      // Ensure we have a valid name of no more than 9 characters
		f << ExportString (p->GetLabel(), 9) << ' ';

      // Additive code for this node
		Line[p->GetIndex()] = '1';
		NodePtr q = p;
		do {
			q = q->GetAnc();
			Line[q->GetIndex()] = '1';
		} while (q != Root);

      f << Line << EOLN;
	}

   delete [] Line;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
}


//------------------------------------------------------------------------------
float Tree::GetMaxHeight ()
{
	tp.maxheight = 0.0;
	Root->SetHeight(0.0);
	getHeights (Root);
	return tp.maxheight;
}


//------------------------------------------------------------------------------
// From CPTREE.PAS
// Rotate children of binary node
void Tree::RotateDescendants (NodePtr p)
{
	p->RotateDescendants();
}


//------------------------------------------------------------------------------
void Tree::flagTrav (NodePtr p)
{
	if (p)
	{
		if (p != CurNode)
			p->SetFlag (avalue, aflag);
		flagTrav (p->GetChild());
		if (p != CurNode)
			flagTrav (p->GetSibling());
	}
}

//------------------------------------------------------------------------------
// Set flag for all nodes above p
void Tree::SetFlagAbove (NodePtr p, int flag, BooleaN on)
{
	CurNode = p;
	avalue = flag;
	aflag = on;
	flagTrav (p);
}


//------------------------------------------------------------------------------
void Tree::RandomiseBranchLengths ()
{
	Permutation p (Leaves + Internals);
	p.Permute ();

	float *temp;
	temp = new float[Leaves + Internals];
	int i;
	for (i = 0; i < Leaves + Internals; i++)
		temp[i] = Nodes[i]->GetEdgeLength();

	// Fix for root
	int there = Root->GetLabelNumber() - 1;
	int j = 0;
	while (p.P[j] != there) j++;

	p.P[j] = p.P[there];
	p.P[there] = there;

	for (i = 0; i < Leaves + Internals; i++)
		Nodes[i]->SetEdgeLength(temp[p.P[i]]);

   delete [] temp;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
}

//------------------------------------------------------------------------------
void Tree::MakeRootBinary ()
{
	if (!Root->IsBinary())
	{
		NodePtr SplitNode = NewNode();
		if (SplitNode)
		{
			Internals++;
			SplitNode->SetChild (Root->GetChild()->GetSibling());
			Root->SetChildSibling (SplitNode);
			SplitNode->SetAnc (Root);
			Root->SetDegree (2);
			NodePtr q = SplitNode->GetChild();
			while (q)
			{
				q->SetAnc (SplitNode);
				SplitNode->IncrementDegree();
				q = q->GetSibling();
			}
		}
	}
}



// Glasgow

//------------------------------------------------------------------------------
// COMPONENT code
BooleaN Tree::MustSwap (NodePtr L, NodePtr R, LadderType Ladder)
{
	BooleaN result = False;

	switch (Ladder)
	{
		case Ladder_Default:
			break;
		case Ladder_Left:
			result = (BooleaN) (L->GetWeight () < R->GetWeight ());
			break;
		case Ladder_Right:
			result = (BooleaN) (L->GetWeight () > R->GetWeight ());
			break;
		default:
			break;
	}
	return result;
}

//------------------------------------------------------------------------------
// COMPONENT code
void Tree::SortDescendants (NodePtr node, LadderType Ladder)
{
	NodePtr head = node->GetChild ();
	NodePtr tail = head;
	while (tail->GetSibling () != NULL)
	{
		NodePtr p = tail->GetSibling ();
		if (MustSwap (head, p, Ladder))
		{
			tail->SetSibling (p->GetSibling ());
			p->SetSibling (head);
			head = p;
			p->GetAnc()->SetChild (p);
		}
		else
		{
			NodePtr q = head;
			NodePtr r = q->GetSibling ();
			while (MustSwap (p, r, Ladder))
			{
				q = r;
				r = q->GetSibling ();
			}
			if (p == r)
				tail = p;
			else
			{
				tail->SetSibling (p->GetSibling ());
				p->SetSibling (r);
				q->SetSibling (p);
			}
		}
	}
}

//------------------------------------------------------------------------------
void Tree::ladderTraverse (NodePtr p)
{
	if (p)
	{
		ladderTraverse (p->GetChild());
		if (p != CurNode)
			ladderTraverse (p->GetSibling());
		if (!p->IsLeaf ())
			SortDescendants (p, Laddering);
	}
}


//------------------------------------------------------------------------------
void Tree::Ladderise (LadderType Ladder)
{
	if (Ladder != Ladder_Default)
	{
		CurNode = Root;
		Laddering = Ladder;
		ladderTraverse (Root);
	}
}

// Glasgow 22/11

//------------------------------------------------------------------------------
// COMPONENT code
// Mark nodes on path between p and Root inclusive
void Tree::MarkPath (NodePtr p)
{
	NodePtr q = p;
	while (q != NULL)
	{
		q->SetFlag (NF_MARKED, True);
		q = q->GetAnc ();
	}
}


//------------------------------------------------------------------------------
NodePtr Tree::FirstDescendant (NodePtr p)
{
	CurNode = p->GetChild ();
	return CurNode;
}

//------------------------------------------------------------------------------
NodePtr Tree::NextDescendant ()
{
	CurNode = CurNode->GetSibling ();
	return CurNode;
}

//------------------------------------------------------------------------------
// Return next desc of p that is not on the path OG..Root.
NodePtr Tree::nextNonOGDesc ()
{
	NodePtr q = NextDescendant ();

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

// 6 May 1999
// Fix rerooting bug that would:
// 1. sometimes produce apical polytomy
// 2. eat basal branch on some topologies
 
#define USE_NEW_ROOT 1

//------------------------------------------------------------------------------
void Tree::listOtherDesc (NodePtr p)
{
	NodePtr q = FirstDescendant (p);
	if (q->IsFlag (NF_MARKED))
		q = nextNonOGDesc ();

#if USE_NEW_ROOT
	if ((AddThere->IsLeaf()) || (AddThere->GetChild()))
    {
		AddThere->SetSibling (q);
		q->SetAnc (AddThere->GetAnc ());
    }
    else
	{
		AddThere->SetChild (q);
		q->SetAnc (AddThere);
	}
#else

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
#endif
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
void Tree::ReRoot (NodePtr OG_PTR)
{
	if ((OG_PTR == NULL) || (OG_PTR == Root)) return; // Ensure

	SetFlag (TF_ROOTED, True);

	if (OG_PTR->GetAnc () == Root) return;  // No need to reroot


	MarkPath (OG_PTR);

#if USE_NEW_ROOT
	NodePtr IG_PTR = NewNode ();
	IG_PTR->SetAnc (OG_PTR->GetAnc());


/*    bool breakEdge = true;
    
    if (breakEdge)
    {
		// Split outgroup edge length among out and ingroup (does this require things to be binary?)
    	int half = OG_PTR->GetEdgeLength() / 2;
	    IG_PTR->SetEdgeLength (half);
    	OG_PTR->SetEdgeLength (OG_PTR->GetEdgeLength() - half);
    }
*/

	AddThere = IG_PTR;
	NodePtr q = OG_PTR->GetAnc ();

	while (q != NULL)
	{
		listOtherDesc (q);
		NodePtr previous_q = q;
		q = q->GetAnc ();
		if (q != NULL)
		{
        	bool makeNew = true;
            if (q == Root)
            {
				makeNew = (q->GetDegree() > 2);

				if (makeNew)
				{
					// 15/2/2000
					// A bug reported by J. Peter Gogarten
					// <gogarten@uconnvm.uconn.edu> and Martyn Kennedy is due
					// to Internals not being incremented. This would cause the
					// Tree Editor to crash in TreeView 1.6.0 if the user rerooted
					// a tree with a basal polytomy.
					Internals++;
				}
				else
				{
					// This is the last branch to add and the root of the original
                    // tree is binary.
                    //
                    //   previous_q     x
                    //           \     /
					//        e1 \   /  e2
					//            q
                    //
                    //
                    //   Hence the edge length below x in the rerooted tree must be
                    //   e1 + e2.

                    NodePtr x = q->GetChild();
                    if (x == previous_q)
                    	x = x->GetSibling();
                    x->SetEdgeLength (x->GetEdgeLength() + previous_q->GetEdgeLength());
				}
			}
            if (makeNew)
            {
				NodePtr p = NewNode ();
				AddThere->SetSibling (p);
				p->SetAnc (AddThere->GetAnc ());
				p->GetAnc ()->IncrementDegree ();
                p->SetEdgeLength (previous_q->GetEdgeLength ());
				p->SetLabel (previous_q->GetLabel ());
				AddThere = p;
            }
		}
	}
	OG_PTR->GetAnc()->SetChild (OG_PTR);
	OG_PTR->SetSibling (IG_PTR);
	Root = OG_PTR->GetAnc ();

	q = Root->GetAnc();
	while (q != NULL)
	{
		NodePtr p = q;
		q = q->GetAnc ();
		delete p;
	}

	Root->SetAnc (NULL);
	Root->SetSibling (NULL);
    Root->SetLabel (NULL);
#else

	NodePtr IG_PTR = NewNode ();
	IG_PTR->SetAnc (OG_PTR->GetAnc());

	AddThere = IG_PTR;
	NodePtr q = OG_PTR->GetAnc ();

    IG_PTR->SetLabel ("IG_PTR");
    OG_PTR->SetLabel ("OG_PTR");
    q->SetLabel ("q");

	// Split outgroup edge length among out and ingroup (does this require things to be binary?)
    int half = OG_PTR->GetEdgeLength() / 2;
    IG_PTR->SetEdgeLength (half);
    OG_PTR->SetEdgeLength (OG_PTR->GetEdgeLength() - half);


	// adjust edge length
/*	if (q)
	{
		// Bug fix 21/12/95
		AddThere->SetEdgeLength (0); // for now
		// Assign bootstrap value!
		AddThere->SetLabel (NULL);
	} */
	while (q != NULL)
	{
		listOtherDesc (q);

		NodePtr t = q;
		q = q->GetAnc ();
		if ((q != NULL) && (q != Root))
		{
			NodePtr p = NewNode ();
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
		NodePtr p = q;
		q = q->GetAnc ();
		delete p;
	}

	Root->SetAnc (NULL);
	Root->SetSibling (NULL);

#endif

	SetFlag (TF_VALID, False);
	Update ();

	Root->SetFlag (NF_MARKED, False);
	OG_PTR->SetFlag (NF_MARKED, False);
}

/*	  Returns pointer to outgroup node in OG_PTR. If outgroup is
	  paraphyletic on tree then return ptr to outgroup's complement.

	  An outgroup is a partition O:O'of the set [1,..,leaves].
	  In order to be valid either O or O' must be a cluster
	  in the tree.
*/
//------------------------------------------------------------------------------
//
// @mfunc NodePtr | Tree | CanRootWith | Returns outgroup node corresponding to outgroup.
//
// @parm Bitset | outgroup | Outgroup.
//
#ifdef UseMikesBBitset
 NodePtr Tree::CanRootWith (BBitset &outgroup)
#else
 NodePtr Tree::CanRootWith (Bitset &outgroup)
#endif
{
	NodePtr OG_PTR = NULL;
#ifdef UseMikesBBitset
	if (outgroup.isEmpty())
		return NULL; // ensure
	if (outgroup.getCard() == 1)
	{
		OG_PTR = Nodes[outgroup[1] - 1]; // root with leaf
	}
#else
	if (outgroup.IsNull())
		return NULL; // ensure
	if (outgroup.Cardinality () == 1)
	{
		OG_PTR = Nodes[outgroup[1] - 1]; // root with leaf
	}
#endif
	else
	{
		NodePtr q = LUB(outgroup);
		if (q == Root)
		{
			// get LUB of complement of outgroup
#ifdef UseMikesBBitset
			BBitset f;
			f.fill(Leaves);
			// set difference!
			BBitset c = f;
			c -= outgroup;
#else
			Bitset f;
			f.FullSet (Leaves);
			// set difference!
			Bitset c = f - outgroup;
#endif
			q = LUB (c);
#ifdef UseMikesBBitset
			if (q->ClusterRelationship (c) == IDENTITY)
#else
			if (q->ClusterRelationship (c) == rdmpIDENTITY)
#endif
			{
				OG_PTR = q;
			}
		}
		else
		{
#ifdef UseMikesBBitset
			if (q->ClusterRelationship (outgroup) == IDENTITY)
#else
			if (q->ClusterRelationship (outgroup) == rdmpIDENTITY)
#endif
			{
				OG_PTR = q;
			}
		}
	}
	return OG_PTR;
}

//------------------------------------------------------------------------------
void Tree::getRadialNodeHeights(NodePtr p)
{
	if (p)
	{
		if (p != Root)
		{
			NodePtr a = p->GetAnc ();
#ifndef DAYLIGHT
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
#else
            p->SetHeight (Leaves - p->GetWeight ());
            p->SetEdgeLength (1.0); // for radial trees
#endif
		}
		if (p->GetHeight() > tp.maxheight)
			tp.maxheight = p->GetHeight();
		getRadialNodeHeights (p->GetChild());
		getRadialNodeHeights (p->GetSibling());
	}
}



//------------------------------------------------------------------------------
// Draw the tree
void Tree::radialDrawTraverse (NodePtr p)
{
	if (p)
	{
		radialDrawTraverse (p->GetChild());
		p->RadialDraw (tp);
		radialDrawTraverse(p->GetSibling());
	}
}

//------------------------------------------------------------------------------
void Tree::centroidTraverse (NodePtr p)
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

//
// @mfunc void | Tree | Centroid | Find the centroid of an unrooted tree.
//
// @parm NodePtr | centre | The centroid.
// @parm BooleaN | edge | True if centroid is an edge.
//
void Tree::GetCentroid (NodePtr &centre, BooleaN &edge)
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
NodePtr stopNode, focusNode;
int offset;
float amin, amax;
float maxRotation;
float rotation;
float xchange, ychange;
float xoffset, yoffset;

float ttop, tleft, tright, tbottom, scale_real;
float labelleft, labelright, labeltop, labelbottom;
int rotatingLabels;

void SetFocalNode (NodePtr p, NodePtr root);
void traverseFocus (NodePtr p);
float GetDaylight(NodePtr p, NodePtr root);
void RotateWedges(NodePtr p, float skySeg);
void PolarToRect(NodePtr root);
void GetCanopyAbove (NodePtr p);
float GetCanopyBelow(NodePtr p, NodePtr root);
void CalcRelRotation(NodePtr q, float m);
void traverseCanopyAbove (NodePtr p);
void traverseCanopyBelow (NodePtr p);
void traverseRotate (NodePtr q);
void traversePolar (NodePtr p);
void PolarToRect(NodePtr root);
void traverseDeviceTree (NodePtr p);
void traverseCentreTree (NodePtr p);
void traverseTreeExtent (NodePtr p);


//------------------------------------------------------------------------------
void traverseFocus (NodePtr p)
{
	if (p)
    {
        // translate rectangular coordinate so that p is at (0,0) ...
        p->x -= xchange;
        p->y -= ychange;
		// ... then calculate corresponding polar coordinate

		if ((p->x*p->x + p->y*p->y) < 0.001)
			p->SetRadius (0.0);
		else
			p->SetRadius (sqrt (p->x*p->x + p->y*p->y));
		float a = 0.0;
		if (p->GetRadius () > epsilon)
		{
 //			float xtest = p->x / p->GetRadius();
 //			if (xtest > 1.0 || xtest < -1.0)
 //				Message (MSG_ERROR, "acos");
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
void SetFocalNode (NodePtr p, NodePtr root)
{
	p->GetXY (focus);
    xchange = p->x;
    ychange = p->y;
    traverseFocus (root);
}

//------------------------------------------------------------------------------
// Calculate rotation angle of vector leading to node q relative to vector leading from focal node
//	to root of current subtree.
void CalcRelRotation(NodePtr q, float m)
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
void traverseCanopyAbove (NodePtr p)
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
void GetCanopyAbove (NodePtr p)
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
void traverseCanopyBelow (NodePtr p)
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
float GetCanopyBelow(NodePtr p, NodePtr root)
{
	stopNode = p;
	amin = amax = 0.0;
    NodePtr s = p->GetAnc();
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
float GetDaylight(NodePtr p, NodePtr root)
{
	int n = 0;
	float sky = 2 * M_PI;

    NodePtr q = p->GetChild();
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
void traverseRotate (NodePtr q)
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
void RotateWedges(NodePtr p, float skySeg)
{
	NodePtr	q;
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
void traversePolar (NodePtr p)
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
void PolarToRect(NodePtr root)
{
    stopNode = root;
    traversePolar (root);
}


//------------------------------------------------------------------------------
void Tree::traverseDaylight (NodePtr p)
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
void traverseTreeExtent (NodePtr p)
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
			if (rotatingLabels)
			{
				float dx = fabs (p->x - p->GetAnc()->x);
				float dy = fabs (p->y - p->GetAnc()->y);
				float radius = sqrt (dx*dx + dy*dy);
				float a = 0.0;
				if (radius > epsilon)
				{
					a = acos (dx/radius);
				}
				if (p->x < p->GetAnc()->x)
					labelleft = MIN (labelleft, p->x - (float)w * cos (a));
				else
					labelright = MAX (labelright, p->x + (float)w * cos (a));
				if (p->y < p->GetAnc()->y)
					labeltop = MIN (labeltop, p->y - (float)w * sin (a));
				else
					labelbottom = MAX (labelbottom, p->y + (float)w * sin (a));
			}
			else
			{
				if (p->x < p->GetAnc()->x)
					labelleft = MIN (labelleft, p->x - w);
				else
					labelright = MAX (labelright, p->x + w);
			}
        }
        traverseTreeExtent (p->GetChild());
       	traverseTreeExtent (p->GetSibling ());
    }
}

//------------------------------------------------------------------------------
void traverseCentreTree (NodePtr p)
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
void traverseDeviceTree (NodePtr p)
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
//------------------------------------------------------------------------------
// @mfunc void | Tree | RadialDraw | Draw a radial tree.
//
// @parm VRect | r | Rectangle tree will be drawn in.
// @parm VFontPtr | font | Font ot be used for labels.
// @parm unsigned int | style | Flags determining style of tree.
//
void Tree::RadialDraw (VRect r, VFontPtr font, unsigned int style, VFontPtr EdgeFont)
{
	// To make a nicer picture, root the tree at it's centre of gravity.
	NodePtr p;
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
		NodePtr toDelete = Root->GetChild();
		NodePtr toKeep = toDelete->GetSibling();
		NodePtr tmp;
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
    int maxPasses = 5;
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
	tleft 		= 1000.0;
	tright 		= 0.0;
	ttop 		= 1000.0;
	tbottom 	= 0.0;
	labelleft 	= 1000.0;
	labelright 	= 0.0;
	labeltop 	= 1000.0;
	labelbottom = 0.0;
	rotatingLabels = (tp.style & TS_ROTATE_LABELS);
	traverseTreeExtent (Root);


	// Allow for space occupied by labels
	int fontHeight = Port.FontHeight();
	if (tp.style & TS_ROTATE_LABELS)
	{
		// rotated labels
		treeRect.SetTop (treeRect.GetTop() + (ttop - labeltop));
		treeRect.SetBottom (treeRect.GetBottom () - (labelbottom - tbottom));
	}
	else
		// allow for font height
		treeRect.Inset (0, fontHeight);

	treeRect.SetLeft (treeRect.GetLeft() + (tleft - labelleft));
	treeRect.SetRight (treeRect.GetRight() - (labelright - tright));
	w = treeRect.GetWidth();
	h = treeRect.GetHeight();

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
	if (tp.style & TS_ROTATE_LABELS)
		yoffset = r.GetTop () + h/2 + (ttop - labeltop);
	else
		yoffset = r.GetTop() + h/2 + fontHeight;
	traverseDeviceTree (Root);

#ifdef SCALE
	// Draw a scale bar to show amount of evolutionary change
	if (IsFlag (TF_HAS_EDGES))
   {

		float maxXYdist = 0.0; // max distance in device coordinates
		float maxdist = 0.0;   // actual evolutionary distance
		for (i = 0; i < Leaves; i++)
		{
			NodePtr p = Nodes[i];
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
		float bar = pow ((float)10, (float)i);
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
		int l = Port.TextWidth (buf, strlen (buf)) + Port.MaxCharWidth ();
		if (l > scalebar)
      	pt2.x = pt1.x + l;
		VRect r;
		r.SetLeft(pt1.x);
		r.SetRight (pt2.x);
		r.SetBottom(pt1.y);
		r.SetTop (r.GetBottom() - Port.FontHeight ());
#ifdef Macintosh
		r.Offset (0,2);

#else
		r.Offset (0, -5);
#endif
   		Port.DrawText (r, buf, strlen (buf), 'c', FALSE);
 	}
#endif



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
// @mfunc void | Tree | RadialDraw | Draw a radial tree.
//
// @parm VRect | r | Rectangle tree will be drawn in.
// @parm VFontPtr | font | Font ot be used for labels.
// @parm unsigned int | style | Flags determining style of tree.
//
void Tree::RadialDraw (VRect r, VFontPtr font, unsigned int style, VFontPtr EdgeFont)
{
	// To make a nicer picture, root the tree at it's centre of gravity.
	NodePtr p;
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

	int i;
	// Actual drawing might be quite small due to angles of branches, so
	// find most distant leaf from centre and rescale accordingly

	float maxdist = 0.0;
	PoinT ptRoot;
	Root->GetXY (ptRoot);
	for (i = 0; i < Leaves; i++)
	{
		PoinT ptLeaf;
		Nodes[i]->GetXY (ptLeaf);

		// Use floats as 16 bit integers overflow.
		float a = ptLeaf.x - ptRoot.x;
		float b = ptLeaf.y - ptRoot.y;
		float c = a*a + b*b;
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
	NodePtr nodeLeft = NULL;
	NodePtr nodeRight = NULL;
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
		maxTop = min (maxTop, ptLeaf.y);
		maxBottom = max (maxBottom, ptLeaf.y);
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
			NodePtr p = Nodes[i];
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
		int l = Port.TextWidth (buf, strlen (buf)) + Port.MaxCharWidth ();
		if (l > scalebar)
      	pt2.x = pt1.x + l;
		VRect r;
		r.SetLeft(pt1.x);
		r.SetRight (pt2.x);
		r.SetBottom(pt1.y);
		r.SetTop (r.GetBottom() - Port.FontHeight ());
#ifdef Macintosh
		r.Offset (0,2);
#else
		r.Offset (0, -5);
#endif
   		Port.DrawText (r, buf, strlen (buf), 'c', FALSE);
 	}
#endif


	// Draw completed tree
	radialDrawTraverse (Root);
}
#endif


// 29/3/96 Tree exporting

/**************************************************************************/
void Tree::Hennig86traverse (NodePtr p)
{
	if (p)
	{
		if (p->IsFlag (NF_LEAF))
		{
			*treeStream << (p->GetLabelNumber() - 1) << " ";
		}
		else
		{
			*treeStream << "(";
		}
		Hennig86traverse (p->GetChild());
		if (p->GetSibling() == NULL && p != Root)
		{
			*treeStream << ")";
		}
		Hennig86traverse (p->GetSibling());
	}
}


void Tree::WriteHennig86 (ostream &f)
{
	treeStream = &f;
	Ladderise (Ladder_Right);
	Hennig86traverse (Root);
}

void Tree::WritePHYLIP (ostream &f)
{
	treeStream = &f;
	traverse (Root);
}

#ifdef RECONCILE

//-----------------------------CopyOfSubTree------------------------------------
/*
	  Return a copy of the subtree in rooted at RootedAt.
     The function result is the root of the subtree.

						 Tree

					 x---y    b---c              x'--y'
                 \        \                  \
						\        \                  \
		  RootedAt ->w--------a       ==>        CopyOfSubTree -> NULL
                    \
							\

	  which corresponds to copying

					 x'    y'             x     y b     c
					  \   /                \   /   \   /
						\ /                  \ /     \ /
						 w'       from        w       a
                                         \     /
														\   /
														 \ /
														  v


*/

//------------------------------------------------------------------------------
// 21/4/96 Reconciled trees
// This code uses the RECONCILE define to
// declare Tree as a friend of Node, so that I can access
// the child and sib fields of Node, which is essential for this
// code to work.
void Tree::copyTraverse (NodePtr p1, NodePtr &p2)
{
	if (p1)
	{
		p2 = NewNode ();
		p1->Copy (p2);
		// Debugging for now
//		p2->SetFlag (NF_HIGHLIGHT, True);

		// Note the call to p2->child, not p2->GetChild(). Calling the field
		// is essential because calling GetChild merely passes a temporary
		// copy of the child, hence we are not actually creating a child of p2.
		// We can access child directly by making Tree a friend of Node (see
		// Node.h).
		copyTraverse (p1->GetChild(), p2->child);
		if (p2->GetChild())
			p2->GetChild()->SetAnc (p2);

		// Ensure we don't copy RootedAt sibling. If the sibling is NULL then
		// we won't anyway, but this line ensures this for all cases.
		if (p1 != CurNode)
			copyTraverse (p1->GetSibling(), p2->sib);  // note sib
		if (p2->GetChild ())
		{
			NodePtr q = p2->GetChild()->GetSibling();
			while (q)
			{
				q->SetAnc (p2);
				q = q->GetSibling();
			}
		}
	}
}

//------------------------------------------------------------------------------
NodePtr Tree::CopyOfSubtree (NodePtr RootedAt)
{
	CurNode = RootedAt;   // Store this to avoid copying too much of the tree
	NodePtr placeHolder;  // This becomes the root of the subtree
	copyTraverse (CurNode, placeHolder);
	return placeHolder;
}

#endif // RECONCILE



//------------------------------------------------------------------------------
// 15/5/96
int Tree::NodesBetween (NodePtr descendant, NodePtr ancestor)
{
	int count = 0;
	NodePtr p = descendant->GetAnc();
	while (p != ancestor)
	{
		count++;
		p = p->GetAnc();
	}
	return count;
}

#ifdef NODE_DEBUG
//------------------------------------------------------------------------------
void Tree::memDumptraverse (NodePtr p, ofstream &f)
{
	if (p)
	{
		p->MemDump (f);
		f << endl;
		memDumptraverse (p->GetChild (), f);
		memDumptraverse (p->GetSibling (), f);
	}
}

//------------------------------------------------------------------------------
// Dump the tree to cout for debugging
void Tree::MemDump (ofstream &f)
{
	f << "Tree = " <<  " (memory dump)" << endl;
	memDumptraverse (Root, f);
}

#endif



//------------------------------------------------------------------------------
// Create a star tree with n leaves
void Tree::StarTree (int n)
{
	Leaves = n;
	Internals = 1;

	Root = NewNode();
	Root->SetWeight (n);
	Root->SetDegree (n);
#ifdef UseMikesBBitset
	Root->cluster.fill(n);
#else
	Root->cluster.FullSet (n);
#endif

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
		NodePtr q = NewNode ();
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
#ifdef UseMikesBBitset
 void Tree::checkSiblings (NodePtr q, BBitset &b)
#else
 void Tree::checkSiblings (NodePtr q, Bitset &b)
#endif
{
	NodePtr r = q;
	NodePtr s = q->sib;
	NodePtr t = q->anc;

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
#ifdef UseMikesBBitset
 int Tree::AddCluster (BBitset &b, char *label)
#else
int Tree::AddCluster (Bitset &b, char *label)
#endif
{
	Error = 0;
#ifdef UseMikesBBitset
	BBitset s;
	s = b;
	int n = s.getCard();
#else
	Bitset s;
	s.NullSet();
	s += b;
	int n = s.Cardinality();
#endif

#ifdef UseMikesBBitset
	if ((n > 1) && (n < Root->cluster.getCard()))
#else
	if (n > 1 && n < Root->cluster.Cardinality())
#endif
	{
		// non trivial set
		NodePtr p = Root;
		NodePtr q = p->child;
		BooleaN Finished = False;
#ifdef UseMikesBBitset
		SetRels r;
#else
		SetRelations r;
#endif

		// locate insertion point
		while (q && !Finished)
		{
#ifdef UseMikesBBitset
			r = s.rel(q->cluster);
#else
			r = s.Relationship (q->cluster);
#endif
			switch (r)
			{
#ifdef UseMikesBBitset
				case IDENTITY:
#else
				case rdmpIDENTITY:
#endif
					Finished = True;
					Error = 1;
					break;

#ifdef UseMikesBBitset
				case SUBSET:
#else
				case rdmpSUBSET:
#endif
					p = q;
					q = q->child;
					break;

#ifdef UseMikesBBitset
				case DISJOINT:
#else
				case rdmpDISJOINT:
#endif
					p = q;
					q = q->sib;
					break;

#ifdef UseMikesBBitset
				case SUPERSET:
#else
				case rdmpSUPERSET:
#endif
					Finished = True;
					break;

#ifdef UseMikesBBitset
				case OVERLAPPING:
#else
				case rdmpOVERLAPPING:
#endif
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
			NodePtr nunode = NewNode ();
			Internals++;
			nunode->weight = n;
			nunode->cluster += s;
					
			if (label)
				nunode->SetLabel (label);


			// Insert node in tree
			switch (r)
			{
#ifdef UseMikesBBitset
				case SUBSET:
#else
				case rdmpSUBSET:
#endif
					p->child =  nunode;
					nunode->anc = p;
					break;

#ifdef UseMikesBBitset
				case DISJOINT:
#else
				case rdmpDISJOINT:
#endif
					p->sib = nunode;
					nunode->anc = p->anc;
					break;

#ifdef UseMikesBBitset
				case SUPERSET:
#else
				case rdmpSUPERSET:
#endif
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

//------------------------------------------------------------------------------
// 19/9/96
// Support for tree editor
void Tree::LadderAbove (NodePtr p, LadderType Ladder)
{
	Laddering = Ladder;
	CurNode = p;
	ladderTraverse (p);
}

//------------------------------------------------------------------------------
NodePtr Tree::RemoveNode (NodePtr Node)
{
	NodePtr result = NULL;

	if (Node == Root)
		return result;

	NodePtr p;
	NodePtr Ancestor = Node->GetAnc();

	if (Ancestor->IsBinary ())
	{
		// ancestor is binary, so remove node and its ancestor
		if (Node->IsTheChild ())
			p = Node->GetSibling();
		else
			p = Ancestor->GetChild();
		NodePtr q = Ancestor->GetAnc();
		p->SetAnc (q);
		if (q != NULL)
		{
			if (Ancestor->IsTheChild())
				q->SetChild (p);
			else
			{
				NodePtr r = Ancestor->LeftSiblingOf ();
				r->SetSibling (p);
			}
			p->SetSibling (Ancestor->GetSibling());
			result = p;
		}
		else
		{
			// Ancestor is the root
			Root = p;
			p->SetSibling (NULL);
			result = p;
		}
		delete Ancestor;
		Internals--;
		if (Node->IsLeaf())
			Leaves--;
		Node->SetAnc (NULL);
		Node->SetSibling (NULL);
	}
	else
	{
		// polytomy, just remove node
		NodePtr q;
		if (Node->IsTheChild())
		{
			Ancestor->SetChild (Node->GetSibling());
			q = Node->GetSibling ();
		}
		else
		{
			q = Node->LeftSiblingOf ();
			q->SetSibling (Node->GetSibling ());
		}
		Node->SetSibling (NULL);
		Node->SetAnc (NULL);
		if (Node->IsLeaf())
			Leaves--;
		Ancestor->SetDegree (Ancestor->GetDegree() - 1);
		result = q;
	}
	return result;
}

//------------------------------------------------------------------------------
// 31/10/96
// GeneTree: support for "pruning" trees using flags
#ifdef UseMikesBBitset
 void Tree::PruneByMarking (BBitset &keep)
#else
void Tree::PruneByMarking (Bitset &keep)
#endif
{
	ClearFlags (NF_MARKED);

	for (int i = 0; i < Leaves; i++)
	{
		NodePtr p = Nodes[i];
#ifdef UseMikesBBitset
		if (!keep.contains(p->GetLabelNumber()))
#else
		if (!keep.IsElement (p->GetLabelNumber()))
#endif
		{
			p->SetFlag (NF_MARKED, True);
			NodePtr q = p->GetAnc ();
			while (q->IsFlag (NF_MARKED))
			{
				q = q->GetAnc();
			}
			q->SetFlag (NF_MARKED, True);
		}
	}
}

//------------------------------------------------------------------------------
// 31/10/96
int Tree::UnmarkedNodesBetween (NodePtr descendant, NodePtr ancestor)
{
	int count = 0;
	NodePtr p = descendant->GetAnc();
	while (p != ancestor)
	{
		if (!p->IsFlag (NF_MARKED))
			count++;
		p = p->GetAnc();
	}
	return count;
}


// Home 4/5/97



//------------------------------------------------------------------------------
void Tree::DrawInteriorEdge (NodePtr p)
{
	NodePtr r = p->GetAnc ();
	int stop = r->GetHeight() * tp.scale;
	if (p->IsTheChild ())
	{
		// Visiting ancestor for the first time, so draw the
		// end symbol
		if (r == Root)
		{
			if (IsRooted ())
				Line[stop] = TEE;   // ´
			else
				Line[stop] = VBAR;  // ³
		}
		else
		{
			Line[stop] = TEE;      // ´
		}


		// Draw branch itself
		if (r != Root)
		{
			// Line
			int start = r->GetAnc()->GetHeight() * tp.scale;
			for (int i = start + 1; i < stop; i++)
			{
				Line[i] = HBAR; // Ä
			}
			// Start symbol
			if (start == stop)
				Line[start] = VBAR;     // ³
			else if (r->IsTheChild ())
				Line[start] = LEFT;     // Ú
			else if (r->GetSibling ())
				Line[start] = SIB;      // Ã
			else Line[start] = RIGHT;  // À

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
	if (r->GetLabel() && p->IsTheChild ()) // 10/7/98
		*treeStream  << r->GetLabel();
	 *treeStream  << EOLN;

	// Clear the line for the next pass
	int j = MAXLEAVES + MAXNAMELENGTH;
	for (int i = 0; i < j; i++)
		Line[i] = ' ';
}


//------------------------------------------------------------------------------
void Tree::DrawPendantEdge (NodePtr p)
{
		Node *q = p->GetAnc();
		int start = q->GetHeight() * tp.scale;
		int stop = p->GetHeight() * tp.scale;
		char	symbol;

		// Draw line between p and its ancestor
		int i;
		for (i = start + 1; i <= stop; i++)
			Line[i] = (char)HBAR;      // Ä

		// Find appropriate symbol for link to ancestor
		if (p == q->GetChild())
		{
			symbol = (char)LEFT;       // Ú
		}
		else
		{
			// p is a sibling
			if (p->GetSibling())
				symbol = (char)SIB;     // Ã
			else symbol = (char)RIGHT; // À
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

void Tree::Fill_In_Ancestors (NodePtr p)
{

	NodePtr q = p->GetAnc ();
	NodePtr r = p;
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
void Tree::drawAsTextTraverse (NodePtr p)
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

//------------------------------------------------------------------------------
// Steel and Penny's path difference metric
float Tree::PathDifference (Tree &t, BooleaN useEdgeLength)
{
	float sum = 0.0;
	BooleaN useEdges = BooleaN (useEdgeLength && (BooleaN) IsFlag (TF_HAS_EDGES));
	for (int i = 1; i < Leaves; i++)
	{
		for (int j = 0; j < i; j++)
		{
			// Tree 1 (this tree)
			float pathT1 = PathLengthBetweenNodes (i, j, useEdges);
			// Tree 2
			float pathT2 = t.PathLengthBetweenNodes (i, j, useEdges);
			
			float d = pathT1 - pathT2;
			sum += (d * d);
			
//			sum += pow ((float)abs (pathT1 - pathT2), 2.0); // this is the problem!!!!
		}
	}
	return sqrt (sum);
}

//------------------------------------------------------------------------------
float Tree::PathLengthBetweenNodes (int i, int j, BooleaN useEdgeLengths)
{
	return PathLengthBetweenNodes (Nodes[i], Nodes[j], useEdgeLengths);
}

//------------------------------------------------------------------------------
float Tree::PathLengthBetweenNodes (NodePtr p, NodePtr q, BooleaN useEdgeLengths)
{
	float path = 0.0;
	NodePtr m = p->MRCA (q);
	NodePtr r = p;
	while (r != m)
	{
		if (useEdgeLengths)
			path += r->GetEdgeLength ();
		else
			path += 1.0;
		r = r->GetAnc ();
	}
	r = q;
	while (r != m)
	{
		if (useEdgeLengths)
			path += r->GetEdgeLength ();
		else
			path += 1.0;
		r = r->GetAnc ();
	}
	// If counting edges but ignoring their lengths and
	// the tree is unrooted then ignore the root node
	if (!IsRooted () && !useEdgeLengths)
		path -= 1.0;
		
	return (path);
}

//------------------------------------------------------------------------------
void Tree::MarkPathij (NodePtr i, NodePtr j)
{
	i->MarkPathTo (j, True);
}

//------------------------------------------------------------------------------
void Tree::UnMarkPathij (NodePtr i, NodePtr j)
{
	i->MarkPathTo (j, False);
}

//------------------------------------------------------------------------------
// Mark nodes on path between p and Root inclusive
void Tree::UnMarkPath (NodePtr p)
{
	NodePtr q = p;
	while (q != NULL)
	{
		q->SetFlag (NF_MARKED, False);
		q = q->GetAnc ();
	}
}

//------------------------------------------------------------------------------
//18/8/97
// Prune tree. Note this unlike COMPONENT 2.0 code, tree is rebuilt so that
// node list (Nodes[]) is continuous rather than fragmented. This should make
// handling pruned trees a lot easier.
#ifdef UseMikesBBitset
 void Tree::Prune (BBitset &ToBePruned)
#else
void Tree::Prune (Bitset &ToBePruned)
#endif
{
	int i, j;
	int OriginalLeaves = Leaves;

	// Remove leaves in b
	for (i = 0; i < OriginalLeaves; i++)
	{
#ifdef UseMikesBBitset
		if (ToBePruned.contains(i + 1))
#else
		if (ToBePruned.IsElement (i + 1))
#endif
			RemoveNode (Nodes[i]);
	}
	
	// Reset leaf numbers so that they are <1,...,n>
	i = 0;
	j = 0;
	while (i < OriginalLeaves)
	{
		while (Nodes[i]->GetAnc() == NULL)
			i++;
		j++;
		Nodes[i]->SetLeafNumber (j);
		i++;
	}

	// Rebuild tree data (this rebuilds the node list)
	SetFlag (TF_VALID, False);
	Update ();
}

//------------------------------------------------------------------------------
// 27/1/98
// Compute node depth (i.e, height above root). Based on COMPONENT 2.0 code, 
// assumes count is set to 0 prior to calling code
void Tree::getNodeDepth(NodePtr p)
{
	// Windows code
	if (p)
    {

    	p->SetHeight (count);
		if (!p->IsFlag (NF_MARKED))  // 26/4/98
        	count++;
        getNodeDepth (p->GetChild());
		if (!p->IsFlag (NF_MARKED))	// 26/4/98
        	count--;
        getNodeDepth (p->GetSibling());
    }
    /*
	if (p)
	{
		if (p->GetAnc() && p->IsTheChild())
			count++;
		p->SetHeight (count);
		getNodeDepth (p->GetChild());
		getNodeDepth (p->GetSibling());
		count = p->GetHeight() - 1;

	}*/
}

//------------------------------------------------------------------------------
// 27/1/98
// Compute node depth (i.e, height above root). Based on COMPONENT 2.0 code.
void Tree::GetNodeDepths ()
{
	count = 0;
	getNodeDepth (Root);
}




// 7/10/99
//------------------------------------------------------------------------------
void Tree::hasInternalLabelsTraverse (NodePtr p)
{
	if (p)
    {
		if (!p->IsLeaf() && p->GetLabel())
        	CurNode = p;
		if (!CurNode) hasInternalLabelsTraverse (p->GetChild());
		if (!CurNode) hasInternalLabelsTraverse (p->GetSibling());

    }
}

//------------------------------------------------------------------------------
bool Tree::HasInternalLabels ()
{
	CurNode = NULL;
	hasInternalLabelsTraverse (Root);
	return (bool)(CurNode != NULL);
}

//------------------------------------------------------------------------------
void Tree::MidpointRoot()
{
	NodePtr from;
    NodePtr to;
    float maxPath = 0.0;

    for (int i = 1; i < Leaves; i++)
    {
    	for (int j = 0; j < i; j++)
        {
        	NodePtr p = Nodes[i];
            NodePtr q = Nodes[j];
            float pathLength = 0.0;

            while (p->GetLabelNumber() != q->GetLabelNumber())
            {

               if (p->GetLabelNumber() > q->GetLabelNumber())
               {
                	pathLength += q->GetEdgeLength();
                    q = q->GetAnc();
               }
               else
               {
		           	pathLength += p->GetEdgeLength();
                    p = p->GetAnc();
				}
            }
            if (pathLength > maxPath)
            {
            	maxPath = pathLength;
                from = Nodes[i];
                to = Nodes[j];
            }
		}
    }

    float half = maxPath/2.0;

    NodePtr there;
    float path = 0.0;

    while ((path < half) && from)
    {
    	path += from->GetEdgeLength();
        there = from;
        from = from->GetAnc();
    }

    if (path < half)
    {
    	path = 0.0;
        while ((path < half) && to)
        {
            path += to->GetEdgeLength();
            there = to;
            to = to->GetAnc();
        }
    }


//	ReRoot (there);

	if ((there == Root) || (there->GetAnc() == Root)) return;

#if 0
    if (there->GetAnc() == Root)
    {
    	// We are going to split an edge
        NodePtr IG_PTR = NewNode();
        float excess = path - half;

        if (there->IsTheChild())
        {
        	IG_PTR->SetChild(there);
            Root->SetChild(there->GetSibling());
            there->SetSibling (Root);
            there->SetAnc(IG_PTR);
            Root->SetAnc(IG_PTR);
            there->SetEdgeLength (half);
            Root->SetEdgeLength (excess);
            Root = IG_PTR;
        }
        else
        {
        	NodePtr c = Root->GetChild();
            while (c->GetSibling() != there)
            	c = c->GetSibling();

       		IG_PTR->SetChild(there);
            c->SetSibling (there->GetSibling());
            there->SetSibling (Root);
            there->SetAnc(IG_PTR);
            Root->SetAnc(IG_PTR);
            there->SetEdgeLength (half);
            Root->SetEdgeLength (excess);
            Root = IG_PTR;
		}
    }
    else
    {

#endif
        SetFlag (TF_ROOTED, True);
        MarkPath (there);

        NodePtr IG_PTR = NewNode ();
        IG_PTR->SetAnc (there->GetAnc());


        AddThere = IG_PTR;
        NodePtr q = there->GetAnc ();

        while (q != NULL)
        {
            listOtherDesc (q);
            NodePtr previous_q = q;
            q = q->GetAnc ();
            if (q != NULL)
            {
                bool makeNew = true;
                if (q == Root)
                {
                    makeNew = (q->GetDegree() > 2);

                    if (makeNew)
                    {
                        Internals++;
                    }
                    else
                    {
                        NodePtr x = q->GetChild();
                        if (x == previous_q)
                            x = x->GetSibling();
                        x->SetEdgeLength (x->GetEdgeLength() + previous_q->GetEdgeLength());
                    }
                }
                if (makeNew)
                {
                    NodePtr p = NewNode ();
                    AddThere->SetSibling (p);
                    p->SetAnc (AddThere->GetAnc ());
                    p->GetAnc ()->IncrementDegree ();
                    p->SetEdgeLength (previous_q->GetEdgeLength ());
                    p->SetLabel (previous_q->GetLabel ());
                    AddThere = p;
                }
            }
        }
        there->GetAnc()->SetChild (there);
        there->SetSibling (IG_PTR);
        Root = there->GetAnc ();

        q = Root->GetAnc();
        while (q != NULL)
        {
            NodePtr p = q;
            q = q->GetAnc ();
            delete p;
        }
//	}
	Root->SetAnc (NULL);
	Root->SetSibling (NULL);
    Root->SetLabel (NULL);

    float excess = path - half;
    there->SetEdgeLength (half);
    IG_PTR->SetEdgeLength (IG_PTR->GetEdgeLength() + excess);

	SetFlag (TF_VALID, False);
	Update ();

	Root->SetFlag (NF_MARKED, False);
	there->SetFlag (NF_MARKED, False);

}



