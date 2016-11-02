#include "global.h"

#if 0
static void MultiLngPrnStr(uchar *str, uchar mode);


enum{
	NORMAL=0,
	SMALL,
	BIG
};
uchar flagSizeFont;

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
				memset(buffer, 0x20, (47-strlen(str)));
				memcpy(buffer+(47-strlen(str)), str, strlen(str));
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
	else
		PrnStr(str);

#endif
}
#endif


int PrnTBank(uchar ucPrnFlag)
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

		if(ucPrnFlag==PRN_REPRINT)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}

		//TID MID
		PrnHeaderTidMid();

		PrnHeaderDateTime();
		
	//	PrnHeaderCardNumber();

		PrnHeaderTraceApprCode();

		PrnHeaderBatchRefNum();

		PrnStr("\n");
		PrnHeaderTransactionName();

		switch(glProcInfo.stTranLog.ucTranType)
		{
			case TRANS_TYPE_CASHOUT:
				PrnCasOut();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_CASH_IN:
				PrnCasIn();
				PrnFooterInfoSaldo();
				break;
		}

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

			if(( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID) && !(glProcInfo.stTranLog.uiStatus & TS_VOID) )
				PrnFooterInformation();
					
	       		PrnSetNormal();
			if( ucNum==0 )
			{
				PrnAgenCopy();
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

#if 1		//irf,  02 Jan 2014
			if((ucNum<=1)&&(ucMaxPrint>2))
			{
				if(( ucNum==0 )&&(ucMaxPrint>2))
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
				else
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
			}
#else
			if(( ucNum==0 ))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			
			if(((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
#endif			
			/*
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			*/
		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0;
}	


int PrnReceipt(uchar ucPrnFlag)
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

		if(ucPrnFlag==PRN_REPRINT)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}

		PrnStep(10);

		//TID MID
//		PrnTidMid();
		PrnTid();
		PrnMid();

		//CARD TYPE
		PrnCardType();

		//CARD NUMBER
		PrnCardNumberBig();
		
		//CARD HOLDER NAME
		PrnCardHolderNameBig();
		PrnExpDate();
		PrnStep(10);

		//TRANSACTION NAME
		PrnTransactionName();
		
		//DATE TIME
		PrnDateTime();
	
		//BATCH INVOICE
		PrnBatchInvoice();

		//REFNUM , APPV CODE
		PrnRefNumApprCode();

		//AMOUNT
		PrnAmount_New((uchar *)"", TRUE);  //Print amount

		PrnStep(10);

		//EMV
#ifdef ENABLE_EMV

				if(PrivateLabelCardDetect)
							{
								//////SetPackSalePrvtLbl();
							}
							else {
				if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
				{
					PrnStr("\n");
					PrnAPPPreferName();
					//PrnAPPLABEL(); // zaki add 16 Jan 13
					PrnAIDEMV();
					PrnTCEMV();
					//PrnTVREMV(); //4212 13-3-14
					//PrnTSI();
					
					//PrintTagEMV(); //tes zaki 1 Januari 2014
				}
							}
							


#endif


		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{	
			if(( ucNum==0 ) || ((ucNum==1)&&(ucMaxPrint>2)))
			{
				PrnPinSignature();
			}
			else
			{
				PrnPinNoSignature();
			}

			//if(( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID) && !(glProcInfo.stTranLog.uiStatus & TS_VOID) )
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

#if 1		//irf,  02 Jan 2014
			if((ucNum<=1)&&(ucMaxPrint>2))
			{
				if(( ucNum==0 )&&(ucMaxPrint>2))
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
				else
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
			}
#else
			if(( ucNum==0 ))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			
			if(((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
#endif			
			/*
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			*/
		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0;
}	


int PrnSale(uchar ucPrnFlag)
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

		if(ucPrnFlag==PRN_REPRINT)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}

		//TID MID
//		PrnTidMid();
		PrnTid();
		PrnMid();

		//CARD TYPE
		PrnCardType();

		//CARD NUMBER
		PrnCardNumberBig();
		
		//CARD HOLDER NAME
		PrnCardHolderNameBig();

		PrnStep(10);

		//TRANSACTION NAME
		PrnTransactionName();
		
		//DATE TIME
		PrnDateTime();
	
		//BATCH INVOICE
		PrnBatchInvoice();

		//REFNUM , APPV CODE
		PrnRefNumApprCode();

#ifdef ENABLE_EMV		
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			PrnStr("\n");
			PrnAIDEMV();
			PrnTCEMV();
			PrnTVREMV();
		}
#endif
		//AMOUNT
		PrnAmount_New((uchar *)"", TRUE);  //Print amount

		//EMV


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

				if(( ucNum==0 ))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			
			if(((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			/*
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			*/
		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0;
}	

int PrnSaleRedeemption(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;
	uchar	szBuff[50];
	uchar	PrintBuff[50];

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

		if(ucPrnFlag==PRN_REPRINT)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}

		//TID MID
//		PrnTidMid();
		PrnTid();
		PrnMid();

		//CARD TYPE
		PrnCardType();

		//CARD NUMBER
		PrnCardNumberBig();
		
		//CARD HOLDER NAME
		PrnCardHolderNameBig();

		PrnStep(10);

		//TRANSACTION NAME
		PrnTransactionName();
		
		//DATE TIME
		PrnDateTime();
	
		//BATCH INVOICE
		PrnBatchInvoice();

		//REFNUM , APPV CODE
		PrnRefNumApprCode();

#ifdef ENABLE_EMV		
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			PrnStr("\n");
			PrnAIDEMV();
			PrnTCEMV();
			PrnTVREMV();
		}
#endif
		//AMOUNT
		PrnStr("\n");
		PrnSetNormal();
		MultiLngPrnStr("Informasi Redemption\n", DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.SaleRedeemption.SaleAmt, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "    Sales Amount                %16s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.SaleRedeemption.RedeemAmt, PrintBuff, GA_SEPARATOR|GA_NEGATIVE);
		sprintf((char *)szBuff, "    Redeem Amt                  %16s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.SaleRedeemption.NetSaleAmt, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "    Net Sale Amt                %16s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		sprintf(PrintBuff, "-%lu", glProcInfo.stTranLog.BRI.SaleRedeemption.PointRedeem);
		sprintf((char *)szBuff, "    Point Redeem                %16s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "    Point Balance               %16lu\n", glProcInfo.stTranLog.BRI.SaleRedeemption.PointBalance);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnSetBig();
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.SaleRedeemption.NetSaleAmt, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "TOTAL   %16s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnSetNormal();
		PrnStr("\n");
		
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

			if(( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID) && !(glProcInfo.stTranLog.uiStatus & TS_VOID) )
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


#if 1		//irf,  02 Jan 2014
			if((ucNum<=1)&&(ucMaxPrint>2))
			{
				if(( ucNum==0 )&&(ucMaxPrint>2))
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
				else
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
			}
#else
			if(( ucNum==0 ))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			
			if(((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
#endif			
			/*
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			*/

		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0;
}	

void PrnPrivilage(void)
{
	PrnSetNormal();

	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0)
		{
	PrnStr("\n\n");
	MultiLngPrnStr("TRANSACTION ACCEPTED", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("Silakan menikmati Privilege Prioritas", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("BRI", DISP_LINE_CENTER);
		}
	else
		{
	PrnStr("\n\n");
	MultiLngPrnStr("TRANSACTION DENIED", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("Mohon maaf untuk sementara fasilitas", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("ini tidak dapat Anda pergunakan", DISP_LINE_CENTER);
		}
}

int PrnCicilanBring(uchar ucPrnFlag)
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

		if(ucPrnFlag==PRN_REPRINT)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}

		//TID MID
//		PrnTidMid();
		PrnTid();
		PrnMid();

		//CARD TYPE
		PrnCardType();

		//CARD NUMBER
		PrnCardNumberBig();
		
		//CARD HOLDER NAME
		PrnCardHolderNameBig();

		PrnStep(10);

		//TRANSACTION NAME
		PrnTransactionName();
		
		//DATE TIME
		PrnDateTime();
	
		//BATCH INVOICE
		PrnBatchInvoice();

		//REFNUM , APPV CODE
		PrnRefNumApprCode();

#ifdef ENABLE_EMV		
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			PrnStr("\n");
			PrnAIDEMV();
			PrnTCEMV();
			PrnTVREMV();
		}
#endif
		//AMOUNT
		PrnStr("\n");
		PrnSetNormal();
		//AMOUNT
		PrnAmount_New((uchar *)"", TRUE);  //Print amount
		PrnStr("\n");
		
		PrnSetNormal();
		MultiLngPrnStr("INFORMASI CICILAN BRING", DISP_LINE_LEFT);
		PrnStr("\n");
		PrnPokokCB();
		PrnBulanCicilanBring();
		PrnCicilanBulanan();
		PrnSetNormal();
		PrnStr("\n");

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

			if(( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID) && !(glProcInfo.stTranLog.uiStatus & TS_VOID) )
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
#if 1		//irf,  02 Jan 2014
			if((ucNum<=1)&&(ucMaxPrint>2))
			{
				if(( ucNum==0 )&&(ucMaxPrint>2))
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
				else
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
			}
#else
			if(( ucNum==0 ))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			
			if(((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
#endif			
			/*
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			*/
		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0; 
}

int PrnCicilanBukopin(uchar ucPrnFlag)
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

		if(ucPrnFlag==PRN_REPRINT)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}

		//TID MID
//		PrnTidMid();
		PrnTid();
		PrnMid();

		//CARD TYPE
		PrnCardType();

		//CARD NUMBER
		PrnCardNumberBig();
		
		//CARD HOLDER NAME
		PrnCardHolderNameBig();
		PrnExpDate();
		PrnStep(10);

		//TRANSACTION NAME
		PrnTransactionName();
		
		//DATE TIME
		PrnDateTime();
	
		//BATCH INVOICE
		PrnBatchInvoice();

		//REFNUM , APPV CODE
		PrnRefNumApprCode();

#ifdef ENABLE_EMV		

		if(PrivateLabelCardDetect)
		{
			//////SetPackSalePrvtLbl();
		}
		else {
										

		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			PrnStr("\n");
			PrnAIDEMV();
			PrnTCEMV();
			//PrnTVREMV();
		}
			}
#endif
		//AMOUNT
		PrnStr("\n");
		PrnSetNormal();
		//AMOUNT
		PrnAmount_New((uchar *)"", TRUE);  //Print amount
		//PrnStr("\n");
		//MultiLngPrnStr("INFORMASI CICILAN BUKOPIN", DISP_LINE_LEFT);
		//PrnSetBig();
		//PrnStr("\n");
		//PrnPokokCB();
		PrnBulanCicilanBring();
		#ifdef BANK_HSBC
		#else
		PrnCicilanBulanan();
		#endif
		#ifdef BANK_PANIN
		PrnInterest();
		#endif
		PrnSetNormal();
		PrnStr("\n");

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				PrnPinSignature();
				//if(( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID) && !(glProcInfo.stTranLog.uiStatus & TS_VOID) )
				PrnFooterInformation();
			}
			else
			{
				PrnPinNoSignature();
			}

					
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
#if 1		//irf,  02 Jan 2014
			if((ucNum<=1)&&(ucMaxPrint>2))
			{
				if(( ucNum==0 )&&(ucMaxPrint>2))
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
				else
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
			}
#else
			if(( ucNum==0 ))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			
			if(((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
#endif			
			/*
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			*/
		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0; 
}


void PrnVersi(void)
{
	PrnInit();
	PrnSetBig();
	PrnStr("\n");
	MultiLngPrnStr("EDC BRI", DISP_LINE_LEFT);
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr("Version   : v2014.1.0.0.4", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Dev       : ", DISP_LINE_LEFT);
	PrnStr("\n\n");
	PrnStart();
}

void PrnAktivasiKartu(void)
{
	char		NoKartu[16+1];
	char		printNoKartu[50+1];
	char		printNoRek[50+1];
	char		printNama[50+1];

	MEM_ZERO(NoKartu);
	
	MEM_ZERO(printNoKartu);
	MEM_ZERO(printNoRek);
	MEM_ZERO(printNama);

	memcpy(NoKartu,glProcInfo.stTranLog.szPan,16);

	sprintf((char *)printNoKartu, "NOMOR KARTU BARU    : %s",NoKartu);
	sprintf((char *)printNoRek, "NOMOR REKENING      : %s", glProcInfo.stTranLog.BRI.Card_Services.NoRek);
	sprintf((char *)printNama,"NAMA NASABAH        : %s",glProcInfo.stTranLog.BRI.Card_Services.Nama);

	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNoKartu, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNoRek, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("AKTIVASI KARTU BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("Segera lakukan registrasi SMS Banking,", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Phone Banking, Transaksi Phone Banking, dan", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Internet Banking", DISP_LINE_CENTER);
}


void PrnRenewalKartu(void)
{
	char		Buffer[50+1];

	PrnSetNormal();
	MEM_ZERO(Buffer);
	sprintf((char *)Buffer, "NOMOR KARTU LAMA    : %s\n",glProcInfo.stTranLog.BRI.Card_Services.NoKartu);
	MultiLngPrnStr(Buffer, DISP_LINE_LEFT);

	MEM_ZERO(Buffer);
	sprintf((char *)Buffer, "NOMOR KARTU BARU    : %s\n",glProcInfo.stTranLog.szPan);
	MultiLngPrnStr(Buffer, DISP_LINE_LEFT);

	MEM_ZERO(Buffer);
	sprintf((char *)Buffer, "NOMOR REKENING      : %s\n", glProcInfo.stTranLog.BRI.Card_Services.NoRek);
	MultiLngPrnStr(Buffer, DISP_LINE_LEFT);

	MEM_ZERO(Buffer);
	sprintf((char *)Buffer,"NAMA NASABAH        : %s\n",glProcInfo.stTranLog.BRI.Card_Services.Nama);
	MultiLngPrnStr(Buffer, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("AKTIVASI KARTU BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("Segera lakukan registrasi SMS Banking,", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Phone Banking, Transaksi Phone Banking, dan", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Internet Banking", DISP_LINE_CENTER);
}

void PrnReissuePin(void)
{
	PrnSetBig();
	PrnStr("\n");
	MultiLngPrnStr("REISSUE PIN BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("KARTU SUDAH AKTIF", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("KEMBALI", DISP_LINE_CENTER);
}


void PrnUbahPinCS(void)
{
	PrnSetBig();
	PrnStr("\n");
	MultiLngPrnStr("UBAH PIN BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");
	PrnSetNormal();
	MultiLngPrnStr("HARAP BUKTI TRANSAKSI INI DISIMPAN", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("SEBAGAI BUKTI TRANSAKSI", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("*** Terima Kasih ***", DISP_LINE_CENTER);
}


void PrnUbahPinSPV(void)
{
	PrnSetBig();
	PrnStr("\n");
	MultiLngPrnStr("UBAH PASSWORD PETUGAS", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Berhasil", DISP_LINE_CENTER);
}

void PrnRegistrasiSMSBanking(void)
{
	char		noHP[13+1];
	uchar	printnoHP[50+1];

	MEM_ZERO(noHP);
	MEM_ZERO(printnoHP);

	memcpy(noHP,glProcInfo.szReq48+16,13);
	sprintf((char *)printnoHP, "NOMOR HP : %s", noHP);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printnoHP, DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("REGISTRASI SMS BANKING", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("BERHASIL", DISP_LINE_CENTER);
}


void PrnRegistrasiPhoneBanking(void)
{
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr("REGISTRASI PHONE BANKING", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("Mulai saat ini anda dapat", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("melakukan transaksi Phone Banking", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("melalui Call BRI", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("ke 14017 atau 021-57987400", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("JAGA KERAHASIAAN PIN KARTU ANDA.", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("HATI-HATI TERHADAP PENIPUAN.", DISP_LINE_CENTER);
}

void PrnRegistrasiIBanking(void)
{
	char 	bit48[lenBit48+1];
	char		Id[10+1];
	uchar	printId[50+1];
	ulong 	lenData;

	MEM_ZERO(Id);
	MEM_ZERO(bit48);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48, glRecvPack.szBit48+2, lenData);

	memcpy(Id,bit48+16,10);
	sprintf((char *)printId, "user ID : %s", Id);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr("REG INTERNET BANKING", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("Internet Banking BRI dapat diakses ", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("melalui internet dengan alamat", DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("https://ib.bri.co.id", DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr(printId, DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("Gunakan User ID dan PIN Internet", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Banking Anda untuk login. Selanjutnya", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("ikuti petunjuk yang ada di Internet", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Banking BRI.", DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("Untuk transaksi Finansial melalui", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Internet Banking, daftarkan nomor HP", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Anda sebagai Token di kantor BRI", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("terdekat.", DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("Untuk keamanan transaksi, jaga", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("kerahasiaan User ID dan Password", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Internet Banking Anda.", DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("Informasi lebih lanjut, silahkan hubungi", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Call BRI di 14017, 021-500017, atau", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("021-57987400.", DISP_LINE_LEFT);
	
}


void PrnRegistrasiTRXPBTransfer(void)
{
	uchar		szBuff[50+1];

	MEM_ZERO(szBuff);

	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr("REG PHONE BANKING TRANSFER\n" , DISP_LINE_CENTER);
	//MultiLngPrnStr("Melalui Phone Banking\n" , DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("BERHASIL\n" , DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("No Rek yang didaftarkan :\n" , DISP_LINE_CENTER);
	MultiLngPrnStr(glProcInfo.stTranLog.BRI.RegTRXPB.NoRek, DISP_LINE_CENTER);
	PrnStr("\n");
	sprintf((char *)szBuff, "Nama  : %s\n", glProcInfo.stTranLog.BRI.RegTRXPB.Nama);
	PrnStr("\n");
	
	MultiLngPrnStr("Sekarang anda dapat melakukan transfer\n", DISP_LINE_LEFT);
	MultiLngPrnStr("ke No Rekening tersebut melalui Phone Banking\n", DISP_LINE_LEFT);
	MultiLngPrnStr("BRI.\n", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Untuk keamanan transaksi, jaga kerahasiaan\n", DISP_LINE_LEFT);
	MultiLngPrnStr("PIN ATM anda.\n", DISP_LINE_LEFT);
}



void PrnRegistrasiTRXPBNoHP(void)
{
	uchar		szBuff[50+1];
	int 		len;

	MEM_ZERO(szBuff);

	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr("REG PHONE BANKING\n" , DISP_LINE_CENTER);
	MultiLngPrnStr("BELI PULSA\n" , DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("BERHASIL\n" , DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("No HP yang didaftarkan :\n" , DISP_LINE_CENTER);
	len = strlen(glProcInfo.stTranLog.BRI.RegTRXPB.NoHP);
	strcpy(szBuff, glProcInfo.stTranLog.BRI.RegTRXPB.NoHP);
	memset(szBuff, '*', (len-3));
	MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
	PrnStr("\n\n");
	
	MultiLngPrnStr("Sekarang anda dapat melakukan Pembelian\n", DISP_LINE_LEFT);
	MultiLngPrnStr("Pulsa untuk Nomor HP tersebut melalui Phone Banking\n", DISP_LINE_LEFT);
	MultiLngPrnStr("BRI.\n", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("Untuk keamanan transaksi, jaga kerahasiaan\n", DISP_LINE_LEFT);
	MultiLngPrnStr("PIN ATM anda.\n", DISP_LINE_LEFT);
}


void PrnInfoSaldoBRI(void)
{
	char 	bit48[lenBit48+1];
	char 	saldo[16+1];
	char 	saldoFmt[20+1];
	char 	nama[30+1];
	ulong 	lenData;

	uchar	printNama[50+1];
	uchar	printSaldo[50+1];

	MEM_ZERO(bit48);
	MEM_ZERO(saldo);
	MEM_ZERO(nama);
	MEM_ZERO(saldoFmt);
	MEM_ZERO(printNama);
	MEM_ZERO(printSaldo);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48, glRecvPack.szBit48+2, lenData);

	memcpy(saldo,bit48,16);
	fmtAmt(saldoFmt,saldo,2,",.");
	memcpy(nama,bit48+16,30);

	sprintf((char *)printNama, "Nama      : %s", nama);
	sprintf((char *)printSaldo, "Saldo     : Rp. %s", saldoFmt);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printSaldo, DISP_LINE_LEFT);
}


void PrnInfoSaldoBankLain(void)
{
	char 	bit48[lenBit48+1];
	char 	saldo[16+1];
	char 	saldoFmt[20+1];
	char 	nama[30+1];
	char		JenisRekening[20+1];
	ulong 	lenData;

	uchar	printNama[50+1];
	uchar	printSaldo[50+1];
	uchar	printJenis[50+1];
	byte		JenisRek;

	MEM_ZERO(bit48);
	MEM_ZERO(saldo);
	MEM_ZERO(nama);
	MEM_ZERO(saldoFmt);
	MEM_ZERO(printNama);
	MEM_ZERO(printSaldo);
	MEM_ZERO(printJenis);
	MEM_ZERO(JenisRekening);
	
	mapGetByte(traSumberRek, JenisRek);
	switch(JenisRek)
		{
		case 1:
			strcpy(JenisRekening,"TABUNGAN");
			break;
		case 2:
			strcpy(JenisRekening,"GIRO");
			break;
		}
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48, glRecvPack.szBit48+2, lenData);

	memcpy(saldo,bit48,16);
	fmtAmt(saldoFmt,saldo,2,",.");
	memcpy(nama,bit48+16,30);

	sprintf((char *)printNama, "Nasabah/Bank : %s", nama);
	sprintf((char *)printJenis, "Jenis Rekening : %s", JenisRekening);
	sprintf((char *)printSaldo, "Saldo Rp.%s", saldoFmt);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printJenis, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printSaldo, DISP_LINE_LEFT);
}


void PrnMiniStatement(void)
{
	char 	bit48[lenBit48+1];
	ulong 	lenData;

	char		header[50+1];
	char		garis[50+1];
	char		line1[50+1];
	char		amt1[12+1];
	char		amtFmt1[20+1];
	char		line2[50+1];
	char		amt2[12+1];
	char		amtFmt2[20+1];
	char		line3[50+1];
	char		amt3[12+1];
	char		amtFmt3[20+1];
	char		line4[50+1];
	char		amt4[12+1];
	char		amtFmt4[20+1];
	char		line5[50+1];
	char		amt5[12+1];
	char		amtFmt5[20+1];
	int		jloop;

	MEM_ZERO(bit48);
	MEM_ZERO(header);
	MEM_ZERO(garis);
	MEM_0x20(line1);
	MEM_ZERO(amt1);
	MEM_ZERO(amtFmt1);
	MEM_0x20(line2);
	MEM_ZERO(amt2);
	MEM_ZERO(amtFmt2);
	MEM_0x20(line3);
	MEM_ZERO(amt3);
	MEM_ZERO(amtFmt3);
	MEM_0x20(line4);
	MEM_ZERO(amt4);
	MEM_ZERO(amtFmt4);
	MEM_0x20(line5);
	MEM_ZERO(amt5);
	MEM_ZERO(amtFmt5);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48, glRecvPack.szBit48+2, lenData);

	//hex_dump("Berak1", glRecvPack.szBit48, lenData);

	memcpy(header,"Date",4);
	memcpy(header+4,"        ",8);
	memcpy(header+12,"Trx",3);
	memcpy(header+15,"  ",2);
	memcpy(header+17,"Amount",6);

	memcpy(garis,"----------  ---  ----------------------------",45);
	// line 1
	// ambil tanggal
	memcpy(line1,bit48,2);
	memcpy(line1+2,"/",1);
	memcpy(line1+3,bit48+2,2);
	memcpy(line1+5,"/",1);
	memcpy(line1+6,bit48+4,4);
	//ambil K/D
	memcpy(line1+13,bit48+8,1);
	//ambil Amount
	memcpy(amt1,bit48+9,12);
	fmtAmt(amtFmt1,amt1,2,",.");
	memcpy(line1+17,"Rp.",3);
	strcpy(line1+30,amtFmt1);

	// line 2
	// ambil tanggal
	memcpy(line2,bit48+21,2);
	memcpy(line2+2,"/",1);
	memcpy(line2+3,bit48+23,2);
	memcpy(line2+5,"/",1);
	memcpy(line2+6,bit48+25,4);

	//ambil K/D
	memcpy(line2+13,bit48+29,1);

	//ambil Amount
	memcpy(amt2,bit48+30,12);
	fmtAmt(amtFmt2,amt2,2,",.");
	memcpy(line2+17,"Rp.",3);
	strcpy(line2+30,amtFmt2);

	// line 3
	// ambil tanggal
	memcpy(line3,bit48+42,2);
	memcpy(line3+2,"/",1);
	memcpy(line3+3,bit48+44,2);
	memcpy(line3+5,"/",1);
	memcpy(line3+6,bit48+46,4);

	//ambil K/D
	memcpy(line3+13,bit48+50,1);

	//ambil Amount
	memcpy(amt3,bit48+51,12);
	fmtAmt(amtFmt3,amt3,2,",.");
	memcpy(line3+17,"Rp.",3);
	strcpy(line3+30,amtFmt3);

	// line 4
	// ambil tanggal
	memcpy(line4,bit48+63,2);
	memcpy(line4+2,"/",1);
	memcpy(line4+3,bit48+65,2);
	memcpy(line4+5,"/",1);
	memcpy(line4+6,bit48+67,4);

	//ambil K/D
	memcpy(line4+13,bit48+71,1);

	//ambil Amount
	memcpy(amt4,bit48+72,12);
	fmtAmt(amtFmt4,amt4,2,",.");
	memcpy(line4+17,"Rp.",3);
	strcpy(line4+30,amtFmt4);

	// line 5
	// ambil tanggal
	memcpy(line5,bit48+84,2);
	memcpy(line5+2,"/",1);
	memcpy(line5+3,bit48+86,2);
	memcpy(line5+5,"/",1);
	memcpy(line5+6,bit48+88,4);

	//ambil K/D
	memcpy(line5+13,bit48+92,1);

	//ambil Amount
	memcpy(amt5,bit48+93,12);
	fmtAmt(amtFmt5,amt5,2,",.");
	memcpy(line5+17,"Rp.",3);
	strcpy(line5+30,amtFmt5);

	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(header, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(garis, DISP_LINE_LEFT);
//hex_dump("Berak2", line1, 52);
//hex_dump("Berak3", line2, 52);
//hex_dump("Berak4", line3, 52);
//hex_dump("Berak5", line4, 52);
//hex_dump("Berak6", line5, 52);
jloop = lenData / 21;
	
	switch(jloop)
		{
		case 1:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			break;
		case 2:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line2, DISP_LINE_LEFT);
			break;
		case 3:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line3, DISP_LINE_LEFT);
			break;
		case 4:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line3, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line4, DISP_LINE_LEFT);
			break;
		case 5:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line3, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line4, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line5, DISP_LINE_LEFT);
			break;


		}
	
}

void PrnMutasiRekening(void)
{
	char 	bit48[lenBit48+1];
	ulong 	lenData;

	char		noRek[15+1];
	char		printNoRek[50+1];
	char		header[50+1];
	char		remark[50+1];
	char		garis[50+1];
	char		line1[50+1];
	char		amt1[12+1];
	char		amtFmt1[20+1];
	char		remark1[23+1];
	char		line2[50+1];
	char		amt2[12+1];
	char		amtFmt2[20+1];
	char		remark2[23+1];
	char		line3[50+1];
	char		amt3[12+1];
	char		amtFmt3[20+1];
	char		remark3[23+1];
	char		line4[50+1];
	char		amt4[12+1];
	char		amtFmt4[20+1];
	char		remark4[23+1];
	char		line5[50+1];
	char		amt5[12+1];
	char		amtFmt5[20+1];
	char		remark5[23+1];
	char		rmk1[50+1];
	char		rmk2[50+1];
	char		rmk3[50+1];
	char		rmk4[50+1];
	char		rmk5[50+1];

	MEM_ZERO(rmk1);
	MEM_ZERO(rmk2);
	MEM_ZERO(rmk3);
	MEM_ZERO(rmk4);
	MEM_ZERO(rmk5);
	MEM_ZERO(bit48);
	MEM_ZERO(noRek);
	MEM_0x20(printNoRek);
	MEM_ZERO(header);
	MEM_ZERO(remark);
	MEM_ZERO(garis);
	MEM_0x20(line1);
	MEM_ZERO(amt1);
	MEM_ZERO(amtFmt1);
	MEM_ZERO(remark1);
	MEM_0x20(line2);
	MEM_ZERO(amt2);
	MEM_ZERO(amtFmt2);
	MEM_ZERO(remark2);
	MEM_0x20(line3);
	MEM_ZERO(amt3);
	MEM_ZERO(amtFmt3);
	MEM_ZERO(remark3);
	MEM_0x20(line4);
	MEM_ZERO(amt4);
	MEM_ZERO(amtFmt4);
	MEM_ZERO(remark4);
	MEM_0x20(line5);
	MEM_ZERO(amt5);
	MEM_ZERO(amtFmt5);
	MEM_ZERO(remark5);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48, glRecvPack.szBit48+2, lenData);

	memcpy(noRek,bit48,15);
	sprintf((char *)printNoRek, "No.Rekening : %s", noRek);

	memcpy(header,"Date",4);
	memcpy(header+4,"        ",8);
	memcpy(header+12,"Trx",3);
	memcpy(header+15,"  ",2);
	memcpy(header+17,"Amount",6);
	strcpy(remark,"   Remark");

	memcpy(garis,"----------  ---  ----------------------------",45);
	// line 1
	// ambil tanggal
	memcpy(line1,bit48+15,10);
	//ambil K/D
	memcpy(line1+13,bit48+25,1);
	//ambil Amount
	memcpy(amt1,bit48+26,10);
	fmtAmt(amtFmt1,amt1,0,",.");
	memcpy(line1+17,"Rp.",3);
	strcpy(line1+30,amtFmt1);
	memcpy(remark1,bit48+36,23);
	strcpy(rmk1,"   ");
	strcat(rmk1,remark1);

	// line 2
	// ambil tanggal
	memcpy(line2,bit48+59,10);
	//ambil K/D
	memcpy(line2+13,bit48+69,1);
	//ambil Amount
	memcpy(amt2,bit48+70,10);
	fmtAmt(amtFmt2,amt2,0,",.");
	memcpy(line2+17,"Rp.",3);
	strcpy(line2+30,amtFmt2);
	memcpy(remark2,bit48+80,23);
	strcpy(rmk2,"   ");
	strcat(rmk2,remark2);

	// line 3
	// ambil tanggal
	memcpy(line3,bit48+103,10);
	//ambil K/D
	memcpy(line3+13,bit48+113,1);
	//ambil Amount
	memcpy(amt3,bit48+114,10);
	fmtAmt(amtFmt3,amt3,0,",.");
	memcpy(line3+17,"Rp.",3);
	strcpy(line3+30,amtFmt3);
	memcpy(remark3,bit48+124,23);
	strcpy(rmk3,"   ");
	strcat(rmk3,remark3);
	
	// line 4
	// ambil tanggal
	memcpy(line4,bit48+147,10);
	//ambil K/D
	memcpy(line4+13,bit48+157,1);
	//ambil Amount
	memcpy(amt4,bit48+158,10);
	fmtAmt(amtFmt4,amt4,0,",.");
	memcpy(line4+17,"Rp.",3);
	strcpy(line4+30,amtFmt4);
	memcpy(remark4,bit48+168,23);
	strcpy(rmk4,"   ");
	strcat(rmk4,remark4);

	// line 2
	// ambil tanggal
	memcpy(line5,bit48+191,10);
	//ambil K/D
	memcpy(line5+13,bit48+201,1);
	//ambil Amount
	memcpy(amt5,bit48+202,10);
	fmtAmt(amtFmt5,amt5,0,",.");
	memcpy(line5+17,"Rp.",3);
	strcpy(line5+30,amtFmt5);
	memcpy(remark5,bit48+212,23);
	strcpy(rmk5,"   ");
	strcat(rmk5,remark5);

	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNoRek, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(header, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(remark, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(garis, DISP_LINE_LEFT);
	switch(lenData)
		{
		case 59:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk1, DISP_LINE_LEFT);
			break;
		case 103:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk2, DISP_LINE_LEFT);
			break;
		case 147:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line3, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk3, DISP_LINE_LEFT);
			break;
		case 191:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line3, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk3, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line4, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk4, DISP_LINE_LEFT);
			break;
		case 235:
			PrnStr("\n");
			MultiLngPrnStr(line1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk1, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk2, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line3, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk3, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line4, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk4, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(line5, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(rmk5, DISP_LINE_LEFT);
			PrnStr("\n");			
			break;		
		}
	
}

void PrnTrfSesamaBRI(void)
{
	uchar	printNoRek[48+1];
	uchar	printNama[48+1];
	uchar	printTotal[48+1];
		
	sprintf((char *)printNoRek, "No Rek : %s", glProcInfo.stTranLog.BRI.TRF_SESAMABRI.NoRek);
	sprintf((char *)printNama,  "Nama   : %s", glProcInfo.stTranLog.BRI.TRF_SESAMABRI.Nama);
	sprintf((char *)printTotal,   "TOTAL  : Rp.%s", glProcInfo.stTranLog.BRI.TRF_SESAMABRI.Total);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNoRek, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTotal, DISP_LINE_LEFT);
}

void PrnTrfAntarBank(void)
{
	uchar	printTotal[48+1];
	uchar	printBankPengirim[48+1];
	uchar	printNamaPengirim[48+1];
	uchar	printNoRekPengirim[48+1];

	uchar	printBankPenerima[48+1];
	uchar	printNamaPenerima[48+1];
	uchar	printNoRekPenerima[48+1];
	uchar	printNoRefPenerima[48+1];
	
	sprintf((char *)printBankPengirim, "Bank   : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPengirim);
	sprintf((char *)printNamaPengirim,  "Nama   : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPengirim);
	sprintf((char *)printNoRekPengirim,   "NoRek  : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim);

	sprintf((char *)printBankPenerima, "Bank   : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPenerima);
	sprintf((char *)printNamaPenerima,  "Nama   : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPenerima);
	sprintf((char *)printNoRekPenerima,   "NoRek  : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPenerima);
	sprintf((char *)printNoRefPenerima,  "NoRef  : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRefPenerima);
	sprintf((char *)printTotal,   "Jumlah : Rp.%s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Total);

	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.JenisRekening, DISP_LINE_LEFT);
	PrnStr("\n");

	MultiLngPrnStr(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur, DISP_LINE_LEFT);
	PrnStr("\n\n");

	PrnSetBig();
	MultiLngPrnStr("ASAL", DISP_LINE_LEFT);
	PrnSetNormal();
	PrnStr("\n\n");
	
	MultiLngPrnStr(printBankPengirim, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNamaPengirim, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNoRekPengirim, DISP_LINE_LEFT);
	PrnStr("\n\n");

	PrnSetBig();
	MultiLngPrnStr("TUJUAN", DISP_LINE_LEFT);
	PrnSetNormal();
	PrnStr("\n\n");

	MultiLngPrnStr(printBankPenerima, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNamaPenerima, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNoRekPenerima, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNoRefPenerima, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTotal, DISP_LINE_LEFT);
	PrnStr("\n");
	
}

void PrnPembTelkom(void)
{

	uchar	szBuff[48+1];
	uchar	PrintBuff[20+1];

	uchar		printOperator[48+1];
	uchar		printNama[48+1];
	uchar		printTagihan[48+1];
	uchar		printBulanTahun[48+1];

	//uchar	NoRef[48+1];
	//uchar	NoCard[48+1];
	uchar	NoRek[48+1];

	uchar szBuffReff[50+1];	
	char szBuf1[50+1];
	char szBuff2[50+1];

	MEM_ZERO(printBulanTahun);

	PrnSetNormal();

	/*
	PrnStr("\n");
	MEM_ZERO(NoRek);
	sprintf((char *)NoRek,         "N0 REKENING   : %s\n", glProcInfo.stTranLog.BRI.PEMB_TELKOM.NoRek);
	MultiLngPrnStr(NoRek, DISP_LINE_LEFT);

	PrnStr("\n");
	memset(szBuff2, 0, sizeof(szBuff2));
	memset(szBuf1, 0, sizeof(szBuf1));
	strcpy((char *)szBuff2, (char *)glProcInfo.stTranLog.szPan);
	sprintf((char *)szBuf1,        "NO KARTU       : %s", szBuff2);
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
	*/

	PrnStr("\n");
	MEM_ZERO(szBuffReff);
	sprintf((char *)szBuffReff,    "REF NO         : %12s", glProcInfo.stTranLog.szRRN);
	MultiLngPrnStr(szBuffReff, DISP_LINE_LEFT);

	
	PrnSetNormal();
	PrnStr("\n");
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff,        "ID/NO TELEPON  : %s", glProcInfo.stTranLog.BRI.PEMB_TELKOM.NoTelepon);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff,        "NAMA           : %s", glProcInfo.stTranLog.BRI.PEMB_TELKOM.Nama);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff,        "BULAN/TAHUN    : %s", glProcInfo.stTranLog.BRI.PEMB_TELKOM.PayPeriod);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);


	PrnStr("\n");
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff,        "JUMLAH TAGIHAN : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_TELKOM.Total);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff,        "JUMLAH DIBAYAR : Rp. %s\n", glProcInfo.stTranLog.BRI.PEMB_TELKOM.Total);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	

	/*
	PrnStr("\n");
	MEM_ZERO(szBuff);
	MEM_ZERO(PrintBuff);
	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
	sprintf((char *)szBuff,        "JUMLAH TAGIHAN   : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	*/
	
	//PrnStr("\n");
	//MultiLngPrnStr("BRI menyatakan struk ini sebagai", DISP_LINE_CENTER);
	//PrnStr("\n");
	//MultiLngPrnStr("bukti pembayaran TELKOM yang sah", DISP_LINE_CENTER);
	//PrnStr("\n\n");
	//MultiLngPrnStr("*** Terima Kasih ***", DISP_LINE_CENTER);
}


void PrnPascaBayarHalo(void)
{
	uchar		printOperator[48+1];
	uchar		printNama[48+1];
	uchar		printTagihan[48+1];

	sprintf((char *)printOperator, "NO HALO        : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUHALO.NoHP);		
	sprintf((char *)printNama, "Nama           : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUHALO.Nama);
	sprintf((char *)printTagihan, "Jumlah Tagihan : Rp.%s", glProcInfo.stTranLog.BRI.PEMB_KARTUHALO.Total);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printOperator, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTagihan, DISP_LINE_LEFT);
	PrnStr("\n\n");

	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
		{
	MultiLngPrnStr("PEMBAYARAN BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");	
	MultiLngPrnStr("TELKOMSEL Menyatakan Struk ini", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("sebagai Bukti Pembayaran yang Sah", DISP_LINE_CENTER);
		}
	else
		{
	MultiLngPrnStr("PEMBAYARAN SEDANG DIPROSES", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Silakan Cek Saldo Anda", DISP_LINE_CENTER);
		}
}

void PrnPascaBayarMatrix(void)
{
	uchar		printOperator[48+1];
	uchar		printNama[48+1];
	uchar		printTagihan[48+1];
	uchar		printPembayaran[48+1];
	uchar		printBulanTahun[48+1];

	uchar		printProvider[48+1];

	//uchar	NoRef[48+1];
	//uchar	NoCard[48+1];
	uchar	NoRek[48+1];

	
	uchar szBuffReff[50+1];	
	char szBuf1[50+1];
	char szBuff[50+1];

	MEM_ZERO(printBulanTahun);

	MEM_ZERO(printProvider);
	PrnStr("\n");
	sprintf((char *)printProvider, "PROVIDER        : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Provider);

	
	MEM_ZERO(NoRek);
	sprintf((char *)NoRek,         "N0 REKENING     : %s\n", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoRek);
	
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	//strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	//sprintf((char *)szBuf1,        "NO KARTU      : %s", szBuff);
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBuf1,        "NO KARTU        : %s****", szBuff);

	MEM_ZERO(szBuffReff);
	sprintf((char *)szBuffReff,    "REF NO          : %12s", glProcInfo.stTranLog.szRRN);

	sprintf((char *)printOperator,    "ID PELANGGAN    : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP);		
	sprintf((char *)printNama,        "NAMA            : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Nama);
	sprintf((char *)printBulanTahun,  "BULAN/TAHUN     : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.PayPeriod);
	sprintf((char *)printTagihan,     "JUMLAH TAGIHAN  : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Total);
	sprintf((char *)printPembayaran,  "JUMLAH DIBAYAR  : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Total);


	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printProvider,DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(szBuf1,DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(NoRek,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBuffReff,DISP_LINE_LEFT);
	//PrnStr("\n");
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printOperator, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printBulanTahun, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTagihan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printPembayaran, DISP_LINE_LEFT);
	PrnStr("\n\n");

	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
		{
	//MultiLngPrnStr("PEMBAYARAN BERHASIL", DISP_LINE_CENTER);
	//PrnStr("\n\n");	
	//MultiLngPrnStr("INDOSAT Menyatakan Struk ini", DISP_LINE_CENTER);
	//PrnStr("\n");
	//MultiLngPrnStr("sebagai Bukti Pembayaran yang Sah", DISP_LINE_CENTER);
		}
	else
		{
	MultiLngPrnStr("PEMBAYARAN SEDANG DIPROSES", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Silakan Cek Saldo Anda", DISP_LINE_CENTER);
		}
}

void PrnPLNPascaBayar(void)
{
	uchar	printIDpel[48+1];
	uchar	printNama[48+1];
	uchar	printTarifDaya[48+1];
	uchar	printBulan[48+1];
	uchar	printStandMeter[48+1];
	uchar	printRegTag[48+1];
	uchar	printMLPO[48+1];
	uchar	printTotal[48+1];
	uchar	printFooterTunggakan[48+1];
	uchar	printFooterTelpon[48+1];

	sprintf((char *)printIDpel, "IDPEL         : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.NoIDPel);		
	sprintf((char *)printNama, "Nama          : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Nama);	
	sprintf((char *)printTarifDaya, "Tarif/Daya    : %s VA", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.TarifDaya);
	sprintf((char *)printBulan, "BL/TH         : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Bulan);
	sprintf((char *)printStandMeter, "Stand Meter   : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.StandMeter);
	sprintf((char *)printRegTag, "Rp TAG PLN    : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.RegTag);
	sprintf((char *)printMLPO, "MPLO Ref      : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.MLPO);
	sprintf((char *)printTotal, "TOTAL BAYAR   : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Total);
	sprintf((char *)printFooterTunggakan, "Anda Masih Memiliki Sisa Tunggakan %s Bulan", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Tunggakan);
	sprintf((char *)printFooterTelpon, "Atau Hub. PLN Terdekat : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Telepon);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printIDpel, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTarifDaya, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printBulan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printStandMeter, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printRegTag, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printMLPO, DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("PLN MENYATAKAN STRUK INI SEBAGAI BUKTI", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("PEMBAYARAN YANG SAH", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("ADMIN BANK    : Rp. 2.500", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTotal, DISP_LINE_LEFT);

	if (memcmp(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Tunggakan, "00",2)!=0)
		{
			PrnStr("\n\n");
			MultiLngPrnStr(printFooterTunggakan, DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr("Rincian Tagihan Dapat Diakses di www.pln.co.id", DISP_LINE_CENTER);
		}
	else
		{
			PrnStr("\n\n");
			MultiLngPrnStr("TERIMA KASIH", DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr("Rincian Tagihan Dapat Diakses di", DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr("www.pln.co.id", DISP_LINE_CENTER);
		}

	PrnStr("\n\n");
	MultiLngPrnStr("Informasi Hubungi Call Center : 123", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr(printFooterTelpon, DISP_LINE_CENTER);
}

void PrnPLNPraBayar(void)
{
	uchar	printNoMeter[48+1];
	uchar	printIDpel[48+1];
	uchar	printNama[48+1];
	uchar	printTarifDaya[48+1];
	uchar	printMLPO[48+1];
	uchar	printBayar[48+1];
	uchar 	printMaterai[48+1];
	uchar	printPPN[48+1];
	uchar	printPPJ[48+1];
	uchar	printAngsuran[48+1];
	uchar	printRpStroomToken[48+1];
	uchar	printKwn[48+1];
	uchar	printStroomToken[48+1];
	uchar	printTelepon[48+1];
	uchar	printTokenNoMeter[48+1];
	uchar	printTokenNoRef[48+1];
	
	sprintf((char *)printNoMeter, "NO METER      : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.NoMeter);		
	sprintf((char *)printIDpel, "IDPEL         : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.NoIDPel);		
	sprintf((char *)printNama, "Nama          : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Nama);	
	sprintf((char *)printTarifDaya, "Tarif/Daya    : %s VA", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.TarifDaya);
	sprintf((char *)printMLPO, "MPLO REF      : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.MLPO);
	sprintf((char *)printBayar, "RP BAYAR      : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Bayar);
	sprintf((char *)printMaterai, "MATERAI       : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Materai);
	sprintf((char *)printPPN, "PPN           : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.PPN);
	sprintf((char *)printPPJ, "PPJ           : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.PPJ);
	sprintf((char *)printAngsuran, "ANGSURAN      : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Angsuran);
	sprintf((char *)printRpStroomToken, "RP STROOM/TOKEN : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.RPToken);
	sprintf((char *)printKwn, "JML KWH       : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.KWH);
	sprintf((char *)printStroomToken, "STROOM/TOKEN  : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Token);
	sprintf((char *)printTelepon, "atau Hub PLN Terdekat %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Telepon);

	sprintf((char *)printTokenNoMeter, "- Nomor Meteran : %s", glProcInfo.stTranLog.BRI.PEMB_PLNPRA.NoMeter);
	sprintf((char *)printTokenNoRef, "- Ref No Transaksi : %s", glProcInfo.stTranLog.szRRN);

	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNoMeter, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printIDpel, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTarifDaya, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printMLPO, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printBayar, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("ADMIN         : Rp. 2.500", DISP_LINE_LEFT);
	
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0)
		{
			PrnStr("\n");
			MultiLngPrnStr(printPPN, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(printPPJ, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(printAngsuran, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(printRpStroomToken, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(printKwn, DISP_LINE_LEFT);
			PrnStr("\n");
			MultiLngPrnStr(printStroomToken, DISP_LINE_LEFT);
			PrnStr("\n\n");
			MultiLngPrnStr("Informasi Hubungi Call Center : 123", DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr(printTelepon, DISP_LINE_CENTER);
		}
	else
		{
			PrnStr("\n\n");
			MultiLngPrnStr("Transaksi Belum Berhasil.", DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr("Lakukan Cetak Token melalui EDC BRI", DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr("di menu Cetak Token Listrik,", DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr("HARI INI", DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr("dengan menginput :", DISP_LINE_CENTER);
			PrnStr("\n\n");
			MultiLngPrnStr(printTokenNoMeter, DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr(printTokenNoRef, DISP_LINE_CENTER);
			PrnStr("\n\n");
			MultiLngPrnStr("Informasi Hubungi Call Center : 123", DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr("atau Hub PLN Terdekat", DISP_LINE_CENTER);
		}

	
}


void PrnKK_BRI(void)
{
	char 	bit48Rsp[999+1];
	char		noKK[16+1];
	char 	tagihan[12+1];
	char 	tagihanFmt[20+1];
	char 	nama[30+1];
	char		minBayar[12+1];
	char		minBayarFmt[20+1];
	char		DueDate[8+1];
	char		total[12+1];
	char		totalFmt[20+1];

	char		printNama[50+1];
	char		printNoKK[50+1];
	char		printTagihan[50+1];
	char		printMinBayar[50+1];
	char		printDueDate[50+1];
	char		printTotalBayar[50+1];

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(noKK);
	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	MEM_ZERO(minBayar);
	MEM_ZERO(minBayarFmt);
	MEM_ZERO(DueDate);
	MEM_ZERO(total);
	MEM_ZERO(totalFmt);

	mapGet(traResponseInq, bit48Rsp, 999);

	memcpy(nama,bit48Rsp,30);
	memcpy(noKK,glProcInfo.szReq48,16);
	memcpy(tagihan,bit48Rsp+31,9);
	fmtAmt(tagihanFmt,tagihan,0,",.");
	memcpy(minBayar,bit48Rsp+42,10);
	fmtAmt(minBayarFmt,minBayar,0,",.");
	memcpy(DueDate,bit48Rsp+54,8);
	memcpy(total,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(totalFmt,total,0,",.");

	sprintf((char *)printNoKK, "Nomor Kartu    : %s", glProcInfo.stTranLog.BRI.PEMB_KKBRI.Nomor);		
	sprintf((char *)printNama, "Nama           : %s", glProcInfo.stTranLog.BRI.PEMB_KKBRI.Nama);	
	sprintf((char *)printTagihan, "Total Tagihan  : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_KKBRI.Tagihan);
	sprintf((char *)printMinBayar, "Minimal Payment: Rp. %s", glProcInfo.stTranLog.BRI.PEMB_KKBRI.MinBayar);
	sprintf((char *)printDueDate, "Pay Due Date   : %s", glProcInfo.stTranLog.BRI.PEMB_KKBRI.DueDate);
	sprintf((char *)printTotalBayar, "Jumlah Tagihan : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_KKBRI.Total);

	PrnSetBig();
	MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_KKBRI.Tampilan, DISP_LINE_LEFT);
	PrnStr("\n");
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNoKK, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTagihan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printMinBayar, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printDueDate, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTotalBayar, DISP_LINE_LEFT);
	PrnStr("\n\n");
}


void PrnKK_KTA(void)
{
	uchar	printnoKK[48+1];
	uchar	printJumlah[48+1];
	uchar	NoRef[48+1];
	uchar	NoCard[48+1];
	uchar	NoRek[48+1];
	uchar szBuffReff[50+1];	
	uchar	Nama[48+1];
	char szBuf1[50+1];
	char szBuff[50+1];

	
	MEM_ZERO(NoRek);
	sprintf((char *)NoRek,        "N0 REKENING   : %s", glProcInfo.stTranLog.BRI.PEMB_KK_KTA.NoRek);
	
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	//strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	//sprintf((char *)szBuf1, "NO KARTU  : %s", szBuff);
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBuf1,       "NO KARTU      : %s****", szBuff);

	MEM_ZERO(szBuffReff);
	sprintf((char *)szBuffReff,   "NO REF        : %12s", glProcInfo.stTranLog.szRRN);

	MEM_ZERO(Nama);
	sprintf((char *)Nama,         "NAMA          : %s", glProcInfo.stTranLog.BRI.PEMB_KK_KTA.NamaCust);
	
	sprintf((char *)printnoKK,    "NOMOR KARTU KREDIT : %s", glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nomor);
	sprintf((char *)printJumlah,  "JUMLAH TAGIHAN     : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Tagihan);

	PrnSetBig();
	MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama, DISP_LINE_LEFT);
	PrnStr("\n");

	//PrnSetNormal();
	//PrnStr("\n");
	//MultiLngPrnStr(szBuf1,DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(NoRek,DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(szBuffReff,DISP_LINE_LEFT);
	
	PrnSetNormal();
	//PrnStr("\n");
	//MultiLngPrnStr(Nama,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printnoKK, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printJumlah, DISP_LINE_LEFT);
	PrnStr("\n\n");
}

void PrnCicilan(void)
{
	uchar	printNoKontrak[48+1];
	uchar	printNama[48+1];
	uchar	printJatuhTempo[48+1];
	uchar	printAngsuran[48+1];
	uchar	printTagihan[48+1];
	uchar	printTotalBayar[48+1];
	uchar	printMLPO[48+1];
	uchar	printFooter[48+1];


	uchar	NoRek[48+1];
	uchar szBuffReff[50+1];	
	char szBuf1[50+1];
	char szBuff[50+1];


	uchar		billid[48+1];
	uchar		name[48+1];
	uchar		ptname[48+1];
	uchar		branchname[48+1];
	uchar		serialnumber[48+1];
	uchar		norangka[48+1];
	uchar		merkkendaraan[48+1];
	uchar		tenor[48+1];
	uchar		sisaangsuran[48+1];
	uchar		noangsuran[48+1];
	uchar		jatuhtempo[48+1];
	uchar		nilaitagihan[48+1];
	uchar		nilaiangsuran[48+1];
	uchar		nilaidenda[48+1];
	uchar		nilaiadmin[48+1];
	uchar		nilailainnya[48+1];
	uchar		referencenumber[48+1];

	uchar		sisaangsuranFmt[48+1];
	uchar		nilaitagihanFmt[48+1];
	uchar		nilaiangsuranFmt[48+1];
	uchar		nilaidendaFmt[48+1];

	uchar		nilaiadminFmt[48+1];
	uchar		nilailainnyaFmt[48+1];

	uchar		totalPembayaran[48+1];
	uchar		totalPembayaranFmt[48+1];

	MEM_ZERO(billid);
	MEM_ZERO(name);
	MEM_ZERO(ptname);
	MEM_ZERO(branchname);
	MEM_ZERO(serialnumber);
	MEM_ZERO(norangka);
	MEM_ZERO(merkkendaraan);
	MEM_ZERO(tenor);
	MEM_ZERO(sisaangsuran);
	MEM_ZERO(noangsuran);
	MEM_ZERO(jatuhtempo);
	MEM_ZERO(nilaitagihan);
	MEM_ZERO(nilaiangsuran);
	MEM_ZERO(nilaidenda);
	MEM_ZERO(referencenumber);
	
	MEM_ZERO(nilaitagihanFmt);
	MEM_ZERO(nilaiangsuranFmt);
	MEM_ZERO(nilaidendaFmt);
	MEM_ZERO(sisaangsuranFmt);

	MEM_ZERO(nilaiadmin);
	MEM_ZERO(nilailainnya);
	MEM_ZERO(nilaiadminFmt);
	MEM_ZERO(nilailainnyaFmt);

	MEM_ZERO(totalPembayaran);
	MEM_ZERO(totalPembayaranFmt);

	
	sprintf((char *)billid,         "No Kontrak   : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.billid);
	sprintf((char *)name,           "Nama Konsumen: %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.name);
	sprintf((char *)ptname,         "Nama PT      : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.ptname);
	sprintf((char *)branchname,     "Cabang       : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.branchname);
	sprintf((char *)serialnumber,   "No Polisi    : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.serialnumber);
	sprintf((char *)norangka,       "No Rangka    : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.norangka);
	sprintf((char *)merkkendaraan,  "Merk         : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.merkkendaraan);
	sprintf((char *)tenor,          "Tenor        : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.tenor);
	sprintf((char *)sisaangsuran,   "Sisa Angsuran: Rp. %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.sisaangsuran);
	sprintf((char *)noangsuran,     "No Angsuran  : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.noangsuran);
	sprintf((char *)jatuhtempo,     "Jatuh Tempo  : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.jatuhtempo);
	sprintf((char *)nilaitagihan,   "Tagihan      : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilaitagihan);
	sprintf((char *)nilaiangsuran,  "Angsuran     : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilaiangsuran);
	sprintf((char *)nilaidenda,     "Denda        : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilaidenda);
	sprintf((char *)nilaiadmin,     "Admin        : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilaiadmin);
	sprintf((char *)nilailainnya,   "Lainnya      : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilailainnya);
	sprintf((char *)totalPembayaran,"Total Tagihan: Rp. %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.TotalPembayaran);

	

	
	MEM_ZERO(NoRek);
	sprintf((char *)NoRek,          "N0 REKENING  : %s", glProcInfo.stTranLog.BRI.PEMB_CICILAN.NoRek);
	
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	//strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	//sprintf((char *)szBuf1, "NO KARTU  : %s", szBuff);
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBuf1,         "NO KARTU     : %s****", szBuff);

	MEM_ZERO(szBuffReff);
	sprintf((char *)szBuffReff,     "NO REF       : %12s", glProcInfo.stTranLog.szRRN);
	

	PrnSetNormal();
	/*
	PrnStr("\n");
	MultiLngPrnStr(szBuf1,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(NoRek,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBuffReff,DISP_LINE_LEFT);
	*/
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(billid, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(name, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(ptname, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(branchname, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(serialnumber, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(norangka, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(merkkendaraan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(tenor, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(sisaangsuran, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(noangsuran, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(jatuhtempo, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(nilaitagihan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(nilaiangsuran, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(nilaidenda, DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(nilaiadmin, DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(nilailainnya, DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr(totalPembayaran, DISP_LINE_LEFT);
	PrnStr("\n\n");
	
}

void PrnPembZakat(void)
{
	uchar	printNoRek[50+1];
	uchar	printNama[50+1];
	uchar	printTotal[50+1];
		
	sprintf((char *)printNama, "Nama Pengelola       : %s", glProcInfo.stTranLog.BRI.PEMB_ZAKAT_INFAQ.Nama);
	sprintf((char *)printNoRek,  "Rekening Pengelola   : %s", glProcInfo.stTranLog.BRI.PEMB_ZAKAT_INFAQ.Nomor);
	sprintf((char *)printTotal,   "JUMLAH               : Rp.%s", glProcInfo.stTranLog.BRI.PEMB_ZAKAT_INFAQ.Total);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNoRek, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTotal, DISP_LINE_LEFT);
}

void PrnPembDPLK(void)
{
	char 	bit48[lenBit48+1];
	char		noDPLK[7+1];
	char		total[12+1];
	ulong 	lenData;

	uchar	printNoDPLK[50+1];
	uchar	printTotal[50+1];

	
	MEM_ZERO(bit48);
	MEM_ZERO(noDPLK);
	MEM_ZERO(printNoDPLK);
	MEM_ZERO(printTotal);
	MEM_ZERO(total);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48, glRecvPack.szBit48+2, lenData);
	
	fmtAmt(total,glProcInfo.stTranLog.szAmount,2,",.");
	memcpy(noDPLK,glProcInfo.szReq48+3,7);
		
	sprintf((char *)printNoDPLK,  "No DPLK : %s", glProcInfo.stTranLog.BRI.PEMB_DPLK.NoDPLK);
	sprintf((char *)printTotal,   "JUMLAH  : Rp.%s", glProcInfo.stTranLog.BRI.PEMB_DPLK.Total);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNoDPLK, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTotal, DISP_LINE_LEFT);
}

void PrnTiketPesawat(void)
{
	uchar	printKodeBayar[48+1];
	uchar	printKodeBooking[48+1];
	uchar	printNama[48+1];
	uchar	printJmlhFlight[48+1];
	uchar	printJmlhBayar[48+1];
	uchar	printFooter[48+1];

	sprintf((char *)printKodeBayar, "KODE BAYAR     : %s", glProcInfo.stTranLog.BRI.PEMB_TIKET.KodeBayar);		
	sprintf((char *)printKodeBooking, "KODE BOOKING   : %s", glProcInfo.stTranLog.BRI.PEMB_TIKET.KodeBooking);	
	sprintf((char *)printNama, "NAMA PENUMPANG : %s", glProcInfo.stTranLog.BRI.PEMB_TIKET.Nama);
	sprintf((char *)printJmlhFlight, "JUMLAH FLIGHT  : %s", glProcInfo.stTranLog.BRI.PEMB_TIKET.JmlhFlight);
	sprintf((char *)printJmlhBayar, "JUMLAH BAYAR   : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_TIKET.Total);
	sprintf((char *)printFooter,"%s MENYATAKAN STRUK INI", glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel);

	PrnSetBig();
	MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_TIKET.Tampilan, DISP_LINE_LEFT);
	PrnStr("\n");
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printKodeBayar, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printKodeBooking, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printJmlhFlight, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printJmlhBayar, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("NO.PENERBANGAN :", DISP_LINE_LEFT);
	PrnStr("\n\n");

	if ( memcmp(glProcInfo.stTranLog.BRI.PEMB_TIKET.JmlhFlight, "1", 1)==0 )
		{
			MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1, DISP_LINE_CENTER);
			PrnStr("\n\n");
		}
	if ( memcmp(glProcInfo.stTranLog.BRI.PEMB_TIKET.JmlhFlight, "2", 1)==0 )
		{
			MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1, DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2, DISP_LINE_CENTER);
			PrnStr("\n\n");
		}
	if ( memcmp(glProcInfo.stTranLog.BRI.PEMB_TIKET.JmlhFlight, "3", 1)==0 )
		{
			MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1, DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2, DISP_LINE_CENTER);
			PrnStr("\n");
			MultiLngPrnStr(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3, DISP_LINE_CENTER);
			PrnStr("\n\n");
		}

	MultiLngPrnStr(printFooter, DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("SEBAGAI BUKTI PEMBAYARAN YANG SAH", DISP_LINE_CENTER);
	PrnStr("\n");
}

void PrnBriva(void)
{
	char 	bit48Rsp[999+1];
	char		noBriva[20+1];
	char		namaInstitusi[20+1];
	char		namaPelanggan[35+1];
	char		keterangan[20+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];
	char		Bayar[12+1];
	char		BayarFmt[20+1];
	char		Total[12+1];
	char		TotalFmt[20+1];

	char		printNoBriva[50+1];
	char		printNamaInstitusi[50+1];
	char		printNamaPelanggan[50+1];
	char		printKeterangan[50+1];
	char		printTagihan[50+1];
	char		printBayar[50+1];
	char		printTotalBayar[50+1];

	card		numBayar;

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(noBriva);
	MEM_ZERO(namaInstitusi);
	MEM_ZERO(namaPelanggan);
	MEM_ZERO(keterangan);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	MEM_ZERO(Bayar);
	MEM_ZERO(BayarFmt);

	mapGet(traResponseInq, bit48Rsp, 999);

	memcpy(noBriva,bit48Rsp,20);
	memcpy(namaInstitusi,bit48Rsp+118,20);
	memcpy(namaPelanggan,bit48Rsp+56,30);
	memcpy(keterangan,bit48Rsp+20,20);

	memcpy(tagihan,bit48Rsp+91,12);
	fmtAmt(tagihanFmt,tagihan,0,",.");

	memcpy(Bayar,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(BayarFmt,Bayar,0,",.");

	dec2num(&numBayar,Bayar,0);
	numBayar+=2250;
	num2dec(Total,numBayar,0);
	fmtAmt(TotalFmt,Total,0,",.");
	
	sprintf((char *)printNoBriva, "NO BRIVA       : %s", glProcInfo.stTranLog.BRI.PEMB_BRIVA.NoBriva);		
	sprintf((char *)printNamaInstitusi, "INSTITUSI      : %s", glProcInfo.stTranLog.BRI.PEMB_BRIVA.NamaInstitusi);	
	sprintf((char *)printNamaPelanggan, "NAMA PELANGGAN : %s", glProcInfo.stTranLog.BRI.PEMB_BRIVA.NamaPelanggan);	
	sprintf((char *)printKeterangan, "KETERANGAN     : %s", glProcInfo.stTranLog.BRI.PEMB_BRIVA.Keterangan);
	sprintf((char *)printTagihan, "TAGIHAN        : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_BRIVA.Tagihan);
	sprintf((char *)printBayar, "PEMBAYARAN     : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_BRIVA.Bayar);
	//sprintf((char *)printAdmin, "Pembayaran     : Rp. %s", tagihanFmt);
	sprintf((char *)printTotalBayar, "TOTAL PEMBYRN  : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_BRIVA.Total);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNoBriva, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNamaInstitusi, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNamaPelanggan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printKeterangan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTagihan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printBayar, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr("ADMIN BANK     : Rp. 2.250", DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTotalBayar, DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n");
}

void PrnPendidikan(void)
{
	char 	bit48Rsp[999+1];
	char		noBill[20+1];
	char		namaMahasiswa[20+1];
	char		noMahasiswa[20+1];
	char		Tipe[20+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

	char		printNoBill[50+1];
	char		printNamaMahasiswa[50+1];
	char		printNoMahasiswa[50+1];
	char		printTipe[50+1];
	char		printTagihan[50+1];

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(noBill);
	MEM_ZERO(namaMahasiswa);
	MEM_ZERO(noMahasiswa);
	MEM_ZERO(Tipe);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	
	mapGet(traResponseInq, bit48Rsp, 999);

	memcpy(noBill,bit48Rsp,20);
	memcpy(namaMahasiswa,bit48Rsp+60,20);
	memcpy(noMahasiswa,bit48Rsp+20,20);
	memcpy(Tipe,bit48Rsp+80,20);

	memcpy(tagihan,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(tagihanFmt,tagihan,0,",.");
	
	sprintf((char *)printNoBill, "NO. BILLING    : %s", glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.NoBill);		
	sprintf((char *)printNamaMahasiswa, "NAMA MHS       : %s", glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.NamaMahasiswa);	
	sprintf((char *)printNoMahasiswa, "NO MHS         : %s", glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.NoMahasiswa);	
	sprintf((char *)printTipe, "TIPE           : %s", glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.Tipe);
	sprintf((char *)printTagihan, "JUMLAH         : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.Tagihan);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printNoBill, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNamaMahasiswa, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNoMahasiswa, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTipe, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTagihan, DISP_LINE_LEFT);
	PrnStr("\n\n");
	MultiLngPrnStr("BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n");
}


void PrnIsiUlang(void)
{
	uchar	printnoHP[48+1];
	uchar	printNominal[48+1];
	uchar	printPulsa[48+1];

	uchar	printOperator[48+1];

	uchar	NoRef[48+1];
	uchar	NoCard[48+1];
	uchar	NoRek[48+1];

	
	uchar szBuffReff[50+1];	
	char szBuf1[50+1];
	char szBuff[50+1];


	MEM_ZERO(printOperator);
	sprintf((char *)printOperator, "%s\n", glProcInfo.stTranLog.BRI.ISIULANG.OperatorProvider);

	
	MEM_ZERO(NoRek);
	sprintf((char *)NoRek, "N0 REKENING : %s\n", glProcInfo.stTranLog.BRI.ISIULANG.NoRekPengirim);
	
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	//strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	//sprintf((char *)szBuf1, "NO KARTU  : %s", szBuff);
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBuf1,       "NO KARTU    : %s****", szBuff);

	MEM_ZERO(szBuffReff);
	sprintf((char *)szBuffReff,   "NO REF      : %12s", glProcInfo.stTranLog.szRRN);
	
	sprintf((char *)printnoHP,    "NO HP       : %s", glProcInfo.stTranLog.BRI.ISIULANG.NoHP);
	sprintf((char *)printNominal, "HARGA       : Rp. %s", glProcInfo.stTranLog.BRI.ISIULANG.Nominal);
	sprintf((char *)printPulsa,   "PULSA       : Rp. %s", glProcInfo.stTranLog.BRI.ISIULANG.Pulsa);

	PrnSetBig();
	MultiLngPrnStr(printOperator,DISP_LINE_LEFT);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(szBuf1,DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(NoRek,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBuffReff,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printnoHP, DISP_LINE_LEFT);
	PrnStr("\n");
	//MultiLngPrnStr(printNominal, DISP_LINE_LEFT);
	MultiLngPrnStr(printPulsa, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(               "BIAYA ADMIN : Rp. 0", DISP_LINE_LEFT);
	PrnStr("\n");
	//MultiLngPrnStr(printPulsa, DISP_LINE_LEFT);
	MultiLngPrnStr(printNominal, DISP_LINE_LEFT);
	PrnStr("\n\n");
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
			{
				//MultiLngPrnStr("Pulsa Otomatis Bertambah", DISP_LINE_CENTER);
				PrnSetNormal();
	///PrnStr("\n\n");
	///MultiLngPrnStr("Transaksi Berhasil", DISP_LINE_CENTER);
	PrnStr("\n\n");
	//PrnStr("\n");
	//MultiLngPrnStr("PULSA ANDA AKAN TERISI OTOMATIS", DISP_LINE_CENTER);
	//PrnStr("\n");
	MultiLngPrnStr("HARAP TANDA TERIMA INI DISIMPAN", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("SEBAGAI BUKTI TRANSAKSI YANG SAH", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("******* Terima Kasih *******", DISP_LINE_CENTER);
	PrnStr("\n");
	
			}
	else if ( memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)==0 )
			{
				MultiLngPrnStr("Transaksi Sedang Diproses", DISP_LINE_CENTER);
			}
}

void PrnIsiUlangSmartFren(void)
{
	uchar	printnoHP[48+1];
	uchar	printNominal[48+1];
	uchar	printnoResi[48+1];


	sprintf((char *)printnoHP, "NO HP      : %s", glProcInfo.stTranLog.BRI.ISIULANG.NoHP);
	sprintf((char *)printnoResi, "NO SERI    : %s", glProcInfo.stTranLog.BRI.ISIULANG.NomerResiSF);
	sprintf((char *)printNominal, "NOMINAL    : Rp. %s", glProcInfo.stTranLog.BRI.ISIULANG.Nominal);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printnoHP, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printnoResi, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNominal, DISP_LINE_LEFT);
	PrnStr("\n\n");
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
		{
			MultiLngPrnStr("Pulsa Otomatis Bertambah", DISP_LINE_CENTER);
		}
	else if ( memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)==0 )
		{
			MultiLngPrnStr("Transaksi Sedang Diproses", DISP_LINE_CENTER);
		}
}

void PrnIsiUlangIM3(void)
{
	uchar	printnoHP[48+1];
	uchar	printNominal[48+1];
	uchar	printPeriode[48+1];
	
	sprintf((char *)printnoHP, "NO HP      : %s", glProcInfo.stTranLog.BRI.ISIULANG.NoHP);
	sprintf((char *)printNominal, "NOMINAL    : Rp. %s", glProcInfo.stTranLog.BRI.ISIULANG.Nominal);
	sprintf((char *)printPeriode, "PERIODE    : %s", glProcInfo.stTranLog.BRI.ISIULANG.Periode);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printnoHP, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNominal, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printPeriode, DISP_LINE_LEFT);
	PrnStr("\n\n");
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
		{
			MultiLngPrnStr("Pulsa Otomatis Bertambah", DISP_LINE_CENTER);
		}
	else if ( memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)==0 )
		{
			MultiLngPrnStr("Transaksi Sedang Diproses", DISP_LINE_CENTER);
		}
}

void PrnIsiUlangSimpati(void)
{
	uchar	printnoHP[48+1];
	uchar	printNominal[48+1];
	uchar	printPeriode[48+1];
	uchar	printKodeVoucer[48+1];
	
	sprintf((char *)printnoHP, "NO HP          : %s", glProcInfo.stTranLog.BRI.ISIULANG.NoHP);
	sprintf((char *)printNominal, "NOMINAL        : Rp. %s", glProcInfo.stTranLog.BRI.ISIULANG.Nominal);
	sprintf((char *)printPeriode, "PERIODE        : %s", glProcInfo.stTranLog.BRI.ISIULANG.Periode);
	sprintf((char *)printKodeVoucer, "VOUCER NO      : %s", glProcInfo.stTranLog.BRI.ISIULANG.KodeVoucer);
	
	PrnSetNormal();
	PrnStr("\n");
	MultiLngPrnStr(printnoHP, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNominal, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printPeriode, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printKodeVoucer, DISP_LINE_LEFT);
	PrnStr("\n\n");
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
		{
			MultiLngPrnStr("Pulsa Otomatis Bertambah", DISP_LINE_CENTER);
		}
	else if ( memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)==0 )
		{
			MultiLngPrnStr("Transaksi Sedang Diproses", DISP_LINE_CENTER);
		}
}

void PrnTarikTunai(void)
{
	uchar	szBuff[50+1];
	uchar 	PrintBuff[30+1];
	

	PrnSetNormal();
	PrnStr("\n");
	
	MEM_ZERO(szBuff);
	MEM_ZERO(PrintBuff);
	App_ConvAmount(glProcInfo.stTranLog.szAmount, PrintBuff, 0,2, GA_SEPARATOR);
	if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TARIK_TUNAI)
		sprintf((char *)szBuff, "JUMLAH PENARIKAN : %s\n", PrintBuff);
	else
		sprintf((char *)szBuff, "JUMLAH VOID      : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	MEM_ZERO(PrintBuff);
	App_ConvAmount(glProcInfo.stTranLog.BRI.Tunai.Fee, PrintBuff, 2,0,GA_SEPARATOR);
	sprintf((char *)szBuff, "FEE              : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	MEM_ZERO(PrintBuff);
	App_ConvAmount(glProcInfo.stTranLog.BRI.Tunai.Saldo, PrintBuff, 2,0, GA_SEPARATOR);
	sprintf((char *)szBuff, "SALDO            : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

}

void PrnSetorTunai(void)
{
	uchar	szBuff[50+1];
	uchar 	PrintBuff[20+1];
	

	PrnSetNormal();
	PrnStr("\n");
	
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "NOMOR REKENING : %s\n", glProcInfo.stTranLog.BRI.Tunai.NoRek);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "NAMA           : %s\n", glProcInfo.stTranLog.BRI.Tunai.Nama);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	MEM_ZERO(PrintBuff);
	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
	sprintf((char *)szBuff, "AMOUNT         : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

}

void PrnHeaderTidMid(void)
{
	char szTID[48+1];
	char szMID[48+1];
	char szTID2[48+1];
	char szMID2[48+1];
	
	PrnSetNormal();
	MEM_ZERO(szTID);
	MEM_ZERO(szMID);
	MEM_ZERO(szTID2);
	MEM_ZERO(szMID2);

	memcpy(szTID2,glCurTlmTmsAcq.acqTID,4);
	memcpy(szMID2,glCurTlmTmsAcq.acqMID,11);

#ifdef	__TELIUM_PARAM__
	//sprintf(szTID,"TERMINAL ID :                           %s",glCurTlmTmsAcq.acqTID);
	//sprintf(szMID,"MERCHANT ID :                    %s",glCurTlmTmsAcq.acqMID);
	sprintf(szTID,"TERMINAL ID :                           %s****",szTID2);
	sprintf(szMID,"MERCHANT ID :                    %s****",szMID2);
#else
	sprintf(szTID,"TERMINAL ID :                           %s",glCurAcq.acqTID);
	sprintf(szMID,"MERCHANT ID :                    %s",glCurAcq.acqMID);
#endif
	
	MultiLngPrnStr(szTID, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szMID, DISP_LINE_LEFT);
	PrnStep(10);
}

void PrnHeaderDateTime(void)
{
	uchar szBuff[50+1];
	uchar szBuf1[50+1];

	MEM_ZERO(szBuff);
	MEM_ZERO(szBuf1);
	
	PrnSetNormal();
	Conv2EngTime(glProcInfo.stTranLog.szDateTime, szBuf1);
	//DD_MMM_YYYY__hh:mm:ss    
	sprintf((char *)szBuff, "DATE : %-23.11s TIME : %8s", szBuf1, szBuf1+13);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");
}

void PrnHeaderCardNumber(void)
{
	char szBuff[50+1];
	char szBuf1[50+1];

	memset(szBuff, 0, sizeof(szBuff));
	
	PrnSetNormal();
	if (!glCurTlmTmsIss.issFullPAN)
	{
		MaskPan(glProcInfo.stTranLog.szPan, szBuff);
	}
	else
	{
		strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	}

	sprintf((char *)szBuf1, "CARD  : %s", szBuff);
	MultiLngPrnStr(szBuf1,DISP_LINE_LEFT);
	PrnStr("\n");
}

void PrnHeaderTraceApprCode(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	PrnSetNormal();
	sprintf((char *)szBuff, "TRACE : %-25.06lu APPR. : %06s", glProcInfo.stTranLog.ulInvoiceNo,  glProcInfo.stTranLog.szAuthCode);

	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");
}

void PrnHeaderBatchRefNum(void)
{
	uchar szBuff[50+1];

	MEM_ZERO(szBuff);
	PrnSetNormal();
	sprintf((char *)szBuff, "BATCH : %-18.06lu REF NO : %12s", glCurTlmTmsAcq.ulCurBatchNo, glProcInfo.stTranLog.szRRN);

	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");
}

void PrnHeaderTransactionName(void)
{
	PrnSetBig();
	MultiLngPrnStr(glProcInfo.stTranLog.szTransName, DISP_LINE_LEFT);
	PrnStr("\n");	
}

void PrnFooterTerimaKasih(void)
{	
	PrnSetNormal();
	PrnStr("\n\n");
	MultiLngPrnStr("*** Terima Kasih ***", DISP_LINE_CENTER);
	PrnStr("\n");
}

void PrnFooterInfoSaldo(void)
{

	PrnSetNormal();
	///PrnStr("\n\n");
	///MultiLngPrnStr("Transaksi Berhasil", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("HARAP TANDA TERIMA INI DISIMPAN", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("SEBAGAI BUKTI TRANSAKSI YANG SAH", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("******* Terima Kasih *******", DISP_LINE_CENTER);
	PrnStr("\n");
}


void PrnFooterSSB(void)
{

	PrnSetNormal();
	PrnStr("\n\n");
	MultiLngPrnStr("Harap Tanda Terima Ini Disimpan Sebagai", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Bukti Pembayaran Yang Sah", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("Segala Kesalahan Input Menjadi", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Tanggung Jawab Nasabah", DISP_LINE_CENTER);
	PrnStr("\n");
}
void PrnFooterTrfSesamaBRI(void)
{
	PrnSetNormal();
	PrnStr("\n\n");
	MultiLngPrnStr("Berhasil", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("BRI menyatakan struk ini sebagai bukti", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("transfer yang sah", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("*** Terima Kasih ***", DISP_LINE_CENTER);
	PrnStr("\n");
}

void PrnFooterKK_KTA(void)
{	
	PrnSetNormal();
	///PrnStr("\n\n");
	///MultiLngPrnStr("Transaksi Berhasil", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("HARAP TANDA TERIMA INI DISIMPAN", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("SEBAGAI BUKTI TRANSAKSI YANG SAH", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("******* Terima Kasih *******", DISP_LINE_CENTER);
	PrnStr("\n");
}



void PrnSetorPasti(void)
{
	uchar	szBuff[50+1];
	uchar 	PrintBuff[20+1];

	if(memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)==0)
	{
		PrnSetBig();
		MultiLngPrnStr("Melalui Bank Lain", DISP_LINE_LEFT);
		
		PrnSetNormal();
		PrnStr("\n\n");
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "Jumlah Transaksi         : %s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		MultiLngPrnStr("Transaksi Sedang Diproses.\n", DISP_LINE_CENTER);
		MultiLngPrnStr("Cek Saldo Anda atau Hubungi Call BRI\n", DISP_LINE_CENTER);		
		MultiLngPrnStr("14017.\n", DISP_LINE_CENTER);		
	}
	else
	{
		PrnSetBig();
		if(glProcInfo.stTranLog.BRI.ucflagDebitBri)
		{	
			MultiLngPrnStr("Antar BRI\n", DISP_LINE_LEFT);
		}
		else
		{
			MEM_ZERO(szBuff);
			if(memcmp(glProcInfo.stTranLog.BRI.SetorPasti.KodeJenisTransfer, "01", 2)==0)
			{
				sprintf((char *)szBuff, "Melalui Bank Lain (LINK)\n");
			}
			else if(memcmp(glProcInfo.stTranLog.BRI.SetorPasti.KodeJenisTransfer, "02", 2)==0)
			{
				sprintf((char *)szBuff, "Melalui Bank Lain (PRIMA)\n");
			}
			else if(memcmp(glProcInfo.stTranLog.BRI.SetorPasti.KodeJenisTransfer, "03", 2)==0)
			{
				sprintf((char *)szBuff, "Melalui Bank Lain (BERSAMA)\n");
			}
			else
			{
				sprintf((char *)szBuff, "Melalui Bank Lain (BRI)\n");
			}


			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		}

		PrnStr("\n");
		PrnSetBig();
		MultiLngPrnStr("ASAL", DISP_LINE_LEFT);

		PrnSetNormal(); 
		PrnStr("\n");

		MEM_ZERO(szBuff);
		if(glProcInfo.stTranLog.BRI.ucflagDebitBri)
			sprintf((char *)szBuff, "BANK           : BANK BRI\n");
		else
			sprintf((char *)szBuff, "BANK           : %s\n", glProcInfo.stTranLog.BRI.SetorPasti.BankAsal);
		
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "NAMA           : %s\n", glProcInfo.stTranLog.BRI.SetorPasti.NamaAsal);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "NO REK         : %s\n", glProcInfo.stTranLog.BRI.SetorPasti.NoRekAsal);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnSetBig();
		MultiLngPrnStr("TUJUAN", DISP_LINE_LEFT);

		PrnSetNormal();
		PrnStr("\n");
		
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "BANK           : BANK BRI\n");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "NAMA           : %s\n", glProcInfo.stTranLog.BRI.SetorPasti.NamaTujuan);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "NO REK         : %s\n", glProcInfo.stTranLog.BRI.SetorPasti.NoRekTujuan);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);


		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "JUMLAH         : %s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnSetBig();
		MultiLngPrnStr("Berhasil\n", DISP_LINE_CENTER);

	}
}

void PrnSSB(void)
{
	uchar	szBuff[50+1];
	uchar 	PrintBuff[20+1];

	PrnSetBig();
	MultiLngPrnStr(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1, DISP_LINE_LEFT);
	PrnStr("\n");

	PrnSetNormal();
	PrnStr("\n");

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Kode Satpas         : %s\n", glProcInfo.stTranLog.BRI.Ssb.KodeSatpasSSB);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Nama Satpas         : %s\n", glProcInfo.stTranLog.BRI.Ssb.NamaSatpasSSB);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "No Rekening         : %s\n", glProcInfo.stTranLog.BRI.Ssb.RekSatpasSSB);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "No Reff             : %s\n", glProcInfo.stTranLog.szRRN);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);


	switch(glProcInfo.stTranLog.ucOrgTranType)
	{	
		case TRANS_TYPE_BPKB_BARU:
		case TRANS_TYPE_BPKB_GANTI_PEMILIK:
			MEM_ZERO(szBuff);
			sprintf((char *)szBuff, "Jumlah              : %lu\n", glProcInfo.stTranLog.BRI.Ssb.Jumlah);
			MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
			break;
		
	}

	MEM_ZERO(PrintBuff);
	MEM_ZERO(szBuff);
	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
	sprintf((char *)szBuff, "Total               : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	
	PrnStr("\n\n");
}

void PrnCasIn(void)
{
	uchar	szBuff[50+1];
	uchar 	PrintBuff[20+1];
	uchar 	pszPrefix[12];
	uchar 	ucDeciPos;
	uchar 	ucIgnoreDigit;
	uchar 	ucMisc;
	uchar	noHp[20+1];
	int 		len;
	len = strlen(glProcInfo.stTranLog.BRI.CashIn.noHP);

	PrnSetNormal();
	PrnStr("\n");
	
	memset(noHp, 0x2A, 20);
	memcpy(noHp,glProcInfo.stTranLog.BRI.CashIn.noHP, len-4);
	noHp[len]=0;
	
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Nomor HP          : %s\n", glProcInfo.stTranLog.BRI.CashIn.noHP);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	MEM_ZERO(PrintBuff);
	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
	sprintf((char *)szBuff, "Jumlah Penyetoran : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
	MultiLngPrnStr("TRANSAKSI BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");
	
	MultiLngPrnStr("Transaksi ini dikenakan\n", DISP_LINE_CENTER);
	
	
	MEM_ZERO(szBuff);
	MEM_ZERO(pszPrefix);
	MEM_ZERO(PrintBuff);
	strcpy(pszPrefix, "Rp.");

	ucDeciPos=0;
	ucIgnoreDigit=0;
	ucMisc=GA_SEPARATOR;
	PubConvAmount(pszPrefix, glProcInfo.stTranLog.BRI.CashIn.Fee, ucDeciPos, ucIgnoreDigit, PrintBuff, ucMisc);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	sprintf((char *)szBuff, "biaya administrasi %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_CENTER);


	PrnStr("\n\n");
}

void PrnCasOut(void)
{
	uchar	szBuff[50+1];
	uchar 	PrintBuff[20+1];
	uchar 	pszPrefix[12];
	uchar 	ucDeciPos;
	uchar 	ucIgnoreDigit;
	uchar 	ucMisc;
	uchar	noHp[20+1];
	int 		len;
	len = strlen(glProcInfo.stTranLog.BRI.CashOut.noHP);
	
	
	PrnSetNormal();
	PrnStr("\n");

	memset(noHp, 0x2A, 20);
	memcpy(noHp,glProcInfo.stTranLog.BRI.CashOut.noHP, len-4);
	noHp[len]=0;
	
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Nomor HP          : %s\n", noHp);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	MEM_ZERO(PrintBuff);
	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
	sprintf((char *)szBuff, "Jumlah Penarikan  : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	MEM_ZERO(pszPrefix);
	MEM_ZERO(PrintBuff);
	strcpy(pszPrefix, "Rp.");

	ucDeciPos=0;
	ucIgnoreDigit=0;
	ucMisc=GA_SEPARATOR;
	PubConvAmount(pszPrefix, glProcInfo.stTranLog.BRI.CashOut.Saldo, ucDeciPos, ucIgnoreDigit, PrintBuff, ucMisc);
	sprintf((char *)szBuff, "Saldo             : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MultiLngPrnStr("Transaksi ini dikenakan\n", DISP_LINE_CENTER);
	
	MEM_ZERO(szBuff);
	MEM_ZERO(pszPrefix);
	MEM_ZERO(PrintBuff);
	strcpy(pszPrefix, "Rp.");

	ucDeciPos=0;
	ucIgnoreDigit=0;
	ucMisc=GA_SEPARATOR;
	PubConvAmount(pszPrefix, glProcInfo.stTranLog.BRI.CashOut.Fee, ucDeciPos, ucIgnoreDigit, PrintBuff, ucMisc);
	sprintf((char *)szBuff, "biaya administrasi %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_CENTER);

	PrnStr("\n\n");




}




int PrnReceiptSSB(uchar ucPrnFlag)
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
		ucMaxPrint = 2;
	
	for(ucNum=0; ucNum<ucMaxPrint; ucNum++)
	{
		PrnInit();
		PrnSetNormal();

		//LOGO
		PrnCustomLogo_T();

		//SITE NAME & ADDRESS
		PrnHeader();

		if(ucPrnFlag)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}
		
		//TID MID
		PrnHeaderTidMid();

		PrnHeaderDateTime();

		PrnHeaderCardNumber();

		PrnHeaderTraceApprCode();

		PrnHeaderBatchRefNum();

		PrnStr("\n");
		PrnHeaderTransactionName();

		PrnSSB();
		PrnFooterSSB();
		PrnFooterInfoSaldo();

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
					
	       		PrnSetNormal();
			if( ucNum==0 )
			{
				PrnSetNormal();
				MultiLngPrnStr("--POLRI COPY--",DISP_LINE_CENTER);

			}
			else if( ucNum>0 )
			{
					PrnCustomerCopy();
			}

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

			if( ucNum==0 )
			{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
			}
		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0;
}	



// -------------------- diki add for mini atm bukopin


void PrnTrfAntarBankBkpn(void)
{
	uchar	printTotal[48+1];
	uchar	printBankPengirim[48+1];
	uchar	printNamaPengirim[48+1];
	uchar	printNoRekPengirim[48+1];

	uchar	printBankPenerima[48+1];
	uchar	printNamaPenerima[48+1];
	uchar	printNoRekPenerima[48+1];
	uchar	printNoRefPenerima[48+1];
	byte	  jenisRekening;

	uchar szBuffRef[50+1];

	char szBufCard[50+1];
	char szBuff[50+1];

	memset(szBufCard, 0, sizeof(szBufCard));
	memset(szBuff, 0, sizeof(szBuff));
	//strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	//sprintf((char *)szBufCard,          "NOMOR KARTU     : %s", szBuff);	
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBufCard,          "NOMOR KARTU     : %s****", szBuff);

	MEM_ZERO(szBuffRef);
	sprintf((char *)szBuffRef,          "NO REF          : %12s", glProcInfo.stTranLog.szRRN);
	
	//sprintf((char *)printBankPengirim, "Bank   : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPengirim);
	//sprintf((char *)printNamaPengirim,  "Nama   : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPengirim);
	sprintf((char *)printNoRekPengirim, "NO REKENING     : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim);

	sprintf((char *)printBankPenerima,  "BANK TUJUAN     : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPenerima);
	sprintf((char *)printNoRekPenerima, "REKENING TUJUAN : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPenerima);
	sprintf((char *)printNamaPenerima,  "NAMA PENERIMA   : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPenerima);
	//sprintf((char *)printNoRefPenerima,  "NoRef  : %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRefPenerima);
	sprintf((char *)printTotal,         "NILAI TRANSFER  : Rp. %s", glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Total);

	
	PrnSetBig();
	PrnStr("\n");
	MultiLngPrnStr(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.JenisRekening, DISP_LINE_CENTER);
	PrnStr("\n");

	///MultiLngPrnStr(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur, DISP_LINE_CENTER);
	///PrnStr("\n\n");

	PrnSetNormal();
	//MultiLngPrnStr("REKENING ASAL", DISP_LINE_LEFT);
	//PrnSetNormal();
	//PrnStr("\n");
	
	//MultiLngPrnStr(printBankPengirim, DISP_LINE_LEFT);
	//PrnStr("\n");
	//MultiLngPrnStr(printNamaPengirim, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBufCard,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNoRekPengirim, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBuffRef, DISP_LINE_LEFT);
	//PrnStr("\n");

	//PrnSetNormal();
	//MultiLngPrnStr("REKENING TUJUAN", DISP_LINE_LEFT);

	PrnStr("\n");
	MultiLngPrnStr(printNoRekPenerima, DISP_LINE_LEFT);
	
	//PrnSetNormal();
	//PrnStr("\n");
	//MultiLngPrnStr(printBankPenerima, DISP_LINE_LEFT);
	mapGetByte(traSumberRek, jenisRekening);
	switch(jenisRekening)
		{
			case 1:
				break;
			case 2:
				PrnStr("\n");
				MultiLngPrnStr(printBankPenerima, DISP_LINE_LEFT);
				break;
		}
	
	
	PrnStr("\n");
	MultiLngPrnStr(printNamaPenerima, DISP_LINE_LEFT);
	PrnStr("\n");
	//PrnStr("\n");
	//MultiLngPrnStr(printNoRefPenerima, DISP_LINE_LEFT);
	//PrnStr("\n");
	//PrnSetBig();
	MultiLngPrnStr(printTotal, DISP_LINE_LEFT);
	PrnSetNormal();
	PrnStr("\n");
	
}



void PrnFooterTrfSesamaBkpn(void)
{
	PrnSetNormal();
	///PrnStr("\n\n");
	///MultiLngPrnStr("Transaksi Berhasil", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("HARAP TANDA TERIMA INI DISIMPAN", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("SEBAGAI BUKTI TRANSAKSI YANG SAH", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("******* Terima Kasih *******", DISP_LINE_CENTER);
	PrnStr("\n");
}


// add diki for private label bukopin

int PrnReceiptPrvtLbl(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;

	char szBuff123[50+1];
	MEM_ZERO(szBuff123);

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

		if(ucPrnFlag==PRN_REPRINT)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}

		PrnStep(10);

		//TID MID
//		PrnTidMid();
		PrnTid();
		PrnMid();

		//CARD TYPE
		PrnCardType();

		//CARD NUMBER
		////PrnCardNumberBig();
		PrnCardNumberBigPrvtLbl();
		
		//CARD HOLDER NAME
		PrnCardHolderNameBig();
		PrnExpDate();
		PrnStep(10);

		//TRANSACTION NAME
		PrnTransactionName();
		//PrnSetBig();
		//memcpy(szBuff123,"PRIVATE LABEL",13);
		//MultiLngPrnStr(szBuff123, DISP_LINE_LEFT);
		PrnStr("\n");
		
		
		//DATE TIME
		PrnDateTime();
	
		//BATCH INVOICE
		PrnBatchInvoice();

		//REFNUM , APPV CODE
		PrnRefNumApprCode();

		//AMOUNT
		PrnAmount_New((uchar *)"", TRUE);  //Print amount

		PrnStep(10);

		//EMV
#ifdef ENABLE_EMV		
				/*
				if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
				{
					PrnStr("\n");
					PrnAPPPreferName();
					//PrnAPPLABEL(); // zaki add 16 Jan 13
					PrnAIDEMV();
					PrnTCEMV();
					//PrnTVREMV(); //4212 13-3-14
					//PrnTSI();
					
					//PrintTagEMV(); //tes zaki 1 Januari 2014
				}
				*/
#endif


		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{	
			if(( ucNum==0 ) || ((ucNum==1)&&(ucMaxPrint>2)))
			{
				PrnPinSignature();
			}
			else
			{
				PrnPinNoSignature();
			}

			//if(( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID) && !(glProcInfo.stTranLog.uiStatus & TS_VOID) )
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

#if 1		//irf,  02 Jan 2014
			if((ucNum<=1)&&(ucMaxPrint>2))
			{
				if(( ucNum==0 )&&(ucMaxPrint>2))
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
				else
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
			}
#else
			if(( ucNum==0 ))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			
			if(((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
#endif			
			/*
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			*/
		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0;
}



int PrnCicilanBukopinPrvtLbl(uchar ucPrnFlag)
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

		if(ucPrnFlag==PRN_REPRINT)
		{
			PrnDuplicateHeader();
			PrnStr("\n");
		}

		//TID MID
//		PrnTidMid();
		PrnTid();
		PrnMid();

		//CARD TYPE
		PrnCardType();

		//CARD NUMBER
		PrnCardNumberBig();
		
		//CARD HOLDER NAME
		PrnCardHolderNameBig();
		PrnExpDate();
		PrnStep(10);

		//TRANSACTION NAME
		PrnTransactionName();
		
		//DATE TIME
		PrnDateTime();
	
		//BATCH INVOICE
		PrnBatchInvoice();

		//REFNUM , APPV CODE
		PrnRefNumApprCode();

#ifdef ENABLE_EMV		
		if( glProcInfo.stTranLog.uiEntryMode & MODE_CHIP_INPUT )
		{
			/*
			PrnStr("\n");
			PrnAIDEMV();
			PrnTCEMV();
			//PrnTVREMV();
			*/
		}
#endif
		//AMOUNT
		PrnStr("\n");
		PrnSetNormal();
		//AMOUNT
		PrnAmount_New((uchar *)"", TRUE);  //Print amount
		//PrnStr("\n");
		//MultiLngPrnStr("INFORMASI CICILAN BUKOPIN", DISP_LINE_LEFT);
		//PrnSetBig();
		//PrnStr("\n");
		//PrnPokokCB();
		PrnBulanCicilanBring();
		#ifdef BANK_HSBC
		#else
		PrnCicilanBulanan();
		#endif
		#ifdef BANK_PANIN
		PrnInterest();
		#endif
		PrnSetNormal();
		PrnStr("\n");

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				PrnPinSignature();
				//if(( glProcInfo.stTranLog.ucTranType!=TRANS_TYPE_VOID) && !(glProcInfo.stTranLog.uiStatus & TS_VOID) )
				PrnFooterInformation();
			}
			else
			{
				PrnPinNoSignature();
			}

					
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
#if 1		//irf,  02 Jan 2014
			if((ucNum<=1)&&(ucMaxPrint>2))
			{
				if(( ucNum==0 )&&(ucMaxPrint>2))
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
				else
				{
					kbflush();
					ScrClrLine(2,7);
					PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
					PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
					PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
					if(PubWaitKey(5)==KEYCANCEL)
						return 0;
				}
			}
#else
			if(( ucNum==0 ))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("BANK COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			
			if(((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRINT"), 2|DISP_LINE_CENTER);
				PubDispString(_T("COSTUMER COPY"), 4|DISP_LINE_CENTER);
				PubDispString(_T("CONFIRM    (Y/N)"), 6|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
#endif			
			/*
			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return 0;
			}
			*/
		}
		else
		{
//			PrnPinNoSignature();
//			PrnFooterInformation();
			PrnStr("\n");
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return 0; 
}




void PrnPdamPalyjaBkpn(void)
{
	uchar		printOperator[48+1];
	uchar		printNama[48+1];
	uchar		printTagihan[48+1];

	uchar		PrintNoKartu[48+1];
	uchar		PrintNoRek[48+1];
	uchar		PrintNoID[48+1];
	uchar		PrintNama[48+1];
	uchar		PrintJumlahTag[48+1];
	uchar		PrintPeriode[48+1];
	uchar		PrintKubikasi[48+1];
	uchar		PrintRefNum[48+1];
	uchar		PrintAdminFee[48+1];
	uchar		PrintOtherFee[48+1];
	uchar		PrintPinalty[48+1];
	uchar		PrintTax[48+1];
	uchar		PrintTotalTag[48+1];
	uchar		PrintTotalBayar[48+1];


	uchar	NoRek[48+1];	
	char szBuf1[20+1];
	char szBuff[20+1];

/*
	uchar	ProductId[4+1];
	uchar	BillId[20+1];
	uchar	CustName[30+1];
	uchar	NumOfBill[1+1];
	uchar	PayBallance[20+1];
	uchar	PayPeriod[20+1];
	uchar	PayPenalti[20+1];
	uchar	PayTax[20+1];
	uchar	TotalBayar[20+1];
	uchar   NoRekAsal[30+1];
	uchar   AdminFee[20+1];
	uchar   OtherFee[20+1];
	uchar   Kubikasi[20+1];
	*/

	/*
		uchar	ProductId[4+1];
	uchar	BillId[20+1];
	uchar	CustName[30+1];
	uchar	NumOfBill[1+1];
	uchar	PayBallance[20+1];
	uchar	PayPeriod[20+1];
	uchar	PayPenalti[20+1];
	uchar	PayTax[20+1];
	uchar	TotalBayar[20+1];
	uchar   NoRekAsal[30+1];
	*/

	MEM_ZERO(printNama);
	MEM_ZERO(PrintRefNum);




	//sprintf((char *)PrintNoKartu, 	"NO KARTU      :");
	//sprintf((char *)PrintNoRek, 	"NO REKENING   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.NoRekAsal);
	sprintf((char *)PrintNoID,   	"N0 REKENING   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.BillId);
	//sprintf((char *)PrintNama, 		"NAMA          : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.CustName);
	sprintf((char *)PrintJumlahTag, "JMLH TAGIHAN  : 1");
	sprintf((char *)PrintPeriode, 	"PERIODE       : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayPeriod);
	sprintf((char *)PrintKubikasi, 	"KUBIKASI      : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.Kubikasi);
	sprintf((char *)PrintRefNum, 	"REFF NUMBER   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.ReffNum);
	sprintf((char *)PrintAdminFee, 	"ADMIN FEE     : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.AdminFee);
	sprintf((char *)PrintOtherFee, 	"OTHER FEE     : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.OtherFee);
	sprintf((char *)PrintPinalty, 	"PINALTY       : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayPenalti);
	sprintf((char *)PrintTax, 		"TAX           : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayTax);
	sprintf((char *)PrintTotalTag, 	"TAGIHAN       : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayBallance);
	sprintf((char *)PrintTotalBayar,"TOTAL BAYAR   : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.TotalBayar);

	

	//sprintf((char *)printOperator, "NO ID PALYJA   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.BillId);		
	sprintf((char *)printNama,      "NAMA          : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.CustName);
	//sprintf((char *)printTagihan,  "Jumlah Tagihan : Rp.%s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayBallance);


	MEM_ZERO(NoRek);
	//sprintf((char *)NoRek,          "N0 REKENING   : %s\n", glProcInfo.stTranLog.BRI.PEMB_PDAM.NoRekAsal);
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBuf1,         "NOMOR KARTU   : %s****", szBuff);

	
	
	PrnSetNormal();
	//PrnStr("\n");
	//MultiLngPrnStr(NoRek, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintNoID, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintJumlahTag, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintPeriode, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintKubikasi, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintRefNum, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintAdminFee, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintOtherFee, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintPinalty, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintTax, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintTotalTag, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(PrintTotalBayar, DISP_LINE_LEFT);
	PrnStr("\n\n\n");





	/*
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
		{
	MultiLngPrnStr("PEMBAYARAN BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");	
	//MultiLngPrnStr("INDOSAT Menyatakan Struk ini", DISP_LINE_CENTER);
	//PrnStr("\n");
	//MultiLngPrnStr("sebagai Bukti Pembayaran yang Sah", DISP_LINE_CENTER);
		}
	else
		{
	MultiLngPrnStr("PEMBAYARAN SEDANG DIPROSES", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Silakan Cek Saldo Anda", DISP_LINE_CENTER);
		}
	*/

	//MultiLngPrnStr("PEMBAYARAN BERHASIL", DISP_LINE_CENTER);
	//PrnStr("\n\n");
}



void PrnPdamAetraBkpn(void)
{
	
	uchar		printOperator[48+1];
		uchar		printNama[48+1];
		uchar		printTagihan[48+1];
	
		uchar		PrintNoKartu[48+1];
		uchar		PrintNoRek[48+1];
		uchar		PrintNoID[48+1];
		uchar		PrintNama[48+1];
		uchar		PrintJumlahTag[48+1];
		uchar		PrintPeriode[48+1];
		uchar		PrintKubikasi[48+1];
		uchar		PrintRefNum[48+1];
		uchar		PrintAdminFee[48+1];
		uchar		PrintOtherFee[48+1];
		uchar		PrintPinalty[48+1];
		uchar		PrintTax[48+1];
		uchar		PrintTotalTag[48+1];
		uchar		PrintTotalBayar[48+1];


	uchar	NoRek[48+1];	
	char szBuf1[50+1];
	char szBuff[50+1];
	
	/*
		uchar	ProductId[4+1];
		uchar	BillId[20+1];
		uchar	CustName[30+1];
		uchar	NumOfBill[1+1];
		uchar	PayBallance[20+1];
		uchar	PayPeriod[20+1];
		uchar	PayPenalti[20+1];
		uchar	PayTax[20+1];
		uchar	TotalBayar[20+1];
		uchar	NoRekAsal[30+1];
		uchar	AdminFee[20+1];
		uchar	OtherFee[20+1];
		uchar	Kubikasi[20+1];
		*/
	
		/*
			uchar	ProductId[4+1];
		uchar	BillId[20+1];
		uchar	CustName[30+1];
		uchar	NumOfBill[1+1];
		uchar	PayBallance[20+1];
		uchar	PayPeriod[20+1];
		uchar	PayPenalti[20+1];
		uchar	PayTax[20+1];
		uchar	TotalBayar[20+1];
		uchar	NoRekAsal[30+1];
		*/
	
	
		//sprintf((char *)PrintNoKartu,	"NO KARTU      :");
		//sprintf((char *)PrintNoRek, 	"NO REKENING   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.NoRekAsal);
		sprintf((char *)PrintNoID,		"NO REKENING   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.BillId);
		//sprintf((char *)PrintNama,		"NAMA          : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.CustName);
		sprintf((char *)PrintJumlahTag, "JMLH TAGIHAN  : 1");
		sprintf((char *)PrintPeriode,	"PERIODE       : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayPeriod);
		sprintf((char *)PrintKubikasi,	"KUBIKASI      : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.Kubikasi);
		sprintf((char *)PrintRefNum,	"REFF NUMBER   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.ReffNum);
		sprintf((char *)PrintAdminFee,	"ADMIN FEE     : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.AdminFee);
		sprintf((char *)PrintOtherFee,	"OTHER FEE     : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.OtherFee);
		sprintf((char *)PrintPinalty,	"PINALTY       : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayPenalti);
		sprintf((char *)PrintTax,		"TAX           : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayTax);
		sprintf((char *)PrintTotalTag,	"TAGIHAN       : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayBallance);
		sprintf((char *)PrintTotalBayar,"TOTAL BAYAR   : Rp. %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.TotalBayar);
	
		
	
		//sprintf((char *)printOperator, "NO ID AETRA   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.BillId);		
		sprintf((char *)printNama,	    "NAMA          : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.CustName);
		//sprintf((char *)printTagihan,  "Jumlah Tagihan : Rp.%s", glProcInfo.stTranLog.BRI.PEMB_PDAM.PayBallance);

		MEM_ZERO(NoRek);
	//sprintf((char *)NoRek,              "N0 REKENING   : %s", glProcInfo.stTranLog.BRI.PEMB_PDAM.NoRekAsal);
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBuf1,             "NOMOR KARTU   : %s****", szBuff);

	
		
		PrnSetNormal();
		//PrnStr("\n");
		//MultiLngPrnStr(NoRek, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintNoID, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(printNama, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintJumlahTag, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintPeriode, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintKubikasi, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintRefNum, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintAdminFee, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintOtherFee, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintPinalty, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintTax, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintTotalTag, DISP_LINE_LEFT);
		PrnStr("\n");
		MultiLngPrnStr(PrintTotalBayar, DISP_LINE_LEFT);
		PrnStr("\n\n\n");
	
	
	

		


	/*
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)==0 )
		{
	MultiLngPrnStr("PEMBAYARAN BERHASIL", DISP_LINE_CENTER);
	PrnStr("\n\n");	
	//MultiLngPrnStr("INDOSAT Menyatakan Struk ini", DISP_LINE_CENTER);
	//PrnStr("\n");
	//MultiLngPrnStr("sebagai Bukti Pembayaran yang Sah", DISP_LINE_CENTER);
		}
	else
		{
	MultiLngPrnStr("PEMBAYARAN SEDANG DIPROSES", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Silakan Cek Saldo Anda", DISP_LINE_CENTER);
		}
	*/

	//MultiLngPrnStr("PEMBAYARAN BERHASIL", DISP_LINE_CENTER);
	//PrnStr("\n\n");	
}



void PrnTarikTunaiBkpn(void)
{
	uchar		printOperator[48+1];
	uchar		printNama[48+1];
	uchar		printTagihan[48+1];

	//uchar		printProvider[48+1];

	//uchar	NoRef[48+1];
	//uchar	NoCard[48+1];
	uchar	NoRek[48+1];

	
	uchar szBuffReff[50+1];	
	char szBuf1[50+1];
	char szBuff[50+1];

	//MEM_ZERO(printProvider);
	PrnStr("\n");
	//sprintf((char *)printProvider, "PROVIDER      : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Provider);

	
	MEM_ZERO(NoRek);
	sprintf((char *)NoRek,         "N0 REKENING   : %s\n", glProcInfo.stTranLog.BRI.Tunai.NoRek);
	
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	//strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	//sprintf((char *)szBuf1,        "NO KARTU      : %s", szBuff);
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBuf1,        "NOMOR KARTU   : %s****", szBuff);

	MEM_ZERO(szBuffReff);
	sprintf((char *)szBuffReff,    "REF NO        : %12s", glProcInfo.stTranLog.szRRN);

	//sprintf((char *)printOperator, "NO HP         : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP);		
	sprintf((char *)printNama,     "NOMINAL TARIK : Rp.%s", glProcInfo.stTranLog.BRI.Tunai.NominalTarik);
	sprintf((char *)printTagihan,  "SALDO AKHIR   : Rp.%s", glProcInfo.stTranLog.BRI.Tunai.Saldo);


	PrnSetNormal();
	//PrnStr("\n");
	//MultiLngPrnStr(printProvider,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBuf1,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(NoRek,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBuffReff,DISP_LINE_LEFT);
	//PrnStr("\n");
	
	PrnSetNormal();
	//PrnStr("\n");
	//MultiLngPrnStr(printOperator, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTagihan, DISP_LINE_LEFT);
	PrnStr("\n\n");

}




void PrnSetorTunaiBkpn(void)
{
	uchar		printOperator[48+1];
	uchar		printNama[48+1];
	uchar		printTagihan[48+1];

	//uchar		printProvider[48+1];

	//uchar	NoRef[48+1];
	//uchar	NoCard[48+1];
	uchar	NoRek[48+1];

	uchar	NoRekTujuan[48+1];

	
	uchar szBuffReff[50+1];	
	char szBuf1[50+1];
	char szBuff[50+1];

	//MEM_ZERO(printProvider);
	PrnStr("\n");
	//sprintf((char *)printProvider, "PROVIDER      : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Provider);

	
	MEM_ZERO(NoRek);
	sprintf((char *)NoRek,         "N0 REKENING ASAL   : %s", glProcInfo.stTranLog.BRI.SetorPasti.NoRekAsal);

	/*
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	sprintf((char *)szBuf1,        "NO KARTU      : %s", szBuff);
	*/

	MEM_ZERO(szBuffReff);
	sprintf((char *)szBuffReff,    "NO REFERENSI       : %12s", glProcInfo.stTranLog.szRRN);
	

	//sprintf((char *)printOperator, "NO HP         : %s", glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP);	
	MEM_ZERO(NoRekTujuan);
	sprintf((char *)NoRekTujuan,   "NO REKENING TUJUAN : %s", glProcInfo.stTranLog.BRI.SetorPasti.NoRekTujuan);
	sprintf((char *)printNama,     "JUMLAH SETORAN     : Rp.%s", glProcInfo.stTranLog.BRI.SetorPasti.Setoran);
	sprintf((char *)printTagihan,  "SALDO AKHIR        : Rp.%s", glProcInfo.stTranLog.BRI.SetorPasti.SaldoAkhir);


	PrnSetNormal();
	//PrnStr("\n");
	//MultiLngPrnStr(printProvider,DISP_LINE_LEFT);
	
	//PrnStr("\n");
	//MultiLngPrnStr(szBuf1,DISP_LINE_LEFT);
	PrnStr("\n");
	
	MultiLngPrnStr(NoRek,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(szBuffReff,DISP_LINE_LEFT);
	
	//PrnStr("\n");
	
	PrnSetNormal();
	//PrnStr("\n");
	//MultiLngPrnStr(printOperator, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(NoRekTujuan, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printNama, DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(printTagihan, DISP_LINE_LEFT);
	PrnStr("\n\n");

}



void PrnHeaderCardNumberBkpn(void)
{
	char szBufCard[50+1];
	char szBuff[50+1];
	char szBuff2[50+1];

	memset(szBufCard, 0, sizeof(szBufCard));
	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuff2, 0, sizeof(szBuff2));
	strcpy((char *)szBuff, (char *)glProcInfo.stTranLog.szPan);
	//sprintf((char *)szBufCard,          "NOMOR KARTU     : %s", szBuff);	
	memcpy(szBuff2,szBuff,12);
	sprintf((char *)szBufCard,          "CARD  : %s****", szBuff2);	
	
	PrnSetNormal();
	MultiLngPrnStr(szBufCard,DISP_LINE_LEFT);
	PrnStr("\n");
}


void PrnGantiPinBkpn(void)
{
	uchar szBuffReff[50+1];	
	char szBuf1[50+1];
	char szBuff[50+1];

	memset(szBuff, 0, sizeof(szBuff));
	memset(szBuf1, 0, sizeof(szBuf1));
	memcpy(szBuff,glProcInfo.stTranLog.szPan,12);
	sprintf((char *)szBuf1,  "NOMOR KARTU  : %s****", szBuff);

	PrnSetNormal();
	PrnStr("\n");
	PrnStr("\n");
	MultiLngPrnStr(szBuf1,DISP_LINE_LEFT);
	PrnStr("\n");
	MultiLngPrnStr(          "TRANS        : UBAH PIN",DISP_LINE_LEFT);
	PrnStr("\n");
	//MultiLngPrnStr(printTagihan, DISP_LINE_LEFT);
	//PrnStr("\n\n");

	PrnSetNormal();
	PrnStr("\n\n");
	MultiLngPrnStr("MULAI SAAT INI ANDA DAPAT", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("MENGGUNAKAN NOMOR PIN", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("YANG BARU", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("******* Terima Kasih *******", DISP_LINE_CENTER);
	PrnStr("\n");
	

}


void PrnFooterInfoWomBkpn(void)
{

	PrnSetNormal();
	///PrnStr("\n\n");
	///MultiLngPrnStr("Transaksi Berhasil", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("Pembayaran angsuran terakhir harus", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("dilakukan dikantor cabang WOM Finance", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("dan wajib langsung mengambil BPKB", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("Contact Center WOM Finance", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("0804-1-123888", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("******* Terima Kasih *******", DISP_LINE_CENTER);
	PrnStr("\n");
}

void PrnFooterInfoBafBkpn(void)
{

	PrnSetNormal();
	///PrnStr("\n\n");
	///MultiLngPrnStr("Transaksi Berhasil", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("BAF menyatakan resi ini sah", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("dan sudah termasuk biaya adm BAF", DISP_LINE_CENTER);
	PrnStr("\n");
	MultiLngPrnStr("sebesar Rp. 1.000", DISP_LINE_CENTER);
	PrnStr("\n\n");
	MultiLngPrnStr("******* Terima Kasih *******", DISP_LINE_CENTER);
	PrnStr("\n");
}



