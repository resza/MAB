#ifndef _FSTRUCT_H_
#define _FSTRUCT_H_

#include "farsi.h"

//#define     AR_PRN_FNAME            "FA_PRN32.FNT"
//#define     AR_DISP_FNAME           "FA_DISP16.FNT"
#define     ARFILE_NAME            "AR_PRNDISP.FNT"

#define     ARFILE_TITLE_LEN        128
#define     ARFILE_VER_LEN          16

#define     DISP_LINE_LEN           128
#define     PRN_LINE_LEN            384

#define     TYPE_CONNECTOR          0
#define     TYPE_NONCONNECTOR       1
#define     TYPE_RHYME              2
#define     TYPE_INTERPUNCTION      3
#define     TYPE_INTER_NUM          4
#define     TYPE_CHAR               5
#define     TYPE_NUMERIC            6
#define     TYPE_EXP                7
#define     TYPE_NONE               8

#define     FORM_INDEPEND           0
#define     FORM_TERMINAL           1
#define     FORM_PREFIX             2
#define     FORM_MIDDLE             3

typedef struct
{
    uchar       width;
    uchar       height;
    ushort      unicode;
    uint        offset;
}tFontDot;

typedef struct
{
    uchar       code;
    uchar       type;
    ushort      unicode;
    tFontDot    fdot[4];
}tFontIndex;

#endif //_FSTRUCT_H_
