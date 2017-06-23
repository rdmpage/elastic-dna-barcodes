/***************************************************************************
*                                                                          *
*  	COMPONENT C++ translation                                             *
*                                                                          *
*  	(c) 1993, Roderic D. M. Page                                          *
*      All rights reserved                                                 *
*                                                                          *
*                                                                          *
*  	Compiler:  Borland Turbo C++ for Windows 3.1                          *
*  	Platform:  Microsoft Windows 3.1                                      *
*                                                                          *
*        File: flags.cpp                                                   *
*	 Purpose: bitmask flags                                               *
*     History: 15 June 1993                                                *
* 	                                                                      *
***************************************************************************/

#ifndef FLAGS
#define FLAGS 

class flags
{
public:
	flags() { flag=0; };
	int IsFlag (const int f) { return (f & flag); };
	void SetFlag (const int f, const int On)
	{
		if (On) flag = flag | f; 
		else flag = flag & (~f);
     }
 
private:
	int flag;
};

#endif

