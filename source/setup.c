
#include "global.h"

/********************** Internal macros declaration ************************/

/********************** Internal structure declaration *********************/

/********************** Internal functions declaration *********************/
void testprintamount(void);

static void SetSystemParam(void);
static void SetSystemParamSub(uchar ucPermission);
//static int  SetCommType(uchar ucMode);
//static void SetSysCommParam(uchar ucPermission);
static int  SetCommDetails(uchar *pucCommType);
static int  SetCommDetailsTMS(uchar *pucCommType); // Add By Zaki 24 April 2013
static int  SetPabx(void);
static int  SetModemParam(void);
static int  SetModemParamTMS(void); // Add By Zaki 25 April 2013
static void SetAcqParam(uchar ucPermission);

static int  SetBaudRateRS232(void);  // Add By Zaki 29 April 2013
#ifdef SXX_IP_MODULE
static int  SetTcpIpParam_S80(TCPIP_PARA *pstParam);
#endif
static int  GetIPAddress(uchar *pszPrompts, uchar bAllowNull, uchar *pszIPAddress);
static uchar ValidIPAddress(char *pszIPAddr);
static int  GetIPPort(uchar *pszPrompts, uchar bAllowNull, uchar *pszPortNo);
static void SetIssuerParam(uchar ucPermission);
static uchar DispAcqPama(void);
static int  SetAcqTransComm(uchar ucCommType);
static int  SetAcqTransTelNo(void);
static int  SetTel(uchar *pszTelNo, uchar *psDisp);
static void SetEdcParam(uchar ucPermission);
static int  SetTermCurrency(uchar ucPermission);
static int  SetMerchantName(uchar ucPermission);
static int  SetGetSysTraceNo(uchar ucPermission);
static int  SetGetSysInvoiceNo(uchar ucPermission);
static int  SetPEDMode(void);
static int  SetAcceptTimeOut(void);
static int  SetPrinterType(void);
static int  SetNumOfReceipt(void);
static int  SetCallInTime(void);
static uchar IsValidTime(uchar *pszTime);
static int  ModifyOptList(uchar *psOption, uchar ucMode, uchar ucPermission);
static void ChangePassword(void);
//static void SetSysTime(void);
static void SetEdcLang(void);
static void SetPowerSave(void);
static void TestMagicCard1(void);
static void TestMagicCard2(void);
static void TestMagicCard3(void);
static void TestMagicCard(int iTrackNum);
static void ToolsViewPreTransMsg(void);
static void ShowExchangePack(void);
static void PrnExchangePack(void);
static void DebugNacTxd(uchar ucPortNo, uchar *psTxdData, ushort uiDataLen);

/********************** Internal variables declaration *********************/
/*
static MenuItem sgFuncMenu[] =
{
	{FALSE, _T_NOOP("0.INIT"),			DownLoadTMSPara_Manual},
	{TRUE,  _T_NOOP("1.VIEW RECORD"),	ViewTranList},
	{FALSE, _T_NOOP("2.SETUP"),			SetSystemParam},
	{TRUE,  _T_NOOP("3.LANGUAGE"),		SetEdcLang},
//	{TRUE,  _T_NOOP("4.LOCK TERM"),		LockTerm},
	{TRUE,  _T_NOOP("4.DEBUG MESSAGE"),		DebugSet},
	{TRUE,  _T_NOOP("5.VIEW TOTAL"),	ViewTotal},
	{FALSE, _T_NOOP("6.ECHO TEST"),		TransEchoTest},
	{FALSE, _T_NOOP("7. VIEW TVR"),		ViewTVR_TSI}, // zaki 9 Januari 2014
	{FALSE, _T_NOOP("8. Debug Tag EMV"),		DebugTagEmvSet}, // zaki 13 Januari 2014
#ifdef ENABLE_EMV
	{FALSE, _T_NOOP("9.LAST TSI TVR"),	ViewTVR_TSI},
#endif
	{TRUE,  _T_NOOP("10.SET TIME"),		SetSysTime},

	{TRUE,  _T_NOOP("11.FUNCTION 11"),		Function_11},
//#ifdef BANK_MANDIRI
//	{TRUE,  _T_NOOP("11.DEBUG TLE"),	TLEDebugSet},
//	{TRUE,  _T_NOOP("12.SET TLE"),	TLESet},	
//#endif	
	
	{TRUE,  _T_NOOP("13.SET DEBUG EMV"),	DebugEMVICCSet},
	{TRUE,  _T_NOOP("14.SET BLOWFISH"),	BlowFishSet},
	{TRUE, _T_NOOP("20.PRINT PARA"),	PrintParam}, //PrintParamNew(); 	//TransEchoTest();
	
	{TRUE,  _T_NOOP("21.TXN REVIEW"),	ViewSpecList},
//    {FALSE, _T_NOOP("22.ADJUST"),		TransAdjust},
	{TRUE,  _T_NOOP("22.FUNCTION 22"),	Function_22},
	{TRUE, _T_NOOP("23.LAST PARA PRINT "),  PrintParamNew}, // Add By Zaki 23 April 2013


#if 1//irf, 20121128
//	{FALSE, _T_NOOP("30.INPUT TMK"),		InputTMK},
//	{FALSE, _T_NOOP("31.INPUT TWK"),		InputTWK},
	{FALSE, _T_NOOP("32.PRINT PARAM"),	PrintParam},
	{TRUE, _T_NOOP("33.DISPLAY VER"),		DispVersion},
	{FALSE, _T_NOOP("34.TMS"),		TmsTeliumDownLoad},

	//{FALSE, _T_NOOP("42.CLEAR CUM DEBIT"),		BrizziClearCumDebit},
	{FALSE, _T_NOOP("43.test amount"),		testprintamount},

//	{FALSE, _T_NOOP("45.print menu"),		PrintMenu},
	
	
#endif	//end, irf 20121128

	{TRUE,	_T_NOOP("50.APP UPDATE"),	DownLoadTMSPara_Auto},	// hidden, not for public use until confirm.

//	{TRUE,	_T_NOOP("51.KEK SAM"),	LTMKKeyDownload},	// hidden, not for public use until confirm.
//	{TRUE,	_T_NOOP("52.KEK SAM"),	test_picc},	// hidden, not for public use until confirm.

	{FALSE,	_T_NOOP("60.CHECK FONTS"),	EnumSysFonts},
	{TRUE,  	_T_NOOP("71.REPRN SETTLE"),	RePrnSettleNew},
	{TRUE,  	_T_NOOP("72.REPRINT LAST"),	PrnLastTrans},
	{TRUE,  	_T_NOOP("73.REPRINT BILL"),	RePrnSpecTrans},
	{TRUE,  	_T_NOOP("74.PRINT TOTAL"),	PrnTotal},
	{TRUE, 	_T_NOOP("75.PRINT LOG"),		PrnAllList},
	{TRUE,  	_T_NOOP("81.POWER SAVE"),	SetPowerSave},
//	{FALSE, 	_T_NOOP("87.TEST TRACK1"),	TestMagicCard1},
//	{FALSE, 	_T_NOOP("88.TEST TRACK2"),	TestMagicCard2},
//	{FALSE, 	_T_NOOP("89.TEST TRACK3"),	TestMagicCard3},
	{TRUE,  	_T_NOOP("90.MODIFY PWD"),	ChangePassword},
	{TRUE,  	_T_NOOP("91.DISP VER"),		DispVersion},
	{FALSE,	_T_NOOP("95.SHOW PACKAGE"),	ToolsViewPreTransMsg},
#ifdef ENABLE_EMV
	{FALSE, 	_T_NOOP("96.PRINT ERR LOG"),PrintEmvErrLog},
#endif
	{FALSE, 	_T_NOOP("99.CLEAR"),		DoClear},
	{TRUE,  "", NULL},
};
*/

static MenuItem sgFuncMenu[] =
{	//{FALSE, _T_NOOP("7. VIEW TVR"),			ViewTVR_TSI}, // zaki 9 Januari 2014
	//{FALSE, _T_NOOP("8. Debug Tag EMV"),		DebugTagEmvSet}, // zaki 13 Januari 2014 PrintTagEMVTes
	{TRUE, 	_T_NOOP("2. SETUP"),			SetSystemParam},
	{TRUE,  _T_NOOP("4. DEBUG MESSAGE"),	DebugSet},
	//{TRUE, 	_T_NOOP("0. ECHO TEST"),		TransEchoTest},
	{FALSE, _T_NOOP("8. Debug Tag EMV"),	DebugTagEmvSet}, // zaki 13 Januari 2014
	{FALSE,	_T_NOOP("9. SET DEBUG EMV"),	DebugEMVICCSet}, // 4212 for tes
	{TRUE, 	_T_NOOP("10. TMS"),				TmsTeliumDownLoad},
	//{FALSE, _T_NOOP("12. Debug Reques"),	DebugIsoReqSet},
	//{FALSE, _T_NOOP("13. Debug Respond"),	DebugIsoResSet},
#ifdef BANK_BRI
	{TRUE,  _T_NOOP("11.FUNCTION 11"),		Function_11},
	{TRUE,  _T_NOOP("22.FUNCTION 22"),		Function_22},

	//{FALSE, _T_NOOP("42.CLEAR CUM DEBIT"),	BrizziClearCumDebit}, 
#endif
	//{FALSE, _T_NOOP("43.test amount"),		testprintamount},
	#ifdef ENABLE_EMV
	//{FALSE, _T_NOOP("96.PRINT ERR LOG"),	PrintEmvErrLog},
	//{FALSE, _T_NOOP("97.PRINT ERR LOG"),	SetBIT48},
	#endif
	
	{TRUE,  _T_NOOP("12. SET TENOR"),	SwitchMenuCicilan},
	{TRUE,  _T_NOOP("20. INPUT WK"),	INPUT_WK},
	{TRUE,  _T_NOOP("21. INPUT MK"),	INPUT_MK},
	{TRUE,  _T_NOOP("22. PASS"),		ChangePassword},
	{TRUE,  "", NULL},
};

static MenuItem sgInitMenu[] =
{
	{FALSE, _T_NOOP("0.INIT"),			DownLoadTMSPara_Manual},
	{FALSE, _T_NOOP("2.SETUP"),			SetSystemParam},	//new
//	{FALSE, _T_NOOP("2.SETUP"),			SetSystemParam},
	{TRUE,  _T_NOOP("3.LANGUAGE"),		SetEdcLang},
	{TRUE,  _T_NOOP("10.SET TIME"),		SetSysTime},
	{FALSE, _T_NOOP("34.TMS"),			TmsTeliumDownLoad},		//new
	{FALSE,	_T_NOOP("60.CHECK FONTS"),	EnumSysFonts},
	{FALSE, _T_NOOP("87.TEST TRACK1"),	TestMagicCard1},
	{FALSE, _T_NOOP("88.TEST TRACK2"),	TestMagicCard2},
	{FALSE, _T_NOOP("89.TEST TRACK3"),	TestMagicCard3},
	{TRUE,  _T_NOOP("90.MODIFY PWD"),	ChangePassword},
	{TRUE,  _T_NOOP("91.DISP VER"),		DispVersion},
	{FALSE, _T_NOOP("99.CLEAR"),		DoClear},
	{TRUE,  "", NULL},
};

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/
void GetAllSupportFunc(char *pszBuff)
{
	int	ii;
	ulong	ulTemp;

	pszBuff[0] = 0;
	for (ii=0; ii<sizeof(sgFuncMenu)/sizeof(sgFuncMenu[0]); ii++)
	{
		if (sgFuncMenu[ii].szMenuName[0]!=0)
		{
			ulTemp = PubAsc2Long(sgFuncMenu[ii].szMenuName, 2, NULL);
			if (ulTemp<100)
			{
				if (strlen(pszBuff)!=0)
				{
					strcat(pszBuff, ",");
				}
				sprintf(pszBuff+strlen(pszBuff), "%lu", ulTemp);
			}
		}
	}
}

// 执行指定功能号的函数
void FunctionExe(uchar bUseInitMenu, int iFuncNo)
{
	int			iCnt, iTempNo;
	MenuItem	*pstMenu;
	
	pstMenu = bUseInitMenu ? sgInitMenu : sgFuncMenu;
	for(iCnt=0; pstMenu[iCnt].szMenuName[0]!=0; iCnt++)
	{
		if( isdigit(pstMenu[iCnt].szMenuName[1]) )
		{
			iTempNo = (int)PubAsc2Long(pstMenu[iCnt].szMenuName, 2, NULL);
		}
		else
		{
			iTempNo = (int)PubAsc2Long(pstMenu[iCnt].szMenuName, 1, NULL);
		}
		if( iTempNo==iFuncNo )
		{
			if( pstMenu[iCnt].pfMenuFunc==NULL )
			{
				break;
			}
			(*pstMenu[iCnt].pfMenuFunc)();
			return;
		}
	}

	ScrClrLine(4, 7);
	PubDispString(_T("FUNC NUMBER ERR"), 5|DISP_LINE_CENTER);
	PubBeepErr();
	PubWaitKey(3);
}

void FunctionMenu(void)
{
	PubGetMenu((uchar *)_T("FUNCTION:"), sgFuncMenu, MENU_USESNO|MENU_CFONT, USER_OPER_TIMEOUT);
}

void FunctionInit(void)
{
	int		iFuncNo;

	iFuncNo = FunctionInput();
	if( iFuncNo>=0 )
	{
		FunctionExe(TRUE, iFuncNo);
	}
}

// 设置系统参数
void SetSystemParam(void)
{
	uchar ucPermission;

#ifdef FUN2_READ_ONLY
	ucPermission = PM_LOW;		// 低权限
#else
	ucPermission = PM_MEDIUM;	// 中等权限
#endif

	PubShowTitle(TRUE, (uchar *)_T("TERM SETUP"));
	if( PasswordBank()!=0 )
	{
		return;
	}

	SetSystemParamSub(ucPermission);
}
		
void SetSystemParamSub(uchar ucPermission)
{
	uchar	ucKey;
	while( 1 )
	{
		ScrCls();
		PubDispString(_T("1.COMM PARA"),     DISP_LINE_LEFT|0);
		PubDispString(_T("2.VIEW EDC"),      DISP_LINE_LEFT|2);
		#ifndef PRODUCTION
		PubDispString(_T("3.VIEW ISSUER"),   DISP_LINE_LEFT|4);
		#endif
		PubDispString(_T("4.VIEW ACQUIRER"), DISP_LINE_LEFT|6);
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return;
		}
		else if( ucKey==KEY1 )
		{
			SetSysCommParam(ucPermission);
		}
		else if( ucKey==KEY2 )
		{
			SetEdcParam(ucPermission);
		}
		#ifndef PRODUCTION
		else if( ucKey==KEY3 )
		{
		
			SetIssuerParam(ucPermission);
		}
		#endif
		else if( ucKey==KEY4 )
		{
			SetAcqParam(ucPermission);
		}
	}
}

void SetSystemParamAll(void)
{
	// 最高权限，可以修改所有参数
	SetSystemParamSub(PM_HIGH);
}

int GetCommName(uchar ucCommType, uchar *pszText)
{
	switch(ucCommType)
	{
	case CT_RS232:
		sprintf((char *)pszText, "COM");
		return 0;
	case CT_MODEM:
		sprintf((char *)pszText, "MODEM");
	    return 0;
	case CT_TCPIP:
		sprintf((char *)pszText, "TCPIP");
	    return 0;
	case CT_CDMA:
		sprintf((char *)pszText, "CDMA");
		return 0;
	case CT_GPRS:
		sprintf((char *)pszText, "GPRS");
		return 0;
	case CT_WIFI:
		sprintf((char *)pszText, "WIFI");
	    return 0;
		/* //05/12/2014
	case CT_DEMO:
		sprintf((char *)pszText, "DEMO");
	    return 0;
	    */
	default:
		sprintf((char *)pszText, "DISABLED");
	    return -1;
	}
}

// ucForAcq : set custom comm type for ACQ
int SetCommType(uchar ucMode)
{
	int		iRet;
	char	szTitle[32];
	uchar	*pucCommType;
	SMART_MENU	stSmDownMode;
	MENU_ITEM stCommMenu[] =
	{
		{"SELECT COMM",		-1,			NULL},
		{"DISABLE",			CT_NONE,	NULL},
		{"MODEM",			CT_MODEM,	NULL},
		{"TCPIP",			CT_TCPIP,	NULL},
		{"GPRS",			CT_GPRS,	NULL},
		{"CDMA",			CT_CDMA,	NULL},
		{"WIFI",			CT_WIFI,	NULL},
		{"RS232",			CT_RS232,	NULL},
		//////{"DEMO ONLY",		CT_DEMO,	NULL},
		{"",				0, NULL},
	};// This menu does not provide translation

	//--------------------------------------------------
	memset(&stSmDownMode, 0, sizeof(stSmDownMode));
	PubSmInit(&stSmDownMode, stCommMenu);

	if (ucMode==0)
	{
		PubSmMask(&stSmDownMode, "DISABLE",   SM_OFF);	// Only secondary comm type can be "disabled"
	}
	if (ucMode!=0)
	{
		PubSmMask(&stSmDownMode, "DEMO ONLY", SM_OFF);	// Only primary comm type can be "demo"
	}
	
	// diki add komen for demo / dev
	////#ifdef PRODUCTION
	////	PubSmMask(&stSmDownMode, "DEMO ONLY", SM_OFF);	// Only primary comm type can be "demo"
	////#endif
	
	if (ChkTerm(_TERMINAL_P60_S1_) || ChkTerm(_TERMINAL_P70_S_))
	{
		PubSmMask(&stSmDownMode, "TCPIP", SM_OFF);	
		PubSmMask(&stSmDownMode, "GPRS", SM_OFF);
		PubSmMask(&stSmDownMode, "CDMA", SM_OFF);
		PubSmMask(&stSmDownMode, "WIFI", SM_OFF);
	} 
	else
	{
#ifdef __TELIUM_PARAM__
		if (ChkHardware(HWCFG_MODEM, HW_NONE) ||
			((ucMode!=0) && (glTlmSysParam.stTxnCommCfg.ucCommType==CT_MODEM)) )
		{
			PubSmMask(&stSmDownMode, "MODEM", SM_OFF);
		}
		if (ChkHardware(HWCFG_LAN, HW_NONE) ||									// If no LAN module
			((ucMode!=0) && (glTlmSysParam.stTxnCommCfg.ucCommType==CT_TCPIP)) )	// and now is selecting 2nd comm && 1st comm already selected LAN
		{
			PubSmMask(&stSmDownMode, "TCPIP", SM_OFF);							// then disable this option
		}
		if ((ChkHardware(HWCFG_GPRS, HW_NONE) && !ChkTerm(_TERMINAL_P90_)) ||	// Sometimes P90 cannot get module info
			((ucMode!=0) && (glTlmSysParam.stTxnCommCfg.ucCommType==CT_GPRS)) )
		{
			PubSmMask(&stSmDownMode, "GPRS", SM_OFF);
		}
		if ((ChkHardware(HWCFG_CDMA, HW_NONE) && !ChkTerm(_TERMINAL_P90_)) ||	// Sometimes P90 cannot get module info
			((ucMode!=0) && (glTlmSysParam.stTxnCommCfg.ucCommType==CT_CDMA)) )
		{
			PubSmMask(&stSmDownMode, "CDMA", SM_OFF);
		}
		if ((ChkHardware(HWCFG_WIFI, HW_NONE) && !ChkTerm(_TERMINAL_S80_)) ||
			((ucMode!=0) && (glTlmSysParam.stTxnCommCfg.ucCommType==CT_WIFI)) )
		{
			PubSmMask(&stSmDownMode, "WIFI", SM_OFF);
		}
		if ((ucMode!=0) && (glTlmSysParam.stTxnCommCfg.ucCommType==CT_RS232))
		{
			PubSmMask(&stSmDownMode, "RS232", SM_OFF);
		}
		// diki add for mini atm bukopin
		if ((ucMode!=0) && (glTlmSysParam.stTxnCommCfg.ucCommType==CT_DEMO))
		{
			PubSmMask(&stSmDownMode, "DEMO!", SM_OFF);
		}

#else
		if (ChkHardware(HWCFG_MODEM, HW_NONE) ||
			((ucMode!=0) && (glSysParam.stTxnCommCfg.ucCommType==CT_MODEM)) )
		{
			PubSmMask(&stSmDownMode, "MODEM", SM_OFF);
		}
		if (ChkHardware(HWCFG_LAN, HW_NONE) ||									// If no LAN module
			((ucMode!=0) && (glSysParam.stTxnCommCfg.ucCommType==CT_TCPIP)) )	// and now is selecting 2nd comm && 1st comm already selected LAN
		{
			PubSmMask(&stSmDownMode, "TCPIP", SM_OFF);							// then disable this option
		}
		if ((ChkHardware(HWCFG_GPRS, HW_NONE) && !ChkTerm(_TERMINAL_P90_)) ||	// Sometimes P90 cannot get module info
			((ucMode!=0) && (glSysParam.stTxnCommCfg.ucCommType==CT_GPRS)) )
		{
			PubSmMask(&stSmDownMode, "GPRS", SM_OFF);
		}
		if ((ChkHardware(HWCFG_CDMA, HW_NONE) && !ChkTerm(_TERMINAL_P90_)) ||	// Sometimes P90 cannot get module info
			((ucMode!=0) && (glSysParam.stTxnCommCfg.ucCommType==CT_CDMA)) )
		{
			PubSmMask(&stSmDownMode, "CDMA", SM_OFF);
		}
		if ((ChkHardware(HWCFG_WIFI, HW_NONE) && !ChkTerm(_TERMINAL_S80_)) ||
			((ucMode!=0) && (glSysParam.stTxnCommCfg.ucCommType==CT_WIFI)) )
		{
			PubSmMask(&stSmDownMode, "WIFI", SM_OFF);
		}
		if ((ucMode!=0) && (glSysParam.stTxnCommCfg.ucCommType==CT_RS232))
		{
			PubSmMask(&stSmDownMode, "RS232", SM_OFF);
		}
		// diki add for mini atm bukopin
		if ((ucMode!=0) && (glTlmSysParam.stTxnCommCfg.ucCommType==CT_DEMO))
		{
			PubSmMask(&stSmDownMode, "DEMO!", SM_OFF);
		}
#endif		
	}

	memset(szTitle, 0, sizeof(szTitle));
	stCommMenu[0].pszText = szTitle;

#ifdef	__TELIUM_PARAM__
	if (ucMode==0)
	{
		pucCommType = &glTlmSysParam.stTxnCommCfg.ucCommType;
		strcpy((char *)stCommMenu[0].pszText, "1st:");
	}
	else
	{
		pucCommType = &glTlmSysParam.stTxnCommCfg.ucCommTypeBak;
		strcpy((char *)stCommMenu[0].pszText, "2nd:");
	}
#else
	if (ucMode==0)
	{
		pucCommType = &glSysParam.stTxnCommCfg.ucCommType;
		strcpy((char *)stCommMenu[0].pszText, "1st:");
	}
	else
	{
		pucCommType = &glSysParam.stTxnCommCfg.ucCommTypeBak;
		strcpy((char *)stCommMenu[0].pszText, "2nd:");
	}
#endif
	GetCommName(*pucCommType, stCommMenu[0].pszText+strlen((char *)stCommMenu[0].pszText));
	iRet = PubSmartMenuEx(&stSmDownMode, SM_1ITEM_PER_LINE, USER_OPER_TIMEOUT);
	if (iRet<0)
	{
		return ERR_USERCANCEL;
	}

	if (!ChkIfBatchEmpty() && (*pucCommType!=(uchar)iRet))
	{
		// Not allow to switch into/off demo mode unless batch empty
		if ((*pucCommType==CT_DEMO) || ((uchar)iRet==CT_DEMO))
		{
			ScrClrLine(2,7);
			PubDispString(_T("PLS SETTLE BATCH"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(USER_OPER_TIMEOUT);
			return ERR_USERCANCEL;
		}
	}

	*pucCommType = (uchar)iRet;
	return 0;
}

// 设置通讯参数
void SetSysCommParam(uchar ucPermission)
{
	PubShowTitle(TRUE, (uchar *)"SETUP COMM      ");

	while (1)
	{
		if (SetCommType(0)!=0)
		{
			break;
		}

#ifdef __TELIUM_PARAM__
		if (SetCommDetails(&glTlmSysParam.stTxnCommCfg.ucCommType))
		{
			break;
		}
#else
		if (SetCommDetails(&glSysParam.stTxnCommCfg.ucCommType))
		{
			break;
		}
#endif

		if (ChkIfTrainMode())	// if demo mode, no need to set second one
		{
			break;
		}

		if (SetCommType(1)!=0)
		{
			break;
		}

#ifdef __TELIUM_PARAM__
		if (SetCommDetails(&glTlmSysParam.stTxnCommCfg.ucCommTypeBak))
		{
			break;
		}
#else
		if (SetCommDetails(&glSysParam.stTxnCommCfg.ucCommTypeBak))
		{
			break;
		}
#endif
		break;
	}
#ifdef 	__TELIUM_PARAM__
	SaveTlmSysParam();
#else
	SaveSysParam();
#endif
ProcUserMsg(); // zaki 13 Januari 2014
}

int SetCommDetails(uchar *pucCommType)
{
	uchar	szDispBuff[32];

	sprintf((char *)szDispBuff, "SETUP ");
	GetCommName(*pucCommType, szDispBuff+strlen((char *)szDispBuff));
	ScrCls();
	PubDispString(szDispBuff, DISP_LINE_REVER);

	switch( *pucCommType )
	{
	case CT_RS232:
		break;

	case CT_MODEM:
		SetModemParam();
		glTlmSysParam._TmsPSTNPara.ucSendMode = glTlmSysParam._TxnPSTNPara.ucSendMode;
		glTlmSysParam._TmsPSTNPara.ucSignalLevel = glTlmSysParam._TxnPSTNPara.ucSignalLevel;
		memcpy(&glTlmSysParam._TmsModemPara, &glTlmSysParam._TxnModemPara, sizeof(COMM_PARA));
		break;

	case CT_TCPIP:
#ifdef	__TELIUM_PARAM__
		SetTcpIpSharedPara(&glTlmSysParam.stTxnCommCfg);
		SetTcpIpParam(&glTlmSysParam._TxnTcpIpPara);
		SyncTcpIpParam(&glTlmSysParam._TmsTcpIpPara, &glTlmSysParam._TxnTcpIpPara);
		DispWait();
		CommInitModule(&glTlmSysParam.stTxnCommCfg);
		
#else
		SetTcpIpSharedPara(&glSysParam.stTxnCommCfg);
		SetTcpIpParam(&glSysParam._TxnTcpIpPara);
		SyncTcpIpParam(&glSysParam._TmsTcpIpPara, &glSysParam._TxnTcpIpPara);
		DispWait();
		CommInitModule(&glSysParam.stTxnCommCfg);
		
#endif		
	    break;

	case CT_GPRS:
	case CT_CDMA:
#ifdef	__TELIUM_PARAM__
		SetTcpIpSharedPara(&glTlmSysParam.stTxnCommCfg);
		SetAPN(); // zaki 13 Januari 2014
		//SetWirelessParam(&glTlmSysParam._TxnWirlessPara); //ditutup zaki 13 Januari 2014
		SyncWirelessParam(&glTlmSysParam._TmsWirlessPara, &glTlmSysParam._TxnWirlessPara);
		CommOnHook(TRUE);
		DispWait();
		CommInitModule(&glTlmSysParam.stTxnCommCfg);
		
#else
		SetTcpIpSharedPara(&glSysParam.stTxnCommCfg);
		SetAPN(); // zaki 13 Januari 2014
		//SetWirelessParam(&glTlmSysParam._TxnWirlessPara); //ditutup zaki 13 Januari 2014
		SyncWirelessParam(&glSysParam._TmsWirlessPara, &glSysParam._TxnWirlessPara);
		CommOnHook(TRUE);
		DispWait();
		CommInitModule(&glSysParam.stTxnCommCfg);
		
#endif		
		break;

//	case CT_WIFI:
//		...
//		CommInitModule(&glSysParam.stTxnCommCfg);
//		break;

	case CT_DEMO:
	default:
	    break;
	}

	return 0;
}

// 输入PABX
int SetPabx(void)
{
	PubDispString("MODIFY PABX", DISP_LINE_LEFT|2);
#ifdef	__TELIUM_PARAM__
	if( PubGetString(ALPHA_IN|ECHO_IN, 0, 10, glTlmSysParam.stEdcInfo.szPabx, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
#else
	if( PubGetString(ALPHA_IN|ECHO_IN, 0, 10, glSysParam.stEdcInfo.szPabx, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
#endif
	return 0;
}

// 修改Modem参数
int SetModemParam(void)
{
	uchar	ucKey, szBuff[20], ucCurBaud, ucCnt, ucTemp;
	static	char	*ppszBaudRate[4] = {"1200", "2400", "9600", "14400"};

	if( SetPabx()!=0 )
	{
		return ERR_USERCANCEL;
	}

    //---------------------------------------------------
#ifdef	__TELIUM_PARAM__
	ucTemp = glTlmSysParam.stEdcInfo.bPreDial;
#else
	ucTemp = glSysParam.stEdcInfo.bPreDial;
#endif
	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("PRE DIAL", DISP_LINE_CENTER|2);
		sprintf((char *)szBuff, "(%c)", ucTemp ? '1' : '0');
		PubDispString(szBuff,       DISP_LINE_CENTER|4);
		PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if ( ucKey==KEY0 )
		{
			ucTemp = FALSE;
		}
		else if ( ucKey==KEY1 )
		{
			ucTemp = TRUE;
		}
	}
#ifdef	__TELIUM_PARAM__
	glTlmSysParam.stEdcInfo.bPreDial = ucTemp;
#else
	glSysParam.stEdcInfo.bPreDial = ucTemp;
#endif
    //---------------------------------------------------
#ifdef	__TELIUM_PARAM__
	ucTemp = glTlmSysParam._TxnModemPara.DP;
#else
	ucTemp = glSysParam._TxnModemPara.DP;
#endif
	while( 2 )
	{
		ScrClrLine(2, 7);
		sprintf((char *)szBuff, "(%c)",  ucTemp ? '1' : '0');
		PubDispString("DIAL MODE",      DISP_LINE_CENTER|2);
		PubDispString(szBuff,           DISP_LINE_CENTER|4);
		PubDispString("0.DTMF 1.PULSE", DISP_LINE_CENTER|6);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if ( ucKey==KEY0 )
		{
			 ucTemp = 0;
		}
		else if ( ucKey==KEY1 )
		{
			ucTemp = 1;
		}
	}
#ifdef	__TELIUM_PARAM__
	glTlmSysParam._TxnModemPara.DP = ucTemp; 
#else
	glSysParam._TxnModemPara.DP = ucTemp;
#endif

    //---------------------------------------------------

#ifdef	__TELIUM_PARAM__
	ucTemp = glTlmSysParam._TxnModemPara.CHDT;
#else
	ucTemp = glSysParam._TxnModemPara.CHDT;
#endif
	while( 3 )
	{
		ScrClrLine(2, 7);
		sprintf((char *)szBuff, "(%c)", ucTemp ? '1' : '0');
		PubDispString("DIAL TONE",        DISP_LINE_CENTER|2);
		PubDispString(szBuff,             DISP_LINE_CENTER|4);
		PubDispString("0.DETECT 1IGNORE", DISP_LINE_CENTER|6);
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if ( ucKey==KEY0 )
		{
			ucTemp = 0;
		}
		else if ( ucKey==KEY1 )
		{
			ucTemp = 1;
		}
	}
#ifdef	__TELIUM_PARAM__
	glTlmSysParam._TxnModemPara.CHDT = ucTemp;
#else
	glSysParam._TxnModemPara.CHDT = ucTemp;
#endif
    //---------------------------------------------------
	ScrClrLine(2, 7);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "OLD:%u(*100ms)", (uint)glTlmSysParam._TxnModemPara.DT1);
#else
	sprintf((char *)szBuff, "OLD:%u(*100ms)", (uint)glSysParam._TxnModemPara.DT1);
#endif
	PubDispString(szBuff,       DISP_LINE_LEFT|2);
	PubDispString("DIAL WAIT:", DISP_LINE_LEFT|4);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "%u", (uint)glTlmSysParam._TxnModemPara.DT1);
#else
	sprintf((char *)szBuff, "%u", (uint)glSysParam._TxnModemPara.DT1);
#endif
	if( PubGetString(NUM_IN|ECHO_IN, 0, 2, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
#ifdef __TELIUM_PARAM__
	glTlmSysParam._TxnModemPara.DT1 = (uchar)atoi((char *)szBuff);
#else
	glSysParam._TxnModemPara.DT1 = (uchar)atoi((char *)szBuff);
#endif
    //---------------------------------------------------
	ScrClrLine(2, 7);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "OLD:%u(*100ms)", (uint)glTlmSysParam._TxnModemPara.DT2);
#else
	sprintf((char *)szBuff, "OLD:%u(*100ms)", (uint)glSysParam._TxnModemPara.DT2);
#endif
	PubDispString(szBuff,        DISP_LINE_LEFT|2);
	PubDispString("PABX PAUSE:", DISP_LINE_LEFT|4);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff,"%u", (uint)glTlmSysParam._TxnModemPara.DT2);
#else	
	sprintf((char *)szBuff,"%u", (uint)glSysParam._TxnModemPara.DT2);
#endif
	if( PubGetString(NUM_IN|ECHO_IN, 0, 2, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
 	}
#ifdef	__TELIUM_PARAM__
	glTlmSysParam._TxnModemPara.DT2 = (uchar)atoi((char *)szBuff);
#else
	glSysParam._TxnModemPara.DT2 = (uchar)atoi((char *)szBuff);
#endif
    //---------------------------------------------------
	ScrClrLine(2, 7);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "OLD:%u(ms)", (uint)glTlmSysParam._TxnModemPara.HT);
#else
	sprintf((char *)szBuff, "OLD:%u(ms)", (uint)glSysParam._TxnModemPara.HT);
#endif
	PubDispString(szBuff,           DISP_LINE_LEFT|2);
	PubDispString("ONE DTMF HOLD:", DISP_LINE_LEFT|4);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "%u", (uint)glTlmSysParam._TxnModemPara.HT);
#else
	sprintf((char *)szBuff, "%u", (uint)glSysParam._TxnModemPara.HT);
#endif
	if( PubGetString(NUM_IN|ECHO_IN, 0, 3, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
#ifdef	__TELIUM_PARAM__
	glTlmSysParam._TxnModemPara.HT = (uchar)atoi((char *)szBuff);
#else
	glSysParam._TxnModemPara.HT = (uchar)atoi((char *)szBuff);
#endif

    //---------------------------------------------------
	ScrClrLine(2, 7);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "OLD:%u(*10ms)", (uint)glTlmSysParam._TxnModemPara.WT);
#else
	sprintf((char *)szBuff, "OLD:%u(*10ms)", (uint)glSysParam._TxnModemPara.WT);
#endif
	PubDispString(szBuff,             DISP_LINE_LEFT|2);
	PubDispString("DTMF CODE SPACE:", DISP_LINE_LEFT|4);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "%u", (uint)glTlmSysParam._TxnModemPara.WT);
#else
	sprintf((char *)szBuff, "%u", (uint)glSysParam._TxnModemPara.WT);
#endif
	if( PubGetString(NUM_IN|ECHO_IN, 0, 3, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
#ifdef	__TELIUM_PARAM__
	glTlmSysParam._TxnModemPara.WT = (uchar)atoi((char *)szBuff);
#else
	glSysParam._TxnModemPara.WT = (uchar)atoi((char *)szBuff);
#endif

    //---------------------------------------------------
    while (1)
    {
	    ScrClrLine(2, 7);
#ifdef	__TELIUM_PARAM__
	    sprintf((char *)szBuff, "OLD:%u(0, 1~15)", (uint)glTlmSysParam._TxnPSTNPara.ucSignalLevel);
#else
	    sprintf((char *)szBuff, "OLD:%u(0, 1~15)", (uint)glSysParam._TxnPSTNPara.ucSignalLevel);
#endif
	    PubDispString(szBuff,             DISP_LINE_LEFT|2);
	    PubDispString("SIGNAL LEVEL:", DISP_LINE_LEFT|4);
#ifdef	__TELIUM_PARAM__
	    sprintf((char *)szBuff, "%u", (uint)glTlmSysParam._TxnPSTNPara.ucSignalLevel);
#else
  	    sprintf((char *)szBuff, "%u", (uint)glSysParam._TxnPSTNPara.ucSignalLevel);
#endif
	    if( PubGetString(NUM_IN|ECHO_IN, 0, 2, szBuff, USER_OPER_TIMEOUT)!=0 )
	    {
		    return ERR_USERCANCEL;
	    }
        ucTemp = (uchar)atoi((char *)szBuff);
        if (ucTemp<16)
        {
#ifdef	__TELIUM_PARAM__
	        glTlmSysParam._TxnPSTNPara.ucSignalLevel = ucTemp;
#else
		glSysParam._TxnPSTNPara.ucSignalLevel = ucTemp;
#endif
            break;
        }
    }

    //---------------------------------------------------
#ifdef	__TELIUM_PARAM__
	ucCurBaud = (glTlmSysParam._TxnModemPara.SSETUP>>5) & 0x03;
#else
	ucCurBaud = (glSysParam._TxnModemPara.SSETUP>>5) & 0x03;
#endif
	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("BAUD RATE:", 2|DISP_LINE_LEFT);
		for(ucCnt=0; ucCnt<4; ucCnt++)
		{
			ScrPrint(0, (uchar)(ucCnt+4), ASCII, "   %s%d.%s", (ucCnt==ucCurBaud ? ">" : " "),
					(int)(ucCnt+1), ppszBaudRate[ucCnt]);
		}
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if( ucKey==KEYUP )
		{
			ucCurBaud = (ucCurBaud==0) ? 3 : ucCurBaud-1;
		}
		else if( ucKey==KEYDOWN )
		{
			ucCurBaud = (ucCurBaud>=3) ? 0 : ucCurBaud+1;
		}
		else if( ucKey>=KEY1 && ucKey<=KEY4 )
		{
			ucCurBaud = ucKey - KEY1;
			break;
		}
	}
#ifdef	__TELIUM_PARAM__
	glTlmSysParam._TxnModemPara.SSETUP &= 0x9F;	// 1001 1111
	glTlmSysParam._TxnModemPara.SSETUP |= (ucCurBaud<<5);
#else
	glSysParam._TxnModemPara.SSETUP &= 0x9F;	// 1001 1111
	glSysParam._TxnModemPara.SSETUP |= (ucCurBaud<<5);
#endif
	return 0;
}

int GetDownLoadRemoteIP(void)
{
	int	iRet;

	
	iRet= GetRemoteIp("PRIMARY",FALSE, &glTlmSysParam.stEdcInfo.stDownIPPriNo);
	if (iRet != 0)
	{
		return ERR_USERCANCEL;
	}
	iRet= GetRemoteIp("SECONDARY",FALSE, &glTlmSysParam.stEdcInfo.stDownIPSecNo);
	if (iRet != 0)
	{
		return ERR_USERCANCEL;
	}
	
	return 0;
	}

int GetIpLocalSettings(void *pstParam)
{
	int		iRet;
	TCPIP_PARA *pstTcpipPara;

	pstTcpipPara = (TCPIP_PARA *)pstParam;

	iRet = GetIPAddress((uchar *)"LOCAL IP", TRUE, pstTcpipPara->szLocalIP);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetIPAddress((uchar *)"IP MASK", TRUE, pstTcpipPara->szNetMask);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetIPAddress((uchar *)"GATEWAY IP", TRUE, pstTcpipPara->szGatewayIP);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	iRet = GetIPAddress((uchar *)"DNS", TRUE, pstTcpipPara->szDNSIP);
	if( iRet!=0 )
	{
		return iRet;
	}

	return 0;
}

#ifdef	__TELIUM_PARAM__
int GetTlmRemoteIp(uchar *pszHalfText, uchar bAllowNull, uchar *pstIPAddr, uchar *pstPort)
{
	int		iRet;
	uchar	szBuff[32];

	sprintf((char *)szBuff, "%s IP", pszHalfText);
	iRet = GetIPAddress(szBuff, bAllowNull, pstIPAddr);
	if( iRet!=0 )
	{
		return iRet;
	}

	sprintf((char *)szBuff, "%s PORT", pszHalfText);
	iRet = GetIPPort(szBuff, bAllowNull, pstPort);
	if( iRet<0 )
	{
		return iRet;
	}

	return 0;
}
#endif

int GetRemoteIp(uchar *pszHalfText, uchar bAllowNull, void *pstIPAddr)
{
	int		iRet;
	IP_ADDR	*pstIp;
	uchar	szBuff[32];

	pstIp = (IP_ADDR *)pstIPAddr;
	
	sprintf((char *)szBuff, "%s IP", pszHalfText);
	//strcpy(pstIp->szIP, "010.011.012.013");
	iRet = GetIPAddress(szBuff, bAllowNull, pstIp->szIP);
	if( iRet!=0 )
	{
		return iRet;
	}

	sprintf((char *)szBuff, "%s PORT", pszHalfText);	
	//strcpy(pstIp->szPort, "2014");
	iRet = GetIPPort(szBuff, bAllowNull, pstIp->szPort);
	if( iRet<0 )
	{
		return iRet;
	}

	return 0;
}

int ChkIfValidIp(uchar *pszIP)
{
	return ((pszIP[0]!=0) && (ValidIPAddress(pszIP)));
}

int ChkIfValidPort(uchar *pszPort)
{
	return ((pszPort[0]!=0) &&
			(atol((uchar *)pszPort)>0) &&
			(atol((uchar *)pszPort)<65536));
}

int SetTcpIpSharedPara(COMM_CONFIG *pstCommCfg)
{
	uchar	ucKey;
	uchar	szDispBuff[128];

	while (1)
	{
		ScrClrLine(2, 7);
		sprintf((char *)szDispBuff, "TCP LENGTH: %s", (pstCommCfg->ucTCPClass_BCDHeader ? "BCD" : "HEX"));
	
		ucKey = PubSelectOneTwo((char *)szDispBuff, (uchar *)_T("BCD"), (uchar *)_T("HEXIMAL"));
		if (ucKey==KEYCANCEL)
		{
			return -1;
		}
		if (ucKey==KEYENTER)
		{
			return 0;
		}
		if (ucKey==KEY1)
		{
			pstCommCfg->ucTCPClass_BCDHeader = TRUE;
			return 0;
		}
		if (ucKey==KEY2)
		{
			pstCommCfg->ucTCPClass_BCDHeader = FALSE;
			return 0;
		}
	}
}

// 设置TCP/IP参数
int SetTcpIpParam(void *pstParam)
{
	int		iRet;

	// !!!! 需要应用到开机步骤

#ifdef SXX_IP_MODULE
    iRet = SetTcpIpParam_S80((TCPIP_PARA *)pstParam);
	return iRet;

#else
	iRet = GetIpLocalSettings((TCPIP_PARA *)pstParam);
	if( iRet!=0 )
	{
		return iRet;
	}

	return 0;

#endif
}
void SyncTcpIpParam(void *pstDst, void *pstSrc)
{
	((TCPIP_PARA *)pstDst)->ucDhcp = ((TCPIP_PARA *)pstSrc)->ucDhcp;
	strcpy((char *)(((TCPIP_PARA *)pstDst)->szLocalIP),   (char *)(((TCPIP_PARA *)pstSrc)->szLocalIP));
	strcpy((char *)(((TCPIP_PARA *)pstDst)->szGatewayIP), (char *)(((TCPIP_PARA *)pstSrc)->szGatewayIP));
	strcpy((char *)(((TCPIP_PARA *)pstDst)->szNetMask),   (char *)(((TCPIP_PARA *)pstSrc)->szNetMask));
	strcpy((char *)(((TCPIP_PARA *)pstDst)->szDNSIP),     (char *)(((TCPIP_PARA *)pstSrc)->szDNSIP));
}

#ifdef SXX_IP_MODULE
int SetTcpIpParam_S80(TCPIP_PARA *pstParam)
{
	int		iRet;
	uchar	ucKey;
	uchar	szDispBuff[32];
	long lTcpState;

	while (1)
	{
		ScrClrLine(2, 7);
		iRet = DhcpCheck();
		if (iRet==0)
		{
			sprintf((char *)szDispBuff, "DHCP: OK");
		}
		else
		{
			sprintf((char *)szDispBuff, "DHCP: STOPPED");
		}
	
		ucKey = PubSelectOneTwo((char *)szDispBuff, (uchar *)_T("START"), (uchar *)_T("STOP"));
		if (ucKey==KEYCANCEL)
		{
			return -1;
		}
		if (ucKey==KEYENTER)
		{
			continue;
		}
		if (ucKey==KEY1)
		{
			pstParam->ucDhcp = 1;

			ScrClrLine(2, 7);
			PubDispString("GETTING IP...", 4|DISP_LINE_LEFT);
			if (SxxDhcpStart(FALSE, 30)==0)
			{
				iRet = EthGet(pstParam->szLocalIP, pstParam->szNetMask, pstParam->szGatewayIP, pstParam->szDNSIP, &lTcpState);
				
				ScrClrLine(2, 7);
				PubDispString("LOCAL IP", 2|DISP_LINE_LEFT);
				PubDispString(pstParam->szLocalIP, 6|DISP_LINE_RIGHT);
				getkey();
				
				ScrClrLine(2, 7);
				PubDispString("IP MASK", 2|DISP_LINE_LEFT);
				PubDispString(pstParam->szNetMask, 6|DISP_LINE_RIGHT);
				getkey();

				ScrClrLine(2, 7);
				PubDispString("GATEWAY IP", 2|DISP_LINE_LEFT);
				PubDispString(pstParam->szGatewayIP, 6|DISP_LINE_RIGHT);
				getkey();

				ScrClrLine(2, 7);
				PubDispString("DNS", 2|DISP_LINE_LEFT);
				PubDispString(pstParam->szDNSIP, 6|DISP_LINE_RIGHT);
				getkey();

				return 0;
			}
		}
		if (ucKey==KEY2)
		{
			pstParam->ucDhcp = 0;
			break;
		}
	}

	// Manual setup
	if (iRet == 0)
	{
		DhcpStop();
	}

	if (pstParam->ucDhcp)
	{
		iRet = EthGet(pstParam->szLocalIP, pstParam->szNetMask, pstParam->szGatewayIP, pstParam->szDNSIP, &lTcpState);
	}

	iRet = GetIpLocalSettings(pstParam);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = EthSet(pstParam->szLocalIP, pstParam->szNetMask, pstParam->szGatewayIP, pstParam->szDNSIP);
	if (iRet < 0)
	{
		ScrClrLine(2, 7);
		PubDispString("SET STATIC IP", 2|DISP_LINE_LEFT);
		PubDispString("FAILED.",       4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(USER_OPER_TIMEOUT);
		return -1;
	}

	return 0;
}
#endif

// 输入IP地址
int GetIPAddress(uchar *pszPrompts, uchar bAllowNull, uchar *pszIPAddress)
{
	uchar	szTemp[20], szDispBuff[20];

	sprintf((char *)szTemp, "%.15s", pszIPAddress);
	while( 1 )
	{
		ScrClrLine(2, 7);
		sprintf((char *)szDispBuff, "%.16s", pszPrompts);
		PubDispString(szDispBuff, 2|DISP_LINE_LEFT);
		if( PubGetString(ALPHA_IN|ECHO_IN, 0, 15, szTemp, USER_OPER_TIMEOUT)!=0 )
		{
			return ERR_USERCANCEL;
		}
		if( bAllowNull && szTemp[0]==0 )
		{
			*pszIPAddress = 0;
			break;
		}
		if( ValidIPAddress((char *)szTemp) )
		{
			sprintf((char *)pszIPAddress, "%.15s", szTemp);
			break;
		}

		ScrClrLine(4, 7);
		PubDispString(_T("INV IP ADDR"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(4);
	}

	return 0;
}

// 检查IP地址
uchar ValidIPAddress(char *pszIPAddr)
{
	int		i;
	char	*p, *q, szBuf[5+1];

	PubTrimStr((uchar *)pszIPAddr);
	if( *pszIPAddr==0 )
	{
		return FALSE;
	}

	p = strchr(pszIPAddr, ' ');
	if( p!=NULL )
	{
		return FALSE;
	}
	if( strlen(pszIPAddr)>15 )
	{
		return FALSE;
	}

	// 1st --- 3rd  part
	for(q=pszIPAddr, i=0; i<3; i++)
	{
		p = strchr(q, '.');
		if( p==NULL || p==q || p-q>3 )
		{
			return FALSE;
		}
		sprintf(szBuf, "%.*s", (int)(p-q), q);
		if( !IsNumStr(szBuf) || atoi(szBuf)>255 )
		{
			return FALSE;
		}
		q = p + 1;
	}

	// final part
	p = strchr((char *)q, '.');
	if( p!=NULL || !IsNumStr(q) || strlen(q)==0 || strlen(q)>3 || atoi(q)>255 )
	{
		return FALSE;
	}

	return TRUE;
}

// 输入端口
int GetIPPort(uchar *pszPrompts, uchar bAllowNull, uchar *pszPortNo)
{
	int		iTemp;
	uchar	ucMinLen, szTemp[15], szDispBuff[20];

	ucMinLen = (bAllowNull ? 0 : 1);
	while( 1 )
	{
		sprintf((char *)szTemp, "%.5s", pszPortNo);
		ScrClrLine(2, 7);
		sprintf((char *)szDispBuff, "%.16s", pszPrompts);
		PubDispString(szDispBuff, 2|DISP_LINE_LEFT);
		if( PubGetString(NUM_IN|ECHO_IN, ucMinLen, 5, szTemp, USER_OPER_TIMEOUT)!=0 )
		{
			return ERR_USERCANCEL;
		}
		iTemp = atoi((char *)szTemp);
		if( iTemp>0 && iTemp<65535 )
		{
			sprintf((char *)pszPortNo, "%.5s", szTemp);
			break;
		}
		if (bAllowNull)
		{
			pszPortNo[0] = 0;
			break;
		}

		ScrClrLine(4, 7);
		PubDispString(_T("INV PORT #"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(4);
	}

	return 0;
}

// 选择发卡行并修改参数
void SetIssuerParam(uchar ucPermission)
{
	int		iRet;
	uchar	ucIndex, szBuff[32], szBuff2[32];
#ifdef ENABLE_EMV
	int		iCnt;
	ulong	ulTemp;
	EMV_APPLIST	stEmvApp;
#endif

	while (1)
	{
		iRet = SelectIssuer(&ucIndex);
		if( iRet!=0 )
		{
			return;
		}

#ifdef	__TELIUM_PARAM__
		sprintf((char *)szBuff, "ISSUER: %-8.8s", (char *)glCurTlmTmsIss.issRef);
#else
		sprintf((char *)szBuff, "ISSUER: %-8.8s", (char *)glCurIssuer.szName);
#endif
		PubShowTitle(TRUE, szBuff);
		ModifyOptList(glSysParam.stIssuerList[ucIndex].sOption, 'I', ucPermission);

		if (ucPermission==PM_HIGH)
		{
			while (1)
			{
				PubShowTitle(TRUE, szBuff);
				PubDispString(_T("NON-EMV FLOOR"),  DISP_LINE_LEFT|2);
				PubDispString(_T("EXCLUDE DECIMAL"),  DISP_LINE_LEFT|4);
#ifdef	__TELIUM_PARAM__
				sprintf((char *)szBuff2, "%lu", glTlmSysParam.Iss[ucIndex].issFloorLimit);
#else
				sprintf((char *)szBuff2, "%lu", glSysParam.stIssuerList[ucIndex].ulFloorLimit);
#endif
				iRet = PubGetString(NUM_IN|ECHO_IN, 1, 2, szBuff2, USER_OPER_TIMEOUT);
				if( iRet!=0 )
				{
					break;
				}
#ifdef	__TELIUM_PARAM__
				glTlmSysParam.Iss[ucIndex].issFloorLimit = atoi(szBuff2);
#else
				glSysParam.stIssuerList[ucIndex].ulFloorLimit = (ulong)atoi(szBuff2);
#endif
				break;
			}

#ifdef ENABLE_EMV
			while (1)
			{
				PubShowTitle(TRUE, szBuff);
				PubDispString(_T("EMV FLOOR LIMIT"),  DISP_LINE_LEFT|2);
				PubDispString(_T("EXCLUDE DECIMAL"),  DISP_LINE_LEFT|4);
				memset(szBuff2, 0, sizeof(szBuff2));
				iRet = PubGetString(NUM_IN|ECHO_IN, 1, 4, szBuff2, USER_OPER_TIMEOUT);
				if( iRet!=0 )
				{
					break;
				}
				ulTemp = (ulong)atoi(szBuff2);
#ifdef	__TELIUM_PARAM__
				for (iCnt=0;
					iCnt<glTlmSysParam.stEdcInfo.stLocalCurrency.ucDecimal+glTlmSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit;
					iCnt++)
				{
					ulTemp *= 10;
				}
#else
				for (iCnt=0;
					iCnt<glSysParam.stEdcInfo.stLocalCurrency.ucDecimal+glSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit;
					iCnt++)
				{
					ulTemp *= 10;
				}
#endif
				for(iCnt=0; iCnt<MAX_APP_NUM; iCnt++)
				{
					memset(&stEmvApp, 0, sizeof(EMV_APPLIST));
					iRet = EMVGetApp(iCnt, &stEmvApp);
					if( iRet!=EMV_OK )
					{
						continue;
					}
					stEmvApp.FloorLimit = ulTemp;
					iRet = EMVAddApp(&stEmvApp);
				}

				break;
			}
#endif

			if (ChkEdcOption(EDC_TIP_PROCESS))
			{
				while (1)
				{
					PubShowTitle(TRUE, szBuff);
					PubDispString(_T("ADJUST PERCENT"),  DISP_LINE_LEFT|2);
#ifdef	__TELIUM_PARAM__
					sprintf((char *)szBuff2, "%d", (int)(glTlmSysParam.Iss[ucIndex].ucAdjustPercent%100));
#else
					sprintf((char *)szBuff2, "%d", (int)(glSysParam.stIssuerList[ucIndex].ucAdjustPercent%100));
#endif
					if( PubGetString(NUM_IN|ECHO_IN, 1, 6, szBuff2, USER_OPER_TIMEOUT)!=0 )
					{
						break;
					}
					if (atoi(szBuff2)>50)
					{
						continue;
					}

#ifdef	__TELIUM_PARAM__
					glTlmSysParam.Iss[ucIndex].ucAdjustPercent = (uchar)atoi(szBuff2);
#else
					glSysParam.stIssuerList[ucIndex].ucAdjustPercent = (uchar)atoi(szBuff2);
#endif
					break;
				}
			}

#ifndef	__TELIUM_PARAM__
			if (ChkEdcOption(EDC_REFERRAL_DIAL))
			{
				while (1)
				{
					PubShowTitle(TRUE, szBuff);
					PubDispString(_T("REFERRAL TEL"),  DISP_LINE_LEFT|2);
					sprintf((char *)szBuff2, "%.12s", (char *)glSysParam.stIssuerList[ucIndex].szRefTelNo);
					if( PubGetString(NUM_IN|ECHO_IN, 0, 12, szBuff2, USER_OPER_TIMEOUT)!=0 )
					{
						break;
					}

					sprintf((char *)glSysParam.stIssuerList[ucIndex].szRefTelNo, "%.12s", (char *)szBuff2);
					break;
				}
			}
#endif			
		}

#ifdef	__TELIUM_PARAM__
		SaveTlmSysParam();
		if( glTlmSysParam.ucIssNum<2 )
		{
			break;
		}
#else
		SaveSysParam();
		if( glSysParam.ucIssuerNum<2 )
		{
			break;
		}
#endif
		
	}
}

void SetTIDMID(uchar ucPermission)
{
	int		iRet;
	uchar	szTitle[16+1],ucKey;

	while (1)
	{
#ifdef	__TELIUM_PARAM__
		sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>9 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#else
		sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>9 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#endif
		iRet = SelectAcq(FALSE, szTitle, NULL);
		if( iRet!=0 )
		{
			return;
		}

#ifdef	__TELIUM_PARAM__
		sprintf((char *)szTitle, "ACQ: %-10.10s ", (char *)glCurTlmTmsAcq.acqName);
#else
		sprintf((char *)szTitle, "ACQ: %-10.10s ", (char *)glCurAcq.szName);
#endif
		while( 1 )
		{	
			ucKey = DispAcqPama();
			if( ucKey==KEYCANCEL || ucKey==NOKEY )
			{
				break;
			}
			
			if (ucPermission>PM_MEDIUM)
			{
				PubShowTitle(TRUE, szTitle);
				PubDispString("ACQUIRER NAME", DISP_LINE_LEFT|2);
#ifdef	__TELIUM_PARAM__
				if( PubGetString(NUM_IN|ALPHA_IN|ECHO_IN, 3, 10, glCurTlmTmsAcq.acqName, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}
#else
				if( PubGetString(NUM_IN|ALPHA_IN|ECHO_IN, 3, 10, glCurAcq.szName, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}
#endif
			}

			if (ucPermission>PM_LOW)
			{
				PubShowTitle(TRUE, szTitle);
#ifdef	__TELIUM_PARAM__
				PubDispString("TERMINAL ID", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 8, 8, glCurTlmTmsAcq.acqTID, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

				ScrClrLine(2, 7);
				PubDispString("MERCHANT ID", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 8, 15, glCurTlmTmsAcq.acqMID, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

			
#else
				PubDispString("TERMINAL ID", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 8, 8, glCurAcq.szTermID, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

				ScrClrLine(2, 7);
				PubDispString("MERCHANT ID", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 8, 15, glCurAcq.szMerchantID, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

		

#endif
			}
			break;
		}

#ifdef	__TELIUM_PARAM__
		memcpy(&glTlmSysParam.Acq[glCurTlmTmsAcq.ucIndex], &glCurTlmTmsAcq, sizeof(TLM_ACQ));
		SaveTlmSysParam();
		if (glTlmSysParam.ucAcqNum<2)
		{
			break;
		}
#else
		memcpy(&glSysParam.stAcqList[glCurAcq.ucIndex], &glCurAcq, sizeof(ACQUIRER));
		SaveSysParam();
		if (glSysParam.ucAcqNum<2)
		{
			break;
		}
#endif
	}
}

void InfoAcquirers(uchar ucPermission)
{
	int		iRet;
	uchar	szTitle[16+1],ucKey;

	while (1)
	{
#ifdef	__TELIUM_PARAM__
		sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>9 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#else
		sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>9 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#endif
		iRet = SelectAcq(FALSE, szTitle, NULL);
		if( iRet!=0 )
		{
			return;
		}

#ifdef	__TELIUM_PARAM__
		sprintf((char *)szTitle, "ACQ: %-10.10s ", (char *)glCurTlmTmsAcq.acqName);
#else
		sprintf((char *)szTitle, "ACQ: %-10.10s ", (char *)glCurAcq.szName);
#endif
		while( 1 )
		{	
			ucKey = DispAcqPama();
			if( ucKey==KEYCANCEL || ucKey==NOKEY )
			{
				break;
			}
			
			if (ucPermission>PM_MEDIUM)
			{
				PubShowTitle(TRUE, szTitle);
				PubDispString("ACQUIRER NAME", DISP_LINE_LEFT|2);
#ifdef	__TELIUM_PARAM__
				if( PubGetString(NUM_IN|ALPHA_IN|ECHO_IN, 3, 10, glCurTlmTmsAcq.acqName, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}
#else
				if( PubGetString(NUM_IN|ALPHA_IN|ECHO_IN, 3, 10, glCurAcq.szName, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}
#endif
			}

			if (ucPermission>PM_LOW)
			{
				PubShowTitle(TRUE, szTitle);

			
			}
			break;
		}

#ifdef	__TELIUM_PARAM__
		memcpy(&glTlmSysParam.Acq[glCurTlmTmsAcq.ucIndex], &glCurTlmTmsAcq, sizeof(TLM_ACQ));
		SaveTlmSysParam();
		if (glTlmSysParam.ucAcqNum<2)
		{
			break;
		}
#else
		memcpy(&glSysParam.stAcqList[glCurAcq.ucIndex], &glCurAcq, sizeof(ACQUIRER));
		SaveSysParam();
		if (glSysParam.ucAcqNum<2)
		{
			break;
		}
#endif
	}
}


// 设置收单行参数
void SetAcqParam(uchar ucPermission)
{
	int		iRet;
	uchar	ucKey, szTitle[16+1], szBuff[16+1];

	while (1)
	{
#ifdef	__TELIUM_PARAM__
		sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>9 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#else
		sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>9 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#endif
		iRet = SelectAcq(FALSE, szTitle, NULL);
		if( iRet!=0 )
		{
			return;
		}

#ifdef	__TELIUM_PARAM__
		sprintf((char *)szTitle, "ACQ: %-10.10s ", (char *)glCurTlmTmsAcq.acqName);
#else
		sprintf((char *)szTitle, "ACQ: %-10.10s ", (char *)glCurAcq.szName);
#endif
		while( 1 )
		{
			ucKey = DispAcqPama();
			if( ucKey==KEYCANCEL || ucKey==NOKEY )
			{
				break;
			}

			// set all bit flag options
			ScrCls();
			PubShowTitle(TRUE, szTitle);
			if( ModifyOptList(glCurAcq.sOption, 'A', ucPermission)!=0 )
			{
				break;
			}

			if (ucPermission>PM_MEDIUM)
			{
				PubShowTitle(TRUE, szTitle);
				PubDispString("ACQUIRER NAME", DISP_LINE_LEFT|2);
#ifdef	__TELIUM_PARAM__
				if( PubGetString(NUM_IN|ALPHA_IN|ECHO_IN, 3, 10, glCurTlmTmsAcq.acqName, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}
#else
				if( PubGetString(NUM_IN|ALPHA_IN|ECHO_IN, 3, 10, glCurAcq.szName, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}
#endif
			}

			if (ucPermission>PM_LOW)
			{
				PubShowTitle(TRUE, szTitle);
#ifdef	__TELIUM_PARAM__
				PubDispString("TERMINAL ID", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 8, 8, glCurTlmTmsAcq.acqTID, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

				ScrClrLine(2, 7);
				PubDispString("MERCHANT ID", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 8, 15, glCurTlmTmsAcq.acqMID, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

				ScrClrLine(2, 7);
				PubDispString("NII", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 3, 3, glCurTlmTmsAcq.acqNII, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

				ScrClrLine(2, 7);
				PubDispString("BATCH NO.", DISP_LINE_LEFT|2);
				sprintf((char *)szBuff, "%lu", glCurTlmTmsAcq.ulCurBatchNo);
				if( PubGetString(NUM_IN|ECHO_IN, 1, 6, szBuff, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}
				if (glCurTlmTmsAcq.ulCurBatchNo!=(ulong)atoi(szBuff))
				{
					glCurTlmTmsAcq.ulCurBatchNo  = (ulong)atoi(szBuff);
					glCurTlmTmsAcq.ulNextBatchNo = GetNewBatchNo(glCurTlmTmsAcq.ulCurBatchNo);
				}

				if( SetAcqTransComm(glTlmSysParam.stTxnCommCfg.ucCommType)!=0 )
				{
					break;
				}

				if( SetAcqTransComm(glTlmSysParam.stTxnCommCfg.ucCommTypeBak)!=0 )
				{
					break;
				}
#else
				PubDispString("TERMINAL ID", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 8, 8, glCurAcq.szTermID, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

				ScrClrLine(2, 7);
				PubDispString("MERCHANT ID", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 8, 15, glCurAcq.szMerchantID, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

				ScrClrLine(2, 7);
				PubDispString("NII", DISP_LINE_LEFT|2);
				if( PubGetString(NUM_IN|ECHO_IN, 3, 3, glCurAcq.szNii, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}

				ScrClrLine(2, 7);
				PubDispString("BATCH NO.", DISP_LINE_LEFT|2);
				sprintf((char *)szBuff, "%lu", glCurAcq.ulCurBatchNo);
				if( PubGetString(NUM_IN|ECHO_IN, 1, 6, szBuff, USER_OPER_TIMEOUT)!=0 )
				{
					break;
				}
				if (glCurAcq.ulCurBatchNo!=(ulong)atoi(szBuff))
				{
					glCurAcq.ulCurBatchNo  = (ulong)atoi(szBuff);
					glCurAcq.ulNextBatchNo = GetNewBatchNo(glCurAcq.ulCurBatchNo);
				}

				if( SetAcqTransComm(glSysParam.stTxnCommCfg.ucCommType)!=0 )
				{
					break;
				}

				if( SetAcqTransComm(glSysParam.stTxnCommCfg.ucCommTypeBak)!=0 )
				{
					break;
				}

#endif
			}
			break;
		}

#ifdef	__TELIUM_PARAM__
		memcpy(&glTlmSysParam.Acq[glCurTlmTmsAcq.ucIndex], &glCurTlmTmsAcq, sizeof(TLM_ACQ));
		SaveTlmSysParam();
		if (glTlmSysParam.ucAcqNum<2)
		{
			break;
		}
#else
		memcpy(&glSysParam.stAcqList[glCurAcq.ucIndex], &glCurAcq, sizeof(ACQUIRER));
		SaveSysParam();
		if (glSysParam.ucAcqNum<2)
		{
			break;
		}
#endif
	}
}

// 显示当前收单行参数信息
uchar DispAcqPama(void)
{
	int		ii;
	uchar	ucShowComm, ucKey;

	for (ii=0; ii<2; ii++)
	{
		ScrCls();
#ifdef	__TELIUM_PARAM__
		ScrPrint(0, 0, CFONT, "%-10.10s   %3.3s", glCurTlmTmsAcq.acqName, glCurTlmTmsAcq.acqNII);
		if (ii==0)
		{
			ScrPrint(0, 2, ASCII, "TID:%s", glCurTlmTmsAcq.acqTID);
			ScrPrint(0, 3, ASCII, "MID:%s", glCurTlmTmsAcq.acqMID);
		}
#else
		ScrPrint(0, 0, CFONT, "%-10.10s   %3.3s", glCurAcq.szName, glCurAcq.szNii);
		if (ii==0)
		{
			ScrPrint(0, 2, ASCII, "TID:%s", glCurAcq.szTermID);
			ScrPrint(0, 3, ASCII, "MID:%s", glCurAcq.szMerchantID);
		}
#endif
		else
		{
			ScrPrint(0, 3, ASCII, "SECONDARY COMM");
		}

#ifdef	__TELIUM_PARAM__
		ucShowComm = (ii ? glTlmSysParam.stTxnCommCfg.ucCommTypeBak : glTlmSysParam.stTxnCommCfg.ucCommType);
#else
		ucShowComm = (ii ? glSysParam.stTxnCommCfg.ucCommTypeBak : glSysParam.stTxnCommCfg.ucCommType);
#endif
		if (ucShowComm==CT_NONE)
		{
			continue;
		}
		else if ((ucShowComm==CT_TCPIP) || (ucShowComm==CT_WIFI))
		{
#ifdef	__TELIUM_PARAM__
			ScrPrint(0, 4, ASCII, "%.15s %.5s", glCurTlmTmsAcq.acqPriTxnIP, glCurTlmTmsAcq.acqPriTxnPort);
			ScrPrint(0, 5, ASCII, "%.15s %.5s", glCurTlmTmsAcq.acqSecTxnIP, glCurTlmTmsAcq.acqSecTxnPort);
			ScrPrint(0, 6, ASCII, "%.15s %.5s", glCurTlmTmsAcq.acqPriStlIP, glCurTlmTmsAcq.acqPriStlPort);
			ScrPrint(0, 7, ASCII, "%.15s %.5s", glCurTlmTmsAcq.acqSecTxnIP, glCurTlmTmsAcq.acqSecStlPort);
#else
			ScrPrint(0, 4, ASCII, "%.15s %.5s", glCurAcq.TxnTcpIp1, glCurAcq.TxnTcpPort1);
			ScrPrint(0, 5, ASCII, "%.15s %.5s", glCurAcq.TxnTcpIp2, glCurAcq.TxnTcpPort2);
			ScrPrint(0, 6, ASCII, "%.15s %.5s", glCurAcq.StlTcpIp1, glCurAcq.StlTcpPort1);
			ScrPrint(0, 7, ASCII, "%.15s %.5s", glCurAcq.StlTcpIp2, glCurAcq.StlTcpPort2);
#endif			
		}
		//else if (ucShowComm==CT_PPP)
		// ...
		else if ((ucShowComm==CT_GPRS) || (ucShowComm==CT_CDMA))
		{
#ifdef	__TELIUM_PARAM__
			ScrPrint(0, 4, ASCII, "%.15s %.5s", glCurTlmTmsAcq.acqPriTxnIP, glCurTlmTmsAcq.acqPriTxnPort);
			ScrPrint(0, 5, ASCII, "%.15s %.5s", glCurTlmTmsAcq.acqSecTxnIP, glCurTlmTmsAcq.acqSecTxnPort);
			ScrPrint(0, 6, ASCII, "%.15s %.5s", glCurTlmTmsAcq.acqPriStlIP, glCurTlmTmsAcq.acqPriStlPort);
			ScrPrint(0, 7, ASCII, "%.15s %.5s", glCurTlmTmsAcq.acqSecTxnIP, glCurTlmTmsAcq.acqSecStlPort);
#else
			ScrPrint(0, 4, ASCII, "%.15s %.5s", glCurAcq.TxnGPRSIp1, glCurAcq.TxnGPRSPort1);
			ScrPrint(0, 5, ASCII, "%.15s %.5s", glCurAcq.TxnGPRSIp2, glCurAcq.TxnGPRSPort2);
			ScrPrint(0, 6, ASCII, "%.15s %.5s", glCurAcq.StlGPRSIp1, glCurAcq.StlGPRSPort1);
			ScrPrint(0, 7, ASCII, "%.15s %.5s", glCurAcq.StlGPRSIp2, glCurAcq.StlGPRSPort2);
#endif			
		}
		else
		{
#ifdef	__TELIUM_PARAM__
			ScrPrint(0, 4, ASCII, "%.21s", glCurTlmTmsAcq.acqPriTxnPhnSrv);
			ScrPrint(0, 5, ASCII, "%.21s", glCurTlmTmsAcq.acqSecTxnPhnSrv);
			ScrPrint(0, 6, ASCII, "%.21s", glCurTlmTmsAcq.acqPriStlPhnSrv);
			ScrPrint(0, 7, ASCII, "%.21s", glCurTlmTmsAcq.acqSecStlPhnSrv);
#else
			ScrPrint(0, 4, ASCII, "%.21s", glCurAcq.TxnTelNo1);
			ScrPrint(0, 5, ASCII, "%.21s", glCurAcq.TxnTelNo2);
			ScrPrint(0, 6, ASCII, "%.21s", glCurAcq.StlTelNo1);
			ScrPrint(0, 7, ASCII, "%.21s", glCurAcq.StlTelNo2);
#endif			
		}

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if ((ucKey==KEYCANCEL) || (ucKey==NOKEY))
		{
			return ucKey;
		}
	}

	return ucKey;
}

int SetAcqTransComm(uchar ucCommType)
{
	int		iRet;
#ifdef	__TELIUM_PARAM__
	switch(ucCommType)
	{
	case CT_TCPIP:
	case CT_WIFI:
		iRet = GetTlmRemoteIp("TRANS IP 1",  TRUE, glCurTlmTmsAcq.acqPriTxnIP, glCurTlmTmsAcq.acqPriTxnPort);
		if (iRet)
			break;
		iRet = GetTlmRemoteIp("TRANS IP 2",  TRUE, glCurTlmTmsAcq.acqSecTxnIP, glCurTlmTmsAcq.acqSecTxnPort);
		if (iRet)
			break;
		iRet = GetTlmRemoteIp("SETTLE IP 1",  TRUE, glCurTlmTmsAcq.acqPriStlIP, glCurTlmTmsAcq.acqPriStlPort);
		if (iRet)
			break;
		iRet = GetTlmRemoteIp("SETTLE IP 2",  TRUE, glCurTlmTmsAcq.acqSecStlIP, glCurTlmTmsAcq.acqSecStlPort);
		if (iRet)
			break;//iRet = GetRemoteIp("SETTLE 1st", TRUE, &glCurAcq.stStlTCPIPInfo[0]);
		//iRet = GetRemoteIp("SETTLE 2nd", TRUE, &glCurAcq.stStlTCPIPInfo[1]);
		break;
	case CT_GPRS:
	case CT_CDMA:
		iRet = GetTlmRemoteIp("TRANS IP 1",  TRUE, glCurTlmTmsAcq.acqPriTxnIP, glCurTlmTmsAcq.acqPriTxnPort);
		if (iRet)
			break;
		iRet = GetTlmRemoteIp("TRANS IP 2",  TRUE, glCurTlmTmsAcq.acqSecTxnIP, glCurTlmTmsAcq.acqSecTxnPort);
		if (iRet)
			break;
		iRet = GetTlmRemoteIp("SETTLE IP 1",  TRUE, glCurTlmTmsAcq.acqPriStlIP, glCurTlmTmsAcq.acqPriStlPort);
		if (iRet)
			break;
		iRet = GetTlmRemoteIp("SETTLE IP 2",  TRUE, glCurTlmTmsAcq.acqSecStlIP, glCurTlmTmsAcq.acqSecStlPort);
		if (iRet)
			break;
		//iRet = GetRemoteIp("SETTLE 1st", TRUE, &glCurAcq.stStlGPRSInfo[0]);
		//iRet = GetRemoteIp("SETTLE 2nd", TRUE, &glCurAcq.stStlGPRSInfo[1]);
		break;
	case CT_MODEM:
		iRet = SetAcqTransTelNo();
	case CT_RS232:
	case CT_NONE:
	default:
		iRet = 0;
	    break;
	}
#else
	switch(ucCommType)
	{
	case CT_TCPIP:
	case CT_WIFI:
		iRet = GetRemoteIp("TRANS IP 1",  TRUE, &glCurAcq.stTxnTCPIPInfo[0]);
		if (iRet)
		{
			break;
		}
		iRet = GetRemoteIp("TRANS IP 2",  TRUE, &glCurAcq.stTxnTCPIPInfo[1]);
		//iRet = GetRemoteIp("SETTLE 1st", TRUE, &glCurAcq.stStlTCPIPInfo[0]);
		//iRet = GetRemoteIp("SETTLE 2nd", TRUE, &glCurAcq.stStlTCPIPInfo[1]);
		break;
	case CT_GPRS:
	case CT_CDMA:
		iRet = GetRemoteIp("TRANS IP 1",  TRUE, &glCurAcq.stTxnGPRSInfo[0]);
		if (iRet)
		{
			break;
		}
		iRet = GetRemoteIp("TRANS IP 2",  TRUE, &glCurAcq.stTxnGPRSInfo[1]);
		//iRet = GetRemoteIp("SETTLE 1st", TRUE, &glCurAcq.stStlGPRSInfo[0]);
		//iRet = GetRemoteIp("SETTLE 2nd", TRUE, &glCurAcq.stStlGPRSInfo[1]);
	    break;
		break;
	case CT_MODEM:
		iRet = SetAcqTransTelNo();
	case CT_RS232:
	case CT_NONE:
	default:
		iRet = 0;
	    break;
	}

#endif

	return iRet;
}

// 设置当前收单行交易电话号码
int SetAcqTransTelNo(void)
{
#ifdef	__TELIUM_PARAM__
	if( SetTel(glCurTlmTmsAcq.acqPriTxnPhnSrv, (uchar *)_T("TRANS TELNO 1"))!=0 )
	{
		return ERR_USERCANCEL;
	}
	memcpy(&glTlmSysParam.Acq[glCurTlmTmsAcq.ucIndex], &glCurTlmTmsAcq, sizeof(TLM_ACQ));

	if( SetTel(glCurTlmTmsAcq.acqSecTxnPhnSrv, (uchar *)_T("TRANS TELNO 2"))!=0 )
	{
		return ERR_USERCANCEL;
	}
	memcpy(&glTlmSysParam.Acq[glCurTlmTmsAcq.ucIndex], &glCurTlmTmsAcq, sizeof(TLM_ACQ));

	if( SetTel(glCurTlmTmsAcq.acqPriStlPhnSrv, (uchar *)_T("SETTTLE TELNO 1"))!=0 )
	{
		return ERR_USERCANCEL;
	}
	memcpy(&glTlmSysParam.Acq[glCurTlmTmsAcq.ucIndex], &glCurTlmTmsAcq, sizeof(TLM_ACQ));

	if( SetTel(glCurTlmTmsAcq.acqSecStlPhnSrv, (uchar *)_T("SETTTLE TELNO 2"))!=0 )
	{
		return ERR_USERCANCEL;
	}
	memcpy(&glTlmSysParam.Acq[glCurTlmTmsAcq.ucIndex], &glCurTlmTmsAcq, sizeof(TLM_ACQ));

#else
	if( SetTel(glCurAcq.TxnTelNo1, (uchar *)_T("TRANS TELNO 1"))!=0 )
	{
		return ERR_USERCANCEL;
	}
	memcpy(&glSysParam.stAcqList[glCurAcq.ucIndex], &glCurAcq, sizeof(ACQUIRER));

	if( SetTel(glCurAcq.TxnTelNo2, (uchar *)_T("TRANS TELNO 2"))!=0 )
	{
		return ERR_USERCANCEL;
	}
	memcpy(&glSysParam.stAcqList[glCurAcq.ucIndex], &glCurAcq, sizeof(ACQUIRER));

	if( SetTel(glCurAcq.StlTelNo1, (uchar *)_T("SETTTLE TELNO 1"))!=0 )
	{
		return ERR_USERCANCEL;
	}
	memcpy(&glSysParam.stAcqList[glCurAcq.ucIndex], &glCurAcq, sizeof(ACQUIRER));

	if( SetTel(glCurAcq.StlTelNo2, (uchar *)_T("SETTTLE TELNO 2"))!=0 )
	{
		return ERR_USERCANCEL;
	}
	memcpy(&glSysParam.stAcqList[glCurAcq.ucIndex], &glCurAcq, sizeof(ACQUIRER));
#endif
	return 0;
}

// 设置电话号码
int SetTel(uchar *pszTelNo, uchar *pszPromptInfo)
{
	ScrClrLine(2, 7);
	PubDispString(pszPromptInfo, DISP_LINE_LEFT|2);
	if( PubGetString(ALPHA_IN|ECHO_IN, 0, 24, pszTelNo, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}

	return 0;
}

int SetWirelessParamNew(WIRELESS_PARAM *pstParam)
{
	if (pstParam==NULL)
	{
		return ERR_NO_DISP;
	}

	PubShowTitle(TRUE, (uchar *)"SETUP WIRELESS  ");

	ScrClrLine(2, 7);
	PubDispString("APN", 2|DISP_LINE_LEFT);
	if( PubGetString(ALPHA_IN|ECHO_IN, 0, 32, pstParam->szAPN, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}

	return 0;
}

int SetAPNTelkomsel(WIRELESS_PARAM *pstParam)
{
	sprintf((char *)glTlmSysParam._TxnWirlessPara.szAPN, "EDCBRITS");

	return PrnSetAPN(&glTlmSysParam._TmsWirlessPara);
}

int SetAPNIndosat(WIRELESS_PARAM *pstParam)
{
	sprintf((char *)glTlmSysParam._TxnWirlessPara.szAPN, "EDCBRI");

	return PrnSetAPN(&glTlmSysParam._TmsWirlessPara);
}

int SetAPNXL(WIRELESS_PARAM *pstParam)
{
	sprintf((char *)glTlmSysParam._TxnWirlessPara.szAPN, "EDCBRI");

	return PrnSetAPN(&glTlmSysParam._TmsWirlessPara);
}

int SetWirelessParam(WIRELESS_PARAM *pstParam)
{
	if (pstParam==NULL)
	{
		return ERR_NO_DISP;
	}

	PubShowTitle(TRUE, (uchar *)"SETUP WIRELESS  ");

	ScrClrLine(2, 7);
	PubDispString("APN", 2|DISP_LINE_LEFT);
	if( PubGetString(ALPHA_IN|ECHO_IN, 0, 32, pstParam->szAPN, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}

	ScrClrLine(2, 7);
	//PubDispString("LOGIN NAME", 2|DISP_LINE_LEFT);
	PubDispString("USER", 2|DISP_LINE_LEFT);
	if( PubGetString(ALPHA_IN|ECHO_IN, 0, 32, pstParam->szUID, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}

	ScrClrLine(2, 7);
	PubDispString("LOGIN PWD", 2|DISP_LINE_LEFT);
	//PubDispString("PASS", 2|DISP_LINE_LEFT);
	if( PubGetString(ALPHA_IN|ECHO_IN, 0, 16, pstParam->szPwd, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}

	ScrClrLine(2, 7);
	PubDispString("SIM PIN", 2|DISP_LINE_LEFT);
	if( PubGetString(ALPHA_IN|ECHO_IN, 0, 16, pstParam->szSimPin, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}

	return PrnSetAPN(&glTlmSysParam._TmsWirlessPara);
}


int PrnSetAPN(WIRELESS_PARAM *pstParam)
{
	char szAPN[48+1];
	char szUSER[48+1];
	char szLOGIN[48+1];
	char szPIN[48+1];

	PrnInit();
	PrnSetNormal();
	MEM_ZERO(szAPN);
	MEM_ZERO(szUSER);
	MEM_ZERO(szLOGIN);
	MEM_ZERO(szPIN);

	sprintf(szAPN,"APN :                           %s",pstParam->szAPN);
	//sprintf(szUSER,"USER :                    %s",pstParam->szUID);
	//sprintf(szLOGIN,"LOGIN PWD :                           %s",pstParam->szPwd);
	//sprintf(szPIN,"SIM PIN :                    %s",pstParam->szSimPin);

	
	MultiLngPrnStr(szAPN, DISP_LINE_LEFT);
	PrnStr("\n");
	//MultiLngPrnStr(szUSER, DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(szLOGIN, DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(szPIN, DISP_LINE_LEFT);
	//PrnStr("\n");
	PrnStart();
	
	return 0;
}


void SyncWirelessParam(WIRELESS_PARAM *pstDst, WIRELESS_PARAM *pstSrc)
{
	strcpy((char *)(pstDst->szAPN),    (char *)(pstSrc->szAPN));
	strcpy((char *)(pstDst->szUID),    (char *)(pstSrc->szUID));
	strcpy((char *)(pstDst->szPwd),    (char *)(pstSrc->szPwd));
	strcpy((char *)(pstDst->szSimPin), (char *)(pstSrc->szSimPin));
	strcpy((char *)(pstDst->szDNS),    (char *)(pstSrc->szDNS));
}

// 设置EDC参数
void SetEdcParam(uchar ucPermission)
{
	while( 1 )
	{
		PubShowTitle(TRUE, (uchar *)_T("SETUP EDC       "));
		if (SetTermCurrency(ucPermission)!=0)
		{
			break;
		}
		if( SetMerchantName(ucPermission)!=0 )
		{
			break;
		}
		if( SetPEDMode()!=0 )
		{
			break;
		}

		PubShowTitle(TRUE, (uchar *)_T("SETUP EDC       "));
		if( SetAcceptTimeOut()!=0 )
		{
			break;
		}
		if( SetPrinterType()!=0 )
		{
			break;
		}
		if( SetNumOfReceipt()!=0 )
		{
			break;
		}
		if( SetGetSysTraceNo(ucPermission)!=0 )
		{
			break;
		}
		if( SetGetSysInvoiceNo(ucPermission)!=0 )
		{
			break;
		}

#ifdef	__TELIUM_PARAM__
		ModifyOptList(glTlmSysParam.stEdcInfo.sOption, 'E', ucPermission);
		ModifyOptList(glTlmSysParam.stEdcInfo.sExtOption, 'e', ucPermission);
#else
		ModifyOptList(glSysParam.stEdcInfo.sOption, 'E', ucPermission);
		ModifyOptList(glSysParam.stEdcInfo.sExtOption, 'e', ucPermission);
#endif
		break;
	}

#ifdef	__TELIUM_PARAM__
	SaveTlmEdcParam();
#else
	SaveEdcParam();
#endif
	

#ifdef ENABLE_EMV
#ifdef	__TELIUM_PARAM__
	SyncEmvCurrency(glTlmSysParam.stEdcInfo.stLocalCurrency.sCountryCode,
                    glTlmSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode,
					glTlmSysParam.stEdcInfo.stLocalCurrency.ucDecimal);
#else
	SyncEmvCurrency(glSysParam.stEdcInfo.stLocalCurrency.sCountryCode,
                    glSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode,
					glSysParam.stEdcInfo.stLocalCurrency.ucDecimal);
#endif
#endif
}

// -1 : 值无改变 -2 : 超时或取消
// >=0 : 输入的合法值
// ucEdit      : 是否允许编辑
// pszFirstMsg : 标题下面第一行提示
// pszSecMsg   : 标题下面第二行提示
// ulMin,ulMax : 允许的取值范围
// lOrgValue   : 原值
long ViewGetValue(uchar ucEdit, void *pszFirstMsg, void *pszSecMsg,
				  ulong ulMin, ulong ulMax, long lOrgValue)
{
	uchar	szBuff[32], ucMinDigit, ucMaxDigit, ucTemp;
	ulong	ulTemp;

	PubASSERT(ulMax<2147483648);

	ulTemp = ulMin;
	ucMinDigit = 0;
	do{ucMinDigit++;}while (ulTemp/=10);

	ulTemp = ulMax;
	ucMaxDigit = 0;
	do{ucMaxDigit++;}while (ulTemp/=10);

	memset(szBuff, 0, sizeof(szBuff));
	if (lOrgValue>=0)
	{
		sprintf((char *)szBuff, "%ld", lOrgValue);
	}

	while (1)
	{
		ScrClrLine(2, 7);
		if (pszFirstMsg!=NULL)
		{
			PubDispString(_T(pszFirstMsg), 2|DISP_LINE_LEFT);
		}

		if (pszSecMsg!=NULL)
		{
			PubDispString(_T(pszSecMsg), 4|DISP_LINE_LEFT);
		}

		if (ucEdit)
		{
			// Allow modify 允许修改
			if (PubGetString(NUM_IN|ECHO_IN, ucMinDigit, ucMaxDigit, szBuff, USER_OPER_TIMEOUT)!=0)
			{
				return -2;
			}
			ulTemp = (ulong)atol((char *)szBuff);
			if ((ulTemp<ulMin) || (ulTemp>ulMax))
			{
				ScrClrLine(4, 7);
				PubDispString(_T("INVALID VALUE"), 5|DISP_LINE_LEFT);
				PubBeepErr();
				PubWaitKey(3);
				continue;
			}

			if (ulTemp!=(ulong)lOrgValue)
			{
				return (long)ulTemp;
			}
			return -1;
		}
		else
		{
			// Read only 只读
			PubDispString(szBuff, 6|DISP_LINE_RIGHT);
			ucTemp = PubWaitKey(USER_OPER_TIMEOUT);
			if ((ucTemp==KEYCANCEL) || (ucTemp==NOKEY))
			{
				return -2;
			}
			return -1;
		}
	}
}

int SetTermCurrency(uchar ucPermission)
{
	uchar	szBuff[32], ucTemp;
	long	lTemp;
	CURRENCY_CONFIG	stCurrency;

    // Country Code
	while (1)
	{
		ScrClrLine(2, 7);
#ifdef	__TELIUM_PARAM__
		PubBcd2Asc0(glTlmSysParam.stEdcInfo.stLocalCurrency.sCountryCode, 2, szBuff);
#else
		PubBcd2Asc0(glSysParam.stEdcInfo.stLocalCurrency.sCountryCode, 2, szBuff);
#endif
		PubDispString(_T("AREA CODE"), 2|DISP_LINE_LEFT);
        // Allow modify 允许修改
        if (PubGetString(NUM_IN|ECHO_IN, 3, 3, szBuff+1, USER_OPER_TIMEOUT)!=0)
        {
            return ERR_USERCANCEL;
        }
#ifdef	__TELIUM_PARAM__
        PubAsc2Bcd(szBuff, 3, glTlmSysParam.stEdcInfo.stLocalCurrency.sCountryCode);
#else
	 PubAsc2Bcd(szBuff, 3, glSysParam.stEdcInfo.stLocalCurrency.sCountryCode);
#endif
		break;
	}

	// Currency
	while (1)
	{
		ScrClrLine(2, 7);
#ifdef	__TELIUM_PARAM__
		PubBcd2Asc0(glTlmSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode, 2, szBuff);
#else
		PubBcd2Asc0(glSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode, 2, szBuff);
#endif
		memmove(szBuff, szBuff+1, 4);

		PubDispString(_T("CURRENCY CODE"), 2|DISP_LINE_LEFT);

		if (ucPermission<PM_HIGH)
		{
			// Read only 只读
			PubDispString(szBuff, 6|DISP_LINE_RIGHT);
			ucTemp = PubWaitKey(USER_OPER_TIMEOUT);
			if ((ucTemp==KEYCANCEL) || (ucTemp==NOKEY))
			{
				return ERR_USERCANCEL;
			}
		}
		else
		{
			// Allow modify 允许修改
			if (PubGetString(NUM_IN|ECHO_IN, 3, 3, szBuff, USER_OPER_TIMEOUT)!=0)
			{
				return ERR_USERCANCEL;
			}
			if (FindCurrency(szBuff, &stCurrency)!=0)
			{
				ScrClrLine(4, 7);
				PubDispString(_T("INVALID CURRENCY"), 5|DISP_LINE_LEFT);
				PubBeepErr();
				PubWaitKey(3);
				continue;
			}

			ScrClrLine(2, 7);
			sprintf((char *)szBuff, "%.3s %02X%02X",
				stCurrency.szName, stCurrency.sCurrencyCode[0], stCurrency.sCurrencyCode[1]);
			PubDispString(szBuff, 2|DISP_LINE_LEFT);
			if (!AskYesNo())
			{
				continue;
			}
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glTlmSysParam.stEdcInfo.stLocalCurrency.szName, "%.3s", stCurrency.szName);
			memcpy(glTlmSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode, stCurrency.sCurrencyCode, 2);
#else	
			sprintf((char *)glSysParam.stEdcInfo.stLocalCurrency.szName, "%.3s", stCurrency.szName);
			memcpy(glSysParam.stEdcInfo.stLocalCurrency.sCurrencyCode, stCurrency.sCurrencyCode, 2);
#endif
		}
		break;
	}

	// Input decimal position value, 0<=x<=3
	// for JPY and KRW, x=0; for TWD, x=0 or x=2
#ifdef	__TELIUM_PARAM__
	lTemp = ViewGetValue((uchar)(ucPermission>PM_MEDIUM), _T("DECIMAL POSITION"), NULL,
						0, 3, (long)glTlmSysParam.stEdcInfo.stLocalCurrency.ucDecimal);
#else
	lTemp = ViewGetValue((uchar)(ucPermission>PM_MEDIUM), _T("DECIMAL POSITION"), NULL,
						0, 3, (long)glSysParam.stEdcInfo.stLocalCurrency.ucDecimal);
#endif
	if (lTemp==-2)
	{
		return ERR_USERCANCEL;
	}
	if (lTemp>=0)
	{
#ifdef	__TELIUM_PARAM__
		glTlmSysParam.stEdcInfo.stLocalCurrency.ucDecimal = (uchar)lTemp;
#else
		glSysParam.stEdcInfo.stLocalCurrency.ucDecimal = (uchar)lTemp;
#endif
	}

	// Input ignore digit value, 0<=x<=3
	// for JPY and KRW, x=2; for TWD, when decimal=0, x=2; decimal=2, x=0;
#ifdef	__TELIUM_PARAM__
	lTemp = ViewGetValue((uchar)(ucPermission>PM_MEDIUM), _T("IGNORE DIGIT"), NULL,
						0, 3, (long)glTlmSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit);
#else
	lTemp = ViewGetValue((uchar)(ucPermission>PM_MEDIUM), _T("IGNORE DIGIT"), NULL,
						0, 3, (long)glSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit);
#endif
	if (lTemp==-2)
	{
		return ERR_USERCANCEL;
	}
	if (lTemp>=0)
	{
#ifdef	__TELIUM_PARAM__
		glTlmSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit = (uchar)lTemp;
#else
		glSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit = (uchar)lTemp;
#endif
	}

	return 0;
}

int SetMerchantName(uchar ucPermission)
{
	uchar	szBuff[46+1];

	if (ucPermission<PM_HIGH)	// Not allow to set
	{
		return 0;
	}

	ScrClrLine(2, 7);
	PubDispString(_T("MERCHANT NAME"), DISP_LINE_LEFT|2);
#ifdef __TELIUM_PARAM__
	sprintf((char *)szBuff, "%.23s", (char *)glTlmSysParam.stEdcInfo.szMerchantName);
#else
	sprintf((char *)szBuff, "%.23s", (char *)glSysParam.stEdcInfo.szMerchantName);
#endif
	if( PubGetString(ALPHA_IN|ECHO_IN, 1, 23, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
#ifdef	__TELIUM_PARAM__
	if (strcmp((char *)glTlmSysParam.stEdcInfo.szMerchantName, (char *)szBuff)!=0)
	{
		sprintf((char *)glTlmSysParam.stEdcInfo.szMerchantName, "%.23s", (char *)szBuff);
	}
#else
	if (strcmp((char *)glSysParam.stEdcInfo.szMerchantName, (char *)szBuff)!=0)
	{
		sprintf((char *)glSysParam.stEdcInfo.szMerchantName, "%.23s", (char *)szBuff);
	}
#endif
	ScrClrLine(2, 7);
	PubDispString(_T("MERCHANT ADDR"), DISP_LINE_LEFT|2);
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "%.46s", (char *)glTlmSysParam.stEdcInfo.szMerchantAddr);
#else
	sprintf((char *)szBuff, "%.46s", (char *)glSysParam.stEdcInfo.szMerchantAddr);
#endif
	if( PubGetString(ALPHA_IN|ECHO_IN, 1, 46, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
#ifdef	__TELIUM_PARAM__
	if (strcmp((char *)glTlmSysParam.stEdcInfo.szMerchantAddr, (char *)szBuff)!=0)
	{
		sprintf((char *)glTlmSysParam.stEdcInfo.szMerchantAddr, "%.23s", (char *)szBuff);
	}
#else
	if (strcmp((char *)glSysParam.stEdcInfo.szMerchantAddr, (char *)szBuff)!=0)
	{
		sprintf((char *)glSysParam.stEdcInfo.szMerchantAddr, "%.23s", (char *)szBuff);
	}
#endif
	return 0;
}

int SetGetSysTraceNo(uchar ucPermission)
{
	uchar	szBuff[6+1];

	ScrClrLine(2, 7);
	PubDispString("S.T.A.N.", DISP_LINE_LEFT|2);
	sprintf((char *)szBuff, "%06ld", glSysCtrl.ulSTAN);

	if (ucPermission>PM_LOW)
	{
		if( PubGetString(NUM_IN|ECHO_IN, 1, 6, szBuff, USER_OPER_TIMEOUT)!=0 )
		{
			return ERR_USERCANCEL;
		}
		glSysCtrl.ulSTAN = (ulong)atol((char *)szBuff);
		SaveSysCtrlBase();
	} 
	else
	{
		PubDispString(szBuff, DISP_LINE_RIGHT|6);
		PubYesNo(USER_OPER_TIMEOUT);
	}

	return 0;
}

void SetTraceNo(void)
{
	uchar	szBuff[6+1];

	ScrClrLine(2, 7);
	PubDispString("TRACE NO", DISP_LINE_LEFT|2);
	sprintf((char *)szBuff, "%06ld", glSysCtrl.ulInvoiceNo);

		if( PubGetString(NUM_IN|ECHO_IN, 1, 6, szBuff, USER_OPER_TIMEOUT)!=0 )
		{
			return;
		}
		glSysCtrl.ulInvoiceNo = (ulong)atol((char *)szBuff);
		SaveSysCtrlBase();
}

void ViewTraceNo(void)
{
	uchar	szBuff[6+1];
//	uchar 	ucRet;

	ScrClrLine(2, 7);
	PubDispString("TRACE NO", DISP_LINE_LEFT|2);
	sprintf((char *)szBuff, "%06ld", glSysCtrl.ulInvoiceNo);
	PubDispString(szBuff, DISP_LINE_RIGHT|6);
	if( PubYesNo(USER_OPER_TIMEOUT) )
	{
		return;
	}

}

	
void SetStanNo(void)
{
	uchar	szBuff[6+1];

	ScrClrLine(2, 7);
	PubDispString("S.T.A.N.", DISP_LINE_LEFT|2);
	sprintf((char *)szBuff, "%06ld", glSysCtrl.ulSTAN);

		if( PubGetString(NUM_IN|ECHO_IN, 1, 6, szBuff, USER_OPER_TIMEOUT)!=0 )
		{
			return;
		}
		glSysCtrl.ulSTAN = (ulong)atol((char *)szBuff);
		SaveSysCtrlBase();
}

void ViewStanNo(void)
{
	uchar	szBuff[6+1];
	//uchar	ucRet;

	ScrClrLine(2, 7);
	PubDispString("S.T.A.N.", DISP_LINE_LEFT|2);
	sprintf(szBuff, "%06ld", glSysCtrl.ulSTAN);
	PubDispString(szBuff, DISP_LINE_RIGHT|6);
	if( PubYesNo(USER_OPER_TIMEOUT) )
	{
		return;
	}	
}

int SetGetSysInvoiceNo(uchar ucPermission)
{
	uchar	szBuff[6+1];

	ScrClrLine(2, 7);
	PubDispString("TRACE NO", DISP_LINE_LEFT|2);
	sprintf((char *)szBuff, "%06ld", glSysCtrl.ulInvoiceNo);

	if (ucPermission>PM_LOW)
	{
		if( PubGetString(NUM_IN|ECHO_IN, 1, 6, szBuff, USER_OPER_TIMEOUT)!=0 )
		{
			return ERR_USERCANCEL;
		}
		glSysCtrl.ulInvoiceNo = (ulong)atol((char *)szBuff);
		SaveSysCtrlBase();
	} 
	else
	{
		PubDispString(szBuff, DISP_LINE_RIGHT|6);
		PubYesNo(USER_OPER_TIMEOUT);
	}

	return 0;
}

// Select PED mode used.
int SetPEDMode(void)
{
	int		iCnt;
	char	szBuff[32];
	uchar	ucKey;

#ifdef _P60_S1_
#ifdef	__TELIUM_PARAM__
	glTlmSysParam.stEdcInfo.ucPedMode = PED_INT_PXX;
#else
	glSysParam.stEdcInfo.ucPedMode = PED_INT_PXX;
#endif
	return 0;

#else

	struct {
		char	szPrompt[16+1];
		uchar	ucPEDMode;
	} stModeList[] = {
		{"PINPAD        ", PED_EXT_PP},
#if defined(_P60_S1_) || defined(_P80_) || defined(_P90_)
		{"PED           ", PED_INT_PXX},
#endif
#ifdef _S_SERIES_
		{"PCI PED       ", PED_INT_PCI},
		{"EXT PCI PINPAD", PED_EXT_PCI},
#endif
		{"", 0}
	};

	sprintf(szBuff, "CUR: NONE");
	for (iCnt=0; iCnt<sizeof(stModeList)/sizeof(stModeList[0]); iCnt++)
	{
#ifdef	__TELIUM_PARAM__
		if (glTlmSysParam.stEdcInfo.ucPedMode==stModeList[iCnt].ucPEDMode)
		{
			sprintf(szBuff, "CUR: %.16s", stModeList[iCnt].szPrompt);
			break;
		}
#else
		if (glSysParam.stEdcInfo.ucPedMode==stModeList[iCnt].ucPEDMode)
		{
			sprintf(szBuff, "CUR: %.16s", stModeList[iCnt].szPrompt);
			break;
		}
#endif
	}

	while (1)
	{
		ScrClrLine(2, 7);
		PubDispString(szBuff, 2|DISP_LINE_LEFT);
		for (iCnt=0; iCnt<sizeof(stModeList)/sizeof(stModeList[0])-1; iCnt++)
		{
			ScrPrint(0, (uchar)(4+iCnt), ASCII, "%d.%.14s", iCnt+1, stModeList[iCnt].szPrompt);
		}

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if ((ucKey==KEYCANCEL) || (ucKey==NOKEY))
		{
			return ERR_USERCANCEL;
		}
		if (ucKey==KEYENTER)
		{
			return 0;
		}
		if ((ucKey<KEY1) || (ucKey>KEY0+iCnt))
		{
			continue;
		}
#ifdef	__TELIUM_PARAM__
		glTlmSysParam.stEdcInfo.ucPedMode = stModeList[ucKey-KEY1].ucPEDMode;
#else
		glSysParam.stEdcInfo.ucPedMode = stModeList[ucKey-KEY1].ucPEDMode;
#endif
		return 0;
	}

#endif
}

// 输入交易成功时确认信息显示时间
int SetAcceptTimeOut(void)
{
	uchar	szBuff[2+1];

#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "%d", glTlmSysParam.stEdcInfo.ucAcceptTimeout);
#else
	sprintf((char *)szBuff, "%d", glSysParam.stEdcInfo.ucAcceptTimeout);
#endif
	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("Confirm Timeout", DISP_LINE_LEFT|2);
		PubDispString("[0..60]",       DISP_LINE_LEFT|4);
		if( PubGetString(NUM_IN|ECHO_IN, 1, 2, szBuff, USER_OPER_TIMEOUT)!=0 )
		{
			return ERR_USERCANCEL;
		}
		if( atoi((char *)szBuff)<=60 )
		{
			break;
		}

		ScrClrLine(4, 7);
		PubDispString(_T("INVALID INPUT"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}
#ifdef	__TELIUM_PARAM__
	glTlmSysParam.stEdcInfo.ucAcceptTimeout = (uchar)atoi((char *)szBuff);
#else
	glSysParam.stEdcInfo.ucAcceptTimeout = (uchar)atoi((char *)szBuff);
#endif
	return 0;
}

int SetPrinterType(void)
{
	char 	ucKey, szBuff[16+1];

	// 仅适用于分离式打印机
	if (!ChkTerm(_TERMINAL_S60_))
	{
		return 0;
	}

	sprintf(szBuff, "PRINT: %s", (ChkIfThermalPrinter() ? "THERMAL" : "SPROCKET"));
	ucKey = PubSelectOneTwo((uchar *)szBuff, "THERMAL", "SPROCKET");
	switch(ucKey)
	{
	case KEY1:
#ifdef	__TELIUM_PARAM__
		glTlmSysParam.stEdcInfo.ucPrinterType = 1;
#else
		glSysParam.stEdcInfo.ucPrinterType = 1;
#endif
		return 0;
	case KEY2:
#ifdef	__TELIUM_PARAM__
		glTlmSysParam.stEdcInfo.ucPrinterType = 0;
#else
		glSysParam.stEdcInfo.ucPrinterType = 0;
#endif
		return 0;
	case KEYENTER:
	    return 0;
	case KEYCANCEL:
	default:
	    return ERR_USERCANCEL;
	};
}

// 输入热敏打印单据张数
int SetNumOfReceipt(void)
{
	uchar 	ucNum, szBuff[1+1];

	if( !ChkIfThermalPrinter() )
	{
		return 0;
	}

	sprintf((char *)szBuff, "%d", NumOfReceipt());
	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("Receipt pages", DISP_LINE_LEFT|2);
		PubDispString("[1..4]Thermal",   DISP_LINE_LEFT|4);
		if( PubGetString(NUM_IN|ECHO_IN, 1, 1, szBuff, USER_OPER_TIMEOUT)!=0 )
		{
			return ERR_USERCANCEL;
		}

		ucNum = (uchar)atoi((char *)szBuff);
		if( ucNum>=1 && ucNum<=4 )
		{
			ucNum--;
			break;
		}

		ScrClrLine(4, 7);
		PubDispString(_T("INVALID INPUT"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}

#ifdef	__TELIUM_PARAM__
	glTlmSysParam.stEdcInfo.sOption[EDC_NUM_PRINT_LOW/0x100]  &= (0xFF^(EDC_NUM_PRINT_LOW%0x100));
	glTlmSysParam.stEdcInfo.sOption[EDC_NUM_PRINT_HIGH/0x100] &= (0xFF^(EDC_NUM_PRINT_HIGH%0x100));
	if( ucNum & 0x01 )
	{
		glTlmSysParam.stEdcInfo.sOption[EDC_NUM_PRINT_LOW/0x100] |= (EDC_NUM_PRINT_LOW%0x100);
	}
	if( ucNum & 0x02 )
	{
		glTlmSysParam.stEdcInfo.sOption[EDC_NUM_PRINT_HIGH/0x100] |= (EDC_NUM_PRINT_HIGH%0x100);
	}
#else
	glSysParam.stEdcInfo.sOption[EDC_NUM_PRINT_LOW/0x100]  &= (0xFF^(EDC_NUM_PRINT_LOW%0x100));
	glSysParam.stEdcInfo.sOption[EDC_NUM_PRINT_HIGH/0x100] &= (0xFF^(EDC_NUM_PRINT_HIGH%0x100));
	if( ucNum & 0x01 )
	{
		glSysParam.stEdcInfo.sOption[EDC_NUM_PRINT_LOW/0x100] |= (EDC_NUM_PRINT_LOW%0x100);
	}
	if( ucNum & 0x02 )
	{
		glSysParam.stEdcInfo.sOption[EDC_NUM_PRINT_HIGH/0x100] |= (EDC_NUM_PRINT_HIGH%0x100);
	}

#endif
	return 0;
}

// 输入参数自动更新时间
int SetCallInTime(void)
{
	return 0;
}

// TRUE:判断时间是否合法
uchar IsValidTime(uchar *pszTime)
{
	int		i, iHour, iMinute;

	for(i=0; i<4; i++)
	{
		if( pszTime[i]<'0' || pszTime[i]>'9' )
		{
			return FALSE;
		}
	}

	iHour   = (int)PubAsc2Long(pszTime, 2, NULL);
	iMinute = (int)PubAsc2Long(pszTime+2, 2, NULL);
	if( iHour>24 || iMinute>59 )
	{
		return FALSE;
	}
	if( iHour==24 && iMinute!=0 )
	{
		return FALSE;
	}

	return TRUE;
}

// 修改或者查看开关选项
int ModifyOptList(uchar *psOption, uchar ucMode, uchar ucPermission)
{
	// 通过FUN2进入设置时，若定义了FUN2_READ_ONLY，则用户权限为PM_LOW，否则用户权限为PM_MEDIUM
	// 使用无下载方式初始化时，用户权限为PM_HIGH

	// Protims可控的issuer option列表
	static OPTION_INFO stIssuerOptList[] =
	{
// 		{"CAPTURE CASH?",		ALLOW_EXTEND_PAY,			FALSE,	PM_MEDIUM},
		{"CAPTURE TXN",			ISSUER_CAPTURE_TXN,			FALSE,	PM_MEDIUM},
		{"ENABLE BALANCE?",		ISSUER_EN_BALANCE,			FALSE,	PM_MEDIUM},
		{"ENABLE ADJUST",		ISSUER_EN_ADJUST,			FALSE,	PM_MEDIUM},
		{"ENABLE OFFLINE",		ISSUER_EN_OFFLINE,			FALSE,	PM_MEDIUM},
		{"ALLOW (PRE)AUTH",		ISSUER_NO_PREAUTH,			TRUE,	PM_MEDIUM},
		{"ALLOW REFUND",		ISSUER_NO_REFUND,			TRUE,	PM_MEDIUM},
		{"ALLOW VOID",			ISSUER_NO_VOID,				TRUE,	PM_MEDIUM},
		{"ENABLE EXPIRY",		ISSUER_EN_EXPIRY,			FALSE,	PM_MEDIUM},
		{"CHECK EXPIRY",		ISSUER_CHECK_EXPIRY,		FALSE,	PM_MEDIUM},
//		{"CHKEXP OFFLINE",		ISSUER_CHECK_EXPIRY_OFFLINE,FALSE,	PM_MEDIUM},
		{"CHECK PAN",			ISSUER_CHKPAN_MOD10,		FALSE,	PM_MEDIUM},
// 		{"CHECK PAN11",			ISSUER_CHKPAN_MOD11,		FALSE,	PM_MEDIUM},
//		{"EN DISCRIPTOR",		ISSUER_EN_DISCRIPTOR,		FALSE,	PM_MEDIUM},
		{"ENABLE MANUAL",		ISSUER_EN_MANUAL,			FALSE,	PM_MEDIUM},
		{"ENABLE PRINT",		ISSUER_EN_PRINT,			FALSE,	PM_MEDIUM},
		{"VOICE REFERRAL",		ISSUER_EN_VOICE_REFERRAL,	FALSE,	PM_MEDIUM},
		{"PIN REQUIRED",		ISSUER_EN_PIN,				FALSE,	PM_HIGH},
#ifdef ISSUER_EN_EMVPIN_BYPASS
		{"EMV PIN BYPASS",		ISSUER_EN_EMVPIN_BYPASS,	FALSE,	PM_MEDIUM},
#endif
//		{"ACCOUNT SELECT",		ISSUER_EN_ACCOUNT_SELECTION,FALSE,	PM_MEDIUM},
//		{"ROC INPUT REQ",		ISSUER_ROC_INPUT_REQ,		FALSE,	PM_MEDIUM},
//		{"DISP AUTH CODE",		ISSUER_AUTH_CODE,			FALSE,	PM_MEDIUM},
//		{"ADDTIONAL DATA",		ISSUER_ADDTIONAL_DATA,		FALSE,	PM_MEDIUM},
		{"4DBC WHEN SWIPE",		ISSUER_SECURITY_SWIPE,		FALSE,	PM_MEDIUM},
		{"4DBC WHEN MANUL",		ISSUER_SECURITY_MANUL,		FALSE,	PM_MEDIUM},
		{NULL, 0, FALSE, PM_DISABLE},
	};

	// Protims可控的acquirer option列表
	static OPTION_INFO stAcqOptList[] =
	{
		{"ONLINE VOID",			ACQ_ONLINE_VOID,			FALSE,	PM_MEDIUM},
		{"ONLINE REFUND",		ACQ_ONLINE_REFUND,			FALSE,	PM_MEDIUM},
		{"EN. TRICK FEED",		ACQ_DISABLE_TRICK_FEED,		TRUE,	PM_MEDIUM},
//		{"ADDTION PROMPT",		ACQ_ADDTIONAL_PROMPT,		FALSE,	PM_MEDIUM},
		{"AMEX ACQUIRER",		ACQ_AMEX_SPECIFIC_FEATURE,	FALSE,	PM_HIGH},
		{"DBS FEATURE",			ACQ_DBS_FEATURE,			FALSE,	PM_MEDIUM},
		{"BOC INSTALMENT",		ACQ_BOC_INSTALMENT_FEATURE,	FALSE,	PM_MEDIUM},
		{"CITI INSTALMENT",		ACQ_CITYBANK_INSTALMENT_FEATURE,FALSE,	PM_MEDIUM},
#ifdef ENABLE_EMV
		{"EMV ACQUIRER",		ACQ_EMV_FEATURE,			FALSE,	PM_HIGH},
#endif
		{NULL, 0, FALSE, PM_DISABLE},
	};

	// Protims不可控的acquirer option列表
	static OPTION_INFO stAcqExtOptList[] =
	{
		// 因为只能在且必须在POS上修改，因此权限设为PM_LOW
		{NULL, 0, FALSE, PM_DISABLE},
	};

	// Protims可控的edc option列表
	static OPTION_INFO stEdcOptList[] =
	{
//		{"AUTH PAN MASKING",	EDC_AUTH_PAN_MASKING,	FALSE,	PM_LOW},
//		{"SELECT ACQ_CARD",		EDC_SELECT_ACQ_FOR_CARD,FALSE,	PM_LOW},
//		{"ENABLE ECR",			EDC_ECR_ENABLE,			FALSE,	PM_MEDIUM},
		{"FREE PRINT",			EDC_FREE_PRINT,			FALSE,  PM_LOW},
		{"EN. INSTALMENT?",		EDC_ENABLE_INSTALMENT,	FALSE,	PM_MEDIUM},
		{"CAPTURE CASH",		EDC_CASH_PROCESS,		FALSE,	PM_MEDIUM},
		{"REFERRAL DIAL",		EDC_REFERRAL_DIAL,		FALSE,	PM_MEDIUM},
		{"1.AUTH 0.PREAUTH",	EDC_AUTH_PREAUTH,		FALSE,	PM_MEDIUM},
//		{"PRINT TIME",			EDC_PRINT_TIME,			FALSE,	PM_MEDIUM},
		{"TIP PROCESSING",		EDC_TIP_PROCESS,		FALSE,	PM_MEDIUM},
//		{"USE PRINTER",			EDC_USE_PRINTER,		FALSE,	PM_MEDIUM},
		{"NEED ADJUST PWD",		EDC_NOT_ADJUST_PWD,		TRUE,	PM_HIGH},
		{"NEED SETTLE PWD",		EDC_NOT_SETTLE_PWD,		TRUE,	PM_HIGH},
		{"NEED REFUND PWD",		EDC_NOT_REFUND_PWD,		TRUE,	PM_HIGH},
		{"NEED VOID PWD",		EDC_NOT_VOID_PWD,		TRUE,	PM_HIGH},
		{"NEED MANUAL PWD",		EDC_NOT_MANUAL_PWD,		TRUE,	PM_HIGH},
//		{"LOCKED EDC",			EDC_NOT_KEYBOARD_LOCKED,TRUE,	PM_MEDIUM},
		{NULL, 0, FALSE, PM_DISABLE},
	};

	// Protims不可控的edc option列表
	static OPTION_INFO stEdcExtOptList[] =
	{
		// 因为只能在且必须在POS上修改，因此权限设为PM_LOW
		{NULL, 0, FALSE, PM_DISABLE},
	};

	OPTION_INFO		*pstCurOpt;
	uchar			ucCnt, szBuff[4], ucOptIndex, ucOptBitNo;

	switch(ucMode)
	{
	case 'I':
	case 'i':
		pstCurOpt = (OPTION_INFO *)stIssuerOptList;
		break;
	case 'E':
		pstCurOpt = (OPTION_INFO *)stEdcOptList;
		break;
	case 'e':
		pstCurOpt = (OPTION_INFO *)stEdcExtOptList;
		break;
	case 'A':
		pstCurOpt = (OPTION_INFO *)stAcqOptList;
		break;
	case 'a':
		pstCurOpt = (OPTION_INFO *)stAcqExtOptList;
		break;
	default:
		break;
	}

	if( pstCurOpt->pText==NULL )
	{
		return 0;
	}

	ucCnt = 0;
	while( 1 )
	{
		ucOptIndex = (uchar)(pstCurOpt[ucCnt].uiOptVal>>8);
		ucOptBitNo = (uchar)(pstCurOpt[ucCnt].uiOptVal & 0xFF);

		ScrClrLine(2, 7);
		PubDispString(pstCurOpt[ucCnt].pText, DISP_LINE_CENTER|2);
		if (pstCurOpt[ucCnt].ucInverseLogic)
		{
			sprintf((char *)szBuff, "(%c)", (psOption[ucOptIndex] & ucOptBitNo) ? '0' : '1');
		}
		else
		{
			sprintf((char *)szBuff, "(%c)", (psOption[ucOptIndex] & ucOptBitNo) ? '1' : '0');
		}
		PubDispString(szBuff,       DISP_LINE_CENTER|4);
		PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
		switch( PubWaitKey(USER_OPER_TIMEOUT) )
		{
		case KEY1:
			if (ucPermission>=pstCurOpt[ucCnt].ucPermissionLevel)
			{
				if (pstCurOpt[ucCnt].ucInverseLogic)
				{
					psOption[ucOptIndex] &= ~ucOptBitNo;
				}
				else
				{
					psOption[ucOptIndex] |= ucOptBitNo;
				}
			} 
			break;

		case KEY0:
			if (ucPermission>=pstCurOpt[ucCnt].ucPermissionLevel)
			{
				if (pstCurOpt[ucCnt].ucInverseLogic)
				{
					psOption[ucOptIndex] |= ucOptBitNo;
				}
				else
				{
					psOption[ucOptIndex] &= ~ucOptBitNo;
				}
			}
			break;

		case KEYUP:
			if( ucCnt>0 )
			{
				ucCnt--;
			}
			break;

		case KEYDOWN:
		case KEYENTER:
			if( pstCurOpt[ucCnt+1].pText==NULL )
			{
				return 0;
			}
			ucCnt++;
			break;

		case NOKEY:
		case KEYCANCEL:
			return ERR_USERCANCEL;
		}
	}
}

// 修改口令
void ChangePassword(void)
{
	static MenuItem stChgPwdMenu[] =
	{
		{TRUE, _T_NOOP("TERMINAL   PWD"),   ModifyPasswordTerm},
		{TRUE, _T_NOOP("BANK       PWD"),   ModifyPasswordBank},
		{TRUE, _T_NOOP("MERCHANT   PWD"),   ModifyPasswordMerchant},
		{TRUE, _T_NOOP("VOID       PWD"),   ModifyPasswordVoid},
		{TRUE, _T_NOOP("REFUND     PWD"),   ModifyPasswordRefund},
		{TRUE, _T_NOOP("ADJUST     PWD"),   ModifyPasswordAdjust},
		{TRUE, _T_NOOP("SETTLE     PWD"),   ModifyPasswordSettle},
		{TRUE, "", NULL},
	};
	if(PasswordBank()!=0)
		return;

	PubGetMenu((uchar *)_T("CHANGE PWD"),       stChgPwdMenu, MENU_AUTOSNO|MENU_ASCII, USER_OPER_TIMEOUT);
}

// 手工设置系统时间
void SetSysTime(void)
{
	uchar	szBuff[14+1], sInputTime[6];

	PubShowTitle(TRUE, (uchar *)_T("SET TIME"));

	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("YYYYMMDDHHMMSS", DISP_LINE_RIGHT|2);
		GetDateTime(szBuff);
		PubDispString(szBuff, DISP_LINE_RIGHT|4);

		if( PubGetString(NUM_IN|ECHO_IN, 14, 14, szBuff, USER_OPER_TIMEOUT)!=0 )
		{
			return;
		}
		PubAsc2Bcd(&szBuff[2], 12, sInputTime);
		if( SetTime(sInputTime)==0 )
		{
			return;
		}
		ScrClrLine(4, 7);
		PubDispString(_T("INVALID TIME"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}
}


void SetSysDate(void)
{
	uchar	szBuff[14+1], sInputTime[6];
	uchar 	szBuff2[14+1];
	uchar 	szBuff3[14+1];

	PubShowTitle(TRUE, (uchar *)_T("SET DATE"));
	
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuff2, 0, sizeof(szBuff2));
	memset(szBuff3, 0, sizeof(szBuff3));
	
//	while( 1 )
	//{
		ScrClrLine(2, 7);
		PubDispString("YYYYMMDD", DISP_LINE_RIGHT|2);
		GetDateTime(szBuff);
		memcpy(szBuff2, szBuff, 8);
		PubDispString(szBuff2, DISP_LINE_RIGHT|4);
	
		if( PubGetString(NUM_IN|ECHO_IN, 8, 8, szBuff3, USER_OPER_TIMEOUT)!=0 )
		{
			return;
		}

		memcpy(szBuff, szBuff3, 8);
		PubAsc2Bcd(&szBuff[2], 12, sInputTime);
		if( SetTime(sInputTime)==0 )
		{
			SetSysTimeNew ();
			return;
		}
		ScrClrLine(4, 7);
		PubDispString(_T("INVALID DATE"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	//}
		

}


void SetSysTimeNew(void)
{
	uchar	szBuff[14+1], sInputTime[6];
	uchar 	szBuff2[14+1];
	uchar 	szBuff3[14+1];

	PubShowTitle(TRUE, (uchar *)_T("SET TIME"));

	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuff2, 0, sizeof(szBuff2));
	memset(szBuff3, 0, sizeof(szBuff3));
	
	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("HHMMSS", DISP_LINE_RIGHT|2);
		GetDateTime(szBuff);
		memcpy(szBuff2, szBuff+8, 6);
		PubDispString(szBuff2, DISP_LINE_RIGHT|4);

		if( PubGetString(NUM_IN|ECHO_IN, 6, 6, szBuff3, USER_OPER_TIMEOUT)!=0 )
		{
			return;
		}

		memcpy(szBuff+8, szBuff3, 6);
		PubAsc2Bcd(&szBuff[2], 12, sInputTime);
		if( SetTime(sInputTime)==0 )
		{
			return;
		}
		ScrClrLine(4, 7);
		PubDispString(_T("INVALID TIME"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}
}


// provide manual select and prompt message when pszLngName==NULL
// mode:
// 0--auto load the first available non-english language (if language file available)
// 1--auto load the last time used language
// 2--provide a menu for selection
void SetSysLang(uchar ucSelectMode)
{
	int	iCnt, iTotal, iRet;
	LANG_CONFIG		stLastCfg;

	MenuItem stLangMenu[32];

	if (ucSelectMode==2)
	{
#ifdef	__TELIUM_PARAM__
		stLastCfg = glTlmSysParam.stEdcInfo.stLangCfg;
#else
		stLastCfg = glSysParam.stEdcInfo.stLangCfg;
#endif
	}

REDO_SELECT_LANG:
	if (ucSelectMode==0 || ucSelectMode==2)
	{
		// 搜寻已下载的语言文件，准备菜单
		for (iCnt=0, iTotal=0;
			iCnt<sizeof(stLangMenu)/sizeof(stLangMenu[0])-1;
			iCnt++)
		{
			if (glLangList[iCnt].szDispName[0]==0)
			{
				break;
			}
			if ((iCnt==0) || fexist((char *)glLangList[iCnt].szFileName)>=0)
			{
				stLangMenu[iTotal].bVisible = TRUE;
				strcpy(stLangMenu[iTotal].szMenuName, _T((char *)glLangList[iCnt].szDispName));
				stLangMenu[iTotal].pfMenuFunc = NULL;
				iTotal++;
			}
		}
		stLangMenu[iTotal].bVisible = TRUE;
		strcpy(stLangMenu[iTotal].szMenuName, "");
		stLangMenu[iTotal].pfMenuFunc = NULL;

		if (ucSelectMode==0)// 首次加载
		{
			// 如果有一个或多个非英文语言，自动选择第一个；否则选择英语
			iRet = ((iTotal>1) ? 1 : 0);
		}
		else
		{
			// 菜单手动选择
			iRet = PubGetMenu((uchar *)_T("SELECT LANG"), stLangMenu, MENU_CFONT|MENU_AUTOSNO, 60);
			if (iRet<0)
			{
				return;
			}
		}

		for (iCnt=0; glLangList[iCnt].szDispName[0]!=0; iCnt++)
		{
			if (strcmp((char *)glLangList[iCnt].szDispName,(char *)stLangMenu[iRet].szMenuName)==0)
			{
#ifdef	__TELIUM_PARAM__
				glTlmSysParam.stEdcInfo.stLangCfg = glLangList[iCnt];
#else
				glSysParam.stEdcInfo.stLangCfg = glLangList[iCnt];
#endif
				break;
			}
		}
	}

#ifdef	__TELIUM_PARAM__
	if (strcmp(glTlmSysParam.stEdcInfo.stLangCfg.szFileName, "")==0)
	{
		iRet = SetLng(NULL);
		return;
	}

	iRet = SetLng(glTlmSysParam.stEdcInfo.stLangCfg.szFileName);
	if (iRet!=0)
	{
		glTlmSysParam.stEdcInfo.stLangCfg = glLangList[0];
		return;
	}
#else
	// 设为英语
	if (strcmp(glSysParam.stEdcInfo.stLangCfg.szFileName, "")==0)
	{
		iRet = SetLng(NULL);
		return;
	}

	iRet = SetLng(glSysParam.stEdcInfo.stLangCfg.szFileName);
	if (iRet!=0)
	{
		glSysParam.stEdcInfo.stLangCfg = glLangList[0];
		return;
	}
#endif	
	if ((ucSelectMode==0) || (ucSelectMode==2))
	{
		// 在初次加载或者手动选择模式下，检查字库是否含有此内码
		if (CheckSysFont()!=0)
		{
			ScrCls();
			PubDispString((uchar *)"SELECT LANG",  DISP_LINE_LEFT|DISP_LINE_REVER);
			PubDispString((uchar *)"DISPLAY/PRINT",    2|DISP_LINE_LEFT);
			PubDispString((uchar *)"MAY HAVE PROBLEM", 4|DISP_LINE_LEFT);
			PubDispString((uchar *)"SET ANYWAY ?",     6|DISP_LINE_LEFT);
			if (PubYesNo(USER_OPER_TIMEOUT)==0)
			{
				return;
			}

			iRet = SetLng(NULL);
#ifdef	__TELIUM_PARAM__
			glTlmSysParam.stEdcInfo.stLangCfg = glLangList[0];
#else
			glSysParam.stEdcInfo.stLangCfg = glLangList[0];
#endif
			ucSelectMode = 2;
			goto REDO_SELECT_LANG;
		}
	}
}

// Set system language
void SetEdcLang(void)
{
	LANG_CONFIG	stLangBak;

#ifdef	__TELIUM_PARAM__
	memcpy(&stLangBak, &glTlmSysParam.stEdcInfo.stLangCfg, sizeof(LANG_CONFIG));
#else
	memcpy(&stLangBak, &glSysParam.stEdcInfo.stLangCfg, sizeof(LANG_CONFIG));
#endif

	SetSysLang(2);
#ifdef AREA_Arabia
    CustomizeAppLibForArabiaLang( strcmp(LANGCONFIG, "Arabia")==0 );
#endif

#ifdef	__TELIUM_PARAM__
	if (memcmp(&stLangBak, &glTlmSysParam.stEdcInfo.stLangCfg, sizeof(LANG_CONFIG)) != 0)
	{
		SaveTlmEdcParam();
	}
#else
	if (memcmp(&stLangBak, &glSysParam.stEdcInfo.stLangCfg, sizeof(LANG_CONFIG)) != 0)
	{
		SaveEdcParam();
	}
#endif
}

#ifndef APP_MANAGER_VER
void SetEdcLangExt(char *pszDispName)
{
	int	ii;
	for (ii=0; glLangList[ii].szDispName[0]!=0; ii++)
	{
		if (PubStrNoCaseCmp((uchar *)glLangList[ii].szDispName, pszDispName)==0)
		{
			if ((ii==0) || (fexist((char *)glLangList[ii].szFileName)>=0))
			{
#ifdef	__TELIUM_PARAM__
				glTlmSysParam.stEdcInfo.stLangCfg = glLangList[ii];
#else
				glSysParam.stEdcInfo.stLangCfg = glLangList[ii];
#endif
				SetSysLang(1);
#ifdef AREA_Arabia
                CustomizeAppLibForArabiaLang( strcmp(LANGCONFIG, "Arabia")==0 );
#endif
			}
		}
	}
}
#endif

void SetPowerSave(void)
{
#ifdef _S_SERIES_
	uchar	ucRet, ucTemp, szBuff[32];

	ScrCls();
	PubDispString(_T("POWERSAVE OPTION"), DISP_LINE_REVER);
	if (!ChkTerm(_TERMINAL_S90_))
	{
		PubDispString(_T("UNSUPPORTED."), 2|DISP_LINE_LEFT);
		PubWaitKey(USER_OPER_TIMEOUT);
		return;
	}
	else
	{
		while (1)
		{
			sprintf((char *)szBuff, _T("IDLE: POWER SAVE"));
#ifdef	__TELIUM_PARAM__
			if (glTlmSysParam.stEdcInfo.ucIdleShutdown)
			{
				sprintf((char *)szBuff, _T("IDLE: SHUTDOWN  "));
			}
#else
			if (glSysParam.stEdcInfo.ucIdleShutdown)
			{
				sprintf((char *)szBuff, _T("IDLE: SHUTDOWN  "));
			}
#endif
			ScrClrLine(2, 7);
			ucRet = PubSelectOneTwo(szBuff, _T("POWER SAVE"), _T("SHUTDOWN"));
			if (ucRet==KEY1)
			{
#ifdef	__TELIUM_PARAM__
				glTlmSysParam.stEdcInfo.ucIdleShutdown = 0;
				SaveTlmSysParam();
#else
				glSysParam.stEdcInfo.ucIdleShutdown = 0;
				SaveSysParam();
#endif
			}
			if (ucRet==KEY2)
			{
#ifdef	__TELIUM_PARAM__
				glTlmSysParam.stEdcInfo.ucIdleShutdown = 1;
				if (glTlmSysParam.stEdcInfo.ucIdleMinute<5)
				{
					glTlmSysParam.stEdcInfo.ucIdleMinute = 5;
				}
				SaveTlmSysParam();
#else
				glSysParam.stEdcInfo.ucIdleShutdown = 1;
				if (glSysParam.stEdcInfo.ucIdleMinute<5)
				{
					glSysParam.stEdcInfo.ucIdleMinute = 5;
				}
				SaveSysParam();
#endif
			}
			if (ucRet==KEYCANCEL)
			{
				return;
			}
			break;
		}

#ifdef	__TELIUM_PARAM__
		ucTemp = glTlmSysParam.stEdcInfo.ucIdleMinute;
#else
		ucTemp = glSysParam.stEdcInfo.ucIdleMinute;
#endif
		while (1)
		{
			ScrClrLine(2, 7);
#ifdef	__TELIUM_PARAM__
			if (glTlmSysParam.stEdcInfo.ucIdleShutdown)
			{
				PubDispString(_T("SHUTDOWN TIMEOUT"), 2|DISP_LINE_LEFT|DISP_LINE_REVER);
				PubDispString("(5-60 minutes)", 4|DISP_LINE_LEFT);
			}
#else
			if (glSysParam.stEdcInfo.ucIdleShutdown)
			{
				PubDispString(_T("SHUTDOWN TIMEOUT"), 2|DISP_LINE_LEFT|DISP_LINE_REVER);
				PubDispString("(5-60 minutes)", 4|DISP_LINE_LEFT);
			}
#endif
			else
			{
				PubDispString(_T("PWR SAVE TIMEOUT"), 2|DISP_LINE_LEFT|DISP_LINE_REVER);
				PubDispString("(1-60 minutes)", 4|DISP_LINE_LEFT);
			}
			sprintf((char *)szBuff, "%d", (int)ucTemp);
			ucRet = PubGetString(NUM_IN|ECHO_IN, 1, 2, szBuff, USER_OPER_TIMEOUT);
			if (ucRet!=0)
			{
				return;
			}
			ucTemp = (uchar)atol((char *)szBuff);
			if (ucTemp>60 || ucTemp<1)
			{
				continue;
			}
#ifdef	__TELIUM_PARAM__
			if (glTlmSysParam.stEdcInfo.ucIdleShutdown && (ucTemp<5))
			{
				continue;
			}

			if (glTlmSysParam.stEdcInfo.ucIdleMinute!=ucTemp)
			{
				glTlmSysParam.stEdcInfo.ucIdleMinute = ucTemp;
				SaveTlmSysParam();
			}
#else
			if (glSysParam.stEdcInfo.ucIdleShutdown && (ucTemp<5))
			{
				continue;
			}

			if (glSysParam.stEdcInfo.ucIdleMinute!=ucTemp)
			{
				glSysParam.stEdcInfo.ucIdleMinute = ucTemp;
				SaveSysParam();
			}
#endif
			break;
		}
	}
#endif
}

void TestMagicCard1(void)
{
	TestMagicCard(1);
}

void TestMagicCard2(void)
{
	TestMagicCard(2);
}

void TestMagicCard3(void)
{
	TestMagicCard(3);
}

void TestMagicCard(int iTrackNum)
{
	uchar	ucRet, ucKey;
	uchar	szMagTrack1[79+1], szMagTrack2[40+1], szMagTrack3[104+1];
	uchar	szTitle[16+1];

	MagClose();
	MagOpen();
	MagReset();
	while( 1 )
	{
		sprintf((char *)szTitle, "TRACK %d TEST", iTrackNum);
		PubShowTitle(TRUE, szTitle);
		PubDispString(_T("PLS SWIPE CARD"), 4|DISP_LINE_LEFT);
		while( 2 )
		{
			if( PubKeyPressed() )
			{
				if( getkey()==KEYCANCEL )
				{
					MagClose();
					return;
				}
			}
			if( MagSwiped()==0 )
			{
				break;
			}
		}

		memset(szMagTrack1, 0, sizeof(szMagTrack1));
		memset(szMagTrack2, 0, sizeof(szMagTrack2));
		memset(szMagTrack3, 0, sizeof(szMagTrack3));
		ucRet = MagRead(szMagTrack1, szMagTrack2, szMagTrack3);
		ScrClrLine(2, 7);
		if( iTrackNum==1 )
		{
			ScrPrint(0, 5, ASCII, "%.21s", szMagTrack1[0]==0 ? (uchar *)"NULL" : szMagTrack1);
			ScrPrint(0, 7, ASCII, "Length=[%d]", strlen((char *)szMagTrack1));
		}
		else if (iTrackNum == 2)
		{
			ScrPrint(0, 5, ASCII, "%.21s", szMagTrack2[0]==0 ? (uchar *)"NULL" : szMagTrack2);
			ScrPrint(0, 7, ASCII, "Length=[%d]", strlen((char *)szMagTrack2));
		}
		else
		{
			ScrPrint(0, 5, ASCII, "%.21s", szMagTrack3[0]==0 ? (uchar *)"NULL" : szMagTrack3);
			ScrPrint(0, 7, ASCII, "Length=[%d]", strlen((char *)szMagTrack3));
		}
		ScrPrint(128-6*6, 7, ASCII, "RET:%02X", ucRet);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return;
		}
	}
}

void ToolsViewPreTransMsg(void)
{
	static MenuItem stViewMsgMenu[] =
	{
		{TRUE, "OUTPUT SEND/RECV",	ShowExchangePack},
		{TRUE, "PRINT SEND/RECV",	PrnExchangePack},
		{TRUE, "",	NULL},
	};

	PubShowTitle(TRUE, (uchar *)_T("VIEW MSG"));
	if( PasswordBank()!=0 )
	{
		return;
	}

	PubGetMenu((uchar *)_T("VIEW MSG"), 
				stViewMsgMenu, MENU_AUTOSNO|MENU_ASCII, USER_OPER_TIMEOUT);
}

// 发送通讯报文到COM1
// send comm package
void ShowExchangePack(void)
{
#if defined(WIN32) && defined(_S_SERIES_)
#define DEBUG_OUT_PORT	PINPAD
#else
#define DEBUG_OUT_PORT	0
#endif
	PubShowTitle(TRUE, (uchar *)_T("VIEW MSG"));
	if (!glSendData.uiLength && !glRecvData.uiLength)
	{
		DispErrMsg((uchar *)_T("NO DATA"), NULL, 5, 0);
		return;
	}
	
	PubDispString(_T("SENDING..."), 4|DISP_LINE_LEFT);

	DebugNacTxd(DEBUG_OUT_PORT, glSendData.sContent, glSendData.uiLength);
	DelayMs(2000);
	DebugNacTxd(DEBUG_OUT_PORT, glRecvData.sContent, glRecvData.uiLength);

	PubDispString(_T("SEND OK"), 4|DISP_LINE_LEFT);
	PubWaitKey(2);
}

// 打印通讯报文
// Print comm package
void PrnExchangePack(void)
{
	PubShowTitle(TRUE, (uchar *)_T("VIEW MSG"));
	if (!glSendData.uiLength && !glRecvData.uiLength)
	{
		DispErrMsg((uchar *)_T("NO DATA"), NULL, 5, 0);
		return;
	}

	PubDebugOutput((uchar *)_T("VIEW MSG"), glSendData.sContent, glSendData.uiLength,
					DEVICE_PRN, ISO_MODE);
	PubDebugOutput((uchar *)_T("VIEW MSG"), glRecvData.sContent, glRecvData.uiLength,
					DEVICE_PRN, ISO_MODE);
}

void DebugNacTxd(uchar ucPortNo, uchar *psTxdData, ushort uiDataLen)
{
	uchar	*psTemp, sWorkBuf[LEN_MAX_COMM_DATA+10];
	uchar  ucInit = 0;
	
	if( uiDataLen>LEN_MAX_COMM_DATA )
	{
		PubDispString(_T("INVALID PACK"), 4|DISP_LINE_LEFT);
		PubWaitKey(2);
		return;
	}

	sWorkBuf[0] = STX;
	sWorkBuf[1] = (uiDataLen/1000)<<4    | (uiDataLen/100)%10;	// convert to BCD
	sWorkBuf[2] = ((uiDataLen/10)%10)<<4 | uiDataLen%10;
	memcpy(&sWorkBuf[3], psTxdData, uiDataLen);
	sWorkBuf[3+uiDataLen]   = ETX;

	//sWorkBuf[3+uiDataLen+1] = PubCalcLRC(psTxdData, uiDataLen, (uchar)(sWorkBuf[1] ^ sWorkBuf[2] ^ ETX));
	PubCalcLRC(sWorkBuf + 1, (ushort)(uiDataLen+3), &ucInit);
	sWorkBuf[3+uiDataLen+1] = ucInit;
	//end
	uiDataLen += 5;

	PortClose(ucPortNo);
	PortOpen(ucPortNo, (void *)"9600,8,n,1");
	psTemp = sWorkBuf;
	while( uiDataLen-->0 )
	{
		if( PortSend(ucPortNo, *psTemp++)!=0 )
		{
			break;
		}
	}
	PortClose(ucPortNo);
}

// end of file
void DebugSetNew(void)
{
	uchar		szBuff[4];

	ScrCls();
	PubDispString("DEBUG REQUEST", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugTx) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugTx = TRUE;
			break;

		case KEY0:
			debugTx = FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}

	ScrCls();
	PubDispString("DEBUG RESPONSE", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugRx) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugRx = TRUE;
			break;

		case KEY0:
			debugRx = FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return ;
	}
	
}

void DebugSet(void)
{
	uchar		szBuff[4];

	if(PasswordBank()!=0)
		return;

	ScrCls();
	PubDispString("DEBUG REQUEST", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugTx) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugTx = TRUE;
			break;

		case KEY0:
			debugTx = FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}

	ScrCls();
	PubDispString("DEBUG RESPONSE", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugRx) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugRx = TRUE;
			break;

		case KEY0:
			debugRx = FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return ;
	}

	DebugIsoReqSet();
	DebugIsoResSet();
	
}


void TLEDebugSet(void)
{
	uchar		szBuff[4];

	ScrCls();
	PubDispString("TLE DEBUG", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugTLE) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugTLE= TRUE;
			break;

		case KEY0:
			debugTLE= FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
}

void BlowFishSet(void)
{
	uchar		szBuff[4];

	ScrCls();
	PubDispString("BLOWFISH", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (glSysCtrl.ucFlagBlowfish) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			blowfishSetting= TRUE;
			glSysCtrl.ucFlagBlowfish=TRUE;
			break;

		case KEY0:
			blowfishSetting= FALSE;
			glSysCtrl.ucFlagBlowfish=FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
	SaveSysCtrlBase();
}

void TLESet(void)
{
	uchar		szBuff[4];

	mapGetByte(appTleSet, TLESetting);
	ScrCls();
	PubDispString("TLE SETTING", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (TLESetting) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			TLESetting= TRUE;
			break;

		case KEY0:
			TLESetting= FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
	mapPutByte(appTleSet, TLESetting);
}

void DebugIsoReqSet(void)
{
	uchar		szBuff[4];

	ScrCls();
	PubDispString("DEBUG ISO REQ", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugIsoReq) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugIsoReq= TRUE;
			break;

		case KEY0:
			debugIsoReq= FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
}


void SetKeyIn(void)
{
	uchar		szBuff[4];

	ScrCls();
	PubDispString("Set Key In", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (setKeyIn) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			setKeyIn= TRUE;
			break;

		case KEY0:
			setKeyIn= FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
}


void DebugIsoResSet(void)
{
	uchar		szBuff[4];

	ScrCls();
	PubDispString("DEBUG ISO RSP", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugIsoRes) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugIsoRes= TRUE;
			break;

		case KEY0:
			debugIsoRes= FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
}

void DebugEMVICCSet(void)
{
	uchar		szBuff[4];

	if(PasswordBank()!=0)
		return;

	ScrCls();
	PubDispString("DEBUG EMV ICC", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugEMV) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugEMV= TRUE;
			break;

		case KEY0:
			debugEMV= FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
	//*/
	ScrCls();
	PubDispString("DEBUG EMV CALC", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugEMVCalc) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugEMVCalc= TRUE;
			break;

		case KEY0:
			debugEMVCalc= FALSE;
			break;
		case KEYENTER:
			break;
		case NOKEY:
		case KEYCANCEL:
			return;
	}
	//*/
}

void DebugTagEmvSet(void)
{
	uchar		szBuff[4];

	if(PasswordBank()!=0)
		return;

	ScrCls();
	PubDispString("DEBUG TAG EMV", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (debugTagEmv) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			debugTagEmv= TRUE;
			break;

		case KEY0:
			debugTagEmv= FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
}

int SetSysCommLoadParam(uchar ucPermission) // Add By Zaki 24 April 2013 
{
	PubShowTitle(TRUE, (uchar *)"SETUP COMM      ");

	
		if (SetCommTypeTMS(0)!=0)
		{
			return-1;
		}

		if (SetCommDetailsTMS(&glTlmSysParam.stTMSCommCfg.ucCommType)!=0)
		{
			return-1;
		}

	
	SaveTlmSysParam();
	return 0;
}

// ucForAcq : set custom comm type for ACQ Add By Zaki
int SetCommTypeTMS(uchar ucMode)
{
	int		iRet;
	uchar 	CurrComm=0;
	char	szTitle[32];
	uchar	*pucCommType;
	SMART_MENU	stSmDownMode;
	MENU_ITEM stCommMenu[] =
	{
		{"SELECT COMM",		-1,			NULL},
		{"DISABLE",			CT_NONE,	NULL},
		{"MODEM",			CT_MODEM,	NULL},
		{"TCPIP",			CT_TCPIP,	NULL},
		{"GPRS",			CT_GPRS,	NULL},
		{"CDMA",			CT_CDMA,	NULL},
		{"WIFI",			CT_WIFI,	NULL},
		{"RS232",			CT_RS232,	NULL},
		{"DEMO ONLY",		CT_DEMO,	NULL},
		{"",				0, NULL},
	};// This menu does not provide translation

	//--------------------------------------------------
	memset(&stSmDownMode, 0, sizeof(stSmDownMode));
	PubSmInit(&stSmDownMode, stCommMenu);

	if (ucMode==0)
	{
		PubSmMask(&stSmDownMode, "DISABLE",   SM_OFF);	// Only secondary comm type can be "disabled"
	}
	if (ucMode!=0)
	{
		PubSmMask(&stSmDownMode, "DEMO ONLY", SM_OFF);	// Only primary comm type can be "demo"
	}

	if (ChkTerm(_TERMINAL_P60_S1_) || ChkTerm(_TERMINAL_P70_S_))
	{
		PubSmMask(&stSmDownMode, "TCPIP", SM_OFF);	
		PubSmMask(&stSmDownMode, "GPRS", SM_OFF);
		PubSmMask(&stSmDownMode, "CDMA", SM_OFF);
		PubSmMask(&stSmDownMode, "WIFI", SM_OFF);
	} 
	else
	{
		if (ChkHardware(HWCFG_MODEM, HW_NONE) ||
			((ucMode!=0) && (glTlmSysParam.stTMSCommCfg.ucCommType==CT_MODEM)) )
		{
			PubSmMask(&stSmDownMode, "MODEM", SM_ON);
		}
		if (ChkHardware(HWCFG_LAN, HW_NONE) ||									// If no LAN module
			((ucMode!=0) && (glTlmSysParam.stTMSCommCfg.ucCommType==CT_TCPIP)) )	// and now is selecting 2nd comm && 1st comm already selected LAN
		{
			PubSmMask(&stSmDownMode, "TCPIP", SM_ON);							// then disable this option
		}
		if ((ChkHardware(HWCFG_GPRS, HW_NONE) && !ChkTerm(_TERMINAL_P90_)) ||	// Sometimes P90 cannot get module info
			((ucMode!=0) && (glTlmSysParam.stTMSCommCfg.ucCommType==CT_GPRS)) )
		{
			PubSmMask(&stSmDownMode, "GPRS", SM_ON);
		}
		if ((ChkHardware(HWCFG_CDMA, HW_NONE) && !ChkTerm(_TERMINAL_P90_)) ||	// Sometimes P90 cannot get module info
			((ucMode!=0) && (glTlmSysParam.stTMSCommCfg.ucCommType==CT_CDMA)) )
		{
			PubSmMask(&stSmDownMode, "CDMA", SM_ON);
		}
		if ((ChkHardware(HWCFG_WIFI, HW_NONE) && !ChkTerm(_TERMINAL_S80_)) ||
			((ucMode!=0) && (glTlmSysParam.stTMSCommCfg.ucCommType==CT_WIFI)) )
		{
			PubSmMask(&stSmDownMode, "WIFI", SM_ON);
		}
		if ((ucMode!=0) && (glTlmSysParam.stTMSCommCfg.ucCommType==CT_RS232))
		{
			PubSmMask(&stSmDownMode, "RS232", SM_ON);
		}

	}

	memset(szTitle, 0, sizeof(szTitle));
	stCommMenu[0].pszText = szTitle;

	if (ucMode==0)
	{
		pucCommType = &glTlmSysParam.stTMSCommCfg.ucCommType;
		CurrComm = glTlmSysParam.stTMSCommCfg.ucCommType;
		strcpy((char *)stCommMenu[0].pszText, "LINK : ");
	}
	else
	{
		pucCommType = &glTlmSysParam.stTMSCommCfg.ucCommTypeBak;
		CurrComm = glTlmSysParam.stTMSCommCfg.ucCommTypeBak;
		strcpy((char *)stCommMenu[0].pszText, "LINK : ");
	}
	GetCommName(*pucCommType, stCommMenu[0].pszText+strlen((char *)stCommMenu[0].pszText));
	iRet = PubSmartMenuExNew(&stSmDownMode, SM_1ITEM_PER_LINE, USER_OPER_TIMEOUT);
	if (iRet<0)
	{
		return ERR_USERCANCEL;
	}

	if (!ChkIfBatchEmpty() && (*pucCommType!=(uchar)iRet))
	{
		// Not allow to switch into/off demo mode unless batch empty
		if ((*pucCommType==CT_DEMO) || ((uchar)iRet==CT_DEMO))
		{
			ScrClrLine(2,7);
			PubDispString(_T("PLS SETTLE BATCH"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(USER_OPER_TIMEOUT);
			return ERR_USERCANCEL;
		}
	}

	if(iRet==0)
		*pucCommType = CurrComm ;
	else
		*pucCommType = (uchar)iRet;
	return 0;
}

// Add By Zaki 24 April 2013
int SetCommDetailsTMS(uchar *pucCommType) 
{
	uchar	szDispBuff[32];
	int		iRet=0;
	sprintf((char *)szDispBuff, "SETUP ");
	GetCommName(*pucCommType, szDispBuff+strlen((char *)szDispBuff));
	ScrCls();
	PubDispString(szDispBuff, DISP_LINE_REVER);

	switch( *pucCommType )
	{
	case CT_RS232:
		iRet =SetBaudRateRS232();
		break;

	case CT_MODEM:
		iRet =GetDownLoadTMSNo();
		break;

	case CT_TCPIP:
		iRet =GetDownLoadRemoteIP();
	    break;

	case CT_GPRS:
	case CT_CDMA:
		iRet =GetDownLoadRemoteIP();
		break;

//	case CT_WIFI:
//		...
//		CommInitModule(&glSysParam.stTxnCommCfg);
//		break;

	case CT_DEMO:
	default:
	
	    break;
	}
	SaveTlmSysParam();
	return iRet;
}

/*
// 修改Modem参数 Add By Zaki 25 April 2013
int SetModemParamTMS(void)
{
	uchar	ucKey, szBuff[20], ucCurBaud, ucCnt, ucTemp;
	static	char	*ppszBaudRate[4] = {"1200", "2400", "9600", "14400"};

	if( SetPabx()!=0 )
	{
		return ERR_USERCANCEL;
	}

    //---------------------------------------------------
	ucTemp = glTlmSysParam.stEdcInfo.bPreDial;
	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("PRE DIAL", DISP_LINE_CENTER|2);
		sprintf((char *)szBuff, "(%c)", ucTemp ? '1' : '0');
		PubDispString(szBuff,       DISP_LINE_CENTER|4);
		PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if ( ucKey==KEY0 )
		{
			ucTemp = FALSE;
		}
		else if ( ucKey==KEY1 )
		{
			ucTemp = TRUE;
		}
	}
	glTlmSysParam.stEdcInfo.bPreDial = ucTemp;
    //---------------------------------------------------
	ucTemp = glTlmSysParam._TmsModemPara.DP;
	while( 2 )
	{
		ScrClrLine(2, 7);
		sprintf((char *)szBuff, "(%c)",  ucTemp ? '1' : '0');
		PubDispString("DIAL MODE",      DISP_LINE_CENTER|2);
		PubDispString(szBuff,           DISP_LINE_CENTER|4);
		PubDispString("0.DTMF 1.PULSE", DISP_LINE_CENTER|6);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if ( ucKey==KEY0 )
		{
			 ucTemp = 0;
		}
		else if ( ucKey==KEY1 )
		{
			ucTemp = 1;
		}
	}
	glTlmSysParam._TmsModemPara.DP = ucTemp; 

    //---------------------------------------------------

	ucTemp = glTlmSysParam._TmsModemPara.CHDT;
	while( 3 )
	{
		ScrClrLine(2, 7);
		sprintf((char *)szBuff, "(%c)", ucTemp ? '1' : '0');
		PubDispString("DIAL TONE",        DISP_LINE_CENTER|2);
		PubDispString(szBuff,             DISP_LINE_CENTER|4);
		PubDispString("0.DETECT 1IGNORE", DISP_LINE_CENTER|6);
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if ( ucKey==KEY0 )
		{
			ucTemp = 0;
		}
		else if ( ucKey==KEY1 )
		{
			ucTemp = 1;
		}
	}
	glTlmSysParam._TmsModemPara.CHDT = ucTemp;
    //---------------------------------------------------
	ScrClrLine(2, 7);
	sprintf((char *)szBuff, "OLD:%u(*100ms)", (uint)glTlmSysParam._TmsModemPara.DT1);
	PubDispString(szBuff,       DISP_LINE_LEFT|2);
	PubDispString("DIAL WAIT:", DISP_LINE_LEFT|4);
	sprintf((char *)szBuff, "%u", (uint)glTlmSysParam._TmsModemPara.DT1);
	if( PubGetString(NUM_IN|ECHO_IN, 0, 2, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
	glTlmSysParam._TmsModemPara.DT1 = (uchar)atoi((char *)szBuff);
    //---------------------------------------------------
	ScrClrLine(2, 7);
	sprintf((char *)szBuff, "OLD:%u(*100ms)", (uint)glTlmSysParam._TmsModemPara.DT2);
	PubDispString(szBuff,        DISP_LINE_LEFT|2);
	PubDispString("PABX PAUSE:", DISP_LINE_LEFT|4);
	sprintf((char *)szBuff,"%u", (uint)glTlmSysParam._TmsModemPara.DT2);
	if( PubGetString(NUM_IN|ECHO_IN, 0, 2, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
 	}
	glTlmSysParam._TmsModemPara.DT2 = (uchar)atoi((char *)szBuff);
    //---------------------------------------------------
	ScrClrLine(2, 7);
	sprintf((char *)szBuff, "OLD:%u(ms)", (uint)glTlmSysParam._TmsModemPara.HT);
	PubDispString(szBuff,           DISP_LINE_LEFT|2);
	PubDispString("ONE DTMF HOLD:", DISP_LINE_LEFT|4);
	sprintf((char *)szBuff, "%u", (uint)glTlmSysParam._TmsModemPara.HT);
	if( PubGetString(NUM_IN|ECHO_IN, 0, 3, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
	glTlmSysParam._TmsModemPara.HT = (uchar)atoi((char *)szBuff);

    //---------------------------------------------------
	ScrClrLine(2, 7);
	sprintf((char *)szBuff, "OLD:%u(*10ms)", (uint)glTlmSysParam._TmsModemPara.WT);
	PubDispString(szBuff,             DISP_LINE_LEFT|2);
	PubDispString("DTMF CODE SPACE:", DISP_LINE_LEFT|4);
	sprintf((char *)szBuff, "%u", (uint)glTlmSysParam._TmsModemPara.WT);
	if( PubGetString(NUM_IN|ECHO_IN, 0, 3, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
	glTlmSysParam._TmsModemPara.WT = (uchar)atoi((char *)szBuff);

    //---------------------------------------------------
    while (1)
    {
	ScrClrLine(2, 7);
	sprintf((char *)szBuff, "OLD:%u(0, 1~15)", (uint)glTlmSysParam._TmsPSTNPara.ucSignalLevel);
	PubDispString(szBuff,             DISP_LINE_LEFT|2);
	PubDispString("SIGNAL LEVEL:", DISP_LINE_LEFT|4);
	sprintf((char *)szBuff, "%u", (uint)glTlmSysParam._TmsPSTNPara.ucSignalLevel);
	if( PubGetString(NUM_IN|ECHO_IN, 0, 2, szBuff, USER_OPER_TIMEOUT)!=0 )
	{
		return ERR_USERCANCEL;
	}
        ucTemp = (uchar)atoi((char *)szBuff);

	if (ucTemp<16)
        {
	        glTlmSysParam._TmsPSTNPara.ucSignalLevel = ucTemp;
            	break;
        }
    }

    //---------------------------------------------------
	ucCurBaud = (glTlmSysParam._TmsModemPara.SSETUP>>5) & 0x03;
	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("BAUD RATE:", 2|DISP_LINE_LEFT);
		for(ucCnt=0; ucCnt<4; ucCnt++)
		{
			ScrPrint(0, (uchar)(ucCnt+4), ASCII, "   %s%d.%s", (ucCnt==ucCurBaud ? ">" : " "),
					(int)(ucCnt+1), ppszBaudRate[ucCnt]);
		}
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if( ucKey==KEYUP )
		{
			ucCurBaud = (ucCurBaud==0) ? 3 : ucCurBaud-1;
		}
		else if( ucKey==KEYDOWN )
		{
			ucCurBaud = (ucCurBaud>=3) ? 0 : ucCurBaud+1;
		}
		else if( ucKey>=KEY1 && ucKey<=KEY4 )
		{
			ucCurBaud = ucKey - KEY1;
			break;
		}
	}
	glTlmSysParam._TmsModemPara.SSETUP &= 0x9F;	// 1001 1111
	glTlmSysParam._TmsModemPara.SSETUP |= (ucCurBaud<<5);
	return 0;
}
*/

int  SetBaudRateRS232(void)
{
	uchar	ucKey,  ucCurBaud, ucCnt;
	uchar 	szAttr[20+1];
	int iLoop;
	
	static	char	*ppszBaudRate[4] = {"9600\0", "19200\0", "38400\0", "115200\0"};


   //----------------------------------------------------
   //ucCurBaud = (glTlmSysParam._TxnModemPara.SSETUP>>5) & 0x03;
   //memcpy(szAttr, glTlmSysParam._TmsRS232Para.szAttr, sizeof(szAttr));

	memset(szAttr, 0, sizeof(szAttr));
	
	for(iLoop=0;iLoop<(int)strlen(glTlmSysParam._TmsRS232Para.szAttr);iLoop++)
	{
		if(glTlmSysParam._TmsRS232Para.szAttr[iLoop]==',')
			break;

		szAttr[iLoop]=glTlmSysParam._TmsRS232Para.szAttr[iLoop];
	}

   //hex_dump_char("szAttr", szAttr, strlen(szAttr));
	
	ucCurBaud = 0;
	for(iLoop=0;iLoop<4;iLoop++)
	{
		if(strcmp(szAttr, ppszBaudRate[iLoop])==0)
		{
			ucCurBaud=iLoop;
			break;
		}
	}

		
	
	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString("BAUD RATE:", 2|DISP_LINE_LEFT);
		for(ucCnt=0; ucCnt<4; ucCnt++)
		{
			ScrPrint(0, (uchar)(ucCnt+4), ASCII, "   %s%d.%s", (ucCnt==ucCurBaud ? ">" : " "),
					(int)(ucCnt+1), ppszBaudRate[ucCnt]);
		}
		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYENTER )
		{
			break;
		}
		else if( ucKey==KEYUP )
		{
			ucCurBaud = (ucCurBaud==0) ? 3 : ucCurBaud-1;
		}
		else if( ucKey==KEYDOWN )
		{
			ucCurBaud = (ucCurBaud>=3) ? 0 : ucCurBaud+1;
		}
		else if( ucKey>=KEY1 && ucKey<=KEY4 )
		{
			ucCurBaud = ucKey - KEY1;
			break;
		}
	}

	glTlmSysParam._TmsRS232Para.ucPortNo   = COM1;
	glTlmSysParam._TmsRS232Para.ucSendMode = CM_RAW;
	strcpy((char *)glTlmSysParam._TmsRS232Para.szAttr, ppszBaudRate[ucCurBaud]);
	strcat((char *)glTlmSysParam._TmsRS232Para.szAttr,",8,n,1");

	return 0;
}


void testprintamount(void)
{
	uchar pszIn[12+1];
	uchar pszPrefix[12];
	uchar ucDeciPos;
	uchar ucIgnoreDigit;
	uchar pszOut[32];
	uchar ucMisc;

	MEM_ZERO(pszIn);
	MEM_ZERO(pszPrefix);
	MEM_ZERO(pszOut);

	strcpy(pszPrefix, "Rp.");
	strcpy(pszIn, "001234567812");

	PrnInit();
	PrnStr(pszIn);
	PrnStr("\n");
//   1
	ucDeciPos=0;
	ucIgnoreDigit=0;
	ucMisc=GA_SEPARATOR;
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   2
	ucDeciPos=1;
	ucIgnoreDigit=0;
	ucMisc=GA_SEPARATOR;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   3
	ucDeciPos=2;
	ucIgnoreDigit=0;
	ucMisc=GA_SEPARATOR;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   4
	ucDeciPos=0;
	ucIgnoreDigit=1;
	ucMisc=GA_SEPARATOR;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   5
	ucDeciPos=0;
	ucIgnoreDigit=2;
	ucMisc=GA_SEPARATOR;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   6
	ucDeciPos=1;
	ucIgnoreDigit=1;
	ucMisc=GA_SEPARATOR;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   7
	ucDeciPos=2;
	ucIgnoreDigit=2;
	ucMisc=GA_SEPARATOR;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   8
	ucDeciPos=1;
	ucIgnoreDigit=2;
	ucMisc=GA_SEPARATOR;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   2
	ucDeciPos=2;
	ucIgnoreDigit=1;
	ucMisc=GA_SEPARATOR;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");
//   2
	ucDeciPos=0;
	ucIgnoreDigit=0;
	ucMisc=GA_NEGATIVE;
	MEM_ZERO(pszOut);
	PubConvAmount(pszPrefix, pszIn, ucDeciPos, ucIgnoreDigit, pszOut, ucMisc);
	PrnStr(pszOut);
	PrnStr("\n");

	PrnStart();
	
}

void SetAPN (void)
{

	int ret;
	char *menu[]={
		"SET APN>",
		"MANUAL",
		"BRI T-SEL",
		"BRI MATRIX",
		"BRI XL",
		0,
	};

	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch(ret){
		case 1:	
			SetWirelessParam(&glTlmSysParam._TxnWirlessPara);
			break;
		case 2:
			SetAPNTelkomsel(&glTlmSysParam._TxnWirlessPara);
			break;	
		case 3:	
			SetAPNIndosat(&glTlmSysParam._TxnWirlessPara);
			break;
		case 4:
			SetAPNXL(&glTlmSysParam._TxnWirlessPara);
			break;	
		default:
			break;
	};
}


void SetBIT48(void)
{
	uchar		szBuff[4];

	ScrCls();
	PubDispString("BIT 48", DISP_LINE_CENTER|2);
	sprintf((char *)szBuff, "(%c)", (BIT48ON) ? '1' : '0');
		
	PubDispString(szBuff,       DISP_LINE_CENTER|4);
	PubDispString("1.ON 0.OFF", DISP_LINE_CENTER|6);
	switch( PubWaitKey(USER_OPER_TIMEOUT) )
	{
		case KEY1:
			BIT48ON= TRUE;
			break;

		case KEY0:
			BIT48ON= FALSE;
			break;
		case KEYENTER:
			break;

		case NOKEY:
		case KEYCANCEL:
			return;
	}
}

