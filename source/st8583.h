
/****************************************************************************
NAME
    st8583.h - 定义系统8583结构

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
****************************************************************************/

#ifndef _ST8583_H
#define _ST8583_H

#include "key.h"

#define	LEN_MSG_CODE			4
#define	LEN_BITMAP				8
#define	LEN_PAN					19
#define	LEN_PROC_CODE			6
#define	LEN_TRAN_AMT			12
#define	LEN_FRN_AMT				12
#define	LEN_DCC_RATE			8
#define	LEN_STAN				6
#define	LEN_LOCAL_TIME			6
#define	LEN_LOCAL_DATE			4
#define	LEN_EXP_DATE			4
#define	LEN_SETTLE_DATE			4
#define	LEN_ENTRY_MODE			4
#define	LEN_PAN_SEQ_NO			3
#define	LEN_NII					3
#define	LEN_COND_CODE			2
#define	LEN_TRACK2				37
#define	LEN_TRACK3				104
#define	LEN_RRN					12
#define	LEN_AUTH_CODE			6
#define	LEN_RSP_CODE			2
#define	LEN_TERM_ID				8
#define	LEN_MERCHANT_ID			15
#define	LEN_ADDL_RSP			2
#define	LEN_TRACK1				76
#define	LEN_FIELD48				300
#define	LEN_CURCY_CODE			3
#define	LEN_PIN_DATA			8
#define	LEN_EXT_AMOUNT			12
#define	LEN_ICC_DATA			260
#define	LEN_ICC_DATA2			110
#define	LEN_FIELD60				22
#define	LEN_TMSFIELD60			600
#define	LEN_FIELD61				30
#define	LEN_INVOICE_NO			6
#define LEN_FIELD62             100
#define	LEN_FIELD63				800
#define LEN_MAC					8
#define LEN_MAX_LLLVAR				999		//irf, 20121129

#define	LEN_FIELD102				28
#define	LEN_FIELD103				28



#if 0
enum eBitLen{ ///<Field Lengths
    lenBit1= 8,
    lenBit2= 19,
    lenBit3= 6,
    lenBit4= 12,
    lenBit5= 12,
    lenBit6= 12,
    lenBit7= 10,
    lenBit8= 8,
    lenBit9= 8,
    lenBit10= 8,
    lenBit11= 6,
    lenBit12= 6,
    lenBit13= 4,
    lenBit14= 4,
    lenBit15= 4,
    lenBit16= 4,
    lenBit17= 4,
    lenBit18= 4,
    lenBit19= 3,
    lenBit20= 28,
    lenBit21= 3,
    lenBit22= 4,// 3
    lenBit23= 3,
    lenBit24= 3,
    lenBit25= 2,
    lenBit26= 2,
    lenBit27= 1,
    lenBit28= 8,
    lenBit29= 8,
    lenBit30= 8,
    lenBit31= 8,
    lenBit32= 11,
    lenBit33= 11,
    lenBit34= 28,
    lenBit35= 37,
    lenBit36= 104,
    lenBit37= 12,
    lenBit38= 6,
    lenBit39= 2,
    lenBit40= 3,
    lenBit41= 8,
    lenBit42= 15,
    lenBit43= 40,
    lenBit44= 25,
    lenBit45= 76,
    lenBit46= 999,
    lenBit47= 999,
    lenBit48= 999,
    lenBit49= 3,
    lenBit50= 3,
    lenBit51= 3,
    lenBit52= 8,
    lenBit53= 16,
    lenBit54= 999,
    lenBit55= 999,
    lenBit56= 999,
    lenBit57= 999,
    lenBit58= 999,
    lenBit59= 999,
    lenBit60= 999,
    lenBit61= 999,
    lenBit62= 999,
    lenBit63= 999,
    lenBit64= 8,
 //   lenBit65= 1,
 //   lenBit66= 1,
 /////   lenBit103= 28,
};
#endif

//ISO8583使用说明
//Step2: 根据Step1的定义，定义结构中使用到的有实际意义的变量名，Attr_UnUsed及Attr_Over型的跳过不定义
//注意与glEdcDataDef结构要一定要一一对应,否则打包会出错.
//注意若在glEdcDataDef该域为Attr_a, Attr_n, Attr_z型时，为sz打头
//若在glEdcDataDef该域为Attr_b型时，为s打头

// Usage of ISO8583 module (For step 1, see in st8583.c; For step 3, see in TranProc.c)
// Step 2
// According to the definition in step 1, defines member variable in below "STISO8583".
// Those of type "Attr_UnUsed" and "Attr_Over" needn't to define.
// NOTICE that it must be corresponding to the sequence in "glEdcDataDef", one by one.
// if in "glEdcDataDef", the bit attribute is "Attr_a", "Attr_n" or "Attr_z", the member name should be start with "sz"
// if in "glEdcDataDef", the bit attribute is "Attr_b", the member name should be start with "s"

// 说明：
// 1. 对Binary类型的域，结构成员变量的前面2个字节为数据长度
//    格式为高有效为在前的网络字节顺序
// 2. 对非Binary类型域，直接使用C的字符串操作/格式化函数进行赋值即可
//    多余的2个字节是为了存储'\0'字符(为方便处理冗余了一个字符)
// 1. To those bit attribute "Attr_b" (member name: sxxxxxxx[]), reserves heading 2 bytes for storing length in hex.
//      The length format is like : "\x01\x2A" when length=0x012A
// 2. To those not "Attr_b", can use "sprintf" to fill data in ASCII.
//      The extra 2 bytes are to store the ending "\x00". (for abandon, use 2 bytes.)
#if 0
typedef struct _tagSTISO8583
{
	uchar	szMsgCode[LEN_MSG_CODE+2];				// message code
	uchar	sBitMap[2*LEN_BITMAP];					// !!!! No leading 2 length bytes !!!!  不要加2
	uchar	szPan[LEN_PAN+2];						// PAN
	uchar	szProcCode[LEN_PROC_CODE+2];			// proc code
	uchar	szTranAmt[LEN_TRAN_AMT+2];				// Txn Amount
	uchar	szFrnAmt[LEN_FRN_AMT+2];				// Foreign amt
	uchar	szDccRate[LEN_DCC_RATE+2];				// DCC Rate
	uchar	szSTAN[LEN_STAN+2];						// STAN
	uchar	szLocalTime[LEN_LOCAL_TIME+2];			// time, hhmmss
	uchar	szLocalDate[LEN_LOCAL_DATE+2];			// date, YYMM
	uchar	szExpDate[LEN_EXP_DATE+2];				// Expiry, YYMM
	uchar	szEntryMode[LEN_ENTRY_MODE+2];			// entry mode
	uchar	szPanSeqNo[LEN_PAN_SEQ_NO+2];			// PAN seq #
	uchar	szNii[LEN_NII+2];						// NII
	uchar	szCondCode[LEN_COND_CODE+2];			// Cond. code
	uchar	szTrack2[LEN_TRACK2+2];					// track 2
	uchar	szTrack3[LEN_TRACK3+2];					// track 3
	uchar	szRRN[LEN_RRN+2];						// RRN
	uchar	szAuthCode[LEN_AUTH_CODE+2];			// auth code
	uchar	szRspCode[LEN_RSP_CODE+2];				// rsp code
	uchar	szTermID[LEN_TERM_ID+2];				// terminal id
	uchar	szMerchantID[LEN_MERCHANT_ID+2];		// merchant id
	uchar	szAddlRsp[LEN_ADDL_RSP+2];				// add'l rsp
	uchar	szTrack1[LEN_TRACK1+2];					// track 1
	uchar	sField48[LEN_FIELD48+2];				// for instalment or cvv2 for visa/master card
	uchar	szTranCurcyCode[LEN_CURCY_CODE+2];		// for DCC, transaction currency
	uchar	szHolderCurcyCode[LEN_CURCY_CODE+2];	// for DCC, holder currency
	uchar	sPINData[LEN_PIN_DATA+2];				// PIN data
//	uchar	szExtAmount[LEN_EXT_AMOUNT+2];			// extra amount
	uchar	szExtAmount[LEN_MAX_LLLVAR+2];			// extra amount	//irf, 20121129
	uchar	sICCData[LEN_ICC_DATA+2];				// ICC data, or AMEX non-EMV transaction 4DBC
	uchar	sICCData2[LEN_ICC_DATA2+2];				// ICC data, FOR HK
//	uchar	szField60[LEN_FIELD60+2];
	uchar	szField60[LEN_MAX_LLLVAR+2];		//irf, 20121129
//	uchar	szField61[LEN_FIELD61+2];
	uchar	szField61[LEN_MAX_LLLVAR+2];		//irf, 20121129
//	uchar	sField62[LEN_FIELD62+2];
	uchar	sField62[LEN_MAX_LLLVAR+2];		//irf, 20121129
//	uchar	sField63[LEN_FIELD63+2];
	uchar	sField63[LEN_MAX_LLLVAR+2];		//irf, 20121129
	uchar	sMac[LEN_MAC+2];
///	uchar	szBit65[1+2];
///	uchar	szBit66[1+2];
}STISO8583;
#else	//irf 20121129
typedef struct _tagSTISO8583
{
/*MTI*/		uchar	szMsgCode[LEN_MSG_CODE+2];			// message code						
/*BITMAP*/	uchar	sBitMap[LEN_BITMAP*2];					// !!!! No leading 2 length bytes !!!!  不要加2
/*BIT 1*/	// uchar	szBit1[lenBit1+2];			// 									
/*BIT 2*/	uchar	szBit2[lenBit2+2];						// PAN								
/*BIT 3*/	uchar	szBit3[lenBit3+2];			// proc code							
/*BIT 4*/	uchar	szBit4[lenBit4+2];				// Txn Amount							
/*BIT 5*/	uchar	szBit5[lenBit5+2];				// Foreign amt
/*BIT 6*/	uchar	szBit6[lenBit6+2];				
/*BIT 7*/	uchar	szBit7[lenBit7+2];				
/*BIT 8*/	uchar	szBit8[lenBit8+2];	
/*BIT 9*/	uchar	szBit9[lenBit9+2];	
/*BIT 10*/	uchar	szBit10[lenBit10+2];				// DCC Rate
/*BIT 11*/	uchar	szBit11[lenBit11+2];						// STAN
/*BIT 12*/	uchar	szBit12[lenBit12+2];			// time, hhmmss
/*BIT 13*/	uchar	szBit13[lenBit13+2];			// date, YYMM
/*BIT 14*/	uchar	szBit14[lenBit14+2];				// Expiry, YYMM
/*BIT 15*/	uchar	szBit15[lenBit15+2];				
/*BIT 16*/	uchar	szBit16[lenBit16+2];				
/*BIT 17*/	uchar	szBit17[lenBit17+2];				
/*BIT 18*/	uchar	szBit18[lenBit18+2];				
/*BIT 19*/	uchar	szBit19[lenBit19+2];				
/*BIT 20*/	uchar	szBit20[lenBit20+2];				
/*BIT 21*/	uchar	szBit21[lenBit21+2];				
/*BIT 22*/	uchar	szBit22[lenBit22+2];			// entry mode
/*BIT 23*/	uchar	szBit23[lenBit23+2];			// PAN seq #
/*BIT 24*/	uchar	szBit24[lenBit24+2];						// NII
/*BIT 25*/	uchar	szBit25[lenBit25+2];			// Cond. code
/*BIT 26*/	uchar	szBit26[lenBit26+2];				
/*BIT 27*/	uchar	szBit27[lenBit27+2];				
/*BIT 28*/	uchar	szBit28[lenBit28+2];				
/*BIT 29*/	uchar	szBit29[lenBit29+2];				
/*BIT 30*/	uchar	szBit30[lenBit30+2];				
/*BIT 31*/	uchar	szBit31[lenBit31+2];				
/*BIT 32*/	uchar	szBit32[lenBit32+2];				
/*BIT 33*/	uchar	szBit33[lenBit33+2];				
/*BIT 34*/	uchar	szBit34[lenBit34+2];				
/*BIT 35*/	uchar	szBit35[lenBit35+2];					// track 2
/*BIT 36*/	uchar	szBit36[lenBit36+2];					// track 3
/*BIT 37*/	uchar	szBit37[lenBit37+2];						// RRN
/*BIT 38*/	uchar	szBit38[lenBit38+2];			// auth code
/*BIT 39*/	uchar	szBit39[lenBit39+2];				// rsp code
/*BIT 40*/	uchar	szBit40[lenBit40+2];				
/*BIT 41*/	uchar	szBit41[lenBit41+2];				// terminal id
/*BIT 42*/	uchar	szBit42[lenBit42+2];		// merchant id
/*BIT 43*/	uchar	szBit43[lenBit43+2];				
/*BIT 44*/	uchar	szBit44[lenBit44+2];				// add'l rsp
/*BIT 45*/	uchar	szBit45[lenBit45+2];					// track 1
/*BIT 46*/	uchar	szBit46[lenBit46+2];			
/*BIT 44*/	uchar	szBit47[lenBit47+2];			
/*BIT 48*/	uchar	szBit48[lenBit48+2];				// for instalment or cvv2 for visa/master card
/*BIT 49*/	uchar	szBit49[lenBit49+2];		// for DCC, transaction currency
/*BIT 50*/	uchar	szBit50[lenBit50+2];			
/*BIT 51*/	uchar	szBit51[lenBit51+2];	// for DCC, holder currency
/*BIT 52*/	uchar	szBit52[lenBit52+2];				// PIN data
/*BIT 53*/	uchar	szBit53[lenBit53+2];			
/*BIT 54*/	uchar	szBit54[lenBit54+2];			// extra amount	//irf, 20121129
/*BIT 55*/	uchar	szBit55[lenBit55+2];				// ICC data, or AMEX non-EMV transaction 4DBC
/*BIT 56*/	uchar	szBit56[lenBit56+2];				// ICC data, FOR HK
/*BIT 57*/	uchar	szBit57[lenBit57+2];			
/*BIT 58*/	uchar	szBit58[lenBit58+2];			
/*BIT 59*/	uchar	szBit59[lenBit59+2];			
/*BIT 60*/	uchar	szBit60[lenBit60+2];		//irf, 20121129
/*BIT 61*/	uchar	szBit61[lenBit61+2];		//irf, 20121129
/*BIT 62*/	uchar	szBit62[lenBit62+2];		//irf, 20121129
/*BIT 63*/	uchar	szBit63[lenBit63+2];		//irf, 20121129
/*BIT 64*/	uchar	sMac[LEN_MAC+2];
/*BIT 65*/	////uchar	szBit65[1+2];
/*BIT 66*/	////uchar	szBit66[1+2];
/*BIT 102*/	uchar	szField102[LEN_FIELD102+2];
/*BIT 103*/	uchar	szField103[LEN_FIELD103+2];
/*BIT 103*/	/////uchar	szBit103[lenBit103+2]; 		// DIKI ADD FOR MINI ATM BUKOPIN
}STISO8583;

#endif
// TMS专用报文
// For TMS use.
typedef struct _tagSTTMS8583
{
	uchar	szMsgCode[LEN_MSG_CODE+2];				// message code
	uchar	sBitMap[2*LEN_BITMAP];					// 不要加2
	uchar	szProcCode[LEN_PROC_CODE+2];			// proc code
	uchar	szSTAN[LEN_STAN+2];						// STAN
	uchar	szLocalTime[LEN_LOCAL_TIME+2];			// time, hhmmss
	uchar	szLocalDate[LEN_LOCAL_DATE+2];			// date, YYMM
	uchar	szNii[LEN_NII+2];						// NII
	uchar	szRspCode[LEN_RSP_CODE+2];				// rsp code
	uchar	szTermID[LEN_TERM_ID+2];				// terminal id
	uchar	szMerchantID[LEN_MERCHANT_ID+2];		// merchant id
	uchar	sField60[LEN_TMSFIELD60+2];
	uchar	szField61[LEN_FIELD61+2];
}STTMS8583;

// For TMS TELIUM use.
typedef struct _tagSTTMSTELIUM8583
{
	uchar	szMsgCode[LEN_MSG_CODE+2];			// message code
	uchar	sBitMap[2*LEN_BITMAP];					// bitmap
	uchar	szBit3[lenBit3+2];							// proc code
	uchar  	szBit8[lenBit8+2];
	uchar	szBit24[lenBit24+2];						// NII
	uchar	szBit39[lenBit39+2];						// rsp code
	uchar	szBit41[lenBit41+2];						// terminal id
	uchar	szBit42[lenBit42+2];						// merchant id
	uchar	szBit60[lenBit60+2];
	uchar	szBit61[lenBit61+2];
	uchar	szBit62[lenBit62+2];
}STTMSTELIUM8583;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern FIELD_ATTR glEdcDataDef[];		// 8583消息域定义
extern FIELD_ATTR glTMSDataDef[];		// 8583消息域定义
extern FIELD_ATTR glTMSTELIUMDataDef[];		//irf, 20121205

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _ST8583_H

// end of file
