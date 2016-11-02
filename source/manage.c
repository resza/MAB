
#include "global.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
static void ClearReversalSub(void);
static int  ViewTranSub(int iStartRecNo);

/********************** Internal variables declaration *********************/
/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

void UnLockTerminal(void)
{
#ifdef	__TELIUM_PARAM__
	if(glTlmSysParam.Con.appLocOpt & EDC_TLM_TMS_KEYBOARD_LOCKED)
	{
		while(1)
		{
			PubShowTitle(TRUE, (uchar *)_T("TERMINAL  LOCKED"));
			PubDispString(_T("TERMINAL PWD"), 4|DISP_LINE_LEFT);
			PubWaitKey(-1);
			if( PasswordTerm()==0 )
			{
				glTlmSysParam.Con.appLocOpt &= ~(EDC_TLM_TMS_KEYBOARD_LOCKED & 0xFF);
				SaveTlmSysParam();
				PubBeepOk();
				return;
			}
		}
	}
#else
	if( ChkEdcOption(EDC_NOT_KEYBOARD_LOCKED) )
	{
		return;
	}

	while( !ChkEdcOption(EDC_NOT_KEYBOARD_LOCKED) )
	{
		PubShowTitle(TRUE, (uchar *)_T("TERMINAL  LOCKED"));
		PubDispString(_T("MERCHANT PWD"), 4|DISP_LINE_LEFT);
		PubWaitKey(-1);
		if( PasswordMerchant()==0 )
		{
			glSysParam.stEdcInfo.sOption[EDC_NOT_KEYBOARD_LOCKED>>8] |= (EDC_NOT_KEYBOARD_LOCKED & 0xFF);
			SaveEdcParam();
			PubBeepOk();
		}
	}
#endif
}

void LockTerm(void)
{
	PubShowTitle(TRUE, (uchar *)_T("LOCK TERM"));
	PubDispString(_T("TERMINAL LOCK? "), 3|DISP_LINE_LEFT);
	if( AskYesNo() )
	{
#ifdef	__TELIUM_PARAM__
		glTlmSysParam.Con.appLocOpt &= ~(EDC_TLM_TMS_KEYBOARD_LOCKED & 0xFF);
		SaveTlmSysParam();
		glTlmSysParam.stEdcInfo.sOption[EDC_NOT_KEYBOARD_LOCKED>>8] &= ~(EDC_NOT_KEYBOARD_LOCKED & 0xFF);
		SaveTlmEdcParam();
#else
		glSysParam.stEdcInfo.sOption[EDC_NOT_KEYBOARD_LOCKED>>8] &= ~(EDC_NOT_KEYBOARD_LOCKED & 0xFF);
		SaveEdcParam();
#endif
		PubBeepOk();
		UnLockTerminal();
	}
}

void ClearAllRecord(void)
{
	PubShowTitle(TRUE, (uchar *)_T("CLR BATCH"));
	PubDispString(_T("CONFIRM CLEAR"), 3|DISP_LINE_CENTER);
	if( !AskYesNo() )
	{
		return;
	}

	DispProcess();

	ClearRecord(ACQ_ALL);
	ScrClrLine(2, 7);
	DispClearOk();
}

void ClearConfig(void)
{
	PubShowTitle(TRUE, (uchar *)_T("CLR CONFIG"));
	PubDispString(_T("CONFIRM CLEAR"), 3|DISP_LINE_CENTER);
	if( !AskYesNo())
	{
		return;
	}
	DispProcess();

	LoadEdcDefault();

#ifdef ENABLE_EMV
	LoadEmvDefault();
#endif

	ScrClrLine(2, 7);
	DispClearOk();
}

void ClearPassword(void)
{
	PubShowTitle(TRUE, (uchar *)_T("CLR PASSWORD"));
	PubDispString(_T("CONFIRM CLEAR"), 3|DISP_LINE_CENTER);
	if( !AskYesNo())
	{
		return;
	}

	DispProcess();

	ResetPwdAll();
#ifdef	__TELIUM_PARAM__
	SaveTlmPassword();
#else
	SavePassword();
#endif
	ScrClrLine(2, 7);
	DispClearOk();
}

void ClearReversal(void)
{
	PubShowTitle(TRUE, (uchar *)_T("CLR REVERSAL"));
	PubDispString(_T("CONFIRM CLEAR"), 3|DISP_LINE_CENTER);
	if( !AskYesNo())
	{
		return;
	}

	DispProcess();

	ClearReversalSub();
	ScrClrLine(2, 7);
	DispClearOk();
}

void ClearReversalSub(void)
{
	int		iCnt;

#ifdef	__TELIUM_PARAM__
	for(iCnt=0; iCnt<(int)glTlmSysParam.ucAcqNum; iCnt++)
#else
	for(iCnt=0; iCnt<(int)glSysParam.ucAcqNum; iCnt++)
#endif
	{
		glSysCtrl.stRevInfo[iCnt].bNeedReversal = FALSE;
		if( glSysCtrl.sAcqStatus[iCnt]==S_PENDING )	// 为何删除结算标志???
		{
			glSysCtrl.sAcqStatus[iCnt] = S_USE;
		}
	}
	SaveSysCtrlNormal();

}

#if 0
// 删除特定acquirer的交易记录
// Delete transaction records belonging to specific acquirer
void ClearRecord(uchar ucAcqKey)
{
	int		iCnt;

	if( ucAcqKey==ACQ_ALL )
	{
		glSysCtrl.uiLastRecNo = 0xFFFF;
		// 删除交易日志
		// Delete record
		for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
		{
			glSysCtrl.sAcqKeyList[iCnt]    = INV_ACQ_KEY;
			glSysCtrl.sIssuerKeyList[iCnt] = INV_ISSUER_KEY;
		}

		// 恢复收单行状态及清除冲正标志
		// Reset status and reversal flag
		for(iCnt=0; iCnt<(int)glTlmSysParam.ucAcqNum; iCnt++)
		{
			glSysCtrl.sAcqStatus[iCnt]         = S_USE;
			glSysCtrl.stRevInfo[iCnt].bNeedReversal = FALSE;
			glSysCtrl.uiLastRecNoList[iCnt]    = 0xFFFF;
			if( !(glTlmSysParam.Acq[iCnt].ulCurBatchNo>0 &&
				  glTlmSysParam.Acq[iCnt].ulCurBatchNo<=999999L) )
			{
				glTlmSysParam.Acq[iCnt].ulCurBatchNo = 1L;
			}
			glSysCtrl.sAcqUploadStatus[iCnt] = FALSE;
		}
		SaveSysCtrlAll();
	}
	else
	{
		if( glSysCtrl.uiLastRecNo<MAX_TRANLOG &&
			glSysCtrl.sAcqKeyList[glSysCtrl.uiLastRecNo]==ucAcqKey )
		{
			glSysCtrl.uiLastRecNo = 0xFFFF;
		}

		// 删除交易日志
		for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
		{
			if( glSysCtrl.sAcqKeyList[iCnt]==ucAcqKey )
			{
				glSysCtrl.sAcqKeyList[iCnt]    = INV_ACQ_KEY;
				glSysCtrl.sIssuerKeyList[iCnt] = INV_ISSUER_KEY;
			}
		}

		for(iCnt=0; iCnt<(int)glTlmSysParam.ucAcqNum; iCnt++)
		{
			if( glTlmSysParam.Acq[iCnt].acqID==ucAcqKey )
			{
				glSysCtrl.sAcqStatus[iCnt] = S_USE;
				glSysCtrl.stRevInfo[iCnt].bNeedReversal = FALSE;
				glSysCtrl.uiLastRecNoList[iCnt] = 0xFFFF;
				
				// increase batch no
				glTlmSysParam.Acq[iCnt].ulCurBatchNo = GetNewBatchNo(glTlmSysParam.Acq[iCnt].ulCurBatchNo);

				glSysCtrl.sAcqUploadStatus[iCnt] = FALSE;

				break;
			}
		}

		SaveSysCtrlNormal();
	}

	SaveTlmSysParam();
}
#endif

// 删除特定acquirer的交易记录
// Delete transaction records belonging to specific acquirer
void ClearRecord(uchar ucAcqKey)
{
	int		iCnt;

	if( ucAcqKey==ACQ_ALL )
	{
		glSysCtrl.uiLastRecNo = 0xFFFF;
		glSysCtrl.uiBrizziLastRecNo = 0xFFFF;
		glSysCtrl.uiMiniAtmLastRecNo = 0xFFFF;
		glSysCtrl.uiCardServicesLastRecNo = 0xFFFF;
		glSysCtrl.uiTunaiLastRecNo = 0xFFFF;
		glSysCtrl.uiTBankLastRecNo = 0xFFFF;
		// 删除交易日志
		// Delete record
		for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
		{
			glSysCtrl.sAcqKeyList[iCnt]    = INV_ACQ_KEY;
			glSysCtrl.sBrizziAcqKeyList[iCnt]    = INV_ACQ_KEY;
			//glSysCtrl.sTunaiAcqKeyList[iCnt]    = INV_ACQ_KEY;
			glSysCtrl.sIssuerKeyList[iCnt] = INV_ISSUER_KEY;
			glSysCtrl.sBrizziIssuerKeyList[iCnt] = INV_ISSUER_KEY;
			//glSysCtrl.sTunaiIssuerKeyList[iCnt] = INV_ISSUER_KEY;
		}

		for(iCnt=0; iCnt<(MAX_MINIATM_TRANLOG*2); iCnt++)
		{
			glSysCtrl.sMiniAtmCSAcqKeyList[iCnt]    = INV_ACQ_KEY;
			glSysCtrl.sMiniAtmCSIssuerKeyList[iCnt] = INV_ISSUER_KEY;
		}

		for(iCnt=0; iCnt<(MAX_TUNAI_TRANLOG*2); iCnt++)
		{
			glSysCtrl.sTunaiAcqKeyList[iCnt]    = INV_ACQ_KEY;
			glSysCtrl.sTunaiIssuerKeyList[iCnt] = INV_ISSUER_KEY;
		}

		// 恢复收单行状态及清除冲正标志
		// Reset status and reversal flag
		for(iCnt=0; iCnt<(int)glTlmSysParam.ucAcqNum; iCnt++)
		{
			glSysCtrl.sAcqStatus[iCnt]         = S_USE;
			glSysCtrl.stRevInfo[iCnt].bNeedReversal = FALSE;
			glSysCtrl.uiLastRecNoList[iCnt]    = 0xFFFF;
			if( !(glTlmSysParam.Acq[iCnt].ulCurBatchNo>0 &&
				  glTlmSysParam.Acq[iCnt].ulCurBatchNo<=999999L) )
			{
				glTlmSysParam.Acq[iCnt].ulCurBatchNo = 1L;
			}
			glSysCtrl.sAcqUploadStatus[iCnt] = FALSE;
		}
		SaveSysCtrlAll();
	}
	else
	{
		if( glSysCtrl.uiLastRecNo<MAX_TRANLOG &&	glSysCtrl.sAcqKeyList[glSysCtrl.uiLastRecNo]==ucAcqKey )
		{
			glSysCtrl.uiLastRecNo = 0xFFFF;
		}

		// 删除交易日志
		for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
		{
			if( glSysCtrl.sAcqKeyList[iCnt]==ucAcqKey )
			{
				glSysCtrl.sAcqKeyList[iCnt]    = INV_ACQ_KEY;
				glSysCtrl.sIssuerKeyList[iCnt] = INV_ISSUER_KEY;
			}
		}

		for(iCnt=0; iCnt<(int)glTlmSysParam.ucAcqNum; iCnt++)
		{
			if( glTlmSysParam.Acq[iCnt].acqID==ucAcqKey )
			{
				glSysCtrl.sAcqStatus[iCnt] = S_USE;
				glSysCtrl.stRevInfo[iCnt].bNeedReversal = FALSE;
				glSysCtrl.uiLastRecNoList[iCnt] = 0xFFFF;
				
				// increase batch no
				glTlmSysParam.Acq[iCnt].ulCurBatchNo = GetNewBatchNo(glTlmSysParam.Acq[iCnt].ulCurBatchNo);

				glSysCtrl.sAcqUploadStatus[iCnt] = FALSE;

				break;
			}
		}

		SaveSysCtrlNormal();
	}

	SaveTlmSysParam();
}


// Delete Brizzi transaction records
void ClearBrizziRecord(uchar ucAcqKey)
{
	int		iCnt;

	{
		if( glSysCtrl.uiBrizziLastRecNo<MAX_TRANLOG &&
			glSysCtrl.sBrizziAcqKeyList[glSysCtrl.uiBrizziLastRecNo]==ucAcqKey )
		{
			glSysCtrl.uiBrizziLastRecNo = 0xFFFF;
		}

		// 删除交易日志
		for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
		{
			if( glSysCtrl.sBrizziAcqKeyList[iCnt]==ucAcqKey )
			{
				glSysCtrl.sBrizziAcqKeyList[iCnt]    = INV_ACQ_KEY;
				glSysCtrl.sBrizziIssuerKeyList[iCnt] = INV_ISSUER_KEY;
			}
		}

		for(iCnt=0; iCnt<(int)glTlmSysParam.ucAcqNum; iCnt++)
		{
			if( glTlmSysParam.Acq[iCnt].acqID==ucAcqKey )
			{
				glSysCtrl.sAcqStatus[iCnt] = S_USE;
				glSysCtrl.stRevInfo[iCnt].bNeedReversal = FALSE;
				glSysCtrl.uiLastRecNoList[iCnt] = 0xFFFF;
				
				// increase batch no
				glTlmSysParam.Acq[iCnt].ulCurBatchNo = GetNewBatchNo(glTlmSysParam.Acq[iCnt].ulCurBatchNo);

				glSysCtrl.sAcqUploadStatus[iCnt] = FALSE;

				break;
			}
		}

		SaveSysCtrlNormal();
	}

	SaveTlmSysParam();
}



// Delete Mini Atm transaction records
void ClearMiniAtmCSRecord(void)
{
	int		iCnt;

	glSysCtrl.uiMiniAtmLastRecNo = 0xFFFF;
	glSysCtrl.uiCardServicesLastRecNo = 0xFFFF;

	for(iCnt=0; iCnt<(MAX_MINIATM_TRANLOG*2); iCnt++)
	{
		glSysCtrl.sMiniAtmCSAcqKeyList[iCnt]    = INV_ACQ_KEY;
		glSysCtrl.sMiniAtmCSIssuerKeyList[iCnt] = INV_ISSUER_KEY;
	}

	SaveSysCtrlNormal();
}



// Delete Tunai transaction records
void ClearTunaiTBankRecord(void)
{
	int		iCnt;

	glSysCtrl.uiTunaiLastRecNo = 0xFFFF;
	glSysCtrl.uiTBankLastRecNo = 0xFFFF;

	for(iCnt=0; iCnt<MAX_TUNAI_TRANLOG; iCnt++)
	{
		glSysCtrl.sTunaiAcqKeyList[iCnt]    = INV_ACQ_KEY;
		glSysCtrl.sTunaiIssuerKeyList[iCnt] = INV_ISSUER_KEY;
	}

	SaveSysCtrlNormal();
} 

void SuperviserMenu(uchar ucPermission)  
{	
	int ret;

	char *menu[]={
		"CLEAR>",
		"CLEAR BATCH",
		"CLEAR REVERSAL",
		0,
	};
	PubShowTitle(TRUE, (uchar *)_T("CLEAR"));
	if( PasswordBank()!=0 )
	{
		return;
	}	
	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch(ret){
		case 1:	
			ClearAllRecord();
			break;
		case 2:
			ClearReversal();
			break;	
		default:
			break;
	};
	
}


// 清除终端数据界面
// Interface of "Clear". (FUNC99) 
void DoClear(void)
{
	static MenuItem stClearMenu[] =
	{
		{TRUE, "CLR CONFIG",		ClearConfig},
		{TRUE, "CLR BATCH            ",		ClearAllRecord},
		{TRUE, "CLR REVERSAL         ",	ClearReversal},
		{TRUE, "CLR PWD",		ClearPassword},
		{TRUE, "",	NULL},
	};

	PubShowTitle(TRUE, (uchar *)_T("CLEAR"));
	if( PasswordBank()!=0 )
	{
		return;
	}

	PubGetMenu((uchar *)_T("CLEAR"),            stClearMenu, MENU_NOSPEC|MENU_ASCII, USER_OPER_TIMEOUT);
}

// 查看交易汇总
// View total. (glTransTotal)
void ViewTotal(void)
{
	CalcTotal(ACQ_ALL);
	DispTransTotal(TRUE);
}

// 查看所有交易记录
// View all transaction record
void ViewTranList(void)
{
	if( GetTranLogNum(ACQ_ALL)==0 )
	{
		PubShowTitle(TRUE, (uchar *)_T("EDC       REVIEW"));
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	ViewTranSub(-1);	// 浏览所以交易
}

// 查看指定交易记录
// View specific record
void ViewSpecList(void)
{
	int			iRet;
	TRAN_LOG	stLog;

	while (1)
	{
		PubShowTitle(TRUE, (uchar *)"EDC       REVIEW");
		if( GetTranLogNum(ACQ_ALL)==0 )
		{
			PubShowTitle(TRUE, (uchar *)"EDC       REVIEW");
			PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(5);
			return;
		}

		iRet = GetRecord(TS_ALL_LOG, &stLog);
		if( iRet!=0 )
		{
			return;
		}

		if (ViewTranSub((int)glProcInfo.uiRecNo)!=0)
		{
			break;
		}
	}
}

// 交易记录浏览控制
// 返回：ERR_USERCANCEL--取消或超时退出；0--其它按键（或原因）退出
int ViewTranSub(int iStartRecNo)
{
	uchar		ucKey;
	int			iRecNo, iStep, iCnt, iActRecNo;
	TRAN_LOG	stLog;

	iRecNo = iStartRecNo;
	iStep  = iStartRecNo<0 ? 1 : 0;
	while( 1 )
	{
		iRecNo = iRecNo + iStep;
		if( iRecNo>=MAX_TRANLOG )
		{
			ScrCls();
			PubDispString(_T("END OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = 0;
		}
		else if( iRecNo<0 )
		{
			ScrCls();
			PubDispString(_T("START OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = MAX_TRANLOG-1;
		}
		if( glSysCtrl.sAcqKeyList[iRecNo]==INV_ACQ_KEY )
		{
			continue;
		}
		memset(&stLog, 0, sizeof(TRAN_LOG));
		LoadTranLog(&stLog, (ushort)iRecNo);
		for(iActRecNo=iCnt=0; iCnt<=iRecNo; iCnt++)
		{
			if( glSysCtrl.sAcqKeyList[iCnt]!=INV_ACQ_KEY )
			{
				iActRecNo++;
			}
		}
		DispTranLog((ushort)iActRecNo, &stLog);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYDOWN || ucKey==KEYENTER )
		{
			iStep = 1;
		}
		else if( ucKey==KEYUP || ucKey==KEYCLEAR )
		{
			iStep = -1;
		}
		else
		{
			iStep = 0;
		}

		if (iStartRecNo>=0)		// 查阅指定记录。不上下翻页
		{
			return 0;
		}
	}
}

int ViewBrizziTranSub(int iStartRecNo)
{
	uchar		ucKey;
	int			iRecNo, iStep, iCnt, iActRecNo;
	TRAN_LOG	stLog;

	iRecNo = iStartRecNo;
	iStep  = iStartRecNo<0 ? 1 : 0;
	while( 1 )
	{
		iRecNo = iRecNo + iStep;
		if( iRecNo>=MAX_TRANLOG )
		{
			ScrCls();
			PubDispString(_T("END OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = 0;
		}
		else if( iRecNo<0 )
		{
			ScrCls();
			PubDispString(_T("START OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = MAX_TRANLOG-1;
		}
		if( glSysCtrl.sBrizziAcqKeyList[iRecNo]==INV_ACQ_KEY )
		{
			continue;
		}
		memset(&stLog, 0, sizeof(TRAN_LOG));
		LoadBrizziTranLog(&stLog, (ushort)iRecNo);
		for(iActRecNo=iCnt=0; iCnt<=iRecNo; iCnt++)
		{
			if( glSysCtrl.sBrizziAcqKeyList[iCnt]!=INV_ACQ_KEY )
			{
				iActRecNo++;
			}
		}
		DispTranLog((ushort)iActRecNo, &stLog);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYDOWN || ucKey==KEYENTER )
		{
			iStep = 1;
		}
		else if( ucKey==KEYUP || ucKey==KEYCLEAR )
		{
			iStep = -1;
		}
		else
		{
			iStep = 0;
		}

		if (iStartRecNo>=0)		// 查阅指定记录。不上下翻页
		{
			return 0;
		}
	}
}

int ViewMiniAtmTranSub(int iStartRecNo)
{
	uchar		ucKey;
	int			iRecNo, iStep, iCnt, iActRecNo;
	TRAN_LOG	stLog;

	iRecNo = iStartRecNo;
	iStep  = iStartRecNo<0 ? 1 : 0;
	while( 1 )
	{
		iRecNo = iRecNo + iStep;
		if( iRecNo>=MAX_MINIATM_TRANLOG )
		{
			ScrCls();
			PubDispString(_T("END OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = 0;
		}
		else if( iRecNo<0 )
		{
			ScrCls();
			PubDispString(_T("START OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = MAX_MINIATM_TRANLOG-1;
		}
		if( glSysCtrl.sMiniAtmCSAcqKeyList[iRecNo]==INV_ACQ_KEY )
		{
			continue;
		}
		memset(&stLog, 0, sizeof(TRAN_LOG));
		LoadMiniAtmCSTranLog(&stLog, (ushort)iRecNo);
		for(iActRecNo=iCnt=0; iCnt<=iRecNo; iCnt++)
		{
			if( glSysCtrl.sMiniAtmCSAcqKeyList[iCnt]!=INV_ACQ_KEY )
			{
				iActRecNo++;
			}
		}
		DispTranLog((ushort)iActRecNo, &stLog);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYDOWN || ucKey==KEYENTER )
		{
			iStep = 1;
		}
		else if( ucKey==KEYUP || ucKey==KEYCLEAR )
		{
			iStep = -1;
		}
		else
		{
			iStep = 0;
		}

		if (iStartRecNo>=0)		// 查阅指定记录。不上下翻页
		{
			return 0;
		}
	}
}

int ViewCardServicesTranSub(int iStartRecNo)
{
	uchar		ucKey;
	int			iRecNo, iStep, iCnt, iActRecNo;
	TRAN_LOG	stLog;

	iRecNo = iStartRecNo;
	iStep  = iStartRecNo<0 ? 1 : 0;
	while( 1 )
	{
		iRecNo = iRecNo + iStep;
		if( iRecNo>=MAX_MINIATM_TRANLOG )
		{
			ScrCls();
			PubDispString(_T("END OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = 0;
		}
		else if( iRecNo<0 )
		{
			ScrCls();
			PubDispString(_T("START OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = MAX_MINIATM_TRANLOG-1;
		}
		if( glSysCtrl.sMiniAtmCSAcqKeyList[iRecNo+MAX_MINIATM_TRANLOG]==INV_ACQ_KEY )
		{
			continue;
		}
		memset(&stLog, 0, sizeof(TRAN_LOG));
		LoadMiniAtmCSTranLog(&stLog, (ushort)(iRecNo+MAX_MINIATM_TRANLOG));
		for(iActRecNo=iCnt=0; iCnt<=iRecNo; iCnt++)
		{
			if( glSysCtrl.sMiniAtmCSAcqKeyList[iCnt+MAX_MINIATM_TRANLOG]!=INV_ACQ_KEY )
			{
				iActRecNo++;
			}
		}
		DispTranLog((ushort)iActRecNo, &stLog);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYDOWN || ucKey==KEYENTER )
		{
			iStep = 1;
		}
		else if( ucKey==KEYUP || ucKey==KEYCLEAR )
		{
			iStep = -1;
		}
		else
		{
			iStep = 0;
		}

		if (iStartRecNo>=0)		// 查阅指定记录。不上下翻页
		{
			return 0;
		}
	}
}

int ViewTunaiTranSub(int iStartRecNo)
{
	uchar		ucKey;
	int			iRecNo, iStep, iCnt, iActRecNo;
	TRAN_LOG	stLog;

	iRecNo = iStartRecNo;
	iStep  = iStartRecNo<0 ? 1 : 0;
	while( 1 )
	{
		iRecNo = iRecNo + iStep;
		if( iRecNo>=MAX_TUNAI_TRANLOG )
		{
			ScrCls();
			PubDispString(_T("END OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = 0;
		}
		else if( iRecNo<0 )
		{
			ScrCls();
			PubDispString(_T("START OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = MAX_TUNAI_TRANLOG-1;
		}
		if( glSysCtrl.sTunaiAcqKeyList[iRecNo]==INV_ACQ_KEY )
		{
			continue;
		}
		memset(&stLog, 0, sizeof(TRAN_LOG));
		LoadTunaiTBankTranLog(&stLog, (ushort)iRecNo);
		for(iActRecNo=iCnt=0; iCnt<=iRecNo; iCnt++)
		{
			if( glSysCtrl.sTunaiAcqKeyList[iCnt]!=INV_ACQ_KEY )
			{
				iActRecNo++;
			}
		}
		DispTranLog((ushort)iActRecNo, &stLog);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYDOWN || ucKey==KEYENTER )
		{
			iStep = 1;
		}
		else if( ucKey==KEYUP || ucKey==KEYCLEAR )
		{
			iStep = -1;
		}
		else
		{
			iStep = 0;
		}

		if (iStartRecNo>=0)		// 查阅指定记录。不上下翻页
		{
			return 0;
		}
	}
}

int ViewTBankTranSub(int iStartRecNo)
{
	uchar		ucKey;
	int			iRecNo, iStep, iCnt, iActRecNo;
	TRAN_LOG	stLog;

	iRecNo = iStartRecNo;
	iStep  = iStartRecNo<0 ? 1 : 0;
	while( 1 )
	{
		iRecNo = iRecNo + iStep;
		if( iRecNo>=MAX_TUNAI_TRANLOG )
		{
			ScrCls();
			PubDispString(_T("END OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = 0;
		}
		else if( iRecNo<0 )
		{
			ScrCls();
			PubDispString(_T("START OF BATCH"), 4|DISP_LINE_LEFT);
			DelayMs(1000);
			iRecNo = MAX_TUNAI_TRANLOG-1;
		}
		if( glSysCtrl.sTunaiAcqKeyList[iRecNo+MAX_TUNAI_TRANLOG]==INV_ACQ_KEY )
		{
			continue;
		}
		memset(&stLog, 0, sizeof(TRAN_LOG));
		LoadTunaiTBankTranLog(&stLog, (ushort)(iRecNo+MAX_TUNAI_TRANLOG));
		for(iActRecNo=iCnt=0; iCnt<=iRecNo; iCnt++)
		{
			if( glSysCtrl.sTunaiAcqKeyList[iCnt+MAX_TUNAI_TRANLOG]!=INV_ACQ_KEY )
			{
				iActRecNo++;
			}
		}
		DispTranLog((ushort)iActRecNo, &stLog);

		ucKey = PubWaitKey(USER_OPER_TIMEOUT);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			return ERR_USERCANCEL;
		}
		else if( ucKey==KEYDOWN || ucKey==KEYENTER )
		{
			iStep = 1;
		}
		else if( ucKey==KEYUP || ucKey==KEYCLEAR )
		{
			iStep = -1;
		}
		else
		{
			iStep = 0;
		}

		if (iStartRecNo>=0)		// 查阅指定记录。不上下翻页
		{
			return 0;
		}
	}
}

void PrnLastTrans(void)
{
	int			iRet;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( glSysCtrl.uiLastRecNo>=MAX_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = LoadTranLog(&glProcInfo.stTranLog, glSysCtrl.uiLastRecNo);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);
	PrintReceipt(PRN_REPRINT);
}

void RePrnSpecTrans(void)
{
	int			iRet;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( GetTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);
	PrintReceipt(PRN_REPRINT);
}

void PrnTotal(void)
{
	int		iRet;
	uchar	szTitle[16+1], ucIndex;

	PubShowTitle(TRUE, (uchar *)_T("PRINT TOTAL     "));
	sprintf((char *)szTitle, _T("PRINT TOTAL     "));
	iRet = SelectAcq(FALSE, szTitle, &ucIndex);
	if( iRet!=0 )
	{
		return;
	}
#ifdef	__TELIUM_PARAM__
	CalcTotal(glCurTlmTmsAcq.acqID);
#else
	CalcTotal(glCurAcq.ucKey);
#endif

	PubShowTitle(TRUE, (uchar *)_T("PRINT TOTAL     "));
	if (ChkIfZeroTotal(&glTransTotal))
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
		return ;
	}

	PrnTotalAcq();
}


void RePrnSettleNew(void)
{
//	int		iRet;
//	uchar	szTitle[16+1];
	uchar	ucIndex;

	ScrCls();
	PubDispString("REPRINT SETTLE", DISP_LINE_CENTER|DISP_LINE_REVER|0);

	for(ucIndex=0;ucIndex<glTlmSysParam.ucAcqNum;ucIndex++)
	{
		SetCurAcq(ucIndex);
		if( glSysCtrl.stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex] )
			break;
	}

	if(ucIndex>=glTlmSysParam.ucAcqNum)
	{
			
			PubDispString(_T("NO RECEIPT"), 4|DISP_LINE_LEFT);
			PubWaitKey(3);
			return;
	}

	DispPrinting();
	PrintSettleNew(PRN_REPRINT);
}


void RePrnSettle(void)
{
	int		iRet;
	uchar	szTitle[16+1];

	PubShowTitle(TRUE, (uchar *)_T("REPRINT SETTLE  "));
	sprintf((char *)szTitle, _T("REPRINT SETTLE  "));
	iRet = SelectAcq(FALSE, szTitle, NULL);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT SETTLE  "));

#ifdef	__TELIUM_PARAM__
	if( !glSysCtrl.stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex] )
	{
		PubDispString(_T("NO RECEIPT"), 4|DISP_LINE_LEFT);
		PubWaitKey(3);
		return;
	}

	sprintf((char *)glProcInfo.szSettleMsg, "%s", glSysCtrl.stRePrnStlInfo.szSettleMsg[glCurTlmTmsAcq.ucIndex]);
	memcpy(&glTransTotal, &glSysCtrl.stRePrnStlInfo.stAcqTotal[glCurTlmTmsAcq.ucIndex], sizeof(TOTAL_INFO));
//	memcpy(glIssuerTotal, glSysCtrl.stRePrnStlInfo.stIssTotal[glCurTlmTmsAcq.ucIndex], sizeof(glIssuerTotal));
	glCurTlmTmsAcq.ulCurBatchNo = glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex];
	sprintf((char *)glCurTlmTmsAcq.acqCurBat, "%06lu", glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex]);
#else
	if( !glSysCtrl.stRePrnStlInfo.bValid[glCurAcq.ucIndex] )
	{
		PubDispString(_T("NO RECEIPT"), 4|DISP_LINE_LEFT);
		PubWaitKey(3);
		return;
	}

	sprintf((char *)glProcInfo.szSettleMsg, "%s", glSysCtrl.stRePrnStlInfo.szSettleMsg[glCurAcq.ucIndex]);
	memcpy(&glTransTotal, &glSysCtrl.stRePrnStlInfo.stAcqTotal[glCurAcq.ucIndex], sizeof(TOTAL_INFO));
//	memcpy(glIssuerTotal, glSysCtrl.stRePrnStlInfo.stIssTotal[glCurAcq.ucIndex], sizeof(glIssuerTotal));
	glCurAcq.ulCurBatchNo = glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurAcq.ucIndex];

#endif
	DispPrinting();
	PrintSettle(PRN_REPRINT);
}

#ifdef ENABLE_EMV
// Print EMV error log message
void PrintEmvErrLog(void)
{
	ushort			uiCnt, uiActNum, uiTemp;
	uchar			szBuff[50];
	EMV_ERR_LOG		stErrLog;

	PubShowTitle(TRUE, (uchar *)_T("PRINT ERROR LOG"));
	if( PasswordBank()!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("PRINT ERROR LOG"));
	DispProcess();

	PrnInit();
	PrnSetNormal();
	PrnStr("  EMV ERROR LOG\n\n");

	for(uiActNum=uiCnt=0; uiCnt<MAX_ERR_LOG; uiCnt++)
	{
		memset(&stErrLog, 0, sizeof(EMV_ERR_LOG));
		LoadErrLog(uiCnt, &stErrLog);
		if( !stErrLog.bValid )
		{
			continue;
		}

		uiActNum++;
		PrnStr("\nSTAN: %06lu\n", stErrLog.ulSTAN);
		PubBcd2Asc0(stErrLog.sAID, stErrLog.ucAidLen, szBuff);
		PrnStr("AID: %s\n", szBuff);
		PrnStr("PAN: %s\n", stErrLog.szPAN);
		PrnStr("PAN SEQ #: %02X\n", stErrLog.ucPANSeqNo);
		PrnStr("AMT: %.12s\n", stErrLog.szAmount);
		PrnStr("TIP: %.12s\n", stErrLog.szTipAmt);
		PrnStr("RSP: %.2s\n",  stErrLog.szRspCode);
		PrnStr("RRN: %.12s\n", stErrLog.szRRN);
		PrnStr("AUT: %.6s\n",  stErrLog.szAuthCode);
		PrnStr("TVR: %02X %02X %02X %02X %02X\n", stErrLog.sTVR[0], stErrLog.sTVR[1],
				stErrLog.sTVR[2], stErrLog.sTVR[3], stErrLog.sTVR[4]);
		PrnStr("TSI: %02X %02X\n", stErrLog.sTSI[0], stErrLog.sTSI[1]);

		PrnStr("REQ BIT 55:\n");
		for(uiTemp=0; uiTemp<stErrLog.uiReqICCDataLen; uiTemp++)
		{
			PrnStr("%02X %s", stErrLog.sReqICCData[uiTemp], (uiTemp%10)==9 ? "\n" : "");
		}
		PrnStr("%s", uiTemp>0 ? "\n" : "");

		PrnStr("RSP BIT 55:\n");
		for(uiTemp=0; uiTemp<stErrLog.uiRspICCDataLen; uiTemp++)
		{
			PrnStr("%02X ", stErrLog.sRspICCData[uiTemp]);
		}
		PrnStr("%s", uiTemp>0 ? "\n" : "");

		if( (uiActNum%5)==4 )
		{
			if( StartPrinter()!=0 )
			{
				return;
			}

			PrnInit();
			PrnSetNormal();
		}
	}

	if (uiActNum>0)
	{
		PrnStr("%s", (ChkIfThermalPrinter() ? "\n\n\n\n\n\n" : "\f"));
	}
	else
	{
		PrnStr("\n  ( NO RECORD )\n\n\n\n\n\n");
	}

	StartPrinter();
}
#endif

// end of file

