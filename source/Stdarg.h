/*********************************************************************
Stdlib.h:prototypes for variable argument lists
        Defined for P60-EFT simulator
Created:
        by zy at 05/16/2001
Modified:
*********************************************************************/
#ifndef _STDARG_H
#define _STDARG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif
typedef char *va_list;
#if 0
#define va_start(ap,v) ap = (va_list)&v + sizeof(v)

#define va_arg(ap,t) (((t *)ap)++[0])
#define va_end(ap) (ap)
#endif

#define _INTSIZEOF(n) \
((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) )

#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )

#define va_arg(ap,t) \
( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )

#define va_end(ap) ( ap = (va_list)0 )

#ifdef __cplusplus
};
#endif

#endif



