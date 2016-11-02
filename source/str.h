//Subversion reference
//$HeadURL: https://svn.ingenico.com/UTAPROJECTS/sea-partner/5W/trunk/ISO5WPH/str/str.h $
//$Id: str.h 2047 2009-05-22 03:58:55Z ajloreto $

#ifndef __STR_H

/** \defgroup strgroup STR Structure components
 *
 * @{
 */

#include "ctx.h"

#ifdef __TEST__
#define __mnuTEST__ "? Test>           "
#else
#define __mnuTEST__ "? Test>           "
#endif

#define __DFTAPN__ "|swamandiri|||"
#define __DFTCHN__ "G"

#define __DFTSRV__  "010.204.038.091|8888"	//HOST DEV
#define __DFTSRVTMS__ "010.254.085.114|5406"	//HOST TMS
//#define __DFTSRV__  "010.254.085.114|5406"
//#define __DFTSRV__  "192.168.1.116|2000"
//#define __DFTCHN__ "T"		//test wifi

#define __DFTPPP__ "080989999|telkomnet@instan|telkom|60"			//smartweb

#define __DFTMONSRV__ "10.204.31.100|5001"

//#define __SoftwareName__ "SWM1.I" //len = 6
//#define __SoftwareVersion__ "0404" //len = 4

#ifdef __IWL__
#define __SoftwareName__ "SWM2I" //len = 5
#else
#define __SoftwareName__ "SWM4I" //len = 5
#endif
#define __SoftwareVersion__ "0800" //len = 4
#define __CtrlSoftwareVersion__ "1"

enum binLogoType {
    binLogoBeg,
    binLogoVisa,
    binLogoMC,
    binLogoJCB,
    binLogoAmex,
    binLogoCirrus,
    binLogoMaestro,
    binLogoEnd
};

#include "pri.h"
#include "key.h"

int traReset(void);             ///<reset volatile memory: transaction related data = tra section
int strFormatDatTim(word key);  ///<get date & time from logDatTim and format it for printing into traFmtDate

int usrInfo(word key);          ///<retrieve user screen from database and display it
int scrDlg(word infKey, word prmKey);
int isReversibleSend(void);     //is current transaction reversible one?
int isReversibleRecv(void);     //is current transaction reversible one?
int incCard(word key);          ///< increments the inputted key of card data type with max value of 999999

void hdlMainMenu(void);
void CekMenuAllowed();

#define MAPGET(KEY,BUF,LBL) { ret= mapGet(KEY,BUF,sizeof(BUF)); CHECK(ret>=0,LBL);}
#define MAPGETBYTE(KEY,VAR,LBL) { ret= mapGetByte(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPGETWORD(KEY,VAR,LBL) { ret= mapGetWord(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPGETCARD(KEY,VAR,LBL) { ret= mapGetCard(KEY,VAR); CHECK(ret>=0,LBL);}

#define MAPPUTSTR(KEY,VAR,LBL) { ret= mapPutStr(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPPUTBYTE(KEY,VAR,LBL) { ret= mapPutByte(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPPUTWORD(KEY,VAR,LBL) { ret= mapPutWord(KEY,VAR); CHECK(ret>=0,LBL);}
#define MAPPUTCARD(KEY,VAR,LBL) { ret= mapPutCard(KEY,VAR); CHECK(ret>=0,LBL);}

#define __BOLD__        "\x1B\x1A"
#define __COMPRESSED__  "\x1B\x20"
#define __DBLSIZE__  	"\x1b\x1e"
#define __DBLHEIGHT__	"\x1b\x1d"
#define __DBLWIDTH__	"\x1b\x1c"
#define __RIGHT__			"\x1b\x18"
#define __CENTER__	  "\x1b\x17"

#define __BOLD_COMPR__    "\x1B\x1A\x1B\x20"//mcy
#define __BOLD_CENTER__	  "\x1B\x1A\x1b\x17"
#define __COMPR_CENTER__  "\x1B\x20\x1b\x17"
#define __DBLHEIGHT_BOLD__	"\x1b\x1d\x1B\x1A" 
#define __SPACES__	  "\x1b\x13" //mcy

/** @} */
#define MEM_ZERO(VAR) memset(VAR, 0, sizeof(VAR));
#define MEM_0x20(VAR) memset(VAR, 0x20, sizeof(VAR));
#define MEM_0x30(VAR) memset(VAR, 0x30, sizeof(VAR));

/* @} */
#define RUN_FUNCTION(F, LBL) {ret = F(); CHECK(ret > 0, LBL);} //mcy
#define dsp_ALIGN_LEFT 		0
#define dsp_ALIGN_CENTER 	1
#define dsp_ALIGN_RIGHT 	2

#define kbdF1_kbdF4 		1
#define kbdVAL_kbdANN 		0


#define __STR_H
#endif
