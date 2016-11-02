
#include "global.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal constants declaration *********************/
/********************** Internal functions declaration *********************/
static uchar ChkIfAcqAvail(uchar ucIndex);

/********************** Internal variables declaration *********************/
/********************** external reference declaration *********************/
#if defined(_P60_S1_) || defined(_P70_S_)
extern const APPINFO	AppInfo;
#endif

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// 检测机器型号
// Check Term Model
uchar ChkTerm(uchar ucTermType)
{
#ifdef _P60_S1_
	return (ucTermType==_TERMINAL_P60_S1_);
#elif defined(_P70_S_)
	return (ucTermType==_TERMINAL_P70_S_);
#else
#ifdef __TELIUM_PARAM__
	return (glTlmSysParam.sTermInfo[HWCFG_MODEL]==ucTermType);
#endif
	return (glSysParam.sTermInfo[HWCFG_MODEL]==ucTermType);
#endif
}

// 检查硬件配置。信息源自GetTermInfo()的返回(缓存在glSysParam.sTermInfo[])
// Check Term Hardware Config, by Checking the info returned by GetTermInfo() (buffered in glSysParam.sTermInfo[])
uchar ChkHardware(uchar ucChkType, uchar ucValue)
{
	PubASSERT(ucChkType<HWCFG_END);

#ifdef __TELIUM_PARAM__
	return (glTlmSysParam.sTermInfo[ucChkType]==ucValue);	// return value: TRUE/FALSE
#endif

	return (glSysParam.sTermInfo[ucChkType]==ucValue);	// return value: TRUE/FALSE
}

// 检查是否红外通信的分离式打印机 Check whether it is an IrDA-communication printer
uchar ChkIfIrDAPrinter(void)
{
	return (ChkTerm(_TERMINAL_P60_S1_) || ChkTerm(_TERMINAL_S60_));
}

// 1.00.0009 delete
//uchar ChkIfSupportCommType(uchar ucCommType)

uchar ChkTermPEDMode(uchar ucMode)
{
#ifdef	__TELIUM_PARAM__
	return (glTlmSysParam.stEdcInfo.ucPedMode==ucMode);
#else
	return (glSysParam.stEdcInfo.ucPedMode==ucMode);
#endif
}

// 扫描所有收单行信息,判断终端是否指出EMV
// Scan all Acquirer and check whether one of them support EMV
uchar ChkIfEmvEnable(void)
{
#ifdef ENABLE_EMV
	uchar	i;

#ifdef	__TELIUM_PARAM__
	for(i=0; i<glTlmTmsAid.ucAidNum; i++)
	{
		if(glTlmTmsAid.Aid[i].emvAcqId != 0)
		{
			return TRUE;
		}
	}
#else
	for(i=0; i<glSysParam.ucAcqNum; i++)
	{
		if( ChkOptionExt(glSysParam.stAcqList[i].sOption, ACQ_EMV_FEATURE) )
		{
			return TRUE;
		}
	}
#endif	//__TELIUM_PARAM__
#endif

	return FALSE;
}

// 检查当前发卡行的选项
// Check current Issuer OPtion
uchar ChkIssuerOption(ushort uiOption)
{
	PubASSERT( (uiOption>>8)<sizeof(glCurIssuer.sOption) );
	return ChkOptionExt(glCurIssuer.sOption, uiOption);
}

// 检查当前收单行选项
// Check Current Acquirer option
uchar ChkAcqOption(ushort uiOption)
{
	PubASSERT( (uiOption>>8)<sizeof(glCurAcq.sOption) );
	return ChkOptionExt(glCurAcq.sOption, uiOption);
}

// 检查终端选项
// Check EDC option
uchar ChkEdcOption(ushort uiOption)
{
#ifdef	__TELIUM_PARAM__
	PubASSERT( (uiOption>>8)<sizeof(glTlmSysParam.stEdcInfo.sOption) );
	return ChkOptionExt(glTlmSysParam.stEdcInfo.sOption, uiOption);
#else
	PubASSERT( (uiOption>>8)<sizeof(glSysParam.stEdcInfo.sOption) );
	return ChkOptionExt(glSysParam.stEdcInfo.sOption, uiOption);
#endif
}

// 扩展选项检查
// Extension of option checking
uchar ChkOptionExt(uchar *psExtOpt, ushort uiOption)
{
	return (psExtOpt[uiOption>>8] & (uiOption & 0xFF));
}

void SetOptionExt(uchar *psExtOpt, ushort uiOption)
{
	psExtOpt[uiOption>>8] |= (uiOption & 0xFF);
}

// 功能：是否必须输入密码 入口：无 出口： TRUE 必输入 FALSE 不必输入
// Check whether need PIN for current Issuer
uchar ChkIfNeedPIN(void)
{
#ifdef	__TELIUM_PARAM__
	if(glCurTlmTmsIss.issPinOpt == 0x32)
		return TRUE;
	else
		return FALSE;
#else
	return ChkIssuerOption(ISSUER_EN_PIN);
#endif
}

uchar ChkIfPinReqdAllIssuer(void)
{
	uchar	i;

#ifdef	__TELIUM_PARAM__
	for(i=0; i<glTlmSysParam.ucIssNum; i++)
	{
		if( glTlmSysParam.Iss[i].issPinOpt == 0x32)
		{
			return TRUE;
		}
	}
#else
	for(i=0; i<glSysParam.ucIssuerNum; i++)
	{
		if( ChkOptionExt(glSysParam.stIssuerList[i].sOption, ISSUER_EN_PIN) )
		{
			return TRUE;
		}
	}
#endif
	return FALSE;
}

uchar ChkIfBRICredit(void)
{
	return ChkCurAcqName("CREDIT", FALSE);
}

uchar ChkIfBRIDebit(void)
{
	return ChkCurAcqName("DEBIT", FALSE);
}


// 检查磁卡交易金额是否低于Floor Limit
// Check whether below floor limit. Only for Non-EMV.
uchar ChkIfBelowMagFloor(void)
{
	int		ii;
	uchar	szBuff[12+1];

#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "%lu", glCurTlmTmsIss.issFloorLimit);
	for (ii=0; ii<glTlmSysParam.stEdcInfo.stLocalCurrency.ucDecimal; ii++)
	{
		strcat(szBuff, "0");
	}
	AmtConvToBit4Format(szBuff, glTlmSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit);
	// Now szBuff is local floor limit
	if( memcmp(glProcInfo.stTranLog.szAmount, szBuff, 12)>=0 )
	{
		return FALSE;
	}

#else
	sprintf((char *)szBuff, "%lu", glCurIssuer.ulFloorLimit);
	for (ii=0; ii<glSysParam.stEdcInfo.stLocalCurrency.ucDecimal; ii++)
	{
		strcat(szBuff, "0");
	}
	AmtConvToBit4Format(szBuff, glSysParam.stEdcInfo.stLocalCurrency.ucIgnoreDigit);
	// Now szBuff is local floor limit
	if( memcmp(glProcInfo.stTranLog.szAmount, szBuff, 12)>=0 )
	{
		return FALSE;
	}
#endif
	return TRUE;
}

// 在未事先确定交易类型的情况下，自动判断选择需要进行的交易
void CheckCapture(void)
{
	if( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_SALE_OR_AUTH )
	{
		return;
	}

	if (ChkInstalmentAllAcq() && ChkEdcOption(EDC_ENABLE_INSTALMENT))	// 当所有ACQ都是installment
	{
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_INSTALMENT;
	}
	else if( ChkEdcOption(EDC_CASH_PROCESS) )
	{
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_CASH;
	}
	else if( ChkIssuerOption(ISSUER_CAPTURE_TXN) )
	{
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE;
	}
	else
	{
		if( ChkEdcOption(EDC_AUTH_PREAUTH) )
		{
			glProcInfo.stTranLog.ucTranType = TRANS_TYPE_AUTH;
		}
		else
		{
			glProcInfo.stTranLog.ucTranType = TRANS_TYPE_PREAUTH;
		}
	}
}

uchar ChkInstalmentAllAcq(void)
{
#ifdef	__TELIUM_PARAM__
	return FALSE;
#else
	uchar	sBuff[256];
	uchar	i;

	memset(sBuff, 0, sizeof(sBuff));
	for(i=0; i<glSysParam.ucPlanNum; i++)
	{
		sBuff[glSysParam.stPlanList[i].ucAcqIndex] = 1;
	}

	for(i=0; i<glSysParam.ucAcqNum; i++)
	{
		if( sBuff[i]==0 )
		{
			return FALSE;
		}
	}

	return TRUE;
#endif	
}

uchar ChkIfDispMaskPan2(void)
{
	return ChkOptionExt(glCurIssuer.sPanMask, 0x0040);
}

uchar ChkIfInstalmentPara(void)
{
#ifdef	__TELIUM_PARAM__
	return FALSE;
#else
	if( !ChkEdcOption(EDC_ENABLE_INSTALMENT) )
	{
		return FALSE;
	}

	return (glSysParam.ucPlanNum > 0);
#endif	
}

uchar ChkIfTransMaskPan(uchar ucCurPage)
{
	PubASSERT(ucCurPage>=0 && ucCurPage<4);
	
	if ( ((glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PREAUTH) || (glProcInfo.stTranLog.ucTranType==TRANS_TYPE_AUTH)) &&
			!ChkEdcOption(EDC_AUTH_PAN_MASKING))
	{
		return FALSE;
	}

	return TRUE;
}

// compare acquirer name in upper case
uchar ChkCurAcqName(void *pszKeyword, uchar ucPrefix)
{
	uchar	szBuff[10+1];

#ifdef	__TELIUM_PARAM__
	sprintf((char *)szBuff, "%.10s", glCurTlmTmsAcq.acqName);
#else
	sprintf((char *)szBuff, "%.10s", glCurAcq.szName);
#endif

	PubStrUpper(szBuff);

	if (ucPrefix)	// the specific string is only allowed at the beginning of the acquirer name
	{
		if( memcmp(szBuff, pszKeyword, strlen((char *)pszKeyword))==0 )
		{
			return TRUE;
		}
	} 
	else	// the specific string is allowed at any place within acquirer name string
	{
		if (strstr(szBuff, pszKeyword)!=NULL)
		{
			return TRUE;
		}
	}

	return FALSE;
}

uchar ChkIfTranAllow(uchar ucTranType)
{
	if( ucTranType==TRANS_TYPE_SETTLEMENT || ucTranType==TRANS_TYPE_BATCH_UPLOAD )
	{
		return TRUE;
	}

	if( GetTranLogNum(ACQ_ALL)>=MAX_TRANLOG )
	{
		PubShowTitle(TRUE, (uchar *)_T("BATCH FULL"));
		PubDispString(_T("PLS SETTLE BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return FALSE;
	}

	if( (glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INSTALMENT) && !ChkIfInstalmentPara() )
	{
		return FALSE;
	}

	return TRUE;
}

uchar ChkIfZeroAmt(uchar *pszIsoAmountStr)
{
	if (strcmp(pszIsoAmountStr, "")==0)
	{
		return TRUE;
	}
	if (strcmp(pszIsoAmountStr, "0")==0)
	{
		return TRUE;
	}
	return (strcmp(pszIsoAmountStr, "000000000000")==0);
}

uchar ChkIfBatchEmpty(void)
{
	int	ii;
	for (ii=0; ii<MAX_TRANLOG; ii++)
	{
		if (glSysCtrl.sAcqKeyList[ii] != INV_ACQ_KEY)
		{
			return FALSE;
		}
	}
	return TRUE;
}

uchar ChkIfBatchBrizziEmpty(void)
{
	int	ii;
	for (ii=0; ii<MAX_TRANLOG; ii++)
	{
		if (glSysCtrl.sBrizziAcqKeyList[ii] != INV_ACQ_KEY)
		{
			return FALSE;
		}
	}
	return TRUE;
}

uchar ChkIfBatchMiniAtmCSEmpty(void)
{
	int	ii;
	for (ii=0; ii<MAX_TRANLOG; ii++)
	{
		if (glSysCtrl.sMiniAtmCSAcqKeyList[ii] != INV_ACQ_KEY)
		{
			return FALSE;
		}
	}
	return TRUE;
}

uchar ChkIfBatchTunaiEmpty(void)
{
	int	ii;
	for (ii=0; ii<MAX_TRANLOG; ii++)
	{
		if (glSysCtrl.sTunaiAcqKeyList[ii] != INV_ACQ_KEY)
		{
			return FALSE;
		}
	}
	return TRUE;
}

uchar ChkIfZeroTotal(void *pstTotal)
{
	TOTAL_INFO	*pTotal = (TOTAL_INFO *)pstTotal;

	if ( pTotal->uiSaleCnt==0 && pTotal->uiRefundCnt==0 &&
		pTotal->uiVoidSaleCnt==0 && pTotal->uiVoidRefundCnt==0 // &&
		//pTotal->uiSaleNonManualCnt==0 && pTotal->uiSaleManualCnt==0 &&
		//pTotal->uiVoidSaleNonManualCnt==0 && pTotal->uiVoidSaleManualCnt==0
		)
	{
		return TRUE;
	}
	return FALSE;
}

uchar ChkSettle(void)
{
#ifdef	__TELIUM_PARAM__
	if( glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex]==S_PENDING )
	{
		PubShowTitle(TRUE, (uchar *)_T("SETTLE PENDING"));
		PubDispString(_T("PLS SETTLE BATCH"), 4|DISP_LINE_LEFT);
		PubLongBeep();
		PubWaitKey(5);
		return FALSE;
	}
#else
	if( glSysCtrl.sAcqStatus[glCurAcq.ucIndex]==S_PENDING )
	{
		PubShowTitle(TRUE, (uchar *)_T("SETTLE PENDING"));
		PubDispString(_T("PLS SETTLE BATCH"), 4|DISP_LINE_LEFT);
		PubLongBeep();
		PubWaitKey(5);
		return FALSE;
	}
#endif

	return TRUE;
}

uchar ChkIfNeedTip(void)
{
#ifdef	__TELIUM_PARAM__
	if((glTlmSysParam.Con.appTermOpt1 & 0x04) &&
		((glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE)     ||
		 (glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFF_SALE) ||
		 (glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CASH))      )
	{
		return TRUE;
	}
#else
	if( ChkEdcOption(EDC_TIP_PROCESS) &&
		((glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE)     ||
		 (glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFF_SALE) ||
		 (glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CASH))      )
	{
		return TRUE;
	}
#endif
	return FALSE;
}

uchar ChkIfAcqAvail(uchar ucIndex)
{
	return (glSysCtrl.sAcqStatus[ucIndex]!=S_RESET);
}


uchar ChkIfIccTran(ushort uiEntryMode)
{
	if( (uiEntryMode & MODE_CHIP_INPUT) ||
		(uiEntryMode & MODE_FALLBACK_SWIPE) ||
		(uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		return TRUE;
	}

	return FALSE;
}

uchar ChkIfPrnReceipt(void)
{
	PubASSERT( glProcInfo.stTranLog.ucTranType<MAX_TRANTYPE );
	return (glTranConfig[glProcInfo.stTranLog.ucTranType].ucTranAct & PRN_RECEIPT);
}

uchar ChkIfNeedReversal(void)
{
	PubASSERT( glProcInfo.stTranLog.ucTranType<MAX_TRANTYPE );
	return (glTranConfig[glProcInfo.stTranLog.ucTranType].ucTranAct & NEED_REVERSAL);
}

uchar ChkIfSaveLog(void)
{
	PubASSERT( glProcInfo.stTranLog.ucTranType<MAX_TRANTYPE );
	return (glTranConfig[glProcInfo.stTranLog.ucTranType].ucTranAct & WRT_RECORD);
}

uchar ChkIfThermalPrinter(void)
{
#if defined(_P60_S1_) || defined(_P70_S_)
    return FALSE;
#endif
#ifdef	__TELIUM_PARAM__
	return (glTlmSysParam.stEdcInfo.ucPrinterType==1);
#else
	return (glSysParam.stEdcInfo.ucPrinterType==1);
#endif
}

uchar ChkIfNeedSecurityCode(void)
{
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_OFF_SALE || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID)
	{
		return FALSE;
	}

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_REFUND )
	{
		return FALSE;
	}

	if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT ||
		glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE )
	{
		if( ChkIssuerOption(ISSUER_SECURITY_SWIPE) )
		{
			return TRUE;
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT) ||
			 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		if( ChkIssuerOption(ISSUER_SECURITY_MANUL) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

uchar ChkIfNeedMac(void)
{
#ifdef BANK_MANDIRI
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_TMS_TELIUM:
		case TRANS_TYPE_LTMK:
		case TRANS_TYPE_LTWK:
		case TRANS_TYPE_SETTLEMENT:
		case TRANS_TYPE_ECHO_TEST:
			return FALSE;
			break;
	}
	
	return TRUE;
#else
	return FALSE;
#endif	
}

uchar ChkIfAcqNeedTC(void)
{
	// more banks may need to add in later
	return FALSE;
}



// 检查是否允许退出程序
uchar ChkIfAllowExit(void)
{
#ifdef _WIN32
	return FALSE;
#else
	APPINFO	stTempAppInf;

	if (ReadAppInfo(0, &stTempAppInf)==0)
	{
		return TRUE;
	}
	return FALSE;
#endif
}

//irf, 20121129
uchar ChkIfKeysAlreadyLoaded(uchar ucTranType)
{

//irf, 20121129
	if(!CheckFileTMK() && !CheckFileTWK() && ChkIfNeedPIN())
//	if((!CheckFileTMK()) && (!CheckFileTWK()))
	{
		PubShowTitle(TRUE, (uchar *)_T("KEYS ERROR"));
		PubDispString(_T("KEYS NOT LOAD"), 4|DISP_LINE_CENTER);
		PubBeepErr();
		PubWaitKey(5);
		return FALSE;
	}
//

	return TRUE;
}

uchar ChkIfBatchFull(void)
{
	if( GetTranLogNum(ACQ_ALL)>=MAX_TRANLOG )
	{
		PubShowTitle(TRUE, (uchar *)_T("BATCH FULL"));
		PubDispString(_T("PLS SETTLE BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return FALSE;
	}
	return TRUE;
}


uchar ChkIfBatchBrizziFull(void)
{
	if( GetBrizziTranLogNum(ACQ_ALL)>=MAX_TRANLOG )
	{
		PubShowTitle(TRUE, (uchar *)_T("BATCH FULL"));
		PubDispString(_T("PLS SETTLE BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return FALSE;
	}
	return TRUE;
}


#ifdef	__TELIUM_PARAM__
static uchar EMVMiscGetTransTypeChipInitEntryMode(void)
{
	uchar InitChipResult = FALSE;
	
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE:
			InitChipResult = (glTlmSysParam.Con.appEMSale & EM_CHIP_INITIAL);
			break;

		case TRANS_TYPE_REFUND:
			InitChipResult = (glTlmSysParam.Con.appEMRefund & EM_CHIP_INITIAL);
			break;

		case TRANS_TYPE_OFF_SALE:
			InitChipResult = (glTlmSysParam.Con.appEMOffline & EM_CHIP_INITIAL);
			break;

		case TRANS_TYPE_CASH:
			InitChipResult = (glTlmSysParam.Con.appEMCash & EM_CHIP_INITIAL);
			break;
			
		case TRANS_TYPE_PREAUTH:
			InitChipResult = (glTlmSysParam.Con.appEMPreAuth & EM_CHIP_INITIAL);
			break;

		case TRANS_TYPE_CARD_VERIFY:
			InitChipResult = (glTlmSysParam.Con.appEMCrdVrfy & EM_CHIP_INITIAL);
			break;

		case TRANS_TYPE_BALANCE:
			InitChipResult = (glTlmSysParam.Con.appEMBalance & EM_CHIP_INITIAL);
			break;
	}

	if(InitChipResult>0)
		InitChipResult = TRUE;
	
	return InitChipResult;
}

static uchar EMVMiscGetTransTypeMsgInitEntryMode(void)
{
	uchar InitMsgResult = FALSE;
	
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE:
			InitMsgResult = (glTlmSysParam.Con.appEMSale & EM_MAGNETIC_STRIPE_INITIAL);
			break;

		case TRANS_TYPE_PREAUTH:
			InitMsgResult = (glTlmSysParam.Con.appEMPreAuth & EM_MAGNETIC_STRIPE_INITIAL);
			break;

		case TRANS_TYPE_CARD_VERIFY:
			InitMsgResult = (glTlmSysParam.Con.appEMCrdVrfy & EM_MAGNETIC_STRIPE_INITIAL);
			break;

		case TRANS_TYPE_BALANCE:
			InitMsgResult = (glTlmSysParam.Con.appEMBalance & EM_MAGNETIC_STRIPE_INITIAL);
			break;

		case TRANS_TYPE_CASH:
			InitMsgResult = (glTlmSysParam.Con.appEMCash & EM_MAGNETIC_STRIPE_INITIAL);
			break;

		case TRANS_TYPE_OFF_SALE:
			InitMsgResult = (glTlmSysParam.Con.appEMOffline & EM_MAGNETIC_STRIPE_INITIAL);
			break;

		case TRANS_TYPE_REFUND:
			InitMsgResult = (glTlmSysParam.Con.appEMRefund & EM_MAGNETIC_STRIPE_INITIAL);
			break;

	}

	if(InitMsgResult>0)
		InitMsgResult = TRUE;
	
	return InitMsgResult;
}


static uchar EMVMiscGetTransTypeManualInitEntryMode(void)
{
	uchar InitManualResult = FALSE;
	
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE:
			InitManualResult = (glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL);
			break;

		case TRANS_TYPE_PREAUTH:
			InitManualResult = (glTlmSysParam.Con.appEMPreAuth & EM_MANUAL_INITIAL);
			break;

		case TRANS_TYPE_CARD_VERIFY:
			InitManualResult = (glTlmSysParam.Con.appEMCrdVrfy & EM_MANUAL_INITIAL);
			break;

		case TRANS_TYPE_BALANCE:
			InitManualResult = (glTlmSysParam.Con.appEMBalance & EM_MANUAL_INITIAL);
			break;

		case TRANS_TYPE_CASH:
			InitManualResult = (glTlmSysParam.Con.appEMCash & EM_MANUAL_INITIAL);
			break;

		case TRANS_TYPE_OFF_SALE:
			InitManualResult = (glTlmSysParam.Con.appEMOffline & EM_MANUAL_INITIAL);
			break;

		case TRANS_TYPE_REFUND:
			InitManualResult = (glTlmSysParam.Con.appEMRefund & EM_MANUAL_INITIAL);
			break;

	}

	if(InitManualResult>0)
		InitManualResult = TRUE;
	
	return InitManualResult;
}

static uchar EMVMiscGetTransTypeFallbackChipInitEntryMode(void)
{
	uchar InitChipResult = FALSE;
	
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE:
			InitChipResult = (glTlmSysParam.Con.appEMSale & EM_FALLBACK_CHIP);
			break;

		case TRANS_TYPE_PREAUTH:
			InitChipResult = (glTlmSysParam.Con.appEMPreAuth & EM_FALLBACK_CHIP);
			break;

		case TRANS_TYPE_CARD_VERIFY:
			InitChipResult = (glTlmSysParam.Con.appEMCrdVrfy & EM_FALLBACK_CHIP);
			break;

		case TRANS_TYPE_BALANCE:
			InitChipResult = (glTlmSysParam.Con.appEMBalance & EM_FALLBACK_CHIP);
			break;

		case TRANS_TYPE_CASH:
			InitChipResult = (glTlmSysParam.Con.appEMCash & EM_FALLBACK_CHIP);
			break;

		case TRANS_TYPE_OFF_SALE:
			InitChipResult = (glTlmSysParam.Con.appEMOffline & EM_FALLBACK_CHIP);
			break;

		case TRANS_TYPE_REFUND:
			InitChipResult = (glTlmSysParam.Con.appEMRefund & EM_FALLBACK_CHIP);
			break;

	}

	if(InitChipResult>0)
		InitChipResult = TRUE;
	
	return InitChipResult;
}

static uchar EMVMiscGetTransTypeFallbackMsgInitEntryMode(void)
{
	uchar InitMsgResult = FALSE;
	
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE:
			InitMsgResult = (glTlmSysParam.Con.appEMSale & EM_FALLBACK_MAGNETIC_STRIP);
			break;

		case TRANS_TYPE_PREAUTH:
			InitMsgResult = (glTlmSysParam.Con.appEMPreAuth & EM_FALLBACK_MAGNETIC_STRIP);
			break;

		case TRANS_TYPE_CARD_VERIFY:
			InitMsgResult = (glTlmSysParam.Con.appEMCrdVrfy & EM_FALLBACK_MAGNETIC_STRIP);
			break;

		case TRANS_TYPE_BALANCE:
			InitMsgResult = (glTlmSysParam.Con.appEMBalance & EM_FALLBACK_MAGNETIC_STRIP);
			break;

		case TRANS_TYPE_CASH:
			InitMsgResult = (glTlmSysParam.Con.appEMCash & EM_FALLBACK_MAGNETIC_STRIP);
			break;

		case TRANS_TYPE_OFF_SALE:
			InitMsgResult = (glTlmSysParam.Con.appEMOffline & EM_FALLBACK_MAGNETIC_STRIP);
			break;

		case TRANS_TYPE_REFUND:
			InitMsgResult = (glTlmSysParam.Con.appEMRefund & EM_FALLBACK_MAGNETIC_STRIP);
			break;

	}

	if(InitMsgResult>0)
		InitMsgResult = TRUE;
	
	return InitMsgResult;
}


static uchar EMVMiscGetTransTypeFallbackManualInitEntryMode(void)
{
	uchar InitManualResult = FALSE;
	
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE:
			InitManualResult = (glTlmSysParam.Con.appEMSale & EM_FALLBACK_MANUAL);
			break;

		case TRANS_TYPE_PREAUTH:
			InitManualResult = (glTlmSysParam.Con.appEMPreAuth & EM_FALLBACK_MANUAL);
			break;

		case TRANS_TYPE_CARD_VERIFY:
			InitManualResult = (glTlmSysParam.Con.appEMCrdVrfy & EM_FALLBACK_MANUAL);
			break;

		case TRANS_TYPE_BALANCE:
			InitManualResult = (glTlmSysParam.Con.appEMBalance & EM_FALLBACK_MANUAL);
			break;

		case TRANS_TYPE_CASH:
			InitManualResult = (glTlmSysParam.Con.appEMCash & EM_FALLBACK_MANUAL);
			break;

		case TRANS_TYPE_OFF_SALE:
			InitManualResult = (glTlmSysParam.Con.appEMOffline & EM_FALLBACK_MANUAL);
			break;

		case TRANS_TYPE_REFUND:
			InitManualResult = (glTlmSysParam.Con.appEMRefund & EM_FALLBACK_MANUAL);
			break;

	}

	if(InitManualResult>0)
		InitManualResult = TRUE;
	
	return InitManualResult;
}

uchar CheckTransAllowed(void)
{
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_PRIVILAGE:
		case TRANS_TYPE_CICILAN_BRING:
		case TRANS_TYPE_SALE_REDEEMPTION:
		case TRANS_TYPE_CICILAN_BUKOPIN:
			
		case TRANS_TYPE_SMSBANKING:
		case TRANS_TYPE_PHONEBANKING:
		case TRANS_TYPE_IBANKING:
			
		case TRANS_TYPE_INFOSALDO:
		case TRANS_TYPE_INFOSALDO_BANKLAIN:
		case TRANS_TYPE_MINISTATEMENT:
		case TRANS_TYPE_MUTASIREKENING:
			
		case TRANS_TYPE_INQ_TRFSESAMABRI:
		case TRANS_TYPE_INQ_TRFANTARBANK:

		case TRANS_TYPE_INQ_TELKOM:
		case TRANS_TYPE_INQ_KARTUHALO:
		case TRANS_TYPE_INQ_KARTUMATRIX:

		case TRANS_TYPE_INQ_PLNPASCABAYAR:
		case TRANS_TYPE_INQ_PLNPRABAYAR:
		case TRANS_TYPE_PLNTOKEN:

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
			
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
		// case TRANS_TYPE_FREN_MOBI_HEPI: //ditutup 6 Januari 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:

		case TRANS_TYPE_AKTIFASIKARTU_PETUGAS:
		case TRANS_TYPE_AKTIFASIKARTU_NASABAH:

		case TRANS_TYPE_REISSUE_PIN_PETUGAS:

		case TRANS_TYPE_UBAHPIN_CS:
		case TRANS_TYPE_UBAHPWD_SPV:
			
		case TRANS_TYPE_ABSENSI:
		//BRIZZI
		case TRANS_TYPE_BRIZZI_PURCHASE:
		case TRANS_TYPE_BRIZZI_TOPUP_ONLINE:
		case TRANS_TYPE_BRIZZI_AKTV_DEPOS:
		case TRANS_TYPE_BRIZZI_INFO_DEPOS:
		case TRANS_TYPE_BRIZZI_TOPUP_DEP:
		case TRANS_TYPE_BRIZZI_REAKTIVASI:


		// MINI ATM BUKOPIN DIKI
		case TRANS_TYPE_INQ_TRFANTARBANK_BKPN:
		case TRANS_TYPE_TRFANTARBANK_BKPN:



		//multi finance mini atm bukopin
		case TRANS_TYPE_INQ_MAF_BKPN:
		case TRANS_TYPE_MAF_BKPN:
		case TRANS_TYPE_INQ_BAF_BKPN:
		case TRANS_TYPE_BAF_BKPN:
		case TRANS_TYPE_INQ_MCF_BKPN:
		case TRANS_TYPE_MCF_BKPN:
		case TRANS_TYPE_INQ_WOM_BKPN:
		case TRANS_TYPE_WOM_BKPN:
			

		//ARTHA JASA
		case TRANS_TYPE_INQ_PURCHASE:
		case TRANS_TYPE_PURCHASE:
			return TRUE;
			break;
	}
	
	if(glProcInfo.stTranLog.uiEntryMode != MODE_NO_INPUT)
	{
		if ((EMVMiscGetTransTypeChipInitEntryMode() != TRUE)
		        &&	(EMVMiscGetTransTypeMsgInitEntryMode() != TRUE)
		        &&	(EMVMiscGetTransTypeManualInitEntryMode() != TRUE)
		    )
		{
			DispErrMsg("CHIP", "ENTRY BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
		}
		if((EMVMiscGetTransTypeMsgInitEntryMode() != TRUE)
		        &&	(glProcInfo.stTranLog.uiEntryMode == MODE_SWIPE_INPUT)
		   )
		{
			DispErrMsg("MAGSTRIPE","ENTRY BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
		}
		if((EMVMiscGetTransTypeManualInitEntryMode() != TRUE)
		        &&	(glProcInfo.stTranLog.uiEntryMode == MODE_MANUAL_INPUT)
		   )
		{
			DispErrMsg("MANUAL","ENTRY BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
		}
	}

	if( glProcInfo.bIsFallBack==TRUE )
	{
		if ((EMVMiscGetTransTypeFallbackChipInitEntryMode() != TRUE)
		        &&	(EMVMiscGetTransTypeFallbackMsgInitEntryMode() != TRUE)
		        &&	(EMVMiscGetTransTypeFallbackManualInitEntryMode() != TRUE)
		    )
		{
			DispErrMsg("CHIP", "ENTRY BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
		}
		if ((EMVMiscGetTransTypeFallbackMsgInitEntryMode() != TRUE)
		        &&	(glProcInfo.stTranLog.uiEntryMode == MODE_FALLBACK_SWIPE)
		    )
		{
			DispErrMsg("MAGSTRIPE","ENTRY BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
		}
		if((EMVMiscGetTransTypeFallbackManualInitEntryMode() != TRUE)
		        &&	(glProcInfo.stTranLog.uiEntryMode == MODE_FALLBACK_MANUAL)
		   )
		{
			DispErrMsg("MANUAL","ENTRY BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
		}
	}

	if ((glProcInfo.stTranLog.uiEntryMode == MODE_MANUAL_INPUT)
		&& (glCurTlmTmsIss.issManual == FALSE)
	    )
	{
			DispErrMsg("MANUAL","ENTRY BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
	}
	if ((glProcInfo.stTranLog.ucTranType == TRANS_TYPE_VOID)
	        && (glCurTlmTmsIss.issVoid == FALSE)
	    )
	{
			DispErrMsg("VOID","BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
	}
	if ((glProcInfo.stTranLog.ucTranType == TRANS_TYPE_ADJUST)
	        && (glCurTlmTmsIss.issAdjust == FALSE)
	    )
	{
			DispErrMsg("ADJUST","BARRED", TIME_OUT, DERR_BEEP);
			return FALSE;
	}

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_REFUND:
			if(glCurTlmTmsIss.issRefund == FALSE)
			{
				DispErrMsg("TRANSACTION","NOT ALLOWED", TIME_OUT, DERR_BEEP);
				return FALSE;
			}
			break;

		case TRANS_TYPE_OFF_SALE:
			if(glCurTlmTmsIss.issOffline== FALSE)
			{
				DispErrMsg("TRANSACTION","NOT ALLOWED", TIME_OUT, DERR_BEEP);
				return FALSE;
			}
			break;

		case TRANS_TYPE_PREAUTH:
			if(glCurTlmTmsIss.issPreauth == FALSE)
			{
				DispErrMsg("PRE-AUTH","NOT ALLOWED", TIME_OUT, DERR_BEEP);
				return FALSE;
			}
			break;

		case TRANS_TYPE_CASH:
			if(glCurTlmTmsIss.issCashOut == FALSE)
			{
				DispErrMsg("TRANSACTION","NOT ALLOWED", TIME_OUT, DERR_BEEP);
				return FALSE;
			}
			break;

		case TRANS_TYPE_BALANCE:
			if(glCurTlmTmsIss.issBalance == FALSE)
			{
				DispErrMsg("TRANSACTION","NOT ALLOWED", TIME_OUT, DERR_BEEP);
				return FALSE;
			}
			break;
		default:
			break;

	}
	return TRUE;
}
#endif

uchar ChkIfNeedTLE(void)
{
	mapGetByte(appTleSet, TLESetting);
	if(!TLESetting)
		return FALSE;
	
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_TMS_TELIUM:
		case TRANS_TYPE_LTMK:
		case TRANS_TYPE_LTWK:
		case TRANS_TYPE_SETTLEMENT:
		case TRANS_TYPE_ECHO_TEST:
			return FALSE;
			break;
	}
	return TRUE;
}


void ZeroAcqLogonFlag()
{
	word i;
	int ret;

	for(i = 0; i<(word)glTlmSysParam.ucAcqNum; i++)
	{
		mapMove(acqBeg, i);
		MAPPUTBYTE(acqLogonFlag, 0, lblKO);
	}

lblKO:
		;
}


// end of file

