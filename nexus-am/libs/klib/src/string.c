#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

/* pa2.2
 * 2020-12-6
 * implements the std::string function
 * reference the glibc
 */

size_t strlen(const char *s) {
  assert(s != NULL);
  size_t strlen = 0;
  while ((*s++) != '\0') {
    strlen++;
  }
  return strlen;
}

char *strcpy(char* dst,const char* src) {
  assert( dst != NULL && src != NULL);
  char *cp = dst;
  while ((*cp++ = *src++) != '\0');
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  assert( dst != NULL && src != NULL);
  char *cp = dst;
  while (n && (*cp++ = *src++) != '\0') {
    n--;
  }
  if (n) {
    while (n--)
    {
      *cp++ = '\0';
    }
    
  }
  return dst;
}

char* strcat(char* dst, const char* src) {
  assert( dst != NULL && src != NULL);
  char *cp = dst;
  while (*cp != '\0') {
    cp++;
  }
  while ((*cp++ = *src++) != '\0');
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  assert(s1 != NULL && s2 != NULL);
  while (*s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  if (*(unsigned char *)s1 > *(unsigned char *)s2) {
    return 1;
  }
  if (*s1 == *s2) {
    return 0;
  }
  return -1;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert(s1 != NULL && s2 != NULL);
  while (n-- && *s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  if (*(unsigned char *)s1 > *(unsigned char *)s2) {
    return 1;
  }
  if (*s1 == *s2) {
    return 0;
  }
  return -1;
}

void* memset(void* v,int c,size_t n) {
  if (v == NULL) {
    return NULL;
  }
  char *cp = (char *)v;
  while (n--) {
    *cp++ = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *src = (char *)in;
  char *dst = (char *)out;
  assert(dst <= src || dst >= src + n);
  while (n--) {
    *dst++ = *src++;
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert(s1 != NULL && s2 != NULL);
  unsigned char *cp1 = (unsigned char *)s1;
  unsigned char *cp2 = (unsigned char *)s2;
  while (n-- && *cp1 == *cp2) {
    cp1++;
    cp2++;
  }
  if (*cp1 > *cp2) {
    return 1;
  }
  if (*cp1 == *cp2) {
    return 0;
  }
  return -1;
}

#endif