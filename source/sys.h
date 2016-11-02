#ifndef __SYS_H
#define __SYS_H

#define dspW 16
#define dspH 4
#define fntH 16

#define PRT_NORMAL_SIZE   24
#define __PRTW__ PRT_NORMAL_SIZE
#define prtW __PRTW__


typedef unsigned char byte;     ///< 0x00..0xFF
typedef unsigned short word;    ///< 0x0000..0xFFFF*
typedef unsigned long card;     ///< 0x00000000..0xFFFFFFFF


/** \weakgroup SYSBWC byte-word-card manipulation macros
 * @{
 */
///Combine two bytes into a word
#define WORDHL(H,L) ((word)((((word)H)<<8)|((word)L)))

///Extract highest byte from a word
#define HBYTE(W) (byte)(((word)W&0xFF00)>>8)

///Extract lowest byte from a word
#define LBYTE(W) (byte)((word)W&0x00FF)

///Combine two words into a card
#define CARDHL(H,L) ((card)((((card)H)<<16)|((card)L)))

///Extract highest word from a card
#define HWORD(C) (word)(((card)C&0xFFFF0000UL)>>16)

///Extract lowest word from a card
#define LWORD(C) (word)((card)C&0x0000FFFFUL)
/** @} */

#define CHECK(CND,LBL) {if(!(CND)){goto LBL;}}

const byte *bPtr(byte val);     ///< return the pointer to a byte containing the value val
const word *wPtr(word val);     ///< return the pointer to a word containing the value val
const card *cPtr(card val);     ///< return the pointer to a card containing the value val
const byte *dPtr(const char *s);    ///< return the pointer to a binary data


/** @} */

/** \weakgroup SYSCVTFUN Elementary conversion functions
 * The conversion functions below operate with the following data types:
 *  - num: a numeric value of type card, for example, 1234 or 0x1234
 *  - dec: a string containing only decimal characters, for example, "1234"
 *  - hex: a string containing only hexadecimal characters, for example, "12AB"
 *  - bin: a string containing any characters, for example, "\x12\x34"
 *
 * The return value is the number of bytes processed if OK.
 *
 * In case of error it is zero
 *
 * The destination is always the first parameter
 *
 * The second parameter is the source
 *
 * The last parameter is the length of a string (without ending zero) or
 * the length of a binary buffer.
 *
 * The zero value of len means that it should be calculated as strlen(source)
 * or number of digits in source etc according the situation
 *
 * The argument len is the length of source for the functions ...2num and bin2...
 * dec2num,hex2num,bin2hex,bin2num
 *
 * It is the length of destination for functions num2... and ...2bin
 *  num2dec,num2hex,hex2bin,num2bin
 * @{
*/

byte num2dec(char *dec, card num, byte len);    ///< numeric value num --> decimal string dec of length len
byte dec2num(card * num, const char *dec, byte len);    ///< decimal string dec of length len --> numeric value num
byte num2hex(char *hex, card num, byte len);    ///< numeric value num --> hexadecimal string hex of length len
byte hex2num(card * num, const char *hex, byte len);    ///< hexadecimal string hex of length len --> numeric value num
int bin2hex(char *hex, const byte * bin, int len);  ///< binary buffer bin of length len --> hexadecimal string hex
int hex2bin(byte * bin, const char *hex, int len);  ///< hexadecimal string hex --> binary buffer bin of length len
byte bin2num(card * num, const byte * bin, byte len);   ///< binary buffer bin of length len --> numeric value num using direct byte order
byte num2bin(byte * bin, card num, byte len);   ///< numeric value num --> binary buffer bin of length len using direct byte order

/** @} */

/** \weakgroup SYSBIT Bit operations
 * The leftmost bit of buf has number 1
 *
 * The rightmost bit of the first byte of buf has number 8
 *
 * The leftmost bit of the second byte of buf has number 9
 *
 * etc
 * @{
*/
void bitOn(byte * buf, byte idx);   ///< turn on bit idx in buf
void bitOff(byte * buf, byte idx);  ///< turn off bit idx in buf
byte bitTest(const byte * buf, byte idx);   ///< test the state of bit idx in buf
void tbxParity(byte * buf, word dim, byte par, byte typ);   ///<set parity bits
byte tbxCheckParity(byte * buf, word dim, byte par, byte typ);  ///<check parity bits

/** @} */

/** \weakgroup SYSBUF Safe buffer structure
 * This structure is used to fill a memory in a safe manner.
 * The memory buffer is reserved before calling the constructor bufInit() function.
 * When filling this memory space a checking is performed whether there is enough space available
 * The structure members should be never accessed directly (treated like private class members).
 * A set of functions are provided to perform the operations on a buffer
 *
 *The parameter convention in the functions below:
 * - buf: pointer to sBuffer structure to perform an operation
 * - dat: pointer to data to be copied inside the buffer
 * - len: length of the data to be copied; two values have special meaning:
 *   - len==-1 means: strlen(dat) bytes will be copied
 *   - len==0 means: same as -1 and one more byte set by zero
 *   - pos moved by strlen(dat) bytes in both cases
 * - num: number of bytes to be set (starting from the pos offset)
 *
 * The negative return value means an error (not enough space)
 * otherwise, the current buffer position is returned
 * \test tcab0009.c
 * @{
*/
///buffer descriptor
typedef struct sBuffer {
    byte *ptr;                  ///< buffer containing the data
    word dim;                   ///< number of bytes in the buffer
    word pos;                   ///< current position
} tBuffer;

void bufInit(tBuffer * buf, byte * ptr, word dim);  ///<Initialize a buffer
word bufDim(const tBuffer * buf);   ///<Get buffer dimension
const byte *bufPtr(const tBuffer * buf);    ///<Get buffer pointer
word bufLen(const tBuffer * buf);   ///<Get buffer length (position)
void bufReset(tBuffer * buf);   ///<Fill the buffer by zeroes and reset the current position
int bufSet(tBuffer * buf, byte val, word num);  ///<Set num bytes of buffer to the value val
int bufIns(tBuffer * buf, word ofs, const byte * dat, int len); ///<Insert data at a given offset
int bufOwr(tBuffer * buf, word ofs, const byte * dat, int len); ///<Overwrite data at a given offset
int bufCat(tBuffer * dst, tBuffer * src);   //Append the content of src buffer to dst buffer
int bufDel(tBuffer * buf, word ofs, int len);   ///<Delete data at a given offset
int bufApp(tBuffer * buf, const byte * dat, int len);   ///<Append data to the end of buffer
int bufCpy(tBuffer * buf, const byte * dat, int len);   ///<Reset buffer and copy new data into it
int bufGet(tBuffer * buf, byte * dat);

/** A shortcut for string appending:
 * the third argument is zero (calculated as strlen)
*/
#define bufAppStr(BUF,STR) bufApp(BUF,(byte *)STR,0) /** Shortcut to append a zero-ended string */
/** @} */

/** \weakgroup SYSQUE Queue structure
 * This structure provides a container of type queue.
 * It is widely used to interface with EMV Kernel when transmitting the parameter sets to the EMV Kernel function, creating data object lists etc.
 * The structure members should be never accessed directly.
 * A set of functions are provided to perform the operations on a queue.
 *
 * The negative return values mean error
 * If operation is performed OK, the length of the element is returned
 * The len==0 means that len is to be calculated as strlen(dat)
 * \test tcab0010.c
 * @{
*/
///queue descriptor
typedef struct sQueue {
    tBuffer *buf;               ///< buffer containing the queue elements
    word cur;                   ///< current read position
    word put;                   ///< number of elements put (written)
    word get;                   ///< number of elements to get (written and not read yet)
} tQueue;

void queInit(tQueue * que, tBuffer * buf);  ///<Initialize a queue
int quePut(tQueue * que, const byte * dat, int len);    ///<Put a data element into a queue
int queGet(tQueue * que, byte * dat);   ///<Retrieve a data element from the queue
word queNxt(const tQueue * que);    ///<Get the size of the next data element in the queue
word queLen(const tQueue * que);    ///<Get queue length (number of elements not rtetrieved yet)
void queReset(tQueue * que);    ///<Empty the queue
void queRewind(tQueue * que);   ///<Put the current position to the beginning of the queue
int quePutTag(tQueue * que, card tag);  ///<Put a tag into queue
int queGetTag(tQueue * que, card * tag);    ///<Retrieve a tag from the queue
int quePutTlv(tQueue * que, card tag, word len, byte * val);    ///<Put (Tag-Length-Value) triple into the queue
int queGetTlv(tQueue * que, card * tag, word * len, byte * val);    ///<Retrieve (Tag-Length-Value) from the queue


#define trcS(s) {}
#define trcFL(f,l) {}
#define trcFS(f,s) {}
#define trcFN(f,n) {}
#define trcBuf(b,n) {}
#define trcBN(b,n) {}
#define trcAN(b,n) {}
#define trcBAN(b,n) {}
#define trcQ(q) {}
#define trcErr(n) {}
#define TRCDT(s) {}
#define trcQueDol(q) {}
#define trcQueTlv(q) {}

#define PMDSET(STA)
#define PMDLOG(STR)

/** @} */
/** @} */

/** \addtogroup i32group I32 Ingenico
 * I32 component implements the interface with terminal perypherals,
 * and wraps the original Ingedev library functions.
 * @{
*/



/** Make a pause of sec seconds. The timer 0 is used for it.
 * \param sec (I) Number of seconds to wait
 * \return no
 * \header sys\\sys.h
 * \source sys\\i32.c
 */
void tmrPause(byte sec);        ///<make a pause of sec seconds



#define INV(L) (0x80|(L)) /**< display at line L in inverted mode */
#define BLD(L) (0x40|(L)) /**< display in bold font at line L */
#define RGT(L) (0x20|(L)) /**< display in right-justified string at line L */
#define BIG(L) (0x10|(L)) /**< display in big characters at line L */



typedef enum txnStep {
    IDLE,
    INITIALISED,
    INTERFACE_OPENED,
    TXN_READY,
    PRELIMINARY_TXN_COMPLETE,
    ONLINE_PROC,
    COMPLETE_TXN,
    WAIT_FOR_CARD_REMOVAL,
    CLSCARD_REMOVED,
    FINALISE_TXN,
    ERROR,
    FINISHED
} txnStep_t;


/** \weakgroup SYSCHN Communication processing
 * @{
 */
enum eChn {                     ///<UTA channels
    chnBeg,                     ///<start sentinel
    chnMdm,                     ///<internal modem
    chnCom1,                    ///<Com1
    chnCom2,                    ///<Com2
    chnCom3,                    ///<Com3
    chnTcp5100,                 ///<tcp connection I5100 with ethernet card
    chnTcp = chnTcp5100,        ///<tcp connection generic
    chnTcp7780,                 ///<tcp connection I7780 bluetooth
    chnPpp,                     ///<PPP
    chnGprs,                    ///<GPRS
    chnHdlc,                    ///<HDLC
    chnUSB,                     ///<USB Slave
    chnUSBMaster,               /// <USB Master
    chnSck,                     /// <Socket
    chnWifi,                    /// <Wifi
    chnSsl5100,                 ///<ssl connection I5100 with ethernet card    
    chnSsl = chnSsl5100,        ///<ssl connection generic
    chnSsl7780,                 ///<ssl connection I7780 bluetooth
    chnEnd                      ///<end sentinel
};

enum eChnRole {                 ///<channel roles
    chnRoleBeg,                 ///<start sentinel
    chnRoleSrv,                 //SERVER
    chnRoleClt,                 //CLIENT
    chnRoleEnd                  ///<end sentinel
};

enum eCom {                     ///<Communication errors
    comBeg,                     ///<start sentinel
    comTimeout,                 ///<timeout
    comAnn,                     ///<user cancel
    comNoCarrier,               ///<NO CARRIER
    comNoDialTone,              ///<NO DIAL TONE
    comBusy,                    ///<BUSY
    comKO                       ///<unclassified error
};


/** \addtogroup gclgroup GCL communication processing
 * GCL component contains the wrappers around the functions implemented in Generic Communication Library.
 * It is an alternative way to CHN module.
 * @{
*/
/** \weakgroup SYSGCL GCL (Generic Communication Library) wrappers.
 *
 * The GCL wrappers will replace the old UTA communication functions.
 *
 * It is implemented for 32 bit only
 * @{
 */
enum eGcl {                     ///<gcl communication modes
    gclBeg,                     ///<start sentinel
    gclHdlc,                    ///< HDLC modem connection
    gclAsyn,                    ///< Asynchronous modem connection
    gclISDN,                    ///< ISDN modem
    gclSer,                     ///< Serial communication
    gclWnb,                     ///< WNB serial connection
    gclPpp,                     ///< PPP over a modem connection
    gclPppCom,                  ///< PPP over a serial port
    gclEth,                     ///< Ethernet connection
    gclGsm,                     ///< GSM data call
    gclPppEth,                  ///< PPP over Ethernet connection
    gclGprs,                    ///< GPRS connection
    gclPppGsm,                  ///< PPP over a GSM connection
    gclSck,                     ///< Socket interface
    gclWifi,                    ///< WIFI interface
    gclEnd                      ///<end sentinel
};

enum eGclSta {                  ///<gcl states
    gclStaBeg,                  ///<start sentinel
    gclStaPreDial,              ///<predial
    gclStaDial,                 ///<dial
    gclStaConnect,              ///<connect
    gclStaLogin,                ///<login
    gclStaFinished,             ///<finished
    gclStaEnd                   ///<end sentinel
};

enum eGclErr {                  ///<gcl errors
    gclErrBeg,                  ///<start sentinel
    gclErrCancel,               ///<user cancel
    gclErrDial,                 ///<dial error
    gclErrBusy,                 ///<BUSY
    gclErrNoDialTone,           ///<NO DIAL TONE
    gclErrNoCarrier,            ///<NO CARRIER
    gclErrNoAnswer,             ///<NO ANSWER
    gclErrLogin,                ///<login error
    gclErrCableRemoved,         ////<Cable Removed Error
    gclErrEnd                   ///<end sentinel
};


/** \addtogroup pgngroup PGN GMA plugin interface functions
 * PGN component contains the wrappers around the GMA plugin interface functions.
 * @{
*/
enum ePgnConn {
    pgnConnNull,
    pgnConnGsm,
    pgnConnGprs,
    pgnConnEnd
};

int prtStart(void);             ///<Start printer perypheral
int prtStop(void);              ///<Stop printer perypheral
int prtCtrl(void);              ///<Control paper presence
int prtS(const char *str);      ///<print a string
int prtES(const char *esc, const char *str);    ///<print a string preceeded by an escape characters sequence
int prtImg(const byte * img, int hgt, int wdt, const char *ctl);    ///<Print an image


enum eNvm {                     ///<non-volatile memory error processing codes
    nvmBeg,                     ///< start sentinel
    nvmOpenErr,                 ///< error opening file
    nvmSeekErr,                 ///< offset not found
    nvmKO                       ///< unknown error (read/write)
};

void nvmFileName(char *buf, byte sec);  ///<build DFS file name containing section data
void nvmPathFileName(char *buf, byte sec);  ///<build full path file name containing section data
void nvmStart(void);            ///<Initialize internal non-volatile memory descriptors
int nvmHold(byte sec);          ///<Hold section sec opened until nvmRelease() called.
void nvmRelease(byte sec);      ///<Close section sec opened by preceding nvmHold() call.
int nvmRemove(byte sec);        ///<Remove DFS file containing the section sec
int nvmSize(byte sec);          ///<Return the size of a section
int nvmMove(byte srcSec, byte dstSec);  ///<Move a section to an another section (owerwrite)
int nvmSave(byte sec, const void *buf, card ofs, word len); ///<Save data to nvm section
int nvmSet(byte sec, byte val, card ofs, card len); ///<fill data in nvm section by a given value
int nvmLoad(byte sec, void *buf, card ofs, word len);   ///<get data from nvm section
int nvmSeqOpen(const char *file, char atr);    ///<open a file for sequential access (reading or writing)
void nvmSeqClose(card hdl);     ///<close sequential access file
int nvmSeqPos(card hdl, card pos);  ///<set file position
int nvmSeqGet(card hdl, void *buf, word len);   ///<sequential read
int nvmSeqPut(card hdl, const void *buf, word len); ///<sequential write

#endif
