#include "global.h"

void SetPosEntryMode(void)
{
	sprintf((char *)glSendPack.szBit22, "0000");
	if( glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT )
	{
		//memcpy(&glSendPack.szBit22[1], "012", 3);
		return;
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT )
	{
		switch(glProcInfo.stTranLog.ucTranType)
			{
			case TRANS_TYPE_SALE:
			case TRANS_TYPE_VOID:
			//	memcpy(&glSendPack.szBit22[0], "0022", 4);
			//break;
			default:
				memcpy(&glSendPack.szBit22[0], "0901", 4);
				break;
			}
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		memcpy(&glSendPack.szBit22[0], "0052", 4);
	}
	else if( glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE )
	{
		memcpy(&glSendPack.szBit22[1], "802", 3);
	}


	//if(glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT || (glProcInfo.stTranLog.uiEntryMode & MODE_OFF_PIN))
	if(glCurTlmTmsIss.issPinOpt == 0x32 || glCurTlmTmsIss.issPinOpt == 0x31)
	{
		glSendPack.szBit22[3]='1';
	}

}

void CalculatePinBlock(uchar *sPinBlock)
{
	char PinKey[1+16];
	char dataXor[8+1];
	char dataPin[8+1];

	MEM_ZERO(PinKey);
	MEM_ZERO(dataXor);		
	MEM_ZERO(dataPin);
	
	//mapGet(acqPinKey, PinKey, sizeof(PinKey));
	memcpy(PinKey,glCurTlmTmsAcq.acqPinKey,16);
	
	memcpy(dataXor, glProcInfo.sPlainPinBlock, 8); 

	stdEDE(dataPin, dataXor, PinKey);
	
	memcpy(sPinBlock, dataPin, 8);
	//*
	#ifdef ENABLE_EMV	// 
				if(debugEMVCalc)
				{	
				hex_dump_char("PinKey___CalcPinBlock", PinKey, 16);
				hex_dump_char("DataXor__CalcPinBlock", dataXor, 8);
				hex_dump_char("PinBlock_CalcPinBlock", sPinBlock, 8);
				}
	#endif
	//*/
}

void CalculatePinBlockCS(uchar *sPinBlock)
{
	char PinKey[1+16];
	char dataXor[8+1];
	char dataPin[8+1];

	MEM_ZERO(PinKey);
	MEM_ZERO(dataXor);		
	MEM_ZERO(dataPin);
	
	mapGet(acqPinKey, PinKey, sizeof(PinKey));
	memcpy(dataXor, glProcInfo.sPlainPinBlock2, 8); 

	stdEDE(dataPin, dataXor, PinKey);
	
	memcpy(sPinBlock, dataPin, 8);
}

void SetPackSettlement(void)
{
#ifdef ENABLE_EMV
//	int	iLength;
#endif
	uchar	szTotalAmt[12+1];
	uchar 	ucBufAmt[12+1];

	MEM_ZERO(ucBufAmt);
	MEM_ZERO(szTotalAmt);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

	
//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

	//BIT 60, BATCH NUM
	//==================================================================================
	memcpy(glSendPack.szBit60, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit60[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================


	if( ChkIfNeedMac() )
	{
		PubLong2Char((ulong)LEN_MAC, 2, glSendPack.sMac);
	}
}


void setPackLogonAJ(void)
{	
	//uchar sn[15+1]; // Add Zaki 20 Januari 2014 [32+1]
	int len=0;

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
/*//
//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
//*/	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurAcq.szNii);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurAcq.szTermID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurAcq.szMerchantID);
	//==================================================================================
}


void setPackLogonBukopin(void)
{	
	//uchar sn[15+1]; // Add Zaki 20 Januari 2014 [32+1]
	int len=0;

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
/*//
//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
//*/	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurAcq.szNii);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurAcq.szTermID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurAcq.szMerchantID);
	//==================================================================================
}


void SetPackPurchase(void)
{
#ifdef ENABLE_EMV
//	int	iLength;
#endif
	uchar	szTotalAmt[12+1];
	uchar 	ucBufAmt[12+1];

//	hex_dump_char("SINI", &glProcInfo.stTranLog.uiEntryMode, sizeof(glProcInfo.stTranLog.uiEntryMode));

	MEM_ZERO(ucBufAmt);
	MEM_ZERO(szTotalAmt);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	//==================================================================================

// BIT 4
	//==================================================================================
	if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//		strcpy(ucBufAmt, szTotalAmt);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
		sprintf((char *)glSendPack.szBit4,   "%.*s", LEN_TRAN_AMT,   szTotalAmt);
#ifndef	BANK_MANDIRI
		sprintf((char *)glSendPack.szBit54, "%.*s", LEN_EXT_AMOUNT, glProcInfo.stTranLog.szTipAmount);
#endif
	}
	else
	{
//		strcpy(ucBufAmt, glProcInfo.stTranLog.szAmount);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
	}
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 23
	//==================================================================================				
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
	{	
		if( glProcInfo.stTranLog.bPanSeqOK )
		{	
			sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
		}
	}
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================
	
//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	if(BIT48ON)
	{
		PubLong2Char((ulong)1, 2, glSendPack.szBit48);
		memcpy(&glSendPack.szBit48[2], "1", 1);
	}
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		CalculatePinBlock(glProcInfo.sPinBlock);
		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================
//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
	
//BIT 63
	//==================================================================================
	PubLong2Char((ulong)SetBit63_AJ(), 2, glSendPack.szBit63);
	sprintf((char *)&(glSendPack.szBit63[2]), "%s", bit63req);
	//==================================================================================
		
//BIT 64
	//==================================================================================
	PubLong2Char(8, 2, glSendPack.sMac);
	sprintf((char *)&(glSendPack.sMac[2]), "%s", "00000000");
	//==================================================================================

return;
}

void SetPackSale(void)
{
#ifdef ENABLE_EMV
	int	iLength;
#endif
	uchar	szTotalAmt[12+1];
	uchar 	ucBufAmt[12+1];

//	hex_dump_char("SINI", &glProcInfo.stTranLog.uiEntryMode, sizeof(glProcInfo.stTranLog.uiEntryMode));

	MEM_ZERO(ucBufAmt);
	MEM_ZERO(szTotalAmt);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	//==================================================================================

// BIT 4
	//==================================================================================
	if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//		strcpy(ucBufAmt, szTotalAmt);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
		sprintf((char *)glSendPack.szBit4,   "%.*s", LEN_TRAN_AMT,   szTotalAmt);
#ifndef	BANK_MANDIRI
		sprintf((char *)glSendPack.szBit54, "%.*s", LEN_EXT_AMOUNT, glProcInfo.stTranLog.szTipAmount);
#endif
	}
	else
	{
//		strcpy(ucBufAmt, glProcInfo.stTranLog.szAmount);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
	}
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 23
	//==================================================================================				
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
	{	
		if( glProcInfo.stTranLog.bPanSeqOK )
		{	
			sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
		}
	}
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================


// PAN/track 1/2/3/expiry etc
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		if(glProcInfo.szTrack2[0]==0)
		{
	//BIT 2
	//==================================================================================
			sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================
	
	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
		}
		else
		{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT) ||
		(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) )
	{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
	//BIT 2
	//==================================================================================
		sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
	}

	
//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//	sprintf((char *)glSendPack.szBit42, "%-15s", glCurTlmTmsAcq.acqMID+5);
	//==================================================================================

//BIT 48
	//==================================================================================
	if(BIT48ON)
	{
		PubLong2Char((ulong)1, 2, glSendPack.szBit48);
		memcpy(&glSendPack.szBit48[2], "1", 1);
	}
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT)
	{
		CalculatePinBlock(glProcInfo.sPinBlock);
	
		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

	
//BIT 55
	//==================================================================================
#ifdef ENABLE_EMV
		else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
			(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
		{
			SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
			PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
			memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);
			glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
		}
#endif
	//==================================================================================

//BIT 57
#ifdef BANK_BRI
	if(ChkIfBRIDebit())
	{	
		PubLong2Char((ulong)8, 2, glSendPack.szBit57);
		memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	}
#endif

	//BIT 62, ROC/SOC
	//==================================================================================
	#ifndef BANK_PANIN
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	#else

	//uchar SerialNo[99];
	//ReadSN(SerialNo);
	//mapGet(appTid,SerialNo,8);
	//PrnInit();
	//PrnStr("SerialNo : %s",SerialNo);


	#endif
	//==================================================================================
}

void SetPackSaleRedeemption(void)
{
#ifdef ENABLE_EMV
	int	iLength;
#endif
	uchar	szTotalAmt[12+1];
	uchar 	ucBufAmt[12+1];

	MEM_ZERO(ucBufAmt);
	MEM_ZERO(szTotalAmt);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

// BIT 4
	//==================================================================================
	if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//		strcpy(ucBufAmt, szTotalAmt);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
		sprintf((char *)glSendPack.szBit4,   "%.*s", LEN_TRAN_AMT,   szTotalAmt);
#ifndef	BANK_MANDIRI
		sprintf((char *)glSendPack.szBit54, "%.*s", LEN_EXT_AMOUNT, glProcInfo.stTranLog.szTipAmount);
#endif
	}
	else
	{
//		strcpy(ucBufAmt, glProcInfo.stTranLog.szAmount);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
	}
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 23
	//==================================================================================				
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
	{	
		if( glProcInfo.stTranLog.bPanSeqOK )
		{	
			sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
		}
	}
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================


// PAN/track 1/2/3/expiry etc
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		if(glProcInfo.szTrack2[0]==0)
		{
	//BIT 2
	//==================================================================================
			sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================
	
	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
		}
		else
		{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT) ||
		(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) )
	{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
	//BIT 2
	//==================================================================================
		sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
	}

	
//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

	
//BIT 55
	//==================================================================================
#ifdef ENABLE_EMV
		else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
			(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
		{
			SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
			PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
			memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);
			glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
		}
#endif
	//==================================================================================
	
	//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

	//BIT 63
	//==================================================================================
	PubLong2Char((ulong)105, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2],glProcInfo.szReq63SaleRedeemption, 105);
	memcpy(&glProcInfo.stTranLog.BRI.SaleRedeemption.Buff63,glProcInfo.szReq63SaleRedeemption,105);
	//==================================================================================

	
}


void setPackPrivilage(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003PRIVIL           ", 20);
	//==================================================================================
}

void SetPackCicilanBukopin(void)
{
#ifdef ENABLE_EMV
	int	iLength;
#endif
	uchar	szTotalAmt[12+1];
	uchar 	ucBufAmt[12+1];

	MEM_ZERO(ucBufAmt);
	MEM_ZERO(szTotalAmt);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

// BIT 4
	//==================================================================================
	if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//		strcpy(ucBufAmt, szTotalAmt);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
		sprintf((char *)glSendPack.szBit4,   "%.*s", LEN_TRAN_AMT,   szTotalAmt);
#ifndef	BANK_MANDIRI
		sprintf((char *)glSendPack.szBit54, "%.*s", LEN_EXT_AMOUNT, glProcInfo.stTranLog.szTipAmount);
#endif
	}
	else
	{
//		strcpy(ucBufAmt, glProcInfo.stTranLog.szAmount);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
	}
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 23
	//==================================================================================				
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
	{	
		if( glProcInfo.stTranLog.bPanSeqOK )
		{	
			sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
		}
	}
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================


// PAN/track 1/2/3/expiry etc
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		if(glProcInfo.szTrack2[0]==0)
		{
	//BIT 2
	//==================================================================================
			sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================
	
	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
		}
		else
		{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT) ||
		(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) )
	{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
	//BIT 2
	//==================================================================================
		sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
	}

	
//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

	
//BIT 55
	//==================================================================================
#ifdef ENABLE_EMV
		else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
			(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
		{
			SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
			PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
			memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);
			glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
		}
#endif
	//==================================================================================

//BIT 61
	//#undef BANK_HSBC
	//==================================================================================
	#ifdef BANK_PANIN
	PubLong2Char((ulong)13, 2, glSendPack.szBit61);
	memcpy(&glSendPack.szBit61[2], glProcInfo.szReq61CicilanBukopin, 13);
	#else
		#ifdef BANK_HSBC
		#else
		PubLong2Char((ulong)3, 2, glSendPack.szBit61);
		memcpy(&glSendPack.szBit61[2], glProcInfo.szReq61CicilanBukopin, 3);
		#endif
	#endif
	//==================================================================================
	
//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
}

void SetPackCicilanBring(void)
{
#ifdef ENABLE_EMV
	int	iLength;
#endif
	uchar	szTotalAmt[12+1];
	uchar 	ucBufAmt[12+1];

	MEM_ZERO(ucBufAmt);
	MEM_ZERO(szTotalAmt);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

// BIT 4
	//==================================================================================
	if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//		strcpy(ucBufAmt, szTotalAmt);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
		sprintf((char *)glSendPack.szBit4,   "%.*s", LEN_TRAN_AMT,   szTotalAmt);
#ifndef	BANK_MANDIRI
		sprintf((char *)glSendPack.szBit54, "%.*s", LEN_EXT_AMOUNT, glProcInfo.stTranLog.szTipAmount);
#endif
	}
	else
	{
//		strcpy(ucBufAmt, glProcInfo.stTranLog.szAmount);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
	}
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 23
	//==================================================================================				
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
	{	
		if( glProcInfo.stTranLog.bPanSeqOK )
		{	
			sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
		}
	}
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================


// PAN/track 1/2/3/expiry etc
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		if(glProcInfo.szTrack2[0]==0)
		{
	//BIT 2
	//==================================================================================
			sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================
	
	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
		}
		else
		{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT) ||
		(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) )
	{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
	//BIT 2
	//==================================================================================
		sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
	}

	
//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

	
//BIT 55
	//==================================================================================
#ifdef ENABLE_EMV
		else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
			(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
		{
			SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
			PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
			memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);
			glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
		}
#endif
	//==================================================================================

//BIT 61
	//==================================================================================
	PubLong2Char((ulong)14, 2, glSendPack.szBit61);
	memcpy(&glSendPack.szBit61[2], glProcInfo.szReq61CicilanBring, 14);
	//==================================================================================
	
	//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
}

#ifdef BANK_BRI
// ============================= CARD SERVICE ============================================= 
void setPackAktivasiKartuPetugas(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 23);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001OTO              ", 20);
	//==================================================================================
}


void setPackAktivasiKartuNasabah(void)
{
	char		null[8+1];
	MEM_ZERO(null);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], null, LEN_PIN_DATA);
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "204AKTIVASI         ", 20);
	//==================================================================================
}

void setPackAktivasiKartuPrint(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)60, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 60);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003AKTIVASI         ", 20);
	//==================================================================================
}

void setPackKartuPetugas(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 23);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001OTO              ", 20);
	//==================================================================================
}


void setPackRenewalKartuLama(void)
{
	char		null[8+1];
	MEM_ZERO(null);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], null, LEN_PIN_DATA);
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "204RENEWAL          ", 20);
	//==================================================================================
}

void setPackRenewalKartuBaru(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)60, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 60);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003RENEWAL          ", 20);
	//==================================================================================
}

void setPackReissuePinPetugas(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 23);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001OTO              ", 20);
	//==================================================================================
}

void setPackReissuePinNasabah(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)60, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 60);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		CalculatePinBlockCS(glProcInfo.sPinBlock2);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock2, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003REISSUE          ", 20);
	//==================================================================================
}


void setPackUbahPinCS(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)16, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 16);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "011PINCHG           ", 20);
	//==================================================================================
}

void setPackUbahPwdSPV(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 23);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001OTO              ", 20);
	//==================================================================================
}

void setPackLogon(void)
{	
	uchar sn[15+1]; // Add Zaki 20 Januari 2014 [32+1]
	int len=0;

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurAcq.szNii);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurAcq.szTermID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurAcq.szMerchantID);
	//==================================================================================

//BIT 63
	//==================================================================================
	ReadSN(sn); // Add Zaki 20 Januari 2014
	memset(glSendPack.szBit63, 0x30, 32);

	PubLong2Char((ulong)30, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "00v2014.1.0.0.4", 15); 
	len = 15-strlen(sn);
	memcpy(glSendPack.szBit63+17+len,  sn, len+1);

	//==================================================================================
}

// ============================= REGISTRASI =============================================== 
void setPackSMSBanking(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)35, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 35);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "012SMSREG           ", 20);
	//==================================================================================
}


void setPackPhoneBanking(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)35, 2, glSendPack.szBit48);
	if( glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT )
		{
		memcpy(&glSendPack.szBit48[2], "1103                               ", 35);
		}
	else if ( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
		memcpy(&glSendPack.szBit48[2], "1109                               ", 35);
		}
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "012PBREG            ", 20);
	//==================================================================================
}

void setPackIBanking(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)35, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 35);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "012IBANKREG         ", 20);
	//==================================================================================
}


void setPackTrxPBTransferInq(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	memset(glSendPack.szBit48, 0x20, 25);
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(glSendPack.szBit48+2, "000", 3);
	memcpy(glSendPack.szBit48+5, glProcInfo.stTranLog.BRI.RegTRXPB.NoRek, strlen(glProcInfo.stTranLog.BRI.RegTRXPB.NoRek));
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001BRI              ", 20);
	//==================================================================================
}


void setPackTrxPB(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	memset(glSendPack.szBit48, 0x20, 37);
	PubLong2Char((ulong)35, 2, glSendPack.szBit48);
	if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TRX_PB_TRANSFER)
	{
		memcpy(glSendPack.szBit48+2, "1", 1);
		memcpy(glSendPack.szBit48+3, glProcInfo.stTranLog.BRI.RegTRXPB.NoRek, strlen(glProcInfo.stTranLog.BRI.RegTRXPB.NoRek));
	}
	else
	{
		memcpy(glSendPack.szBit48+2, "2", 1);
		memcpy(glSendPack.szBit48+3, glProcInfo.stTranLog.BRI.RegTRXPB.NoHP, strlen(glProcInfo.stTranLog.BRI.RegTRXPB.NoHP));
	}
	
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "012PBTRXREG         ", 20);
	//==================================================================================
}

// ============================= MINI ATM BRI ============================================== 
void setPackInfoSaldo(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "000BRI              ", 20);
	//==================================================================================
}


void setPackInfoSaldoBankLain(void)
{
	byte		jenisRekening;
	mapGetByte(traSumberRek, jenisRekening);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	switch(jenisRekening)
		{
		case 1:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "301000");
			break;
		case 2:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "302000");
			break;
		}
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "000INQBLAIN         ", 20);
	//==================================================================================
}


void setPackMiniStatement(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "004MINISTAT         ", 20);
	//==================================================================================
}


void setPackMutasiRekening(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "500MUTASI           ", 20);
	//==================================================================================
}



void setPackInqTransferSesamaBRI(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 23);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
	
// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001BRI              ", 20);
	//==================================================================================
}

void setPackTransferSesamaBRI(void)
{	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================
	
//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================
	
//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 23);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
	
// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001BRI              ", 20);
	//==================================================================================
}


void setPackInqTransferAntarBank(void)
{
	byte jenisRekening;
	mapGetByte(traSumberRek, jenisRekening);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	switch(jenisRekening)
		{
		case 1:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "304000");
			break;
		case 2:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "304020");
			break;
		}
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)39, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 39);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
	
// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003ANTARBANK        ", 20);
	//==================================================================================
}


void setPackTransferAntarBank(void)
{
	byte jenisRekening;
	char bit48Inq[lenBit48+1];
	char bit48Req[76+1];
	char nama[30+1];
	ulong	lenData;

	MEM_ZERO(bit48Inq);
	MEM_ZERO(nama);
	MEM_0x20(bit48Req);

	mapGetByte(traSumberRek, jenisRekening);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Inq, glRecvPack.szBit48+2, lenData);

	memcpy(bit48Req,glProcInfo.szReq48, 23);
	memcpy(bit48Req+23,bit48Inq,30);
	memcpy(bit48Req+60,glProcInfo.szReq48+23,16);

	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	switch(jenisRekening)
		{
		case 1:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "401000");
			break;
		case 2:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "402000");
			break;
		}
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================
	
//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================
	
//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)76, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], bit48Req, 76);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
	
// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "006ANTARBANK        ", 20);
	//==================================================================================
}

void setPackInqTelkom(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)13, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 13);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "200TELMD            ", 20);
	//==================================================================================
}

void setPackTelkom(void)
{
	char		bit48Rsp[999+1];

	MEM_ZERO(bit48Rsp);
	mapGet(traResponseInq, bit48Rsp, 999);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)strlen(bit48Rsp), 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], bit48Rsp, strlen(bit48Rsp));
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "200TELMD            ", 20);
	//==================================================================================
}


void setPackInqPascaBayar(void)
{
	word	mnuItm;
	mapGetWord(traMnuItm, mnuItm);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)13, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 13);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	switch(mnuItm)
		{
		case mnuHalo:
			memcpy(&glSendPack.szBit63[2], "203TELKOMSEL        ", 20);
			break;
		case mnuMatrix:
			memcpy(&glSendPack.szBit63[2], "202INDOSAT          ", 20);
			break;
		}
	//==================================================================================
}


void setPackPascaBayar(void)
{
	word	mnuItm;
	char		bit48Rsp[999+1];
//	char		bit48Req_1[24+1];
//	char		bit48Req_2[954+1];
	char		ReqMatrix[59+1];
	char		buff[80];

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(ReqMatrix);
	MEM_0x20(buff);
	mapGet(traResponseInq, bit48Rsp, 999);
	mapGetWord(traMnuItm, mnuItm);

	memcpy(ReqMatrix,glProcInfo.szReq48,13);
	memcpy(ReqMatrix+13,bit48Rsp+2,46);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)strlen(bit48Rsp), 2, glSendPack.szBit48);
	switch(mnuItm)
		{

		case mnuHalo: 	
			memcpy(&glSendPack.szBit48[2], bit48Rsp, strlen(bit48Rsp)); 
			memcpy(buff, bit48Rsp, 13);
			memcpy(buff+13, bit48Rsp+13, 11);
			memcpy(buff+24, bit48Rsp+54, 12);
			memcpy(buff+36, bit48Rsp+66, 2);
			/*memcpy(bit48Req_1, bit48Rsp, 24); 
			//memcpy(bit48Req_2, bit48Rsp+45, bit48Rsp-45); 
			memcpy(bit48Req_1+24,bit48Rsp+45,strlen(bit48Req_2));
			//strcpy(glProcInfo.szReq48[2],bit48Req_1);
			memcpy(glProcInfo.szReq48,bit48Req_1,strlen(bit48Rsp));*/
			break;
		case mnuMatrix:
			memcpy(buff, glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP, 13);
			memcpy(buff+13, ReqMatrix, 47);
			memcpy(&glSendPack.szBit48[2], buff, 59);
			break;
		}
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	switch(mnuItm)
		{
		case mnuHalo:
			memcpy(&glSendPack.szBit63[2], "203TELKOMSEL        ", 20);
			break;
		case mnuMatrix:
			memcpy(&glSendPack.szBit63[2], "202INDOSAT          ", 20);
			break;
		}
	//==================================================================================
}


void setPackInqPLNPascaBayar(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)12, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 12);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "201PLNPOST          ", 20);
	//==================================================================================
}

void setPackPLNPascaBayar(void)
{
	char		bit48Rsp[999+1];
	char		buffer[999+1];

	MEM_ZERO(buffer);
	MEM_ZERO(bit48Rsp);
	mapGet(traResponseInq, bit48Rsp, 999);

	memcpy(buffer,bit48Rsp,3);
	memcpy(buffer+3,"4",1);
	memcpy(buffer+4,bit48Rsp+3,406);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)410, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], buffer, 410);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "201PLNPOST          ", 20);
	//==================================================================================
}


void setPackInqPLNPraBayar(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)12, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 12);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "201PLNPREPAID       ", 20);
	//==================================================================================
}

void setPackPLNPraBayar(void)
{
	char		bit48Rsp[999+1];
	MEM_ZERO(bit48Rsp);
	mapGet(traResponseInq, bit48Rsp, 999);

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)409, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], bit48Rsp, 409);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "201PLNPREPAID       ", 20);
	//==================================================================================
}

void setPackPLNToken(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)28, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 28);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "201PLNTOKEN         ", 20);
	//==================================================================================
}


void setPackInqKK_KTA(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)16, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 16);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "204BRICC             ", 20);
	//==================================================================================
}


void setPackKK_KTA(void)
{
	word	mnuItm;
	mapGetWord(traMnuItm, mnuItm);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	switch(mnuItm)
		{
		case mnuKKBRI:
			PubLong2Char((ulong)16, 2, glSendPack.szBit48);
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 16);
			break;
		case mnuKKCitibank:
			PubLong2Char((ulong)32, 2, glSendPack.szBit48);
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 32);

			break;
		case mnuKTACitibank:
			PubLong2Char((ulong)16, 2, glSendPack.szBit48);
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 16);
			break;
		case mnuKKStandChat:
		case mnuKTAStandChat:
		case mnuKKANZ:
		case mnuKKHSBC:
		case mnuKTAHSBC:
		case mnuKKAMRO:
		case mnuKTAAMRO:
			PubLong2Char((ulong)17, 2, glSendPack.szBit48);
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 17);
			break;
		}
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	switch(mnuItm)
		{
		case mnuKKBRI:
			memcpy(&glSendPack.szBit63[2], "204BRICC             ", 20);
			break;
		case mnuKKCitibank:
			memcpy(&glSendPack.szBit63[2], "204CTCC             ", 20);
			break;
		case mnuKTACitibank:
			memcpy(&glSendPack.szBit63[2], "204CTPL             ", 20);
			break;
		case mnuKKStandChat:
		case mnuKTAStandChat:
			memcpy(&glSendPack.szBit63[2], "204SCBCC            ", 20);
			break;
		case mnuKKANZ:
			memcpy(&glSendPack.szBit63[2], "204ANZCC            ", 20);
			break;
		case mnuKKHSBC:
		case mnuKTAHSBC:
			memcpy(&glSendPack.szBit63[2], "204HSBCCC           ", 20);
			break;
		case mnuKKAMRO:
		case mnuKTAAMRO:
			memcpy(&glSendPack.szBit63[2], "204RBSCC            ", 20);
			break;
		}
	//==================================================================================
}

void setPackInqCicilan(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)18, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 18);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "205SYB              ", 20);
	//==================================================================================
}

void setPackCicilan(void)
{
	word	mnuItm;
	mapGetWord(traMnuItm, mnuItm);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)18, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 18);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	switch(mnuItm)
		{
		case mnuFIF:
			memcpy(&glSendPack.szBit63[2], "206SYBFIF           ", 20);
			break;
		case mnuBAF:
			memcpy(&glSendPack.szBit63[2], "206SYBBAF           ", 20);
			break;
		case mnuOTO:
			memcpy(&glSendPack.szBit63[2], "206SYBOTO           ", 20);
			break;
		case mnuFinansia:
			memcpy(&glSendPack.szBit63[2], "206SYBFNS           ", 20);
			break;
		case mnuVerena:
			memcpy(&glSendPack.szBit63[2], "206SYBVER           ", 20);
			break;
		case mnuWOM:
			memcpy(&glSendPack.szBit63[2], "206SYBWOM           ", 20);
			break;
		}
	//==================================================================================
}

void setPackPembayaranZakat_Infaq(void)
{
	word mnuItm;
	mapGetWord(traMnuItm, mnuItm);
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	switch(mnuItm)
		{
		case mnuDmptDuafaZ:
		case mnuDmptDuafaI:
			PubLong2Char((ulong)23, 2, glSendPack.szBit48);
			memcpy(&glSendPack.szBit48[2], "001                    ", 23);
			break;
		case mnuYBMZ:
		case mnuYBMI:
			PubLong2Char((ulong)23, 2, glSendPack.szBit48);
			memcpy(&glSendPack.szBit48[2], "002                    ", 23);
			break;
		}
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	switch(mnuItm)
		{
		case mnuDmptDuafaZ:
		case mnuYBMZ:
			PubLong2Char((ulong)20, 2, glSendPack.szBit63);
			memcpy(&glSendPack.szBit63[2], "001ZAKAT            ", 20);	
			break;
		case mnuDmptDuafaI:
		case mnuYBMI:
			PubLong2Char((ulong)20, 2, glSendPack.szBit63);
			memcpy(&glSendPack.szBit63[2], "001INFAQ            ", 20);	
			break;
		}
	//==================================================================================
}

void setPackPembayaranDPLK(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================
	
//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 23);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
	
// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2],  "001DPLK             ", 20);
	//==================================================================================
}

void setPackInqTiketPesawat(void)
{
word	mnuItm;
mapGetWord(traMnuItm, mnuItm);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)13, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 13);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	//==================================================================================
	switch(mnuItm)
		{
		case mnuGaruda:
			memcpy(&glSendPack.szBit63[2], "200GARUDA           ", 20);
			break;
		case mnuLion:
			memcpy(&glSendPack.szBit63[2], "200LION             ", 20);
			break;
		case mnuSriwjy:
			memcpy(&glSendPack.szBit63[2], "200SRIWJY           ", 20); 
			break;
		case mnuMerpati:
			memcpy(&glSendPack.szBit63[2], "200MERPATI          ", 20);
			break;
		}
	//==================================================================================
}

void setPackTiketPesawat(void)
{
	char		bit48Rsp[999+1];
	MEM_ZERO(bit48Rsp);

	mapGet(traResponseInq, bit48Rsp, 999);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)strlen(bit48Rsp), 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2],bit48Rsp, strlen(bit48Rsp));
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "200TICKET           ", 20);
	//==================================================================================
}

void setPackInqBriva(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================
	
//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2],glProcInfo.szReq48, 20);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "500BRIVA            ", 20);
	//==================================================================================
}


void setPackBriva(void)
{
	char		bit48Rsp[999+1];
	MEM_ZERO(bit48Rsp);

	mapGet(traResponseInq, bit48Rsp, 999);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)strlen(bit48Rsp), 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2],bit48Rsp, strlen(bit48Rsp));
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "500BRIVA            ", 20);
	//==================================================================================
}



void setPackInqPembayaranKuliah(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)25, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 25);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "204SPPONLINE        ", 20);
	//==================================================================================
}


void setPackPembayaranKuliah(void)
{
	char		bit48[999+1];
	char		bit48Req[43+1];
	
	MEM_ZERO(bit48);
	MEM_ZERO(bit48Req);

	mapGet(traResponseInq, bit48, 999);

	memcpy(bit48Req,glProcInfo.szReq48+2,3);
	memcpy(bit48Req+3,bit48,20);
	memcpy(bit48Req+23,bit48+40,20);
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================


//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)strlen(bit48Req), 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], bit48Req, strlen(bit48Req));
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "204SPPONLINE        ", 20);
	//==================================================================================
}


void setPackIsiUlang(void)
{
word	mnuItm;
mapGetWord(traMnuItm, mnuItm);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)25, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 25);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	switch(mnuItm)
		{
		case mnuAxis25rb:
   		case mnuAxis50rb:
   		case mnuAxis75rb:
   		case mnuAxis100rb:
   		case mnuAxis150rb:
   		case mnuAxis200rb:
   		case mnuAxis300rb:
			memcpy(&glSendPack.szBit63[2], "300AXIS             ", 20);
			break;
		case mnuEsia25rb:
    		case mnuEsia50rb:
    		case mnuEsia100rb:
    		case mnuEsia150rb:
    		case mnuEsia250rb:
			memcpy(&glSendPack.szBit63[2], "300ESIA             ", 20);
			break;
		case mnuImThree25rb:
    		case mnuImThree50rb:
    		case mnuImThree75rb:
    		case mnuImThree100rb:
    		case mnuImThree150rb:
    		case mnuImThree200rb:
    		case mnuImThree500rb:
    		case mnuImThree1jt:
			memcpy(&glSendPack.szBit63[2], "300IM3SMART         ", 20);
			break;
		case mnuMentari25rb:
    		case mnuMentari50rb:
    		case mnuMentari100rb:
		case mnuMentari150rb:
    		case mnuMentari250rb:
    		case mnuMentari500rb:
    		case mnuMentari1jt:
			memcpy(&glSendPack.szBit63[2], "300MENTARI          ", 20);
			break;
		case mnuSimpati10rb:
		case mnuSimpati20rb:
		case mnuSimpati25rb:
		case mnuSimpati50rb:
    		case mnuSimpati100rb:
    		case mnuSimpati150rb:
    		case mnuSimpati200rb:
    		case mnuSimpati300rb:
    		case mnuSimpati500rb:
    		case mnuSimpati1jt:
			memcpy(&glSendPack.szBit63[2], "300SIMPATI          ", 20);
			break;
		case mnuSmartFren20rb:
		case mnuSmartFren25rb:
    		case mnuSmartFren50rb:
    		case mnuSmartFren100rb:
		case mnuSmartFren150rb:
    		case mnuSmartFren200rb:
		case mnuSmartFren300rb:
		case mnuSmartFren500rb:
			memcpy(&glSendPack.szBit63[2], "300SMART            ", 20);
			break;
		case mnuThree20rb:
   		case mnuThree50rb:
   		case mnuThree75rb:
   		case mnuThree100rb:
   		case mnuThree150rb:
   		case mnuThree300rb:
   		case mnuThree500rb:
			memcpy(&glSendPack.szBit63[2], "300THREE            ", 20);
			break;
		case mnuXLV20rb:
		case mnuXLV25rb:
    		case mnuXLV50rb:
    		case mnuXLV100rb:
    		case mnuXLV200rb:
		case mnuXLV500rb:
			memcpy(&glSendPack.szBit63[2], "300XL               ", 20);
			break;
		}
	//==================================================================================
}



// ============================= ABSENSI ============================================== 
void setPackAbsensi(void)
{
char		bit48Req[24+1];
word	mnuItm;

MEM_0x20(bit48Req);
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	mapGetWord(traMnuItm, mnuItm);
	switch(mnuItm)
		{
		case mnuDatang:
			memcpy(bit48Req,"A",1);
			break;
		case mnuPulang:
			memcpy(bit48Req,"B",1);
			break;
		case mnuIstrhtMulai:
			memcpy(bit48Req,"C",1);
			break;
		case mnuIstrhtSelesai:
			memcpy(bit48Req,"D",1);
			break;
		case mnuLemburDatang:
			memcpy(bit48Req,"E",1);
			break;
		case mnuLemburPulang:
			memcpy(bit48Req,"F",1);
			break;
		case mnuShif1Datang:
			memcpy(bit48Req,"G",1);
			break;
		case mnuShif1Pulang:
			memcpy(bit48Req,"H",1);
			break;
		case mnuShif2Datang:
			memcpy(bit48Req,"I",1);
			break;
		case mnuShif2Pulang:
			memcpy(bit48Req,"J",1);
			break;
		case mnuShif3Datang:
			memcpy(bit48Req,"L",1);
			break;
		case mnuShif3Pulang:
			memcpy(bit48Req,"K",1);
			break;
		}
	PubLong2Char((ulong)24, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2],bit48Req, 24);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "002BRI              ", 20);
	//==================================================================================
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// BRIZZI /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


// 
void setPackBrizziTopUpOnline(void)
{
	uchar Saldo[12+1];
	uchar UID[12+1];
	uchar RandomData[48+1];

	memset(Saldo, 0, sizeof(Saldo));
	memset(UID, 0, sizeof(UID));
	memset(RandomData, 0, sizeof(RandomData));
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================

	memset(glSendPack.szBit48, 0x20, sizeof(glSendPack.szBit48));
	PubLong2Char((ulong)60, 2, glSendPack.szBit48);
	bin2hex(RandomData, glProcInfo.rndKeyBrizzi, 24);
	memcpy(&glSendPack.szBit48[2], RandomData, 48);

	memcpy(Saldo, glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, sizeof(Saldo));
	memcpy(&glSendPack.szBit48[2+48], Saldo+2, 10);
	
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003EMONEY           ", 20);
	//==================================================================================
}


void setPackBrizziAktvDeposit(void)
{
	uchar Saldo[12+1];
	uchar UID[12+1];
	uchar RandomData[48+1];

	memset(Saldo, 0, sizeof(Saldo));
	memset(UID, 0, sizeof(UID));
	memset(RandomData, 0, sizeof(RandomData));
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================

	memset(glSendPack.szBit48, 0x20, 60);
	PubLong2Char((ulong)60, 2, glSendPack.szBit48);
	bin2hex(RandomData, glProcInfo.rndKeyBrizzi, 24);
	memcpy(&glSendPack.szBit48[2], RandomData, 48);

	memcpy(Saldo, glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, sizeof(Saldo));
	memcpy(&glSendPack.szBit48[2+48], Saldo+2, 10);
	
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003EMONEY           ", 20);
	//==================================================================================
}



void setPackBrizziInfoDeposit(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)16, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.stTranLog.BRI.Brizzi.NoKartu, 16);	
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "204EMONEY           ", 20);
	//==================================================================================
}

// 
void setPackBrizziTopUpDeposit(void)
{
	uchar Saldo[12+1];
	uchar UID[12+1];
	uchar RandomData[48+1];

	memset(Saldo, 0, sizeof(Saldo));
	memset(UID, 0, sizeof(UID));
	memset(RandomData, 0, sizeof(RandomData));
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	memset(glSendPack.szBit48, 0x20, sizeof(glSendPack.szBit48));
	PubLong2Char((ulong)16, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.stTranLog.BRI.Brizzi.NoKartu, 16);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "204EMONEY           ", 20);
	//==================================================================================
}

void setPackBrizziVoidReq(void)
{
	uchar TraceNum[6+1];

	memset(TraceNum, 0, sizeof(TraceNum));
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================

	memset(glSendPack.szBit48, 0x20, sizeof(glSendPack.szBit48));
	PubLong2Char((ulong)60, 2, glSendPack.szBit48);
	
	memcpy(&glSendPack.szBit48[2], glProcInfo.stTranLog.szDateTime+2, 12);//YYMMDDhhmmss
	memcpy(&glSendPack.szBit48[14], glTranConfig[glProcInfo.stTranLog.ucOrgTranType].szProcCode, 6);//procode
	memcpy(&glSendPack.szBit48[20], glProcInfo.stTranLog.szAmount+2, 10);//ori amt

	sprintf(TraceNum, "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	memcpy(&glSendPack.szBit48[30], TraceNum, 6);

	sprintf(TraceNum, "%06lu", glCurTlmTmsAcq.ulCurBatchNo);
	memcpy(&glSendPack.szBit48[36], TraceNum+4, 2);

	memcpy(&glSendPack.szBit48[38], glProcInfo.stTranLog.BRI.Brizzi.NoKartu, 16);

	memcpy(&glSendPack.szBit48[54], glProcInfo.stTranLog.BRI.Brizzi.Hash, 8);//ori amt

	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================


// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003EMONEY           ", 20);
	//==================================================================================
}


void setPackBrizziRedeem(void)
{
	uchar Saldo[12+1];
	uchar UID[12+1];
	uchar RandomData[48+1];

	memset(Saldo, 0, sizeof(Saldo));
	memset(UID, 0, sizeof(UID));
	memset(RandomData, 0, sizeof(RandomData));
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================

	memset(glSendPack.szBit48, 0x20, 60);
	PubLong2Char((ulong)26, 2, glSendPack.szBit48);
	memcpy(glSendPack.szBit48+2, glProcInfo.stTranLog.BRI.Brizzi.NoKartu, 16);

	memcpy(glSendPack.szBit48+18, glProcInfo.stTranLog.szAmount+2, 10);
	
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "204EMONEY           ", 20);
	//==================================================================================
}



void setPackBrizziSettlement(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================

//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char(strlen(glProcInfo.ucDataSettlement), 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.ucDataSettlement, strlen(glProcInfo.ucDataSettlement));	
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================


// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "500BRIZZI           ", 20);
	//==================================================================================
}


void setPackBrizziReaktivasi(uchar *LastTrxDate)
{
	uchar Saldo[12+1];
	uchar UID[12+1];
	uchar RandomData[48+1];
	uchar lasttrxdate[6+1];

	memset(Saldo, 0, sizeof(Saldo));
	memset(UID, 0, sizeof(UID));
	memset(RandomData, 0, sizeof(RandomData));
	memset(lasttrxdate, 0, sizeof(lasttrxdate));
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================


//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================

	memset(glSendPack.szBit48, 0x20, 60);
	PubLong2Char((ulong)32, 2, glSendPack.szBit48);
	
	memcpy(&glSendPack.szBit48[2], glProcInfo.stTranLog.BRI.Brizzi.NoKartu, 16);

	memcpy(Saldo, glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, sizeof(Saldo));
	memcpy(&glSendPack.szBit48[2+16], Saldo, 10);

	bin2hex(lasttrxdate, LastTrxDate, 3);
	memcpy(&glSendPack.szBit48[2+26], lasttrxdate+4, 2);
	memcpy(&glSendPack.szBit48[2+28], lasttrxdate+2, 2);
	memcpy(&glSendPack.szBit48[2+30], lasttrxdate, 2);

	

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003EMONEY           ", 20);
	//==================================================================================
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// BRIZZI /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////



void setPackTarikTunai(void)
{
	word mnuItm;
	mapGetWord(traMnuItm, mnuItm);
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)46, 2, glSendPack.szBit48);
	memset(glSendPack.szBit48+2, 0x20, 46);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "000BRI              ", 20);	
	//==================================================================================
}


void setPackSetorTunaiInq(void)
{
	word mnuItm;
	mapGetWord(traMnuItm, mnuItm);
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)46, 2, glSendPack.szBit48);
	memcpy(glSendPack.szBit48+2, glProcInfo.szReq48, 23);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001BRI              ", 20);	
	//==================================================================================
}


void setPackSetorTunai(void)
{
	word mnuItm;
	mapGetWord(traMnuItm, mnuItm);
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)46, 2, glSendPack.szBit48);
	memcpy(glSendPack.szBit48+2, glProcInfo.szReq48, 23);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001BRI              ", 20);	
	//==================================================================================
}

void setPackVoidTarikTunai(void)
{
	word mnuItm;
	mapGetWord(traMnuItm, mnuItm);
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)46, 2, glSendPack.szBit48);
	memset(glSendPack.szBit48+2, 0x20, 46);
	memcpy(glSendPack.szBit48+2, glProcInfo.stTranLog.BRI.Tunai.Saldo, 16);
	//==================================================================================
	
//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "000BRI              ", 20);	
	//==================================================================================
}


void setPackCashIn(void)
{

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================
	
//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)60, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 60);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003BRI              ", 20);
	//==================================================================================
}

void setPackInqCashOut(void)
{

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================
	
//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)60, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "002BRI        ", 60);
	//==================================================================================
}

void setPackCashOut(void)
{

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================
	
//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35,
	"%.*s", LEN_TRACK2, "9999999999999999=10010000000000000000");
	//"x\39x\39x\39x\39x\39x\39x\39x\39x\39x\39x\39x\39x\39x\39x\39x\39x\3Dx\31x\30x\30x\31x\30x\30x\30x\30x\30x\30x\30x\30x\30x\30x\30x\30x\30x\30x\30x\30");
			//sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szReq35);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)60, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 60);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "003BRI              ", 20);
	//==================================================================================
}

void SetPackSetorPasti(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2],"@POSBRI#", 8);
	//==================================================================================

// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	if(glProcInfo.stTranLog.BRI.ucflagDebitBri)
		memcpy(&glSendPack.szBit63[2], "006BRI              ", 20);
	else
		memcpy(&glSendPack.szBit63[2], "003ANTARBANK        ", 20);	
	//==================================================================================
}

void setPackSSB(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

//BIT 48
	//==================================================================================
	PubLong2Char((ulong)23, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 23);
	memcpy(&glSendPack.szBit48[14], glProcInfo.stTranLog.BRI.Ssb.KodeSSB, 2);
	//==================================================================================
	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 57
	//==================================================================================
	PubLong2Char((ulong)8, 2, glSendPack.szBit57);
	memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	//==================================================================================

//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
	
// BIT 63
	//==================================================================================
	PubLong2Char((ulong)20, 2, glSendPack.szBit63);
	memcpy(&glSendPack.szBit63[2], "001POLRI            ", 20);
	//==================================================================================
}
#endif


// DIKI ADD FOR MINI ATM BUKOPIN

void setPackInqTransferAntarBankBkpn(void)
{
	byte jenisRekening;
	mapGetByte(traSumberRek, jenisRekening);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	switch(jenisRekening)
		{
		case 1:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "010050");
			break;
		case 2:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "120000");
			break;
		}
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		////memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

/***********
//BIT 48
	//==================================================================================
	PubLong2Char((ulong)39, 2, glSendPack.szBit48);
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 39);
	//==================================================================================
************/
	////PubLong2Char((ulong)12, 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103, 147);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));

	//PubLong2Char((ulong)147, 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,147);

	//memcpy(&glSendPack.szBit48[2], "4848",4);

	/*
	switch(jenisRekening)
		{
		case 1:
			//strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "010050");
			memcpy(glSendPack.szBit48[2], "456         ", 12);  // 
			break;
		case 2:
			memcpy(glSendPack.szBit48[2], "123         ", 12);  //  
			break;
		}
	
	*/

	PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
	

//BIT 49
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	////uchar kodecur[3+1];
	////strcpy(kodecur,"360");
	////sprintf((char *)glSendPack.szBit49, "%.3s", kodecur);
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 55
		//==================================================================================
		/*
#ifdef ENABLE_EMV
			else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
				(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
			{
				SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
				PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
				memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);
				glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
			}
#endif
*/
		//==================================================================================


//BIT 103
	//==================================================================================
	////PubLong2Char((ulong)28, 2, glSendPack.szBit103);
	////memcpy(&glSendPack.szBit103[2], glProcInfo.szReq103, 28);
	//szBit48
///	PubLong2Char((ulong)28, 2, glSendPack.szBit48);
////	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103, 28);
	//==================================================================================
		



//BIT 103, 
	//==================================================================================
	//	memcpy(glSendPack.szField102, "\x00\x08", 2);
	//sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//sprintf((char *)glSendPack.szField102,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);

	//sprintf((char *)glSendPack.szField103,     "%.*s", 28,      glProcInfo.szReq103);
			

	PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	//==================================================================================\

//BIT 103, 
	//==================================================================================
//	memcpy(glSendPack.szField103, "\x00\x06", 2);
	//sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);

	//PubLong2Char((ulong)20, 2, glSendPack.szBit63);
//	memcpy(&glSendPack.szField103[2], "123456", 6);
	//==================================================================================

	




}


void setPackTransferAntarBankBkpn(void)
{
	byte jenisRekening;
	mapGetByte(traSumberRek, jenisRekening);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	switch(jenisRekening)
		{
		case 1:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "110000");
			break;
		case 2:
			strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "130000");
			break;
		}
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		//memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 48
	//==================================================================================
	////PubLong2Char((ulong)39, 2, glSendPack.szBit48);
	////memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 39);

	PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
	//==================================================================================


//	PubLong2Char((ulong)28, 2, glSendPack.szBit48);
//	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103, 28);
	

//BIT 49
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	////uchar kodecur[3+1];
	////strcpy(kodecur,"360");
	////sprintf((char *)glSendPack.szBit49, "%.3s", kodecur);
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 55
		//==================================================================================
		/*
#ifdef ENABLE_EMV
			else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
				(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
			{
				SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
				PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
				memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);
				glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
			}
#endif
*/
		//==================================================================================


// BIT 102
	PubLong2Char((ulong)strlen(glProcInfo.szReq102), 2, glSendPack.szField102);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	memcpy(&glSendPack.szField102[2], glProcInfo.szReq102,strlen(glProcInfo.szReq102));
		

//BIT 103
	//==================================================================================
	////PubLong2Char((ulong)28, 2, glSendPack.szBit103);
	////memcpy(&glSendPack.szBit103[2], glProcInfo.szReq103, 28);
	//szBit48
///	PubLong2Char((ulong)28, 2, glSendPack.szBit48);
////	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103, 28);
	//==================================================================================
		



//BIT 103, 
	//==================================================================================
	//	memcpy(glSendPack.szField102, "\x00\x08", 2);
	//sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//sprintf((char *)glSendPack.szField102,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);

	//sprintf((char *)glSendPack.szField103,     "%.*s", 28,      glProcInfo.szReq103);

///	PubLong2Char((ulong)28, 2, glSendPack.szField103);
///	memcpy(&glSendPack.szField103[2], glProcInfo.szReq103, 28);
	//==================================================================================\

//BIT 103, 
	//==================================================================================
//	memcpy(glSendPack.szField103, "\x00\x06", 2);
	//sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);

	//PubLong2Char((ulong)20, 2, glSendPack.szBit63);
//	memcpy(&glSendPack.szField103[2], "123456", 6);
	//==================================================================================

	PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));




}


// add diki for private label bukopin, no bit 55

void SetPackSalePrvtLbl(void)
{
#ifdef ENABLE_EMV
	int	iLength;
#endif
	uchar	szTotalAmt[12+1];
	uchar 	ucBufAmt[12+1];

//	hex_dump_char("SINI", &glProcInfo.stTranLog.uiEntryMode, sizeof(glProcInfo.stTranLog.uiEntryMode));

	MEM_ZERO(ucBufAmt);
	MEM_ZERO(szTotalAmt);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
////	sprintf((char *)glSendPack.szBit2,	   "%.*s", LEN_PAN, 	 glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	//==================================================================================

// BIT 4
	//==================================================================================
	if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//		strcpy(ucBufAmt, szTotalAmt);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
		sprintf((char *)glSendPack.szBit4,   "%.*s", LEN_TRAN_AMT,   szTotalAmt);
#ifndef	BANK_MANDIRI
		sprintf((char *)glSendPack.szBit54, "%.*s", LEN_EXT_AMOUNT, glProcInfo.stTranLog.szTipAmount);
#endif
	}
	else
	{
//		strcpy(ucBufAmt, glProcInfo.stTranLog.szAmount);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
	}
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	//SetPosEntryMode();
	memcpy(&glSendPack.szBit22[0], "0051", 4);
	//==================================================================================

//BIT 23
	//==================================================================================	
	/*
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
	{	
		if( glProcInfo.stTranLog.bPanSeqOK )
		{	
			sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
		}
	}
	*/
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================

/*
// PAN/track 1/2/3/expiry etc
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		if(glProcInfo.szTrack2[0]==0)
		{
	//BIT 2
	//==================================================================================
			sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================
	
	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
		}
		else
		{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT) ||
		(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) )
	{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
	//BIT 2
	//==================================================================================
		sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
	}
	*/

//BIT 2
		//==================================================================================
////				sprintf((char *)glSendPack.szBit2,	   "%.*s", LEN_PAN, 	 glProcInfo.stTranLog.szPan);
		//==================================================================================


//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================



	
//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//	sprintf((char *)glSendPack.szBit42, "%-15s", glCurTlmTmsAcq.acqMID+5);
	//==================================================================================

//BIT 48
	//==================================================================================
	if(BIT48ON)
	{
		PubLong2Char((ulong)1, 2, glSendPack.szBit48);
		memcpy(&glSendPack.szBit48[2], "1", 1);
	}
	//==================================================================================

//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT)
	{
		CalculatePinBlock(glProcInfo.sPinBlock);
	
		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

	
//BIT 55
	//==================================================================================
	/*
#ifdef ENABLE_EMV
		else if( (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
			(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
		{
			SetDE55(DE55_SALE, &glSendPack.szBit55[2], &iLength);
			PubLong2Char((ulong)iLength, 2, glSendPack.szBit55);
			memcpy(glProcInfo.stTranLog.sIccData, &glSendPack.szBit55[2], iLength);
			glProcInfo.stTranLog.uiIccDataLen = (ushort)iLength;
		}
#endif
*/
	//==================================================================================

//BIT 57
#ifdef BANK_BRI
	if(ChkIfBRIDebit())
	{	
		PubLong2Char((ulong)8, 2, glSendPack.szBit57);
		memcpy(&glSendPack.szBit57[2], "@POSBRI#", 8);
	}
#endif

	//BIT 62, ROC/SOC
	//==================================================================================
	#ifndef BANK_PANIN
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	#else

	//uchar SerialNo[99];
	//ReadSN(SerialNo);
	//mapGet(appTid,SerialNo,8);
	//PrnInit();
	//PrnStr("SerialNo : %s",SerialNo);


	#endif
	//==================================================================================
}


void SetPackCicilanBukopinPrvtLbl(void)
{
#ifdef ENABLE_EMV
	int	iLength;
#endif
	uchar	szTotalAmt[12+1];
	uchar 	ucBufAmt[12+1];

	MEM_ZERO(ucBufAmt);
	MEM_ZERO(szTotalAmt);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

// BIT 4
	//==================================================================================
	if( !ChkIfZeroAmt(glProcInfo.stTranLog.szTipAmount) )
	{
		PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTotalAmt);
//		strcpy(ucBufAmt, szTotalAmt);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
		sprintf((char *)glSendPack.szBit4,   "%.*s", LEN_TRAN_AMT,   szTotalAmt);
#ifndef	BANK_MANDIRI
		sprintf((char *)glSendPack.szBit54, "%.*s", LEN_EXT_AMOUNT, glProcInfo.stTranLog.szTipAmount);
#endif
	}
	else
	{
//		strcpy(ucBufAmt, glProcInfo.stTranLog.szAmount);
//		PubAscMul(ucBufAmt, "100", ucBufAmt);
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
//		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, ucBufAmt);
	}
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 22
	//==================================================================================
	/////SetPosEntryMode();
	//==================================================================================
	//BIT 22
		//==================================================================================
		//SetPosEntryMode();
		memcpy(&glSendPack.szBit22[0], "0051", 4);
		//==================================================================================


//BIT 23
	//==================================================================================
	/*
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT)
	{	
		if( glProcInfo.stTranLog.bPanSeqOK )
		{	
			sprintf((char *)glSendPack.szBit23, "%0*X", LEN_PAN_SEQ_NO, glProcInfo.stTranLog.ucPanSeqNo);
		}
	}
	*/
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 25
	//==================================================================================
	sprintf((char *)glProcInfo.stTranLog.szCondCode, "00");
	sprintf((char *)glSendPack.szBit25, "%.2s", glProcInfo.stTranLog.szCondCode);
	//==================================================================================


//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================

/*
// PAN/track 1/2/3/expiry etc
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
	{
		if(glProcInfo.szTrack2[0]==0)
		{
	//BIT 2
	//==================================================================================
			sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================
	
	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
		}
		else
		{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35,  "%.*s", LEN_TRACK2,   glProcInfo.szTrack2);
	//==================================================================================
		}
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_SWIPE_INPUT) ||
		(glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_SWIPE) )
	{
	//BIT 35
	//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================
	}
	else if( (glProcInfo.stTranLog.uiEntryMode & MODE_MANUAL_INPUT) ||
		 (glProcInfo.stTranLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
	//BIT 2
	//==================================================================================
		sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

	//BIT 14
	//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================
	}
	*/

	
//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


/*
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================
*/

	//BIT 52
		//==================================================================================
		if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT)
		{
			CalculatePinBlock(glProcInfo.sPinBlock);
		
			PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
			memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
		}
		//==================================================================================


	

//BIT 61
	//#undef BANK_HSBC
	//==================================================================================
	#ifdef BANK_PANIN
	PubLong2Char((ulong)13, 2, glSendPack.szBit61);
	memcpy(&glSendPack.szBit61[2], glProcInfo.szReq61CicilanBukopin, 13);
	#else
		#ifdef BANK_HSBC
		#else
		PubLong2Char((ulong)3, 2, glSendPack.szBit61);
		memcpy(&glSendPack.szBit61[2], glProcInfo.szReq61CicilanBukopin, 3);
		#endif
	#endif
	//==================================================================================
	
//BIT 62, ROC/SOC
	//==================================================================================
	memcpy(glSendPack.szBit62, "\x00\x06", 2);
	sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//==================================================================================
}


void setPackIsiUlangBkpn(void)
{
word	mnuItm;
mapGetWord(traMnuItm, mnuItm);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "110074");
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
	glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		////memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================
	//BIT 49
		//==================================================================================
		///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
		////uchar kodecur[3+1];
		////strcpy(kodecur,"360");
		////sprintf((char *)glSendPack.szBit49, "%.3s", kodecur);
		memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
		//==================================================================================
	
		
	//BIT 52
		//==================================================================================
		if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
		{
			//		CalculatePinBlock(glProcInfo.sPinBlock);
	
			PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
			memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
		}
		//==================================================================================
	

	
	
	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));

		///PubLong2Char((ulong)25, 2, glSendPack.szBit48);
		///memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 25);
		//==================================================================================\
		}
	//==================================================================================
}



void setPackInqMultifinanceBkpn(void)
{
	word	mnuItm;
	mapGetWord(traMnuItm, mnuItm);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

	//BIT 2
		//==================================================================================
		sprintf((char *)glSendPack.szBit2,	   "%.*s", LEN_PAN, 	 glProcInfo.stTranLog.szPan);
		//==================================================================================


//BIT 3
	//==================================================================================
	strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "010078");
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
	glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

	//BIT 4
			//==================================================================================
			////sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
			memcpy(glSendPack.szBit4, "000000000000", 12);	// saat inqueri amount = 0 
			//==================================================================================
	
	//BIT 7
			//==================================================================================
			sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
			//==================================================================================
	
	//BIT 11
		//==================================================================================
		sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
		glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
		//==================================================================================
	
	//BIT 12
		//==================================================================================
		sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
		//==================================================================================
		
	//BIT 13
		//==================================================================================
		sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================
	
	//BIT 14
		//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
		//==================================================================================
	
	//BIT 15
		//==================================================================================
		sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================
	
	//BIT 18
		//==================================================================================
		memcpy(glSendPack.szBit18, "6011", 4);;
		//==================================================================================
	
	//BIT 22
		//==================================================================================
		SetPosEntryMode();
		//==================================================================================
	
	//BIT 24
		//==================================================================================
		sprintf((char *)glSendPack.szBit24, 	   "%.*s", LEN_NII, 		glCurTlmTmsAcq.acqNII);
		//==================================================================================
	
	//BIT 35
		//==================================================================================
		sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
		//==================================================================================
	
	//BIT 37
		//==================================================================================
		///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
		///uchar rrninq[12+1];
		///strcpy(kodecur," 		   ");
		///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
		memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
		//==================================================================================
		
	
	//BIT 41
		//==================================================================================
		sprintf((char *)glSendPack.szBit41, 	"%.*s", LEN_TERM_ID,	 glCurTlmTmsAcq.acqTID);
		//==================================================================================
	
	//BIT 42
		//==================================================================================
		sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
		//==================================================================================
	//BIT 49
			//==================================================================================
			///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
			////uchar kodecur[3+1];
			////strcpy(kodecur,"360");
			////sprintf((char *)glSendPack.szBit49, "%.3s", kodecur);
			memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
			//==================================================================================
		
			
		//BIT 52
			//==================================================================================
			if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
			{
				//		CalculatePinBlock(glProcInfo.sPinBlock);
		
				PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
				memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
			}
			//==================================================================================

	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
		


}

void setPackMultifinanceBkpn(void)
{
	word	mnuItm;
	mapGetWord(traMnuItm, mnuItm);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

	//BIT 2
		//==================================================================================
		sprintf((char *)glSendPack.szBit2,	   "%.*s", LEN_PAN, 	 glProcInfo.stTranLog.szPan);
		//==================================================================================


//BIT 3
	//==================================================================================
	strcpy(glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode, "110078");
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
	glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

	//BIT 4
			//==================================================================================
			sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
			////memcpy(glSendPack.szBit4, "000000000000", 12);	// saat inqueri amount = 0 
			//==================================================================================
	
	//BIT 7
			//==================================================================================
			sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
			//==================================================================================
	
	//BIT 11
		//==================================================================================
		sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
		glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
		//==================================================================================
	
	//BIT 12
		//==================================================================================
		sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
		//==================================================================================
		
	//BIT 13
		//==================================================================================
		sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================
	
	//BIT 14
		//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
		//==================================================================================
	
	//BIT 15
		//==================================================================================
		sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================
	
	//BIT 18
		//==================================================================================
		memcpy(glSendPack.szBit18, "6011", 4);;
		//==================================================================================
	
	//BIT 22
		//==================================================================================
		SetPosEntryMode();
		//==================================================================================
	
	//BIT 24
		//==================================================================================
		sprintf((char *)glSendPack.szBit24, 	   "%.*s", LEN_NII, 		glCurTlmTmsAcq.acqNII);
		//==================================================================================
	
	//BIT 35
		//==================================================================================
		sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
		//==================================================================================
	
	//BIT 37
		//==================================================================================
		///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
		///uchar rrninq[12+1];
		///strcpy(kodecur," 		   ");
		///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
		memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
		//==================================================================================
		
	
	//BIT 41
		//==================================================================================
		sprintf((char *)glSendPack.szBit41, 	"%.*s", LEN_TERM_ID,	 glCurTlmTmsAcq.acqTID);
		//==================================================================================
	
	//BIT 42
		//==================================================================================
		sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
		//==================================================================================

	//BIT 48
			//==================================================================================
			PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
			//========================================


	//BIT 49
			//==================================================================================
			///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
			////uchar kodecur[3+1];
			////strcpy(kodecur,"360");
			////sprintf((char *)glSendPack.szBit49, "%.3s", kodecur);
			memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
			//==================================================================================
		
			
		//BIT 52
			//==================================================================================
			if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
			{
				//		CalculatePinBlock(glProcInfo.sPinBlock);
		
				PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
				memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
			}


			//==================================================================================

		// BIT 102
			PubLong2Char((ulong)strlen(glProcInfo.szReq102), 2, glSendPack.szField102);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szField102[2], glProcInfo.szReq102,strlen(glProcInfo.szReq102));
			//=========================================================================




	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
		


}




void setPackTarikTunaiBkpnInq(void)
{
	word mnuItm;
	mapGetWord(traMnuItm, mnuItm);
	
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

	//BIT 2
		//==================================================================================
		sprintf((char *)glSendPack.szBit2,	   "%.*s", LEN_PAN, 	 glProcInfo.stTranLog.szPan);
		//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 4
	//==================================================================================
	sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
	//==================================================================================

	//BIT 7
			//==================================================================================
			sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
			//===================================================================
			

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

	//BIT 14
		//==================================================================================
		sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
		//==================================================================================
	
	//BIT 15
		//==================================================================================
		sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================
	
	//BIT 18
		//==================================================================================
		memcpy(glSendPack.szBit18, "6011", 4);;
		//==================================================================

		
		//BIT 22
			//==================================================================================
			SetPosEntryMode();
			//==================================================================================
		
		//BIT 24
			//==================================================================================
			sprintf((char *)glSendPack.szBit24, 	   "%.*s", LEN_NII, 		glCurTlmTmsAcq.acqNII);
			//==================================================================================
		
		//BIT 35
			//==================================================================================
			sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
			//==================================================================================
		
		//BIT 37
			//==================================================================================
			///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
			///uchar rrninq[12+1];
			///strcpy(kodecur," 		   ");
			///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
			memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
			//==================================================================================


			
			//BIT 41
				//==================================================================================
				sprintf((char *)glSendPack.szBit41, 	"%.*s", LEN_TERM_ID,	 glCurTlmTmsAcq.acqTID);
				//==================================================================================
			
			//BIT 42
				//==================================================================================
				sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
				//==================================================================================
						
			//BIT 49
				//==================================================================================
				memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
				//==================================================================================
			
				
			//BIT 52
				//==================================================================================
				if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
				{
					//		CalculatePinBlock(glProcInfo.sPinBlock);
			
					PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
					memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
				}
				//==================================================================================
		
			
			//BIT 103, 
				//==================================================================================
			
			///	PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
				//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			///	memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
				//==================================================================================\
			

	
	//==================================================================================
}






void setPackInfoSaldoBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==========================================================================

//BIT 4
		//==================================================================================
		//sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================




}






void setPackUbahPinBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==========================================================================

//BIT 4
		//==================================================================================
		//sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================


	//BIT 63  // pin baru
		//==================================================================================
		PubLong2Char((ulong)16, 2, glSendPack.szBit63);
		memcpy(&glSendPack.szBit63[2], glProcInfo.szReq48, 16);
		//===================================================



}



void setPackInqKKBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			//glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	memcpy(glSendPack.szBit3, "010079", 6);
	//==========================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		//memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

//BIT 103, 
		//==================================================================================
	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//===========================


}



void setPackKKBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			//glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	memcpy(glSendPack.szBit3, "110079", 6);
	//==========================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		//memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


	//BIT 48
			//==================================================================================
			PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
			//========================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

		// BIT 102
			PubLong2Char((ulong)strlen(glProcInfo.szReq102), 2, glSendPack.szField102);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szField102[2], glProcInfo.szReq102,strlen(glProcInfo.szReq102));
			//=========================================================================




	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
	

}



void setPackInqTelkomBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			//glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	memcpy(glSendPack.szBit3, "010072", 6);
	//==========================================================================

//BIT 4
		//==================================================================================
		//sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================


	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
	

}




void setPackTelkomBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			//glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	memcpy(glSendPack.szBit3, "110072", 6);
	//==========================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		//memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


	//BIT 48
			//==================================================================================
			PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
			//========================================
			


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

		// BIT 102
			PubLong2Char((ulong)strlen(glProcInfo.szReq102), 2, glSendPack.szField102);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szField102[2], glProcInfo.szReq102,strlen(glProcInfo.szReq102));
			//=========================================================================




	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
	

}




void setPackInqSelularBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			//glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	memcpy(glSendPack.szBit3, "010073", 6);
	//==========================================================================

//BIT 4
		//==================================================================================
		//sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================


	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
	

}




void setPackSelularBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			//glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	memcpy(glSendPack.szBit3, "110073", 6);
	//==========================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		//memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


	//BIT 48
			//==================================================================================
			PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
			//========================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

		// BIT 102
			PubLong2Char((ulong)strlen(glProcInfo.szReq102), 2, glSendPack.szField102);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szField102[2], glProcInfo.szReq102,strlen(glProcInfo.szReq102));
			//=========================================================================




	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
	

}




void setPackInqPdamBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			//glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	memcpy(glSendPack.szBit3, "010077", 6);
	//==========================================================================

//BIT 4
		//==================================================================================
		//sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================


	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
	

}




void setPackPdamBkpn(void)
{
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			//glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	memcpy(glSendPack.szBit3, "110077", 6);
	//==========================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		//memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


	//BIT 48
			//==================================================================================
			PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
			//========================================


//BIT 49
	//==================================================================================
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================

		// BIT 102
			PubLong2Char((ulong)strlen(glProcInfo.szReq102), 2, glSendPack.szField102);
			//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
			memcpy(&glSendPack.szField102[2], glProcInfo.szReq102,strlen(glProcInfo.szReq102));
			//=========================================================================




	//BIT 103, 
		//==================================================================================	
		PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
		//==================================================================
	

}


void setPackInqSetorTunaiBankBkpn(void)
{
	byte jenisRekening;
	mapGetByte(traSumberRek, jenisRekening);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
	//		glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	memcpy(glSendPack.szBit3, "010050", 6);
	//==================================================================================

//BIT 4
		//==================================================================================
		//sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================

	

//BIT 49
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	////uchar kodecur[3+1];
	////strcpy(kodecur,"360");
	////sprintf((char *)glSendPack.szBit49, "%.3s", kodecur);
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================




//BIT 103, 
	//==================================================================================
	//	memcpy(glSendPack.szField102, "\x00\x08", 2);
	//sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//sprintf((char *)glSendPack.szField102,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);

	//sprintf((char *)glSendPack.szField103,     "%.*s", 28,      glProcInfo.szReq103);
			

	PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	//==================================================================================
	//==================================================================================

}


void setPackSetorTunaiBankBkpn(void)
{
	byte jenisRekening;
	mapGetByte(traSumberRek, jenisRekening);
//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 2
	//==================================================================================
	sprintf((char *)glSendPack.szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
	//==================================================================================

//BIT 3
	//==================================================================================
	//sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
	//		glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	memcpy(glSendPack.szBit3, "070000", 6);

	//==================================================================================

//BIT 4
		//==================================================================================
		sprintf((char *)glSendPack.szBit4, "%.*s", LEN_TRAN_AMT, glProcInfo.stTranLog.szAmount);
		//memcpy(glSendPack.szBit4, "000000000000", 12);  // saat inqueri amount = 0 
		//==================================================================================

//BIT 7
		//==================================================================================
		sprintf((char *)glSendPack.szBit7, "%.*s", 10, glProcInfo.stTranLog.szDateTime+4);
		//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================

//BIT 12
	//==================================================================================
	sprintf((char *)glSendPack.szBit12, "%.*s", 6, glProcInfo.stTranLog.szDateTime+8);
	//==================================================================================
	
//BIT 13
	//==================================================================================
	sprintf((char *)glSendPack.szBit13, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 14
	//==================================================================================
	sprintf((char *)glSendPack.szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
	//==================================================================================

//BIT 15
	//==================================================================================
	sprintf((char *)glSendPack.szBit15, "%.*s", 4, glProcInfo.stTranLog.szDateTime+4);
	//==================================================================================

//BIT 18
	//==================================================================================
	memcpy(glSendPack.szBit18, "6011", 4);;
	//==================================================================================

//BIT 22
	//==================================================================================
	SetPosEntryMode();
	//==================================================================================

//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//==================================================================================

//BIT 35
	//==================================================================================
	sprintf((char *)glSendPack.szBit35, "%.*s", LEN_TRACK2, glProcInfo.szTrack2);
	//==================================================================================

//BIT 37
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	///uchar rrninq[12+1];
	///strcpy(kodecur,"            ");
	///sprintf((char *)glSendPack.szBit37, "%.12s", rrninq);
	memcpy(glSendPack.szBit37, "            ", 12);  //   RRN AWAL
	//==================================================================================
	

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//==================================================================================

//BIT 42
	//==================================================================================
	sprintf((char *)glSendPack.szBit42, "%.*s", LEN_MERCHANT_ID, glCurTlmTmsAcq.acqMID);
	//==================================================================================


//BIT 48
	//==================================================================================
	////PubLong2Char((ulong)39, 2, glSendPack.szBit48);
	////memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48, 39);

	///PubLong2Char((ulong)strlen(glProcInfo.szReq48), 2, glSendPack.szBit48);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	///memcpy(&glSendPack.szBit48[2], glProcInfo.szReq48,strlen(glProcInfo.szReq48));
	//==================================================================================


//	PubLong2Char((ulong)28, 2, glSendPack.szBit48);
//	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103, 28);
	

//BIT 49
	//==================================================================================
	///sprintf((char *)glSendPack.szBit37, "%.12s", glProcInfo.stTranLog.szRRN);
	////uchar kodecur[3+1];
	////strcpy(kodecur,"360");
	////sprintf((char *)glSendPack.szBit49, "%.3s", kodecur);
	memcpy(glSendPack.szBit49, "360", 3);  // IDR Currency 360
	//==================================================================================

	
//BIT 52
	//==================================================================================
	if( glProcInfo.stTranLog.uiEntryMode & MODE_PIN_INPUT )
	{
		//		CalculatePinBlock(glProcInfo.sPinBlock);

		PubLong2Char((ulong)LEN_PIN_DATA, 2, glSendPack.szBit52);
		memcpy(&glSendPack.szBit52[2], glProcInfo.sPinBlock, LEN_PIN_DATA);
	}
	//==================================================================================


// BIT 102
	////PubLong2Char((ulong)strlen(glProcInfo.szReq102), 2, glSendPack.szField102);
		//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	///memcpy(&glSendPack.szField102[2], glProcInfo.szReq102,strlen(glProcInfo.szReq102));
		

//BIT 103
	//==================================================================================
	////PubLong2Char((ulong)28, 2, glSendPack.szBit103);
	////memcpy(&glSendPack.szBit103[2], glProcInfo.szReq103, 28);
	//szBit48
///	PubLong2Char((ulong)28, 2, glSendPack.szBit48);
////	memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103, 28);
	//==================================================================================
		



//BIT 103, 
	//==================================================================================
	//	memcpy(glSendPack.szField102, "\x00\x08", 2);
	//sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);
	//sprintf((char *)glSendPack.szField102,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);

	//sprintf((char *)glSendPack.szField103,     "%.*s", 28,      glProcInfo.szReq103);

///	PubLong2Char((ulong)28, 2, glSendPack.szField103);
///	memcpy(&glSendPack.szField103[2], glProcInfo.szReq103, 28);
	//==================================================================================\

//BIT 103, 
	//==================================================================================
//	memcpy(glSendPack.szField103, "\x00\x06", 2);
	//sprintf((char *)&(glSendPack.szBit62[2]), "%06lu", glProcInfo.stTranLog.ulInvoiceNo);

	//PubLong2Char((ulong)20, 2, glSendPack.szBit63);
//	memcpy(&glSendPack.szField103[2], "123456", 6);
	//==================================================================================

	PubLong2Char((ulong)strlen(glProcInfo.szReq103), 2, glSendPack.szField103);
	//memcpy(&glSendPack.szBit48[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));
	memcpy(&glSendPack.szField103[2], glProcInfo.szReq103,strlen(glProcInfo.szReq103));




}


