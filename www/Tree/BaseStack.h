
#ifndef BASESTACKH
#define BASESTACKH

#define STACK_DEBUG

#include "cpvars.h"
#include "BaseNode.h"

class BaseNodeStack
{
public:
	BaseNodeStack() { Clear (); };
	void		Clear() { stkptr = -1; };
	int 		Push (BaseNodePtr p)
				{
				if (stkptr < MAXNODES)
				{
					stack[++stkptr] = p;
					return (True);
                }
				else return (False);
			};
	void 	Pop() { --stkptr; };
	BaseNodePtr TopOfStack() { return (stack[stkptr]); };
	int		EmptyStack() { return (stkptr == -1); };
#ifdef STACK_DEBUG
	int		GetStkPtr() { return (stkptr); };
#endif
private:
	BaseNodePtr stack[MAXNODES];
	int		stkptr;
};

#endif

