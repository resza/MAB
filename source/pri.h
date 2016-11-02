/** \file
 * Primitive group components
 *
 * Subversion reference
 *
 * $HeadURL: https://svn.ingenico.com/UTA/core/utaYYMMDD/pri/pri.h $
 *
 * $Id: pri.h 2616 2010-04-07 11:16:41Z abarantsev $
 */

/** \defgroup prigroup PRI Primitive components
 *
 * The system component functions are declared in the header file pri.h in pri directory.
 * They are implemented in the files:
 *  - FMT.C: (ForMaT) formatting functions.
 *  - DLG.C: (DiaLoG) user dialogs: data input, menu, display screens
 *  - STD.C: (STandarD) various standard utilities like CRC calcualtion
 *  - COM.C: (COMmunication) high level communication functions, deprecated along with CHN.C
 *  - A40.C: (APACS 40) a set of functions implementing APACS40 key management
 *  - DBA.C: (Data Base Access) table and record structures and their methods
 *  - DRV.C: (DeRiVated) container, converter and combo implementation
 *  - DRW.C: (DRaWing) shape drawing functions and structures
 *  - GUI.C: (Graphical User Interface) dialogs, menus, input forms
 *  - STM.C: (STreaM) I/O stream processing
 * @{
 */
#ifndef __PRI_H
#include "sys.h"


#define __MNUMAX__  18
#define __MNUMAX2__  20


/** \addtogroup fmtgroup FMT ForMaT
 * FMT component contains string formatting functions frequently used in banking
 * payment application context.
 * @{
 */
int fmtSbs(char *dst, const char *src, byte ofs, byte len); ///<copy into dst substring of src starting from ofs of length
int fmtTok(char *dst, const char *src, const char *sep);    ///<Extract a token dst from the string src that can contain several tokens separated by characters from sep
int fmtPad(char *str, int len, char chr);   ///<pad the string str to length abs(len) by the character chr
int fmtCnt(char *str, int len, char chr);   ///<center the string str to length abs(len) by the character chr
int fmtAmt(char *dst, const char *src, byte exp, const char *sep);  ///<format an amount string with decimals
int fmtMut(char *dst, const char *fgd, const char *bgd, const char *ctl);   ///<Permutation of fgd and bgd according to the control string ctl
int fmtIns(char *dst, const char *bgd, const char *ins, byte ofs, int len); ///<insert a string to an another sting
int fmtDel(char *dst, const char *src, byte ofs, int len);  ///<erase the len long substring from ofs
int fmtTcp(char *dst, char *src, int len);  ///<format TCP/IP address
int fmtDate(char *dst, char *src, int len); //firas
int fmtTime(char *dst, char *src, int len); //firas

/** @} */

/** \addtogroup stdgroup STD STanDard
 * STD component contains the functions implementing some standard algorythms of general usage.
 * @{
*/
/** \weakgroup PRIGEN Various generic purpose functions
 * @{
 */
byte isSorted(word a, word b, word c);  ///<returns 1 if a<b<c and 0 otherwise
byte stdLrc(const byte * buf, int len); ///<calculate checksum lrc for the buffer buf of length len
byte stdLuhnCheck(const char *pan); ///<verify PAN according to the Luhn check digit, algorithm ANSI  X4.13
word stdCrc16(const byte * buf, word len, byte augment_message_for_crc);    ///<Crc16(Xmodem) calculation according to CCITT-1021
void stdArgMsgCrc16(word * crc_val);
card stdPow(int num, int exp);
byte stdGetAcc(byte * acc, const char *pan);    ///<extract account number from pan for ANSIX 9.8 pin calculation
void xor(const byte * b1, const byte * b2, byte * dst, word len);   ///<XOR len byte of b1 and b2 to obtain dst
void stdParity(byte * buf, word dim, byte par); ///<set parity bits; par=1..8 is number of parity bit
card hashJerkinsBin(const byte * buf, byte len);    ///<Integer hash function for binary buffer
word bmpWdt(const byte * bmp);
word bmpHgt(const byte * bmp);
const byte *bmpDat(const byte * bmp);
byte trk2Lrc(const byte * buf, int len);    ///<calculate checksum lrc for the track2 buf of length len ,keep bit 7,6and 5 always 011 in output lrc
int Radix64Encode(byte * inBuf, word inLen, byte * outBuf); ///<encodes radix (base 64)
int Radix64Decode(byte * inBuf, byte * outBuf, word * outLength);   ///<decodes radix (base 64)
int BaseToDec(char *number, card base);
card dt2ts(const char *YYMMDDhhmmss);   //datetime to timestamp converter
void ts2dt(card ts, char *YYMMDDhhmmss);    //timestamp to datetime converter

/** @} */

/** \weakgroup PRIDES DES encryption/decryption functions
 * @{
 */
void stdDES(byte * dst, byte * src, byte * key);    ///<Encrypt the binary array src using the key and put the result into the buffer dst
void stdSED(byte * dst, byte * src, byte * key);    ///<Decrypt the binary array src using the key key and put the result into the buffer dst
void stdEDE(byte * dst, byte * src, byte * key);    ///<Triple DES encryption of  binary array src by double length key
void stdDED(byte * dst, byte * src, byte * key);    ///<Triple DES decryption of  binary array src by double length key
int stdOWF(byte * dst, const byte * dat, const byte * key); ///<One Way Function of binary array dat by simple length key

/** @} */

/** \weakgroup PRIARI Arithmetic operations with strings
 * @{
 */
byte addStr(char *dst, const char *arg1, const char *arg2); ///<dst:= arg1+arg2 in decimal; the maximum length is 16
byte subStr(char *dst, const char *arg1, const char *arg2); ///<dst:= arg1-arg2 in decimal; the maximum length is 16

/** @} */

/** @} */

/** \addtogroup a40group A40 APACS 40
 * @{
 */

int a40ABCD(char *abcd, const char *src);
int a40GenA40Keys(const byte * ABCD, byte locPK, byte locMK, byte locRK);
int a40CheckMac(byte TransactionType,
                byte AuthParameterType,
                byte * ConstructedRspPacket,
                byte * AuthParam,
                byte * TxnAmount,
                byte * ReqMacResidue,
                byte * ResMacResidue,
                byte * ResMac, byte MacKeyLoc, byte PinKeyLoc);
int a40GenerateAuthParameter(const byte TransactionType,
                             const byte AuthParameterType,
                             const byte * TransactionData,
                             byte locKey, byte * AuthParameter);
int a40UpdateKeyRegister(const byte * RequestMacResidue,
                         const byte * ResponseMacResidue, byte locKeyReg);
/** @} */

/** \addtogroup comgroup COM communication functions
 *
 * @{
 */

/** Control characters
 */
enum eComCtl {
    cNUL = 0x00,
    cSOH = 0x01,
    cSTX = 0x02,
    cETX = 0x03,
    cEOT = 0x04,
    cENQ = 0x05,
    cACK = 0x06,
    cBEL = 0x07,
    cHT = 0x09,
    cLF = 0x0A,
    cCR = 0x0D,
    cDLE = 0x10,
    cDC1 = 0x11,
    cDC2 = 0x12,
    cNAK = 0x15,
    cSYN = 0x16,
    cETB = 0x17,
    cFS = 0x1C,
    cGS = 0x1D,
    cRS = 0x1E,
    cUS = 0x1F,
    cDS = 0x40,
    cPR = 0x2A,
    cH1 = 0x32,
    cH2 = 0x41
};

int comConnect(char com, const char *chn, const char *srv); ///<Initializes a communication line com and/or connects to server
int comDisconnect(char com, const char *chn, const char *srv);  ///<disconnect from the server
int comSendReq(const byte * req, word len); ///<send request req of length len to server
int comRecvRsp(tBuffer * rsp, const byte * trm, byte dly);  ///<receive response terminated by a character from trm in timeout dly
int comRecvStxDataEtxLrc(tBuffer * rsp, byte dly);  ///<receive a packet formatted as STX-DATA-ETX-LRC into buffer rsp using the timeout dly
int comRecvDataEtx(tBuffer * rsp, byte dly);    ///<receive response in timeout dly formatted as DATA-ETX
int comRecvPat(tBuffer * rsp, const tBuffer * pat, byte dly);   ///<receive pattern pat in timeout dly

/** @} */

int clsGet(const char *para, byte * buf);   // get external VIVOTech 4500 track data.
int chkStart(const char *para); // Open com external eN-Check 2500
int chkGet(byte * rawData);     // get data external eN-Check 2500
int chkStop(void);              // Closed exernal eN-Check 2500

/** \addtogroup dlggroup DLG dialog functions
 * @{
 */

void stub(const char *s);       ///<display a string for 3 seconds
void dlgClick(byte clk);        ///<enable/disable clicking while key pressing in dialogs

typedef const char *Pchar;

#define MNUMAX __MNUMAX__
#define MNUMAX2 __MNUMAX2__
int mnuFunction(const Pchar * mnu, int sta, int dly);
int mnuSelect(const Pchar * mnu, int sta, int dly); ///< propose a menu of strings to the user
int enterStr(byte loc, char *str, byte len);    ///<enter a digital string using scrolling
int enterStr2(byte loc, char *str, byte len);   ///<process user input
int enterPhr(byte loc, char *str, byte len);    ///<enter digital string using wrapping
int enterPwd(byte loc, char *str, byte len);    ///<process password input
int enterAmt(byte loc, char *amt, byte exp);    ///<process amount input
int enterTcp(byte loc, char *buf, byte len);    ///<process IP address input
int enterData(byte loc, char *amt, byte len);
int enterTxt(byte loc, char *str, byte len, byte fmt);  ///<Process alphabetical input like authorisation code or password input
int enterTxt2(byte loc, char *str, byte len, byte fmt); ///<Process alphabetical input in alternative way
int enterTxt3(byte loc, char *str, byte len, byte fmt); ///<Process alphabetical input in alternative way with a cursor
int enterTxt4(byte loc, char *str, byte len, byte fmt); ///<Process alphabetical input in alternative way with a cursor
int enterTxt5(byte loc, char *str, byte len, byte fmt); ///<Process alphabetical input in alternative way with a cursor
int enterFmtStrFarsi(byte loc, char *str, byte fmt, byte len);
int enterCard(byte * buf, const char *ctl); ///<process card input
void removeCard(void);          ///<wait for chip card removal
void DetectCard(void);
int enterDate(byte loc, char *buf, byte len);
int enterTime(byte loc, char *buf, byte len);
int enterPwd2(byte loc, char *str, byte len);

/** The structure is used to present information to a user.
 * It contains:
 * - cmd: command to be executes during information presentation
 * - four strings to be displayed
 * - timeout for displaying
 *
 * The function info below displays the information accrotding to the structure and waits fotr user input
 * during the timeout.
 * Ifa a key is pressed during waiting the function returns the value of key pressed immediately.
 * Otherwise it waits for the given timeout and returns 0.
 */
///user screen information structure
typedef struct sInfo {
    char cmd;                   ///<command: i- ignore, s- show, b- beep
    char msg[dspH][dspW + 1];   ///<messages to display on each line
    byte dly;                   ///<timeout in seconds
} tInfo;

int info(tInfo * inf);          ///<Display a user screen and wait for user reaction according to the command

/** @} */

/** \addtogroup dbagroup DBA data base access functions
 *
 * dba operates by sections.
 *
 * Section 0 corresponds to data page 0 (file page0000.nvm).
 *
 * Section 1 corresponds to data page 1 (file page0001.nvm).
 *
 * etc
 *
 * The special section number 0xFF is used for volatile memory section
 *
 * For U32 the section number 0xFE is used for internal secured memory section
 * @{
 */

/** \weakgroup PRILDB Low level data base processing
 * @{
 */
void dbaSetPtr(void *ptr);

int dbaExport(byte dst, byte src, card ofs, word len, char fmt, const char *cmt);   ///<export a data element
int dbaImport(byte sec);        ///<import data elements

/** The structure is used to manage cache memory by tables and records.
 * It is declared in this file just to make it possible to use it in other structure declarations.
 * All operations are performed internally in dba component.
 *
 * Cache memory definition from wikipedia: http://en.wikipedia.org/wiki/Caching
 *
 * In computer science, a cache (pronounced like the English word cash) is a collection of data
 * duplicating original values stored elsewhere or computed earlier,
 * where the original data is expensive (usually in terms of access time)
 * to fetch or compute relative to reading the cache.
 *
 * Once the data is stored in the cache, future use can be made by accessing the cached copy
 * rather than refetching or recomputing the original data,
 * so that the average access time is lower.
*/
///memory cache descriptor
typedef struct sCache {
    byte *ptr;                  ///< memory buffer reserved for cache
    word dim;                   ///< size of cache memory
    byte sec;                   ///< section associated with cache memory
    card ofs;                   ///< starting offset in non-volatile section
    word mod;                   ///< relative offset of modified part
    word len;                   ///< length of modified part
} tCache;

/** @} */

/** \weakgroup PRITAB Table processing
 * A table contains a fixed number (hgt, height) of rows of a given length (wdt, width).
 * The rows are stored sequentially in the section sec starting from the offset ofs.
 * @{
 */

/** A table descriptor contains the information on where the table rows are stored.
 * Using this descriptor it is possible to treat any row of the table.
 *
 * A cache buffer cch can be associated with a table to accelerate access to its rows.
 * The size of the cache buffer should be big enough to contain the whole table.
 *
 * Using a cache is convenient for for read-only tables of small or medium size.
 * The table is loaded into cache once at the beginnning,
 * after that any row can be accessed from RAM cache buffer.
 *
 * Cache buffer can be activated also when a table is filled by some default values.
 * In this case all rows are stored in the cache buffer, then the cache buffer is written into
 * non-volatile memory using only one write operation.
 *
 * The operations with tables are implemented using Object Oriented approach.
 * For any allowed operation there exists a method to perform it.
 * It is forbidden to access structure elements directly.
*/
///table descriptor
typedef struct sTable {
    byte sec;                   ///<section where the table is stored
    card ofs;                   ///<starting offset
    word wdt;                   ///<(width) record length
    word hgt;                   ///<number of records in the table
    char fmt;                   ///<row format
    tCache cch;                 ///<associated cache memory descriptor
} tTable;

int tabInit(tTable * tab, byte sec, card ofs, word wdt, word hgt, char fmt);    ///<table constructor
int tabCache(tTable * tab, byte * cch, word dim);   ///<enable/disable cache for the table
int tabFlush(tTable * tab);     ///<flush associated cache to non-volatile memory
int tabLoad(tTable * tab);      ///<load the table from non-volatile memory into cache
int tabSave(tTable * tab);      ///<save unconditionnally the whole table from cache to non-volatile memory
word tabDim(const tTable * tab);    ///<get table dimension (number of rows)
word tabWdt(const tTable * tab);    ///<get table width (number of bytes in a row)
card tabSize(const tTable * tab);   ///<get table size (number of byte occupied)
char tabRowFmt(const tTable * tab); ///<get row format
int tabReset(tTable * tab);     ///<fill by zeroes
int tabGet(const tTable * tab, word idx, void *buf, word len);  ///<get a table row
int tabPut(tTable * tab, word idx, const void *buf, word len);  ///<put a table row
int tabExport(tTable * tab, word idx, byte sec, char fmt, const char *cmt); ///<export row idx into sec

/** @} */

/** \weakgroup PRISHT Shortcut defines for table processing
 * These defines allow to reduce the number of parameters for tables containing ASCIIZ string rows.
 *@{
 */
#define tabGetStr(TAB,IDX,DST) tabGet(TAB,IDX,DST,(word)(tabWdt(TAB)+1))
#define tabPutStr(TAB,IDX,SRC) tabPut(TAB,IDX,SRC,0)
/** @} */

/** \weakgroup PRIREC Record processing
 * A record is a set of fields of different lengths and various formats.
 * Each field can be accessed using its number.
 * @{
 */

/** A record descriptor contains the information on where the record fields are stored.
 * Using this descriptor it is possible to treat any fields of the record.
 *
 * A cache buffer cch can be associated with a record to accelerate access to its fields.
 * The size of the cache buffer should be big enough to contain the whole record.
 *
 * The operations with records are implemented using Object Oriented approach.
 * For any allowed operation there exists a method to perform it.
 * It is forbidden to access structure elements directly.
 */
///record descriptor
typedef struct sRecord {
    byte sec;                   ///<section where the record is stored
    card ofs;                   ///<starting offset within the section
    word dim;                   ///<number of fields in the record (dimension)
    const word *len;            ///<array of field lengths
    const char *fmt;            ///<array of field formats
    const word *map;            ///<array of field offsets (map[0] is the offset of field 1, map[dim-1] is the  record size
    tCache cch;                 ///<associated cache memory
} tRecord;

int recInit(tRecord * rec, byte sec, card ofs, word dim, const word * len, const char *fmt, word * map);    ///<record constructor
int recCache(tRecord * rec, byte * cch, word dim);  ///<enable/disable cache for the record
int recFlush(tRecord * rec);    ///<flush associated cache to non-volatile memory
int recLoad(tRecord * rec);     ///<load the record from non-volatile memory into cache
int recSave(tRecord * rec);     ///<save unconditionnally the whole record from cache to non-volatile memory
word recDim(const tRecord * rec);   ///<get record dimension (number of fields)
card recSize(const tRecord * rec);  ///<get record size (number of byte occupied)
char recFldFmt(const tRecord * rec, word fld);  ///<get field format
word recFldLen(const tRecord * rec, word fld);  ///<get field length
int recReset(tRecord * rec);    ///<fill by zeroes
int recGet(const tRecord * rec, word fld, void *buf, word len); ///<get a record field
int recPut(tRecord * rec, word fld, const void *buf, word len); ///<put a record field
int recExport(tRecord * rec, word fld, byte sec, char fmt, const char *cmt);    ///<export field fld into section sec using format fmt

/** @} */

/** \weakgroup PRISHR Shortcut defines for record processing
 * These defines allow to reduce the number of parameters for various field types in records.
 *@{
 */
#define recGetByte(REC,FLD,DST) recGet(REC,FLD,&DST,sizeof(byte))
#define recPutByte(REC,FLD,SRC) recPut(REC,FLD,bPtr(SRC),sizeof(byte))
#define recGetWord(REC,FLD,DST) recGet(REC,FLD,&DST,sizeof(word))
#define recPutWord(REC,FLD,SRC) recPut(REC,FLD,wPtr(SRC),sizeof(word))
#define recGetCard(REC,FLD,DST) recGet(REC,FLD,&DST,sizeof(card))
#define recPutCard(REC,FLD,SRC) recPut(REC,FLD,cPtr(SRC),sizeof(card))
#define recGetStr(REC,FLD,DST) recGet(REC,FLD,DST,(word)(REC->len[FLD]+1))
#define recPutStr(REC,FLD,SRC) recPut(REC,FLD,SRC,0)
/** @} */

/** \weakgroup PRIMUL Multirecord processing
 * The UTA table contains rows without internal structure.
 * The elementary processing unit is a row.
 *
 * The UTA record is a set of fields of various lengths.
 * The elementary processing unit is a field.
 *
 * Multirecord is a table where each row is a record.
 * It is a real data base table.
 *
 * In UTA it is implemented as a table.
 * Only one row of a table can be accessed at a time.
 * To work with an internal structure of the row an UTA record is overlayed on the table row.
 * It means that the initial offset of the record is set to the beginning of the related row.
 * The function recMove should be called to move the cursor in the multirecord.
 *
 * Five levels of search operations are considered in UTA.
 *  - Linear search.
 *    The records are retrieved in the same order as they are saved in the data base.
 *    The search can be performed in forward or backward direction.
 *    The average time of search is N/2.
 *    It is the slowest method. It can be used when the number of records is small or
 *    the speed of operation is not critical.
 *  - Binary search.
 *    If the multirecord sorted according to the search field(s) the binary search
 *    can be applied.
 *    The average time of search is log N (say 8 readings for 256 records).
 *    Whenever possible this method should be used.
 *  - Index search.
 *    An index array is created that contains the record numbers sorted according
 *    to a given criteria.
 *    The array is stored in RAM, so all the operations are fast in index.
 *    Using this simple index the binary search can be applied.
 *    However the index integrity should be maintained each time when a record
 *    is added/deleted/modified.
 *    When a record is added to the multirecord the full index is rewritten.
 *    The method requires more RAM (two bytes per record) and uses additional DFS space.
 *    The index integrity becomes a real problem in case of power failure.
 *    The method can be applied for medium-sized multirecords (upto 512) that
 *    are not modified too often, for example, transaction log.
 *  - Key search.
 *    Not only the record number is stored in the RAM but the values of key fields also.
 *    It allows to perform binary search in RAM.
 *    Only one reading is required at the end of search.
 *    The payment terminal should not perform the functionality of data base server.
 *    The size of index+key is bigger that in the previous case.
 *    More RAM and DFS is required for it.
 *    So the price of modification is higher.
 *    The method can be used for tables that are rarely modified, like BIN table.
 *  - B-Tree search.
 *    For real data base servers (ORACLE, INFORMIX, DB2 etc) the index is not stored in
 *    RAM. For huge tables (millions of records) only part of index is loaded in RAM at any
 *    given time.
 *    This mechanism is implemented in form of B-trees to always keep the indices as a balanced
 *    tree.
 *    This mechanism is out of scope of UTA.
 *@{
 */
int recMove(tRecord * rec, const tTable * tab, word idx);   ///<move cursor in a table with structured rows (multirecord)
int recPos(const tRecord * rec, const tTable * tab);    ///<get current position within the multirecord

///search configuration descripto
typedef struct sSearch {
    const tTable *tab;          ///<table containing multirecord
    tRecord *rec;               ///<multirecord cursor record
    const struct sContainer *mtc;   ///<match container
} tSearch;

int searchInit(tSearch * srh, const tTable * tab, tRecord * rec, const struct sContainer *mtc); ///<search constructor
int linFind(tSearch * srh, const char *pat, word beg, word end);    ///<linear search for a pattern in a given range of row numbers
int binFind(tSearch * srh, const char *pat, word beg, word end);    ///<binary search for a pattern in a given range of row numbers

///index descriptor
typedef struct sIndex {
    byte sec;                   ///<section where the index is saved
    card ofs;                   ///<index offset within the section
    const tTable *tab;          ///<table containing multirecord
    tRecord *rec;               ///<multirecord cursor record
    const struct sContainer *fld;   ///<index key container (field(s) to be indexed)
    word *idx;                  ///<row numbers sorted according to the index rule
    byte len;                   ///<index key length
    char *key;                  ///<array of keys corresponding to idx
    word dim;                   ///<number of indexed rows
    word cur;                   ///<current index position
} tIndex;
int idxInit(tIndex * idx, byte sec, card ofs, const tTable * tab, tRecord * rec, const struct sContainer *fld, word * ptr, byte len, char *key);    ///<search constructor
word idxDim(const tIndex * idx);    ///<number of indexed items
card idxSize(const tIndex * idx);   ///<the size of non-volatile memory required to save index
card idxReset(tIndex * idx);    ///<clear the index in RAM and DFS
int idxFind(tIndex * idx, const char *pat, word beg, word end); ///<indexed search for a pattern in a given range of row numbers
int idxSort(tIndex * idx);      ///<quick sort
int idxIns(tIndex * idx);       ///<insert current record into index
int idxDel(tIndex * idx);       ///<delete current record from index
int idxApp(tIndex * idx);       ///<append current record into index without insertion (sort it aftyerwards)
int idxLoad(tIndex * idx);      ///<load index from non-volatile memory into RAM
int idxSave(tIndex * idx);      ///<save index from RAM into non-volatile memory
int idxSet(tIndex * idx, word pos); ///<set iterator position

typedef struct sHash {
    byte sec;                   ///<section where the index is saved
    card ofs;                   ///<index offset within the section
    byte len;                   ///<index key length
    word dimHsh;                ///<number of hash blocks
    word dimBlk;                ///<number of elements in a hash block
    word dimOvf;                ///<number of elements in a overflow block
    word limOvf;                ///<number of cells index in overflow block
    word vacOvf;                ///<first vacant cell index in overflow block
    word scanBlk, scanIdx;      ///<used for scanning
    card scanHdl;               ///<file handle for scanning
} tHash;

int hshInit(tHash * hsh, byte sec, card ofs, byte len, byte dimHsh, word dimBlk, word dimOvf);  ///>hash constructor
int hshReset(tHash * hsh);      ///<reset the hash table
int hshClean(tHash * hsh);      ///<clean the hash table
int hshFind(tHash * hsh, const byte * key); ///<search in hash table
int hshIns(tHash * hsh, const byte * key, word idx);    ///<insert new item to hash table
int hshDel(tHash * hsh, const byte * key);  ///<remove an item from hash table
int hshScanBeg(tHash * hsh);    ///<start hash scanning
int hshScanEnd(tHash * hsh);    ///<end hash scanning
int hshScan(tHash * hsh, void *key, word * idx);    ///<scan one cell

/** @} */

/** @} */

/** \addtogroup drvgroup DRV derivated data elements
 * The derivated data elements are not stored in the database.
 * They are calculated from other data elements.
 * @{
 */

/** \weakgroup PRICNT Container abstraction
 * Container represents an abstract data structure that can be one of the following:
 *  - table
 *  - record
 *  - converter
 *  - combo
 *  - string
 *
 * In terms of C++ it is a parent class for all the structures above.
 * @{
 */
///container descriptor
typedef struct sContainer {
    char typ;                   ///<'t': tTable, 'r': tRecord, 'c': tConvert, 'x': tCombo, 's': string
    union uContainer {          ///<union containing a pointer to the structure specified in typ field
        tTable *tab;            ///<it is used when container is built around a table
        tRecord *rec;           ///<it is used when container is built around a record
        struct sConvert *cvt;   ///<it is used when container is built around a converter
        struct sCombo *cmb;     ///<it is used when container is built around a combo
        char *str;              ///<it is used when container is built around a string
    } ptr;
} tContainer;

int cntInit(tContainer * cnt, char typ, void *ptr); ///<constructor
word cntLen(const tContainer * cnt, word key);  ///<getter, returns the length of the related data element
int cntGet(const tContainer * cnt, word key, void *buf, word len);  ///<retrieve data associated to the key into memory of size len pointed to by buf
int cntPut(const tContainer * cnt, word key, const void *buf, word len);    ///<save data of length len from the buffer buf into space reserved for the key

#define cntGetByte(CNT,FLD,DST) cntGet(CNT,FLD,&DST,sizeof(byte))
#define cntPutByte(CNT,FLD,SRC) cntPut(CNT,FLD,bPtr(SRC),sizeof(byte))
#define cntGetWord(CNT,FLD,DST) cntGet(CNT,FLD,&DST,sizeof(word))
#define cntPutWord(CNT,FLD,SRC) cntPut(CNT,FLD,wPtr(SRC),sizeof(word))
#define cntGetCard(CNT,FLD,DST) cntGet(CNT,FLD,&DST,sizeof(card))
#define cntPutCard(CNT,FLD,SRC) cntPut(CNT,FLD,cPtr(SRC),sizeof(card))
#define cntGetStr(CNT,FLD,DST) cntGet(CNT,FLD,DST,(word)(cntLen(CNT,FLD)+1))
#define cntPutStr(CNT,FLD,SRC) cntPut(CNT,FLD,SRC,0)

/** non-defined access key
 * It is used for converter and container Get and Put functions.
 *
 * If this value is indicated when accessing a data element,
 * the value of key used in Init function will be used.
 * Otherwise the value of key provided as the argument of Get or Put is used.
 */
#define NULLKEY 0xFFFF

/** @} */

/** \weakgroup PRICVT Converter processing
 * Converter is a calculated data element.
 * It performs data format conversion.
 *
 * When cvtGet function is called it retrieves the related value, converts it to another format and returns it
 * instead of original data format.
 *
 * When cvtPut function is called it converts the input value into another format and saves its converted value
 * instead of original input.
 *
 * A converter can be built on any type of container: table, record, combo or even another converter.
 * @{
 */

/** Converter data formats
 * A converter can perform conversions between the data formats in the enum eFmt.
 *
 * Not all the conversion types are implemented.
 * The allowed conversions are:
 * \verbatim
 +---------------------+
 |     Num Dec Hex Bin |
 | Num  X   Y   Y   Y  |
 | Dec  Y   X   X   Y  |
 | Hex  Y   X   X   Y  |
 | Bin  Y   Y   Y   X  |
 +---------------------+
 \endverbatim
 * where: X - no sense; Y - Yes, implemented
 */
enum eFmt {
    fmtBeg,                     ///<start sentinel
    fmtNum,                     ///<numeric format, for example 0x1234
    fmtDec,                     ///<decimal string, for example "1234"
    fmtHex,                     ///<hexadecimale string, for example "12AB"
    fmtBin,                     ///<any data, for example {0x12,0x34}
    fmtEnd                      ///<end sentinel
};                              ///<data formats

/** Converter descriptor.
 *
 * The following elements are used to describe a converter:
 *  - pointer to encapsulated container
 *  - the key to a data element in the container (table row or record field)
 *  - data format of the data element in the container (internal format)
 *  - external data format: output for cvtGet or input for cvtPut
 *  - length of the data element in the external format
 */
///converter descriptor
typedef struct sConvert {
    const tContainer *cnt;      ///< related container
    word fld;                   ///< field to be converted
    byte dbf;                   ///< database format, how it is stored in the database
    byte fmt;                   ///< converted format, how it is visible from outside
    word len;                   ///< length of external field format
} tConvert;

int cvtInit(tConvert * cvt, const tContainer * cnt, word fld, byte dbf, byte fmt, word len);    ///<converter constructor
int cvtLen(const tConvert * cvt);   ///<return a length of the data element in external format
int cvtGet(const tConvert * cvt, word key, void *buf, word len);    ///<retrieve the field into buffer buf of length len in the external format
int cvtPut(const tConvert * cvt, word key, const void *buf, word len);  ///<save the data from buffer buf of length len into related field of the converter after perofrming the conversion

/** @} */

/** \weakgroup PRICMB Combo processing
 * Combo is a calculated data element.
 * An operation is performed with several data elements to calculate the value of combo.
 *
 * It is a read-only derivated container.
 * The function cmbPut is not implemented for it.
 *
 * When cmbGet function is called it retrieves the values of operation arguments,
 * executes the operation and returns the result of the operation.
 *
 * A combo can be built on any type of container: table, record, converter or even another combo.
 * @{
 */

/** Locator structure is used to describe arguments of combo fields.
 * It allows to retrieve a value from any container data element whitout knowing its type.
 */
///locator descriptor
typedef struct sLocator {
    const tContainer *cnt;      ///<related container
    word key;                   ///<related key
} tLocator;

/** Combo operations can have up to 3 arguments.
 * The result of each operation is always an ASCIIZ string.
 */
enum eOpr {
    oprBeg,                     ///<start sentinel
    oprCat,                     ///< concatenation
    oprSbs,                     ///< substring
    oprTok,                     ///< token
    oprPad,                     ///< padding
    oprMut,                     ///< permutation
    oprAmt,                     ///< amount formatting
    oprSpr,                     ///< sprinf format
    oprAdd,                     ///< addition
    oprSub,                     ///< subtraction
    oprIf,                      ///< if else structure
    oprEq,                      ///< equal combo
    oprOr,                      ///< or (logical) combo   
    oprEnd                      ///<end sentinel
};                              ///<Combo operations

/** The structure sCombo (type tCombo) is used to retrieve calculated fields constructed from existing data.
 *
 * The following elements are used to describe a combo:
 *  - operation code from eOpr enum
 *  - the array of three locators of up to three arguments
 *
 * If an operation does not require all the three arguments the related pointer is set to zero.
 */

///combo descriptor
typedef struct sCombo {
    byte opr;                   ///< operation to perform
    word len;                   ///< result length
    tLocator arg[3];            ///< operation arguments
} tCombo;

int cmbInit(tCombo * cmb, byte opr, const tLocator * arg, word len);    ///<combo constructor
int cmbLen(const tCombo * cmb, word key);   ///<Return a length of the output combo data element
int cmbGet(const tCombo * cmb, word key, void *buf, word len);  ///<Retrieve the combo data element into buffer buf of length len

/** @} */

/** @} */

/** \addtogroup drwgroup DRW drawing structures and functions
 * @{
 */

/** \weakgroup PRIPIX Pixel shape
 * @{
 */
///pixel shape descriptor
typedef struct sPixel {
    word clr;                   ///<color
} tPixel;

int drwPixelInit(tPixel * shp, word clr);   ///<shape constructor
int drwPixelDraw(tPixel * shp, word x, word y); ///<draw shape anchored to the position (x,y)

/** @} */

/** \weakgroup PRILIN Line shape
 * @{
 */
///line shape descriptor
typedef struct sLine {
    word x, y;                  ///<line end coordinates
    word clr;                   ///<color
} tLine;

int drwLineInit(tLine * shp, word x, word y, word clr); ///<shape constructor
int drwLineDraw(tLine * shp, word x, word y);   ///<draw shape anchored to the position (x,y)

/** @} */

/** \weakgroup PRIBAR Bar shape
 * @{
 */
///bar shape descriptor
typedef struct sBar {
    word wdt, hgt;              ///<bar shape width and height
    word clr;                   ///<color
} tBar;

int drwBarInit(tBar * shp, word wdt, word hgt, word clr);   ///<shape constructor
int drwBarDraw(tBar * shp, word x, word y); ///<draw shape anchored to the position (x,y)

/** @} */

/** \weakgroup PRITXT Text shape
 * @{
 */
///text shape descriptor
typedef struct sText {
    const tContainer *cnt;      ///<contains the text to be displayed
    word key;                   ///<key to the text data element in the container
    byte fnt;                   ///<font
    card clr;                   ///<colors: background and foreground
} tText;

int drwTextInit(tText * shp, const tContainer * cnt, word key, byte fnt, card clr); ///<shape constructor
int drwTextDraw(tText * shp, word x, word y);   ///<draw shape anchored to the position (x,y)
int drwTextClone(tText * shp, const tContainer * cnt, word * key, byte * fnt, word * clr, byte dim);    ///<clone a text shape

/** @} */

/** \weakgroup PRITIL Tile shape
 * @{
 */
///tile shape descriptor
typedef struct sTile {
    const byte *ptr;            ///<pointer to tile/bitmap
    word wdt, hgt;              ///<tile shape width and height
    int clr;                    ///<color
} tTile;

int drwTileInit(tTile * shp, const byte * ptr, word wdt, word hgt, int clr);    ///<shape constructor
int drwTileDraw(tTile * shp, word x, word y);   ///<draw shape anchored to the position (x,y)

/** @} */

/** \weakgroup PRIBOX Box shape
 * @{
 */
///box shape descriptor
typedef struct sBox {
    word wdt, hgt;              ///<box shape width and height
    word clr;                   ///<color
} tBox;

int drwBoxInit(tBox * shp, word wdt, word hgt, word clr);   ///<shape constructor
int drwBoxDraw(tBox * shp, word x, word y); ///<draw shape anchored to the position (x,y)

/** @} */

/** \weakgroup PRICIR Circle shape
 * @{
 */
///circle shape descriptor
typedef struct sCircle {
    word rad;                   ///<radius
    word clr;                   ///<color
} tCircle;

int drwCircleInit(tCircle * shp, word rad, word clr);   ///<shape constructor
int drwCircleDraw(tCircle * shp, word x, word y);   ///<draw shape anchored to the position (x,y)

/** @} */

/** \weakgroup PRIDSC Disc shape
 * @{
 */
///disc shape descriptor
typedef struct sDisc {
    word rad;                   ///<radius
    word clr;                   ///<color
} tDisc;

int drwDiscInit(tDisc * shp, word rad, word clr);   ///<shape constructor
int drwDiscDraw(tDisc * shp, word x, word y);   ///<draw shape anchored to the position (x,y)

/** @} */

/** \weakgroup PRIBEZ Bezier curve degree 3 shape
 * @{
 */
///Bezier curve shape descriptor
typedef struct sBezier {
    card ctl[4];                ///<control points in packed format
    word clr;                   ///<color
} tBezier;

int drwBezierInit(tBezier * shp, const card * ctl, word clr);   ///<shape constructor
int drwBezierDraw(tBezier * shp, word x, word y);   ///<draw shape anchored to the position (x,y)

/** @} */

/** \weakgroup PRISHP Shape abstraction
 * @{
 */
enum eShp {
    shpBeg,                     ///<start sentinel
    shpPixel,                   ///<primitive pixel shape
    shpLine,                    ///<primitive line shape
    shpBar,                     ///<primitive bar shape
    shpText,                    ///<primitive text shape
    shpTile,                    ///<primitive tile shape
    shpBox,                     ///<box shape
    shpCircle,                  ///<circle shape
    shpDisc,                    ///<disc shape
    shpBezier,                  ///<Bezier curve shape
    shpButton,                  ///<button shape
    shpSelector,                ///<selector shape
    shpSlider,                  ///<slider shape
    shpVideo,                   ///<video shape
    shpDrawing,                 ///<drawing shape contains a set of primitive shapes or drawings
    shpEnd                      ///<end sentinel
};                              ///<shape enum contains all primitive shapes supported

///shape container descriptor
typedef struct sShape {
    byte typ;                   ///<shape type from eShp enum
    union uShape {              ///<union containing a pointer to the structure specified in typ field
        tPixel *pix;            ///<pixel shape
        tLine *lin;             ///<line shape
        tBar *bar;              ///<bar shape
        tText *txt;             ///<text shape
        tTile *til;             ///<tile shape
        tBox *box;              ///<box shape
        tCircle *cir;           ///<circle shape
        tDisc *dsc;             ///<disc shape
        tBezier *bzr;           ///<Bezier curve shape
        struct sButton *btn;    ///<button shape
        struct sSelector *sel;  ///<selector shape
        struct sSlider *sld;    ///<slider shape
        struct sVideo *vid;     ///<video shape
        struct sDrawing *drw;   ///<drawing shape (containing other shapes)
    } ptr;
    card loc;                   ///<location where the shape was rendered last time
    char key;                   ///a keyboard key associated with the shape
} tShape;

int drwShapeInit(tShape * shp, byte typ, void *ptr);    ///<shape constructor
int drwShapeDraw(tShape * shp, word x, word y); ///<draw shape anchored to the position (x,y)
card drwShapeLoc(const tShape * shp);   ///<get the location where the shape was rendered last time
card drwShapeRel(const tShape * shp, int x, int y); ///<get relative position of the point within the shape
void drwShapeKey(tShape * shp, char key);   ///<associate a keyboard key with the shape

#define RELOUT 0x7FFF7FFFL
/** @} */

/** \weakgroup PRIDRW Drawing
 * A drawing is a set of shapes shp displayed on the screen.
 * The drawing contains a set of reference points that define the relative position of each shape.
 * The coordinates of reference points are packed into array rfp of cards.
 * @{
 */
///drawing descriptor
typedef struct sDrawing {
    byte dim;                   ///<number of reference points in the drawing
    card *rfp;                  ///<array of reference points
    tShape *shp;                ///<array of shapes constituting the drawing
    card loc;                   ///<the location where the drawing was drawn last time
} tDrawing;

int drwDrawingInit(tDrawing * drw, tShape * shp, card * rfp);   ///<drawing shape constructor
int drwDrawingDraw(tDrawing * drw, word x, word y); ///<draw shape anchored to the position (x,y)
int drwDrawingRefresh(tDrawing * drw, byte num);    ///<redraw num shapes of the the drawing at the same location
byte drwDim(const tDrawing * drw);  ///<return number of shapes in the drawing
void drwShow(tDrawing * drw, byte idx); ///<make shape idx visible
void drwHide(tDrawing * drw, byte idx); ///<make shape idx invisible
void drwMove(tDrawing * drw, byte idx, word x, word y); ///<move the shape idx to another location
byte drwIsVisible(const tDrawing * drw, byte idx);  ///<is the shape idx invisible?
byte drwIdx(const tDrawing * drw, void *shp);   ///<return the shape index in the drawing
byte drwPush(tDrawing * drw, byte src, byte dst);   ///<push a shape inside the drawing from its original layer src to the layer dst
tShape *drwDrawingShape(const tDrawing * drw, int x, int y);    ///<get a pointer to a shape containing the given point

/** @} */

/** \weakgroup PRIBTN Button shape
 * Button shape is constructed as a drawing consisting of the following shapes:
 *  - shadow bar
 *  - background bar
 *  - surrounding frame box
 *  - focus indicator bar
 *  - button text
 *  - button icon (not implemented yet)
 *
 * Shadow and background bars are created in constructor automatically.
 *
 * The surrounding frame, button text and its relative position inside the button drawing (in the future the icon)
 * are provided to the constructor as input.
 *
 * A button can be in one of the following states:
 *  - 'n': normal state
 *  - 'f': focused button
 *  - 'p': pushed button
 * @{
 */
///shape indices in button drawing
enum {
    idxBtnShadow,
    idxBtnBgd,
    idxBtnFrame,
    idxBtnFocus,
    idxBtnIcon,
    idxBtnText,
    idxBtnEnd
};

///button shape descriptor
typedef struct sButton {
    tBar shw;                   ///<shadow
    tBar bgd;                   ///<background
    tBox frm;                   ///<surrounding frame
    tBar foc;                   ///<focus indicator
    card rfpBtn;                ///<relative text and icon position within the button drawing
    tTile ico;                  ///<button icon
    tText txt;                  ///<button text
    tShape shp[idxBtnEnd + 1];  ///<drawing shapes
    card rfp[idxBtnEnd];        ///<reference points
    tDrawing drw;               ///<button drawing
    char sta;                   ///<button state: 'n'-normal; 'f'-focused; 'p'-pressed
    const card *pal;            ///<color palette: foreground, background, shadow
} tButton;

///button palette
enum palBtn {
    palBtnFgd,
    palBtnBgd,
    palBtnShw,
    palBtnTxt,
    palBtnEnd
};

int drwButtonInit(tButton * shp, tBox * frm, card rfpBtn, tText * txt, tTile * ico, const card * pal);  ///<shape constructor
int drwButtonDraw(tButton * shp, word x, word y);   ///<draw shape anchored to the position (x,y)
int btnState(tButton * btn, char sta);  ///<set button state
int btnPush(tButton * btn);     ///<push button
int btnRefresh(tButton * btn);  ///<redraw the button at the same location

/** @} */

/** \weakgroup PRISLD Slider shape
 * Slider shape is constructed as a drawing consisting of the following shapes:
 *  - background bar
 *  - slider bar
 *
 * Slider and background bars are created in constructor automatically.
 *
 * The starting and ending bars are used to interpolate the slider position .
 *
 * A slider can take positions from 0 to sldDim().
 * The dimension of a slider is calculated as the maximal difference between starting and ending bar corners.
 * @{
 */
enum eSld {
    sldBgd,
    sldFgd,
    sldEnd
};

///slider shape descriptor
typedef struct sSlider {
    tBar bgd;                   ///<slider background
    tBar fgd;                   ///<slider bar
    tShape shp[sldEnd + 1];     ///<drawing shapes
    card rfp[sldEnd];           ///<reference points
    tDrawing drw;               ///<slider drawing
    word dim;                   ///<slider maximal position
    int bx1, by1, bx2, by2;     ///<initial corners
    int ex1, ey1, ex2, ey2;     ///<final corners
} tSlider;

int drwSliderInit(tSlider * shp, tBar * beg, tBar * end, int dx, int dy);   ///<shape constructor
int drwSliderDraw(tSlider * shp, word x, word y);   ///<draw shape anchored to the position (x,y)
word sldDim(const tSlider * sld);   ///<return slider dimension
word sldPos(tSlider * sld, word pos);   ///<set slider position
int sldRefresh(tSlider * sld);  ///<redraw the slider at the same location

/** @} */

/** \weakgroup PRISLC Selector shape
 * Menu shape is constructed as a drawing consisting of the following shapes:
 *  - shadow bar
 *  - background bar
 *  - surrounding frame box
 *  - array of selector items
 *  - focus selector box
 *
 * There is also an array of string items, part of them are displayed at any given time within
 * the selector window.
 *
 * The maximal number of selector items is __MNUMAX__ defined in the application context.
 *
 * @{
 */

///shape indices in menu drawing
enum {
    idxSelBgd,
    idxSelBak,
    idxSelFoc,
    idxSelEnd,
    dimSel = (idxSelEnd + dspH + 1)
};

///selector shape descriptor
typedef struct sSelector {
    tBar bgd;                   ///<background
    tBar bak;                   ///<background highlighting bar
    tBox foc;                   ///<focus selector box
    char itm[dspH][dspW + 1];   ///<selector item strings visible on the screen
    tContainer cnt[dspH];       ///<containers around the itm array
    tText txt[dspH];            ///<text shapes built on item containers
    tShape shp[dimSel + 1];     ///<drawing shapes
    card rfp[dimSel];           ///<reference points
    tDrawing drw;               ///<selector drawing
    word pos;                   ///<current position (top,foc)
    const card *pal;            ///<color palette: foreground, highlight bar, highlight text, item font
    tSlider *sld;               ///<associated slider (optional)
} tSelector;

///selector palette
enum palSel {
    palSelFgd,
    palSelBak,
    palSelCur,
    palSelFnt,
    palSelEnd
};

int drwSelectorInit(tSelector * shp, tBar * bgd, tBox * foc, tSlider * sld, const card * pal);  ///<shape constructor
int drwSelectorDraw(tSelector * shp, word x, word y);   ///<draw shape anchored to the position (x,y)
byte selDim(const tSelector * sel); ///<get number of selector items displayed
int selPos(tSelector * sel, const Pchar * str, word pos);   ///<set selector position
int brwPos(tSelector * sel, card pos, word max);    ///<set browser position
word selWhere(tSelector * sel, card loc);   ///<get the selector item touched
int selRefresh(tSelector * sel);    ///<redraw the selector at the same location

/** @} */

/** \weakgroup PRIVID Video shape
 * Video is a drawing where only two shapes are visible:
 *  - the first shape (0) is used as a background
 *  - the current shape
 * Usually all shapes have the same size, and the first one is a bar of background color.
 *
 * A video can take positions from 0 to sldDim().
 * @{
 */
///video shape descriptor
typedef struct sVideo {
    byte pos;                   ///<current video position
    tDrawing *drw;              ///<video drawing
} tVideo;

int drwVideoInit(tVideo * shp, tDrawing * drw); ///<shape constructor
int drwVideoDraw(tVideo * shp, word x, word y); ///<draw shape anchored to the position (x,y)
word vidDim(const tVideo * vid);    ///<return video dimension
word vidPos(tVideo * vid, word pos);    ///<set video position
int vidRefresh(tVideo * vid);   ///<redraw the video at the same location

/** @} */

/** @} */

/** \addtogroup guigroup GUI Graphical User Interface
 * @{
 */
/** \weakgroup PRIINP Input
 * An input is a drawing where each shape is associated with a key alias.
 * When a screen is touched on a shape this action is interpreted as pressing a key.
 * Some of this keys do not exist on the terminal keyboard.
 *
 * For example,
 *  - touching a slider arrow can be interpreted as key UP or key DOWN;
 *  - touching a slider body can be interpreted as page UP or page DOWN;
 *  - a set of buttons can be interpreted as virtual keyboard
 *  - etc.
 *
 * This structure is used in GUI to implement input dialogs, filling forms, navigating menus, browsing lists etc.
 * @{
 */
///virtual methods table for guiInput waiting
typedef struct sVmtGui {
    int (*poll) (card idx);     ///<the function is called in the loop while waiting
    int (*hdl) (char mod, tBuffer * buf);   ///<the function is called when an event happens
} tVmtGui;

///Click control during press button
void guiClick(byte clk);        ///<enable/disable clicking while key pressing in dialogs

///input descriptor
typedef struct sInput {
    tDrawing *drw;              ///<associated drawing
    word x, y;                  ///<drawing location
    byte dly;                   ///<waiting timeout
    char ctl[8 + 1];            ///<entry mode control string
    const tVmtGui *vmt;         ///<virtual functions
} tInput;

int guiInputInit(tInput * inp, tDrawing * drw, word x, word y, const char *ctl, const tVmtGui * vmt, byte dly); ///<input constructor
int guiInputWait(tInput * inp, byte num, tBuffer * buf);    ///<wait for user input
int guiInputDialog(tInput * inp, const tContainer * cnt, word key, const char *spc, byte maxLen);   ///<enter data using input form
int guiInputSelect(tInput * inp, const Pchar * str, word pos);  ///<select an item from the list
card guiGetTftLoc(const tBuffer * buf); ///<get coordinates of touch point
tShape *guiGetTftShp(const tBuffer * buf);  ///<get shape touched
char guiGetTftKey(const tBuffer * buf); ///<get associated key if any

///virtual methods table for guiInputBrowse fonction
typedef struct sVmtBrw {
    int (*dim) (void);          ///<return the number of items to browse
    int (*getTxt) (word idx, char *buf, byte len);  ///<get the row content
    int (*getFnt) (word idx);   ///<get the row text font
} tVmtBrw;
card guiInputBrowse(tInput * inp, const tVmtBrw * brw, card pos);   ///<browse a set of items

/** @} */

/** @} */

/** \addtogroup stmgroup STM Input STreaM and LZW compression
 * @{
 */
/** \weakgroup PRISTM Buffered I/O stream
 * This structure is used to access an input stream via RAM buffer.
 * It reduces the number of readings.
 * \test tcab0143.c
 * @{
 */
///stream descriptor
typedef struct sStream {
    tBuffer *buf;               ///<stream buffer
    byte sec;                   ///<nvm section
    card pos;                   ///<current read position within the stream
    card hdl;                   ///<file handle
    char mod;                   ///<access mode: 'r'=read, 'w'=write
} tStream;
int stmInit(tStream * stm, tBuffer * buf, const char *file, char mod);  ///<initialize a stream
int stmClose(tStream * stm);    ///<close the stream
int stmGet(tStream * stm, byte * ptr, int len, const byte * trm);   ///<get data from the stream until a terminator
int stmPut(tStream * stm, const byte * ptr, word len);  ///<put data into the stream

int lzwCompress(tStream * src, tStream * dst);  ///<compress src stream to dst stream using LZW compression
int lzwExpand(tStream * src, tStream * dst);    ///<decompress src stream to dst stream using LZW compression

enum bmlBit {                   ///<binary XML constants
    bmlBitA = 0x80,             ///<Indicates whether attributes follow the tag code.
    bmlBitC = 0x40              ///<Indicates whether this tag begins an element containing content.
};

///Binary XML build functions
int bmlPutHdr(tStream * stm);   ///<reset output stream and append header 03 01 03 00
int bmlPutTagBeg(tStream * stm, byte tag);  ///<append a tag to output stream
int bmlPutAtr(tStream * stm, byte atr, const char *val);    ///<append an attribute with value to output stream
int bmlPutTagEnd(tStream * stm, byte tag);  ///<if needed append END token to finish the set of tags
int bmlPutAtrEnd(tStream * stm, byte tag);  ///<if needed append END token to finish the set of attributes

typedef struct sVmtBml {
    int (*bmlHdlHdr) (const byte * buf);    ///<process Binary XML header
    int (*bmlHdlTag) (int tag, byte lvl);   ///<process tag
    int (*bmlHdlAtr) (byte atr, const char *buf);   ///<process attribute
} tVmtBml;
int bmlParse(tStream * stm, tVmtBml * vmt); ///<parse Binary XML stream using virtual handlers

/** @} */

/** @} */

/** @} */

#define __PRI_H
#endif
/** @} */
