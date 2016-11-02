/************************************************************************
    Standard definitions.

    prtdiff_t	signed integral type of result of subtracing two pointers
    size_t	unsigned integral type of the result of sizeof
    wchar_t	integral type which can represent all characters in any
		    supported extended character set

    NULL	a null pointer constant

    offsetof(type,member)
		expands to a type size_t expression representing the
		offset in bytes from the beginning of a structure of
		type "type" to the named "member".
************************************************************************/
#ifndef __STDDEF
#define __STDDEF

#ifdef __cplusplus
extern "C" {
#endif

//typedef _ptrdiff_t	ptrdiff_t;

#ifndef __SIZE_T
#define __SIZE_T
#ifdef _INTBITS
    typedef _size_t		size_t;
#else
    typedef unsigned int	size_t;
#endif
#endif

//#ifndef __WCHAR_T
//#define __WCHAR_T
//typedef char		wchar_t;
//#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define offsetof(t,m)	((size_t)(((char*)&(((t*)0)->m))-(char*)0))


#ifdef __cplusplus
};
#endif

#ifndef __cplusplus
	#ifndef __WCHAR_T
	#define __WCHAR_T
	typedef char		wchar_t;
	#endif
#endif

#endif
