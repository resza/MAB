/** \file
 * Derivated Data Elements
 *
 * Subversion reference
 *
 * $HeadURL: https://svn.ingenico.com/UTA/core/utaYYMMDD/pri/drv.c $
 *
 * $Id: drv.c 2217 2009-03-20 14:52:31Z abarantsev $
 */

#include <string.h>
#include <stdio.h>
#include "pri.h"

#undef trcOpt
//static const card trcOpt = TRCOPT & BIT(trcDRV);
#define trcOpt (TRCOPT & BIT(trcDRV))

/** Container constructor: 
 * Given an existing table or record or converter or combo pointed to by ptr create a container of related type typ.
 * \param cnt (O) Pointer to the container to be initialized
 * \param typ (I) Container type:
 *  - 't': table
 *  - 'r': record
 *  - 'c': converter
 *  - 'x': combo
 *  - 's': string
 * \param ptr (I) Pointer to the container body that depends on the value of typ argument
 *  - 't': tTable
 *  - 'r': tRecord
 *  - 'c': tConvert
 *  - 'x': tCombo
 *  - 's': string
 * \pre 
 *  - cnt!=0
 *  - ptr!=0
 *  - typ is one of 't'|'r'|'c'|'x'|'s'
 * \return 1
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0032.c, tcab0033.c
*/
int cntInit(tContainer * cnt, char typ, void *ptr) {
    cnt->typ = typ;
    switch (typ) {
      case 't':
          cnt->ptr.tab = (tTable *) ptr;
          break;
      case 'r':
          cnt->ptr.rec = (tRecord *) ptr;
          break;
      case 'c':
          cnt->ptr.cvt = (tConvert *) ptr;
          break;
      case 'x':
          cnt->ptr.cmb = (tCombo *) ptr;
          break;
      default:
          cnt->ptr.str = (char *) ptr;
          break;
    }
    return 1;
}

/** Return a length of the data corresponding to the key
 * \param cnt (I) Pointer to the container
 * \param key (I) key value. It can be
 *  - row number if the container is built around a table
 *  - field number if the container is built around a record
 *  - 0 if the container is built around a converter (ignored)
 *  - 0 if the container is built around a combo (ignored)
 * \pre 
 *  - cnt!=0
 *  - buf!=0
 * \return length of the related data element
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0032.c, tcab0033.c
*/
word cntLen(const tContainer * cnt, word key) {
    word len;

    
    switch (cnt->typ) {
      case 't':
          len = tabWdt(cnt->ptr.tab);
          break;
      case 'r':
          len = cnt->ptr.rec->len[key];
          break;
      case 'c':
          len = cvtLen(cnt->ptr.cvt);
          break;
      case 'x':
          len = cmbLen(cnt->ptr.cmb, key);
          break;
      default:
          
          len = strlen(cnt->ptr.str);
          break;
    }
    return len;
}

/** Retrieve data associated to the key into memory of size len pointed to by buf
 * It is a virtual function.
 * The related function of the encapsulated data structure is called.
 * \param cnt (I) Pointer to the container
 * \param key (I) key value (its meaning depends of the encapsulated data structure)
 * \param buf (O) Pointer to the buffer where to get the value
 * \param len (I) Length of the memory reserved in buf
 * \pre 
 *  - cnt!=0
 *  - buf!=0
 * \return length of the related data element
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0032.c, tcab0033.c
*/
int cntGet(const tContainer * cnt, word key, void *buf, word len) {
    int ret;

    switch (cnt->typ) {
      case 't':
          ret = tabGet(cnt->ptr.tab, key, buf, len);
          break;
      case 'r':
          ret = recGet(cnt->ptr.rec, key, buf, len);
          break;
      case 'c':
          ret = cvtGet(cnt->ptr.cvt, key, buf, len);
          break;
      case 'x':
          ret = cmbGet(cnt->ptr.cmb, key, buf, len);
          break;
      default:
          strncpy((char *) buf, cnt->ptr.str, len);
          ret = len;
          break;
    }
    //trcFN("cntGet: key=%d ",key);
    //trcBAN(buf,len);
    return ret;
}

/** Save data of length len from the buffer buf into space reserved for the key
 * It is a virtual function.
 * The related function of the encapsulated data structure is called.
 * \param cnt (I) Pointer to the container
 * \param key (I) key value (its meaning depends of the encapsulated data structure)
 * \param buf (I) Pointer to the buffer containing the data to save
 * \param len (I) Length of the data in buf
 * \pre 
 *  - cnt!=0
 *  - buf!=0
 * \return length of the related data element
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0032.c, tcab0033.c
*/
int cntPut(const tContainer * cnt, word key, const void *buf, word len) {
    int ret;

    switch (cnt->typ) {
      case 't':
          ret = tabPut(cnt->ptr.tab, key, buf, len);
          break;
      case 'r':
          ret = recPut(cnt->ptr.rec, key, buf, len);
          break;
      case 'c':
          ret = cvtPut(cnt->ptr.cvt, key, buf, len);
          break;
      case 'x':                //this operation has no sense here
          ret = -1;
          break;
      default:
          strncpy(cnt->ptr.str, (char *) buf, len);
          ret = len;
          break;
    }
    return ret;
}

/** Converter constructor: 
 * Associate a field fld of container cnt with internal database format dbf and external format fmt.
 * \param cvt (O) Pointer to the converter to be initialized
 * \param cnt (I) Encapsulated container
 * \param fld (I) Key to access the field in the encapsulated container:
 *  - row number if it is a table
 *  - field number if it is a record
 *  - relayed if it is a converter or combo
 * \param dbf (I) Data format of the data element fld of the container cnt (external)
 * \param fmt (I) Interanl data format: as it is seen from external world:
 *  - it is the format of the input value if cvtPut is called 
 *  - it is the resulting format of the output if cvtGet is called
 * \param len (I) Length of the data element in external format
 * \pre 
 *  - cvt!=0
 *  - cnt!=0
 *  - fld should be valid for the container cnt
 *  - fmtBeg < dbf < fmtEnd
 *  - fmtBeg < fmt < fmtEnd
 * \return 1
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0032.c
*/
int cvtInit(tConvert * cvt, const tContainer * cnt, word fld, byte dbf,
            byte fmt, word len) {
    cvt->cnt = cnt;
    cvt->fld = fld;
    cvt->dbf = dbf;
    cvt->fmt = fmt;
    cvt->len = len;
    return 1;
}

/** Return a length of the data element in external format
 * \param cvt (I) Pointer to the converter
 * \pre 
 *  - cvt!=0
 * \return length of the data element in external format
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0032.c
*/
int cvtLen(const tConvert * cvt) {
    return cvt->len;
}

//data sizes
#define SB sizeof(byte)
#define SW sizeof(word)
#define SC sizeof(card)

static void cpy2Num(void *dst, word ld, card src) {
    byte aByte;
    word aWord;
    const void *ptr;

    switch (ld) {
      case SB:
          aByte = (byte) src;
          ptr = &aByte;
          break;
      case SW:
          aWord = (word) src;
          ptr = &aWord;
          break;
      default:
          ptr = &src;
          break;
    }
    memcpy(dst, ptr, ld);
}

static card cpyNum2Card(const void *src, word ls) {
    byte aByte;
    word aWord;
    card aCard;

    switch (ls) {
      case SB:
          memcpy(&aByte, src, ls);
          aCard = aByte;
          break;
      case SW:
          memcpy(&aWord, src, ls);
          aCard = aWord;
          break;
      default:
          memcpy(&aCard, src, ls);
          break;
    }
    return aCard;
}

static int cvtDecNum(void *dst, word ld, const void *src, word ls) {
    int ret;
    card buf;

    ret = dec2num(&buf, (char *) src, (byte) ls);
    cpy2Num(dst, ld, buf);
    return ret;
}

static int cvtHexNum(void *dst, word ld, const void *src, word ls) {
    int ret;
    card buf;

    ret = hex2num(&buf, (char *) src, (byte) ls);
    cpy2Num(dst, ld, buf);
    return ret;
}

static int cvtBinNum(void *dst, word ld, const void *src, word ls) {
    int ret;
    card buf;

    ret = bin2num(&buf, (byte *) src, (byte) ls);
    cpy2Num(dst, ld, buf);
    return ret;
}

static int cvtNumDec(void *dst, word ld, const void *src, word ls) {
    card buf;

    buf = cpyNum2Card(src, ls);
    return num2dec((char *) dst, buf, (byte) ld);
}

static int cvtNumHex(void *dst, word ld, const void *src, word ls) {
    card buf;

    buf = cpyNum2Card(src, ls);
    return num2hex((char *) dst, buf, (byte) ld);
}

static int cvtNumBin(void *dst, word ld, const void *src, word ls) {
    card buf;

    buf = cpyNum2Card(src, ls);
    return num2bin((byte *) dst, buf, (byte) ld);
}

static int cvtStrBin(void *dst, word ld, const void *src, word ls) {
    return hex2bin((byte *) dst, (char *) src, (byte) ld);
}

static int cvtBinStr(void *dst, word ld, const void *src, word ls) {
    if(ld) {
        return bin2hex((char *) dst, (byte *) src, (byte) ls);
    } else {
        ls = *(byte *) src;
        return bin2hex((char *) dst, (byte *) src + 1, (byte) ls);
    }
}

static int cvtBinDec(void *dst, word ld, const void *src, word ls) {
    card num;

    if(ld) {
        bin2num(&num, src, (byte) ls);
        return num2dec((char *) dst, num, (byte) ld);
    } else {
        ls = *(byte *) src;
        bin2num(&num, (byte *) src + 1, (byte) ls);
        return num2dec((char *) dst, num, 0);
    }
}

static int cvt2num(void *dst, word ld, const void *src, byte fs, word ls) {
    switch (fs) {
      case fmtNum:
          break;                //num->num: impossible
      case fmtDec:
          return cvtDecNum(dst, ld, src, ls);
      case fmtHex:
          return cvtHexNum(dst, ld, src, ls);
      case fmtBin:
          return cvtBinNum(dst, ld, src, ls);
      default:
          break;
    }
    return -1;
}

static int cvt2dec(void *dst, word ld, const void *src, byte fs, word ls) {
    switch (fs) {
      case fmtNum:
          return cvtNumDec(dst, ld, src, ls);
      case fmtDec:
          break;                //dec->dec: impossible
      case fmtHex:
          break;                //not implemented; return cvtHexDec(dst,ld,src,ls);
      case fmtBin:
          return cvtBinDec(dst, ld, src, ls);
      default:
          break;
    }
    return -1;
}

static int cvt2hex(void *dst, word ld, const void *src, byte fs, word ls) {
    switch (fs) {
      case fmtNum:
          return cvtNumHex(dst, ld, src, ls);
      case fmtDec:
          break;                //not implemented; return cvtDecHex(dst,ld,src,ls);
      case fmtHex:
          break;                //hex->hex: impossible
      case fmtBin:
          return cvtBinStr(dst, ld, src, ls);
      default:
          break;
    }
    return -1;
}

static int cvt2bin(void *dst, word ld, const void *src, byte fs, word ls) {
    switch (fs) {
      case fmtNum:
          return cvtNumBin(dst, ld, src, ls);
      case fmtDec:
          return cvtStrBin(dst, ld, src, ls);
      case fmtHex:
          return cvtStrBin(dst, ld, src, ls);
      case fmtBin:
          break;                //bin->bin: impossible
      default:
          break;
    }
    return -1;
}

static int convert(void *dst, byte fd, word ld, const void *src, byte fs,
                   word ls) {
    switch (fd) {
      case fmtNum:
          return cvt2num(dst, ld, src, fs, ls);
      case fmtDec:
          return cvt2dec(dst, ld, src, fs, ls);
      case fmtHex:
          return cvt2hex(dst, ld, src, fs, ls);
      case fmtBin:
          return cvt2bin(dst, ld, src, fs, ls);
      default:
          break;
    }
    return -1;
}

/** Save the data from buffer buf of length len into related field of the converter after perofrming the conversion.
 * The data element is converted from fmt into dbf format before saving.
 * \param cvt (I) Pointer to the converter
 * \param key (I) key value; if NULLKEY, the key value given by cvtInit is used instead
 * \param buf (I) Pointer to the buffer containing the data to save
 * \param len (I) Length of the data in buf
 * \pre 
 *  - cvt!=0
 *  - buf!=0
 * \return length of the data element in internal format
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0032.c
*/
int cvtPut(const tConvert * cvt, word key, const void *buf, word len) {
    int ret;
    word size;                  //size of data in data base
    byte tmp[512];

    if(key == NULLKEY)
        key = cvt->fld;
    if(!len)
        len = strlen((char *) buf);
    size = cntLen(cvt->cnt, cvt->fld);
    ret = convert(tmp, cvt->dbf, size, buf, cvt->fmt, cvt->len);

    ret = cntPut(cvt->cnt, key, tmp, size);

    return ret;
}

/** Retrieve the field into buffer buf of length len in the external format.
 * The data element is converted from dbf into fmt format before returning.
 * \param cvt (I) Pointer to the converter
 * \param key (I) key value; if NULLKEY, the key value given by cvtInit is used instead
 * \param buf (I) Pointer to the destination buffer where to put the value
 * \param len (I) Length of the data in buf
 * \pre 
 *  - cvt!=0
 *  - buf!=0
 * \return length of the data element in external format
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0032.c
*/
int cvtGet(const tConvert * cvt, word key, void *buf, word len) {
    int ret;
    word size;                  //size of data in data base
    byte tmp[512];

    if(key == NULLKEY)
        key = cvt->fld;
    if(!len)
        len = cvt->len;
    
    ret = cntGet(cvt->cnt, key, tmp, 0);
    
    size = cntLen(cvt->cnt, cvt->fld);
    ret = convert(buf, cvt->fmt, cvt->len, tmp, cvt->dbf, size);

    //trcFN("cvtGet: key=%d ",key);
    //trcBAN(buf,len);
    return ret;
}

/** Combo constructor: 
 * Defines a combo data element calculated as the result of operation opr on arguments arg.
 * \param cmb (O) Pointer to the combo to be initialized
 * \param opr (I) operation format
 * \param arg (I) the array of locators of the combo arguments
 * \param len (I) Length of the resulting data element
 * \pre 
 *  - cmb!=0
 *  - arg!=0
 *  - oprBeg < opr < oprEnd
 * \return 1
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0033.c
*/
int cmbInit(tCombo * cmb, byte opr, const tLocator * arg, word len) {
    byte dim;
    byte idx;

    memset(cmb, 0, sizeof(tCombo));
    cmb->opr = opr;
    dim = 0;
    switch (opr) {              //find the number of arguments for an operation
      case oprCat:
          dim = 3;
          break;                //str1,str2
      case oprSbs:
          dim = 3;
          break;                //src,ofs,num
      case oprTok:
          dim = 2;              //scr,sep
          break;
      case oprPad:
          dim = 3;
          break;                //src,how,ctl
      case oprMut:
          dim = 3;
          break;                //fgd,bgd,ctl
      case oprSpr:
          dim = 3;
          break;                //str,par1,par2
      case oprAdd:
          dim = 2;              //arg1,arg2
          break;
      case oprSub:
          dim = 2;              //arg1,arg2
          break;
      case oprIf:
          dim = 3;              //cnd,val1,val2
          break;
      case oprEq:
          dim = 3;              //val1, val2
          break;
      case oprOr:
          dim = 3;              //cnd1, cnd2, cnd3
          break;
      default:
          dim = 3;              //src,exp,sep
          break;
    }
    for (idx = 0; idx < dim; idx++) {
        cmb->arg[idx].cnt = arg[idx].cnt;
        cmb->arg[idx].key = arg[idx].key;
    }
    cmb->len = len;
    return 1;
}

/** Return a length of the output combo data element
 * \param cmb (I) Pointer to the combo
 * \param key (I) key value
 * \pre 
 *  - cmb!=0
 * \return length of the output combo data element
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0033.c
*/
int cmbLen(const tCombo * cmb, word key) {
    return cmb->len;
}

#define ARGLEN 128
static int cmbGetArgStr(void *dst, const tLocator * loc, word key) {
    int ret;

    if(!loc->cnt)               //argument is missing
        return 0;

    if(key == NULLKEY)
        key = loc->key;         //key value is prioritaire
    ret = cntGet(loc->cnt, key, dst, ARGLEN);
    return ret;
}

static int cmbGetArg(void *dst, const tLocator * loc, word key, word len) {
    int ret;

    if(key == NULLKEY)
        key = loc->key;         //key value is prioritaire
    if(loc->cnt == 0) {         //arg is not in database, it is key
        if(len >= SW) {
            memcpy(dst, &key, SW);
            ret = SW;
        } else {
            *(byte *) dst = (byte) key;
            ret = SB;
        }
    } else {                    //retrieve argument
        ret = cntGet(loc->cnt, key, dst, len);
    }
    return ret;
}

static int cmbGetCat(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char dst[ARGLEN];
    char str1[ARGLEN];
    char str2[ARGLEN];
    char how;                   //empty value means concatenation, otherwise it is a padding character

    ret = cmbGetArgStr(str1, &cmb->arg[0], key);    //only the first arg can be accessed using a key here
    CHECK(ret >= 0, lblKO);
    ret = cmbGetArgStr(str2, &cmb->arg[1], NULLKEY);
    CHECK(ret >= 0, lblKO);
    ret = cmbGetArg(&how, &cmb->arg[2], NULLKEY, SB);
    CHECK(ret > 0, lblKO);

    if(how == 0) {
        // normal concatenation
        strcpy(dst, str1);
        strcat(dst, str2);
        
        memcpy(buf, dst, len);
        buf[len - 1] = 0;

        return strlen(buf);
    } else {
        int iLeftEnd;
        int iRightEnd;
        word dim;

        dim = cmbLen(cmb, NULLKEY) - 1;
        if(len >= dim)
            len = dim;

        iLeftEnd = strlen(str1);
        while(str1[iLeftEnd - 1] == how)
            iLeftEnd--;
        
        iRightEnd = strlen(str2);
        while(str2[--iRightEnd] == how)
            str2[iRightEnd] = 0;

        fmtPad(str2, -len, how);

        memcpy(buf, str1, iLeftEnd);
        memcpy(buf + iLeftEnd, str2 + iLeftEnd, len - iLeftEnd);
        buf[len] = 0;

        return len;
    }
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetSbs(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char src[ARGLEN];
    char dst[ARGLEN];
    byte ofs;
    byte num;

    ret = cmbGetArgStr(src, &cmb->arg[0], key); //only the first arg can be accessed using a key here
    CHECK(ret > 0, lblKO);
    ret = cmbGetArg(&ofs, &cmb->arg[1], NULLKEY, SB);
    CHECK(ret > 0, lblKO);
    ret = cmbGetArg(&num, &cmb->arg[2], NULLKEY, SB);
    CHECK(ret > 0, lblKO);
    ret = fmtSbs(dst, src, ofs, num);
    
    memcpy(buf, dst, len);
    buf[len - 1] = 0;

    return ret;
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetPad(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char dst[ARGLEN];
    char src[ARGLEN];
    char ctl[ARGLEN];
    int wdt;
    char how;                   //'L'= left, 'R'= right
    char pad;                   //pad charcter

    ret = cmbGetArgStr(src, &cmb->arg[0], key); //only the first arg can be accessed using a key here
    CHECK(ret > 0, lblKO);
    ret = cmbGetArg(&how, &cmb->arg[1], NULLKEY, SB);
    CHECK(ret > 0, lblKO);
    ret = cmbGetArgStr(ctl, &cmb->arg[2], NULLKEY);
    CHECK(ret > 0, lblKO);
    wdt = strlen(ctl);
    strcpy(dst, src);
    pad = 0;

    switch (how) {
      case 'L':
      case 'l':
          pad = ctl[0];
          ret = fmtPad(dst, -(int) wdt, pad);
          break;
      case 'R':
      case 'r':
          pad = ctl[wdt - 1];
          ret = fmtPad(dst, (int) wdt, pad);
          break;
      case 'C':
      case 'c':
          pad = ctl[0];
          ret = fmtPad(dst, (int) (wdt + strlen(src)) / 2, pad);
          CHECK(ret > 0, lblKO);
          ret = fmtPad(dst, -(int) wdt, pad);
          break;
      case 'S':
      case 's':                // substitute
          pad = ctl[0];
          for (ret = 0; src[ret]; ret++)
              dst[ret] = pad;
          dst[ret] = 0;
          if(ret == 0)
              ret = 1;
          break;
      default:
          ret = -1;
          break;
    }
    CHECK(ret > 0, lblKO);
    
    memcpy(buf, dst, wdt);
    buf[wdt] = 0;

    return ret;
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetMut(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char fgd[ARGLEN];
    char bgd[ARGLEN];
    char ctl[ARGLEN];
    char dst[ARGLEN];

    ret = cmbGetArgStr(fgd, &cmb->arg[0], key); //only the first arg can be accessed using a key here
    CHECK(ret > 0, lblKO);
    ret = cmbGetArgStr(bgd, &cmb->arg[1], NULLKEY);
    CHECK(ret > 0, lblKO);
    ret = cmbGetArgStr(ctl, &cmb->arg[2], NULLKEY);
    CHECK(ret > 0, lblKO);
    ret = fmtMut(dst, fgd, bgd, ctl);
    
    memcpy(buf, dst, len);
    buf[len - 1] = 0;

    return ret;
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetAmt(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char dst[ARGLEN];
    char src[ARGLEN];
    char sep[ARGLEN];
    byte exp = 0;

    memset(sep, 0, sizeof(sep));
    ret = cmbGetArgStr(src, &cmb->arg[0], key); //only the first arg can be accessed using a key here
    CHECK(ret > 0, lblKO);
    ret = cmbGetArg(&exp, &cmb->arg[1], NULLKEY, SB);
    CHECK(ret > 0, lblKO);
    ret = cmbGetArgStr(sep, &cmb->arg[2], NULLKEY); //optional
    CHECK(ret >= 0, lblKO);
    ret = fmtAmt(dst, src, exp, sep);
    memcpy(buf, dst, len);
    buf[len - 1] = 0;

    return ret;
  lblKO:
    trcErr(ret);
    return -1;
}

static char isNumericFormat(const char *str) {
    int ret;

    ret = fmtTok(0, str, "cCdiouxXeEfgGnpsS");
    switch (str[ret]) {
          //numeric formats
      case 'c':
      case 'C':
      case 'd':
      case 'i':
      case 'o':
      case 'u':
      case 'x':
      case 'X':
      case 'n':
      case 'p':
          ret = 1;
          break;

          //string formats:
      case 's':
      case 'S':
          ret = 0;
          break;

          //float is not supported
      case 'e':
      case 'E':
      case 'f':
      case 'g':
      case 'G':
      default:
          ret = -1;
          break;
    }
    return ret;
}

static int cmbGetSpr(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char fmt[ARGLEN];           //sprintf format argument
    char val[ARGLEN];           //value to format
    char sup[ARGLEN];           //supplementary value (optional)
    char dst[ARGLEN];           //destination string
    card num1 = 0, num2 = 0;
    byte idx;
    char *ptr;

    idx = 0;
    ret = cmbGetArgStr(fmt, &cmb->arg[0], NULLKEY); //only the first arg can be accessed using a key here
    CHECK(ret > 0, lblKO);

    ret = cmbGetArgStr(val, &cmb->arg[1], NULLKEY); //this argument is optional
    CHECK(ret >= 0, lblKO);     //0 value means that sup is missing
    if(ret)
        idx++;

    ret = cmbGetArgStr(sup, &cmb->arg[2], NULLKEY); //this argument is optional
    CHECK(ret >= 0, lblKO);     //0 value means that sup is missing
    if(ret)
        idx++;

    switch (idx) {
      case 2:
          idx = 0;
          ptr = fmt;
          ret = fmtTok(0, ptr, "%");
          CHECK(ret >= 0, lblKO);
          ptr += ret;
          CHECK(*ptr == '%', lblKO);
          ret = isNumericFormat(ptr);
          CHECK(ret >= 0, lblKO);
          if(ret) {
              //num1 = *(card *) val;
              memcpy(&num1, val, sizeof(card));
              idx |= 0x01;
          }

          ptr++;
          ret = fmtTok(0, ptr, "%");
          CHECK(ret >= 0, lblKO);
          ptr += ret;
          CHECK(*ptr == '%', lblKO);
          ret = isNumericFormat(ptr);
          CHECK(ret >= 0, lblKO);
          if(ret) {
              //num2 = *(card *) sup;
              memcpy(&num2, sup, sizeof(card));
              idx |= 0x10;
          }

          switch (idx) {
            case 0x00:
                sprintf(dst, fmt, val, sup);
                break;
            case 0x01:
                sprintf(dst, fmt, num1, sup);
                break;
            case 0x10:
                sprintf(dst, fmt, val, num2);
                break;
            case 0x11:
                sprintf(dst, fmt, num1, num2);
                break;
          }
          break;
      case 1:
          ret = fmtTok(0, fmt, "%");
          ret = isNumericFormat(fmt + ret);
          CHECK(ret >= 0, lblKO);
          if(ret) {
              //num1 = *(card *) val;
              memcpy(&num1, val, sizeof(card));
              sprintf(dst, fmt, num1, "");
          } else
              sprintf(dst, fmt, val, "");
          break;
      default:
          sprintf(dst, fmt, "", "");

          break;
    }
    
    memcpy(buf, dst, len);
    buf[len - 1] = 0;

    return strlen(buf);
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetMat(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char dst[ARGLEN];
    char str1[ARGLEN];
    char str2[ARGLEN];

    ret = cmbGetArgStr(str1, &cmb->arg[0], key);    //only the first arg can be accessed using a key here
    CHECK(ret >= 0, lblKO);
    ret = cmbGetArgStr(str2, &cmb->arg[1], NULLKEY);
    CHECK(ret >= 0, lblKO);

    switch (cmb->opr) {
      case oprAdd:
          addStr(dst, str1, str2);
          break;
      case oprSub:
          subStr(dst, str1, str2);
          break;
      default:
          break;
    }
    memcpy(buf, dst, len);
    buf[len - 1] = 0;

    return strlen(buf);
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetIf(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char cnd[ARGLEN];
    char val1[ARGLEN];

    ret = cmbGetArgStr(cnd, &cmb->arg[0], key); //only the first arg can be accessed using a key here
    CHECK(ret > 0, lblKO);

    if(cnd[0]) {
        ret = cmbGetArgStr(val1, &cmb->arg[1], NULLKEY);
        CHECK(ret >= 0, lblKO);
    } else {
        ret = cmbGetArgStr(val1, &cmb->arg[2], NULLKEY);
        CHECK(ret >= 0, lblKO);
    }
    

    memcpy(buf, val1, len);
    buf[len - 1] = 0;

    return strlen(buf);
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetOr(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char cnd1[ARGLEN];
    char cnd2[ARGLEN];
    char cnd3[ARGLEN];

    ret = cmbGetArgStr(cnd1, &cmb->arg[0], NULLKEY);
    CHECK(ret > 0, lblKO);
    ret = cmbGetArgStr(cnd2, &cmb->arg[1], NULLKEY);
    CHECK(ret > 0, lblKO);
    cnd3[0] = 0;                //optional
    ret = cmbGetArgStr(cnd3, &cmb->arg[2], NULLKEY);
    CHECK(ret >= 0, lblKO);

    buf[0] = cnd1[0] || cnd2[0] || cnd3[0];
    return 1;
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetEq(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char val1[ARGLEN + 1];
    char val2[ARGLEN + 1];
    word bufLen;

    memset(val1, 0, sizeof(val1));
    memset(val2, 0, sizeof(val2));

    ret = cmbGetArg(val1, &cmb->arg[0], NULLKEY, ARGLEN);
    CHECK(ret > 0, lblKO);
    ret = cmbGetArg(val2, &cmb->arg[1], NULLKEY, ARGLEN);
    CHECK(ret > 0, lblKO);
    bufLen = cmb->arg[2].key;
    
    if(!bufLen)
        buf[0] = !strcmp(val1, val2);
    else
        buf[0] = !memcmp(val1, val2, bufLen);

    return 1;
  lblKO:
    trcErr(ret);
    return -1;
}

static int cmbGetTok(const tCombo * cmb, word key, char *buf, word len) {
    int ret;
    char scr[ARGLEN + 1];
    char sep[ARGLEN + 1];

    memset(scr, 0, sizeof(scr));
    memset(sep, 0, sizeof(sep));

    ret = cmbGetArgStr(scr, &cmb->arg[0], NULLKEY);
    CHECK(ret > 0, lblKO);
    ret = cmbGetArgStr(sep, &cmb->arg[1], NULLKEY);
    CHECK(ret > 0, lblKO);

    ret = fmtTok(buf, scr, sep);
    CHECK(ret >= 0, lblKO);
    return strlen(buf);
  lblKO:
    trcErr(ret);
    return -1;
}

/** Retrieve the combo data element into buffer buf of length len.
 * The data element is calculated as a result of the combo operation on the arguments pointed to by the locator array.
 * \param cmb (I) Pointer to the combo
 * \param key (I) key value, 0 or NULLKEY
 * \param buf (I) Pointer to the destination buffer where to put the value
 * \param len (I) Length of the data in buf
 * \pre 
 *  - cmb!=0
 *  - buf!=0
 * \return length of the output data element
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0033.c
*/
int cmbGet(const tCombo * cmb, word key, void *buf, word len) {
    int ret;
    word size;

    size = cmb->len;
    if(!len)
        len = size;

    memset(buf, 0, len);
    ret = -1;
    switch (cmb->opr) {
      case oprCat:
          ret = cmbGetCat(cmb, key, buf, len);
          break;
      case oprSbs:
          ret = cmbGetSbs(cmb, key, buf, len);
          break;
      case oprTok:
          ret = cmbGetTok(cmb, key, buf, len);
          break;
      case oprPad:
          ret = cmbGetPad(cmb, key, buf, len);
          break;
      case oprMut:
          ret = cmbGetMut(cmb, key, buf, len);
          break;
      case oprSpr:
          ret = cmbGetSpr(cmb, key, buf, len);
          break;
      case oprAdd:
      case oprSub:
          ret = cmbGetMat(cmb, key, buf, len);
          break;
      case oprIf:
          ret = cmbGetIf(cmb, key, buf, len);
          break;
      case oprEq:
          ret = cmbGetEq(cmb, key, buf, len);
          break;
      case oprOr:
          ret = cmbGetOr(cmb, key, buf, len);
          break;
      default:
          ret = cmbGetAmt(cmb, key, buf, len);
          break;
    }
    return ret;
}
