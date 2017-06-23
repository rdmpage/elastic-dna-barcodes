#include <math.h>
#include "BaseNode.h"

//------------------------------------------------------------------------------
// Constructor
BaseNode::BaseNode()
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
//	value		= 0;
}


//------------------------------------------------------------------------------
// Label the node with alabel
void BaseNode::SetLabel (const char *alabel)
{
	if (alabel)
	{
		label = new char[strlen (alabel) + 1];
		*label = '\0';
		strcpy (label, alabel);
	}
    else
    {
    	if (label)
        	delete label;
    	label = NULL;
    }
}

//------------------------------------------------------------------------------
// Return the rightmost sibling of the node
BaseNode *BaseNode::GetRightMostSibling ()
{
	BaseNode *p = this;

	while (p->sib) p = p->sib;
	return (p);
}




//------------------------------------------------------------------------------
void BaseNode::Draw (treeplot &tp)
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
				strcpy (tempbuf, label);
			else
				sprintf (tempbuf, "%d", LabelNumber);
		}

      if (!IsLeaf ())
      {
      	Port.Red ();
      }


		if ((tp.style & TS_LEFT) == TS_LEFT)
		{
			labelRect.Load (xy.x,
							xy.y - h,
							xy.x + Port.TextWidth (tempbuf, strlen (tempbuf)) + 2,
							xy.y + h);

			labelRect.Offset (Port.MaxCharWidth()/2, 0);
			Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'l', FALSE);
			LoadPt (&labelxy, labelRect.GetRight(), xy.y);

    	}
		else
		{
			labelRect.Load (xy.x - Port.TextWidth (tempbuf, strlen (tempbuf)) - 2,
							xy.y - h,
							xy.x,
							xy.y + h);


			labelRect.Offset (-Port.MaxCharWidth()/2, 0);
			Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'r', FALSE);
			LoadPt (&labelxy, labelRect.GetLeft(), xy.y);
      	}

      if (!IsLeaf ())
      {
      	Port.Black ();
      }


	}
	else
	{
		// Ensure labelxy is meaningful
		LoadPt (&labelxy, xy.x, xy.y);

		VRect labelRect;

		int h = Port.FontHeight () / 2;
		labelRect.Load (xy.x, xy.y, xy.x + h, xy.y + h);

		labelRect.Offset (-h / 2, -h / 2);
  	}

#endif
}

//------------------------------------------------------------------------------
void BaseNode::Calc (treeplot &tp)
{

	if (IsLeaf())
	{
#ifdef VCLASS
		xy.y     = tp.r.GetTop() + tp.count * tp.leafgap;
#else
		xy.y     = tp.r.top + tp.count * tp.leafgap;
#endif
		tp.lasty = xy.y;
		tp.count++;

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

	}
	else
	{
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
	}

}


//------------------------------------------------------------------------------
// Return the most recent common ancestor of node and p
BaseNode *BaseNode::MRCA (BaseNode *p)
{
	BaseNodePtr r = p;

	switch (ClusterRelationship (p->cluster))
	{
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
		default:
			return NULL;
	}
}


//------------------------------------------------------------------------------
// True if node is a left descendant of q
BooleaN BaseNode::IsALeftDescendantOf (BaseNode *q)
{
	BaseNodePtr r = this;
	while ((r) && (r != q->child) && (r != q)) r = r->anc;
	return (BooleaN)(r == q->child);
}


//------------------------------------------------------------------------------
// Test whether node is an ancestor of p
BooleaN BaseNode::IsAnAncestorOf (BaseNode *p)
{
	BaseNode *q = p;
	while ((q != NULL) && (q != this)) q = q->GetAnc ();
     return ((BooleaN)(q != NULL)); 
}


//------------------------------------------------------------------------------
//
// @mfunc void | Node | RadialCalc | Calculate coorindates of node for a radial tree.
//
// @parm treeplot | tp | Tree plot information.
//
void BaseNode::RadialCalc (treeplot &tp)
{
	if (anc != NULL)
	{
		PoinT pt;
		anc->GetXY (pt);
		xy.x = pt.x + tp.scale * EdgeLength * cos (height);
		xy.y = pt.y + tp.scale * EdgeLength * sin (height);

        x = xy.x;
        y = xy.y;

	}
}

//------------------------------------------------------------------------------
//
// @mfunc void | Node | RadialDraw | Draw a node in a radial tree.
//
// @parm treeplot | tp | Tree plot information.
//
void BaseNode::RadialDraw (treeplot &tp)
{
	if (anc) 
		Port.DrawLine (xy, anc->xy);
		
	// Set font to use 16/4/96
	if (IsLeaf ())
		Port.SelectAFont (tp.labelfont);
	else
		Port.SelectAFont (tp.edgefont);


	if (IsLeaf () || (tp.style & (TS_LABELEDGES | TS_USEINTERNALLABEL)))
	{
		VRect labelRect;
		char tempbuf[MAXNAMELENGTH + 1];
//		Port.SelectAFont (tp.labelfont);
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

		// Decide if we draw it left or right of tip of edge
		if (anc && (tempbuf[0] != '\0'))
		{
			PoinT ancXY;
			anc->GetXY (ancXY);
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

      		if (!IsLeaf ())
      		{
            	Port.SetMode (vMERGE);
      			Port.Red ();
      		}

			}

			// draw labels
			Port.DrawText (labelRect, tempbuf, strlen (tempbuf), 'l', FALSE);
			LoadPt (&labelxy, labelRect.GetRight(), xy.y);

      		if (!IsLeaf ())
      		{
      			Port.Black ();
           		Port.SetMode (vCOPY);
      		}

		}
	}
}


//------------------------------------------------------------------------------
// 11/9/96
int BaseNode::GetDegreeOf ()
{
	BaseNode *s = child;
	int count = 0;
	while (s)
	{
		count++;
		s = s->sib;
	}
   degree = count;
	return count;
}



