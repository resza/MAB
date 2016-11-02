#include"global.h"

#ifdef BANK_MANDIRI
//POWERBUY
int TransPowerBuy(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	iRet = TransInit(TRANS_TYPE_POWERBUY);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_INSERTED;

	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	return TransPowerBuySub();
}

// POWERBUY
int TransPowerBuySub(void)
{
	int		iRet;
	ushort	usMnuItm;
	uchar KeyPowerBuy[5+1];

	
	MEM_ZERO(KeyPowerBuy);
	
	if(!ChkIfBatchFull())
	{
		return ERR_NO_DISP;
	}

	if(!CheckTransAllowed())
	{
		return ERR_NO_DISP;
	}

	if( !ChkSettle() )	//check pending settlement
	{
		return ERR_NO_DISP;
	}

	//Get Tenor
	mapGetWord(traMnuItm, usMnuItm);
	switch(usMnuItm)
	{
		case mnuTenor03:
			strcpy(KeyPowerBuy, "IPP03");
			break;
		case mnuTenor06:
			strcpy(KeyPowerBuy, "IPP06");
			break;
		case mnuTenor12:
			strcpy(KeyPowerBuy, "IPP12");
			break;
		case mnuTenor18:
			strcpy(KeyPowerBuy, "IPP18");
			break;
		case mnuTenor24:
			strcpy(KeyPowerBuy, "IPP24");
			break;
		case mnuTenor36:
			strcpy(KeyPowerBuy, "IPP36");
			break;

	}
	strcpy(glProcInfo.stTranLog.szKeyPowerBuy, KeyPowerBuy);

	//Get Amount
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();

	SetPackPowerBuy();
	return FinishEmvTran();
}


int PrintReceiptPowerBuy(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;

	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return 0;
	}

	DispPrinting();
	if( !ChkIfThermalPrinter() )
	{
		return 0;
	}

	if(ucPrnFlag==PRN_REPRINT)
		ucMaxPrint = 1;
	else
		ucMaxPrint = (uchar)NumOfReceipt();
	
	for(ucNum=0; ucNum<ucMaxPrint; ucNum++)
	{
		PrnInit();
		PrnSetNormal();

		//LOGO
		PrnCustomLogo_T();

		//SITE NAME & ADDRESS
		PrnHeader();

		//TID MID
		PrnTidMid();

		//CARD TYPE
		PrnCardType();

		//CARD NUMBER
		PrnCardNumber();
		
		//CARD HOLDER NAME
		PrnCardHolderName();

		//TRANSACTION NAME
		PrnTransactionName();
		
		//BATCH INVOICE
		PrnBatchInvoice();

		//DATE TIME
		PrnDateTime();
		
		//REFNUM , APPV CODE
		PrnRefNumApprCode();

		//AMOUNT
		PrnAmount_New((uchar *)"", TRUE);  //Print amount

		//TERM MONTHLY
		PrnTermMonthlyPowerBuy();

		//EMV
#ifdef ENABLE_EMV		
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			PrnAppEMV();
			PrnAIDEMV();
			PrnTCEMV();
		}
#endif

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				PrnPinSignature();
			}
			else
			{
				PrnPinNoSignature();
			}

			PrnFooterInformation();
					
	       		PrnSetNormal();
			if( ucNum==0 )
			{
				PrnMerchantCopy();
			}
			else if( ucNum>0 )
			{
				if((ucNum==1)&&(ucMaxPrint>2))
					PrnBankCopy();
				else
					PrnCustomerCopy();
			}

			PrnStr("\n\n\n\n\n\n\n\n");

			StartPrinter();

	//		if( ucNum==0 && NumOfReceipt() != 1)
			{
	            		kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				PubWaitKey(USER_OPER_TIMEOUT);
			}
		}
		else
		{
			PrnPinNoSignature();
			PrnFooterInformation();
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0;
}	
#endif

