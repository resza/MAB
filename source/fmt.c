/** \file
 * Formatting functions
 *
 * Subversion reference
 *
 * $HeadURL: https://svn.ingenico.com/UTA/core/utaYYMMDD/pri/fmt.c $
 *
 * $Id: fmt.c 2616 2010-04-07 11:16:41Z abarantsev $
 */

#include "global.h"

//#undef trcOpt
//static const card trcOpt = TRCOPT & BIT(trcFMT);
//#define trcOpt (TRCOPT & BIT(trcECR))

//Copy into dst substring of src starting from ofs of length 
//return number of character copied (len)
/** Extract a substring dst from the string src of length len starting from the position ofs.
 * The function returns the length of the string extracted, strlen(dst).
 * \param dst (O) Destination buffer for the substring extracted
 * \param       src (I) Original string
 * \param       ofs (I) Starting position of the substring to be extracted
 * \param       len (I) Number of characters in the destination
 * \pre 
 *    - dst!=0
 *    - src!=0
 * \return  
 *    - strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
*/
int fmtSbs(char *dst, const char *src, byte ofs, byte len) {
    memcpy(dst, src + ofs, len);
    dst[len] = 0;

    return strlen(dst);
}

/** Extract a token dst from the string src that can contain several tokens separated
 * by characters from sep.
 * The function returns the length of the string extracted, strlen(dst).
 * If dst is zero the characters are counted only; they are not copied into dst.
 * \param dst (O) Destination buffer for the token extracted; if zero, the characters are counted only
 * \param src (I) Original string
 * \param sep (I) String containing token separators
 * \pre 
 *    - src!=0
 *    - sep!=0
 * \return  
 *    - strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
*/
int fmtTok(char *dst, const char *src, const char *sep) {
    int len;
    const char *ptr;

    len = 0;
    while(*src) {
        ptr = sep;
        while(*ptr) {
            if(*src == *ptr)
                break;
            ptr++;
        }
        if(*src == *ptr)
            break;
        if(dst)
            *dst++ = *src;
        src++;
        len++;
    }
    if(dst)
        *dst = 0;
    return len;
}

/** Pad the string str to length abs(len) by the character chr.
 * It is right padded if len>0 and left padded otherwise.
 * The function returns the length of the resulted padded string, strlen(str).
 * \param str (I/O) The original input string; the padded output string replaces it
 * \param len (I) Number of characters in the padded string.
 *        The negative value indicates left padding, the positive value provides
 *        right padding.
 *        It is the absolute value abs(len) that is the real length of resulting string
 * \param chr (I) Pad character
 * \pre 
 *    - str!=0
 *    - len!=0
 * \return  
 *    - the length of the result string strlen(str)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
*/
int fmtPad(char *str, int len, char chr) {
    int dim;                    //the initial string length
    char *ptr;                  //points to the start of padding
    char dir;                   //pad direction (left or right)

    dir = 'r';
    if(len < 0) {               //left pad?
        dir = 'l';
        len = -len;
    }
    str[len] = 0;

    dim = strlen(str);
    if(dim >= len)
        return dim;             //string is long enough - nothing to pad

    ptr = str;                  //ptr will pont to the start of padding
    if(dir == 'l')
        memmove(str + len - dim, str, dim); //left padding: move characters to the right
    else
        ptr += dim;             //right padding: move the padding pointer to the end of the string

    memset(ptr, chr, len - dim);    //now pad dim characters chr starting from ptr

    return strlen(str);
}

/** Center the string str to length abs(len) by the character chr.
 * The function returns the length of the resulted padded string, strlen(str).
 * Before padding left and right, all leading and trailing characters chr are removed.
 * \param str (I) The original input string; the padded output string replaces it
 * \param len (I) number of characters in the centered string. 
 * \param       chr (I) pad character
 * \pre 
 *    - str!=0
 *    - len!=0
 * \return  
 *    - strlen(str)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
*/
int fmtCnt(char *str, int len, char chr) {
    byte idx;
    char *ptr;

    idx = strlen(str);
    while(idx--) {              //remove trailing chr
        if(str[idx] != chr)
            break;
        str[idx] = 0;
    }

    idx = 0;
    while(str[idx] == chr)
        idx++;                  //calculate leading chr

    if(idx) {
        ptr = str;
        while(str[idx]) {
            *ptr++ = str[idx++];
        }
        *ptr = 0;
    }

    idx = strlen(str);
    while(idx < len) {
        if(idx % 2) {
            str[idx++] = chr;
            str[idx++] = 0;
        } else {
            str[idx + 1] = 0;
            while(idx) {
                str[idx] = str[idx - 1];
                idx--;
            }
            *str = chr;
        }
        idx = strlen(str);
    }
    return len;
}

/** Format an amount string with decimals.
 * \param dst (O) Destination string to put a formatted amount
 * \param src (I) Original string containing decimal digits
 * \param exp (I) number of decimals, can be 0 or 2 or 3
 * \param sep (I) separators, a string containing up to 2 characters
 *  - decimal separator
 *  - thousands separator
 * \pre 
 *    - dst!=0
 *    - src!=0
 *    - The original string contains decimal digits only
 * \return  
 *    - the length of the resulting string strlen(dst)
 * If sep is zero the default value "." is used.
 * It means that decimals are separated by '.', and thousands are not separated.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c, tcgs0017.c
*/
int fmtAmt(char *dst, const char *src, byte exp, const char *sep) {
    int ret;
    int len;
    char tmp[prtW + 1];
    char _dst[prtW + 1];
    char decSep[2] = { '.', 0 };    //decimal separator
    char thsSep[2] = { 0, 0 };  //thousand separator
    byte isSign = 0;

    memset(_dst, 0, prtW + 1);
    if(*src == '-' || *src == '+') {
        char sign[2] = { 0, 0 };
        isSign = 1;
        sign[0] = *src;
        strcpy(_dst, sign);
        src++;
    }

    ret = fmtTok(0, src, "123456789");
    src += ret;
    len = strlen(src);

    memset(tmp, 0, sizeof(tmp));
    if(len) {
        if(len <= exp) {
            memset(tmp, '0', exp - len + 1);
            strcat(tmp, src);
        } else
            strcpy(tmp, src);
    } else {
        memset(tmp, '0', exp + 1);
    }
    len = strlen(tmp);

    if(sep && sep[0]) 
    {
        decSep[0] = sep[0];
        if(sep[1]) 
	{
            thsSep[0] = sep[1];
        } 
	else 
	{
        }
    }

    if(exp && exp < len) {
        fmtIns(tmp, tmp, decSep, (byte) (len - exp), prtW);
        len++;
    }
    if(thsSep[0]) {
        for (len -= 3 + (exp ? (exp + 1) : 0); len > 0; len -= 3) {
            ret = fmtIns(tmp, tmp, thsSep, (byte) len, prtW);
        }
    }
#ifdef __CURR__
    if(exp) {
        strcat(_dst, __CURR__);
    }
#endif
    strcat(_dst, tmp);          //concatenate the prefix and the decimals
    strcpy(dst, _dst);
    return strlen(dst);
}

/** Permutation of fgd and bgd according to the control string ctl. 
 * The control string ctl contains uppercase and lowercase Latin letters. 
 * Lowercase letters 'a'..'z' are associated with correspondent characters of fgd string:
 * 'a' with fgd[0], 'b' with fgd[1] etc.
 * In the same way uppercase letters 'A'..'Z' are associated with correspondent characters
 * of bgd string: 'A' with bgd[0], 'B' with bgd[1] etc.
 * The function converts ctl into dst replacing upper- and lower-case control characters
 * by related characters from bgd and fgd. 
 * 
 *For example the input strings :
 * - fgd: "20040902164700"
 * - bgd: "-: "
 * - ctl: "ghAefAabcdCCijBklBmn"
 *
 * provide the result:
 * - dst: "02-09-2004  16:47:00"
 *
 * \param dst (O) Destination string for permutation
 * \param fgd (I) Foreground string
 * \param bgd (I) Background string
 * \param ctl (I) Control string
 * \pre 
 *    - dst!=0
 *    - fgd!=0
 *    - bgd!=0
 *    - ctl!=0
 *    - The destination buffer should be big enough to accept the result (greater than or equal to strlen(ctl)+1)       
 * \return
 *    - the length of the resulting string strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
*/
int fmtMut(char *dst, const char *fgd, const char *bgd, const char *ctl) {
    byte idx;
    byte ref;
    byte len;

    idx = 0;
    len = 0;
    while(*ctl) {
        if(('a' <= (*ctl)) && ((*ctl) <= 'z')) {    //foreground index
            ref = (*ctl) - 'a';
            if(strlen(fgd) <= ref)
                dst[idx] = ' '; //fgd too small
            else
                dst[idx] = fgd[ref];
        } else if(('A' <= (*ctl)) && ((*ctl) <= 'Z')) { //background index
            ref = (*ctl) - 'A';
            if(strlen(bgd) <= ref)
                break;          //bgd too short
            else
                dst[idx] = bgd[ref];
        } else {
            if(strlen(fgd) <= len)
                break;          //fgd too short
            else
                dst[idx] = *ctl;
        }
        idx++;
        ctl++;
        len++;
    }
    dst[idx] = 0;               //add ending zero
    return idx;
}

/** Insert a string to an another sting
 * \param dst (O) Destination string
 * \param bgd (I) the string where you insert the another string
 * \param ins (I) the string, what you insert
 * \param ofs (I) starting point for inserting in bgd
 * \param len (I) maximal length of the destenation string 
 *        (0 means the length will be the sum of the bgd and ins)
 * \pre 
 *    - dst!=0
 *    - src!=0
 * \return  
 *    - the length of the resulting string strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcgs0007.c
*/
int fmtIns(char *dst, const char *bgd, const char *ins, byte ofs, int len) {
    word idx;
    int lenIns;

    if(len == 0)
        len = strlen(bgd) + strlen(ins);
    lenIns = strlen(ins);
    lenIns = (lenIns < len - ofs) ? lenIns : (len - ofs);

    memcpy(dst, bgd, ofs);
    for (idx = len - 1; idx >= ofs + lenIns; idx--)
        memcpy(dst + idx, bgd + idx - lenIns, 1);
    memcpy(dst + ofs, ins, lenIns);
    memset(dst + len, 0, 1);

    return strlen(dst);
}

/** Erase the len long substring from ofs
 * \param dst (O) Destination string
 * \param src (I) Original string
 * \param ofs (I) starting point for erasing
 * \param len (I) number of erased characters
 * \pre 
 *    - dst!=0
 *    - src!=0
 * \return  
 *    - the length of the resulting string strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcgs0007.c
*/
int fmtDel(char *dst, const char *src, byte ofs, int len) {
    word idx;

    if((word) (ofs + len) >= strlen(src)) {
        memcpy(dst, src, ofs);
        dst[ofs] = 0;
        return strlen(dst);
    }
    memcpy(dst, src, ofs);
    for (idx = ofs + len; idx <= strlen(src); idx++) {
        memcpy(dst + idx - len, src + idx, 1);
    }
    return strlen(dst);
}

/** Format an Host IP Address string with ".".
 * \param dst (O) Destination string to put a formatted Address
 * \param src (I) Original string 
 * \param len (I) length of the Original string without null pointer
 * \pre 
 *    - src!=0
 *    - The original string contains decimal digits only
 * \return  
 *    - the length of the resulting string strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
*/

int fmtTcp(char *dst, char *src, int len) {
    int dim;

    for (dim = 0; dim < len; dim++) // Verify the size of the address
    {
        if(dim != 0 && (dim + 1) % 4 == 0 && *src != '.') {
            *dst = '.';         // Add a dot every 4 inputs - the index starts at 0
            len++;
        } else {
            *dst = *src;
            src++;
        }

        dst++;
    }
    *dst = 0;
    dim--;
    return dim;
}

int fmtDate(char *dst, char *src, int len) {
    int dim;

    for (dim = 0; dim < len; dim++) // Verify the size of the address
    {
        if(dim != 0 && (dim + 1) % 3 == 0 && *src != '/') {
            *dst = '/';         // Add a dot every 4 inputs - the index starts at 0
            len++;
        } else {
            *dst = *src;
            src++;
        }

        dst++;
    }
    *dst = 0;
    dim--;
    return dim;
}

int fmtTime(char *dst, char *src, int len) {
    int dim;

    for (dim = 0; dim < len; dim++) // Verify the size of the address
    {
        if(dim != 0 && (dim + 1) % 3 == 0 && *src != ':') {
            *dst = ':';         // Add a dot every 4 inputs - the index starts at 0
            len++;
        } else {
            *dst = *src;
            src++;
        }

        dst++;
    }
    *dst = 0;
    dim--;
    return dim;
}
