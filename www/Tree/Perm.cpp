#include "perm.h"

Permutation::Permutation ()
{
	P = NULL;
}

// Create vector <1, ..., n>
Permutation::Permutation (int n, long seed)
{
	P = NULL;
	Initialise (n, seed);
}


// Create vector <1, ..., n>
void Permutation::Initialise (int n, long seed)
{
	if (P) delete [] P;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
	P = new int[n];
	items = n;
	if (seed != 0) U.SetSeed (seed);
	for (int i = 0; i < n; i++) P[i] = i;

	D = NULL;
}

// Create vector <1, ..., n>
void Permutation::Initialise (int n)
{
	if (P) delete [] P;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
	P = new int[n];
	items = n;
	for (int i = 0; i < n; i++) P[i] = i;

	D = NULL;
}


Permutation::~Permutation ()
{
	if (P) delete [] P;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
	if (D) delete [] D;
}

void Permutation::Permute ()
{
	for (int i = 0; i < items - 1; i++)
	{
		int j =  items - 1 - i;
		int k = U.RandomInteger (0, j);
		int t = P[j];
		P[j] = P[k];
		P[k] = t;
	}
} 	

void Permutation::Permute (Uniform &u)
{
	for (int i = 0; i < items - 1; i++)
	{
		int j =  items - 1 - i;
		int k = u.RandomInteger (0, j);
		int t = P[j];
		P[j] = P[k];
		P[k] = t;
	}
} 	


void Permutation::Dump ()
{
	for (int i = 0; i < items; i++) cout << P[i] << " ";
	cout << endl;
}


int Permutation::GetNext ()
{
	int i;
	if (D == NULL)
	{
		for (i = 0; i < items; i++) P[i] = i;
		D = new int[items + 1];
		for (i = 0; i <= items; i++) D[i] = 0;
		return 1;
	}
	int j = -1;
	int kl = 1;
	for (int k = 2; k <= items; k++)
	{
		int dk = D[k];
		if (dk != kl)
		{
			D[k] = ++dk;
			if (j != 1 && dk > 2) kl = k - D[k];
			int t = P[k-1];
			P[k-1] = P[kl-1];
			P[kl-1] = t;
			return 1;
		}
		D[k] = 0;
		j = -j;
		kl = k;
	}
	if (D) delete [] D;	// 25.8.00 MAC -- this should use delete [] since was created with new [].
	D = NULL;
	return 0;
} 

/*int main ()
{
	Permutation P;
        Uniform U;

	P.Initialise (4);
   P.Dump();
	P.Permute (U);
   P.Dump();

 //	while (P.GetNext()) P.Dump();
	return 0;
}
*/
