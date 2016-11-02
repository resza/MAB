
#include "global.h"




#ifdef ENABLE_EMV




/********************** Internal macros declaration ************************/
// macros for analyze EMV TLV string
#define TAGMASK_CLASS			0xC0	// tag mask of tag class
#define TAGMASK_CONSTRUCTED		0x20	// tag mask of constructed/primitive data
#define TAGMASK_FIRSTBYTE		0x1F	// tag mask of first byte
#define TAGMASK_NEXTBYTE		0x80	// tag mask of next byte

#define LENMASK_NEXTBYTE		0x80	// length mask
#define LENMASK_LENBYTES		0x7F	// mask of bytes of lenght

#define TAG_NULL_1				0x00	// null tag
#define TAG_NULL_2				0xFF	// null tag

#define DE55_LEN_FIXED		0x01	// for amex
#define DE55_LEN_VAR1		0x02
#define DE55_LEN_VAR2		0x03

#define DE55_MUST_SET		0x10	// 必须存在
#define DE55_OPT_SET		0x20	// 可选择存在
#define DE55_COND_SET		0x30	// 根据条件存在
/********************** Internal structure declaration *********************/
// callback function for GetTLVItem() to save TLV value
typedef void (*SaveTLVData)(uint uiTag, uchar *psData, int iDataLen);

typedef struct _tagDE55Tag
{
	ushort	uiEmvTag;
	uchar	ucOption;
	uchar	ucLen;		// for process AMEX bit 55, no used for ADVT/TIP
}DE55Tag;

typedef struct _tagScriptInfo
{
	ushort	uiTag;
	int		iIDLen;
	uchar	sScriptID[4];
	int		iCmdLen[20];
	uchar	sScriptCmd[20][300];
}ScriptInfo;

/********************** Internal functions declaration *********************/
static int  SetAmexDE55(DE55Tag *pstList, uchar *psOutData, int *piOutLen);
static int  AppendStdTagList(DE55Tag *pstList, ushort uiTag, uchar ucOption, uchar ucMaxLen);
static int  RemoveFromTagList(DE55Tag *pstList, ushort uiTag);
static int  SetStdDE55(uchar bForUpLoad, DE55Tag *pstList, uchar *psOutData, int *piOutLen);
static int  GetTLVItem(uchar **ppsTLVString, int iMaxLen, SaveTLVData pfSaveData, uchar bExpandAll);
static int  GetSpecTLVItem(uchar *psTLVString, int iMaxLen, uint uiSearchTag, uchar *psOutTLV, ushort *puiOutLen);
static int  GetDE55Amex(uchar *psSendHeader, uchar *psRecvDE55, int iLen);
static int  IsConstructedTag(uint uiTag);
static void SaveRspICCData(uint uiTag, uchar *psData, int iDataLen);
static void BuildTLVString(ushort uiEmvTag, uchar *psData, int iLength, uchar **ppsOutData);
static void SaveEmvData(void);
static void AdjustIssuerScript(void);
static void SaveScriptData(uint uiTag, uchar *psData, int iDataLen);
static void PackTLVData(uint uiTag, uchar *psData, uint uiDataLen, uchar *psOutData, int *piOutLen);
static void PackTLVHead(uint uiTag, uint uiDataLen, uchar *psOutData, int *piOutLen);
static int  CalcTLVTotalLen(uint uiTag, uint uiDataLen);
static void PackScriptData(void);
static void SaveTVRTSI(uchar bBeforeOnline);
static void UpdateEntryModeForOfflinePIN(void);

/********************** Internal variables declaration *********************/
static uchar sAuthData[16];			// 发卡方认证数据
static uchar sIssuerScript[300];	// 发卡方脚本
// { // for test only
// 	0x71, 0x12+0x0F, 0x9F, 0x18, 0x00, 0x86, 0x0D, 0x84, 0x1E, 0x00, 0x00, 0x08,
// 	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
// 	0x86, 0x0D, 0x84, 0x1E, 0x00, 0x00, 0x08,
// 	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
// 	0x72, 0x12+4, 0x9F, 0x18, 0x04, 0,1,2,3, 0x86, 0x0D, 0x84, 0x1E, 0x00, 0x00, 0x08,
// 	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
// };
//static int iScriptLen=40+15+4;

static int			sgAuthDataLen, sgScriptLen;
static ScriptInfo	sgScriptInfo;
static uchar		sgScriptBak[300];
static int			sgCurScript, bHaveScript, sgScriptBakLen;


// AMEX format bit 55
static DE55Tag sgAmexTagList[] =
{
	{0x9F26, DE55_LEN_FIXED, 8},
	{0x9F10, DE55_LEN_VAR1,  32},
	{0x9F37, DE55_LEN_FIXED, 4},
	{0x9F36, DE55_LEN_FIXED, 2},
	{0x95,   DE55_LEN_FIXED, 5},
	{0x9A,   DE55_LEN_FIXED, 3},
	{0x9C,   DE55_LEN_FIXED, 1},
	{0x9F02, DE55_LEN_FIXED, 6},
	{0x5F2A, DE55_LEN_FIXED, 2},
	{0x9F1A, DE55_LEN_FIXED, 2},
	{0x82,   DE55_LEN_FIXED, 2},
	{0x9F03, DE55_LEN_FIXED, 6},
	{0x5F34, DE55_LEN_FIXED, 1},
	{0x9F27, DE55_LEN_FIXED, 1},
	{0x9F06, DE55_LEN_VAR1,  16},
	{0x9F09, DE55_LEN_FIXED, 2},
	{0x9F34, DE55_LEN_FIXED, 3},
	{0x9F0E, DE55_LEN_FIXED, 5},
	{0x9F0F, DE55_LEN_FIXED, 5},
	{0x9F0D, DE55_LEN_FIXED, 5},
	{0},
};

// 消费/(预)授权,55域EMV标签, TLV format
static DE55Tag sgStdEmvTagList[] =
{	
	/*{0x9F27, DE55_MUST_SET, 0}, //tagCID
	{0x9F26, DE55_MUST_SET, 0}, //tagIssTrnCrt
	{0x95, DE55_MUST_SET, 0}, //tagTVR
	{0x9F34, DE55_MUST_SET, 0}, //tagCVMRes
	{0x9F02, DE55_MUST_SET, 0}, //tagAmtNum
	{0x9F03, DE55_MUST_SET, 0}, //tagAmtOthNum
	{0x5F2A, DE55_MUST_SET, 0}, //tagTrnCurCod
	{0x9A, DE55_MUST_SET, 0}, //tagTrnDat
	{0x9F37, DE55_MUST_SET, 0}, //tagUnpNum
	{0x4F, DE55_MUST_SET, 0}, //tagAID
	{0x84, DE55_MUST_SET, 0}, //tagDFNam
	{0x9F10, DE55_MUST_SET, 0}, //tagIAD
	{0x5F34, DE55_OPT_SET, 0}, //tagPANSeq
	{0x9F36, DE55_MUST_SET, 0}, //tagATC
	{0x9C, DE55_MUST_SET, 0}, //tagTrnTyp
	{0x9F1A, DE55_MUST_SET, 0}, //tagAccCntCod
	{0x9F33, DE55_MUST_SET, 0}, //tagTrmCap
	{0x9F35, DE55_MUST_SET, 0}, //tagTrmTyp
	{0x9F1E, DE55_MUST_SET, 0}, //tagIFDSerNum
	*/
/*// by 4212 for BUKOPIN

	//EMV LAST SET
	{0x5F2A, DE55_MUST_SET, 0},
	{0x5F34, DE55_OPT_SET, 0},  
	{0x82, DE55_MUST_SET, 0},
	{0x84, DE55_MUST_SET, 0},
	{0x95, DE55_MUST_SET, 0},
	{0x9A, DE55_MUST_SET, 0},
	{0x9C, DE55_MUST_SET, 0},
	{0x9F02, DE55_MUST_SET, 0},
	{0x9F03, DE55_MUST_SET, 0},
	{0x9F09, DE55_MUST_SET, 0},
	{0x9F10, DE55_MUST_SET, 0},
	{0x9F1A, DE55_MUST_SET, 0},
	{0x9F1E, DE55_MUST_SET, 0},
	{0x9F26, DE55_MUST_SET, 0},
	{0x9F27, DE55_MUST_SET, 0},
	{0x9F33, DE55_MUST_SET, 0},
	{0x9F34, DE55_MUST_SET, 0},
	{0x9F35, DE55_MUST_SET, 0},
	{0x9F36, DE55_MUST_SET, 0},
	{0x9F37, DE55_MUST_SET, 0},
	{0x9F41, DE55_MUST_SET, 0},
	
//*/	
	//*// 4212
	
		//EMV BUKOPIN
		{0x82, DE55_MUST_SET, 0},
		{0x84, DE55_MUST_SET, 0},
		{0x95, DE55_MUST_SET, 0},
		{0x9A, DE55_MUST_SET, 0},
		{0x9C, DE55_MUST_SET, 0},
		{0x5F2A, DE55_MUST_SET, 0},
		{0x9F02, DE55_MUST_SET, 0},
		{0x9F03, DE55_MUST_SET, 0},
		{0x9F09, DE55_MUST_SET, 0},
		{0x9F10, DE55_MUST_SET, 0},
		{0x9F1A, DE55_MUST_SET, 0},
		{0x9F1E, DE55_MUST_SET, 0},
		{0x9F26, DE55_MUST_SET, 0},
		{0x9F27, DE55_MUST_SET, 0},
		{0x9F33, DE55_MUST_SET, 0},
		{0x9F34, DE55_MUST_SET, 0},
		{0x9F35, DE55_MUST_SET, 0},
		{0x9F36, DE55_MUST_SET, 0},
		{0x9F37, DE55_MUST_SET, 0},
		{0x9F41, DE55_MUST_SET, 0},
		//{0x5F34, DE55_OPT_SET, 0},  
		
	//*/	
//	
/*/	
	{0x5A, DE55_MUST_SET, 0},
	
	
	
//*/
/*
	{0x5A, DE55_MUST_SET, 0},
	{0x82, DE55_MUST_SET, 0},
	{0x84, DE55_MUST_SET, 0},
	{0x95, DE55_MUST_SET, 0},
	{0x9A, DE55_MUST_SET, 0},
	{0x9C, DE55_MUST_SET, 0},
	{0x5F2A, DE55_MUST_SET, 0},
	{0x5F34, DE55_MUST_SET, 0},
	{0x9F02, DE55_MUST_SET, 0},
	{0x9F03, DE55_MUST_SET, 0},
	{0x9F09, DE55_MUST_SET, 0},
	{0x9F10, DE55_MUST_SET, 0},
	{0x9F1A, DE55_MUST_SET, 0},
	{0x9F1E, DE55_MUST_SET, 0},
	{0x9F26, DE55_MUST_SET, 0},
	{0x9F27, DE55_MUST_SET, 0},
	{0x9F33, DE55_MUST_SET, 0},
	{0x9F34, DE55_MUST_SET, 0},
	{0x9F36, DE55_MUST_SET, 0},
	{0x9F37, DE55_MUST_SET, 0},
	{0x9F41, DE55_MUST_SET, 0},
*/
//	{0x9F53, DE55_MUST_SET, 0},
/*	
	{0x9F27, DE55_MUST_SET, 0},
	{0x9F26, DE55_MUST_SET, 0},
	{0x95,   DE55_MUST_SET, 0},
	{0x9F34, DE55_MUST_SET, 0},
	{0x9F02, DE55_MUST_SET, 0},
	{0x9F03, DE55_MUST_SET, 0},
	{0x5F2A, DE55_MUST_SET, 0},
	{0x9A,   DE55_MUST_SET, 0},
	{0x9F37, DE55_MUST_SET, 0},
	
	{0x84,   DE55_MUST_SET, 0},
	{0x9F10, DE55_MUST_SET, 0},
	{0x9F36, DE55_MUST_SET, 0},
	{0x9C,   DE55_MUST_SET, 0},
	{0x9F1A, DE55_MUST_SET, 0},
	{0x9F33, DE55_MUST_SET, 0},
	{0x9F35, DE55_MUST_SET, 0},
	{0x9F1E, DE55_MUST_SET, 0},
// 	{0x5F34, DE55_MUST_SET, 1},  //closed by irf, 27-02-2013 // notice it's limited to L=1
//	{0x9F09, DE55_MUST_SET, 0},	//closed by irf, 27-02-2013
//	{0x9F41, DE55_MUST_SET, 0},	//closed by irf, 27-02-2013
*/
	{0},
	
};

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

#ifdef _SP30_	//for libemv_v501, sp30 pinpad
unsigned char  cEMVPiccIsoCommand (unsigned char cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv)
{
	return 0;
}
#endif
// This is NOT a callback of EMV lib
void AppSetMckParam(uchar bEnablePinBypass)
{
	//在交易处理前调用接口函数  EMVSetMCKParam 设置是否使用批数据采集
	EMV_MCKPARAM	stMckParam;
	EMV_EXTMPARAM	stExtMckParam;

#ifdef __EMV_DEBUG__ 
	countprocEMV++;
	hex_dump_char("AppSetMckParam = ", &countprocEMV, 1);
#endif	

	stMckParam.ucBypassPin = (bEnablePinBypass ? 1 : 0);
	stMckParam.ucBatchCapture = 1;
	memset(&stExtMckParam, 0, sizeof(stExtMckParam));
	stExtMckParam.ucUseTermAIPFlg = 1;
	stExtMckParam.aucTermAIP[0] = 0x08;	// always perform term risk management
	stExtMckParam.ucBypassAllFlg = 1;
	stMckParam.pvoid = &stExtMckParam;
	EMVSetMCKParam(&stMckParam);
}

// Set to default EMV parameter, since it may be changed during last transaction.
void InitTransEMVCfg(void)
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("InitTransEMVCfg = ", &countprocEMV, 1);
#endif	
	//在交易处理前调用接口函数  EMVSetMCKParam 设置是否使用批数据采集
	AppSetMckParam(TRUE);

	EMVGetParameter(&glEmvParam);
	//glEmvParam.ForceOnline   = 1;	//always online
#ifdef	__TELIUM_PARAM__
	memcpy(glEmvParam.CountryCode,   glTlmSysParam.stEdcInfo.stLocalCurrency.sCountryCode, 2);
	memcpy(glEmvParam.TransCurrCode, glTlmSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode, 2);
	memcpy(glEmvParam.ReferCurrCode, glTlmSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode, 2);
	glEmvParam.TransCurrExp = glTlmSysParam.stEdcInfo.stLocalCurrency.ucDecimal;
	glEmvParam.ReferCurrExp = glTlmSysParam.stEdcInfo.stLocalCurrency.ucDecimal;
#else
	memcpy(glEmvParam.CountryCode,   glSysParam.stEdcInfo.stLocalCurrency.sCountryCode, 2);
	memcpy(glEmvParam.TransCurrCode, glSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode, 2);
	memcpy(glEmvParam.ReferCurrCode, glSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode, 2);
	glEmvParam.TransCurrExp = glSysParam.stEdcInfo.stLocalCurrency.ucDecimal;
	glEmvParam.ReferCurrExp = glSysParam.stEdcInfo.stLocalCurrency.ucDecimal;
#endif	
	EMVSetParameter(&glEmvParam);
}

//显示应用列表，等待持卡人选择
//如果只有一个应用，则EMV内核不会调用该函数
int cEMVWaitAppSel(int TryCnt, EMV_APPLIST List[], int AppNum)
{
	int			iRet, iCnt;
	MenuItem	stAppMenu[MAX_APP_NUM+1];

#ifdef __EMV_DEBUG__
countprocEMV++;
	countprocEMV++;
	hex_dump_char("cEMVWaitAppSel", &countprocEMV, 1);
#endif	

	if( TryCnt!=0 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("NOT ACCEPT"),    3|DISP_LINE_LEFT);
		PubDispString(_T("PLS TRY AGAIN"), 5|DISP_LINE_LEFT);
		PubWaitKey(3);
	}

	PubASSERT( AppNum<=MAX_APP_NUM );
	memset(&stAppMenu[0], 0, sizeof(stAppMenu));
	for(iCnt=0; iCnt<AppNum && iCnt<MAX_APP_NUM; iCnt++)
	{
		stAppMenu[iCnt].bVisible = TRUE;
		sprintf((char *)stAppMenu[iCnt].szMenuName, "%.16s", List[iCnt].AppName);
	}

	iRet = PubGetMenu((uchar *)_T("Select App"), &stAppMenu[0], MENU_CFONT, USER_OPER_TIMEOUT);
	if( iRet<0 )
	{
		return EMV_USER_CANCEL;
	}

	return iRet;
}

//交易金额输入函数
//应用应该根据实际情况修改
// Callback function required by EMV core.
// developer customize
int cEMVInputAmount(ulong *AuthAmt, ulong *CashBackAmt)
{
	uchar	szTotalAmt[20];

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cEMVInputAmount", &countprocEMV, 1);
#endif	

	if( glProcInfo.stTranLog.szAmount[0]!=0 )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
		*AuthAmt = atol((char *)szTotalAmt);
		if( CashBackAmt!=NULL )
		{
			*CashBackAmt = 0L;
		}
	}
	else
	{
		*AuthAmt = 0L;
		if( CashBackAmt!=NULL )
		{
			*CashBackAmt = 0L;
		}
	}

#if 0		//no cash back
	if( CashBackAmt==NULL )
	{	// 设置CASHBACK金额,以便以后GetTLVData()不会失败!!!
		// Set cash back amount
		EMVSetTLVData(0x9F03, (uchar *)"\x00\x00\x00\x00\x00\x00", 6);
		EMVSetTLVData(0x9F04, (uchar *)"\x00\x00\x00\x00", 4);
	}
#endif

	return EMV_OK;
}

//处理DOL的过程中，EMV库遇到不识别的TAG时会调用该回调函数，要求应用程序处理
//如果应用程序无法处理，则直接返回-1，提供该函数只为解决一些不符合EMV的特殊
//应用程序的要求，一般情况下返回-1即可
// Callback function required by EMV core.
// When processing DOL, if there is a tag that EMV core doesn't know about, core will call this function.
// developer should offer processing for proprietary tag.
// if really unable to, just return -1
int cEMVUnknowTLVData(ushort iTag, uchar *psDat, int iDataLen)
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cEMVUnknowTLVData", &countprocEMV, 1);
#endif	

	switch( iTag )
	{
		/*
		'C' = CASH DESBUS
		'Z' = ATM CASH
		'O' = COLLEGE SCHOOL
		'H' = HOTEL/SHIP
		'X' = TRANSFER
		'A' = AUTO MOBILE/RENTAL
		'F' = RESTAURANT
		'T' = TELEPHONE ORDER PREAUTH
		'U' = UNIQUE TRANS
		'R' = RENTAL/OTHER TRANS
		*/
	#if 1 //4212
	case 0x9F53:		// Transaction Category Code (TCC) - Master Card
		*psDat = 'R';	// 0x52;
		break;
	#endif
	default:
		return -1;
	}

	return EMV_OK;
}

#if defined(_P60_S1_) || defined(_P80_)
// Generates random hex string
static void GetRandHexStr(uchar *psOut, uchar ucOutLen)
{
    int ii;
    uchar   sBuff[16];

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("GetRandHexStr = ", &countprocEMV, 1);
#endif	

    GetTime(sBuff);
    ii = sBuff[3]*65536 + sBuff[4]*256 + sBuff[5];
    srand(ii);

    for (ii=0; ii<ucOutLen; ii++)
    {
        psOut[ii] = (uchar)rand();
    }
}
#endif

//等待用户输入卡密码
//下边是处理代码，应用应该根据实际情况修改
// Callback function required by EMV core.
// Wait holder enter PIN.
// developer customized.
int cEMVGetHolderPwd(int iTryFlag, int iRemainCnt, uchar *pszPlainPin)
{
	int		iResult;
	//uchar	ucRet, szBuff[30];
	uchar szAmount[15];
#if 0
	uchar	sTemp[8];
	int		iLength;
	int iRet;
#endif
	
#if defined(_P60_S1_) || defined(_P80_)
    int     iRealLen, iByteLen, ii; 
    uchar	sTmpMKey[8], sTmpEncWKey[8], sTmpPlainWKey[8];
#endif

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cEMVGetHolderPwd = ", &countprocEMV, 1);
#endif	

#if 0
	if(iTryFlag==0)
	{
		memset(sTemp, 0, sizeof(sTemp));
		iRet = EMVGetTLVData(0x9F34, sTemp, &iLength);
		if( iRet==EMV_OK )
		{
			sTemp[0] &= 0x3F;
			if (sTemp[0]==0x02) 	// online PIN
			{
				pszPlainPin[0]=0;
			}
		}
	}
#endif
			//hex_dump_char("glProcInfo.sPinBlock", glProcInfo.sPinBlock, sizeof(glProcInfo.sPinBlock));
			//hex_dump_char("glProcInfo.sPlainPinBlock", glProcInfo.sPlainPinBlock, sizeof(glProcInfo.sPlainPinBlock));
			//hex_dump_char("pszPlainPin", pszPlainPin, sizeof(pszPlainPin));

	// 联机PIN处理
	if( pszPlainPin==NULL )
	{
		iResult = GetPIN_Manual(GETPIN_EMV);
//		hex_dump_char("iResult", &iResult, sizeof(iResult));
//		hex_dump_char("glProcInfo.stTranLog.uiEntryMode", &glProcInfo.stTranLog.uiEntryMode, sizeof(glProcInfo.stTranLog.uiEntryMode));
//		hex_dump_char("glProcInfo.sPinBlock", glProcInfo.sPinBlock, sizeof(glProcInfo.sPinBlock));
		if( iResult==0 )
		{
			if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
			{
//				hex_dump_char("glProcInfo.stTranLog.uiEntryMode", &glProcInfo.stTranLog.uiEntryMode, sizeof(glProcInfo.stTranLog.uiEntryMode));
				return EMV_OK;
			}
			else
			{
				return EMV_NO_PASSWORD;
			}
		}
		else if( iResult==ERR_USERCANCEL )
		{
			return EMV_USER_CANCEL;
		}
		else
		{
			return EMV_NO_PINPAD;
		}
	}

	// 脱机pin处理(明文/密文)
	// Offline plain/enciphered PIN processing below

	ScrClrLine(2, 7);
	if( iRemainCnt==1 )
	{
		PubDispString(_T("LAST ENTER PIN"), 4|DISP_LINE_LEFT);
		DelayMs(1500);
	}

	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szAmount);
	ScrClrLine(2, 7);
	if( iTryFlag==0 )
	{
		DispAmount(2, szAmount);
	}
	else
	{
		PubDispString(_T("PIN ERR, RETRY"), 2|DISP_LINE_LEFT);
	}

//ENTERPIN:
	PubDispString(_T("PLS ENTER PIN"), 4|DISP_LINE_LEFT);

	if (ChkTermPEDMode(PED_INT_PCI))
	{
		// Offline PIN, done by core itself since EMV core V25_T1. Application only needs to display prompt message.
        // In this mode, cEMVGetHolderPwd() will be called twice. the first time is to display message to user,
        // then back to kernel and wait PIN. afterwards kernel call this again and inform the process result.
        if (pszPlainPin[0]==EMV_PED_TIMEOUT)
        {
            // EMV core has processed PIN entry and it's timeout
        	ScrClrLine(2, 7);
            PubDispString(_T("TIME OUT"), 6|DISP_LINE_LEFT);
            PubBeepErr();
            PubWaitKey(3);
            return EMV_TIME_OUT;
        }
        else if (pszPlainPin[0]==EMV_PED_WAIT)
        {
            // API is called too frequently
            DelayMs(1000);
        	ScrClrLine(2, 7);
            ScrGotoxy(32, 6);
	    	return EMV_OK;
        }
        else if (pszPlainPin[0]==EMV_PED_FAIL)
        {
            // EMV core has processed PIN entry and PED failed.
        	ScrClrLine(2, 7);
            PubDispString(_T("PED ERROR"), 6|DISP_LINE_LEFT);
            PubBeepErr();
            PubWaitKey(3);
            return EMV_NO_PINPAD;
        }
        else
        {
            // EMV PIN not processed yet. So just display.
            ScrGotoxy(32, 6);
	    	return EMV_OK;
        }
	}
	else if (ChkTermPEDMode(PED_INT_PXX))
	{
#if defined(_P60_S1_) || defined(_P80_)
        // Generate random master key
        memset(sTmpMKey, 0, sizeof(sTmpMKey));
        GetRandHexStr(sTmpMKey, 8);
        if (PEDWriteMKey(GENERAL_MKEY_ID, 0x01, sTmpMKey)!=0)
        {
            return EMV_NO_PINPAD;
        }
        // Generate random work key
        memset(sTmpEncWKey, 0, sizeof(sTmpEncWKey));
        GetRandHexStr(sTmpEncWKey, 8);
	    if (PEDWriteWKey(GENERAL_MKEY_ID, GENERAL_WKEY_ID, 0x81, sTmpEncWKey)!=0)
        {
            return EMV_NO_PINPAD;
        }

        // input
		memset(szBuff, 0, sizeof(szBuff));
		ScrGotoxy(40, 6);
        ucRet = PEDGetPwd(GENERAL_WKEY_ID, 4, 6, "1234567890123456", szBuff, 0);   // Use virtual PAN "1234567890123456"
	    if ((ucRet==0x0A))
	    {
            return EMV_NO_PASSWORD; // Bypass
	    }
        else if ((ucRet==0x06) || (ucRet==0xC6) || (ucRet==0x07))
		{
			return EMV_USER_CANCEL; // Cancel or timeout
		}
        else if (ucRet!=0)
        {
		    return EMV_NO_PINPAD;   // PED error
        }

        // decrypt to plain text
        // get plaintext PIN
	    des(sTmpEncWKey, sTmpPlainWKey, sTmpMKey, 0);
	    des(szBuff, szBuff+15, sTmpPlainWKey, 0);
        // virtual PAN block
	    memcpy(szBuff, "\x00\x00\x56\x78\x90\x12\x34\x56", 8);
        // Get plaintext PINBlock
        PubXor(szBuff, szBuff+15, 8, szBuff);
	    iRealLen= szBuff[0] & 0x0F;
        iByteLen = iRealLen/2;
	    if (iRealLen % 2)
        {
            iByteLen += 1;
        }
		// Get pure PIN string
        PubBcd2Asc0(szBuff+1, iByteLen, pszPlainPin);
        PubTrimTailChars(pszPlainPin, 'F');
		glProcInfo.stTranLog.uiEntryMode |= MODE_OFF_PIN;
		return EMV_OK;
#else
		return EMV_USER_CANCEL;
#endif
	}
	else	// PED_EXT_PCI
	{
		// !!!! extern PCI, to be implemented.
		ScrClrLine(2, 7);
		PubDispString("EXT PCI PINPAD  ", 4|DISP_LINE_LEFT);
		PubDispString("NOT IMPLEMENTED.", 6|DISP_LINE_LEFT);
		PubWaitKey(30);
		return ERR_NO_DISP;
	}
}

/*语音参考处理
  如果不支持，如果是卡片发起的参考，
  则可根据发卡行要求直接返回REFER_DENIAL或REFER_ONLINE,
  一般情况下不应该直接返回REFER_APPROVE(除非发卡行要求这么做)

  如果不支持，如果是发卡行(主机)发起的参考，
  则可根据发卡行要求直接返回REFER_DENIAL
  一般情况下不应该直接返回REFER_APPROVE(除非发卡行要求这么做)

  下边是参考的处理代码，供参考
 */
// Callback function required by EMV core.
// Voice referral.
// If not support, return REFER_DENIAL.
int cEMVReferProc(void)
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cEMVReferProc = ", &countprocEMV, 1);
#endif	

	return REFER_DENIAL;
}

//通知处理(如果不支持，直接返回就可以了)
// Callback function required by EMV core.
// TC advise after EMV transaction. If not support, immediately return.
void cEMVAdviceProc(void)
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cEMVAdviceProc = ", &countprocEMV, 1);
#endif	

/*
	脱机通知的处理：
	通过函数EMVGetTLVData()获得通知数据包需要的数据，存贮到交易日志中，
	然后在交易结算时，再联机发送到主机。
	需要注意的是：通知数据包的任何数据(比如金额等)不可以用于交易对帐。

	联机通知的处理：
	(1)拨号连接主机。
	(2)通过函数EMVGetTLVData()获得通知数据包需要的数据，再发送到主机。
	需要注意的是：联机通知方式在我们的POS机中应该不会使用。
*/
}

//联机处理
/*
	处理步骤：
	(1)拨号连接主机,如果所有交易都要联机，那么可以在插入IC卡时预拨号,
	   如果拨号失败返回ONLINE_FAILED
	(2)通过函数EMVGetTLVData()获得交易数据包需要的数据，并打包。
	(3)保存冲正数据及标志,然后发送交易数据包到主机(冲正处理完全由应用完成)
	(4)接收主机的回应数据包,根据主机的回应，做如下返回：
	   A.如果主机返回批准，则根据返回数据填写RspCode、AuthCode、AuthCodeLen等
		 参数的值，并返回ONLINE_APPROVE
	   B.如果主机拒绝交易,则根据返回数据填写RspCode,如果其他参数也有数据值，
		 同样需要填写，返回ONLINE_DENIAL
	   C.如果主机请求语音参考,则根据返回数据填写RspCode,如果其他参数也有数据值，
		 同样需要填写，返回ONLINE_REFER。需要说明的是：很多情况可能没有参考处理，
		 在这种情况下，应用程序就不需要返回ONLINE_REFER了

	等交易处理成功后，应用程序才可以清除冲正标志。
*/
/* Online processing.
    steps:
	(1) Dial. If dial failed, return ONLINE_FAILED
	(2) Use EMVGetTLVData() to retrieve data from core, pack to ISO8583.
	(3) Save reversal data and flag, then send request to host
	(4) Receive from host, then do accordingly:
	   A. If host approved, copy RspCode,AuthCode,AuthCodeLen or so, and return ONLINE_APPROVE
	   B. If host denial, copy RspCode or so, return ONLINE_DENIAL
	   C. If host require voice referral, copy RspCode or so.,return ONLINE_REFER.
	       Note that if not support, needn't return ONLINE_REFER but directly ONLINE_DENIAL

	Reversal flag can only be cleared after all EMV processing, NOT immediately after online.
*/
int  cEMVOnlineProc(uchar *psRspCode,  uchar *psAuthCode, int *piAuthCodeLen,
					uchar *psAuthData, int *piAuthDataLen,
					uchar *psScript,   int *piScriptLen)
{
	int		iRet, iLength, iRetryPIN;
	ulong	ulICCDataLen;
	uchar	*psICCData, *psTemp;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cEMVOnlineProc = ", &countprocEMV, 1);
#endif	

	// initialize output parameters
	*psRspCode      = 0;
	*piAuthCodeLen  = 0;
	*piAuthDataLen  = 0;
	*piScriptLen    = 0;
	SaveTVRTSI(TRUE);
	glProcInfo.bIsFirstGAC = FALSE;
	
	UpdateEntryModeForOfflinePIN();
/////4212
	if( !(glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT) && !(glProcInfo.stTranLog.uiEntryMode & MODE_OFF_PIN) && !(glProcInfo.stTranLog.uiEntryMode & MODE_PIN_BYPASS)){
		
		iRet = GetPIN_ForcePIN(FALSE);
		if(iRet != 0)
		{
			return iRet;
		}
	}
	SetPosEntryMode();
/////end

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT)
	{
		CalculatePinBlock(glProcInfo.sPinBlock);
		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================
	

	// prepare online DE55 data
	iRet = SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
	if( iRet!=0 )
	{
		glProcInfo.ucOnlineStatus = ST_ONLINE_FAIL;
		return ONLINE_FAILED;
	}
	PubASSERT( iLength<LEN_ICC_DATA );
	PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
	memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);	// save for batch upload
	glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;

#ifdef BANK_MANDIRI
	//LOGON
	iRet = TransLogon();
	if(iRet != 0)
	{
		return iRet;
	}
#endif

	// 冲正交易处理 & 离线交易上送
	// 判断上次交易是否需要进行冲正等
	// send reversal here. If required by bank, also send offline here
	iRet = TranReversal();
	if( iRet!=0 )
	{
		if(iRet == ERR_COMM_ERROR)
			glProcInfo.ucOnlineStatus = ST_ONLINE_TIMEOUT;
		else
			glProcInfo.ucOnlineStatus = ST_ONLINE_FAIL;

		return ONLINE_FAILED;
	}

	iRetryPIN = 0;
	while( 1 )
	{
		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			if(iRet == ERR_COMM_ERROR)
				glProcInfo.ucOnlineStatus = ST_ONLINE_TIMEOUT;
			else
				glProcInfo.ucOnlineStatus = ST_ONLINE_FAIL;

			return ONLINE_FAILED;
		}
#if 0
		if( memcmp(glRecvPack.szBit39, "55", 2)!=0 || ++iRetryPIN>3 || !ChkIfNeedPIN() )
		{
			break;
		}

		// 重新输入PIN
		// retry EMV online PIN
		iRet = GetPIN_Manual(GETPIN_RETRY);
		if( iRet!=0 )
		{
			return ONLINE_DENIAL;
		}
		sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);
		memcpy(&glSendPack.szBit52[0], "\x00\x08", 2);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, 8);
#endif		
		break;
	}

	// set response code
	memcpy(psRspCode,  glRecvPack.szBit39,  LEN_RSP_CODE);
	glProcInfo.ucOnlineStatus = ST_ONLINE_APPV;

	// get response issuer data
	sgAuthDataLen = sgScriptLen = 0;

	PubChar2Long(glRecvPack.szBit55, 2, &ulICCDataLen);
	psICCData = &glRecvPack.szBit55[2];
	for(psTemp=psICCData; psTemp<psICCData+ulICCDataLen; )
	{
		iRet = GetTLVItem(&psTemp, psICCData+ulICCDataLen-psTemp, SaveRspICCData, TRUE);
		if( iRet<0 )
		{	// if analyze response ICC data failed, return fail
			glProcInfo.ucOnlineStatus = ST_ONLINE_FAIL;
			return ONLINE_FAILED;
		}
	}

	memcpy(psAuthData, sAuthData, sgAuthDataLen);
	*piAuthDataLen = sgAuthDataLen;
	
	AdjustIssuerScript();
	memcpy(psScript, sIssuerScript, sgScriptLen);
	*piScriptLen = sgScriptLen;

	if( memcmp(glRecvPack.szBit39, "00", LEN_RSP_CODE)!=0 )
	{
		return ONLINE_DENIAL;
	}
	// set authorize code only if txn approved
	memcpy(psAuthCode, glRecvPack.szBit38, LEN_AUTH_CODE);
	*piAuthCodeLen = strlen((char *)glRecvPack.szBit38);

	return ONLINE_APPROVE;
}

//如果不需要提示密码验证成功，则直接返回就可以了
// Callback function required by EMV core.
// Display "EMV PIN OK" info. (plaintext/enciphered PIN)
void cEMVVerifyPINOK(void)
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cEMVVerifyPINOK = ", &countprocEMV, 1);
#endif	

	ScrClrLine(2, 7);
	PubDispString(_T("PIN VERIFY OK"), 4|DISP_LINE_LEFT);
	PubWaitKey(1);
}

//持卡人认证例程
// Callback function required by EMV core.
// Don't need to care about this function
int cCertVerify(void)
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cCertVerify = ", &countprocEMV, 1);
#endif	

//	AppSetMckParam(!ChkIssuerOption(ISSUER_EN_EMVPIN_BYPASS));
	return -1;
}

// Callback function required by EMV core.
// in EMV ver 2.1+, this function is called before GPO
int cEMVSetParam(void)
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("cEMVSetParam = ", &countprocEMV, 1);
#endif	

	return 0;
}

int FinishEmvTran(void)
{
	int		iRet, iLength;
	uchar	ucSW1, ucSW2;
	uchar ForceOnline=0;
#ifdef __EMV_DEBUG__
	EMV_APPLIST ptEMV_APP;
	countprocEMV++;
	hex_dump_char("FinishEmvTran = ", &countprocEMV, 1);
#endif	

#if 0
	// 根据需要设置是否强制联机
	// decide whether need force online
	EMVGetParameter(&glEmvParam);

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_OFF_SALE:
			glEmvParam.ForceOnline = 0;
			break;
		default:
			glEmvParam.ForceOnline = 1;
			break;
	}
	
	
#endif
	

	//glEmvParam.ForceOnline = (glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFF_SALE? 1 : 0); //4212 4 offline sale
	glEmvParam.ForceOnline = ForceOnline;
	EMVSetParameter(&glEmvParam);
	// Only in this trasaction, so DON'T back up

	// clear last EMV status
	memset(&glEmvStatus, 0, sizeof(EMV_STATUS));
	SaveEmvStatus();

#if defined(_S_SERIES_) || defined(_SP30_)
	if (ChkTermPEDMode(PED_INT_PCI))
	{
		iRet = EMVSetPCIModeParam(1, (uchar *)"0,4,5,6,7,8", 120000);
	}
#endif

#ifdef __EMV_DEBUG__
	EMVGetFinalAppPara(&ptEMV_APP);
	hex_dump_char("AppName", ptEMV_APP.AppName, strlen(ptEMV_APP.AppName));
	hex_dump_char("AID", ptEMV_APP.AID, sizeof(ptEMV_APP.AID));
	hex_dump_char("AidLen", &ptEMV_APP.AidLen, sizeof(ptEMV_APP.AidLen));
	hex_dump_char("SelFlag", &ptEMV_APP.SelFlag, sizeof(ptEMV_APP.SelFlag));
	hex_dump_char("Priority", &ptEMV_APP.Priority, sizeof(ptEMV_APP.Priority));
	hex_dump_char("TargetPer", &ptEMV_APP.TargetPer, sizeof(ptEMV_APP.TargetPer));
	hex_dump_char("MaxTargetPer", &ptEMV_APP.MaxTargetPer, sizeof(ptEMV_APP.MaxTargetPer));
	hex_dump_char("FloorLimitCheck", &ptEMV_APP.FloorLimitCheck, sizeof(ptEMV_APP.FloorLimitCheck));
	hex_dump_char("RandTransSel", &ptEMV_APP.RandTransSel, sizeof(ptEMV_APP.RandTransSel));
	hex_dump_char("VelocityCheck", &ptEMV_APP.VelocityCheck, sizeof(ptEMV_APP.VelocityCheck));
	hex_dump_char("FloorLimit", &ptEMV_APP.FloorLimit, sizeof(ptEMV_APP.FloorLimit));
	hex_dump_char("Threshold", &ptEMV_APP.Threshold, sizeof(ptEMV_APP.Threshold));
	hex_dump_char("TACDenial", ptEMV_APP.TACDenial, sizeof(ptEMV_APP.TACDenial));
	hex_dump_char("TACOnline", ptEMV_APP.TACOnline, sizeof(ptEMV_APP.TACOnline));
	hex_dump_char("TACDefault", ptEMV_APP.TACDefault, sizeof(ptEMV_APP.TACDefault));
	hex_dump_char("AcquierId", ptEMV_APP.AcquierId, sizeof(ptEMV_APP.AcquierId));
	hex_dump_char("dDOL", ptEMV_APP.dDOL, 36);
	hex_dump_char("tDOL", ptEMV_APP.tDOL, 36);
	hex_dump_char("Version", ptEMV_APP.Version, sizeof(ptEMV_APP.Version));
	hex_dump_char("RiskManData", ptEMV_APP.RiskManData, sizeof(ptEMV_APP.RiskManData));

	EMVGetParameter(&glEmvParam);
	hex_dump_char("MerchCateCode", glEmvParam.MerchCateCode, sizeof(glEmvParam.MerchCateCode));
	hex_dump_char("MerchId", glEmvParam.MerchId, sizeof(glEmvParam.MerchId));
	hex_dump_char("TermId", glEmvParam.TermId, sizeof(glEmvParam.TermId));
	hex_dump_char("TerminalType", &glEmvParam.TerminalType, sizeof(glEmvParam.TerminalType));
	hex_dump_char("Capability", glEmvParam.Capability, sizeof(glEmvParam.Capability));
	hex_dump_char("ExCapability", glEmvParam.ExCapability, sizeof(glEmvParam.ExCapability));
	hex_dump_char("TransCurrExp", &glEmvParam.TransCurrExp, sizeof(glEmvParam.TransCurrExp));
	hex_dump_char("ReferCurrExp", &glEmvParam.ReferCurrExp, sizeof(glEmvParam.ReferCurrExp));
	hex_dump_char("ReferCurrCode", glEmvParam.ReferCurrCode, sizeof(glEmvParam.ReferCurrCode));
	hex_dump_char("CountryCode", glEmvParam.CountryCode, sizeof(glEmvParam.CountryCode));
	hex_dump_char("TransCurrCode", glEmvParam.TransCurrCode, sizeof(glEmvParam.TransCurrCode));
	hex_dump_char("ReferCurrCon", &glEmvParam.ReferCurrCon, sizeof(glEmvParam.ReferCurrCon));
	hex_dump_char("TransType", &glEmvParam.TransType, sizeof(glEmvParam.TransType));
	hex_dump_char("ForceOnline", &glEmvParam.ForceOnline, sizeof(glEmvParam.ForceOnline));
	hex_dump_char("GetDataPIN", &glEmvParam.GetDataPIN, sizeof(glEmvParam.GetDataPIN));
	hex_dump_char("SurportPSESel", &glEmvParam.SurportPSESel, sizeof(glEmvParam.SurportPSESel));

	
#endif

	

	// 处理EMV交易
	// Process EMV transaction.
	iRet = EMVProcTrans();
#ifdef __EMV_DEBUG__
	hex_dump_char("EMVProcTrans iRet", &iRet, sizeof(iRet));
	PubWaitKey(30);
#endif
	SaveTVRTSI(FALSE);
	UpdateEntryModeForOfflinePIN();
	if( iRet==EMV_TIME_OUT || iRet==EMV_USER_CANCEL )
	{
		return ERR_USERCANCEL;
	}
	if( (glProcInfo.ucOnlineStatus==ST_ONLINE_APPV) && memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
	{
		// update for reversal(maybe have script result)
		SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
		PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
		glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
		SaveRevInfo(TRUE);	// update reversal information
	}

	glEmvStatus.ucSW1 = 0x90;
	glEmvStatus.ucSW2 = 0x00;
	
	if( iRet!=EMV_OK )
	{
		
		SaveEmvErrLog();
		EMVGetICCStatus(&ucSW1, &ucSW2);
		glEmvStatus.ucSW1 = ucSW1;
		glEmvStatus.ucSW2 = ucSW2;
	
		if( glProcInfo.bIsFirstGAC && ucSW1==0x69 && ucSW2==0x85 &&
			glProcInfo.stTranLog.szPan[0]=='5' )
		{	// for TIP fallback when 1st GAC return 6985
			//hex_dump_char("ERR_NEED_FALLBACK","test",4);
			return ERR_NEED_FALLBACK;
		}

		// for sale completion only send 5A not 57 [1/11/2007 Tommy]
		if( (glProcInfo.ucOnlineStatus!=ST_OFFLINE) &&
			(memcmp(glProcInfo.stTranLog.szRspCode, "01", 2)==0 ||
			 memcmp(glProcInfo.stTranLog.szRspCode, "02", 2)==0)
			)
		{
			SetDE55(DE55_UPLOAD, glProcInfo.stTranLog.sIccData, &iLength);
			glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
			//hex_dump_char("ST_OFFLINE","test",4);
		}

		if( glProcInfo.stTranLog.szRspCode[0]!=0 &&
			memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
		{	// show reject code from host
			//hex_dump_char("AfterTranProc","RC0",3);
			return AfterTranProc();
		}
		//emv_test();
		//hex_dump_char("ERR_TRAN_FAIL","test",4);
		return ERR_TRAN_FAIL;
	}

	if(glProcInfo.ucOnlineStatus == ST_ONLINE_TIMEOUT)
		return ERR_TRAN_FAIL;
		//hex_dump_char("ERR_TRAN_FAIL","END",3);
	
	// 交易成功, 保存EMV交易数据
	// transaction approved. save EMV data
	SaveEmvData();
	if( glProcInfo.ucOnlineStatus!=ST_ONLINE_APPV )
	{
		return FinishOffLine();
	}
	
	if (ChkIfAcqNeedTC())
	{
		glProcInfo.stTranLog.uiStatus |= TS_NEED_TC;
	}
	
	//hex_dump_char("AfterTranProc","END",3);
	return AfterTranProc();
}

// Set bit 55 data for online transaction.
int SetDE55(DE55_TYPE ucType, uchar *psOutData, int *piOutLen )
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("SetDE55 = ", &countprocEMV, 1);
#endif	
		return SetStdDE55((uchar)ucType, sgStdEmvTagList, psOutData, piOutLen);
}

// set AMEX bit 55, structure of TLV items
int SetAmexDE55(DE55Tag *pstList, uchar *psOutData, int *piOutLen)
{
	int		iRet, iCnt, iLength;
	uchar	*psTemp, sBuff[128];

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("SetAmexDE55 = ", &countprocEMV, 1);
#endif	

	*piOutLen = 0;
	memcpy(psOutData, "\xC1\xC7\xD5\xE2\x00\x01", 6);	// AMEX header
	psTemp = psOutData+6;

	for(iCnt=0; pstList[iCnt].uiEmvTag!=0; iCnt++)
	{
		iLength = 0;
		memset(sBuff, 0, sizeof(sBuff));
		iRet = EMVGetTLVData(pstList[iCnt].uiEmvTag, sBuff, &iLength);
		if( iRet!=EMV_OK )
		{
			return ERR_TRAN_FAIL;
		}

		if( pstList[iCnt].ucOption==DE55_LEN_VAR1 )
		{
			*psTemp++ = (uchar)iLength;
		}
		else if( pstList[iCnt].ucOption==DE55_LEN_VAR2 )
		{
			*psTemp++ = (uchar)(iLength>>8);
			*psTemp++ = (uchar)iLength;
		}
		memcpy(psTemp, sBuff, iLength);
		psTemp += iLength;
	}
	*piOutLen = (psTemp-psOutData);

	return 0;
}

// this function will not check the overflow risk of array pointed by pstList.
int AppendStdTagList(DE55Tag *pstList, ushort uiTag, uchar ucOption, uchar ucMaxLen)
{
	int	iCnt;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("AppendStdTagList = ", &countprocEMV, 1);
#endif	

	iCnt = 0;
	while (pstList[iCnt].uiEmvTag!=0)
	{
		iCnt++;
	}
	pstList[iCnt].uiEmvTag = uiTag;
	pstList[iCnt].ucOption = ucOption;
	pstList[iCnt].ucLen    = ucMaxLen;
	pstList[iCnt+1].uiEmvTag = 0;
	pstList[iCnt+1].ucOption = 0;
	pstList[iCnt+1].ucLen    = 0;
	return 0;
}

int RemoveFromTagList(DE55Tag *pstList, ushort uiTag)
{
	int	iCnt;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("RemoveFromTagList = ", &countprocEMV, 1);
#endif	

	for (iCnt=0; pstList[iCnt].uiEmvTag!=0; iCnt++)
	{
		if (pstList[iCnt].uiEmvTag==uiTag)
		{
			break;
		}
	}
	if (pstList[iCnt].uiEmvTag==0)
	{
		return -1;
	}

	for (; pstList[iCnt].uiEmvTag!=0; iCnt++)
	{
		pstList[iCnt] = pstList[iCnt+1];
	}

	return 0;
}

// set ADVT/TIP bit 55
int SetStdDE55(uchar bForUpLoad, DE55Tag *pstList, uchar *psOutData, int *piOutLen)
{
	int		iRet, iCnt, iLength;
	uchar	*psTemp, sBuff[200];
	DE55Tag	astLocalTaglist[64];

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("SetStdDE55 = ", &countprocEMV, 1);
#endif	

	*piOutLen = 0;
	psTemp    = psOutData;

	// ???? MODE_FALLBACK_MANUAL ????
	if ( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE ) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		{
			memcpy(psTemp, "\xDF\x5A\x01\x01", 4);
			psTemp += 4;
			memcpy(psTemp, "\xDF\x39\x01\x01", 4);
			psTemp += 4;
		}
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		// Copy from std tag list
		//-----------------------------------------------------------
		memset(astLocalTaglist, 0, sizeof(astLocalTaglist));
		for(iCnt=0; pstList[iCnt].uiEmvTag!=0; iCnt++)
		{
			astLocalTaglist[iCnt] = pstList[iCnt];
		}
		
		//-----------------------------------------------------------
		// Customize tag list according to different acquirer
		//RemoveFromTagList(astLocalTaglist, 0x5F34);
		//AppendStdTagList(astLocalTaglist, 0x5A, DE55_MUST_SET, 0);

		//-----------------------------------------------------------
		// Generate data by tag list
		for(iCnt=0; pstList[iCnt].uiEmvTag!=0; iCnt++)
		{
			memset(sBuff, 0, sizeof(sBuff));
			iRet = EMVGetTLVData(pstList[iCnt].uiEmvTag, sBuff, &iLength);
			if( iRet==EMV_OK )
			{
				if ((pstList[iCnt].ucLen > 0) && (iLength > pstList[iCnt].ucLen))
				{
					iLength = pstList[iCnt].ucLen;
				}
				BuildTLVString(pstList[iCnt].uiEmvTag, sBuff, iLength, &psTemp);
			}
			else if( pstList[iCnt].ucOption==DE55_MUST_SET )
			{
				if(pstList[iCnt].uiEmvTag == 0x9F03)
					BuildTLVString(pstList[iCnt].uiEmvTag, "\x00\x00\x00\x00\x00\x00", 6, &psTemp);
				else
					BuildTLVString(pstList[iCnt].uiEmvTag, NULL, 0, &psTemp);
//				return ERR_TRAN_FAIL;	// 如果必须存在的TAG不存在,交易失败
			}
		}

		//-----------------------------------------------------------
		// Generate custom tag content
		#if 1 //4212
		if( glProcInfo.stTranLog.szPan[0]=='5' )
		{	// for master card TCC = "R" -- retail
			BuildTLVString(0x9F53, (uchar *)"R", 1, &psTemp);
			EMVSetTLVData(0x9F53, (uchar *)"R", 1);
		}
		#endif
		memset(sBuff, 0, sizeof(sBuff));
		iRet = EMVGetScriptResult(sBuff, &iLength);
		if( iRet==EMV_OK )
		{
			BuildTLVString(0xDF5B, sBuff, iLength, &psTemp);
		}
	}
	else
	{
		return 0;
	}

	*piOutLen = (psTemp-psOutData);

	return 0;
}

int SetTCDE55(void *pstTranLog, uchar *psOutData, int *piOutLen)
{
#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("SetTCDE55 = ", &countprocEMV, 1);
#endif	

    *piOutLen = ((TRAN_LOG *)pstTranLog)->uiIccDataLen;
    memcpy(psOutData, ((TRAN_LOG *)pstTranLog)->sIccData, *piOutLen);
    return 0;
}

// bExpandAll:       TRUE: expand constructed item, FALSE: not
int GetTLVItem(uchar **ppsTLVString, int iMaxLen, SaveTLVData pfSaveData, uchar bExpandAll)
{
	int			iRet;
	uchar		*psTag, *psSubTag;
	uint		uiTag, uiLenBytes;
	ulong		lTemp;

	// skip null tags
	for(psTag=*ppsTLVString; psTag<*ppsTLVString+iMaxLen; psTag++)
	{
		if( (*psTag!=TAG_NULL_1) && (*psTag!=TAG_NULL_2) )
		{
			break;
		}
	}
	if( psTag>=*ppsTLVString+iMaxLen )
	{
		*ppsTLVString = psTag;
		return 0;	// no tag available
	}

	// process tag bytes
	uiTag = *psTag++;
	if( (uiTag & TAGMASK_FIRSTBYTE)==TAGMASK_FIRSTBYTE )
	{	// have another byte
		uiTag = (uiTag<<8) + *psTag++;
	}
	if( psTag>=*ppsTLVString+iMaxLen )
	{
		return -1;
	}

	// process length bytes
	if( (*psTag & LENMASK_NEXTBYTE)==LENMASK_NEXTBYTE )
	{
		uiLenBytes = *psTag & LENMASK_LENBYTES;
		PubChar2Long(psTag+1, uiLenBytes, &lTemp);
	}
	else
	{
		uiLenBytes = 0;
		lTemp      = *psTag & LENMASK_LENBYTES;
	}
	psTag += uiLenBytes+1;
	if( psTag+lTemp>*ppsTLVString+iMaxLen )
	{
		return -2;
	}
	*ppsTLVString = psTag+lTemp;	// advance pointer of TLV string

	// save data
	(*pfSaveData)(uiTag, psTag, (int)lTemp);
	if( !IsConstructedTag(uiTag) || !bExpandAll )
	{
		return 0;
	}

	// constructed data
	for(psSubTag=psTag; psSubTag<psTag+lTemp; )
	{
		iRet = GetTLVItem(&psSubTag, psTag+lTemp-psSubTag, pfSaveData, TRUE);
		if( iRet<0 )
		{
			return iRet;
		}
	}

	return 0;
}

int GetSpecTLVItem(uchar *psTLVString, int iMaxLen, uint uiSearchTag, uchar *psOutTLV, ushort *puiOutLen)
{
	uchar		*psTag, *psTagStr;
	uint		uiTag, uiLenBytes;
	ulong		lTemp;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("GetSpecTLVItem = ", &countprocEMV, 1);
#endif	

	// skip null tags
    for (psTag=psTLVString; psTag<psTLVString+iMaxLen; psTag++)
    {
        if ((*psTag!=TAG_NULL_1) && (*psTag!=TAG_NULL_2))
        {
            break;
        }
    }
    if ( psTag>=psTLVString+iMaxLen )
    {
        return -1;	// no tag available
    }
    
    while (1)
    {
        psTagStr = psTag;
        // process tag bytes
        uiTag = *psTag++;
        if ((uiTag & TAGMASK_FIRSTBYTE)==TAGMASK_FIRSTBYTE)
        {	// have another byte
            uiTag = (uiTag<<8) + *psTag++;
        }
        if (psTag>=psTLVString+iMaxLen)
        {
            return -2;	// specific tag not found
        }
        
        // process length bytes
        if ((*psTag & LENMASK_NEXTBYTE)==LENMASK_NEXTBYTE)
        {
            uiLenBytes = *psTag & LENMASK_LENBYTES;
            PubChar2Long(psTag+1, uiLenBytes, &lTemp);
        }
        else
        {
            uiLenBytes = 0;
            lTemp      = *psTag & LENMASK_LENBYTES;
        }
        psTag += uiLenBytes+1;
        if (psTag+lTemp>psTLVString+iMaxLen)
        {
            return -2;	// specific tag not found also
        }
        
        // Check if tag needed
        if (uiTag==uiSearchTag)
        {
            *puiOutLen = (ushort)(psTag-psTagStr+lTemp);
            memcpy(psOutTLV, psTagStr, *puiOutLen);
            return 0;
        }
        
        if (IsConstructedTag(uiTag))
        {
            if (GetSpecTLVItem(psTag, (int)lTemp, uiSearchTag, psOutTLV, puiOutLen)==0)
            {
                return 0;
            }
        }
        
        psTag += lTemp;	// advance pointer of TLV string
        if (psTag>=psTLVString+iMaxLen)
        {
            return -2;
        }
    }
}


int GetDE55Amex(uchar *psSendHeader, uchar *psRecvDE55, int iLen)
{
	uchar	*psTmp;
	uint	uiLenBytes;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("GetDE55Amex = ", &countprocEMV, 1);
#endif	

	// invalid length
	if (iLen<6)
	{
		return -1;
	}
	// header mismatch
	if (memcmp(psSendHeader, psRecvDE55, 6)!=0)
	{
		return -1;
	}

	psTmp = psRecvDE55+6;

	// Data Sub Field 1 : Issuer Authentication Data EMV (Tag 91)
	uiLenBytes = *psTmp++;
	if (uiLenBytes>16)
	{
		return -2;
	}
	memcpy(sAuthData, psTmp, uiLenBytes);
	sgAuthDataLen = uiLenBytes;
	psTmp += uiLenBytes;
	if (psTmp-psRecvDE55>iLen)
	{
		return -3;
	}
	if (psTmp-psRecvDE55==iLen)	// end up
	{
		return 0;
	}

	// Data Sub Field 2 : Issuer Script Data
	uiLenBytes = *psTmp++;
	if (uiLenBytes>128)
	{
		return -2;
	}
	sgScriptLen = 0;
	memcpy(&sIssuerScript[sgScriptLen], psTmp, uiLenBytes);
	sgScriptLen += uiLenBytes;
	psTmp += uiLenBytes;
	if (psTmp-psRecvDE55>iLen)
	{
		return -3;
	}
	if (psTmp-psRecvDE55==iLen)	// end up
	{
		return 0;
	}

	return 0;
}

int IsConstructedTag(uint uiTag)
{
	int		i;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("IsConstructedTag = ", &countprocEMV, 1);
#endif	

	for(i=0; (uiTag&0xFF00) && i<2; i++)
	{
		uiTag >>= 8;
	}

	return ((uiTag & TAGMASK_CONSTRUCTED)==TAGMASK_CONSTRUCTED);
}

// Save Iuuser Authentication Data, Issuer Script.
void SaveRspICCData(uint uiTag, uchar *psData, int iDataLen)
{

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("SaveRspICCData = ", &countprocEMV, 1);
#endif	

	switch( uiTag )
	{
	case 0x91:
		memcpy(sAuthData, psData, MIN(iDataLen, 16));
		sgAuthDataLen = MIN(iDataLen, 16);
		break;

	case 0x71:
	case 0x72:
		sIssuerScript[sgScriptLen++] = (uchar)uiTag;
		if( iDataLen>127 )
		{
			sIssuerScript[sgScriptLen++] = 0x81;
		}
		sIssuerScript[sgScriptLen++] = (uchar)iDataLen;
		memcpy(&sIssuerScript[sgScriptLen], psData, iDataLen);
		sgScriptLen += iDataLen;
		break;

	case 0x9F36:
//		memcpy(sATC, psData, MIN(iDataLen, 2));	// ignore
		break;

	default:
		break;
	}
}

// 只处理基本数据元素Tag,不包括结构/模板类的Tag
// Build basic TLV data, exclude structure/template.
void BuildTLVString(ushort uiEmvTag, uchar *psData, int iLength, uchar **ppsOutData)
{
	uchar	*psTemp;


	if( iLength<0 )
	{
		return;
	}

	// 设置TAG
	// write tag
	psTemp = *ppsOutData;
	if( uiEmvTag & 0xFF00 )
	{
		*psTemp++ = (uchar)(uiEmvTag >> 8);
	}
	*psTemp++ = (uchar)uiEmvTag;

	// 设置Length
	// write length
	if( iLength<=127 )	// 目前数据长度均小余127字节,但仍按标准进行处理
	{
		*psTemp++ = (uchar)iLength;
	}
	else
	{	// EMV规定最多255字节的数据
		*psTemp++ = 0x81;
		*psTemp++ = (uchar)iLength;
	}

	// 设置Value
	// write value
	if( iLength>0 )
	{
		memcpy(psTemp, psData, iLength);
		psTemp += iLength;
	}

	*ppsOutData = psTemp;
}

// Retrieve EMV data from core, for saving record or upload use.
void SaveEmvData(void)
{
	int		iLength;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("SaveEmvData = ", &countprocEMV, 1);
#endif	

	EMVGetTLVData(0x9F26, glProcInfo.stTranLog.sAppCrypto, &iLength);
	EMVGetTLVData(0x8A,   glProcInfo.stTranLog.szRspCode,  &iLength);
	EMVGetTLVData(0x95,   glProcInfo.stTranLog.sTVR,       &iLength);
	EMVGetTLVData(0x9B,   glProcInfo.stTranLog.sTSI,       &iLength);
	EMVGetTLVData(0x9F36, glProcInfo.stTranLog.sATC,       &iLength);

	// save for upload
	SetDE55(DE55_UPLOAD, glProcInfo.stTranLog.sIccData, &iLength);
	glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;

	if( glProcInfo.ucOnlineStatus!=ST_ONLINE_APPV )
	{	// ICC脱机, offline approved
		SaveTVRTSI(TRUE);
		GetNewTraceNo();
//		sprintf((char *)glProcInfo.stTranLog.szRspCode, "00");
//		sprintf((char *)glProcInfo.stTranLog.szCondCode, "06");
		sprintf((char *)glProcInfo.stTranLog.szAuthCode, "%06lu", glSysCtrl.ulSTAN);
	}
}

// core cannot process correctly if length of 9F18 is zero
// eg, 71 12 9F 18 00 86 0D 84 1E 00 00 08 11 22 33 44 55 66 77 88
void AdjustIssuerScript(void)
{
	int		iRet;
	uchar	*psTemp;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("AdjustIssuerScript = ", &countprocEMV, 1);
#endif	

	memset(sgScriptBak, 0, sizeof(sgScriptBak));
	memset(&sgScriptInfo, 0, sizeof(sgScriptInfo));
	sgCurScript = sgScriptBakLen = 0;
	bHaveScript  = FALSE;
	for(psTemp=sIssuerScript; psTemp<sIssuerScript+sgScriptLen; )
	{
		iRet = GetTLVItem(&psTemp, sIssuerScript+sgScriptLen-psTemp, SaveScriptData, TRUE);
		if( iRet<0 )
		{
			return;
		}
	}
	if( bHaveScript && sgCurScript>0 )
	{
		PackScriptData();
	}

	memcpy(sIssuerScript, sgScriptBak, sgScriptBakLen);
	sgScriptLen = sgScriptBakLen;
}

// callback function for process issuer script
void  SaveScriptData(uint uiTag, uchar *psData, int iDataLen)
{

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("SaveScriptData = ", &countprocEMV, 1);
#endif	

	switch( uiTag )
	{
	case 0x71:
	case 0x72:
		if( bHaveScript && sgCurScript>0 )
		{
			PackScriptData();
		}
		sgScriptInfo.uiTag = uiTag;
		bHaveScript = TRUE;
		break;

	case 0x9F18:
		sgScriptInfo.iIDLen = MIN(4, iDataLen);
		memcpy(sgScriptInfo.sScriptID, psData, MIN(4, iDataLen));
		break;

	case 0x86:
		sgScriptInfo.iCmdLen[sgCurScript] = iDataLen;
		memcpy(sgScriptInfo.sScriptCmd[sgCurScript], psData, iDataLen);
		sgCurScript++;
		break;

	default:
		break;
	}
}

void PackTLVData(uint uiTag, uchar *psData, uint uiDataLen, uchar *psOutData, int *piOutLen)
{
	int		iHeadLen;

	PackTLVHead(uiTag, uiDataLen, psOutData, &iHeadLen);
	memcpy(psOutData+iHeadLen, psData, uiDataLen);
	*piOutLen = (uiDataLen+iHeadLen);
}

void PackTLVHead(uint uiTag, uint uiDataLen, uchar *psOutData, int *piOutLen)
{
	uchar	*psTemp;

	// pack tag bytes
	psTemp = psOutData;
	if( uiTag & 0xFF00 )
	{
		*psTemp++ = uiTag>>8;
	}
	*psTemp++ = uiTag;

	// pack length bytes
	if( uiDataLen<=127 )
	{
		*psTemp++ = (uchar)uiDataLen;
	}
	else
	{
		*psTemp++ = LENMASK_NEXTBYTE|0x01;	// one byte length
		*psTemp++ = (uchar)uiDataLen;
	}

	*piOutLen = (psTemp-psOutData);
}

int CalcTLVTotalLen(uint uiTag, uint uiDataLen)
{
	int		iLen;

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("CalcTLVTotalLen = ", &countprocEMV, 1);
#endif	

	// get length of TLV tag bytes
	iLen = 1;
	if( uiTag & 0xFF00 )
	{
		iLen++;
	}

	// get length of TLV length bytes
	iLen++;
	if( uiDataLen>127 )
	{
		iLen++;
	}

	return (iLen+uiDataLen);
}

// re-generate issuer script(remove issuer script ID, if the length is zero)
void PackScriptData(void)
{
	int		iCnt, iTotalLen, iTempLen;

	iTotalLen = 0;
	if( sgScriptInfo.iIDLen>0 )
	{
		iTotalLen += CalcTLVTotalLen(0x9F18, 4);
	}
	for(iCnt=0; iCnt<sgCurScript; iCnt++)
	{
		iTotalLen += CalcTLVTotalLen(0x86, sgScriptInfo.iCmdLen[iCnt]);
	}
	PackTLVHead(sgScriptInfo.uiTag, iTotalLen, &sgScriptBak[sgScriptBakLen], &iTempLen);
	sgScriptBakLen += iTempLen;

	if( sgScriptInfo.iIDLen>0 )
	{
		PackTLVData(0x9F18, sgScriptInfo.sScriptID, 4, &sgScriptBak[sgScriptBakLen], &iTempLen);
		sgScriptBakLen += iTempLen;
	}
	for(iCnt=0; iCnt<sgCurScript; iCnt++)
	{
		PackTLVData(0x86, sgScriptInfo.sScriptCmd[iCnt], sgScriptInfo.iCmdLen[iCnt], &sgScriptBak[sgScriptBakLen], &iTempLen);
		sgScriptBakLen += iTempLen;
	}

	memset(&sgScriptInfo, 0, sizeof(sgScriptInfo));
	sgCurScript = 0;
}

// save EMV status for FUNC 9
void SaveTVRTSI(uchar bBeforeOnline)
{
	int				iRet, iLength, iCnt;
	uchar			sTermAID[16], sBuff[512];
	uchar			*psTLVData;
	EMV_APPLIST		stEmvApp;
	DE55Tag stList[] =
	{
		{0x5A,   DE55_MUST_SET, 0},
		{0x5F2A, DE55_MUST_SET, 0},
		{0x5F34, DE55_MUST_SET, 0},
		{0x82,   DE55_MUST_SET, 0},
		{0x84,   DE55_MUST_SET, 0},
		{0x8A,   DE55_MUST_SET, 0},
		{0x95,   DE55_MUST_SET, 0},
		{0x9A,   DE55_MUST_SET, 0},
		{0x9C,   DE55_MUST_SET, 0},
		{0x9F02, DE55_MUST_SET, 0},
		{0x9F03, DE55_MUST_SET, 0},
		{0x9F09, DE55_MUST_SET, 0},
		{0x9F10, DE55_MUST_SET, 0},
		{0x9F1A, DE55_MUST_SET, 0},
		{0x9F1E, DE55_MUST_SET, 0},
		{0x9F33, DE55_MUST_SET, 0},
		{0x9F34, DE55_MUST_SET, 0},
		{0x9F35, DE55_MUST_SET, 0},
		{0x9F36, DE55_MUST_SET, 0},
		{0x9F37, DE55_MUST_SET, 0},
		{0x9F41, DE55_MUST_SET, 0},
		{0},
	};

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("SaveTVRTSI = ", &countprocEMV, 1);
#endif	

	SetStdDE55(FALSE, stList, glEmvStatus.sTLV+2, &iLength);
	glEmvStatus.sTLV[0] = iLength/256;
	glEmvStatus.sTLV[1] = iLength%256;

	if (glProcInfo.bIsFirstGAC)
	{
		psTLVData = glEmvStatus.sAppCryptoFirst+2;

		EMVGetTLVData(0x9F26, sBuff, &iLength);
		BuildTLVString(0x9F26, sBuff, iLength, &psTLVData);

		memset(sBuff, 0, sizeof(sBuff));
		EMVGetTLVData(0x9F27, sBuff, &iLength);
		BuildTLVString(0x9F27, sBuff, iLength, &psTLVData);

		iLength = psTLVData - glEmvStatus.sAppCryptoFirst - 2;
		glEmvStatus.sAppCryptoFirst[0] = iLength/256;
		glEmvStatus.sAppCryptoFirst[1] = iLength%256;

		glEmvStatus.GenACCID1st = sBuff[0];
	}
	else
	{
		psTLVData = glEmvStatus.sAppCryptoSecond+2;

		EMVGetTLVData(0x9F26, sBuff, &iLength);
		BuildTLVString(0x9F26, sBuff, iLength, &psTLVData);

		memset(sBuff, 0, sizeof(sBuff));
		EMVGetTLVData(0x9F27, sBuff, &iLength);
		BuildTLVString(0x9F27, sBuff, iLength, &psTLVData);

		iLength = psTLVData - glEmvStatus.sAppCryptoSecond - 2;
		glEmvStatus.sAppCryptoSecond[0] = iLength/256;
		glEmvStatus.sAppCryptoSecond[1] = iLength%256;

		glEmvStatus.GenACCID2nd= sBuff[0];
	}

	if( bBeforeOnline )
	{
		EMVGetTLVData(0x95,   glEmvStatus.sgTVROld,  &iLength);
		EMVGetTLVData(0x9B,   glEmvStatus.sgTSIOld,  &iLength);
		glEmvStatus.sgARQCLenOld = 0;
		EMVGetTLVData(0x9F10, glEmvStatus.sgARQCOld, &glEmvStatus.sgARQCLenOld);

		EMVGetTLVData(0x9F0E, glEmvStatus.sgIACDeinal, &iLength);
		EMVGetTLVData(0x9F0F, glEmvStatus.sgIACOnline, &iLength);
		EMVGetTLVData(0x9F0D, glEmvStatus.sgIACDefault, &iLength);

		// search TAC from terminal parameter
		memset(sTermAID, 0, sizeof(sTermAID));
		EMVGetTLVData(0x9F06, sTermAID, &iLength);
		for(iCnt=0; iCnt<MAX_APP_NUM; iCnt++)
		{
			memset(&stEmvApp, 0, sizeof(EMV_APPLIST));
			iRet = EMVGetApp(iCnt, &stEmvApp);
			if( iRet!=EMV_OK )
			{
				continue;
			}
			if( memcmp(sTermAID, stEmvApp.AID, stEmvApp.AidLen)==0 )
			{
				memcpy(glEmvStatus.sgTACDeinal,  stEmvApp.TACDenial,  5);
				memcpy(glEmvStatus.sgTACOnline,  stEmvApp.TACOnline,  5);
				memcpy(glEmvStatus.sgTACDefault, stEmvApp.TACDefault, 5);
				break;
			}
		}
	}
	else
	{
		EMVGetTLVData(0x95,   glEmvStatus.sgTVRNew,  &iLength);
		EMVGetTLVData(0x9B,   glEmvStatus.sgTSINew,  &iLength);
	}
	SaveEmvStatus();
}

void UpdateEntryModeForOfflinePIN(void)
{
	int		iRet, iLength;
	uchar	sTemp[64];

#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("UpdateEntryModeForOfflinePIN = ", &countprocEMV, 1);
#endif	

	if ( !(glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) )
	{
		return;
	}

	memset(sTemp, 0, sizeof(sTemp));
	iRet = EMVGetTLVData(0x9F34, sTemp, &iLength);
	if( iRet==EMV_OK )
	{
		sTemp[0] &= 0x3F;
		if (sTemp[2]==0x02)		// last CVM succeed
		{
			if (sTemp[0]==0x01 ||	// plaintext PIN
				sTemp[0]==0x03 ||	// plaintext PIN and signature
				sTemp[0]==0x04 ||	// enciphered PIN
				sTemp[0]==0x05)		// enciphered PIN and signature
			{
				glProcInfo.stTranLog.uiEntryMode |= MODE_OFF_PIN;
			}
		}
		
	}
}
      
// show last EMV status
void ViewTVR_TSI(void)
{
	int		iTemp;
	uchar	szBuff[20];


	if( PasswordBank()!=0 )
	{
		return;
	}


//#ifdef __EMV_DEBUG__
countprocEMV++;
	hex_dump_char("ViewTVR_TSI = ", &countprocEMV, 1);
//#endif	

	LoadEmvStatus();

	ScrCls();
	ScrPrint(0, 0, ASCII, "Before TSI=%02X %02X", glEmvStatus.sgTSIOld[0], glEmvStatus.sgTSIOld[1]);
	ScrPrint(0, 1, ASCII, "TVR=%02X %02X %02X %02X %02X",
			glEmvStatus.sgTVROld[0], glEmvStatus.sgTVROld[1], glEmvStatus.sgTVROld[2],
			glEmvStatus.sgTVROld[3], glEmvStatus.sgTVROld[4]);
	ScrPrint(0, 2, ASCII, "IssuAppData=");
	ScrGotoxy(0, 3);
	ScrFontSet(ASCII);
	for(iTemp=0; iTemp<glEmvStatus.sgARQCLenOld; iTemp++)
	{
#if defined(_P60_S1_) || defined(_P70_S_)
		printf("%02X", glEmvStatus.sgARQCOld[iTemp]);
#else	
		Lcdprintf("%02X", glEmvStatus.sgARQCOld[iTemp]);
#endif
	}
	PubWaitKey(USER_OPER_TIMEOUT);

	ScrCls();
	ScrPrint(0, 0, ASCII, "After TSI=%02X %02X", glEmvStatus.sgTSINew[0], glEmvStatus.sgTSINew[1]);
	ScrPrint(0, 1, ASCII, "TVR=%02X %02X %02X %02X %02X",
			glEmvStatus.sgTVRNew[0], glEmvStatus.sgTVRNew[1], glEmvStatus.sgTVRNew[2],
			glEmvStatus.sgTVRNew[3], glEmvStatus.sgTVRNew[4]);

	PubBcd2Asc0(glEmvStatus.sgTACDeinal, 5, szBuff);
	ScrPrint(0, 2, ASCII, "TACDenial =%10.10s", szBuff);

	PubBcd2Asc0(glEmvStatus.sgTACOnline, 5, szBuff);
	ScrPrint(0, 3, ASCII, "TACOnline =%10.10s", szBuff);

	PubBcd2Asc0(glEmvStatus.sgTACDefault, 5, szBuff);
	ScrPrint(0, 4, ASCII, "TACDefault=%10.10s", szBuff);

	PubBcd2Asc0(glEmvStatus.sgIACDeinal, 5, szBuff);
	ScrPrint(0, 5, ASCII, "IACDenial =%10.10s", szBuff);

	PubBcd2Asc0(glEmvStatus.sgIACOnline, 5, szBuff);
	ScrPrint(0, 6, ASCII, "IACOnline =%10.10s", szBuff);

	PubBcd2Asc0(glEmvStatus.sgIACDefault, 5, szBuff);
	ScrPrint(0, 7, ASCII, "IACDefault=%10.10s", szBuff);

	PubWaitKey(USER_OPER_TIMEOUT);

	ScrCls();
	PubDispString(_T("PRINT DETAIL?"), DISP_LINE_LEFT+4);
	if( PubYesNo(USER_OPER_TIMEOUT) )
	{
		return;
	}

	PrnInit();
	PrnSetNormal();
	PubDebugOutput("FIRST GAC", glEmvStatus.sAppCryptoFirst+2,
					glEmvStatus.sAppCryptoFirst[1],
					DEVICE_PRN, TLV_MODE);
	PubDebugOutput("SECOND GAC", glEmvStatus.sAppCryptoSecond+2,
					glEmvStatus.sAppCryptoSecond[1],
					DEVICE_PRN, TLV_MODE);
	PubDebugOutput("TRANS TLV", glEmvStatus.sTLV+2,
					glEmvStatus.sTLV[1],
					DEVICE_PRN, TLV_MODE);
}


#endif		// #ifdef ENABLE_EMV


// end of file

