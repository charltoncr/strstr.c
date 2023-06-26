<!-- title: strstr.c Read Me -->
<!-- $Id: README.md,v 1.3 2023-06-26 11:38:56-04 ron Exp $ -->

# strstr.c

This public domain strstr returns a pointer to the first occurrence of string
s2 in string s1, or a NULL pointer if s2 does not occur in s1.  strstr returns
s1 if s2 points to a zero length string.  It duplicates the C Standard
Library's strstr but is faster.

This implementation of strstr is tied for speed with its nearest competitor
among 18 diverse strstr implementations when finding English words in
English text.  This strstr is 17 executable lines long versus 67 lines for
its nearest competitor, without the convoluted code of that competitor.
Its other competitors are between 10.5% and 348% slower.

```c
char *strstr(register const char *s1, register const char *s2)
{
    register const char *p1, *p2;
    register char c;

    if (!(c = *s2++)) return (char *)s1;

    for (;;) {
        // strchr-like for loop unrolled for speed
        for (; *s1 != c; ++s1) {
            if (!*s1) return NULL;
            if (*++s1 == c) break;
            if (!*s1) return NULL;
            if (*++s1 == c) break;
            if (!*s1) return NULL;
        }
        for (p1 = ++s1, p2 = s2; (*p1 == *p2) && *p2;)
             ++p1, ++p2;
        if (!*p2)   // reached end of s2: match at s1-1
            return (char *)--s1;
    }
}
```

Ron Charlton
