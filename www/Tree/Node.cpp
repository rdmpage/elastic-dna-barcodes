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
*        File: node.h                                                     
*	 Purpose: node class                                                 
*     History: 15 June 1993                                               
* 	                                                                      
***************************************************************************/


// 28/6/94 Compiled with BC++ 4.0

#include <math.h>

#include "Node.h"

//------------------------------------------------------------------------------
// Constructor
Node::Node()
{
	anc 	  	= NULL;
	child  		= NULL;
	sib 	  	= NULL;
	right		= NULL;
	degree 		= 0;
	index  		= 0;
	LabelNumber = 0;
	LeafNumber	= 0;
	weight 		= 0;
	height		= 0.0;
	EdgeLength	= 0.0;
	label  		= NULL;
	value		= 0;
	
	x			= 0.0;
	y			= 0.0;
	angle		= 0.0;
	radius		= 0.0;
    aleft		= 0.0;
    aright		= 0.0;
    
    parent		= NULL;
	affiliation	= NULL;
	basket		= NULL;
	next		= NULL;
	rank		= 0;;

}

#ifdef NODE_DEBUG
//------------------------------------------------------------------------------
// Dump node info to cout (used in debugging)
void Node::Dump ()
{
	cout << setw(3) << index
		<< setw(3) << LeafNumber;
	if (!child) cout << " - ";
	else cout << setw(3) << child->GetIndex ();
	if (!sib) cout << " - ";
	else cout << setw(3) << sib->GetIndex ();
	if (!anc) cout << " - ";
     else cout << setw(3) << anc->GetIndex ();
	cluster.Dump ();
}
#endif

//------------------------------------------------------------------------------
// Label the node with alabel
void Node::SetLabel (const char *alabel)
{
	if (alabel)
	{
    	if (label)
        	delete [] label;
        		// ANSII standard: arrays created with new _ [] must be deleted with []. MAC 14.8.00
        		// [] added by MAC
		label = new char[strlen (alabel) + 1];
		*label = '\0';
		strcpy (label, alabel);
	}
    else
    {
    	if (label)
        	delete [] label;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
    	label = NULL;
    }
}

//------------------------------------------------------------------------------
// Return the rightmost sibling of the node
Node *Node::GetRightMostSibling ()
{
	Node *p = this;

	while (p->sib) p = p->sib;
	return (p);
}


#ifdef VCLASS
//------------------------------------------------------------------------------
// Draw node label
void Node::DrawLabel (BooleaN left)
{
	VRect labelRect;
	char tempbuf[MAXNAMELENGTH + 1];

	int h = Port.FontHeight () / 2;
	if (label)
		strcpy (tempbuf, label);
	else
		sprintf (tempbuf, "%d", LabelNumber);
	if (left)
	{
		labelRect.Load (xy.x,
						xy.y - h,
						xy.x + Port.TextWidth (tempbuf, strlen (tempbuf)),
						xy.y + h);

		if (IsFlag (NF_HIGHLIGHT)) 
			Port.FrameRect (labelRect);

		labelRect.Inset (-2, 0);
		labelRect.Offset (Port.MaxCharWidth ()/2, 0);
		Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'l', FALSE);
		LoadPt (&labelxy, labelRect.GetRight(), xy.y);
	}
	else
	{
		labelRect.Load (xy.x - Port.TextWidth (tempbuf, strlen (tempbuf)),
						xy.y - h,
						xy.x,
						xy.y + h);

		if (IsFlag (NF_HIGHLIGHT)) Port.FrameRect (labelRect);

		labelRect.Offset (-Port.MaxCharWidth ()/2, 0);
		Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'r', FALSE);
		LoadPt (&labelxy, labelRect.GetLeft(), xy.y);
	}
}
#endif

//------------------------------------------------------------------------------
void Node::Draw (treeplot &tp)
{
#ifdef VCLASS

	// Set font to use 16/4/96
	if (IsLeaf ())
		Port.SelectAFont (tp.labelfont);
	else
		Port.SelectAFont (tp.edgefont);

	if ((tp.style & TS_SLANT) == TS_SLANT)
	{
		if (anc) Port.DrawLine (xy, anc->xy);
	}
	if ((tp.style & TS_ANGLE) == TS_ANGLE)
	{
		if (anc)
		{
			PoinT pt1;
			LoadPt (&pt1, anc->xy.x, xy.y);
			Port.DrawLine (xy, pt1);

// Glasgow
			if ((tp.style & TS_LABELEDGES) && (EdgeLength > 1)
				&& !IsLeaf()) // Fred Rickson
			{
				VRect labelRect;
				char tempbuf[MAXNAMELENGTH + 1];

				int h = Port.FontHeight () / 2;
				sprintf (tempbuf, "%d", (int)EdgeLength);
				labelRect.Load (xy.x,
						xy.y - h,
						xy.x + Port.TextWidth (tempbuf, strlen (tempbuf)) + 2,
						xy.y + h);
				labelRect.Offset (Port.MaxCharWidth()/2, 0);
				Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'l', FALSE);
				LoadPt (&labelxy, labelRect.GetRight(), xy.y);
			}
// 21/5/96 Quick test of edge length labels
/*			if (tp.style & TS_LABELEDGES) // change this to using edge lengths...
			{
				VRect labelRect;
				char tempbuf[MAXNAMELENGTH + 1];
				sprintf (tempbuf, "%.4f", EdgeLength);

				Port.SelectAFont (tp.edgefont);
				int h = Port.FontHeight ();
	            // Ensure rectangle is large enough to enclose the edge length label
	            int edge = xy.x - anc->xy.x;
	            int w = MAX (Port.TextWidth (tempbuf, strlen (tempbuf)), abs (edge));
	            if (tp.style & TS_LEFT)
	            {
						labelRect.Load (xy.x - w, xy.y - h, xy.x, xy.y);
	            }
	            else
	            {
	            	labelRect.Load (xy.x, xy.y - h, xy.x + w, xy.y);
	            }
	
	            // Position it in the middle of the edge
	            labelRect.Offset (0, -h/4);
	            Port.SetMode (vMERGE);
	            Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'c', FALSE);
	            if (IsLeaf ())
	            Port.SelectAFont (tp.labelfont);
			}*/

		}
		if (!IsLeaf())
		{
			PoinT pt1, pt2;
			LoadPt (&pt1, xy.x, child->xy.y);
			LoadPt (&pt2, xy.x, child->GetRightMostSibling()->xy.y);
			Port.DrawLine (pt1, pt2);
		}
	}

	// Draw leaf label
	if (IsLeaf() || (tp.style & (TS_LABELINTERNALS | TS_USEINTERNALLABEL)))
	{
		VRect labelRect;
		char tempbuf[MAXNAMELENGTH + 1];

		int h = Port.FontHeight () / 2;

		if (tp.style & TS_USEINTERNALLABEL)
		{
			if (IsLeaf() || label)
				strcpy (tempbuf, label);
			else
				tempbuf[0] = '\0';
		}
		else
		{
			if (label)
			{
				strcpy (tempbuf, label);
			}
			else
			{
				sprintf (tempbuf, "%d", LabelNumber);
			}
		}
		if ((tp.style & TS_LEFT) == TS_LEFT)
		{
			labelRect.Load (xy.x,
							xy.y - h,
							xy.x + Port.TextWidth (tempbuf, strlen (tempbuf)) + 2,
							xy.y + h);

//			if (IsFlag (NF_HIGHLIGHT)) Port.FrameRect (labelRect);

			labelRect.Offset (Port.MaxCharWidth()/2, 0);
			Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'l', FALSE);
			LoadPt (&labelxy, labelRect.GetRight(), xy.y);

			if (IsFlag (NF_HIGHLIGHT) && (tp.style & TS_HIGHLIGHT))
			{
				labelRect.Load (xy.x - h/2,
							xy.y - h/2,
							xy.x + h/2,
							xy.y + h/2);
				Port.FrameOval (labelRect);
				Port.PaintOval (labelRect);
			}
		}
		else
		{
			labelRect.Load (xy.x - Port.TextWidth (tempbuf, strlen (tempbuf)) - 2,
							xy.y - h,
							xy.x,
							xy.y + h);

//			if (IsFlag (NF_HIGHLIGHT)) Port.FrameRect (labelRect);

			labelRect.Offset (-Port.MaxCharWidth()/2, 0);
			Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'r', FALSE);
			LoadPt (&labelxy, labelRect.GetLeft(), xy.y);

			if (IsFlag (NF_HIGHLIGHT) && (tp.style & TS_HIGHLIGHT))
			{
				labelRect.Load (xy.x - h/2,
							xy.y - h/2,
							xy.x + h/2,
							xy.y + h/2);
				Port.FrameOval (labelRect);
				Port.PaintOval (labelRect);
			}

			if (IsFlag (NF_NAIL))
			{
				labelRect.Load (xy.x,
							xy.y - h/2,
							xy.x + h,
							xy.y + h/2);
#ifndef Macintosh
				VBitmap b ("NAIL");
				Port.DrawBitmap (labelRect, b);
#endif
         	}
		}
	}
	else
	{
		// Ensure labelxy is meaningful
		LoadPt (&labelxy, xy.x, xy.y);

		VRect labelRect;

		int h = Port.FontHeight () / 2;
		labelRect.Load (xy.x, xy.y, xy.x + h, xy.y + h);

//		if (IsFlag (NF_HIGHLIGHT)) Port.FrameRect (labelRect);
		if (IsFlag (NF_HIGHLIGHT) && (tp.style & TS_HIGHLIGHT))
		{
			labelRect.Load (xy.x - h/2,
						xy.y - h/2,
						xy.x + h/2,
						xy.y + h/2);
			Port.FrameOval (labelRect);
			Port.PaintOval (labelRect);
		}

		if (IsFlag (NF_NAIL))
		{
			labelRect.Load (xy.x,
						xy.y - h/2,
						xy.x + h,
						xy.y + h/2);
#ifndef Macintosh
			VBitmap b ("NAIL");
			Port.DrawBitmap (labelRect, b);
#endif
		}

		labelRect.Offset (-h / 2, -h / 2);
	}

#endif


#ifdef wx_msw
	switch (tp.style)
	{
		case CLADOGRAM:
			if (anc) tp.port->DrawLine (xy.x, xy.y, anc->xy.x, anc->xy.y);
			break;
		case PHYLOGRAM:
			if (anc)
				tp.port->DrawLine (xy.x, xy.y, anc->xy.x, xy.y);
			if (!IsLeaf())
				tp.port->DrawLine (xy.x, child->xy.y, xy.x, 
							child->GetRightMostSibling()->xy.y);
			break;
	}
	if (IsLeaf())
	{
		tp.port->SetFont (tp.labelfont);
		if (label) tp.port->DrawText (label, xy.x, xy.y);
		else
		{
			char tempbuf[10];
			sprintf (tempbuf, " %d", LabelNumber);
			tp.port->DrawText (tempbuf, xy.x, xy.y);
		}
	}
#endif
}

//------------------------------------------------------------------------------
void Node::Calc (treeplot &tp)

{
// CODE FOR RADIAL
/*
	if (anc != NULL)
	{
		PoinT pt;
		anc->GetXY (pt);
		xy.x = pt.x + tp.scale * EdgeLength * cos (height);
		xy.y = pt.y + tp.scale * EdgeLength * sin (height);
	}
*/

	if (IsLeaf())
	{
#ifdef VCLASS
		xy.y     = tp.r.GetTop() + tp.count * tp.leafgap;
#else
		xy.y     = tp.r.top + tp.count * tp.leafgap;
#endif
		tp.lasty = xy.y;
		tp.count++;

#ifdef VIBRANT
		if ((tp.style & TS_SLANT) == TS_SLANT)
		{
			if ((tp.style & TS_LEFT) == TS_LEFT)
			{
				xy.x = tp.r.right;
               }
			else
			{
				xy.x = tp.r.left;
               }
		}
		if ((tp.style & TS_ANGLE) == TS_ANGLE)
		{
			if ((tp.style & TS_LEFT) == TS_LEFT)
					{
				xy.x = tp.r.left + ((height / tp.maxheight) * (tp.r.right - tp.r.left));
			}
			else
			{
				xy.x = tp.r.right - ((height / tp.maxheight) * (tp.r.right - tp.r.left));
			}
		}
#endif


#ifdef VCLASS
		if ((tp.style & TS_SLANT) == TS_SLANT)
		{
			if ((tp.style & TS_LEFT) == TS_LEFT)
			{
				xy.x = tp.r.GetRight();
               }
			else
			{
				xy.x = tp.r.GetLeft();
               }
		}
		if ((tp.style & TS_ANGLE) == TS_ANGLE)
		{
			if ((tp.style & TS_LEFT) == TS_LEFT)
               {
				xy.x = tp.r.GetLeft() + ((height / tp.maxheight) * (tp.r.GetRight() - tp.r.GetLeft()));
			}
			else
			{
				xy.x = tp.r.GetRight() - ((height / tp.maxheight) * (tp.r.GetRight() - tp.r.GetLeft()));
			}
		}
#endif


#ifdef wx_msw
		switch (tp.style)
		{
			case CLADOGRAM:
				xy.x = tp.r.left + tp.r.width;
                    break;
			case PHYLOGRAM:
				xy.x = tp.r.left + (height / tp.maxheight) * tp.r.width;
				break;
		}
#endif
	}
	else
	{
#ifdef VIBRANT
		if ((tp.style & TS_SLANT) == TS_SLANT)
		{
			if ((tp.style & TS_LEFT) == TS_LEFT)
			{
				xy.x = tp.r.left + tp.nodegap * (tp.leaves - weight);
			}
			else
			{
				xy.x = tp.r.right - tp.nodegap * (tp.leaves - weight);
			}
			xy.y = tp.lasty - (((weight - 1) * tp.leafgap) / 2);
		}
		if ((tp.style & TS_ANGLE) == TS_ANGLE)
		{
			if ((tp.style & TS_LEFT) == TS_LEFT)
               {
				xy.x = tp.r.left + (height / tp.maxheight) * (tp.r.right - tp.r.left);
			}
			else
			{
				xy.x = tp.r.right - ((height / tp.maxheight) * (tp.r.right - tp.r.left));
			}
			xy.y = child->xy.y +
				 (child->GetRightMostSibling()->xy.y - child->xy.y) / 2;
          }
#endif


#ifdef VCLASS
		if ((tp.style & TS_SLANT) == TS_SLANT)
		{
			if ((tp.style & TS_LEFT) == TS_LEFT)
			{
				xy.x = tp.r.GetLeft() + tp.nodegap * (tp.leaves - weight);
			}
			else
			{
				xy.x = tp.r.GetRight() - tp.nodegap * (tp.leaves - weight);
			}
			xy.y = tp.lasty - (((weight - 1) * tp.leafgap) / 2);
		}
		if ((tp.style & TS_ANGLE) == TS_ANGLE)
		{
			if ((tp.style & TS_LEFT) == TS_LEFT)
               {
				xy.x = tp.r.GetLeft() + (height / tp.maxheight) * (tp.r.GetRight() - tp.r.GetLeft());
			}
			else
			{
				xy.x = tp.r.GetRight() - ((height / tp.maxheight) * (tp.r.GetRight() - tp.r.GetLeft()));
			}
			xy.y = child->xy.y +
				 (child->GetRightMostSibling()->xy.y - child->xy.y) / 2;
          }
#endif



#ifdef wx_msw
		switch (tp.style)
		{
			case CLADOGRAM:
				xy.x = tp.r.left + tp.nodegap * (tp.leaves - weight);
				xy.y = tp.lasty - (((weight - 1) * tp.leafgap) / 2);
				break;
			case PHYLOGRAM:
				xy.x = tp.r.left + (height / tp.maxheight) * tp.r.width;
				xy.y = child->xy.y +
				 (child->GetRightMostSibling()->xy.y - child->xy.y) / 2;
				break;
		}
#endif
	}

}


//------------------------------------------------------------------------------
// Return the most recent common ancestor of node and p
Node *Node::MRCA (Node *p)
{
	NodePtr r = p;

	switch (ClusterRelationship (p->cluster))
	{
#ifdef UseMikesBBitset
		case IDENTITY:
			return this;
		case SUBSET:
			return p;
		case SUPERSET:
				return this;
		case DISJOINT:
			// nodes are on different branches
			while (ClusterRelationship (r->cluster) == DISJOINT)
				r = r->GetAnc ();
			return r;
#else
		case rdmpIDENTITY:
			return this;
		case rdmpSUBSET:
			return p;
		case rdmpSUPERSET:
				return this;
		case rdmpDISJOINT:
			// nodes are on different branches
			while (ClusterRelationship (r->cluster) == rdmpDISJOINT)
				r = r->GetAnc ();
			return r;
#endif
		default:
			return NULL;
	}
}


//------------------------------------------------------------------------------
//Add p to the linked list to the right of the node and increment value.
void Node::SetRightMost (Node *p)
{
	if (!right)
	{
		right = p;
		p->SetValue (value + 1);
	}
	else
	{
		NodePtr q = right;
		while (q->right) q= q->right;
          q->right = p;
		p->SetValue (q->value + 1);
	}
	p->SetRight (NULL);
}

//------------------------------------------------------------------------------
// Insert p into the linked list to the right of the node in front
// of before and increment values to the right
void Node::InsertBeforeInRightList (Node *p, Node *before)
{
	Node *q = this;
	Node *r = right;
	if (r)
	{
     	// Find node in fron of before
		while (r != before)
		{
			q = r;
			r = r->right;
		}
		  // Insert p in front of before
		q->right = p;
		p->right = before;
		  p->SetValue (before->GetValue());

		// Increment value fields downstream of p
		while (r != NULL)
		{
			r->IncrementValue ();
			r = r->right;
		}
	}
}

///------------------------------------------------------------------------------
// True if node is a left descendant of q
BooleaN Node::IsALeftDescendantOf (Node *q)
{
	NodePtr r = this;
	while ((r) && (r != q->child) && (r != q)) r = r->anc;
	return (BooleaN)(r == q->child);
}

//------------------------------------------------------------------------------
// Return rightmost node in linked list attached to right field
Node *Node::GetRightMost ()
{
	if (!right) return this;
	else
	{
		NodePtr q = right;
		while (q->right) q = q->right;
			 return (q);
	}
}

#ifdef VCLASS

//------------------------------------------------------------------------------
BooleaN Node::PtOnNode (PoinT &pt, BooleaN Left)
{
	VRect labelRect;

	int h = Port.FontHeight () / 2;
	if (IsLeaf () && !IsFlag (NF_WIDESPREAD))
	{
		char tempbuf[MAXNAMELENGTH + 1];
		if (label)
		{
			strcpy (tempbuf, label);
		}
		else
		{
			sprintf (tempbuf, "%d", LabelNumber);
		}
		if (Left)
		{
			labelRect.Load (xy.x,
				xy.y - h,
				xy.x + Port.TextWidth (tempbuf, strlen (tempbuf)) + 2,
				xy.y + h);
			labelRect.Offset (Port.MaxCharWidth ()/2, 0);
		}
		else
      {
			labelRect.Load (xy.x - Port.TextWidth (tempbuf, strlen (tempbuf)),
					xy.y - h,
					xy.x,
					xy.y + h);

			labelRect.Offset (-Port.MaxCharWidth ()/2, 0);
      }
	}
	else
	{
		labelRect.Load (xy.x, xy.y, xy.x + h, xy.y + h);
		labelRect.Offset (-h / 2, -h / 2);
	  }
	return ( (BooleaN) labelRect.PtInRect (pt));
}
#endif


//------------------------------------------------------------------------------
// Test whether node is an ancestor of p
BooleaN Node::IsAnAncestorOf (Node *p)
{
	Node *q = p;
	while ((q != NULL) && (q != this)) q = q->GetAnc ();
     return ((BooleaN)(q != NULL)); 
}

//------------------------------------------------------------------------------
//
// @mfunc void | Node | RotateDescendants | Rotate descendants of a binary node.
//
void Node::RotateDescendants ()
{
	if (!IsLeaf() && IsBinary())
	{ 
		Node *r = child;
		child = r->sib;
		child->sib = r;
      	r->sib = NULL;
	}
}

//------------------------------------------------------------------------------
//
// @mfunc void | Node | RadialCalc | Calculate coorindates of node for a radial tree.
//
// @parm treeplot | tp | Tree plot information.
//
void Node::RadialCalc (treeplot &tp)
{
	if (anc != NULL)
	{
		PoinT pt;
		anc->GetXY (pt);
		xy.x = pt.x + tp.scale * EdgeLength * cos (height);
		xy.y = pt.y + tp.scale * EdgeLength * sin (height);

        x = xy.x;
        y = xy.y;


#ifdef ROTATE_LABELS
		// The final dimensions of a radial tree are hard to predict, so
		// we keep track of the rectangle occupied by the tree itself,
		// and by the terminal labels.
		// 20/4/97
		// Update our record of the extent of the tree
		tp.treeExtent.SetLeft (MIN (xy.x, tp.treeExtent.GetLeft ()));
		tp.treeExtent.SetRight (MAX (xy.x, tp.treeExtent.GetRight ()));
		tp.treeExtent.SetTop (MIN (xy.y, tp.treeExtent.GetTop ()));
		tp.treeExtent.SetBottom (MAX (xy.y, tp.treeExtent.GetBottom ()));

		// Update the extent of the tree plus taxon labels
		//15/4/97
		PoinT tmp;
		LoadPt (&tmp, xy.x, xy.y);
		if (IsLeaf() && label)
		{
			Port.SelectAFont (tp.labelfont);
			int w = Port.TextWidth (label, strlen (label));
			if (tp.style & TS_ROTATE_LABELS)
			{
				// Take into account the rotation of the labels
				tmp.x += w * cos (height);
				tmp.y += w * sin (height);
			}
			else
			{
				if (pt.x < xy.y)
					tmp.x -= w;
				else
					tmp.x += w;
				if (pt.y < xy.y)
					tmp.y -= Port.FontHeight ();
				else
					tmp.y += Port.FontHeight ();
			}
			tp.labelExtent.SetLeft (MIN (tmp.x, tp.labelExtent.GetLeft ()));
			tp.labelExtent.SetRight (MAX (tmp.x, tp.labelExtent.GetRight ()));
			tp.labelExtent.SetTop (MIN (tmp.y, tp.labelExtent.GetTop ()));
			tp.labelExtent.SetBottom (MAX (tmp.y, tp.labelExtent.GetBottom ()));
		}
#endif
	}
}

//------------------------------------------------------------------------------
//
// @mfunc void | Node | RadialDraw | Draw a node in a radial tree.
//
// @parm treeplot | tp | Tree plot information.
//
void Node::RadialDraw (treeplot &tp)
{
	if (anc)
		Port.DrawLine (xy, anc->xy);

	// Set font to use 16/4/96
	if (IsLeaf ())
		Port.SelectAFont (tp.labelfont);
	else
		Port.SelectAFont (tp.edgefont);


	// clw
	if (IsLeaf () || (tp.style & (TS_LABELEDGES | TS_USEINTERNALLABEL)))
	{
		VRect labelRect;
		char tempbuf[MAXNAMELENGTH + 1];
		int h = Port.FontHeight ();

		if (IsLeaf () || (tp.style & TS_USEINTERNALLABEL))
		{
			if (label)
				strcpy (tempbuf, label);
			else
				tempbuf[0] = '\0';
		}
		else
		{
			if (EdgeLength > 0)
				sprintf (tempbuf, "%d", (int)EdgeLength);
			else
				tempbuf[0] = '\0';
		}
		int w = Port.TextWidth (tempbuf, strlen (tempbuf)) + 2;

 		// Draw the label
 		if (anc && (tempbuf[0] != '\0'))
		{
			PoinT ancXY;
			anc->GetXY (ancXY);

#ifdef ROTATE_LABELS
			// Rotate or straight?
			if ((tp.style & TS_ROTATE_LABELS) && IsLeaf())
			{
				// Rotated label
				// 4/3/97 New font drawing
				float dx = xy.x - ancXY.x;
				float dy = ancXY.y - xy.y;
				float angle;
				if (fabs(dx) < 0.00001)
				{
					// avoid floating point error
					if (dy < 0.0)
						angle = M_PI / 2.0;
					else
						angle = M_PI * 3.0 / 2.0;
				}
				else
				{
					// Convert to our clockwise rotation from x-axis
					// coordinates
					angle = 2 * M_PI - atan (dy/dx);
					if (dx < 0.0)
						angle -= M_PI;
				}
				Port.DrawRotatedText (tempbuf, xy, tp.labelfont, angle);
			}
			else
			{
				if (ancXY.x >= xy.x)
				{
					// to left
					labelRect.SetRight (xy.x);
					labelRect.SetLeft (xy.x - w);
					labelRect.Offset (-Port.MaxCharWidth()/2, 0);
				}
				else
				{
					labelRect.SetLeft (xy.x);
					labelRect.SetRight (xy.x + w);
					labelRect.Offset (Port.MaxCharWidth()/2, 0);
				}

				// Decide if we draw above or below tip
				if (ancXY.y > xy.y)
				{
					// above
					labelRect.SetBottom (xy.y);
					labelRect.SetTop (xy.y - h);
				}
				else
				{
					labelRect.SetTop (xy.y);
					labelRect.SetBottom (xy.y + h);
				}

				if (!IsLeaf ())
				{
					// translate bootstrap value to middle of branch
					labelRect.SetBottom (xy.y + h/2);
					labelRect.SetTop (xy.y - h/2);
					int xx = (ancXY.x - xy.x) / 2;
					int yy = (ancXY.y - xy.y) / 2;

					// 20/4/97
					// If edge is horizontal then we should move label above
					// edge
					if (abs (yy) < h)
					{
						if (yy < 0)
							yy -= h;
						else
							yy += h;

						// Centre it along the edge
						if (xx < 0)
							xx -= w/2;
						else
							xx += w/2;
					}
					labelRect.Offset (xx, yy);
				}

				// draw labels
				Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'l', FALSE);
				LoadPt (&labelxy, labelRect.GetRight(), xy.y);
         }
#else
     		// Decide if we draw it left or right of tip of edge
			if (ancXY.x >= xy.x)
			{
				// to left
				labelRect.SetRight (xy.x);
				labelRect.SetLeft (xy.x - w);
				labelRect.Offset (-Port.MaxCharWidth()/2, 0);

			}
			else
			{
				labelRect.SetLeft (xy.x);
				labelRect.SetRight (xy.x + w);
				labelRect.Offset (Port.MaxCharWidth()/2, 0);
			}

			// Decide if we draw above or below tip
			if (ancXY.y > xy.y)
			{
				// above
				labelRect.SetBottom (xy.y);
				labelRect.SetTop (xy.y - h);
			}
			else
			{
				labelRect.SetTop (xy.y);
				labelRect.SetBottom (xy.y + h);
			}

			// clw
			if (!IsLeaf ())
			{
				// translate bootstrap value to middle of branch
				labelRect.SetBottom (xy.y + h/2);
				labelRect.SetTop (xy.y - h/2);
				int xx = (ancXY.x - xy.x) / 2;
				int yy = (ancXY.y - xy.y) / 2;
				labelRect.Offset (xx, yy);
			}

			// draw labels
			Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'l', FALSE);
			LoadPt (&labelxy, labelRect.GetRight(), xy.y);

#endif
		}
	}
}

//------------------------------------------------------------------------------
// Reconciled trees
void Node::Copy (Node *theCopy)
{
	if (IsLeaf())
		theCopy->SetFlag (NF_LEAF, True);
	theCopy->LabelNumber = LabelNumber;
	theCopy->LeafNumber = LeafNumber;
	// eventually improve this, but we need a local copy
	if (label)
	{
		theCopy->label = new char[strlen (label) + 1];
		strcpy (theCopy->label, label);
	}
}


#ifdef NODE_DEBUG
//------------------------------------------------------------------------------
// Memory debugging
void Node::MemDump (ofstream &f)
{
	f << setw(3) << index
		<< setw(3) << LeafNumber;
	if (!child) f << " - ";
	else
		f << "(" << setw(3) << child->GetIndex () << ") " << (void *)child;
	if (!sib) f << " - ";
	else
		f << "(" << setw(3) << sib->GetIndex () << ") " << (void *)sib;
	if (!anc) f << " - ";
	else
		f << "(" << setw(3) << anc->GetIndex () << ") " << (void *)anc;
}
#endif

//------------------------------------------------------------------------------
// 11 /7/96
int Node::GetDegreeOf ()
{
	Node *s = child;
	int count = 0;
	while (s)
	{
		count++;
		s = s->sib;
	}
   degree = count;
	return count;
}

//------------------------------------------------------------------------------
// 19/9/96
// Return sibling node that is immediately to the LEFT of this node
Node *Node::LeftSiblingOf ()
{
	NodePtr q = anc->child;
	while (q->sib != this)
		q = q->sib;
	return q;
}

//------------------------------------------------------------------------------
// 28/7/97
// (Un)Mark path between node and p
void Node::MarkPathTo (Node *p, BooleaN mark)
{
	// Mark path this -- MRCA (this, p)
	NodePtr q = this;
#ifdef UseMikesBBitset
	while (q->cluster.rel(p->cluster) == DISJOINT)
#else
	while (q->cluster.Relationship (p->cluster) == rdmpDISJOINT)
#endif
	{
		q->SetFlag (NF_MARKED, mark);
		q = q->GetAnc ();
	}
	// Mark path p -- MRCA (this, p)
	q = p;
#ifdef UseMikesBBitset
	while (q->cluster.rel(this->cluster) == DISJOINT)
#else
	while (q->cluster.Relationship (this->cluster) == rdmpDISJOINT)
#endif
	{
		q->SetFlag (NF_MARKED, mark);
		q = q->GetAnc ();
	}
}		


