
#include "global.h"
#include "newPack.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
static int  AskModemReDial(int iRet);
static int  GenSendPacket(void);
static int  ProcRecvPacket(void);
static void SaveRecvPackData(void);
static int  AdjustCommParam(void);
static void DispCommErrMsg(int iErrCode);

/********************** Internal variables declaration *********************/
/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// 交易处理
// process transaction
int TranProcess(void)
{
	int		iRet, iRetryPIN;

#ifdef ENABLE_EMV
	// EMV卡交易处理
	if(PrivateLabelCardDetect)
		{
////////////
		}
		else {
	if( (glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) &&
		(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SALE || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_CASH ||
		 glProcInfo.stTranLog.ucTranType==TRANS_TYPE_AUTH || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PREAUTH ||
		 glProcInfo.stTranLog.ucTranType==TRANS_TYPE_REFUND || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INSTALMENT) )
	{
		return FinishEmvTran();
	}
			}
#endif
//#ifdef BANK_MANDIRI
#ifdef BANK_ARTHA_JASA
	iRet = TransLogonAJ();
	if(iRet != 0)
	{
		return iRet;
	}
#endif

	// 判断上次交易是否需要进行冲正等
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	// Some banks may want TC to be sent prior to sale

	iRetryPIN = 0;
	while( 1 )
	{
		iRet = SendRecvPacket();
		if( iRet!=0 )
		{
			return iRet;
		}

#if 0		//not use
		if( memcmp(glRecvPack.szBit39, "55", 2)!=0 || ++iRetryPIN>3 || !ChkIfNeedPIN() )
		{
			break;
		}
		// 重新输入PIN
		iRet = GetPIN(GETPIN_RETRY);
		if( iRet!=0 )
		{
			return iRet;
		}
		sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);
		memcpy(&glSendPack.szBit52[0], "\x00\x08", 2);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, 8);
#else
		break;		
#endif
	}

	
	return AfterTranProc();
}

// 交换通讯报文
// exchange package(send request & wait response)
int SendRecvPacket(void)
{
	int		iRet;
	

	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	// 保存结算状态/冲正数据
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SETTLEMENT )
	{
#ifdef	__TELIUM_PARAM__
		glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex] = S_PENDING;
#else
		glSysCtrl.sAcqStatus[glCurAcq.ucIndex] = S_PENDING;
#endif
		SaveSysCtrlBase();	//save reg
	}
//	SaveRevInfo(TRUE);

	iRet = RecvPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	return 0;
}

// 生成发送报文
// generate request package
int GenSendPacket(void)
{
	int		iRet;
	uint	uiLength;
	uchar	szBuff[20], sMacOut[LEN_MAC];

	// prepare TPDU header
	memset(&glSendData, 0, sizeof(COMM_DATA));
#ifdef	__TELIUM_PARAM__
	if(glCommCfg.ucCommType==CT_MODEM)//4212
	sprintf((char *)szBuff, "600%3.3s3000", glCurTlmTmsAcq.acqNII);
	else
	sprintf((char *)szBuff, "600%3.3s0000", glCurTlmTmsAcq.acqNII);
#else
	sprintf((char *)szBuff, "600%3.3s0000", glCurAcq.szNii);
#endif
	PubAsc2Bcd(szBuff, 10, glSendData.sContent);

	// generate iso8583 data
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
		iRet = PubPack8583(glTMSDataDef, &glTMSSend, &glSendData.sContent[5], &uiLength);
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM )
	{
		iRet = PubPack8583(glTMSTELIUMDataDef, &glTMSTELIUMSend, &glSendData.sContent[5], &uiLength);
	}
	else
	{
		iRet = PubPack8583(glEdcDataDef, &glSendPack, &glSendData.sContent[5], &uiLength);
	}
	if( iRet<0 )
	{
		ScrCls();
		ScrPrint(0, 3, ASCII, "%*s", glSendData.sContent[5], uiLength);

		Get8583ErrMsg(TRUE, iRet, szBuff);
		PubDispString(szBuff, 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}
	glSendData.uiLength = (ushort)(uiLength + 5);

	// If don't need MAC
	if( ChkIfNeedMac() )
	{
		// fill mac data
		memset(sMacOut, 0, sizeof(sMacOut));
		// !!!! the MAC key ID (1) should be per acquirer specified.
		// !!!! the algorithm should be per acquirer specified.
//		getMAC(&glSendData.sContent[5], (ushort)(uiLength-LEN_MAC), sMacOut); // closed cung
/*		
		iRet = GetMAC(MAC_ANSIX99, &glSendData.sContent[5], (ushort)(uiLength-LEN_MAC), 1, sMacOut);
		if( iRet!=0 )
		{
			return iRet;
		}
*/
		memcpy(glSendPack.sMac+2, sMacOut, LEN_MAC);
		memcpy(&glSendData.sContent[glSendData.uiLength-LEN_MAC], sMacOut, LEN_MAC);
	}

	
#if 0
#ifdef BANK_MANDIRI
	if(ChkIfNeedTLE())
	{

		if(debugTx)
			hex_dump_char("TX (Original):", glSendData.sContent, glSendData.uiLength);

		if(debugIsoReq)
		{
			pckIsoDump8583(&glSendData.sContent[5]);
		}

//		hex_dump_char("ISO8583 ORI", glSendData.sContent, glSendData.uiLength);
		getTLEData(&glSendData.sContent[5]);
		iRet = PubPack8583(glEdcDataDef, &glSendPack, &glSendData.sContent[5], &uiLength);
		if( iRet<0 )
		{
			ScrCls();
			ScrPrint(0, 3, ASCII, "%*s", glSendData.sContent[5], uiLength);

			Get8583ErrMsg(TRUE, iRet, szBuff);
			PubDispString(szBuff, 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(5);
			return ERR_NO_DISP;
		}
		glSendData.uiLength = (ushort)(uiLength + 5);

//		hex_dump_char("ISO8583 TLE", glSendData.sContent, glSendData.uiLength);
	}
#endif	
#endif
	return 0;
}

// 预拨号
// pre-connect to host
int PreDial(void)
{
	int		iRet;

	if( !glTlmSysParam.stEdcInfo.bPreDial )
	{
		return 0;
	}

#if defined(_P60_S1_) || defined(_S_SERIES_)
	if( ChkOnBase()!=0 )
	{
		return 0;
	}
#endif

	iRet = AdjustCommParam();
	if( iRet!=0 )
	{
		return iRet;
	}

	return CommDial(DM_PREDIAL);
}

// 连接主机
// connect to host
int ConnectHost(void)
{
	int		iRet;

	while( 1 )
	{
		// 设置通信参数（从ACQ取得IP，电话号码）
		iRet = AdjustCommParam();
		if (iRet!=0)
		{
			if ((glCommCfg.ucCommTypeBak!=CT_NONE) && (glCommCfg.ucCommType!=glCommCfg.ucCommTypeBak))		// switch to next connection
			{
				// 如果第一套通信的参数不存在，则切换到备用通信方式
				glCommCfg.ucCommType = glTlmSysParam.stTxnCommCfg.ucCommTypeBak;
				CommSwitchType(glTlmSysParam.stTxnCommCfg.ucCommTypeBak);
				continue;
			}

			if( iRet!=ERR_NO_TELNO )
			{
				DispCommErrMsg(iRet);
				return ERR_NO_DISP;
			}
			return iRet;
		}

#if defined(_S_SERIES_) || defined(_P60_S1_)
        SetOffBase(OffBaseDisplay);
#endif

		// 连接
		kbflush();
		switch(glCommCfg.ucCommType)
		{
			case CT_RS232:
			case CT_TCPIP:
			case	CT_CDMA	:
			case CT_GPRS:
			case CT_WIFI:
				DispConnecting();
				break;
			case CT_MODEM:
			default:
				DispDial();
		}

		iRet = CommDial(DM_DIAL);
		if (iRet==0)
		{
			return 0;
		}

#ifdef SXX_WIRELESS_MODULE
		if(iRet == WL_RET_ERR_NOSIM)
		{
			DispCommErrMsg(iRet);
			return ERR_NO_DISP;
		}
#endif

		// 是否按过取消
		if ((kbhit()==0) && (getkey()==KEYCANCEL))
		{
			return ERR_USERCANCEL;
		}

		// 如果第一套通信方式连接失败，则切换到备用通信方式
		if ((glCommCfg.ucCommTypeBak!=CT_NONE) && (glCommCfg.ucCommType!=glCommCfg.ucCommTypeBak))
		{
			glCommCfg.ucCommType = glCommCfg.ucCommTypeBak;
			CommSwitchType(glCommCfg.ucCommTypeBak);
			continue;
		}

		// 非Modem 错误，直接返回
		if( (iRet & MASK_COMM_TYPE)!=ERR_COMM_MODEM_BASE )
		{
			DispCommErrMsg(iRet);
			return ERR_NO_DISP;
		}

		// Modem 错误，询问是否重拨
		if (AskModemReDial(iRet))
		{
			return ERR_USERCANCEL;
		}
	}

	return 0;
}

int AskModemReDial(int iRet)
{
	uchar	ucKey;

	if( iRet==ERR_COMM_MODEM_OCCUPIED || iRet==ERR_COMM_MODEM_NO_LINE ||
		iRet==ERR_COMM_MODEM_NO_LINE_2 )
	{
		DispCommErrMsg(iRet);
	}
	else if( iRet==ERR_COMM_MODEM_LINE || iRet==ERR_COMM_MODEM_NO_ACK ||
			 iRet==ERR_COMM_MODEM_LINE_BUSY )
	{
		DispResult(iRet);
		ScrClrLine(2, 7);
		PubDispString(_T("RE DIAL ?    "), 4|DISP_LINE_LEFT);
	}
	else
	{
		PubBeepErr();
		ScrClrLine(2, 7);
		PubDispString(_T("DIAL FAIL,RETRY?"), 4|DISP_LINE_LEFT);
		ScrPrint(114, 7, ASCII, "%02X", (uchar)(iRet & MASK_ERR_CODE));
	}
	ucKey = PubWaitKey(USER_OPER_TIMEOUT/2);
	if( ucKey==KEYCANCEL || ucKey==NOKEY )
	{
		return ERR_USERCANCEL;
	}

	return 0;
}

// 发送报文
// send request package
int SendPacket(void)
{
	int		iRet;
#ifdef BANK_MANDIRI
	uchar	szBuff[20];
	uint		uiLength;
#endif
	if(glProcInfo.stTranLog.ucTranType == TRANS_TYPE_OFF_SALE){
		return ERR_NO_DISP;
	} else
	iRet = ConnectHost();
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GenSendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	//save reversal data
	SaveRevInfo(TRUE);


	DispSend();
	if( glCommCfg.ucCommType==CT_TCPIP ||
		glCommCfg.ucCommType==CT_GPRS  ||
		glCommCfg.ucCommType==CT_CDMA  )
	{
		memmove(&glSendData.sContent[2], glSendData.sContent, glSendData.uiLength);
		if (glCommCfg.ucTCPClass_BCDHeader)
		{// BCD 格式的长度字节
			PubLong2Bcd((ulong)glSendData.uiLength,  2, glSendData.sContent);
		} 
		else
		{
			PubLong2Char((ulong)glSendData.uiLength, 2, glSendData.sContent);
		}
		glSendData.uiLength += 2;
	}


	if(debugTx)
		hex_dump_char("TX:", glSendData.sContent, glSendData.uiLength);

	if(debugIsoReq)
	{
		if( glCommCfg.ucCommType==CT_TCPIP ||
			glCommCfg.ucCommType==CT_GPRS  ||
			glCommCfg.ucCommType==CT_CDMA ||
			glCommCfg.ucCommType==CT_WIFI)
			pckIsoDump8583(&glSendData.sContent[7]);
		else
			pckIsoDump8583(&glSendData.sContent[5]);
	}
	
	if(glSysCtrl.ucFlagBlowfish)
	{
		char bfKey[16+1];
		char tmp[32+1];
		word len;
		memset(bfKey,0,8);

		mapGet(appBfKey, bfKey,16);

		len = strlen(bfKey);
		    strcpy(tmp, bfKey);
		    if(len % 2 != 0)
		        tmp[len] = 'F';

			memset(BlowKey,0,8);
			hex2bin(BlowKey, tmp, 0);

			glSendData.uiLength= Blowfish_EncryptMsg(BlowKey,glSendData.sContent, glSendData.uiLength, 2);
	}

	memset(glProcInfo.stTranLog.szRspCode, 0, sizeof(glProcInfo.stTranLog.szRspCode));
	
	iRet = CommTxd(glSendData.sContent, glSendData.uiLength, USER_OPER_TIMEOUT);	// 不能用 no timeout
	if( iRet!=0 )
	{
		DispCommErrMsg(iRet);	// !!!! 转移到CommTxd
		return ERR_NO_DISP;
	}

	GetNewTraceNo();

	if( glCommCfg.ucCommType!=CT_RS232 )
		DelayMs(500);

	return 0;
}

// 接收报文
int RecvPacket(void)
{
	int		iRet;
	ushort	uiTimeOut;
	ulong	ulTemp;

	if(glProcInfo.stTranLog.ucTranType == TRANS_TYPE_OFF_SALE){
		memcpy(glProcInfo.stTranLog.szRspCode, "00", 2);
		SaveRevInfo(FALSE);
		return ERR_NO_DISP;
	}
	DispReceive();
#ifdef	__TELIUM_PARAM__
	if(glProcInfo.stTranLog.ucTranType == TRANS_TYPE_SETTLEMENT)
		uiTimeOut = glCurTlmTmsAcq.acqStlRxTO;	// !!!! should according to comm type	
	else
		uiTimeOut = glCurTlmTmsAcq.acqTxnRxTO;	// !!!! should according to comm type
#else
	uiTimeOut = glCurAcq.ucPhoneTimeOut;	// !!!! should according to comm type
#endif

	
	memset(&glRecvData, 0, sizeof(COMM_DATA));	

	iRet = CommRxd(glRecvData.sContent, LEN_MAX_COMM_DATA, uiTimeOut, &glRecvData.uiLength);
	if( iRet!=0 )
	{
		DispCommErrMsg(iRet);	//!!!! 转移到CommRxd
		return ERR_COMM_ERROR; //ERR_NO_DISP;
	}


	if(glSysCtrl.ucFlagBlowfish)
		glRecvData.uiLength=Blowfish_DecryptMsg(BlowKey, glRecvData.sContent, glRecvData.uiLength,2);

	if(debugRx)
		hex_dump_char("RX (ORIGINAL) :", glRecvData.sContent, glRecvData.uiLength);

	if(debugIsoRes)
	{
		if( glCommCfg.ucCommType==CT_TCPIP ||
			glCommCfg.ucCommType==CT_GPRS  ||
			glCommCfg.ucCommType==CT_CDMA ||
			glCommCfg.ucCommType==CT_WIFI)
			pckIsoDump8583(&glRecvData.sContent[7]);
		else
				pckIsoDump8583(&glRecvData.sContent[5]);
	}

	if( glCommCfg.ucCommType==CT_TCPIP ||
		glCommCfg.ucCommType==CT_GPRS  ||
		glCommCfg.ucCommType==CT_CDMA  )
	{
		if (glCommCfg.ucTCPClass_BCDHeader)
		{// BCD 格式的长度字节
			PubBcd2Long(glRecvData.sContent,  2, &ulTemp);
		}
		else
		{
			PubChar2Long(glRecvData.sContent, 2, &ulTemp);
		}
		
		if( ulTemp+2!=(ulong)glRecvData.uiLength )
		{
			ScrClrLine(2, 7);
			PubDispString(_T("RECV DATA ERR"), 4|DISP_LINE_LEFT);
			PubBeepErr();
			PubWaitKey(2);
			return ERR_NO_DISP;
		}
		memmove(glRecvData.sContent, &glRecvData.sContent[2], (uint)ulTemp);
		glRecvData.uiLength = (ushort)ulTemp;
	}

	if (glCommCfg.ucCommType==CT_DEMO)
	{
		iRet = CreatDummyRecvData(&glProcInfo, &glRecvPack);
		return iRet;
	}
/*// 4212
	iRet = ProcRecvPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	return 0;
*/
	return ProcRecvPacket();
}

// 处理接收包，并检查公共数据要素
// Process receive packet content
int ProcRecvPacket(void)
{
	int		iRet;
	long	lTemp1, lTemp2;
	uchar	bFailFlag, szBuff[20];

	DispProcess();
	// 检查接收到的数据长度(TPDU+MTI+Bitmap+stan+tid)
	// Check received length
	if( glRecvData.uiLength<5+2+8+8+3 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("RECV DATA ERR"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}
	bFailFlag = FALSE;

	// 处理TPDU(5)
	// Processing TPDU
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
		memset(&glTMSRecv, 0, sizeof(STTMS8583));
		iRet = PubUnPack8583(glTMSDataDef, &glRecvData.sContent[5], glRecvData.uiLength-5, &glTMSRecv);
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
	{
		memset(&glTMSTELIUMRecv, 0, sizeof(STTMS8583));
		iRet = PubUnPack8583(glTMSTELIUMDataDef, &glRecvData.sContent[5], glRecvData.uiLength-5, &glTMSTELIUMRecv);
	}
	else
	{
		memset(&glRecvPack, 0, sizeof(STISO8583));
		iRet = PubUnPack8583(glEdcDataDef, &glRecvData.sContent[5], glRecvData.uiLength-5, &glRecvPack);
	}
	if( iRet<0 )
	{
		Get8583ErrMsg(FALSE, iRet, szBuff);
		ScrClrLine(2, 7);
		PubDispString(szBuff, 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}

	// 检查响应数据包的要素并与请求包匹配
	// Check message type
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
		lTemp1 = atol((char *)glTMSSend.szMsgCode);
		lTemp2 = atol((char *)glTMSRecv.szMsgCode);
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
	{
		lTemp1 = atol((char *)glTMSTELIUMSend.szMsgCode);
		lTemp2 = atol((char *)glTMSTELIUMRecv.szMsgCode);
	}
	else
	{
		lTemp1 = atol((char *)glSendPack.szMsgCode);
		lTemp2 = atol((char *)glRecvPack.szMsgCode);
	}
	if( lTemp2!=(lTemp1+10) )	// 检查消息码
	{
		ScrClrLine(2, 7);
		PubDispString(_T("MSG ID MISMATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}

    // 保存请求/响应数据
    // Save received response packet.
    SaveRecvPackData();
    
	// 检查处理码
	// Check processing code
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
        	if( (glTMSSend.szProcCode[0]!=0) &&
            	(memcmp(glTMSSend.szProcCode, glTMSRecv.szProcCode, LEN_PROC_CODE-1)!=0) )
		{
			bFailFlag = TRUE;
		}
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
	{
        	if( (glTMSTELIUMSend.szBit3[0]!=0) &&
            	(memcmp(glTMSTELIUMSend.szBit3, glTMSTELIUMRecv.szBit3, LEN_PROC_CODE-1)!=0) )
		{
			bFailFlag = TRUE;
		}
	}
	else
	{
        	if( (glSendPack.szBit3[0]!=0) &&
            	(memcmp(glSendPack.szBit3, glRecvPack.szBit3, LEN_PROC_CODE-1)!=0) )
		{
			bFailFlag = TRUE;
		}
	}

	if( bFailFlag )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("BIT 3 MISMATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}

	// 检查STAN
	// Check STAN (Sys Trace Audit Number, bit11)
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
		if( glTMSSend.szSTAN[0]!=0 &&
			memcmp(glTMSSend.szSTAN, glTMSRecv.szSTAN, LEN_STAN)!=0 )
		{
			bFailFlag = TRUE;
		}
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
	{
		bFailFlag = FALSE;	//no stan in TMS TELIUM
	}
	else if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOGON || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_ECHO_TEST ||  glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOGON_BUKOPIN )
	{
		bFailFlag = FALSE;	//not mandatory in logon & echo
	}	
	else
	{
		if( glSendPack.szBit11[0]!=0 &&
			memcmp(glSendPack.szBit11, glRecvPack.szBit11, LEN_STAN)!=0 )
		{
			bFailFlag = TRUE;
		}
	}
	
	if( bFailFlag )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("STAN MISMATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}

	// 检查终端号/商户号
	// Check TID, MID
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
		if( memcmp(glTMSSend.szTermID, glTMSRecv.szTermID, LEN_TERM_ID)!=0 )
		{
			bFailFlag = TRUE;
		}
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
	{
		if( memcmp(glTMSTELIUMSend.szBit41, glTMSTELIUMRecv.szBit41, LEN_TERM_ID)!=0 )
		{
			bFailFlag = TRUE;
		}
	}
	else
	{
		if( memcmp(glSendPack.szBit41, glRecvPack.szBit41, LEN_TERM_ID)!=0 )
		{
			bFailFlag = TRUE;
		}
	}
	
	if( bFailFlag )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("TID MISMATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}

#ifdef BANK_MANDIRI
	// More basic checks can be placed here.
	if(ChkIfNeedTLE())
	{
		if(memcmp(glRecvPack.szBit39, "00", 2)==0)
		{
			iRet = ParseTLEData();
			if(iRet <0)
				return ERR_NO_DISP;

			if((debugRx)||(debugIsoRes))
			{
				PubPack8583(glEdcDataDef, &glRecvPack, &glRecvData.sContent[5], (uint*)&glRecvData.uiLength);
			}

			if(debugRx)
				hex_dump_char("RX:", glRecvData.sContent, glRecvData.uiLength);

			if(debugIsoRes)
			{
				pckIsoDump8583(&glRecvData.sContent[5]);
			}
			
		}
	}
#endif
	/*
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA || glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM
		|| memcmp(glRecvPack.szBit39, "00", 2)==0)
		return 0;
	else 
		return ERR_NO_DISP;
	*/
	return 0;
}

// 保存接收报文要素到交易信息结构
// save some data element from receive packet to transaction data.
void SaveRecvPackData(void)
{
	uchar	szLocalTime[14+1];

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glTMSRecv.szRspCode);
		return;
	}

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glTMSTELIUMRecv.szBit39);
		return;
	}

	if( (glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_OFFLINE_SEND) &&
		(glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_TC_SEND) )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glRecvPack.szBit39);
		//ScrPrint(0, 4, ASCII, "%-15s:%2s", "BIT 39 BROO",glRecvPack.szBit39);
		//ScrPrint(0, 6, ASCII, "%-15s:%2s", "RESPON CODE",glProcInfo.stTranLog.szRspCode );
	}
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SETTLEMENT ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_BATCH_UPLOAD     ||
		glProcInfo.stTranLog.ucTranType==TRANS_TYPE_REVERSAL )
	{
		return;
	}

	UpdateLocalTime(NULL, glRecvPack.szBit13, glRecvPack.szBit12);

	GetDateTime(szLocalTime);
	sprintf((char *)glProcInfo.stTranLog.szDateTime, "%.14s", szLocalTime);

	if( glRecvPack.szBit38[0]!=0 )
	{
		sprintf((char *)glProcInfo.stTranLog.szAuthCode, "%.6s", glRecvPack.szBit38);
	}
	if( glRecvPack.szBit37[0]!=0 )
	{
		sprintf((char *)glProcInfo.stTranLog.szRRN, "%.12s", glRecvPack.szBit37);
	}
	sprintf((char *)glProcInfo.stTranLog.szCondCode,  "%.2s",  glSendPack.szBit25);
	sprintf((char *)glProcInfo.stTranLog.szFrnAmount, "%.12s", glRecvPack.szBit5);

	FindCurrency(glRecvPack.szBit51, &glProcInfo.stTranLog.stHolderCurrency);

	#ifdef BANK_ARTHA_JASA
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INQ_PURCHASE ||  glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PURCHASE)
			GetBit63_AJ(glRecvPack.szBit63);

	#endif
}

int AfterTranProc(void)
{
//	int		iRet;
	int		iTransResult;

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
	{
		if( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
		{
			
			DispResult(ERR_HOST_REJ);
			return ERR_NO_DISP;
		}

		DispResult(0);
		return 0;
	}
	else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM )
	{
		if( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
		{
			DispResult(ERR_HOST_REJ);
			return ERR_NO_DISP;
		}

		DispResult(0);
		return 0;
	}

	

	iTransResult = ERR_HOST_REJ;
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
	{
		iTransResult = 0;
	}
	
	// 交易失败处理
	if (iTransResult)
	{
		//hex_dump_char("iTransResult",(uchar *)iTransResult,sizeof(iTransResult));
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		//#ifdef BANK_BRI
		displayResponse();
		//#endif
/*
		DispResult(ERR_HOST_REJ);
		if( memcmp(glProcInfo.stTranLog.szRspCode, "01", 2)==0 ||
			memcmp(glProcInfo.stTranLog.szRspCode, "02", 2)==0 ||
			(glProcInfo.stTranLog.ucInstalment!=0 && ChkAcqOption(ACQ_BOC_INSTALMENT_FEATURE)) )
		{
			// BOC 银行instalment交易时假如交易结果不是"00",都进入referrel交易
			iRet = TranSaleComplete();
			if( iRet==0 )
			{
				return 0;
			}
			
		}
*/
		
		//emv_test();
		return ERR_NO_DISP;
	}

	unPackTransaction();

	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_VOID)
	{
		glProcInfo.stTranLog.uiStatus |= TS_VOID;
		glProcInfo.stTranLog.uiStatus &= ~(TS_ADJ|TS_NOSEND|TS_OFFLINE_SEND|TS_NEED_TC);
		UpdateTranLog(&glProcInfo.stTranLog, glProcInfo.uiRecNo);
	}
	else if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
	}
	SaveRevInfo(FALSE);

	OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	DispTransName();
    
    	if (1)
    	{
        	CommOnHook(FALSE);
    	}
    
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		if( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID)
		{
			GetNewInvoiceNo();
		}
		PrintReceipt(PRN_NORMAL);
/*
#ifdef ENABLE_EMV	// zaki 7 Januari 2014	
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
			{
				if(debugEMV)
				{	PrnInit(); 
				//#ifdef BANK_BRI //untuk sertifikasi
					PrintTagEMV();
				//#endif
					PrnStart();
				}
				if(debugTagEmv)
				{	PrnInit();
				#ifdef BANK_BRI
					PrintTagEMVTes();
				#endif
					PrnStart();
				}
			}
#endif
//*/
	}

	PromptRemoveICC();
//	DispResult(0);
	return 0;
}

// voice referral dialing
int ReferralDial(uchar *pszPhoneNo)
{
#ifndef _P90_
	uchar	ucRet, szTelNo[50];

	if( pszPhoneNo==NULL || *pszPhoneNo==0 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T("TEL# ERROR"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
	}

#ifdef	__TELIUM_PARAM__
	sprintf((char*)szTelNo, "%s%s.", glTlmSysParam.stEdcInfo.szPabx, pszPhoneNo);
#else
	sprintf((char*)szTelNo, "%s%s.", glSysParam.stEdcInfo.szPabx, pszPhoneNo);
#endif
	while( 1 )
	{
		OnHook();
#if !defined(WIN32) && !defined(_P60_S1_) && !defined(_P70_S_)
		ucRet = ModemExCommand("AT-STE=0",NULL,NULL,0);
#endif
		ucRet = ModemDial(NULL, szTelNo, 1);
		if( ucRet==0x83 )
		{	// 旁置电话、并线电话均空闲(仅用于发号转人工接听方式)
			PubDispString(_T("PLS USE PHONE "), 4|DISP_LINE_LEFT);
			PubBeepErr();
		}
		DelayMs(1000);
		if( ucRet==0x06 || ucRet==0x00 || PubWaitKey(0)==KEYENTER )
		{
			return 0;
		}
		if( ucRet==0x0D )
		{
			ScrClrLine(2, 7);
			PubDispString(_T("LINE BUSY"), 4|DISP_LINE_LEFT);
			OnHook();
			PubWaitKey(5);
			return ERR_NO_DISP;
		}
		PubWaitKey(1);
	}
#else
	return 0;
#endif
}

// 调整通讯参数(只是设置Modem电话号码)
// get communication parameter from appropriate source, and set into glCommCfg
int AdjustCommParam(void)
{
	uchar	szNewTelNo[100+1];

	// 构造拨号电话号码
	if (glCommCfg.ucCommType==CT_MODEM)
	{
		if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
		{
#ifdef	__TELIUM_PARAM__
			//sprintf((char *)szNewTelNo, "%s.", glTlmSysParam.stEdcInfo.szDownTelNo); // by 4212
			sprintf((char *)szNewTelNo, "%s%s;%s%s.",
						glTlmSysParam.stEdcInfo.szPabx, glTlmSysParam.stEdcInfo.szDownTelPriNo,
						glTlmSysParam.stEdcInfo.szPabx, glTlmSysParam.stEdcInfo.szDownTelSecNo);
#else
			sprintf((char *)szNewTelNo, "%s.", glSysParam.stEdcInfo.szDownTelNo);
#endif
		}
		else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
		{
#ifdef	__TELIUM_PARAM__
			//sprintf((char *)szNewTelNo, "%s.", glTlmSysParam.stEdcInfo.szDownTelNo); // by 4212
			sprintf((char *)szNewTelNo, "%s%s;%s%s.",
						glTlmSysParam.stEdcInfo.szPabx, glTlmSysParam.stEdcInfo.szDownTelPriNo,
						glTlmSysParam.stEdcInfo.szPabx, glTlmSysParam.stEdcInfo.szDownTelSecNo);

#else
			sprintf((char *)szNewTelNo, "%s.", glSysParam.stEdcInfo.szDownTelNo);
#endif
		}
		else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SETTLEMENT )
		{
#ifdef	__TELIUM_PARAM__
			if( (glCurTlmTmsAcq.acqPriStlPhnSrv[0]!=0) &&
				(glCurTlmTmsAcq.acqSecStlPhnSrv[0]!=0) )
			{	// 都不是空
				sprintf((char *)szNewTelNo, "%s%s;%s%s.",
						glTlmSysParam.stEdcInfo.szPabx, glCurTlmTmsAcq.acqPriStlPhnSrv,
						glTlmSysParam.stEdcInfo.szPabx, glCurTlmTmsAcq.acqSecStlPhnSrv);
			}
			else if( glCurTlmTmsAcq.acqPriStlPhnSrv[0]!=0 )
			{
				sprintf((char *)szNewTelNo, "%s%s.",
						glTlmSysParam.stEdcInfo.szPabx, glCurTlmTmsAcq.acqPriStlPhnSrv);
			}
			else if( glCurTlmTmsAcq.acqSecStlPhnSrv[0]!=0 )
			{
				sprintf((char *)szNewTelNo, "%s%s.",
						glTlmSysParam.stEdcInfo.szPabx, glCurTlmTmsAcq.acqSecStlPhnSrv);
			}
			else
			{
				return ERR_NO_TELNO;
			}
#else
			if( (glCurAcq.stStlPhoneInfo[0].szTelNo[0]!=0) &&
				(glCurAcq.stStlPhoneInfo[1].szTelNo[0]!=0) )
			{	// 都不是空
				sprintf((char *)szNewTelNo, "%s%s;%s%s.",
						glSysParam.stEdcInfo.szPabx, glCurAcq.stStlPhoneInfo[0].szTelNo,
						glSysParam.stEdcInfo.szPabx, glCurAcq.stStlPhoneInfo[1].szTelNo);
			}
			else if( glCurAcq.stStlPhoneInfo[0].szTelNo[0]!=0 )
			{
				sprintf((char *)szNewTelNo, "%s%s.",
						glSysParam.stEdcInfo.szPabx, glCurAcq.stStlPhoneInfo[0].szTelNo);
			}
			else if( glCurAcq.stStlPhoneInfo[1].szTelNo[0]!=0 )
			{
				sprintf((char *)szNewTelNo, "%s%s.",
						glSysParam.stEdcInfo.szPabx, glCurAcq.stStlPhoneInfo[1].szTelNo);
			}
			else
			{
				return ERR_NO_TELNO;
			}
#endif			
		}
		else
		{
#ifdef	__TELIUM_PARAM__
			if( (glCurTlmTmsAcq.acqPriTxnPhnSrv[0]!=0) &&
				(glCurTlmTmsAcq.acqSecTxnPhnSrv[0]!=0) )
			{	// 都不是空
				sprintf((char *)szNewTelNo, "%s%s;%s%s.",
						glTlmSysParam.stEdcInfo.szPabx, glCurTlmTmsAcq.acqPriTxnPhnSrv,
						glTlmSysParam.stEdcInfo.szPabx, glCurTlmTmsAcq.acqSecTxnPhnSrv);
			}
			else if( glCurTlmTmsAcq.acqPriTxnPhnSrv[0]!=0 )
			{
				sprintf((char *)szNewTelNo, "%s%s.",
						glTlmSysParam.stEdcInfo.szPabx, glCurTlmTmsAcq.acqPriTxnPhnSrv);
			}
			else if( glCurTlmTmsAcq.acqSecTxnPhnSrv[0]!=0 )
			{
				sprintf((char *)szNewTelNo, "%s%s.",
						glTlmSysParam.stEdcInfo.szPabx, glCurTlmTmsAcq.acqSecTxnPhnSrv);
			}
			else
			{
				return ERR_NO_TELNO;
			}
#else
			if( (glCurAcq.stTxnPhoneInfo[0].szTelNo[0]!=0) &&
				(glCurAcq.stTxnPhoneInfo[1].szTelNo[0]!=0) )
			{	// 都不是空
				sprintf((char *)szNewTelNo, "%s%s;%s%s.",
						glSysParam.stEdcInfo.szPabx, glCurAcq.stTxnPhoneInfo[0].szTelNo,
						glSysParam.stEdcInfo.szPabx, glCurAcq.stTxnPhoneInfo[1].szTelNo);
			}
			else if( glCurAcq.stTxnPhoneInfo[0].szTelNo[0]!=0 )
			{
				sprintf((char *)szNewTelNo, "%s%s.",
						glSysParam.stEdcInfo.szPabx, glCurAcq.stTxnPhoneInfo[0].szTelNo);
			}
			else if( glCurAcq.stTxnPhoneInfo[1].szTelNo[0]!=0 )
			{
				sprintf((char *)szNewTelNo, "%s%s.",
						glSysParam.stEdcInfo.szPabx, glCurAcq.stTxnPhoneInfo[1].szTelNo);
			}
			else
			{
				return ERR_NO_TELNO;
			}
#endif
		}
		sprintf((char *)glCommCfg.stPSTNPara.szTelNo, "%s", szNewTelNo);

		if( !CommChkIfSameTelNo(szNewTelNo) )
		{
			CommOnHook(FALSE);
		}
	}

	// 从ACQUIRER提取目标IP
	if ((glCommCfg.ucCommType==CT_TCPIP) || (glCommCfg.ucCommType==CT_WIFI))
	{
		if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
		{
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_1, "%.15s", glTlmSysParam.stEdcInfo.stDownIpAddr.szIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_1, "%.5s", glTlmSysParam.stEdcInfo.stDownIpAddr.szPort);
#else
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_1, "%.15s", glSysParam.stEdcInfo.stDownIpAddr.szIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_1, "%.5s", glSysParam.stEdcInfo.stDownIpAddr.szPort);
#endif
		}
		else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM )
		{
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_1, "%.15s", glTlmSysParam.stEdcInfo.stDownIPPriNo.szIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_1, "%.5s", glTlmSysParam.stEdcInfo.stDownIPPriNo.szPort);

			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_2, "%.15s", glTlmSysParam.stEdcInfo.stDownIPSecNo.szIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_2, "%.5s", glTlmSysParam.stEdcInfo.stDownIPSecNo.szPort);
#else
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_1, "%.15s", glSysParam.stEdcInfo.stDownIpAddr.szIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_1, "%.5s", glSysParam.stEdcInfo.stDownIpAddr.szPort);
#endif			
		}
		else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SETTLEMENT )
		{
			// 使用transaction IP 1,2. 不使用 settle IP
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_1, "%.15s", glCurTlmTmsAcq.acqPriStlIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_1, "%.5s", glCurTlmTmsAcq.acqPriStlPort);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_2, "%.15s", glCurTlmTmsAcq.acqSecStlIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_2, "%.5s", glCurTlmTmsAcq.acqSecStlPort);
#else
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_1, "%.15s", glCurAcq.StlTcpIp1);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_1, "%.5s", glCurAcq.StlTcpPort1);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_2, "%.15s", glCurAcq.StlTcpIp2);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_2, "%.5s", glCurAcq.StlTcpPort2);
#endif
		}
		else
		{
			// 使用transaction IP 1,2. 不使用 settle IP
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_1, "%.15s", glCurTlmTmsAcq.acqPriTxnIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_1, "%.5s", glCurTlmTmsAcq.acqPriTxnPort);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_2, "%.15s", glCurTlmTmsAcq.acqSecTxnIP);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_2, "%.5s", glCurTlmTmsAcq.acqSecTxnPort);
#else
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_1, "%.15s", glCurAcq.TxnTcpIp1);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_1, "%.5s", glCurAcq.TxnTcpPort1);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemoteIP_2, "%.15s", glCurAcq.TxnTcpIp2);
			sprintf((char *)glCommCfg.stTcpIpPara.szRemotePort_2, "%.5s", glCurAcq.TxnTcpPort2);
#endif
		}
	}

	// 从ACQUIRER提取目标IP
	if ((glCommCfg.ucCommType==CT_GPRS) || (glCommCfg.ucCommType==CT_CDMA))
	{
		if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_LOAD_PARA )
		{
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_1, "%.15s", glTlmSysParam.stEdcInfo.stDownIpAddr.szIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_1, "%.5s", glTlmSysParam.stEdcInfo.stDownIpAddr.szPort);
#else
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_1, "%.15s", glSysParam.stEdcInfo.stDownIpAddr.szIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_1, "%.5s", glSysParam.stEdcInfo.stDownIpAddr.szPort);
#endif			
		}
		else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
		{
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_1, "%.15s", glTlmSysParam.stEdcInfo.stDownIPPriNo.szIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_1, "%.5s", glTlmSysParam.stEdcInfo.stDownIPPriNo.szPort);

			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_2, "%.15s", glTlmSysParam.stEdcInfo.stDownIPSecNo.szIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_2, "%.5s", glTlmSysParam.stEdcInfo.stDownIPSecNo.szPort);
#else
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_1, "%.15s", glSysParam.stEdcInfo.stDownIpAddr.szIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_1, "%.5s", glSysParam.stEdcInfo.stDownIpAddr.szPort);
#endif
		}
		else if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_SETTLEMENT )
		{
			// 使用transaction IP 1,2. 不使用 settle IP
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_1, "%.15s", glCurTlmTmsAcq.acqPriStlIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_1, "%.5s", glCurTlmTmsAcq.acqPriStlPort);
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_2, "%.15s", glCurTlmTmsAcq.acqSecStlIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_2, "%.5s", glCurTlmTmsAcq.acqSecStlPort);
#else
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_1, "%.15s", glCurAcq.StlGPRSIp1);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_1, "%.5s", glCurAcq.StlGPRSPort1);
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_2, "%.15s", glCurAcq.StlGPRSIp2);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_2, "%.5s", glCurAcq.StlGPRSPort2);
#endif
		}
		else
		{
			// 使用transaction IP 1,2. 不使用 settle IP
#ifdef	__TELIUM_PARAM__
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_1, "%.15s", glCurTlmTmsAcq.acqPriTxnIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_1, "%.5s", glCurTlmTmsAcq.acqPriTxnPort);
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_2, "%.15s", glCurTlmTmsAcq.acqSecTxnIP);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_2, "%.5s", glCurTlmTmsAcq.acqSecTxnPort);
#else
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_1, "%.15s", glCurAcq.TxnGPRSIp1);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_1, "%.5s", glCurAcq.TxnGPRSPort1);
			sprintf((char *)glCommCfg.stWirlessPara.szRemoteIP_2, "%.15s", glCurAcq.TxnGPRSIp2);
			sprintf((char *)glCommCfg.stWirlessPara.szRemotePort_2, "%.5s", glCurAcq.TxnGPRSPort2);
#endif
		}
	}

	return CommSetCfgParam(&glCommCfg);
}

void DispCommErrMsg(int iErrCode)
{
	COMM_ERR_MSG	stCommErrMsg;

	CommGetErrMsg(iErrCode, &stCommErrMsg);

	ScrClrLine(2, 7);
	PubDispString(_T(stCommErrMsg.szMsg), 4|DISP_LINE_LEFT);

#if 0	//closed by irf, 20130103
	if (iErrCode<0)
	{
		ScrPrint(100, 7, ASCII, "%d", iErrCode);
	}
	else
	{
		ScrPrint(100, 7, ASCII, "%02X", (iErrCode & MASK_ERR_CODE));
	}
#endif

	PubBeepErr();
	PubWaitKey(5);
}

// end of file

