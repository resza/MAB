
#include "global.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
/********************** Internal variables declaration *********************/

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// 处理用户自定义事件
// When the first time called by manager, the msg type must be USER_MSG
int ProcUserMsg(void)
{
	int		iRet;
	uchar	szEngTime[16+1];
	uchar	ucAcqIndex;

#ifdef	__TELIUM_PARAM__
	// no need to init module if running as minor sub-app
	if( glTlmSysParam.stTxnCommCfg.ucCommType==CT_GPRS  ||
		glTlmSysParam.stTxnCommCfg.ucCommType==CT_CDMA  ||
		glTlmSysParam.stTxnCommCfg.ucCommType==CT_TCPIP ||
		glTlmSysParam.stTxnCommCfg.ucCommType==CT_WIFI)
	{
		GetEngTime(szEngTime);
		PubShowTitle(TRUE, szEngTime);

		if(glTlmSysParam.stTxnCommCfg.ucCommType==CT_GPRS  ||
			glTlmSysParam.stTxnCommCfg.ucCommType==CT_CDMA )
		{
			PubDispString(_T("SEARCHING SIGNAL"), 4|DISP_LINE_CENTER);
			PubDispString(_T("PLEASE WAIT"), 6|DISP_LINE_CENTER);
		}
		else
			PubDispString(_T("INIT COMM..."), 4|DISP_LINE_LEFT);
		
		iRet = CommInitModule(&glTlmSysParam.stTxnCommCfg);
		if (iRet!=0)
		{
			PubShowTitle(TRUE, szEngTime);
			PubDispString(_T("INIT FAIL"), 2|DISP_LINE_LEFT);
			if (glTlmSysParam.stTxnCommCfg.ucCommType==CT_GPRS  ||
				glTlmSysParam.stTxnCommCfg.ucCommType==CT_CDMA)
			{
				PubDispString(_T("PLS CHECK SIM OR"), 4|DISP_LINE_LEFT);
				PubDispString(_T("HARDWARE/SIGNAL."), 6|DISP_LINE_LEFT);
			}
			if (glTlmSysParam.stTxnCommCfg.ucCommType==CT_TCPIP)
			{
				PubDispString(_T("PLS CHECK CABLE"),  4|DISP_LINE_LEFT);
				PubDispString(_T("OR CONFIG."),       6|DISP_LINE_LEFT);
			}
			if (glTlmSysParam.stTxnCommCfg.ucCommType==CT_WIFI)
			{
				PubDispString(_T("PLS CHECK SIGNAL"), 4|DISP_LINE_LEFT);
				PubDispString(_T("OR CONFIG."),       6|DISP_LINE_LEFT);
			}
			PubWaitKey(-1);
		}
	}

	if((glTlmSysParam.stTxnCommCfg.ucCommType==CT_GPRS  ||	glTlmSysParam.stTxnCommCfg.ucCommType==CT_CDMA )&&(iRet==0))
	{
		iRet = WlPppLogin(glTlmSysParam.stTxnCommCfg.stWirlessPara.szAPN, 
			glTlmSysParam.stTxnCommCfg.stWirlessPara.szUID, 
			glTlmSysParam.stTxnCommCfg.stWirlessPara.szPwd, 
			0xFF, 0, 0);
		
		if(iRet==0)
		{
			ScrSetIcon(ICON_PHONE, ONHOOKICON);
			SXXWlDispSignal();
		}

		TimerSet(TIMER_TEMPORARY, (ushort)(30*10));
		while (TimerCheck(TIMER_TEMPORARY)!=0)
		{
			iRet = WlPppCheck();

			if (iRet == 0)
			{
				break;
			}
		}	
		
	}
	// 改用新方式告诉管理器本程序是否支持EMV
	// Use a new method to communicates with app-manager

	// erase transaction log of last settlement
	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		if( glSysCtrl.sAcqStatus[ucAcqIndex]==S_CLR_LOG )
		{
			ClearRecord(glTlmSysParam.Acq[ucAcqIndex].acqID);
		}
	}
#else
	// no need to init module if running as minor sub-app
	if( glSysParam.stTxnCommCfg.ucCommType==CT_GPRS  ||
		glSysParam.stTxnCommCfg.ucCommType==CT_CDMA  ||
		glSysParam.stTxnCommCfg.ucCommType==CT_TCPIP ||
		glSysParam.stTxnCommCfg.ucCommType==CT_WIFI)
	{
		GetEngTime(szEngTime);
		PubShowTitle(TRUE, szEngTime);
		PubDispString(_T("INIT COMM..."), 4|DISP_LINE_LEFT);
		
		iRet = CommInitModule(&glSysParam.stTxnCommCfg);
		if (iRet!=0)
		{
			PubShowTitle(TRUE, szEngTime);
			PubDispString(_T("INIT FAIL"), 2|DISP_LINE_LEFT);
			if (glSysParam.stTxnCommCfg.ucCommType==CT_GPRS  ||
				glSysParam.stTxnCommCfg.ucCommType==CT_CDMA)
			{
				PubDispString(_T("PLS CHECK SIM OR"), 4|DISP_LINE_LEFT);
				PubDispString(_T("HARDWARE/SIGNAL."), 6|DISP_LINE_LEFT);
			}
			if (glSysParam.stTxnCommCfg.ucCommType==CT_TCPIP)
			{
				PubDispString(_T("PLS CHECK CABLE"),  4|DISP_LINE_LEFT);
				PubDispString(_T("OR CONFIG."),       6|DISP_LINE_LEFT);
			}
			if (glSysParam.stTxnCommCfg.ucCommType==CT_WIFI)
			{
				PubDispString(_T("PLS CHECK SIGNAL"), 4|DISP_LINE_LEFT);
				PubDispString(_T("OR CONFIG."),       6|DISP_LINE_LEFT);
			}
			PubWaitKey(-1);
		}
	}

	// 改用新方式告诉管理器本程序是否支持EMV
	// Use a new method to communicates with app-manager

	// erase transaction log of last settlement
	for(ucAcqIndex=0; ucAcqIndex<glSysParam.ucAcqNum; ucAcqIndex++)
	{
		if( glSysCtrl.sAcqStatus[ucAcqIndex]==S_CLR_LOG )
		{
			ClearRecord(glSysParam.stAcqList[ucAcqIndex].ucKey);
		}
	}
#endif
	return 0;
}

// 自动下载参数
// Auto download parameter, new Protims protocol
int AutoDownParam(void)
{
	return 0;
}

// 读取自动参数下载参数事件
// get auto download parameter and return it to app manager
int GetAutoDownParam(void)
{
	return 0;
}

// 处理ECR事件
// Process ECR message
int ProcEcrMsg(void)
{
	return 0;
}

// 处理磁卡刷卡事件
// Process magcard swipe event
int ProcMagMsg(void)
{
	int		iRet;

		

//	glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE_OR_AUTH;
	glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE;
	iRet = SwipeCardProc(TRUE);
	if( iRet!=0 )
	{
		return ERR_NO_DISP;
	}

	//cek apakah mesti settle atau tidak? diki add for bukopin
	/*
	iRet = ValidTransSettleBkpn();
		if( iRet==1 )
		{
			return 0;
		}
		*/
		

	iRet = ValidCard();
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = TransCapture();
	if( iRet!=0 )
	{
		CommOnHook(FALSE);
		return iRet;
	}

	return 0;
}

// 处理IC卡插卡事件
// Process IC card insertion event
int ProcICCMsg(void)
{
	int		iRet;

	//cek apakah mesti settle atau tidak? diki add for bukopin
	/*
	iRet = ValidTransSettleBkpn();
		if( iRet==1 )
		{
			return 0;
		}
		*/
	

//	glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE_OR_AUTH;
	iRet = 0;
	iRet = Req_ICC_PrvtLbl_Cek();
	if( iRet==0 )
	{
		iRet = TransSalePrvtLbl();
		if( iRet!=0 )
		{
			CommOnHook(FALSE);
			return iRet;
		}
	} else {


	glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE;
	iRet = GetCard(SKIP_DETECT_ICC|CARD_INSERTED);
	if( iRet!=0 )
	{
		return iRet;
	}
		

	iRet = TransCapture();
	if( iRet!=0 )
	{
		CommOnHook(FALSE);
		return iRet;
	}

	}

	return 0;
}

// 处理功能键按键事件
// Process FN key pressed event
int ProcFuncKeyMsg(void)
{
	FunctionExe(FALSE, glEdcMsgPtr->KeyValue);
	CommOnHook(FALSE);

	return 0;
}

// 处理普通按键事件
// Process common key pressed event
int ProcKeyMsg(void)
{
	int		iRet, iFuncNo;

	// if UserMsg points to a string "CARDNO=XXXXXX..."
	if ((glEdcMsgPtr->UserMsg!=NULL) &&
		(memcmp((char *)(glEdcMsgPtr->UserMsg), "CARDNO=", 7)==0))
	{
//		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE_OR_AUTH;
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE;

		iRet = GetManualPanFromMsg((char *)(glEdcMsgPtr->UserMsg)+7);	// iRet determines the flow is continue or not
		if (iRet!=0)
		{
			return iRet;
		}

		iRet = TransCapture();
		CommOnHook(FALSE);
		return ERR_NO_DISP;
	}

#if !defined(APP_MANAGER_VER) && !defined(_WIN32)
	if (glEdcMsgPtr->KeyValue==KEYCANCEL)
	{
		if (ChkIfAllowExit())
		{
			return ERR_EXIT_APP;
		}
	}
#endif

	if( glEdcMsgPtr->KeyValue>=KEY0 && glEdcMsgPtr->KeyValue<=KEY9 )
	{
#ifdef	__TELIUM_PARAM__
	 	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
	 	{
	//		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE_OR_AUTH;
			glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE;
			//if (setKeyIn) // dituup zaki 26022014
			//{ // dituup zaki 26022014
				iRet = ManualInputPan(glEdcMsgPtr->KeyValue);
			
				if( iRet!=0 )
				{
					return iRet;
				}
				iRet = TransCapture();
				if( iRet!=0 )
				{
					CommOnHook(FALSE);
					return iRet;
				//} // dituup zaki 26022014
			}
	 	}

		
#else
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_SALE;
		iRet = ManualInputPan(glEdcMsgPtr->KeyValue);
		if( iRet!=0 )
		{
			return iRet;
		}
		iRet = TransCapture();
		if( iRet!=0 )
		{
			CommOnHook(FALSE);
			return iRet;
		}
#endif
	}

	iRet = ERR_NO_DISP;
	switch( glEdcMsgPtr->KeyValue )
	{
		case KEYFN:
			iFuncNo = FunctionInput();
			if( iFuncNo>=0 )
			{
				glEdcMsgPtr->KeyValue = (uchar)iFuncNo;
				ProcFuncKeyMsg();
			}
			break;

		case KEYUP:
			hdlMainMenu();
	//		iRet = TransOther();
	//		PromptRemoveICC();
			
	//		FunctionMenu();
	//		TransMandiri();
			break;
	        
		case KEYDOWN:
#ifdef _SP30_
			iRet = TransOther();		//Main Menu, irf, 20121129
			PromptRemoveICC();
#else
			hdlMainMenu();
	//		iRet = TransOther();
			PromptRemoveICC();

	//		FunctionMenu();
	//		iRet = TransMandiri();
#endif
			break;

		case KEYMENU:
			hdlMainMenu();
//			iRet = TransOther();
			PromptRemoveICC();
			break;

#ifdef _P80_
		case KEYF1:
			iRet = TransVoid();
			break;

		case KEYF2:
			iRet = TransSettle();
			break;

		case KEYF3:
			iRet = TransRefund();
			PromptRemoveICC();
			break;

		case KEYF4:
			TransAdjust();
			break;

		case KEYF6:
			PrnLastTrans();
			break;
#endif
#ifdef _P60_S1_
		case KEYF1:
			iRet = TransVoid();
			break;

		case KEYF2:
			TransAdjust();
			break;

		case KEYF3:
			iRet = TransSettle();
			break;

		case KEYF4:
			PrnLastTrans();
			break;
#endif
#ifdef _P70_S_
		case KEYF1:
			iRet = TransVoid();
			break;

		case KEYF2:
			PrnLastTrans();
			break;

		case KEYF3:
			TransAdjust();
			break;

		case KEYF4:
			iRet = TransSettle();
			break;
#endif
#ifdef _S_SERIES_
		case KEYATM1:
			iRet = TransVoid();
			break;

		case KEYATM2:
			//TransAdjust();
			iRet=TransOffSale();
			break;

		case KEYATM3:
			iRet = TransSettleNew();
			break;

		case KEYATM4:
			PrnLastTrans();
			break;
#endif
		case KEYALPHA:
			TransEchoTest();
			break;
			
		case KEYCLEAR:
			//RollOut();
			break;
			
		default:
			break;
	}
	if( iRet!=0 )
	{
		CommOnHook(FALSE);
		return iRet;
	}

	return 0;
}




// cek aplikasi private label atau bukan

int Req_ICC_PrvtLbl_Cek(void)
{
    	byte Buf[100];          // Buffer to accept the input
    	byte Buf222[100];          // Buffer to accept the input
    	byte Buf2[56+1];
    	int ret; // icc = 0;

		byte cmd[6+1];
		byte dat[24+2]; 			// first byte is length of dat (16 bytes)
		int ucRet;
		APDU_SEND	ApduSend;
		APDU_RESP	ApduResp;

    	char dataIn[16+1];	

		uchar DataBuff1[2];
		uchar DataBuff2[2];
		uchar DataBuff3[40];
		uchar DataBuff3HolderName[40];
		uchar szTempName[40];
	
	uchar buffer[999];
	//uchar ucRet;

	int ilength, iRet, i;

		MEM_ZERO(cmd);
			MEM_ZERO(dat);
			MEM_ZERO(&ApduSend);
			MEM_ZERO(&ApduResp);

			IccClose(0);

		memset(Buf, 0, sizeof(Buf));
		ucRet = IccInit(0x80, (uchar *) Buf);



    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
		memset(dataIn, 0, sizeof(dataIn));
    	///memcpy(dataIn, "\xA0\x00\x00\x00\x18\x4b\x65\x6b\x53\x61\x6d\x00\x00\x01\x01\x02", 16);	
    	//unsigned char selectApp1[13]={0x00,0xA4,0x04,0x00,0x07,0xA0,0x00,0x00,0x00,0x65,0x00,0x00,0x00};
		// \x07\xA0\x00\x00\x00\x65\x00\x00\x00
		memcpy(dataIn, "\xA0\x00\x00\x00\x65\x00\x00", 7);	

	
	
	ApduSend.Command[0] = 0x00;				//STAPDUCMD.CLA
	ApduSend.Command[1] = 0xA4;				//STAPDUCMD.INS
	ApduSend.Command[2] = 0x04;		    	//STAPDUCMD.P1
	ApduSend.Command[3] = 0x00;				//STAPDUCMD.P2

	ApduSend.Lc = 7;  					//5STAPDUCMD.P3
	///ApduSend.Le = 0;

	memcpy(ApduSend.DataIn, dataIn, 7);                 	// 3 bytes of LMTK_ACQ_ID
	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	////hex_dump_char("\nDATA &ApduSend:", &ApduSend, sizeof(&ApduSend));
	////hex_dump_char("\nDATA &ApduResp:", &ApduResp, sizeof(&ApduResp));

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			//memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			IccClose(0);
			return 0;
		}
		else
			IccClose(0);
			return 1;
	}
	
	IccClose(0);
	return;

}


// end of file

