/*********************************************************************
Stdio.h:prototypes for standard i/o functions
        Defined for P60-EFT simulator
Created:
        by zy at 05/16/2001
Modified:
*********************************************************************/
#ifndef _STDIO_H
#define _STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef __SIZE_T
#define __SIZE_T
typedef unsigned int size_t;
#endif

#if     (defined(_MT) || defined(_DLL)) && !defined(_MAC)
extern int * __cdecl _errno(void);
#define errno   (*_errno())
#else   /* ndef _MT && ndef _DLL */
extern int errno;
#endif  /* _MT || _DLL */
#include <stdarg.h>
#include <stdio.h>
typedef int          ptrdiff_t;
#ifndef _FILE_DEFINED
struct _iobuf {
	char *_ptr;
	int   _cnt;
	char *_base;
	int   _flag;
	int   _file;
	int   _charbuf;
	int   _bufsiz;
	char *_tmpfname;
};
typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif

typedef struct fpos_t {
	unsigned int lopart;
	int          hipart;
        } fpos_t;

int  printf(const char *, ...);
int  sprintf(char *, const char *, ...);
int  sscanf(char *, const char *, ...);


int putc( int ch, FILE *stream );
int putchar( int ch );
int puts( char *str );
int getc( FILE *stream );
int ungetc( int ch, FILE *stream );
int getchar( void );
char *gets( char *str );
int scanf( const char *format, ... );

void perror( const char *str );
void clearerr( FILE *stream );


int vprintf( char *format, va_list arg_ptr );
int vfprintf( FILE *stream, const char *format, va_list arg_ptr );
int vsprintf( char *buffer, char *format, va_list arg_ptr );

void setbuf( FILE *stream, char *buffer );
int setvbuf( FILE *stream, char *buffer, int mode, size_t size );

FILE *fopen( const char *fname, const char *mode );
FILE *freopen( const char *fname, const char *mode, FILE *stream );
int fclose( FILE *stream );
int fread( void *buffer, size_t size, size_t num, FILE *stream );
int fwrite( const void *buffer, size_t size, size_t count, FILE *stream );
int fgetc( FILE *stream );
char *fgets( char *str, int num, FILE *stream );
int fputs( const char *str, FILE *stream );
int fscanf( FILE *stream, const char *format, ... );
int fprintf( FILE *stream, const char *format, ... );
int fgetpos( FILE *stream, fpos_t *position );
int fsetpos( FILE *stream, const fpos_t *position );
int fseek( FILE *stream, long offset, int origin );
long ftell( FILE *stream );
int feof( FILE *stream );
int ferror( FILE *stream );
int fflush( FILE *stream );

void rewind( FILE *stream );
char *tmpnam( char *name );
FILE *tmpfile( void );


#ifdef __cplusplus
};
#endif

#endif
