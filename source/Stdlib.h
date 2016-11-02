/*********************************************************************
Stdlib.h:prototypes for standard functions
        Defined for P60-EFT simulator
Created:
        by zy at 05/16/2001
Modified:
*********************************************************************/
#ifndef _STDLIB_H
#define _STDLIB_H
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DIV_T_DEFINED

	typedef struct _div_t {
        int quot;
        int rem;
	} div_t;

	typedef struct _ldiv_t {
        long quot;
        long rem;
	} ldiv_t;

#define _DIV_T_DEFINED
#endif


float atof (void *s1);
long  atol (void *s1);
int   atoi (void *s1);
int   rand (void);
void  srand(int);
void abort( void );
int abs( int n );
//int atexit(void);

void *calloc( size_t num, size_t size );
div_t div( int numer, int denom );
void exit( int status );
void free( void *memblock );
char *getenv( const char *varname );
long labs( long n );
ldiv_t ldiv( long int numer, long int denom );
void *malloc( size_t size );
int mblen( const char *mbstr, size_t count );
int mbtowc( unsigned short *wchar, const char *mbchar, size_t count );
int wctomb( char *mbchar, unsigned short wchar );
size_t mbstowcs( unsigned short *wcstr, const char *mbstr, size_t count );
size_t wcstombs( char *mbstr, const unsigned short *wcstr, size_t count );

void *realloc( void *memblock, size_t size );
double strtod( const char *nptr, char **endptr );
long strtol( const char *nptr, char **endptr, int base );
unsigned long strtoul( const char *nptr, char **endptr, int base );
int system( const char *command );

#ifdef __cplusplus
};
#endif

#endif
