/*
strstr implementations by various entities.  Taken from the USENET comp.lang.c
newsgroup.

For general use strstr1 and strstr20 are the fastest by maybe 10%.
Jonas M|ls{ (strstr4-6) are always slowest by a factor of 3 or more.
strstr1 and strstr20 are within 0.5% speed when searching English or French text,
or a disordered or ordered, unique word list.  In the case where the first
character of s2 is found few times or not at all, strstr8, strstr15,
strstr16 and strstr7 are 6.5 times faster than the others, likely due to the
library strchr using SIMD instructions to search fast.
*/


#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

char rcsid_ssF[] =
		"$Id: strstrFunctions.c,v 1.76 2024-02-28 09:58:09-05 ron Exp $";

char *submitters[] = {
    "Compiler library strstr",                  /* 0  */
    "Ron Charlton",                             /* 1  */
    "Dale Moore strstr2",                       /* 2  */
    "Chris Torek (BSD)",                        /* 3  */
    "Jonas M|ls{ 1",                            /* 4  */
    "Jonas M|ls{ 2",                            /* 5  */
    "Jonas M|ls{ 3",                            /* 6  */
    "Dan Stubbs (Plauger)",                     /* 7  */
    "Dale Moore strstr8",                       /* 8  */
    "Compiler library strstr (was SUN)",        /* 9  */
    "osf1",                                     /* 10 */
    "Moore1a (same as strstr2)",                /* 11 */
    "Moore1b",                                  /* 12 */
    "Moore2a",                                  /* 13 */
    "Moore2b",                                  /* 14 */
    "Moore3a",                                  /* 15 */
    "Moore3b",                                  /* 16 */
    "Compiler library strstr (was Boyer-M-H)",  /* 17 */
    "Visual C/C++ v6.0, 8.0, 10.0 C source",    /* 18 */
    "Berkeley",                                 /* 19 */
    "GNU coreutils 5.3.0",                      /* 20 */
};

/*---------------------------(strstr1)------------------------------------*/

/* Ron Charlton (charltoncr@wowway.com or Ron@RonCharlton.org)
 *
 * strstr1 returns a pointer to the first occurrence of string s2 in string s1,
 * or a NULL pointer if s2 does not occur in s1.  It returns s1 if s2 points
 * to a zero length string.
 *
 * Algorithm: Do a strchr-like search for s2's first character in s1 and then,
 * if found, follow it with a char-by-char comparison of the remainder of
 * s2 with the appropriate part of s1.  Repeat progressively if necessary
 * until a match is found or the end of s1 is reached.
 */

char *strstr1(register const char *s1, register const char *s2)
{
	register const char *p1, *p2;
    register char c, d;

	if (!(c = *s2++)) return (char *)s1;
    d = *s2++;

	for (;;) {
        // strchr-like for loop unrolled for speed
        for (; *s1 != c; ++s1) {
            if (!*s1) return NULL;
            if (*++s1 == c) break;
            if (!*s1) return NULL;
        }
        if (!d) return (char *)s1;
        if (*++s1 == d) {
            for (p1 = ++s1, p2 = s2; (*p1 == *p2) && *p2;) ++p1, ++p2;
            if (!*p2) return (char *)s1 - 2;
        } else
            if (!*s1) return NULL;
	}
}


/*---------------------------(strstr2)------------------------------------*/

/*
 * From: moore@MOORE.FAC.CS.CMU.EDU (Dale Moore)
 * Newsgroups: comp.lang.c
 * Subject: Re: question about strstr() algorithm
 * Date: 11 Oct 1994 21:10:05 GMT
 * Organization: Carnegie Mellon University
 */
/* added const - RonC 2005-12-22 */

/* Dale.Moore@cs.cmu.edu (412-268-7685) as of 2005-12-28 */

char *strstr2(const char *s1, const char *s2)
{
	const char *a, *b;

	for (;;) {
		a = s1;
		b = s2;
		do {
			if (!*b) return (char *)s1;
			if (!*a) return NULL;
		} while (*a++ == *b++);
		s1++;
	}
}


/*---------------------------(strstr3)------------------------------------*/

/*
 * From: torek@elf.bsdi.com (Chris Torek)
 * Newsgroups: comp.lang.c
 * Subject: Re: question about strstr() algorithm
 * Date: 12 Oct 1994 04:18:55 -0700
 * Organization: Berkeley Software Design, Inc.
 *
 * As the author of the 4.4BSD strstr(), I am not sure if I am supposed
 * to be offended. :-)
 *
 * Here is the code, minus copyright and includes:
 */

/* Find the first occurrence of find in s.
 */
char *strstr3(register const char *s, register const char *find)
{
	register char c, sc;
	register size_t len;

	if ((c = *find++) != 0) {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return (NULL);
			} while (sc != c);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return ((char *) s);
}


/*---------------------------(strstr4)------------------------------------*/


/* From Jonas M|ls{ (jbm@nada.kth.se) */


char *strstr4( const char *string,	/* string to search     */
			  const char *pattern)
{								/* pattern to look for  */
	const char *s = string;
	const char *p = pattern;

	while (*s != '\0' && *p != '\0')
		if (*s == *p) {
			++s;
			++p;
		} else {
			s = ++string;
			p = pattern;
		}

	return (*p == '\0') ? (char *) string : 0;
}


/*---------------------------(strstr5)------------------------------------*/

/* From Jonas M|ls{ (jbm@nada.kth.se) */

char *strstr5( const char *string,	/* string to search     */
			  const char *pattern)
{								/* pattern to look for  */
	const char *s = string;
	const char *p = pattern;
	char sc;
	char pc;

	while ((pc = *p++) && (sc = *s++))
		if (pc != sc) {
			s = ++string;
			p = pattern;
		}


	return (pc == '\0') ? (char *) string : 0;
}


/*---------------------------(strstr6)------------------------------------*/

/* From Jonas M|ls{ (jbm@nada.kth.se) */

char *strstr6( register const char *string,	/* string to search     */
			  register const char *pattern)	/* pattern to look for  */
{								
	register const char *s = string;
	register const char *p = pattern;
	register char sc;
	register char pc;

	while ((pc = *p++) && (sc = *s++))
		if (pc != sc) {
			s = ++string;
			p = pattern;
		}


	return (pc == '\0') ? (char *) string : 0;
}


/*---------------------------(strstr7)------------------------------------*/

/* From: dstubbs@garden.csc.calpoly.edu (Dan Stubbs)
 *
 * I came across the implementations given in Plauger, "The Standard C
 * Library," Prentice Hall, 1992 a short time ago and have copied them
 * into this message. Since strstr calls strchr I included it too.
 */

/* [Calling macOS clang strchr is slower here - Ron Charlton 2023-05-01] */

char *strchr7(const char *s, int c)
{
	const char ch = c;
	for (; *s != ch; ++s)
		if (*s == '\0') return NULL;
	return (char *)s;
}

char *strstr7(const char *s1, const char *s2)
{
	if (*s2 == '\0')
		return ((char *) s1);
	for (; (s1 = strchr(s1, *s2)) != NULL; ++s1) {
		const char *sc1, *sc2;
		for (sc1 = s1, sc2 = s2;;) {
			if (*++sc2 == '\0') {
				return ((char *)s1);
			} else if (*++sc1 != *sc2)
				break;
		}
	}
	return (NULL);
}


/*==========================From Dale Moore============================*/

/* See ArticlesAndMail/frommoore.txt for original function names. */
/*
 * Date: Mon, 20 Nov 1995 07:15:13 -0500
 * From: Dale Moore <Dale_Moore@moore.fac.cs.cmu.edu>
 * strstr8() - strstr16()
 */

/* Dale.Moore@cs.cmu.edu (412-268-7685) as of 2005-12-28 */

/*
 * Description:
 *   A program to test out various implementations of the ANSI strstr
 *   routine.
 *
 * Written By:
 *   Dale Moore Jun 1995, Carnegie Mellon University.
 *   With attributions to DEC, OSF, Sun, UCB, & BSD.
 */

/*
 * From ANSI X3.159-1989
 *
 *  4.11.5.7 The strstr Function:
 *
 * Synopsis:
 *   #include <string.h>
 *   char *strstr(const char *s1, const char *s2);
 *
 * Description:
 *   The strstr function locates the first occurrence in the string
 *   pointed to by s1 of the sequence of characters (excluding the
 *   terminating null character) in the string pointed to by s2.
 *
 * Returns:
 *   The strstr function returns a pointer to located string, or a
 *   null pointer if the string is not found.  If s2 points to a
 *   string with zero length, the function returns s1.
 *
 */


/*---------------------------(strstr8)------------------------------------*/

char *strstr8(const char *s1, const char *s2)
/*
 * Algorithm:
 *   A variation on the BSD code.
 *   Attempt to reduce the number of times the outer loop is done.
 *   The outer loop is done the number of times the first character
 *   in the sub string appears in the search string.  But of course,
 *   we end up with an additional routine call.
 *   In every time through the loop we do
 *     - one assignment (s1 = strchr())
 *     - one autoincrment (++s1)
 *     - two routine calls (strchr, strncmp)
 *     - two comparisons (if, if)
 *
 * Benchmark Notes:
 *  This routine is generally slower than strstr_bsd.  
 *  It's speed varies from 87% (Dec PMAX) to 200%(sparc), avg 126%
 *  the speed of strstr_bsd.  I attribute this to routine call overhead.
 */
{
	char c;
	size_t len;

	if ((c = *s2++)) {
		len = strlen(s2);
		for (;;) {
			if (!((s1 = strchr(s1, c)))) return NULL;
			if (!strncmp(++s1, s2, len)) break;
		}
		s1--;
	}
	return (char *)s1;
}


/*---------------------------(strstr Broken)------------------------------*/

/* one strstr (Ultrix) was severely broken.  I eliminated it. */


/*---------------------------(strstr9)-----------------------------------*/

/* This is so severely slow (15,000 times slower on mobyThesaurus.txt's
 * "cut" line) that I replaced it with the compiler's
 * strstr to avoid biasing the timings unduly. RonC 2009-08-24
 */

/* sun */
char *strstr9(const char *s1, const char *s2)
/*
 * Algorithm:
 *   Very STUPID.  Perhaps a most excellent example of how not to
 *   do strstr.  This routine is from Sun's 4.1.1 release of Sun-OS.
 *   I hope that they've fixed it by now.
 *   Even though we advance s1 at a regular pace 
 *   through the search string, we recompute the length
 *   of s1 on each loop.  Other than that, use the standard
 *   strlen & strncmp routines.
 *   The outer loop is done strlen(search string) times.
 *   Each time through the loop, we do
 *     - two routine calls (strlen, strncmp)
 *     - two comparisons (while, if)
 *     - one autoincrement.
 *
 * Benchmark Notes:
 *   Truly abysmal when compared to strstr_bsd.  In my tests this routine
 *   averaged almost 5 times slower (496%) than strstr_bsd.
 */
{
	register size_t len = strlen(s2);

	if (!len)
		return (char *) s1;

	while (strlen(s1) >= len) {
		if (!strncmp(s1, s2, len))
			return (char *) s1;
		s1++;
	}
	return NULL;
}


/*---------------------------(strstr10)-----------------------------------*/

/* osf1 */
char *strstr10(const char *s1, const char *s2)
/*
 * Algorithm:
 *   Gratuitiously obtuse and convoluted.  Perhaps another example
 *   of now not to do it.  This is from the Open Software Foundation's
 *   OSF/1 example release.  Although the algorithm is not that much
 *   different, we seem to have a few gratuitous checks and variables.
 *   And some possible redundant indirection.  A really good compiler
 *   might be able to optimize alot of this away, but I just dont
 *   think that the compiler technology is there yet.
 *
 * Benchmark Notes:
 *   Even though this implementation has no calls to outside routines,
 *   and avoids that associated overhead, it is still takes 150% of the time
 *   of the strstr_bsd routine on my tests.  In my tests, there was only
 *   one case where it was faster than strstr_bsd.
 */
{
	char *p, *q, *r;

	if (*s2 == '\0')
		return ((char *) s1);

	for (q = (char *) s1; *q != '\0'; q++) {
		for (r = q, p = (char *) s2; *r != '\0' && *p != '\0'; r++, p++) {
			if (*p != *r)
				break;
		}
		if (*p == '\0')
			break;
	}
	if (*q)
		return (q);
	else
		return (NULL);
}


/*---------------------------(strstr11)-----------------------------------*/

/* Moore1a */
char *strstr11(const char *s1, const char *s2)
/*
 * Algorithm Notes:
 *   We do the outer loop strlen (search_string) times.
 *   Each time through the outer loop we do
 *      - two assignments (a = s1, b = s2)
 *      - three autoincrements (a++, b++, s1++)
 *            [Two of these could be moved afer the test].
 *      - two indirections (*b, *a)
 *            [If the compiler is stupid, we might have two more];
 *      - three conditionals (if, if, if)
 *
 *   The algorithm loses because every time through the outer loop
 *   we have to initialize both temporaries.  And the everytime we enter
 *   the inner loop we check the first char of the substr for end of string.
 *   The check against (*a) might occasionally be a win if done in the
 *   inner loop instead of the outer loop.  But the outer loop might be
 *   the more conventional place to put it.
 *   Loop unrolling should be obvious.  Merely duplicate everything in the
 *   for(;;) loop.
 *
 *   I originally was fond of this routine for it's compactness and
 *   simplicity.  I thought that compact would be fast.  I've since
 *   changed my mind about this routine.
 *
 *  Benchmark Notes:
 *   This routine is slightly slower than strstr_bsd.  For my tests it
 *   it generally runs in about 122% of the time taken for strstr_bsd.
 *   If you know your data, and the string is to be found relatively
 *   early and often, then this routine will win over strstr_bsd.
 *   But if we have to look very far for it, or if we search lots in
 *   vain, then this routine will be quite slow compared to strstr_bsd.
 *
 */
{
	const char *a, *b;

	for (;;) {
		a = s1;
		b = s2;
		for (;;) {
			if (!*b) return (char *)s1;
			if (!*a) return NULL;
			if (*a++ != *b++) break;
		}
		s1++;
	}
}


/*---------------------------(strstr12)-----------------------------------*/

/* Moore1b */
char *strstr12(const char *s1, const char *s2)
/*
 * Algorithm Notes:
 *   We do the outer loop strlen (search_string) times.
 *   Each time throug the outer loop we do
 *      - four assignments (a = s1, b = s2, bc=*b, ac=*a)
 *      - one autoincrements (s1++)
 *      - three conditionals (if, if, if)
 *
 *   This routine is very similar to the above routine, but will be
 *   slightly faster.  First be lazy, don't autoincrement until we have
 *   to in the inner loop.  Second, avoid indirection, use temporaries
 *   to hold the indirection value.  Third, move test outside inner loop.
 *
 *   The algorithm looses because every time through the outer loop
 *   we have to initialize both temporaries.  And the everytime we enter
 *   the inner loop we check the first char of the substr for end of string.
 *
 *   Loop unrolling should be obvious.  Merely duplicate everything in the
 *   for(;;) loop.
 *
 * Benchmark Notes:
 *   According to my test, this routine runs in about 110% of
 *   the time required for strstr_bsd.  This is a slight improvement
 *   over the previous version, but not a significant change. 
 *   It is still slower than strstr_bsd because it 
 *   spends too much time checking the first character of s2.
 *
 */
{
	const char *a, *b;
	char ac, bc;

	for (;;) {
		a = s1;
		b = s2;
		for (;;) {
			bc = *b;
			if (!bc) return (char *)s1;
			ac = *a;
			if (ac != bc) break;
			b++;
			a++;
		}
		if (!ac)
			return NULL;
		s1++;
	}
}


/*---------------------------(strstr13)-----------------------------------*/

/* Moore2a */
char *strstr13(const char *s1, const char *s2)
/*
 * Algorithm Notes:
 *   The outer loop is done strlen(search_string) times.
 *   Each time through the outer loop we do
 *    - One [possibly two] indirections (*s1)
 *    - one autoincrment (s1++).
 *    - two  tests (if, if)
 *
 *   The inner loop is done only if the first char of the substring is matched.
 *
 *   Loop unrolling should be obvious.  Merely duplicate everything in the
 *   for(;;) loops.
 *
 * Benchmark Notes:
 *   For my tests, This routine generally runs in about 80% of the time
 *   required for strstr_bsd.  I suspect it is mostly because we don't
 *   have that additional routine call overhead.
 */
{
	const char *a, *b;
	char c;

	c = *s2++;
	if (!c)
		return (char *)s1;

	for (;;) {
		if (!*s1)
			return NULL;
		if (*s1++ != c) continue;
		for (a = s1, b = s2;;) {
			if (!*b) return (char *) --s1;
			if (*a++ != *b++) break;
		}
	}
}


/*---------------------------(strstr14)-----------------------------------*/

/* Moore2b */
char *strstr14(const char *s1, const char *s2)
/*
 * Algorithm Notes:
 *   The outer loop is done strlen(search_string) times.
 *   Each time through the outer loop we do
 *    - One indirection (*s1)
 *    - one autoincrment (s1++).
 *    - two  tests (if, if)
 *
 *   Very similar to above routine.  Except
 *     - be lazy, don't autoincrement in inner loop unless we have to.
 *     - avoid indirection, use temporaries to store value of indirection.
 *   The inner loop is done only if the first char of the substring is matched.
 *
 *   Loop unrolling should be obvious.  Merely duplicate everything in the
 *   for(;;) loops.
 *
 * Benchmark Notes:
 *   This routine generally takes about 74% of the time required by strstr_bsd.
 *   I suspect that this improvement over the previous version is because
 *   the compilers generally dont recognize that it can occasionally
 *   save on the indirection and use common subexpression elimination.
 *   Also, the laziness might help.
 */
{
	const char *a, *b;
	char cs1, cs2, ca, cb;

	cs2 = *s2++;
	if (!cs2)
		return (char *) s1;

	for (;;) {
		cs1 = *s1++;
		if (!cs1) return NULL;
		if (cs1 != cs2) continue;
		a = s1;
		b = s2;
		for (;;) {
			cb = *b;
			if (!cb) return (char *) --s1;
			ca = *a;
			if (ca != cb) break;
			a++;
			b++;
		}
	}
}


/*---------------------------(strstr15)-----------------------------------*/

/* Moore3a */
char *strstr15(const char *s1, const char *s2)
/*
 * Algorithm:
 *    Rely completely on str routines.
 *    Each time through the loop we do
 *      - one assignment
 *      - two routine calls (strchr, strncmp).
 *      - one indirection.
 *      - one autoincrement;
 *    If we can come up with routines strchr, and strncmp which
 *    are faster than the simple byte at a time routines, such as
 *    the locc & cmpc3 vax-instruction, or doing the searches or compares
 *    a word at a time, then we might win here.
 *
 *    If we are going to use the str routines to implement this routine
 *    I think that this is the elegant way to do it.
 *
 * Benchmark Notes:
 *    For my tests, this routine generally takes about 123% of the time
 *    required by strstr_bsd.  I suspect most of that is routine call
 *    overhead.
 */
{
	size_t len = strlen(s2);

	if (!*s2)
		return (char *) s1;

	for (;;) {
		if (!(s1 = strchr(s1, *s2))) return NULL;
		if (!strncmp(s1, s2, len)) return (char *)s1;
		s1++;
	}
}


/*---------------------------(strstr16)-----------------------------------*/

/* Moore3b */
char *strstr16(const char *s1, const char *s2)
/*
 * Algorithm:
 *    Rely completely on str routines.
 *    Same as above routine, only we removed one possible indirection.
 *    Each time through the loop we do
 *      - one assignment
 *      - two routine calls (strchr, strncmp).
 *      - one autoincrement;
 *
 * Benchmark Notes:
 *   This routine benchmarked almost the same as the previous version.
 *   Removing the possible indirection helped not at all, or only slightly.
 *   It runs in about 122% of the time strstr_bsd for my test.
 */
{
	size_t len = strlen(s2);
	char c = *s2;

	if (!c)
		return (char *) s1;

	for (;;) {
		if (!(s1 = strchr(s1, c))) return NULL;
		if (!strncmp(s1, s2, len)) return (char *)s1;
		s1++;
	}
}

/*========================[end Dale Moore stuff]========================*/

/*---------------------------(strstr17)-----------------------------------*/
/* Boyer-Moore-Horspool */
/* See ArticlesAndMail/fromcoffin_b-m.txt for source. */
/* Only works on short strings to search so I replaced it with compiler strstr.
 * RonC
 */
extern char *strstr17(const char *s1, const char *s2);


/*---------------------------(strstr18)-----------------------------------*/
/* Microsoft Visual C/C++ v6.0, v8.0 and v10.0 source codes are same */

/***
*strstr.c - search for one string inside another
*
*       Copyright (c) 1985-1994, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines strstr() - search for one string inside another
*
*******************************************************************************/

/* #include <cruntime.h> */
/* #include <string.h> */

/***
*char *strstr(string1, string2) - search for string2 in string1
*
*Purpose:
*       finds the first occurrence of string2 in string1
*
*Entry:
*       char *string1 - string to search in
*       char *string2 - string to search for
*
*Exit:
*       returns a pointer to the first occurrence of string2 in
*       string1, or NULL if string2 does not occur in string1
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/
char *strstr18 ( 			/* removed __cdecl */
        const char * str1,
        const char * str2
        )
{
        char *cp = (char *) str1;
        char *s1, *s2;

        if ( !*str2 )
            return((char *)str1);

        while (*cp)
        {
                s1 = cp;
                s2 = (char *) str2;

                while ( *s1 && *s2 && !(*s1-*s2) )
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return(NULL);

}


/*---------------------------(strstr19)-----------------------------------*/
/* Berkeley */

char *
strstr19(const register char *string, const char *substring)
{
	register const char *a, *b;

	/* First scan quickly through the two strings looking for a
	 * single-character match.  When it's found, then compare the
	 * rest of the substring.
	 */

	b = substring;
	if (*b == 0) {
		return (char *)string;
	}
	for (; *string != 0; string += 1) {
		if (*string != *b) {
			continue;
		}
		a = string;
		while (1) {
			if (*b == 0) {
				return (char *)string;
			}
			if (*a++ != *b++) {
				break;
			}
		}
		b = substring;
	}

	return (char *)0;
}


/*---------------------------(strstr20)-----------------------------------*/
/* GNU coreutils 5.3.0 */

/* Copyright (C) 1994, 1999, 2002-2003 Free Software Foundation, Inc.
This file is part of the GNU C Library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
 * My personal strstr() implementation that beats most other algorithms.
 * Until someone tells me otherwise, I assume that this is the
 * fastest implementation of strstr() in C.
 * I deliberately chose not to comment it.  You should have at least
 * as much fun trying to understand it, as I had to write it :-).
 *
 * Stephen R. van den Berg, berg@pool.informatik.rwth-aachen.de	*/

#if HAVE_CONFIG_H
# include <config.h>
#endif

typedef unsigned chartype;

#undef strstr

char *strstr20(const char *phaystack, const char *pneedle)
{
	register const unsigned char *haystack, *needle;
	register chartype b, c;

	haystack = (const unsigned char *) phaystack;
	needle = (const unsigned char *) pneedle;

	b = *needle;
	if (b != '\0') {
		haystack--;				/* possible ANSI violation */
		do {
			c = *++haystack;
			if (c == '\0')
				goto ret0;
		} while (c != b);

		c = *++needle;
		if (c == '\0')
			goto foundneedle;
		++needle;
		goto jin;

		for (;;) {
			register chartype a;
			register const unsigned char *rhaystack, *rneedle;

			do {
				a = *++haystack;
				if (a == '\0')
					goto ret0;
				if (a == b)
					break;
				a = *++haystack;
				if (a == '\0')
					goto ret0;
			  shloop:;
			} while (a != b);

			jin:
			a = *++haystack;
			if (a == '\0')
				goto ret0;

			if (a != c)
				goto shloop;

			rhaystack = haystack-- + 1;
			rneedle = needle;
			a = *rneedle;

			if (*rhaystack == a)
				do {
					if (a == '\0')
						goto foundneedle;
					++rhaystack;
					a = *++needle;
					if (*rhaystack != a)
						break;
					if (a == '\0')
						goto foundneedle;
					++rhaystack;
					a = *++needle;
				} while (*rhaystack == a);

			needle = rneedle;	/* took the register-poor approach */

			if (a == '\0')
				break;
		}
	}
  foundneedle:
	return (char *) haystack;
  ret0:
	return 0;
}

#ifdef __cplusplus
}
#endif
