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

#ifndef BINTREE
#define BINTREE

#include "tree.h"
#include "random.h"
#include "perm.h"

class BinaryTree : public Tree
{
public:
	BinaryTree() : Tree() {}
	BinaryTree (BinaryTree &t) : Tree (t) {}
	virtual void 	AddNodeBelow (Node *node, Node *below);
	virtual void 	Random (BooleaN rooted, Uniform &U, int n);
	virtual	void	ThreeLeafTree (int n);
	virtual	void	TwoLeafTree (int n);
	virtual	void	Markovian (Uniform &U, int n);
	virtual NodePtr RemoveNode (NodePtr Node);

    virtual void 	RandomlyResolve (Uniform &U);
	virtual void 	DefaultComb (int n);    
protected:
	Uniform	*u;
    virtual void	resolveTraverse (NodePtr p);

};

typedef BinaryTree *BinaryTreePtr;

#endif /* BINTREE */


