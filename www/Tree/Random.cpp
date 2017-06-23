//
// (c) 1993, Roderic D. M. Page
//
// Filename:		random.cpp
//
// Author:       	Rod Page
//
// Date:            23 July 1993
//
// Class:			Uniform
//
// Inheritance:
//
// Purpose:		Encapsulate a uniform random number generator.
//				A straight translation of CPWRAND.PAS
//
// Modifications:
//
// 28/6/94 Compiled with BC++ 4.0

#include <stdlib.h>
#include <time.h>
#include "random.h"

#define rdmpRAND_P 2147483647
#define rdmpRAND_A      16807
#define rdmpRAND_B15    32768
#define rdmpRAND_B16    65536

// Use C++ stdlib routines randomize and random to seed the generator
Uniform::Uniform (long seed)
{
	if (seed != 0) Seed = seed;
	else
	{
		time_t t;
		srand((unsigned) time(&t));
		Seed = (rdmpRAND_P / RAND_MAX) * rand ();
		if (Seed == 0) Seed++;
	}
}


// Return 0 <= random number <= 1
double Uniform::Random01 ()
{
	long IX, XALO, XHI, LEFTLO, FHI, K;

	IX 	 = Seed;
	XHI 	 = IX / rdmpRAND_B16;
	XALO   = (IX - XHI * rdmpRAND_B16) * rdmpRAND_A;
	LEFTLO = XALO / rdmpRAND_B16;
	FHI 	 = XHI * rdmpRAND_A + LEFTLO;
	K	  	 = FHI / rdmpRAND_B15;
	IX 	 = (((XALO - LEFTLO * rdmpRAND_B16) - rdmpRAND_P) + (FHI - K * rdmpRAND_B15) * rdmpRAND_B16) + K;
	if (IX < 0) IX = IX + rdmpRAND_P;
	Seed   = IX;
	return (double)IX / (double) rdmpRAND_P;
}


// Return minvalue <= random number <= maxvalue
long Uniform::RandomInteger (long minvalue, long maxvalue)
{
	return (minvalue + ((maxvalue - minvalue + 1) * Random01()));
}


