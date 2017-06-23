// 14/6/94

// Permutations

#ifndef PERMH
#define PERMH

#define PERM_DEBUG


#ifdef PERM_DEBUG
	#include <iostream.h>
#endif

#include "random.h"

// Permutations
class Permutation 
{
public:
	int *P;
	Permutation ();
	Permutation (int n, long seed = 0);
	~Permutation ();
	void Dump();
	void Initialise (int n);
	void Initialise (int n, long seed);
     // Generate a random permutation
	void Permute ();
	void Permute (Uniform &u);
	// Generate all permutations, return 0 if no more
	int 	GetNext ();
private:
	int		items;
	Uniform U;
	int		*D;
};

#endif


