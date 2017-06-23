#ifndef STACKH
#define STACKH

#define STACK_DEBUG

#include "CpVars.h"
#include "Node.h"

class NodeStack
{
public:
	NodeStack() { Clear (); };
	void	Clear() { stkptr = -1; };
	int 	Push (NodePtr p)
			{
				if (stkptr < MAXNODES)
				{
					stack[++stkptr] = p;
					return (True);
                    }
				else return (False);
			};
	void 	Pop() { --stkptr; };
	NodePtr	TopOfStack() { return (stack[stkptr]); };
	int		EmptyStack() { return (stkptr == -1); };
#ifdef STACK_DEBUG
	int		GetStkPtr() { return (stkptr); };
#endif
private:
	NodePtr stack[MAXNODES];
	int		stkptr;
};

#endif /* STACKH */

