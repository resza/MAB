/** \file
 * Toolbox: various functions used in system group of components
 *
 * Subversion reference
 *
 * $HeadURL: https://svn.ingenico.com/UTA/core/utaYYMMDD/sys/tbx.c $
 *
 * $Id: tbx.c 2565 2010-02-02 01:45:37Z jlapara $
 */
#include "global.h"
//#include <stdio.h>
//#include <string.h>
//#include "sys.h"

//#undef trcOpt
//#define trcOpt (TRCOPT & BIT(trcTBX))

const byte *bPtr(byte val) {
    static byte x;

    x = val;
    return &x;
}

const word *wPtr(word val) {
    static word x;

    x = val;
    return &x;
}

const card *cPtr(card val) {
    static card x;

    x = val;
    return &x;
}

const byte *dPtr(const char *s) {
    return (byte *) s;
}

//get number of digits in the numeric value based on base
static byte getNumLen(card num, byte base) {
    byte ret;

    ret = 0;
    while(num) {
        ret++;
        num /= base;
    }
    return ret;
}

/** Convert a numeric value num into a decimal string dec of length len. 
 * The zero value of len means that the length is to be calculated as the number of decimal digits in num.
 * If the argument len is too small, zero is returned; otherwise the number of characters converted is returned.
 * \param dec (O) Destination zero-ended string to be filled by (len+1) characters. 
 * \param num (I) Numeric value to be converted.
 * \param len (I) Number of characters in the destination.
 * \pre dec!=0
 * \remark len is too small, i.e. the number of decimal digits in num is more than len
 * \return
 *    - strlen(dec) if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte num2dec(char *dec, card num, byte len) {
    byte ret;                   //to be returned: number of characters processed
    byte idx;                   //current index

    //normally len is the length of the acceptor
    //if len is large, destination is zero padded from left
    if(!len) {                  //calculate len to be equal to the number of decimal digits in num
        len = getNumLen(num, 10);
        if(!len)
            len = 1;            //zero should be convertod to "0"
    }

    ret = len;                  //number of bytes processed is returned
    idx = len;
    dec[idx] = 0;
    while(idx--) {              //fill dec from right to left
        dec[idx] = (char) ((num % 10) + '0');
        num /= 10;
    }
    CHECK(!num, lblKO);         //error: len too small
    return ret;
  lblKO:
    return 0;
}

static byte chr2nib(char chr) { //convert hexadecimal character to nibble
    switch (chr) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
          return (byte) (chr - '0');
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
          return (byte) (chr - 'A' + 10);
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
          return (byte) (chr - 'a' + 10);
      default:
          break;
    }
    return 0x10;                //KO
}

static char nib2chr(byte nib) { //convert nibble to hexadecimal character
    if(nib < 10)
        return (char) ('0' + nib);
    return (char) ('A' + (nib - 10));
}

/** Convert a decimal string dec of length len into a numeric value num. 
 * The zero value of len means that the length is to be calculated as the strlen(dec).
 * The spaces and tabs at the beginning of dec are ignored.
 * If operation is performed successfully, the number of characters converted is returned.
 * In case of invalid input (empty string, too long string, invalid character) zero value is returned.
 * \param       dec (I) Source string containing decimal digits only.
 * \param num (O) Pointer to the result numeric value.
 * \param len (I) Number of characters in the destination.
 * \pre 
 *    - num!=0
 *    - dec!=0
 * \remarks
 *    - dec contains blank characters only.
 *        - dec contains an invalid character (non-decimal digit).
 *        - dec is too long (i.e. it is not in the card range 00000000..FFFFFFFF).
 * \return 
 *    - number of characters processed from dec if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte dec2num(card * num, const char *dec, byte len) {
    byte ret;                   //to be returned: number of characters processed
    byte tmp;

    //normally len is the number of characters to get from source string
    if(!len)
        len = (byte) strlen(dec);   //calcualte length if missing

    ret = 0;
    *num = 0;
    while((*dec == ' ') || (*dec == '\t')) {    //skip blank characters
        dec++;
        ret++;
        CHECK(len, lblKO);      //the input is empty
        len--;
    }

    while(*dec) {               //main loop: perform the conversion
        if(*dec < '0')
            break;
        if(*dec > '9')
            break;
        tmp = chr2nib(*dec++);  //convert one more character
        CHECK(tmp < 10, lblKO); //validity check
        CHECK(*num <= 0xFFFFFFFFL / 10, lblKO); //dec exceeds 0xFFFFFFFFL
        CHECK(!((*num == 0xFFFFFFFFL / 10) && (tmp > 5)), lblKO);   //dec exceeds 0xFFFFFFFFL
        *num *= 10;             //OK, we can shift it left
        *num += tmp;            //OK, let us do it
        ret++;                  //one more digit processed
        len--;
        if(!len)
            break;
    }

    return ret;
  lblKO:
    return 0;
}

/** Convert a numeric value num into a hexadecimal string hex of length len. 
 * The zero value of len means that the length is to be calculated as the number of hexadecimal digits in num.
 * If the argument len is too small, zero is returned; otherwise the number of characters converted is returned.
 * \param hex (O) Destination zero-ended string to be filled by (len+1) characters. 
 * \param num (I) Numeric value to be converted.
 * \param       len (I) Number of characters in the destination.
 * \pre hex!=0
 * \remark len too small, i.e. the number of hexadecimal digits in num is more than len.
 * \return
 *    - strlen(hex) if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte num2hex(char *hex, card num, byte len) {
    byte ret;                   //to be returned: number of characters processed
    byte idx;                   //current index

    //normally len is the length of the acceptor
    //if len is large, destination is zero padded from left
    if(!len) {                  //calculate len to be equal to the number of hexadecimal digits in num
        len = getNumLen(num, 16);
        if(!len)
            len = 1;            //zero should be convertod to "0"
    }

    ret = len;
    idx = len;
    hex[idx] = 0;
    while(idx--) {              //fill hex from right to left
        hex[idx] = (byte) (num % 16);
        if(hex[idx] < 10)
            hex[idx] += (char) '0';
        else
            hex[idx] += (char) ('A' - 10);
        num /= 16;
    }
    CHECK(!num, lblKO);         //error: len is too small
    return ret;
  lblKO:
    return 0;
}

/** Convert a hexadecimal string hex of length len into a numeric value num. 
 * The zero value of len means that the length is to be calculated as the strlen(hex).
 * The spaces and tabs at the beginning of hex are ignored.
 * If operation is performed successfully, the number of characters converted is returned.
 * In case of invalid input (empty string, too long string, invalid character) zero value is returned.
 * \param num (O) Pointer to the result numeric value.
 * \param       hex (I) Source string containing hexadecimal digits only.
 * \param len (I) Number of characters in the destination.
 * \pre
 *    - num!=0
 *    - hex!=0
 * \remark
 *        - hex contains blank characters only.
 *        - hex contains an invalid character (non-hexadecimal digit).
 *    - hex is too long (i.e. it is not in the card range 00000000..FFFFFFFF).
 * \return  
 *    - number of characters processed from hex if OK.
 *    - 0 otherwise. 
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte hex2num(card * num, const char *hex, byte len) {
    byte ret;                   //to be returned: number of characters processed
    byte tmp;

    //normally len is the number of characters to get from source string
    if(!len)
        len = (byte) strlen(hex);   //calcualte length if missing

    ret = 0;
    *num = 0;
    while((*hex == ' ') || (*hex == '\t')) {    //skip blank characters
        hex++;
        ret++;
        CHECK(len, lblKO);      //the input is empty
        len--;
    }

    while(*hex) {               //main loop: perform the conversion
        tmp = chr2nib(*hex++);  //convert one more character
        CHECK(tmp < 0x10, lblKO);   //validity check
        CHECK(*num < 0xFFFFFFFL, lblKO);    //is it too large?
        *num *= 16;             //OK, we can shift it left
        CHECK(*num < 0xFFFFFFFFL - tmp, lblKO); //can we add one more digit?
        *num += tmp;            //OK, let us do it
        ret++;                  //one more digit processed
        len--;
        if(!len)
            break;
    }

    return ret;
  lblKO:
    return 0;
}

/** Convert a binary buffer bin of length len  into a hexadecimal string hex. 
 * The return value is always len*2.
 * \param hex (O) Destination zero-ended string to be filled by (2*len+1) characters. 
 * \param bin (I) Pointer to the binary buffer to be converted.
 * \param len (O) Number of bytes to convert.
 * \pre
 *    - hex!=0
 *    - bin!=0
 *    - len!=0
 * \remark no
 * \return  
 *    - strlen(hex). There is no possibility of errors: any input data are acceptable.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
int bin2hex(char *hex, const byte * bin, int len) {
    int ret;                    //to be returned: number of characters processed
    byte tmp;

    //normally len is the number of characters to get from source string
    //if(!len) len= strlen((char *)bin); //is not allowed
    ret = len + len;            //each byte is converted into 2 characters
    while(len--) {
        tmp = *bin++;           //get a byte
        *hex++ = nib2chr((byte) (tmp / 16));    //convert first nibble
        *hex++ = nib2chr((byte) (tmp % 16));    //convert second nibble
    }
    *hex = 0;
    return ret;
}

/** Convert a hexadecimal string hex into a binary buffer bin of length len. 
 * The zero value of len means that the length is to be calculated as the strlen(hex).
 * The spaces and tabs at the beginning of hex are not allowed.
 * If operation is performed successfully, the number of bytes converted is returned.
 * In case of invalid input (invalid character) zero value is returned.
 * \param bin (O) Pointer to the buffer to be filled.
 * \param hex (I) Source string containing hexadecimal digits only.
 * \param len (I) Number of characters in the destination.
 * \pre
 *    - bin!=0
 *    - hex!=0
 * \remark 
 *        - The length of hex is not even in case of len==0.
 *        - hex contains an invalid character (non-hexadecimal digit).
 * \return
 *    - number of bytes written into bin if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
int hex2bin(byte * bin, const char *hex, int len) {
    int ret;                    //to be returned: number of characters processed
    byte tmp;

    //if non-zero, len is the length of acceptor buffer bin
    if(!len) {                  //calcualte length if missing
        len = strlen(hex);
        //CHECK(len % 2 == 0, lblKO); //there should be 2 source characters for each output byte
        //len /= 2;
    }
    ret = 0;
    while(len--) {
        tmp = chr2nib(*hex++);  //get first nibble
        if(tmp >= 0x10)
            break;
        *bin = (byte) (tmp << 4);

        tmp = chr2nib(*hex++);  //get second nibble
        CHECK(tmp < 0x10, lblKO);
        *bin |= tmp;

        bin++;
        ret++;
    }
    return ret;
  lblKO:
    return 0;
}

/** Convert a binary buffer bin of length len into a numeric value num using direct byte order. 
 * The zero value of len is not accepted.
 * The return value is always len.
 * \param num (O) pointer to a numeric value to be calculated.
 * \param       bin (I) pointer to the binary buffer to be converted.
 * \param       len (I) number of bytes to convert.
 * \pre
 *    - num!=0
 *    - bin!=0
 *    - len!=0
 *    - len<=4
 * \remark no
 * \return  
 *    - len. There is no possibility of errors: any input data are acceptable.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte bin2num(card * num, const byte * bin, byte len) {
    byte ret;                   //to be returned: number of characters processed

    //len is the number of characters to get from source string
    ret = len;
    *num = 0;
    while(len--) {
        *num *= 256;
        *num += *bin++;
    }
    return ret;
}

/** Convert a numeric value num into a binary buffer bin of length len using direct byte order.
 * The zero value of len is not accepted.
 * \param bin (O) Pointer to the buffer to be filled.
 * \param num (I) Numeric value to be converted.
 * \param       len (I) Number of bytes in the destination.
 * \pre
 *    - bin!=0
 *    - len!=0
 *    - len<=4
 * \remark len is too small, i.e. the number of non-zero bytes in num is more than len
 * \return
 *    - number of bytes written into bin if OK.
 *    - 0 otherwise. 
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
*/
byte num2bin(byte * bin, card num, byte len) {
    byte ret;                   //to be returned: number of characters processed

    //len is the length of acceptor buffer bib
    ret = len;
    while(len--) {              //fill bin from right to left in direct order
        bin[len] = (byte) (num % 256);
        num /= 256;
    }
    CHECK(!num, lblKO);         //error: len is too small
    return ret;
  lblKO:
    return 0;
}

void tmrPause(byte sec) {
//    tmrStart(0, sec * 100);     //start 0th timer for sec seconds
//    while(tmrGet(0));           //wait until the 0th timer stops
//    tmrStop(0);                 //close the 0th timer
	DelayMs((ushort)(sec * 1000));	
}

/** Initialise the buffer buf of size dim; the memory space pointed to by ptr should be reserved before calling this function.
 * This method should be called before using a buffer.
 * \param buf (O) pointer to buffer descriptor to be initialized
 * \param ptr (I) pointer to a piece of memory reserved for buffer operations
 * \param dim (I) the size of the memory space pointed to by the pointer ptr (buffer dimension)
 * \pre
 *    - buf!=0
 *    - ptr!=0
 *    - dim!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
*/
void bufInit(tBuffer * buf, byte * ptr, word dim) {
    buf->ptr = ptr;
    buf->dim = dim;
    buf->pos = 0;

    bufReset(buf);
}

/** Return buffer dimension,i.e. the maximum number of bytes reserved in buffer memory space. 
 * This value is defined during buffer initialisation.
 * \param buf (I) pointer to buffer descriptor
 * \pre
 *    - buf!=0
 * \return buffer dimension
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
*/
word bufDim(const tBuffer * buf) {
    return buf->dim;
}

/** Return buffer pointer,i.e. the pointer to the reserved space defined during buffer initialisation.
 * \param buf (I) pointer to buffer descriptor
 * \pre
 *    - buf!=0
 * \return pointer to the buffer data
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
*/
const byte *bufPtr(const tBuffer * buf) {
    return buf->ptr;
}

/** Return the number of bytes already filled. 
 * The buffer length is updated by bufSet, bufIns, bufApp, bufCpy functions and can be set to zero by bufReset function. 
 * \param buf (I) pointer to buffer descriptor
 * \pre
 *    - buf!=0
 * \return current buffer position
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
**/
word bufLen(const tBuffer * buf) {
    return buf->pos;
}

/** Fill the reserved memory space by zeroes and reset the current position.
 * \param buf (M) pointer to buffer descriptor
 * \pre
 *    - buf!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
**/
void bufReset(tBuffer * buf) {
    
    memset(buf->ptr, 0, buf->dim);
    buf->pos = 0;
}

/** Set num bytes of the reserved memory space to the value val starting from the current position.
 * The current position is moved by num bytes.
 * \param buf (M) pointer to buffer descriptor
 * \param val (I) the value that is assigned to the next num bytes in the buffer
 * \param num (I) number of bytes to be set
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
**/
int bufSet(tBuffer * buf, byte val, word num) {
    CHECK(buf->pos + num <= buf->dim, lblKO);
    memset(buf->ptr + buf->pos, val, num);
    buf->pos += num;
    return buf->pos;
  lblKO:return -1;
}

/** Insert len bytes before the position ofs and copy the content of dat into it.
 * The current position is moved by len bytes.
 * \param buf (M) pointer to buffer descriptor
 * \param ofs (I) the offset within the buffer where to insert the data
 * \param dat (I) pointer to the data to be inserted
 * \param len (I) number of bytes to be inserted
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
*/
int bufIns(tBuffer * buf, word ofs, const byte * dat, int len) {
    word num;

    num = (word) len;           //number of bytes from dat to operate
    if(len <= 0) {              //calculate it if a special case
        num = (word) strlen((char *) dat);
    }

    CHECK(buf->pos + num <= buf->dim, lblKO);   //is there enough space?
    if(!len) {                  //special case: add zero at the end
        CHECK(buf->pos + num < buf->dim, lblKO);
    }

    if(ofs != buf->pos) {       //if not at the end, the data will be inserted
        memmove(buf->ptr + ofs + num, buf->ptr + ofs, buf->pos - ofs);
    }

    memcpy(buf->ptr + ofs, dat, num);   //now perform the copy operation
    buf->pos += num;            //and move current position
    if(!len) {                  //for the special case append zero
        buf->ptr[buf->pos] = 0;
    }

    return buf->pos;
  lblKO:return -1;
}

/** Write len bytes at the position ofs, overwriting any existing data
 * The current position is updated when written bytes
 * go beyond the original position
 * \param buf (M) pointer to buffer descriptor
 * \param ofs (I) the offset within the buffer where to write the data
 * \param dat (I) pointer to the data to be written
 * \param len (I) number of bytes to be written
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; -1 if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
*/
int bufOwr(tBuffer * buf, word ofs, const byte * dat, int len) {
    int ret;
    word oldPos;

    oldPos = buf->pos;
    buf->pos = ofs;

    ret = bufIns(buf, ofs, dat, len);
    CHECK(ret != -1, lblKO);

    if(oldPos > buf->pos) {
        buf->pos = oldPos;
    }

    return buf->pos;

  lblKO:
    // bufIns failed : restore the original position
    buf->pos = oldPos;
    return -1;
}

/** Delete len bytes from the position ofs.
 * The current position is moved by len bytes.
 * \param buf (M) pointer to buffer descriptor
 * \param ofs (I) the starting offset of data to be deleted
 * \param len (I) number of bytes to be deleted, if zero then delete all
 * \pre
 *    - buf!=0
 * \return current buffer position
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcgs0021.c
*/
int bufDel(tBuffer * buf, word ofs, int len) {
    if(len == 0 || (ofs + len > buf->pos)) {    //"erase" all after ofs
        buf->pos = ofs;
        return buf->pos;
    }

    memmove(buf->ptr + ofs, buf->ptr + ofs + len, buf->pos - ofs - len);
    buf->pos -= len;
    return buf->pos;
}

/** Append len bytes to the end of the buffer and copy the content of dat into it. 
 * The current position is moved by len bytes.
 * \param buf (M) pointer to buffer descriptor
 * \param dat (I) pointer to the data to be appended
 * \param len (I) number of bytes to be appended
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
*/
int bufApp(tBuffer * buf, const byte * dat, int len) {
    return bufIns(buf, buf->pos, dat, len);
}

/** Append the content of src buffer to dst buffer. 
 * The current dst position is moved by the length of src buffer.
 * \param dst (M) pointer to destination buffer descriptor
 * \param src (I) pointer to the source buffer to be appended
 * \pre
 *    - dst!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
*/
int bufCat(tBuffer * dst, tBuffer * src) {
    return bufIns(dst, dst->pos, bufPtr(src), bufLen(src));
}

/** Just call bufReset and bufApp functions.
 * \param buf (M) pointer to buffer descriptor
 * \param dat (I) pointer to the data to be appended
 * \param len (I) number of bytes to be appended
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
*/
int bufCpy(tBuffer * buf, const byte * dat, int len) {
    bufReset(buf);
    return bufApp(buf, dat, len);
}

int bufGet(tBuffer * buf, byte * dat) {
    memcpy(dat, buf->ptr, buf->pos);
    return buf->pos;
}

/** Initialize a queue que linked to the buffer buf.
 * The buffer will be used to store all the elements of the queue and their lengths.
 * \param que (O) pointer to queue descriptor to be initialized
 * \param buf (I) pointer to the buffer that will stroe the queue elements
 * \pre
 *    - que!=0
 *    - buf!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
*/
void queInit(tQueue * que, tBuffer * buf) {
    que->buf = buf;
    queReset(que);
}

/** Empty the queue que.
 * \param que (M) pointer to queue descriptor
 * \pre
 *    - que!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
*/
void queReset(tQueue * que) {
    que->cur = que->put = que->get = 0;
    bufReset(que->buf);
}

/** Put an element of len bytes pointed to by dat in the queue que. 
 * The data are copied into the queue. 
 * The last parameter len can have a special value len==0; it means that the number of bytes should be calculated as strlen(dat).
 * \param que (M) pointer to queue descriptor
 * \param dat (I) pointer to the data to put into the queue
 * \param len (I) length the data to put into the queue
 * \pre
 *    - que!=0
 *    - dat!=0
 * \return number of bytes put into the queue; negative value if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
*/
int quePut(tQueue * que, const byte * dat, int len) {
    int ret;
    byte tmp[2];

    if(!len)
        len = strlen((char *) dat); //calculate len if needed
    tmp[0] = (byte) (len / 256);    //get highest byte
    tmp[1] = (byte) (len % 256);    //get lowest byte

    ret = bufApp(que->buf, tmp, 2); //save element length in direct byte order
    CHECK(ret > 0, lblKO);

    ret = bufApp(que->buf, dat, len);   //save the data itself
    CHECK(ret > 0, lblKO);

    que->put++;                 //increment number of put elements
    que->get++;                 //increment number of get elements

    return len;
  lblKO:return -1;
}

/** Retrieve an element from the queue que into the buffer pointed by dat. 
 * It is assumed that there is enought space in the destination buffer.
 * The size of the date retrieved from the que following the FIFO principle -First In First Out.
 * \param que (M) pointer to queue descriptor
 * \param dat (I) pointer to the data to get from the queue
 * \pre
 *    - que!=0
 *    - dat!=0
 * \return length of data element retrieved; negative value if there is no more data elements to be retrieved
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
*/
int queGet(tQueue * que, byte * dat) {
    word len;

    CHECK(que->get, lblKO);     //are there any element to get?

    len = queNxt(que);          //retrieve the length of the next element
    que->cur += (word) 2;       //skip the length

    memcpy(dat, bufPtr(que->buf) + que->cur, len);  //copy data into destination buffer
    que->cur += len;            //move current position

    que->get--;                 //decrement number of elements to get

    return len;
  lblKO:return -1;
}

/** Get the number of bytes in the next element of queue to be retrieved.
 * \param que (I) pointer to queue descriptor
 * \pre
 *    - que!=0
 * \return the length of the next data element; zero if the current element is the last
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
*/
word queNxt(const tQueue * que) {
    const byte *ptr;
    word len;

    if(!que->get)
        return 0;

    ptr = bufPtr(que->buf) + que->cur;  //ptr points to the length in direct order
    len = *ptr++;               //get highest byte
    len *= (word) 256;
    len += *ptr;                //and now get lowest byte

    return len;
}

/** Return the number of elements in the queue that are saved but not retrieved yet.
 * \param que (I) pointer to queue descriptor to be initialized
 * \pre
 *    - que!=0
 * \return the number of data elements waiting to be retrieved
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
*/
word queLen(const tQueue * que) {
    return que->get;
}

/** Move the reading head of the queue to the begining. 
 * After calling this function the next element to read will be the first element saved.
 * \param que (M) pointer to queue descriptor
 * \pre
 *    - que!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
*/
void queRewind(tQueue * que) {
    que->cur = 0;
    que->get = que->put;
}

/** Put a tag into a queue.
 * This structure is mainly used to transfert parameters to EMV Kernel wrappers. Data object list is 
 * organised as a queue of tags. Two shortcut functions are provided for this type of queues.
 * \param que (M) pointer to queue descriptor
 * \param tag (I) the tag to be put
 * \pre
 *    - que!=0
 * \return number of bytes put into the queue (sizeof(card)); negative value if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
*/
int quePutTag(tQueue * que, card tag) {
    int ret;

    ret = quePut(que, (byte *) & tag, sizeof(card));
    CHECK(ret >= 0, lblKO);

    return ret;
  lblKO:return -1;
}

/** Put a tag into a queue followed by the value val of length len
 * \param que (M) pointer to queue descriptor
 * \param tag (I) the tag to be put
 * \param val (I) pointer to the data containing tag value
 * \param len (I) the length of the tag to be put
 * \pre
 *    - que!=0
 *    - tag!=0
 * \return number of bytes put into the queue; negative value if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
*/
int quePutTlv(tQueue * que, card tag, word len, byte * val) {
    int ret;

    
    if(!len)
        len = (word) strlen((char *) val);
    ret = quePut(que, (byte *) & tag, sizeof(card));
    CHECK(ret >= 0, lblKO);

    ret = quePut(que, val, len);
    CHECK(ret >= 0, lblKO);

    return ret;
  lblKO:return -1;
}

/** Get a tag from a queue
 * \param que (M) pointer to queue descriptor
 * \param tag (I) the tag to be retrieved
 * \pre
 *    - que!=0
 *    - queNxt(que)==sizeof(card)
 * \return number of bytes retrieved (sizeof(card)); negative value if failure
 * \header sys\\sys.h
 * \source sys\\tbx.c
*/
int queGetTag(tQueue * que, card * tag) {
    int ret;

    ret = queGet(que, (byte *) tag);
    
    return ret;
}

/** Get a tag from a queue; then get its length and value.
 * \param que (M) pointer to queue descriptor
 * \param tag (O) the tag to be retrieved
 * \param len (O) length of the data retrieved into val
 * \param val (O) pointer to the data containing tag value
 * \pre
 *    - que!=0
 *    - tag!=0
 *    - len!=0
 *    - val!=0
 * \return number of bytes retrieved; negative value if failure
 * \header sys\\sys.h
 * \source sys\\tbx.c
*/
int queGetTlv(tQueue * que, card * tag, word * len, byte * val) {
    int ret;

    
    ret = queGetTag(que, tag);
    
    ret = queGet(que, val);
    
    *len = (word) ret;

    return ret;
}

/** Switch on the bit number idx in buf. Bits are numbered from left to right starting from 1.
 * \param buf (I) Binary buffer containing a bitmap.
 * \param idx (I) Related bit number.
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
void bitOn(byte * buf, byte idx) {
    idx--;
    while(idx >= 8) {
        buf++;
        idx -= 8;
    }
    *buf |= (0x01 << (7 - idx));
}

/** Switch off the bit number idx in buf. Bits are numbered from left to right starting from 1.
 * \param       buf (I) Binary buffer containing a bitmap.
 * \param       idx (I) Related bit number.
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
void bitOff(byte * buf, byte idx) {
    idx--;
    while(idx >= 8) {
        buf++;
        idx -= 8;
    }
    *buf &= ~(0x01 << (7 - idx));
}

/** Tests the bit number idx in buf. Bits are numbered from left to right starting from 1.
 * \param buf (I) Binary buffer containing a bitmap.
 * \param       idx (I) Related bit number.
 * \return  
 *    - zero if the related bit is off.
 *    - non-zero otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
byte bitTest(const byte * buf, byte idx) {
    idx--;
    while(idx >= 8) {
        buf++;
        idx -= 8;
    }
    return ((*buf) >> (7 - idx)) & 0x01;
}

/** Set parity bits; par=1..8 is number of parity bit.
 * \param buf (I) Binary buffer to be set
 * \param dim (I) Buffer size
 * \param par (I) Position of the parity bit 1 or 8
 * \param typ (I) Even or Odd parity
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\tbx.c
 */
void tbxParity(byte * buf, word dim, byte par, byte typ) {  //set parity bits
    byte cnt, idx;

    while(dim--) {
        cnt = 0;
        for (idx = 1; idx <= 8; idx++) {
            if(idx == par)
                continue;
            if(bitTest(buf, idx))
                cnt++;
        }
        switch (typ) {
          case 'E':
              if(cnt % 2)
                  bitOff(buf, par);
              else
                  bitOn(buf, par);
              break;
          case 'O':
          default:
              if(cnt % 2)
                  bitOn(buf, par);
              else
                  bitOff(buf, par);
              break;
        }

        buf++;
    }
}

/** Check parity bits; par=1..8 is number of parity bit.
 * \param buf (I) Binary buffer to be set
 * \param dim (I) Buffer size
 * \param par (I) Position of the parity bit 1 or 8
 * \param typ (I) Even or Odd parity
 * \return 1 if parity is OK, 0 if parity NOK;
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\tbx.c
 */
byte tbxCheckParity(byte * buf, word dim, byte par, byte typ) { //check parity bits
    byte ret;
    byte cnt, idx, sta;

    ret = 1;
    while(dim--) {
        cnt = 0;
        for (idx = 1; idx <= 8; idx++) {
            if(idx == par)
                continue;
            if(bitTest(buf, idx))
                cnt++;
        }
        sta = bitTest(buf, par);
        switch (typ) {
          case 'O':
              if(!(cnt % 2))
                  ret = 0;
              break;
          case 'E':
          default:
              if(cnt % 2)
                  ret = 0;
              break;
        }
        buf++;
    }
    return ret;
}
