/*********************************************************************
String.h:prototypes for string functions
         Defined for P60-EFT simulator
Created:
        by zy at 05/16/2001
Modified:
*********************************************************************/
#ifndef _STRING_H
#define _STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SIZE_T
typedef unsigned int size_t;
#define __SIZE_T
#endif

char *strcat(char *s1, const char *s2);
char *strncat(char *s1, const char *s2, size_t n);
int  strcmp(const char *s1, const char *s2);
int  strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *s1, const char *s2);
char *strncpy(char *s1, const char *s2, size_t n);
size_t  strlen(const char *);
char *strchr(const char *s, int c);
int  strpos(const char *s, char c);
char *strrchr(const char *s, int c);
int  strrpos(const char *s, char c);
size_t  strspn(const char *s, const char *set);
size_t  strcspn(const char *s, const char *set);
char *strpbrk(const char *s, const char *set);
int	 strcoll(const char *s1, const char *s2);
char *strerror( int errnum );
char *strstr( const char *string, const char *strCharSet );
char *strtok( char *strToken, const char *strDelimit );
size_t strxfrm( char *strDest, const char *strSource, size_t count );

int  memcmp(const void *s1,const void *s2, size_t n);
void *memcpy(void *s1, const void *s2, size_t n);
void *memchr(const void *s, int val, size_t n);
void *memccpy(void *s1, const void *s2, int val, size_t n);
void *memmove(void *s1, const void *s2, size_t n);
void *memset(void *s, int val, size_t n);

#ifdef __cplusplus
};
#endif

#endif
