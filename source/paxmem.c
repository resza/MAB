/** \file
 * Non volatile memory processing for UNICAPT (DFS and ISM)
 *
 * Subversion reference
 *
 * $HeadURL: https://svn.ingenico.com/UTA/core/utaYYMMDD/sys/mem.c $
 *
 * $Id: mem.c 2616 2010-04-07 11:16:41Z abarantsev $
 */
#include "global.h"
#include <string.h>
#include <stdio.h>
#include "sys.h"

#undef trcOpt
#define trcOpt (TRCOPT & BIT(trcMEM))

/** Get DFS file name for a given section.
 * 
 * The DFS file name is built as "page00NN.nvm" where NN is the section number in hex.
 * \param buf (O) the DFS file name
 * \param sec (I) nvm section
 * \return no
 * \header sys\\sys.h
 * \source sys\\mem.c
 */
void nvmFileName(char *buf, byte sec) {
    sprintf(buf, "page%04x.nvm", sec);
}

void nvmPathFileName(char *buf, byte sec) {
    nvmFileName(buf, sec);
}

static uint hNvm;             //DFS file handle holded
static byte sNvm;               //0xFF if not hold; otherwise section number holded

/** Initialize non-volatile memory internal variables.
 * Should be called before calling any non-volatile memory treatment, normally during POWERON event.
 * \return no
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcab0005.c
 */
void nvmStart(void) {
    trcFS("%s\n", "nvmStart");
    hNvm = 0;
    sNvm = 0xFF;
}

static int nvmOpen(byte sec, const char *atr) {
   	int hdl;                 //dfs file handle
    	char tmp[8 + 1 + 3 + 1];    //to store file name
    	uchar tmpatr;

    	if(sNvm == sec)
       	return hNvm;
    	nvmFileName(tmp, sec);      //generate file name
	trcFS("nvmFileName = %s", tmp);


	tmpatr=O_RDWR;
	hdl = open(tmp, tmpatr);
	trcFN("nvmOpen 1, hdl= %d", hdl);
	
	if(hdl<0)
	{
		tmpatr = O_CREATE;
		hdl = open(tmp, tmpatr);
		trcFN("nvmOpen 2, hdl= %d", hdl);
		
		if(hdl<0)
			goto lblKO;
	}
    	return hdl;
lblKO:
	return -1;
}

static void nvmClose(byte sec, uint hdl) {
	trcS("nvmClose");
    	if(sNvm == sec)
        	return;                 //it is holded
	close((int)hdl);
}

/** Remove the DFS file containing the section sec.
 * \param sec (I) Section number
 * \return 1 if OK; negative otherwise
 * \header sys\\sys.h
 * \source sys\\mem.c
*/
int nvmRemove(byte sec) {
    int ret;
    char tmp[8 + 1 + 3 + 1];    //to store file name

    trcFN("nvmRemove sec=%d\n", (word) sec);
    nvmFileName(tmp, sec);      //generate file name
    ret = remove(tmp);
    if(ret<0)
		goto lblKO;
	
//    CHECK(ret == RET_OK, lblKO);
    return 1;
  lblKO:return 0;
}

/** Hold section sec opened until nvmRelease() called.
 *
 * The couple of function nvmHold()/nvmRelease() is used to avoid file opening and closing
 * at each data access operation.
 * If section sec is accessed several times sequentially the performance can be improved
 * by calling nvmHold at the beginning and nvmRelease at the end. Other sections should not be accessed in hold state.
 * Opens the related DFS file without closing it
 * \param sec (I) Section number
 * \return no
 * \header sys\\sys.h
 * \source sys\\mem.c
 */
int nvmHold(byte sec) {
    int hdl;

    hdl = nvmOpen(sec, "r+");
	if(hdl<0)
		goto lblKO;
	
//    CHECK(hdl, lblKO);
    sNvm = sec;
    hNvm = hdl;
    return hdl;
  lblKO:return -1;
}

/** Close section sec opened by preceding nvmHold() call.
 *
 * The couple of function nvmHold()/nvmRelease() is used to avoid file opening and closing
 * at each data access operation.
 * If section sec is accessed several times sequentially the performance can be improved
 * by calling nvmHold at the beginning and nvmRelease at the end.
 * Other sections should not be accessed in hold state.
 * Closes the related DFS file
 * \param sec (I) Section number
 * \return no
 * \header sys\\sys.h
 * \source sys\\mem.c
 */
void nvmRelease(byte sec) {
    if(sNvm != 0xFF) {
        close((int)hNvm);
        sNvm = 0xFF;
        hNvm = 0;
    }
}

#ifdef __BLOCKSIZE__
#define BLK __BLOCKSIZE__
#else
#define BLK 512
#endif
/** Save len bytes from buffer buf into section sec starting from the position ofs.
 *  The information is saved into pageNNNN.nvm file
 * \param sec (I) Section number
 * \param buf (I) Pointer to a buffer to be saved
 * \param ofs (I) Starting offset within the section; 0XFFFFFFFFL meaning append to the end of file
 * \param len (I) Number of bytes to be saved
 * \return
 *    - (-eNvm code) if failure.
 *    - len if not.
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcab0005.c
 */
int nvmSave(byte sec, const void *buf, card ofs, word len) {
    int ret;                    //to be returned: number of bytes saved
    uint hdl;                 //dfs file handle
    uint pos;                 //file position
    card idx;                   //number of bytes rest to save
    card blk;                   //number of bytes in the current block
    const byte *ptr;

    hdl = 0;
    if(len) {
        hdl = nvmOpen(sec, "r+");
	if(hdl<0)
		goto lblOpen;
	
       // CHECK(hdl, lblOpen);

        if(ofs == 0xFFFFFFFFL)
            pos = seek(hdl, 0, SEEK_END);   //move the end of file
        else {
            pos = seek(hdl, ofs, SEEK_SET); //seek given offset
            CHECK(pos == 0, lblSeek);
        }

        idx = len;
        ptr = buf;
        while(idx) {            //fill by portions of BLK bytes or less
            blk = BLK;          //the size of portion to save
            if(idx < blk)       //is there enough bytes?
                blk = idx;
		if(flagprint)
		{
			hex_dump_char("ptr", (uchar*)ptr, blk);
		}
		
            ret = write(hdl, (uchar *) ptr, (int) blk);  //save it
            CHECK(ret >= RET_OK, lblKO);
            idx -= blk;
            ptr += blk;
        }
        //ret = psyFileWrite(hdl, (void *) buf, (ushort) len);    //and save it
        //CHECK(ret == RET_OK, lblKO);
    }

    trcFS("%s: ", "nvmSave");
    trcFN("sec=%d ", sec);
    trcFN("ofs=%d buf=", ofs);
    trcBN((byte *) buf, (word) len);
    trcS(" [");
    trcAN(buf, len);
    trcS("]\n");

    ret = len;
    goto lblEnd;

  lblOpen:trcErr(hdl);
    return -nvmOpenErr;
  lblSeek:trcErr(pos);
    ret = -nvmSeekErr;
    goto lblEnd;
  lblKO:trcErr(ret);
    ret = -nvmKO;
    goto lblEnd;
  lblEnd:
    if(len)
        nvmClose(sec, hdl);
    return ret;                 //close it anyway
}

/** Fill len bytes by value val into section sec starting from the position ofs.
 *  The information is written into pageNNNN.nvm file
 * \param sec (I) Section number
 * \param val (I) Value to fill
 * \param ofs (I) Starting offset within the section; 0XFFFFFFFFL meaning append to the end of file
 * \param len (I) Number of bytes to be filled
 * \return  
 *    - (-eNvm code) if failure.
 *    - len if not.
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcab0005.c
 */
int nvmSet(byte sec, unsigned char val, card ofs, card len) {
    int ret;                    //to be returned: number of bytes set
    uint hdl;                 //dfs file handle
    uint pos;                 //file position
    byte tmp[BLK];              //to blocks saved
    card idx;                   //number of bytes rest to save
    card blk;                   //number of bytes in the current block

    trcFS("%s: ", "nvmSet");
    trcFN("sec=%d ", sec);
    trcFN("ofs=%d ", ofs);
    trcFS("val=", "");
    trcBN(&val, 1);
    trcFN("len=%d\n", len);

    hdl = nvmOpen(sec, "r+");
    CHECK(hdl>=0, lblOpen);

    memset(tmp, val, BLK);      //fill buffer by val
    idx = len;
    if(ofs == 0xFFFFFFFFL)
        pos = seek(hdl, 0, SEEK_END);   //move the end of file
    else
        pos = seek(hdl, ofs, SEEK_SET); //seek given offset
    CHECK(pos == 0, lblSeek);
    while(idx) {                //fill by portions of BLK bytes or less
        blk = BLK;              //the size of portion to save
        if(idx < blk)           //is there enough bytes?
            blk = idx;
        ret = write(hdl, tmp, (int) blk);   //save it
        if(ret<0)
        	goto lblKO;
        idx -= blk;             //calculate the number of bytes rest to fill
    }

    ret = len;
    goto lblEnd;
  lblOpen:trcErr(hdl);
    return -nvmOpenErr;
  lblSeek:trcErr(pos);
    ret = -nvmSeekErr;
    goto lblEnd;
  lblKO:trcErr(ret);
    ret = -nvmKO;
    goto lblEnd;
  lblEnd:nvmClose(sec, hdl);
  trcFN("nvmSet: ret = %d", ret);
    return ret;
}

#undef BLK

/** Load len bytes into buffer buf from section sec starting from the position ofs.
 * The information is loaded from pageNNNN.nvm file
 * \param sec (I) Section number
 * \param buf (O) Pointer to a buffer to be loaded
 * \param ofs (I) Starting offset within the section
 * \param len (I) Number of bytes to be saved
 * \return
 *    - (-eNvm code) if failure.
 *    - len if not.
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcab0005.c
 */
int nvmLoad(byte sec, void *buf, card ofs, word len) {
    int ret;                    //to be returned: number of bytes loaded
    int hdl;                 //dfs file handle
    uint pos;                 //file position
    ushort blk;                 //number of bytes read

    hdl = nvmOpen(sec, "r+");
    CHECK(hdl, lblOpen);

    pos = seek(hdl, ofs, SEEK_SET); //seek given offset
    CHECK(pos == 0, lblSeek);
    blk = len;
    ret = read(hdl, (uchar *)buf, (int)blk);  //load from file
	if(ret<0)
		goto lblKO;
    //CHECK((ret == RET_OK) && (blk == len), lblKO);
    //CHECK(ret == RET_OK, lblKO);

    trcFS("%s: ", "nvmLoad");
    trcFN("sec=%d ", sec);
    trcFN("ofs=%d buf=", ofs);
    trcBN((byte *) buf, (word) blk);
    trcS(" [");
    trcAN(buf, blk);
    trcS("]\n");

    //ret = len;
    //ret = blk;
    goto lblEnd;

  lblOpen:trcErr(hdl);
    return -nvmOpenErr;
  lblSeek:trcErr(pos);
    ret = -nvmSeekErr;
    goto lblEnd;
  lblKO:trcErr(ret);
    ret = -nvmKO;
    goto lblEnd;
  lblEnd:nvmClose(sec, hdl);
    return ret;
}

/** Return the size of a section
 * \param sec (I) section number
 * \return size of the section if OK; negative otherwise
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcgs0030.c
*/
int nvmSize(byte sec) {
    long pos;
	
    int ret;
    char tmp[8 + 1 + 3 + 1];    //to store file name

    MEM_ZERO(tmp);
    ret = 0;

    nvmFileName(tmp, sec);      //generate file name
    pos = filesize(tmp);
    if(pos<0)
		goto lblKO;

    ret = (int)pos;
    goto lblEnd;
  lblKO:
  lblEnd:
    return ret;
}

/** Move a section to an another place (remove it from original place, and overwrite the destination place).
 * \param srcSec (I) Section number of the source
 * \param dstSec (I) Section number of the destination
 * \return zero if the size of the file is greater than the expected size
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcgs0030.c
*/
int nvmMove(byte srcSec, byte dstSec) {
    int ret = -1;
    char tmp1[8 + 1 + 3 + 1];   //to store file name
    char tmp2[8 + 1 + 3 + 1];   //to store file name

    CHECK(hNvm != dstSec && hNvm != srcSec, lblKO);
    CHECK(nvmSize(srcSec), lblKO);  //Does it exist?
    nvmFileName(tmp1, srcSec);
    nvmFileName(tmp2, dstSec);
    nvmRemove(dstSec);
//    ret = psyFileRename(tmp1, tmp2);
//    CHECK(ret == RET_OK, lblKO);
    nvmRemove(srcSec);
    ret = 1;
    goto lblEnd;
  lblKO:
    trcErr(ret);
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

/** Open a DFS file for sequential access (reading or writing)
 * \param file (I) file name
 * \param atr (I) attribute: 'r'= reading; 'w': writing
 * \return file handle that should be used in nvmSeqClose,nvmSeqGet,nvmSeqPut
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcab0186.c
*/
int nvmSeqOpen(const char *file, char atr) {
    uint hdl;
//    char tmp[3];
    uchar newtmp;

    newtmp=O_RDWR;
    
    hdl = open((char *) file, newtmp);
    if(hdl<0)
    {
    	goto lblKO;
    }
    //CHECK(hdl, lblKO);
    return hdl;
  lblKO:return -1;
}

/** Close a DFS file opened by nvmSeqOpen() for sequential access (reading or writing)
 * \param hdl (I) DFS file handle returned by nvmSeqOpen()
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcab0186.c
*/
void nvmSeqClose(card hdl) {
    close(hdl);
}

/** Erase a file.
 * \param file (I) file name
 * \header sys\\sys.h
 * \source sys\\tlmmem.c
 * \test 
 */
int nvmSeqRemove(const char *file) {
    int ret;

    ret = remove((char *) file);
    CHECK(ret == 0, lblKO);

    return 1;
  lblKO:
    return 0;
}

/** Set file position
 * \param hdl (I) DFS file handle returned by nvmSeqOpen()
 * \param pos (I) file position to set
 * \header sys\\sys.h
 * \source sys\\mem.c
*/
int nvmSeqPos(card hdl, card pos) {
    int ret;

    ret = seek(hdl, pos, SEEK_SET);
    CHECK(ret == 0, lblKO);
    return pos;
  lblKO:
    return -1;
}

/** Read up to len bytes into the buffer buf from the file handle hdl
 * \param hdl (I) DFS file handle returned by nvmSeqOpen()
 * \param buf (O) destination buffer pointer
 * \param len (I) maximal number of bytes to read
 * \return number of bytes actually read
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcab0186.c
*/
int nvmSeqGet(card hdl, void *buf, word len) {
    int ret;
    int num;                 //number of bytes retrieved

    memset(buf, 0, len);
    num = len;
    ret = read(hdl, (uchar *)buf, num);
    CHECK(ret == RET_OK, lblKO);
    ret = num;

    trcS("nvmSeqGet: ");
    trcBAN((byte *) buf, ret);
    trcS("\n");
    goto lblEnd;
  lblKO:
    trcErr(ret);
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}

/** Write len bytes into the file handle hdl from the buffer buf 
 * \param hdl (I) DFS file handle returned by nvmSeqOpen()
 * \param buf (I) source buffer pointer
 * \param len (I) maximal number of bytes to write
 * \header sys\\sys.h
 * \source sys\\mem.c
 * \test tcab0186.c
*/
int nvmSeqPut(card hdl, const void *buf, word len) {
    int ret;

    ret = write((int)hdl, (uchar *) buf, (int) len);
    if(ret<0)
		goto lblKO;

    ret = len;
    goto lblEnd;
  lblKO:
    trcErr(ret);
    ret = -1;
    goto lblEnd;
  lblEnd:
    return ret;
}


