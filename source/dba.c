/** \file
 * Data Base Access processing
 *
 * Subversion reference
 *
 * $HeadURL: https://svn.ingenico.com/UTA/core/utaYYMMDD/pri/dba.c $
 *
 * $Id: dba.c 2217 2009-03-20 14:52:31Z abarantsev $
 */

#include "global.h"


#undef trcOpt
//static const card trcOpt = TRCOPT & BIT(trcDBA);
#define trcOpt (TRCOPT & BIT(trcDBA))

static byte *volPtr;

/** Set the pointer to a piece of memory that should be used for volatile data storage.
 *
 * This pointer must be set before using volatile data containers.
 *
 * The size of the memory space should be large enough to contain all the data that should be stored.
 *
 * \param ptr (I) The pointer to the buffer to be used by the volatile data base access functions
 *
 * \pre 
 *    - ptr!=0
 *
 * \return no
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0029.c
*/
void dbaSetPtr(void *ptr) {
    volPtr = ptr;
}

static int dbaSet(byte sec, byte val, card ofs, card len) {
    int ret;

    trcFS("%s: ", "dbaSet");
    trcFN("sec=%02X ", (word) sec);
    trcFN("ofs=%d ", (card) ofs);
    trcFS("val=", "");
    trcBN(&val, 1);
    trcFN("len=%d\n", len);

    switch (sec) {
      case 0xFF:
          memset(volPtr + ofs, val, len);
          ret = len;
          break;
#ifdef __UNICAPT__
      case 0xFE:
          ret = ismSet(val, ofs, len);
          break;
#endif
      default:
          ret = nvmSet(sec, val, ofs, len);
          break;
    }


    return ret;
}

static int dbaLoad(byte sec, void *buf, card ofs, word len) {
    int ret;

    switch (sec) {
      case 0xFF:               //volatile section
          memcpy(buf, volPtr + ofs, len);
          break;
#ifdef __UNICAPT__
      case 0xFE:               //ism storage
          ret = ismLoad(buf, ofs, len);
          CHECK(ret == len, lblKO);
          break;
#endif
      default:                 //non-volatile section
          ret = nvmLoad(sec, buf, ofs, len);
          CHECK(ret == len, lblKO);
          break;
    }
    trcFS("%s: ", "dbaLoad");
    trcFN("sec=%02X ", sec);
    trcFN("ofs=%d buf=", ofs);
    trcBAN((byte *) buf, len);
    trcS("\n");

    return len;
  lblKO:
    trcErr(ret);
    return -1;
}

static int dbaSave(byte sec, const void *buf, card ofs, word len) {
    int ret;

    switch (sec) {
      case 0xFF:               //volatile section
          memcpy(volPtr + ofs, buf, len);
          break;
#ifdef __UNICAPT__
      case 0xFE:               // ism storage
          ret = ismSave(buf, ofs, (word) len);
          CHECK(ret == len, lblKO);
          break;
#endif
      default:                 //non-volatile section
          ret = nvmSave(sec, buf, ofs, (word) len);
          CHECK(ret == len, lblKO);
          break;
    }

    trcFS("%s: ", "dbaSave");
    trcFN("sec=%02X ", (word) sec);
    trcFN("ofs=%d buf=", ofs);
    trcBAN((byte *) buf, len);
    trcS("\n");

    return len;
  lblKO:
    trcErr(ret);
    return -1;
}

static int dbaFill(byte sec, const void *buf, card ofs, word len, word size) {
    int ret;

    if(size <= len)
        return dbaSave(sec, buf, ofs, size);

    ret = dbaSet(sec, 0, ofs, size);
    CHECK(ret == size, lblKO);

    return dbaSave(sec, buf, ofs, len);
  lblKO:
    trcErr(ret);
    return -1;
}

/** Export data element from src section to dst section.
 * \param dst (I) destination section, where the data will be exported
 * \param src (I) source section, where the data are located
 * \param ofs (I) offset within source section
 * \param len (I) data element length
 * \param fmt (I) export format 'S' for string, 'B' for binary, 'H' for hex numeric, 'D' for dec numeric
 * \param cmt (I) optional comment (zero pointer if missing)
 *
 * \pre 
 *    - ptr!=0
 *
 * The exported string looks like
 *
 * SS:FFFFFFFF:LLLL:T:VVVV...
 *
 * where
 *  - SS is source section number in hex format
 *  - FFFFFFFF is offset within source section in hex format
 *  - LLLL data element length in hex format
 *  - T data format:
 *    - 'S' for string
 *    - 'B' for binary with leading byte as length
 *    - 'b' just binary 
 *    - 'H' for hex numeric
 *    - 'D' for dec numeric
 *
 * \return length of string exported if OK; negative otherwise
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0150.c
*/
int dbaExport(byte dst, byte src, card ofs, word len, char fmt, const char *cmt) {
    int ret;
    char buf[1024];
    char tmp[1024];
    char tmp2[1024];
    card num = 0;
    char *ptr1;
    const char *ptr2;

    memset(buf, 0, 1024);
    memset(tmp, 0, 1024);
    memset(tmp2, 0, 1024);

    num2hex(tmp, src, 2);
    strcat(buf, tmp);
    strcat(buf, ":");

    num2hex(tmp, ofs, 8);
    strcat(buf, tmp);
    strcat(buf, ":");

    num2hex(tmp, len, 4);
    strcat(buf, tmp);
    strcat(buf, ":");

    buf[strlen(buf)] = fmt;
    strcat(buf, ":");

    ret = dbaLoad(src, tmp, ofs, len);
    CHECK(ret >= 0, lblKO);
    switch (fmt) {
      case 'H':
      case 'D':
          switch (len) {
            case sizeof(byte):
                //num = *(byte *) tmp;
                memcpy(&num, tmp, sizeof(byte));
                break;
            case sizeof(word):
                //num = *(word *) tmp;
                memcpy(&num, tmp, sizeof(word));
                break;
            case sizeof(card):
                //num = *(card *) tmp;
                memcpy(&num, tmp, sizeof(card));
                break;
            default:
                break;
          }
          if(fmt == 'H')
              num2hex(tmp, num, (byte) (len * 2));
          else
              num2dec(tmp, num, 0);
          break;
      case 'B':
          ret = bin2hex(tmp2, (byte *) tmp, (byte) (*tmp) + 1);
          memcpy(tmp, tmp2, ret);
          tmp[ret] = 0;
          break;
      case 'b':
          ret = bin2hex(tmp2, (byte *) tmp, len);
          memcpy(tmp, tmp2, ret);
          tmp[ret] = 0;
          break;
      default:
          strcpy(tmp2, "\"");
          ptr1 = tmp2 + 1;
          ptr2 = tmp;
          while(len--) {
              if(*ptr2 == '"')
                  *ptr1++ = '\\';
              *ptr1++ = *ptr2++;
          }
          strcat(tmp2, "\"");
          strcpy(tmp, tmp2);
          break;
    }
    strcat(buf, tmp);
    if(cmt) {
        strcat(buf, "\t# ");
        strcat(buf, cmt);
    }
    strcat(buf, "\x0D\x0A");
    ret = nvmSave(dst, buf, 0xFFFFFFFFL, (word) strlen(buf));
    return strlen(buf);

  lblKO:
    trcErr(ret);
    return -1;
}

/** Import data elements from section sec.
 * \param sec (I) section containing exported data
 *²
 * The import string should look like
 *
 * SS:FFFFFFFF:LLLL:T:VVVV...
 *
 * where
 *  - SS is section number in hex format
 *  - FFFFFFFF is offset within the section in hex format
 *  - LLLL data element length in hex format
 *  - T data format:
 *    - 'S' for string
 *    - 'B' for binary with leading byte as length
 *    - 'b' just binary 
 *    - 'H' for hex numeric
 *    - 'D' for dec numeric
 *
 * \return length of string exported if OK; negaive otherwise
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0150.c
*/
int dbaImport(byte sec) {
    int ret;
    char buf[1024];
    byte tmp[1024];
    card ofs;                   //import file offset
    char chr;
    char *ptr;
    word dim;
    card num;
    byte dst;                   //destination section
    card adr;                   //data element offset
    word len;                   //data element length
    char fmt;                   //data element format
    word idx;

    ofs = 0;
    dim = 0;
    while(1) {
        memset(buf, 0, 1024);
        ptr = buf;
        nvmHold(sec);
        do {
            ret = nvmLoad(sec, &chr, ofs++, 1);
            if(ret < 0)
                break;
            if(chr == '\n')
                break;
            *ptr++ = chr;
        } while(chr != '\n');
        nvmRelease(sec);
        if(ret < 0)
            break;
        ptr = buf;
        if(*ptr == '\r')
            ptr++;

        hex2num(&num, ptr, 2);
        dst = (byte) num;
        ptr += 2;

        CHECK(*ptr == ':', lblKO);
        ptr++;

        hex2num(&num, ptr, 8);
        adr = num;
        ptr += 8;

        CHECK(*ptr == ':', lblKO);
        ptr++;

        hex2num(&num, ptr, 4);
        len = (word) num;
        ptr += 4;

        CHECK(*ptr == ':', lblKO);
        ptr++;

        fmt = *ptr++;

        CHECK(*ptr == ':', lblKO);
        ptr++;

        memset(tmp, 0, 1024);
        switch (fmt) {
          case 'H':
              hex2num(&num, ptr, (byte) (len * 2));
              memcpy(tmp, &num, len);
              break;
          case 'D':
              dec2num(&num, ptr, 0);
              memcpy(tmp, &num, len);
              break;
          case 'B':
              hex2num(&num, ptr, 2);
              ptr += 2;
              *tmp = (byte) num;
              hex2bin(tmp + 1, ptr, num);
              break;
          case 'b':
              hex2bin(tmp, ptr, 0);
              break;
          default:
              CHECK(fmt == 'S', lblKO);
              CHECK(*ptr == '"', lblKO);
              ptr++;
              idx = 0;
              while(*ptr != '"') {
                  switch (*ptr) {
                    case '\\':
                        ptr++;
                        break;
                    case '\t':
                        *ptr = ' ';
                        break;
                    default:
                        break;
                  }
                  tmp[idx++] = *ptr++;
                  if(idx >= len)
                      break;
              }
        }
        ret = dbaSave(dst, tmp, adr, len);
        dim++;
    }

    ret = dim;
    goto lblEnd;
  lblKO:
    trcErr(ret);
    ret = -1;
  lblEnd:
    return ret;
}

static int cchInit(tCache * cch, byte * ptr, word dim, byte sec, card ofs) {
    cch->ptr = ptr;
    cch->dim = dim;
    cch->sec = sec;
    cch->ofs = ofs;
    cch->mod = 0;
    cch->len = 0;
    memset(cch->ptr, 0, cch->dim);
    return cch->dim;
}

static word cchDim(const tCache * cch) {
    return cch->dim;
}

static int cchReset(tCache * cch) {
    
    memset(cch->ptr, 0, cch->dim);
    cch->mod = 0;
    cch->len = cch->dim;

    trcFN("cchReset: sec=%02X ", (word) cch->sec);
    trcFN("ofs=%d ", cch->ofs);
    trcFN("dim=%d", cch->dim);
    trcS("\n");

    return cch->dim;
}

static int cchLoad(const tCache * cch) {
    return dbaLoad(cch->sec, cch->ptr, cch->ofs, cch->dim);
}

static int cchSave(tCache * cch) {
    int ret;

    ret = dbaSave(cch->sec, cch->ptr, cch->ofs, cch->dim);
    CHECK(ret == cch->dim, lblKO);
    cch->mod = 0;
    cch->len = 0;
    return cch->dim;
  lblKO:
    return -1;
}

static int cchFlush(tCache * cch) {
    int ret;

    if(!cch->len)
        return 0;               //not modified
    ret = dbaSave(cch->sec, cch->ptr + cch->mod, cch->ofs + cch->mod, cch->len);
    CHECK(ret == cch->len, lblKO);
    cch->mod = 0;
    cch->len = 0;
    return ret;
  lblKO:
    return -1;
}

static int cchGet(const tCache * cch, void *buf, word ofs, word len) {
    memcpy(buf, cch->ptr + ofs, len);
    trcFS("%s: ", "cchGet");
    trcFN("ofs=%d buf=", ofs);
    trcBAN((byte *) buf, len);
    trcS("\n");
    return len;
}

static void cchUpdate(tCache * cch, word ofs, word len) {
    if(!cch->len) {
        cch->mod = ofs;
        cch->len = len;
    } else {
        word lft, rgt, lft1, lft2, rgt1, rgt2;

        lft1 = cch->mod;
        rgt1 = cch->mod + cch->len;
        lft2 = ofs;
        rgt2 = ofs + len;

        lft = lft1;
        if(lft2 < lft1)
            lft = lft2;
        cch->mod = lft;

        rgt = rgt1;
        if(rgt1 < rgt2)
            rgt = rgt2;
        cch->len = rgt - lft;
        }
}

static int cchPut(tCache * cch, const void *buf, word ofs, word len) {
    if(!len)
        return 0;

    memcpy(cch->ptr + ofs, buf, len);
    cchUpdate(cch, ofs, len);

    trcFS("%s: ", "cchPut");
    trcFN("ofs=%d buf=", ofs);
    trcBAN((byte *) buf, len);
    trcS("\n");

    return len;
}

static int cchFill(tCache * cch, const void *buf, word ofs, word len, word size) {
    
    if(size <= len)
        return cchPut(cch, buf, ofs, size);
    memset(cch->ptr + ofs, 0, size);
    cchUpdate(cch, ofs, size);
    return cchPut(cch, buf, ofs, len);
}

/** Table constructor: Associate a table of records of length wdt and dimension hgt with a memory space
 * at offset ofs in section sec.
 * \param tab (O) The pointer to the table descriptor to be initialized
 * \param sec (I) Section number where the table is located
 * \param ofs (I) Starting offset of the table within the section 
 * \param wdt (I) Row length 
 * \param hgt (I) Number of rows in the table
 * \param fmt (I) Row format
 * \pre 
 *    - tab!=0
 * \return the size of space occupied by the table.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0029.c
*/
int tabInit(tTable * tab, byte sec, card ofs, word wdt, word hgt, char fmt) {
    tab->sec = sec;
    tab->ofs = ofs;
    tab->wdt = wdt;
    tab->hgt = hgt;
    tab->fmt = fmt;
    memset(&tab->cch, 0, sizeof(tab->cch));
    return tabSize(tab);
}

/** Associate a cache buffer with a table or disable it if the pointer cch is zero.
 * \param tab (IO) Pointer to the table descriptor
 * \param cch (IO) Pointer to cache memory buffer
 * \param dim (I) Size of cache memory buffer 
 * \pre 
 *    - tab!=0
 *    - dim>=tabSize(tab) if cch!=0
 * \return the cache size
 * \remark if cch!=0 the internal cache descriptor is initialized.
 *  The memory should be allocated before calling this function.
 *  If the pointer cch is zero the caching is disabled.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0029.c
*/
int tabCache(tTable * tab, byte * cch, word dim) {
    if(!cch) {
        memset(&tab->cch, 0, sizeof(tab->cch));
        return 0;
    }
    cchInit(&tab->cch, cch, (word) tabSize(tab), tab->sec, tab->ofs);
    return cchDim(&tab->cch);
}

/** Getter: return number of rows in the table
 * \param tab (I) Pointer to the table descriptor
 * \return number of rows in the table (table dimension)
 * \pre 
 *    - tab!=0
 * \test tcab0029.c
*/
word tabDim(const tTable * tab) {
    return tab->hgt;
}

/** Getter: return row length
 * \param tab (I) Pointer to the table descriptor
 * \return number of bytes in a table row
 * \pre 
 *    - tab!=0
 * \test tcab0029.c
*/
word tabWdt(const tTable * tab) {
    return tab->wdt;
}

/** Getter: return table size
 * \param tab (I) Pointer to the table descriptor
 * \return number of bytes occupied by the table
 * \pre 
 *    - tab!=0
 * \test tcab0029.c
 */
card tabSize(const tTable * tab) {
    return tab->hgt * tab->wdt;
}

/** Getter: return row format
 * \param tab (I) Pointer to the table descriptor
 * \return row format
 * \pre 
 *    - tab!=0
 * \test tcab0029.c
 */
char tabRowFmt(const tTable * tab) {
    return tab->fmt;
}

/** Fill all the table by zeroes
 * \param tab (I) Pointer to the table descriptor
 * \return number of bytes occupied by the table
 * \pre 
 *    - tab!=0
 * \test tcab0029.c
*/
int tabReset(tTable * tab) {
    if(tab->cch.dim)
        return cchReset(&tab->cch);
    return dbaSet(tab->sec, 0, tab->ofs, tabSize(tab));
}

/** Retrieve a single table row number idx into buffer buf of length len
 * \param tab (I) Pointer to the table descriptor
 * \param idx (I) Index of the row to be retrieved
 * \param buf (O) Destination buffer to be filled
 * \param len (I) Length of the destination buffer
 * \return number of bytes retrieved
 * \pre 
 *    - tab!=0
 *    - len>=tabWdt(tab)
 *    - idx<tabHgt(tab)
 * \test tcab0029.c
*/
int tabGet(const tTable * tab, word idx, void *buf, word len) {
    int ret;
    card ofs;

    
    if(!len)
        len = tab->wdt + 1;     //assign default value
    
    memset(buf, 0, len);
    if(tab->cch.dim) {
        ofs = (tab->wdt) * idx;
        ret = cchGet(&tab->cch, buf, (word) ofs, tab->wdt);
    } else {
        ofs = (tab->ofs) + (tab->wdt) * idx;
        ret = dbaLoad(tab->sec, buf, ofs, tab->wdt);
    }
    if(ret < 0)
        return ret;
    return len;                 //number of bytes modified in destination
}

/** Save buffer buf of length len into a table row number idx 
 * \param tab (I) Pointer to the table descriptor
 * \param idx (I) Index of the row to be saved
 * \param buf (O) Source buffer to be saved
 * \param len (I) Length of the source buffer
 * \return number of bytes saved
 * \pre 
 *    - tab!=0
 *    - len<=tabWdt(tab)
 *    - idx<tabHgt(tab)
 * \test tcab0029.c
*/
int tabPut(tTable * tab, word idx, const void *buf, word len) {
    int ret;
    card ofs;

    if(!len)
        len = strlen((char *) buf) + 1;
    if(tab->cch.dim) {
        ofs = (tab->wdt) * idx;
        ret = cchFill(&tab->cch, buf, (word) ofs, len, tab->wdt);
    } else {
        ofs = (tab->ofs) + (tab->wdt) * idx;
        ret = dbaFill(tab->sec, buf, ofs, len, tab->wdt);
    }
    return ret;
}

/** Export row idx into section sec using format fmt
 * \param tab (I) Pointer to the table descriptor
 * \param idx (I) Index of the row to be exported
 * \param sec (I) Export section
 * \param fmt (I) Export format
 * \param cmt (I) Optional comment
 * \return tabWdt(tab) if OK; otherwise negative
 * \pre 
 *    - tab!=0
 *    - idx<tabDim(tab)
 * \test tcab0151.c
*/
int tabExport(tTable * tab, word idx, byte sec, char fmt, const char *cmt) {
    int ret;
    card ofs;

    ofs = (tab->ofs) + (tab->wdt) * idx;
    ret = dbaExport(sec, tab->sec, ofs, tabWdt(tab), fmt, cmt);
    CHECK(ret > 0, lblKO);

    ret = tabWdt(tab);
    goto lblEnd;

  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

/** Flush the cache buffer if it is active.
 * All the data modified in cache after the last reading are saved to the non-volatile memory.
 * If there is no modifications niothing is done.
 * \param tab (O) Pointer to the table descriptor
 * \pre 
 *    - tab!=0
 * \return the number of bytes written to the non-volatile memory
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0029.c
*/
int tabFlush(tTable * tab) {
    if(!tab->cch.dim)
        return 0;
    return cchFlush(&tab->cch);
}

/** Load the table into cache.
 * The operation is performed using only one reading.
 * After that all accesses to table rows are performed with cache memory in RAM instead of
 * working with non-volatile memory.
 * \param tab (IO) Pointer to the table descriptor
 * \pre 
 *    - tab!=0
 * \return the number of bytes read (table size)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0029.c
*/
int tabLoad(tTable * tab) {
    if(!tab->cch.dim)
        return 0;
    return cchLoad(&tab->cch);
}

/** Save cache buffer unconditionnally to the non-volatile memory.
 * The operation is performed using only one writing.
 * All data are saved even if it is not changed.
 * \param tab (IO) Pointer to the table descriptor
 * \pre 
 *    - tab!=0
 * \return the number of bytes written (table size)
 * \remark The operation is convenient to initial fill the table by default values.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0029.c
*/
int tabSave(tTable * tab) {
    if(!tab->cch.dim)
        return 0;
    return cchSave(&tab->cch);
}

static card fldOfs(const word * map, word fld) {    //retrieve data offset from map array for a given field
    if(!fld)
        return 0;               //the initial offset is zero
    return map[fld - 1];        //otherwise it is map[fld-1]
}

/** Record constructor: Associate a record of dimension dim with a memory space
 * at offset ofs in section sec. 
 * \param rec (O) The pointer to the record descriptor to be initialized
 * \param sec (I) Section number where the record is located
 * \param ofs (I) Starting offset of the record within the section 
 * \param dim (I) Number of fields in the record
 * \param len (I) Array of field lengths
 * \param fmt (I) Array of field formats
 * \param map (O) Array of relative field offsets (calculated inside the function)
 * \pre 
 *    - rec!=0
 *    - len!=0
 *    - map!=0
 *    - dim!=0
 * \return the size of space occupied by the record
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0030.c
*/
int recInit(tRecord * rec, byte sec, card ofs, word dim, const word * len,
            const char *fmt, word * map) {
    card fld;

    rec->sec = sec;
    rec->ofs = ofs;
    rec->dim = dim;
    rec->len = len;
    rec->fmt = fmt;             //can be zero if not used
    rec->map = map;

    for (fld = 0; fld < dim; fld++) {   //calculate offsets
        map[fld] = fld ? map[fld - 1] : 0;  //copy the previous offset
        map[fld] += len[fld];   //offset = current length + previous offset
    }
    memset(&rec->cch, 0, sizeof(rec->cch));
    return recSize(rec);
}

/** Associate a cache buffer with a record or disable it if the pointer cch is zero.
 * \param rec (IO) Pointer to the record descriptor
 * \param cch (IO) Pointer to cache memory buffer
 * \param dim (I) Size of cache memory buffer 
 * \pre 
 *    - rec!=0
 *    - dim>=recSize(rec) if cch!=0
 * \return the cache size
 * \remark if cch!=0 the internal cache descriptor is initialized.
 *  The memory should be allocated before calling this function.
 *  If the pointer cch is zero the caching is disabled.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0030.c
*/
int recCache(tRecord * rec, byte * cch, word dim) {
    if(!cch) {
        memset(&rec->cch, 0, sizeof(rec->cch));
        return 0;
    }
    cchInit(&rec->cch, cch, (word) recSize(rec), rec->sec, rec->ofs);
    return cchDim(&rec->cch);
}

/** Getter: return number of fields in the record
 * \param rec (I) Pointer to the record descriptor
 * \return number of fields in the record (record dimension)
 * \pre 
 *    - rec!=0
 * \test tcab0030.c
*/
word recDim(const tRecord * rec) {
    return rec->dim;
}

/** Getter: return record size
 * \param rec (I) Pointer to the record descriptor
 * \return number of bytes occupied by the record 
 * \pre 
 *    - rec!=0
 * \test tcab0030.c
*/
card recSize(const tRecord * rec) {
    return rec->map[rec->dim - 1];  //last offset
}

/** Getter: return field format
 * \param rec (I) Pointer to the record descriptor
 * \param fld (I) Field number
 * \return format of the field; 0 if not defined during initialization 
 * \pre 
 *    - rec!=0
 *    - fld<recDim(rec)
 * \test tcab0030.c
*/
char recFldFmt(const tRecord * rec, word fld) {
    if(!rec->fmt)
        return 0;
    return rec->fmt[fld];
}

/** Getter: return field length
 * \param rec (I) Pointer to the record descriptor
 * \param fld (I) Field number
 * \return length reserved for the field
 * \pre 
 *    - rec!=0
 *    - fld<recDim(rec)
 * \test tcab0030.c
*/
word recFldLen(const tRecord * rec, word fld) {
    return rec->len[fld];
}

/** Fill all the record memory space by zeroes 
 * \param rec (I) Pointer to the record descriptor
 * \return number of bytes occupied by the record
 * \pre 
 *    - rec!=0
 * \test tcab0030.c
*/
int recReset(tRecord * rec) {
    if(rec->cch.dim)
        return cchReset(&rec->cch);
    return dbaSet(rec->sec, 0, rec->ofs, recSize(rec));
}

/** Retrieve a data corresponding to the field fld into buffer buf of length len
 * \param rec (I) Pointer to the record descriptor
 * \param fld (I) Index of the field to be retrieved
 * \param buf (O) Destination buffer to be filled
 * \param len (I) Length of the destination buffer
 * \return number of bytes retrieved
 * \pre 
 *    - rec!=0
 *    - len>=length of the field
 *    - idx<recDim(rec)
 * \test tcab0030.c
*/
int recGet(const tRecord * rec, word fld, void *buf, word len) {
    int ret;
    card ofs;
    word size;
    word num;

    size = rec->len[fld];
    if(!len)
        len = size;

    memset(buf, 0, len);
    num = ((len < size) ? len : size);

    if(rec->cch.dim) {
        ofs = fldOfs(rec->map, fld);
        ret = cchGet(&rec->cch, buf, (word) ofs, rec->len[fld]);
    } else {
        ofs = rec->ofs + fldOfs(rec->map, fld);
        ret = dbaLoad(rec->sec, buf, ofs, num);
    }

    if(ret < 0)
        return ret;
    return len;                 //number of bytes modified in destination
}

/** Save buffer buf of length len into a data spaced reserved for fld.
 * If the length len is too big, the data are truncated;
 * if it is too small the tail is filled by zeroes.
 * \param rec (I) Pointer to the record descriptor
 * \param fld (I) Index of the field to be retrieved
 * \param buf (O) Source buffer to be saved
 * \param len (I) Length of the source buffer
 * \return number of bytes saved
 * \pre 
 *    - rec!=0
 *    - idx<recDim(rec)
 * \test tcab0030.c
*/
int recPut(tRecord * rec, word fld, const void *buf, word len) {
    int ret;
    card ofs;
    word size;

    size = rec->len[fld];
    if(!len)
        len = strlen((char *) buf);
    if(size < len)
        len = size;

    if(rec->cch.dim) {
        ofs = fldOfs(rec->map, fld);
        ret = cchFill(&rec->cch, buf, (word) ofs, len, rec->len[fld]);
    } else {
        ofs = rec->ofs + fldOfs(rec->map, fld);
        ret = dbaFill(rec->sec, buf, ofs, len, rec->len[fld]);
    }

    return ret;
}

/** Export field fld into section sec using format fmt
 * \param rec (I) Pointer to the record descriptor
 * \param fld (I) Index of the field to be exported
 * \param sec (I) Export section
 * \param fmt (I) Export format
 * \param cmt (I) Optional comment
 * \return field length if OK; otherwise negative
 * \pre 
 *    - rec!=0
 *    - fld<recDim(tab)
 * \test tcab0151.c
*/
int recExport(tRecord * rec, word fld, byte sec, char fmt, const char *cmt) {
    int ret;
    card ofs;

    //byte buf[256];
    //ret= recGet(rec,fld,buf,256);
    //CHECK(ret>0,lblKO);

    ofs = rec->ofs + fldOfs(rec->map, fld);
    ret = dbaExport(sec, rec->sec, ofs, rec->len[fld], fmt, cmt);
    CHECK(ret > 0, lblKO);

    ret = rec->len[fld];
    goto lblEnd;

  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

/** Flush the cache buffer if it is active.
 * All the data modified in cache after the last reading are saved to the non-volatile memory.
 * If there is no modifications nothing is done.
 * \param rec (O) Pointer to the record descriptor
 * \pre 
 *    - rec!=0
 * \return the number of bytes written to the non-volatile memory
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0030.c
*/
int recFlush(tRecord * rec) {
    if(!rec->cch.dim)
        return 0;
    return cchFlush(&rec->cch);
}

/** Load the record into cache.
 * The operation is performed using only one reading.
 * After that all accesses to record fields are performed with cache memory in RAM instead of
 * working with non-volatile memory.
 * \param rec (IO) Pointer to the record descriptor
 * \pre 
 *    - rec!=0
 * \return the number of bytes read (record size)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0030.c
*/
int recLoad(tRecord * rec) {
    if(!rec->cch.dim)
        return 0;
    return cchLoad(&rec->cch);
}

/** Save cache buffer unconditionnally to the non-volatile memory.
 * The operation is performed using only one writing.
 * All data are saved even if it is not changed.
 * \param rec (IO) Pointer to the record descriptor
 * \pre 
 *    - rec!=0
 * \return the number of bytes written (record size)
 * \remark The operation is convenient to initial fill the record by default values.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0030.c
*/
int recSave(tRecord * rec) {
    if(!rec->cch.dim)
        return 0;
    return cchSave(&rec->cch);
}

/** Move record rec to the line idx of the table tab.
 * It is assumed that the table tab consists of rows having the structure described by rec. 
 * After calling this function the fields of the record number idx can be accessed
 * using the structure rec.
 *
 * If the cache is activated for the record it is flushed before moving the cursor.
 *
 * \param rec (I) Pointer to record structure to be moved
 * \param tab (I) Pointer to the table structure where the cursor will be moved
 * \param idx (I) Record number where to put the cursor
 *
 * \pre 
 *    - rec!=0
 *    - tab!=0
 *    - idx<tabDim(tab)
 *    - rec->sec==tab->sec;  memory section compatibility
 *    - recSize(rec)==tab->wdt; the table consists of the rows described in rec
 *
 * \return  
 *    - the current record number within the table
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0031.c
*/
int recMove(tRecord * rec, const tTable * tab, word idx) {
    //int ret;

    rec->sec = tab->sec;        //synchronize memory section
    rec->ofs = (tab->ofs) + recSize(rec) * idx;

    rec->cch.sec = rec->sec;
    rec->cch.ofs = rec->ofs;

    return idx;
}

/** Get current cursor position in the multirecord
 * \param rec (I) Pointer to record structure
 * \param tab (I) Pointer to the table structure
 *
 * \pre 
 *    - rec!=0
 *    - tab!=0
 *    - rec->sec==tab->sec;  memory section compatibility
 *    - recSize(rec)==tab->wdt; the table consists of the rows described in rec
 *
 * \return  
 *    - the current record number within the table
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0031.c
*/
int recPos(const tRecord * rec, const tTable * tab) {
    int ret;

    ret = (word) ((rec->ofs - tab->ofs) / tabWdt(tab));
  
    return ret;
}

/** Search descriptor constructor.
 * \param srh (O) The pointer to the search descriptor to be initialized
 * \param tab (I) Multirecord table
 * \param rec (I) Cursor record
 * \param mtc (I) Match container: contains field to match
 * \remark The match container can contain only one field from the record or a combination of
 * several fields combined/converted into one field.
 *
 * The match pattern should be always a zero-ended string.
 * \pre 
 *    - srh!=0
 *    - rec!=0
 *    - tab!=0
 *    - tab->sec!=0xFF
 *    - mtc!=0
 * \return number of records in multirecord
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0120.c
*/
int searchInit(tSearch * srh, const tTable * tab, tRecord * rec,
               const tContainer * mtc) {
    srh->tab = tab;
    srh->rec = rec;
    srh->mtc = mtc;

    return tabDim(tab);
}

static int compare(const tTable * tab, tRecord * rec, const tContainer * mtc,
                   word idx, const char *pat) {
    int ret;
    char buf[256];

    recMove(rec, tab, idx);
    ret = cntGet(mtc, NULLKEY, buf, sizeof(buf));
    CHECK(ret >= 0, lblKO);
    ret = strcmp(buf, pat);
    if(ret < 0)
        ret = '<';
    else if(ret > 0)
        ret = '>';
    else
        ret = '=';
    trcFS("compare: [%s] ", buf);
    trcFN("%c ", ret);
    trcFS("[%s]\n", pat);

    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    return ret;
}

static int compareSrh(const tSearch * srh, word pos, const char *pat) {
    return compare(srh->tab, srh->rec, srh->mtc, pos, pat);
}

static int compareIdx(const tIndex * idx, word pos, const char *pat) {
    return compare(idx->tab, idx->rec, idx->fld, pos, pat);
}

/** Find a record with given key fields using sequential linear search
 * The search can be performed in backward direction if end<beg.
 * \param srh (I) The pointer to the search descriptor
 * \param pat (I) Search pattern
 * \param beg (I) Starting index
 * \param end (I) Ending index
 * \remark During the search the pattern is compared with match container field
 * retrieve from each instance of the multirecord
 * \pre 
 *    - srh!=0
 *    - pat!=0
 *    - beg<=tabDim(srh->tab)
 *    - end<=tabDim(srh->tab)
 * \return
 *    - row number if found
 *    - tabDim(srh->tab) if not found
 *    - -1 in case of problem
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0120.c
*/
int linFind(tSearch * srh, const char *pat, word beg, word end) {
    int ret;
    int dir;
    int idx;

    if(beg == tabDim(srh->tab))
        beg--;
    if(end == tabDim(srh->tab))
        end--;
    dir = (beg <= end) ? 1 : (-1);

    nvmHold(srh->tab->sec);
    for (idx = beg; idx != (end + dir); idx += dir) {
        ret = compareSrh(srh, (word) idx, pat);
        CHECK(ret >= 0, lblKO);
        if(ret == '=')
            break;
    }
    ret = (idx == end + dir) ? tabDim(srh->tab) : idx;
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    nvmRelease(srh->tab->sec);
    return ret;
}

/** Find a record with given key fields using binary search
 * \param srh (I) The pointer to the search descriptor
 * \param pat (I) Search pattern
 * \param beg (I) Starting index
 * \param end (I) Ending index
 * \remark During the search the pattern is compared with match container field
 * retrieve from each instance of the multirecord
 * \pre 
 *    - srh!=0
 *    - pat!=0
 *    - beg<=end
 *    - end<=tabDim(srh->tab)
 * \return
 *    - row number if found
 *    - tabDim(srh->tab) if not found
 *    - -1 in case of problem
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0121.c
*/
int binFind(tSearch * srh, const char *pat, word beg, word end) {
    int ret;
    int lft, rgt, mid;

    if(end == tabDim(srh->tab))
        end--;
    lft = beg;
    rgt = end;
    nvmHold(srh->tab->sec);

    while(lft <= rgt) {
        mid = (lft + rgt) / 2;
        ret = compareSrh(srh, (word) mid, pat);
        CHECK(ret >= 0, lblKO);
        switch (ret) {
          case '<':            // next search in upper half
              lft = mid + 1;
              break;
          case '>':            // next search in lower half
              rgt = mid - 1;
              break;
          default:
              ret = mid;        // found
              goto lblEnd;
        }
    }

    ret = tabDim(srh->tab);     //not found
    goto lblEnd;

  lblKO:
    ret = -1;
  lblEnd:
    nvmRelease(srh->tab->sec);
    return ret;
}

/** Index descriptor constructor.
 * The index contains an array of row numbers sorted according to fld string.
 * Optionally it contains also the array of keys.
 * \param idx (O) The pointer to the index descriptor to be initialized
 * \param sec (I) Section where to save the index
 * \param ofs (I) Offset within the section
 * \param tab (I) Multirecord table
 * \param rec (I) Cursor record
 * \param fld (I) Contains index fields concatentted in one string
 * \param ptr (I) Pointer to the memory reserved for index array of tabDim(tab) elements
 * \param len (I) length of fld string
 * \param key (I) Pointer to the memory reserved for key array of tabDim(tab) elements, each one of length len
 * \pre 
 *    - idx!=0
 *    - tab!=0
 *    - tab->sec!=0xFF
 *    - rec!=0
 *    - fld!=0
 *    - ptr!=0, points to the reserved memory space of size tabDim(tab)*sizeof(word) bytes
 *    - key!=0, points to the reserved memory space of size tabDim(tab)*len bytes
 *    - sec!=0xFF
 *    - len!=0
 * \return number of records in multirecord
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
int idxInit(tIndex * idx, byte sec, card ofs, const tTable * tab,
            tRecord * rec, const tContainer * fld, word * ptr, byte len,
            char *key) {
    memset(idx, 0, sizeof(tIndex));
    idx->tab = tab;
    idx->rec = rec;
    idx->fld = fld;
    idx->idx = ptr;
    memset(ptr, 0, sizeof(word) * tabDim(tab)); //can contain upto dim numbers
    if(key) {
        idx->key = key;
        memset(key, 0, len * tabDim(tab));  //can contain upto dim keys of length len
    }
    idx->sec = sec;
    idx->ofs = ofs;
    idx->len = len;

    return idxSize(idx);
}

/** Getter: return number of indexed items
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return number of indexed items
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
word idxDim(const tIndex * idx) {
    return idx->dim;
}

/** Find a record with given key fields using binary search via index
 * \param idx (I) The pointer to the index descriptor
 * \param pat (I) Search pattern
 * \param beg (I) Starting index
 * \param end (I) Ending index
 * \remark During the search the pattern is compared with match container field
 * retrieve from each instance of the multirecord.
 * The pattern can be the index key or can include it.
 * \pre 
 *    - idx!=0
 *    - srh!=0
 *    - pat!=0
 *    - beg<=end
 *    - end<=tabDim(idx->tab)
 * \return
 *    - row number if found
 *    - tabDim(idx->tab) if not found
 *    - -1 in case of problem
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
int idxFind(tIndex * idx, const char *pat, word beg, word end) {
    int ret = -1;
    int lft, rgt, mid;
    const char *ptr;

    if(end == tabDim(idx->tab))
        end--;
    lft = beg;
    rgt = end;
    mid = (lft + rgt) / 2;
    nvmHold(idx->tab->sec);

    while(lft <= rgt) {
        mid = (lft + rgt) / 2;
        idx->cur = mid;
        if(!idx->key)
            ret = compareIdx(idx, idx->idx[idx->cur], pat);
        else {
            ptr = idx->key + (idx->cur) * (idx->len);
            ret = memcmp(ptr, pat, idx->len);
            if(ret < 0)
                ret = '<';
            else if(ret > 0)
                ret = '>';
            else
                ret = '=';
            trcFS("compare: [%s] ", (char *) ptr);
            trcFN("%c ", ret);
            trcFS("[%s]\n", pat);
        }
        CHECK(ret >= 0, lblKO);
        switch (ret) {
          case '<':            // next search in upper half
              lft = mid + 1;
              break;
          case '>':            // next search in lower half
              rgt = mid - 1;
              break;
          default:
              ret = mid;        // found
              goto lblEnd;
        }
    }

    //Sorry, not found.
//  lblNotFound:
    if(mid == rgt && ret == '<')    //the search pattern is bigger than any element
        idx->cur = rgt + 1;
    //if not found put cursor at the insert position
    recMove(idx->rec, idx->tab, idx->idx[idx->cur]);
    ret = tabDim(idx->tab);
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    nvmRelease(idx->tab->sec);
    trcFN("idxFind cur=%d\n", idx->cur);
    return ret;
}

static int compareIdxIJ(const tIndex * idx, word i, word j) {
    int ret;
    char buf[256];

    recMove(idx->rec, idx->tab, idx->idx[j]);
    ret = cntGet(idx->fld, NULLKEY, buf, 256);
    CHECK(ret >= 0, lblKO);
    ret = compare(idx->tab, idx->rec, idx->fld, idx->idx[i], buf);
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    return ret;
}

static void swap(const tIndex * idx, word i, word j) {
    word tmp;

    tmp = idx->idx[i];
    idx->idx[i] = idx->idx[j];
    idx->idx[j] = tmp;
    if(idx->key) {
        byte buf[256];
        char *ptr1;
        char *ptr2;

        ptr1 = (idx->key) + i * (idx->len);
        ptr2 = (idx->key) + j * (idx->len);
        memcpy(buf, ptr1, idx->len);
        memcpy(ptr1, ptr2, idx->len);
        memcpy(ptr2, buf, idx->len);
    }
}

static int quickSort(tIndex * idx, int lft, int rgt) {
    int ret;
    word mid, beg, end, num;

    trcFN("quick Sort lft=%d", lft);
    trcFN(" rgt=%d\n", rgt);

    beg = lft;
    end = rgt;
    mid = (lft + rgt) / 2;
    num = 0;
    while(lft < rgt) {
        while(lft < mid) {      //move lft to the right while sorted
            ret = compareIdxIJ(idx, (word) lft, mid);
            CHECK(ret >= 0, lblKO);
            if(ret != '<')
                break;
            lft++;
        }
        while(mid < rgt) {      //move rgt to the left while sorted
            ret = compareIdxIJ(idx, mid, (word) rgt);
            CHECK(ret >= 0, lblKO);
            if(ret != '<')
                break;
            rgt--;
        }
        if(lft <= rgt) {
            if(lft != rgt) {
                swap(idx, (word) lft, (word) rgt);
                num++;
            }
            if(mid == lft) {
                mid = rgt;
                lft++;
            } else if(mid == rgt) {
                mid = lft;
                rgt--;
            } else {
                lft++;
                rgt--;
            }
        }
    }
    if(rgt > beg) {
        ret = quickSort(idx, beg, (word) rgt);
        CHECK(ret >= 0, lblKO);
        num += ret;
    }
    if(lft < end) {
        ret = quickSort(idx, (word) lft, end);
        CHECK(ret >= 0, lblKO);
        num += ret;
    }
    ret = num;
    goto lblEnd;
  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

/** Quick sort of the index
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return number of swaps (zero if it is already sorted); negative otherwise
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0171.c
*/
int idxSort(tIndex * idx) {
    return quickSort(idx, 0, (word) (idxDim(idx) - 1));
}

/** Insert the number of current row into the sorted index array
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return the position of the row in the sorted array
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
int idxIns(tIndex * idx) {
    int ret;
    char buf[256];
    word pos;
    word *ptr1;
    word *ptr2;
    char *key1;
    char *key2;

    pos = recPos(idx->rec, idx->tab);   //get current cursor position in the multirecord
    
    //get current key value
    ret = cntGet(idx->fld, NULLKEY, buf, sizeof(buf));
    CHECK(ret >= 0, lblKO);
    if(idx->dim) {
        //find the place to insert
        ret = idxFind(idx, buf, 0, (word) (idx->dim - 1));
        CHECK(ret == tabDim(idx->tab), lblDup); //duplicated keys are not allowed

        //move the tail forward if necessary
        if(idx->cur < idx->dim) {
            ptr1 = &idx->idx[idx->cur];
            ptr2 = ptr1 + 1;
            memmove(ptr2, ptr1, ((idx->dim) - (idx->cur)) * sizeof(word));
            if(idx->key) {
                key1 = (idx->key) + (idx->cur) * (idx->len);
                key2 = key1 + (idx->len);
                memmove(key2, key1, ((idx->dim) - (idx->cur)) * (idx->len));
            }
        }
    }
    //Now insert
    idx->dim++;                 //increment number of indexed records
    idx->idx[idx->cur] = pos;   //it is the new element
    if(idx->key)
        memcpy(idx->key + (idx->cur) * (idx->len), buf, (idx->len));
    ret = pos;
    goto lblEnd;
  lblDup:                      //the key already exists in multirecord; duplicated keys are not allowed
    ret = -2;
    goto lblEnd;
  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    trcFN("idxIns ret=%d\n", ret);
    return ret;
}

/** Delete the number current record from index array
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return the position of the row deleted
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0123.c
*/
int idxDel(tIndex * idx) {
    word *ptr1;
    word *ptr2;
    char *key1;
    char *key2;
    word num;

    idx->dim--;

    ptr1 = &idx->idx[idx->cur];
    ptr2 = ptr1 + 1;
    num = (idx->dim) - (idx->cur);
    memmove(ptr1, ptr2, num * sizeof(word));
    idx->idx[idx->dim] = 0;

    if(idx->key) {
        key1 = (idx->key) + (idx->cur) * (idx->len);
        key2 = key1 + (idx->len);
        memmove(key1, key2, num * (idx->len));
        memset(idx->key + (idx->dim) * (idx->len), 0, idx->len);
    }

    return (idx->cur);
}

/** Append one more index to index array without verifying the order.
 *
 * The function idxSort should be called after it and before using the index.
 *
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return the index dimension
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0171.c
*/
int idxApp(tIndex * idx) {
    idx->idx[idx->dim] = idx->dim;
    idx->dim++;
    return idx->dim;
}

/** Calculate the size in DFS needed to save the index.
 * The following data are saved:
 *  - number of indexed rows (dim)
 *  - array of row numbers (idx)
 *  - array of keys optionally
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return index size
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
card idxSize(const tIndex * idx) {
    card tot;

    tot = sizeof(word);
    tot += sizeof(word) * tabDim(idx->tab);
    if(idx->key)
        tot += idx->len * tabDim(idx->tab);
    return tot;
}

/** Save the index to DFS
 * The following data are saved:
 *  - number of indexed rows (dim)
 *  - array of row numbers (idx)
 *  - array of keys optionally
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return number of indexed rows
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
int idxSave(tIndex * idx) {
    int ret;
    card ofs;
    word len;

    ofs = idx->ofs;
    len = sizeof(idx->dim);
    ret = nvmSave(idx->sec, &idx->dim, ofs, len);
    CHECK(ret >= 0, lblKO);

    ofs += len;
    len = idx->dim * sizeof(word);
    ret = nvmSave(idx->sec, idx->idx, ofs, len);
    CHECK(ret >= 0, lblKO);

    if(idx->key) {
        ofs += len;
        len = idx->dim * idx->len;
        ret = nvmSave(idx->sec, idx->key, ofs, len);
        CHECK(ret >= 0, lblKO);
    }

    idx->cur = 0;
    ret = idx->dim;
    goto lblEnd;
  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

/** Load the index to DFS
 * The following data are loaded:
 *  - number of indexed rows (dim)
 *  - array of row numbers (idx)
 *  - array of keys optionally
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return number of indexed rows
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
int idxLoad(tIndex * idx) {
    int ret;
    card ofs;
    word len;

    ofs = idx->ofs;
    len = sizeof(idx->dim);
    ret = nvmLoad(idx->sec, &idx->dim, ofs, len);
    CHECK(ret >= 0, lblKO);

    ofs += len;
    len = idx->dim * sizeof(word);
    ret = nvmLoad(idx->sec, idx->idx, ofs, len);
    CHECK(ret >= 0, lblKO);

    if(idx->key) {
        ofs += len;
        len = idx->dim * idx->len;
        ret = nvmLoad(idx->sec, idx->idx, ofs, len);
        CHECK(ret >= 0, lblKO);
    }

    idx->cur = 0;
    ret = idx->dim;
    goto lblEnd;
  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

/** Erase the index from DFS, clear the array of indexed rows and reset dim
 * \param idx (I) The pointer to the index descriptor
 * \pre 
 *    - idx!=0
 * \return 0 if OK, negative if failure
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
card idxReset(tIndex * idx) {
    int ret;

    ret = nvmSet(idx->sec, 0, idx->ofs, idxSize(idx));
    CHECK(ret >= 0, lblKO);

    memset(idx->idx, 0, tabDim(idx->tab) * sizeof(word));
    if(idx->key)
        memset(idx->key, 0, tabDim(idx->tab) * (idx->len));
    idx->dim = 0;

    ret = 0;
    goto lblEnd;
  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

/** Move the cursor to the position number pos in the sorted array.
 * This function can be used to retrieve records in sorted order.
 * \param idx (I) The pointer to the index descriptor
 * \param pos (I) Position in the array of sorted row numbers
 * \pre 
 *    - idx!=0
 * \return 0 if OK, negative if failure
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0122.c
*/
int idxSet(tIndex * idx, word pos) {
    int ret;

    CHECK(pos < idxDim(idx), lblKO);
    idx->cur = pos;
    recMove(idx->rec, idx->tab, idx->idx[idx->cur]);
    ret = pos;
    goto lblEnd;
  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

//the number of bytes in the hash cell: key[len]+vac
#define DIMCELL (word)(hsh->len+sizeof(word))

//number of bytes in the hash block: end+vac+cell[dimBlk]
#define DIMBLK (word)(sizeof(word)*2+(hsh->dimBlk)*DIMCELL)

//number of bytes in the overflow area: end+vac+cell[dimOvf]
#define DIMOVF (word)(sizeof(word)*2+(hsh->dimOvf)*DIMCELL)

//number of bytes in the hash index
#define DIMHSH (card)((hsh->dimHsh)*DIMBLK+DIMOVF)

#define hshOfsLim(blk) (card)((hsh->ofs) + (DIMBLK*blk))
#define hshOfsVac(blk) (card)(hshOfsLim(blk)+sizeof(word))
#define hshOfsBeg(blk) (card)(hshOfsVac(blk)+sizeof(word))
static int hshGetLim(tHash * hsh, word blk) {   //Retrieve number of filled cells in the block
    int ret;
    card ofs;
    word val;

    if(blk == hsh->dimHsh) {    //if it is overflow area, look in cache
        if(hsh->limOvf <= hsh->dimOvf) {    //if already set, get it from RAM
            val = hsh->limOvf;
            goto lblEnd;
        }
    }
    ofs = hshOfsLim(blk);
    ret = nvmLoad(hsh->sec, &val, ofs, sizeof(word));
    if(ret != sizeof(word))
        return -1;
    if(blk == hsh->dimHsh) {    //if it is overflow area, put it in cache
        hsh->limOvf = val;
    }
  lblEnd:
    return val;
}

static int hshPutLim(tHash * hsh, word blk, word val) { //Save number of filled cells in the block
    int ret;
    card ofs;

    ofs = hshOfsLim(blk);
    if(blk == hsh->dimHsh) {    //if it is overflow area, put it in cache
        hsh->limOvf = val;
    }
    ret = nvmSave(hsh->sec, &val, ofs, sizeof(word));
    if(ret != sizeof(word))
        return -1;
    return val;
}

static int hshGetVac(tHash * hsh, word blk) {   //Get the index of the first vacant cell in the block
    int ret;
    card ofs;
    word val;

    if(blk == hsh->dimHsh) {    //if it is overflow area, look in cache
        if(hsh->vacOvf <= hsh->dimBlk)
            return hsh->vacOvf; //if already set, get it from RAM
    }
    ofs = hshOfsVac(blk);
    ret = nvmLoad(hsh->sec, &val, ofs, sizeof(word));
    if(ret != sizeof(word))
        return -1;
    if(blk == hsh->dimHsh) {    //if it is overflow area, put it in cache
        hsh->vacOvf = val;
    }
    return val;
}

static int hshPutVac(tHash * hsh, word blk, word val) { //Put the index of the first vacant cell in the block
    int ret;
    card ofs;

    ofs = hshOfsVac(blk);
    ret = nvmSave(hsh->sec, &val, ofs, sizeof(word));
    if(ret != sizeof(word))
        return -1;
    if(blk == hsh->dimHsh) {    //if it is overflow area, put it in cache
        hsh->vacOvf = val;
    }
    return val;
}

#define hshOfsKey(blk,cell) (hshOfsBeg(blk)+(DIMCELL*cell))
#define hshOfsIdx(blk,cell) (hshOfsKey(blk,cell)+(hsh->len))
static int hshGetIdx(tHash * hsh, word blk, word cell) {    //Get the index value
    int ret;
    card ofs;
    word val;

    ofs = hshOfsIdx(blk, cell);
    ret = nvmLoad(hsh->sec, &val, ofs, sizeof(word));
    if(ret != sizeof(word))
        return -1;
    return val;
}

static int hshPutCell(tHash * hsh, word blk, word cell, const byte * key, word idx) {   //Put the (key,idx) cell value
    int ret;
    card ofs;
    byte tmp[256];

    memcpy(tmp, key, hsh->len);
    memcpy(tmp + hsh->len, &idx, sizeof(word));
    ofs = hshOfsKey(blk, cell);
    ret = nvmSave(hsh->sec, tmp, ofs, (word) (hsh->len + sizeof(word)));
    return ret;
}

/** Hash index descriptor constructor.
 * The hash index contains an a set of hash blocks.
 *
 * Each hash block is an array of (key,index) cells where
 *  - key is the key field in the target multirecord where a search is to be performed
 *  - idx is the number of row in the multirecord that corresponds to the key value
 *
 * \param hsh (O) The pointer to the hash index descriptor to be initialized
 * \param sec (I) Section where to save the hash index
 * \param ofs (I) Offset within the section
 * \param len (I) Length of hash key
 * \param dimHsh (I) Number of hash blocks
 * \param dimBlk (I) Number of (key,index) pairs in each hash block
 * \param dimOvf (I) Number of (key,index) pairs in overflow area
 * \pre 
 *    - hsh!=0
 *    - sec!=0xFF
 *    - len!=0
 *    - dimHsh!=0
 *    - dimBlk!=0
 * \return the size of non-volatile memory occupied by hash index
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0172.c
 *
 * The number of hash block where to store (key,index) value is calculated using hashJerkinsBin(key,len) call.
 * To retrieve an index corresponding a given key a linear search within the hash block is performed.
 * 
 * If a hash block is full the cell is put into overfloaw area.
 * If overflow area is full, no more keys can be inserted into hash index.
 *
 * If a (key,index) pair in hash block is not vacant (not occupied), the key value is equal to the key
 * value in the target multirecord; the index value has the following meaning
 *  - the leftmost bit is set to 1
 *  - other bits provide the number of the row in the target multirecord
 *
 * If the pair is vacant, the key is filled by zeroes, and the index value has the following meaning
 *  - the leftmost bit is set to 0
 *  - if the index iz zero it means that the next cell in hash block is also vacant
 *  - otherwise the 15 last bits of the the index is the (cell number+1) of the next vacant cell
 *
 * When a key is to be deleted from the hash index, it is done in the following way:
 *  - find the hash block containing the key
 *  - fill key in the cell by zeroes
 *  - put the leftmost bit of index to zero
 *  - get the value of the first vacant key in the block that is saved at the beginning of the block
 *    and increment it by 1 and put it into index
 *  - replace the index of the first vacanty cell by the number of deleted cell
 *
 * To insert a new key into hash index the following steps should be done:
 *  - find the hash block containing the key
 *  - get the number of the first vacant cell from the beginning of the hsah block
 *  - copy the key value into the cell
 *  - set the leftmost bit of the index value to 1
 *  - get the value of the next vacant element from the index (decrementing it by 1)
 *  - replace the number of the first vacant element in vac array
 *  - put the number of the target multirecord row into index
 *
 * If the index of the first vacant cell is equal to dimBlk it means that there is no more places in hash block.
 *
 * Overflow area is treated in a similar way as a normal hash block.
 * However, the size of overflow area is dimOvf, not dimBlk.
 * The keys that are in overflow area can have different hash values.
 *
 * At the beginning of each hash block (normal and overflow area) two values are saved
 * to improve performance:
 *  - cut: the block cut-off index: cells with indices greater than or equal to cut are empty
 *  - vac: the index of the first vacant cell (if equal to the block dimension, the block is full)
 * 
 * The cut-off index reduces the time of find operation:
 * the block is checked until cut-off index;
 * if the block is empty (cut==0), it is not searched at all.
 *
 * The vac value saves the time of delete/insert operations:
 * the vacant cells are treated as a list, the head of the list pointed by this index.
 *
 * The vac and cut values of overflow area are cached in hsh record: vacOvf and cutOvf.
 * They are retrieved form DFS only once; then they are accessed form RAM.
 *
 * The clean operation attempts to put all keys from overflow area to normal hash block;
 * for each hash block vacant cells are moved to the end.
*/
int hshInit(tHash * hsh, byte sec, card ofs, byte len, byte dimHsh, word dimBlk, word dimOvf) { //The mapping of non volatile memory occupied by the hash index data:
    int ret;

    memset(hsh, 0, sizeof(tHash));
    hsh->sec = sec;
    hsh->ofs = ofs;
    hsh->len = len;
    hsh->dimHsh = dimHsh;
    hsh->dimBlk = dimBlk;
    hsh->dimOvf = dimOvf;
    hsh->limOvf = dimOvf + 1;   //means that the value is not set yet
    hsh->vacOvf = dimOvf + 1;   //means that the value is not set yet
    ret = DIMHSH;
    return ret;
}

/** Reset all hash data to zeroes
 * \param hsh (I) The pointer to the hash descriptor
 * \pre 
 *    - hsh!=0
 * \return number of saved bytes; negative in case of problems
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0172.c
*/
int hshReset(tHash * hsh) {
    int ret;

    hsh->limOvf = 0;            //all cells are vacant in overflow area
    hsh->vacOvf = 0;            //index of the first vacant cell in the overflow area
    ret = nvmSet(hsh->sec, 0, hsh->ofs, DIMHSH);
    trcFN("hshReset ret=%d\n", ret);
    return ret;
}

static byte hshIdx(const byte * key, byte len, word dim) {
    card idx;

    idx = hashJerkinsBin(key, len);
    idx =
        HBYTE(HWORD(idx)) ^ LBYTE(HWORD(idx)) ^ HBYTE(LWORD(idx)) ^
        LBYTE(LWORD(idx));
    return (byte) (idx % dim);
}

/** Insert a new key into hash index.
 * 
 * The key should be unique; however, for the sake of performance it is not checked.
 * \param hsh (I) The pointer to the hash descriptor
 * \param key (I) Key value
 * \param idx (I) Row index in the target multirecord
 * \pre 
 *    - hsh!=0
 *    - key!=0
 * \return 1 if success; negative otherwise
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0172.c
*/
int hshIns(tHash * hsh, const byte * key, word idx) {
    int ret;
    word blk;                   //number of hash block where to add the cell
    word vac;                   //index of the first vacant cell
    word lim;                   //number of filled cells
    word ins;                   //index of the cell where to insert

    nvmHold(hsh->sec);
    blk = hshIdx(key, hsh->len, hsh->dimHsh);   //the hash block index where to save the cell

    ret = hshGetVac(hsh, blk);  //get the index of the first vacant cell in the block
    CHECK(ret >= 0, lblKO);
    vac = ret;
    
    if(vac >= hsh->dimBlk) {    //no vacant cells, block is full; look overflow area
        blk = hsh->dimHsh;      //overflow area

        ret = hshGetVac(hsh, blk);  //get the index of the first vacant cell in the block
        CHECK(ret >= 0, lblKO);
        vac = ret;
        CHECK(vac < hsh->dimOvf, lblKO);    //if there is no cell in overflow area no more keys can be saved
    }
    ret = hshGetLim(hsh, blk);  //get the block cut-off index
    CHECK(ret >= 0, lblKO);
    lim = ret;

    ins = vac;                  //it is the vacant cell number where the key is to be inserted
    
    ret = hshGetIdx(hsh, blk, ins); //in vacant cells idx contains the pointer to the next vacant cell
    CHECK(ret >= 0, lblKO);

    vac = ret;                  //it will be the  new index of the first vacant cell

    if(!vac) {                  //zero value means that the next cell is vacant
        vac = ins + 1;
    } else {
        vac--;                  //vac starts from zero
    }

    idx |= 0x8000;              //set 'occupied' bit
    ret = hshPutCell(hsh, blk, ins, key, idx);  //save the key and index into the cell
    CHECK(ret == (int) ((hsh->len) + sizeof(word)), lblKO);

    ret = hshPutVac(hsh, blk, vac); //save new value of the first vacant cell
    CHECK(ret >= 0, lblKO);
    lim++;
    ret = hshPutLim(hsh, blk, lim); //increment number of filled cells
    CHECK(ret >= 0, lblKO);

    ret = 1;
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    nvmRelease(hsh->sec);
    return ret;
}

typedef struct sLookResult {
    card ofs;
    word cell;
    word idx;
} tLookResult;
static int hshLook(const byte * key, byte sec, card ofs, byte len, word dim,
                   tLookResult * res) {
    int ret;
    word num;
    byte buf[256];
    word cell;

    if(res)
        memset(res, 0, sizeof(*res));
    cell = 0;
    while(dim) {
        ret = nvmLoad(sec, buf, ofs, (word) (len + sizeof(word)));  //get (key,idx) cell
        CHECK(ret == (int) (len + sizeof(word)), lblKO);
        memcpy(&num, buf + len, sizeof(word));  //get index into num
        if(num & 0x8000) {      //if not vacant, check it
            if(memcmp(key, buf, len) == 0) {    //found
                num &= 0x7FFF;  //clear 'occupied' bit
                if(res) {
                    res->idx = num;
                    res->cell = cell;
                    res->ofs = ofs;
                }
                return num;
            }
            dim--;
        }
        ofs += len + sizeof(word);
        cell++;
    }
  lblKO:
    return -1;
}

/** Find the index in the target multirecord that is associated to the given key
 * \param hsh (I) The pointer to the hash descriptor
 * \param key (I) Key value
 * \pre 
 *    - hsh!=0
 *    - key!=0
 * \return the index in the target multirecord ; negative if failure
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0172.c
 */
int hshFind(tHash * hsh, const byte * key) {
    int ret;
    word blk;                   //index of hash block to look
    word lim;                   //number of filled cells in the block
    card ofs;

    nvmHold(hsh->sec);

    blk = hshIdx(key, hsh->len, hsh->dimHsh);   //the hash block index where the cell should be saved normally
    ret = hshGetLim(hsh, blk);  //get the number of filled cells
    CHECK(ret >= 0, lblKO);
    lim = ret;

    if(lim == 0)
        goto lblNotFound;       //the block is empty
    ofs = hshOfsBeg(blk);       //block offset in non volatile memory
    ret = hshLook(key, hsh->sec, ofs, hsh->len, lim, 0);    //look in the normal hash block
    if(ret >= 0)
        goto lblEnd;

    blk = hsh->dimHsh;          //now look overflow area
    ret = hshGetLim(hsh, blk);  //get the number of filled cells
    CHECK(ret >= 0, lblKO);
    lim = ret;

    if(lim == 0)
        goto lblNotFound;       //the block is empty
    ofs = hshOfsBeg(blk);       //block offset in non volatile memory
    ret = hshLook(key, hsh->sec, ofs, hsh->len, lim, 0);    //look in the overflow area
    if(ret >= 0)
        goto lblEnd;

  lblNotFound:
    ret = -1;
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    nvmRelease(hsh->sec);
    trcFN("hshFind ret=%d\n", ret);
    return ret;
}

/** Remove the key from hash index
 * \param hsh (I) The pointer to the hash descriptor
 * \param key (I) Key value
 * \pre 
 *    - hsh!=0
 *    - key!=0
 * \return
 *  - 1 if removed
 *  - 0 if not found
 *  - negative if failure
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0172.c
*/
int hshDel(tHash * hsh, const byte * key) {
    int ret;
    word blk;                   //number of hash block
    card ofs;
    word lim, vac;
    tLookResult res;
    byte buf[256];

    nvmHold(hsh->sec);
    blk = hshIdx(key, hsh->len, hsh->dimHsh);   //the hash block index where the cell should be
    ret = hshGetLim(hsh, blk);  //get the number of filled cells
    CHECK(ret >= 0, lblKO);
    lim = ret;

    if(lim == 0)
        goto lblNotFound;       //the block is empty
    ofs = hshOfsBeg(blk);       //block offset in non volatile memory
    ret = hshLook(key, hsh->sec, ofs, hsh->len, lim, &res); //look in the normal hash block
    if(ret >= 0)
        goto lblFound;

    blk = hsh->dimHsh;          //now look overflow area
    ret = hshGetLim(hsh, blk);  //get the number of filled cells
    CHECK(ret >= 0, lblKO);
    lim = ret;
    
    if(lim == 0)
        goto lblNotFound;       //the block is empty
    ofs = hshOfsBeg(blk);       //block offset in non volatile memory
    ret = hshLook(key, hsh->sec, ofs, hsh->len, lim, &res); //look in the overflow area
    if(ret >= 0)
        goto lblFound;

  lblNotFound:
    ret = 0;
    goto lblEnd;

  lblFound:
    ret = hshGetVac(hsh, blk);  //get the number of the first vacant cell
    CHECK(ret >= 0, lblKO);
    vac = ret;
    vac++;

    ret = hshGetLim(hsh, blk);  //get the number of filled cells
    CHECK(ret >= 0, lblKO);
    lim = ret;

    memset(buf, 0, hsh->len);
    ret = hshPutCell(hsh, blk, res.cell, buf, vac);
    CHECK(ret == (int) (hsh->len + sizeof(word)), lblKO);

    ret = hshPutVac(hsh, blk, res.cell);    //replace the value of the first vacant cell
    CHECK(ret >= 0, lblKO);

    
    lim--;
    ret = hshPutLim(hsh, blk, lim); //decrement number of filled cells
    CHECK(ret >= 0, lblKO);

    ret = 1;
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    nvmRelease(hsh->sec);
    return ret;
}

static int hshCleanDelInsOvf(tHash * hsh) {
    int ret;
    byte buf[256];
    card ofs;
    word idx, num;

    ofs = hshOfsBeg(hsh->dimHsh);   //the starting offset of the first cell in overflow area
    for (idx = 0; idx < hsh->dimOvf; idx++) {
        ret = nvmLoad(hsh->sec, buf, ofs, DIMCELL); //load a cell
        CHECK(ret >= 0, lblKO);
        ofs += DIMCELL;
        memcpy(&num, buf + hsh->len, sizeof(word));
        if((num & 0x8000) == 0) //if it is vacant, just continue
            continue;
        //otherwise delete and insert the key: maybe it will be inserted into normal hash block
        ret = hshDel(hsh, buf);
        CHECK(ret >= 0, lblKO);
        num &= 0x7FFF;
        ret = hshIns(hsh, buf, num);
        CHECK(ret >= 0, lblKO);
    }
    ret = 1;
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    trcFN("hshCleanDelInsOvf ret=%d\n", ret);
    return ret;
}

static int hshCleanBlk(byte sec, card ofs, word dimBlk, byte len) {
    int ret;
    byte buf[256];
    word num, vac, lim;
    int lft, rgt;
    byte dimCel;

    ret = nvmLoad(sec, &lim, ofs, sizeof(word));
    CHECK(ret == sizeof(word), lblKO);
    ofs += sizeof(word);

    ret = nvmLoad(sec, &vac, ofs, sizeof(word));
    CHECK(ret == sizeof(word), lblKO);
    ofs += sizeof(word);

    dimCel = len + sizeof(word);
    lft = 0;
    rgt = dimBlk - 1;
    if(0 < lim && lim < dimBlk) {
        while(lft < rgt) {
            while(lft <= rgt) { //look for the next vacant cell (->)
                ret = nvmLoad(sec, buf, ofs + lft * dimCel, dimCel);
                CHECK(ret >= 0, lblKO);
                memcpy(&num, buf + len, sizeof(word));
                if((num & 0x8000) == 0)
                    break;
                lft++;
            }
            while(lft <= rgt) { //look for the next non-vacant cell (<-)
                ret = nvmLoad(sec, buf, ofs + rgt * dimCel, dimCel);
                CHECK(ret >= 0, lblKO);
                memcpy(&num, buf + len, sizeof(word));
                if((num & 0x8000) != 0)
                    break;
                rgt--;
            }
            trcFN("hshCleanBlk lft=%d", lft);
            trcFN(" rgt=%d\n", rgt);
            if(lft >= rgt)
                break;
            ret = nvmSave(sec, buf, ofs + lft * dimCel, dimCel);    //move the rgt non-vacant cell to lft vacant cell
            CHECK(ret >= 0, lblKO);
            lft++;
            rgt--;
        }
    }

    if(lim < dimBlk) {          //fill the vacant tail by zeroes
        ret = nvmSet(sec, 0, ofs + lim * dimCel, (dimBlk - lim) * dimCel);
        CHECK(ret >= 0, lblKO);
    }

    ofs -= sizeof(word);        //go to the offset of the vac
    vac = lim;                  //the first vacant cell is located just after the last filled cell

    ret = lim;
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    trcFN("hshCleanBlk ret=%d\n", ret);
    return ret;
}

/** Clean hash table index
 * \param hsh (I) The pointer to the hash descriptor
 * \pre 
 *    - hsh!=0
 * \return
 *  - 1 if OK
 *  - negative if failure
 * 
 * This function moves the elements from overflow area to ordinary blocks whenever possible.
 * Then in each block all the elements are moved to the beginning of the block.
 * The vacant cells are moved to the end of each block.
 * The clean operation makes the eventual search operation more performant.
 * 
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dba.c
 * \test tcab0172.c
*/
int hshClean(tHash * hsh) {
    int ret;
    word blk;
    word dim;

    //for each element in overflow area insert and delete it
    //in this way the cells are moved to normal hash blocks whenever possible
    ret = hshCleanDelInsOvf(hsh);
    CHECK(ret >= 0, lblKO);

    for (blk = 0; blk <= hsh->dimHsh; blk++) {
        dim = hsh->dimBlk;
        if(blk == hsh->dimHsh)
            dim = hsh->dimOvf;
        trcFN("hshCleanBlk blk=%d\n", blk);
        ret = hshCleanBlk(hsh->sec, hshOfsLim(blk), dim, hsh->len);
        CHECK(ret >= 0, lblKO);
        dim = ret;
        ret = hshPutVac(hsh, blk, dim); //replace the value of the first vacant cell
        CHECK(ret >= 0, lblKO);
    }

    ret = 1;
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    trcFN("hshClean ret=%d\n", ret);
    return ret;
}

int hshScanBeg(tHash * hsh) {
    int ret;
    char src[128];

    
    hsh->scanBlk = 0;
    hsh->scanIdx = 0;

    nvmFileName(src, hsh->sec);
    hsh->scanHdl = nvmSeqOpen(src, 'r');
    if(hsh->scanHdl == 0)
        return -1;
    ret = nvmSeqPos(hsh->scanHdl, hsh->ofs);
    if(ret < 0)
        return -1;
    return 1;
}

int hshScanEnd(tHash * hsh) {
    
    nvmSeqClose(hsh->scanHdl);
    hsh->scanHdl = 0;
    hsh->scanBlk = 0;
    hsh->scanIdx = 0;
    return 0;
}

int hshScan(tHash * hsh, void *key, word * idx) {
    int ret;
    word dim, lim, vac;

    if(hsh->scanIdx == 0) {
        if(hsh->scanBlk > hsh->dimHsh)  //finished
            return -1;
        ret = nvmSeqGet(hsh->scanHdl, &lim, sizeof(word));  //number of filled cells in current block
        CHECK(ret >= 0, lblKO);
        ret = nvmSeqGet(hsh->scanHdl, &vac, sizeof(word));  //index of the first vacant cell
        CHECK(ret >= 0, lblKO);
    }

    ret = nvmSeqGet(hsh->scanHdl, key, hsh->len);
    CHECK(ret >= 0, lblKO);
    ret = nvmSeqGet(hsh->scanHdl, idx, sizeof(word));
    CHECK(ret >= 0, lblKO);

    hsh->scanIdx++;
    dim = (word) ((hsh->scanBlk == hsh->dimHsh) ? hsh->dimOvf : hsh->dimBlk);
    if(hsh->scanIdx >= dim) {   //next block
        hsh->scanBlk++;
        hsh->scanIdx = 0;
    }

    vac = (*idx) & 0x8000;
    if(vac == 0)                //vacant cell
        return 0;

    *idx &= 0x7FFF;
    return 1;
  lblKO:
    return -2;
}

