/*********************************************************************
Ctype.h:prototypes for character functions
        Defined for P60-EFT simulator
Created:
        by zy at 05/16/2001
Modified:
*********************************************************************/
#ifndef _CTYPE_H
#define _CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

int isalpha(int);
int isupper(int);
int islower(int);
int isdigit(int);
int isxdigit(int);
int isspace(int);
int ispunct(int);
int isalnum(int);
int isprint(int);
int isgraph(int);
int iscntrl(int);
int toupper(int);
int tolower(int);

#define _tolower(c) ((c)-'A'+'a')
#define _toupper(c) ((c)-'a'+'A')
#define toascii(c)  ((c) & 0x7F)

#ifdef __cplusplus
};
#endif

#endif
