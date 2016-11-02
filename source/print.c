
#include "global.h"
#include "print.h"
/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
#if 0
enum{
	PRN_6x8=0,
	PRN_8x16,
	PRN_16x16,
	PRN_12x24,
	PRN_24x24,
	PRN_6x12,
	PRN_12x12,
	PRN_NULL=0xFF
};
//static void PrnSmallConstStr(uchar *str);
enum{
	NORMAL=0,
	SMALL,
	BIG
};
uchar flagSizeFont;
#endif

static int  PrintReceipt_FreeFmat(uchar ucPrnFlag);
static int  PrintReceipt_T(uchar ucPrnFlag);
static int  PrnCurAcqTransList_T(void);
static void PrnHead(uchar ucFreeFmat);
//static int  PrnCustomLogo_T(void);
static void PrnHead_T(void);
static void PrnAmount(uchar *pszIndent, uchar ucNeedSepLine);
static void PrnDescriptor(void);
static void PrnAdditionalPrompt(void);
static void PrnStatement(void);
static void PrnTotalInfo(void *pstInfo);
static int  PrnParaAcq(uchar ucAcqIndex);
static void PrnParaIssuer(uchar ucAcqIndex);
static void PrnParaIssuerSub(uchar ucIssuerKey);
static void PrnIssuerOption(uchar *psOption);
static void PrnCardTable(uchar ucIssuerKey);
static int  PrnInstalmentPara(void);
static int  PrnEmvPara(void);
static void PrnHexString(char *pszTitle, uchar *psHexStr, int iLen, uchar bNewLine);
static void DispPrnError(uchar ucErrCode);
//static void MultiLngPrnStr(uchar *str, uchar mode);
static int  PrnEmvParaNew(void); //Add By Zaki 22 April 2013
static int  PrnParaAcqNew(uchar ucAcqIndex); //Add By Zaki 22 April 2013

/********************** Internal variables declaration *********************/
/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// For thermal, small={8x16,16x16}
// For sprocket, small=normal={6x8,16x16}
void PrnSetSmall(void)
{
	if (ChkIfThermalPrinter())
	{
#if defined(_S_SERIES_) || defined(_SP30_) || defined(_P58_)
		PrnFontSetNew(PRN_8x16, PRN_16x16);
		PrnSpaceSet(1, 2);
#else
		PrnFontSet(0, 0);
		PrnSpaceSet(1, 2);
#endif
		flagSizeFont=SMALL;
	}
	else
	{
		PrnSetNormal();
	}
}

// For thermal, normal={12x24,24x24}
// For sprocket, normal={6x8,16x16}
void PrnSetNormal(void)
{
	if (ChkIfThermalPrinter())
	{
#if 0 	
#if defined(_S_SERIES_) || defined(_SP30_) || defined(_P58_)
		PrnFontSetNew(PRN_12x24, PRN_24x24);
#else
		PrnFontSet(1, 1);
#endif
#endif
		PrnFontSet(2, 2);
		PrnSpaceSet(0, 0);
		flagSizeFont=NORMAL;
	}
	else
	{
#if defined(_S_SERIES_) || defined(_SP30_) || defined(_P58_)
		PrnFontSetNew(PRN_6x8, PRN_16x16);
#else
		PrnFontSet(0, 0);
#endif
		PrnSpaceSet(0, 2);
	}
}

// For thermal, normal=big={12x24,24x24}
// For sprocket, big={8x16,16x16}
void PrnSetBig(void)
{
	if (ChkIfThermalPrinter())
	{
//		PrnSetNormal();
		PrnFontSet(6, 6);
		PrnSpaceSet(0, 0);
		flagSizeFont=BIG;

	}
	else
	{
#if defined(_S_SERIES_) || defined(_SP30_) || defined(_P58_)
		PrnFontSetNew(PRN_8x16, PRN_16x16);
#else
		PrnFontSet(1, 1);
#endif
		PrnSpaceSet(0, 2);
	}
}

// 打印明细
// Print the list of all transaction
void PrnAllList(void)
{
	int		iRet;
	uchar	szTitle[16+1], ucIndex;

	PubShowTitle(TRUE, (uchar *)_T("PRINT LOG       "));
	sprintf((char *)szTitle, _T("PRINT LOG       "));
	iRet = SelectAcq(FALSE, szTitle, &ucIndex);
	if( iRet!=0 )
	{
		return;
	}

	if( ucIndex!=ACQ_ALL )
	{
//		SetCurAcq(ucIndex);  //ALEX ADD
		PrnCurAcqTransList();
		return;
	}

#ifdef	__TELIUM_PARAM__
	for(ucIndex=0; ucIndex<glTlmSysParam.ucAcqNum; ucIndex++)
	{
		SetCurAcq(ucIndex);
		PrnCurAcqTransList();
	}
#else
	for(ucIndex=0; ucIndex<glSysParam.ucAcqNum; ucIndex++)
	{
		SetCurAcq(ucIndex);
		PrnCurAcqTransList();
	}
#endif	
}

// print list of transaction of current acquirer
int PrnCurAcqTransList(void)
{
	int		iCnt, iNumOfOnePage, iPageNum, iMaxNumOfOnePage;
	uchar	szBuff[30], szIssuerName[10+1];
	uchar   szDateTime[14+1], szTranName[16+1];

	PubShowTitle(TRUE, (uchar *)"PRINT LOG       ");
#ifdef	__TELIUM_PARAM__
	if( GetTranLogNum(glCurTlmTmsAcq.acqID)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_CENTER);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}
#else
	if( GetTranLogNum(glCurAcq.ucKey)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_CENTER);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}
#endif

	DispPrinting();
	if( ChkIfThermalPrinter() )
	{
		return PrnCurAcqTransList_T();
	}

	PrnInit();
	PrnSetNormal();

	PrnStep(30);
	PrnStr("      TRANSACTION LIST\n");
	GetDateTime(szDateTime);
	Conv2EngTime(szDateTime, szBuff);
	PrnStr("%s\n", szBuff);

#ifdef	__TELIUM_PARAM__
	PrnStr("HOST NII: %.3s   %.10s\n", glCurTlmTmsAcq.acqNII, glCurTlmTmsAcq.acqName);
#else
	PrnStr("HOST NII: %.3s   %.10s\n", glCurAcq.szNii, glCurAcq.szName);
#endif
	PrnStep(15);
#ifdef	__TELIUM_PARAM__
	PrnStr("%14.8s\n%21.15s\n\n", glCurTlmTmsAcq.acqTID, glCurTlmTmsAcq.acqMID);
#else
	PrnStr("%14.8s\n%21.15s\n\n", glCurAcq.szTermID, glCurAcq.szMerchantID);
#endif
	iMaxNumOfOnePage = ChkEdcOption(EDC_TIP_PROCESS) ? 4 : 6;
	iNumOfOnePage = 0;
	iPageNum = 1;
	for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
	{
#ifdef	__TELIUM_PARAM__	
		if( glSysCtrl.sAcqKeyList[iCnt]!=glCurTlmTmsAcq.acqID)
		{
			continue;
		}
#else
		if( glSysCtrl.sAcqKeyList[iCnt]!=glCurAcq.ucKey )
		{
			continue;
		}
#endif
		memset(&glProcInfo.stTranLog, 0, sizeof(TRAN_LOG));
		LoadTranLog(&glProcInfo.stTranLog, (ushort)iCnt);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);  //Alex add

#ifdef	__TELIUM_PARAM__
		ConvIssuerName(glCurTlmTmsIss.issRef, szIssuerName);
#else
		ConvIssuerName(glCurIssuer.szName, szIssuerName);
#endif

		PrnStr("ISSUER: %.10s\n", szIssuerName); // cung closed

		if( ChkIfDispMaskPan2() )
		{
			MaskPan(glProcInfo.stTranLog.szPan, szBuff);
			PrnStr("PAN: %s\n", szBuff);
		}
		else
		{
			PrnStr("PAN: %s\n", glProcInfo.stTranLog.szPan);
		}

		sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel);
		if( glProcInfo.stTranLog.uiStatus & TS_VOID )
		{
			sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucOrgTranType].szLabel);
			sprintf((char *)szBuff, "%s(%s)", _T(szTranName), _T("VOID"));
		}
		else if( glProcInfo.stTranLog.uiStatus & TS_ADJ )
		{
			sprintf((char *)szBuff, "%s(%s)", _T(szTranName), _T("ADJ"));
		}
		else
		{
			sprintf((char *)szBuff, "%s", _T(szTranName));
		}
		PrnStr("%-21.21s%06lu\n", szBuff, glProcInfo.stTranLog.ulInvoiceNo);
		PrnAmount((uchar *)"", FALSE);
		PrnStr("\n");

		iNumOfOnePage++;
		if( (iPageNum==1 && iNumOfOnePage==3) ||
			(iPageNum!=1 && iNumOfOnePage==iMaxNumOfOnePage) )
		{
			PrnStr("\f");
			if( StartPrinter()!=0 )
			{
				return 1;
			}

			iNumOfOnePage = 0;
			iPageNum++;

			PrnInit();
			PrnSetNormal();

			PrnStep(20);
		}
	}
	PrnStr("      END OF LIST   ");
	PrnStr("\f");

	return StartPrinter();
}

int PrnCurAcqTransList_T(void)
{
	int		iCnt, iNumOfOnePage;
	uchar	szBuff[30], szIssuerName[10+1];
	uchar   szTranName[16+1];

	PrnInit();
	PrnSetNormal();

	PrnStep(30);
	PrnStr("     TRANSACTION LIST\n");
	PrnEngTime();

#ifdef	__TELIUM_PARAM__
	PrnStr("HOST NII: %.3s   %.10s\n", glCurTlmTmsAcq.acqNII, glCurTlmTmsAcq.acqName);
#else
	PrnStr("HOST NII: %.3s   %.10s\n", glCurAcq.szNii, glCurAcq.szName);
#endif
	PrnStep(15);
#ifdef	__TELIUM_PARAM__
	PrnStr("TID: %s\nMID: %s\n", glCurTlmTmsAcq.acqTID, glCurTlmTmsAcq.acqMID);
#else
	PrnStr("TID: %s\nMID: %s\n", glCurAcq.szTermID, glCurAcq.szMerchantID);
#endif
	PrnStr("============================\n");
	iNumOfOnePage = 0;
	for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
	{
#ifdef	__TELIUM_PARAM__
		if( glSysCtrl.sAcqKeyList[iCnt]!=glCurTlmTmsAcq.acqID)
		{
			continue;
		}
#else
		if( glSysCtrl.sAcqKeyList[iCnt]!=glCurAcq.ucKey )
		{
			continue;
		}
#endif

		memset(&glProcInfo.stTranLog, 0, sizeof(TRAN_LOG));
		LoadTranLog(&glProcInfo.stTranLog, (ushort)iCnt);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);  //Alex add
#ifdef	__TELIUM_PARAM__
		ConvIssuerName(glCurTlmTmsIss.issRef, szIssuerName);
#else
		ConvIssuerName(glCurIssuer.szName, szIssuerName);
#endif
		//PrnStr("ISSUER: %.10s\n", szIssuerName); //close cung
		PrnStr("ISSUER: %.10s\n", glProcInfo.stTranLog.szCardType);

		if( ChkIfDispMaskPan2() )
		{
			MaskPan(glProcInfo.stTranLog.szPan, szBuff);
			PrnStr("PAN: %s\n", szBuff);
		}
		else
		{
			PrnStr("PAN: %s\n", glProcInfo.stTranLog.szPan);
		}

		sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel);
		if( glProcInfo.stTranLog.uiStatus & TS_VOID )
		{
			sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucOrgTranType].szLabel);
			sprintf((char *)szBuff, "%s(%s)", _T(szTranName), _T("VOID"));
		}
		else if( glProcInfo.stTranLog.uiStatus & TS_ADJ )
		{
			sprintf((char *)szBuff, "%s(%s)", _T(szTranName), _T("ADJ"));
		}
		else
		{
			sprintf((char *)szBuff, "%s", _T(szTranName));
		}
		PrnStr("TXN: %-21.21s\nTRACE: %06lu\n", szBuff, glProcInfo.stTranLog.ulInvoiceNo);
		PrnAmount((uchar *)"", FALSE);
		PrnStr("============================");
		PrnStr("\n");
		
		iNumOfOnePage++;
		if( (iNumOfOnePage%5)==0 )
		{
			if( StartPrinter()!=0 )
			{
				return 1;
			}
			iNumOfOnePage = 0;
			PrnInit();
			PrnSetNormal();
			PrnStep(20);
		}
	}
	PrnStr("      END OF LIST   ");
	PrnStr("\n\n\n\n\n\n");
	return StartPrinter();	
}

int PrintReceipt_FreeFmat(uchar ucPrnFlag)
{
	uchar	szBuff[50];
	uchar	szIssuerName[10+1];

	PrnInit();
	PrnSetNormal();

	PrnStr("\n\n\n");
	PrnHead(TRUE);

	// issuer Name
#ifdef	__TELIUM_PARAM__
		ConvIssuerName(glCurTlmTmsIss.issRef, szIssuerName);
#else
		ConvIssuerName(glCurIssuer.szName, szIssuerName);
#endif
	PrnStr("CARD TYPE: %-10.10s    ", szIssuerName);

#ifdef	__TELIUM_PARAM__
	// Expiry date
	if(glCurTlmTmsIss.issExpiryDate)
	{
		PrnStr("%2.2s/%2.2s", &glProcInfo.stTranLog.szExpDate[2], glProcInfo.stTranLog.szExpDate);
	}
#else
	// Expiry date
	if( ChkIssuerOption(ISSUER_EN_EXPIRY) )
	{
		if( ChkIssuerOption(ISSUER_MASK_EXPIRY) )
		{
			PrnStr("**/**");
		}
		else
		{
			PrnStr("%2.2s/%2.2s", &glProcInfo.stTranLog.szExpDate[2],
					glProcInfo.stTranLog.szExpDate);
		}
	}
#endif

	PrnStr("\n");

	//	PAN
	if (ChkIfTransMaskPan(1))
	{
		MaskPan(glProcInfo.stTranLog.szPan, szBuff);
	}
	else
	{
		strcpy(szBuff, glProcInfo.stTranLog.szPan);
	}
	if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT )
	{
		PrnStr("CARD NO: %s S\n", szBuff);
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		PrnStr("CARD NO: %s C\n", szBuff);
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
			 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		PrnStr("CARD NO: %s F\n", szBuff);
	}
	else
	{
		PrnStr("CARD NO: %s M\n", szBuff);
	}

	// Holder
	PrnStr("HOLDER: %s\n", glProcInfo.stTranLog.szHolderName);

	// print txn name & expiry
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID || (glProcInfo.stTranLog.uiStatus & TS_VOID) )
	{		
		sprintf((char *)szBuff, "%s(%s)", _T("VOID"), _T(glTranConfig[glProcInfo.stTranLog.ucOrgTranType].szLabel));
	}
	else if( glProcInfo.stTranLog.uiStatus & TS_ADJ )
	{
		sprintf((char *)szBuff, "%s(%s)", _T(glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel), _T("ADJ"));
	}
	else
	{
		sprintf((char *)szBuff, "%s", _T(glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel));
	}
	PrnStr("%s\n", szBuff);

	// Batch NO & invoice #
	PrnStep(6);
#ifdef	__TELIUM_PARAM__
	PrnStr("BATCH NO: %06ld   REF: %06ld\n", glCurTlmTmsAcq.ulCurBatchNo, glProcInfo.stTranLog.ulInvoiceNo);
#else
	PrnStr("BATCH NO: %06ld   REF: %06ld\n", glCurAcq.ulCurBatchNo, glProcInfo.stTranLog.ulInvoiceNo);
#endif
	// RRN, AuthCode
	PrnStr("RRN: %-12.12s AUTH: %-6.6s\n", glProcInfo.stTranLog.szRRN, glProcInfo.stTranLog.szAuthCode);

	PrnStep(2);
	if( glProcInfo.stTranLog.ucInstalment!=0 )
	{
		PrnStr("   NO. OF INSTALMENT:%02d\n", glProcInfo.stTranLog.ucInstalment);
	}

	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
        if (strlen(glProcInfo.stTranLog.szAppPreferName)!=0)
        {
		    PrnStr("   APP: %.16s\n", glProcInfo.stTranLog.szAppPreferName);
        } 
        else
        {
		    PrnStr("   APP: %.16s\n", glProcInfo.stTranLog.szAppLabel);
        }
		PubBcd2Asc0(glProcInfo.stTranLog.sAID, glProcInfo.stTranLog.ucAidLen, szBuff);
		PubTrimTailChars(szBuff, 'F');
		PrnStr("   AID: %s\n", szBuff);
		PubBcd2Asc0(glProcInfo.stTranLog.sAppCrypto, 8, szBuff);
		PrnStr("   TC : %s\n", szBuff);
#ifdef ENABLE_EMV
#ifdef EMV_TEST_VERSION
// 		PubBcd2Asc0(glProcInfo.stTranLog.sTSI, 2, szBuff);
// 		PrnStr("   TSI: %s\n", szBuff);
// 		PubBcd2Asc0(glProcInfo.stTranLog.sTVR, 5, szBuff);
// 		PrnStr("   TVR: %s\n", szBuff);
#endif
#endif
		if( glProcInfo.stTranLog.uiEntryMode & MODE_OFF_PIN )
		{
			PrnStr("   PIN VERIFIED\n");
		}
		else
		{
			PrnStr("\n");
		}
	}

	PrnDescriptor();

	PrnAmount((uchar *)"   ", TRUE);

	PrnAdditionalPrompt();

	PrnStatement();

	if( ucPrnFlag==PRN_REPRINT )
	{
		PrnFontSetNew(PRN_8x16, PRN_16x16);
		PrnStr("       REPRINT\n");
	}
	if (ChkIfTrainMode())
	{
		PrnStr("    ****DEMO****");
	}

	PrnStr("\f");

	StartPrinter();
	return 0;
}

int PrintReceipt_T(uchar ucPrnFlag)
{	
	uchar	ucNum;
	uchar	szBuff[50],szBuf1[50];
	uchar	szIssuerName[10+1], szTranName[16+1];

	for(ucNum=0; ucNum<NumOfReceipt(); ucNum++)
	{
		PrnInit();
		PrnSetNormal();

		PrnCustomLogo_T();

		PrnHead_T();

		// issuer Name
		memset(szBuff, 0, sizeof(szBuff));
		sprintf((char *)szBuff, "%s\n", _T("CARD TYPE:"));
//		PrnSetSmall();
		MultiLngPrnStr(szBuff,DISP_LINE_LEFT);

#ifdef	__TELIUM_PARAM__
		ConvIssuerName(glCurTlmTmsIss.issRef, szIssuerName);
#else
		ConvIssuerName(glCurIssuer.szName, szIssuerName);
#endif
		PrnSetNormal();
		memset(szBuff, 0, sizeof(szBuff));
		sprintf((char *)szBuff, "%s\n", szIssuerName);
		MultiLngPrnStr(szBuff,DISP_LINE_LEFT);

		// PAN
		sprintf((char *)szBuff, "%s\n", _T("CARD NO./EXP. DATE"));
//		PrnSetSmall();
		MultiLngPrnStr(szBuff,DISP_LINE_LEFT);
		//memcpy(szBuff, glProcInfo.stTranLog.szPan, sizeof(glProcInfo.stTranLog.szPan));

		if (ChkIfTransMaskPan(ucNum))
		{
			MaskPan(glProcInfo.stTranLog.szPan, szBuff);
		}
		else
		{
			strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
		}

		if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT )
		{
			sprintf(szBuf1, "%s -SWIPE", szBuff);
		}
		else if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			sprintf(szBuf1, "%s -DIP", szBuff);
		}
		else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
				 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
		{
			sprintf(szBuf1, "%s -FALLBACK", szBuff);
		}
		else
		{
			sprintf(szBuf1, "%s-MANUAL", szBuff);
		}
		PrnSetNormal();
		memset(szBuff, 0, sizeof(szBuff));
		sprintf((char *)szBuff, "%-23.23s", szBuf1);
		MultiLngPrnStr(szBuff,DISP_LINE_LEFT);

#ifdef	__TELIUM_PARAM__
		// print expiry
		if(glCurTlmTmsIss.issExpiryDate)
		{
			memset(szBuff, 0, sizeof(szBuff));
			sprintf((char *)szBuff, " %2.2s/%2.2s", &glProcInfo.stTranLog.szExpDate[2],
			glProcInfo.stTranLog.szExpDate);
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		}
#else
		// print expiry
		if( ChkIssuerOption(ISSUER_EN_EXPIRY) )
		{
			if( ChkIssuerOption(ISSUER_MASK_EXPIRY) )
			{
				PrnStr(" **/**");
			}
			else
			{
				memset(szBuff, 0, sizeof(szBuff));
				sprintf((char *)szBuff, " %2.2s/%2.2s", &glProcInfo.stTranLog.szExpDate[2],
						glProcInfo.stTranLog.szExpDate);
				MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
			}
		}
#endif

		PrnStr("\n");

		// Holder name
		sprintf((char *)szBuff, "%s\n", _T("HOLDER"));
//		PrnSetSmall();
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnSetNormal();
		memset(szBuff, 0, sizeof(szBuff));
		sprintf((char *)szBuff, "%-23.23s\n", glProcInfo.stTranLog.szHolderName);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		// Transaction type
		sprintf((char *)szBuff, "%s\n", _T("TRANS. TYPE"));
//		PrnSetSmall();
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel);
		if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID || (glProcInfo.stTranLog.uiStatus & TS_VOID) )
		{
			sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucOrgTranType].szLabel);
			sprintf((char *)szBuff, "%s(%s)", _T(szTranName), _T("VOID"));
		}
		else if( glProcInfo.stTranLog.uiStatus & TS_ADJ )
		{
			sprintf((char *)szBuff, "%s(%s)", _T(szTranName), _T("ADJ"));
		}
		else
		{
			sprintf((char *)szBuff, "%s", szTranName);
		}
		PrnSetNormal();
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		// Batch, Invoice
		sprintf((char *)szBuff, "%s\n", _T("BATCH NO.              TRACE NO."));
//		PrnSetSmall();
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

#ifdef	__TELIUM_PARAM__
		sprintf((char *)szBuff, "%06lu          %06lu\n", glCurTlmTmsAcq.ulCurBatchNo, glProcInfo.stTranLog.ulInvoiceNo);
#else
		sprintf((char *)szBuff, "%06lu          %06lu\n", glCurAcq.ulCurBatchNo, glProcInfo.stTranLog.ulInvoiceNo);
#endif
		PrnSetNormal();
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		// Date, time
		sprintf((char *)szBuff, "%s\n", _T("DATE/TIME                       "));
//		PrnSetSmall();
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		Conv2EngTime(glProcInfo.stTranLog.szDateTime, szBuff);
		PrnSetNormal();
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "%-22.22s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		// REF, APPV
		sprintf((char *)szBuff, "%s\n", _T("REF. NO.               APP. CODE"));
//		PrnSetSmall();
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnSetNormal();
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "%-14.14s  %-12.12s\n", glProcInfo.stTranLog.szRRN, glProcInfo.stTranLog.szAuthCode);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		PrnStr("\n");

		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			memset(szBuf1, 0, sizeof(szBuf1));
			sprintf((char *)szBuf1, "APP:%.16s\n", glProcInfo.stTranLog.szAppLabel);
			MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

			PubBcd2Asc0(glProcInfo.stTranLog.sAID, glProcInfo.stTranLog.ucAidLen, szBuff);
			PubTrimTailChars(szBuff, 'F');
			memset(szBuf1, 0, sizeof(szBuf1));
			sprintf((char *)szBuf1, "AID:%.32s\n", szBuff);
			MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

			PubBcd2Asc0(glProcInfo.stTranLog.sAppCrypto, 8, szBuff);
			memset(szBuf1, 0, sizeof(szBuf1));
			sprintf((char *)szBuf1, "TC: %.16s\n", szBuff);
			MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

#ifdef ENABLE_EMV
#ifdef EMV_TEST_VERSION
// 			PubBcd2Asc0(glProcInfo.stTranLog.sTSI, 2, szBuff);
// 			PrnStr("   TSI: %s\n", szBuff);
// 			PubBcd2Asc0(glProcInfo.stTranLog.sTVR, 5, szBuff);
// 			PrnStr("   TVR: %s\n", szBuff);
#endif
#endif
			if( glProcInfo.stTranLog.uiEntryMode & MODE_OFF_PIN )
			{
				MultiLngPrnStr("   PIN VERIFIED\n", DISP_LINE_LEFT);
			}
			else
			{
				PrnStr("\n");
			}
		}

		PrnDescriptor();

		// amount
		PrnAmount((uchar *)"", TRUE);  //Print amount

		PrnAdditionalPrompt();

		PrnStatement();

#ifdef BANK_MANDIRI
//for test power buy
		if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_POWERBUY)
		{
			sprintf((char *)szBuff, "%s %s\n", _T("TERM     "), glProcInfo.stTranLog.szTenorPowerBuy);
			PrnStr(szBuff);
			App_ConvAmountTran(glProcInfo.stTranLog.szAmountMonthlyPowerBuy, szBuff, GA_SEPARATOR);
			sprintf((char *)szBuf1, "%s %s\n", _T("MONTHLY  "), szBuff);
			PrnStr(szBuf1);
		}
#endif
		if( ucPrnFlag==PRN_REPRINT )
		{
			sprintf((char *)szBuff, "%s\n", _T("         * REPRINT *"));
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		}

		if( ucNum==0 )
		{
			sprintf((char *)szBuff, "%s\n", _T("CARDHOLDER SIGNATURE"));
//			PrnSetSmall();
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
			PrnSetNormal();
			PrnStr("\n\n\n\n");
			PrnStr("-----------------------------\n");
			PrnSetSmall();
			MultiLngPrnStr("I ACKNOWLEDGE SATISFACTORY RECEIPT\n", DISP_LINE_CENTER);
			MultiLngPrnStr("OF RELATIVE  GOODS/SERVICE\n", DISP_LINE_CENTER);
		}

		if (ChkIfTrainMode())
		{
			sprintf((char *)szBuff, "%s\n", _T("     ***** DEMO *****"));
			PrnSetNormal();
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		}
		else
		{
#ifdef ENABLE_EMV
#ifdef EMV_TEST_VERSION
			sprintf((char *)szBuff, "%s\n", _T("  ** FOR EMV TEST ONLY **"));
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
#endif
#endif
		}

        PrnSetNormal();
		if( ucNum==0 )
		{
			MultiLngPrnStr(_T("  **** MERCHANT COPY ****  "), DISP_LINE_CENTER);
		}
		else if( ucNum==1 )
		{
			MultiLngPrnStr(_T("  **** CUSTOMER COPY ****  "), DISP_LINE_CENTER);
		}
		else if( ucNum==2 )
		{
			MultiLngPrnStr(_T("  **** BANK COPY ****  "), DISP_LINE_CENTER);
		}
		PrnStr("\n\n\n\n\n\n\n\n");

		StartPrinter();

		if( ucNum==0 && NumOfReceipt() != 1)
		{
            kbflush();
			ScrClrLine(2,7);
			PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
			PubWaitKey(USER_OPER_TIMEOUT);
		}
	}

	return 0;
}

void PrnHead(uchar ucFreeFmat)
{
	uchar	szBuff[32];

	if (ChkIfTrainMode())
	{
		PrnStr(_T("DEMONSTRATE ONLY\n"));
		sprintf(szBuff, "%.30s", _T("NOT FOR PAYMENT PROOF"));
		if( szBuff[0]>=0xA0 )
		{
			PrnStr("%.30s\n", szBuff);
		}
		else
		{
			PrnStr("%.30s\n\n", szBuff);
		}
	} 
	else
	{
#ifdef __TELIUM_PARAM__
		PrnStr("%.23s\n", glTlmSysParam.Con.rptHdrSiteName);
		if( glTlmSysParam.Con.rptHdrSiteAddr1[0]!=0)
		{
			PrnStr("%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr1);
		}
		if( glTlmSysParam.Con.rptHdrSiteAddr2[0]!=0)
		{
			PrnStr("%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr2);
		}
		if( glTlmSysParam.Con.rptHdrSiteAddr3[0]!=0)
		{
			PrnStr("%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr3);
		}
#else
		PrnStr("%.23s\n", glSysParam.stEdcInfo.szMerchantName);
		if( glSysParam.stEdcInfo.szMerchantAddr[0]>=0xA0 )
		{
			PrnStr("%.23s\n", glSysParam.stEdcInfo.szMerchantAddr);
		}
		else
		{
			PrnStr("%.23s\n%.23s\n", glSysParam.stEdcInfo.szMerchantAddr,
					&glSysParam.stEdcInfo.szMerchantAddr[23]);
		}
#endif
	}

	PrnStep(15);

#ifdef	__TELIUM_PARAM__
	if (ucFreeFmat)
	{
		GetEngTime(szBuff);
		PrnStr("MID:%-15.15s      %5.5s\n",   glCurTlmTmsAcq.acqMID, szBuff+11);
		PrnStr("TID:%-8.8s        %10.10s\n", glCurTlmTmsAcq.acqTID, szBuff);
	}
	else
	{
		PrnStr("%14.8s\n%21.15s\n", glCurTlmTmsAcq.acqTID, glCurTlmTmsAcq.acqMID);
	}
#else
	if (ucFreeFmat)
	{
		GetEngTime(szBuff);
		PrnStr("MID:%-15.15s      %5.5s\n",   glCurAcq.szMerchantID, szBuff+11);
		PrnStr("TID:%-8.8s        %10.10s\n", glCurAcq.szTermID, szBuff);
	}
	else
	{
		PrnStr("%14.8s\n%21.15s\n", glCurAcq.szTermID, glCurAcq.szMerchantID);
	}
#endif	
	PrnStep(15);
}


int PrnFontSetNew(uchar ucEnType, uchar ucSQType)
{
#if defined(_S_SERIES_) || defined(_P58_)
	int	iRet;
	ST_FONT font1,font2;

	font1.CharSet = CHARSET_WEST;
	font1.Width   = 8;
	font1.Height  = 16;
	font1.Bold    = 0;
	font1.Italic  = 0;

#ifdef	__TELIUM_PARAM__
	font2.CharSet = glTlmSysParam.stEdcInfo.stLangCfg.ucCharSet;
#else
	font2.CharSet = glSysParam.stEdcInfo.stLangCfg.ucCharSet;
#endif
	font2.Width   = 16;
	font2.Height  = 16;
	font2.Bold    = 0;
	font2.Italic  = 0;

	if (ucEnType==PRN_6x8)
	{
		font1.Width   = 6;
		font1.Height  = 8;
	}
	if (ucEnType==PRN_6x12)
	{
		font1.Width   = 6;
		font1.Height  = 12;
	}
	if (ucEnType==PRN_12x24)
	{
		font1.Width   = 12;
		font1.Height  = 24;
	}

	if (ucSQType==PRN_12x12)
	{
		font2.Width   = 12;
		font2.Height  = 12;
	}
	if (ucSQType==PRN_24x24)
	{
		font2.Width   = 24;
		font2.Height  = 24;
	}

	// in WIN32 do not allow NULL
#ifndef WIN32
	if (font1.CharSet==font2.CharSet)
	{
		iRet = PrnSelectFont(&font1,NULL);
	} 
	else
#endif
	{
		iRet = PrnSelectFont(&font1,&font2);
	}
	PrnDoubleWidth(0, 0);
	PrnDoubleHeight(0, 0);

	if ((iRet!=0) && (font1.Width>=12) && (font2.Width>=12))
	{
		font1.Width /= 2;
		font1.Height /= 2;
		font2.Width /= 2;
		font2.Height /= 2;
		iRet = PrnSelectFont(&font1,&font2);
		if (iRet==0)
		{
			PrnDoubleWidth(1, 1);
			PrnDoubleHeight(1, 1);
		}		
	}

	return iRet;

#else
	if (ucEnType==PRN_6x8)
	{
		PrnFontSet(0, 0);
	}
	else
	{
		PrnFontSet(1, 1);
	}

	return 0;
	
#endif
}

// for thermal only
// void PrnSmallConstStr(uchar *str)
// {
// 	PrnSetSmall();
// 	PrnStr(str);
// 	PrnSetNormal();
// }

int PrnCustomLogo_T(void)
{
	uchar	*psLogoData;
	int		iWidth, iHeigh;

	psLogoData = NULL;
	GetNowPrnLogo(&psLogoData);
	if (psLogoData!=NULL)
	{
		iWidth = 0;
		iHeigh = 0;
		GetLogoWidthHeigh(psLogoData, &iWidth, &iHeigh);
//		hex_dump_char("iWidth", &iWidth, sizeof(int));
//		hex_dump_char("iHeigh", &iHeigh, sizeof(int));
		iWidth=384-iWidth;
		
		if ((iWidth)<384)
		{
			iWidth = (384 -iWidth)/2;	// let logo be printed at center
		}
		else
		{
			iWidth = 0;

		}
//		hex_dump_char("iWidth", &iWidth, sizeof(int));
//		PrnLeftIndent((ushort)iWidth);
		PrnLeftIndent(0);				//geseran 4212
		PrnLeftIndent(63);    // diki add geser biar di tengah
		PrnSetNormal();
		PrnLogo(psLogoData);
		PrnLeftIndent(0);
		PrnStr("\n");
		return 0;
	}

	return -1;
}

void PrnHead_T(void)
{
	uchar	szBuff[32];

	PrnStep(30);
	PrnSetNormal();

	if (ChkIfTrainMode())
	{
		MultiLngPrnStr(_T("* DEMONSTRATE ONLY *"), DISP_LINE_LEFT);
		PrnStr("\n");
		PrnStep(10);
		
		memset(szBuff, 0, sizeof(szBuff));
		sprintf(szBuff, "%.30s", _T("* NOT FOR PAYMENT PROOF *"));
		if( szBuff[0]>=0xA0 )
		{
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
			PrnStr("\n");
		}
		else
		{
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
			PrnStr("\n\n");
		}
		PrnStep(15);
	}
	else
	{
#ifdef	__TELIUM_PARAM__
		memset(szBuff, 0, sizeof(szBuff));
		sprintf(szBuff, "%.23s\n", glTlmSysParam.Con.rptHdrSiteName);
		MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
		PrnStep(10);

//		PrnSetSmall();
		if( glTlmSysParam.Con.rptHdrSiteAddr1[0]!=0)
		{
			memset(szBuff, 0, sizeof(szBuff));
			sprintf(szBuff, "%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr1);
			MultiLngPrnStr(szBuff, DISP_LINE_CENTER);

//			PrnStr("%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr1);
		}
		if( glTlmSysParam.Con.rptHdrSiteAddr2[0]!=0)
		{
			memset(szBuff, 0, sizeof(szBuff));
			sprintf(szBuff, "%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr2);
			MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
			
//			PrnStr("%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr2);
		}
		if( glTlmSysParam.Con.rptHdrSiteAddr3[0]!=0)
		{
			memset(szBuff, 0, sizeof(szBuff));
			sprintf(szBuff, "%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr3);
			MultiLngPrnStr(szBuff, DISP_LINE_CENTER);

//			PrnStr("%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr3);
		}
#else
		memset(szBuff, 0, sizeof(szBuff));
		sprintf(szBuff, "%.23s\n", glSysParam.stEdcInfo.szMerchantName);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStep(10);

		if (strlen(glSysParam.stEdcInfo.szMerchantAddr)>23)
		{
			memset(szBuff, 0, sizeof(szBuff));
			sprintf(szBuff, "%.23s\n", glSysParam.stEdcInfo.szMerchantAddr);
//			PrnSetSmall();
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
			sprintf(szBuff, "%.23s", glSysParam.stEdcInfo.szMerchantAddr+23);
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		}
		else
		{
//			PrnSetSmall();
			PrnStr(glSysParam.stEdcInfo.szMerchantAddr);
			PrnStr("\n");
		}
#endif		
		PrnSetNormal();
		PrnStr("\n");
		PrnStep(15);
	}


//	PrnSetSmall();
	MultiLngPrnStr(_T("MERCHANT ID.        "), DISP_LINE_LEFT);
	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
#ifdef	__TELIUM_PARAM__
	sprintf(szBuff,"%15.15s\n", glCurTlmTmsAcq.acqMID);
#else
	sprintf(szBuff,"%15.15s\n", glCurAcq.szMerchantID);
#endif
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

//	PrnSetSmall();
	MultiLngPrnStr(_T("TERMINAL ID.        "), DISP_LINE_LEFT);
	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
#ifdef	__TELIUM_PARAM__
	sprintf(szBuff,"%15.8s\n", glCurTlmTmsAcq.acqTID);
#else
	sprintf(szBuff,"%15.15s\n", glCurAcq.szTermID);
#endif
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MultiLngPrnStr("=============================\n", DISP_LINE_LEFT);
	PrnStep(15);
}


void PrnAmount_New(uchar *pszIndent, uchar ucNeedSepLine)
{
	uchar	szBuff[50], szTotalAmt[12+1];
	uchar   szTempBuff[100];

	PrnStep(10);
	
	if( ChkIfNeedTip() &&
		!(glProcInfo.stTranLog.uiStatus &TS_VOID) )
	{
		//-------------------------------- BASE --------------------------------
		memset(szTempBuff, 0, sizeof(szTempBuff));
//		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, szBuff, 0);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, szBuff, GA_SEPARATOR);
		if (ChkIfThermalPrinter())
		{
			sprintf((char *)szTempBuff, _T("%sBASE      %17.17s\n"), pszIndent, szBuff);
		}
		else
		{
			sprintf((char *)szTempBuff, "%sBASE      %17.17s\n", pszIndent, szBuff);
		}
		MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);

		//-------------------------------- TIPS --------------------------------
		memset(szTempBuff, 0, sizeof(szTempBuff));
		if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
		{
//			App_ConvAmountTran(glProcInfo.stTranLog.szTipAmount, szBuff, 0);
			App_ConvAmountTran(glProcInfo.stTranLog.szTipAmount, szBuff, GA_SEPARATOR);
			if (ChkIfThermalPrinter())
			{
				sprintf((char *)szTempBuff, _T("%sTIPS      %17.17s\n"), pszIndent, szBuff);
			} 
			else
			{
				sprintf((char *)szTempBuff, "%sTIPS      %17.17s\n", pszIndent, szBuff);
			}
			MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		}
		else
		{
			if (ChkIfThermalPrinter())
			{
				sprintf((char *)szTempBuff, _T("%sTIPS\n"), pszIndent);
			} 
			else
			{
				sprintf((char *)szTempBuff, "%sTIPS\n", pszIndent);
			}
		}
		MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);

		//-------------------------------- TOTAL --------------------------------
		memset(szTempBuff, 0, sizeof(szTempBuff));
		if( ucNeedSepLine )
		{
			sprintf((char *)szTempBuff, "%s          -----------------\n", pszIndent);
			MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		}

		memset(szTempBuff, 0, sizeof(szTempBuff));
		if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
		{
			PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//			App_ConvAmountTran(szTotalAmt, szBuff, 0);
			App_ConvAmountTran(szTotalAmt, szBuff, GA_SEPARATOR);
			if (ChkIfThermalPrinter())
			{
				sprintf((char *)szTempBuff, _T("%sTOTAL  %17s\n"), pszIndent, szBuff);
			} 
			else
			{
				sprintf((char *)szTempBuff, "%sTOTAL  %17s\n", pszIndent, szBuff);
			}
		}
		else
		{
			if (ChkIfThermalPrinter())
			{
				sprintf((char *)szTempBuff, _T("%sTOTAL\n"), pszIndent);
			} 
			else
			{
				sprintf((char *)szTempBuff, "%sTOTAL\n", pszIndent);
			}
		}
		PrnSetBig();
		MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		PrnSetNormal();
	
	}
	else
	{
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, szBuff, GetTranAmountInfo(&glProcInfo.stTranLog));

		memset(szTempBuff, 0, sizeof(szTempBuff));
		if (ChkIfThermalPrinter())
		{
			if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID || (glProcInfo.stTranLog.uiStatus & TS_VOID) )
				sprintf((char *)szTempBuff, _T("%sVOID:  %17s\n"), pszIndent, szBuff);
			else
				sprintf((char *)szTempBuff, _T("%sAMOUNT:%17s\n"), pszIndent, szBuff);
		} 
		else
		{
			if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID || (glProcInfo.stTranLog.uiStatus & TS_VOID) )
				sprintf((char *)szTempBuff, _T("%sVOID:  %17s\n"), pszIndent, szBuff);
			else
				sprintf((char *)szTempBuff, "%sAMOUNT:%17s\n", pszIndent, szBuff);
		}
		PrnSetBig();
		MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		PrnSetNormal();
	
	}
}

void PrnAmount(uchar *pszIndent, uchar ucNeedSepLine)
{
	uchar	szBuff[50], szTotalAmt[12+1];
	uchar   szTempBuff[100];

	if( ChkIfNeedTip() &&
		!(glProcInfo.stTranLog.uiStatus &TS_VOID) )
	{
		//-------------------------------- BASE --------------------------------
		memset(szTempBuff, 0, sizeof(szTempBuff));
//		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, szBuff, 0);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, szBuff, GA_SEPARATOR);
		if (ChkIfThermalPrinter())
		{
			sprintf((char *)szTempBuff, _T("%sBASE      %17.17s\n"), pszIndent, szBuff);
		}
		else
		{
			sprintf((char *)szTempBuff, "%sBASE      %17.17s\n", pszIndent, szBuff);
		}
		MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);

		//-------------------------------- TIPS --------------------------------
		memset(szTempBuff, 0, sizeof(szTempBuff));
		if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
		{
//			App_ConvAmountTran(glProcInfo.stTranLog.szTipAmount, szBuff, 0);
			App_ConvAmountTran(glProcInfo.stTranLog.szTipAmount, szBuff, GA_SEPARATOR);
			if (ChkIfThermalPrinter())
			{
				sprintf((char *)szTempBuff, _T("%sTIPS      %17.17s\n"), pszIndent, szBuff);
			} 
			else
			{
				sprintf((char *)szTempBuff, "%sTIPS      %17.17s\n", pszIndent, szBuff);
			}
			MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		}
		else
		{
			if (ChkIfThermalPrinter())
			{
				sprintf((char *)szTempBuff, _T("%sTIPS\n"), pszIndent);
			} 
			else
			{
				sprintf((char *)szTempBuff, "%sTIPS\n", pszIndent);
			}
		}
		MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);

		//-------------------------------- TOTAL --------------------------------
		memset(szTempBuff, 0, sizeof(szTempBuff));
		if( ucNeedSepLine )
		{
			sprintf((char *)szTempBuff, "%s          -----------------\n", pszIndent);
			MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		}

		memset(szTempBuff, 0, sizeof(szTempBuff));
		if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
		{
			PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//			App_ConvAmountTran(szTotalAmt, szBuff, 0);
			App_ConvAmountTran(szTotalAmt, szBuff, GA_SEPARATOR);
			if (ChkIfThermalPrinter())
			{
				sprintf((char *)szTempBuff, _T("%sTOTAL     %17.17s\n"), pszIndent, szBuff);
			} 
			else
			{
				sprintf((char *)szTempBuff, "%sTOTAL     %17.17s\n", pszIndent, szBuff);
			}
		}
		else
		{
			if (ChkIfThermalPrinter())
			{
				sprintf((char *)szTempBuff, _T("%sTOTAL\n"), pszIndent);
			} 
			else
			{
				sprintf((char *)szTempBuff, "%sTOTAL\n", pszIndent);
			}
		}
		PrnSetBig();
		MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		PrnSetNormal();
	
		if( ucNeedSepLine )
		{
			memset(szTempBuff, 0, sizeof(szTempBuff));
			sprintf((char *)szTempBuff, "%s          =================\n", pszIndent);
			MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		}
	}
	else
	{
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, szBuff, GetTranAmountInfo(&glProcInfo.stTranLog));

		memset(szTempBuff, 0, sizeof(szTempBuff));
		if (ChkIfThermalPrinter())
		{
			sprintf((char *)szTempBuff, _T("%sTOTAL     %14.14s\n"), pszIndent, szBuff);
		} 
		else
		{
			sprintf((char *)szTempBuff, "%sTOTAL     %14.14s\n", pszIndent, szBuff);
		}
		PrnSetBig();
		MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		PrnSetNormal();

		if( ucNeedSepLine )
		{
			memset(szTempBuff, 0, sizeof(szTempBuff));
			sprintf((char *)szTempBuff, "%s          =================\n", pszIndent);
			MultiLngPrnStr(szTempBuff, DISP_LINE_LEFT);
		}
	}
}

// print product descriptor
void PrnDescriptor(void)
{
	uchar	ucCnt, ucMaxNum, ucTemp;
	uchar	szBuf[50];

	ucMaxNum = (uchar)MIN(MAX_GET_DESC, glProcInfo.stTranLog.ucDescTotal);
	for(ucCnt=0; ucCnt<ucMaxNum; ucCnt++)
	{
		ucTemp = glProcInfo.stTranLog.szDescriptor[ucCnt] - '0';
		PubASSERT( ucTemp<MAX_DESCRIPTOR );
		memset(szBuf, 0, sizeof(szBuf));
		sprintf((char *)szBuf, "   %-20.20s\n", glSysParam.stDescList[ucTemp].szText);
		MultiLngPrnStr(szBuf, DISP_LINE_LEFT);
	}

	if (!ChkIfThermalPrinter())
	{
		if( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) || ChkEdcOption(EDC_FREE_PRINT) )
		{
			PrnStr("\n");
		}
		else
		{
			for(; ucCnt<MAX_GET_DESC; ucCnt++)
			{
				PrnStr("\n");
			}
		}
	}
}

void PrnAdditionalPrompt(void)
{
	uchar	szBuf[50];

	if( !ChkAcqOption(ACQ_ADDTIONAL_PROMPT) && !ChkAcqOption(ACQ_AIR_TICKET) )
	{
		return;
	}
	memset(szBuf, 0, sizeof(szBuf));
	sprintf((char *)szBuf, "%-14.14s%16.16s\n", glSysParam.stEdcInfo.szAddlPrompt,
			glProcInfo.stTranLog.szAddlPrompt);
	MultiLngPrnStr(szBuf, DISP_LINE_LEFT);
}

void PrnStatement(void)
{
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INSTALMENT )
	{
		//
	}
}

// print total information of ucIssuerKey
int PrnTotalIssuer(uchar ucIssuerKey)
{
	uchar	ucIndex, szBuff[20];

#ifdef	__TELIUM_PARAM__
	for(ucIndex=0; ucIndex<glTlmSysParam.ucIssNum; ucIndex++)
	{
		if( glTlmSysParam.Iss[ucIndex].issID==ucIssuerKey )
		{
			break;
		}
	}
#else
	for(ucIndex=0; ucIndex<glSysParam.ucIssuerNum; ucIndex++)
	{
		if( glSysParam.stIssuerList[ucIndex].ucKey==ucIssuerKey )
		{
			break;
		}
	}
#endif
	memcpy(&glPrnTotal, &glIssuerTotal[ucIndex], sizeof(TOTAL_INFO));

	//if( ChkIfZeroTotal(&glPrnTotal) )
	//{
		//masuk sini nih
	//	return 0;
	//}
	
	PrnStr("---------------------------\n");
#ifdef	__TELIUM_PARAM__
	ConvIssuerName(glTlmSysParam.Iss[ucIndex].issRef, szBuff);
#else
	ConvIssuerName(glSysParam.stIssuerList[ucIndex].szName, szBuff);
#endif
	if( ChkIfThermalPrinter() )
	{
		PrnStr("ISSUER  : %-10.10s\n", szBuff);
	}
	else
	{
		PrnStr("  ISSUER  : %-10.10s\n", szBuff);
	}

	PrnTotalInfo(&glPrnTotal);

	return 0;
}
\
// print total information
void PrnTotalInfo(void *pstInfo)
{
	uchar		szBuff[50];
	uchar		szBuffSum[12+1];
	TOTAL_INFO	*pstTotal;

	PrnStr("    TRANSACTION     COUNT       AMOUNT\n");
	
	pstTotal = (TOTAL_INFO *)pstInfo;
#ifdef BANK_BUKOPIN
	App_ConvAmountTran(pstTotal->szSaleOnlineAmt, szBuff, GA_SEPARATOR);
	PrnStr("    SALE            %03d        %17s\n", pstTotal->uiSaleOnlineCnt, szBuff);
	
	App_ConvAmountTran(pstTotal->szVoidSaleOnlineAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
	PrnStr("    VOID SALE       %03d        %17s\n", pstTotal->uiVoidSaleOnlineCnt, szBuff);

	/*
	App_ConvAmountTran(pstTotal->szSaleOfflineAmt, szBuff, GA_SEPARATOR);
	PrnStr("    OFFLINE         %03d        %17s\n", pstTotal->uiSaleOfflineCnt, szBuff);
	
	App_ConvAmountTran(pstTotal->szVoidSaleOfflineAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
	PrnStr("    VOID OFFLINE    %03d        %17s\n", pstTotal->uiVoidSaleOfflineCnt, szBuff);
	*/
////*//	
#endif
/*
	App_ConvAmountTran(pstTotal->szRefundAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
	PrnStr("    REFUND          %03d        %17s\n", pstTotal->uiRefundCnt, szBuff);
	
	App_ConvAmountTran(pstTotal->szVoidRefundAmt, szBuff, GA_SEPARATOR);
	PrnStr("    VOID REFUND     %03d        %17s\n", pstTotal->uiVoidRefundCnt, szBuff);
	*/

#ifdef BANK_BUKOPIN
/////*/
	App_ConvAmountTran(pstTotal->szSaleAmt, szBuff, GA_SEPARATOR);
	PrnStr("------------------------------------------------\n");
	PrnStr("    TOTAL           %03d        %17s\n", pstTotal->uiSaleCnt, szBuff);
	/////PrnStr("TOTAL SALE          %03d        %17s\n", pstTotal->uiSaleCnt, szBuff);
	//PrnStr("\n");

	PubAscAdd(pstTotal->szVoidSaleOnlineAmt,pstTotal->szVoidSaleOfflineAmt,12,szBuffSum);
	App_ConvAmountTran(szBuffSum, szBuff, GA_NEGATIVE|GA_SEPARATOR);
	//PrnStr("------------------------------------------------\n");
	PrnStr("    TOTAL VOID      %03d        %17s\n", (pstTotal->uiVoidSaleOnlineCnt+pstTotal->uiVoidSaleOfflineCnt), szBuff);
	/////PrnStr("TOTAL VOID SALE     %03d        %17s\n", (pstTotal->uiVoidSaleOnlineCnt+pstTotal->uiVoidSaleOfflineCnt), szBuff);
	PrnStr("\n");
#else
	App_ConvAmountTran(pstTotal->szSaleNonManualAmt, szBuff, GA_SEPARATOR);
	PrnStr("    SALE            %03d        %17s\n", pstTotal->uiSaleNonManualCnt, szBuff);
	
	App_ConvAmountTran(pstTotal->szVoidSaleNonManualAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
	PrnStr("    VOID SALE       %03d        %17s\n", pstTotal->uiVoidSaleNonManualCnt, szBuff);
	
	App_ConvAmountTran(pstTotal->szSaleManualAmt, szBuff, GA_SEPARATOR);
	PrnStr("    MANUAL          %03d        %17s\n", pstTotal->uiSaleManualCnt, szBuff);
	
	App_ConvAmountTran(pstTotal->szVoidSaleManualAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
	PrnStr("    VOID MANUAL     %03d        %17s\n", pstTotal->uiVoidSaleManualCnt, szBuff);
#endif



}

// print total information
void PrnTotalInfo2(void *pstInfo)
{
	uchar		szBuff[50];
	TOTAL_INFO	*pstTotal;
	uchar		SumAmt[12+1]; ushort SumCnt;

	PrnStr("   TRANSACTION    COUNT               AMOUNT\n");
	
	pstTotal = (TOTAL_INFO *)pstInfo;
#ifdef BANK_BUKOPIN
	if(pstTotal->uiSaleOnlineCnt>0)
	{
		App_ConvAmountTran(pstTotal->szSaleOnlineAmt, szBuff, GA_SEPARATOR);
		PrnStr("    SALE            %03d        %17s\n", pstTotal->uiSaleOnlineCnt, szBuff);
	}
	if(pstTotal->uiVoidSaleOnlineCnt>0)
	{
		App_ConvAmountTran(pstTotal->szVoidSaleOnlineAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
		PrnStr("    VOID SALE       %03d        %17s\n", pstTotal->uiVoidSaleOnlineCnt, szBuff);
	}
	if(pstTotal->uiSaleOfflineCnt>0)
	{
		App_ConvAmountTran(pstTotal->szSaleOfflineAmt, szBuff, GA_SEPARATOR);
		PrnStr("    OFFLINE         %03d        %17s\n", pstTotal->uiSaleOfflineCnt, szBuff);
	}
	if(pstTotal->uiVoidSaleOfflineCnt>0)
	{
		App_ConvAmountTran(pstTotal->szVoidSaleOfflineAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
		PrnStr("    VOID OFFLINE    %03d        %17s\n", pstTotal->uiVoidSaleOfflineCnt, szBuff);
	}

	if(pstTotal->uiRefundCnt>0)
	{
		App_ConvAmountTran(pstTotal->szRefundAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
		PrnStr("    REFUND          %03d        %17s\n", pstTotal->uiRefundCnt, szBuff);
	}
	if(pstTotal->uiVoidRefundCnt>0)
	{
		App_ConvAmountTran(pstTotal->szVoidRefundAmt, szBuff, GA_SEPARATOR);
		PrnStr("    VOID REFUND     %03d        %17s\n", pstTotal->uiVoidRefundCnt, szBuff);
	}
	
	if(pstTotal->uiSaleCnt>0)
	{
		App_ConvAmountTran(pstTotal->szSaleAmt, szBuff, GA_SEPARATOR);
		PrnStr("------------------------------------------------\n");
		PrnStr("    TOTAL           %03d        %17s\n", pstTotal->uiSaleCnt, szBuff);
		/////PrnStr("TOTAL SALE          %03d        %17s\n", pstTotal->uiSaleCnt, szBuff);
		//PrnStr("\n");
	}
	SumCnt=pstTotal->uiVoidSaleOnlineCnt + pstTotal->uiVoidSaleOfflineCnt;
	if(SumCnt>0)
	{
		PubAscAdd(pstTotal->szVoidSaleOnlineAmt,pstTotal->szVoidSaleOfflineAmt,12,SumAmt);
		App_ConvAmountTran(SumAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
		//PrnStr("------------------------------------------------\n");
		PrnStr("    TOTAL VOID      %03d        %17s\n", SumCnt, szBuff);
		/////PrnStr("TOTAL VOID SALE     %03d        %17s\n", SumCnt, szBuff);
		PrnStr("\n");
	}
#else
	if(pstTotal->uiSaleNonManualCnt>0)
	{
		App_ConvAmountTran(pstTotal->szSaleNonManualAmt, szBuff, GA_SEPARATOR);
		PrnStr("    SALE            %03d        %17s\n", pstTotal->uiSaleNonManualCnt, szBuff);
	}
	if(pstTotal->uiVoidSaleNonManualCnt>0)
	{
		App_ConvAmountTran(pstTotal->szVoidSaleNonManualAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
		PrnStr("    VOID SALE       %03d        %17s\n", pstTotal->uiVoidSaleNonManualCnt, szBuff);
	}
	if(pstTotal->uiSaleManualCnt>0)
	{
		App_ConvAmountTran(pstTotal->szSaleManualAmt, szBuff, GA_SEPARATOR);
		PrnStr("    MANUAL          %03d        %17s\n", pstTotal->uiSaleManualCnt, szBuff);
	}
	if(pstTotal->uiVoidSaleManualCnt>0)
	{
		App_ConvAmountTran(pstTotal->szVoidSaleManualAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
		PrnStr("    VOID MANUAL     %03d        %17s\n", pstTotal->uiVoidSaleManualCnt, szBuff);
	}
	if(pstTotal->uiRefundCnt>0)
	{
		App_ConvAmountTran(pstTotal->szRefundAmt, szBuff, GA_NEGATIVE|GA_SEPARATOR);
		PrnStr("    REFUND          %03d        %17s\n", pstTotal->uiRefundCnt, szBuff);
	}
	if(pstTotal->uiVoidRefundCnt>0)
	{
		App_ConvAmountTran(pstTotal->szVoidRefundAmt, szBuff, GA_SEPARATOR);
		PrnStr("    VOID REFUND     %03d        %17s\n", pstTotal->uiVoidRefundCnt, szBuff);
	}
#endif

}

int PrnTotalAcq(void)
{
	uchar	ucCnt;
	int		iRet;

	PrnInit();
	PrnSetNormal();
	
	PrnStep(30);
	PrnStr("  TRANS TOTALS BY CARD\n");
	PrnEngTime();
	PrnStep(20);
	if( ChkIfThermalPrinter() )
	{
#ifdef	__TELIUM_PARAM__
		PrnStr("TID: %s\nMID: %s\n", glCurTlmTmsAcq.acqTID, glCurTlmTmsAcq.acqMID);
		PrnStep(15);
		PrnStr("ACQUIRER: %-10.10s\n", glCurTlmTmsAcq.acqName);
#else
		PrnStr("TID: %s\nMID: %s\n", glCurAcq.szTermID, glCurAcq.szMerchantID);
		PrnStep(15);
		PrnStr("ACQUIRER: %-10.10s\n", glCurAcq.szName);
#endif
	}
	else
	{
#ifdef	__TELIUM_PARAM__
		PrnStr("%14.8s\n%21.15s\n", glCurTlmTmsAcq.acqTID, glCurTlmTmsAcq.acqMID);
		PrnStep(15);
		PrnStr("  ACQUIRER: %-10.10s\n", glCurTlmTmsAcq.acqName);
#else
		PrnStr("%14.8s\n%21.15s\n", glCurAcq.szTermID, glCurAcq.szMerchantID);
		PrnStep(15);
		PrnStr("  ACQUIRER: %-10.10s\n", glCurAcq.szName);
#endif		
	}

#ifdef	__TELIUM_PARAM__
	for(ucCnt=0; ucCnt<glTlmSysParam.ucIssNum; ucCnt++)
	{
		
		if( glCurTlmTmsAcq.sIssuerKey[ucCnt]!=INV_ISSUER_KEY )
		{
			PrnTotalIssuer(glCurTlmTmsAcq.sIssuerKey[ucCnt]);
		}
		if( (ucCnt%5)==4 )
		{
			iRet = StartPrinter();
			if( iRet!=0 )
			{
				return iRet;
			}
			PrnInit();
			PrnSetNormal();
		}
	}
#else
	for(ucCnt=0; ucCnt<glSysParam.ucIssuerNum; ucCnt++)
	{
		if( glCurAcq.sIssuerKey[ucCnt]!=INV_ISSUER_KEY )
		{
			PrnTotalIssuer(glCurAcq.sIssuerKey[ucCnt]);
		}
		if( (ucCnt%5)==4 )
		{
			iRet = StartPrinter();
			if( iRet!=0 )
			{
				return iRet;
			}
			PrnInit();
			PrnSetNormal();
		}
	}
#endif	
	PrnStr("   END  OF  TOTAL\n");
	PrnStr("%s", (ChkIfThermalPrinter() ? "\n\n\n\n\n\n" : "\f"));

	StartPrinter();
	return 0;
}

int PrnTotalByAcq(void)
{

	uchar	ucIdx;

	for(ucIdx=0;ucIdx<glTlmSysParam.ucAcqNum;ucIdx++)
	{
		SetCurAcq(ucIdx);

//		sprintf((char *)glProcInfo.szSettleMsg, "%s", glSysCtrl.stRePrnStlInfo.szSettleMsg[glCurTlmTmsAcq.ucIndex]);
		if(!glSysCtrl.stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex])
			continue;
		
		memcpy(&glTransTotal, &glSysCtrl.stRePrnStlInfo.stAcqTotal[glCurTlmTmsAcq.ucIndex], sizeof(TOTAL_INFO));
//		memcpy(glIssuerTotal, glSysCtrl.stRePrnStlInfo.stIssTotal[glCurTlmTmsAcq.ucIndex], sizeof(glIssuerTotal));
		glCurTlmTmsAcq.ulCurBatchNo = glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex];
		sprintf((char *)glCurTlmTmsAcq.acqCurBat, "%06lu", glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex]);

		if( ChkIfZeroTotal(&glTransTotal) )
		{
			continue;
		}
		
		//if(ChkIfBRICredit()||ChkIfBRIDebit())
		//{
			//PrnInit();//4212
			PrnSetNormal();
			PrnTidMid();
			PrnStr("HOST %s %s\n", glCurTlmTmsAcq.acqNII, glCurTlmTmsAcq.acqName);
			PrnStr("BATCH %06lu     CLOSED\n", glCurTlmTmsAcq.ulCurBatchNo);
			PrnTotalInfo(&glTransTotal);
			PrnStr("================================================\n");
			//StartPrinter();
		//}
	}
	return 0;
}

int PrnTotalByCard(void)
{
	uchar	ucIndex;

	PrnSetNormal();

	for(ucIndex=0; ucIndex<glTlmSysParam.ucBinNum; ucIndex++)
	{
		memset(&glPrnTotal, 0, sizeof(TOTAL_INFO));
		memcpy(&glPrnTotal, &glCardTotal[ucIndex], sizeof(TOTAL_INFO));

		if( ChkIfZeroTotal(&glPrnTotal) )
		{
			continue;
		}
	
		//MultiLngPrnStr(glTlmSysParam.Bin[ucIndex].binName, DISP_LINE_LEFT);
		PrnStr("%s\n",glTlmSysParam.Bin[ucIndex].binName);
		PrnTotalInfo2(&glPrnTotal);
	}
	return 0;
}


int PrintSettleNew(uchar ucPrnFlag)
{
	uchar	buff[50+1];

	memset(buff, 0, sizeof(buff));
	
	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	PrnSetBig();
	if( ucPrnFlag==PRN_REPRINT )
	{
		MultiLngPrnStr("REPRINT\n", DISP_LINE_CENTER);
	}

	MultiLngPrnStr("SETTLEMENT\n", DISP_LINE_CENTER);
	
	//TID MID
	//PrnTidMid();
	PrnDateTime();

	
	PrnStr("================================================\n");
	PrnStr("TOTAL BY ACQUIRER\n\n");
	//StartPrinter();
	PrnTotalByAcq();
	
	//PrnInit();//4212
	PrnSetNormal();
	PrnStr("TOTAL BY CARD\n\n");
	memcpy(&glCardTotal, &glSysCtrl.stRePrnStlInfo.stCardTotal, sizeof(glCardTotal));
	PrnTotalByCard();
	PrnStr("================================================\n");	
	PrnStr("\n");
	PrnSetBig();
	MultiLngPrnStr("SETTLEMENT CONFIRMED\n", DISP_LINE_CENTER);

	PrnStr("%s", ChkIfThermalPrinter() ? "\n\n\n\n\n\n\n\n" : "\f");
	StartPrinter();
	return 0;
}
	 

int PrintSettle(uchar ucPrnFlag)
{
//	ulong	ulInvoice;
	uchar	buff[50+1];

	memset(buff, 0, sizeof(buff));
	if( ucPrnFlag==PRN_NORMAL )
	{
		// save settle information for reprint
		glSysCtrl.stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex]    = TRUE;
		glSysCtrl.stRePrnStlInfo.ulSOC[glCurTlmTmsAcq.ucIndex]     = glSysCtrl.ulInvoiceNo;
		glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex] = glCurTlmTmsAcq.ulCurBatchNo;
		sprintf((char *)glSysCtrl.stRePrnStlInfo.szSettleMsg[glCurTlmTmsAcq.ucIndex], "%s", glProcInfo.szSettleMsg);
		memcpy(&glSysCtrl.stRePrnStlInfo.stAcqTotal[glCurTlmTmsAcq.ucIndex], &glAcqTotal[glCurTlmTmsAcq.ucIndex], sizeof(TOTAL_INFO));
//		memcpy(glSysCtrl.stRePrnStlInfo.stIssTotal[glCurTlmTmsAcq.ucIndex], glIssuerTotal, sizeof(glIssuerTotal));
		SaveRePrnStlInfo();
	}

	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	PrnSetBig();
	MultiLngPrnStr("SETTLEMENT\n", DISP_LINE_CENTER);
	
	//TID MID
	PrnTidMid();
	PrnDateTime();

	
	PrnStr("================================================\n");
	PrnStr("TOTAL BY ACQUIRER\n");
	PrnStr("HOST %s %s\n", glCurTlmTmsAcq.acqNII, glCurTlmTmsAcq.acqName);
	PrnStr("BATCH %06lu     CLOSED\n", glCurTlmTmsAcq.ulCurBatchNo);
	PrnStr("%s\n", glProcInfo.szSettleMsg);
	PrnStr("\n");
	PrnTotalInfo(&glTransTotal);
	PrnStr("\n");
	PrnTotalByCard();

	PrnSetBig();
	MultiLngPrnStr("SETTLEMENT CONFIRMED\n", DISP_LINE_CENTER);

	PrnSetNormal();
	if( ucPrnFlag==PRN_REPRINT )
	{
		PrnStr(_T("       REPRINT\n"));
	}
	PrnStr("%s", ChkIfThermalPrinter() ? "\n\n\n\n\n\n\n\n" : "\f");

	return StartPrinter();
}

void PrnEngTime(void)
{
	uchar	szDateTime[14+1], szBuff[30];

	GetDateTime(szDateTime);
	Conv2EngTime(szDateTime, szBuff);
	PrnStr("%s\n", szBuff);
}

// 打印终端参数
// Print parameter
void PrintParam(void)
{
	uchar	ucCnt;

	PubShowTitle(TRUE, (uchar *)_T("PRINT PARAMETER"));
	if( PasswordTerm()!=0 )
	{
		return;
	}

#if defined(_P60_S1_) || defined(_S_SERIES_)
	SetOffBase(OffBaseCheckPrint);
#endif

	PubShowTitle(TRUE, (uchar *)_T("PRINT PARAMETER"));

	PrnInit();
	PrnSetNormal();

#ifdef	__TELIUM_PARAM__
	PrnStr("\n\n%.23s\n", glTlmSysParam.stEdcInfo.szMerchantName);
	PrnStr("%.23s\n",     glTlmSysParam.stEdcInfo.szMerchantAddr);
	PrnStr("%.23s\n",     glTlmSysParam.stEdcInfo.szMerchantAddr+23);
	PrnStr("INIT TID: %.8s\n", glTlmSysParam.stEdcInfo.szDownLoadTID);
	PrnHexString("EDC OPTION:", glTlmSysParam.stEdcInfo.sOption, 5, TRUE);

	PrnEngTime();
	PrnStr("APP VERSION: %s\n", EDC_VER_PUB);

	if (glTlmSysParam.ucDescNum)
	{
		PrnStr("DESCRIPTION:\n");
		for(ucCnt=0; ucCnt<glTlmSysParam.ucDescNum; ucCnt++)
		{
			PrnStr("   %.2s:%.20s\n", glTlmSysParam.stDescList[ucCnt].szCode,
					glTlmSysParam.stDescList[ucCnt].szText);
		}
	}

	PrnInstalmentPara();

	for(ucCnt=0; ucCnt<glTlmSysParam.ucAcqNum; ucCnt++)
	{
		if( PrnParaAcq(ucCnt)==0 )
		{
			if( StartPrinter()!=0 )
			{
				return;
			}
			PrnInit();
			PrnSetNormal();
			PrnStr("\n\n");
		}
	}
#else
	PrnStr("\n\n%.23s\n", glSysParam.stEdcInfo.szMerchantName);
	PrnStr("%.23s\n",     glSysParam.stEdcInfo.szMerchantAddr);
	PrnStr("%.23s\n",     glSysParam.stEdcInfo.szMerchantAddr+23);
	PrnStr("INIT TID: %.8s\n", glSysParam.stEdcInfo.szDownLoadTID);
	PrnHexString("EDC OPTION:", glSysParam.stEdcInfo.sOption, 5, TRUE);

	PrnEngTime();
	PrnStr("APP VERSION: %s\n", EDC_VER_PUB);

	if (glSysParam.ucDescNum)
	{
		PrnStr("DESCRIPTION:\n");
		for(ucCnt=0; ucCnt<glSysParam.ucDescNum; ucCnt++)
		{
			PrnStr("   %.2s:%.20s\n", glSysParam.stDescList[ucCnt].szCode,
					glSysParam.stDescList[ucCnt].szText);
		}
	}

	PrnInstalmentPara();

	for(ucCnt=0; ucCnt<glSysParam.ucAcqNum; ucCnt++)
	{
		if( PrnParaAcq(ucCnt)==0 )
		{
			if( StartPrinter()!=0 )
			{
				return;
			}
			PrnInit();
			PrnSetNormal();
			PrnStr("\n\n");
		}
	}
#endif
	
	if( StartPrinter()!=0 )
	{
		return;
	}

	if (!ChkIfEmvEnable())
	{
		return;
	}

#ifdef ENABLE_EMV
	PubShowTitle(TRUE, (uchar *)_T("PRINT PARAMETER"));
	PubDispString(_T("PRN EMV PARA ?"), 4|DISP_LINE_LEFT);
	if (PubYesNo(USER_OPER_TIMEOUT))
	{
		return;
	}

	PrnEmvPara();
#endif
}

int PrnParaAcq(uchar ucAcqIndex)
{
#ifdef	__TELIUM_PARAM__
	PubASSERT(glTlmSysParam.Acq[ucAcqIndex].acqID!=INV_ACQ_KEY);
	if( glTlmSysParam.Acq[ucAcqIndex].acqID==INV_ACQ_KEY )
	{
		return 1;
	}

	PrnStr("\n========================\n");
	PrnStr("%.3s  %.10s\n", glTlmSysParam.Acq[ucAcqIndex].acqNII,
			glTlmSysParam.Acq[ucAcqIndex].acqName);
	PrnStr("TID: %.8s\n",   glTlmSysParam.Acq[ucAcqIndex].acqTID);
	PrnStr("MID: %.15s\n",  glTlmSysParam.Acq[ucAcqIndex].acqMID);
	PrnStr("TXN TEL1 %.24s\n", glTlmSysParam.Acq[ucAcqIndex].acqPriTxnPhnSrv);
	PrnStr("TXN TEL2 %.24s\n", glTlmSysParam.Acq[ucAcqIndex].acqSecTxnPhnSrv);
	PrnStr("SET TEL1 %.24s\n", glTlmSysParam.Acq[ucAcqIndex].acqPriStlPhnSrv);
	PrnStr("SET TEL2 %.24s\n", glTlmSysParam.Acq[ucAcqIndex].acqSecStlPhnSrv);

	//tcp/ip
	if (strlen(glTlmSysParam.Acq[ucAcqIndex].acqPriTxnIP))
	{
		PrnStr("TCP/IP PARAM:\n");
		PrnStr("TXN 1: %.15s:%.5s\n",	glTlmSysParam.Acq[ucAcqIndex].acqPriTxnIP,
										glTlmSysParam.Acq[ucAcqIndex].acqPriTxnPort);
	}
	if (strlen(glTlmSysParam.Acq[ucAcqIndex].acqSecTxnIP))
	{
		PrnStr("TXN 2: %.15s:%.5s\n",	glTlmSysParam.Acq[ucAcqIndex].acqSecTxnIP,
										glTlmSysParam.Acq[ucAcqIndex].acqSecTxnPort);
	}

	if (strlen(glTlmSysParam.Acq[ucAcqIndex].acqPriStlIP))
	{
		PrnStr("TCP/IP PARAM:\n");
		PrnStr("TXN 1: %.15s:%.5s\n",	glTlmSysParam.Acq[ucAcqIndex].acqPriStlIP,
										glTlmSysParam.Acq[ucAcqIndex].acqPriStlPort);
	}
	if (strlen(glTlmSysParam.Acq[ucAcqIndex].acqSecTxnIP))
	{
		PrnStr("TXN 2: %.15s:%.5s\n",	glTlmSysParam.Acq[ucAcqIndex].acqSecStlIP,
										glTlmSysParam.Acq[ucAcqIndex].acqSecStlPort);
	}
#else
	PubASSERT(glSysParam.stAcqList[ucAcqIndex].ucKey!=INV_ACQ_KEY);
	if( glSysParam.stAcqList[ucAcqIndex].ucKey==INV_ACQ_KEY )
	{
		return 1;
	}

	PrnStr("\n========================\n");
	PrnStr("%.3s  %.10s\n", glSysParam.stAcqList[ucAcqIndex].szNii,
			glSysParam.stAcqList[ucAcqIndex].szName);
	PrnStr("TID: %.8s\n",   glSysParam.stAcqList[ucAcqIndex].szTermID);
	PrnStr("MID: %.15s\n",  glSysParam.stAcqList[ucAcqIndex].szMerchantID);
	PrnHexString("ACQ OPTION:", glSysParam.stAcqList[ucAcqIndex].sOption, 4, TRUE);
	PrnStr("TXN TEL1 %.24s\n", glSysParam.stAcqList[ucAcqIndex].TxnTelNo1);
	PrnStr("TXN TEL2 %.24s\n", glSysParam.stAcqList[ucAcqIndex].TxnTelNo2);
	PrnStr("SET TEL1 %.24s\n", glSysParam.stAcqList[ucAcqIndex].StlTelNo1);
	PrnStr("SET TEL2 %.24s\n", glSysParam.stAcqList[ucAcqIndex].StlTelNo2);

	//tcp/ip
	if (strlen(glSysParam.stAcqList[ucAcqIndex].stTxnTCPIPInfo[0].szIP))
	{
		PrnStr("TCP/IP PARAM:\n");
		PrnStr("TXN 1: %.15s:%.5s\n",	glSysParam.stAcqList[ucAcqIndex].stTxnTCPIPInfo[0].szIP,
										glSysParam.stAcqList[ucAcqIndex].stTxnTCPIPInfo[0].szPort);
	}
	if (strlen(glSysParam.stAcqList[ucAcqIndex].stTxnTCPIPInfo[1].szIP))
	{
		PrnStr("TXN 2: %.15s:%.5s\n",	glSysParam.stAcqList[ucAcqIndex].stTxnTCPIPInfo[1].szIP,
										glSysParam.stAcqList[ucAcqIndex].stTxnTCPIPInfo[1].szPort);
	}

	if (strlen(glSysParam.stAcqList[ucAcqIndex].stTxnGPRSInfo[0].szIP))
	{
		PrnStr("GPRS PARAM:\n");
		PrnStr("TXN 1: %.15s:%.5s\n",	glSysParam.stAcqList[ucAcqIndex].stTxnGPRSInfo[0].szIP,
										glSysParam.stAcqList[ucAcqIndex].stTxnGPRSInfo[0].szPort);
	}
	if (strlen(glSysParam.stAcqList[ucAcqIndex].stTxnGPRSInfo[1].szIP))
	{
		PrnStr("TXN 2: %.15s:%.5s\n",	glSysParam.stAcqList[ucAcqIndex].stTxnGPRSInfo[1].szIP,
										glSysParam.stAcqList[ucAcqIndex].stTxnGPRSInfo[1].szPort);
	}
#endif

	PrnParaIssuer(ucAcqIndex);

	return 0;
}

void PrnParaIssuer(uchar ucAcqIndex)
{
	uchar	ucCnt;

#ifdef	__TELIUM_PARAM__
	for(ucCnt=0; ucCnt<glTlmSysParam.ucIssNum; ucCnt++)
	{
		if( glTlmSysParam.Acq[ucAcqIndex].sIssuerKey[ucCnt]!=INV_ISSUER_KEY )
		{
			PrnParaIssuerSub(glTlmSysParam.Acq[ucAcqIndex].sIssuerKey[ucCnt]);
		}
	}
#else
	for(ucCnt=0; ucCnt<glSysParam.ucIssuerNum; ucCnt++)
	{
		if( glSysParam.stAcqList[ucAcqIndex].sIssuerKey[ucCnt]!=INV_ISSUER_KEY )
		{
			PrnParaIssuerSub(glSysParam.stAcqList[ucAcqIndex].sIssuerKey[ucCnt]);
		}
	}
#endif	
}

void PrnParaIssuerSub(uchar ucIssuerKey)
{
	uchar	ucCnt;
#ifndef	__TELIUM_PARAM__
	uchar szBuff[20];
#endif

	if( ucIssuerKey==INV_ISSUER_KEY )
	{
		return;
	}

#ifdef	__TELIUM_PARAM__
	for(ucCnt=0; ucCnt<glTlmSysParam.ucIssNum; ucCnt++)
	{
		if( glTlmSysParam.Iss[ucCnt].issID==ucIssuerKey )
		{
			break;
		}
	}
	PubASSERT( ucCnt<glTlmSysParam.ucIssNum );
	if( ucCnt>=glTlmSysParam.ucIssNum )
	{
		return;
	}

	PrnStr("---ISSUER: %-10.10s---\n", glTlmSysParam.Iss[ucCnt].issRef);

#else
	for(ucCnt=0; ucCnt<glSysParam.ucIssuerNum; ucCnt++)
	{
		if( glSysParam.stIssuerList[ucCnt].ucKey==ucIssuerKey )
		{
			break;
		}
	}
	PubASSERT( ucCnt<glSysParam.ucIssuerNum );
	if( ucCnt>=glSysParam.ucIssuerNum )
	{
		return;
	}

	PrnStr("---ISSUER: %-10.10s---\n", glSysParam.stIssuerList[ucCnt].szName);
	PrnHexString("ISSUER OPTION:", glSysParam.stIssuerList[ucCnt].sOption, 4, TRUE);
	PrnStr("PAN MASK: %02x %02x %02x\n",
			(uchar)(0xFF^glSysParam.stIssuerList[ucCnt].sPanMask[0]),
			(uchar)(0xFF^glSysParam.stIssuerList[ucCnt].sPanMask[1]),
			(uchar)(0xFF^glSysParam.stIssuerList[ucCnt].sPanMask[2]));

	sprintf((char *)szBuff, "%lu", glSysParam.stIssuerList[ucCnt].ulFloorLimit);
	PrnStr("FLOOR LIMIT: %s\n", szBuff);
	PrnStr("ADJUSTMENT PERCENT: %d%%\n", glSysParam.stIssuerList[ucCnt].ucAdjustPercent);

	PrnIssuerOption(glSysParam.stIssuerList[ucCnt].sOption);
#endif	
	PrnCardTable(ucIssuerKey);
}

void PrnIssuerOption(uchar *psOption)
{
	static	OPTION_INFO	stIssuerOptList[] =
	{
// 		{"ENABLE BALANCE?",		ISSUER_EN_BALANCE},
		{"ENABLE ADJUST",		ISSUER_EN_ADJUST,			FALSE,	PM_MEDIUM},
		{"ENABLE OFFLINE",		ISSUER_EN_OFFLINE,			FALSE,	PM_MEDIUM},
		{"EN. (PRE)AUTH",		ISSUER_NO_PREAUTH,			TRUE,	PM_MEDIUM},
		{"EN. REFUND",			ISSUER_NO_REFUND,			TRUE,	PM_MEDIUM},
		{"EN. VOID",			ISSUER_NO_VOID,				TRUE,	PM_MEDIUM},
// 		{"ENABLE EXPIRY",		ISSUER_EN_EXPIRY,			FALSE,	PM_MEDIUM},
// 		{"CHECK EXPIRY",		ISSUER_CHECK_EXPIRY,		FALSE,	PM_MEDIUM},
// 		{"CHKEXP OFFLINE",		ISSUER_CHECK_EXPIRY_OFFLINE,FALSE,	PM_MEDIUM},
// 		{"CHECK PAN",			ISSUER_CHKPAN_MOD10,		FALSE,	PM_MEDIUM},
// 		{"EN DISCRIPTOR",		ISSUER_EN_DISCRIPTOR,		FALSE,	PM_MEDIUM},
		{"ENABLE MANUAL",		ISSUER_EN_MANUAL,			FALSE,	PM_MEDIUM},
// 		{"ENABLE PRINT",		ISSUER_EN_PRINT,			FALSE,	PM_MEDIUM},
		{"VOICE REFERRAL",		ISSUER_EN_VOICE_REFERRAL,	FALSE,	PM_MEDIUM},
// 		{"PIN REQUIRED",		ISSUER_EN_PIN,				FALSE,	PM_MEDIUM},
// 		{"ACCOUNT SELECT",		ISSUER_EN_ACCOUNT_SELECTION,FALSE,	PM_MEDIUM},
// 		{"ROC INPUT REQ",		ISSUER_ROC_INPUT_REQ,		FALSE,	PM_MEDIUM},
// 		{"DISP AUTH CODE",		ISSUER_AUTH_CODE,			FALSE,	PM_MEDIUM},
// 		{"ADDTIONAL DATA",		ISSUER_ADDTIONAL_DATA,		FALSE,	PM_MEDIUM},
		{"SECURITY CODE",		ISSUER_SECURITY_SWIPE,		FALSE,	PM_MEDIUM},
		{"SECU. CODE MANUL",	ISSUER_SECURITY_MANUL,		FALSE,	PM_MEDIUM},
		{NULL, 0, FALSE, PM_MEDIUM},
	};
	uchar	ucCnt;

	for(ucCnt=0; stIssuerOptList[ucCnt].pText!=NULL; ucCnt++)
	{
		if( (!stIssuerOptList[ucCnt].ucInverseLogic && ChkOptionExt(psOption, stIssuerOptList[ucCnt].uiOptVal)) ||
			(stIssuerOptList[ucCnt].ucInverseLogic && !ChkOptionExt(psOption, stIssuerOptList[ucCnt].uiOptVal)) )
		{
			PrnStr("    %-16.16s[ on]\n", stIssuerOptList[ucCnt].pText);
		}
		else
		{
			PrnStr("    %-16.16s[off]\n", stIssuerOptList[ucCnt].pText);
		}
	}
}

void PrnCardTable(uchar ucIssuerKey)
{
	uchar	ucCnt, szBuff[30];

	if( ucIssuerKey==INV_ISSUER_KEY )
	{
		return;
	}

#ifdef	__TELIUM_PARAM__
	for(ucCnt=0; ucCnt<glTlmSysParam.ucBinNum; ucCnt++)
	{
		if( glTlmSysParam.Bin[ucCnt].binID==ucIssuerKey )
		{
			PubBcd2Asc0(glTlmSysParam.Bin[ucCnt].binLo+1, 5, szBuff);
			szBuff[10] = '~';
			PubBcd2Asc0(glTlmSysParam.Bin[ucCnt].binHi+1, 5, &szBuff[11]);
			PrnStr("%s\n", szBuff);
		}
	}
#else
	for(ucCnt=0; ucCnt<glSysParam.ucCardNum; ucCnt++)
	{
		if( glSysParam.stCardTable[ucCnt].ucIssuerKey==ucIssuerKey )
		{
			PubBcd2Asc0(glSysParam.stCardTable[ucCnt].sPanRangeLow, 5, szBuff);
			szBuff[10] = '~';
			PubBcd2Asc0(glSysParam.stCardTable[ucCnt].sPanRangeHigh, 5, &szBuff[11]);
			PrnStr("%s\n", szBuff);
		}
	}
#endif	
}

int PrnInstalmentPara(void)
{
	uchar	ucCnt, szBuff[20], szBuff1[50], ucAcqIndex;

	if( !ChkIfInstalmentPara() )
	{
		return 1;
	}

	PrnStr("\n========INSTALMENT=======\n");
	for(ucCnt=0; ucCnt<glSysParam.ucPlanNum; ucCnt++)
	{
		PubASSERT( glSysParam.stPlanList[ucCnt].ucIndex!=0xFF );
		PrnStr("%02d.%-7.7s - %dmths\n", ucCnt+1, glSysParam.stPlanList[ucCnt].szName,
				glSysParam.stPlanList[ucCnt].ucMonths);

		sprintf((char *)szBuff, "%lu", glSysParam.stPlanList[ucCnt].ulBottomAmt);
		App_ConvAmountLocal(szBuff, szBuff1, 0);
		ucAcqIndex = glSysParam.stPlanList[ucCnt].ucAcqIndex;
		PrnStr("%-10.10s  %s\n", glSysParam.stAcqList[ucAcqIndex].szName, szBuff1);
	}

	return 0;
}

#ifdef ENABLE_EMV
// 打印EMV参数
// Print EMV parameter
int PrnEmvPara(void)
{
	int			iRet, iCnt;
	EMV_APPLIST	stEmvApp;
	EMV_CAPK	stEmvCapk;

	PrnInit();
	PrnSetNormal();

	PrnStr("\n=========EMV PARAMETER=======\n");
	EMVGetParameter(&glEmvParam);
	PrnStr("TERMINAL TYPE: %02X\n", glEmvParam.TerminalType);
	PrnHexString("TERMINAL CAPA:",  glEmvParam.Capability, 3, TRUE);
	PrnHexString("TERM EX-CAPA :",  glEmvParam.ExCapability, 5, TRUE);
	PrnStr("TXN CURR EXP : %02X\n", glEmvParam.TransCurrExp);
	PrnStr("REF CURR EXP : %02X\n", glEmvParam.ReferCurrExp);
	PrnHexString("REF CURR CODE:", glEmvParam.ReferCurrCode, 2, TRUE);
	PrnHexString("COUNTRY CODE :", glEmvParam.CountryCode, 2, TRUE);
	PrnHexString("TXN CURR CODE:", glEmvParam.TransCurrCode, 2, TRUE);
	PrnStr("REF CURR CON : %ld\n", glEmvParam.ReferCurrCon);
	PrnStr("SELECT PSE   : %s\n",  glEmvParam.SurportPSESel ? "YES" : "NO");

	PrnStr("\n\n\n========EMV APP LIST=========\n");
	for(iCnt=0; iCnt<MAX_APP_NUM; iCnt++)
	{
		memset(&stEmvApp, 0, sizeof(EMV_APPLIST));
		iRet = EMVGetApp(iCnt, &stEmvApp);
		if( iRet!=EMV_OK )
		{
			continue;
		}
		PrnHexString("AID:",  stEmvApp.AID, (int)stEmvApp.AidLen, TRUE);
		PrnHexString("VERSION:",  stEmvApp.Version, 2, TRUE);
		PrnStr("SELECT FLAG   : %s MATCH\n", stEmvApp.SelFlag==FULL_MATCH ? "FULL" : "PARTIAL");
		PrnStr("PRIORITY      : %d\n", stEmvApp.Priority);
		PrnStr("TARGET PER    : %d\n", stEmvApp.TargetPer);
		PrnStr("MAX TARGET PER: %d\n", stEmvApp.MaxTargetPer);
		PrnStr("CHECK FLOOR   : %s\n", stEmvApp.FloorLimitCheck ? "YES" : "NO");
		PrnStr("RANDOM SELECT : %s\n", stEmvApp.RandTransSel    ? "YES" : "NO");
		PrnStr("CHECK VELOCITY: %s\n", stEmvApp.VelocityCheck   ? "YES" : "NO");
		PrnStr("FLOOR LIMIT   : %lu\n", stEmvApp.FloorLimit);
		PrnStr("THRESHOLD     : %lu\n", stEmvApp.Threshold);
		PrnHexString("TAC DENIAL :",  stEmvApp.TACDenial,  5, TRUE);
		PrnHexString("TAC ONLINE :",  stEmvApp.TACOnline,  5, TRUE);
		PrnHexString("TAC DEFAULT:",  stEmvApp.TACDefault, 5, TRUE);
		PrnStr("-----------------------------\n");
		if( (iCnt%5)==0 )
		{
			if( StartPrinter()!=0 )
			{
				return 1;
			}
			PrnInit();
			PrnSetNormal();
		}
	}

	PrnStr("\n\n=========EMV CAPK LIST========\n");
	for(iCnt=0; iCnt<MAX_KEY_NUM; iCnt++)
	{
		memset(&stEmvCapk, 0, sizeof(EMV_CAPK));
		iRet = EMVGetCAPK(iCnt, &stEmvCapk);
		if( iRet!=EMV_OK )
		{
			continue;
		}
		PrnHexString("RID:",  stEmvCapk.RID, 5, FALSE);
		PrnStr(" ID: %02X\n",  stEmvCapk.KeyID);
// 		PrnStr("HASH   : %02X\n",  stEmvCapk.HashInd);
// 		PrnStr("ARITH  : %02X\n",  stEmvCapk.ArithInd);
		PrnHexString("EXP DATE:",  stEmvCapk.ExpDate, 3, TRUE);
		PrnStr("MOD LEN: %d ",  (int)(8 * stEmvCapk.ModulLen));
		PrnHexString("EXPONENT:",  stEmvCapk.Exponent, (int)stEmvCapk.ExponentLen, TRUE);
		PrnStr("-----------------------------\n");
		if( (iCnt%5)==0 )
		{
// 			PrnStr("\f");
			if( StartPrinter()!=0 )
			{
				return 1;
			}
			PrnInit();
			PrnSetNormal();
		}
	}
	PrnStr("\f");
	return StartPrinter();
}
#endif


void PrnHexString(char *pszTitle, uchar *psHexStr, int iLen, uchar bNewLine)
{
	int		iCnt;

	PrnStr("%s", pszTitle);
	for(iCnt=0; iCnt<iLen; iCnt++)
	{
		PrnStr(" %02X", psHexStr[iCnt]);
	}
	if (bNewLine)
	{
		PrnStr("\n");
	}
}

// 打印错误提示
// Start-up printer, and show error if any.
int StartPrinterNoLogo(void)
{
	uchar	ucRet, ucKey;

	if (!ChkIfIrDAPrinter())
	{
		while( 1 )
		{
			ucRet = PrnStart();
			if( ucRet==PRN_OK )
			{
				return 0;	// print success!
			}
			ScrCls();
			DispPrnError(ucRet);
			if( ucRet!=PRN_NO_PAPER )
			{
				PubWaitKey(3);
				break;
			}

			ucKey = PubWaitKey(USER_OPER_TIMEOUT);
			if( ucKey==KEYCANCEL || ucKey==NOKEY )
			{
				ScrCls();
				PubDispString(_T("PLEASE REPRINT"), 4|DISP_LINE_CENTER);
				PubWaitKey(2);
				break;
			}
		}
	}
	else
	{
#if defined(_P60_S1_) || defined(_S_SERIES_)
		SetOffBase(OffBaseCheckPrint);	//????
#endif
		PrnStart();
		while( 1 )
		{
			ucRet = PrnStatus();
			if( ucRet==PRN_OK || ucRet==PRN_CANCEL )
			{
				return (ucRet==PRN_OK ? 0 : 1);
			}
			else if( ucRet==PRN_BUSY )
			{
				DelayMs(500);
				continue;
			}
			ScrCls();
			DispPrnError(ucRet);
			if( ucRet!=PRN_NO_PAPER )
			{
				PubWaitKey(3);
				break;
			}

			ucKey = PubWaitKey(USER_OPER_TIMEOUT);
			if( ucKey==KEYCANCEL || ucKey==NOKEY )
			{
				break;
			}
			PrnStart();
		}
	}

	return ERR_NO_DISP;
}


// 打印错误提示
// Start-up printer, and show error if any.
int StartPrinter(void)
{
	uchar	ucRet, ucKey;

	if (!ChkIfIrDAPrinter())
	{
		while( 1 )
		{
			DispPrinting();
			PrintOne();
			ucRet = PrnStart();
			if( ucRet==PRN_OK )
			{
				return 0;	// print success!
			}
			ScrClrLine(2, 7);
			DispPrnError(ucRet);
			if( ucRet!=PRN_NO_PAPER )
			{
				PubWaitKey(3);
				break;
			}

			ucKey = PubWaitKey(USER_OPER_TIMEOUT);
			if( ucKey==KEYCANCEL || ucKey==NOKEY )
			{
				ScrClrLine(4, 7);
				PubDispString(_T("PLEASE REPRINT"), 4|DISP_LINE_CENTER);
				PubWaitKey(2);
				break;
			}
		}
	}
	else
	{
#if defined(_P60_S1_) || defined(_S_SERIES_)
		SetOffBase(OffBaseCheckPrint);	//????
#endif
		DispPrinting();
		PrnStart();
		PrintOne();
		while( 1 )
		{
			ucRet = PrnStatus();
			if( ucRet==PRN_OK || ucRet==PRN_CANCEL )
			{
				return (ucRet==PRN_OK ? 0 : 1);
			}
			else if( ucRet==PRN_BUSY )
			{
				DelayMs(500);
				continue;
			}
			ScrClrLine(2, 7);
			DispPrnError(ucRet);
			if( ucRet!=PRN_NO_PAPER )
			{
				PubWaitKey(3);
				break;
			}

			ucKey = PubWaitKey(USER_OPER_TIMEOUT);
			if( ucKey==KEYCANCEL || ucKey==NOKEY )
			{
				break;
			}
			DispPrinting();
			PrnStart();
			PrintOne();
		}
	}

	return ERR_NO_DISP;
}

void DispPrnError(uchar ucErrCode)
{
	switch( ucErrCode )
	{
	case PRN_BUSY:
		PubDispString(_T("PRINTER BUSY"), 4|DISP_LINE_CENTER);
		break;

	case PRN_NO_PAPER:
		PubDispString(_T("OUT OF PAPER"), 4|DISP_LINE_CENTER);
		break;

	case PRN_DATA_ERR:
		PubDispString(_T("PRN DATA ERROR"), 4|DISP_LINE_CENTER);
		break;

	case PRN_ERR:
		PubDispString(_T("PRINTER ERROR"), 4|DISP_LINE_CENTER);
		break;

	case PRN_NO_DOT:
		PubDispString(_T("FONT MISSING"), 4|DISP_LINE_CENTER);
		break;

	case PRN_DATA_BIG:
		PubDispString(_T("PRN OVERFLOW"), 4|DISP_LINE_CENTER);
		break;

	default:
		PubDispString(_T("PRINT FAILED"), 4|DISP_LINE_CENTER);
		break;
	}
	PubBeepErr();
}

/*
int PrnTest(void)
{
	PrnInit();
	PrnSetNormal();

	//...

	StartPrinter();
	return 0;
}
*/

void MultiLngPrnStr(uchar *str, uchar mode)
{
	char buffer[50+1];
	uchar ucLen=0;

	MEM_ZERO(buffer);
	ucLen = strlen(str);
	if(ucLen==0)
		return;

	if(str[ucLen-1]==0x0A)
		ucLen--;
	
#ifdef AREA_Arabia
	if(strcmp(LANGCONFIG, "Arabia") == 0)
	{
		if (mode == DISP_LINE_LEFT)
		{
			mode = SHIFT_LEFT;
		}
		else if (mode == DISP_LINE_RIGHT)
		{
			mode = SHIFT_RIGHT;
		}
		else if (mode == DISP_LINE_CENTER)
		{
			mode = SHIFT_CENTER;
		}
		ArPrintString(mode, str);
	}
	else
	{
		PrnStr(str);
	}
#else
	if(flagSizeFont==NORMAL)
	{
		if(ucLen<48)
		{
			if(mode==DISP_LINE_RIGHT)
			{
				memset(buffer, 0x20, (48-strlen(str)));
				memcpy(buffer+(48-strlen(str)), str, strlen(str));
				PrnStr(buffer);
			}
			else if(mode == DISP_LINE_CENTER)
			{
				memset(buffer, 0x20, ((47-strlen(str))/2));
				memcpy(buffer+((47-strlen(str))/2), str, strlen(str));
				PrnStr(buffer);
			}
			else
				PrnStr(str);
		}
		else
			PrnStr(str);
	}
	else if(flagSizeFont==SMALL)
	{
		if(strlen(str)<42)
		{
			if(mode==DISP_LINE_RIGHT)
			{
				memset(buffer, 0x20, (41-strlen(str)));
				memcpy(buffer+(41-strlen(str)), str, strlen(str));
				PrnStr(buffer);
			}
			else if(mode == DISP_LINE_CENTER)
			{
				memset(buffer, 0x20, ((41-strlen(str))/2));
				memcpy(buffer+((41-strlen(str))/2), str, strlen(str));
				PrnStr(buffer);
			}
			else
				PrnStr(str);
		}
		else
			PrnStr(str);
	}
	else if(flagSizeFont==BIG)
	{
		if(ucLen<24)
		{
			if(mode==DISP_LINE_RIGHT)
			{
				memset(buffer, 0x20, (24-strlen(str)));
				memcpy(buffer+(24-strlen(str)), str, strlen(str));
				PrnStr(buffer);
			}
			else if(mode == DISP_LINE_CENTER)
			{
				memset(buffer, 0x20, ((24-ucLen)/2));
				memcpy(buffer+((24-ucLen)/2), str, strlen(str));
				PrnStr(buffer);
			}
			else
				PrnStr(str);
		}
		else
			PrnStr(str);
	}
	else
		PrnStr(str);

#endif
}
///////////////////////////////////////////////////////////////////////////////////////////////

void PrnHeader(void)
{
	uchar	szBuff[32];

//	PrnStep(15);
	PrnSetNormal();

#ifdef	__TELIUM_PARAM__
	memset(szBuff, 0, sizeof(szBuff));
	sprintf(szBuff, "%.23s\n", glTlmSysParam.Con.rptHdrSiteName);
	MultiLngPrnStr(szBuff, DISP_LINE_CENTER);

	if( glTlmSysParam.Con.rptHdrSiteAddr1[0]!=0)
	{
		memset(szBuff, 0, sizeof(szBuff));
		sprintf(szBuff, "%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr1);
		MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
	}
	if( glTlmSysParam.Con.rptHdrSiteAddr2[0]!=0)
	{
		memset(szBuff, 0, sizeof(szBuff));
		sprintf(szBuff, "%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr2);
		MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
	}
	if( glTlmSysParam.Con.rptHdrSiteAddr3[0]!=0)
	{
		memset(szBuff, 0, sizeof(szBuff));
		sprintf(szBuff, "%.23s\n", glTlmSysParam.Con.rptHdrSiteAddr3);
		MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
	}
#else
	memset(szBuff, 0, sizeof(szBuff));
	sprintf(szBuff, "%.23s\n", glSysParam.stEdcInfo.szMerchantName);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStep(10);

	if (strlen(glSysParam.stEdcInfo.szMerchantAddr)>23)
	{
		memset(szBuff, 0, sizeof(szBuff));
		sprintf(szBuff, "%.23s\n", glSysParam.stEdcInfo.szMerchantAddr);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		sprintf(szBuff, "%.23s", glSysParam.stEdcInfo.szMerchantAddr+23);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	}
	else
	{
		PrnStr(glSysParam.stEdcInfo.szMerchantAddr);
		PrnStr("\n");
	}
#endif		
	PrnSetNormal();
	PrnStr("\n");
//	PrnStep(15);
}

void PrnTid(void)
{
	char szBuff[50+1];
	char unmask[30+1];
		
	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
	
#ifdef	__TELIUM_PARAM__
	MaskPan(glCurTlmTmsAcq.acqTID,unmask);
#else
	MaskPan(glCurAcq.acqTID,unmask);
#endif
	sprintf(szBuff,"TERMINAL ID :%30s\n", unmask);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}

void PrnMid(void)
{
	char szBuff[50+1];
	char unmask[30+1];
	
	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
	memset(unmask, 0,sizeof(unmask));
#ifdef	__TELIUM_PARAM__
	memcpy(unmask,glCurTlmTmsAcq.acqMID,lenMid);
#else
	memcpy(unmask,glCurAcq.acqMID,lenMid);
#endif
	memset(unmask+5,'*',5);
	sprintf(szBuff,"MERCHANT ID :%30s\n", unmask);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStep(10);

}

void PrnTidMid(void)
{
	char szBuff[50+1];
	char unmaskTID[30+1];
	char unmaskMID[30+1];
	
	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
	
	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
	memset(unmaskMID, 0,sizeof(unmaskMID));
#ifdef	__TELIUM_PARAM__
	MaskPan(glCurTlmTmsAcq.acqTID,unmaskTID);
	memcpy(unmaskMID,glCurTlmTmsAcq.acqMID,lenMid);
#else
	MaskPan(glCurAcq.acqTID,unmaskTID);
	memcpy(unmaskMID,glCurAcq.acqMID,lenMid);
#endif
	memset(unmaskMID+5,'*',5);
	sprintf(szBuff,"TID : %-21s MID: %15s\n", unmaskTID, unmaskMID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStep(10);
}

void PrnCardType(void)
{
	char szBuff[50+1];

	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char *)szBuff, "CARD TYPE: %s\n", glProcInfo.stTranLog.szCardType);
	MultiLngPrnStr(szBuff,DISP_LINE_LEFT);
}

void PrnCardNumber(void)
{
	char szBuff[50+1];
	char szBuf1[50+1];
//	char PAN[19+1];

	PrnSetNormal();
	if (!glCurTlmTmsIss.issFullPAN)
	{
		MaskPan(glProcInfo.stTranLog.szPan, szBuff);
	}
	else
	{
		strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	}

	if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT )
	{
		sprintf(szBuf1, "%s -SWIPE", szBuff);
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		sprintf(szBuf1, "%s -DIP", szBuff);
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		sprintf(szBuf1, "%s -FALLBACK", szBuff);
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CONTACTLESS )
	{
		sprintf(szBuf1, "%s -TAP", szBuff);
	}
	else
	{
		sprintf(szBuf1, "%s-MANUAL", szBuff);
	}
		
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char *)szBuff, "%-23.23s\n", szBuf1);
	MultiLngPrnStr(szBuff,DISP_LINE_LEFT);
}

void PrnCardHolderName(void)
{
	uchar szBuff[50+1];
	
	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char *)szBuff, "%-23.23s\n", glProcInfo.stTranLog.szHolderName);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}

void PrnCardNumberBig(void)
{
	char szBuff[50+1];
	char szBuf1[50+1];

	PrnSetBig();
	if (!glCurTlmTmsIss.issFullPAN)
	{
		MaskPan(glProcInfo.stTranLog.szPan, szBuff);
	}
	else
	{
		strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	}

	if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT )
	{
		sprintf(szBuf1, "%s -SWIPE", szBuff);
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		sprintf(szBuf1, "%s (CHIP)", szBuff);
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		sprintf(szBuf1, "%s -FALLBACK", szBuff);
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CONTACTLESS )
	{
		sprintf(szBuf1, "%s -TAP", szBuff);
	}
	else
	{
		sprintf(szBuf1, "%s-MANUAL", szBuff);
	}
		
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char *)szBuff, "%-23.23s\n", szBuf1);
	MultiLngPrnStr(szBuff,DISP_LINE_LEFT);
}

void PrnCardHolderNameBig(void)
{
	uchar szBuff[50+1];
	
	PrnSetBig();
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char *)szBuff, "%-23.23s\n", glProcInfo.stTranLog.szHolderName);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}

void PrnTransactionName(void)
{
	uchar szTranName[50+1];
	uchar szBuff[50+1];
	
	PrnSetBig();
	sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel);
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID || (glProcInfo.stTranLog.uiStatus & TS_VOID) )
	{
		sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucOrgTranType].szLabel);
//		sprintf((char *)szBuff, "%s(%s)\n", _T(szTranName), _T("VOID"));
		sprintf((char *)szBuff, "%s%s\n", _T("VOID "), _T(szTranName));
	}
	else if( glProcInfo.stTranLog.uiStatus & TS_ADJ )
	{
		sprintf((char *)szBuff, "%s(%s)\n", _T(szTranName), _T("ADJ"));
	}
	else
	{
		sprintf((char *)szBuff, "%s\n", szTranName);
	}
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}

void PrnExpDate(void)
	{
		uchar szBuff[50+1];
		
		PrnSetNormal();
		sprintf((char *)szBuff, "EXPY DATE : %2.2s/%2.2s\n",(char *)glProcInfo.stTranLog.szExpDate+2,(char *)glProcInfo.stTranLog.szExpDate);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}

void PrnBatchInvoice(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	PrnSetNormal();
	sprintf((char *)szBuff, "BATCH: %-24.06lu TRACE NO: %06lu\n", 
		glCurTlmTmsAcq.ulCurBatchNo, glProcInfo.stTranLog.ulInvoiceNo);

	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}

void PrnInvoiceBatch(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	PrnSetNormal();
	sprintf((char *)szBuff, "TRACE NO: %-23.06lu BATCH#: %06lu\n", 
		glProcInfo.stTranLog.ulInvoiceNo, glCurTlmTmsAcq.ulCurBatchNo);

	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}


void PrnDateTime(void)
{
	uchar szBuff[50+1];
	uchar szBuf1[50+1];

	MEM_ZERO(szBuff);
	MEM_ZERO(szBuf1);
	
	PrnSetNormal();
	Conv2EngTime(glProcInfo.stTranLog.szDateTime, szBuf1);
	//DD_MMM_YYYY__hh:mm:ss    
	sprintf((char *)szBuff, "DATE: %-27.11s TIME: %8s\n", szBuf1, szBuf1+13);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}

void PrnRefNumApprCode(void)
{
	uchar szBuff[50+1];
	
	MEM_ZERO(szBuff);
	PrnSetNormal();
	sprintf((char *)szBuff, "REF NO: %-22.12s APPR CODE: %6s\n",  
		glProcInfo.stTranLog.szRRN, glProcInfo.stTranLog.szAuthCode);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}

void PrnAppEMV(void)
{
	uchar szBuf1[50+1];

	PrnSetNormal();

	memset(szBuf1, 0, sizeof(szBuf1));
	sprintf((char *)szBuf1, "APP:%.16s\n", glProcInfo.stTranLog.szAppLabel);
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

}

void PrnAPPLABEL(void)
{
	uchar szBuf1[50+1];

	PrnSetNormal();
	
	memset(szBuf1, 0, sizeof(szBuf1));
	sprintf((char *)szBuf1, "APP LBL   : %.16s\n", glProcInfo.stTranLog.szAppLabel); 
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
}

void PrnAPPPreferName(void)
{
	uchar szBuf1[50+1];

	PrnSetNormal();
	
	memset(szBuf1, 0, sizeof(szBuf1));
	sprintf((char *)szBuf1, "APP       : %.16s\n", glProcInfo.stTranLog.szAppPreferName); 
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
}
void PrnAIDEMV(void)
{
	uchar szBuff[50+1];
	uchar szBuf1[50+1];

	PrnSetNormal();

	PubBcd2Asc0(glProcInfo.stTranLog.sAID, glProcInfo.stTranLog.ucAidLen, szBuff);
	PubTrimTailChars(szBuff, 'F');
	memset(szBuf1, 0, sizeof(szBuf1));
	sprintf((char *)szBuf1, "APPL ID   : %.32s\n", szBuff);
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
}

void PrnTCEMV(void)
{
	uchar szBuff[50+1];
	uchar szBuf1[50+1];

	PrnSetNormal();
	
	PubBcd2Asc0(glProcInfo.stTranLog.sAppCrypto, 8, szBuff);
	memset(szBuf1, 0, sizeof(szBuf1));
	sprintf((char *)szBuf1, "APP CRYPT : %.16s\n", szBuff);
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
}

void PrnTVREMV(void)
{
	uchar szBuff[50+1];
	uchar szBuf1[50+1];

	PrnSetNormal();
	
	PubBcd2Asc0(glProcInfo.stTranLog.sTVR, 5, szBuff);
	memset(szBuf1, 0, sizeof(szBuf1));
	sprintf((char *)szBuf1, "TVR VALUE : %.10s\n", szBuff);
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
}

void PrnTSI(void)
{	
	uchar DataOut[128];
	int OutLen;
	uchar szBuff[50];
	uchar szBuf1[50];
	uchar	ucSW1=0, ucSW2=0;

		//PrnInit();
	PrnSetNormal();
	memset(DataOut, 0, sizeof(DataOut));
	memset(szBuff, 0, sizeof(szBuff));
	OutLen=0;
	EMVGetTLVData(0x9B,DataOut, &OutLen);
//	hex_dump_char(0x9F40, DataOut, OutLen);
	PubBcd2Asc0(DataOut, OutLen, szBuff);
	memset(szBuf1, 0, sizeof(szBuf1));
	sprintf((char *)szBuf1, "TSI : %.4s\n", szBuff);
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

}
void PrnPinSignature(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	
	PrnSetNormal();

	if(( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )|| ( glProcInfo.stTranLog.uiEntryMode & MODE_OFF_PIN))
	{
		MultiLngPrnStr("*** PIN VERIFICATION SUCCESS ***\n", DISP_LINE_CENTER);
	}
	else
	{
		PrnStr("\n\n\n\n");
		PrnStr("SIGN X------------------------------------------\n");
		memset(szBuff, 0, sizeof(szBuff));
		//sprintf((char *)szBuff, "%s\n", glProcInfo.stTranLog.szHolderName); close cung
		//MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
	}
}		

void PrnPinNoSignature(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	
	PrnSetNormal();
	MultiLngPrnStr("NO SIGNATURE REQUIRED\n", DISP_LINE_CENTER);
					
	if(( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )|| ( glProcInfo.stTranLog.uiEntryMode & MODE_OFF_PIN))
	{
		MultiLngPrnStr("*** PIN VERIFICATION SUCCESS ***\n", DISP_LINE_CENTER);
	}
}			

void PrnFooterInformation(void)
{
	#ifdef BANK_HSBC
	MultiLngPrnStr("DENGAN MENANDATANGANI STRUK INI\n", DISP_LINE_CENTER);
	MultiLngPrnStr("SAYA MENYATAKAN SETUJU BAHWA\n", DISP_LINE_CENTER);
	MultiLngPrnStr("TRANSAKSI INI DIUBAH MENJADI\n", DISP_LINE_CENTER);
	MultiLngPrnStr("TRANSAKSI CICILAN RINGAN HSBC\n", DISP_LINE_CENTER); 
	MultiLngPrnStr("DENGAN SYARAT DAN KETENTUAN BERLAKU\n", DISP_LINE_CENTER);
	#else
	MultiLngPrnStr("I AGREE TO PAY ABOVE TOTAL AMOUNT\n", DISP_LINE_CENTER);
	MultiLngPrnStr("ACCORDING TO CARD ISSUER AGREEMENT\n", DISP_LINE_CENTER);
	#endif
}
void PrnAgenCopy(void)
{
	uchar szBuff[50+1];
	uchar SoftName[13+1];

	MEM_ZERO(SoftName);
	mapGet(traAppName, SoftName, 13);

	MEM_ZERO(szBuff);
	PrnSetNormal();
	
	//sprintf((char *)szBuff, "-- MERCHANT COPY --                %13s\n",  SoftName);
	//MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MultiLngPrnStr("--AGENT COPY--",DISP_LINE_CENTER);
//	PubDispString(_T("VERIFY SIGNATURE"), 2|DISP_LINE_CENTER);		
//	PubDispString(_T("MERCHANT COPY"), 4|DISP_LINE_CENTER);
//	PubDispString(_T("PRINTING"), 6|DISP_LINE_CENTER);
}

void PrnMerchantCopy(void)
{
	uchar szBuff[50+1];
	uchar SoftName[13+1];

	MEM_ZERO(SoftName);
	mapGet(traAppName, SoftName, 13);

	MEM_ZERO(szBuff);
	PrnSetNormal();
	
	//sprintf((char *)szBuff, "-- MERCHANT COPY --                %13s\n",  SoftName);
	//MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MultiLngPrnStr("--MERCHANT COPY--",DISP_LINE_CENTER);
//	PubDispString(_T("VERIFY SIGNATURE"), 2|DISP_LINE_CENTER);		
//	PubDispString(_T("MERCHANT COPY"), 4|DISP_LINE_CENTER);
//	PubDispString(_T("PRINTING"), 6|DISP_LINE_CENTER);
}

void PrnBankCopy(void)
{
	uchar szBuff[50+1];
	uchar SoftName[13+1];

	MEM_ZERO(SoftName);
	mapGet(traAppName, SoftName, 13);

	MEM_ZERO(szBuff);
	PrnSetNormal();
	//sprintf((char *)szBuff, "-- BANK COPY --                    %13s\n",  SoftName);
	//MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MultiLngPrnStr("--BANK COPY--",DISP_LINE_CENTER);
}

void PrnCustomerCopy(void)
{
	uchar szBuff[50+1];
	uchar SoftName[13+1];

	MEM_ZERO(SoftName);
	mapGet(traAppName, SoftName, 13);

	MEM_ZERO(szBuff);
	PrnSetNormal();
	//sprintf((char *)szBuff, "-- CUSTOMER COPY --                %13s\n",  SoftName);
	//MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MultiLngPrnStr("--CUSTOMER COPY--",DISP_LINE_CENTER);
}

void PrnDuplicateHeader(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	PrnSetNormal();
	MultiLngPrnStr("**********DUPLICATE**********\n",DISP_LINE_CENTER);
}


void PrnDuplicateCopy(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	PrnSetNormal();
	MultiLngPrnStr("--DUPLICATE COPY--",DISP_LINE_CENTER);
}

#ifdef BANK_MANDIRI
void PrnTermMonthlyPowerBuy(void)
{
	uchar szBuff[50+1];
	uchar szBuf1[50+1];

	MEM_ZERO(szBuff);
	MEM_ZERO(szBuf1);

	PrnSetBig();
	sprintf((char *)szBuff, _T("TERM           %9s\n"), glProcInfo.stTranLog.szTenorPowerBuy);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	App_ConvAmountTran(glProcInfo.stTranLog.szAmountMonthlyPowerBuy, szBuff, GA_SEPARATOR);
	sprintf((char *)szBuf1, _T("MONTHLY%17s\n"), szBuff);
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
}
#endif

int PrintReceiptTrx(uchar ucPrnFlag)
{	
	uchar	ucNum;
	uchar	szBuff[50],szBuf1[50];
	uchar	szTranName[16+1];

	for(ucNum=0; ucNum<NumOfReceipt(); ucNum++)
	{
		PrnInit();
		PrnSetNormal();

		PrnCustomLogo_T();

		PrnHeader();

		PrnTidMid();

		////////////////////////////////////////////////////////////////////////
		// Card Name
		PrnSetNormal();
		memset(szBuff, 0, sizeof(szBuff));
		sprintf((char *)szBuff, "CARD TYPE: %s\n", glProcInfo.stTranLog.szCardType);
		MultiLngPrnStr(szBuff,DISP_LINE_LEFT);

		///////////////////////////////////////////////////////////////////////
		// PAN
		PrnSetNormal();
		if (ChkIfTransMaskPan(ucNum))
		{
			MaskPan(glProcInfo.stTranLog.szPan, szBuff);
		}
		else
		{
			strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
		}

		if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT )
		{
			sprintf(szBuf1, "%s -SWIPE", szBuff);
		}
		else if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			sprintf(szBuf1, "%s -DIP", szBuff);
		}
		else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
				 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
		{
			sprintf(szBuf1, "%s -FALLBACK", szBuff);
		}
		else if( glProcInfo.stTranLog.uiEntryMode & MODE_CONTACTLESS )
		{
			sprintf(szBuf1, "%s -TAP", szBuff);
		}
		else
		{
			sprintf(szBuf1, "%s-MANUAL", szBuff);
		}
		
		memset(szBuff, 0, sizeof(szBuff));
		sprintf((char *)szBuff, "%-23.23s\n", szBuf1);
		MultiLngPrnStr(szBuff,DISP_LINE_LEFT);
		////////////////////////////////////////////////////////////////////////////////////
		// Holder name
		PrnSetNormal();
		memset(szBuff, 0, sizeof(szBuff));
		sprintf((char *)szBuff, "%-23.23s\n", glProcInfo.stTranLog.szHolderName);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		///////////////////////////////////////////////////////////////////////////////////
		// Transaction type
		PrnSetBig();
		sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel);
		if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID || (glProcInfo.stTranLog.uiStatus & TS_VOID) )
		{
			sprintf(szTranName, "%.16s", glTranConfig[glProcInfo.stTranLog.ucOrgTranType].szLabel);
			sprintf((char *)szBuff, "%s(%s)\n", _T(szTranName), _T("VOID"));
		}
		else if( glProcInfo.stTranLog.uiStatus & TS_ADJ )
		{
			sprintf((char *)szBuff, "%s(%s)\n", _T(szTranName), _T("ADJ"));
		}
		else
		{
			sprintf((char *)szBuff, "%s\n", szTranName);
		}
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		/////////////////////////////////////////////////////////////////////////////////////
		// Batch, Invoice
		PrnSetNormal();
		sprintf((char *)szBuff, "BATCH: %-24.06lu TRACE NO: %06lu\n", 
			glCurTlmTmsAcq.ulCurBatchNo, glProcInfo.stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		/////////////////////////////////////////////////////////////////////////////////////
		// Date, time
		PrnSetNormal();
		Conv2EngTime(glProcInfo.stTranLog.szDateTime, szBuf1);
		//DD_MMM_YYYY__hh:mm:ss    
		sprintf((char *)szBuff, "DATE: %-27.11s TIME: %8s\n", szBuf1, szBuf1+13);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		////////////////////////////////////////////////////////////////////////////////////////////////
		// REF, APPV
		PrnSetNormal();
		sprintf((char *)szBuff, "REF NO: %-22.12s APPR CODE: %6s\n",  
			glProcInfo.stTranLog.szRRN, glProcInfo.stTranLog.szAuthCode);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		// amount
		PrnAmount_New((uchar *)"", TRUE);  //Print amount
		
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			memset(szBuf1, 0, sizeof(szBuf1));
			sprintf((char *)szBuf1, "APP:%.16s\n", glProcInfo.stTranLog.szAppLabel);
			MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

			PubBcd2Asc0(glProcInfo.stTranLog.sAID, glProcInfo.stTranLog.ucAidLen, szBuff);
			PubTrimTailChars(szBuff, 'F');
			memset(szBuf1, 0, sizeof(szBuf1));
			sprintf((char *)szBuf1, "AID:%.32s\n", szBuff);
			MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

			PubBcd2Asc0(glProcInfo.stTranLog.sAppCrypto, 8, szBuff);
			memset(szBuf1, 0, sizeof(szBuf1));
			sprintf((char *)szBuf1, "TC: %.16s\n", szBuff);
			MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

			if( glProcInfo.stTranLog.uiEntryMode & MODE_OFF_PIN )
			{
				MultiLngPrnStr("   PIN VERIFIED\n", DISP_LINE_LEFT);
			}
			else
			{
				PrnStr("\n");
			}
		}

		if( ucPrnFlag==PRN_REPRINT )
		{
			PrnSetBig();
			sprintf((char *)szBuff, "%s\n", _T("********REPRINT********"));
			MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
			PrnSetNormal();
		}

		if(( ucNum==0 ) ||((ucNum==1)&&(NumOfReceipt()>2)))
		{
			PrnSetNormal();
			if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
			{
				MultiLngPrnStr("*** PIN VERIFICATION SUCCESS ***\n", DISP_LINE_CENTER);
			}
			else
			{
				PrnStr("\n\n\n\n");
				PrnStr("SIGN X------------------------------------------\n");
			}
					
			MultiLngPrnStr("I AGREE TO PAY ABOVE TOTAL AMOUNT\n", DISP_LINE_CENTER);
			MultiLngPrnStr("ACCORDING TO CARD ISSUER AGREEMENT\n", DISP_LINE_CENTER);
		}
		else
		{
			PrnSetNormal();
			MultiLngPrnStr("NO SIGNATURE REQUIRED\n", DISP_LINE_CENTER);
			
			if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
			{
				MultiLngPrnStr("*** PIN VERIFICATION SUCCESS ***\n", DISP_LINE_CENTER);
			}
			
			MultiLngPrnStr("I AGREE TO PAY ABOVE TOTAL AMOUNT\n", DISP_LINE_CENTER);
			MultiLngPrnStr("ACCORDING TO CARD ISSUER AGREEMENT\n", DISP_LINE_CENTER);
		}

		PrnSetNormal();
		if( ucNum==0 )
		{
			MultiLngPrnStr(_T("  **** MERCHANT COPY ****  "), DISP_LINE_CENTER);
		}
		else if( ucNum>0 )
		{
			if((ucNum==1)&&(NumOfReceipt()>2))
				MultiLngPrnStr(_T("  **** BANK COPY ****  "), DISP_LINE_CENTER);
			else
				MultiLngPrnStr(_T("  **** CUSTOMER COPY ****  "), DISP_LINE_CENTER);
		}

		PrnStr("\n\n\n\n\n\n\n\n");

		StartPrinter();

		if( ucNum==0 && NumOfReceipt() != 1)
		{
	        	kbflush();
			ScrClrLine(2,7);
			PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
			PubWaitKey(USER_OPER_TIMEOUT);
		}
	}

	return 0;
}


int PrintReceipt(uchar ucPrnFlag)
{
//	uchar	szBuff[50];/
//	uchar	szIssuerName[10+1];

	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return 0;
	}

	if (ChkIfIrDAPrinter())
	{
		SetOffBase(OffBaseDisplay);
		ChkOnBase();
	}

	DispPrinting();
	if( ChkIfThermalPrinter() )
	{
		switch(glProcInfo.stTranLog.ucTranType)
		{
			case TRANS_TYPE_SALE_REDEEMPTION:
				return PrnSaleRedeemption(ucPrnFlag);
			case TRANS_TYPE_CICILAN_BRING:
				return PrnCicilanBring(ucPrnFlag);
			case TRANS_TYPE_CICILAN_BUKOPIN:
				return PrnCicilanBukopin(ucPrnFlag);
			case TRANS_TYPE_PRIVILAGE:
			
			default:
				return PrnReceipt(ucPrnFlag);
		}
	}
	
	return 0;
}
// end of file

// Add By Zaki 22 April 2013
void PrintParamNew(void)
{
	uchar	ucCnt;

#if defined(_P60_S1_) || defined(_S_SERIES_)
	SetOffBase(OffBaseCheckPrint);
#endif

	PubShowTitle(TRUE, (uchar *)_T("PRINT PARAMETER"));

	PrnInit();
	PrnSetNormal();

	MultiLngPrnStr("***   INITIALISE   ***\n",	DISP_LINE_CENTER);
	PrnHeader();
	PrnEngTime();
	

	if (glTlmSysParam.ucDescNum)
	{
		PrnStr("DESCRIPTION:\n");
		for(ucCnt=0; ucCnt<glTlmSysParam.ucDescNum; ucCnt++)
		{
			PrnStr("   %.2s:%.20s\n", glTlmSysParam.stDescList[ucCnt].szCode,
					glTlmSysParam.stDescList[ucCnt].szText);
		}
	}

	PrnInstalmentPara();

	for(ucCnt=0; ucCnt<glTlmSysParam.ucAcqNum; ucCnt++)
	{
		if( PrnParaAcqNew(ucCnt)==0 )
		{
			if( StartPrinter()!=0 )
			{
				return;
			}
			PrnInit();
			PrnSetNormal();
			PrnStr("\n\n");
		}
	}
	
	if( StartPrinter()!=0 )
	{
		return;
	}

	if (!ChkIfEmvEnable())
	{
		return;
	}

#ifdef ENABLE_EMV
	PubShowTitle(TRUE, (uchar *)_T("PRN EMV PARA"));
	PrnEmvParaNew();
#endif
}

#ifdef ENABLE_EMV
// Print EMV parameter STDCREDITDEBIT //Add By Zaki 22 April 2013
int PrnEmvParaNew(void)
{
	int			iRet, iCnt;
	EMV_APPLIST	stEmvApp;
	int			 iLoop;

	PrnInit();
	PrnSetBig();
	PrnStr("EMV CREDIT\n");
	PrnStr("----EMV PARAMETERS----\n");
	EMVGetParameter(&glEmvParam);
	PrnStr("LOADED  AIDS\n");
	for(iCnt=0; iCnt<MAX_APP_NUM; iCnt++)
	{
		memset(&stEmvApp, 0, sizeof(EMV_APPLIST));
		iRet = EMVGetApp(iCnt, &stEmvApp);
		if( iRet!=EMV_OK )
		{
			continue;
		}

		for(iLoop=0; iLoop<(int)stEmvApp.AidLen; iLoop++)
	{
		PrnStr("%02X", stEmvApp.AID[iLoop]);
	}
		PrnStr("\n");
		
	}
	PrnStr("\n\n\n\n\n\n");
	return StartPrinter();
}
#endif


int PrnParaAcqNew(uchar ucAcqIndex)  //Add By Zaki 22 April 2013
{
	PubASSERT(glTlmSysParam.Acq[ucAcqIndex].acqID!=INV_ACQ_KEY);
	if( glTlmSysParam.Acq[ucAcqIndex].acqID==INV_ACQ_KEY )
	{
		return 1;
	}
	PrnStr("\n");
	PrnStr("HOST  %.3s  %.10s\n", glTlmSysParam.Acq[ucAcqIndex].acqNII,
			glTlmSysParam.Acq[ucAcqIndex].acqName);
	PrnStr("    TID: %.8s",   glTlmSysParam.Acq[ucAcqIndex].acqTID);
	PrnStr("           MID: %.15s\n",  glTlmSysParam.Acq[ucAcqIndex].acqMID);
	PrnStr("    PRI# %.24s", glTlmSysParam.Acq[ucAcqIndex].acqPriTxnPhnSrv);
	PrnStr("                SEC# %.24s\n", glTlmSysParam.Acq[ucAcqIndex].acqSecTxnPhnSrv);
	

	//tcp/ip
	if (strlen(glTlmSysParam.Acq[ucAcqIndex].acqPriTxnIP))
	{
	
		PrnStr("    PRI IP: %.15s|%.5s\n",	glTlmSysParam.Acq[ucAcqIndex].acqPriTxnIP,
										glTlmSysParam.Acq[ucAcqIndex].acqPriTxnPort);
	}
	if (strlen(glTlmSysParam.Acq[ucAcqIndex].acqSecTxnIP))
	{
		PrnStr("    SEC IP: %.15s|%.5s\n",	glTlmSysParam.Acq[ucAcqIndex].acqSecTxnIP,
										glTlmSysParam.Acq[ucAcqIndex].acqSecTxnPort);
	}

	return 0;
}


	
// end of file



void PrintSummaryReport(void)
{
	uchar	buff[50+1];
	uchar	ucIdx;

	memset(buff, 0, sizeof(buff));

	ScrCls();
	PubDispString("SUMMARY REPORT", DISP_LINE_CENTER|DISP_LINE_REVER|0);
	DispProcess();

	CalcTotal(ACQ_ALL);

	if (ChkIfZeroTotal(&glTransTotal))
	{
		ScrClrLine(2, 7);
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	for(ucIdx=0;ucIdx<glTlmSysParam.ucAcqNum;ucIdx++)
	{
		SetCurAcq(ucIdx);
		if(ChkIfBRICredit()||ChkIfBRIDebit())
			break;
	}
	
	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	PrnSetBig();
	MultiLngPrnStr("SUMMARY REPORT\n", DISP_LINE_CENTER);
	
	//TID MID
	//PrnTidMid();
	PrnDateTime();

	
	PrnStr("================================================\n");
	PrnStr("TOTAL BY ACQUIRER\n");
	StartPrinter();
	//*//
	for(ucIdx=0;ucIdx<glTlmSysParam.ucAcqNum;ucIdx++)
	{
		
		memset(&glTransTotal, 0, sizeof(TOTAL_INFO));
		memcpy(&glTransTotal, &glAcqTotal[ucIdx], sizeof(TOTAL_INFO));

		if( ChkIfZeroTotal(&glTransTotal) )
		{
			continue;
		}

		SetCurAcq(ucIdx);
		
		//if(ChkIfBRICredit()||ChkIfBRIDebit())
		//{
			PrnInit();
			PrnSetNormal();
			PrnTidMid();
			PrnStr("HOST %s %s BATCH %06lu\n", glCurTlmTmsAcq.acqNII, glCurTlmTmsAcq.acqName, glCurTlmTmsAcq.ulCurBatchNo);
			PrnTotalInfo(&glTransTotal);
			//PrnTotalByAcq();
			PrnStr("------------------------------------------------\n");
			StartPrinter();
		//}
	}
	//*/
	PrnInit();
	PrnSetNormal();
	PrnStr("================================================\n");	

	PrnStr("%s", ChkIfThermalPrinter() ? "\n\n\n\n\n\n\n\n" : "\f");
	StartPrinter();
}


void PurchaseParseDetail(void)
{
	int	iRet;
	ushort	RecNo;
	TRAN_LOG	stTranLog;

	uchar szBuff[50+1]; int z=0;
	uchar szTransType[24];
	uchar PrintBuff[24];
	uchar PrintBuff2[24];
	uchar ucTranType;
	uchar flagVoid=FALSE;
	uchar buffName[10+1];
	
	memcpy(buffName,glCurTlmTmsAcq.acqName,10);

	for(RecNo=0;RecNo<MAX_TRANLOG;RecNo++)
	{
		if(glSysCtrl.sAcqKeyList[RecNo]==INV_ACQ_KEY)
			continue;

		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadTranLog(&stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}

		FindAcq(stTranLog.ucAcqKey);
		FindIssuer(stTranLog.ucIssuerKey);
		if(memcmp(glCurTlmTmsAcq.acqName,buffName,10))
			continue;
		z++;
		
		PrnInit();
		PrnSetNormal();
		if(z==1){
		MultiLngPrnStr("================================================\n", DISP_LINE_LEFT);
		PrnTidMid();
		PrnStr("HOST %s %s\n", glCurTlmTmsAcq.acqNII, glCurTlmTmsAcq.acqName);
			}

		flagVoid=FALSE;
		
		if( stTranLog.ucTranType==TRANS_TYPE_VOID || (stTranLog.uiStatus & TS_VOID) )
		{
			ucTranType = stTranLog.ucOrgTranType;
			flagVoid=TRUE;
		}
		else
		{
			ucTranType = stTranLog.ucTranType;
		}

		switch(ucTranType)
		{
			case TRANS_TYPE_SALE:
				if(flagVoid)
				{
					if(stTranLog.uiEntryMode==MODE_MANUAL_INPUT)
						sprintf(szTransType, "VOID MANUAL          ");
					else
						sprintf(szTransType, "VOID SALE            ");
				}
				else
				{
					if(stTranLog.uiEntryMode==MODE_MANUAL_INPUT)
						sprintf(szTransType, "MANUAL               ");
					else
						sprintf(szTransType, "SALE                 ");
				}
				break;
			case TRANS_TYPE_REFUND:
				if(flagVoid)
					sprintf(szTransType, "VOID REFUND          ");
				else
					sprintf(szTransType, "REFUND               ");
				break;
			case TRANS_TYPE_CICILAN_BRING:
				if(flagVoid)
					sprintf(szTransType, "VOID CICILAN BRING   ");
				else
					sprintf(szTransType, "CICILAN BRING        ");
				break;
			case TRANS_TYPE_PRIVILAGE:
				if(flagVoid)
					sprintf(szTransType, "VOID PRIVILAGE       ");
				else
					sprintf(szTransType, "PRIVILAGE            ");
				break;
			case TRANS_TYPE_SALE_REDEEMPTION:
				if(flagVoid)
					sprintf(szTransType, "VOID SALE REDEEMPTON ");
				else
					sprintf(szTransType, "SALE REDEEMPTON      ");
				break;
			case TRANS_TYPE_CARD_VERIFY:
				sprintf(szTransType, "CARD VERIFY          ");
				break;
			case TRANS_TYPE_CICILAN_BUKOPIN:
				#ifdef BANK_HSBC
				if(flagVoid)
					sprintf(szTransType, "VOID CICILAN HSBC ");
				else
					sprintf(szTransType, "CICILAN HSBC      ");
				break;
				#else
				if(flagVoid)
					sprintf(szTransType, "VOID CICILAN BUKOPIN ");
				else
					sprintf(szTransType, "CICILAN BUKOPIN      ");
				break;
				#endif
			case TRANS_TYPE_OFF_SALE:
				if(flagVoid)
					sprintf(szTransType, "VOID OFFLINE SALE");
				else
					sprintf(szTransType, "OFFLINE SALE      ");
				break;
		}
		
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(stTranLog.szAmount, PrintBuff, GA_SEPARATOR);

		PrnSetNormal();

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%-31.21s %16s\n", szTransType, PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MEM_ZERO(PrintBuff2);
		memcpy(PrintBuff, stTranLog.szPan, strlen(stTranLog.szPan));
		memcpy(PrintBuff+6, "******", 6);
		
		strcpy(PrintBuff2, "  /  ");
		memcpy(PrintBuff2, stTranLog.szExpDate, 2);
		memcpy(PrintBuff2+3, stTranLog.szExpDate+2, 2);
		sprintf((char *)szBuff, "%-42.19s %5s\n", PrintBuff, PrintBuff2);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		//REFNUM
		//MEM_ZERO(szBuff);
		//sprintf(szBuff, "REF# : %-24.12s APPR  :	 %6s\n", stTranLog.szRRN, stTranLog.szAuthCode);
		//MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		//STAN //TRACE
		MEM_ZERO(szBuff);
		//sprintf(szBuff, "STAN : %-24.06lu TRACE :   %06lu\n", stTranLog.ulSTAN, stTranLog.ulInvoiceNo);
		sprintf(szBuff,   "APPR : %-24.6s TRACE :   %06lu\n", stTranLog.szAuthCode, stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
	
		Conv2EngTime(glProcInfo.stTranLog.szDateTime, PrintBuff);
		//DD_MMM_YYYY__hh:mm:ss    
		sprintf((char *)szBuff, "DATE : %-24.11s TIME  : %8s\n", PrintBuff, PrintBuff+13);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		//PrnDateTime();
		
		StartPrinter();
		
	}
	return;
}

void PrintDetailReport(void)
{
	uchar	buff[50+1];
	uchar	ucIdx;

	memset(buff, 0, sizeof(buff));

	ScrCls();
	PubDispString("DETAIL REPORT", DISP_LINE_CENTER|DISP_LINE_REVER|0);
	DispProcess();

	CalcTotal(ACQ_ALL);

	if (ChkIfZeroTotal(&glTransTotal))
	{
		ScrClrLine(2, 7);
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	
	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	PrnSetBig();
	MultiLngPrnStr("DETAIL REPORT\n", DISP_LINE_CENTER);
	
	//TID MID
	//PrnTidMid();
	PrnDateTime();

	
	PrnStr("================================================\n");
	StartPrinter();

	for(ucIdx=0;ucIdx<glTlmSysParam.ucAcqNum;ucIdx++)
		{
			SetCurAcq(ucIdx);
			//hex_dump_char("name",glCurTlmTmsAcq.acqName,10);
			PurchaseParseDetail();
		}
		
	//PurchaseParseDetail();

	PrnInit();
	PrnSetNormal();
	PrnStr("================================================\n");	

	PrnStr("%s", ChkIfThermalPrinter() ? "\n\n\n\n\n\n\n\n" : "\f");
	StartPrinter();
}

// Zaki 12/2/2013
void PrnCardTypeNew(void)
{
	char szBuff[50+1];

	PrnSetNormal();
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char *)szBuff, "%s\n", glProcInfo.stTranLog.szCardType);
	MultiLngPrnStr(szBuff,DISP_LINE_LEFT);
}

// Zaki 12/2/2013
void PrnPokokCB(void)
{	
	uchar 	pszOut[32];
	uchar	szBuff[50];
	
	MEM_ZERO(szBuff);
	MEM_ZERO(pszOut);
	#ifdef BANK_BRI //4212 for bukopin
	App_ConvAmountTran(glProcInfo.stTranLog.BRI.CicilanBring.PokokAmt, pszOut, GA_SEPARATOR);
	#else
	App_ConvAmountTran(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.PokokAmt, pszOut, GA_SEPARATOR);
	#endif
	sprintf((char *)szBuff, "    POKOK                       %16s\n", pszOut);
	MultiLngPrnStr(szBuff, DISP_LINE_RIGHT);
}

// Zaki 12/2/2013
void PrnBulanCicilanBring(void)
{
	uchar	szBuff[50];
		int i=0;
	
	MEM_ZERO(szBuff);
	#ifdef BANK_BRI //4212 for bukopin
	sprintf((char *)szBuff, "CICILAN:        %2s Bulan\n", glProcInfo.stTranLog.BRI.CicilanBring.Bulan);
	#elif defined (BANK_HSBC)
	sprintf((char *)szBuff, "CICILAN:    %2s Bulan 0", glProcInfo.szReq61CicilanBukopin);strcat(szBuff, " %%\n");
	#else
	sprintf((char *)szBuff, "CICILAN:        %2s Bulan\n", glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.Bulan);
	#endif
	PrnSetBig();
	MultiLngPrnStr(szBuff, DISP_LINE_RIGHT);
	PrnSetNormal();
	
}


// Zaki 12/2/2013
void PrnCicilanBulanan(void)
{
	uchar 	pszOut[32];
	uchar	szBuff[50];
	
	MEM_ZERO(szBuff);
	MEM_ZERO(pszOut);
	#ifdef BANK_BRI //4212 for bukopin
	App_ConvAmountTran(glProcInfo.stTranLog.BRI.CicilanBring.BulanAmt, pszOut, GA_SEPARATOR);
	#else
	App_ConvAmountTran(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.BulanAmt, pszOut, GA_SEPARATOR);
	#endif
	sprintf((char *)szBuff, "PERBULAN:%15s\n", pszOut);
	PrnSetBig();
	MultiLngPrnStr(szBuff, DISP_LINE_RIGHT);
	PrnSetNormal();
}

// Zaki 26/2/2014
void PrnInterest(void)
{
	//uchar 	pszOut[32];
	uchar	szBuff[50];
	
	MEM_ZERO(szBuff);
	/*MEM_ZERO(pszOut);
	memcpy(pszOut, glProcInfo.stTranLog.BRI.CicilanBring.Interest, GA_SEPARATOR);
	sprintf((char *)szBuff, "INTEREST :                      %3s %%\n", pszOut);
	MultiLngPrnStr(szBuff, DISP_LINE_RIGHT);
	*/
	MEM_ZERO(szBuff);
	
	sprintf((char *)szBuff, "INTEREST :                                 %s", glProcInfo.stTranLog.BRI.CicilanBring.Interest);
	strcat(szBuff, " %%\n");
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
}



// add for private label bukopin , remove metode card :

void PrnCardNumberBigPrvtLbl(void)
{
	char szBuff[50+1];
	char szBuf1[50+1];

	PrnSetBig();
	if (!glCurTlmTmsIss.issFullPAN)
	{
		MaskPan(glProcInfo.stTranLog.szPan, szBuff);
	}
	else
	{
		strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	}

	if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT )
	{
		sprintf(szBuf1, "%s -SWIPE", szBuff);
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		sprintf(szBuf1, "%s (CHIP)", szBuff);
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		sprintf(szBuf1, "%s -FALLBACK", szBuff);
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CONTACTLESS )
	{
		sprintf(szBuf1, "%s -TAP", szBuff);
	}
	else
	{
		sprintf(szBuf1, "%s-MANUAL", szBuff);
	}
		
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char *)szBuff, "%-23.23s\n", szBuf1);
	MultiLngPrnStr(szBuff,DISP_LINE_LEFT);
}


