<!-- title: strstr.c Read Me -->
<!-- $Id: README.md,v 1.7 2024-02-28 15:18:14-05 ron Exp $ -->

# strstr.c

This public domain strstr returns a pointer to the first occurrence of string
s2 in string s1, or a NULL pointer if s2 does not occur in s1.  strstr returns
s1 if s2 points to a zero length string.  It duplicates the C Standard
Library's strstr's function but is faster in many cases.

This implementation of strstr is tied for speed with its nearest competitor
among 18 diverse strstr implementations when finding English words in
English text.  This strstr is 16 executable lines long versus 67 lines for
its nearest competitor, without the convoluted code of that competitor.
Its other competitors are between 10.5% and 348% slower.

```C
char *strstr(register const char *s1, register const char *s2)
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
```

Ron Charlton
