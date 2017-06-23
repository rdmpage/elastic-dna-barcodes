#include <string.h>
#include <ctype.h>
#include "cpvars.h"
#include "cputils.h"

// If label contains single quote(s), double quote them and enclose whole string
// in quotes, or return original label with spaces replaced by underscores.
// Used when writing NEXUS files.
char *OutputString (char *s)
{
	char	buf[128];
	char 	*p = s;
	BooleaN 	enclose  = False;
	BooleaN	hasSpace = False;

	if (isalpha (*p))
	{
		// First character is a letter, check the rest
		p++;
		while (*p && !enclose)
		{
			if (!isalnum (*p) && (*p != ' ') && (*p != '_') && (*p != '.'))
				enclose = True;
			if (*p == ' ') hasSpace = True;
			p++;
		}
	}
	else
	{
		// First character is not a letter, so enclose string
		enclose = True;
	}

	if (!enclose)
	{
		if (hasSpace)
		{
			// Replace spaces by underscores
			p = s;
			while (*p)
			{
				if (*p == ' ') *p = '_';
				p++;
			}
		}
		return (s);
	}

	//Enclose
	buf[0] = '\'';
	int j = 1;

	// Insert single quotes where needed
	p = s;
	while (*p)
	{
		if (*p == '\'') buf[j++] = '\'';
			 buf[j++] = *p;
		p++;
	}

	// Append last single quote
	buf[j++] = '\'';
	buf[j] = '\0';
	strcpy (s, buf);
	return (s);
}



// Export a string of n characters, right padded with spaces if necessary
// Non alphanumeric characters are replaced by X
char *ExportString (char *s, int n)
{
	static char buf[MAXNAMELENGTH + 1]; // rdp 11/1/96

	buf[0] = '\0';

	int len = strlen (s);
	if (len > n)
   {
		strncpy (buf, s, n);
	}
	else
	{
		strcpy (buf, s);
		for (int j = len; j < n; j++) buf[j] = ' ';
   }
	buf[n] = '\0';

	char *p = buf;
	while (*p)
	{
		if (!isalnum( (int) p)) *p = 'X';
		p++;
   }

	return buf;
}

