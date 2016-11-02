
/****************************************************************************
NAME
    pack8583.h - 定义POS操作员本地管理的基本操作库函数

DESCRIPTION
    实现标准的ISO8583报文的打包/解包库函数,支持64Bit/128Bit域的ISO8583报文的
    打包/解包.

REFERENCE
    1. ISO8583-1998.doc

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2004-07-03      - created
****************************************************************************/

/*****************************************************************************
COMMENT:
   FIELD_ATTR element:
      eElementAttr:
	 Attr_n  :  numeric data('0'~'9','A'~'F')
                    each data element represents 1 nibble.
                    (2 data elements = 1 byte)
	 Attr_z  :  numeric data(0x30~0x3F)
                    each data element represents 1 nibble.
                    (2 data elements = 1 byte)
	 Attr_b  :  binary data, each data element represents 1 bit.
                    (8 data elements = 1 byte)
	 Attr_a  :  alpha-number and special characters.
                    each data element represents 1 byte.
	 Attr_UnUsed  :  the element is no used.
         Attr_Over    :  definiens over.
      eLengthAttr:
	 Attr_fix  - fixed length.
	 Attr_var1 - variable length, one byte at the beginning of the data
                 element identify the number of positions following to the
                 end of that data element.
                 (0x00 ~ 0x99)
	 Attr_var2 - variable length, two bytes at the beginning of the data
                 element identify the number of positions following to the
                end of that data element.
                (0x00 0x00 ~ 0x09 0x99)
      uiLength:
	    length of the data element.
NOTE:
   All fixed length Attr_n data elements are right justified with leading
   zeroes.
   All fixed length Attr_a data elements are left justified with trailing
   spaces.
   All fixed length Attr_b data elements are left justified with trailing
   zeroes.
   All variable length Attr_n data elements are left justified with trailing
   zero.
   There are two bytes for showing length at the beginning of all Attr_b data
   elements while the struct ST_8583 are inputed or outputed.
*****************************************************************************/

#ifndef _PACK8583_H
#define _PACK8583_H

#include "applib.h"


//#define VarAttrN_RightJustify     // Special processing: when packing variable length and packed BCD data, make it right justified.
//#define VarAttrN_PadFToOddLength    // Special processing: when attr_n data has odd length, pad 'F' at last.

#define  ERR_UNPACK_LEN     -1000         /* 解包后数据长度与实际长度不符 */

#define Attr_a			0
#define Attr_b			1
#define Attr_n			2
#define Attr_z			3
#define Attr_UnUsed		4
#define Attr_Over		5

#define Attr_var1		0
#define Attr_var2		1
#define Attr_fix		2

typedef struct _tagFIELD_ATTR
{
   uchar	eElementAttr;
   uchar	eLengthAttr;
   uint		uiLength;
}FIELD_ATTR;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int PackElement(FIELD_ATTR *pAttr, uchar *pusIn, uchar *pusOut,
                        uint *puiOutLen);
int UnPackElement(FIELD_ATTR *pAttr, uchar *pusIn, uchar *pusOut,
                        uint *puiInLen);


/****************************************************************************
 Function:          change 8583 struct to 8583 packet
 Param in:
    pDataAttr       struct array to define attribute of bitmap and
                    data elements
    pSt8583         pointer to point to struct 8583
 Param out:
    pusOut          8583 packet
    puiOutLen       bytes of 8583 packet
 Return Code:
    0               Ok
    <0              message header error
    <-1000          length of msg field overflow
    <-2000          attributes define error
****************************************************************************/
int PubPack8583(FIELD_ATTR *pDataAttr, void *pSt8583, uchar *pusOut, uint *puiOutLen);

/****************************************************************************
 Function:          change 8583 packet to 8583 struct
 Param in:
   pDataAttr        struct array to define attribute of bitmap and
   pusIn            8583 packet
   uiInLen          bytes of 8583 packet
 Param out:
   pSt8583          pointer to point to struct 8583
 Return Code:
    0               Ok
    <0              message header error
    -1000           length of unpacked not same as actural data
    <-1000          length of msg field overflow
    <-2000          attributes define error
****************************************************************************/
int PubUnPack8583(FIELD_ATTR *pDataAttr, uchar *pusIn, uint uiInLen, void *pSt8583);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _PACK8583_H */

// end of file
