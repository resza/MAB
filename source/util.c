
#include "global.h"

/********************** Internal macros declaration ************************/
#define TIMER_TEMPORARY		4
/********************** Internal structure declaration *********************/

/********************** Internal functions declaration *********************/
static void DispBox2(void);
static uchar ChkAcqRestrictForCard(uchar *pszPan);
//static int  MatchCardTable(uchar *pszPAN);
static int  MatchCardTableForInstalment(uchar acq_index);
static int  SelectAcqForCard(uchar *psAcqMatchFlag, uchar *pucCardIndex);
static int	AutoAcqForCard(uchar *psAcqMatchFlag, uchar *pucCardIndex);
static void GetHolderNameFromTrack1(uchar *pszHolderName);
static void ConvertHolderName(uchar *pszOrgName, uchar *pszNormalName);
static int  GetEmvTrackData(void);
static int  GetPanFromTrack(uchar *pszPAN, uchar *pszExpDate);
static int  MatchTrack2AndPan(uchar *pszTrack2, uchar *pszPan);
static int  MatchCardBin(uchar *pszPAN);
static int  GetSecurityCode(void);
static int  DetectCardEvent(uchar ucMode);
static void DispFallBackPrompt(void);
static int  InputAmount(uchar ucAmtType);
static int  VerifyManualPan(void);
static int  GetTipAmount(void);
static int  ExtractPAN(uchar *pszPAN, uchar *pszOutPan);
static void DispWelcomeOnPED(void);
static int  GetExpiry(void);
static void DispOkSub(uchar ucLogoNo);


/********************** Internal variables declaration *********************/

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

#ifdef AREA_Arabia
int CustomizeAppLibForArabiaLang(uchar bSetToArabia)
{
    APPLIB_CONFIG   stAppLibCfg;

    if (bSetToArabia)
    {
        PubGetLibConfig(&stAppLibCfg);
        stAppLibCfg.bStrFromRightToLeft = TRUE;
        stAppLibCfg.pFnShowLeftRightString = ArShowString;  // pass the Arabia string display function into the applib.
        PubSetLibConfig(&stAppLibCfg);
        
		if (ArInit(3)==AR_SUCCESS)
        {
            return 0;
        }
        return -1;
    } 
    else
    {
        PubGetLibConfig(&stAppLibCfg);
        stAppLibCfg.bStrFromRightToLeft = FALSE;
        stAppLibCfg.pFnShowLeftRightString = NULL;
        PubSetLibConfig(&stAppLibCfg);

        if (ArStop()==AR_SUCCESS)
        {
            return 0;
        }
        return -1;
    }
}
#endif

// 初始化交易参数
void InitTransInfo(void)			//irf
{
	memset(&glProcInfo, 0, sizeof(SYS_PROC_INFO));	//clear global data
	glProcInfo.uiRecNo     = 0xFFFF;
	glProcInfo.bIsFirstGAC = TRUE;
	sprintf((char *)glProcInfo.stTranLog.szTipAmount, "%012ld", 0L);

	// set initial transaction currency to local currency at first
#ifdef	__TELIUM_PARAM__
	glProcInfo.stTranLog.stTranCurrency = glTlmSysParam.stEdcInfo.stLocalCurrency;		// initial currency
#else
	glProcInfo.stTranLog.stTranCurrency = glSysParam.stEdcInfo.stLocalCurrency;		// initial currency
#endif
	glProcInfo.stTranLog.uiEntryMode = MODE_NO_INPUT;
	glProcInfo.stTranLog.bPanSeqOK   = FALSE;
	glProcInfo.stTranLog.ulInvoiceNo = glSysCtrl.ulInvoiceNo;
	GetDateTime(glProcInfo.stTranLog.szDateTime);	// set default txn time

#ifdef	__TELIUM_PARAM__
	memcpy(&glCommCfg, &glTlmSysParam.stTxnCommCfg, sizeof(COMM_CONFIG));
#else
	memcpy(&glCommCfg, &glSysParam.stTxnCommCfg, sizeof(COMM_CONFIG));
#endif
}

// 获得新的流水号
ulong GetNewTraceNo(void)
{
	glSysCtrl.ulSTAN++;
	if( !(glSysCtrl.ulSTAN>0 && glSysCtrl.ulSTAN<=999999L) )
	{
		glSysCtrl.ulSTAN = 1L;
	}
	SaveSysCtrlBase();

	return (glSysCtrl.ulSTAN);
}

// calculate new batch number
ulong GetNewBatchNo(ulong ulCurBatchNo)
{
	ulCurBatchNo++;
	if( !(ulCurBatchNo>0 && ulCurBatchNo<=999999L) )
	{
		return 1L;
	}

	return ulCurBatchNo;
}

// 获得新的票据号
ulong GetNewInvoiceNo(void)
{
	glSysCtrl.ulInvoiceNo++;
	if( !(glSysCtrl.ulInvoiceNo>0 && glSysCtrl.ulInvoiceNo<=999999L) )
	{
		glSysCtrl.ulInvoiceNo = 1L;
	}
	SaveSysCtrlBase();

	return (glSysCtrl.ulInvoiceNo);
}


// 获得新的票据号
ulong GetNewTleCountNo(void)
{
	glSysCtrl.TlmReg.regTleCnt++;
	if( !(glSysCtrl.TlmReg.regTleCnt>0 && glSysCtrl.TlmReg.regTleCnt<=999999L) )
	{
		glSysCtrl.TlmReg.regTleCnt = 1L;
	}
	SaveSysCtrlBase();

	return (glSysCtrl.TlmReg.regTleCnt);
}

// 提示拔出IC卡
void PromptRemoveICC(void)
{
	if( !ChkIfEmvEnable() )
	{
		return;
	}

	IccClose(ICC_USER);
	if( IccDetect(ICC_USER)!=0 )	// 如果IC卡已拔出，直接返回。
	{
		return;
	}

	// 显示并等待IC卡拔出
	ScrCls();
	DispTransName();
	PubDispString(_T("PLS REMOVE CARD"), 4|DISP_LINE_CENTER);
	while( IccDetect(ICC_USER)==0 )
	{
		Beep();
		DelayMs(500);
	}
}

//New
void PromptRemoveICCNew(void)
{
	uchar	ATR[2+1];
	if( !ChkIfEmvEnable() )
	{
		return;
	}

	IccClose(ICC_USER);
	IccInit(ICC_USER, ATR);
	if( IccDetect(ICC_USER)!=0 )	// 如果IC卡已拔出，直接返回。
	{
		hex_dump_char("PromptRemoveICCNew", "GA ADA KARTU", 12);
		return;
	}

	// 显示并等待IC卡拔出
	ScrCls();
	DispTransName();
	PubDispString(_T("REMOVE CARD"), 4|DISP_LINE_CENTER);
	while( IccDetect(ICC_USER)==0 )
	{
		Beep();
	}
}


#ifdef ENABLE_EMV
// 删除过期CAPK
// erase expired CAPK
void EraseExpireCAPK(void)
{
	int			iRet, iCnt;
	EMV_CAPK	stCAPK;

	for(iCnt=0; iCnt<MAX_KEY_NUM; iCnt++)
	{
		memset(&stCAPK, 0, sizeof(EMV_CAPK));
		iRet = EMVCheckCAPK(&stCAPK.KeyID, stCAPK.RID);
		if( iRet==EMV_OK )
		{
			break;
		}
		EMVDelCAPK(stCAPK.KeyID, stCAPK.RID);
	}
}
#endif

static int DispMainLogo(void)
{
	uchar	*psLogoData;
	int		iWidth, iHeigh;

	psLogoData = NULL;
	GetNowDispLogo(&psLogoData);
	if (psLogoData!=NULL)
	{
		iWidth = 0;
		iHeigh = 0;
		GetLogoWidthHeigh(psLogoData, &iWidth, &iHeigh);
		ScrDrLogoxy((char)((128-iWidth)/2), 16, psLogoData);
		return 0;
	}
	return -1;
}

static void DispIndicator(void)
{
#ifdef APP_DEBUG
	ScrPrint(0, 2, ASCII, "DEBUG");
#elif defined(EMV_TEST_VERSION) && defined(ENABLE_EMV)
	ScrPrint(0, 2, ASCII, "EMVTEST");
#endif
	if (ChkIfTrainMode())
	{
		ScrPrint(0, 3, ASCII, "DEMO");
	}
}

// 显示刷卡/插卡界面
void DispSwipeCard(uchar bShowLogo)
{
	char *pszStr;

	if (bShowLogo)
	{
		if (DispMainLogo()!=0)
		{
			bShowLogo = FALSE;
		}
	}

	DispIndicator();
//* //4212
	if( ChkIfEmvEnable() )
	{
		pszStr = _T_NOOP("SWIPE/INSERT/TAP ...");
	}
	else
	{
		pszStr = _T_NOOP("PLS SWIPE CARD");
	}
//*/
	if (bShowLogo)
	{
#ifdef	BANK_MANDIRI
	char SoftName[13+1];
	MEM_ZERO(SoftName);
	fmtAppNameAndVersion();
	mapGet(traAppName, SoftName, 13);

//	pszStr = _T_NOOP("        SWM1I.01.01.0");	//irf for mandiri
	pszStr = _T_NOOP(SoftName);	//irf for mandiri
	ScrPrint((6*8), 7, ASCII, pszStr);
#elif	defined(BANK_BRI)
	char SoftName[13+1];
	MEM_ZERO(SoftName);
	fmtAppNameAndVersion();
	mapGet(traAppName, SoftName, 13);
//	pszStr = _T_NOOP("        SWM1I.01.01.0");	//irf for mandiri
	pszStr = _T_NOOP(SoftName);	//irf for mandiri
	ScrPrint((6*8), 7, ASCII, pszStr);
#else
	ScrPrint(0, 7, ASCII, pszStr);
#endif
	}
	else
	{
		PubDispString(_T(pszStr), 4|DISP_LINE_LEFT);
	}

}

void DispSwipeOnly(void)
{
	PubDispString(_T("PLS SWIPE CARD"), 4|DISP_LINE_LEFT);
}

void DispSwipeCardOnly(void)
{
	PubDispString(_T("PLEASE SWIPE"), 4|DISP_LINE_CENTER);
	PubDispString(_T("CARD"), 6|DISP_LINE_CENTER);
}

void DispSwipeAgenCard(void)
{
	PubDispString(_T("PLEASE SWIPE"), 4|DISP_LINE_CENTER);
	PubDispString(_T("CARD"), 6|DISP_LINE_CENTER);
}

void DispSwipeInsertCardOnly(void)
{
	PubDispString(_T("SWIPE/INSERT/TAP"), 4|DISP_LINE_CENTER);
	PubDispString(_T("CARD"), 6|DISP_LINE_CENTER);
}

void DispInsertOnly(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("INSERT/TAP CARD"), 4|DISP_LINE_LEFT);
}

void DispFallBackSwipeCard(void)
{
	PubDispString(_T("PLS SWIPE CARD"), 4|DISP_LINE_LEFT);
	ScrPrint(0, 7, ASCII|REVER, _T("      FALL BACK      "));
}

void DispBlockFunc(void)
{
	ScrClrLine(2, 7);
    PubDispString(_T("TRANS NOT ALLOW"), 4|DISP_LINE_LEFT);
	PubBeepErr();
    PubWaitKey(3);
}

void DispProcess(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("PROCESSING...."), 4|DISP_LINE_LEFT);
//	DispBox2();
}

void DispWait(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("PLEASE WAIT..."), 4|DISP_LINE_LEFT);
}

void DispDial(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("DIALING...."), 4|DISP_LINE_LEFT);
}

void DispConnecting(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("CONNECTING...."), 4|DISP_LINE_LEFT);
}
void DispSend(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("SENDING...."), 4|DISP_LINE_LEFT);
}

void DispReceive(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("RECEIVING..."), 4|DISP_LINE_LEFT);
}

void DispPrinting(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("PRINTING..."), 4|DISP_LINE_LEFT);
}

void DispClearOk(void)
{
	DispOperOk(_T("CLEARED     "));
}

void DispAdjustTitle(void)
{
	PubShowTitle(TRUE, (uchar *)_T("ADJUST          "));
}

void DispVersion(void)
{
	uchar	sBuff[32];

	// Show public version
	PubShowTitle(TRUE, (uchar *)_T("VERSION"));
	sprintf(sBuff, "%s %s", APP_NAME, EDC_VER_PUB);
	if (strlen(sBuff)>16)
	{
		ScrPrint(0, 2, ASCII, "%s", APP_NAME);
		ScrPrint(0, 3, ASCII, "%s", EDC_VER_PUB);
	} 
	else
	{
		ScrPrint(0, 2, CFONT, sBuff);
	}

#ifdef ENABLE_EMV
	// Show EMV lib version (EMV lib updates sometimes)
	memset(sBuff, 0, sizeof(sBuff));
	if (ChkIfEmvEnable() && (EMVReadVerInfo(sBuff)==EMV_OK))
	{
		if (strstr(sBuff, " ")!=NULL)
		{
			*(strstr(sBuff, " ")) = 0;
		}
		ScrPrint(0, 5, ASCII, "EMV :%.16s", sBuff);
	}
#endif

	// Free memory on flash
	ScrPrint(0, 6, ASCII, _T("FREE:%-8ld"), freesize());
	// Downloaded terminal ID
#ifdef	__TELIUM_PARAM__
	ScrPrint(0, 7, ASCII, _T("TERM ID:%.8s"), glTlmSysParam.stEdcInfo.szDownLoadTID);
#else
	ScrPrint(0, 7, ASCII, _T("TERM ID:%.8s"), glSysParam.stEdcInfo.szDownLoadTID);
#endif
	if (PubWaitKey(USER_OPER_TIMEOUT)!=KEYFN)
	{
		return;
	}

    //--------------------------------------------------
	// Internal version
	ScrClrLine(2, 7);
	ScrPrint(0, 2, ASCII, "DETAIL VERSION");
	ScrPrint(0, 3, ASCII, "%s", EDC_VER_INTERN);
#ifdef	__TELIUM_PARAM__
	if (glTlmSysParam.ucTermStatus!=INIT_MODE)
    	{
	    	ScrPrint(0, 4, ASCII, "PARA INIT DATE");
	    	ScrPrint(0, 5, ASCII, "%.14s", glTlmSysParam.stEdcInfo.szInitTime);
    	}
#else
    	if (glSysParam.ucTermStatus!=INIT_MODE)
    	{
	    	ScrPrint(0, 4, ASCII, "PARA INIT DATE");
	    	ScrPrint(0, 5, ASCII, "%.14s", glSysParam.stEdcInfo.szInitTime);
    	}
#endif	
	if (PubWaitKey(USER_OPER_TIMEOUT)!=KEYFN)
	{
		return;
	}
}

// 接收等待回调函数
void DispWaitRspStatus(ushort uiLeftTime)
{
	ScrPrint(87, 3, CFONT, "%-3d", uiLeftTime);
}

void DispBox2(void)
{
	ShowBoxNoShadow(2, 0, 8, 21);
}

void ShowBoxNoShadow(uchar y1, uchar x1, uchar y2, uchar x2)
{
	uchar	i, wn, m, n;

	wn = x2 * 6;
	m = y1 * 8 + 4;
	n = y2 * 8 - 4;
	for(i = x1 * 6 + 1; i < wn; i++)
	{
		ScrPlot(i, m, 1);
		ScrPlot(i, n, 1);
	}

	wn = y2 * 8 - 4;
	m = x1 * 6 + 1;
	n = x2 * 6 - 1;
	for(i = y1 * 8 + 5; i < wn; i++)
	{
		ScrPlot(m, i, 1);
		ScrPlot(n, i, 1);
	}
}

// 输入功能号码
int FunctionInput(void)
{
	uchar szFuncNo[2+1];

	ScrClrLine(2,7);
	PubDispString(_T("FUNCTION ?"), 2|DISP_LINE_LEFT);
	memset(szFuncNo, 0, sizeof(szFuncNo));
	if( PubGetString(NUM_IN, 1, 2, szFuncNo, USER_OPER_TIMEOUT)!=0 )
	{
		return -1;
	}

	return  atoi((char *)szFuncNo);
}

void SysHalt(void)
{
	ScrCls();
	PubDispString(_T("HALT FOR SAFETY "), DISP_LINE_REVER|DISP_LINE_LEFT);
	PubDispString(_T("PLS RESTART POS "), 4|DISP_LINE_LEFT);
	while(1);
}

void SysHaltInfo(void *pszDispInfo, ...)
{
	uchar		szBuff[1024+1];
	va_list		pVaList;

	if( pszDispInfo==NULL || *(uchar *)pszDispInfo==0 )
	{
		return;
	}

	va_start(pVaList, pszDispInfo);
	vsprintf((char*)szBuff, (char*)pszDispInfo, pVaList);
	va_end(pVaList);

	ScrCls();
	PubDispString(_T("HALT FOR SAFETY "), DISP_LINE_REVER|DISP_LINE_LEFT);
	PubDispString(_T(szBuff),            3|DISP_LINE_LEFT);
	while(1);
}

void DispMagReadErr(void)
{
	PubDispString(_T("READ CARD ERR."), 4|DISP_LINE_LEFT);
}

void DispTransName(void)
{
	uchar	szTitle[16+1];

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE_OR_AUTH )
	{
		GetEngTime(szTitle);
		ScrPrint(0, 0, CFONT|REVER, "%-16.16s", szTitle);
	}
	else
	{
#ifdef AREA_Arabia
		if(strcmp(LANGCONFIG, "Arabia") == 0)
		{
			sprintf(szTitle, "%-16.16s", _T(glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel));
			ArShowString(0,SHIFT_RIGHT|EFT_REVERSE,szTitle);	
		}
		else
#endif
		{
			PubDispString(_T(glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel), DISP_LINE_CENTER|DISP_LINE_REVER|0);
//			ScrPrint(0, 0, CFONT|REVER, "%-16.16s", _T(glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel));
		}
	}
}

void DispAccepted(void)
{
	ScrClrLine(2, 7);
	PubDispString(_T("TXN ACCEPTED"), 4|DISP_LINE_LEFT);
}

void DispErrMsg(char *pFirstMsg, char *pSecondMsg, short sTimeOutSec, ushort usOption)
{
	// DERR_BEEP     : error beep

	PubASSERT(pFirstMsg!=NULL);

	ScrClrLine(2, 7);
	if (pSecondMsg==NULL)
	{
		PubShowMsg(4, pFirstMsg);
	}
	else
	{
		PubShowMsg(3, pFirstMsg);
		PubShowMsg(5, pSecondMsg);
	}

	if (usOption & DERR_BEEP)
	{
		PubBeepErr();
	}

	PubWaitKey(sTimeOutSec);
}

void PrintOne(void)
{
	static uchar sPrinterLogo[137] =
	{
		0x04,
		0x00,0x20,
		0x0,0xf8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,
		0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0xf8,

		0x00,0x20,
		0x0,0xff,0x0,0x0,0x0,0x0,0x80,0x40,0x20,0x70,0x4c,0x44,0x54,0x54,0x54,0x54,
		0x54,0x54,0x44,0x44,0x44,0x74,0x2c,0x20,0xa0,0xe0,0xe0,0x0,0x0,0x0,0x0,0xff,

		0x00,0x20,
		0x0,0xff,0x0,0x0,0x0,0x3f,0x21,0xe1,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,
		0x21,0x21,0x21,0x25,0x25,0xe1,0xe1,0x7f,0x1f,0xf,0x7,0x0,0x0,0x0,0x0,0xff,

		0x00,0x20,
		0x0,0xf,0x8,0x8,0x8,0x8,0x8,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,
		0x9,0x9,0x9,0x9,0x9,0x9,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0xf
	};

	ScrGotoxy(86, 3);
	ScrDrLogo(sPrinterLogo);
}

void apiBeef(uchar mode,ushort DlyTime)
{
#if defined(_P78_) || defined(_P80_) || defined(_P90_) || defined(_S_SERIES_) ||  defined(_SP30_) || defined(_P58_)
	Beef(mode, DlyTime);
#else
	kbbeef(mode, DlyTime);
#endif
}

int SelectTransCurrency(void)
{
	// TODO: implement currency selection
	// let  glProcInfo.stTranLog.stTranCurrency = selected currency
	// ...

#ifdef ENABLE_EMV
	EMVGetParameter(&glEmvParam);
	memcpy(glEmvParam.TransCurrCode, glProcInfo.stTranLog.stTranCurrency.sCurrencyCode, 2);
	glEmvParam.TransCurrExp = glProcInfo.stTranLog.stTranCurrency.ucDecimal;
	EMVSetParameter(&glEmvParam);
	// Only in this trasaction, so DON'T back up
#endif

	return 0;
}

// 根据Issuer Key设置glCurIssuer
// set current issuer by key
void FindIssuer(uchar ucIssuerKey)
{
	uchar	ucIndex;
#ifdef	__TELIUM_PARAM__
for(ucIndex=0; ucIndex<glTlmSysParam.ucIssNum;ucIndex++)
	{
		if( glTlmSysParam.Iss[ucIndex].issID==ucIssuerKey )
		{
			SetCurIssuer(ucIndex);
			glProcInfo.stTranLog.ucIssuerKey = ucIssuerKey;
			break;
		}
	}
#else
	for(ucIndex=0; ucIndex<glSysParam.ucIssuerNum; ucIndex++)
	{
		if( glSysParam.stIssuerList[ucIndex].ucKey==ucIssuerKey )
		{
			SetCurIssuer(ucIndex);
			glProcInfo.stTranLog.ucIssuerKey = ucIssuerKey;
			break;
		}
	}
#endif	
}

// 根据Key设置glCurAcq
// set current acquirer by key.
void FindAcq(uchar ucAcqKey)
{
	uchar	ucIndex;

#ifdef	__TELIUM_PARAM__
	for(ucIndex=0; ucIndex<glTlmSysParam.ucAcqNum; ucIndex++)
	{
		if( glTlmSysParam.Acq[ucIndex].acqID==ucAcqKey )
		{
			SetCurAcq(ucIndex);
			glProcInfo.stTranLog.ucAcqKey = ucAcqKey;
			break;
		}
	}
#else
	for(ucIndex=0; ucIndex<glSysParam.ucAcqNum; ucIndex++)
	{
		if( glSysParam.stAcqList[ucIndex].ucKey==ucAcqKey )
		{
			SetCurAcq(ucIndex);
			glProcInfo.stTranLog.ucAcqKey = ucAcqKey;
			break;
		}
	}
#endif	
}

// 根据pszAcqName查找Acq
// Find acquirer by name. Return index if found, else return MAX_ACQ
uchar FindAcqIdxByName(char *pszAcqName, uchar bFullMatch)
{
	uchar	ucIndex;
	
#ifdef	__TELIUM_PARAM__
	for(ucIndex=0; ucIndex<glTlmSysParam.ucAcqNum; ucIndex++)
	{
		if (bFullMatch)
		{
			if( strcmp(glTlmSysParam.Acq[ucIndex].acqName, pszAcqName)==0 )
			{
				return ucIndex;
			}
		}
		else
		{
			if( strstr(glTlmSysParam.Acq[ucIndex].acqName, pszAcqName)!=NULL )
			{
				return ucIndex;
			}
		}
	}
#else
	for(ucIndex=0; ucIndex<glSysParam.ucAcqNum; ucIndex++)
	{
		if (bFullMatch)
		{
			if( strcmp(glSysParam.stAcqList[ucIndex].szName, pszAcqName)==0 )
			{
				return ucIndex;
			}
		}
		else
		{
			if( strstr(glSysParam.stAcqList[ucIndex].szName, pszAcqName)!=NULL )
			{
				return ucIndex;
			}
		}
	}
#endif
	return MAX_ACQ;
}

// 设置当前收单行信息
// set current acquirer by index
void SetCurAcq(uchar ucAcqIndex)
{
#ifdef	__TELIUM_PARAM__
	memcpy(&glCurTlmTmsAcq, &glTlmSysParam.Acq[ucAcqIndex], sizeof(TLM_ACQ));
	glCurTlmTmsAcq.ucIndex = ucAcqIndex;
	mapMove(acqBeg, (word) ucAcqIndex);
#else
	memcpy(&glCurAcq, &glSysParam.stAcqList[ucAcqIndex], sizeof(ACQUIRER));
	glCurAcq.ucIndex = ucAcqIndex;
#endif	
}

// 设置当前发卡行信息
// set current issuer by index
void SetCurIssuer(uchar ucIssuerIndex)
{
#ifdef	__TELIUM_PARAM__
	memcpy(&glCurTlmTmsIss, &glTlmSysParam.Iss[ucIssuerIndex], sizeof(TLM_ISS));
	mapMove(issBeg, (word) ucIssuerIndex);
#else
	memcpy(&glCurIssuer, &glSysParam.stIssuerList[ucIssuerIndex], sizeof(ISSUER));
#endif
}

// 读取磁卡磁道及PAN
int ReadMagCardInfo(void)
{
	int		iRet;

	if( glEdcMsgPtr->MsgType==MAGCARD_MSG )
	{
		glProcInfo.stTranLog.uiEntryMode = MODE_SWIPE_INPUT;
		sprintf((char *)glProcInfo.szTrack1, "%.*s", LEN_TRACK1, glEdcMsgPtr->MagMsg.track1);
		sprintf((char *)glProcInfo.szTrack2, "%.*s", LEN_TRACK2, glEdcMsgPtr->MagMsg.track2);
		sprintf((char *)glProcInfo.szTrack3, "%.*s", LEN_TRACK3, glEdcMsgPtr->MagMsg.track3);
	}
	else
	{
		glProcInfo.stTranLog.uiEntryMode = MODE_SWIPE_INPUT;
		MagRead(glProcInfo.szTrack1, glProcInfo.szTrack2, glProcInfo.szTrack3);
	}

	iRet = GetPanFromTrack(glProcInfo.stTranLog.szPan, glProcInfo.stTranLog.szExpDate);
	if( iRet!=0 )
	{
		DispMagReadErr();
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
	Beep();

	return 0;
}

// 从磁道信息分析出卡号(PAN)
int GetPanFromTrack(uchar *pszPAN, uchar *pszExpDate)
{
	int		iPanLen;
	char	*p, *pszTemp;

	// 从2磁道开始到'＝'
	if( strlen((char *)glProcInfo.szTrack2)>0 )
	{
		pszTemp = (char *)glProcInfo.szTrack2;
	}
	else if( strlen((char *)glProcInfo.szTrack3)>0 )
	{
		pszTemp = (char *)&glProcInfo.szTrack3[2];
	}
	else
	{	// 2、3磁道都没有
		return ERR_SWIPECARD;
	}

	p = strchr((char *)pszTemp, '=');
	if( p==NULL )
	{
		return ERR_SWIPECARD;
	}
	iPanLen = p - pszTemp;
	if( iPanLen<13 || iPanLen>19 )
	{
		return ERR_SWIPECARD;
	}

	sprintf((char *)pszPAN, "%.*s", iPanLen, pszTemp);
	if( pszTemp==(char *)glProcInfo.szTrack2 )
	{
		sprintf((char *)pszExpDate, "%.4s", p+1);
	}
	else
	{
		sprintf((char *)pszExpDate, "0000");
	}

	return 0;
}

// 检测磁道信息是否为IC卡磁道信息
// Check service code in track 2, whther it is 2 or 6
uchar IsChipCardSvcCode(uchar *pszTrack2)
{
	char	*pszSeperator;

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE:
			if(!(glTlmSysParam.Con.appEMSale & EM_EMV_SVC))
				return FALSE;
			break;
		case TRANS_TYPE_REFUND:
			if(!(glTlmSysParam.Con.appEMRefund & EM_EMV_SVC))
				return FALSE;
			break;
		case TRANS_TYPE_OFF_SALE:
			if(!(glTlmSysParam.Con.appEMOffline & EM_EMV_SVC))
				return FALSE;
			break;
		case TRANS_TYPE_CASH:
			if(!(glTlmSysParam.Con.appEMCash & EM_EMV_SVC))
				return FALSE;
			break;
		case TRANS_TYPE_PREAUTH:
			if(!(glTlmSysParam.Con.appEMPreAuth & EM_EMV_SVC))
				return FALSE;
			break;
	}

	if( *pszTrack2==0 )
	{
		return FALSE;
	}

	pszSeperator = strchr((char *)pszTrack2, '=');
	if( pszSeperator==NULL )
	{
		return FALSE;
	}
	if( (pszSeperator[5]=='2') || (pszSeperator[5]=='6') )
	{
		return TRUE;
	}

	return FALSE;
}

// 校验卡号
int ValidPanNo(uchar *pszPanNo)
{
	uchar	bFlag, ucTemp, ucResult;
	uchar	*pszTemp;

	// 是否检查卡号
#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issDoModPAN != TRUE)
	{
		return 0;
	}
#else
	if( !ChkIssuerOption(ISSUER_CHKPAN_MOD10) )
	{
		return 0;
	}
#endif

	// (2121算法)
	bFlag    = FALSE;
	pszTemp  = &pszPanNo[strlen((char *)pszPanNo)-1];
	ucResult = 0;
	while( pszTemp>=pszPanNo )
	{
		ucTemp = (*pszTemp--) & 0x0F;
		if( bFlag )    ucTemp *= 2;
		if( ucTemp>9 ) ucTemp -= 9;
		ucResult = (ucTemp + ucResult) % 10;
		bFlag = !bFlag;
	}

	if( ucResult!=0 )
	{
		ScrClrLine(2,7);
		PubDispString(_T("INVALID CARD"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}

	return 0;
}

// 检查卡的有效期(YYMM)
int ValidCardExpiry(void)
{
	uchar	szDateTime[14+1];
	ulong	ulCardYear, ulCardMonth;
	ulong	ulCurYear, ulCurMonth;
	uchar	ucInvalidFormat;

	glProcInfo.bExpiryError = FALSE;
	ucInvalidFormat = FALSE;

	ulCardYear  = PubAsc2Long(glProcInfo.stTranLog.szExpDate, 2, NULL);
	ulCardYear += (ulCardYear>80) ? 1900 : 2000;
	ulCardMonth = PubAsc2Long(glProcInfo.stTranLog.szExpDate+2, 2, NULL);

	GetDateTime(szDateTime);
	ulCurYear  = PubAsc2Long(szDateTime, 4, NULL);
	ulCurMonth = PubAsc2Long(szDateTime+4, 2, NULL);

	if( ulCardMonth>12 || ulCardMonth<1 )
	{
		ucInvalidFormat = TRUE;
		glProcInfo.bExpiryError = TRUE;
	}
	if (//ulCardYear>ulCurYear+20 ||	// 是否需要判断有效期太长的卡?
		ulCardYear<ulCurYear || 
		(ulCurYear==ulCardYear && ulCurMonth>ulCardMonth) )
	{
		glProcInfo.bExpiryError = TRUE;
	}

	if (glProcInfo.bExpiryError)
	{
		if((glCurTlmTmsIss.issExpiryDatReq)||(glCurTlmTmsIss.issCheckExpOff)||(glCurTlmTmsIss.issCheckExpAll))
		{
            		ScrClrLine(2,7);
			if (ucInvalidFormat)
			{
				PubDispString(_T("ERR EXP. FORMAT"), 4|DISP_LINE_LEFT);
			} 
			else
			{
				PubDispString(_T("CARD EXPIRED"), 4|DISP_LINE_LEFT);
			}
			if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
			{	// if EMV expired card, let core to continue process(based upon TACs/IACs)
				DelayMs(1500);
				return 0;
			}
			else
			{
				PubBeepErr();
				PubWaitKey(3);
				return ERR_NO_DISP;
			}
		}
	}

	return 0;
}

// 获取终端当前时间,格式:YYYYMMDDhhmmss
// format : YYYYMMDDhhmmss
void GetDateTime(uchar *pszDateTime)
{
	uchar	sCurTime[7];

	GetTime(sCurTime);
	sprintf((char *)pszDateTime, "%02X%02X%02X%02X%02X%02X%02X", // asli ditutup zaki 29 jan 2014
	//sprintf((char *)pszDateTime, "%02X/ %02X/ %02X%/ 02X%/ 02X%:02X%:02X", 
			(sCurTime[0]>0x80 ? 0x19 : 0x20), sCurTime[0], sCurTime[1],
			sCurTime[2], sCurTime[3], sCurTime[4], sCurTime[5]);
}

int UpdateLocalTime(uchar *pszNewYear, uchar *pszNewDate, uchar *pszNewTime)
{
	uchar	szLocalTime[14+1], sBuffer[16];

	if ((pszNewDate!=0) && (pszNewTime!=0))
	{
		if (pszNewYear==NULL)
		{
			memset(szLocalTime, 0, sizeof(szLocalTime));
			GetDateTime(szLocalTime);

			if ((memcmp(szLocalTime+4, "12", 2)==0) &&		// local month is DECEMBER
				(memcmp(pszNewDate, "01", 2)==0))			// received month is JANUARY. local clock slower
			{
				PubAscInc(szLocalTime, 4);		// increase local year
			}
			if ((memcmp(szLocalTime+4, "01", 2)==0) &&		// local month is JANUARY
				(memcmp(pszNewDate, "12", 2)==0))			// received month is DECEMBER. local clock faster
			{
				PubAscDec(szLocalTime, 4);		// increase local year
			}
		}
		else
		{
			memcpy(szLocalTime, pszNewYear, 4);
		}

		memcpy(szLocalTime+4, pszNewDate, 4);	// MMDD
		memcpy(szLocalTime+8, pszNewTime, 6);	// hhmmss

		memset(sBuffer, 0, sizeof(sBuffer));
		PubAsc2Bcd(szLocalTime+2, 12, sBuffer);
		return SetTime(sBuffer);
	}

	return -1;
}

// 英文习惯的日期时间(16 bytes, eg: "OCT07,2006 11:22")
void GetEngTime(uchar *pszCurTime)
{
#ifdef	__TELIUM_PARAM__
	uchar	sCurTime[7];

	GetTime(sCurTime);
	//DD
	sprintf((char *)pszCurTime, "%02X-%02X-%02X%02X %02X:%02X",sCurTime[2],sCurTime[1],
			(sCurTime[0]>0x80 ? 0x19 : 0x20), sCurTime[0], 
			 sCurTime[3], sCurTime[4]);
#else
	uchar	Month[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
	uchar	sCurTime[7], ucMonth;

	GetTime(sCurTime);
	ucMonth = (sCurTime[1]>>4) * 10 + (sCurTime[1] & 0x0F) - 1;
	sprintf((char *)pszCurTime, "%.3s%02X,%02X%02X %02X:%02X", Month[ucMonth],
			sCurTime[2], (sCurTime[0]>0x80 ? 0x19 : 0x20),
			sCurTime[0], sCurTime[3], sCurTime[4]);
#endif
}

// 转换YYYYMMDDhhmmss 到 OCT 07, 2006  11:22
// Convert from ... to ...
void Conv2EngTime(uchar *pszDateTime, uchar *pszEngTime)
{
	uchar	Month[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
	uchar	ucMonth;

#ifdef	__TELIUM_PARAM__
	ucMonth = (uchar)((PubAsc2Long(&pszDateTime[4], 2, NULL)-1) % 12);
	sprintf((char *)pszEngTime, "%2.2s %3.3s %4.4s  %2.2s:%2.2s:%2.2s", pszDateTime+6, Month[ucMonth],
			pszDateTime, pszDateTime+8, pszDateTime+10, pszDateTime+12);
#else
	ucMonth = (uchar)((PubAsc2Long(&pszDateTime[4], 2, NULL)-1) % 12);
	sprintf((char *)pszEngTime, "%3.3s %2.2s, %4.4s  %2.2s:%2.2s", Month[ucMonth],
			pszDateTime+6, pszDateTime, pszDateTime+8, pszDateTime+10);
#endif	
}

// 检查卡号,并确定收单行/发卡行(必须在读出卡号后调用)
// Check PAN, and determine Issuer/Acquirer.
int ValidCard(void)
{
	int		iRet;
	uchar 	szPan[lenPan+1];

	MEM_ZERO(szPan);

/*
#ifdef BANK_BRI
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_PRIVILAGE:
		case TRANS_TYPE_SMSBANKING:
		case TRANS_TYPE_PHONEBANKING:
		case TRANS_TYPE_IBANKING:
		case TRANS_TYPE_TRX_PB_TRANSFER_INQ:
		case TRANS_TYPE_TRX_PB_TRANSFER:
		case TRANS_TYPE_TRX_PB_NO_HP:
		case TRANS_TYPE_INFOSALDO:
		case TRANS_TYPE_INFOSALDO_BANKLAIN:
		case TRANS_TYPE_MINISTATEMENT:
		case TRANS_TYPE_MUTASIREKENING:
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
		//case TRANS_TYPE_FREN_MOBI_HEPI://ditutup 6 Januari 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:

		case TRANS_TYPE_INQ_TRFSESAMABRI:
		case TRANS_TYPE_INQ_TRFANTARBANK:

		case TRANS_TYPE_INQ_TELKOM:
		case TRANS_TYPE_INQ_KARTUHALO:
		case TRANS_TYPE_INQ_KARTUMATRIX:

		case TRANS_TYPE_INQ_PLNPASCABAYAR:
		case TRANS_TYPE_INQ_PLNPRABAYAR:

		case TRANS_TYPE_INQ_KKBRI:
		case TRANS_TYPE_KKCITIBANK:
		case TRANS_TYPE_KTACITIBANK:
		case TRANS_TYPE_KKSCB:
		case TRANS_TYPE_KTASCB:
		case TRANS_TYPE_KKANZ:
		case TRANS_TYPE_KKHSBC:
		case TRANS_TYPE_KTAHSBC:
		case TRANS_TYPE_KKRBS:
		case TRANS_TYPE_KTARBS:

		case TRANS_TYPE_INQ_FIF:
		case TRANS_TYPE_INQ_BAF:
		case TRANS_TYPE_INQ_OTO:
		case TRANS_TYPE_INQ_FINANSIA:
		case TRANS_TYPE_INQ_VERENA:
		case TRANS_TYPE_INQ_WOM:

		case TRANS_TYPE_ZAKAT:
		case TRANS_TYPE_INFAQ:
		case TRANS_TYPE_DPLK:

		case TRANS_TYPE_INQ_GARUDA:
		case TRANS_TYPE_INQ_LIONAIR:
		case TRANS_TYPE_INQ_SRIWIJAYA:

		case TRANS_TYPE_INQ_BRIVA:

		case TRANS_TYPE_INQ_BAYARKULIAH:

		case TRANS_TYPE_AKTIFASIKARTU_PETUGAS:
		case TRANS_TYPE_AKTIFASIKARTU_NASABAH:

		case TRANS_TYPE_REISSUE_PIN_NASABAH:
		case TRANS_TYPE_REISSUE_PIN_PETUGAS:	

		case TRANS_TYPE_UBAHPIN_CS:
		case TRANS_TYPE_UBAHPWD_SPV:
			
		case TRANS_TYPE_ABSENSI:
		case TRANS_TYPE_BRIZZI_TOPUP_ONLINE:

		case TRANS_TYPE_RENEWALKARTU_PETUGAS:
		case TRANS_TYPE_RENEWALKARTU_LAMA:
		case TRANS_TYPE_RENEWALKARTU_BARU:
		case TRANS_TYPE_SETORPASTI:

	
	//	case TRANS_TYPE_INQ_CASHOUT:

		//SSB
		case TRANS_TYPE_SIM_BARU:
		case TRANS_TYPE_SIM_PERPANJANGAN:
		case TRANS_TYPE_SIM_PENINGKATAN:
		case TRANS_TYPE_SIM_PENURUNAN:
		case TRANS_TYPE_SIM_MUTASI_GOL_SAMA:
		case TRANS_TYPE_SIM_MUTASI_PENINGKATAN:
		case TRANS_TYPE_SIM_MUTASI_PENURUNAN:
		case TRANS_TYPE_SIM_HILANG_RUSAK_GOLSAMA:
		case TRANS_TYPE_SIM_HILANG_RUSAK_PENINGKATAN:
		case TRANS_TYPE_SIM_HILANG_RUSAK_PENURUNAN:
		case TRANS_TYPE_UJI_SIMULATOR:
		case TRANS_TYPE_BPKB_BARU:
		case TRANS_TYPE_BPKB_GANTI_PEMILIK:
		case TRANS_TYPE_STCK:
		case TRANS_TYPE_MUTASI_RANMOR:
		case TRANS_TYPE_SSB:

			strcpy(szPan, "90");
			memcpy(szPan+2, glProcInfo.stTranLog.szPan, strlen(glProcInfo.stTranLog.szPan)-2);
			break;
		case TRANS_TYPE_SETOR_INQ_PINJ:
		case TRANS_TYPE_SETOR_INQ_SIMP:
		case TRANS_TYPE_SETOR_PINJAMAN:
		case TRANS_TYPE_SETOR_SIMPANAN:
		case TRANS_TYPE_TARIK_TUNAI:
			//T-BANK
		case TRANS_TYPE_CASH_IN:
			strcpy(szPan, "91");
			memcpy(szPan+2, glProcInfo.stTranLog.szPan, strlen(glProcInfo.stTranLog.szPan)-2);
			break;
		default:
			strcpy(szPan, glProcInfo.stTranLog.szPan);
			break;
	}
#else
		strcpy(szPan, glProcInfo.stTranLog.szPan);
#endif
*/


////// MINI ATM BUKOPIN

switch(glProcInfo.stTranLog.ucTranType)
	{	
		case TRANS_TYPE_PDAM_AETRA_BKPN_INQ:
		case TRANS_TYPE_PDAM_AETRA_BKPN:
		case TRANS_TYPE_PDAM_PALYJA_BKPN_INQ:
		case TRANS_TYPE_PDAM_PALYJA_BKPN:
		case TRANS_TYPE_INQ_KARTUMATRIX:
		case TRANS_TYPE_KARTUMATRIX:
		case TRANS_TYPE_INQ_TELKOM:
		case TRANS_TYPE_TELKOM:
		case TRANS_TYPE_KK_ANZ_BKPN_INQ:
		case TRANS_TYPE_KK_BUKOPIN_BKPN_INQ:
		case TRANS_TYPE_KK_BNI_BKPN_INQ:
		case TRANS_TYPE_KK_HSBC_BKPN_INQ:
		case TRANS_TYPE_KK_ANZ_BKPN:
		case TRANS_TYPE_KK_BUKOPIN_BKPN:
		case TRANS_TYPE_KK_BNI_BKPN:
		case TRANS_TYPE_KK_HSBC_BKPN:
		case TRANS_TYPE_UBAH_PIN_BKPN:
		case TRANS_TYPE_INFO_SALDO_BKPN:
		case TRANS_TYPE_TARIK_TUNAI_BKPN:
		case TRANS_TYPE_SETOR_TUNAI_BKPN_INQ:
		case TRANS_TYPE_SETOR_TUNAI_BKPN:
		case TRANS_TYPE_INQ_TRFANTARBANK_BKPN:
		case TRANS_TYPE_TRFANTARBANK_BKPN:
		case TRANS_TYPE_INQ_MAF_BKPN:
		case TRANS_TYPE_INQ_MCF_BKPN:
		case TRANS_TYPE_INQ_WOM_BKPN:
		case TRANS_TYPE_INQ_BAF_BKPN:
		case TRANS_TYPE_MAF_BKPN:
		case TRANS_TYPE_BAF_BKPN:
		case TRANS_TYPE_MCF_BKPN:
		case TRANS_TYPE_WOM_BKPN:
		case TRANS_TYPE_ESIA:
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_XL:		
			strcpy(szPan, "91");
			memcpy(szPan+2, glProcInfo.stTranLog.szPan, strlen(glProcInfo.stTranLog.szPan)-2);
			break;
		default:
			// for CC Bukopin
			strcpy(szPan, glProcInfo.stTranLog.szPan);
			break;
	}



// end mini atm bukopin



//	iRet = MatchCardTable(glProcInfo.stTranLog.szPan);

//	hex_dump_char("pan", szPan, sizeof(szPan));

	iRet = MatchCardTable(szPan);
	if( iRet!=0 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("UNSUPPORT CARD"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}


	/*
	iRet = ValidPanNo(glProcInfo.stTranLog.szPan);	//check luhn digit
	if( iRet!=0 )
	{
		return iRet;
	}
	*/


#ifdef MINI_ATM_BUKOPIN
	iRet = 0;	// no check luhn digit for mini atm bukopin, ALL PAN VALID. DIKI ADD
	if( iRet!=0 )
	{
		return iRet;
	}
#else
	iRet = ValidPanNo(glProcInfo.stTranLog.szPan);	//check luhn digit
	if( iRet!=0 )
	{
		return iRet;
	}
#endif


	iRet = ValidCardExpiry();
	if( iRet!=0 )
	{
		return iRet;
	}

	CheckCapture();

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INSTALMENT )
	{
		if( glProcInfo.bIsFallBack || glEdcMsgPtr->MsgType==ICCARD_MSG )
		{
			PubDispString(_T("PLS SWIPE AGAIN"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(5);
			return ERR_NEED_SWIPE;
		}
	}

	GetCardHolderName(glProcInfo.stTranLog.szHolderName);
#ifdef	__TELIUM_PARAM__
	if(glTlmSysParam.Con.appTermOpt1 & EDC_TLM_TMS_DISPLAY_PAN)
	{
		iRet = ConfirmPanInfo();
		if( iRet!=0 )
		{
			CommOnHook(FALSE);	//release predial
			return iRet;
		}
	}
#else
	iRet = ConfirmPanInfo();
	if( iRet!=0 )
	{
		CommOnHook(FALSE);	//release predial
		return iRet;
	}
#endif

#ifndef	__TELIUM_PARAM__	//not use
	iRet = GetSecurityCode();
	if( iRet!=0 )
	{
		return iRet;
	}
#endif
	return 0;
}


// Check PAN, and determine Issuer/Acquirer.
int ValidCardCashOut(void)
{
	int		iRet;
	uchar 	szPan[lenPan+1];

	MEM_ZERO(szPan);

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_CASHOUT:
		strcpy(szPan, "91");
			memcpy(szPan+2, glProcInfo.stTranLog.szPan, strlen(glProcInfo.stTranLog.szPan)-2);
			break;
		default:
			strcpy(szPan, glProcInfo.stTranLog.szPan);
			break;
	}

	iRet = MatchCardTable(szPan);
	if( iRet!=0 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("UNSUPPORT CARD"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}

	return 0;
}

// 检查卡号,并确定收单行/发卡行(必须在读出卡号后调用)
// Check PAN, and determine Issuer/Acquirer.
int ValidCardBRIZZI(void)
{
	int		iRet;
	uchar 	szPan[lenPan+1];

	MEM_ZERO(szPan);

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_BRIZZI_INFO_SALDO:
		case TRANS_TYPE_BRIZZI_TOPUP_ONLINE:
		case TRANS_TYPE_BRIZZI_AKTV_DEPOS:
		case TRANS_TYPE_BRIZZI_INFO_DEPOS:
		case TRANS_TYPE_BRIZZI_TOPUP_DEP:
		case TRANS_TYPE_BRIZZI_REDEEM_REQ:
		case TRANS_TYPE_BRIZZI_REDEEM:
		case TRANS_TYPE_BRIZZI_REAKT_REQ:
		case TRANS_TYPE_BRIZZI_REAKTIVASI:
			strcpy(szPan, "92");
			memcpy(szPan+2, glProcInfo.stTranLog.BRI.Brizzi.NoKartu, strlen(glProcInfo.stTranLog.BRI.Brizzi.NoKartu)-2);
			break;
		case TRANS_TYPE_BRIZZI_PURCHASE:
		case TRANS_TYPE_BRIZZI_VOID:
		case TRANS_TYPE_BRIZZI_VOID_REQ:
			strcpy(szPan, "93");
			memcpy(szPan+2, glProcInfo.stTranLog.BRI.Brizzi.NoKartu, strlen(glProcInfo.stTranLog.BRI.Brizzi.NoKartu)-2);
			break;
		default:
			strcpy(szPan, glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
			break;
	}

	iRet = MatchCardTable(szPan);
	if( iRet!=0 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("UNSUPPORT CARD"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}

	return 0;
}


// 获得持卡人姓名(已经转换为打印格式)
// Read and convert holder name to printable format.
void GetCardHolderName(uchar *pszHolderName)
{
#ifdef ENABLE_EMV
	int		iRet, iTagLen;
	uchar	szBuff[40];
#endif
	uchar	szTempName[40];

	*pszHolderName = 0;

#ifdef ENABLE_EMV
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		memset(szBuff, 0, sizeof(szBuff));
		iRet = EMVGetTLVData(0x5F20, szBuff, &iTagLen);
		if( iRet!=EMV_OK )
		{
			return;
		}
		ConvertHolderName(szBuff, szTempName);
	}
	else	// other entry mode, just get it from track 1 data
#endif
	{
		GetHolderNameFromTrack1(szTempName);
	}

	sprintf((char *)pszHolderName, "%.26s", szTempName);
}

// get holder name form track 1, which is identified by '^'
void GetHolderNameFromTrack1(uchar *pszHolderName)
{
	char	*p, *q;
	uchar	szOrgName[50];
	int		iLen;

	*pszHolderName = 0;
	if( glProcInfo.szTrack1[0]==0 )
	{
		return;
	}

	p = strchr((char *)glProcInfo.szTrack1, '^');
	if( p==NULL )
	{
		return;
	}
	p++;
	iLen = strlen(p);

	q = strchr(p, '^');
	if( q!=NULL )
	{
		iLen = MIN(q-p, iLen);
	}

	sprintf((char *)szOrgName, "%.*s", (int)MIN(sizeof(szOrgName)-1, iLen), p);
	ConvertHolderName(szOrgName, pszHolderName);
}

// 转换ISO7813格式人名为打印格式
// "Amex/F D.Mr" --> "Mr F D Amex"
void ConvertHolderName(uchar *pszOrgName, uchar *pszNormalName)
{
	char	*pszTitle, *pszMidName, *pszTemp, szBuff[50];

	sprintf((char *)pszNormalName, "%s", pszOrgName);
	if( *pszOrgName==0 )
	{
		return;
	}
	pszTemp = (char *)pszNormalName;

	pszMidName = strchr((char *)pszOrgName, '/');
	if( pszMidName==NULL )
	{
		return;
	}

	pszTitle = strrchr((char *)pszOrgName, '.');
	if( pszTitle!=NULL )
	{
		sprintf(szBuff, "%s ", pszTitle+1);
		PubTrimStr((uchar *)szBuff);
		pszTemp += sprintf(pszTemp, "%s ", szBuff);

		sprintf(szBuff, "%.*s ", (int)(pszTitle-pszMidName-1), pszMidName+1);
		PubTrimStr((uchar *)szBuff);
		pszTemp += sprintf(pszTemp, "%s ", szBuff);
	}
	else
	{
		sprintf(szBuff, "%s", pszMidName+1);
		PubTrimStr((uchar *)szBuff);
		pszTemp += sprintf(pszTemp, "%s ", szBuff);
	}
	sprintf(pszTemp, "%.*s", (int)(pszMidName-(char *)pszOrgName), pszOrgName);
}

// check whether really match to specific acquirer, due to customized conditions
uchar ChkAcqRestrictForCard(uchar *pszPan)
{
	//if (ChkIfxxx())

	//...

	return TRUE;
}

#ifdef	__TELIUM_PARAM__
// determine glCurAcq and glCurIssuer, due to ACQ-ISS-CARD matching table.
int MatchCardTable(uchar *pszPAN)
{
	int			iRet;
	uchar		ucCnt, ucPanLen, ucAcqNum, ucLastAcqIdx;
	uchar		sPanHeader[5], sCardIndex[dimAcq], sAcqMatchFlag[dimAcq];
	TLM_BIN		*tlmpstCardTbl;

	memset(sCardIndex,    0, sizeof(sCardIndex));
	memset(sAcqMatchFlag, 0, sizeof(sAcqMatchFlag));

	// create a list of matched acquirer.
	ucPanLen = strlen((char *)pszPAN);
	PubAsc2Bcd(pszPAN, 10, sPanHeader);
	//hex_dump_char("pszPAN", pszPAN, strlen(pszPAN));
	//hex_dump_char("sPanHeader", sPanHeader, 5);
	
	for(ucAcqNum=ucCnt=0; ucCnt<glTlmSysParam.ucBinNum; ucCnt++)
	{
		tlmpstCardTbl = &glTlmSysParam.Bin[ucCnt];
		//hex_dump_char("binLo", tlmpstCardTbl->binLo, 6);
		//hex_dump_char("binHi", tlmpstCardTbl->binHi, 6);

		/*
		if( (memcmp(tlmpstCardTbl->binLo+1,  sPanHeader, 5)<=0  &&
			 memcmp(tlmpstCardTbl->binHi+1, sPanHeader, 5)>=0) &&
			(tlmpstCardTbl->binPanLen==0 || tlmpstCardTbl->binPanLen==ucPanLen) )
		*/

		if( (memcmp(tlmpstCardTbl->binLo+1,  sPanHeader, 5)<=0  &&
			 memcmp(tlmpstCardTbl->binHi+1, sPanHeader, 5)>=0))
		{
			FindIssuer(tlmpstCardTbl->binCrIssID);
			FindAcq(tlmpstCardTbl->binCrAcqID);

			if (!ChkAcqRestrictForCard(pszPAN))
			{
				continue;
			}

			sCardIndex[glCurTlmTmsAcq.ucIndex] = ucCnt;
			if( glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex]==S_USE  ||
				glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex]==S_PENDING )
			{
				if( sAcqMatchFlag[glCurTlmTmsAcq.ucIndex]==0 )
				{
					sAcqMatchFlag[glCurTlmTmsAcq.ucIndex] = 1;
					ucAcqNum++;
					ucLastAcqIdx = glCurTlmTmsAcq.ucIndex;
				}
			}
			break;	//stop
		}
	}

	// None card table bingo
	if( ucAcqNum==0 )
	{
		return ERR_UNSUPPORT_CARD;
	}

	// Only one card table bingo
	if( ucAcqNum==1 )
	{
		tlmpstCardTbl = &glTlmSysParam.Bin[sCardIndex[ucLastAcqIdx]];
		FindIssuer(tlmpstCardTbl->binCrIssID);
		FindAcq(tlmpstCardTbl->binCrAcqID);

		strcpy(glProcInfo.stTranLog.szCardType, tlmpstCardTbl->binName);
		return 0;
	}

	// More than one bingo
	if( ChkEdcOption(EDC_SELECT_ACQ_FOR_CARD) &&
		(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_INSTALMENT))
	{
		// 非installment时，手工选择收单行
		iRet = SelectAcqForCard(sAcqMatchFlag, &ucCnt);
	}
	else
	{
		// Select automatically. 自动选择一个收单行
		iRet = AutoAcqForCard(sAcqMatchFlag, &ucCnt);
	}
	if( iRet!=0 )
	{
		return iRet;
	}

	tlmpstCardTbl = &glTlmSysParam.Bin[sCardIndex[ucCnt]];
	FindIssuer(tlmpstCardTbl->binCrIssID);
	FindAcq(tlmpstCardTbl->binCrAcqID);
	return 0;
}

#else
// 根据卡号匹配卡表,并最终确定收单行(glCurAca)和发卡行(glCurIssuer)
// determine glCurAcq and glCurIssuer, due to ACQ-ISS-CARD matching table.
int MatchCardTable(uchar *pszPAN)
{
	int			iRet;
	uchar		ucCnt, ucPanLen, ucAcqNum, ucLastAcqIdx;
	uchar		sPanHeader[5], sCardIndex[MAX_ACQ], sAcqMatchFlag[MAX_ACQ];
	CARD_TABLE	*pstCardTbl;

	memset(sCardIndex,    0, sizeof(sCardIndex));
	memset(sAcqMatchFlag, 0, sizeof(sAcqMatchFlag));

	// 建立收单行列表
	// create a list of matched acquirer.
	ucPanLen = strlen((char *)pszPAN);
	PubAsc2Bcd(pszPAN, 10, sPanHeader);
	for(ucAcqNum=ucCnt=0; ucCnt<glSysParam.ucCardNum; ucCnt++)
	{
		pstCardTbl = &glSysParam.stCardTable[ucCnt];
		if( (memcmp(pstCardTbl->sPanRangeLow,  sPanHeader, 5)<=0  &&
			 memcmp(pstCardTbl->sPanRangeHigh, sPanHeader, 5)>=0) &&
			(pstCardTbl->ucPanLength==0 || pstCardTbl->ucPanLength==ucPanLen) )
		{
			FindIssuer(pstCardTbl->ucIssuerKey);
			FindAcq(pstCardTbl->ucAcqKey);

			if (!ChkAcqRestrictForCard(pszPAN))
			{
				continue;
			}

			sCardIndex[glCurAcq.ucIndex] = ucCnt;
			if( glSysCtrl.sAcqStatus[glCurAcq.ucIndex]==S_USE  ||
				glSysCtrl.sAcqStatus[glCurAcq.ucIndex]==S_PENDING )
			{
				if( sAcqMatchFlag[glCurAcq.ucIndex]==0 )
				{
					sAcqMatchFlag[glCurAcq.ucIndex] = 1;
					ucAcqNum++;
					ucLastAcqIdx = glCurAcq.ucIndex;
				}
			}
		}
	}

	// None card table bingo
	if( ucAcqNum==0 )
	{
		return ERR_UNSUPPORT_CARD;
	}

	// Only one card table bingo
	if( ucAcqNum==1 )
	{
		pstCardTbl = &glSysParam.stCardTable[sCardIndex[ucLastAcqIdx]];
		FindIssuer(pstCardTbl->ucIssuerKey);
		FindAcq(pstCardTbl->ucAcqKey);
		return 0;
	}

	// More than one bingo
	if( ChkEdcOption(EDC_SELECT_ACQ_FOR_CARD) &&
		(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_INSTALMENT))
	{
		// 非installment时，手工选择收单行
		iRet = SelectAcqForCard(sAcqMatchFlag, &ucCnt);
	}
	else
	{
		// Select automatically. 自动选择一个收单行
		iRet = AutoAcqForCard(sAcqMatchFlag, &ucCnt);
	}
	if( iRet!=0 )
	{
		return iRet;
	}

	pstCardTbl = &glSysParam.stCardTable[sCardIndex[ucCnt]];
	FindIssuer(pstCardTbl->ucIssuerKey);
	FindAcq(pstCardTbl->ucAcqKey);
	return 0;
}
#endif

int MatchCardTableForInstalment(uchar ucIndex)
{
	uchar		ucCnt, ucPanLen, ucAcqNum;
	uchar		sPanHeader[5], sCardIndex[MAX_ACQ], sAcqMatchFlag[MAX_ACQ];
#ifdef	__TELIUM_PARAM__
	TLM_BIN		*tlmpstCardTbl;
#else
	CARD_TABLE	*pstCardTbl;
#endif

	memset(sCardIndex,    0, sizeof(sCardIndex));
	memset(sAcqMatchFlag, 0, sizeof(sAcqMatchFlag));

	// 建立收单行列表
	// create a list of matched acquirer.
	ucPanLen = strlen((char *)glProcInfo.stTranLog.szPan);
	PubAsc2Bcd(glProcInfo.stTranLog.szPan, 10, sPanHeader);

#ifdef	__TELIUM_PARAM__
	for(ucAcqNum=ucCnt=0; ucCnt<glTlmSysParam.ucBinNum; ucCnt++)
	{
		tlmpstCardTbl = &glTlmSysParam.Bin[ucCnt];
		if( (memcmp(tlmpstCardTbl->binLo+1,  sPanHeader, 5)<=0  &&
			 memcmp(tlmpstCardTbl->binHi+1, sPanHeader, 5)>=0) &&
			(tlmpstCardTbl->binPanLen ==0 || tlmpstCardTbl->binPanLen==ucPanLen) )
		{
			FindIssuer(tlmpstCardTbl->binCrIssID);
			FindAcq(tlmpstCardTbl->binCrAcqID);
			
			if (glCurTlmTmsAcq.ucIndex != ucIndex)
			{
				continue;	
			}
			
			if( glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex]==S_USE  ||
				glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex]==S_PENDING )
			{
				return 0;
			}
		}
	}
#else
	for(ucAcqNum=ucCnt=0; ucCnt<glSysParam.ucCardNum; ucCnt++)
	{
		pstCardTbl = &glSysParam.stCardTable[ucCnt];
		if( (memcmp(pstCardTbl->sPanRangeLow,  sPanHeader, 5)<=0  &&
			 memcmp(pstCardTbl->sPanRangeHigh, sPanHeader, 5)>=0) &&
			(pstCardTbl->ucPanLength==0 || pstCardTbl->ucPanLength==ucPanLen) )
		{
			FindIssuer(pstCardTbl->ucIssuerKey);
			FindAcq(pstCardTbl->ucAcqKey);
			
			if (glCurAcq.ucIndex != ucIndex)
			{
				continue;	
			}
			
			if( glSysCtrl.sAcqStatus[glCurAcq.ucIndex]==S_USE  ||
				glSysCtrl.sAcqStatus[glCurAcq.ucIndex]==S_PENDING )
			{
				return 0;
			}
		}
	}
#endif

	return ERR_UNSUPPORT_CARD;
}

// 根据卡号匹配的收单行信息,提示用户选择收单行
// Select acquirer for card. only select in already matched acquirers.
int SelectAcqForCard(uchar *psAcqMatchFlag, uchar *pucCardIndex)
{
	int			iMenuNo, iValidAcqCnt;
	uchar		ucCnt, ucAcqNum, szPrompt[16+1];
	MenuItem	stAcqMenu[MAX_ACQ+1];

	// build menu from candidate acquirer's list
	memset(&stAcqMenu[0], 0, sizeof(stAcqMenu));
#ifdef	__TELIUM_PARAM__
	for(ucAcqNum=ucCnt=0; ucCnt<glTlmSysParam.ucAcqNum; ucCnt++)
	{
		if( psAcqMatchFlag[ucCnt]==0 )
		{
			continue;
		}
		stAcqMenu[ucAcqNum].bVisible = TRUE;
		sprintf((char *)stAcqMenu[ucAcqNum].szMenuName, "%.10s", glTlmSysParam.Acq[ucCnt].acqName);
		ucAcqNum++;
	}
#else
	for(ucAcqNum=ucCnt=0; ucCnt<glSysParam.ucAcqNum; ucCnt++)
	{
		if( psAcqMatchFlag[ucCnt]==0 )
		{
			continue;
		}
		stAcqMenu[ucAcqNum].bVisible = TRUE;
		sprintf((char *)stAcqMenu[ucAcqNum].szMenuName, "%.10s", glSysParam.stAcqList[ucCnt].szName);
		ucAcqNum++;
	}
#endif
	// prompt use select acquirer
	if( ucAcqNum<=9 )
	{
		sprintf((char *)szPrompt, _T("SELECT  HOST"));
	}
	else
	{
		sprintf((char *)szPrompt, _T("SELECT:"));
	}
	iMenuNo = PubGetMenu(szPrompt, stAcqMenu, MENU_AUTOSNO, USER_OPER_TIMEOUT);
	if( iMenuNo<0 )
	{
		return (iMenuNo==-3) ? ERR_USERCANCEL : ERR_SEL_ACQ;
	}

	// search selected acquirer index
#ifdef	__TELIUM_PARAM__
	for(iValidAcqCnt=0,ucCnt=0; ucCnt<glTlmSysParam.ucAcqNum; ucCnt++)
	{
		if( psAcqMatchFlag[ucCnt]==0 )
		{
			continue;
		}
		if( iValidAcqCnt==iMenuNo )
		{
			*pucCardIndex = ucCnt;
			return 0;
		}
		iValidAcqCnt++;
	}
#else
	for(iValidAcqCnt=0,ucCnt=0; ucCnt<glSysParam.ucAcqNum; ucCnt++)
	{
		if( psAcqMatchFlag[ucCnt]==0 )
		{
			continue;
		}
		if( iValidAcqCnt==iMenuNo )
		{
			*pucCardIndex = ucCnt;
			return 0;
		}
		iValidAcqCnt++;
	}
#endif	

	return ERR_SEL_ACQ;
}

// 根据特定的规则匹配最优收单行
int AutoAcqForCard(uchar *psAcqMatchFlag, uchar *pucCardIndex)
{
	uchar	ucCnt;

	// 在这里添加其它匹配规则

	// Select automatically. 自动选择一个收单行
	// if no special regulation, select the first matched.
	// installment时稍候会重新决定acquirer，因此现在可以随便先定一个
#ifdef	__TELIUM_PARAM__
	for(ucCnt=0; ucCnt<glTlmSysParam.ucAcqNum; ucCnt++)
	{
		if( psAcqMatchFlag[ucCnt]!=0 )
		{
			*pucCardIndex = ucCnt;
			return 0;
		}
	}
#else
	for(ucCnt=0; ucCnt<glSysParam.ucAcqNum; ucCnt++)
	{
		if( psAcqMatchFlag[ucCnt]!=0 )
		{
			*pucCardIndex = ucCnt;
			return 0;
		}
	}
#endif
	return ERR_SEL_ACQ;
}

/************************************************************************
* 刷卡事件处理函数
* bCheckICC:    TRUE  检查2磁道的service code(对EMV终端有效)
*               FALSE 不检查
************************************************************************/
int SwipeCardProc(uchar bCheckICC)
{
	int		iRet;

	iRet = ReadMagCardInfo();
	if( iRet!=0 )
	{
		return iRet;
	}

	// 交易不要求判断卡片类型或者为非EMV终端,直接返回
	if( !bCheckICC || !ChkIfEmvEnable() )
	{
		return 0;
	}

	// EMV终端,继续检查
	if( glProcInfo.bIsFallBack==TRUE )
	{
		if( IsChipCardSvcCode(glProcInfo.szTrack2) )
		{	// fallback并且是IC卡,则返回成功
			glProcInfo.stTranLog.uiEntryMode = MODE_FALLBACK_SWIPE;
			return 0;
		}
		else
		{
			DispTransName();
			PubDispString(_T("NON EMV,RE-SWIPE"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(3);
			return ERR_NO_DISP;
		}
	}
	else if( IsChipCardSvcCode(glProcInfo.szTrack2) )
	{
		if( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_OFF_SALE &&
			glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_REFUND   &&
			glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_INSTALMENT )
		{
			DispTransName();
			DispInsertOnly();
			PubBeepErr();
			PubWaitKey(3);
			return ERR_NEED_INSERT;
		}
	}

	return 0;
}

#ifdef ENABLE_EMV
// ICC插卡事件处理函数
// Process insertion event.
int InsertCardProc(void)
{
	int		iRet;
	uchar	szTotalAmt[12+1], sTemp[6];

#ifdef __EMV_DEBUG__
	countprocEMV = 0;
#endif

	// 如果已经FALLBACK,忽略IC卡插卡操作
	/*
	ScrClrLine(3, 3);
	PubDispString(_T("atas debug...."), 4|DISP_LINE_LEFT);
	PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---atas debug--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/
	
	if( glProcInfo.bIsFallBack==TRUE )
	{
		return ERR_NEED_FALLBACK;
	}

	/*
	ScrClrLine(6, 6);
	PubDispString(_T(" bawah debug...."), 4|DISP_LINE_LEFT);
	PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/

	//!!!! deleted: it is fixed and not allowed to modify after GPO.
	//ModifyTermCapForPIN();

	glProcInfo.stTranLog.uiEntryMode = MODE_CHIP_INPUT;

	DispTransName();
	DispProcess();

#ifdef ENABLE_EMV
	InitTransEMVCfg();
#endif

	/*
	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 00-11--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/

	// 应用选择
	// EMV application selection. This is EMV kernel API
	iRet = EMVAppSelect(ICC_USER, glSysCtrl.ulSTAN);
	DispTransName();
	if( iRet==EMV_DATA_ERR || iRet==ICC_RESET_ERR || iRet==EMV_NO_APP ||
		iRet==ICC_CMD_ERR  || iRet==EMV_RSP_ERR )
	{
		glProcInfo.bIsFallBack = TRUE;
		glProcInfo.iFallbackErrCode = iRet;

	/*
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 00-11-22--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/

		
		return ERR_NEED_FALLBACK;
	}
	if( iRet==EMV_TIME_OUT || iRet==EMV_USER_CANCEL )
	{
		return ERR_USERCANCEL;
	}
	if( iRet!=EMV_OK )
	{
		return ERR_TRAN_FAIL;
	}

	/*
	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 11--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/
	

	// Clear log to avoid amount accumulation for floor limit checking
    iRet = EMVClearTransLog();

	// Read Track 2 and/or Pan
	DispTransName();
	iRet = GetEmvTrackData();
	if( iRet!=0 )
	{
		return iRet;
	}

	/*
	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 22--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/
		
    
    // Display app prefer name
    if (strlen(glProcInfo.stTranLog.szAppPreferName)!=0)
    {
        PubDispString(glProcInfo.stTranLog.szAppPreferName, 4|DISP_LINE_LEFT);
        PubWaitKey(1);
    }
    else if (strlen(glProcInfo.stTranLog.szAppLabel)!=0)
    {
        PubDispString(glProcInfo.stTranLog.szAppLabel, 4|DISP_LINE_LEFT);
        PubWaitKey(1);
    }

	iRet = ValidCard();	// 校验卡号
	if( iRet!=0 )
	{
		return iRet;
	}
	UpdateEMVTranType();

	/*
	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 33--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/

		

#ifdef ENABLE_EMV
	// EMVSetMCKParam to set bypass PIN
#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issPinOpt==0x30)
		AppSetMckParam(1);	//pin by pass	/ not required
	else
		AppSetMckParam(0);
#else
	AppSetMckParam(ChkIssuerOption(ISSUER_EN_EMVPIN_BYPASS));
#endif
	//end
#endif

//CICILAN BRING
	if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CICILAN_BRING)
	{
#ifdef BANK_BRI
			iRet = redemEnterMonth();
#endif
			if(iRet != 0)
				return ERR_USERCANCEL;

	}
//CICILAN BUKOPIN
/*	if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CICILAN_BUKOPIN)
	{
			iRet = EnterMonthCicilanBukopin();
			if(iRet != 0)
				return ERR_USERCANCEL;
	}
*/
	// 输入交易金额
	if(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID)
	{
		iRet = GetAmount();
		if( iRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
		//PubAddHeadChars(szTotalAmt, 12, '0');  no need: already 12 digits
		PubAsc2Bcd(szTotalAmt, 12, sTemp);
		EMVSetTLVData(0x9F02, sTemp, 6);
		PubLong2Char((ulong)atol((char *)szTotalAmt), 4, sTemp);
		EMVSetTLVData(0x81, sTemp, 4);
	}

	/*
	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 44--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/
		

	DispTransName();
	DispProcess();

	// 卡片数据认证
	// Card data authentication
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_AUTH ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PREAUTH ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CICILAN_BRING ||
 	       glProcInfo.stTranLog.ucTranType!= TRANS_TYPE_SALE_REDEEMPTION||
 	       glProcInfo.stTranLog.ucTranType!= TRANS_TYPE_CICILAN_BUKOPIN)
	{
		iRet = EMVCardAuth();
		if( iRet!=EMV_OK )
		{
			return ERR_TRAN_FAIL;
		}
	}

	/*
	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 55 end--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		*/

	return 0;
}
#endif

#ifdef ENABLE_EMV
// 读取IC卡磁道信息/卡号信息等
int GetEmvTrackData(void)
{
	int		iRet, iLength;
	uchar	sTemp[50], szCardNo[20+1];
	int		i, bReadTrack2, bReadPan;

	// 读取应用数据
	DispProcess();
	iRet = EMVReadAppData();
	if( iRet==EMV_TIME_OUT || iRet==EMV_USER_CANCEL )
	{
		return ERR_USERCANCEL;
	}
	if( iRet!=EMV_OK )
	{
		return ERR_TRAN_FAIL;
	}
	

	// Read Track 2 Equivalent Data
	bReadTrack2 = FALSE;
	memset(sTemp, 0, sizeof(sTemp));
	iRet = EMVGetTLVData(0x57, sTemp, &iLength);
	if( iRet==EMV_OK )
	{

	/*
	PrnStr("\n");
	PrnStr("---NO EMVGetTLVData !!--");
	PrnStr("\n");
	StartPrinter();
	*/

	
		bReadTrack2 = TRUE;
		PubBcd2Asc0(sTemp, iLength, glProcInfo.szTrack2);
		PubTrimTailChars(glProcInfo.szTrack2, 'F');	// erase padded 'F' chars
		for(i=0; glProcInfo.szTrack2[i]!='\0'; i++)		// convert 'D' to '='
		{
			if( glProcInfo.szTrack2[i]=='D' )
			{
				glProcInfo.szTrack2[i] = '=';
				break;
			}
		}
	}


	/*
	PrnStr("\n");
	PrnStr("---aaaaaaa--");
	PrnStr("\n");
	PrnStr(glProcInfo.szTrack2);
	PrnStr("\n");
	StartPrinter();

	PrnStr("\n");
	PrnStr("---aa11--");
	PrnStr("\n");
	StartPrinter();
	*/

	

	// read PAN
	bReadPan = FALSE;
	memset(sTemp, 0, sizeof(sTemp));
	iRet = EMVGetTLVData(0x5A, sTemp, &iLength);
	if( iRet==EMV_OK )
	{
		PubBcd2Asc0(sTemp, iLength, szCardNo);
		PubTrimTailChars(szCardNo, 'F');		// erase padded 'F' chars

	/*
		PrnStr("\n");
	PrnStr("---aa22--");
	PrnStr("\n");
	PrnStr("\n");
	PrnStr(szCardNo);
	PrnStr("\n");
	StartPrinter();
	*/
	
		if( bReadTrack2 && !MatchTrack2AndPan(glProcInfo.szTrack2, szCardNo) )
		{
			// 如果Track2 & PAN 同时存在,则必须匹配
			// if Track2 & PAN exist at the same time, must match
			PubDispString(_T("CARD ERROR"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(3);
			PromptRemoveICC();

		/*
			PrnStr("\n");
	PrnStr("---aa33--");
	PrnStr("\n");
	StartPrinter();
	*/
	
			return ERR_NO_DISP;
//			return ERR_TRAN_FAIL;
		}


	/*
		PrnStr("\n");
	PrnStr("---aa44--");
	PrnStr("\n");
	StartPrinter();
	*/
	
		sprintf((char *)glProcInfo.stTranLog.szPan, "%.19s", szCardNo);
		bReadPan = TRUE;
	}
	else if ( !bReadTrack2 )	// 如果Track 2 和 PAN 都没有,则交易失败
	{


	/*
	PrnStr("\n");
	PrnStr("---aa55--");
	PrnStr("\n");
	StartPrinter();
	*/
	
		return ERR_TRAN_FAIL;
	}
	if( !bReadPan )	// 没有读取PAN，但是有track 2
	{


	/*
	PrnStr("\n");
	PrnStr("---aa66--");
	PrnStr("\n");
	StartPrinter();
	*/
	
		iRet = GetPanFromTrack(glProcInfo.stTranLog.szPan, glProcInfo.stTranLog.szExpDate);
		if( iRet!=0 )
		{

	/*
		PrnStr("\n");
	PrnStr("---aa77--");
	PrnStr("\n");
	StartPrinter();
	*/
	
			return ERR_TRAN_FAIL;
		}
	}


	/*
	PrnStr("\n");
	PrnStr("---bbb--");
	PrnStr("\n");
	PrnStr(szCardNo);
	PrnStr("\n");
	StartPrinter();
	*/
	

	// read PAN sequence number
	glProcInfo.stTranLog.bPanSeqOK = FALSE;
	iRet = EMVGetTLVData(0x5F34, &glProcInfo.stTranLog.ucPanSeqNo, &iLength);
	if( iRet==EMV_OK )
	{
		glProcInfo.stTranLog.bPanSeqOK = TRUE;
	}

	/*
	PrnStr("\n");
	PrnStr("---ccc--");
	PrnStr("\n");
	PrnStr(szCardNo);
	PrnStr("\n");
	StartPrinter();
	*/

	// read Application Expiration Date
	if( bReadPan )
	{
		memset(sTemp, 0, sizeof(sTemp));
		iRet = EMVGetTLVData(0x5F24, sTemp, &iLength);
		if( iRet==EMV_OK )
		{
			PubBcd2Asc0(sTemp, 2, glProcInfo.stTranLog.szExpDate);
		}
	}

	/*
	PrnStr("\n");
	PrnStr("---ddd--");
	PrnStr("\n");
	PrnStr(szCardNo);
	PrnStr("\n");
	StartPrinter();
	*/
	

    // application label
	EMVGetTLVData(0x50, glProcInfo.stTranLog.szAppLabel, &iLength);	// application label
    // Issuer code table
    iRet = EMVGetTLVData(0x9F11, sTemp, &iLength);
    if ((iRet==0) && (sTemp[0]==0x01))
    {
	    EMVGetTLVData(0x9F12, glProcInfo.stTranLog.szAppPreferName, &iLength);  // Application prefer name
    }
    // Application ID
	iRet = EMVGetTLVData(0x4F, glProcInfo.stTranLog.sAID, &iLength);	// AID
	if( iRet==EMV_OK )
	{
		glProcInfo.stTranLog.ucAidLen = (uchar)iLength;
	}

	// read cardholder name
	memset(sTemp, 0, sizeof(sTemp));
	iRet = EMVGetTLVData(0x5F20, sTemp, &iLength);
	if( iRet==EMV_OK )
	{
		sprintf((char *)glProcInfo.stTranLog.szHolderName, "%.20s", sTemp);
	}

	return 0;
}
#endif

// 比较2磁道信息和PAN是否一致(For ICC)
// Check whether track2 (from ICC) and PAN (from ICC) are same.
int MatchTrack2AndPan(uchar *pszTrack2, uchar *pszPan)
{
	int		i;
	uchar	szTemp[19+1];

	for(i=0; i<19 && pszTrack2[i]!='\0'; i++)
	{
		if( pszTrack2[i]=='=' )
		{
			break;
		}
		szTemp[i] = pszTrack2[i];
	}
	szTemp[i] = 0;

	if( strcmp((char *)szTemp, (char *)pszPan)==0 )
	{
		return TRUE;
	}

	return FALSE;
}

// 确认卡号信息
int ConfirmPanInfo(void)
{
	int		iIndex=-1;
	uchar	ucLine;

	if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_BRIZZI_INFO_SALDO||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_BRIZZI_PURCHASE||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_BRIZZI_INFO_DEPOS||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_BRIZZI_AKTV_DEPOS
		)
		return 0;

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE_OR_AUTH ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE         ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_AUTH         ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PREAUTH ||

		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_BRIZZI_TOPUP_ONLINE
	
		)
	{
		ScrCls();
		DispTransName();
		DispWait();
		PreDial();
	}

	ScrCls();
	DispTransName();
	ucLine = 4;

#ifndef	__TELIUM_PARAM__
	iIndex = MatchCardBin(glProcInfo.stTranLog.szPan);
#endif

	ScrPrint(0, ucLine++, ASCII, "%.20s", glProcInfo.stTranLog.szHolderName);
	ScrPrint(0, ucLine++, ASCII, "%.19s", glProcInfo.stTranLog.szPan);

	if((!glCurTlmTmsIss.issExpiryDate)||(!glCurTlmTmsIss.issExpiryDatReq)||
		(!glCurTlmTmsIss.issCheckExpOff)||(!glCurTlmTmsIss.issCheckExpAll))
	{
		ScrPrint(0, ucLine++, ASCII, "EXP DATE:%2.2s/%2.2s",
				&glProcInfo.stTranLog.szExpDate[2], &glProcInfo.stTranLog.szExpDate[0]);
	}

	/*
	PrnInit();
	PrnSetNormal();
	PrnStr("\n");
	PrnStr("---szHolderName nya :--");
	PrnStr("\n");
	PrnStr(glProcInfo.stTranLog.szHolderName);
	PrnStr("\n");
	StartPrinter();
	*/
	

	if( PubYesNo(USER_OPER_TIMEOUT) )
	{
		return ERR_USERCANCEL;
	}

#ifdef ENABLE_EMV
	// set EMV library parameters
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		EMVGetParameter(&glEmvParam);
		memcpy(glEmvParam.MerchId, glCurTlmTmsAcq.acqMID, 15);
		memcpy(glEmvParam.TermId, glCurTlmTmsAcq.acqTID, 8);
		EMVSetParameter(&glEmvParam);
		// Only in this trasaction, so DON'T back up
	}
#endif

	return 0;
}

// RFU for HK
int MatchCardBin(uchar *pszPAN)
{
	uchar	szStartNo[20+1], szEndNo[20+1];
	ushort	i;

	for(i=0; i<glSysParam.uiCardBinNum; i++)
	{
		PubBcd2Asc(glSysParam.stCardBinTable[i].sStartNo, 10, szStartNo);
		PubBcd2Asc(glSysParam.stCardBinTable[i].sEndNo,   10, szEndNo);
		if( memcmp(pszPAN, szStartNo, glSysParam.stCardBinTable[i].ucMatchLen)>=0 &&
			memcmp(pszPAN, szEndNo,   glSysParam.stCardBinTable[i].ucMatchLen)<=0 )
		{
			return (int)glSysParam.stCardBinTable[i].ucIssuerIndex;
		}
	}

	return -1;
}

void ConvIssuerName(uchar *pszOrgName, uchar *pszOutName)
{
	char	*p;

	sprintf((char *)pszOutName, "%.10s", pszOrgName);
	p = strchr((char *)pszOutName, '_');
	if( p!=NULL )
	{
		*p = 0;
	}
}

// input CVV2 or 4DBC
int GetSecurityCode(void)
{
	uchar	ucLen;

	if( !ChkIfNeedSecurityCode() )
	{
		return 0;
	}

#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issManEntryCVV)
	{
		ScrClrLine(2,7);
		PubDispString("ENTER",          DISP_LINE_LEFT|2);
		PubDispString("SECURITY CODE?", DISP_LINE_LEFT|4);
		PubDispString("Y/N",            DISP_LINE_LEFT|6);
		if( PubYesNo(60) )
		{
			return 0;
		}
	}
#else
	if( ChkAcqOption(ACQ_ASK_CVV2) )
	{
		ScrClrLine(2,7);
		PubDispString("ENTER",          DISP_LINE_LEFT|2);
		PubDispString("SECURITY CODE?", DISP_LINE_LEFT|4);
		PubDispString("Y/N",            DISP_LINE_LEFT|6);
		if( PubYesNo(60) )
		{
			return 0;
		}
	}
#endif
	ScrClrLine(2,7);
	ucLen =  3;
	ScrPrint(0, 2, CFONT, "%s", _T("SECURITY CODE"));
	if( PubGetString(NUM_IN, ucLen, ucLen, glProcInfo.szSecurityCode, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}

	glProcInfo.stTranLog.uiEntryMode |= MODE_SECURITYCODE;
	return 0;
}

// 根据参数进行,刷卡/插卡/输入卡号
// Accept different entry mode due to input:ucMode
// ucMode: bit 8: 1=skipping check track 2 service code, 0=check
//         bit 7: 1=fallback swipe
//         bit 6: 1=skip detect ICC
int GetCard(uchar ucMode)
{
	int		iRet, iEventID;
	uchar	bCheckICC, ucKey;


	// diki add
	PrivateLabelCardDetect= FALSE;


	//cek apakah mesti settle atau tidak? diki add for bukopin
	/*
	iRet = ValidTransSettleBkpn();
		if( iRet==1 )
		{
			return 1;
		}
		*/

		

	if( (glProcInfo.stTranLog.uiEntryMode & 0x0F)!=MODE_NO_INPUT )
	{
		return 0;
	}

	if( ucMode & FALLBACK_SWIPE )
	{
		ucMode &= ~(SKIP_CHECK_ICC|CARD_INSERTED);	// clear bit 8, force to check service code
	}

	bCheckICC = !(ucMode & SKIP_CHECK_ICC);
	

	// diki add for mini atmb
	if(MenuAtmbMerchantNasabah)
		{
		iRet = SwipeCardProcBkpn(bCheckICC);
			if( iRet==0 )
			{
				return ValidCard();
			}
		}
	
	

	TimerSet(0, TIME_OUT_INPUT*10);
	while( 1 )
	{
		iEventID = DetectCardEvent(ucMode);
		if( iEventID==CARD_KEYIN )
		{
			ucKey = getkey();
			if( ucKey==KEYCANCEL )
			{
				return ERR_USERCANCEL;
			}
			if( (ucMode & CARD_KEYIN) && ucKey>='0' && ucKey<='9' )
			{
				return ManualInputPan(ucKey);
			}
		}
		else if( iEventID==CARD_SWIPED )
		{
			iRet = SwipeCardProc(bCheckICC);
			if( iRet==0 )
			{
				return ValidCard();
			}
			else if( iRet==ERR_SWIPECARD )
			{
				DispMagReadErr();
				PubBeepErr();
				PubWaitKey(3);
			}
			else if( iRet==ERR_NEED_INSERT )	// 是芯片卡
			{
				if( !(ucMode & CARD_INSERTED) )
				{	// 本身交易不允许插卡
					return iRet;
				}
				ucMode &= ~CARD_SWIPED;			// 去掉刷卡检查
			}
			else
			{
				return iRet;
			}
		}

////////// diki add
		else if( iEventID==CARD_TAP)
		{
			iRet = StartTapPrvtLbl();
			if( iRet==0 )
			{
				PrivateLabelCardDetect= TRUE;
				return ValidCardPrvtLbl();
			}
			else
			{
				return iRet;
			}
		}
///////////////////////
				
#ifdef ENABLE_EMV
		else if( iEventID==CARD_INSERTED )
		{

///////////////////////
		iRet = Req_ICC_PrvtLbl_Cek();
		if( iRet==0 )
		{
			///iRet = TransSalePrvtLbl();
			///if( iRet!=0 )
			///{
			///	CommOnHook(FALSE);
			///	return iRet;
			///}
			
			iRet = Req_ICC_PrvtLbl();
			if( iRet!=0 )
			{
				return iRet;
			}

			iRet = ValidCardPrvtLbl();	// 校验卡号
			if( iRet!=0 )
			{
				return iRet;
			}
			PrivateLabelCardDetect= TRUE;
			return 0;
		} else {
		////}


			iRet = InsertCardProc();
			if( iRet==0 )
			{
				return 0;
			}
			else if( iRet==ERR_NEED_FALLBACK )
			{
				DispFallBackPrompt();
				PromptRemoveICC();
				ucMode = CARD_SWIPED|FALLBACK_SWIPE;	// Now we don't support fallback to manual-PAN-entry
			}
			else if( iRet==ERR_TRAN_FAIL )
			{
				PubDispString(_T("NOT ACCEPTED"), 4|DISP_LINE_LEFT);
				PubBeepErr();
				PubWaitKey(3);
				PromptRemoveICC();
				//emv_test();
				return ERR_NO_DISP;
			}
			else
			{
				return iRet;
			}

			}
		
		}		
		
		else if( iEventID==ERR_NO_DISP)
			return ERR_NO_DISP;
#endif
	}
}

// 用户输入事件检测(按键/刷卡/插卡)
int DetectCardEvent(uchar ucMode)
{
	int		iRet;
	uchar SerialInfo[11+1];
	uchar CardType;
	uchar mode;
	uchar CID;
	
	//磁头上电、打开、清缓冲
	if( ucMode & CARD_SWIPED )
	{
		MagClose();
		MagOpen();
		MagReset();
	}

	if( ucMode & CARD_TAP )
	{


		// When TAP card detect
		mode = 'A';	
		PiccClose();
		iRet = PiccOpen();
		if(iRet!=0)
		{
		///continue;
		}
	
	}
	
	if( ucMode & FALLBACK_SWIPE )
	{
		DispFallBackSwipeCard();
	}
	else if( (ucMode & CARD_SWIPED) && (ucMode & CARD_INSERTED) )
	{
		if( ChkIfEmvEnable() )
		{
			DispSwipeInsertCardOnly();
		}
		else
		{ 
			DispSwipeCardOnly();
		}
//		DispSwipeCard(FALSE);
	}
	else if( (ucMode & CARD_INSERTED)  )
	{
		if( !(ucMode & SKIP_DETECT_ICC) )
		{
			DispInsertOnly();
		}
	}
	else
	{
		DispSwipeCardOnly();
	}
    	kbflush();
	while( 1 )
	{
		if(!TimerCheck(0))
			return ERR_NO_DISP;

		if( PubKeyPressed() )
		{
			return CARD_KEYIN;		// 有按键事件
		}
		if( (ucMode & CARD_SWIPED) && (MagSwiped()==0) )
		{
			return CARD_SWIPED;		// 有刷卡事件
		}
		if( (ucMode & CARD_INSERTED) && ChkIfEmvEnable() )
		{
			if( ucMode & SKIP_DETECT_ICC )
			{
				return CARD_INSERTED;	// 有插入IC卡事件
			}
			else if( IccDetect(ICC_USER)==0 )
			{
				return CARD_INSERTED;	// 有插入IC卡事件
			}
		}

		if( (ucMode & CARD_TAP) && (PiccDetect(mode, &CardType, SerialInfo, &CID, NULL)==0) )
		{
			return CARD_TAP;		// 有刷卡事件
		}
		
	}
}

// 显示Fallback提示界面
void DispFallBackPrompt(void)
{
	uint	iCnt;

	PubDispString(_T("PLS SWIPE CARD"), 4|DISP_LINE_LEFT);  // ditutup zaki 16 Januari 2014
	iCnt = 0;
	while( IccDetect(0)==0 )
	{
		iCnt++;
		if( iCnt>4 )
		{
			Beep();
			//ScrPrint(16*5, 7, ASCII, "%8s", iCnt%2 ? "PULL OUT" : "");  // ditutup zaki 13 Januari 2014
			ScrPrint(8*3, 7, ASCII, "%17s", iCnt%2 ? "CHIP MALFUNCTION" : "");
		}
		DelayMs(500);
	}
}

// 输入金额及小费
// Get amount and tips.
int GetAmount(void)
{
	int		iRet;
	uchar	szTotalAmt[12+1];

	memset(szTotalAmt, 0, sizeof(szTotalAmt));
	
	if( glProcInfo.stTranLog.szAmount[0]!=0 )
	{
		return 0;
	}

	while( 1 )
	{
		memset(glProcInfo.stTranLog.szAmount, 0, sizeof(glProcInfo.stTranLog.szAmount));
		memset(glProcInfo.stTranLog.szTipAmount, 0, sizeof(glProcInfo.stTranLog.szTipAmount));
		
		iRet = InputAmount(AMOUNT);
		if( iRet!=0 )
		{
			return iRet;
		}

		iRet = GetTipAmount();
		if( iRet!=0 )
		{
			return iRet;
		}
		

		memcpy(glProcInfo.stTranLog.szInitialAmount, glProcInfo.stTranLog.szAmount, sizeof(glProcInfo.stTranLog.szInitialAmount));
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
		if (!ValidBigAmount(szTotalAmt))
		{
			continue;
		}

		if(glTlmSysParam.Con.appTermOpt5 & EDC_TLM_TMS_CONFIRM_AMT_ENTRY)
		{
			if( ConfirmAmount(NULL, szTotalAmt) )
			{
				break;
			}
		}
		else
			break;
	}

#ifndef	__TELIUM_PARAM__	//not use
	if( !AllowDuplicateTran() )
	{
		return ERR_USERCANCEL;
	}
#endif

	return 0;
}


// Get amount Brizzi
int GetAmountBrizzi(void)
{
	int		iRet;
	uchar	szTotalAmt[12+1];

	if( glProcInfo.stTranLog.szAmount[0]!=0 )
	{
		return 0;
	}

	while( 1 )
	{
		iRet = InputAmount(AMTBRIZZI);
		if( iRet!=0 )
		{
			return iRet;
		}

		memcpy(glProcInfo.stTranLog.szInitialAmount, glProcInfo.stTranLog.szAmount, sizeof(glProcInfo.stTranLog.szInitialAmount));
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
		if (!ValidBigAmount(szTotalAmt))
		{
			continue;
		}

		break;
	}

	return 0;
}

// 把不含小数点，不含ignore digit的数字串转换为ISO8583 bit4格式的12位ASCII数字串
void AmtConvToBit4Format(uchar *pszString, uchar ucIgnoreDigit)
{
	uint	uiLen;

	if (pszString == NULL)
	{
		return;
	}
	
	uiLen = (uint)strlen((char *)pszString);
	if( uiLen>=(uint)(12-ucIgnoreDigit) )
	{
		return;
	}

	// 前补0
	memmove(pszString+12-uiLen-ucIgnoreDigit, pszString, uiLen+1);
	memset(pszString, '0', 12-uiLen-ucIgnoreDigit);

	// 后补ignore digit个0
	for (uiLen=0; uiLen<ucIgnoreDigit; uiLen++)
	{
		strcat((char *)pszString, "0");
	}
}

int GetInstalPlan(void)
{
	int				ii, iRet;
	ulong			ulAmt;
	uchar			sBuff[64];
 	MenuItem		stPlanItems[MAX_PLAN+1];
	INSTALMENT_PLAN	stPlanList[MAX_PLAN+1];

TAG_SELECT_PLAN:
	if (glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_INSTALMENT)
	{
		return 0;
	}

	// 生成installment plan菜单
	memset(stPlanItems, 0, sizeof(stPlanItems));
	memset(stPlanList,  0, sizeof(stPlanList));
	for (ii=0; ii<glSysParam.ucPlanNum; ii++)
	{
		stPlanItems[ii].bVisible = TRUE;
		sprintf((char *)stPlanItems[ii].szMenuName, "%.*s", NUM_MAXCHARS, (char *)glSysParam.stPlanList[ii].szName);
		stPlanItems[ii].pfMenuFunc = NULL;
		stPlanList[ii] = glSysParam.stPlanList[ii];
	}
	stPlanItems[ii].bVisible = FALSE;
	sprintf((char *)stPlanItems[ii].szMenuName, "");
	stPlanItems[ii].pfMenuFunc = NULL;

	// 菜单选择
	iRet = PubGetMenu((uchar *)_T("SELECT PLAN"), stPlanItems, MENU_AUTOSNO|MENU_ASCII, USER_OPER_TIMEOUT);
	if (iRet<0)
	{
		return ERR_USERCANCEL;
	}
	ii = iRet;

	ulAmt = atol(glProcInfo.stTranLog.szAmount);
	if (stPlanList[ii].ulBottomAmt > ulAmt)
	{
		PubConvAmount(NULL, glProcInfo.stTranLog.szAmount,
					glProcInfo.stTranLog.stTranCurrency.ucDecimal,
					glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit,
					sBuff, 0);
 		DispTransName();
 		PubDispString(_T("LOWER THAN"),   2|DISP_LINE_LEFT);
 		PubDispString(_T("LIMIT AMOUNT"), 4|DISP_LINE_LEFT);
 		PubDispString(sBuff,              6|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);	
		goto TAG_SELECT_PLAN;
	}

 	glProcInfo.stTranLog.ucInstalment = stPlanList[ii].ucMonths;
 	
	iRet = MatchCardTableForInstalment(stPlanList[ii].ucAcqIndex);	// 重新决定ACQ
	if (iRet!=0)
	{
		ScrClrLine(2, 7);
		PubDispString(_T("UNSUPPORT CARD"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
 	
 	return 0;
}

// Use local currency to call PubConvAmount
void App_ConvAmountLocal(uchar *pszIn, uchar *pszOut, uchar ucMisc)
{
	uchar	szBuff[12];

	memset(szBuff, 0, sizeof(szBuff));
#ifdef	__TELIUM_PARAM__
	strcpy((char *)szBuff, (char *)glTlmSysParam.stEdcInfo.stLocalCurrency.szName);
	if ((glTlmSysParam.stEdcInfo.ucCurrencySymbol!=' ')
		&& (glTlmSysParam.stEdcInfo.ucCurrencySymbol!=0) )
	{
		szBuff[strlen(szBuff)] = glTlmSysParam.stEdcInfo.ucCurrencySymbol;
	}

	PubConvAmount(szBuff, pszIn,
				glTlmSysParam.stEdcInfo.stLocalCurrency.ucDecimal,
				glTlmSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit,
				pszOut, ucMisc);
#else
	strcpy((char *)szBuff, (char *)glSysParam.stEdcInfo.stLocalCurrency.szName);
	if ((glSysParam.stEdcInfo.ucCurrencySymbol!=' ')
		&& (glSysParam.stEdcInfo.ucCurrencySymbol!=0) )
	{
		szBuff[strlen(szBuff)] = glSysParam.stEdcInfo.ucCurrencySymbol;
	}

	PubConvAmount(szBuff, pszIn,
				glSysParam.stEdcInfo.stLocalCurrency.ucDecimal,
				glSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit,
				pszOut, ucMisc);
#endif	
}

// Use transaction currency to call PubConvAmount (MAYBE different form local currency)
void App_ConvAmountTran(uchar *pszIn, uchar *pszOut, uchar ucMisc)
{
	uchar	szBuff[12];

	memset(szBuff, 0, sizeof(szBuff));
//	strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.stTranCurrency.szName);
	
	strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.stTranCurrency.szName);

	PubConvAmount(szBuff, pszIn,
				glProcInfo.stTranLog.stTranCurrency.ucDecimal,
				glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit,
				pszOut, ucMisc);
}

void App_ConvAmount(uchar *pszIn, uchar *pszOut, uchar ucDecimal, uchar ucIgnoreDigit, uchar ucMisc)
{
	uchar	szBuff[3+1];

	memset(szBuff, 0, sizeof(szBuff));
	
//	strcpy((char *)szBuff, (char *)glTlmSysParam.Con.appCurrSign);

	strcpy(szBuff, "Rp.");

	PubConvAmount(szBuff, pszIn, ucDecimal, ucIgnoreDigit, pszOut, ucMisc);
}



// 输入交易金额
int InputAmount(uchar ucAmtType)
{
	uchar	ucRet, ucMinLen, ucMaxLen, ucFlag, szCurrName[4+1];
	uchar	*pszAmt;

	PubASSERT( ucAmtType==AMOUNT    || ucAmtType==RFDAMOUNT ||
			   ucAmtType==ORGAMOUNT || ucAmtType==TIPAMOUNT||ucAmtType==AMTBRIZZI);

	ScrClrLine(2, 7);
	switch( ucAmtType )
	{
	case AMOUNT:
		if( ChkIfNeedTip() )
		{
			PubDispString(_T("BASE AMOUNT"), 2|DISP_LINE_LEFT);
		}
		else
		{
			//PubDispString(_T("ENTER AMOUNT"), 2|DISP_LINE_LEFT);
			PubDispString(_T("AMT?"), 4|DISP_LINE_LEFT);
		}
		break;

	case RFDAMOUNT:	// RFU
		PubDispString(_T("REFUND AMOUNT"), 2|DISP_LINE_LEFT);
		break;

	case ORGAMOUNT:	// RFU
		PubDispString(_T("ORIGINAL AMOUNT"), 2|DISP_LINE_LEFT);
		break;

	case TIPAMOUNT:
		PubDispString(_T("ENTER TIP"), 2|DISP_LINE_LEFT);
		break;

	case AMTBRIZZI:
		PubDispString(_T("PEMBAYARAN"), 2|DISP_LINE_LEFT);
		break;
	}

#ifdef	__TELIUM_PARAM__
	sprintf((char *)szCurrName, "%.3s", glTlmSysParam.Con.appCurrSign);
// 	sprintf((char *)szCurrName, "%.3s%1.1s", glSysParam.stEdcInfo.szCurrencyName, &glSysParam.stEdcInfo.ucCurrencySymbol);
	ucMinLen = (ucAmtType==TIPAMOUNT) ? 0 : 1;
//	ucMaxLen = MIN(glSysParam.stEdcInfo.ucTranAmtLen, 10);
	ucMaxLen = MIN(glTlmSysParam.stEdcInfo.ucTranAmtLen, 11);
			
#else
	sprintf((char *)szCurrName, "%.3s", glSysParam.stEdcInfo.stLocalCurrency.szName);
// 	sprintf((char *)szCurrName, "%.3s%1.1s", glSysParam.stEdcInfo.szCurrencyName, &glSysParam.stEdcInfo.ucCurrencySymbol);
	ucMinLen = (ucAmtType==TIPAMOUNT) ? 0 : 1;
//	ucMaxLen = MIN(glSysParam.stEdcInfo.ucTranAmtLen, 10);
	ucMaxLen = MIN(glSysParam.stEdcInfo.ucTranAmtLen, 11);
#endif
//	ucFlag   = 0;
	ucFlag   = GA_SEPARATOR;	//irf, 20121203
	
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_REFUND || ucAmtType==RFDAMOUNT )
	{
//		ucFlag = GA_NEGATIVE;
		ucFlag = ucFlag|GA_NEGATIVE;	//20121203
	}

	if( ucAmtType==TIPAMOUNT )
	{
		pszAmt = glProcInfo.stTranLog.szTipAmount;
	}
	else
	{
		pszAmt = glProcInfo.stTranLog.szAmount;
	}
	ucRet = PubGetAmount(szCurrName, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
						ucMinLen, ucMaxLen, pszAmt, USER_OPER_TIMEOUT, ucFlag);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	// Use transaction currency to do conversion
	AmtConvToBit4Format(pszAmt, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	return 0;
}

// 输入TIP金额
int GetTipAmount(void)
{
	int		iRet;
	uchar	szTotalAmt[12+1];

	if( !ChkIfNeedTip() )
	{
		return 0;
	}

	while( 1 )
	{
		iRet = InputAmount(TIPAMOUNT);
		if( iRet!=0 )
		{
			return iRet;
		}

		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
		if( ValidAdjustAmount(glProcInfo.stTranLog.szAmount, szTotalAmt) )
		{
			break;
		}
	}

	return 0;
}

// 检查调整金额是否合法(TRUE: 合法, FALSE: 非法)
// 金额必须为12数字
uchar ValidAdjustAmount(uchar *pszBaseAmt, uchar *pszTotalAmt)
{
	uchar	szMaxAmt[15+1], szAdjRate[3+1];

	if( memcmp(pszTotalAmt, pszBaseAmt, 12)<0 )
	{
		DispAdjustTitle();
		PubDispString(_T("AMOUNT ERROR"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return FALSE;
	}

#ifdef	__TELIUM_PARAM__
	if( glCurTlmTmsIss.ucAdjustPercent==0 || memcmp(pszTotalAmt, pszBaseAmt, 12)==0 )
	{
		return TRUE;
	}
#else
	if( glCurIssuer.ucAdjustPercent==0 || memcmp(pszTotalAmt, pszBaseAmt, 12)==0 )
	{
		return TRUE;
	}
#endif

#ifdef	__TELIUM_PARAM__
	sprintf((char *)szAdjRate, "%ld", (ulong)(glCurTlmTmsIss.ucAdjustPercent+100));
#else
	sprintf((char *)szAdjRate, "%ld", (ulong)(glCurIssuer.ucAdjustPercent+100));
#endif
	PubAscMul(pszBaseAmt, szAdjRate, szMaxAmt);
	PubAddHeadChars(szMaxAmt, 15, '0');

	if( memcmp(pszTotalAmt, &szMaxAmt[1], 12)>0 )	// 最后两位为小数,不进行比较
	{
		DispAdjustTitle();
		PubDispString(_T("OVER LIMIT"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return FALSE;
	}

	return TRUE;
}

// amount should be less than 4.294.967.296 (max of unsigned long)
uchar ValidBigAmount(uchar *pszAmount)
{
	int	iLen;

//	hex_dump_char("pszAmount", pszAmount, strlen(pszAmount));
	
	iLen = strlen(pszAmount);
	if (iLen<10)
	{
		return TRUE;
	}
	if (iLen>12)
	{
		return FALSE;
	}
	
	if (PubAsc2Long(pszAmount, iLen-3, NULL)>0xffffffff)
	{
		return FALSE;
	}
	return TRUE;
}

// 确认金额界面处理
uchar ConfirmAmount(char *pszDesignation, uchar *pszAmount)
{
	if ((pszDesignation==NULL) || (pszDesignation[0]==0))
	{
		PubDispString(_T("TOTAL           "), 2|DISP_LINE_LEFT);
	}
	else
	{
		PubDispString(pszDesignation, 2|DISP_LINE_LEFT);
	}

	DispAmount(4, pszAmount);
	PubDispString(_T("CORRECT ? Y/N"), DISP_LINE_LEFT|6);
	return !PubYesNo(USER_OPER_TIMEOUT);
}

// 在指定行显示格式化的金额信息。注意金额是bit4格式，即可能含有ignore digit
// display formatted amount in specified line.
void DispAmount(uchar ucLine, uchar *pszAmount)
{
	uchar	ucFlag, ucFont, szOutAmt[30];

//	ucFlag = 0;
	ucFlag = GA_SEPARATOR;

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID|| glProcInfo.stTranLog.ucTranType==TRANS_TYPE_REFUND ||
		*pszAmount=='D' )
	{
		ucFlag |= GA_NEGATIVE;
		if( *pszAmount=='D' )
		{
			*pszAmount = '0';
		}
	}

	App_ConvAmountTran(pszAmount, szOutAmt, ucFlag);
	ucFont = strlen((char *)szOutAmt)>16 ? ASCII : CFONT;
	ScrPrint(0, ucLine, ucFont, "%*.21s", (ucFont==CFONT ? 16 : 21), szOutAmt);
}

void DispAmountASCII(uchar ucLine, uchar *pszAmount)
{
	uchar	ucFlag, ucFont, szOutAmt[30];

//	ucFlag = 0;
	ucFlag = GA_SEPARATOR;

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID|| glProcInfo.stTranLog.ucTranType==TRANS_TYPE_REFUND ||
		*pszAmount=='D' )
	{
		ucFlag |= GA_NEGATIVE;
		if( *pszAmount=='D' )
		{
			*pszAmount = '0';
		}
	}

	App_ConvAmountTran(pszAmount, szOutAmt, ucFlag);
	ucFont = strlen((char *)szOutAmt)>16 ? ASCII : CFONT;
	ScrPrint(0, ucLine, ASCII, "%*.21s",  21, szOutAmt);
}


// 从UsrMsg取得卡号
// Format: "CARDNO=4333884001356283"
int GetManualPanFromMsg(void *pszUsrMsg)
{
	sprintf(glProcInfo.stTranLog.szPan, "%.19s", (char *)pszUsrMsg);
	if (!IsNumStr(glProcInfo.stTranLog.szPan))
	{
		return ERR_NO_DISP;
	}

	return VerifyManualPan();
}

// 手工输入PAN及其相关信息
int ManualInputPan(uchar ucInitChar)
{	
	//if(setKeyIn) // dituup zaki 26022014
	//{ // dituup zaki 26022014
	
	ScrClrLine(2, 7);
	PubDispString(_T("ENTER ACCOUNT NO"), 2|DISP_LINE_LEFT);

	memset(glProcInfo.stTranLog.szPan, 0, sizeof(glProcInfo.stTranLog.szPan));
	if( ucInitChar>='0' && ucInitChar<='9' )
	{
		glProcInfo.stTranLog.szPan[0] = ucInitChar;
	}
	if( PubGetString(NUM_IN|CARRY_IN, 13, LEN_PAN, glProcInfo.stTranLog.szPan, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_NO_DISP;
	}

	if (glProcInfo.bIsFallBack==TRUE)
	{
		glProcInfo.stTranLog.uiEntryMode = MODE_FALLBACK_MANUAL;
	}

	return VerifyManualPan();
	//} // dituup zaki 26022014
	//return ERR_USERCANCEL; // dituup zaki 26022014
} 

int VerifyManualPan(void)
{
	int		iRet;

	glProcInfo.stTranLog.uiEntryMode = MODE_MANUAL_INPUT;

	DispTransName();
	iRet = MatchCardTable(glProcInfo.stTranLog.szPan); 
	if( iRet!=0 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("UNSUPPORT CARD"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}

#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issManual != TRUE)
	{
		PubDispString(_T("NOT ALLOW MANUL"), 4|DISP_LINE_LEFT);
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
#else
	if( !ChkIssuerOption(ISSUER_EN_MANUAL) )
	{
		PubDispString(_T("NOT ALLOW MANUL"), 4|DISP_LINE_LEFT);
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
#endif	

	iRet = ValidPanNo(glProcInfo.stTranLog.szPan);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetExpiry();
	if( iRet!=0 )
	{
		return iRet;
	}

	if( !ChkEdcOption(EDC_NOT_MANUAL_PWD) )
	{
		if( PasswordSettle()!=0 )
		{
			return ERR_USERCANCEL;
		}
	}

	CheckCapture();
	iRet = ConfirmPanInfo();
	if( iRet!=0 )
	{
		CommOnHook(FALSE);
		return iRet;
	}

	iRet = GetSecurityCode();
	if( iRet!=0 )
	{
		return iRet;
	}

	return 0;
}

// 输入有效期
int GetExpiry(void)
{
	int		iRet;
	uchar	szExpDate[4+1];

/*
#ifdef	__TELIUM_PARAM__
	if((!glCurTlmTmsIss.issExpiryDate)||(!glCurTlmTmsIss.issExpiryDatReq)||
		(!glCurTlmTmsIss.issCheckExpOff)||(!glCurTlmTmsIss.issCheckExpAll))
	{
		sprintf((char *)glProcInfo.stTranLog.szExpDate, "1111");
		return 0;
	}
#else
	if( !ChkIssuerOption(ISSUER_EN_EXPIRY) )
	{
		sprintf((char *)glProcInfo.stTranLog.szExpDate, "1111");
		return 0;
	}
#endif
*/
	while( 1 )
	{
		ScrCls();
		DispTransName();
		PubDispString(_T("EXP DATE: MMYY"), 2|DISP_LINE_LEFT);
		if( PubGetString(NUM_IN, 4, 4, szExpDate, USER_OPER_TIMEOUT)!=0 )
		{
			return ERR_USERCANCEL;
		}

		sprintf((char *)glProcInfo.stTranLog.szExpDate, "%.2s%.2s", &szExpDate[2], szExpDate);
		iRet = ValidCardExpiry();
		if( iRet==0 )
		{
			break;
		}
	}

	return 0;
}

// 输入商品描述信息
int GetDescriptor(void)		//not use in telium
{
	uchar	ucCnt, ucKey, ucTotal, ucDesc, bInputDesc;

	if( !ChkIssuerOption(ISSUER_EN_DISCRIPTOR) )
	{
		return 0;
	}

	if( glSysParam.ucDescNum==0 )
	{
		return 0;
	}
	if( glSysParam.ucDescNum==1 )
	{
		glProcInfo.stTranLog.szDescriptor[0] = '0';
		glProcInfo.stTranLog.ucDescTotal     = 1;
		return 0;
	}

	ScrCls();
	DispTransName();
	PubDispString(_T("PRODUCT CODE?"),  DISP_LINE_LEFT|2);

	for(bInputDesc=FALSE,ucTotal=0; ;)
	{
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER && ucTotal>0 )
		{
			glProcInfo.stTranLog.ucDescTotal = ucTotal;
			return 0;
		}
		else if( ucKey>='0' && ucKey<='9' && ucTotal<MAX_GET_DESC )
		{
			ucDesc = (ucKey=='0') ? '9' : ucKey - 1;
			if( strchr((char *)glProcInfo.stTranLog.szDescriptor, ucDesc)==NULL &&
				ucDesc<glSysParam.ucDescNum+'0' )
			{
				glProcInfo.stTranLog.szDescriptor[ucTotal] = ucDesc;
				ucTotal++;
				bInputDesc = TRUE;
			}
		}
		else if ( ucKey==KEYCLEAR && ucTotal>0 )
		{
			glProcInfo.stTranLog.szDescriptor[--ucTotal] = 0;
		}
		if( bInputDesc )
		{
			ScrClrLine(4, 7);
			for(ucCnt=0; ucCnt<ucTotal; ucCnt++)
			{
				ucDesc = glProcInfo.stTranLog.szDescriptor[ucCnt] - '0';
				ScrPrint(0, (uchar)(4+ucCnt), ASCII, "%.21s", glSysParam.stDescList[ucDesc].szText);
			}
		}
	}
}

// 输入附加信息
// Input additional message.
int GetAddlPrompt(void)
{
	uchar	ucRet;

	if( !ChkAcqOption(ACQ_ADDTIONAL_PROMPT) && !ChkAcqOption(ACQ_AIR_TICKET) )
	{
		return 0;
	}

	ScrCls();
	DispTransName();
	PubDispString(glSysParam.stEdcInfo.szAddlPrompt, DISP_LINE_LEFT|4);

	ucRet = PubGetString(ALPHA_IN, 0, 16, glProcInfo.stTranLog.szAddlPrompt, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	return 0;
}

// 输入PIN, ucFlag: bit 8, ICC online PIN
int GetPIN(uchar ucFlag)
{
	int		iRet;
	uchar	szPAN[16+1], szTotalAmt[12+1];
	uchar	ucPinKeyID;
//	uchar	ucAmtFlag, szBuff[25];
#if defined(_P60_S1_) || defined(_P80_)
	uchar	ucLen;
#endif

	// 非EMV PIN的模式下，如果是chip则直接返回，不是chip则检查ISSUER
	// in non-EMV-PIN mode, if it is chip transaction then return directly
	if( !(ucFlag & GETPIN_EMV) )
	{
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) )
		{
			return 0;
		}
#ifdef	__TELIUM_PARAM__
		else if(glCurTlmTmsIss.issPinOpt != 0x32)
		{
			return 0;
		}
#else
		else if( !ChkIssuerOption(ISSUER_EN_PIN) )
		{
			return 0;
		}
#endif
	}

	ucPinKeyID = DEF_PIN_KEY_ID;	//!!!! : 预留扩展：ACQUIRER可定义自己使用的ID

	iRet = ExtractPAN(glProcInfo.stTranLog.szPan, szPAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);

	ucFlag &= (uchar)(0xFF-GETPIN_EMV);
	ScrClrLine(2, 7);
	DispAmount(2, szTotalAmt);
	if( ucFlag==0 )
	{
		PubDispString(_T("PLS ENTER PIN"), 4|DISP_LINE_CENTER);
	}
	else
	{
		PubDispString(_T("PIN ERR, RETRY"), 4|DISP_LINE_CENTER);
	}


	if (ChkTermPEDMode(PED_INT_PCI) || ChkTermPEDMode(PED_INT_PXX))
	{
		ScrGotoxy(32, 6);
	}

	memset(glProcInfo.sPinBlock, 0, sizeof(glProcInfo.sPinBlock));

	// !!!! to be check
#if defined(_S_SERIES_) || defined(_SP30_)
	//-------------- Internal PCI PED --------------
	if (ChkTermPEDMode(PED_INT_PCI))
	{
		iRet = PedGetPinBlock(ucPinKeyID, "0,4,5,6,7,8", szPAN, glProcInfo.sPinBlock, 0, USER_OPER_TIMEOUT*1000);
		if( iRet==0 )
		{
			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
			return 0;
		}
		else if (iRet==PED_RET_ERR_NO_PIN_INPUT)	// !!!! PIN bypass
		{
			return 0;
		}
		else if( iRet==PED_RET_ERR_INPUT_CANCEL )	// !!!! cancel input
		{
			return ERR_USERCANCEL;
		}

		DispPciErrMsg(iRet);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
#endif

	//-------------- External PCI PED --------------
	if (ChkTermPEDMode(PED_EXT_PCI))
	{
		// !!!! to be implemented
		ScrClrLine(2, 7);
		PubDispString("EXT PCI PINPAD ",  4|DISP_LINE_LEFT);
		PubDispString("NOT IMPLEMENTED.", 6|DISP_LINE_LEFT);
		PubWaitKey(30);
		return ERR_NO_DISP;
	}
	
	//-------------- PXX PED, and PINPAD --------------
#if defined(_P60_S1_) || defined(_P80_)
	if (ChkTermPEDMode(PED_INT_PXX))
	{
		iRet = PEDGetPwd_3Des(ucPinKeyID, 4, 6, szPAN, &ucLen, glProcInfo.sPinBlock);
	} 
#endif

	
	if( iRet==0 )
	{
		glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		return 0;
	}
	else if( iRet==0x0A )
	{
		return 0;	// PIN by pass
	}
	else if( iRet==0x06 || iRet==0x07 )
	{
		return ERR_USERCANCEL;
	}

	DispPPPedErrMsg((uchar)iRet);

	return ERR_NO_DISP;
}

void DispWelcomeOnPED(void)
{
// 	PPScrCls();
// 	PPScrPrint(0, 0, (uchar *)"   WELCOME!   ");
// 	PPScrPrint(1, 0, (uchar *)"PAX TECHNOLOGY");
}

// get partial pan for PIN entry
int ExtractPAN(uchar *pszPAN, uchar *pszOutPan)
{
	int		iLength;

	iLength = strlen((char*) pszPAN);
	if( iLength<13 || iLength>19 )
	{
		return ERR_SWIPECARD;
	}

	memset(pszOutPan, '0', 16);
	memcpy(pszOutPan+4, pszPAN+iLength-13, 12);
	pszOutPan[16] = 0;

	return 0;
}

// calculate mac
int GetMAC(uchar ucMode, uchar *psData, ushort uiDataLen, uchar ucMacKeyID, uchar *psOutMAC)
{
//	uchar	ucRet;
	int		iRet;

	if( !ChkIfNeedMac() )
	{
		return 0;
	}

	// 0: ANSI x9.9, 1: fast arithm

#if defined(_P60_S1_) || defined(_P80_) || defined(_P90_)
	if (ChkTermPEDMode(PED_INT_PXX))
	{
		ucRet = PEDMac(ucMacKeyID, 0x01, psData, uiDataLen, psOutMAC, 0);	// !!!! mode 改造
		if( ucRet!=0 )
		{
			DispPPPedErrMsg(ucRet);
			return ERR_NO_DISP;
		}
	}
#endif

#if defined(_S_SERIES_) || defined(_SP30_)
	if (ChkTermPEDMode(PED_INT_PCI))
	{
		iRet = PedGetMac(ucMacKeyID, psData, uiDataLen, psOutMAC, ucMode);
		if (iRet!=0)
		{
			DispPciErrMsg(iRet);
			PubBeepErr();
			PubWaitKey(3);
			return ERR_NO_DISP;
		}
		return 0;
	}
#endif
	
	if (ChkTermPEDMode(PED_EXT_PCI))
	{
		// External PCI
		// !!!! to be implemented
		ScrClrLine(2, 7);
		PubDispString("EXTERNAL PCI",     4|DISP_LINE_LEFT);
		PubDispString("NOT IMPLEMENTED.", 6|DISP_LINE_LEFT);
		PubWaitKey(30);
		return ERR_NO_DISP;
	}

	return 0;
}

// show PED/PINPAD error message
void DispPPPedErrMsg(uchar ucErrCode)
{
	switch( ucErrCode )
	{
	case 0x01:
		PubDispString(_T("INV MAC DATA"), 4|DISP_LINE_LEFT);
		break;

	case 0x02:
	case 0x0B:
		PubDispString(_T("INVALID KEY ID"), 4|DISP_LINE_LEFT);
	    break;

	case 0x03:
		PubDispString(_T("INVALID MODE"), 4|DISP_LINE_LEFT);
		break;

	default:
		ScrClrLine(2, 7);
		ScrPrint(3, 4, CFONT, _T("PED ERROR:%02X"), ucErrCode);
		DispBox2();
	    break;
	}

	PubBeepErr();
	PubWaitKey(3);
}

#if defined(_S_SERIES_) || defined(_SP30_)
void DispPciErrMsg(int iErrCode)
{
	char	szDispBuff[64];

	// to be implemented
	switch( iErrCode )
	{
	case PED_RET_OK:
		return;

	case PED_RET_ERR_NO_KEY:
		sprintf(szDispBuff, _T("KEY MISSING"));
		break;

	case PED_RET_ERR_KEYIDX_ERR:
		sprintf(szDispBuff, _T("INVALID KEY INDEX"));
		break;

	case PED_RET_ERR_DERIVE_ERR:
		sprintf(szDispBuff, _T("INVALID KEY LEVEL"));
	    break;

	case PED_RET_ERR_CHECK_KEY_FAIL:
	case PED_RET_ERR_KCV_CHECK_FAIL:
		sprintf(szDispBuff, _T("CHECK KEY FAIL"));
	    break;

	case PED_RET_ERR_NO_PIN_INPUT:
		sprintf(szDispBuff, _T("PIN BYPASS"));
	    break;

	case PED_RET_ERR_INPUT_CANCEL:
	case PED_RET_ERR_INPUT_TIMEOUT:
		sprintf(szDispBuff, _T("INPUT CANCELLED"));
	    break;

	case PED_RET_ERR_WAIT_INTERVAL:
		sprintf(szDispBuff, _T("PLS TRY AGAIN LATER"));
	    break;

	case PED_RET_ERR_CHECK_MODE_ERR:
		sprintf(szDispBuff, _T("INVALID MODE"));
	    break;

	case PED_RET_ERR_NO_RIGHT_USE:
	case PED_RET_ERR_NEED_ADMIN:
		sprintf(szDispBuff, _T("PED ACCESS DENIALED"));
	    break;

	case PED_RET_ERR_KEY_TYPE_ERR:
	case PED_RET_ERR_SRCKEY_TYPE_ERR:
		sprintf(szDispBuff, _T("INVALID KEY TYPE"));
	    break;

	case PED_RET_ERR_EXPLEN_ERR:
		sprintf(szDispBuff, _T("INVALID PARA"));
	    break;

	case PED_RET_ERR_DSTKEY_IDX_ERR:
		sprintf(szDispBuff, _T("INVALID DST INDEX"));
	    break;

	case PED_RET_ERR_SRCKEY_IDX_ERR:
		sprintf(szDispBuff, _T("INVALID SRC INDEX"));
	    break;

	case PED_RET_ERR_KEY_LEN_ERR:
		sprintf(szDispBuff, _T("INVALID KEY LENGTH"));
	    break;

	case PED_RET_ERR_NO_ICC:
		sprintf(szDispBuff, _T("CARD NOT READY"));
	    break;

	case PED_RET_ERR_ICC_NO_INIT:
		sprintf(szDispBuff, _T("CARD NOT INIT"));
	    break;

	case PED_RET_ERR_GROUP_IDX_ERR:
		sprintf(szDispBuff, _T("INVALID GROUP INDEX"));
	    break;

	case PED_RET_ERR_PARAM_PTR_NULL:
		sprintf(szDispBuff, _T("INVALID PARA"));
	    break;

	case PED_RET_ERR_LOCKED:
		sprintf(szDispBuff, _T("PED LOCKED"));
	    break;

	case PED_RET_ERROR:
		sprintf(szDispBuff, _T("PED GENERAL ERR"));
	    break;

	case PED_RET_ERR_UNSPT_CMD:
	case PED_RET_ERR_DUKPT_OVERFLOW:
	case PED_RET_ERR_NOMORE_BUF:
	case PED_RET_ERR_COMM_ERR:
	case PED_RET_ERR_NO_UAPUK:
	case PED_RET_ERR_ADMIN_ERR:
	case PED_RET_ERR_DOWNLOAD_INACTIVE:
	default:
		sprintf(szDispBuff, _T("PED ERROR:%d"), iErrCode);
	    break;
	}
	
	ScrClrLine(2, 7);
	PubDispString(szDispBuff, 4|DISP_LINE_LEFT);
	DispBox2();
}
#endif

// 根据参数设置对PAN进行掩码
// Output a mask PAN
void MaskPan(uchar *pszInPan, uchar *pszOutPan)
{
	uchar	szBuff[30];
//	int		iCnt;
	int		iPanLen;
//	int		iOption;

	memset(szBuff, 0, sizeof(szBuff));
	iPanLen = strlen((char *)pszInPan);
#ifdef	__TELIUM_PARAM__
#ifdef BANK_BUKOPIN
	strcpy(szBuff, pszInPan);
	if(iPanLen>4)
		memset(szBuff, '*', strlen(szBuff)-4);
	if(iPanLen>10)
		memcpy(szBuff,pszInPan,6);
#endif
#else
	if( !ChkOptionExt(glCurIssuer.sPanMask, 0x0080) )	//the first bit of the 24 bits
	{	// right align
		for(iCnt=0; iCnt<iPanLen; iCnt++)
		{
			iOption = ((2-iCnt/8)<<8) + (1<<(iCnt%8));
			if( !ChkOptionExt(glCurIssuer.sPanMask, (ushort)iOption) )
			{
				szBuff[iPanLen-iCnt-1] = pszInPan[iPanLen-iCnt-1];
			}
			else
			{
				szBuff[iPanLen-iCnt-1] = '*';
			}
		}
	}
	else
	{	// left align
		for(iCnt=0; iCnt<iPanLen; iCnt++)
		{
			iOption = (((iCnt+2)/8)<<8) + (0x80>>((iCnt+2)%8));
			if( !ChkOptionExt(glCurIssuer.sPanMask, (ushort)iOption) )
			{
				szBuff[iCnt] = pszInPan[iCnt];
			}
			else
			{
				szBuff[iCnt] = '*';
			}
		}
	}
#endif
	sprintf((char *)pszOutPan, "%.*s", LEN_PAN, szBuff);
}

// 获取8583打包/解包错误信息
void Get8583ErrMsg(uchar bPackErr, int iErrCode, uchar *pszErrMsg)
{
	PubASSERT( iErrCode<0 );
	if( bPackErr )
	{
		if( iErrCode<-2000 )
		{
			sprintf((char *)pszErrMsg, "BIT %d DEF ERR", -iErrCode-2000);
		}
		else if( iErrCode<-1000 )
		{
			sprintf((char *)pszErrMsg, "BIT %d PACK ERR", -iErrCode-1000);
		}
		else
		{
			sprintf((char *)pszErrMsg, "PACK HEADER ERR");
		}
	}
	else
	{
		if( iErrCode<-2000 )
		{
			sprintf((char *)pszErrMsg, "BIT %d DEF ERR", -iErrCode-2000);
		}
		else if( iErrCode<-1000 )
		{
			sprintf((char *)pszErrMsg, "UNPACK %d ERR", -iErrCode-1000);
		}
		else if( iErrCode==-1000 )
		{
			sprintf((char *)pszErrMsg, "DATA LENGTH ERR");
		}
		else
		{
			sprintf((char *)pszErrMsg, "UNPACK HEAD ERR");
		}
	}
}


// 获取预授权号码
int GetPreAuthCode(void)
{
	uchar	ucRet, szBuff[LEN_AUTH_CODE+1];
	uchar	ucAuthMinLen, ucAuthMaxLen;

	ucAuthMinLen = 2;
	ucAuthMaxLen = LEN_AUTH_CODE;
	PubDispString(_T("APPV CODE ?"), 2|DISP_LINE_LEFT);
	memset(szBuff, 0, sizeof(szBuff));
	ucRet = PubGetString(ALPHA_IN, ucAuthMinLen, ucAuthMaxLen, szBuff, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	sprintf((char *)glProcInfo.stTranLog.szAuthCode, "%-*s", LEN_AUTH_CODE, szBuff);

	return 0;
}

// 获取票据号
int InputInvoiceNo(ulong *pulInvoiceNo)
{
	uchar	ucRet, szBuff[LEN_INVOICE_NO+1];

	ScrClrLine(2, 7);
	PubDispString(_T("TRACE NO ?"), 2|DISP_LINE_LEFT);

	memset(szBuff, 0, sizeof(szBuff));
	ucRet = PubGetString(NUM_IN, 1, LEN_INVOICE_NO, szBuff, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	*pulInvoiceNo = (ulong)atol((char *)szBuff);

	return 0;
}

// 获取要显示的交易状态信息
void GetStateText(ushort uiStatus, uchar *pszStaText)
{
	*pszStaText = 0;
	if( uiStatus & TS_VOID)
	{
		sprintf((char *)pszStaText, "VOIDED");
	}
	else if( uiStatus & TS_ADJ)
	{
		sprintf((char *)pszStaText, "ADJUSTED");
	}
	else if( uiStatus & TS_NOSEND)
	{
		sprintf((char *)pszStaText, "NOT_SEND");
	}
}

// 显示交易状态信息
void DispStateText(ushort uiStatus)
{
	if( uiStatus & TS_VOID )
	{
		PubDispString(_T("ALREADY VOID"), 4|DISP_LINE_LEFT);
	}
	else if( uiStatus & TS_ADJ )
	{
		PubDispString(_T("ALREADY ADJUST"), 4|DISP_LINE_LEFT);
	}
	else if( uiStatus & TS_NOSEND )
	{
		PubDispString(_T("NOT SEND"), 4|DISP_LINE_LEFT);
	}
}

void DispOkSub(uchar ucLogoNo)
{
	static uchar sLogoOkThree[117] =
	{
		0x03,
		0x00,0x24,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc0,0x40,0x0,0x40,0x0,0x40,0x0,0x40,0x0,
		0x40,0x0,0x40,0x0,0x40,0x0,0x80,0xc0,0xc0,0xc0,0xe0,0x60,0x60,0x30,0x30,0x30,
		0x10,0x10,0x10,0x0,

		0x00,0x24,
		0x0,0x4,0x4,0x4,0xc,0x18,0x18,0xba,0x70,0xe0,0xe0,0xc0,0xc0,0x80,0xc0,0xe0,
		0xf0,0xf8,0x7c,0x3e,0x1f,0xf,0x7,0xab,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,

		0x00,0x24,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6,0x4,0x0,0x5,0x3,0x7,0x3,0x7,0x3,
		0x5,0x0,0x4,0x0,0x4,0x0,0x4,0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0
	};
	static uchar sLogoOkTwo[117] =
	{
		0x03,
		0x00,0x24,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc0,0x40,0x0,0x40,0x0,0x40,0x0,0x40,0x0,
		0x40,0x0,0x40,0x0,0x40,0x0,0x40,0xc0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,

		0x00,0x24,
		0x0,0x4,0x4,0x4,0xc,0x18,0x18,0xba,0x70,0xe0,0xe0,0xc0,0xc0,0x80,0xc0,0xe0,
		0xf0,0xf8,0x7c,0x38,0x10,0x0,0x0,0xaa,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,

		0x00,0x24,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6,0x4,0x0,0x5,0x3,0x7,0x3,0x7,0x3,
		0x5,0x0,0x4,0x0,0x4,0x0,0x4,0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0
	};
	static uchar sLogoOkOne[117] =
	{
		0x03,
		0x00,0x24,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc0,0x40,0x0,0x40,0x0,0x40,0x0,0x40,0x0,
		0x40,0x0,0x40,0x0,0x40,0x0,0x40,0xc0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,

		0x00,0x24,
		0x0,0x4,0x4,0x4,0xc,0x18,0x18,0xba,0x70,0xe0,0xe0,0xc0,0xc0,0x80,0x0,0x0,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xaa,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,

		0x00,0x24,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6,0x4,0x0,0x5,0x3,0x7,0x3,0x6,0x0,
		0x4,0x0,0x4,0x0,0x4,0x0,0x4,0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0
	};

	ScrGotoxy(76, 3);
	if( ucLogoNo==0 )
	{
		ScrDrLogo(sLogoOkOne);
	}
	else if( ucLogoNo==1 )
	{
		ScrDrLogo(sLogoOkTwo);
	}
	else
	{
		ScrDrLogo(sLogoOkThree);
	}
}

// 显示操作成功的动画
// Show animation of "done"
void DispOperOk(void *pszMsg)
{
	uchar	ucLogoNo;

	PubDispString(pszMsg, 3|DISP_LINE_LEFT);

	TimerSet(TIMER_TEMPORARY, 2);
	for(ucLogoNo=0; ucLogoNo<3; )
	{
		if( TimerCheck(TIMER_TEMPORARY)==0 )
		{
			DispOkSub(ucLogoNo);
			ucLogoNo++;
			TimerSet(TIMER_TEMPORARY, 2);
		}
	}
	DelayMs(1500);
}

uchar AskYesNo(void)
{
	PubDispString("[ NO ]  [YES ]", DISP_LINE_CENTER|6);

	return !PubYesNo(USER_OPER_TIMEOUT);
}


int SelectAcqCreditDebitOnly(uchar bAllowSelAll, uchar *pszTitle, uchar *pucAcqIndex)
{
	uchar		ucCnt, ucIndex;
	MenuItem	stAcqMenu[MAX_ACQ+1+1];
	int			iSelMenu, iMenuNum;
	uchar		idxAcq[MAX_ACQ+1+1];

	if( glTlmSysParam.ucAcqNum==0 )
	{
		ScrCls();
		PubDispString(_T("NO ACQUIRER"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}

	if((FindAcqIdxByName("CREDIT", FALSE)==MAX_ACQ)&&(FindAcqIdxByName("DEBIT", FALSE)==MAX_ACQ))
	{
		ScrCls();
		PubDispString(_T("NO ACQUIRER"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
	
	if( glTlmSysParam.ucAcqNum==1 )
	{
		SetCurAcq(0);
		if( pucAcqIndex!=NULL )
		{
			*pucAcqIndex = 0;
		}
		return 0;
	}

	// here, glSysParam.ucAcqNum must >= 2
	memset(&stAcqMenu[0], 0, sizeof(stAcqMenu));
	iMenuNum = 0;
	if( bAllowSelAll )
	{
		stAcqMenu[iMenuNum].bVisible = TRUE;
		sprintf((char *)stAcqMenu[iMenuNum].szMenuName, "SETTLE ALL");
		iMenuNum++;
	}

	for(ucCnt=0; ucCnt<glTlmSysParam.ucAcqNum; ucCnt++)
	{
		if( strstr(glTlmSysParam.Acq[ucCnt].acqName, "CREDIT")!=NULL )
		{
			stAcqMenu[iMenuNum].bVisible = TRUE;
			sprintf((char *)stAcqMenu[iMenuNum].szMenuName, "%3.3s %.10s",
				glTlmSysParam.Acq[ucCnt].acqNII, glTlmSysParam.Acq[ucCnt].acqName);

			idxAcq[iMenuNum]=glTlmSysParam.Acq[ucCnt].acqID;
			iMenuNum++;
		}
		
		else if( strstr(glTlmSysParam.Acq[ucCnt].acqName, "DEBIT")!=NULL )
		{
			stAcqMenu[iMenuNum].bVisible = TRUE;
			sprintf((char *)stAcqMenu[iMenuNum].szMenuName, "%3.3s %.10s",
				glTlmSysParam.Acq[ucCnt].acqNII, glTlmSysParam.Acq[ucCnt].acqName);

			idxAcq[iMenuNum]=glTlmSysParam.Acq[ucCnt].acqID;
			iMenuNum++;
		}
	}

	iSelMenu = PubGetMenu(pszTitle, &stAcqMenu[0], MENU_AUTOSNO|MENU_ASCII, USER_OPER_TIMEOUT);
	if( iSelMenu<0 )
	{
		return ERR_USERCANCEL;
	}
	if( bAllowSelAll )
	{
		ucIndex = (uchar)((iSelMenu==0) ? MAX_ACQ : iSelMenu-1);
	}
	else
	{
		ucIndex = (uchar)iSelMenu;
	}

	if( ucIndex!=MAX_ACQ )
	{
		SetCurAcq(idxAcq[ucIndex]);
	}
	
	if( pucAcqIndex!=NULL )
	{
		*pucAcqIndex = idxAcq[ucIndex];
	}

	return 0;
}

// 选择收单行(for settle/reprint ....)
int SelectAcq(uchar bAllowSelAll, uchar *pszTitle, uchar *pucAcqIndex)
{
	uchar		ucCnt, ucIndex;
	MenuItem	stAcqMenu[MAX_ACQ+1+1];
	int			iSelMenu, iMenuNum;

#ifdef	__TELIUM_PARAM__
	if( glTlmSysParam.ucAcqNum==0 )
	{
		ScrCls();
		PubDispString(_T("NO ACQUIRER"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
	if( glTlmSysParam.ucAcqNum==1 )
	{
		SetCurAcq(0);
		if( pucAcqIndex!=NULL )
		{
			*pucAcqIndex = 0;
		}
		return 0;
	}
#else
	if( glSysParam.ucAcqNum==0 )
	{
		ScrCls();
		PubDispString(_T("NO ACQUIRER"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
	if( glSysParam.ucAcqNum==1 )
	{
		SetCurAcq(0);
		if( pucAcqIndex!=NULL )
		{
			*pucAcqIndex = 0;
		}
		return 0;
	}
#endif
	// here, glSysParam.ucAcqNum must >= 2
	memset(&stAcqMenu[0], 0, sizeof(stAcqMenu));
	iMenuNum = 0;
	if( bAllowSelAll )
	{
		stAcqMenu[iMenuNum].bVisible = TRUE;
		sprintf((char *)stAcqMenu[iMenuNum].szMenuName, "SETTLE ALL");
		iMenuNum++;
	}

#ifdef	__TELIUM_PARAM__
	for(ucCnt=0; ucCnt<glTlmSysParam.ucAcqNum; ucCnt++)
	{
		stAcqMenu[iMenuNum].bVisible = TRUE;
		sprintf((char *)stAcqMenu[iMenuNum].szMenuName, "%3.3s %.10s",
				glTlmSysParam.Acq[ucCnt].acqNII, glTlmSysParam.Acq[ucCnt].acqName);
		iMenuNum++;
	}
#else
	for(ucCnt=0; ucCnt<glSysParam.ucAcqNum; ucCnt++)
	{
		stAcqMenu[iMenuNum].bVisible = TRUE;
		sprintf((char *)stAcqMenu[iMenuNum].szMenuName, "%3.3s %.10s",
				glSysParam.stAcqList[ucCnt].szNii, glSysParam.stAcqList[ucCnt].szName);
		iMenuNum++;
	}
#endif
	iSelMenu = PubGetMenu(pszTitle, &stAcqMenu[0], MENU_AUTOSNO|MENU_ASCII, USER_OPER_TIMEOUT);
	if( iSelMenu<0 )
	{
		return ERR_USERCANCEL;
	}
	if( bAllowSelAll )
	{
		ucIndex = (uchar)((iSelMenu==0) ? MAX_ACQ : iSelMenu-1);
	}
	else
	{
		ucIndex = (uchar)iSelMenu;
	}

	if( ucIndex!=MAX_ACQ )
	{
		SetCurAcq(ucIndex);
	}
	if( pucAcqIndex!=NULL )
	{
		*pucAcqIndex = ucIndex;
	}

	return 0;
}

// 选择发卡行
int SelectIssuer(uchar *pucIssuerIndex)
{
	uchar		ucCnt, szTitle[16+1];
	int			iMenuNo;
	MenuItem	stMenuIssuer[MAX_ISSUER+1];

#ifdef	__TELIUM_PARAM__
	if( glTlmSysParam.ucIssNum==0 )
	{
		ScrCls();
		PubDispString(_T("NO ISSUER"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
	if( glTlmSysParam.ucIssNum==1 )
	{
		SetCurIssuer(0);
		if( pucIssuerIndex!=NULL )
		{
			*pucIssuerIndex = 0;
		}
		return 0;
	}

	memset((char*)&stMenuIssuer, 0, sizeof(stMenuIssuer));
	for(ucCnt=0; ucCnt<glTlmSysParam.ucIssNum; ucCnt++)
	{
		stMenuIssuer[ucCnt].bVisible = TRUE;
		sprintf((char *)stMenuIssuer[ucCnt].szMenuName, "%.10s", glTlmSysParam.Iss[ucCnt].issRef);
	}

	sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucIssNum>9 ? "SELECT ISSUER:" : "SELECT ISSUER");
#else
	if( glSysParam.ucIssuerNum==0 )
	{
		ScrCls();
		PubDispString(_T("NO ISSUER"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
	if( glSysParam.ucIssuerNum==1 )
	{
		SetCurIssuer(0);
		if( pucIssuerIndex!=NULL )
		{
			*pucIssuerIndex = 0;
		}
		return 0;
	}

	memset((char*)&stMenuIssuer, 0, sizeof(stMenuIssuer));
	for(ucCnt=0; ucCnt<glSysParam.ucIssuerNum; ucCnt++)
	{
		stMenuIssuer[ucCnt].bVisible = TRUE;
		sprintf((char *)stMenuIssuer[ucCnt].szMenuName, "%.10s", glSysParam.stIssuerList[ucCnt].szName);
	}

	sprintf((char *)szTitle, "%.16s", glSysParam.ucIssuerNum>9 ? "SELECT ISSUER:" : "SELECT ISSUER");
#endif
	iMenuNo = PubGetMenu(szTitle, stMenuIssuer, MENU_ASCII|MENU_AUTOSNO, USER_OPER_TIMEOUT);
	if( iMenuNo<0 )
	{
		return ERR_USERCANCEL;
	}
	SetCurIssuer((uchar)iMenuNo);
	if( pucIssuerIndex!=NULL )
	{
		*pucIssuerIndex = (uchar)iMenuNo;
	}

	return 0;
}

// 清除汇总信息
void ClearTotalInfo(void *pstTotalInfo)
{
	memset(pstTotalInfo, 0, sizeof(TOTAL_INFO));

	memset(((TOTAL_INFO *)pstTotalInfo)->szSaleAmt,      		'0', 12);
	memset(((TOTAL_INFO *)pstTotalInfo)->szTipAmt,       		'0', 12);
	memset(((TOTAL_INFO *)pstTotalInfo)->szRefundAmt,     		'0', 12);
	memset(((TOTAL_INFO *)pstTotalInfo)->szVoidSaleAmt,   		'0', 12);
	memset(((TOTAL_INFO *)pstTotalInfo)->szVoidRefundAmt, 		'0', 12);
	memset(((TOTAL_INFO *)pstTotalInfo)->szSaleNonManualAmt,	'0', 12);
	memset(((TOTAL_INFO *)pstTotalInfo)->szSaleManualAmt,   	'0', 12);
	memset(((TOTAL_INFO *)pstTotalInfo)->szVoidSaleNonManualAmt,'0', 12);
	memset(((TOTAL_INFO *)pstTotalInfo)->szVoidSaleManualAmt,   '0', 12);
	
	memset(((TOTAL_INFO *)pstTotalInfo)->szSaleOnlineAmt,		'0', 12); // for BANK BUKOPIN
	memset(((TOTAL_INFO *)pstTotalInfo)->szSaleOfflineAmt,   	'0', 12); // by 4212
	memset(((TOTAL_INFO *)pstTotalInfo)->szVoidSaleOnlineAmt,	'0', 12); //
	memset(((TOTAL_INFO *)pstTotalInfo)->szVoidSaleOfflineAmt,  '0', 12); //
}

void ClearBrizziTotalInfo(void *pstTotalInfo)
{
	memset(pstTotalInfo, 0, sizeof(TOTAL_INFO));
	memset(((BRIZZI_TOTAL_INFO *)pstTotalInfo)->szSaleAmt,       '0', 12);
	memset(((BRIZZI_TOTAL_INFO *)pstTotalInfo)->szTopupOnlineAmt,        '0', 12);
	memset(((BRIZZI_TOTAL_INFO *)pstTotalInfo)->szTopupDepositAmt,     '0', 12);
	memset(((BRIZZI_TOTAL_INFO *)pstTotalInfo)->szAktivasiDepositAmt,   '0', 12);
	memset(((BRIZZI_TOTAL_INFO *)pstTotalInfo)->szRedeemAmt, '0', 12);
	memset(((BRIZZI_TOTAL_INFO *)pstTotalInfo)->szReaktivasiAmt, '0', 12);
	memset(((BRIZZI_TOTAL_INFO *)pstTotalInfo)->szVoidBrizziAmt, '0', 12);
}

int DispPurchaseTotal(uchar bShowVoidTrans)
{
	uchar	szBuff[25];

	ScrCls();
	DispTransName();
	PubDispString(glCurTlmTmsAcq.acqName, DISP_LINE_LEFT|2);

	if(ChkIfBRICredit())
	{
		if(glTransTotal.uiSaleNonManualCnt>0)
		{
			sprintf((char *)szBuff, _T("SALE      %6d"),    glTransTotal.uiSaleNonManualCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);
			DispAmount(6, glTransTotal.szSaleNonManualAmt);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}

		if(glTransTotal.uiSaleManualCnt>0)
		{
			sprintf((char *)szBuff, _T("MANUAL    %6d"),    glTransTotal.uiSaleManualCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);
			DispAmount(6, glTransTotal.szSaleManualAmt);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}

	}
	else
	{
	#ifdef BANK_BUKOPIN
		if(glTransTotal.uiSaleOnlineCnt>0)
		{
			sprintf((char *)szBuff, _T("%-13s%3d"), "SALE",    glTransTotal.uiSaleOnlineCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);
			DispAmount(6, glTransTotal.szSaleOnlineAmt);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}
		if(glTransTotal.uiSaleOfflineCnt>0)
		{
			sprintf((char *)szBuff, _T("%-13s%3d"), "OFFLINE",    glTransTotal.uiSaleOfflineCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);
			DispAmount(6, glTransTotal.szSaleOfflineAmt);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}
	#else
		if(glTransTotal.uiSaleCnt>0)
		{
			sprintf((char *)szBuff, _T("%-13s%3d"), "SALE", glTransTotal.uiSaleCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);
			DispAmount(6, glTransTotal.szSaleAmt);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}
	#endif
	}

	if(glTransTotal.uiRefundCnt>0)
	{
		ScrClrLine(4, 7);
		sprintf((char *)szBuff, _T("%-13s%3d"), "REFUND",    glTransTotal.uiRefundCnt);
		PubDispString(szBuff, 4|DISP_LINE_LEFT);

		sprintf((char *)szBuff, "%.12s", glTransTotal.szRefundAmt);
		szBuff[0] = 'D';
		DispAmount(6, szBuff);

		if( PubYesNo(USER_OPER_TIMEOUT) )
		{
			return ERR_USERCANCEL;
		}
	}
	
	if( !bShowVoidTrans )
	{
		return 0;
	}

	if(ChkIfBRICredit())
	{
		if(glTransTotal.uiVoidSaleNonManualCnt>0)
		{
			ScrClrLine(4,7);
			sprintf((char *)szBuff, _T("%-13s%3d"), "VOID SALE", glTransTotal.uiVoidSaleNonManualCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);

			sprintf((char *)szBuff, "%.12s", glTransTotal.szVoidSaleNonManualAmt);
			szBuff[0] = 'D';
			DispAmount(6, szBuff);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}
		
		if(glTransTotal.uiVoidSaleManualCnt>0)
		{
			ScrClrLine(4,7);
			sprintf((char *)szBuff, _T("%-13s%3d"), "VOID MANUAL", glTransTotal.uiVoidSaleManualCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);

			sprintf((char *)szBuff, "%.12s", glTransTotal.szVoidSaleManualAmt);
			szBuff[0] = 'D';
			DispAmount(6, szBuff);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}
	}
	else
	{
		#ifdef BANK_BUKOPIN
		if(glTransTotal.uiVoidSaleOnlineCnt>0)
		{
			sprintf((char *)szBuff, _T("%-13s%3d"), "VOID SALE", glTransTotal.uiVoidSaleOnlineCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);
			szBuff[0] = 'D';
			DispAmount(6, glTransTotal.szVoidSaleOnlineAmt);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}
		if(glTransTotal.uiVoidSaleOfflineCnt>0)
		{
			sprintf((char *)szBuff, _T("%-13s%3d"), "VOID OFFLINE", glTransTotal.uiVoidSaleOfflineCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);
			szBuff[0] = 'D';
			DispAmount(6, glTransTotal.szVoidSaleOfflineAmt);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}
		#else
		if(glTransTotal.uiVoidSaleCnt>0)
		{
			ScrClrLine(4,7);
			sprintf((char *)szBuff, _T("%-13s%3d"), "VOID SALE", glTransTotal.uiVoidSaleCnt);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);

			sprintf((char *)szBuff, "%.12s", glTransTotal.szVoidSaleAmt);
			szBuff[0] = 'D';
			DispAmount(6, szBuff);

			if( PubYesNo(USER_OPER_TIMEOUT) )
			{
				return ERR_USERCANCEL;
			}
		}
		#endif
	}

	if(glTransTotal.uiVoidRefundCnt>0)
	{
		ScrClrLine(4,7);
		sprintf((char *)szBuff, _T("VOID REFUND%5d"), glTransTotal.uiVoidRefundCnt);
		PubDispString(szBuff, 4|DISP_LINE_LEFT);
		DispAmount(6, glTransTotal.szVoidRefundAmt);

		if( PubYesNo(USER_OPER_TIMEOUT) )
		{
			return ERR_USERCANCEL;
		}
	}

	return 0;
}

// 显示交易汇总信息(glTransTotal)
int DispTransTotal(uchar bShowVoidTrans)
{
	uchar	szBuff[25];

	ScrCls();
	

#ifdef BANK_BUKOPIN
	sprintf((char *)szBuff, _T("%-13s%3d"), "SALE", glTransTotal.uiSaleOnlineCnt);
	PubDispString(szBuff, 0|DISP_LINE_LEFT);
	DispAmount(2, glTransTotal.szSaleOnlineAmt);
	
	sprintf((char *)szBuff, _T("%-13s%3d"), "OFFLINE",    glTransTotal.uiSaleOfflineCnt);
	PubDispString(szBuff, 4|DISP_LINE_LEFT);
	DispAmount(6, glTransTotal.szSaleOfflineAmt);
#else
	sprintf((char *)szBuff, _T("SALE   %9d"),    glTransTotal.uiSaleCnt);
	PubDispString(szBuff, 0|DISP_LINE_LEFT);
	DispAmount(2, glTransTotal.szSaleAmt);
	
	sprintf((char *)szBuff, _T("REFUND %9d"),    glTransTotal.uiRefundCnt);
	PubDispString(szBuff, 4|DISP_LINE_LEFT);

	sprintf((char *)szBuff, "%.12s", glTransTotal.szRefundAmt);
	szBuff[0] = 'D';
	DispAmount(6, szBuff);
#endif

	if( PubYesNo(USER_OPER_TIMEOUT) )
	{
		return ERR_USERCANCEL;
	}
	if( !bShowVoidTrans )
	{
		return 0;
	}

	ScrCls();

#ifdef BANK_BUKOPIN
	sprintf((char *)szBuff, _T("%-13s%3d"), "VOID SALE", glTransTotal.uiVoidSaleOnlineCnt);
	PubDispString(szBuff, 0|DISP_LINE_LEFT);
	sprintf((char *)szBuff, "%.12s", glTransTotal.szVoidSaleOnlineAmt);
	szBuff[0] = 'D';
	DispAmount(2, szBuff);

	sprintf((char *)szBuff, _T("%-13s%3d"), "VOID OFFLINE", glTransTotal.uiVoidSaleOfflineCnt);
	PubDispString(szBuff, 4|DISP_LINE_LEFT);
	sprintf((char *)szBuff, "%.12s", glTransTotal.szVoidSaleOfflineAmt);
	szBuff[0] = 'D';
	DispAmount(6, szBuff);
#else
	sprintf((char *)szBuff, _T("VOIDED SALE%5d"), glTransTotal.uiVoidSaleCnt);
	PubDispString(szBuff, 0|DISP_LINE_LEFT);

	sprintf((char *)szBuff, "%.12s", glTransTotal.szVoidSaleAmt);
	szBuff[0] = 'D';
	DispAmount(2, szBuff);
	
	sprintf((char *)szBuff, _T("VOIDED REFD%5d"), glTransTotal.uiVoidRefundCnt);
	PubDispString(szBuff, 4|DISP_LINE_LEFT);
	DispAmount(6, glTransTotal.szVoidRefundAmt);
#endif	

	if( !PubYesNo(USER_OPER_TIMEOUT) )
	{
		return 0;
	}

	return ERR_USERCANCEL;
}


// 显示交易汇总信息(glTransTotal)
int DispTransBrizziTotal(uchar bShowVoidTrans)
{
	uchar	szBuff[25];

	ScrCls();
	PubDispStringASCII("SETTLEMENT BRIZZI", DISP_LINE_CENTER);
	sprintf((char *)szBuff, _T("JUMLAH : %12d"),    glBrizziTotal.uiSaleCnt);
	PubDispStringASCII(szBuff, 3|DISP_LINE_LEFT);

	sprintf((char *)szBuff, _T("TOTAL  :"));
	PubDispStringASCII(szBuff, 4|DISP_LINE_LEFT);
	DispAmountASCII(5, glBrizziTotal.szSaleAmt);

	sprintf((char *)szBuff, _T("YES/NO?"));
	PubDispStringASCII(szBuff, 7|DISP_LINE_RIGHT);

	if( !PubYesNo(USER_OPER_TIMEOUT) )
	{
		return 0;
	}

	return ERR_USERCANCEL;
}


// 显示交易日志
void DispTranLog(ushort uiActRecNo, void *pTranLog)
{
	uchar		szBuff[25], szTotalAmt[12+1];
	TRAN_LOG	*pstLog;

	ScrCls();
	pstLog = (TRAN_LOG *)pTranLog;
	GetStateText(pstLog->uiStatus, szBuff);
	if( szBuff[0]!=0 )
	{
	#ifdef BANK_BUKOPIN
		if(pstLog->ucOrgTranType == TRANS_TYPE_SALE || pstLog->ucOrgTranType == TRANS_TYPE_OFF_SALE)
		PubDispString(_T(glTranConfig[pstLog->ucOrgTranType].szLabel), DISP_LINE_RIGHT|0);
		else
		PubDispString(_T(glTranConfig[pstLog->ucTranType].szLabel), DISP_LINE_RIGHT|0);
	#else
		PubDispString(_T(glTranConfig[pstLog->ucTranType].szLabel), DISP_LINE_RIGHT|0);
	#endif
		ScrPrint(0,1, ASCII, "%-10.10s", szBuff);
	}
	else
	{
		PubDispString(_T(glTranConfig[pstLog->ucTranType].szLabel), DISP_LINE_CENTER|0);
	}

	sprintf((char *)szBuff, "%03d/%03d  TRACE:%06lu", uiActRecNo, GetTranLogNum(ACQ_ALL),
			pstLog->ulInvoiceNo);

	ScrPrint(0, 2, ASCII, "%.21s", szBuff);

	if( ChkIfDispMaskPan2() )
	{
		MaskPan(pstLog->szPan, szBuff);
		ScrPrint(0, 3, ASCII, "%.21s", szBuff);
	}
	else
	{
		ScrPrint(0, 3, ASCII, "%.21s", pstLog->szPan);
	}

	PubAscAdd(pstLog->szAmount, pstLog->szTipAmount, 12, szTotalAmt);
	App_ConvAmountTran(szTotalAmt,	szBuff, GetTranAmountInfo(pstLog));
	ScrPrint(0, 4, ASCII, "%.21s", szBuff);
	ScrPrint(0, 5, ASCII, "APPROVAL CODE :%6.6s", pstLog->szAuthCode);
	Conv2EngTime(pstLog->szDateTime, szBuff);
	ScrPrint(0, 6, ASCII, "%.21s", szBuff);
	ScrPrint(0, 7, ASCII, "RRN     :%12.12s", pstLog->szRRN);
}

/*
// 调节屏幕对比度
void AdjustLcd(void)
{
	uchar	ucKey, szBuff[30];

	while( 1 )
	{
		PubShowTitle(TRUE, (uchar *)"ADJUST CONTRAST");
		sprintf((char *)szBuff, _T("STEP = [%d]"), glSysParam.stEdcInfo.ucScrGray);
		PubDispString(szBuff, DISP_LINE_CENTER|3);
		PubDispString(_T("[CANCEL] - EXIT"), DISP_LINE_CENTER|6);
		ScrGray(glSysParam.stEdcInfo.ucScrGray);
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			break;
		}
		glSysParam.stEdcInfo.ucScrGray = (glSysParam.stEdcInfo.ucScrGray+1) % 8;
	}
	SaveEdcParam();
}
*/

// 判断是否为数字串
uchar IsNumStr(char *pszStr)
{
	if( pszStr==NULL || *pszStr==0 )
	{
		return FALSE;
	}

	while( *pszStr )
	{
		if( !isdigit(*pszStr++) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 获取交易的英文名称
//void GetEngTranLabel(uchar *pszTranTitle, uchar *pszEngLabel)

// 取得金额的符号
uchar GetTranAmountInfo(void *pTranLog)
{
	uchar		ucSignChar;
	TRAN_LOG	*pstLog;

	pstLog = (TRAN_LOG *)pTranLog;
	ucSignChar =GA_SEPARATOR;
	if( pstLog->ucTranType==TRANS_TYPE_REFUND || pstLog->ucTranType==TRANS_TYPE_VOID || pstLog->uiStatus &TS_VOID )
	{
//		ucSignChar = GA_NEGATIVE;
		ucSignChar|=GA_NEGATIVE;
	}

	else if( (pstLog->ucTranType==TRANS_TYPE_VOID) && (pstLog->ucOrgTranType==TRANS_TYPE_REFUND) )
	{
//		ucSignChar = 0;
		ucSignChar |= 0;
	}

	else if( (pstLog->ucTranType==TRANS_TYPE_REFUND) && ( pstLog->uiStatus &TS_VOID))
	{
//		ucSignChar = 0;
		ucSignChar |= 0;
	}

	return ucSignChar;
}

void DispHostRspMsg(uchar *pszRspCode, HOST_ERR_MSG *pstMsgArray)
{
	int		iCnt;
	char	szDispMsg[64];
	
	for(iCnt=0; pstMsgArray[iCnt].szRspCode[0]!=0; iCnt++)
	{
		if( memcmp(pszRspCode, pstMsgArray[iCnt].szRspCode, 2)==0 )
		{
			break;
		}
	}

	sprintf(szDispMsg, (char *)(pstMsgArray[iCnt].szMsg));

	PubDispString(_T(szDispMsg), 4|DISP_LINE_CENTER);
	ScrPrint(112, 7, ASCII, "%.2s", pszRspCode);
}

void DispResult(int iErrCode)
{
	int		iCnt;

	if( (iErrCode==ERR_NO_DISP) ||
		(iErrCode==ERR_EXIT_APP)||(iErrCode==ERR_COMM_ERROR))
	{
		return;
	}

	if( iErrCode!=0 )
	{
		ScrCls();
		DispTransName();
	}

	switch( iErrCode )
	{
	case 0:
		switch( glProcInfo.stTranLog.ucTranType )
		{
		case TRANS_TYPE_BATCH_UPLOAD:
		case TRANS_TYPE_LOAD_PARA:
			apiBeef(3, 60);
			break;
		case TRANS_TYPE_TMS_TELIUM:
			ScrCls();
			DispTransName();
			ScrClrLine(2, 7);
			PubDispString(_T("COMPLETED"), 4|DISP_LINE_LEFT);
			PubBeepOk();
			PubWaitKey(2);
			break;
		case TRANS_TYPE_OFF_SALE:
		case TRANS_TYPE_VOID:
			ScrCls();
			DispTransName();
			DispAccepted();
			PubBeepOk();
#ifdef	__TELIUM_PARAM__
			PubWaitKey(glTlmSysParam.stEdcInfo.ucAcceptTimeout);
#else
			PubWaitKey(glSysParam.stEdcInfo.ucAcceptTimeout);
#endif
			break;

		case TRANS_TYPE_ECHO_TEST:
			ScrCls();
			DispTransName();
			PubDispString(_T("ECHO TEST OK"), 4|DISP_LINE_LEFT);
			PubBeepOk();
#ifdef	__TELIUM_PARAM__
			PubWaitKey(glTlmSysParam.stEdcInfo.ucAcceptTimeout);
#else
			PubWaitKey(glSysParam.stEdcInfo.ucAcceptTimeout);
#endif
			break;

		case TRANS_TYPE_LOGON_BRI:
			PubDispString("LOGON COMPLETE", 4|DISP_LINE_LEFT);
			PubBeepOk();
			PubWaitKey(glTlmSysParam.stEdcInfo.ucAcceptTimeout);
			break;

		default:
			ScrCls();
			DispTransName();
			if( glProcInfo.stTranLog.szAuthCode[0]==0 ||
				memcmp(glProcInfo.stTranLog.szAuthCode, "       ", 6)==0 )
			{
				DispAccepted();
			}
			else
			{
				ScrPrint(0, 3, CFONT, _T("  APPV CODE"));
				ScrPrint(0, 5, CFONT,    "  %.6s", glProcInfo.stTranLog.szAuthCode);
			}
			PubBeepOk();
			PubWaitKey(glTlmSysParam.stEdcInfo.ucAcceptTimeout);
		}
		break;

	case ERR_HOST_REJ:
		DispHostRspMsg(glProcInfo.stTranLog.szRspCode, glHostErrMsg);
		PubBeepErr();
		PubWaitKey(5);
		break;

	default:
		for(iCnt=0; glTermErrMsg[iCnt].iErrCode!=0; iCnt++)
		{
			if( glTermErrMsg[iCnt].iErrCode==iErrCode )
			{
				PubDispString(_T(glTermErrMsg[iCnt].szMsg), 4|DISP_LINE_CENTER);
				break;
			}
		}
		if( glTermErrMsg[iCnt].iErrCode==0 )
		{
			PubDispString(_T("SYSTEM ERROR"), 4|DISP_LINE_LEFT);
			ScrPrint(80, 7, ASCII, "%04x", iErrCode);
		}
		PubBeepErr();
		PubWaitKey(5);
	    break;
	}
}

// 计算单据数目
// Get number of receipts to pint out in one transaction.
int NumOfReceipt(void)
{
	int		iNum;

	iNum = 1;
	if( ChkEdcOption(EDC_NUM_PRINT_LOW) )
	{
		iNum++;
	}
	if( ChkEdcOption(EDC_NUM_PRINT_HIGH) )
	{
		iNum += 2;
	}

	return iNum;
}

// if any issuer ticked PIN REQUIRED option, it open the EMV offline PIN
// deleted
//void ModifyTermCapForPIN(void)

#ifdef ENABLE_EMV
// set tag 9C for EMV
void UpdateEMVTranType(void)
{
	// 设置EMV参数
	EMVGetParameter(&glEmvParam);
	switch( glProcInfo.stTranLog.ucTranType )
	{
	case TRANS_TYPE_SALE:
		glEmvParam.TransType = EMV_GOODS;
		EMVSetTLVData(0x9C, (uchar *)"\x00", 1);
		break;

	case TRANS_TYPE_AUTH:
		EMVSetTLVData(0x9C, (uchar *)"\x00", 1);
		glEmvParam.TransType = EMV_GOODS;
		break;

	case TRANS_TYPE_PREAUTH:
		EMVSetTLVData(0x9C, (uchar *)"\x30", 1);
		glEmvParam.TransType = EMV_GOODS;
		break;

	case TRANS_TYPE_CICILAN_BRING:
		glEmvParam.TransType = EMV_GOODS;
		EMVSetTLVData(0x9C, (uchar *)"\x00", 1);
		break;
	case TRANS_TYPE_CICILAN_BUKOPIN:
	case TRANS_TYPE_OFF_SALE:	
	case TRANS_TYPE_OFFLINE_SEND:
		glEmvParam.TransType = EMV_GOODS;
		EMVSetTLVData(0x9C, (uchar *)"\x00", 1);
		break;
// 	case BALANCE:
// 		EMVSetTLVData(0x9C, (uchar *)"\x31", 1);
//		glEmvParam.TransType = EMV_GOODS;
// 		break;

	default:
		return;
	}

	if(memcmp(glProcInfo.stTranLog.sAID, "\xA0\x00\x00\x00\x04", 5)==0)
		memcpy(glEmvParam.Capability,    EMV_CAPABILITY, 3);
	else
		//memcpy(glEmvParam.Capability,    "\xE0\xB8\xC8", 3);
		memcpy(glEmvParam.Capability,    "\xE0\xF8\xC8", 3);
	
	//hex_dump_char("glEmvParam.Capability",glEmvParam.Capability,3);

	EMVSetParameter(&glEmvParam);
	// Only in this trasaction, so DON'T back up
}
#endif

int FindCurrency(uchar *pszCurrencyNameCode, CURRENCY_CONFIG *pstCurrency)
{
	int	iCnt;
	uchar	sBCD[8], sBuff[8];

	for (iCnt=0; glCurrency[iCnt].szName[0]!=0; iCnt++)
	{
		if (IsNumStr(pszCurrencyNameCode))
		{
			sprintf((char *)sBuff, "0%.3s", pszCurrencyNameCode);
			PubAsc2Bcd(sBuff, 3, sBCD);
			if (memcmp(sBCD, glCurrency[iCnt].sCurrencyCode, 2)==0)
			{
				memcpy(pstCurrency, &glCurrency[iCnt], sizeof(CURRENCY_CONFIG));
				return 0;
			}
		}
		else
		{
			if (strcmp((char *)pszCurrencyNameCode, (char *)glCurrency[iCnt].szName)==0)
			{
				memcpy(pstCurrency, &glCurrency[iCnt], sizeof(CURRENCY_CONFIG));
				return 0;
			}
		}
	}

	return -1;
}

#ifdef ENABLE_EMV
// 根据EDC参数设定EMV库的国家代码和货币特性
// Setup EMV core parameter due to EDC para
void SyncEmvCurrency(uchar *psCountryCode, uchar *psCurrencyCode, uchar ucDecimal)
{
	EMVGetParameter(&glEmvParam);
	if ((memcmp(psCountryCode,  glEmvParam.CountryCode,   2)!=0) ||
        (memcmp(psCurrencyCode, glEmvParam.TransCurrCode, 2)!=0) ||
        (glEmvParam.TransCurrExp!=ucDecimal) )
	{
		memcpy(glEmvParam.CountryCode,   psCountryCode,  2);
		memcpy(glEmvParam.TransCurrCode, psCurrencyCode, 2);
		memcpy(glEmvParam.ReferCurrCode, psCurrencyCode, 2);
		glEmvParam.TransCurrExp = ucDecimal;
		glEmvParam.ReferCurrExp = ucDecimal;
		EMVSetParameter(&glEmvParam);
	}
}
#endif

// Read monitor config info, by API: GetTermInfo()
// return: 0--No need save; 1--Need save
// 读取monitor保存的系统配置信息
// 返回：0－－不需要保存更新；1－－需要保存
int UpdateTermInfo(void)
{
	int		iRet;
	uchar	ucNeedUpdate;
	uchar 	sBuff[HWCFG_END];


	ucNeedUpdate = 0;

#if defined(_P70_S_) || defined(_P60_S1_)
	
#else
	while (1)
	{
		memset(sBuff, 0, sizeof(sBuff));
		iRet = GetTermInfo(sBuff);
		if ( iRet<0 )
		{
#ifdef _WIN32
			ScrClrLine(2, 7);
			PubDispString(_T("CONNECT SIMULTR."), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			continue;
#else
			SysHaltInfo(_T("FAIL GET SYSINFO"));
#endif
		}

		break;
	}

	if (memcmp(sBuff, glTlmSysParam.sTermInfo, sizeof(glTlmSysParam.sTermInfo))!=0)
	{
		memcpy(glTlmSysParam.sTermInfo, sBuff, sizeof(glTlmSysParam.sTermInfo));
		ucNeedUpdate = 1;
	}
#endif

	return ucNeedUpdate;
}

#if defined(_S_SERIES_) || defined(_SP30_) || defined(_P58_)
static void ShowMsgFontMissing(uchar bIsPrnFont, ST_FONT *psingle_code_font, ST_FONT *pmulti_code_font, int iErrCode)
{
	uchar	szBuff1[32], szBuff2[32];
	sprintf((char *)szBuff1, "%02d:%d*%d %s%s",
					psingle_code_font->CharSet,
					psingle_code_font->Width, psingle_code_font->Height,
					(psingle_code_font->Bold ? "B" : ""), (psingle_code_font->Italic ? "I" : ""));
	sprintf((char *)szBuff2, "%02d:%d*%d %s%s %d",
					pmulti_code_font->CharSet,
					pmulti_code_font->Width, pmulti_code_font->Height,
					(pmulti_code_font->Bold ? "B" : ""), (pmulti_code_font->Italic ? "I" : ""), iErrCode);

	ScrClrLine(2, 7);
	PubDispString(szBuff1, 2|DISP_LINE_LEFT);
	PubDispString(szBuff2, 4|DISP_LINE_LEFT);
	if (bIsPrnFont)
	{
		PubDispString("PRN FONT MISS", 6|DISP_LINE_LEFT);
	} 
	else
	{
		PubDispString("DISP FONT MISS", 6|DISP_LINE_LEFT);
	}
	PubWaitKey(-1);
}
#endif

// Check whether system has fonts(for print and display) required under selected language.
// 在多语言动态切换环境下，检查系统是否有已选择语言所对应的打印和显示字库
int CheckSysFont(void)
{
#if defined(_S_SERIES_) || defined(_SP30_) || defined(_P58_)
	int		iRet, iRet1, iRet2, ii;

	ST_FONT	stPrnFonts[] = {
		{CHARSET_WEST, 8,  16, 0, 0},	{-1, 16, 16, 0, 0},
		{CHARSET_WEST, 12, 24, 0, 0},	{-1, 24, 24, 0, 0},
	};

	ST_FONT	stDispFonts[] = {
		{CHARSET_WEST, 8, 16, 0, 0}, {-1, 16, 16, 0, 0},
	};

#ifdef AREA_Arabia
	if(strcmp(LANGCONFIG, "Arabia") == 0)
	{
        iRet = CustomizeAppLibForArabiaLang(TRUE);
		return iRet;
	}
	else
	{
        CustomizeAppLibForArabiaLang(FALSE);
        // then continue
	}	
#endif	

	iRet = 0;
	// 检查是否有打印/显示字库 Check printer/display used fonts
	// for non-S60, display and print share the same fonts
	PrnInit();
	for (ii=0; ii<sizeof(stPrnFonts)/sizeof(ST_FONT); ii+=2)
	{
		if (stPrnFonts[ii+1].CharSet==-1)// 换成系统当前已选择的语言的编码
		{
			stPrnFonts[ii+1].CharSet = glTlmSysParam.stEdcInfo.stLangCfg.ucCharSet;
		}

		iRet2 = PrnSelectFont(&stPrnFonts[ii], &stPrnFonts[ii+1]);
		if (iRet2)
		{
			ShowMsgFontMissing(TRUE, &stPrnFonts[ii], &stPrnFonts[ii+1], iRet2);
			iRet = -1;
		}
	}
	// 如果是S60，还要检查手机显示用字库 Check display-used fonts on handset of S60
	if (ChkTerm(_TERMINAL_S60_))
	{
		for (ii=0; ii<sizeof(stDispFonts)/sizeof(ST_FONT); ii+=2)
		{
			if (stDispFonts[ii+1].CharSet==-1)// 换成系统当前已选择的语言的编码
			{
				stDispFonts[ii+1].CharSet = glTlmSysParam.stEdcInfo.stLangCfg.ucCharSet;
			}
			
			iRet1 = ScrSelectFont(&stDispFonts[ii], &stDispFonts[ii+1]);
			if (iRet1)
			{
				ShowMsgFontMissing(FALSE, &stDispFonts[ii], &stDispFonts[ii+1], iRet1);
				iRet = -1;
			}
		}
	}

	return iRet;

#else
	return 0;

#endif
}

// Enumerates all supported fonts in POS.
// 列举系统字库
void EnumSysFonts(void)
{
#if defined(_S_SERIES_) || defined(_SP30_) || defined(_P58_)
	int			iRet, ii;
	ST_FONT		stFontsList[16];
	MenuItem	stInfoList[16+1];

	PubShowTitle(TRUE, _T("ENUM SYS FONTS"));

	iRet = EnumFont(stFontsList, 16);
	if (iRet<1)
	{
		PubDispString(_T("ENUM FAILED"), 4|DISP_LINE_LEFT);
		PubWaitKey(USER_OPER_TIMEOUT);
		return;
	}

	memset(stInfoList, 0, sizeof(stInfoList));
	for (ii=0; ii<iRet; ii++)
	{
		stInfoList[ii].bVisible = TRUE;
		stInfoList[ii].pfMenuFunc = NULL;
		sprintf((char *)stInfoList[ii].szMenuName, "%02d. %02d-%02dx%02d,%d,%d",
				ii+1, stFontsList[ii].CharSet, stFontsList[ii].Width, stFontsList[ii].Height,
				stFontsList[ii].Bold, stFontsList[ii].Italic);
	}
	stInfoList[ii].bVisible = FALSE;
	stInfoList[ii].pfMenuFunc = NULL;
	stInfoList[ii].szMenuName[0] = 0;
	PubGetMenu(_T("ENUM FONTS"), stInfoList, MENU_USESNO|MENU_ASCII, USER_OPER_TIMEOUT);

#else
	PubDispString(_T("NOT SUPPORTED"), 4|DISP_LINE_LEFT);
	PubWaitKey(USER_OPER_TIMEOUT);

#endif

	return;
}

#if defined(_S_SERIES_) || defined(_SP30_)
int SxxWriteKey(uchar ucSrcKeyIdx, uchar *psKeyBCD, uchar ucKeyLen, uchar ucDstKeyId, uchar ucDstKeyType, uchar *psKCV)
{
	int			iRet;
	ST_KEY_INFO	stKeyInfoIn;
	ST_KCV_INFO	stKcvInfoIn;

	memset(&stKeyInfoIn, 0, sizeof(stKeyInfoIn));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));

	memcpy(stKeyInfoIn.aucDstKeyValue, psKeyBCD, ucKeyLen);
	stKeyInfoIn.iDstKeyLen   = ucKeyLen;
	stKeyInfoIn.ucDstKeyIdx  = ucDstKeyId;
	stKeyInfoIn.ucDstKeyType = ucDstKeyType;
	stKeyInfoIn.ucSrcKeyIdx  = ucSrcKeyIdx;
	stKeyInfoIn.ucSrcKeyType = PED_TMK;

	if (psKCV==NULL)
	{
		stKcvInfoIn.iCheckMode = 0;
	}
	else
	{
		stKcvInfoIn.iCheckMode = 1;
		stKcvInfoIn.aucCheckBuf[0] = 4;
		memcpy(stKcvInfoIn.aucCheckBuf+1, psKCV, 4);
	}

	iRet = PedWriteKey(&stKeyInfoIn, &stKcvInfoIn);
	return iRet;
}
#endif


uchar ChkOnBase(void)
{
#ifdef _P60_S1_
	return OnBase();

#elif defined(_S_SERIES_)
	if (!ChkTerm(_TERMINAL_S60_))
	{
		return 0;
	}
	return OnBase();

#else
	return 0;

#endif
}

void SetOffBase(unsigned char (*Handle)())
{
#ifdef _P60_S1_
	SetOffBaseProc(Handle);

#elif defined(_S_SERIES_)
	if (!ChkTerm(_TERMINAL_S60_))
	{
		return;
	}
	if (Handle==NULL)
	{
		//ScrPrint(0,0,0," SET NULL ");DelayMs(500);ScrPrint(0,0,0,"          ");
		ScrSetEcho(0);
		return;
	}
	//ScrPrint(0,0,0," SET ECHO ");DelayMs(500);ScrPrint(0,0,0,"          ");
	ScrSetOutput(2);
	(*Handle)();
	ScrSetEcho(2);
	ScrSetOutput(1);
	ScrSetEcho(1);

#else
	return;

#endif
}

uchar OffBaseDisplay(void)
{
#ifdef _P60_S1_
	if( OnBase()==0 )
	{
		return 0;
	}

	ScrRestore(0);
	ScrClrLine(2, 7);
	PubDispString(_T("RETURN TO BASE"), 4|DISP_LINE_LEFT);
	PubBeepErr();

	TimerSet(TIMER_TEMPORARY, 30);
	while( OnBase()!=0 )
	{
		if( TimerCheck(TIMER_TEMPORARY)==0 )
		{
			TimerSet(TIMER_TEMPORARY, 10);
			Beef(6, 200);
		}
	}
	kbflush();
	ScrRestore(1);
	return 0;

#elif defined(_S_SERIES_)
	ScrClrLine(2, 7);
	PubDispString(_T("RETURN TO BASE"), 4|DISP_LINE_LEFT);
	return 0;

#else
	return 0;

#endif
}

uchar OffBaseCheckPrint(void)
{
#ifdef _P60_S1_
	if( OnBase()==0 )
	{
		return 0;
	}

	ScrRestore(0);
	ScrClrLine(2, 7);
	PubDispString(_T("RETURN TO BASE"), 4|DISP_LINE_LEFT);
	PubBeepErr();

	TimerSet(TIMER_TEMPORARY, 30);
	while( OnBase()!=0 )
	{
		if( PubKeyPressed() )
		{
			if( getkey()==KEYCANCEL )
			{
				PubDispString(_T("  PRINT CANCEL  "), 4|DISP_LINE_LEFT);
				DelayMs(1000);
				kbflush();
				return KEYCANCEL;
			}
		}
		if( TimerCheck(TIMER_TEMPORARY)==0 )
		{
			TimerSet(TIMER_TEMPORARY, 10);
			Beef(6, 200);
		}
	}
	kbflush();
	ScrRestore(1);
	return 0;

#elif defined(_S_SERIES_)
	if( OnBase()==0 )
	{
		return 0;
	}
	return OffBaseDisplay();

#else
	return 0;

#endif
}

uchar OffBaseCheckEcr(void)
{
#ifdef _P60_S1_
	if( OnBase()==0 )
	{
		return 0;
	}

	ScrRestore(0);
	ScrClrLine(2, 7);
	PubDispString(_T("RETURN TO BASE"), 4|DISP_LINE_LEFT);
	PubBeepErr();

	TimerSet(TIMER_TEMPORARY, 30);
	while(OnBase()!=0)
	{
		if( PubKeyPressed() )
		{
			if( getkey()==KEYCANCEL )
			{
				PubDispString(_T(" ECR SEND CANCEL"), 4|DISP_LINE_LEFT);
				DelayMs(1000);
				kbflush();
				return KEYCANCEL;
			}
		}
		if( TimerCheck(TIMER_TEMPORARY)==0 )
		{
			TimerSet(TIMER_TEMPORARY, 10);
			Beef(6, 200);
		}
	}
	kbflush();
	ScrRestore(1);

	return 0;

#elif defined(_S_SERIES_)
	if( OnBase()==0 )
	{
		return 0;
	}
	return OffBaseDisplay();

#else
	return 0;

#endif
}

//added by 4212
int GetPIN_ForcePIN(uchar ucFlag)
{
	int		iRet;
	uchar	szPAN[20+1], szTotalAmt[12+1];
	uchar	ucPinKeyID;
//	uchar	ucAmtFlag, szBuff[25];
#if defined(_P60_S1_) || defined(_P80_)
	uchar	ucLen;
#endif
	uchar pin[6+1];
	uchar data[2+1];
	uchar blok1[16+1];
	uchar blok2[16+1];
	uchar blok1Byte[8+1];
	uchar blok2Byte[8+1];
	uchar dataXor[8+1];
	uchar TMK[8];
	uchar TWK[8];
	uchar TPK[8];
	int len;
	

	memset(TPK, 0, sizeof(TPK));
	memset(TMK, 0, sizeof(TMK));
	memset(TWK, 0, sizeof(TWK));

	// 非EMV PIN的模式下，如果是chip则直接返回，不是chip则检查ISSUER
	/*// //4212
	if( !(ucFlag & GETPIN_EMV) )
	{
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) )
		{
			return 0;
		}
#ifdef	__TELIUM_PARAM__
		else if(glCurTlmTmsIss.issPinOpt != 0x32)
		{
			return 0;
		}
#else
		else if( !ChkIssuerOption(ISSUER_EN_PIN) )
		{
			return 0;
		}
#endif
	}
	/*/
	// in non-EMV-PIN mode, if it is chip transaction then return directly
	if( !(ucFlag & GETPIN_EMV)) 		//4212
	{	
#ifdef	__TELIUM_PARAM__
		if(glCurTlmTmsIss.issPinOpt == 0x31 ){ // pin optionaly 
 			switch(PubSelectYesNo("INPUT PIN ?","YES","NO")){
				case KEYENTER:
					break;
				case KEYCANCEL:
					return 0;
					break;
			}
		}
		else {
			if(glCurTlmTmsIss.issPinOpt == 0x30) // no pin require
				return 0;
		}
#else
		if( !ChkIssuerOption(ISSUER_EN_PIN) )
		{
			return 0;
		}
#endif
	}
	//*/
	
	ucPinKeyID = DEF_PIN_KEY_ID;	//!!!! : 预留扩展：ACQUIRER可定义自己使用的ID

	iRet = ExtractPAN(glProcInfo.stTranLog.szPan, szPAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);

	ucFlag &= (uchar)(0xFF-GETPIN_EMV);
	ScrClrLine(2, 7);

	if( ucFlag==0 )
	{
		PubDispString(_T("INPUT PIN ?"), 4|DISP_LINE_LEFT);
	}
	else
	{
		PubDispString(_T("PIN ERR, RETRY"), 4|DISP_LINE_CENTER);
	}


	if (ChkTermPEDMode(PED_INT_PCI) || ChkTermPEDMode(PED_INT_PXX))
	{
		ScrGotoxy(32, 6);
	}

	memset(glProcInfo.sPinBlock, 0, sizeof(glProcInfo.sPinBlock));

	// !!!! to be check
#if defined(_S_SERIES_) || defined(_SP30_)
	//-------------- Internal PCI PED --------------
	if (ChkTermPEDMode(PED_INT_PCI))
	{
		memset(pin, 0, sizeof(pin));
		iRet= PubGetString(PASS_IN, 4, 6, pin, USER_OPER_TIMEOUT);	// irf
		if( iRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		if((strlen(pin)>=4)&&(strlen(pin)<=6))
		{
			memset(blok1, 0, sizeof(blok1));
			memset(blok2, 0, sizeof(blok2));
			memset(blok1Byte, 0, sizeof(blok1Byte));
			memset(blok2Byte, 0, sizeof(blok2Byte));
			memset(dataXor, 0, sizeof(dataXor));
			memset(data, 0, sizeof(data));

	
			//LoadTMK(TMK);
			//LoadTWK(TWK);

			memcpy(TMK, glCurTlmTmsAcq.acqMasterKey1, lenMKey);
			memcpy(TWK, glSysCtrl.WorkingKey, lenMKey);

//			hex_dump_char("TMK", TMK, sizeof(TMK));
//			hex_dump_char("TWK", TWK, sizeof(TWK));


			PubDes(ONE_DECRYPT, TWK, TMK, TPK);
//			hex_dump_char("TPK", TPK, sizeof(TPK));


			len=strlen(pin);

			sprintf(data, "%02d", len); 
			
			memcpy(blok1, data, 2);

//			hex_dump_char("PIN", pin, len);
			memcpy(blok1 + 2, pin, len);
//			hex_dump_char("blok1", blok1, sizeof(blok1));
			
			fmtPad(blok1, 16, 0x46);
//			hex_dump_char("blok1", blok1, sizeof(blok1));

			memcpy(blok2, szPAN, 16);
			PubAsc2Bcd(blok1, 8*2, blok1Byte);
			PubAsc2Bcd(blok2, 8*2, blok2Byte);

//			hex_dump_char("blok1Byte", blok1Byte, sizeof(blok1Byte));
//			hex_dump_char("blok2Byte", blok2Byte, sizeof(blok2Byte));

			
			PubXor(blok1Byte, blok2Byte, 8, dataXor);

//			hex_dump_char("dataXor", dataXor, sizeof(dataXor));

			memcpy(glProcInfo.sPlainPinBlock, dataXor, 8); 

			PubDes(ONE_ENCRYPT, dataXor, TPK, glProcInfo.sPinBlock);
//			hex_dump_char("sPinBlock", glProcInfo.sPinBlock, sizeof(glProcInfo.sPinBlock));

			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
			
			GetPinKeyPerAcq();

			return 0;
		}
		
		return ERR_NO_DISP;
	}
#endif

	//-------------- External PCI PED --------------
	if (ChkTermPEDMode(PED_EXT_PCI))
	{
		// !!!! to be implemented
		ScrClrLine(2, 7);
		PubDispString("EXT PCI PINPAD ",  4|DISP_LINE_LEFT);
		PubDispString("NOT IMPLEMENTED.", 6|DISP_LINE_LEFT);
		PubWaitKey(30);
		return ERR_NO_DISP;
	}
	
	//-------------- PXX PED, and PINPAD --------------
#if defined(_P60_S1_) || defined(_P80_)
	if (ChkTermPEDMode(PED_INT_PXX))
	{
		iRet = PEDGetPwd_3Des(ucPinKeyID, 4, 6, szPAN, &ucLen, glProcInfo.sPinBlock);
	} 
#endif

	
	if( iRet==0 )
	{
		glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		return 0;
	}
	else if( iRet==0x0A )
	{
		return 0;	// PIN by pass
	}
	else if( iRet==0x06 || iRet==0x07 )
	{
		return ERR_USERCANCEL;
	}

	DispPPPedErrMsg((uchar)iRet);

	return ERR_NO_DISP;
}

//added by irf, 20121128
int GetPIN_Manual(uchar ucFlag)
{
	int		iRet;
	uchar	szPAN[20+1], szTotalAmt[12+1];
	uchar	ucPinKeyID;
//	uchar	ucAmtFlag, szBuff[25];
#if defined(_P60_S1_) || defined(_P80_)
	uchar	ucLen;
#endif
	uchar pin[6+1];
	uchar data[2+1];
	uchar blok1[16+1];
	uchar blok2[16+1];
	uchar blok1Byte[8+1];
	uchar blok2Byte[8+1];
	uchar dataXor[8+1];
	uchar TMK[8];
	uchar TWK[8];
	uchar TPK[8];
	int len;
	

	memset(TPK, 0, sizeof(TPK));
	memset(TMK, 0, sizeof(TMK));
	memset(TWK, 0, sizeof(TWK));

	// 非EMV PIN的模式下，如果是chip则直接返回，不是chip则检查ISSUER
	/*// //4212
	if( !(ucFlag & GETPIN_EMV) )
	{
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) )
		{
			return 0;
		}
#ifdef	__TELIUM_PARAM__
		else if(glCurTlmTmsIss.issPinOpt != 0x32)
		{
			return 0;
		}
#else
		else if( !ChkIssuerOption(ISSUER_EN_PIN) )
		{
			return 0;
		}
#endif
	}
	/*/
	// in non-EMV-PIN mode, if it is chip transaction then return directly
	if( !(ucFlag & GETPIN_EMV) && !(ucFlag & GETPIN_RETRY)) 		//4212
	{	
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT))
		{		
			return 0;
		}
#ifdef	__TELIUM_PARAM__ // 0x31 >> optional pin required , 0x32 >> pin required 
		else {
			if(glCurTlmTmsIss.issPinOpt == 0x31 ){ // pin optionaly 				
 			switch(PubSelectYesNo("INPUT PIN ?","YES","NO")){
				case KEYENTER:
					break;
				case KEYCANCEL:
					return 0;
					break;
				}
			}
			else {
				if(glCurTlmTmsIss.issPinOpt == 0x30 )
					return 0;
			}
		}
#else	
		else {
			if( !ChkIssuerOption(ISSUER_EN_PIN) )
			return 0;
		}
#endif
	}

			
	//*/
	
	ucPinKeyID = DEF_PIN_KEY_ID;	//!!!! : 预留扩展：ACQUIRER可定义自己使用的ID

	iRet = ExtractPAN(glProcInfo.stTranLog.szPan, szPAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);

	ucFlag &= (uchar)(0xFF-GETPIN_EMV);
	ScrClrLine(2, 7);
	
	if( ucFlag==0 )
	{
		PubDispString(_T("INPUT PIN ?"), 4|DISP_LINE_LEFT);
	}
	else
	{
		PubDispString(_T("PIN ERR, RETRY"), 4|DISP_LINE_CENTER);
	}
	
	if (ChkTermPEDMode(PED_INT_PCI) || ChkTermPEDMode(PED_INT_PXX))
	{
		ScrGotoxy(32, 6);
	}

	memset(glProcInfo.sPinBlock, 0, sizeof(glProcInfo.sPinBlock));

	// !!!! to be check
#if defined (_S_SERIES_) || defined (_SP30_)	

	//-------------- Internal PCI PED --------------
	if (ChkTermPEDMode(PED_INT_PCI))
	{
		memset(pin, 0, sizeof(pin));

		if(CheckExpBypassPin())
		{
			iRet= PubGetString(PASS_IN, 4, 6, pin, USER_OPER_TIMEOUT);	// irf
		}
		else
		{
			iRet= PubGetString_IncludeByPass(PASS_IN, 4, 6, pin, USER_OPER_TIMEOUT);	// 4212
		}
		if( iRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		if((strlen(pin)>=4)&&(strlen(pin)<=6))
		{
			memset(blok1, 0, sizeof(blok1));
			memset(blok2, 0, sizeof(blok2));
			memset(blok1Byte, 0, sizeof(blok1Byte));
			memset(blok2Byte, 0, sizeof(blok2Byte));
			memset(dataXor, 0, sizeof(dataXor));
			memset(data, 0, sizeof(data));

	
			//LoadTMK(TMK);
			//LoadTWK(TWK);

			memcpy(TMK, glCurTlmTmsAcq.acqMasterKey1, lenMKey);
			memcpy(TWK, glSysCtrl.WorkingKey, lenMKey);

//			hex_dump_char("TMK", TMK, sizeof(TMK));
//			hex_dump_char("TWK", TWK, sizeof(TWK));


			PubDes(ONE_DECRYPT, TWK, TMK, TPK);
//			hex_dump_char("TPK", TPK, sizeof(TPK));


			len=strlen(pin);

			sprintf(data, "%02d", len); 
			
			memcpy(blok1, data, 2);

//			hex_dump_char("PIN", pin, len);
			memcpy(blok1 + 2, pin, len);
//			hex_dump_char("blok1", blok1, sizeof(blok1));
			
			fmtPad(blok1, 16, 0x46);
//			hex_dump_char("blok1", blok1, sizeof(blok1));

			memcpy(blok2, szPAN, 16);
			PubAsc2Bcd(blok1, 8*2, blok1Byte);
			PubAsc2Bcd(blok2, 8*2, blok2Byte);

//			hex_dump_char("blok1Byte", blok1Byte, sizeof(blok1Byte));
//			hex_dump_char("blok2Byte", blok2Byte, sizeof(blok2Byte));

			
			PubXor(blok1Byte, blok2Byte, 8, dataXor);

//			hex_dump_char("dataXor", dataXor, sizeof(dataXor));

			memcpy(glProcInfo.sPlainPinBlock, dataXor, 8); 

			PubDes(ONE_ENCRYPT, dataXor, TPK, glProcInfo.sPinBlock);
//			hex_dump_char("sPinBlock", glProcInfo.sPinBlock, sizeof(glProcInfo.sPinBlock));

			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
			
			GetPinKeyPerAcq();

			return 0;
		}
		else if(strlen(pin)==0)
		{
			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_BYPASS;
		}
		return ERR_NO_DISP;
	}
#endif

	//-------------- External PCI PED --------------
	if (ChkTermPEDMode(PED_EXT_PCI))
	{
		// !!!! to be implemented
		ScrClrLine(2, 7);
		PubDispString("EXT PCI PINPAD ",  4|DISP_LINE_LEFT);
		PubDispString("NOT IMPLEMENTED.", 6|DISP_LINE_LEFT);
		PubWaitKey(30);
		return ERR_NO_DISP;
	}
	
	//-------------- PXX PED, and PINPAD --------------
#if defined(_P60_S1_) || defined(_P80_)
	if (ChkTermPEDMode(PED_INT_PXX))
	{
		iRet = PEDGetPwd_3Des(ucPinKeyID, 4, 6, szPAN, &ucLen, glProcInfo.sPinBlock);
	} 
#endif

	
	if( iRet==0 )
	{
		glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		return 0;
	}
	else if( iRet==0x0A )
	{
		return 0;	// PIN by pass
	}
	else if( iRet==0x06 || iRet==0x07 )
	{
		return ERR_USERCANCEL;
	}

	DispPPPedErrMsg((uchar)iRet);

	return ERR_NO_DISP;
}



/** Pad the string str to length abs(len) by the character chr.
 * It is right padded if len>0 and left padded otherwise.
 * The function returns the length of the resulted padded string, strlen(str).
 * \param str (I/O) The original input string; the padded output string replaces it
 * \param len (I) Number of characters in the padded string.
 *        The negative value indicates left padding, the positive value provides
 *        right padding.
 *        It is the absolute value abs(len) that is the real length of resulting string
 * \param chr (I) Pad character
 * \pre 
 *    - str!=0
 *    - len!=0
 * \return  
 *    - the length of the result string strlen(str)
*/
/*
int fmtPad(char *str, int len, char chr) {
    int dim;                    //the initial string length
    char *ptr;                  //points to the start of padding
    char dir;                   //pad direction (left or right)

    dir = 'r';
    if(len < 0) {               //left pad?
        dir = 'l';
        len = -len;
    }
    str[len] = 0;

    dim = strlen(str);
    if(dim >= len)
        return dim;             //string is long enough - nothing to pad

    ptr = str;                  //ptr will pont to the start of padding
    if(dir == 'l')
        memmove(str + len - dim, str, dim); //left padding: move characters to the right
    else
        ptr += dim;             //right padding: move the padding pointer to the end of the string

    memset(ptr, chr, len - dim);    //now pad dim characters chr starting from ptr

    return strlen(str);
}
*/

int decTDesCBC(byte *dataIn, int lenInput, byte *dataOut, byte *key){
	int div, i;
	byte temp[8+1];
	byte buf[8+1];
	byte bufIn[1024];

	if (lenInput < 8 ) return 0;
	div = (lenInput - 1) / 8;
	if (div == 0) {
		PubDes(TRI_ENCRYPT, dataIn, key, dataOut);
		return 1;
	}

	memset(temp, 0, 8);
	memset(buf, 0, 8);
	memset(bufIn, 0, sizeof(bufIn));
	memcpy(bufIn, dataIn, lenInput);

	for (i=0; i<=div ; i++){
		PubDes(TRI_DECRYPT, bufIn + 8 * i, key, buf);
		PubXor(buf, temp, 8, dataOut + 8 * i);

		memset(temp, 0, 8);
		memcpy(temp, bufIn + 8 * i, 8);
	}

	return 1;
}


int TDesCBC(byte *dataIn, int lenInput, byte *dataOut, byte *key){
	int div, i;
	byte temp[8+1];
	byte buf[8+1];
	byte bufIn[1024];

	if (lenInput < 8 ) return 0;
	div = (lenInput - 1) / 8;
	if (div == 0) {
		PubDes(TRI_ENCRYPT, dataIn, key, dataOut);
		return 1;
	}

	memset(temp, 0, 8);
	memset(buf, 0, 8);
	memset(bufIn, 0, sizeof(bufIn));
	memcpy(bufIn, dataIn, lenInput);

	PubXor(bufIn, temp, 8, buf);
	PubDes(TRI_ENCRYPT, buf, key, dataOut);

	for (i = 1; i <= div; i++) {
		memset(buf, 0, 8);
		PubXor(bufIn + 8 * i, dataOut + 8 * (i - 1), 8, buf);
		PubDes(TRI_ENCRYPT, buf,  key, dataOut + 8 * i);
	}

	return 1;
}


void hex_dump(char *txt, void * buffer, uint len)
{
	char tg[25];
	char *ptr;
	uint i;
	uchar n, index=0;
//	uchar ucRet;
	
	ptr = (char *)buffer;
	n = 0;
	PrnInit();
	PrnSetNormal();
	PrnStr("%s\n", txt);
	sprintf(tg, "LEN:%u Addr:%p", len, buffer);
	PrnStr("%s\n", tg);
	memset( tg, 0x00, 25 );
	for(i=0; i<len; i++)
	{
		if (n == 0)
			sprintf(&tg[n*3], "%02u|%02X ", index, *(uchar *)&ptr[i]);
		else
			sprintf(&tg[n*3+3], "%02X ", *(uchar *)&ptr[i]);

		if( n == 6 )
		{
			PrnStr("%s\n", tg);
			index++;
			n = 0;
			memset( tg, 0x00, 25 );
		}
		else
			n++;
	}
	if (n != 0)
		PrnStr("%s S\n", tg);

	PrnStart();
}

void hex_dump_char_New(const char * pzTitle, const uchar * buff, long len)
{
	card x, y;
	char buffer[1024];
	char buffer2[1024];
	//char szBuff[32+1];
	char szBuffTemp[1024];
	char ascii[48];
	byte start;
	int slen;
	int pos;
	byte *buffdata;
	int counter;		// used to count line number

	start = 0;
	counter = 0;
	pos = 0;

	MEM_ZERO(buffer);
	MEM_ZERO(ascii);
	//MEM_ZERO(szBuff);
	MEM_ZERO(szBuffTemp);
	
	// copy the data to another memory
	buffdata = (byte *)buff;

	// print title, len and memory address first
	{
		PrnInit();
		PrnSetNormal();
		//PrnFontSet(0, 0);
		PrnSpaceSet(1, 2);
		
		//PrnStr("%s\n", pzTitle);
		//sprintf(szBuff, "Len:%u   Addr:%p", (int)len, buff);
		//PrnStr("%s\n", szBuff);
	}
	
	// if there is no data in the buffer
	// memory is empty
	if (len == 0 || !buffdata) return;

	for (x=0; x<(card)len; x++)
	{
		if (start == 0) // first column w/ line number
		{
			sprintf(&buffer[0], "%02d | %02X ", counter, *buffdata);
			counter++;
		}
		else // next column
		{
			slen = strlen(buffer);
			sprintf(&buffer[slen], "%02X ", *buffdata);
		}

		// get ascii character
		sprintf(&ascii[pos], "%c", ((*buffdata <= 0x1f || *buffdata > 0x7f) ? '.' : *buffdata));

		// next byte
		*buffdata++;
		pos++;

		if (start == 7)
		{
			start = 0;
			pos = 0;

			sprintf(szBuffTemp, "%s", ascii);
			PrnStr("%s\n", szBuffTemp);
			
			MEM_ZERO(buffer);
			MEM_ZERO(ascii);			
			MEM_ZERO(szBuffTemp);
		}
		else
			start++;
	}

	if (start <= 7)
	{
		MEM_ZERO(buffer2);
		MEM_ZERO(szBuffTemp);

		memcpy(buffer2, buffer, strlen(buffer));
		for (y = 0; y < (29 - strlen(buffer2)); y++)
		{
			slen = strlen(buffer);
			sprintf(&buffer[slen], "%s", " "); // fill the empty column with space
		}
		
		sprintf(szBuffTemp, " %s",  ascii);

		PrnStr("%s\n", szBuffTemp);
		
	}
	PrnStart();
}

void hex_dump_char(const char * pzTitle, const uchar * buff, long len)
{
	card x, y;
	char buffer[1024];
	char buffer2[1024];
	char szBuff[32+1];
	char szBuffTemp[1024];
	char ascii[16+1];
	byte start;
	int slen;
	int pos;
	byte *buffdata;
	int counter;		// used to count line number

	start = 0;
	counter = 0;
	pos = 0;

	MEM_ZERO(buffer);
	MEM_ZERO(ascii);
	MEM_ZERO(szBuff);
	MEM_ZERO(szBuffTemp);
	
	// copy the data to another memory
	buffdata = (byte *)buff;

	// print title, len and memory address first
	{
		PrnInit();
	//	PrnSetNormal();
		PrnFontSet(0, 0);
		PrnSpaceSet(1, 2);
		
		PrnStr("%s\n", pzTitle);
		sprintf(szBuff, "Len:%u   Addr:%p", (int)len, buff);
		PrnStr("%s\n", szBuff);
	}
	
	// if there is no data in the buffer
	// memory is empty
	if (len == 0 || !buffdata) return;

	for (x=0; x<(card)len; x++)
	{
		if (start == 0) // first column w/ line number
		{
			sprintf(&buffer[0], "%02d | %02X ", counter, *buffdata);
			counter++;
		}
		else // next column
		{
			slen = strlen(buffer);
			sprintf(&buffer[slen], "%02X ", *buffdata);
		}

		// get ascii character
		sprintf(&ascii[pos], "%c", ((*buffdata <= 0x1f || *buffdata > 0x7f) ? '.' : *buffdata));

		// next byte
		*buffdata++;
		pos++;

		if (start == 7)
		{
			start = 0;
			pos = 0;

			sprintf(szBuffTemp, "%s %s", buffer, ascii);
			PrnStr("%s\n", szBuffTemp);
			
			MEM_ZERO(buffer);
			MEM_ZERO(ascii);			
			MEM_ZERO(szBuffTemp);
		}
		else
			start++;
	}

	if (start <= 7)
	{
		MEM_ZERO(buffer2);
		MEM_ZERO(szBuffTemp);

		memcpy(buffer2, buffer, strlen(buffer));
		for (y = 0; y < (29 - strlen(buffer2)); y++)
		{
			slen = strlen(buffer);
			sprintf(&buffer[slen], "%s", " "); // fill the empty column with space
		}
		
		sprintf(szBuffTemp, "%s %s", buffer, ascii);

		PrnStr("%s\n", szBuffTemp);
		
	}
	PrnStart();
}

// end of file

void test_picc(void)
{
	uchar	ucRet;
	uchar SerialInfo[11];
	uchar CardType;
	uchar mode;

	
	PiccClose();
	ucRet = PiccOpen();
	if(ucRet!=0)
	{
		ScrCls();
		PubDispString("READER NOT FOUND", DISP_LINE_CENTER|2);
		PubBeepErr();
		PubWaitKey(3);
	}
      	mode = 'M';	//mifare / M1
    
    	TimerSet(TIMER_TEMPORARY, 100);       // 100 = 10 s //added by irf, 26-11-2012  (timeout at terminal is 10s)

	ScrCls();
	PubDispString("PLEASE PRESENT", DISP_LINE_CENTER|2);
	PubDispString("CARD", DISP_LINE_CENTER|4);

    	do
    	{
        	ucRet = PiccDetect(mode, &CardType, SerialInfo, NULL, NULL);//ISO mode

        	if (ucRet == 0)
        	{
			ScrCls();
			PubDispString("CARD DETECTED", DISP_LINE_CENTER|2);
			PubBeepOk();
			PubWaitKey(5);
            		break;
        	}

       	if(!TimerCheck(TIMER_TEMPORARY)) 
       	{           

			ScrCls();
			PubDispString("CARD NOT DETECT", DISP_LINE_CENTER|2);
			PubBeepErr();
			PubWaitKey(5);
            		break;
       	}
    }while (1);
}


//added by irf, 20121128
int GetPIN_Manual2(uchar ucFlag)
{
	int		iRet;
	uchar	szPAN[20+1], szTotalAmt[12+1];
	uchar	ucPinKeyID;
//	uchar	ucAmtFlag, szBuff[25];
#if defined(_P60_S1_) || defined(_P80_)
	uchar	ucLen;
#endif
	uchar TMK[8];
	uchar TWK[8];
	uchar TPK[8];

	

	memset(TPK, 0, sizeof(TPK));
	memset(TMK, 0, sizeof(TMK));
	memset(TWK, 0, sizeof(TWK));

	// 非EMV PIN的模式下，如果是chip则直接返回，不是chip则检查ISSUER
	// in non-EMV-PIN mode, if it is chip transaction then return directly
	if( !(ucFlag & GETPIN_EMV) )
	{
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) )
		{
			return 0;
		}
#ifdef	__TELIUM_PARAM__
		else if(glCurTlmTmsIss.issPinOpt != 0x32)
		{
			return 0;
		}
#else
		else if( !ChkIssuerOption(ISSUER_EN_PIN) )
		{
			return 0;
		}
#endif
	}

	ucPinKeyID = DEF_PIN_KEY_ID;	//!!!! : 预留扩展：ACQUIRER可定义自己使用的ID

	iRet = ExtractPAN(glProcInfo.stTranLog.szPan, szPAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);

	ucFlag &= (uchar)(0xFF-GETPIN_EMV);
	ScrClrLine(2, 7);

	if( ucFlag==0 )
	{
		PubDispString(_T("PLS ENTER PIN"), 4|DISP_LINE_CENTER);
	}
	else
	{
		PubDispString(_T("PIN ERR, RETRY"), 4|DISP_LINE_CENTER);
	}


	if (ChkTermPEDMode(PED_INT_PCI) || ChkTermPEDMode(PED_INT_PXX))
	{
		ScrGotoxy(32, 6);
	}

	memset(glProcInfo.sPinBlock, 0, sizeof(glProcInfo.sPinBlock));

	// !!!! to be check
#if defined(_S_SERIES_) || defined(_SP30_)
	//-------------- Internal PCI PED --------------
	if (ChkTermPEDMode(PED_INT_PCI))
	{
		//LoadTMK(TMK);
		//LoadTWK(TWK);

		memcpy(TMK, glCurTlmTmsAcq.acqMasterKey1, lenMKey);
		memcpy(TWK, glSysCtrl.WorkingKey, lenMKey);

//		hex_dump_char("TMK", TMK, sizeof(TMK));
//		hex_dump_char("TWK", TWK, sizeof(TWK));
		
		PubDes(ONE_DECRYPT, TWK, TMK, TPK);

		SxxWriteKey(ucPinKeyID, TPK, 8, ucPinKeyID, PED_TPK, NULL);

		iRet = PedGetPinBlock(ucPinKeyID, "0,4,5,6", szPAN, glProcInfo.sPinBlock, 0, USER_OPER_TIMEOUT*1000);
		if( iRet==0 )
		{
			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
			return 0;
		}
		else if (iRet==PED_RET_ERR_NO_PIN_INPUT)	// !!!! PIN bypass
		{
			return 0;
		}
		else if( iRet==PED_RET_ERR_INPUT_CANCEL )	// !!!! cancel input
		{
			return ERR_USERCANCEL;
		}

		DispPciErrMsg(iRet);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
#endif

	//-------------- External PCI PED --------------
	if (ChkTermPEDMode(PED_EXT_PCI))
	{
		// !!!! to be implemented
		ScrClrLine(2, 7);
		PubDispString("EXT PCI PINPAD ",  4|DISP_LINE_LEFT);
		PubDispString("NOT IMPLEMENTED.", 6|DISP_LINE_LEFT);
		PubWaitKey(30);
		return ERR_NO_DISP;
	}
	
	//-------------- PXX PED, and PINPAD --------------
#if defined(_P60_S1_) || defined(_P80_)
	if (ChkTermPEDMode(PED_INT_PXX))
	{
		iRet = PEDGetPwd_3Des(ucPinKeyID, 4, 6, szPAN, &ucLen, glProcInfo.sPinBlock);
	} 
#endif

	
	if( iRet==0 )
	{
		glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		return 0;
	}
	else if( iRet==0x0A )
	{
		return 0;	// PIN by pass
	}
	else if( iRet==0x06 || iRet==0x07 )
	{
		return ERR_USERCANCEL;
	}

	DispPPPedErrMsg((uchar)iRet);

	return ERR_NO_DISP;
}

void  _trcS(uchar *Text)
{
	PrnInit();
	PrnFontSet(0, 0);
	PrnSpaceSet(1, 2);

	PrnStr(Text);
	PrnStart();
}


void  _trcFN(uchar *Text, int key)
{
	uchar buffer[64];

	MEM_ZERO(buffer);
	
	PrnInit();
	PrnFontSet(0, 0);
	PrnSpaceSet(1, 2);

	sprintf(buffer, Text, key);
	PrnStr(buffer);
	PrnStart();
}


void  _trcFS(uchar *Text, uchar *data)
{
	uchar buffer[64];

	MEM_ZERO(buffer);
	
	PrnInit();
	PrnFontSet(0, 0);
	PrnSpaceSet(1, 2);

	sprintf(buffer, Text, data);
	PrnStr(buffer);
	PrnStart();
}

#ifdef	__TELIUM_PARAM__
void PrintMenu(void)
{
	word idx;
	uchar buffer[24];
	
	for(idx=mnuBeg+1; idx<mnuEnd;idx++)
	{		
		mapGet(idx, buffer, sizeof(buffer));
		hex_dump_char("menu", buffer, strlen(buffer));
	}
}

void PrintMenuNew(void)
{
	int ret;
	int jmlMenu=0,level=0;           							
	int iLoop;
	char itm[lenMnu + 1];       					
	char buf[48+1];
	char lvl[1+1];

	PrnInit();
	PrnStr("DAFTAR MENU");
	PrnStart();
	
	jmlMenu = mnuEnd - mnuMainMenu;

	for(iLoop = 1; iLoop < jmlMenu;iLoop++)
	{
		MEM_ZERO(itm);
		MEM_ZERO(lvl);
		MAPGET((word) (mnuMainMenu+iLoop), itm, lblKO);   //it is the menu name

		MEM_ZERO(buf);
		memset(buf,0x20,48);

		memcpy(lvl, itm, 1);
		if(memcmp(lvl,"?",1)==0){
			memcpy(buf, itm+2, strlen(itm)-2);
			memcpy(buf+41,"Disable",7);
		}		
		else
			level = atoi(lvl);
		
		switch(level){
		case 1 :
			strcpy(buf,"--");			
			break;
		case 2 :
			strcpy(buf,"----");
			break;
		case 3 :
			strcpy(buf,"------");
			break;
		case 4 :
			strcpy(buf,"--------");
			break;
		case 5 :
			strcpy(buf,"----------");
			break;		
		}

		memcpy(buf+(2*level),itm+2,strlen(itm)-2);
		
		if(level <= 10)
		{
			PrnInit();
			PrnStr(buf);
			PrnStart();
		}
	}
	

	
	lblKO:
		;

}


byte valGetByte(word key)
{
	int ret;
	byte varByte;
	
	MAPGETBYTE(key, varByte, lblKO);

	return varByte;
	lblKO:
		return -1;
}

word valGetWord(word key)
{
	int ret;
	word varWord;
	
	MAPGETWORD(key, varWord, lblKO);

	return varWord;
	lblKO:
		return -1;
}

#endif

#ifdef BANK_MANDIRI
int InputKodeArea(void)
{
	uchar	ucRet;

	ScrCls();
	DispTransName();
	PubDispString("KODE AREA:", DISP_LINE_LEFT|4);

	ucRet = PubGetString(NUM_IN, 0, 4, glProcInfo.stTranLog.szNomorId, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	fmtPad(glProcInfo.stTranLog.szNomorId, -4, 0x30);
	return 0;
}
#endif

#ifdef BANK_MANDIRI
int InputNomorTelepon(void)
{
	uchar	ucRet;
	uchar 	szNomorTelepon[20+1];
	uchar 	ucLen;

	MEM_ZERO(szNomorTelepon);
	
	ScrCls();
	DispTransName();
	PubDispString("NOMOR TELEPON:", DISP_LINE_LEFT|4);

	ucRet = PubGetString(NUM_IN, 0, 16, szNomorTelepon, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	ucLen = strlen(glProcInfo.stTranLog.szNomorId);
	memcpy(glProcInfo.stTranLog.szNomorId+ucLen, szNomorTelepon, strlen(szNomorTelepon));
	fmtPad(glProcInfo.stTranLog.szNomorId, 20, 0x20);

	return 0;
}
#endif


int parseTMWK(uchar *buff)
{
	int ret;
	int idx;
	char wKey[32+1];
	char bin[16+1];
	char tag[4+1];
	int iLoop = 0, cnt = 0;
	
	MEM_ZERO(wKey); MEM_ZERO(bin);
	MEM_ZERO(tag);
	
	memcpy(tag, "TMWK", 4);
	
	idx = 0;
	while(*buff)
	{
		for(iLoop = 0; iLoop <= 4; iLoop++)
		{
			if(buff[idx + iLoop] == tag[iLoop])
				cnt++;
		}

		if(cnt == 4)
			break;
					
		idx++;
	}

	memcpy(wKey, &buff[idx + 4], 32);
	hex2bin(bin, wKey, 32);
//	hex_dump_char("bin wKey", bin, 16);
	
	ret = mapPut(acqWorkingKey, bin, 16);
	if(ret<0) goto lblKO;
	
	return ret;
	lblKO:
		return -1;
}


int GetPinKeyPerAcq()
{
	int ret;
	char AcqName[1+lenName];
	char MKey1[8+1], MKey2[8+1], key[16+1];
	char WKey[16+1];
	char PinKey1[8+1], PinKey2[8+1], PinKey[16+1];

	MEM_ZERO(AcqName); 
	MEM_ZERO(WKey);
	MEM_ZERO(MKey1); 	 
	MEM_ZERO(MKey2); 	 
	MEM_ZERO(key);
	MEM_ZERO(PinKey1);   
	MEM_ZERO(PinKey2); 
	MEM_ZERO(PinKey);

	memcpy(MKey1, glCurTlmTmsAcq.acqMasterKey1, lenMKey);
	memcpy(MKey2, glCurTlmTmsAcq.acqMasterKey2, lenMKey);
	
	memcpy(key, MKey1, 8);
	memcpy(key + 8, MKey2, 8);	

	//MAPGET(acqWorkingKey, WKey, lblKO); // 4212 for tes glSysCtrl.WorkingKey
	memcpy(WKey,glCurTlmTmsAcq.acqWorkingKey,16);
	//memcpy(WKey,glSysCtrl.WorkingKey,16);
	
	
	stdDED(PinKey1, WKey, key);
	stdDED(PinKey2, WKey + 8, key);

	memcpy(PinKey, PinKey1, 8);
	memcpy(PinKey + 8, PinKey2, 8);

	ret = mapPut(acqPinKey, PinKey, 16);	
	if(ret<0)	goto lblKO;//end
	memcpy(glCurTlmTmsAcq.acqPinKey,PinKey,16);
	//*
		#ifdef ENABLE_EMV	// 4212	
				if(debugEMVCalc)
				{	
				hex_dump_char("acqName____GetPin", glCurTlmTmsAcq.acqName,lenName);
				hex_dump_char("WorkingKey_GetPin", WKey, 16);
				hex_dump_char("MasterKey__GetPin", key, 16);
				hex_dump_char("PinKey_____GetPin", PinKey, 16);
				}
		#endif
	//*/		
	return ret;
	lblKO:
		return -1;
}

int fmtAppNameAndVersion()
{
	char SoftName[1+6];
	char buff[1+4], version[1+7];
	int ret;
	char name[13+1];
	char ctrlVersion[1+1];


	MEM_ZERO(SoftName);
	MEM_ZERO(buff);
	MEM_ZERO(version);
	MEM_ZERO(name);
	MEM_ZERO(ctrlVersion);
	
	
	MAPGET(appSoftName, SoftName, lblKO);
	MAPGET(appVersion, buff, lblKO);
	MAPGET(appCtrlVersion, ctrlVersion, lblKO);
	//version[0] = 'v';
#ifdef BANK_MANDIRI
	memcpy(version, buff, 2);
	version[2] = '.';
	memcpy(version + 3, buff + 2, 2);
	version[5] = '.';
	memcpy(version + 6, ctrlVersion, 1);
	//	MEM_0x20(name);
	memset(name, 0x20, sizeof(name)-1);
	memcpy(name, SoftName, 5);
	memcpy(name + 6, version, 7);
	MAPPUTSTR(traAppName, name, lblKO);
#elif defined(BANK_BRI)
	memcpy(version, buff, 1);
	version[1] = '.';
	memcpy(version + 2, buff + 1, 1);
	version[3] = '.';
	memcpy(version + 4, buff + 2, 1);
	version[5] = '.';
	memcpy(version + 6, buff + 3, 1);
	
	memset(name, 0x20, sizeof(name)-1);
	memcpy(name, SoftName, 5);
	name[5] = '.';
	memcpy(name + 6, version, 7);
	MAPPUTSTR(traAppName, name, lblKO);
#endif



	return ret;
	lblKO:
		return -1;;
}

uchar CalCAPKCheckSum(EMV_CAPK *capk, uchar *pszCheckSum)
{
	uchar checkData[400];
	uint  index;
	
	index = 0;
	memset(checkData, 0, strlen(checkData));

	memcpy(checkData, capk->RID, 5);
	checkData[5] = capk->KeyID;
	index = 6;
	
	memcpy((uchar*)&checkData[index], capk->Modul, capk->ModulLen);
	index += capk->ModulLen;
	memcpy((uchar*)&checkData[index], capk->Exponent, capk->ExponentLen);
	index += capk->ExponentLen;

	Hash(checkData, index, pszCheckSum);

	return 0;
}

int CheckResponseCode(uchar *ResponseCode)
{
	if( memcmp(ResponseCode, "00", 2)!=0 )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", ResponseCode);
		DispResult(ERR_HOST_REJ);
		return ERR_NO_DISP;
	}
	return 0;
}

int MatchBRIDebitCardTable(uchar *pszPAN)
{
	uchar		ucCnt, ucPanLen, ucAcqNum,ucIndex;
	uchar		sPanHeader[5], sCardIndex[dimAcq], sAcqMatchFlag[dimAcq];
	TLM_BIN		*tlmpstCardTbl;
	

	memset(sCardIndex,    0, sizeof(sCardIndex));
	memset(sAcqMatchFlag, 0, sizeof(sAcqMatchFlag));

	// create a list of matched acquirer.
	ucPanLen = strlen((char *)pszPAN);
	PubAsc2Bcd(pszPAN, 10, sPanHeader);
	
	for(ucAcqNum=ucCnt=0; ucCnt<glTlmSysParam.ucBinNum; ucCnt++)
	{
		tlmpstCardTbl = &glTlmSysParam.Bin[ucCnt];
		if( (memcmp(tlmpstCardTbl->binLo+1,  sPanHeader, 5)<=0  &&
			 memcmp(tlmpstCardTbl->binHi+1, sPanHeader, 5)>=0))
		{
			//FindAcq(tlmpstCardTbl->binCrAcqID);
			for(ucIndex=0; ucIndex<glTlmSysParam.ucAcqNum; ucIndex++)
			{
				if( glTlmSysParam.Acq[ucIndex].acqID==tlmpstCardTbl->binCrAcqID )
				{
					if (strstr(glTlmSysParam.Acq[ucIndex].acqName, "DEBIT")!=NULL)
					{
						glProcInfo.stTranLog.BRI.ucflagDebitBri=TRUE;
						return TRUE;
					}
					break;
				}
			}
		}
	}

	glProcInfo.stTranLog.BRI.ucflagDebitBri=FALSE;
	return FALSE;
}

int MatchBRICreditCardTable(uchar *pszPAN)
{
	uchar		ucCnt, ucPanLen, ucAcqNum,ucIndex;
	uchar		sPanHeader[5], sCardIndex[dimAcq], sAcqMatchFlag[dimAcq];
	TLM_BIN		*tlmpstCardTbl;
	

	memset(sCardIndex,    0, sizeof(sCardIndex));
	memset(sAcqMatchFlag, 0, sizeof(sAcqMatchFlag));

	// create a list of matched acquirer.
	ucPanLen = strlen((char *)pszPAN);
	PubAsc2Bcd(pszPAN, 10, sPanHeader);
	
	for(ucAcqNum=ucCnt=0; ucCnt<glTlmSysParam.ucBinNum; ucCnt++)
	{
		tlmpstCardTbl = &glTlmSysParam.Bin[ucCnt];
		if( (memcmp(tlmpstCardTbl->binLo+1,  sPanHeader, 5)<=0  &&
			 memcmp(tlmpstCardTbl->binHi+1, sPanHeader, 5)>=0))
		{
			//FindAcq(tlmpstCardTbl->binCrAcqID);
			for(ucIndex=0; ucIndex<glTlmSysParam.ucAcqNum; ucIndex++)
			{
				if( glTlmSysParam.Acq[ucIndex].acqID==tlmpstCardTbl->binCrAcqID )
				{
					if (strstr(glTlmSysParam.Acq[ucIndex].acqName, "CREDIT")!=NULL)
					{
						if (strstr(tlmpstCardTbl->binName, "OTHER")==NULL)
						{
							glProcInfo.stTranLog.BRI.ucflagDebitBri=TRUE;
							return TRUE;
						}
					}
					break;
				}
			}
		}
	}

	glProcInfo.stTranLog.BRI.ucflagDebitBri=FALSE;
	return FALSE;
}


// DIKI ADD FOR MINI ATM BUKOPIN

int GetPIN_Manual_MiniAtmBkpn(uchar ucFlag)
{
	int		iRet;
	uchar	szPAN[20+1], szTotalAmt[12+1];
	uchar	ucPinKeyID;
//	uchar	ucAmtFlag, szBuff[25];
#if defined(_P60_S1_) || defined(_P80_)
	uchar	ucLen;
#endif
	uchar pin[6+1];
	uchar data[2+1];
	uchar blok1[16+1];
	uchar blok2[16+1];
	uchar blok1Byte[8+1];
	uchar blok2Byte[8+1];
	uchar dataXor[8+1];
	uchar TMK[8];
	uchar TWK[8];
	uchar TPK[8];
	int len;
	

	memset(TPK, 0, sizeof(TPK));
	memset(TMK, 0, sizeof(TMK));
	memset(TWK, 0, sizeof(TWK));

	// 非EMV PIN的模式下，如果是chip则直接返回，不是chip则检查ISSUER
	/*// //4212
	if( !(ucFlag & GETPIN_EMV) )
	{
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) )
		{
			return 0;
		}
#ifdef	__TELIUM_PARAM__
		else if(glCurTlmTmsIss.issPinOpt != 0x32)
		{
			return 0;
		}
#else
		else if( !ChkIssuerOption(ISSUER_EN_PIN) )
		{
			return 0;
		}
#endif
	}
	/*/
	// in non-EMV-PIN mode, if it is chip transaction then return directly
	if( !(ucFlag & GETPIN_EMV) && !(ucFlag & GETPIN_RETRY)) 		//4212
	{	
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT))
		{
			return 0;
		}
#ifdef	__TELIUM_PARAM__ // 0x31 >> optional pin required , 0x32 >> pin required 
		else {
			if(glCurTlmTmsIss.issPinOpt == 0x31 ){ // pin optionaly 
			/*
 			switch(PubSelectYesNo("INPUT PIN ?","YES","NO")){
				case KEYENTER:
					break;
				case KEYCANCEL:
					return 0;
					break;
				}
			*/
			}
			else {
				if(glCurTlmTmsIss.issPinOpt == 0x30 )
					return 0;
			}
		}
#else	
		else {
			if( !ChkIssuerOption(ISSUER_EN_PIN) )
			return 0;
		}
#endif
	}
	//*/
	
	ucPinKeyID = DEF_PIN_KEY_ID;	//!!!! : 预留扩展：ACQUIRER可定义自己使用的ID

	iRet = ExtractPAN(glProcInfo.stTranLog.szPan, szPAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);

	ucFlag &= (uchar)(0xFF-GETPIN_EMV);
	ScrClrLine(2, 7);
	
	if( ucFlag==0 )
	{
		PubDispString(_T("INPUT PIN :"), 4|DISP_LINE_LEFT);
	}
	else
	{
		PubDispString(_T("PIN ERR, RETRY"), 4|DISP_LINE_CENTER);
	}
	
	if (ChkTermPEDMode(PED_INT_PCI) || ChkTermPEDMode(PED_INT_PXX))
	{
		ScrGotoxy(32, 6);
	}

	memset(glProcInfo.sPinBlock, 0, sizeof(glProcInfo.sPinBlock));

	// !!!! to be check
#if defined (_S_SERIES_) || defined (_SP30_)	

	//-------------- Internal PCI PED --------------
	if (ChkTermPEDMode(PED_INT_PCI))
	{
		memset(pin, 0, sizeof(pin));

		if(CheckExpBypassPin())
		{
			iRet= PubGetString(PASS_IN, 4, 6, pin, USER_OPER_TIMEOUT);	// irf
		}
		else
		{
			iRet= PubGetString_IncludeByPass(PASS_IN, 4, 6, pin, USER_OPER_TIMEOUT);	// 4212
		}
		if( iRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		if((strlen(pin)>=4)&&(strlen(pin)<=6))
		{
			memset(blok1, 0, sizeof(blok1));
			memset(blok2, 0, sizeof(blok2));
			memset(blok1Byte, 0, sizeof(blok1Byte));
			memset(blok2Byte, 0, sizeof(blok2Byte));
			memset(dataXor, 0, sizeof(dataXor));
			memset(data, 0, sizeof(data));

	
			//LoadTMK(TMK);
			//LoadTWK(TWK);

			memcpy(TMK, glCurTlmTmsAcq.acqMasterKey1, lenMKey);
			memcpy(TWK, glSysCtrl.WorkingKey, lenMKey);

//			hex_dump_char("TMK", TMK, sizeof(TMK));
//			hex_dump_char("TWK", TWK, sizeof(TWK));


			PubDes(ONE_DECRYPT, TWK, TMK, TPK);
//			hex_dump_char("TPK", TPK, sizeof(TPK));


			len=strlen(pin);

			sprintf(data, "%02d", len); 
			
			memcpy(blok1, data, 2);

//			hex_dump_char("PIN", pin, len);
			memcpy(blok1 + 2, pin, len);
//			hex_dump_char("blok1", blok1, sizeof(blok1));
			
			fmtPad(blok1, 16, 0x46);
//			hex_dump_char("blok1", blok1, sizeof(blok1));

			memcpy(blok2, szPAN, 16);
			PubAsc2Bcd(blok1, 8*2, blok1Byte);
			PubAsc2Bcd(blok2, 8*2, blok2Byte);

//			hex_dump_char("blok1Byte", blok1Byte, sizeof(blok1Byte));
//			hex_dump_char("blok2Byte", blok2Byte, sizeof(blok2Byte));

			
			PubXor(blok1Byte, blok2Byte, 8, dataXor);

////			hex_dump_char("dataXor", dataXor, sizeof(dataXor));

			memcpy(glProcInfo.sPlainPinBlock, dataXor, 8); 

			/////PubDes(ONE_ENCRYPT, dataXor, TPK, glProcInfo.sPinBlock);
			PubDes(ONE_ENCRYPT, dataXor, TMK, glProcInfo.sPinBlock);
////			hex_dump_char("sPinBlock", glProcInfo.sPinBlock, sizeof(glProcInfo.sPinBlock));

			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
			
			GetPinKeyPerAcq();

			return 0;
		}
		else if(strlen(pin)==0)
		{
			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_BYPASS;
		}
		return ERR_NO_DISP;
	}
#endif

	//-------------- External PCI PED --------------
	if (ChkTermPEDMode(PED_EXT_PCI))
	{
		// !!!! to be implemented
		ScrClrLine(2, 7);
		PubDispString("EXT PCI PINPAD ",  4|DISP_LINE_LEFT);
		PubDispString("NOT IMPLEMENTED.", 6|DISP_LINE_LEFT);
		PubWaitKey(30);
		return ERR_NO_DISP;
	}
	
	//-------------- PXX PED, and PINPAD --------------
#if defined(_P60_S1_) || defined(_P80_)
	if (ChkTermPEDMode(PED_INT_PXX))
	{
		iRet = PEDGetPwd_3Des(ucPinKeyID, 4, 6, szPAN, &ucLen, glProcInfo.sPinBlock);
	} 
#endif

	
	if( iRet==0 )
	{
		glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		return 0;
	}
	else if( iRet==0x0A )
	{
		return 0;	// PIN by pass
	}
	else if( iRet==0x06 || iRet==0x07 )
	{
		return ERR_USERCANCEL;
	}

	DispPPPedErrMsg((uchar)iRet);

	return ERR_NO_DISP;
}




//         DIKI ADD FOR PRIVATE LABEL BUKOPIN
int GetCardPrvtLbl(uchar ucMode)
{
	int		iRet, iEventID;
	uchar	bCheckICC, ucKey;

	if( (glProcInfo.stTranLog.uiEntryMode & 0x0F)!=MODE_NO_INPUT )
	{
		return 0;
	}

	if( ucMode & FALLBACK_SWIPE )
	{
		ucMode &= ~(SKIP_CHECK_ICC|CARD_INSERTED);	// clear bit 8, force to check service code
	}

	bCheckICC = !(ucMode & SKIP_CHECK_ICC);

	TimerSet(0, TIME_OUT_INPUT*10);
	while( 1 )
	{
		iEventID = DetectCardEvent(ucMode);
		if( iEventID==CARD_KEYIN )
		{
			ucKey = getkey();
			if( ucKey==KEYCANCEL )
			{
				return ERR_USERCANCEL;
			}
			if( (ucMode & CARD_KEYIN) && ucKey>='0' && ucKey<='9' )
			{
				return ManualInputPan(ucKey);
			}
		}
		else if( iEventID==CARD_SWIPED )
		{
			iRet = SwipeCardProc(bCheckICC);
			if( iRet==0 )
			{
				return ValidCard();
			}
			else if( iRet==ERR_SWIPECARD )
			{
				DispMagReadErr();
				PubBeepErr();
				PubWaitKey(3);
			}
			else if( iRet==ERR_NEED_INSERT )	// 是芯片卡
			{
				if( !(ucMode & CARD_INSERTED) )
				{	// 本身交易不允许插卡
					return iRet;
				}
				ucMode &= ~CARD_SWIPED;			// 去掉刷卡检查
			}
			else
			{
				return iRet;
			}
		}
#ifdef ENABLE_EMV
		else if( iEventID==CARD_INSERTED )
		{
			iRet = InsertCardProc();
			/////iRet = InsertCardProcPrvtLbl();
			if( iRet==0 )
			{
				return 0;
			}
			else if( iRet==ERR_NEED_FALLBACK )
			{
				DispFallBackPrompt();
				PromptRemoveICC();
				ucMode = CARD_SWIPED|FALLBACK_SWIPE;	// Now we don't support fallback to manual-PAN-entry
			}
			else if( iRet==ERR_TRAN_FAIL )
			{
				PubDispString(_T("NOT ACCEPTED"), 4|DISP_LINE_LEFT);
				PubBeepErr();
				PubWaitKey(3);
				PromptRemoveICC();
				//emv_test();
				return ERR_NO_DISP;
			}
			else
			{
				return iRet;
			}
		}
		else if( iEventID==ERR_NO_DISP)
			return ERR_NO_DISP;
#endif
	}
}




// ICC插卡事件处理函数
// Process insertion event.
int InsertCardProcPrvtLbl(void)
{
	int		iRet;
	uchar	szTotalAmt[12+1], sTemp[6];

#ifdef __EMV_DEBUG__
	countprocEMV = 0;
#endif

	// 如果已经FALLBACK,忽略IC卡插卡操作
	ScrClrLine(3, 3);
	PubDispString(_T("atas debug...."), 4|DISP_LINE_LEFT);
	PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---atas debug--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
	
	if( glProcInfo.bIsFallBack==TRUE )
	{
		return ERR_NEED_FALLBACK;
	}

	ScrClrLine(6, 6);
	PubDispString(_T(" bawah debug...."), 4|DISP_LINE_LEFT);
	PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();

	//!!!! deleted: it is fixed and not allowed to modify after GPO.
	//ModifyTermCapForPIN();

	glProcInfo.stTranLog.uiEntryMode = MODE_CHIP_INPUT;

	DispTransName();
	DispProcess();

	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug  111--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();

/*
#ifdef ENABLE_EMV
	InitTransEMVCfg();
#endif

	// 应用选择
	// EMV application selection. This is EMV kernel API
	iRet = EMVAppSelect(ICC_USER, glSysCtrl.ulSTAN);
	DispTransName();
	if( iRet==EMV_DATA_ERR || iRet==ICC_RESET_ERR || iRet==EMV_NO_APP ||
		iRet==ICC_CMD_ERR  || iRet==EMV_RSP_ERR )
	{
		glProcInfo.bIsFallBack = TRUE;
		glProcInfo.iFallbackErrCode = iRet;
		return ERR_NEED_FALLBACK;
	}
	if( iRet==EMV_TIME_OUT || iRet==EMV_USER_CANCEL )
	{
		return ERR_USERCANCEL;
	}
	if( iRet!=EMV_OK )
	{
		return ERR_TRAN_FAIL;
	}

	// Clear log to avoid amount accumulation for floor limit checking
    iRet = EMVClearTransLog();
*/

	// Read Track 2 and/or Pan
	DispTransName();
	iRet = GetEmvTrackData();
	if( iRet!=0 )
	{

		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 2222--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		return iRet;
	}


	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 333--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
    
    // Display app prefer name
    if (strlen(glProcInfo.stTranLog.szAppPreferName)!=0)
    {
        PubDispString(glProcInfo.stTranLog.szAppPreferName, 4|DISP_LINE_LEFT);
        PubWaitKey(1);
    }
    else if (strlen(glProcInfo.stTranLog.szAppLabel)!=0)
    {
        PubDispString(glProcInfo.stTranLog.szAppLabel, 4|DISP_LINE_LEFT);
        PubWaitKey(1);
    }


	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 444--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();

		
	iRet = ValidCard();	// 校验卡号
	if( iRet!=0 )
	{
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 555--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();
		
		return iRet;
	}

	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 666--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();

		
	UpdateEMVTranType();

	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug 777--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();


/*
#ifdef ENABLE_EMV
	// EMVSetMCKParam to set bypass PIN
#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issPinOpt==0x30)
		AppSetMckParam(1);	//pin by pass	/ not required
	else
		AppSetMckParam(0);
#else
	AppSetMckParam(ChkIssuerOption(ISSUER_EN_EMVPIN_BYPASS));
#endif
	//end
#endif
*/



	// 输入交易金额
	/*
	if(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID)
	{
		iRet = GetAmount();
		if( iRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
		//PubAddHeadChars(szTotalAmt, 12, '0');  no need: already 12 digits
		PubAsc2Bcd(szTotalAmt, 12, sTemp);
		EMVSetTLVData(0x9F02, sTemp, 6);
		PubLong2Char((ulong)atol((char *)szTotalAmt), 4, sTemp);
		EMVSetTLVData(0x81, sTemp, 4);
	}
	*/

	DispTransName();
	DispProcess();

	/*
	// 卡片数据认证
	// Card data authentication
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_AUTH ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PREAUTH ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CICILAN_BRING ||
 	       glProcInfo.stTranLog.ucTranType!= TRANS_TYPE_SALE_REDEEMPTION||
 	       glProcInfo.stTranLog.ucTranType!= TRANS_TYPE_CICILAN_BUKOPIN)
	{
		iRet = EMVCardAuth();
		if( iRet!=EMV_OK )
		{
			return ERR_TRAN_FAIL;
		}
	}
	*/
	PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bawah debug end--");
		PrnStr("\n");
		PrnStr("\n");
		StartPrinter();

	return 0;
}



// Check for BUKOPIN private label , diki add
int ValidCardPrvtLbl(void)
{
	int		iRet;
	uchar 	szPan[lenPan+1];

	MEM_ZERO(szPan);

#ifdef BANK_BRI
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_PRIVILAGE:
		case TRANS_TYPE_SMSBANKING:
		case TRANS_TYPE_PHONEBANKING:
		case TRANS_TYPE_IBANKING:
		case TRANS_TYPE_TRX_PB_TRANSFER_INQ:
		case TRANS_TYPE_TRX_PB_TRANSFER:
		case TRANS_TYPE_TRX_PB_NO_HP:
		case TRANS_TYPE_INFOSALDO:
		case TRANS_TYPE_INFOSALDO_BANKLAIN:
		case TRANS_TYPE_MINISTATEMENT:
		case TRANS_TYPE_MUTASIREKENING:
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
		//case TRANS_TYPE_FREN_MOBI_HEPI://ditutup 6 Januari 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:

		case TRANS_TYPE_INQ_TRFSESAMABRI:
		case TRANS_TYPE_INQ_TRFANTARBANK:

		case TRANS_TYPE_INQ_TELKOM:
		case TRANS_TYPE_INQ_KARTUHALO:
		case TRANS_TYPE_INQ_KARTUMATRIX:

		case TRANS_TYPE_INQ_PLNPASCABAYAR:
		case TRANS_TYPE_INQ_PLNPRABAYAR:

		case TRANS_TYPE_INQ_KKBRI:
		case TRANS_TYPE_KKCITIBANK:
		case TRANS_TYPE_KTACITIBANK:
		case TRANS_TYPE_KKSCB:
		case TRANS_TYPE_KTASCB:
		case TRANS_TYPE_KKANZ:
		case TRANS_TYPE_KKHSBC:
		case TRANS_TYPE_KTAHSBC:
		case TRANS_TYPE_KKRBS:
		case TRANS_TYPE_KTARBS:

		case TRANS_TYPE_INQ_FIF:
		case TRANS_TYPE_INQ_BAF:
		case TRANS_TYPE_INQ_OTO:
		case TRANS_TYPE_INQ_FINANSIA:
		case TRANS_TYPE_INQ_VERENA:
		case TRANS_TYPE_INQ_WOM:

		case TRANS_TYPE_ZAKAT:
		case TRANS_TYPE_INFAQ:
		case TRANS_TYPE_DPLK:

		case TRANS_TYPE_INQ_GARUDA:
		case TRANS_TYPE_INQ_LIONAIR:
		case TRANS_TYPE_INQ_SRIWIJAYA:

		case TRANS_TYPE_INQ_BRIVA:

		case TRANS_TYPE_INQ_BAYARKULIAH:

		case TRANS_TYPE_AKTIFASIKARTU_PETUGAS:
		case TRANS_TYPE_AKTIFASIKARTU_NASABAH:

		case TRANS_TYPE_REISSUE_PIN_NASABAH:
		case TRANS_TYPE_REISSUE_PIN_PETUGAS:	

		case TRANS_TYPE_UBAHPIN_CS:
		case TRANS_TYPE_UBAHPWD_SPV:
			
		case TRANS_TYPE_ABSENSI:
		case TRANS_TYPE_BRIZZI_TOPUP_ONLINE:

		case TRANS_TYPE_RENEWALKARTU_PETUGAS:
		case TRANS_TYPE_RENEWALKARTU_LAMA:
		case TRANS_TYPE_RENEWALKARTU_BARU:
		case TRANS_TYPE_SETORPASTI:

	
	//	case TRANS_TYPE_INQ_CASHOUT:

		//SSB
		case TRANS_TYPE_SIM_BARU:
		case TRANS_TYPE_SIM_PERPANJANGAN:
		case TRANS_TYPE_SIM_PENINGKATAN:
		case TRANS_TYPE_SIM_PENURUNAN:
		case TRANS_TYPE_SIM_MUTASI_GOL_SAMA:
		case TRANS_TYPE_SIM_MUTASI_PENINGKATAN:
		case TRANS_TYPE_SIM_MUTASI_PENURUNAN:
		case TRANS_TYPE_SIM_HILANG_RUSAK_GOLSAMA:
		case TRANS_TYPE_SIM_HILANG_RUSAK_PENINGKATAN:
		case TRANS_TYPE_SIM_HILANG_RUSAK_PENURUNAN:
		case TRANS_TYPE_UJI_SIMULATOR:
		case TRANS_TYPE_BPKB_BARU:
		case TRANS_TYPE_BPKB_GANTI_PEMILIK:
		case TRANS_TYPE_STCK:
		case TRANS_TYPE_MUTASI_RANMOR:
		case TRANS_TYPE_SSB:

			strcpy(szPan, "90");
			memcpy(szPan+2, glProcInfo.stTranLog.szPan, strlen(glProcInfo.stTranLog.szPan)-2);
			break;
		case TRANS_TYPE_SETOR_INQ_PINJ:
		case TRANS_TYPE_SETOR_INQ_SIMP:
		case TRANS_TYPE_SETOR_PINJAMAN:
		case TRANS_TYPE_SETOR_SIMPANAN:
		case TRANS_TYPE_TARIK_TUNAI:
			//T-BANK
		case TRANS_TYPE_CASH_IN:
			strcpy(szPan, "91");
			memcpy(szPan+2, glProcInfo.stTranLog.szPan, strlen(glProcInfo.stTranLog.szPan)-2);
			break;
		default:
			strcpy(szPan, glProcInfo.stTranLog.szPan);
			break;
	}
#else
		strcpy(szPan, glProcInfo.stTranLog.szPan);
#endif
//	iRet = MatchCardTable(glProcInfo.stTranLog.szPan);

//	hex_dump_char("pan", szPan, sizeof(szPan));

	iRet = MatchCardTable(szPan);
	if( iRet!=0 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("UNSUPPORT CARD"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}


	/*
	iRet = ValidPanNo(glProcInfo.stTranLog.szPan);	//check luhn digit
	if( iRet!=0 )
	{
		return iRet;
	}
	*/


#ifdef MINI_ATM_BUKOPIN
	iRet = 0;	// no check luhn digit for mini atm bukopin, ALL PAN VALID. DIKI ADD
	if( iRet!=0 )
	{
		return iRet;
	}
#else
	iRet = ValidPanNo(glProcInfo.stTranLog.szPan);	//check luhn digit
	if( iRet!=0 )
	{
		return iRet;
	}
#endif


	iRet = ValidCardExpiry();
	if( iRet!=0 )
	{
		return iRet;
	}

	CheckCapture();

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INSTALMENT )
	{
		if( glProcInfo.bIsFallBack || glEdcMsgPtr->MsgType==ICCARD_MSG )
		{
			PubDispString(_T("PLS SWIPE AGAIN"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(5);
			return ERR_NEED_SWIPE;
		}
	}

	////GetCardHolderName(glProcInfo.stTranLog.szHolderName);
#ifdef	__TELIUM_PARAM__
	if(glTlmSysParam.Con.appTermOpt1 & EDC_TLM_TMS_DISPLAY_PAN)
	{
		iRet = ConfirmPanInfo();
		if( iRet!=0 )
		{
			CommOnHook(FALSE);	//release predial
			return iRet;
		}
	}
#else
	iRet = ConfirmPanInfo();
	if( iRet!=0 )
	{
		CommOnHook(FALSE);	//release predial
		return iRet;
	}
#endif

#ifndef	__TELIUM_PARAM__	//not use
	iRet = GetSecurityCode();
	if( iRet!=0 )
	{
		return iRet;
	}
#endif
	return 0;
}




//added for bukopin private label, chip emv not use for pin!
int GetPIN_ManualPrvtLbl(uchar ucFlag)
{
	int		iRet;
	uchar	szPAN[20+1], szTotalAmt[12+1];
	uchar	ucPinKeyID;
//	uchar	ucAmtFlag, szBuff[25];
#if defined(_P60_S1_) || defined(_P80_)
	uchar	ucLen;
#endif
	uchar pin[6+1];
	uchar data[2+1];
	uchar blok1[16+1];
	uchar blok2[16+1];
	uchar blok1Byte[8+1];
	uchar blok2Byte[8+1];
	uchar dataXor[8+1];
	uchar TMK[8];
	uchar TWK[8];
	uchar TPK[8];
	int len;
	

	memset(TPK, 0, sizeof(TPK));
	memset(TMK, 0, sizeof(TMK));
	memset(TWK, 0, sizeof(TWK));

	// 非EMV PIN的模式下，如果是chip则直接返回，不是chip则检查ISSUER
	/*// //4212
	if( !(ucFlag & GETPIN_EMV) )
	{
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) )
		{
			return 0;
		}
#ifdef	__TELIUM_PARAM__
		else if(glCurTlmTmsIss.issPinOpt != 0x32)
		{
			return 0;
		}
#else
		else if( !ChkIssuerOption(ISSUER_EN_PIN) )
		{
			return 0;
		}
#endif
	}
	/*/
	// in non-EMV-PIN mode, if it is chip transaction then return directly
	if( !(ucFlag & GETPIN_EMV) && !(ucFlag & GETPIN_RETRY)) 		//4212
	{	
		if ( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT))
		{		
			////return 0;  
			if(glCurTlmTmsIss.issPinOpt == 0x31 ){ // pin optionaly 				
 			switch(PubSelectYesNo("INPUT PIN ?","YES","NO")){
				case KEYENTER:
					break;
				case KEYCANCEL:
					return 0;
					break;
				}
			}
			else {
				if(glCurTlmTmsIss.issPinOpt == 0x30 )
					return 0;
			}
			
		}
#ifdef	__TELIUM_PARAM__ // 0x31 >> optional pin required , 0x32 >> pin required 
		else {
			if(glCurTlmTmsIss.issPinOpt == 0x31 ){ // pin optionaly 				
 			switch(PubSelectYesNo("INPUT PIN ?","YES","NO")){
				case KEYENTER:
					break;
				case KEYCANCEL:
					return 0;
					break;
				}
			}
			else {
				if(glCurTlmTmsIss.issPinOpt == 0x30 )
					return 0;
			}
		}
#else	
		else {
			if( !ChkIssuerOption(ISSUER_EN_PIN) )
			return 0;
		}
#endif
	}

			
	//*/
	
	ucPinKeyID = DEF_PIN_KEY_ID;	//!!!! : 预留扩展：ACQUIRER可定义自己使用的ID

	iRet = ExtractPAN(glProcInfo.stTranLog.szPan, szPAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);

	ucFlag &= (uchar)(0xFF-GETPIN_EMV);
	ScrClrLine(2, 7);
	
	if( ucFlag==0 )
	{
		PubDispString(_T("INPUT PIN ?"), 4|DISP_LINE_LEFT);
	}
	else
	{
		PubDispString(_T("PIN ERR, RETRY"), 4|DISP_LINE_CENTER);
	}
	
	if (ChkTermPEDMode(PED_INT_PCI) || ChkTermPEDMode(PED_INT_PXX))
	{
		ScrGotoxy(32, 6);
	}

	memset(glProcInfo.sPinBlock, 0, sizeof(glProcInfo.sPinBlock));

	// !!!! to be check
#if defined (_S_SERIES_) || defined (_SP30_)	

	//-------------- Internal PCI PED --------------
	if (ChkTermPEDMode(PED_INT_PCI))
	{
		memset(pin, 0, sizeof(pin));

		if(CheckExpBypassPin())
		{
			iRet= PubGetString(PASS_IN, 4, 6, pin, USER_OPER_TIMEOUT);	// irf
		}
		else
		{
			iRet= PubGetString_IncludeByPass(PASS_IN, 4, 6, pin, USER_OPER_TIMEOUT);	// 4212
		}
		if( iRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		if((strlen(pin)>=4)&&(strlen(pin)<=6))
		{
			memset(blok1, 0, sizeof(blok1));
			memset(blok2, 0, sizeof(blok2));
			memset(blok1Byte, 0, sizeof(blok1Byte));
			memset(blok2Byte, 0, sizeof(blok2Byte));
			memset(dataXor, 0, sizeof(dataXor));
			memset(data, 0, sizeof(data));

	
			//LoadTMK(TMK);
			//LoadTWK(TWK);

			memcpy(TMK, glCurTlmTmsAcq.acqMasterKey1, lenMKey);
			memcpy(TWK, glSysCtrl.WorkingKey, lenMKey);

//			hex_dump_char("TMK", TMK, sizeof(TMK));
//			hex_dump_char("TWK", TWK, sizeof(TWK));


			PubDes(ONE_DECRYPT, TWK, TMK, TPK);
//			hex_dump_char("TPK", TPK, sizeof(TPK));


			len=strlen(pin);

			sprintf(data, "%02d", len); 
			
			memcpy(blok1, data, 2);

//			hex_dump_char("PIN", pin, len);
			memcpy(blok1 + 2, pin, len);
//			hex_dump_char("blok1", blok1, sizeof(blok1));
			
			fmtPad(blok1, 16, 0x46);
//			hex_dump_char("blok1", blok1, sizeof(blok1));

			memcpy(blok2, szPAN, 16);
			PubAsc2Bcd(blok1, 8*2, blok1Byte);
			PubAsc2Bcd(blok2, 8*2, blok2Byte);

//			hex_dump_char("blok1Byte", blok1Byte, sizeof(blok1Byte));
//			hex_dump_char("blok2Byte", blok2Byte, sizeof(blok2Byte));

			
			PubXor(blok1Byte, blok2Byte, 8, dataXor);

//			hex_dump_char("dataXor", dataXor, sizeof(dataXor));

			memcpy(glProcInfo.sPlainPinBlock, dataXor, 8); 

			PubDes(ONE_ENCRYPT, dataXor, TPK, glProcInfo.sPinBlock);
//			hex_dump_char("sPinBlock", glProcInfo.sPinBlock, sizeof(glProcInfo.sPinBlock));

			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
			
			GetPinKeyPerAcq();

			return 0;
		}
		else if(strlen(pin)==0)
		{
			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_BYPASS;
		}
		return ERR_NO_DISP;
	}
#endif

	//-------------- External PCI PED --------------
	if (ChkTermPEDMode(PED_EXT_PCI))
	{
		// !!!! to be implemented
		ScrClrLine(2, 7);
		PubDispString("EXT PCI PINPAD ",  4|DISP_LINE_LEFT);
		PubDispString("NOT IMPLEMENTED.", 6|DISP_LINE_LEFT);
		PubWaitKey(30);
		return ERR_NO_DISP;
	}
	
	//-------------- PXX PED, and PINPAD --------------
#if defined(_P60_S1_) || defined(_P80_)
	if (ChkTermPEDMode(PED_INT_PXX))
	{
		iRet = PEDGetPwd_3Des(ucPinKeyID, 4, 6, szPAN, &ucLen, glProcInfo.sPinBlock);
	} 
#endif

	
	if( iRet==0 )
	{
		glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT;
		return 0;
	}
	else if( iRet==0x0A )
	{
		return 0;	// PIN by pass
	}
	else if( iRet==0x06 || iRet==0x07 )
	{
		return ERR_USERCANCEL;
	}

	DispPPPedErrMsg((uchar)iRet);

	return ERR_NO_DISP;
}





// CEK LAST TRANSAKSI FOR SETTLE BUKOPIN , DIKI ADD
int ValidTransSettleBkpn(void)
{
	uchar	szDateTime[14+1];
	//ulong	ulCardYear, ulCardMonth;
	ulong	ulCurYear, ulCurMonth, ulCurDate;
	uchar	ucInvalidFormat;
	int			iRet;
	uchar szBuff[50+1];
	uchar szBuf1[50+1];
	ulong szBufYear;
	ulong szBufMonth;
	ulong szBufDate;
	uchar	ucMustSetttle;

	//glProcInfo.bExpiryError = FALSE;
	//ucInvalidFormat = FALSE;
	ucMustSetttle = FALSE;




	if( glSysCtrl.uiLastRecNo>=MAX_TRANLOG )
	{
		//PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		//PubBeepErr();
		//PubWaitKey(5);
		return 0;
	}

	InitTransInfo();
	iRet = LoadTranLog(&glProcInfo.stTranLog, glSysCtrl.uiLastRecNo);
	if( iRet!=0 )
	{
		return 0;
	}

	//PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);


	MEM_ZERO(szBuff);
	MEM_ZERO(szBuf1);
	
	//Conv2EngTime(glProcInfo.stTranLog.szDateTime, szBuf1);
	//DD_MMM_YYYY__hh:mm:ss    
	//sprintf((char *)szBuff, "DATE: %-27.11s TIME: %8s\n", szBuf1, szBuf1+13);

	memcpy(szBuff, glProcInfo.stTranLog.szDateTime, 14);
	szBufYear  = PubAsc2Long(szBuff, 4, NULL);
	szBufMonth = PubAsc2Long(szBuff+4, 2, NULL);
	szBufDate  = PubAsc2Long(szBuff+6, 2, NULL);
	


	//ulCardYear  = PubAsc2Long(glProcInfo.stTranLog.szExpDate, 2, NULL);
	//ulCardYear += (ulCardYear>80) ? 1900 : 2000;
	//ulCardMonth = PubAsc2Long(glProcInfo.stTranLog.szExpDate+2, 2, NULL);

	GetDateTime(szDateTime);
	ulCurYear  = PubAsc2Long(szDateTime, 4, NULL);
	ulCurMonth = PubAsc2Long(szDateTime+4, 2, NULL);
	ulCurDate  = PubAsc2Long(szDateTime+6, 2, NULL);



	if( szBufYear<ulCurYear )
	{
		ucMustSetttle = TRUE;

		ScrCls();
		//PubDispString(_T("YEAR = EXP"), 0|DISP_LINE_LEFT);
		PubDispString(_T("SILAHKAN SETTLE"), 2|DISP_LINE_LEFT);
		PubDispString(_T("TERLEBIH DAHULU!"), 4|DISP_LINE_LEFT);
		PubBeepOk();
		PubWaitKey(3);
		return 1;
	}
	else if( szBufMonth<ulCurMonth )
	{
		szBufMonth = TRUE;

		ScrCls();
		//PubDispString(_T("Month = EXP"), 0|DISP_LINE_LEFT);
		PubDispString(_T("SILAHKAN SETTLE"), 2|DISP_LINE_LEFT);
		PubDispString(_T("TERLEBIH DAHULU!"), 4|DISP_LINE_LEFT);
		PubBeepOk();
		PubWaitKey(3);
		return 1;
	}
	else if( szBufDate<ulCurDate  )
	{
		szBufMonth = TRUE;

		ScrCls();
		//PubDispString(_T("Date = EXP"), 0|DISP_LINE_LEFT);
		PubDispString(_T("SILAHKAN SETTLE"), 2|DISP_LINE_LEFT);
		PubDispString(_T("TERLEBIH DAHULU!"), 4|DISP_LINE_LEFT);
		PubBeepOk();
		PubWaitKey(3);
		return 1;
	}

	//ScrCls();
	//PubDispString(_T("FlagSett=FALSE"), 4|DISP_LINE_LEFT);
	//PubBeepErr();
	//PubWaitKey(3);




	/*
	

	if( ulCardMonth>12 || ulCardMonth<1 )
	{
		ucInvalidFormat = TRUE;
		glProcInfo.bExpiryError = TRUE;
	}
	if (//ulCardYear>ulCurYear+20 ||	// 是否需要判断有效期太长的卡?
		ulCardYear<ulCurYear || 
		(ulCurYear==ulCardYear && ulCurMonth>ulCardMonth) )
	{
		glProcInfo.bExpiryError = TRUE;
	}

	if (glProcInfo.bExpiryError)
	{
		if((glCurTlmTmsIss.issExpiryDatReq)||(glCurTlmTmsIss.issCheckExpOff)||(glCurTlmTmsIss.issCheckExpAll))
		{
            		ScrClrLine(2,7);
			if (ucInvalidFormat)
			{
				PubDispString(_T("ERR EXP. FORMAT"), 4|DISP_LINE_LEFT);
			} 
			else
			{
				PubDispString(_T("CARD EXPIRED"), 4|DISP_LINE_LEFT);
			}
			if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
			{	// if EMV expired card, let core to continue process(based upon TACs/IACs)
				DelayMs(1500);
				return 0;
			}
			else
			{
				PubBeepErr();
				PubWaitKey(3);
				return ERR_NO_DISP;
			}
		}
	}

	*/

	return 0;
}



int SwipeCardProcBkpn(uchar bCheckICC)
{
	int		iRet;

	iRet = ReadMagCardInfoBkpn();
	if( iRet!=0 )
	{
		return iRet;
	}

	// 交易不要求判断卡片类型或者为非EMV终端,直接返回
	if( !bCheckICC || !ChkIfEmvEnable() )
	{
		return 0;
	}

	// EMV终端,继续检查
	if( glProcInfo.bIsFallBack==TRUE )
	{
		if( IsChipCardSvcCode(glProcInfo.szTrack2) )
		{	// fallback并且是IC卡,则返回成功
			glProcInfo.stTranLog.uiEntryMode = MODE_FALLBACK_SWIPE;
			return 0;
		}
		else
		{
			DispTransName();
			PubDispString(_T("NON EMV,RE-SWIPE"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(3);
			return ERR_NO_DISP;
		}
	}
	else if( IsChipCardSvcCode(glProcInfo.szTrack2) )
	{
		if( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_OFF_SALE &&
			glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_REFUND   &&
			glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_INSTALMENT )
		{
			DispTransName();
			DispInsertOnly();
			PubBeepErr();
			PubWaitKey(3);
			return ERR_NEED_INSERT;
		}
	}

	return 0;
}

int ReadMagCardInfoBkpn(void)
{
	int		iRet;

	//if( glEdcMsgPtr->MsgType==MAGCARD_MSG )
	//{
		glProcInfo.stTranLog.uiEntryMode = MODE_SWIPE_INPUT;
		//sprintf((char *)glProcInfo.szTrack1, "%.*s", LEN_TRACK1, glEdcMsgPtr->MagMsg.track1);
		//sprintf((char *)glProcInfo.szTrack2, "%.*s", LEN_TRACK2, "4731890000000101=2103126888000000");
		sprintf((char *)glProcInfo.szTrack2, "%.*s", LEN_TRACK2, glSysCtrl.szNoKartuBkpn1);
		//sprintf((char *)glProcInfo.szTrack3, "%.*s", LEN_TRACK3, glEdcMsgPtr->MagMsg.track3);
	//}
	//else
	//{
	//	glProcInfo.stTranLog.uiEntryMode = MODE_SWIPE_INPUT;
	//	MagRead(glProcInfo.szTrack1, glProcInfo.szTrack2, glProcInfo.szTrack3);
	//}


	iRet = GetPanFromTrack(glProcInfo.stTranLog.szPan, glProcInfo.stTranLog.szExpDate);
	if( iRet!=0 )
	{
		DispMagReadErr();
		PubBeepErr();
		PubWaitKey(3);
		return ERR_NO_DISP;
	}
	Beep();

	return 0;
}





int GetCardInitKartuBkpn(uchar ucMode)
{
	int		iRet, iEventID;
	uchar	bCheckICC, ucKey;


	// diki add
	PrivateLabelCardDetect= FALSE;


	//cek apakah mesti settle atau tidak? diki add for bukopin
	/*
	iRet = ValidTransSettleBkpn();
		if( iRet==1 )
		{
			return 1;
		}
		*/

		

	if( (glProcInfo.stTranLog.uiEntryMode & 0x0F)!=MODE_NO_INPUT )
	{
		return 0;
	}

	if( ucMode & FALLBACK_SWIPE )
	{
		ucMode &= ~(SKIP_CHECK_ICC|CARD_INSERTED);	// clear bit 8, force to check service code
	}

	bCheckICC = !(ucMode & SKIP_CHECK_ICC);
	

	// diki add for mini atmb
	/*
	if(MenuAtmbMerchantNasabah)
		{
		iRet = SwipeCardProcBkpn(bCheckICC);
			if( iRet==0 )
			{
				return ValidCard();
			}
		}
	*/
	
	

	TimerSet(0, TIME_OUT_INPUT*10);
	while( 1 )
	{
		iEventID = DetectCardEvent(ucMode);
		if( iEventID==CARD_KEYIN )
		{
			ucKey = getkey();
			if( ucKey==KEYCANCEL )
			{
				return ERR_USERCANCEL;
			}
			if( (ucMode & CARD_KEYIN) && ucKey>='0' && ucKey<='9' )
			{
				return ManualInputPan(ucKey);
			}
		}
		else if( iEventID==CARD_SWIPED )
		{
			iRet = SwipeCardProc(bCheckICC);
			if( iRet==0 )
			{
				return ValidCardInitBkpn();
			}
			else if( iRet==ERR_SWIPECARD )
			{
				DispMagReadErr();
				PubBeepErr();
				PubWaitKey(3);
			}
			else if( iRet==ERR_NEED_INSERT )	// 是芯片卡
			{
				if( !(ucMode & CARD_INSERTED) )
				{	// 本身交易不允许插卡
					return iRet;
				}
				ucMode &= ~CARD_SWIPED;			// 去掉刷卡检查
			}
			else
			{
				return iRet;
			}
		}

////////// diki add
		else if( iEventID==CARD_TAP)
		{
			iRet = StartTapPrvtLbl();
			if( iRet==0 )
			{
				PrivateLabelCardDetect= TRUE;
				return ValidCardPrvtLbl();
			}
			else
			{
				return iRet;
			}
		}
///////////////////////
				
#ifdef ENABLE_EMV
		else if( iEventID==CARD_INSERTED )
		{

///////////////////////
		iRet = Req_ICC_PrvtLbl_Cek();
		if( iRet==0 )
		{
			///iRet = TransSalePrvtLbl();
			///if( iRet!=0 )
			///{
			///	CommOnHook(FALSE);
			///	return iRet;
			///}
			
			iRet = Req_ICC_PrvtLbl();
			if( iRet!=0 )
			{
				return iRet;
			}

			iRet = ValidCardPrvtLbl();	// 校验卡号
			if( iRet!=0 )
			{
				return iRet;
			}
			PrivateLabelCardDetect= TRUE;
			return 0;
		} else {
		////}


			iRet = InsertCardProc();
			if( iRet==0 )
			{
				return 0;
			}
			else if( iRet==ERR_NEED_FALLBACK )
			{
				DispFallBackPrompt();
				PromptRemoveICC();
				ucMode = CARD_SWIPED|FALLBACK_SWIPE;	// Now we don't support fallback to manual-PAN-entry
			}
			else if( iRet==ERR_TRAN_FAIL )
			{
				PubDispString(_T("NOT ACCEPTED"), 4|DISP_LINE_LEFT);
				PubBeepErr();
				PubWaitKey(3);
				PromptRemoveICC();
				//emv_test();
				return ERR_NO_DISP;
			}
			else
			{
				return iRet;
			}

			}
		
		}		
		
		else if( iEventID==ERR_NO_DISP)
			return ERR_NO_DISP;
#endif
	}
}



int ValidCardInitBkpn(void)
{
	int		iRet;
	uchar 	szPan[lenPan+1];

	MEM_ZERO(szPan);
	MEM_ZERO(glSysCtrl.szNoKartuBkpn1);
	strcpy(glSysCtrl.szNoKartuBkpn1, glProcInfo.szTrack2);
	
	SaveSysCtrlBase();

	return 0;
}


