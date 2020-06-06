#include "lib.h"

// https://en.wikibooks.org/wiki/C_Programming/String_manipulation#The_more_commonly-used_string_functions

/* strlen */
size_t(_strlen)(const char *s) {
  const char *p = s; /* pointer to character constant */
  /* Loop over the data in s.  */
  while (*p != '\0') p++;
  return (size_t)(p - s);
}

/* strcpy */
char *(_strcpy)(char *restrict s1, const char *restrict s2) {
  char *dst = s1;
  const char *src = s2;
  /* Do the copying in a loop.  */
  while ((*dst++ = *src++) != '\0')
    ; /* The body of this loop is left empty. */
  /* Return the destination string.  */
  return s1;
}

/* strcat */
char *(_strcat)(char *restrict s1, const char *restrict s2) {
  char *s = s1;
  /* Move s so that it points to the end of s1.  */
  while (*s != '\0') s++;
  /* Copy the contents of s2 into the space at the end of s1.  */
  _strcpy(s, s2);
  return s1;
}

/* strchr */
char *(_strchr)(const char *s, int c) {
  char ch = c;
  /* Scan s for the character.  When this loop is finished,
     s will either point to the end of the string or the
     character we were looking for.  */
  while (*s != '\0' && *s != ch) s++;
  return (*s == ch) ? (char *)s : NIL;
}

/* strcmp */
int(_strcmp)(const char *s1, const char *s2) {
  unsigned char uc1, uc2;
  /* Move s1 and s2 to the first differing characters
     in each string, or the ends of the strings if they
     are identical.  */
  while (*s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  /* Compare the characters as unsigned char and
     return the difference.  */
  uc1 = (*(unsigned char *)s1);
  uc2 = (*(unsigned char *)s2);
  return ((uc1 < uc2) ? -1 : (uc1 > uc2));
}

/* int strcmp2(const char *p1, const char *p2) { */
/*   const unsigned char *s1 = (const unsigned char *)p1; */
/*   const unsigned char *s2 = (const unsigned char *)p2; */
/*   unsigned char c1, c2; */
/*   do { */
/*     c1 = (unsigned char)*s1++; */
/*     c2 = (unsigned char)*s2++; */
/*     if (c1 == '\0') return c1 - c2; */
/*   } while (c1 == c2); */
/*   return c1 - c2; */
/* } */

/* strncat */
char *(_strncat)(char *restrict s1, const char *restrict s2, size_t n) {
  char *s = s1;
  /* Loop over the data in s1.  */
  while (*s != '\0') s++;
  /* s now points to s1's trailing null character, now copy
     up to n bytes from s2 into s stopping if a null character
     is encountered in s2.
     It is not safe to use strncpy here since it copies EXACTLY n
     characters, NULL padding if necessary.  */
  while (n != 0 && (*s = *s2++) != '\0') {
    n--;
    s++;
  }
  if (*s != '\0') *s = '\0';
  return s1;
}

/* strncmp */
int(_strncmp)(const char *s1, const char *s2, size_t n) {
  unsigned char uc1, uc2;
  /* Nothing to compare?  Return zero.  */
  if (n == 0) return 0;
  /* Loop, comparing bytes.  */
  while (n-- > 0 && *s1 == *s2) {
    /* If we've run out of bytes or hit a null, return zero
       since we already know *s1 == *s2.  */
    if (n == 0 || *s1 == '\0') return 0;
    s1++;
    s2++;
  }
  uc1 = (*(unsigned char *)s1);
  uc2 = (*(unsigned char *)s2);
  return ((uc1 < uc2) ? -1 : (uc1 > uc2));
}

/* memset */
void *(memset)(void *s, int c, size_t n) {
  unsigned char *us = s;
  unsigned char uc = c;
  while (n-- != 0) *us++ = uc;
  return s;
}

/* strncpy */
char *(_strncpy)(char *restrict s1, const char *restrict s2, size_t n) {
  char *dst = s1;
  const char *src = s2;
  /* Copy bytes, one at a time.  */
  while (n > 0) {
    n--;
    if ((*dst++ = *src++) == '\0') {
      /* If we get here, we found a null character at the end
         of s2, so use memset to put null bytes at the end of
         s1.  */
      memset(dst, '\0', n);
      break;
    }
  }
  return s1;
}
/* strrchr */
char *(_strrchr)(const char *s, int c) {
  const char *last = NIL;
  /* If the character we're looking for is the terminating null,
     we just need to look for that character as there's only one
     of them in the string.  */
  if (c == '\0') return _strchr(s, c);
  /* Loop through, finding the last match before hitting NULL.  */
  while ((s = _strchr(s, c)) != NIL) {
    last = s;
    s++;
  }
  return (char *)last;
}
/* memcmp */
int(memcmp)(const void *s1, const void *s2, size_t n) {
  const unsigned char *us1 = (const unsigned char *)s1;
  const unsigned char *us2 = (const unsigned char *)s2;
  while (n-- != 0) {
    if (*us1 != *us2) return (*us1 < *us2) ? -1 : +1;
    us1++;
    us2++;
  }
  return 0;
}

/* strstr */
char *(_strstr)(const char *haystack, const char *needle) {
  size_t needlelen;
  /* Check for the null needle case.  */
  if (*needle == '\0') return (char *)haystack;
  needlelen = _strlen(needle);
  for (; (haystack = _strchr(haystack, *needle)) != NIL; haystack++)
    if (memcmp(haystack, needle, needlelen) == 0) return (char *)haystack;
  return NIL;
}

/* memmove */
void *(_memmove)(void *s1, const void *s2, size_t n) {
  /* note: these don't have to point to unsigned chars */
  char *p1 = s1;
  const char *p2 = s2;
  /* test for overlap that prevents an ascending copy */
  if (p2 < p1 && p1 < p2 + n) {
    /* do a descending copy */
    p2 += n;
    p1 += n;
    while (n-- != 0) *--p1 = *--p2;
  } else
    while (n-- != 0) *p1++ = *p2++;
  return s1;
}
