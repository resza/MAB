#include "global.h"

int TransSale(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	iRet = TransInit(TRANS_TYPE_SALE);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glTlmSysParam.Con.appEMSale & EM_CHIP_INITIAL)
		ucEntryMode |= CARD_INSERTED;
	if(glTlmSysParam.Con.appEMSale & EM_MAGNETIC_STRIPE_INITIAL)
		ucEntryMode |= CARD_SWIPED;
	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
		ucEntryMode |= CARD_KEYIN;
	
	if(!ucEntryMode)
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

		
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransSubSaleNew();
	
}


int TransPurchase(void) //for Artha Jasa
{
	int		iRet;
	uchar	ucEntryMode=0;

	iRet = TransInit(TRANS_TYPE_INQ_PURCHASE);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glTlmSysParam.Con.appEMSale & EM_CHIP_INITIAL)
		ucEntryMode |= CARD_INSERTED;
	if(glTlmSysParam.Con.appEMSale & EM_MAGNETIC_STRIPE_INITIAL)
		ucEntryMode |= CARD_SWIPED;
	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
		ucEntryMode |= CARD_KEYIN;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}
	
		//CHECK BATCH
		if(!ChkIfBatchFull())
		{
			return ERR_NO_DISP;
		}
	
		//CHECK TRANS ALLOW
		if(!CheckTransAllowed())
		{
			return ERR_NO_DISP;
		}
	
		//CHECK PENDING SETTLEMENT
		if( !ChkSettle() )	//check pending settlement
		{
			return ERR_NO_DISP;
		}

	return TransPurchaseSub();
	
}

int TransSaleSub(void)
{
	int		iRet;

	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();


	if(PrivateLabelCardDetect)
		{
			iRet = GetPIN_ManualPrvtLbl(FALSE);	//get pin
	
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		else {
			iRet = GetPIN_Manual(FALSE);	//get pin
	
			if( iRet!=0 )
			{
			return iRet;
			}
		}
		/*
	iRet = GetPIN_Manual(FALSE);	//get pin
	
	if( iRet!=0 )
	{
		return iRet;
	}
	*/
	
	//GetPinKeyPerAcq();

	//SET ISO 8583

	if(PrivateLabelCardDetect)
			{
				SetPackSalePrvtLbl();
			}
			else {
			SetPackSale();
			}
			
	///SetPackSale();

#ifdef ENABLE_EMV
	//EMV PROCESS
	if(PrivateLabelCardDetect)
		{
////////////
		}
		else {
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
	{
		return FinishEmvTran();
	}
			}
#endif

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}


	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}
	
//#ifdef BANK_BRI
	displayResponse();
//#endif
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

//		DispResult(ERR_HOST_REJ);
		
		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);

	//SEND TC UPLOAD
	OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	DispTransName();

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		PrnReceipt(PRN_NORMAL);
	}
	PromptRemoveICC();
	//emv_test();
	//DISPLAY RESULT
//	DispResult(0);
	return 0;
}



int TransSaleRedeemption(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	uchar	ucRet;
	char		Point[5+1];
	char		Bit63[105+1];

	iRet = TransInit(TRANS_TYPE_SALE_REDEEMPTION);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_INSERTED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	//CHECK BATCH
	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	//CHECK TRANS ALLOW
	if(!CheckTransAllowed())
	{
		return ERR_NO_DISP;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return ERR_NO_DISP;
	}

	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(Point);
	
	ScrCls();
	DispTransName();
	PubDispString("Sale Redemption", DISP_LINE_LEFT|2);
	PubDispString("Point?", DISP_LINE_LEFT|4);
//	PubDispString("0", DISP_LINE_RIGHT|6);
	strcpy(Point, "0");	//irf,  02 Jan 2014
	ucRet = PubGetString(NUM_IN|ECHO_IN, 1,5 , Point, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	fmtPad(Point, -5 ,'0');

	MEM_0x30(Bit63);

	memcpy(Bit63+40,Point,5);
	
	//fmtPad(tempAmt61, -10, '0');
	//memcpy(tempAmt61+10,"00",2);
	memcpy(glProcInfo.szReq63SaleRedeemption,Bit63,105);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	SetPackSaleRedeemption();

#ifdef ENABLE_EMV
	//EMV PROCESS
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
	{
		return FinishEmvTran();
	}
#endif

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		DispResult(ERR_HOST_REJ);
		
		return ERR_NO_DISP;
	}

	unPackSaleRedeemption();
	
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);

	//SEND TC UPLOAD
	OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	DispTransName();

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		PrnSaleRedeemption(PRN_NORMAL);
	}

	PromptRemoveICC();
	//DISPLAY RESULT
//	DispResult(0);
	return 0;
}

int TransPrivilage(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	strcpy(glProcInfo.stTranLog.szTransName,"PRIVILAGE");

	iRet = TransInit(TRANS_TYPE_PRIVILAGE);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}
		
	//CHECK BATCH
	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	//CHECK TRANS ALLOW
	if(!CheckTransAllowed())
	{
		return ERR_NO_DISP;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackPrivilage();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}

	//CHECK RESPONSE FROM HOST
	if (( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0)  && (memcmp(glProcInfo.stTranLog.szRspCode, "01", 2)!=0 ))
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		DispResult(ERR_HOST_REJ);
		
		return ERR_NO_DISP;
	}
		
		//DISCONNECT FROM HOST
    		CommOnHook(FALSE);

		//PRINT RECEIPT
		if( ChkIfPrnReceipt() )
			{
			DispTransName();
			GetNewInvoiceNo();
#ifdef BANK_BRI
			CetakStruk(PRN_NORMAL);
#endif
			}
		

	//DISPLAY RESULT
	DispResult(0);
	return 0;
}

int TransCicilanBring(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		tempAmt61[12+1];

	

	iRet = TransInit(TRANS_TYPE_CICILAN_BRING);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_INSERTED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(!MatchBRICreditCardTable(glProcInfo.stTranLog.szPan))
	{
		DispErrMsg("TRANSACTION","NOT ALLOWED", TIME_OUT, DERR_BEEP);
		return ERR_NO_DISP;
	}


	//CHECK BATCH
	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	//CHECK TRANS ALLOW
	if(!CheckTransAllowed())
	{
		return ERR_NO_DISP;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return ERR_NO_DISP;
	}

	mapGetWord(traMnuItm,mnuItm);
#ifdef BANK_BRI	
	switch(mnuItm)
		{
		case mnuPlan1:
			memcpy(glProcInfo.szReq61CicilanBring,"001",3);
			break;
		case mnuPlan2:
			memcpy(glProcInfo.szReq61CicilanBring,"002",3);
			break;
		case mnuPlan3:
			memcpy(glProcInfo.szReq61CicilanBring,"003",3);
			break;
		}

	iRet = redemEnterMonth();
#endif
	if(iRet != 0)
	{
		return iRet;
	}
	
	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_0x30(tempAmt61);

	memcpy(tempAmt61,glProcInfo.stTranLog.szAmount,12);
	fmtPad(tempAmt61, -10, '0');

	memcpy(tempAmt61+10,"00",2);
	memcpy(glProcInfo.szReq61CicilanBring+5,tempAmt61+3,9);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	SetPackCicilanBring();

#ifdef ENABLE_EMV
	//EMV PROCESS
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
	{
		return FinishEmvTran();
	}
#endif

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		DispResult(ERR_HOST_REJ);
		
		return ERR_NO_DISP;
	}

	unPackCicilanBring();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);

	//SEND TC UPLOAD
	OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	DispTransName();

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		PrnCicilanBring(PRN_NORMAL);
	}

	PromptRemoveICC();
	return 0;
}

//#ifdef BANK_BUKOPIN
int TransCicilanBukopin(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
//	word	mnuItm;
//	char		tempAmt61[12+1];

	

	iRet = TransInit(TRANS_TYPE_CICILAN_BUKOPIN);
	if( iRet!=0 )
	{
		return iRet;
	} 
/*
	ucEntryMode = CARD_INSERTED|CARD_KEYIN|
				  CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(!MatchBRICreditCardTable(glProcInfo.stTranLog.szPan))
	{
		DispErrMsg("TRANSACTION","NOT ALLOWED", TIME_OUT, DERR_BEEP);
		return ERR_NO_DISP;
	}


	//CHECK BATCH
	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	//CHECK TRANS ALLOW
	if(!CheckTransAllowed())
	{
		return ERR_NO_DISP;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return ERR_NO_DISP;
	}

	mapGetWord(traMnuItm,mnuItm);
*/
	/*iRet = EnterMonthCicilanBukopin();
	if(iRet != 0)
	{
		return iRet;
	}
	*/
	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}
/*
	MEM_0x30(tempAmt61);

	memcpy(tempAmt61,glProcInfo.stTranLog.szAmount,12);
	fmtPad(tempAmt61, -10, '0');

	memcpy(tempAmt61+10,"00",2);
	memcpy(glProcInfo.szReq61CicilanBukopin+5,tempAmt61+3,9);
*/
	//GET PIN
	ScrCls();
	DispTransName();
	
	iRet = GetPIN_Manual(FALSE);	//get pin
	
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583

	if(PrivateLabelCardDetect)
			{
				SetPackCicilanBukopinPrvtLbl();
			}
			else {
			SetPackCicilanBukopin();
			}
			
	///SetPackCicilanBukopin();

#ifdef ENABLE_EMV
	//EMV PROCESS
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
	{
		return FinishEmvTran();
	}
#endif

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}
	

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}
	

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		//DispResult(ERR_HOST_REJ);
		
		return ERR_NO_DISP;
	}

	unPackCicilanBukopin();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
	}


	//CLEAR REVERSAL
	SaveRevInfo(FALSE);

	//SEND TC UPLOAD
	OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	DispTransName();


	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		PrnCicilanBukopin(PRN_NORMAL);
	}

	PromptRemoveICC();
	return 0;
}
//#endif


int TransPurchaseSub(void)
{
	int		iRet;

	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

	//Get RekType
	JenisRekening();
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(GETPIN_EMV);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

///// proses for inquery 

	//SET ISO 8583
	SetPackPurchase();
	ModifProCode();
	
/*
#ifdef ENABLE_EMV
	//EMV PROCESS
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
	{
		return FinishEmvTran();
	}
#endif
//*/
	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}


	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}
	
//#ifdef BANK_BRI
	displayResponse();
//#endif
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

//		DispResult(ERR_HOST_REJ);
		
		return ERR_NO_DISP;
	}
	
/////// end inquery prosses
		iRet = TransInit(TRANS_TYPE_PURCHASE);
		if( iRet!=0 )
		{
			return iRet;
		}
	
		//SET ISO 8583
		SetPackPurchase();
		ModifProCode();
	
#ifdef ENABLE_EMV
		//EMV PROCESS
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
		{
			return FinishEmvTran();
		}
#endif
	
		//SEND REVERSAL
		iRet = TranReversal();
		if( iRet!=0 )
		{
			return iRet;
		}
	
		//SEND MESSAGE TO HOST
		iRet = SendPacket();
		if( iRet!=0 )
		{
			return iRet;
		}
	
	
		//RECV MESSAGE FROM HOST
		iRet = RecvPacket();
		if( iRet!=0 )
		{
			if(iRet==ERR_COMM_ERROR)
			{
				//AUTO REVERSAL
				iRet = TranReversal();
				if( iRet!=0 )
				{
					return iRet;
				}
			}
			return iRet;
		}
		
	//#ifdef BANK_BRI
		displayResponse();
	//#endif
		//CHECK RESPONSE FROM HOST
		if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
		{
			CommOnHook(FALSE);
			SaveRevInfo(FALSE);
	
	//		DispResult(ERR_HOST_REJ);
			
			return ERR_NO_DISP;
		}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);

	//SEND TC UPLOAD
	OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	DispTransName();

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		PrnReceipt(PRN_NORMAL);
	}
	PromptRemoveICC();
	//DISPLAY RESULT
//	DispResult(0);
	return 0;
}

int TransCashAdvance(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	iRet = TransInit(TRANS_TYPE_CASH_ADVANCE);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glTlmSysParam.Con.appEMSale & EM_CHIP_INITIAL)
		ucEntryMode |= CARD_INSERTED;
	if(glTlmSysParam.Con.appEMSale & EM_MAGNETIC_STRIPE_INITIAL)
		ucEntryMode |= CARD_SWIPED;
	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
		ucEntryMode |= CARD_KEYIN;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransSaleSub();
	
}





int TransSalePrvtLbl(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	iRet = TransInit(TRANS_TYPE_SALE);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glTlmSysParam.Con.appEMSale & EM_CHIP_INITIAL)
		ucEntryMode |= CARD_INSERTED;
	if(glTlmSysParam.Con.appEMSale & EM_MAGNETIC_STRIPE_INITIAL)
		ucEntryMode |= CARD_SWIPED;
	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
		ucEntryMode |= CARD_KEYIN;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	//iRet = GetCardPrvtLbl(ucEntryMode);
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


	return TransSubSaleNewPrvtLbl();
	
}



int TransSaleSubPrvtLbl(void)
{
	int		iRet;

	iRet = TransInit(TRANS_TYPE_SALE);
	if( iRet!=0 )
	{
		return iRet;
	} 
			

	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}



	//GET PIN
	ScrCls();
	DispTransName();
	
	iRet = GetPIN_ManualPrvtLbl(FALSE);	//get pin
	
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//GetPinKeyPerAcq();

	//SET ISO 8583
	//SetPackSale();
	SetPackSalePrvtLbl();


#ifdef ENABLE_EMV
	//EMV PROCESS
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
	{
		//// return FinishEmvTran();
	}
#endif

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}


	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}
	
//#ifdef BANK_BRI
	displayResponse();
//#endif
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

//		DispResult(ERR_HOST_REJ);
		
		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);

	//SEND TC UPLOAD
	OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	DispTransName();

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		//PrnReceipt(PRN_NORMAL);  // PrnReceiptPrvtLbl
		PrnReceiptPrvtLbl(PRN_NORMAL);
	}
	PromptRemoveICC();
	//emv_test();
	//DISPLAY RESULT
//	DispResult(0);
	return 0;
}



int TransSalePrvtLblTes(void)
{
	int		iRet;
	uchar	ucEntryMode=0;


	iRet = TransInit(TRANS_TYPE_SALE);
	if( iRet!=0 )
	{
		return iRet;
	}

	if(glTlmSysParam.Con.appEMSale & EM_CHIP_INITIAL)
		ucEntryMode |= CARD_INSERTED;
	if(glTlmSysParam.Con.appEMSale & EM_MAGNETIC_STRIPE_INITIAL)
		ucEntryMode |= CARD_SWIPED;
	if(glTlmSysParam.Con.appEMSale & EM_MANUAL_INITIAL)
		ucEntryMode |= CARD_KEYIN;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}

	/*
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}
	*/


	//cek apakah mesti settle atau tidak? diki add for bukopin
	/*
	iRet = ValidTransSettleBkpn();
		if( iRet==1 )
		{
			return 0;
		}
		*/


	iRet = StartTapPrvtLbl();
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = ValidCardPrvtLbl();	// 校验卡号
	if( iRet!=0 )
	{
		return iRet;
	}

	

	return TransSubSaleNewPrvtLbl();
	
	
}



int TransCicilanBukopinPrvtLbl(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
//	word	mnuItm;
//	char		tempAmt61[12+1];

	

	iRet = TransInit(TRANS_TYPE_CICILAN_BUKOPIN);
	if( iRet!=0 )
	{
		return iRet;
	} 

	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}
/*
	MEM_0x30(tempAmt61);

	memcpy(tempAmt61,glProcInfo.stTranLog.szAmount,12);
	fmtPad(tempAmt61, -10, '0');

	memcpy(tempAmt61+10,"00",2);
	memcpy(glProcInfo.szReq61CicilanBukopin+5,tempAmt61+3,9);
*/
	//GET PIN
	ScrCls();
	DispTransName();
	
	iRet = GetPIN_ManualPrvtLbl(FALSE);	//get pin
	
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	//SetPackCicilanBukopin();
	SetPackCicilanBukopinPrvtLbl();

#ifdef ENABLE_EMV
	//EMV PROCESS
	if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT) 
	{
	////	return FinishEmvTran();
	}
#endif

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}
	

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		if(iRet==ERR_COMM_ERROR)
		{
			//AUTO REVERSAL
			iRet = TranReversal();
			if( iRet!=0 )
			{
				return iRet;
			}
		}
		return iRet;
	}
	

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		//DispResult(ERR_HOST_REJ);
		
		return ERR_NO_DISP;
	}

	unPackCicilanBukopin();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
	}


	//CLEAR REVERSAL
	SaveRevInfo(FALSE);

	//SEND TC UPLOAD
	OfflineSend(OFFSEND_TC | OFFSEND_TRAN);
	DispTransName();


	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		PrnCicilanBukopinPrvtLbl(PRN_NORMAL);
	}

	PromptRemoveICC();
	return 0;
}



