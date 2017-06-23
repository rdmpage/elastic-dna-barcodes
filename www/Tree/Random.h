//
// (c) 1993, Roderic D. M. Page
//
// Filename:		random.h
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

#ifndef RANDOMH
#define RANDOMH

class Uniform
{
public:
	// use C++ randomize and random to seed the generator
	Uniform (long seed = 0);
	long		GetSeed () { return Seed; };
	void		SetSeed (long aseed) { Seed = aseed; };
	// 0 <= random number <= 1
	double 	Random01 ();
	// minvalue <= random integer <= maxvalue
	long 	RandomInteger (long minvalue, long maxvalue);
private:
	long		Seed;
};

#endif /* UNIFORM */
