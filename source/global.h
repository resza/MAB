
/*********************************************************************************
NAME
    global.h - 定义公共数据结构

DESCRIPTION
    定义应用的全局变量/全局常量等

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
*********************************************************************************/


/*********************************************************************************
Warning
This source files are for template use only.
If you want to use it as a proto-type of a specific project,
do not directly modify on it.
*********************************************************************************/


#ifndef _GLOBAL_H
#define _GLOBAL_H

/*********************************************************************************
编译前务须根据编译的要求开启/关闭/检查下述宏定义
Please check below macro definition before compile
*********************************************************************************/
// #define _P60_S1_
// #define _P70_S_
// #define _P78_
// #define _P80_
// #define _P90_
// #define _P58_
#define _S_SERIES_	//irf, 20121128
//#define _SP30_

/*********************************************************************************
设定各项特性开启/关闭
Set to correct macro before compile
*********************************************************************************/
//#define APP_MANAGER_VER	// When build this app as manager, should enable this macro 当编译主应用时，开启此宏定义
#define ALLOW_NO_TMS		// allow init default, without TMS. but TMS still available
//#define USE_EXTEND_PXXCOM	// use external Pxx communication module. (external box or so)
#define ENABLE_EMV			// Link EMV lib into the application.
//#define EMV_TEST_VERSION	// use hard coded emv parameters for EMV test. Valid only when ENABLE_EMV defined
//#define APP_DEBUG			// debug mode
//#define FUN2_READ_ONLY	// Allow operator to modify FUNCTION 2.

#ifdef ENABLE_EMV
	//// #define __EMV_DEBUG__    // diki open for emv debug
#endif

// Contactless card support (CLSS)
#define ENABLE_PAYWAVE      // VISA PayWave
#define ENABLE_PAYPASS      // MASTERCARD PayPass
#define ENABLE_PBOC         // China Unionpay QPBOC

//#define BANK_BRI
//#define BANK_MANDIRI	//irf, 20121103
#define BANK_BUKOPIN	//zaki 25022014
//	#define BANK_PANIN		// untuk cicilan bank panin
//	#define BANK_HSBC		// untuk cicilan bank panin
//  #define BANK_ARTHA_JASA	// untuk miniATM Artha Jasa
//#define BANK_BNI	//irf, 20121103
/////#define MINI_ATM_BUKOPIN	//  diki add for mini atm bukopin


#define PRODUCTION // for disable some testing func
/*********************************************************************************
Application attribute 应用属性(名称, ID, 版本)
*********************************************************************************/
#ifndef BANK_BUKOPIN
#define APP_NAME		"EDC TEMPLATE"
#define EDCAPP_AID		"EDC_TEMPLATE_001"
#define EDC_VER_PUB		"1.00"			// 公共版本号. Public version number
#define EDC_VER_INTERN	"1.00.0012"		// 扩展版本号(前部分应和公共版本号一致). Extend version number. should be same as EDC_VER_INTERN's heading.
#else
	#ifdef BANK_PANIN
#define APP_NAME		"EDC_PANIN"
#define EDCAPP_AID		"EDC_PANIN_001"
#define EDC_VER_PUB		"1.00"			// 公共版本号. Public version number
#define EDC_VER_INTERN	"1.00.0014"		// 扩展版本号(前部分应和公共版本号一致). Extend version number. should be same as EDC_VER_INTERN's heading.
	#endif
	#ifdef BANK_HSBC
#define APP_NAME		"EDC HSBC"
#define EDCAPP_AID		"EDC_HSBC_002"
#define EDC_VER_PUB		"2.01"			// 公共版本号. Public version number
#define EDC_VER_INTERN	"2.01.0014"		// 扩展版本号(前部分应和公共版本号一致). Extend version number. should be same as EDC_VER_INTERN's heading.
	#else
#define APP_NAME		"EDC BUKOPIN"
#define EDCAPP_AID		"EDC_BUKOPIN_002"
#define EDC_VER_PUB		"2.01"			// 公共版本号. Public version number
#define EDC_VER_INTERN	"2.01.0014"		// 扩展版本号(前部分应和公共版本号一致). Extend version number. should be same as EDC_VER_INTERN's heading.
	#endif
#endif
/*********************************************************************************
Features of specific area 地区专有属性
*********************************************************************************/
//#define AREA_TAIWAN		// Taiwan
//#define AREA_HK
//#define AREA_SG
//#define AREA_VIETNAM		// Vietnam
//#define AREA_KH			// Cambodia
//#define  AREA_Arabia		// support Arabia
#define AREA_INDONESIA		//indonesia, irf 20121203


#define	dspClear()	ScrCls()
#define	dspLS(position, text)	PubDispString(text, DISP_LINE_CENTER|position*2)
/*********************************************************************************
EMV terminal capability EMV终端能力配置
*********************************************************************************/
#define EMV_CAPABILITY	"\xE0\xF0\xC8"
//	9F33 - Terminal Capabilities Length = 03
//		Byte 1 Bit 8 Manual key entry 
//		Byte 1 Bit 7 Magnetic stripe 
//		Byte 1 Bit 6 IC with contacts 
//		Byte 2 Bit 8 Plaintext PIN for offline ICC verification 
//		Byte 2 Bit 7 Enciphered PIN for online verification 
//		Byte 2 Bit 6 Signature (paper) 
//		Byte 2 Bit 5 Enciphered PIN for offline ICC verification 
//		Byte 2 Bit 4 No CVM Required 
//		Byte 3 Bit 8 Offline static data authentication 
//		Byte 3 Bit 7 Offline dynamic data authentication 
//		Byte 3 Bit 6 Card capture 
//		Byte 3 Bit 4 Combined DDA/AC Generation 

/*********************************************************************************
以下定义仅在变更程序功能属性时修改。如果某些特性和此机型是绑定的，请在此定义
注意：平时请不要手动开启/关闭下列宏。这些宏的开关应由前面的宏是否已被定义而自动决定是否开启
Below macro definitions are related to machine/application feature.
DO NOT modify them unless machine feature is changed.
Warning: DO NOT manually enable/disable below macros. they're determined automatically.
*********************************************************************************/

#if defined(WIN32) && defined(_P60_S1_)		// Convenient for Propay(P70) use.
#undef _P60_S1_
#define _P70_S_
#endif

#ifdef _P60_S1_
#endif

#ifdef _P70_S_
#endif

#ifdef _P80_
#define PXX_COMM
#endif

#ifdef _P90_
#define PXX_COMM				// P80/P90的IP模块无线模块通迅方式
#endif

#ifdef _P58_
#endif

#ifdef USE_EXTEND_PXXCOM
#define PXX_COMM
#endif

// 注意：也有部分老S80不支持SXX_WIRELESS_MODULE
#ifdef _S_SERIES_
#define SXX_IP_MODULE			//S80的IP模块通迅方式
#define SXX_WIRELESS_MODULE		//S系列无线模块
#define S90_WIFI
#endif

// 作用：在GCC环境下设定字节对齐
#if defined(_WIN32) || defined(_WINDOWS)
	#define PACK_STRUCT
#elif defined(_P80_) || defined(_P90_) || defined(_P78_) || defined(_S_SERIES_) || defined(_SP30_)
	#define PACK_STRUCT		__attribute__((packed))
#else
	#define PACK_STRUCT
#endif

extern unsigned char flagprint;

#define __TELIUM_PARAM__

#include "Stdarg.h"

// 在这里添加你的工程的头文件
// Please add your customized header files here.
#include "posapi.h"
#include "appLib.h"

#ifdef ENABLE_EMV
#include "emvlib.h"		// Recommend to link to external file, not copying it into source folder.
						// Use project--settings--c/c++--category=preprocessor--additional include directories
#endif

#include "sys.h"
#include "pri.h"
#include "key.h"
#include "str.h"
#include "dft.h"
#include "map.h"

#include "commlib.h"
#include "util.h"
#include "pack8583.h"
#include "st8583.h"
#include "initpos.h"
#include "setup.h"
#include "fileoper.h"
#include "tranproc.h"
#include "eventmsg.h"
#include "checkopt.h"
#include "commproc.h"
#include "print.h"
#include "password.h"
#include "manage.h"
#include "pedpinpad.h"
#include "cpinpad.h"
#include "MultiApp.h"
#include "lng.h"
#include "Simulate.h"
#include "MultiApp.h"
#include "mlogo.h"

#include "TMS.h"
#include "setpack.h"
#include "unPack.h"
#include "TLE.h"
#include "main.h"

// add cung for BRI
#include "Purchase.h"
#include "miniATM.h"
#include "Blowfish.h"
#include "toolsBRI.h"
#include "Function.h"
#include "PrintBRI.h"
#include "brizzi.h"
// end add cung

//Bukopin
#include "toolsBukopin.h"

#ifdef AREA_Arabia
#include "fstruct.h"
#include "Farsi.h"
#endif

#ifdef ENABLE_EMV
#include "emvproc.h"
#include "emvtest.h"
#endif

#ifdef _P60_S1_
#include "PED.h"
#endif
#if defined(SXX_WIRELESS_MODULE) || defined(SXX_IP_MODULE)
#include "SxxCom.h"
#endif

#define LEN_WORK_KEY	16
// Below IDs should be customized according to your project
#define MASTER_KEY_ID	    1
#define DEF_PIN_KEY_ID	    1		// default PIN key ID (if not assign)
#define GENERAL_MKEY_ID     2       // Used for getting EMV offline plain PIN
#define GENERAL_WKEY_ID     2       // Used for getting EMV offline plain PIN
#define MAC_KEY_ID		    10

// Card slot number for customer.
#define ICC_USER		0

#define MAX_EVENTBUF_SIZE		4096

// 密码类型 Password type
enum {PWD_BANK, PWD_TERM, PWD_MERCHANT, PWD_VOID, PWD_REFUND, PWD_ADJUST, PWD_SETTLE, PWD_MAX};

// 权限类型 Permission Level
// 若某个设置需要PM_MEDIUM权限，则当传入参数为PM_LOW时，此参数不允许被修改
// for example, if editing a parameter require PM_MEDIUM, then when ucPermission==PM_LOW, user cannot modify it.
enum {PM_LOW, PM_MEDIUM, PM_HIGH, PM_DISABLE};

// PED类型 PED type
enum {PED_INT_PXX, PED_INT_PCI, PED_EXT_PP, PED_EXT_PCI};

// GetTermInfo()里面的机器类型 Machine type defined in GetTermInfo().
#define _TERMINAL_P60_S_   1
#define _TERMINAL_P70_S_   2
#define _TERMINAL_P60_S1_  3
#define _TERMINAL_P80_     4
#define _TERMINAL_P78_     5
#define _TERMINAL_P90_     6
#define _TERMINAL_S80_	   7  
#define _TERMINAL_SP30_	   8  
#define _TERMINAL_S60_	   9
#define _TERMINAL_S90_	   10
#define _TERMINAL_S58_	   14

// #define EPSAPP_UPDATEPARAM		100
// #define EPSAPP_GENSUBINFO		110
// #define EPSAPP_DEBITTRANS		200
// #define EPSAPP_NONEPS_DEBIT		250
// #define EPSAPP_TIMEREVENT		400
// #define EPSAPP_RECAP				500
// #define EPSAPP_REPRINT			700

// 刷卡/插卡事件
#define NO_SWIPE_INSERT		0x00	// 没有刷卡/插卡
#define CARD_SWIPED			0x01	// 刷卡
#define CARD_INSERTED		0x02	// 插卡
#define CARD_KEYIN			0x04	// 手输卡号
#define CARD_TAP			0x05	// diki add


///// diki add
#define PRIVATE_LABEL_CARD			0x06	// diki add
#define NON_PRIVATE_LABEL_CARD		0x07	// diki add




#define SKIP_DETECT_ICC		0x20	// 不检查ICC插卡事件 // 防止用户拔卡过快[12/12/2006 Tommy]
#define FALLBACK_SWIPE		0x40	// 刷卡(FALLBACK)
#define SKIP_CHECK_ICC		0x80	// 不检查service code

#define APPMSG_SYNC				11	// 用于manager向子应用传递信息
#define EPSAPP_FUNCKEYMSG		800
#define EDC_ECR_MSG				1231
#define EDCAPP_LOADDOWNPARA		1232
#define EDCAPP_GETPARA			1233
#define EDCAPP_AUTOUPDATE		1235

#define EVENT_GET_NAME		0x2001
#define EVENT_GET_NAME_OK	0x2002

// 系统限制宏定义
#define MAX_PLAN 				24
#define MAX_ACQ					16
#define MAX_ISSUER				32
#define MAX_CARD				128
#define MAX_DESCRIPTOR			10
#define MAX_CARDBIN_NUM			50	// 800
#define MAX_CARDBIN_ISSUER		50		//250
#define MAX_CARBIN_NAME_LEN		30
#define LEN_MAX_AD_DATA			4000
#define MAX_TRANLOG				200	// 200	// must <65535
#define MAX_MINIATM_TRANLOG	100	// 200	// must <65535
#define MAX_TUNAI_TRANLOG	100	// 200	// must <65535
#define MAX_ERR_LOG				1	//10	// # of EMV error transactions, adjust as needed

#define USER_OPER_TIMEOUT		60		// 用户超时设置
#define USER_INPUT_TIMEOUT		60		

#define ACQ_ALL					0xFF	// 所有Acquirer
#define INV_ACQ_KEY				0xFF	// 无效的acquirer key
#define INV_ISSUER_KEY			0xFF	// 无效的issuer key

// RFU
#define ECR_NONE		0
#define ECR_BEGIN		1
#define ECR_SUCCEED		2
#define ECR_CANCEL		3

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////TELIUM/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __TELIUM_PARAM__

#define EM_CHIP_INITIAL 				0x01
#define EM_MAGNETIC_STRIPE_INITIAL	0x02
#define EM_MANUAL_INITIAL				0x04
#define EM_EMV_SVC						0x08
#define EM_FALLBACK_CHIP				0x10
#define EM_FALLBACK_MAGNETIC_STRIP	0x20
#define EM_FALLBACK_MANUAL			0x40
#define EM_EMV_PROCESS					0x80


//option 1
#define EDC_TLM_TMS_DISPLAY_NAME			0x80
#define EDC_TLM_TMS_DATE_FORMAT			0x40
#define EDC_TLM_TMS_TIME_ON_ROC			0x20
#define EDC_TLM_TMS_TIPS_ALLOWED			0x04
#define EDC_TLM_TMS_DISPLAY_PAN			0x02
#define EDC_TLM_TMS_MOTO_ENABLED			0x01
//option 2
#define EDC_TLM_TMS_BASE_TOTAL_ENTRY		0x20
#define EDC_TLM_TMS_PRINTER_ENABLED		0x02
//option 3
//option 4
//option 5
#define EDC_TLM_TMS_FORCE_TRACK_1		0x80
#define EDC_TLM_TMS_CONFIRM_AMT_ENTRY	0x40
//local option
#define EDC_TLM_TMS_ROC_SEARCH_PASSWD	0x08
#define EDC_TLM_TMS_REFUND_PASSWD_REQ	0x04
#define EDC_TLM_TMS_VOID_PASSWD_REQ		0x02
#define EDC_TLM_TMS_KEYBOARD_LOCKED		0x01
#endif

#ifdef __TELIUM_PARAM__
typedef struct _tagregBin
{
	  uint 	regTrxNo;				//'transaction number' />
	  uint	regSTAN;				//'System Trace Audit Number 1..999999' />
	  ulong	regInvNum;				//'invoice number (R.O.C.)' /> 
	  uchar	regBatNum[6+1];			//'Batch number' /> 
	  uchar	regBinCnt;				//'bin Counter' />
	  uchar	regIssCnt;				//'Iss counter' />
	  uchar	regAcqCnt;				//'acq counter' />
	  uchar	regReqSent;				//'Is there request sent and response is not received yet?' />
	  uchar	regLocType;				//T = (dec)84 Location on where to get certain info: L - log, T - tra' />
	  uchar	regAidCnt;				//'Number of AID records in database' />  
	  uchar	regPubCnt;				//'Number of Pub key records in database' />
	  uchar	regTmsRefNum[8+1];	//'TMS Refence Number'/>
	  ulong	regSTANContext;			//'System Trace Audit Number 1..999999' />
	  ulong	regInvNumContext;		//invoice number (R.O.C.)' /> 
	  uchar	regBatNumContext;     //'Batch number' /> 
	  uint	regCurrIdxPtr;                               //'transaction log index storage for tracking previous last transaction. This applies in voided transactions'/>
	  uchar	regToneDetect[3+1]; 	   //'display ON/OFF for Tone Detect'/>
	  uchar	regTonePulse[5+1]; 		    //'display TONE or PULSE'/>
	  uchar	regDHCP[3+1]; 			 //'display ON/OFF DHCP'/>
	  uint 	regOfflineCnt;                             //'Number of Offline in application' /> 
	  ulong	regAutCodOff;                             //'Number of AutCode Offline in application' /> <!--mcy add-->
	  ulong	regTleCnt;                                  //'Encryption counter 0..999999' />
	  uint 	regIssTrxNo;                            //'iss transaction number' />
	  uint 	regPurchaseOfflineCnt;                             //'Number of Offline in application' /><!--ADD Chandra-->        
	  ulong	regErrTimeOutConn;  	                           //'Time Out Connecting - No Answer' />        
	  ulong	regErrTimeOutRecv;  	                           //'Time Out Receiving' />
	  ulong	regErrLineBusy;  		                           //'Line Busy' />
	  ulong	regErrNoCarrier;  		                           //'No Carrier' />
	  ulong	regErrUserCancel;  	                           //'User Cancel' />  
	  ulong	regErrLineDisconnect;  	                           //'Line Disconnect' />
	  uchar	regErrCurBat;				//Current Batch Number' />
}TLM_REG;
#endif //__TELIUM_PARAM__

#ifdef __TELIUM_PARAM__
///<Card Range multirecord
typedef struct _tagbinBeg
{
	uchar	binID; ///<Card Prefix ID
	uchar      binName[lenName+1]; ///<Card Range Name
       uchar     	binLo[1+lenBinPrefix]; ///<Low Prefix (BCD Format)
       uchar     	binHi[1+lenBinPrefix];///<High Prefix (BCD Format)
       uchar     	binCrIssID; ///<Credit Issuer ID
       uchar    	binCrAcqID; ///<Credit Acquirer ID
       uchar 	binPanLen; ///<Pan Length Set
       uchar 	binDbIssID; ///<Debit Issuer ID
       uchar 	binDbAcqID; ///<Debit Acquirer ID
       uchar 	binLogoID; ///<Logo ID to use
}TLM_BIN;

typedef struct _tagtlmbinBeg
{
	uchar	ucBinNum; ///<total bin
	TLM_BIN	Bin[dimBin];
}TLM_TMS_BIN;

#endif	//__TELIUM_PARAM__


#ifdef __TELIUM_PARAM__
//aid.xml
typedef struct _tagaidBeg	/// aid data
{
	uchar	emvAid[1+lenAID];
	uchar	emvTACDft[1+lenTVR];
	uchar	emvTACDen[1+lenTVR];
	uchar	emvTACOnl[1+lenTVR];
	uchar	emvThrVal[1+lenThrVal];
	uchar	emvTarPer[1+lenTarPer];
	uchar	emvMaxTarPer[1+lenMaxTarPer];
	uchar	emvDftValDDOL[1+lenDftValDDOL];
	uchar	emvDftValTDOL[1+lenDftValTDOL];
	uchar	emvTrmAvn[1+lenTrmAvn];
	uchar	emvAcqId[1+lenAcqId];
	uchar	emvTrmFlrLim[1+lenTrmFlrLim];
	uchar	emvTCC[1+lenTCC];
	uchar	emvAidTxnType[1+lenAIDTxnType];
	uchar	emvTrnTyp[1+lenTrnTyp];
}TLM_AID;	//aid param

typedef struct _tagtlmaidBeg	/// aid data
{
	uchar	ucAidNum;
	TLM_AID	Aid[dimAID];
}TLM_TMS_AID;	//aid param

#endif //__TELIUM_PARAM__

#ifdef __TELIUM_PARAM__
#define lenRid 		5
#define lenkeyData 	254
//pub.xml
typedef struct _tagpubBeg///<Certification Authority Public Keys
{
 	uchar	pubRid[1+lenRid]; ///<rid is the first 5 bytes of aid
       uchar     	pubIdx; ///<public key index, retrieved from the card
       uchar     	pubkeyData[1+lenkeyData]; ///<public key Data
       uchar     	pubExp; ///<exponent
       uchar    	pubAlgo; ///<public key Algorithm
}TLM_PUB;

typedef struct _tagtlmpubBeg///<Certification Authority Public Keys
{
 	uchar	ucPubNum;
	TLM_PUB	Pub[dimPub];
}TLM_TMS_PUB;

#endif //__TELIUM_PARAM__


#ifdef __TELIUM_PARAM__
//acq.xml
typedef struct _tagacqBeg///acquirer data
{
    	uchar		acqID; ///<Acquirer ID
       uchar		acqName[lenName+1]; ///<Acquirer Name

       uchar             acqPriTxnPhnSrv[lenSrv+1]; ///<Primary Transaction Phone Number
//       uchar		acqPriTxnSrv[lenSrv]; ///<Primary Transaction Server Phone Number or IP address/port
       uchar 		acqPriTxnAtt; ///<Primary Transaction Server Connect attempts
       
       uchar             acqSecTxnPhnSrv[lenSrv+1]; ///<Secondary Transaction Phone Number
//       uchar      	acqSecTxnSrv[lenSrv]; ///<Secondary Transaction Server Phone Number or IP address/port
       uchar      	acqSecTxnAtt; ///<Secondary Transaction Server Connect attempts

	uchar             acqPriStlPhnSrv[lenSrv+1]; ///<Primary Settle Phone Number
//       uchar             acqPriStlSrv[lenSrv]; ///<Primary Settle Phone Number
       uchar             acqPriStlAtt; ///<Primary Settle Phone Number Dial attempts

	uchar             acqSecStlPhnSrv[lenSrv+1]; ///<Secondary Settle Phone Number
//       uchar             acqSecStlSrv[lenSrv]; ///<Secondary Settle Phone Number
       uchar             acqSecStlAtt; ///<Secondary Settle Phone Number Dial attempts

	uchar             acqTxnDialOpt; ///<Transaction Dial Options
       uchar             acqStlDialOpt; ///<Settle Dial Options
       uchar             acqTxnOpt; ///<Transaction Option
       uchar             acqStlOpt; ///<Settle Option
       ushort		acqTxnRxTO; ///<Transaction Rx Timeout
       ushort            acqTxnICTO; ///<Transaction Inter Character Timeout
       ushort        	acqTxnConTO; ///<Connection Timeout
       ushort            acqStlRxTO; ///<Settle Rx Timeout
       ushort            acqStlICTO; ///<Settle Inter Character Timeout
       ushort        	acqStlConTO; ///<Connection Timeout
       uchar             acqOpt2; ///<Acquirer Option 2
       uchar             acqNII[lenNII+1]; ///<Acquirer NII
       uchar             acqTID[lenTID+1]; ///<Acquirer Terminal ID
       uchar             acqMID[lenMid+1]; ///<Acquirer Merchant ID
       uchar             acqCurBat[lenInvNum+1]; ///<Current Batch Number
//       uchar             acqPriTxnPhnSrv[lenSrv]; ///<Primary Transaction Phone Number
//       uchar             acqSecTxnPhnSrv[lenSrv]; ///<Secondary Transaction Phone Number
//       uchar             acqPriStlPhnSrv[lenSrv]; ///<Primary Settle Phone Number
//       uchar             acqSecStlPhnSrv[lenSrv]; ///<Secondary Settle Phone Number
       uchar             acqPriTxnIPSrv[lenSrv+1]; ///<Primary Transaction IP Server and Port
      	uchar             acqPriTxnIP[lenSrv+1]; ///<Primary Transaction IP Server and Port
      	uchar             acqPriTxnPort[6+1]; ///<Primary Transaction IP Server and Port
       
       uchar             acqSecTxnIPSrv[lenSrv+1]; ///<Secondary Transaction IP Server and Port
       uchar             acqSecTxnIP[lenSrv+1]; ///<Secondary Transaction IP Server and Port
       uchar             acqSecTxnPort[6+1]; ///<Secondary Transaction IP Server and Port

       uchar             acqPriStlIPSrv[lenSrv+1]; ///<Primary Settle IP Server and Port
       uchar             acqPriStlIP[lenSrv+1]; ///<Primary Settle IP Server and Port
       uchar             acqPriStlPort[6+1]; ///<Primary Settle IP Server and Port

       uchar             acqSecStlIPSrv[lenSrv+1]; ///<Secondary Settle IP Server and Port
       uchar             acqSecStlIP[lenSrv+1]; ///<Secondary Settle IP Server and Port
       uchar             acqSecStlPort[6+1]; ///<Secondary Settle IP Server and Port

       uchar             acqMasterKey1[lenMKey+1]; ///<1st 8 byte Master Key
       uchar             acqMasterKey2[lenMKey+1]; ///<2nd 8 byte Master Key
       uchar             acqWorkingKey[16+1]; ///<Working Key
       uchar             acqPinKey[16+1]; ///<Pin Key
       uchar             acqPendingSettle; ///<Flag Settlement Pending
       uchar             acqLogonFlag; ///<Flag Logon
       uchar             acqFlagCountTrx; ///<count trx
       uchar		ucKey;
       uchar		sIssuerKey[MAX_ISSUER];
	ulong		ulCurBatchNo;
	ulong		ulNextBatchNo;
	uchar		ucIndex;
}TLM_ACQ;

typedef struct _tagtlmacqBeg///acquirer data
{
    	uchar		ucAcqNum;
	TLM_ACQ		Acq[MAX_ACQ];
		
}TLM_TMS_ACQ;

#endif //__TELIUM_PARAM__

#ifdef __TELIUM_PARAM__
//iss.xml
typedef struct _tagissBeg///issuer data
{
	uchar            issID; ///<Issuer ID
	uchar            issRef[lenIssRef +1]; ///<Issuer Referral Number
	uchar            issOpt1; ///<Issuer Options 1
	uchar            issOpt2; ///<Issuer Options 2
	uchar            issOpt3; ///<Issuer Options 3
	uchar            issOpt4; ///<Issuer Options 4
	uchar            issDefAcct; ///<Issuer Default Account
	ushort           issFloorLimit; ///<Floor Limit
	uchar            issOpt5; ///<Issuer Options 5
	uchar            issPinOpt; ///<Pin option
	uchar            issAccSelect; 
	uchar            issEMVFallback; 
	uchar            issManual; 
	uchar            issExpiryDatReq; 
	uchar            issOffline; 
	uchar            issRefferalDial; 
	uchar            issProductCod; 
	uchar            issAdjust; 
	uchar            issDoModPAN; 
	uchar            issROCNumReq; 
	uchar            issPrintReceipt; 
	uchar            issCapTxns; 
	uchar            issCheckExpOff; 
	uchar            issRefund; 
	uchar            issPreauth; 
	uchar            issVoid; 
	uchar            issAdditionPrompts; 
	uchar            issCheckExpAll; 
	uchar            issCashOut; 
	uchar            issTrack1; 
	uchar            issBalance; 
	uchar            issErrorReceipt; 
	uchar            issOfflineROC; 
	uchar            issExtServiceCod; 
	uchar            issFullPAN; 
	uchar            issExpiryDate; 
	uchar            issManEntryCVV; 
	uchar	      ucAdjustPercent;
	uchar	      sOption[5];
}TLM_ISS;

typedef struct _tagtlmissBeg///issuer data
{
	uchar      	ucIssNum;
	TLM_ISS	 	Iss[dimIss];
}TLM_TMS_ISS;

#endif //__TELIUM_PARAM__

#ifdef __TELIUM_PARAM__
//tra.xml
typedef struct _tagtraBeg///tra
{
	uchar 		traSta;					//communication session state
	uchar		traTmsMreRec;			//TMS Msgs
	uchar		traTmsEmvFleNme[lenFleNme+1]; //TMS Emv File Name
	uchar		traTmsEmvFleHdl[lenFleHdl+1];	//TMS Emv File Handle
	ulong		traTmsEmvFleOff; 			//TMS Emv File Offset(size of the received file)
	ulong		traTmsEmvFleSze;			//TMS Emv File Size
	uchar 		traTmsMsg;			
	uchar		traBinReq;
	uchar 		traTrkPrepaidPrint[16+1];
	uchar		traHastPrepaid[16+1];
	uchar		traSAMTransactionReport[47+1];
	uchar		traTrkPrepaidSwipePrint[25+1];
	uchar		traCardTypeTBM[lenCardTransType+1];
	uchar		traTrkPrepaidTapPrint[20+1];
	uchar		traBlockPan[lenPan+1];
	uchar		traTrk2Prepaid[lenTrk2+1];

}TLM_TRA;
#endif //__TELIUM_PARAM__

#ifdef __TELIUM_PARAM__
enum eDnlSta {
    dnlBeg,                   //engine states: TMS parameters download
    dnlSendLoginReq,
    dnlRecvLoginRsp,
    dnlSendConTblReq,
    dnlRecvConTblRsp,
    dnlSendBinTblReq,
    dnlRecvBinTblRsp,
    dnlSendIssTblReq,
    dnlRecvIssTblRsp,
    dnlSendAcqTblReq,
    dnlRecvAcqTblRsp,
    dnlSendEmvReq,
    dnlRecvEmvRsp,
    dnlSendEmvFileReq,
    dnlRecvEmvFileRsp,
    dnlSendEmvFileOpnReq,
    dnlRecvEmvFileOpnRsp,
    dnlSendEmvFileRedReq,
    dnlRecvEmvFileRedRsp,
    dnlSendEmvFileClsReq,
    dnlRecvEmvFileClsRsp,
    dnlSendLogOffReq,
    dnlRecvLogOffRsp,

    dnlEnd
};
#endif //__TELIUM_PARAM__


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////END OF TELIUM/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

// EDC 选项访问宏定义(Protims端可设置)
#if 0	//closed by irf, 20121203
#define EDC_DATE_FORMAT				0x0040
#define EDC_PRINT_TIME				0x0020
#define EDC_TIP_PROCESS				0x0004
#define EDC_NUM_PRINT_HIGH			0x0180
#define EDC_NUM_PRINT_LOW			0x0140
#define EDC_USE_PRINTER				0x0102
#define EDC_ENABLE_INSTALMENT		0X0280
#define EDC_AUTH_PREAUTH			0x0220
#define EDC_CASH_PROCESS			0x0210
#define EDC_ECR_ENABLE				0x0208
#define EDC_NOT_ADJUST_PWD			0x0320
#define EDC_NOT_MANUAL_PWD			0x0310
#define EDC_NOT_SETTLE_PWD			0x0308
#define EDC_NOT_REFUND_PWD			0x0304
#define EDC_NOT_VOID_PWD			0x0302
#define EDC_NOT_KEYBOARD_LOCKED		0x0301
#define EDC_SELECT_ACQ_FOR_CARD		0x0480
#define EDC_ENABLE_DCC				0x0440
#define EDC_REFERRAL_DIAL			0x0410
#define EDC_LONG_SLIP				0x0408
#define EDC_AUTH_PAN_MASKING		0x0404
#define EDC_FREE_PRINT				0X0402
#else	//modified by irf, 20121203
//option 1
#define EDC_AMOUNT_DUAL_ENTRY			0x0001		//not used
#define EDC_DISPLAY_TRACK_INFO			0x0002		//not used
#define EDC_TIP_PROCESS					0x0004		//used
#define EDC_USER_ACCOUNTING				0x0008		//not use
#define EDC_LODGING							0x0010
#define EDC_PRINT_TIME						0x0020
#define EDC_DATE_FORMAT					0x0040
#define EDC_CONFIRM_TOTAL					0x0080
//option 2
#define EDC_TRACK							0x0101		//0= track 1, 1=track 2
#define EDC_USE_PRINTER					0x0102
#define EDC_PRINT_REFERENCE_NUM			0x0104
#define EDC_DATE_PROMPT_AT_SETTLE		0x0108
#define EDC_WHOLE_SETTLE					0x0110
#define EDC_BASE_TOTAL_ENTRY				0x0120
#define EDC_NUM_PRINT_LOW					0x0140		//used
#define EDC_NUM_PRINT_HIGH				0x0180		//used
//option 3
#define EDC_ECR_ENABLE						0x0208
#define EDC_CASH_PROCESS					0x0210		//used
#define EDC_AUTH_PREAUTH					0x0220		//used
#define EDC_ENABLE_INSTALMENT				0X0280		//used
//PwdMask	0=not required, 1=required
#define EDC_NOT_KEYBOARD_LOCKED			0x0301		//used
#define EDC_NOT_VOID_PWD					0x0302
#define EDC_NOT_REFUND_PWD				0x0304
#define EDC_NOT_SETTLE_PWD				0x0308
#define EDC_NOT_MANUAL_PWD				0x0310
#define EDC_NOT_ADJUST_PWD				0x0320
//DialOption
#define EDC_FREE_PRINT						0X0402
#define EDC_AUTH_PAN_MASKING				0x0404
#define EDC_LONG_SLIP						0x0408
#define EDC_REFERRAL_DIAL					0x0410
#define EDC_ENABLE_DCC						0x0440
#define EDC_SELECT_ACQ_FOR_CARD			0x0480
#endif
// EDC 扩展选项访问宏定义(Protims端不可设置，必须在POS上设定)
//...

// 发卡行选项访问宏定义(Protims端可设置)
#define ISSUER_EN_ADJUST				0x0080
#define ISSUER_EN_DISCRIPTOR 			0x0040
#define ISSUER_EN_VOICE_REFERRAL		0x0020
#define ISSUER_EN_OFFLINE				0x0010
#define ISSUER_EN_EXPIRY				0x0008
#define ISSUER_EN_MANUAL				0x0004
#define ISSUER_EN_PIN					0x0002
#define ISSUER_EN_ACCOUNT_SELECTION		0x0001
#define ISSUER_NO_PREAUTH				0x0180
#define ISSUER_NO_REFUND				0x0140
#define ISSUER_CHECK_EXPIRY_OFFLINE		0x0110
#define ISSUER_CAPTURE_TXN				0x0108
#define ISSUER_EN_PRINT					0x0104
#define ISSUER_ROC_INPUT_REQ			0x0102
#define ISSUER_CHKPAN_MOD10				0x0101
#define ISSUER_CHKPAN_MOD11				0x0280
#define ISSUER_CHECK_EXPIRY				0x0210
#define ISSUER_AUTH_CODE				0x0208
#define ALLOW_EXTEND_PAY				0x0204
#define ISSUER_ADDTIONAL_DATA			0x0202
#define ISSUER_NO_VOID					0x0201
#define ISSUER_SECURITY_MANUL			0x0380
#define ISSUER_SECURITY_SWIPE			0x0340
#define ISSUER_MASK_EXPIRY				0x0302
#define ISSUER_EN_BALANCE				0x0301
#define ISSUER_EN_EMVPIN_BYPASS		ISSUER_EN_BALANCE	// or use other option instead, as long as it is not in used.

// 收单行选项访问宏定义(Protims端可设置)
#define ACQ_VISA_CHOICE						0x0080
#define ACQ_PRINT_AUDIT_TRAIL				0x0040
#define ACQ_EXPRESS_PAYMENT_PROMPT			0x0020
#define ACQ_DISABLE_BUSINESS_DATE			0x0010
#define ACQ_IGNORE_HOST_TIME				0x0008
#define ACQ_REQ_OPEN_SUSPEND_BATCH			0x0004
#define ACQ_CLEAR_BATCH						0x0002
#define ACQ_AMEX_SPECIFIC_FEATURE			0x0001
#define ACQ_PRINT_AUTH_CODE					0x0120
#define ACQ_DISABLE_TRICK_FEED				0x0110
#define ACQ_BLOCK_AUTH_FOR_OFFLINE			0x0108
#define ACQ_USE_TRACK3						0x0104
#define ACQ_NOT_PRINT_RRN					0x0102
#define ACQ_ADDTIONAL_PROMPT				0x0101
#define ACQ_CITYBANK_INSTALMENT_FEATURE		0x0240
#define ACQ_DBS_FEATURE						0x0220
#define ACQ_BOC_INSTALMENT_FEATURE			0x0210
#define ACQ_ONLINE_VOID						0x0208
#define ACQ_ONLINE_REFUND					0x0204
#define ACQ_AIR_TICKET						0x0201
#define ACQ_EMV_FEATURE						0x0202
#define ACQ_ASK_CVV2						0x0380

// To keep compatible with S series. Copied from posapi_s80.h
#if defined(_P70_S_) || defined(_P60_S1_)
// Char set
#define CHARSET_WEST        0x01      //美国、英国及西欧国家        
#define CHARSET_TAI         0x02      //泰国                        
#define CHARSET_MID_EUROPE  0x03      //中欧                           
#define CHARSET_VIETNAM     0x04      //越南                           
#define CHARSET_GREEK       0x05      //希腊                           
#define CHARSET_BALTIC      0x06      //波罗的海                       
#define CHARSET_TURKEY      0x07      //土耳其                         
#define CHARSET_HEBREW      0x08      //希伯来                          
#define CHARSET_RUSSIAN     0x09      //俄罗斯                        
#define CHARSET_GB2312      0x0A      //简体中文      
#define CHARSET_GBK         0x0B      //简体中文     
#define CHARSET_GB18030     0x0C      //简体中文        
#define CHARSET_BIG5        0x0D      //繁体中文        
#define CHARSET_SHIFT_JIS   0x0E      //日本                          
#define CHARSET_KOREAN      0x0F      //韩国                           
#define CHARSET_ARABIA      0x10      //阿拉伯	                          
#define CHARSET_DIY         0x11      //自定义字体
#endif

// Support right-to-left language
#define LANGCONFIG glSysParam.stEdcInfo.stLangCfg.szDispName

// !!!! 新增
typedef struct _tagIP_ADDR
{
	uchar		szIP[15+1];
	uchar		szPort[5+1];
}IP_ADDR;

// 终端存储EDC信息
typedef struct _tagEDC_INFO
{
#define EDC_INIT_OK		0x5A5A
	ushort	uiInitFlag;				// 0x5A5A OK
	uchar	szInitTime[14+1];		// YYMMDDhhmmss
	uchar	sInitSTAN[3];			// to be sync to SYS_CONTROL

	uchar	ucDllTracking;			// reserved
	uchar	bClearBatch;			// 0-none, 1-clear former batch
	uchar	ucPrinterType;			// 0-Sprocket, 1-Thermal
	uchar	ucEcrSpeed;				// ECR speed, reserved ?
	uchar	szHelpTelNo[24+1];		// for help function
	uchar	sOption[5];				// option for access
									// sOption[0] = ucOption1
									// sOption[1] = ucOption2
									// sOption[2] = ucOption3
									// sOption[3] = ucPwdMask
									// sOption[4] = ucDialOption

	uchar	sExtOption[8];			// extended option. this option cannot be controlled by Protims.

	uchar	sReserved[4];			// sReserved[0] = ucUnused1
									// sReserved[1..3] = sUnused2

	uchar	szMerchantAddr[46+1];	// line 2 and 3 on invoice
//	uchar	szMerchantAddr[69+1];	// line 2, 3 and 4 on invoice	//modified by irf, 20121204
	uchar	szMerchantName[23+1];	// merchant name,line 1
	uchar	szAddlPrompt[20+1];		// for show and print

	//uchar	ucLanguage;				// 0-english,1-spanish,2-portuguese

	CURRENCY_CONFIG	stLocalCurrency;	// local currency name, code, decimal position, ignore digits.
	uchar	ucCurrencySymbol;
	uchar	ucTranAmtLen;			// decimal value
	uchar	ucStlAmtLen;			// decimal value

	ulong	ulOfflineLimit;			// 磁卡Floor limit

	uchar	ucScrGray;				// LCD亮度, 默认为4
	uchar	ucAcceptTimeout;		// 显示交易成功信息的时间
	uchar	szPabx[10+1];			// 外线号码
	uchar	szDownTelNo[25+1];		// 下载电话号码
	uchar	szDownLoadTID[8+1];		// 下载参数终端号
// 	uchar	szDownLoadMID[15+1];	// 下载参数商户号 (reserved)
	uchar	szTMSNii[3+1];			// 下载参数的NII
	uchar	ucTMSTimeOut;			// 下载参数通讯超时
	uchar	szDownTelPriNo[25+1];		// Add By Zaki 25 April 2013
	uchar	szDownTelSecNo[25+1];		// Add By Zaki 25 April 2013
	IP_ADDR	stDownIPPriNo;		// Add By Zaki 26 April 2013
	IP_ADDR	stDownIPSecNo;		// Add By Zaki 26 April 2013
	IP_ADDR	stDownIpAddr;			// 下载参数的远程IP地址

	uchar	bPreDial;				// TRUE: enable pre-dial for modem

	// 以下为ProTims自动更新的参数, 暂时可以不需要
	uchar	szCallInTime[8+1];		// start(HHMM)+end(hhmm)
	uchar	ucAutoMode;				// 0 none, 1 auto, 2 call in
	uchar	szAutoDayTime[10+1];	// YYMMDDHHmm
	ushort	uiAutoInterval;			// unit: day

	LANG_CONFIG	stLangCfg;			// current using language
	uchar	ucPedMode;				// Current using PED (PxxPED/SxxPED/PP/ExtSxxPED)
	uchar	ucIdleMinute;			// Power Save timeout
	uchar	ucIdleShutdown;			// Shutdown when idle for [ucIdleMinute] minutes have passed.
	CicilanInfo 		CicilanInfoBukopin[7];
}EDC_INFO;

#ifdef __TELIUM_PARAM__

typedef struct _tagpax_sys_param
{
//pax param
#define LOGON_MODE		0x01
#define CHANGE_MODE 	0x02
#define TRANS_MODE		0x04
#define INIT_MODE		0x08
	uchar				ucTermStatus;		

#define _TxnPSTNPara	stTxnCommCfg.stPSTNPara
#define _TxnModemPara	stTxnCommCfg.stPSTNPara.stPara
#define _TxnRS232Para	stTxnCommCfg.stRS232Para
#define _TxnTcpIpPara	stTxnCommCfg.stTcpIpPara
#define _TxnWirlessPara	stTxnCommCfg.stWirlessPara
	COMM_CONFIG			stTxnCommCfg;		

#define _TmsPSTNPara	stTMSCommCfg.stPSTNPara
#define _TmsModemPara	stTMSCommCfg.stPSTNPara.stPara
#define _TmsRS232Para	stTMSCommCfg.stRS232Para
#define _TmsTcpIpPara	stTMSCommCfg.stTcpIpPara
#define _TmsWirlessPara	stTMSCommCfg.stWirlessPara
	COMM_CONFIG			stTMSCommCfg;		

	EDC_INFO			stEdcInfo;			// 

	uchar				ucNewTMS;			
	uchar				ucTMSSyncDial;		

	uchar				bTextAdData;		// False: bitmap TRUE: text
	uchar				sAdData[LEN_MAX_AD_DATA];	

	uchar				sTermInfo[HWCFG_END];	// Terminal hardware infomation. for GetTermInfo() use.
#define APMODE_INDEPEND	0		// Current app is app manager.   
#define APMODE_MAJOR		1		// Current app is major sub-app. (EDC for VISA MASTERCARD)
#define APMODE_MINOR		2		// Current app is minor sub-app. (EDC for AE, DINERS, JCB)
	uchar				ucRunMode;

}PAX_SYS_PARAM;

//app.xml
typedef struct _tagappTMSBeg	///app data
{
	uchar	appAmtFmt;
	uchar	appDefTra;
	uchar	appAdmPwd[lenPwd+1];
	uchar	appVoidPwd[lenPwd+1];
	uchar	appSetlPwd[lenPwd+1];
	uchar	appRefPwd[lenPwd+1];
	uchar	appVendorID[8+1];
	uchar	appLtmkAcqID[3+1];
	uchar	appExponent[6+1];
	uchar	appNIIMntr[3+1];
	uchar	appTermOpt1;
	uchar	appTermOpt2;
	uchar	appTermOpt3;
	uchar	appTermOpt4;
	uchar	appTermOpt5;
	uchar	appPinSamCard[24+1];
	uchar	rptHdrSiteName[24+1];
	uchar	rptHdrSiteAddr1[24+1];
	uchar	rptHdrSiteAddr2[24+1];
	uchar	rptHdrSiteAddr3[24+1];
	uchar	appLocOpt;
	uchar	appCurrSign[3+1];
	uchar	appServiceCode;
	uchar	appEMSale;
	uchar	appEMRefund;
	uchar	appEMOffline;
	uchar	appEMCash;
	uchar	appEMPreAuth;
	uchar	appEMCrdVrfy;
	uchar	appEMBalance;
	uchar	appRekMerchant[76+1];
	uchar	appTimeSet[4+1];
//pax param
	PAX_SYS_PARAM	pax;
//end
}TLM_APP_TMS;


//app.xml
typedef struct _tagTlmCon	///app data
{
	uchar	appAmtFmt;
	uchar	appDefTra;
	uchar	appAdmPwd[lenPwd+1];
	uchar	appVoidPwd[lenPwd+1];
	uchar	appSetlPwd[lenPwd+1];
	uchar	appRefPwd[lenPwd+1];
	uchar	appVendorID[8+1];
	uchar	appLtmkAcqID[3+1];
	uchar	appExponent[6+1];
	uchar	appNIIMntr[3+1];
	uchar	appTermOpt1;
	uchar	appTermOpt2;
	uchar	appTermOpt3;
	uchar	appTermOpt4;
	uchar	appTermOpt5;
	uchar	appPinSamCard[24+1];
	uchar	rptHdrSiteName[24+1];
	uchar	rptHdrSiteAddr1[24+1];
	uchar	rptHdrSiteAddr2[24+1];
	uchar	rptHdrSiteAddr3[24+1];
	uchar	appLocOpt;
	uchar	appCurrSign[3+1];
	uchar	appServiceCode;
	uchar	appEMSale;
	uchar	appEMRefund;
	uchar	appEMOffline;
	uchar	appEMCash;
	uchar	appEMPreAuth;
	uchar	appEMCrdVrfy;
	uchar	appEMBalance;
	uchar	appRekMerchant[76+1];
	uchar	appTimeSet[4+1];
}TLM_CON;


//app.xml
typedef struct _tagappBeg	///app data
{
	uchar	appKeySessionTle[16];
	uchar	appTidTle[lenTid+1];
	uchar	appLTMKDATA[112+1];
	uchar	appNiiLTMK[3+1];
	uchar	appLTMK[48+1];
	uchar	appLTWKTID1[104+1];
	uchar 	appFlagDownloadLTWK;
	uchar	appFlagKEKSAM;
	uchar	appChipUIDSAM[33+1];
	uchar	appCIN[3+1];
	uchar	appCHIP_UID[7+1];
	uchar	appDATESAM[2+1];
	uchar	appSEQ_NO[4+1];
	uchar	appStartTimerWorkingKey[14+1];
	
}TLM_APP;


#endif //__TELIUM_PARAM__

// 卡表信息
typedef struct _tagCARD_TABLE
{
	uchar	sPanRangeLow[5];		// 卡号起始号
	uchar	sPanRangeHigh[5];		// 卡号终止号
	uchar	ucIssuerKey;			// 卡表指向的发卡表号
	uchar	ucAcqKey;				// 卡表指向的收单表号
	uchar	ucPanLength;			// 卡号长度, 00为不检查长度
	uchar	ucOption;				// b1-allow payment
}CARD_TABLE;

// 发卡行信息
typedef struct _tagISSUER
{
	uchar	ucKey;					// 表号
	uchar	szName[10+1];			// 发卡行名称
	uchar	szRefTelNo[12*2+1];		// phone # for voice referral

	// sOption[0]:
		// b7-allow adjust, b6-descriptor required
		// b5-voice referral allowed, b4-allow offline
		// b3-expiry required, b2-allow manual pan,
		// b1-pin required, b0-account type selection
		// sOption[1]:
		// b7-inhibit pre-auth, b6-inhibit refund
		// b4-check expiry for offline
		// b3-capture txn, b2-print
		// b1-roc input required, b0-pan MOD 10 check
	// sOption[2]:
		// b7-pan Robinson's MOD 11 check, b4-check expiry
		// b3-omit auth_code display and input for offline
		// b2-extended pay, b1-additional data allowed
		// b0-inhibit void
	// sOption[3]:
		// b6-security code entry
	uchar	sOption[4];

	uchar	ucDefAccount;		// reserved ?
	uchar	sPanMask[3];		// for print slips
								// sPanMask[0]=sReserved[0]
								// sPanMask[1]=sPanMask[1]
								// sPanMask[2]=sPanMask[0]
	ulong	ulFloorLimit;		// unit: cent
	uchar	ucAdjustPercent;
	uchar	ucReserved;			// sReserved[1]
}ISSUER;

typedef struct _tagPHONE_INFO
{
	uchar	szTelNo[12*2+1];
	uchar	ucDialWait;			// by second, time wait after fail
	uchar	ucDialAttempts;		// dial attempt times
}PHONE_INFO;

typedef struct _tagPPP_INFO
{
	uchar	szTelNo[12*2+1];
	uchar	szUserName[20+1];
	uchar	szUserPWD[20+1];
	uchar	szIPAddr[15+1];
	uchar	szIPPort[5+1];
	uchar	ucTimeOut;		//by second, time to wait until failure
}PPP_INFO;

// 收单行信息
typedef struct _tagACQUIRER
{
//	uchar	ucStatus;	//(移到SYS_CONTROL)
	uchar	ucKey;						// 表号
	uchar	szPrgName[10+1];			// 别名？主机名称？reserved?
	uchar	szName[10+1];				// 收单行名称
	uchar	sIssuerKey[MAX_ISSUER];		// 发卡行信息
	uchar	ucIndex;					// 该收单行的数组下标

#define TxnTelNo1	stTxnPhoneInfo[0].szTelNo
#define TxnTelNo2	stTxnPhoneInfo[1].szTelNo
#define StlTelNo1	stStlPhoneInfo[0].szTelNo
#define StlTelNo2	stStlPhoneInfo[1].szTelNo
	PHONE_INFO	stTxnPhoneInfo[2];
	PHONE_INFO	stStlPhoneInfo[2];
	uchar		ucPhoneTimeOut;			// timeout for wait resp

#define TxnTcpIp1	stTxnTCPIPInfo[0].szIP
#define TxnTcpPort1	stTxnTCPIPInfo[0].szPort
#define TxnTcpIp2	stTxnTCPIPInfo[1].szIP
#define TxnTcpPort2	stTxnTCPIPInfo[1].szPort
#define StlTcpIp1	stStlTCPIPInfo[0].szIP
#define StlTcpPort1	stStlTCPIPInfo[0].szPort
#define StlTcpIp2	stStlTCPIPInfo[1].szIP
#define StlTcpPort2	stStlTCPIPInfo[1].szPort
	IP_ADDR		stTxnTCPIPInfo[2];
	IP_ADDR		stStlTCPIPInfo[2];
	uchar		ucTcpTimeOut;

#define TxnPPPIp1	stTxnPPPInfo[0].szIP
#define TxnPPPPort1	stTxnPPPInfo[0].szPort
#define TxnPPPIp2	stTxnPPPInfo[1].szIP
#define TxnPPPPort2	stTxnPPPInfo[1].szPort
#define StlPPPIp1	stStlPPPInfo[0].szIP
#define StlPPPPort1	stStlPPPInfo[0].szPort
#define StlPPPIp2	stStlPPPInfo[1].szIP
#define StlPPPPort2	stStlPPPInfo[1].szPort	
	PPP_INFO	stTxnPPPInfo[2];
	PPP_INFO	stStlPPPInfo[2];
	uchar		ucPppTimeOut;

#define TxnGPRSIp1		stTxnGPRSInfo[0].szIP
#define TxnGPRSPort1	stTxnGPRSInfo[0].szPort
#define TxnGPRSIp2		stTxnGPRSInfo[1].szIP
#define TxnGPRSPort2	stTxnGPRSInfo[1].szPort
#define StlGPRSIp1		stStlGPRSInfo[0].szIP
#define StlGPRSPort1	stStlGPRSInfo[0].szPort
#define StlGPRSIp2		stStlGPRSInfo[1].szIP
#define StlGPRSPort2	stStlGPRSInfo[1].szPort
	IP_ADDR		stTxnGPRSInfo[2];
	IP_ADDR		stStlGPRSInfo[2];	// Since the APN,UID,PWD,SIMPIN are shared, here only store IP,Port for acquirer.
	uchar		ucGprsTimeOut;

	uchar	ucTxnModemMode;		// reserved?
								// 0-bell 1200 sdlc,1-bell 1200 async e71
								// 2-ccitt 1200 sdlc
								// 3-ccitt 1200 async e71,4-bell 300 async e71
								// 5-ccitt 300 async e71,6-ccitt 2400 sdlc,
								// 7-ccitt 2400 async e71,20-ccitt 1200 etsl
	uchar	ucStlModemMode;		// see ucTxnModemMode

	uchar	sCommTypes[8];		// No actual use

	// sOption[0]:
		// b7-visa choice
		// b6-print audit trail after settle
		// b5-express payment prompt
		// b4-disable business date
		// b3-ignore host time
		// b2-request open suspended batch
		// b1-clear batch after initial
		// b0-amex specific feature
	// sOption[1]:
		// b5-prompt auth_code on receipt 1
		// b4-disable trick feed
		// b3-block auth prompt for offline
		// b2-use track
		// b1-no rrn on receipt
		// b0-additional prompt
	// sOption[2]:
		// b3-online void, b2-online refund
	// sOption[3]: // unused
	uchar	sOption[4];

	uchar	szNii[3+1];
	uchar	szTermID[8+1];
	uchar	szMerchantID[15+1];
	ulong	ulCurBatchNo;
	ulong	ulNextBatchNo;
	uchar	szVisa1TermID[23+1];		// reserved
	uchar	sReserved[4];				// reserved
}ACQUIRER;

// 分期付款计划信息
typedef struct _tagINSTALMENT_PLAN
{
	uchar	ucIndex;		// 1~MAX_PLAN
	uchar	ucAcqIndex;
	uchar	szName[7+1];
	uchar	ucMonths;
	ulong	ulBottomAmt;	// 最低金额
}INSTALMENT_PLAN;

// 商品描述信息
typedef struct _tagDESCRIPTOR
{
	uchar	ucKey;
	uchar	szCode[2+1];		// ANS format
	uchar	szText[20+1];		// for display and print
}DESCRIPTOR;

// 详细卡表信息(RFU for HK)
#define LEN_MAX_CARBIN_NAME		30
typedef struct _tagISSUER_NAME
{
	uchar	szChineseName[16+1];
	uchar	szEnglishName[LEN_MAX_CARBIN_NAME+1];
}ISSUER_NAME;

typedef struct _tagCARD_BIN
{
	uchar	ucIssuerIndex;		// 与IssuerName关联的索引号
	uchar	ucPanLen;			// 卡号长度(reserved)
	uchar	ucMatchLen;			// 匹配时候的长度
	uchar	sStartNo[10];
	uchar	sEndNo[10];
}CARD_BIN;

// 自动下载参数信息(RFU for HK),用于传递参数给Manager
typedef struct _tagEDC_DOWN_PARAM
{
	uchar	szPabx[10+1];
	uchar	szTermID[8+1];
	uchar	szCallInTime[8+1];		// HHMM-hhmm
	// EDC允许被叫的起止时间,24时制
	uchar	bEdcSettle;				// 0: not settle 1: settle
	uchar	ucAutoMode;				// 0 none, 1 auto, 2 call in
	uchar	szAutoDayTime[10+1];	// YYMMDDHHmm
}EDC_DOWN_PARAM;

// 与管理器相关数据结构
// EPS系统参数(与旧系统兼容,命名规则与旧系统兼容)
#define EPS_INITIALIZED			0x55
#define EPS_NOT_INITIALIZED		0x00
#define EPS_TERMINAL_LOCKED		0xAA
#define EPS_TERMINAL_UNLOCKED	0x00
typedef struct _tagEPS_MULTISYS_PARAM
{
	uchar	HYear;			// BCD, for display year with yyyy format

	/* here is only high 2 bytes of yyyy */
	uchar	MerchantPWD[4];			/* ASCII (4 bytes only) */
	uchar	TerminalPWD[4];			/* ASCII (4 bytes only) */
	uchar	BANKPWD[6];				/* ASCII (6 bytes only) */
	uchar	PABX[8];					/* ASCII (8 bytes only) */
	uchar	TelNo[2][25];				/* ASCII (25 bytes only)*2 */
	uchar	Nii[2];					/* BCD */
	uchar	MerchantName[41];		/* ASCII(40 bytes+\x00) */
	uchar	MerchantNo[10];			/* ASCII(9 bytes+\x00) */
	uchar	StoreNo[4];				/* ASCII(3 bytes+\x00) */
	uchar	TerminalNo[4];			/* ASCII(3 bytes+\x00) */
	//----------------------------------------------------
	//BYTE	CurrencySymbol[5][5];	/* for multi-currency, but eps use HKD(..[0][]) noly now. */
	uchar	TXNCode[4];  //for multi-currency, DEFAULT:000
	uchar	CurrencySymbol[4];  //for multi-currency, DEFAULT: HKD
	uchar	CurrencyCode[4];		//for multi-currency, DEFAULT: 344
	//----------------------------------------------------
	uchar	DailMode;				/* 0-sync,1-async*/
	uchar	SSETUP;					/* 0x05-SYN, 0x82-ASYN */
	uchar	AsMode;					/* 0x00-SYN, 0x04 -ASYN */
	uchar	DP;						/* 0x00 */
	uchar	CHDT;					/* 0x00 */
	uchar	DT1;					/* 0x05 */
	uchar	DT2;					/* 0x07 */
	uchar	HT;						/* 70 */
	uchar	WT;						/* 5 */
	uchar	DTIMES;					/* 1 */
	uchar	TimeOut;				/* 0x00 */
	uchar	TerminalLocked;			/* 0-unlocked, 1-locked */
	uchar	TerminalContrast;
	uchar	InitialFlag;

	ushort	AnswerToneWait;
	ushort	RedialWait;
	ushort	WelcomeWait;
	ushort	NextTxnWait;
	ushort	DebitRespWait;
	ushort	NonEPSDebitRespWait;
	ushort	AdmRespWait;
	ushort	ReverseRetryWait;
	ushort	T9Wait;
	ushort	T10Wait;

	uchar	ScrambleTAB1[91];
	uchar	ScrambleTAB2[91];
	uchar	KeyinLimitChar[20];
	uchar	TransType;			// 0-both, 1-eps, 2-noneps
} EPS_MULTISYS_PARAM;

// 交易配置信息
typedef struct _tagTRAN_CONFIG
{
	uchar	szLabel[16+1];		// 显示的交易类型
	uchar	szTxMsgID[4+1];		// 交易上送报文信息
	uchar	szProcCode[6+1];	// 交易处理码前面两位


#define PRN_RECEIPT			0x80
#define WRT_RECORD			0x40
#define IN_SALE_TOTAL		0x20
#define IN_REFUND_TOTAL		0x10
#define VOID_ALLOW			0x08
#define NEED_REVERSAL		0x04
#define ACT_INC_TRACE		0x02
#define NEED_LOGON			0x01
#define NO_ACT				0x00
uchar	ucTranAct;		// 该交易需要的功能

#define PRN_RECEIPT2		0x80
#define IN_BRIZZI_REDEEM_TOTAL			0x40
#define IN_BRIZZI_REAKTIVASI_TOTAL		0x20
#define IN_BRIZZI_VOID_TOTAL				0x10
#define IN_BRIZZI_AKT_DEPOSIT_TOTAL		0x08
#define IN_BRIZZI_TOPUP_DEP_TOTAL		0x04
#define IN_BRIZZI_TOPUP_ONL_TOTAL		0x02
#define IN_BRIZZI_OFFLINE_SALE_TOTAL		0x01
#define NO_ACT2				0x00
uchar	ucTranAct2;		// 该交易需要的功能
}TRAN_CONFIG;

// 选项参数信息
typedef struct _tagOPTION_INFO
{
	void	*pText;				// 显示的提示
	ushort	uiOptVal;			// 位置。0304表示 byte[3]的0x04位置
	uchar	ucInverseLogic;		// 是否反向逻辑。例如 EDC_NOT_SETTLE_PWD
	uchar	ucPermissionLevel;	// 进行此修改所需的用户权限
}OPTION_INFO;

typedef struct _tagHOST_ERR_MSG
{
	uchar	szRspCode[2+1];		// 主机返回码
	uchar	szMsg[16+1];
}HOST_ERR_MSG;

typedef struct _tagTERM_ERR_MSG
{
	int		iErrCode;
	uchar	szMsg[16+1];
}TERM_ERR_MSG;

//add cung	BRI
//=========================================================================================================
typedef struct _tagCARDSERVIS
{
	uchar	NoKartu[19+1];
	uchar	NoRek[15+1];		
	uchar	Nama[30+1];
}CARDSERVIS;

typedef struct _tagREGISTRASI_SMSBANKING
{
	uchar	NoHP[13+1];
}REGISTRASI_SMSBANKING;

typedef struct _tagREGISTRASI_IBANKING
{
	uchar	ID[10+1];
}REGISTRASI_IBANKING;

typedef struct _tagREGISTRASI_TRXPB
{
	uchar	NoRek[15+1];
	uchar	Nama[30+1];
	uchar	NoHP[15+1];
}REGISTRASI_TRXPB;


typedef struct _tagTRANSFER_SESAMABRI
{
	uchar	NoRek[15+1];
	uchar	Nama[30+1];
	uchar	Total[12+1];
}TRANSFER_SESAMABRI;


typedef struct _tagTUNAI
{
	uchar	NoRek[30+1];
	uchar	Nama[30+1];
	uchar	Saldo[16+1];
	uchar	Fee[16+1];
	uchar	NominalTarik[16+1];
}TUNAI;

typedef struct _tagCASHIN
{
	uchar	noHP[20+1];
	uchar	Fee[12+1];
}CASHIN;

typedef struct _tagCASHOUT
{
	uchar	noHP[20+1];
	uchar	Saldo[12+1];
	uchar	Fee[12+1];
}CASHOUT;

/*
typedef struct _tagTRANSFER_ANTARBANK
{
	uchar	JenisRekening[13+1];
	uchar	Jalur[15+1];
	uchar	BankPengirim[15+1];
	uchar	NamaPengirim[30+1];
	uchar	NoRekPengirim[20+1];

	uchar	BankPenerima[15+1];
	uchar	NamaPenerima[30+1];
	uchar	NoRekPenerima[20+1];
	uchar	NoRefPenerima[16+1];
	uchar	Total[12+1];
}TRANSFER_ANTARBANK;
*/
// DIKI EDIT FOR MINI ATM BUKOPIN
typedef struct _tagTRANSFER_ANTARBANK
{
	uchar	JenisRekening[35+1];
	uchar	Jalur[30+1];
	uchar	BankPengirim[20+1];
	uchar	NamaPengirim[30+1];
	uchar	NoRekPengirim[28+1];

	uchar	BankPenerima[20+1];
	uchar	NamaPenerima[30+1];
	uchar	NoRekPenerima[28+1];
	uchar	NoRefPenerima[16+1];
	uchar	Total[14+1];
}TRANSFER_ANTARBANK;


typedef struct _tagPEMBAYARAN_TELKOM
{
	uchar	Nama[30+1];
	uchar	NoTelepon[13+1];
	uchar	Total[14+1];
	uchar	NoRek[20+1];
	uchar	PayPeriod[20+1];
}PEMBAYARAN_TELKOM;

typedef struct _tagPEMBAYARAN_KARTUHALO
{
	uchar	NoHP[13+1];
	uchar	Nama[30+1];
	uchar	Total[12+1];
}PEMBAYARAN_KARTUHALO;

typedef struct _tagPEMBAYARAN_KARTUMATRIX
{
	uchar	NoHP[13+1];
	uchar	Nama[30+1];
	uchar	Total[12+1];
	uchar	Provider[20+1];
	uchar	NoRek[20+1];
	uchar	BulanTahun[20+1];
	uchar	PayPeriod[20+1];
}PEMBAYARAN_KARTUMATRIX;

typedef struct _tagPEMBAYARAN_PLNPASCABAYAR
{
	uchar	NoIDPel[12+1];
	uchar	Nama[30+1];
	uchar	TarifDaya[14+1];
	uchar	Bulan[23+1];
	uchar	StandMeter[17+1];
	uchar	RegTag[12+1];
	uchar	Total[12+1];
	uchar	MLPO[32+1];
	uchar	Telepon[15+1];
	uchar	Tunggakan[2+1];
}PEMBAYARAN_PLNPASCABAYAR;

typedef struct _tagPEMBAYARAN_PLNPRABAYAR
{
	uchar	NoMeter[11+1];
	uchar	NoIDPel[12+1];
	uchar	Nama[30+1];
	uchar	TarifDaya[14+1];
	uchar	MLPO[32+1];
	uchar	Bayar[12+1];
	uchar	Materai[9+1];
	uchar	PPN[9+1];
	uchar	PPJ[9+1];
	uchar	Angsuran[9+1];
	uchar	RPToken[11+1];
	uchar	KWH[10+1];
	uchar	Token[20+1];
	uchar	Telepon[12+1];
	uchar	RefNum[12+1];
}PEMBAYARAN_PLNPRABAYAR;

typedef struct _tagPEMBAYARAN_KARTUKREDIT_BRI
{
	uchar	Nomor[16+1];
	uchar	Tagihan[12+1];
	uchar	Nama[30+1];
	uchar	MinBayar[12+1];
	uchar	DueDate[8+1];
	uchar	Total[12+1];
	uchar	Tampilan[24+1];
}PEMBAYARAN_KARTUKREDIT_BRI;

typedef struct _tagPEMBAYARAN_KARTUKREDIT_KTA
{	uchar 	Nama[24+1];
	uchar	Nomor[16+1];
	uchar	Tagihan[12+1];
	uchar	NoRek[20+1];
	uchar	NamaCust[30+1];
}PEMBAYARAN_KARTUKREDIT_KTA;

typedef struct _tagPEMBAYARAN_CICILAN
{

	uchar	TotalPembayaran[20+1];
	uchar	NoRek[20+1];
	uchar	billid[20+1];
	uchar	name[30+1];
	uchar	ptname[30+1];
	uchar	branchname[30+1];
	uchar	serialnumber[20+1];
	uchar	norangka[30+1];
	uchar	merkkendaraan[30+1];
	uchar	tenor[5+1];
	uchar	sisaangsuran[16+1];
	uchar	noangsuran[5+1];
	uchar	jatuhtempo[15+1];
	uchar	nilaitagihan[16+1];
	uchar	nilaiangsuran[16+1];
	uchar	nilaidenda[16+1];
	uchar	nilaiadmin[16+1];
	uchar	nilailainnya[16+1];
	uchar	referencenumber[30+1];
	
}PEMBAYARAN_CICILAN;

typedef struct _tagPEMBAYARAN_ZAKAT_INFAQ
{
	uchar	Nomor[16+1];
	uchar	Nama[20+1];		
	uchar	Total[12+1];
}PEMBAYARAN_ZAKAT_INFAQ;

typedef struct _tagPEMBAYARAN_DPLK
{
	uchar	NoDPLK[7+1];	
	uchar	Total[12+1];
}PEMBAYARAN_DPLK;

typedef struct _tagPEMBAYARAN_TIKET
{
	uchar	KodeBayar[13+1];	
	uchar	KodeBooking[6+1];
	uchar	Nama[30+1];
	uchar	JmlhFlight[1+1];
	uchar	Total[12+1];
	uchar	Jadwal_1[30+1];
	uchar	Jadwal_2[30+1];
	uchar	Jadwal_3[30+1];
	uchar	Tampilan[24+1];
}PEMBAYARAN_TIKET;

typedef struct _tagPEMBAYARAN_BRIVA
{
	uchar	NoBriva[20+1];	
	uchar	NamaInstitusi[20+1];
	uchar	NamaPelanggan[30+1];
	uchar	Keterangan[20+1];
	uchar	Tagihan[12+1];
	uchar	Bayar[12+1];
	uchar	Total[12+1];
	uchar	Status[1+1];
}PEMBAYARAN_BRIVA;

typedef struct _tagPEMBAYARAN_PENDIDIKAN
{
	uchar	NoBill[20+1];	
	uchar	NamaMahasiswa[20+1];
	uchar	NoMahasiswa[20+1];
	uchar	Tipe[20+1];
	uchar	Tagihan[12+1];
}PEMBAYARAN_PENDIDIKAN;

typedef struct _tagISI_ULANG
{
	uchar	NoHP[15+1];	
	uchar	Nominal[12+1];
	uchar	Pulsa[12+1];
	uchar	Periode[8+1];
	uchar	KodeVoucer[16+1];
	uchar	NomerResiSF[20+1];
	uchar	NoRekPengirim[28+1];
	uchar	AdminFee[12+1];
	uchar	OperatorProvider[20+1];
}ISI_ULANG;


typedef struct _tagSETOR_PASTI
{
	uchar	NamaAsal[15+1];	
	uchar	NamaTujuan[20+1];
	uchar	NoRekAsal[30+1];
	uchar	NoRekTujuan[30+1];
	uchar	BankAsal[15+1];
	uchar	KodeJenisTransfer[2+1];
	uchar	SaldoAkhir[20+1];
	uchar	Setoran[20+1];
}SETOR_PASTI;

typedef struct _tagSALE_REDEEMPTION
{
	uchar	SaleAmt[12+1];	
	uchar	RedeemAmt[12+1];	
	uchar	NetSaleAmt[12+1];
	ulong	PointRedeem;
	ulong	PointAward;
	ulong	PointBalance;
	uchar	Buff63[255];
}SALE_REDEEMPTION;

typedef struct _tagCICILAN_BRING
{
	uchar	Term[2+1];	
	uchar	PokokAmt[12+1];	
	uchar	Bulan[2+1];	
	uchar	BulanAmt[12+1];
	uchar	Interest[5+1];
}CICILAN_BRING;
typedef struct _tagCICILAN_BUKOPIN //4212
{
	uchar	Term[2+1];	
	uchar	PokokAmt[12+1];	// total amount
	uchar	Bulan[2+1];		// tenor month
	uchar	BulanAmt[12+1]; // monthly amount to pay
	uchar	Interest[5+1]; 	// bukopin interest
	uchar	InstallmenTenor[3+1]; //in bukopin as planCode
}CICILAN_BUKOPIN;

typedef struct _tagSSB
{
	uchar	KodeSSB[2+1];
	uchar	BayarSSB[24+1];
	uchar	BayarSSB1[24+1];
	uchar	KodeSatpasSSB[4+1];
	uchar	RekSatpasSSB[15+1];
	uchar	NamaSatpasSSB[15+1];
	uint		Jumlah;
}SSB;


typedef struct _tagBRIZZI
{
	uchar	NoKartu[lenPan+1];	
	uchar	UID[11+1];
	uchar	AmountBefore[12+1];
	uchar	AmountAfter[12+1];
	uchar	SisaDeposit[12+1];
	uchar	Hash[8+1];
	uchar	RandomData[24+1];
	uchar	PrintAmount[20+1];
	uchar	PrintAmountBefore[20+1];
	uchar	PrintAmountAfter[20+1];
	uchar 	Fee[12+1];
	uchar 	LamaPasif[2+1];
	uchar	StatusKartu[2+1];
}BRIZZI;




typedef struct _tagPEMBAYARAN_PDAM
{
	uchar	ProductId[4+1];
	uchar	BillId[20+1];
	uchar	CustName[30+1];
	uchar	NumOfBill[1+1];
	uchar	PayBallance[20+1];
	uchar	PayPeriod[20+1];
	uchar	PayPenalti[20+1];
	uchar	PayTax[20+1];
	uchar	TotalBayar[20+1];
	uchar   NoRekAsal[30+1];
	uchar   AdminFee[20+1];
	uchar   OtherFee[20+1];
	uchar   Kubikasi[20+1];
	uchar   ReffNum[20+1];
}PEMBAYARAN_PDAM;


typedef struct _tagDATA_BRI
{
	CARDSERVIS						Card_Services;
	REGISTRASI_SMSBANKING			REG_SMSBANKING;
	REGISTRASI_IBANKING				REG_IBANKING;
	TRANSFER_SESAMABRI				TRF_SESAMABRI;
	TRANSFER_ANTARBANK				TRF_ANTARBANK;
	PEMBAYARAN_TELKOM				PEMB_TELKOM;
	PEMBAYARAN_KARTUHALO			PEMB_KARTUHALO;
	PEMBAYARAN_KARTUMATRIX		PEMB_KARTUMATRIX;
	PEMBAYARAN_PLNPASCABAYAR		PEMB_PLNPASCA;
	PEMBAYARAN_PLNPRABAYAR		PEMB_PLNPRA;
	PEMBAYARAN_KARTUKREDIT_BRI 	PEMB_KKBRI;
	PEMBAYARAN_KARTUKREDIT_KTA	PEMB_KK_KTA;
	PEMBAYARAN_CICILAN				PEMB_CICILAN;
	PEMBAYARAN_ZAKAT_INFAQ			PEMB_ZAKAT_INFAQ;
	PEMBAYARAN_DPLK				PEMB_DPLK;
	PEMBAYARAN_TIKET				PEMB_TIKET;
	PEMBAYARAN_BRIVA				PEMB_BRIVA;
	PEMBAYARAN_PENDIDIKAN			PEMB_PENDIDIKAN;
	ISI_ULANG						ISIULANG;
	TUNAI							Tunai;
	CASHIN							CashIn;
	CASHOUT						CashOut;
	

	BRIZZI							Brizzi;
	SETOR_PASTI						SetorPasti;
	uchar							ucflagDebitBri;
	SALE_REDEEMPTION				SaleRedeemption;
	CICILAN_BRING					CicilanBring;
	REGISTRASI_TRXPB				RegTRXPB;
	SSB								Ssb;

	PEMBAYARAN_PDAM					PEMB_PDAM;
	
	
}DATA_BRI;
//end cung	BRI
//=========================================================================================================

//Data Bukopin
//=========================================================================================================

typedef struct _tagDATA_BUKOPIN
{
	CICILAN_BUKOPIN					CicilanBukopin;
}DATA_BUKOPIN;
//=========================================================================================================
// 交易日志
#define	MAX_GET_DESC		4
typedef struct _tagTRAN_LOG							//irf, this is log, same like log key in log ingedev
{
	uchar	ucTranType;					// 交易类型
	uchar	ucOrgTranType;				// 原交易类型
	uchar	szPan[19+1];				// 卡号
	uchar	szExpDate[4+1];				// 有效期
	uchar	szAmount[12+1];				// 交易金额
	uchar	szAmount2[12+1];				// 交易金额
	uchar	szInitialAmount[12+1];		// 交易最初的金额
	uchar	szTipAmount[12+1];			// 小费金额
	uchar	szOrgAmount[12+1];			// 原交易金额
	uchar	szFrnAmount[12+1];			// 外币金额
	uchar	szDateTime[14+1];			// YYYYMMDDhhmmss
	uchar	szRRN[13+1];				// RRN, system ref. no
	uchar	ucAcqKey;					// 收单行表号
	uchar	ucIssuerKey;				// 发卡行表号
//	uchar	szProcCode[6+1];			// 处理码, Bit 3
	uchar	szCondCode[2+1];			// Bit 25
	uchar	ucDescTotal;				// total # of Descriptor
	uchar	szDescriptor[MAX_GET_DESC+1];
	uchar	szRspCode[2+1];				// 响应码
	uchar	szHolderName[26+1];
	uchar	szAddlPrompt[20+1];
	uchar	szAuthCode[6+1];
	uchar	ucInstalment;
	CURRENCY_CONFIG	stTranCurrency;
	CURRENCY_CONFIG	stHolderCurrency;

	// EMV related data
	uchar	bPanSeqOK;					// TRUE: PAN Seq read OK
	uchar	ucPanSeqNo;
	uchar	sAppCrypto[8];
	uchar	sTVR[5];
	uchar	sTSI[2];
	uchar	sATC[2];
	uchar	szAppLabel[16+1];
	uchar	szAppPreferName[16+1];
	uchar	ucAidLen;
	uchar	sAID[16];
	ushort	uiIccDataLen;
	uchar	sIccData[LEN_ICC_DATA];
//	uchar	szEcrRef[10+1];

#define	MODE_NO_INPUT			0x0000
#define	MODE_MANUAL_INPUT		0x0001	// 手输卡号
#define	MODE_SWIPE_INPUT		0x0002	// 磁卡刷卡
#define	MODE_CHIP_INPUT			0x0004	// EMV卡插卡
#define	MODE_FALLBACK_SWIPE		0x0008	// EMV刷卡
#define	MODE_PIN_INPUT			0x0010	// online PIN input
#define MODE_OFF_PIN			0x0020	// for AMEX
#define MODE_SECURITYCODE		0x0040	// CVV/4DBC entered
#define MODE_CONTACTLESS		0x0080	// 非接触
#define MODE_FALLBACK_MANUAL	0x0100	// 暂时不需要,如果UOB需要再增加
#define MODE_PIN_BYPASS			0x0200	// by 4212 for bypass pin

	ushort	uiEntryMode;			// 输入模式, 可计算出Bit 22

	ulong	ulInvoiceNo;			// 票据号
	ulong	ulSTAN;					// STAN
	ulong	ulOrgSTAN;				// 原交易流水

	uchar	ucUploadStatus;		// 1= already upload, 0=no
		
#define TS_OK			0x0000		// txn accepted
#define TS_NOSEND		0x0001		// txn not sent to host
#define TS_ADJ			0x0002		// txn adjusted
#define TS_REVD			0x0004		// txn reversed (reserved)
#define TS_VOID			0x0008		// txn voided
#define TS_CHANGE_APPV	0x0010		// 替换Approval Code, reserved
#define TS_FLOOR_LIMIT	0x0020		// 交易金额低于Floor Limit
#define TS_OFFLINE_SEND	0x0040		//
#define TS_NOT_UPLOAD	0x0080		// 不需要上送(由NOSEND/ADJ判断)
#define TS_NEED_TC		0x0100		// 需要上送TC
#define TS_ALL_LOG		0xFFFF
	ushort	uiStatus;				// 交易状态

	uchar 	szCardType[lenName+1];

#ifdef BANK_BUKOPIN	
	DATA_BRI	BRI;
	uchar		szTransName[24+1];
	DATA_BUKOPIN	BUKOPIN;
#endif
}TRAN_LOG;

// 系统交易处理信息,不需要存储到文件系统
typedef struct _tagSYS_PROC_INFO		//this tag like tra.xml in telium
{
	TRAN_LOG	stTranLog;
	uchar		szTrack1[79+1];
	uchar		szTrack2[40+1];
	uchar		szTrack3[107+1];

	// 其他数据要素,请根据交易处理需求来增加
	uchar		bIsFallBack;
#define ST_OFFLINE		0
#define ST_ONLINE_FAIL	1
#define ST_ONLINE_APPV	2
#define ST_ONLINE_TIMEOUT 3
	uchar		ucOnlineStatus;			// extension of "bIsOnlineTxn"
	uchar		bIsFirstGAC;			// 是否是第一次GAC
	uchar		ucEcrCtrl;				// ECR标志(RFU)
	uchar		bExpiryError;			// TRUE: 卡号有效期错误
	uchar		szSecurityCode[4+1];	// CVV2/4DBC
// 	uchar		bPinEnter;				// 是否输入了PIN
	uchar		sPinBlock[8];			// PIN block (RFU)
	uchar		sPinBlock2[8];
	uchar		sPlainPinBlock[8];		// plain text pin blok
	uchar		sPlainPinBlock2[8];
	uchar		bAutoDownFlag;
	uchar		szSettleMsg[30+1];		// 结算信息(用于显示/打印等)
	ushort		uiRecNo;				// 原交易记录号(for VOID)
	int			iFallbackErrCode;		// 造成fallback的原因(for AMEX)
	STISO8583	stSendPack;				// 发送报文备份
#ifdef __TELIUM_PARAM__
	TLM_TRA		tlmTra;
#endif
// for BRI
	uchar szReq63SaleRedeemption[105+1];
	uchar szReq61CicilanBring[14+1];
	uchar szReq48[256];
	uchar szReq35[256];

	uchar rndKeyBrizzi[24+1];
	uchar TopUpKeyBrizzi[48+1];
	uchar ucRndNumber[8+1];
	uchar ucDataSettlement[999];

	uchar szReq61CicilanBukopin[14+1];

	// diki add for mini atm bukopin
	uchar szReq102[28];
	uchar szReq103[28];

}SYS_PROC_INFO;


// 交易冲正信息
typedef struct _tagREVERSAL_INFO
{
	uchar			bNeedReversal;		// TRUE：需要冲正, FALSE: 不需要
	STISO8583		stRevPack;			// 原交易请求报文
	ushort			uiEntryMode;		// 输入模式
}REVERSAL_INFO;

// 通讯数据
#define	LEN_MAX_COMM_DATA	1024
typedef struct _tagCOMM_DATA
{
	ushort		uiLength;
	uchar		sContent[LEN_MAX_COMM_DATA+10];
}COMM_DATA;

// 交易汇总信息
typedef struct _tagTOTAL_INFO
{
	ushort		uiSaleCnt;
	ushort		uiTipCnt;
	ushort		uiRefundCnt;
	ushort		uiVoidSaleCnt;
	ushort		uiVoidRefundCnt;
	ushort		uiSaleNonManualCnt;
	ushort		uiVoidSaleNonManualCnt;
	ushort		uiSaleManualCnt;
	ushort		uiVoidSaleManualCnt;

	uchar		szSaleAmt[12+1];	// base amount + tip amount
	uchar		szTipAmt[12+1];
	uchar		szRefundAmt[12+1];
	uchar		szVoidSaleAmt[12+1];
	uchar		szVoidRefundAmt[12+1];
	uchar		szSaleNonManualAmt[12+1];	// base amount + tip amount
	uchar		szVoidSaleNonManualAmt[12+1];
	uchar		szSaleManualAmt[12+1];	// base amount + tip amount
	uchar		szVoidSaleManualAmt[12+1];

	ushort		uiSaleOnlineCnt;	// base amount + tip amount
	uchar		szSaleOnlineAmt[12+1];
	ushort		uiSaleOfflineCnt;	// base amount + tip amount
	uchar		szSaleOfflineAmt[12+1];
	ushort		uiVoidSaleOnlineCnt;	// base amount + tip amount
	uchar		szVoidSaleOnlineAmt[12+1];
	ushort		uiVoidSaleOfflineCnt;	// base amount + tip amount
	uchar		szVoidSaleOfflineAmt[12+1];
	
}TOTAL_INFO;

typedef struct _tagBRIZZI_TOTAL_INFO
{
	ushort		uiSaleCnt;
	ushort		uiTopupOnlineCnt;
	ushort		uiTopupDepositCnt;
	ushort		uiAktivasiDepositCnt;
	ushort		uiRedeemCnt;
	ushort		uiReaktivasiCnt;
	ushort		uiVoidBrizziCnt;
	

	uchar		szSaleAmt[12+1];	// base amount + tip amount
	uchar		szTopupOnlineAmt[12+1];
	uchar		szTopupDepositAmt[12+1];
	uchar		szAktivasiDepositAmt[12+1];
	uchar		szRedeemAmt[12+1];
	uchar		szReaktivasiAmt[12+1];
	uchar		szVoidBrizziAmt[12+1];
}BRIZZI_TOTAL_INFO;


typedef struct _tagMINIATM_TOTAL_INFO
{
	uchar		szNamaFitur[20+1];
	ushort		uiTotalCnt;
	uchar		szTotalAmt[12+1];
}MINIATM_TOTAL_INFO;

typedef struct _tagCARDSERVICES_TOTAL_INFO
{
	uchar		szNamaFitur[25+1];
	ushort		uiTotalCnt;
}CARDSERVICES_TOTAL_INFO;

typedef struct _tagTUNAI_TOTAL_INFO
{
	uchar		szNamaFitur[20+1];
	ushort		uiTotalCnt;
	uchar		szTotalAmt[12+1];
}TUNAI_TOTAL_INFO;

typedef struct _tagTBANK_TOTAL_INFO
{
	uchar		szNamaFitur[25+1];
	ushort		uiTotalCnt;
}TBANK_TOTAL_INFO;

// reprint settle information
typedef struct _tagREPRN_STL_INFO
{
	uchar		bValid[MAX_ACQ];
	uchar		szSettleMsg[MAX_ACQ][30];
	ulong		ulBatchNo[MAX_ACQ];
	ulong		ulSOC[MAX_ACQ];
	TOTAL_INFO	stAcqTotal[MAX_ACQ];
//	TOTAL_INFO	stIssTotal[MAX_ACQ][MAX_ISSUER];
	BRIZZI_TOTAL_INFO	stBrizziTotal;
	TOTAL_INFO	stCardTotal[dimBin];
}REPRN_STL_INFO;

// 系统配置信息在下载参数或者修改参数的时候才需要更新
typedef struct _tagSYS_PARAM		//PAX's TMS
{
#define LOGON_MODE		0x01
#define CHANGE_MODE 	0x02
#define TRANS_MODE		0x04
#define INIT_MODE		0x08
	uchar				ucTermStatus;		// 终端状态

#define _TxnPSTNPara	stTxnCommCfg.stPSTNPara
#define _TxnModemPara	stTxnCommCfg.stPSTNPara.stPara
#define _TxnRS232Para	stTxnCommCfg.stRS232Para
#define _TxnTcpIpPara	stTxnCommCfg.stTcpIpPara
#define _TxnWirlessPara	stTxnCommCfg.stWirlessPara
	COMM_CONFIG			stTxnCommCfg;		// 交易通讯配置

#define _TmsPSTNPara	stTMSCommCfg.stPSTNPara
#define _TmsModemPara	stTMSCommCfg.stPSTNPara.stPara
#define _TmsRS232Para	stTMSCommCfg.stRS232Para
#define _TmsTcpIpPara	stTMSCommCfg.stTcpIpPara
#define _TmsWirlessPara	stTMSCommCfg.stWirlessPara
	COMM_CONFIG			stTMSCommCfg;		// TMS通讯配置
	uchar				ucNewTMS;			// TMS文件协议下载
	uchar				ucTMSSyncDial;		// 同步模式(仅modem)

	EDC_INFO			stEdcInfo;			// 终端参数

	uchar				ucAcqNum;			// 收单行数目
	ACQUIRER			stAcqList[MAX_ACQ];

	uchar				ucIssuerNum;		// 发卡行数目
	ISSUER				stIssuerList[MAX_ISSUER];

	uchar				ucCardNum;			// 卡表数目
	CARD_TABLE			stCardTable[MAX_CARD];

	uchar				ucDescNum;			// 商品描述个数
	DESCRIPTOR			stDescList[MAX_DESCRIPTOR];

	uchar				ucPlanNum;			// 分期付款计划个数
	INSTALMENT_PLAN		stPlanList[MAX_PLAN];

	uchar				sPassword[PWD_MAX][10];	// 密码数据

	// for HK
	ushort				uiIssuerNameNum;
	ISSUER_NAME			stIssuerNameList[MAX_CARDBIN_ISSUER];
	ushort				uiCardBinNum;
	CARD_BIN			stCardBinTable[MAX_CARDBIN_NUM];
	uchar				bTextAdData;		// False: bitmap TRUE: text
	uchar				sAdData[LEN_MAX_AD_DATA];	// 广告信息

	uchar				sTermInfo[HWCFG_END];	// Terminal hardware infomation. for GetTermInfo() use.
#define APMODE_INDEPEND		0		// Current app is app manager.   当前应用为独立运行模式
#define APMODE_MAJOR		1		// Current app is major sub-app. 当前应用为主要子应用(EDC for VISA MASTERCARD)
#define APMODE_MINOR		2		// Current app is minor sub-app. 当前应用为次要子应用(EDC for AE, DINERS, JCB)
	uchar				ucRunMode;
}SYS_PARAM;

typedef struct _tagWRITE_INFO
{
#define SAVE_NONEED		0
#define SAVE_NEW		1
#define SAVE_UPDATE		2
	uchar		bNeedSave;		// TRUE: should save stTranLog
	uchar		bBrizziNeedSave;
	uchar		bMiniAtmNeedSave;
	uchar		bCardServicesNeedSave;
	uchar		bTunaiNeedSave;
	uchar		bTBankNeedSave;
	uchar		ucAcqIndex;		// acquirer index, must < 255
	ushort		uiRecNo;		// record # of stTranLog
	ushort		uiBrizziRecNo;		// Brizzi record # of stTranLog
	ushort		uiMiniAtmRecNo;		// Mini Atm record # of stTranLog
	ushort		uiCardServicesRecNo;		// Mini Atm record # of stTranLog
	ushort		uiTunaiRecNo;		// Mini Atm record # of stTranLog
	ushort		uiTBankRecNo;		
	TRAN_LOG	stTranLog;		// txn information to save
}WRITE_INFO;

#ifdef	__TELIUM_PARAM__
typedef struct _tagTLMFILE///TMS
{
	uchar ucName[32];
	long ulSize;
}TLMFILE;
#endif

// 系统运行参数在每次交易都可能进行修改,存储在文件系统
enum {S_RESET, S_SET, S_LOAD, S_INS, S_OUT, S_USE ,S_PENDING, S_CLR_LOG};
typedef struct _tagSYS_CONTROL							//same like reg in ingedev
{
	ulong			ulSTAN;			// 当前终端流水号
	ulong			ulInvoiceNo;	// 当前票据号
	ushort			uiLastRecNo;	// 最后一笔交易记录#
	ushort			uiBrizziLastRecNo;	
	ushort			uiMiniAtmLastRecNo;
	ushort			uiCardServicesLastRecNo;
	ushort			uiTunaiLastRecNo;	// 最后一笔交易记录#
	ushort			uiTBankLastRecNo;
	ushort			uiErrLogNo;		// 当前记录EMV错误日志的记录#
	ushort			uiLastRecNoList[MAX_ACQ];		// for bea
	uchar			sAcqStatus[MAX_ACQ];			// 收单行状态信息
	uchar			sAcqKeyList[MAX_TRANLOG];		// 交易记录表

	uchar			sBrizziAcqKeyList[MAX_TRANLOG];
	uchar			sMiniAtmCSAcqKeyList[MAX_MINIATM_TRANLOG*2];
	uchar			sTunaiAcqKeyList[MAX_TUNAI_TRANLOG*2];

	uchar			sIssuerKeyList[MAX_TRANLOG];	// 交易记录表
	uchar			sBrizziIssuerKeyList[MAX_TRANLOG];
	uchar			sMiniAtmCSIssuerKeyList[MAX_MINIATM_TRANLOG*2];
	uchar			sTunaiIssuerKeyList[MAX_TUNAI_TRANLOG*2];
	uchar			WorkingKey[lenMKey+1];
	uchar			zPinKey[lenMKey+1];
	uchar			sAcqUploadStatus[MAX_ACQ];			//
	uchar			ucFlagBlowfish;
	uchar			sMenuIndicator[64];
	ulong			ulMinSaldo;
	ulong			ulMaxAkumDebit; 	
	uchar			szAmtDiskonBrizzi[12+1];
	ulong			ulDiskonBrizzi;

	// diki add for mini atm bukopin
	uchar			szNoKartuBkpn1[40+1];  // rack 2
	uchar			szNoKartuBkpn2[19+1];  // PAN

#ifdef __TELIUM_PARAM__
	TLM_APP			stTlmApp;
	TLM_REG			TlmReg;
#endif
	WRITE_INFO		stWriteInfo;	// information for saving txn log

#define LEN_SYSCTRL_BASE	((int)OFFSET(SYS_CONTROL, stRevInfo))
	REVERSAL_INFO	stRevInfo[MAX_ACQ];		// 冲正信息

#define LEN_SYSCTRL_NORMAL	((int)OFFSET(SYS_CONTROL, stRePrnStlInfo))
	REPRN_STL_INFO	stRePrnStlInfo;

#ifdef	__TELIUM_PARAM__
	TLMFILE			stTlmFile[5];
#endif
}SYS_CONTROL;

#if 0
#define lenPwd  		4				
#define lenTVR 		5
#define lenAID		16		
#define lenThrVal		4
#define lenTarPer	2
#define lenMaxTarPer	2
#define lenDftValDDOL	254
#define lenDftValTDOL	254
#define lenTrmAvn	2
#define lenAcqId		6
#define lenTrmFlrLim	4
#define lenTCC		1
#define lenAIDTxnType	2
#define lenTrnTyp	1
#define lenTmsRefNum	8 	///<Tms Reference Number Length
#define lenTmsFleChkSze		444	 ///<Tms File Chunk Size
#define lenTmsIssDatSze		26	 ///<Tms Issuer Data Size
#define lenTmsBinDatSze		28	 ///<Tms Bin Data Size
#endif

#ifdef __TELIUM_PARAM__
//TMS PARAM
typedef struct _tagTMSBeg///TMS
{
	TLM_APP_TMS		TMScon;					
	TLM_TMS_BIN		TMSBin;
	TLM_TMS_ISS		TMSIss;
	TLM_TMS_ACQ	TMSAcq;
	TLM_TMS_AID		TMSAid;
	TLM_TMS_PUB		TMSPub;
}TLM_TMS;

typedef struct _tagTlmSysParam///TMS
{
#define LOGON_MODE		0x01
#define CHANGE_MODE 	0x02
#define TRANS_MODE		0x04
#define INIT_MODE		0x08
	uchar				ucTermStatus;		// 终端状态

#define _TxnPSTNPara	stTxnCommCfg.stPSTNPara
#define _TxnModemPara	stTxnCommCfg.stPSTNPara.stPara
#define _TxnRS232Para	stTxnCommCfg.stRS232Para
#define _TxnTcpIpPara	stTxnCommCfg.stTcpIpPara
#define _TxnWirlessPara	stTxnCommCfg.stWirlessPara
	COMM_CONFIG			stTxnCommCfg;		// 交易通讯配置

#define _TmsPSTNPara	stTMSCommCfg.stPSTNPara
#define _TmsModemPara	stTMSCommCfg.stPSTNPara.stPara
#define _TmsRS232Para	stTMSCommCfg.stRS232Para
#define _TmsTcpIpPara	stTMSCommCfg.stTcpIpPara
#define _TmsWirlessPara	stTMSCommCfg.stWirlessPara
	COMM_CONFIG			stTMSCommCfg;		// TMS通讯配置
	uchar				ucNewTMS;			// TMS文件协议下载
	uchar				ucTMSSyncDial;		// 同步模式(仅modem)

	EDC_INFO			stEdcInfo;			// 终端参数

	TLM_CON				Con;
	
	uchar				ucBinNum;			// 收单行数目
	TLM_BIN				Bin[dimBin];

	uchar				ucIssNum;		// 发卡行数目
	TLM_ISS				Iss[dimIss];

	uchar				ucAcqNum;
	TLM_ACQ				Acq[dimAcq];
	
	uchar				ucDescNum;			// 商品描述个数
	DESCRIPTOR			stDescList[MAX_DESCRIPTOR];

	uchar				ucPlanNum;			// 分期付款计划个数
	INSTALMENT_PLAN		stPlanList[MAX_PLAN];

	uchar				sPassword[PWD_MAX][10];	// 密码数据

	// for HK
	ushort				uiIssuerNameNum;
	ISSUER_NAME			stIssuerNameList[MAX_CARDBIN_ISSUER];
	ushort				uiCardBinNum;
	CARD_BIN			stCardBinTable[MAX_CARDBIN_NUM];
	uchar				bTextAdData;		// False: bitmap TRUE: text
	uchar				sAdData[LEN_MAX_AD_DATA];	// 广告信息

	uchar				sTermInfo[HWCFG_END];	// Terminal hardware infomation. for GetTermInfo() use.
#define APMODE_INDEPEND		0		// Current app is app manager.   当前应用为独立运行模式
#define APMODE_MAJOR		1		// Current app is major sub-app. 当前应用为主要子应用(EDC for VISA MASTERCARD)
#define APMODE_MINOR		2		// Current app is minor sub-app. 当前应用为次要子应用(EDC for AE, DINERS, JCB)
	uchar				ucRunMode;
}TLM_SYS_PARAM;


#endif //__TELIUM_PARAM__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern SYS_PARAM		glSysParam, glSysParamBak;		// 系统配置参数
extern SYS_CONTROL		glSysCtrl;		// 系统控制参数
extern SYS_PROC_INFO	glProcInfo;		// 交易处理信息

extern COMM_DATA		glSendData, glRecvData;		// 通讯数据
extern STISO8583		glSendPack;		// 发送报文
extern STISO8583		glRecvPack;		// 接收报文
extern STISO8583		glReversalPack;	

extern STTMS8583		glTMSSend;		// TMS发送报文
extern STTMS8583		glTMSRecv;		// TMS接收报文

extern STTMSTELIUM8583		glTMSTELIUMSend;		// TMS发送报文
extern STTMSTELIUM8583		glTMSTELIUMRecv;		// TMS接收报文

extern ST_EVENT_MSG		*glEdcMsgPtr;	// 管理器事件

extern ACQUIRER			glCurAcq;		// 当前收单行
extern ISSUER			glCurIssuer;	// 当前发卡行

extern COMM_CONFIG		glCommCfg;		// 当前通讯配置参数

extern TOTAL_INFO		glAcqTotal[MAX_ACQ];		// 收单行汇总信息
extern TOTAL_INFO		glIssuerTotal[MAX_ISSUER];	// 发卡行汇总信息
extern TOTAL_INFO		glCardTotal[dimBin];
extern TOTAL_INFO		glEdcTotal;		// 终端汇总信息
extern TOTAL_INFO		glTransTotal;	// 交易处理使用的汇总信息
extern TOTAL_INFO		glPrnTotal;		// 单据打印使用的汇总信息
extern BRIZZI_TOTAL_INFO	glBrizziTotal;


#ifdef __TELIUM_PARAM__
extern TLM_CON			glTlmTmsCon;
extern TLM_TMS_AID		glTlmTmsAid;
extern TLM_TMS_PUB		glTlmTmsPub;
extern TLM_TMS_BIN		glTlmTmsBin;
extern TLM_TMS_ACQ		glTlmTmsAcq;
extern TLM_TMS_ISS		glTlmTmsIss;
extern TLM_REG			glTlmReg;

extern TLM_TMS			glTlmTmsParam;


extern TLM_ISS			glCurTlmTmsIss;
extern TLM_ACQ			glCurTlmTmsAcq;

extern TLM_SYS_PARAM	glTlmSysParam, glTlmSysParamBak;
#endif //__TELIUM_PARAM__


#ifdef ENABLE_EMV
extern EMV_PARAM		glEmvParam;
extern EMV_STATUS		glEmvStatus;
#endif

extern const LANG_CONFIG glLangList[];
extern const CURRENCY_CONFIG glCurrency[];

extern TRAN_CONFIG		glTranConfig[];

extern HOST_ERR_MSG		glHostErrMsg[];
extern TERM_ERR_MSG		glTermErrMsg[];

extern TLMFILE	TlmFile[5];
extern REPRN_STL_INFO	stRePrnStlInfo;

extern char *strstr(const char *, const char *);
//extern int vsprintf(char *buffer, const char *format, va_list argptr);	//irf, 20121128
extern int vsprintf(char *buffer, char *format, va_list argptr);	//irf, 20121128

#define CHK CHECK(ret>=0,lblKO)

int TrxSale(void);

extern uchar debugTx;
extern uchar debugRx;
extern uchar debugTLE;
extern uchar blowfishSetting;
extern uchar TLESetting;
extern uchar debugIsoReq;
extern uchar debugIsoRes;
extern uchar setKeyIn;
extern uchar countprocEMV;
extern uchar debugEMV;
extern uchar debugEMVCalc;
extern uchar debugTagEmv;
extern uchar flagNoSIM;
extern uchar BIT48ON;


// diki add 
extern uchar PrivateLabelCardDetect;


// diki add ATMB
extern uchar MenuAtmbMerchantNasabah;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _GLOBAL_H

// end of file

