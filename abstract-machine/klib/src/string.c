#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  assert(s != NULL);
  // if (s == NULL)
  //   return 0;
  size_t length_cnt = 0;
  for (; s[length_cnt] != '\0'; length_cnt++)
    ;
  return length_cnt;
}

char *strcpy(char* dst,const char* src) {
  assert(dst != NULL && src != NULL);
  int i = 0;
  for (; src[i] != '\0'; i++)
    dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  assert(dst != NULL && src != NULL);
  int i = 0;
  for (; src[i] != '\0' && i < n; i++)
    dst[i] = src[i];
  for (; i < n; i++)
    dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  int i = 0;
  size_t len = strlen(dst);
  for (; src[i] != '\0'; i++)
    dst[len + i] = src[i];
  dst[len + i] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  if (s1 == NULL)
    return -1;
  if (s2 == NULL)
    return 1;
  for (int i = 0;; i++)
  {
    if (*(s1 + i) == '\0' && *(s2 + i)=='\0')
      return 0;
    else if (*(s1 + i) == '\0' || *(s1 + i) < *(s2 + i))
      return -1;
    else if (*(s2 + i) == '\0' || *(s1 + i) > *(s2 + i))
      return 1;
  }
}

int strncmp(const char* s1, const char* s2, size_t n) {
  if (s1 == NULL)
    return -1;
  if (s2 == NULL)
    return 1;
  for (int i = 0; i < n; i++)
  {
    if (*(s1 + i) == '\0' && *(s2 + i) == '\0')
      return 0;
    else if (*(s1 + i) == '\0' || *(s1 + i) < *(s2 + i))
      return -1;
    else if (*(s2 + i) == '\0' || *(s1 + i) > *(s2 + i))
      return 1;
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  assert(v != NULL && n > 0);
  for (int i = 0; i < n; i++)
  {
    char *p_v = (char *)v + i;
    *p_v = c;
  }
  return v;
}

void* memmove(void* dst,const void* src,size_t n) {
  assert(dst != NULL && src != NULL && n > 0);
  char p_tmp[100];
  // char *p_tmp;
  // p_tmp = malloc(n);
  memcpy(p_tmp, src, n);
  memcpy(dst, p_tmp, n);
  // free(p_tmp);
  return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
  // assert(out != NULL && in != NULL && n > 0);
  for (int i = 0; i < n; i++)
  {
    char *p_out = (char *)out + i;
    char *p_in = (char *)in + i;
    *p_out = *p_in;
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  assert(s1 != NULL && s2 != NULL && n > 0);
  for (int i = 0; i < n; i++)
  {
    char *p_s1 = (char *)s1 + i;
    char *p_s2 = (char *)s2 + i;
    if (*p_s1 < *p_s2)
      return -1;
    else if (*p_s1 > *p_s2)
      return 1;
  }
    return 0;
}

#endif
