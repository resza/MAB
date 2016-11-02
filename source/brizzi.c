#include "global.h"
#include "Brizzi.h"
#include "SamTest.h"

#ifdef BANK_BRI
uchar ApduFlag=0;
uchar CID;
uchar FlagAlreadySAMPrepared = FALSE;

#define SLOT_SAM_PREPAID	0xA2	// 1010 0010

APDU_SEND	ApduSend;
APDU_RESP	ApduRecv;

uchar RandomKey[24+1];

void BrizziPrintCheckBalance(void);
void BrizziPrintInfoDeposit(void);
void BrizziPrintOfflineBrizzi(uchar ucPrnFlag);
void BrizziPrintTopupOnlineBrizzi(uchar ucPrnFlag);
void BrizziPrintAktivasiDepositBrizzi(uchar ucPrnFlag);
void BrizziPrintTopupDeposit(uchar ucPrnFlag);
void BrizziPrintLogBrizzi(uchar ucPrnFlag, uchar *Data, ushort maxdata);
void BrizziPrintVoidBrizzi(uchar ucPrnFlag);
void BrizziPrintSummary(void);
void BrizziPrintRedeem(uchar ucPrnFlag);
void BrizziPrintInfoKartu(uchar *issuedate, uchar *cabangissue, uchar *statuskartu);
void BrizziPrintReaktivasi(uchar ucPrnFlag);
void BrizziPrintDetail(uchar *date, uchar flagDate);


uchar Display_Error(APDU_RESP *ApduRecv)
{
	//add message in here
	ScrCls();
	ScrPrint(17, 0, ASCII, "ERROR CODE %02x %02x", ApduRecv->SWA, ApduRecv->SWB);
	PubDispString("CARD ERROR", DISP_LINE_CENTER|2);
	PubWaitKey(10);
	return FALSE;
}

uchar Check_SW1_SW2(APDU_RESP *ApduRecv)
{
	if((ApduRecv->SWA==0x90)&&(ApduRecv->SWB==0x00))
		return TRUE;
	else
		return Display_Error(ApduRecv);
}

//SAM
uchar GetATR_SAM(uchar *ResponseSAM)
{
	uchar ATR[1+32];
	uchar ucRet;

	memset(ATR, 0, sizeof(ATR));
	ucRet = IccInit(SLOT_SAM_PREPAID,  ATR);
	if(ucRet != 0)
		return FALSE;

	memcpy(ResponseSAM, ATR+1, ATR[0]);

	return TRUE;

}

uchar GetInitApp(void)
{
	memset(&ApduSend, 0, sizeof(APDU_SEND));
	memset(&ApduRecv, 0, sizeof(APDU_RESP));

	ApduSend.Command[0]=0x00;
	ApduSend.Command[1]=0xA4;
	ApduSend.Command[2]=0x04;
	ApduSend.Command[3]=0x0C;

	ApduSend.Lc=0x09;
	memcpy(ApduSend.DataIn,"\xA0\x00\x00\x00\x00\x00\x00\x00\x11", 9);
	if(IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv)==0)
	{
		if(Check_SW1_SW2(&ApduRecv))
		{
			return TRUE;
		}
	}
	else
		FlagAlreadySAMPrepared=FALSE;
	
	return FALSE;
}


int uiGetHash(byte *DataSAM,byte *Output)
{
	memset(&ApduSend, 0, sizeof(APDU_SEND));
	memset(&ApduRecv, 0, sizeof(APDU_RESP));

	ApduSend.Command[0]=0x80;
	ApduSend.Command[1]=0xB4;
	ApduSend.Command[2]=0x00;
	ApduSend.Command[3]=0x00;

	ApduSend.Lc=0x58;
	memcpy(ApduSend.DataIn, DataSAM, 88);

	if(IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv)==0)
	{
		if((ApduRecv.SWA==0x90)&&(ApduRecv.SWB==0x00))
		{
			memcpy(Output,ApduRecv.DataOut, 4);
			return TRUE;
		}

		if (ApduRecv.SWA == 0x61)
		{	
			memset(&ApduSend, 0, sizeof(APDU_SEND));


			ApduSend.Command[0]=0x00;
			ApduSend.Command[1]=0xC0;
			ApduSend.Command[2]=0x00;
			ApduSend.Command[3]=0x00;

			ApduSend.Le=ApduRecv.SWB;	

			memset(&ApduRecv, 0, sizeof(APDU_RESP));
						
			if(IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv)==0)
			{
				if((ApduRecv.SWA==0x90)&&(ApduRecv.SWB==0x00))
				{
					memcpy(Output,ApduRecv.DataOut, 4);
					return TRUE;
				}
			}
		}
	}
	else
		FlagAlreadySAMPrepared=FALSE;
	
	return FALSE;
}

uchar SAMPrepair(uchar Display)
{
	uchar ATR[1+32];

	if(!Display)
	{
		if(FlagAlreadySAMPrepared)
		return TRUE;
	}
	
	if(GetATR_SAM(ATR)!=TRUE)
		goto lblKO;
	
	if(GetInitApp()!=TRUE)
		goto lblKO;
	
	FlagAlreadySAMPrepared=TRUE;

	if(Display)	
	{
		dspClear();
		PubDispString("INITIALISE SAM", DISP_LINE_CENTER|2);
		PubDispString("OK", DISP_LINE_CENTER|4);
		PubBeepOk();
		PubWaitKey(3);
	}
	return TRUE;

lblKO:
	dspClear();
	PubDispString("INITIALISE SAM", DISP_LINE_CENTER|2);
	PubDispString("FAILED", DISP_LINE_CENTER|4);
	Beep();Beep();
	PubWaitKey(3);
	FlagAlreadySAMPrepared=FALSE;
	return FALSE;
}

//CONTACTLESS
uchar PollForToken(char *UID)
{
	uchar ucRet;
	uchar SerialInfo[11+1];
	uchar CardType;
	uchar mode;

	
	PiccClose();
	ucRet = PiccOpen();
	if(ucRet!=0)
	{
		ScrCls();
		PubDispString("READER NOT FOUND", DISP_LINE_CENTER|2);
		PubBeepErr();
		PubWaitKey(3);
		return FALSE;
	}

	mode = 'A';	
    
    	TimerSet(0, 300);

    	do
    	{
        	if(PiccDetect(mode, &CardType, SerialInfo, &CID, NULL)==0)
        	{
        		memcpy(UID, SerialInfo+1, SerialInfo[0]);
			return TRUE;
        	}

		if ((kbhit()==0) && (getkey()==KEYCANCEL))
		{
			ScrCls();
			PubDispString("USER CANCELED", DISP_LINE_CENTER|2);
			PubBeepErr();
			PubWaitKey(2);
			break;
		}

       		if(!TimerCheck(0)) 
       		{           

			ScrCls();
			PubDispString("CARD NOT DETECT", DISP_LINE_CENTER|2);
			PubBeepErr();
			PubWaitKey(3);
            		break;
       		}
		Beef(0, 80);
    	}while (1);
	return FALSE;
}

int getValueBrizzi(unsigned long *amt,byte *Output)
{
	int nCardResponse;
	unsigned char CardResponse[12];
	unsigned long uang;
	byte temp[4+1];


	nCardResponse = sizeof(CardResponse);

	memset(CardResponse, 0, sizeof(CardResponse));
	memset(temp, 0, sizeof(temp));

	if(PiccCmdExchange(2, "\x6C\x00", &nCardResponse, CardResponse) != 0)
		return FALSE;
	
	if(CardResponse[0] != 0x00)
		return FALSE;

	memcpy(temp,CardResponse+1,4);

	uang = temp[0] & 0x000000ff;
	uang +=  ((temp[1] << 8)&0x0000ff00);
	uang += ((temp[2] << 16)&0x00ff0000);
	uang += ((temp[3] << 24)&0xff000000);

	*amt = uang;

	memcpy(Output,temp,4);
				
	return TRUE;
}


int CreditBrizzi(unsigned long amt,byte *Output)
{
	uint nCardResponse;
	byte CardResponse[2];
	byte debit[6];
	byte temp[4+1];

	nCardResponse = sizeof(CardResponse);

	memset(temp,0,sizeof(temp));
	memset(CardResponse, 0, sizeof(CardResponse));

	debit[0] = 0x0C;
	debit[1] = 0x00;

	temp[0] = (unsigned char) amt& 0x000000ff;
	temp[1] = (unsigned char) (( amt >> 8)&0x000000ff);
	temp[2] = (unsigned char) (( amt >> 16)&0x000000ff);
	temp[3] = (unsigned char) (( amt >> 24)&0x000000ff);
		
	
	memcpy(debit+2, temp, 4);

	if(PiccCmdExchange(6, debit, &nCardResponse, CardResponse) != 0)
		return FALSE;
	
	if(CardResponse[0] != 0x00)
		return FALSE;
	
	memcpy(Output, temp, 4);

	return TRUE;
}


int DebitBrizzi(unsigned long amt,byte *Output)
{
	int nCardResponse;
	byte CardResponse[2];
	byte debit[6];
	byte temp[4+1];

	nCardResponse = sizeof(CardResponse);

	memset(temp,0,sizeof(temp));
	memset(CardResponse, 0, sizeof(CardResponse));

	debit[0] = 0xDC;
	debit[1] = 0x00;

	temp[0] = (unsigned char) amt& 0x000000ff;
	temp[1] = (unsigned char) (( amt >> 8)&0x000000ff);
	temp[2] = (unsigned char) (( amt >> 16)&0x000000ff);
	temp[3] = (unsigned char) (( amt >> 24)&0x000000ff);
		
	
	memcpy(debit+2,temp,4);

	if(PiccCmdExchange(6,debit,&nCardResponse,CardResponse) != 0)
		return FALSE;
	
	if(CardResponse[0] != 0x00)
		return FALSE;

	memcpy(Output, temp, 4);

	return TRUE;

}

int CommitBrizzi(void)
{
	int nCardResponse;
	unsigned char CardResponse[2];

	nCardResponse = sizeof(CardResponse);
	memset(CardResponse, 0, sizeof(CardResponse));
	if(PiccCmdExchange(1, "\xC7", &nCardResponse, CardResponse) != 0)
		return FALSE;
	
	if(CardResponse[0] != 0x00)
		return FALSE;

	return TRUE;
}

int WriteRecordBrizzi(byte *input)
{
	int nCardResponse;
	unsigned char CardResponse[2];
	byte writeRecord[40+1];

	nCardResponse = sizeof(CardResponse);

	writeRecord[0] = 0x3B;
	writeRecord[1] = 0x01;
	writeRecord[2] = 0x00;
	writeRecord[3] = 0x00;
	writeRecord[4] = 0x00;
	writeRecord[5] = 0x20;// 1c
	writeRecord[6] = 0x00;
	writeRecord[7] = 0x00;

	memcpy(writeRecord+8,input,32); // 28
	memset(CardResponse, 0, sizeof(CardResponse));
	if(PiccCmdExchange(40,writeRecord,&nCardResponse, CardResponse)!=0)
		return FALSE;

	return TRUE;	
}

int WriteAkumDebitBrizzi(byte *input,unsigned long uangAwal,unsigned long uangAkhir)
{
	int ret, nCardResponse;
	unsigned char CardResponse[2];
	byte writeAkum[15+1];
	byte preBulan[2+1];
	byte preTahun[2+1];
	byte curBulan[2+1];
	byte curTahun[2+1];
	unsigned long akumAwal;
	unsigned long akumAkhir;
	byte temp[4+1];


	nCardResponse = sizeof(CardResponse);

	akumAwal= uangAwal;
	akumAkhir= uangAkhir;

	writeAkum[0] = 0x3D;
	writeAkum[1] = 0x03;
	writeAkum[2] = 0x00;
	writeAkum[3] = 0x00;
	writeAkum[4] = 0x00;
	writeAkum[5] = 0x07;
	writeAkum[6] = 0x00;
	writeAkum[7] = 0x00;

	MAPGET(traPreMonth, preBulan, lblKO);
	MAPGET(traPreYear, preTahun, lblKO);
	MAPGET(traCurMonth, curBulan, lblKO);
	MAPGET(traCurYear, curTahun, lblKO);

	if((preBulan == curBulan) && (preTahun == curTahun))
	{
		if(akumAwal < 20000000)
		{
			akumAkhir= akumAwal + akumAkhir;
						
			temp[0] = (unsigned char) akumAkhir & 0x000000ff;
			temp[1] = (unsigned char) (( akumAkhir>> 8)&0x000000ff);
			temp[2] = (unsigned char) (( akumAkhir>> 16)&0x000000ff);
			temp[3] = (unsigned char) (( akumAkhir>> 24)&0x000000ff);

			memcpy(writeAkum+8,input,3);
			memcpy(writeAkum+11,temp,4);
		}
	}
	else
	{
		memcpy(writeAkum+8,input,7);
	}

	memset(CardResponse, 0, sizeof(CardResponse));
	if(PiccCmdExchange(15, writeAkum, &nCardResponse, CardResponse) != 0)
		return FALSE;
	
	if(CardResponse[0] != 0x00)
		return FALSE;
			
	return TRUE;
	
lblKO:
	return FALSE;
}

//SAM
#if 0
uchar ConnectToSAM(void)
{
	uchar ucRet;
	uchar ATR[1+32];

	if(FlagAlreadySAMPrepared)
		return TRUE;
	
	ucRet = GetATR_SAM(ATR);
	CHECK(ucRet == TRUE, lblKO);

	ucRet = GetInitApp();
	CHECK(ucRet == TRUE, lblKO);

	FlagAlreadySAMPrepared=TRUE;
	return TRUE;

lblKO:
	dspClear();
	PubDispString("INITIALISE SAM", DISP_LINE_CENTER|2);
	PubDispString("FAILED", DISP_LINE_CENTER|4);
	Beep();Beep();
	PubWaitKey(3);
	FlagAlreadySAMPrepared=FALSE;
	return FALSE;

}
#endif

//MIFARE DESFIRE
int GetVersionBrizzi(byte *UID)
{
	dspClear();
	dspLS(1, "DEKATKAN KARTU");
	dspLS(2, "KE LAYAR");
		
	if(PollForToken(UID)!=TRUE)
		return FALSE;

	return TRUE;
 
}

uchar SelectAID01(void)
{
	uchar CardResponse[2];
	unsigned char selectApp1[4]={0x5A,0x01,0x00,0x00};
	uchar ucResult;
	uint uiCardResponse;
	
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(4, selectApp1, &uiCardResponse, CardResponse); // Select App 1
	if(ucResult!=0)
		return FALSE;

	if(CardResponse[0]!=0)
		return FALSE;

	return TRUE;
}

uchar SelectAID03(void)
{
	uchar CardResponse[2];
	unsigned char selectApp3[4]={0x5A,0x03,0x00,0x00};
	uchar ucResult;
	uint uiCardResponse;
	
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(4, selectApp3, &uiCardResponse, CardResponse); // Select App 3
	if(ucResult!=0)
		return FALSE;

	if(CardResponse[0]!=0)
		return FALSE;

	return TRUE;
}


//MIFARE DESFIRE
int ReadCI_Header(byte *Data)
{
	uchar nResult;
	int nCardResponse;
	unsigned char CardResponse[68];
	unsigned char header1[8]={0xBD,0x00,0x00,0x00,0x00,0x17,0x00,0x00};

	nCardResponse = sizeof(CardResponse);

	memset(CardResponse, 0, sizeof(CardResponse));
	nResult = PiccCmdExchange(8,header1,&nCardResponse,CardResponse);
	if(CardResponse[0] != 0x00)
		return FALSE;

	memcpy(Data,CardResponse+1,23);

	return TRUE;
}

void ParseCI_Header(uchar *Data, CI_HEADER *CI_Header)
{
	memcpy(CI_Header->BRI, Data, 3);
	memcpy(CI_Header->Card_Number, Data+3, 8);
	memcpy(CI_Header->Issue_Date, Data+11, 3);
	memcpy(CI_Header->Expiry_Date, Data+14, 3);
	memcpy(CI_Header->Cabang_Issue, Data+17, 2);
	memcpy(CI_Header->Type_Kartu_Bisnis, Data+19, 2);
	memcpy(CI_Header->Model_Kartu, Data+21, 2);
}



//MIFARE DESFIRE
int ReadCI_Status(byte *Data)
{
	uchar nResult;
	int nCardResponse;
	unsigned char CardResponse[68];
	unsigned char header1[8]={0xBD,0x01,0x00,0x00,0x00,0x20,0x00,0x00};

	nCardResponse = sizeof(CardResponse);

	memset(CardResponse, 0, sizeof(CardResponse));
	nResult = PiccCmdExchange(8, header1,&nCardResponse,CardResponse);
	if(CardResponse[0] != 0x00)
		return FALSE;

	memcpy(Data,CardResponse+1,32);

	return TRUE;

}


void ParseCI_Status(uchar *Data, CI_STATUS*CI_Status)
{
	memcpy(CI_Status->Activation_Date, Data, 3);
	memcpy(CI_Status->Status, Data+3, 2);
	memcpy(CI_Status->Interoperability, Data+5, 27);
}


//MIFARE DESFIRE
int ReadLastTrxDateAkumDebit(byte *Data)
{
	uchar nResult;
	int nCardResponse;
	unsigned char CardResponse[68];
	unsigned char header1[8]={0xBD,0x03,0x00,0x00,0x00,0x07,0x00,0x00};

	nCardResponse = sizeof(CardResponse);

	memset(CardResponse, 0, sizeof(CardResponse));
	nResult = PiccCmdExchange(8, header1,&nCardResponse,CardResponse);
	if(CardResponse[0] != 0x00)
		return FALSE;
	
	memcpy(Data,CardResponse+1,7);

	return TRUE;
}

void ParseLastTrxDateAkumDebit(uchar *Data, LAST_TRX_AKUM_DEBIT *LastTrxAkumDebit)
{
	memcpy(LastTrxAkumDebit->LastTrxDate, Data, 3);
	memcpy(LastTrxAkumDebit->AkumulasiDebit, Data+3, 4);
}

uchar GetRandomNoFromCard(uchar *ucRndNumber)
{
	uchar CardResponse[12];
	uchar ucResult;
	uint uiCardResponse;
	
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(2, "\x0A\x00", &uiCardResponse, CardResponse); 

	if(ucResult!=0)
		return FALSE;

	if(CardResponse[0]!=0xAF)
		return FALSE;

	memcpy(ucRndNumber, CardResponse+1,8);

	return TRUE;
}


uchar GetRandomNoFromCard_1(uchar *ucRndNumber)
{
	uchar CardResponse[12];
	uchar ucResult;
	uint uiCardResponse;
	
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(2, "\x0A\x01", &uiCardResponse, CardResponse); 

	if(ucResult!=0)
		return FALSE;

	if(CardResponse[0]!=0xAF)
		return FALSE;

	memcpy(ucRndNumber, CardResponse+1,8);

	return TRUE;
}


uchar GetRandomNoFromSAM_0(uchar *dataSAM, uchar *rndSAM)
{
	uchar ucRet;

	memset(&ApduSend, 0, sizeof(APDU_SEND));
	memset(&ApduRecv, 0, sizeof(APDU_RESP));

	ApduSend.Command[0]=0x80;
	ApduSend.Command[1]=0xB0;
	ApduSend.Command[2]=0x00;
	ApduSend.Command[3]=0x00;

	ApduSend.Lc=0x20;	// 32

	memcpy(ApduSend.DataIn,dataSAM, 32);

	ucRet = IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv);
	if(ucRet==0)
	{
		if((ApduRecv.SWA==0x90)&&(ApduRecv.SWB==0x00))
		{
			memcpy(rndSAM,ApduRecv.DataOut+16, 16);
			return TRUE;
		}

		if (ApduRecv.SWA == 0x61)
		{	
			memset(&ApduSend, 0, sizeof(APDU_SEND));

			ApduSend.Command[0]=0x00;
			ApduSend.Command[1]=0xC0;
			ApduSend.Command[2]=0x00;
			ApduSend.Command[3]=0x00;

			ApduSend.Le=ApduRecv.SWB;	

			memset(&ApduRecv, 0, sizeof(APDU_RESP));
						
			ucRet = IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv);
			if(ucRet==0)
			{
				if((ApduRecv.SWA==0x90)&&(ApduRecv.SWB==0x00))
				{
					memcpy(rndSAM,ApduRecv.DataOut+16, 16);
					return TRUE;
				}
			}
		}
	}
	else
		FlagAlreadySAMPrepared=FALSE;
	
	return FALSE;
}

uchar GetRandomNoFromSAM_1(uchar *dataSAM, uchar *rndSAM)
{
	uchar ucRet;

	memset(&ApduSend, 0, sizeof(APDU_SEND));
	memset(&ApduRecv, 0, sizeof(APDU_RESP));

	ApduSend.Command[0]=0x80;
	ApduSend.Command[1]=0xB2;
	ApduSend.Command[2]=0x00;
	ApduSend.Command[3]=0x00;

	ApduSend.Lc=0x37;	// 55

	memcpy(ApduSend.DataIn,dataSAM, 55);

//	hex_dump_char("GetRandomNoFromSAM_1 DataIn", ApduSend.DataIn, ApduSend.Lc);
	ucRet = IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv);
//	hex_dump_char("GetRandomNoFromSAM_1 ucRet", &ucRet, sizeof(ucRet));

//	hex_dump_char("GetRandomNoFromSAM_1 SWA", &ApduRecv.SWA, sizeof(ApduRecv.SWA));
//	hex_dump_char("GetRandomNoFromSAM_1 SWB", &ApduRecv.SWB, sizeof(ApduRecv.SWB));

//	hex_dump_char("GetRandomNoFromSAM_1 Data", ApduRecv.DataOut, ApduRecv.LenOut);
	if(ucRet==0)
	{
		if((ApduRecv.SWA==0x90)&&(ApduRecv.SWB==0x00))
		{
			memcpy(rndSAM,ApduRecv.DataOut+16, 16);
			return TRUE;
		}

		if (ApduRecv.SWA == 0x61)
		{	
			memset(&ApduSend, 0, sizeof(APDU_SEND));

			ApduSend.Command[0]=0x00;
			ApduSend.Command[1]=0xC0;
			ApduSend.Command[2]=0x00;
			ApduSend.Command[3]=0x00;

			ApduSend.Le=ApduRecv.SWB;	

			memset(&ApduRecv, 0, sizeof(APDU_RESP));
						
			ucRet = IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv);
			if(ucRet==0)
			{
				if((ApduRecv.SWA==0x90)&&(ApduRecv.SWB==0x00))
				{
					memcpy(rndSAM,ApduRecv.DataOut+16, 16);
					return TRUE;
				}
			}
		}
	}
	else
		FlagAlreadySAMPrepared=FALSE;
	
	return FALSE;
}



uchar GenerateRandom(uchar *ucCardNum, uchar *ucUID, uchar *rndKey)
{
	uchar ucRet;
	uchar dataSAM[19+1];
	uchar TID[4+1];

	memset(dataSAM, 0, sizeof(dataSAM));
	memset(TID, 0, sizeof(TID));

	memset(&ApduSend, 0, sizeof(APDU_SEND));
	memset(&ApduRecv, 0, sizeof(APDU_RESP));

	memcpy(dataSAM, ucCardNum, 8);
	memcpy(dataSAM+8, ucUID, 7);

//	hex2bin(TID, glCurTlmTmsAcq.acqTID, 0);
//	memcpy(dataSAM+15, TID, 4);
	
	ApduSend.Command[0]=0x80;
	ApduSend.Command[1]=0xB3;
	ApduSend.Command[2]=0x00;
	ApduSend.Command[3]=0x00;

	ApduSend.Lc=0x13;	// 19

	memcpy(ApduSend.DataIn,dataSAM, 19);

//	hex_dump_char("GenerateRandom", dataSAM, sizeof(dataSAM));
	ucRet = IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv);
//	hex_dump_char("GenerateRandom ucRet", &ucRet, sizeof(ucRet));

	if(ucRet==0)
	{
//		hex_dump_char("GenerateRandom SWA", &ApduRecv.SWA, sizeof(ApduRecv.SWA));
//		hex_dump_char("GenerateRandom SWB", &ApduRecv.SWB, sizeof(ApduRecv.SWB));
//		hex_dump_char("GenerateRandom Data", ApduRecv.DataOut, ApduRecv.LenOut);

		if((ApduRecv.SWA==0x90)&&(ApduRecv.SWB==0x00))
		{
			memcpy(rndKey,ApduRecv.DataOut, 24);
			memcpy(RandomKey, ApduRecv.DataOut, 24);
			return TRUE;
		}

		if (ApduRecv.SWA == 0x61)
		{	
			memset(&ApduSend, 0, sizeof(APDU_SEND));

			ApduSend.Command[0]=0x00;
			ApduSend.Command[1]=0xC0;
			ApduSend.Command[2]=0x00;
			ApduSend.Command[3]=0x00;

			ApduSend.Le=ApduRecv.SWB;	

			memset(&ApduRecv, 0, sizeof(APDU_RESP));
						
			ucRet = IccIsoCommand(SLOT_SAM_PREPAID, &ApduSend, &ApduRecv);
			if(ucRet==0)
			{
				if((ApduRecv.SWA==0x90)&&(ApduRecv.SWB==0x00))
				{
					memcpy(rndKey,ApduRecv.DataOut, 24);
					return TRUE;
				}
			}
		}
	}
	else
		FlagAlreadySAMPrepared=FALSE;
	
	return FALSE;
}

uchar AuthenticateBrizzi(uchar *ucAuthenData, uchar *sesKey)
{
	uchar CardResponse[12];
	uchar ucResult;
	uint uiCardResponse;
	
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(17, ucAuthenData, &uiCardResponse, CardResponse);

//	hex_dump_char("AuthenticateBrizzi", &ucResult, sizeof(ucResult));
	
	if(ucResult!=0)
		return FALSE;

//	hex_dump_char("AuthenticateBrizzi", CardResponse, uiCardResponse);
	if(CardResponse[0]!=0)
		return FALSE;

	memcpy(sesKey,CardResponse+1,8);
	
	return TRUE;
}


uchar AuthenticateBrizzi1(uchar *ucAuthenData, uchar *sesKey)
{
	uchar CardResponse[12];
	uchar ucResult;
	uint uiCardResponse;
	
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(17, ucAuthenData, &uiCardResponse, CardResponse);

//	hex_dump_char("AuthenticateBrizzi", &ucResult, sizeof(ucResult));
	
	if(ucResult!=0)
		return FALSE;

//	hex_dump_char("AuthenticateBrizzi", CardResponse, uiCardResponse);
	if(CardResponse[0]!=0)
		return FALSE;

	memcpy(sesKey,CardResponse+1,8);
	
	return TRUE;
}


//MIFARE DESFIRE
uchar AuthenticateKey0App1(uchar *ucCardNum, uchar *ucUID, uchar *sesKey)
{
	uchar ucRndSAM[32+1];
	uchar dataSAM[32+1];
	uchar authen[17+1];
	uchar ucRndNumber[8+1];

	memset(ucRndSAM, 0, sizeof(ucRndSAM));
	memset(dataSAM, 0, sizeof(dataSAM));
	memset(authen, 0, sizeof(authen));
	memset(ucRndNumber, 0, sizeof(ucRndNumber));
	
	if(GetRandomNoFromCard(ucRndNumber)!=TRUE)
		return FALSE;

	memcpy(glProcInfo.ucRndNumber, ucRndNumber, sizeof(glProcInfo.ucRndNumber));
	//hex_dump_char("GetRandomNoFromCard", ucRndNumber, 8);


	memcpy(dataSAM, ucCardNum, 8);
	memcpy(dataSAM+8, ucUID, 7);
	memcpy(dataSAM+15, "\xFF", 1);
	memcpy(dataSAM+16, "\x00\x00\x01", 3);
	memcpy(dataSAM+19, "\x00", 1);
	memcpy(dataSAM+20, "\x80\x00\x00\x00", 4);
	memcpy(dataSAM+24, ucRndNumber, 8);

	//hex_dump_char("dataSAM", dataSAM, 32);

	if(GetRandomNoFromSAM_0(dataSAM, ucRndSAM)==FALSE)
		return FALSE;

	//hex_dump_char("GetRandomNoFromSAM", ucRndSAM, 16);

	authen[0]= 0xAF;
	memcpy(authen+1, ucRndSAM, 16);

	//hex_dump_char("authen", authen, 17);
	
	if (AuthenticateBrizzi(authen, sesKey)!=TRUE)
		return FALSE;

	return TRUE;
}


//MIFARE DESFIRE
uchar AuthenticateKey0(uchar *ucCardNum, uchar *ucUID, uchar *sesKey)
{
	uchar ucRndSAM[32+1];
	uchar dataSAM[32+1];
	uchar authen[17+1];
	uchar ucRndNumber[8+1];

	memset(ucRndSAM, 0, sizeof(ucRndSAM));
	memset(dataSAM, 0, sizeof(dataSAM));
	memset(authen, 0, sizeof(authen));
	memset(ucRndNumber, 0, sizeof(ucRndNumber));
	
	if(GetRandomNoFromCard(ucRndNumber)!=TRUE)
		return FALSE;

	memcpy(glProcInfo.ucRndNumber, ucRndNumber, sizeof(glProcInfo.ucRndNumber));
	//hex_dump_char("GetRandomNoFromCard", ucRndNumber, 8);


	memcpy(dataSAM, ucCardNum, 8);
	memcpy(dataSAM+8, ucUID, 7);
	memcpy(dataSAM+15, "\xFF", 1);
	memcpy(dataSAM+16, "\x00\x00\x03", 3);
	memcpy(dataSAM+19, "\x00", 1);
	memcpy(dataSAM+20, "\x80\x00\x00\x00", 4);
	memcpy(dataSAM+24, ucRndNumber, 8);

	//hex_dump_char("dataSAM", dataSAM, 32);

	if(GetRandomNoFromSAM_0(dataSAM, ucRndSAM)==FALSE)
		return FALSE;

	//hex_dump_char("GetRandomNoFromSAM", ucRndSAM, 16);

	authen[0]= 0xAF;
	memcpy(authen+1, ucRndSAM, 16);

	//hex_dump_char("authen", authen, 17);
	
	if (AuthenticateBrizzi(authen, sesKey)!=TRUE)
		return FALSE;

	return TRUE;
}



//MIFARE DESFIRE
uchar AuthenticateKey1(uchar *DataFromHost, uchar *ucCardNum, uchar *ucUID, uchar *sesKey)
{
	uchar ucRndSAM[32+1];
	uchar dataSAM[55+1];
	uchar authen[17+1];
	uchar ucRndNumber[8+1];
//	uchar ucRndNumber2[8+1];
	uchar dataHost[24+1];
	uchar dataHost2[24+1];
//	int iLoop;
	int x=0;
	memset(ucRndSAM, 0, sizeof(ucRndSAM));
	memset(dataSAM, 0, sizeof(dataSAM));
	memset(authen, 0, sizeof(authen));
	memset(ucRndNumber, 0, sizeof(ucRndNumber));
	memset(dataHost, 0, sizeof(dataHost));
	memset(dataHost2, 0, sizeof(dataHost2));

	if(GetRandomNoFromCard_1(ucRndNumber)!=TRUE)
		return FALSE;

//	hex_dump_char("GetRandomNoFromCard", ucRndNumber, 8);

//	hex_dump_char("DataFromHost", DataFromHost, 48);

//Data From Host	
	hex2bin(dataHost, DataFromHost, 0);

//	hex_dump_char("DataFromHost bin", dataHost, 24);

	memcpy(dataSAM, dataHost, 24);

//Diversify Data
	memcpy(dataSAM+24, ucCardNum, 8);
	memcpy(dataSAM+32, ucUID, 7);
//Application Reference
	memcpy(dataSAM+39, "\x00\x00\x03", 3);
	memcpy(dataSAM+42, "\x01", 1);
	memcpy(dataSAM+43, "\x80\x00\x00\x00", 4);
//Random Desfire
	memcpy(dataSAM+47, ucRndNumber, 8);

//	hex_dump_char("AuthenticateKey1", dataSAM, 55);

	if(GetRandomNoFromSAM_1(dataSAM, ucRndSAM)==FALSE)
		return FALSE;

//	hex_dump_char("GetRandomNoFromSAM_1", ucRndSAM, 16);

	authen[0]= 0xAF;
	memcpy(authen+1, ucRndSAM, 16);

//	hex_dump_char("authen key 1", authen, 17);
	
	if (AuthenticateBrizzi1(authen, sesKey)!=TRUE)
		return FALSE;

	return TRUE;
}

uchar CheckLastTrxDate(LAST_TRX_AKUM_DEBIT *LastTrxAkumDebit)
{
	uchar szLogDateTrans[6+1];
	uchar szLogBulan[2+1];
	uchar szLogTahun[4+1];
	uint uiLogBulan;
	uint uiLogTahun;
	uchar szCurBulan[2+1];
	uchar szCurTahun[4+1];
	uint uiCurBulan;
	uint uiCurTahun;

	memset(szLogDateTrans, 0, sizeof(szLogDateTrans));
	memset(szLogBulan, 0, sizeof(szLogBulan));
	memset(szLogTahun, 0, sizeof(szLogTahun));
	memset(szCurBulan, 0, sizeof(szCurBulan));
	memset(szCurTahun, 0, sizeof(szCurTahun));

	bin2hex(szLogDateTrans, LastTrxAkumDebit->LastTrxDate, 3);

	memcpy(szLogTahun,szLogDateTrans,2);
	memcpy(szLogBulan,szLogDateTrans+2,2);
	
	uiLogBulan = atoi(szLogBulan);
	uiLogTahun =atoi(szLogTahun);

//	hex_dump_char("szLogBulan", szLogBulan, sizeof(szLogBulan));
//	hex_dump_char("szLogTahun", szLogTahun, sizeof(szLogTahun));

	memcpy(szCurBulan, glProcInfo.stTranLog.szDateTime+4,2);
	memcpy(szCurTahun, glProcInfo.stTranLog.szDateTime+2,2);
	uiCurBulan = atoi(szCurBulan);
	uiCurTahun =atoi(szCurTahun);

	mapPutStr(traPreMonth, szLogBulan);
	mapPutStr(traPreYear, szLogTahun);
	mapPutStr(traCurMonth, szCurBulan);
	mapPutStr(traCurYear, szCurTahun);

//	hex_dump_char("szCurBulan", szCurBulan, sizeof(szCurBulan));
//	hex_dump_char("uiCurTahun", szCurTahun, sizeof(szCurTahun));
	
		if((uiCurTahun - uiLogTahun)>1)	// > 1 year
		{
			//hex_dump_char("uiCurTahun", ">1", 2);
			return FALSE;
		}
		else if((uiCurTahun - uiLogTahun)==0) //same year
		{
			return TRUE;
		}
		else if((uiCurTahun - uiLogTahun)>0) // 1 year
		{
			//uiLogBulan+=12;
			uiCurBulan+=12;
			if(( uiCurBulan-uiLogBulan)>=12)
			{
				//hex_dump_char("uiCurTahun", ">0", 2);
				return FALSE;
			}
		}
		else	//error
			return FALSE;


	return TRUE;
}


uchar CheckAkumDebit(LAST_TRX_AKUM_DEBIT *LastTrxAkumDebit, ulong amt, ulong *cumAmt)
{
	uchar szLogDateTrans[6+1];
	uchar szLogBulan[2+1];
	uchar szLogTahun[4+1];
	uint uiLogBulan;
	uint uiLogTahun;
	uchar szCurBulan[2+1];
	uchar szCurTahun[4+1];
	uint uiCurBulan;
	uint uiCurTahun;
	ulong DebitFix;

	memset(szLogDateTrans, 0, sizeof(szLogDateTrans));
	memset(szLogBulan, 0, sizeof(szLogBulan));
	memset(szLogTahun, 0, sizeof(szLogTahun));
	memset(szCurBulan, 0, sizeof(szCurBulan));
	memset(szCurTahun, 0, sizeof(szCurTahun));

	bin2hex(szLogDateTrans, LastTrxAkumDebit->LastTrxDate, 3);

	memcpy(szLogBulan,szLogDateTrans+2,2);
	memcpy(szLogTahun,szLogDateTrans,2);
	uiLogBulan = atoi(szLogBulan);
	uiLogTahun =atoi(szLogTahun);

//	hex_dump_char("szLogBulan", szLogBulan, sizeof(szLogBulan));
//	hex_dump_char("szLogTahun", szLogTahun, sizeof(szLogTahun));

	memcpy(szCurBulan, glProcInfo.stTranLog.szDateTime+4,2);
	memcpy(szCurTahun, glProcInfo.stTranLog.szDateTime+2,2);
	uiCurBulan = atoi(szCurBulan);
	uiCurTahun =atoi(szCurTahun);

	mapPutStr(traPreMonth, szLogBulan);
	mapPutStr(traPreYear, szLogTahun);
	mapPutStr(traCurMonth, szCurBulan);
	mapPutStr(traCurYear, szCurTahun);

//	hex_dump_char("szCurBulan", szCurBulan, sizeof(szCurBulan));
//	hex_dump_char("uiCurTahun", szCurTahun, sizeof(szCurTahun));
	
	DebitFix= LastTrxAkumDebit->AkumulasiDebit[3] & 0x000000ff;
	DebitFix +=  ((LastTrxAkumDebit->AkumulasiDebit[2] << 8)&0x0000ff00);
	DebitFix += ((LastTrxAkumDebit->AkumulasiDebit[1] << 16)&0x00ff0000);
	DebitFix += ((LastTrxAkumDebit->AkumulasiDebit[0] << 24)&0xff000000);
//		hex_dump_char("DebitFix", &DebitFix, sizeof(DebitFix));
//		hex_dump_char("amt", &amt, sizeof(amt));
		
		if((uiLogBulan == uiCurBulan)&&(uiLogTahun == uiCurTahun))
		{
			*cumAmt = DebitFix+amt;
//			if((DebitFix+amt) >= 20000000)
			if((DebitFix+amt) >= glSysCtrl.ulMaxAkumDebit)
			{
				dspClear();
				dspLS(0, "Total Debit");
				dspLS(1, "Melebihi Batas");
				tmrPause(3);

				return FALSE;
			}	
		}
		else
			*cumAmt = amt;

	return TRUE;
}

uchar CheckAndValidateAmtBrizzi(ulong saldoAwal )
{
	uchar amt[lenAmt+1];
	int lenUang;
	ulong uang;

	memset(amt, 0, sizeof(amt));
	memcpy(amt, glProcInfo.stTranLog.szAmount, sizeof(glProcInfo.stTranLog.szAmount));
	lenUang = strlen(amt);

	dec2num(&uang , amt, (byte)lenUang);

	uang/=100;

	if (uang > saldoAwal)
	{
		dspClear();
		dspLS(1, "Saldo Anda");
		dspLS(2, "Tidak Cukup");
		PubBeepErr();
		PubWaitKey(10);
		return FALSE;
	}

	if((saldoAwal<glSysCtrl.ulMinSaldo)||((saldoAwal-uang)<glSysCtrl.ulMinSaldo))
	{
		dspClear();
		dspLS(1, "Sisa Saldo");
		dspLS(2, "Kurang Dari");
		dspLS(3, "Syarat Minimum");
		PubBeepErr();
		PubWaitKey(10);
		return FALSE;
	}
	
	return TRUE;
}

uchar CreateHashBrizzi(uchar *seskey, uchar *MacKey)
{
	uchar szAmt[12+1];
	uchar szDate[6+1];
	uchar szTime[6+1];
	uchar sCardNum[8+1];
	uchar szInvNum[6+1];
	uchar procCode[6+1];
	uchar BatNum[6+1];
	uchar dataHash[88+1];

	memset(szAmt, 0, sizeof(szAmt));
	memset(szDate, 0, sizeof(szDate));
	memset(szTime, 0, sizeof(szTime));
	memset(sCardNum, 0, sizeof(sCardNum));
	memset(szInvNum, 0, sizeof(szInvNum));
	memset(BatNum, 0, sizeof(BatNum));
	memset(dataHash, 0, sizeof(dataHash));
	
	memcpy(szAmt, glProcInfo.stTranLog.szAmount+2, 10);
//	fmtPad(szAmt, -10, '0');

	memcpy(szDate, glProcInfo.stTranLog.szDateTime+2, 6); 
	memcpy(szTime, glProcInfo.stTranLog.szDateTime+8, 6); 

//	hex_dump_char("szDateTime", glProcInfo.stTranLog.szDateTime, sizeof(glProcInfo.stTranLog.szDateTime));
//	hex_dump_char("szDate", szDate, sizeof(szDate));
//	hex_dump_char("szTime", szTime, sizeof(szTime));
	
//	bin2hex(sCardNum, glProcInfo.stTranLog.BRI.Brizzi.NoKartu, 8);
	hex2bin(sCardNum, glProcInfo.stTranLog.BRI.Brizzi.NoKartu, 16);

	
	sprintf(szInvNum, "%06lu", glProcInfo.stTranLog.ulInvoiceNo);

	sprintf((char *)procCode,   "%.*s", LEN_PROC_CODE,	glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);

	sprintf(BatNum, "%06lu", glCurTlmTmsAcq.ulCurBatchNo);
		
	// Diversify Data
	memcpy(dataHash,sCardNum,8);
	memcpy(dataHash+8,glProcInfo.stTranLog.BRI.Brizzi.UID,7);
	memcpy(dataHash+15,"\xFF",1);

	//Application ID
	memcpy(dataHash+16,"\x00\x00\x03",3);
	memcpy(dataHash+19,"\x00",1);
	memcpy(dataHash+20,"\x80\x00\x00\x00",4);

	//Random Desfire
	memcpy(dataHash+24,seskey,8);

	//HASH Data
	memcpy(dataHash+32,glProcInfo.stTranLog.BRI.Brizzi.NoKartu,16);
	memcpy(dataHash+48,szAmt,10);
	memcpy(dataHash+58,szDate,6);
	memcpy(dataHash+64,szTime,6);
	memcpy(dataHash+70,procCode,6);
	memcpy(dataHash+76,szInvNum,6);
	memcpy(dataHash+82,BatNum+4,2);
	memcpy(dataHash+84,"\xFF\xFF\xFF\xFF",4);

//	hex_dump_char("CreateHashBrizzi", dataHash, 88);
	if(uiGetHash(dataHash,MacKey)!=TRUE)
		return FALSE;

	return TRUE;

}


int CommitTransaction(void)
{
	uchar ucResult;
	int nCardResponse;
	unsigned char CardResponse[256];
	unsigned char Commit[]={0xC7};

	nCardResponse = 256;
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(1, Commit,&nCardResponse,CardResponse);
	if(ucResult!=0)
		return FALSE;

	if(CardResponse[0] != 0x00)
		return FALSE;

	return TRUE;
}

int WriteAkumulativDebitBrizzi(uchar *date, ulong cumAmt)
{
	uchar ucResult;
	int nCardResponse;
	unsigned char CardResponse[256];
	byte writeAkum[15+1];
	byte temp[4+1];


	memset(writeAkum, 0, sizeof(writeAkum));
	memset(CardResponse, 0, sizeof(CardResponse));
	memset(temp, 0, sizeof(temp));

	nCardResponse = 256;

	writeAkum[0] = 0x3D;
	writeAkum[1] = 0x03;
	writeAkum[2] = 0x00;
	writeAkum[3] = 0x00;
	writeAkum[4] = 0x00;
	writeAkum[5] = 0x07;
	writeAkum[6] = 0x00;
	writeAkum[7] = 0x00;


	temp[3] = (unsigned char) cumAmt& 0x000000ff;
	temp[2] = (unsigned char) (( cumAmt>> 8)&0x000000ff);
	temp[1] = (unsigned char) (( cumAmt>> 16)&0x000000ff);
	temp[0] = (unsigned char) (( cumAmt>> 24)&0x000000ff);

	memcpy(writeAkum+8,date,3);
	memcpy(writeAkum+11,temp,4);
					
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(15,writeAkum,&nCardResponse,CardResponse);
	if(ucResult!=0)
		return FALSE;

	if(CardResponse[0] != 0x00)
		return FALSE;

	return TRUE;
}


int WriteCloseStatusCard(CI_STATUS *CI_Status)
{
	uchar ucResult;
	int nCardResponse;
	unsigned char CardResponse[256];
	byte writeAkum[40+1];


	memset(writeAkum, 0, sizeof(writeAkum));
	memset(CardResponse, 0, sizeof(CardResponse));

	nCardResponse = 256;

	writeAkum[0] = 0x3D;
	writeAkum[1] = 0x01;
	writeAkum[2] = 0x00;
	writeAkum[3] = 0x00;
	writeAkum[4] = 0x00;
	writeAkum[5] = 0x20;
	writeAkum[6] = 0x00;
	writeAkum[7] = 0x00;



	memcpy(writeAkum+8,CI_Status->Activation_Date,3);
	memcpy(writeAkum+11,CI_Status->Status,2);
	memcpy(writeAkum+13,CI_Status->Interoperability,27);

//	hex_dump_char("WriteCloseStatusCard", writeAkum, sizeof(writeAkum));
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(40, writeAkum, &nCardResponse, CardResponse);
//	hex_dump_char("WriteCloseStatusCard ucResult ", &ucResult, sizeof(ucResult));
//	hex_dump_char("WriteCloseStatusCard", CardResponse, nCardResponse);
	if(ucResult!=0)
		return FALSE;

	if(CardResponse[0] != 0x00)
		return FALSE;

	return TRUE;
}

//MIFARE DESFIRE
int ReadLogDataBrizzi(byte *Data)
{
	uchar nResult;
	int nCardResponse;
	unsigned char CardResponse[256];
	unsigned char header1[8]={0xBB,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char BufData[1024];
	ushort pos=0;

	nCardResponse = 60;

	memset(BufData, 0, sizeof(BufData));
	memset(CardResponse, 0, sizeof(CardResponse));
	nResult = PiccCmdExchange(8, header1,&nCardResponse,CardResponse);
	if(nResult != 0)
		{goto lblKO;}

//	hex_dump_char("ReadLogDataBrizzi", CardResponse, nCardResponse);
	
	if((CardResponse[0]==0xAF)||(CardResponse[0]==0x00))
	{
		memcpy(&BufData[pos], CardResponse+1, nCardResponse-1);
		pos = nCardResponse-1;
		
		while(CardResponse[0]==0xAF)
		{
			nCardResponse = 60;
			memset(CardResponse, 0, sizeof(CardResponse));
			nResult = PiccCmdExchange(1,"\xAF",&nCardResponse,CardResponse);
			if(nResult != 0)
				{goto lblKO;}

//			hex_dump_char("ReadLogDataBrizzi", CardResponse, nCardResponse);
			
			if((CardResponse[0]==0xAF)||(CardResponse[0]==0x00))
			{
				memcpy(&BufData[pos], CardResponse+1, nCardResponse-1);
				pos+=(nCardResponse-1);
			}
			
		}
	}

//	hex_dump_char("ReadLogDataBrizzi", BufData, pos);
	
	memcpy(Data, BufData, pos);

	return pos;
lblKO:
	return-1;
}
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BrizziCheckBalance(void)
{
	int iRet;
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	uchar ucAmt[4+1];
	card amt;
	char amtTotal[20+1];
	uchar rndNumber[8+1];
	uchar CardNumHex[16+1];
	uchar ucKey=0;

	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;


	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(ucAmt, 0, sizeof(ucAmt));
	memset(amtTotal, 0, sizeof(amtTotal));
	memset(rndNumber, 0, sizeof(rndNumber));
	memset(CardNumHex, 0, sizeof(CardNumHex));

	iRet = TransInit(TRANS_TYPE_BRIZZI_INFO_SALDO);
	if( iRet!=0 )
	{
		return;
	}
	
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	ScrCls();
	PubDispString("PROCESSING", DISP_LINE_CENTER|2);
	
	if(SelectAID01()!=TRUE)
		goto lblErrInvalidCard;

	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		goto lblErrAccessingCard;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	memcpy(glProcInfo.stTranLog.szPan, CardNumHex, sizeof(CardNumHex));
	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}
	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, rndNumber)!=TRUE)
		goto lblErrAccessingCard;

#if 1
	memset(Data, 0, sizeof(Data));
	if(ReadLastTrxDateAkumDebit(Data)==TRUE)
	{
		ParseLastTrxDateAkumDebit(Data, &LastTrxAkumDebit);
	}
	else
		goto lblErrAccessingCard;
#endif


	if(getValueBrizzi(&amt,ucAmt) != TRUE) // For Info Saldo
		goto lblErrAccessingCard;

	num2dec(amtTotal, amt, 0);
	strcat(amtTotal, "00");
	App_ConvAmountTran(amtTotal, amtTotal, GA_SEPARATOR);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter, amtTotal, sizeof(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter));
	
	ScrCls();
	PubDispStringASCII("INFO SALDO", DISP_LINE_CENTER);
	PubDispStringASCII("NOMOR KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(CardNumHex, DISP_LINE_LEFT|3);
	PubDispStringASCII("SALDO :", DISP_LINE_LEFT|4);
	PubDispStringASCII(amtTotal, DISP_LINE_RIGHT|5);

	PubDispStringASCII("KELUAR   CETAK", DISP_LINE_RIGHT|7);

	PubBeepOk();
	ucKey= PubWaitKey(15);
	if((ucKey==KEYENTER)||(ucKey==KEYFN))
		BrizziPrintCheckBalance();

	return;
	
lblErrInvalidCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "INVALID CARD");
	PubBeepErr();
	tmrPause(3);
	return;
	
lblErrAccessingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "ACCESSING CARD");
	PubBeepErr();
	tmrPause(3);
	return;


}


void BrizziOfflinePayment(uchar Diskon)
{
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	int iRet;
	uchar CardNumHex[lenPan+1];
	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;
	uchar ucAmtBefore[4+1];
	uchar ucAmtAfter[4+1];
	ulong amtSaldo;
	ulong amt;
	ulong sisa;
	uchar uangDebit[4+1];
	uchar seskey[8+1];
	uchar MacKey[4+1];
	uchar trxType[1+1];
	uchar dataRecord[32+1];
	uchar dataAkum[7+1];
	uchar trxdate[6+1];
	uchar trxdateAsc[14+1];
	ulong cumAmt;
	char amtTotal[20+1];


	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(CardNumHex, 0, sizeof(CardNumHex));
	memset(&LastTrxAkumDebit, 0, sizeof(LAST_TRX_AKUM_DEBIT));
	memset(ucAmtBefore, 0, sizeof(ucAmtBefore));
	memset(ucAmtAfter, 0, sizeof(ucAmtAfter));
	memset(uangDebit, 0, sizeof(uangDebit));
	memset(seskey, 0, sizeof(seskey));
	memset(MacKey, 0, sizeof(MacKey));
	memset(trxType, 0, sizeof(trxType));
	memset(dataRecord, 0, sizeof(dataRecord));
	memset(dataAkum, 0, sizeof(dataAkum));
	memset(trxdate, 0, sizeof(trxdate));
	memset(amtTotal, 0, sizeof(amtTotal));
	
	//connect to SAM
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	iRet = TransInit(TRANS_TYPE_BRIZZI_PURCHASE);
	if( iRet!=0 )
	{
		return;
	}

	//CHECK BATCH
	if(!ChkIfBatchBrizziFull())
	{
		return;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return;
	}

	//GET AMOUNT
	iRet = GetAmountBrizzi();
	if( iRet!=0 )
	{
		return;
	}

	if(Diskon)
	{
		if(glSysCtrl.szAmtDiskonBrizzi[0] != 0)
			PubAscSub(glProcInfo.stTranLog.szAmount, glSysCtrl.szAmtDiskonBrizzi, 12, glProcInfo.stTranLog.szAmount);
		else if(glSysCtrl.ulDiskonBrizzi != 0)
		{
				amt=0;
				dec2num(&amt , glProcInfo.stTranLog.szAmount, (uchar)strlen(glProcInfo.stTranLog.szAmount));
				amt/=100;

				amt=amt-((amt*glSysCtrl.ulDiskonBrizzi)/100);

				sprintf(glProcInfo.stTranLog.szAmount, "%lu", amt);
				AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
		}
	}

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	if(SelectAID01()!=TRUE)
		goto lblErrInvalidCard;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		goto lblErrAccessingCard;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	memcpy(glProcInfo.stTranLog.szPan, CardNumHex, sizeof(CardNumHex));
	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		goto lblErrAccessingCard;

	//CHECK STATUS
	if ((CI_Status.Status[0] == 0x63)&&(CI_Status.Status[1]==0x6C))
		goto lblClose;


	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;
	
	memset(Data, 0, sizeof(Data));
	if(ReadLastTrxDateAkumDebit(Data)==TRUE)
	{
		ParseLastTrxDateAkumDebit(Data, &LastTrxAkumDebit);
	}
	else
		goto lblErrAccessingCard;

	//CHECK LAST TRX DATE
	if(CheckLastTrxDate(&LastTrxAkumDebit)!=TRUE)
	{
		goto lblPasif;
	}

	dec2num(&amt , glProcInfo.stTranLog.szAmount, (uchar)strlen(glProcInfo.stTranLog.szAmount));
	amt/=100;

	//CHECK AKUM DEBIT
	if(CheckAkumDebit(&LastTrxAkumDebit, amt, &cumAmt)!=TRUE)
	{
		goto lblMaxLimit;
	}

	if(getValueBrizzi(&amtSaldo,ucAmtBefore) != TRUE)
		goto lblErrAccessingCard;
	
	if(CheckAndValidateAmtBrizzi(amtSaldo)!=TRUE)
		return;

	sisa = amtSaldo-amt;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter,"%lu", sisa);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
		
	if(DebitBrizzi(amt, uangDebit)!=TRUE)
		goto lblErrAccessingCard;
	
	if(CreateHashBrizzi(seskey, MacKey)!=TRUE)
		goto lblErrAccessingCard;

	bin2hex(glProcInfo.stTranLog.BRI.Brizzi.Hash, MacKey, 4);
		
	glProcInfo.stTranLog.ulInvoiceNo = glSysCtrl.ulInvoiceNo;

	strcpy(trxType,"\xeb");

	memset(trxdateAsc, 0, sizeof(trxdateAsc));
	
	PubAsc2Bcd(glProcInfo.stTranLog.szDateTime, 14, trxdateAsc);
	
	trxdate[0] = trxdateAsc[3];//dd
	trxdate[1] = trxdateAsc[2];//mm
	trxdate[2] = trxdateAsc[1];//yy
	
	memcpy(trxdate+3, trxdateAsc+4, 3);//hhmmss

	
	MEM_ZERO(dataRecord);

	memcpy(dataRecord,glCurTlmTmsAcq.acqMID+7,8);
	memcpy(dataRecord+8,glCurTlmTmsAcq.acqTID,8);
	memcpy(dataRecord+16,trxdate,6);
	memcpy(dataRecord+22,trxType,1);
	memcpy(dataRecord+23,uangDebit,3);
	memcpy(dataRecord+26,ucAmtBefore,3);

	ucAmtAfter[0] = (unsigned char) sisa& 0x000000ff;
	ucAmtAfter[1] = (unsigned char) (( sisa >> 8)&0x000000ff);
	ucAmtAfter[2] = (unsigned char) (( sisa >> 16)&0x000000ff);
	ucAmtAfter[3] = (unsigned char) (( sisa >> 24)&0x000000ff);

	memcpy(dataRecord+29,ucAmtAfter,3);
	
	if( WriteRecordBrizzi(dataRecord)!= TRUE)
		goto lblErrAccessingCard;

	MEM_ZERO(dataAkum);
	memcpy(dataAkum, trxdateAsc+1, 3);

	if(WriteAkumulativDebitBrizzi(dataAkum, cumAmt) != TRUE)
		goto lblErrAccessingCard;

	if(CommitTransaction()!=TRUE)
		goto lblErrUpdatingCard;

//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveBrizziTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	num2dec(amtTotal, sisa, 0);
	strcat(amtTotal, "00");
	App_ConvAmountTran(amtTotal, amtTotal, GA_SEPARATOR);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter, amtTotal, sizeof(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter));

	ScrCls();
	PubDispStringASCII("PEMBAYARAN BRIZZI", DISP_LINE_CENTER);
	PubDispStringASCII("NOMOR KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(CardNumHex, DISP_LINE_LEFT|3);
	PubDispStringASCII("SISA SALDO :", DISP_LINE_LEFT|4);
	PubDispStringASCII(amtTotal, DISP_LINE_RIGHT|5);
	PubBeepOk();
	BrizziPrintOfflineBrizzi(PRN_NORMAL);
	return;

lblErrInvalidCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "INVALID CARD");
	PubBeepErr();
	tmrPause(3);
	return;
	
lblErrAccessingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "ACCESSING CARD");
	PubBeepErr();
	tmrPause(3);
	return;

lblErrUpdatingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "UPDATING CARD");
	PubBeepErr();
	tmrPause(3);
	return;
	
lblClose:
	dspClear();
	dspLS(1, "KARTU CLOSE");
	PubBeepErr();
	tmrPause(3);
	return;

lblPasif:
	dspClear();
	dspLS(1, "KARTU PASIF");
	PubBeepErr();
	tmrPause(3);
	return;

lblMaxLimit:
	dspClear();
	dspLS(1, "MAKSIMUM");
	dspLS(2, "LIMIT BULANAN");
	PubBeepErr();
	tmrPause(3);
	return;

}

void BrizziTopUpOnLine(void)
{
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;
	int iRet;
	uchar CardNumHex[lenPan+1];
	uchar ucAmtBefore[4+1];
	uchar ucAmtAfter[4+1];
	ulong amtSaldo;
	ulong amt;
	ulong sisa;
	uchar uangCredit[4+1];
	uchar seskey[8+1];
	uchar MacKey[4+1];
	uchar trxType[1+1];
	uchar dataRecord[28+1];
	uchar dataAkum[7+1];
	uchar trxdate[6+1];
	uchar rndKey[24+1];
	uchar trxdateAsc[14+1];
	ulong cumAmt=0;
	uchar UID[11+1];
	char amtTotal[20+1];
	uchar flagAutoRev=FALSE;


	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(CardNumHex, 0, sizeof(CardNumHex));
	memset(&LastTrxAkumDebit, 0, sizeof(LAST_TRX_AKUM_DEBIT));
	memset(ucAmtBefore, 0, sizeof(ucAmtBefore));
	memset(ucAmtAfter, 0, sizeof(ucAmtAfter));
	memset(uangCredit, 0, sizeof(uangCredit));
	memset(seskey, 0, sizeof(seskey));
	memset(MacKey, 0, sizeof(MacKey));
	memset(trxType, 0, sizeof(trxType));
	memset(dataRecord, 0, sizeof(dataRecord));
	memset(dataAkum, 0, sizeof(dataAkum));
	memset(trxdate, 0, sizeof(trxdate));
	memset(rndKey, 0, sizeof(rndKey));
	memset(UID, 0, sizeof(UID));
	memset(amtTotal, 0, sizeof(amtTotal));

	
	//connect to SAM
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	iRet = TransInit(TRANS_TYPE_BRIZZI_TOPUP_ONLINE);
	if( iRet!=0 )
	{
		return;
	}

	iRet = GetCard(CARD_SWIPED);
	if( iRet!=0 )
	{
		return ;
	}

	//CHECK BATCH
	if(!ChkIfBatchBrizziFull())
	{
		return;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return;
	}

	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return ;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return ;
	}

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	DispProcess();

	memcpy(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID));
	if(SelectAID01()!=TRUE)
		goto lblErrInvalidCard;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		goto lblErrAccessingCard;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		goto lblErrAccessingCard;

	//CHECK STATUS
	if ((CI_Status.Status[0] == 0x63)&&(CI_Status.Status[1]==0x6C))
		goto lblClose;


	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;
	
	memset(Data, 0, sizeof(Data));
	if(ReadLastTrxDateAkumDebit(Data)==TRUE)
	{
		ParseLastTrxDateAkumDebit(Data, &LastTrxAkumDebit);
	}
	else
		goto lblErrAccessingCard;

	CheckAkumDebit(&LastTrxAkumDebit, 0, &cumAmt);	//don't be check (get cumAmt only)

	//CHECK LAST TRX DATE
	if(CheckLastTrxDate(&LastTrxAkumDebit)!=TRUE)
	{
		goto lblPasif;
	}

	if(getValueBrizzi(&amtSaldo,ucAmtBefore) != TRUE)
		goto lblErrAccessingCard;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore,"%lu", amtSaldo);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	
	if(GenerateRandom(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, glProcInfo.rndKeyBrizzi)!=TRUE)
		goto lblErrAccessingCard;

	//SET ISO8583
	setPackBrizziTopUpOnline();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	flagAutoRev = TRUE;

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
				return;
			}
		}
		return;
	}

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

	unPackTopUpOnLineBrizzi();

	//Get Version (MIFARE DESFIRE) / TAP CARD
	memset(glProcInfo.stTranLog.BRI.Brizzi.UID,0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.UID));
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		goto lblAutoReversal;
	
	DispProcess();
	
	if(memcmp(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID))!=0)
		goto lblErrInvalidCard;
	
	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	memset(seskey, 0, sizeof(seskey));
	if(AuthenticateKey1(glProcInfo.TopUpKeyBrizzi, CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;

	dec2num(&amt , glProcInfo.stTranLog.szAmount, (uchar)strlen(glProcInfo.stTranLog.szAmount));
	amt/=100;

	sisa = amtSaldo+amt;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter,"%lu", sisa);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
	
	if(CreditBrizzi(amt, uangCredit)!=TRUE)
		goto lblErrAccessingCard;


	strcpy(trxType,"\xec");

	memset(trxdateAsc, 0, sizeof(trxdateAsc));
	PubAsc2Bcd(glProcInfo.stTranLog.szDateTime, 14, trxdateAsc);
	
	trxdate[0] = trxdateAsc[3];//dd
	trxdate[1] = trxdateAsc[2];//mm
	trxdate[2] = trxdateAsc[1];//yy
	
	memcpy(trxdate+3, trxdateAsc+4, 3);//hhmmss

	
	MEM_ZERO(dataRecord);
	memcpy(dataRecord,glCurTlmTmsAcq.acqMID+7,8);
	memcpy(dataRecord+8,glCurTlmTmsAcq.acqTID,8);
	memcpy(dataRecord+16,trxdate,6);
	memcpy(dataRecord+22,trxType,1);
	memcpy(dataRecord+23,uangCredit,3);
	memcpy(dataRecord+26,ucAmtBefore,3);

	ucAmtAfter[0] = (unsigned char) sisa& 0x000000ff;
	ucAmtAfter[1] = (unsigned char) (( sisa >> 8)&0x000000ff);
	ucAmtAfter[2] = (unsigned char) (( sisa >> 16)&0x000000ff);
	ucAmtAfter[3] = (unsigned char) (( sisa >> 24)&0x000000ff);

	memcpy(dataRecord+29,ucAmtAfter,3);
	
	if( WriteRecordBrizzi(dataRecord)!= TRUE)
		goto lblErrAccessingCard;

	MEM_ZERO(dataAkum);
	memcpy(dataAkum, trxdateAsc+1, 3);

	if(WriteAkumulativDebitBrizzi(dataAkum, cumAmt) != TRUE)
		goto lblErrAccessingCard;

	if(CommitTransaction()!=TRUE)
		goto lblErrUpdatingCard;

//SAVE DATA
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveBrizziTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	SaveRevInfo(FALSE);//CLEAR REVERSAL

	num2dec(amtTotal, sisa, 0);
	strcat(amtTotal, "00");
	App_ConvAmountTran(amtTotal, amtTotal, GA_SEPARATOR);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter, amtTotal, sizeof(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter));

	ScrCls();
	PubDispStringASCII("TOPUP ONLINE BRIZZI", DISP_LINE_CENTER);
	PubDispStringASCII("NOMOR KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(CardNumHex, DISP_LINE_LEFT|3);
	PubDispStringASCII("SALDO ANDA :", DISP_LINE_LEFT|4);
	PubDispStringASCII(amtTotal, DISP_LINE_RIGHT|5);
	PubBeepOk();
	BrizziPrintTopupOnlineBrizzi(PRN_NORMAL);
	return;

lblErrInvalidCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "INVALID CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;
	
lblErrAccessingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "ACCESSING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblErrUpdatingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "UPDATING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblClose:
	dspClear();
	dspLS(1, "KARTU CLOSE");
	PubBeepErr();
	tmrPause(3);
	return;

lblPasif:
	dspClear();
	dspLS(1, "KARTU PASIF");
	PubBeepErr();
	tmrPause(3);
	return;

lblAutoReversal:
	//SEND REVERSAL
	if(flagAutoRev)
		TranReversal();


}


void BrizziAktivasiDeposit(void)
{
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	int iRet;
	uchar CardNumHex[lenPan+1];
	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;
	uchar ucAmtBefore[4+1];
	uchar ucAmtAfter[4+1];
	ulong amtSaldo;
	ulong amt;
	ulong sisa;
	uchar uangCredit[4+1];
	uchar seskey[8+1];
	uchar MacKey[4+1];
	uchar trxType[1+1];
	uchar dataRecord[28+1];
	uchar dataAkum[7+1];
	uchar trxdate[6+1];
	uchar rndKey[24+1];
	uchar trxdateAsc[14+1];
	ulong cumAmt=0;
	uchar UID[11+1];
	char amtTotal[20+1];
	uchar flagAutoRev=FALSE;

	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(CardNumHex, 0, sizeof(CardNumHex));
	memset(&LastTrxAkumDebit, 0, sizeof(LAST_TRX_AKUM_DEBIT));
	memset(ucAmtBefore, 0, sizeof(ucAmtBefore));
	memset(ucAmtAfter, 0, sizeof(ucAmtAfter));
	memset(uangCredit, 0, sizeof(uangCredit));
	memset(seskey, 0, sizeof(seskey));
	memset(MacKey, 0, sizeof(MacKey));

	memset(trxType, 0, sizeof(trxType));
	memset(dataRecord, 0, sizeof(dataRecord));
	memset(dataAkum, 0, sizeof(dataAkum));
	memset(trxdate, 0, sizeof(trxdate));
	memset(rndKey, 0, sizeof(rndKey));
	memset(UID, 0, sizeof(UID));
	memset(amtTotal, 0, sizeof(amtTotal));
	
	//connect to SAM
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	iRet = TransInit(TRANS_TYPE_BRIZZI_AKTV_DEPOS);
	if( iRet!=0 )
	{
		return;
	}

	//CHECK BATCH
	if(!ChkIfBatchBrizziFull())
	{
		return;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return;
	}

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	DispProcess();

	memcpy(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID));
	if(SelectAID01()!=TRUE)
		goto lblErrInvalidCard;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		goto lblErrAccessingCard;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		goto lblErrAccessingCard;

	//CHECK STATUS
	if ((CI_Status.Status[0] == 0x63)&&(CI_Status.Status[1]==0x6C))
		{goto lblClose;}


	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;
	
	memset(Data, 0, sizeof(Data));
	if(ReadLastTrxDateAkumDebit(Data)==TRUE)
	{
		ParseLastTrxDateAkumDebit(Data, &LastTrxAkumDebit);
	}
	else
		goto lblErrAccessingCard;

	CheckAkumDebit(&LastTrxAkumDebit, 0, &cumAmt);	//don't be check (get cumAmt only)
	
	//CHECK LAST TRX DATE
	if(CheckLastTrxDate(&LastTrxAkumDebit)!=TRUE)
	{
		goto lblPasif;
	}

	if(getValueBrizzi(&amtSaldo,ucAmtBefore) != TRUE)
		goto lblErrAccessingCard;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore,"%lu", amtSaldo);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	
	if(GenerateRandom(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, glProcInfo.rndKeyBrizzi)!=TRUE)
		goto lblErrAccessingCard;

	//SET ISO8583
	setPackBrizziAktvDeposit();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	flagAutoRev = TRUE;
	
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
				return;
			}
		}
		return;
	}


	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

	unPackAktivasiDepositBrizzi();

	//Get Version (MIFARE DESFIRE) / TAP CARD
	memset(glProcInfo.stTranLog.BRI.Brizzi.UID,0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.UID));
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		goto lblAutoReversal;
	
	DispProcess();
	
	if(memcmp(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID))!=0)
		goto lblErrAccessingCard;
	
	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	memset(seskey, 0, sizeof(seskey));
	if(AuthenticateKey1(glProcInfo.TopUpKeyBrizzi, CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;

	dec2num(&amt , glProcInfo.stTranLog.szAmount, (uchar)strlen(glProcInfo.stTranLog.szAmount));
	amt/=100;

	sisa = amtSaldo+amt;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter,"%lu", sisa);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);


	if(CreditBrizzi(amt, uangCredit)!=TRUE)
		goto lblErrAccessingCard;

	strcpy(trxType,"\xef");

	memset(trxdateAsc, 0, sizeof(trxdateAsc));
	
	PubAsc2Bcd(glProcInfo.stTranLog.szDateTime, 14, trxdateAsc);
	
	trxdate[0] = trxdateAsc[3];//dd
	trxdate[1] = trxdateAsc[2];//mm
	trxdate[2] = trxdateAsc[1];//yy
	
	memcpy(trxdate+3, trxdateAsc+4, 3);//hhmmss

	
	MEM_ZERO(dataRecord);
	memcpy(dataRecord,glCurTlmTmsAcq.acqMID+7,8);
	memcpy(dataRecord+8,glCurTlmTmsAcq.acqTID,8);
	memcpy(dataRecord+16,trxdate,6);
	memcpy(dataRecord+22,trxType,1);
	memcpy(dataRecord+23,uangCredit,3);
	memcpy(dataRecord+26,ucAmtBefore,3);

	ucAmtAfter[0] = (unsigned char) sisa& 0x000000ff;
	ucAmtAfter[1] = (unsigned char) (( sisa >> 8)&0x000000ff);
	ucAmtAfter[2] = (unsigned char) (( sisa >> 16)&0x000000ff);
	ucAmtAfter[3] = (unsigned char) (( sisa >> 24)&0x000000ff);

	memcpy(dataRecord+29,ucAmtAfter,3);
	
	if( WriteRecordBrizzi(dataRecord)!= TRUE)
		goto lblErrAccessingCard;


	MEM_ZERO(dataAkum);
	memcpy(dataAkum, trxdateAsc+1, 3);

	if(WriteAkumulativDebitBrizzi(dataAkum, cumAmt) != TRUE)
		goto lblErrAccessingCard;

	if(CommitTransaction()!=TRUE)
		goto lblErrUpdatingCard;

	//SAVE DATA
	if( ChkIfSaveLog() )
	{
		SaveBrizziTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	SaveRevInfo(FALSE);//CLEAR REVERSAL

	num2dec(amtTotal, sisa, 0);
	strcat(amtTotal, "00");
	App_ConvAmountTran(amtTotal, amtTotal, GA_SEPARATOR);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter, amtTotal, sizeof(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter));

	ScrCls();
	PubDispStringASCII("AKTIVASI DEPOSIT", DISP_LINE_CENTER);
	PubDispStringASCII("NOMOR KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(CardNumHex, DISP_LINE_LEFT|3);
	PubDispStringASCII("SALDO :", DISP_LINE_LEFT|4);
	PubDispStringASCII(amtTotal, DISP_LINE_RIGHT|5);
	PubBeepOk();
	BrizziPrintAktivasiDepositBrizzi(PRN_NORMAL);
	return;

lblErrInvalidCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "INVALID CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;
	
lblErrAccessingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "ACCESSING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblErrUpdatingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "UPDATING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblClose:
	dspClear();
	dspLS(1, "KARTU CLOSE");
	PubBeepErr();
	tmrPause(3);
	return;

lblPasif:
	dspClear();
	dspLS(1, "KARTU PASIF");
	PubBeepErr();
	tmrPause(3);
	return;


lblAutoReversal:
	//SEND REVERSAL
	if(flagAutoRev)
		TranReversal();

}


void BrizziInfoDeposit(void)
{
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	int iRet;
	uchar CardNumHex[lenPan+1];
	uchar amtTotal[20+1];
	uchar ucKey=0;


	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(CardNumHex, 0, sizeof(CardNumHex));
	memset(amtTotal, 0, sizeof(amtTotal));
	
	//connect to SAM
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	iRet = TransInit(TRANS_TYPE_BRIZZI_INFO_DEPOS);
	if( iRet!=0 )
	{
		return;
	}

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	if(SelectAID01()!=TRUE)
		return;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		return;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	memcpy(glProcInfo.stTranLog.szPan, CardNumHex, sizeof(CardNumHex));
	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		return;

	//CHECK STATUS
	if ((CI_Status.Status[0] == 0x63)&&(CI_Status.Status[1]==0x6C))
		{goto lblClose;}


	//SET ISO8583
	setPackBrizziInfoDeposit();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		return ;
	}

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);

		DispResult(ERR_HOST_REJ);
		
		return ;
	}

	unPackInfoDepositBrizzi();


	App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, amtTotal, GA_SEPARATOR);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.PrintAmount, amtTotal, sizeof(glProcInfo.stTranLog.BRI.Brizzi.PrintAmount));
	
	ScrCls();
	PubDispStringASCII("INFO DEPOSIT", DISP_LINE_CENTER);
	PubDispStringASCII("NOMOR KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(CardNumHex, DISP_LINE_LEFT|3);
	PubDispStringASCII("SALDO DEPOSIT :", DISP_LINE_LEFT|4);
	PubDispStringASCII(amtTotal, DISP_LINE_RIGHT|5);

	PubDispStringASCII("KELUAR   CETAK", DISP_LINE_RIGHT|7);

	PubBeepOk();
	ucKey= PubWaitKey(15);
	if((ucKey==KEYENTER)||(ucKey==KEYFN))
		BrizziPrintInfoDeposit();

	return;
	
lblClose:
	dspClear();
	dspLS(1, "KARTU CLOSE");
	PubBeepErr();
	tmrPause(3);
	return;

}


void BrizziTopUpDeposit(void)
{
	int iRet;
	uchar ucRet;
	uchar CardNum[16+1];
	
	//connect to SAM
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	iRet = TransInit(TRANS_TYPE_BRIZZI_TOPUP_DEP);
	if( iRet!=0 )
	{
		return;
	}

	iRet = GetCard(CARD_SWIPED);
	if( iRet!=0 )
	{
		return ;
	}

	//CHECK BATCH
	if(!ChkIfBatchBrizziFull())
	{
		return;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return;
	}

	//GET AMOUNT
	iRet = GetAmount();
	if( iRet!=0 )
	{
		return ;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return ;
	}

	//INPUT NOMOR BRIZZI
	ScrCls();
	DispTransName();
	ScrPrint(0, 4, CFONT, "%s", "NO. KARTU BRIZZI");
	ucRet = PubGetString(NUM_IN, 16,16 , CardNum, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return;
	}
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNum, sizeof(CardNum));


	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	//SET ISO8583
	setPackBrizziTopUpDeposit();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	//RECV MESSAGE FROM HOST
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
				return;
			}
		}
		return;
	}
	


	
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

//SAVE DATA
	if( ChkIfSaveLog() )
	{
		SaveBrizziTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	SaveRevInfo(FALSE);//CLEAR REVERSAL

	BrizziPrintTopupDeposit(PRN_NORMAL);
	return;
}

void BrizziVoid(void)
{
	int			iRet;
	uchar		ucTranAct, szTempAmt[12+1];
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	uchar CardNumHex[lenPan+1];
	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;
	uchar ucAmtBefore[4+1];
	uchar ucAmtAfter[4+1];
	ulong amtSaldo;
	ulong amt;
	ulong sisa;
	uchar uangCredit[4+1];
	uchar seskey[8+1];
	uchar MacKey[4+1];

	uchar trxType[1+1];
	uchar dataRecord[28+1];
	uchar dataAkum[7+1];
	uchar trxdate[6+1];

	uchar rndKey[24+1];

	uchar trxdateAsc[14+1];
	ulong cumAmt=0;
	uchar UID[11+1];
	char amtTotal[20+1];
	uchar flagAutoRev=FALSE;



	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(CardNumHex, 0, sizeof(CardNumHex));
	memset(&LastTrxAkumDebit, 0, sizeof(LAST_TRX_AKUM_DEBIT));
	memset(ucAmtBefore, 0, sizeof(ucAmtBefore));
	memset(ucAmtAfter, 0, sizeof(ucAmtAfter));
	memset(uangCredit, 0, sizeof(uangCredit));
	memset(seskey, 0, sizeof(seskey));
	memset(MacKey, 0, sizeof(MacKey));
	
	memset(trxType, 0, sizeof(trxType));
	memset(dataRecord, 0, sizeof(dataRecord));
	memset(dataAkum, 0, sizeof(dataAkum));
	memset(trxdate, 0, sizeof(trxdate));
	memset(rndKey, 0, sizeof(rndKey));
	memset(UID, 0, sizeof(UID));
	memset(amtTotal, 0, sizeof(amtTotal));


	iRet = TransInit(TRANS_TYPE_BRIZZI_VOID_REQ);
	if( iRet!=0 )
	{
		return;
	}

	ScrCls();
	DispTransName();
	if( !ChkEdcOption(EDC_NOT_VOID_PWD) )
	{
		if( PasswordVoid()!=0 )
		{
			return;
		}
	}

	while( 1 )
	{
		iRet = GetBrizziRecord(TS_OK|TS_NOSEND|TS_ADJ, &glProcInfo.stTranLog);
		if( iRet!=0 )
		{
			return;
		}

		if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_BRIZZI_PURCHASE)
		{
			ucTranAct = glTranConfig[glProcInfo.stTranLog.ucTranType].ucTranAct;
			if( ucTranAct & VOID_ALLOW )
			{
				glProcInfo.stTranLog.ucOrgTranType = glProcInfo.stTranLog.ucTranType;
				glProcInfo.stTranLog.ucTranType    = TRANS_TYPE_BRIZZI_VOID_REQ;
				break;
			}
		}
		PubDispString(_T("NOT ALLOW VOID"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	ScrCls();
	DispTransName();

	if( !ChkSettle() )
	{
		return;
	}

	//connect to SAM
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	ScrCls();
	DispTransName();
	DispWait();
	PreDial();

	PubDispString(_T("TOTAL"), 4|DISP_LINE_LEFT);
	PubAscAdd(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.szTipAmount, 12, szTempAmt);
	DispAmount(4, szTempAmt);	// show total amount
	PubDispString(_T("VOID ? Y/N"), DISP_LINE_LEFT|6);
	if( PubYesNo(USER_OPER_TIMEOUT) )
	{
		return;
	}

	ScrCls();
	DispTransName();

	setPackBrizziVoidReq();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		return ;
	}

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

	//UPDATE SALDO
	iRet = TransInit(TRANS_TYPE_BRIZZI_VOID);
	if( iRet!=0 )
	{
		return;
	}
    
	//Get Version (MIFARE DESFIRE) / TAP CARD
//	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
	if(GetVersionBrizzi(UID)!=TRUE)
		return;

	DispProcess();

	if(memcmp(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID))!=0)
		goto lblErrInvalidCard;

	memcpy(glProcInfo.stTranLog.BRI.Brizzi.UID, UID, sizeof(UID));
	
	if(SelectAID01()!=TRUE)
		goto lblErrInvalidCard;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		goto lblErrAccessingCard;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		goto lblErrAccessingCard;

	//CHECK STATUS
	if ((CI_Status.Status[0] == 0x63)&&(CI_Status.Status[1]==0x6C))
		goto lblClose;


	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;
	
	memset(Data, 0, sizeof(Data));
	if(ReadLastTrxDateAkumDebit(Data)==TRUE)
	{
		ParseLastTrxDateAkumDebit(Data, &LastTrxAkumDebit);
	}
	else
		goto lblErrAccessingCard;

	CheckAkumDebit(&LastTrxAkumDebit, 0, &cumAmt);	//don't be check (get cumAmt only)

	//CHECK LAST TRX DATE
	if(CheckLastTrxDate(&LastTrxAkumDebit)!=TRUE)
	{
		goto lblPasif;
	}

	if(getValueBrizzi(&amtSaldo,ucAmtBefore) != TRUE)
		goto lblErrAccessingCard;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore,"%lu", amtSaldo);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	
	if(GenerateRandom(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, glProcInfo.rndKeyBrizzi)!=TRUE)
		goto lblErrAccessingCard;

	//SET ISO8583
	setPackBrizziTopUpOnline();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	 flagAutoRev=TRUE;
	 
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
				return;
			}
		}
		return;
	}

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

	unPackTopUpOnLineBrizzi();

	//Get Version (MIFARE DESFIRE) / TAP CARD
	memset(glProcInfo.stTranLog.BRI.Brizzi.UID,0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.UID));
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		goto lblAutoReversal;
	
	DispProcess();
	
	if(memcmp(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID))!=0)
		goto lblErrInvalidCard;
	
	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	memset(seskey, 0, sizeof(seskey));
	if(AuthenticateKey1(glProcInfo.TopUpKeyBrizzi, CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;


	dec2num(&amt , glProcInfo.stTranLog.szAmount, (uchar)strlen(glProcInfo.stTranLog.szAmount));
	amt/=100;

	sisa = amtSaldo+amt;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter,"%lu", sisa);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	if(CreditBrizzi(amt, uangCredit)!=TRUE)
		goto lblErrAccessingCard;

	amt=0;
	if(getValueBrizzi(&amt, ucAmtAfter) != TRUE) 
		goto lblErrAccessingCard;

	strcpy(trxType,"\xed");

	memset(trxdateAsc, 0, sizeof(trxdateAsc));
	
	PubAsc2Bcd(glProcInfo.stTranLog.szDateTime, 14, trxdateAsc);
	
	trxdate[0] = trxdateAsc[3];//dd
	trxdate[1] = trxdateAsc[2];//mm
	trxdate[2] = trxdateAsc[1];//yy
	
	memcpy(trxdate+3, trxdateAsc+4, 3);//hhmmss

	
	MEM_ZERO(dataRecord);
	memcpy(dataRecord,glCurTlmTmsAcq.acqMID+7,8);
	memcpy(dataRecord+8,glCurTlmTmsAcq.acqTID,8);
	memcpy(dataRecord+16,trxdate,6);
	memcpy(dataRecord+22,trxType,1);
	memcpy(dataRecord+23,uangCredit,3);
	memcpy(dataRecord+26,ucAmtBefore,3);

	ucAmtAfter[0] = (unsigned char) sisa& 0x000000ff;
	ucAmtAfter[1] = (unsigned char) (( sisa >> 8)&0x000000ff);
	ucAmtAfter[2] = (unsigned char) (( sisa >> 16)&0x000000ff);
	ucAmtAfter[3] = (unsigned char) (( sisa >> 24)&0x000000ff);

	memcpy(dataRecord+29,ucAmtAfter,3);
	
	if( WriteRecordBrizzi(dataRecord)!= TRUE)
		goto lblErrAccessingCard;

	MEM_ZERO(dataAkum);
	memcpy(dataAkum, trxdateAsc+1, 3);

	if(WriteAkumulativDebitBrizzi(dataAkum, cumAmt) != TRUE)
		goto lblErrAccessingCard;
	
	if(CommitTransaction()!=TRUE)
		goto lblErrUpdatingCard;

	glProcInfo.stTranLog.uiStatus |= TS_VOID;
	UpdateBrizziTranLog(&glProcInfo.stTranLog, glProcInfo.uiRecNo);

	SaveRevInfo(FALSE);//CLEAR REVERSAL

	num2dec(amtTotal, sisa, 0);
	strcat(amtTotal, "00");
	App_ConvAmountTran(amtTotal, amtTotal, GA_SEPARATOR);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter, amtTotal, sizeof(glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter));

	
	ScrCls();
	PubDispStringASCII("VOID BRIZZI", DISP_LINE_CENTER);
	PubDispStringASCII("NOMOR KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(CardNumHex, DISP_LINE_LEFT|3);
	PubDispStringASCII("SALDO ANDA :", DISP_LINE_LEFT|4);
	PubDispStringASCII(amtTotal, DISP_LINE_RIGHT|5);
	PubBeepOk();
	BrizziPrintVoidBrizzi(PRN_NORMAL);
	return;


lblErrInvalidCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "INVALID CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;
	
lblErrAccessingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "ACCESSING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblErrUpdatingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "UPDATING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblClose:
	dspClear();
	dspLS(1, "KARTU CLOSE");
	PubBeepErr();
	tmrPause(3);
	return;

lblPasif:
	dspClear();
	dspLS(1, "KARTU PASIF");
	PubBeepErr();
	tmrPause(3);
	return;

lblAutoReversal:
	//SEND REVERSAL
	if(flagAutoRev)
		TranReversal();
}

void BrizziRedeem(void)
{
	int iRet;
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;
	uchar CardNumHex[lenPan+1];
	uchar ucAmtBefore[4+1];
	ulong amtSaldo;
	ulong amt;
	ulong sisa;
	uchar uangDebit[4+1];
	uchar seskey[8+1];
	ulong cumAmt=0;
	uchar UID[11+1];
	char amtTotal[20+1];
	uchar flagAutoRev=FALSE;
	uchar ucKey=0;

	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(CardNumHex, 0, sizeof(CardNumHex));
	memset(&LastTrxAkumDebit, 0, sizeof(LAST_TRX_AKUM_DEBIT));
	memset(ucAmtBefore, 0, sizeof(ucAmtBefore));
	memset(uangDebit, 0, sizeof(uangDebit));
	memset(seskey, 0, sizeof(seskey));
	memset(UID, 0, sizeof(UID));
	memset(amtTotal, 0, sizeof(amtTotal));

	
	//connect to SAM
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	iRet = TransInit(TRANS_TYPE_BRIZZI_REDEEM_REQ);
	if( iRet!=0 )
	{
		return;
	}

	//CHECK BATCH
	if(!ChkIfBatchBrizziFull())
	{
		return;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return;
	}

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	DispProcess();

	memcpy(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID));
	if(SelectAID01()!=TRUE)
		goto lblErrInvalidCard;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		goto lblErrAccessingCard;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		goto lblErrAccessingCard;

	//CHECK STATUS
	if ((CI_Status.Status[0] == 0x63)&&(CI_Status.Status[1]==0x6C))
		{goto lblClose;}


	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;
	
	memset(Data, 0, sizeof(Data));
	if(ReadLastTrxDateAkumDebit(Data)==TRUE)
	{
		ParseLastTrxDateAkumDebit(Data, &LastTrxAkumDebit);
	}
	else
		goto lblErrAccessingCard;

	CheckAkumDebit(&LastTrxAkumDebit, 0, &cumAmt);	//don't be check (get cumAmt only)
	
	//CHECK LAST TRX DATE
	if(CheckLastTrxDate(&LastTrxAkumDebit)!=TRUE)
	{
		goto lblPasif;
	}

	if(getValueBrizzi(&amtSaldo,ucAmtBefore) != TRUE)
		goto lblErrAccessingCard;

	//CHECK AKUM DEBIT
	if(CheckAkumDebit(&LastTrxAkumDebit, amtSaldo, &cumAmt)!=TRUE)
	{
		goto lblMaxLimit;
	}

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore,"%lu", amtSaldo);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	memcpy(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, sizeof(glProcInfo.stTranLog.szAmount));
	
	//SET ISO8583
	setPackBrizziRedeem();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		return ;
	}


	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

	unPackRedeemBrizzi();

	//info redeem kartu
	ScrCls();
	PubDispStringASCII("INFO REDEEM KARTU", DISP_LINE_CENTER);

	memset(amtTotal, 0, sizeof(amtTotal));
	App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, amtTotal, GA_SEPARATOR);
	PubDispStringASCII("SALDO KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(amtTotal, DISP_LINE_RIGHT|3);

	memset(amtTotal, 0, sizeof(amtTotal));
	App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, amtTotal, GA_SEPARATOR);
	PubDispStringASCII("SALDO DEPOSIT :", DISP_LINE_LEFT|4);
	PubDispStringASCII(amtTotal, DISP_LINE_RIGHT|5);

	PubDispStringASCII("KELUAR   LANJUT", DISP_LINE_RIGHT|7);

	Beef(6, 60);
	ucKey= PubWaitKey(60);
	if((ucKey!=KEYENTER)&&(ucKey!=KEYFN))
		return;
	


	iRet = TransInit(TRANS_TYPE_BRIZZI_REDEEM);
	if( iRet!=0 )
	{
		return;
	}

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore,"%lu", amtSaldo);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	memcpy(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, sizeof(glProcInfo.stTranLog.szAmount));


	setPackBrizziRedeem();

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	flagAutoRev =TRUE;
	
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
				return ;
			}
		}
		return ;
	}


	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

	unPackRedeemBrizzi();

	//Get Version (MIFARE DESFIRE) / TAP CARD
	memset(glProcInfo.stTranLog.BRI.Brizzi.UID,0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.UID));
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		goto lblAutoReversal;
	
	DispProcess();
	
	if(memcmp(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID))!=0)
		goto lblErrInvalidCard;

	
	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;

	dec2num(&amt , glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, (uchar)strlen(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore));
	amt/=100;

	sisa = amtSaldo-amt;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter,"%lu", sisa);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);


	if(DebitBrizzi(amt, uangDebit)!=TRUE)
		goto lblErrAccessingCard;

	if(SelectAID01()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0App1(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;

	//CHANGE STATUS
	CI_Status.Status[0] = 0x63;
	CI_Status.Status[1] = 0x6C;

	if(WriteCloseStatusCard(&CI_Status)!= TRUE)
		goto lblErrAccessingCard;

	if(CommitTransaction()!=TRUE)
		goto lblErrUpdatingCard;

	glProcInfo.stTranLog.ulInvoiceNo = glSysCtrl.ulInvoiceNo;
	
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveBrizziTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	SaveRevInfo(FALSE);//CLEAR REVERSAL

	PubBeepOk();
	BrizziPrintRedeem(PRN_NORMAL);
	return;

lblErrInvalidCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "INVALID CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;
	
lblErrAccessingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "ACCESSING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblErrUpdatingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "UPDATING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblClose:
	dspClear();
	dspLS(1, "KARTU CLOSE");
	PubBeepErr();
	tmrPause(3);
	return;

lblPasif:
	dspClear();
	dspLS(1, "KARTU PASIF");
	PubBeepErr();
	tmrPause(3);
	return;

lblMaxLimit:
	dspClear();
	dspLS(1, "MAKSIMUM");
	dspLS(2, "LIMIT BULANAN");
	PubBeepErr();
	tmrPause(3);
	return;

lblAutoReversal:
	//SEND REVERSAL
	if(flagAutoRev)
		TranReversal();

}

void BrizziInfoKartu(void)
{
	int iRet;
	uchar UID[11+1];
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	uchar ucAmt[4+1];
//	card amt;
	char amtTotal[20+1];
	uchar rndNumber[8+1];
	uchar CardNumHex[16+1];
	uchar ucKey=0;
	uchar buffdisplay[22];
	uchar issuedate[8+1];
	uchar cabangissue[4+1];
	uchar statuskartu[8+1];

//	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;


	memset(UID,0,sizeof(UID));
	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(ucAmt, 0, sizeof(ucAmt));
	memset(amtTotal, 0, sizeof(amtTotal));
	memset(rndNumber, 0, sizeof(rndNumber));
	memset(CardNumHex, 0, sizeof(CardNumHex));

	iRet = TransInit(TRANS_TYPE_BRIZZI_INFO_SALDO);
	if( iRet!=0 )
	{
		return;
	}
	
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	if(SelectAID01()!=TRUE)
		return;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		return;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	memcpy(glProcInfo.stTranLog.szPan, CardNumHex, sizeof(CardNumHex));
	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		return;

	
	
	ScrCls();
	PubDispStringASCII("CARD NUMBER :", DISP_LINE_LEFT|0);
	PubDispStringASCII(CardNumHex, DISP_LINE_LEFT|1);

	MEM_ZERO(buffdisplay);
	MEM_ZERO(issuedate);
	bin2hex(buffdisplay, CI_Header.Issue_Date, 3);
	memcpy(issuedate, buffdisplay,2);
	issuedate[2]='-';
	memcpy(issuedate+3, buffdisplay+2,2);
	issuedate[5]='-';
	memcpy(issuedate+6, buffdisplay+4,2);
	PubDispStringASCII("ISSUE DATE :", DISP_LINE_LEFT|2);
	PubDispStringASCII(issuedate, DISP_LINE_LEFT|3);

	MEM_ZERO(cabangissue);
	bin2hex(cabangissue, CI_Header.Cabang_Issue, 2);
	PubDispStringASCII("CABANG ISSUE :", DISP_LINE_LEFT|4);
	PubDispStringASCII(cabangissue, DISP_LINE_LEFT|5);

	MEM_ZERO(statuskartu);
	if ((CI_Status.Status[0] == 0x63)&&(CI_Status.Status[1]==0x6C))
		strcpy(statuskartu, "CLOSE");
	else if ((CI_Status.Status[0] == 0x61)&&(CI_Status.Status[1]==0x61))
		strcpy(statuskartu, "AKTIF");
	else
		strcpy(statuskartu, "UNKNOWN");
	
	PubDispStringASCII("STATUS KARTU :", DISP_LINE_LEFT|6);
	PubDispStringASCII(statuskartu, DISP_LINE_LEFT|7);
	PubBeepOk();
	BrizziPrintInfoKartu(issuedate, cabangissue, statuskartu);

}


void BrizziReAktivasi(void)
{
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	int iRet;
	uchar CardNumHex[lenPan+1];
	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;
	uchar ucAmtBefore[4+1];
	uchar ucAmtAfter[4+1];

	uchar uangDebit[4+1];
	uchar seskey[8+1];
	uchar MacKey[4+1];

	uchar trxType[1+1];
	uchar dataRecord[28+1];
	uchar dataAkum[7+1];
	uchar trxdate[6+1];

	uchar UID[11+1];
	char BuffDisplay[20+1];
	uchar trxdateAsc[14+1];

	uchar ucKey=0;
	uchar flagAutoRev=FALSE;
	ulong cumAmt=0;
	ulong amtSaldo;
	ulong amt;
	ulong amtFee;
	ulong amtSaldoHost;


	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(CardNumHex, 0, sizeof(CardNumHex));
	memset(&LastTrxAkumDebit, 0, sizeof(LAST_TRX_AKUM_DEBIT));
	memset(ucAmtBefore, 0, sizeof(ucAmtBefore));
	memset(ucAmtAfter, 0, sizeof(ucAmtAfter));
	memset(uangDebit, 0, sizeof(uangDebit));
	memset(seskey, 0, sizeof(seskey));
	memset(MacKey, 0, sizeof(MacKey));
	
	memset(trxType, 0, sizeof(trxType));
	memset(dataRecord, 0, sizeof(dataRecord));
	memset(dataAkum, 0, sizeof(dataAkum));
	memset(trxdate, 0, sizeof(trxdate));
	memset(UID, 0, sizeof(UID));
	memset(BuffDisplay, 0, sizeof(BuffDisplay));
	
	//connect to SAM
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	iRet = TransInit(TRANS_TYPE_BRIZZI_REAKT_REQ);
	if( iRet!=0 )
	{
		return;
	}

	//CHECK BATCH
	if(!ChkIfBatchBrizziFull())
	{
		return;
	}

	//CHECK PENDING SETTLEMENT
	if( !ChkSettle() )	//check pending settlement
	{
		return;
	}

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	DispProcess();

	memcpy(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(glProcInfo.stTranLog.BRI.Brizzi.UID));
//	hex_dump_char("UID", UID, sizeof(UID));

	if(SelectAID01()!=TRUE)
		goto lblErrInvalidCard;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		goto lblErrAccessingCard;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		goto lblErrAccessingCard;

	//CHECK STATUS
	if ((CI_Status.Status[0] == 0x63)&&(CI_Status.Status[1]==0x6C))
		{goto lblClose;}


	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;
	
	memset(Data, 0, sizeof(Data));
	if(ReadLastTrxDateAkumDebit(Data)==TRUE)
	{
		ParseLastTrxDateAkumDebit(Data, &LastTrxAkumDebit);
	}
	else
		goto lblErrAccessingCard;

	CheckAkumDebit(&LastTrxAkumDebit, 0, &cumAmt);	//don't be check (get cumAmt only)
	

	if(getValueBrizzi(&amtSaldo,ucAmtBefore) != TRUE)
		goto lblErrAccessingCard;

	sprintf(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore,"%lu", amtSaldo);
	AmtConvToBit4Format(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	memcpy(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, sizeof(glProcInfo.stTranLog.szAmount));
	
	//SET ISO8583
	setPackBrizziReaktivasi(LastTrxAkumDebit.LastTrxDate);

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		return ;
	}


	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

	unPackReaktivasiBrizzi();

	//info redeem kartu
	ScrCls();
	PubDispStringASCII("INFO REAKTIVASI KARTU", DISP_LINE_CENTER);

	memset(BuffDisplay, 0, sizeof(BuffDisplay));
	sprintf(BuffDisplay, "Lama Pasif : %s Bulan", glProcInfo.stTranLog.BRI.Brizzi.LamaPasif);
	PubDispStringASCII(BuffDisplay, DISP_LINE_LEFT|2);

	memset(BuffDisplay, 0, sizeof(BuffDisplay));
	App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, BuffDisplay, GA_SEPARATOR);
	PubDispStringASCII("Saldo Kartu :", DISP_LINE_LEFT|3);
	PubDispStringASCII(BuffDisplay, DISP_LINE_RIGHT|4);

	memset(BuffDisplay, 0, sizeof(BuffDisplay));
	App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, BuffDisplay, GA_SEPARATOR);
	PubDispStringASCII("Saldo Deposit :", DISP_LINE_LEFT|5);
	PubDispStringASCII(BuffDisplay, DISP_LINE_RIGHT|6);

	PubDispStringASCII("KELUAR   LANJUT", DISP_LINE_RIGHT|7);

	Beef(6, 60);
	ucKey= PubWaitKey(60);
	if((ucKey==KEYCANCEL)||(ucKey==KEYCLEAR)||(ucKey==KEYUP))
		return;


	ScrCls();
	PubDispStringASCII("INFO REAKTIVASI KARTU", DISP_LINE_CENTER);

	memset(BuffDisplay, 0, sizeof(BuffDisplay));
	App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.Fee, BuffDisplay, GA_SEPARATOR);
	PubDispStringASCII("Biaya Admin :", DISP_LINE_LEFT|2);
	PubDispStringASCII(BuffDisplay, DISP_LINE_RIGHT|3);

	MEM_ZERO(BuffDisplay);
	if ((glProcInfo.stTranLog.BRI.Brizzi.StatusKartu[0] == 0x63)&&(glProcInfo.stTranLog.BRI.Brizzi.StatusKartu[1]==0x6C))
		strcpy(BuffDisplay, "Reaktivasi : CLOSE");
	else if ((glProcInfo.stTranLog.BRI.Brizzi.StatusKartu[0] == 0x61)&&(glProcInfo.stTranLog.BRI.Brizzi.StatusKartu[1]==0x61))
		strcpy(BuffDisplay, "Reaktivasi : AKTIF");
	else
		strcpy(BuffDisplay, "Reaktivasi : UNKNOWN");
	
	PubDispStringASCII("Status Kartu Setelah", DISP_LINE_LEFT|4);
	PubDispStringASCII(BuffDisplay, DISP_LINE_LEFT|5);

	PubDispStringASCII("KELUAR   LANJUT", DISP_LINE_RIGHT|7);

	ucKey= PubWaitKey(60);
	if((ucKey==KEYCANCEL)||(ucKey==KEYCLEAR)||(ucKey==KEYUP))
		return;


	iRet = TransInit(TRANS_TYPE_BRIZZI_REAKTIVASI);
	if( iRet!=0 )
	{
		return;
	}


	setPackBrizziReaktivasi(LastTrxAkumDebit.LastTrxDate);

	//SEND REVERSAL
	iRet = TranReversal();
	if( iRet!=0 )
	{
		return ;
	}

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return ;
	}

	flagAutoRev =TRUE;
	
	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		return ;
	}


	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		SaveRevInfo(FALSE);
		DispResult(ERR_HOST_REJ);
		
		return ;
	}

//	unPackReaktivasiBrizzi();

	//Get Version (MIFARE DESFIRE) / TAP CARD
	memcpy(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(glProcInfo.stTranLog.BRI.Brizzi.UID));
 
	memset(glProcInfo.stTranLog.BRI.Brizzi.UID,0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.UID));
	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		goto lblAutoReversal;
	
	DispProcess();
	
	if(memcmp(UID, glProcInfo.stTranLog.BRI.Brizzi.UID, sizeof(UID))!=0)
		goto lblErrInvalidCard;

	
	if(SelectAID03()!=TRUE)
		goto lblErrAccessingCard;

	
	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;


	memcpy(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.BRI.Brizzi.Fee, sizeof(glProcInfo.stTranLog.szAmount));

	dec2num(&amtSaldo, glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, (uchar)strlen(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore));
	amtSaldo/=100;

	dec2num(&amtFee, glProcInfo.stTranLog.BRI.Brizzi.Fee, (uchar)strlen(glProcInfo.stTranLog.BRI.Brizzi.Fee));
	amtFee/=100;

	dec2num(&amtSaldoHost, glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, (uchar)strlen(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter));
	amtSaldoHost/=100;

	if(amtSaldoHost<amtSaldo)
		amt=amtSaldo-amtSaldoHost;
	else
		amt=0;
	
	if(DebitBrizzi(amt, uangDebit)!=TRUE)
		goto lblErrAccessingCard;

	strcpy(trxType,"\x5f");

	memset(trxdateAsc, 0, sizeof(trxdateAsc));
	
	PubAsc2Bcd(glProcInfo.stTranLog.szDateTime, 14, trxdateAsc);
	
	trxdate[0] = trxdateAsc[3];//dd
	trxdate[1] = trxdateAsc[2];//mm
	trxdate[2] = trxdateAsc[1];//yy
	
	memcpy(trxdate+3, trxdateAsc+4, 3);//hhmmss

	uangDebit[0] = (unsigned char) amtFee& 0x000000ff;
	uangDebit[1] = (unsigned char) (( amtFee >> 8)&0x000000ff);
	uangDebit[2] = (unsigned char) (( amtFee >> 16)&0x000000ff);
	uangDebit[3] = (unsigned char) (( amtFee >> 24)&0x000000ff);

	
	MEM_ZERO(dataRecord);

	memcpy(dataRecord,glCurTlmTmsAcq.acqMID+7,8);
	memcpy(dataRecord+8,glCurTlmTmsAcq.acqTID,8);
	memcpy(dataRecord+16,trxdate,6);
	memcpy(dataRecord+22,trxType,1);
	memcpy(dataRecord+23,uangDebit,3);
	memcpy(dataRecord+26,ucAmtBefore,3);

	ucAmtAfter[0] = (unsigned char) amtSaldoHost& 0x000000ff;
	ucAmtAfter[1] = (unsigned char) (( amtSaldoHost >> 8)&0x000000ff);
	ucAmtAfter[2] = (unsigned char) (( amtSaldoHost >> 16)&0x000000ff);
	ucAmtAfter[3] = (unsigned char) (( amtSaldoHost >> 24)&0x000000ff);
	
	memcpy(dataRecord+29,ucAmtAfter,3);
	
	if( WriteRecordBrizzi(dataRecord)!= TRUE)
		goto lblErrAccessingCard;

	MEM_ZERO(dataAkum);
	memcpy(dataAkum, trxdateAsc+1, 3);

	if(WriteAkumulativDebitBrizzi(dataAkum, cumAmt) != TRUE)
		goto lblErrAccessingCard;


	if((glProcInfo.stTranLog.BRI.Brizzi.StatusKartu[0]==0x63) && (glProcInfo.stTranLog.BRI.Brizzi.StatusKartu[1]==0x6c))
		goto lblCloseKartu;


	if(CommitTransaction()!=TRUE)
		goto lblErrUpdatingCard;

	glProcInfo.stTranLog.ulInvoiceNo = glSysCtrl.ulInvoiceNo;
	
	if( ChkIfSaveLog() )
	{
		SaveBrizziTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	SaveRevInfo(FALSE);//CLEAR REVERSAL

	

	PubBeepOk();
	BrizziPrintReaktivasi(PRN_NORMAL);
	return;


lblCloseKartu:

	if(SelectAID01()!=TRUE)
		goto lblErrAccessingCard;

	if(AuthenticateKey0App1(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		goto lblErrAccessingCard;

	//CHANGE STATUS
	CI_Status.Status[0] = 0x63;
	CI_Status.Status[1] = 0x6C;

	if(WriteCloseStatusCard(&CI_Status)!= TRUE)
		goto lblErrAccessingCard;

	if(CommitTransaction()!=TRUE)
		goto lblErrUpdatingCard;

	if( ChkIfSaveLog() )
	{
		SaveBrizziTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	SaveRevInfo(FALSE);//CLEAR REVERSAL


	PubBeepOk();
	BrizziPrintReaktivasi(PRN_NORMAL);
	return;

lblErrInvalidCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "INVALID CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;
	
lblErrAccessingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "ACCESSING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblErrUpdatingCard:
	dspClear();
	dspLS(1, "ERROR");
	dspLS(2, "UPDATING CARD");
	PubBeepErr();
	tmrPause(3);
	goto lblAutoReversal;

lblClose:
	dspClear();
	dspLS(1, "KARTU CLOSE");
	PubBeepErr();
	tmrPause(3);
	return;

lblAutoReversal:
	//SEND REVERSAL
	if(flagAutoRev)
		TranReversal();

}




void BrizziPrintLog(void)
{
	int iRet;
	uchar UID[11+1];
	uchar Data[768+1];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	uchar ucAmt[4+1];
//	card amt;
	char amtTotal[20+1];
	uchar rndNumber[8+1];
	uchar CardNumHex[16+1];
	uchar ucKey=0;

//	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;


	memset(UID,0,sizeof(UID));
	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(ucAmt, 0, sizeof(ucAmt));
	memset(amtTotal, 0, sizeof(amtTotal));
	memset(rndNumber, 0, sizeof(rndNumber));
	memset(CardNumHex, 0, sizeof(CardNumHex));

	iRet = TransInit(TRANS_TYPE_BRIZZI_INFO_SALDO);
	if( iRet!=0 )
	{
		return;
	}
	
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	//Get Version (MIFARE DESFIRE) / TAP CARD
	if(GetVersionBrizzi(UID)!=TRUE)
		return;

	ScrCls();
	PubDispString("PROCESSING", DISP_LINE_CENTER|2);
	
	if(SelectAID01()!=TRUE)
		return;

	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		return;

	bin2hex(CardNumHex, CI_Header.Card_Number, 8);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.NoKartu, CardNumHex, sizeof(CardNumHex));

	memcpy(glProcInfo.stTranLog.szPan, CardNumHex, sizeof(CardNumHex));
	iRet = ValidCardBRIZZI();
	if( iRet!=0 )
	{
		return;
	}
	if(SelectAID03()!=TRUE)
		return;

	if(AuthenticateKey0(CI_Header.Card_Number, UID, rndNumber)!=TRUE)
		return;

	iRet =ReadLogDataBrizzi(Data);
	BrizziPrintLogBrizzi(PRN_NORMAL, Data, (ushort)iRet);

}


void BrizziClearCumDebit(void)
{
	uchar Data[32];
	CI_HEADER CI_Header;
	CI_STATUS CI_Status;
	LAST_TRX_AKUM_DEBIT  LastTrxAkumDebit;
	uchar seskey[8+1];
	uchar dataAkum[7+1];
	uchar trxdateAsc[14+1];
	ulong cumAmt;


	memset(Data, 0, sizeof(Data));
	memset(&CI_Header, 0, sizeof(CI_HEADER));
	memset(&CI_Status, 0, sizeof(CI_STATUS));
	memset(&LastTrxAkumDebit, 0, sizeof(LAST_TRX_AKUM_DEBIT));
	
	memset(dataAkum, 0, sizeof(dataAkum));
	
	if(SAMPrepair(FALSE)!=TRUE)
		return;

	if(GetVersionBrizzi(glProcInfo.stTranLog.BRI.Brizzi.UID)!=TRUE)
		return;

	if(SelectAID01()!=TRUE)
		return;
	
	if(ReadCI_Header(Data)==TRUE)
	{
		ParseCI_Header(Data, &CI_Header);
	}
	else
		return;

	memset(Data, 0, sizeof(Data));
	if(ReadCI_Status(Data)==TRUE)
	{
		ParseCI_Status(Data, &CI_Status);
	}
	else
		return;


	if(SelectAID03()!=TRUE)
		return;

	if(AuthenticateKey0(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		return;
	
	memset(Data, 0, sizeof(Data));
	if(ReadLastTrxDateAkumDebit(Data)==TRUE)
	{
		ParseLastTrxDateAkumDebit(Data, &LastTrxAkumDebit);
	}
	else
		return;

	memset(trxdateAsc, 0, sizeof(trxdateAsc));
	
	PubAsc2Bcd(glProcInfo.stTranLog.szDateTime, 14, trxdateAsc);
	
	memcpy(dataAkum, trxdateAsc+1, 3);
	cumAmt = 0;

	if(WriteAkumulativDebitBrizzi(dataAkum, cumAmt) != TRUE)
		return;

	if(SelectAID01()!=TRUE)
		return;

	if(AuthenticateKey0App1(CI_Header.Card_Number, glProcInfo.stTranLog.BRI.Brizzi.UID, seskey)!=TRUE)
		return;
	//CHANGE STATUS
	CI_Status.Status[0] = 0x61;
	CI_Status.Status[1] = 0x61;

	if(WriteCloseStatusCard(&CI_Status))
		return;

	
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//PRINT BRIZZI

void BrizziPrintInfoKartu(uchar *issuedate, uchar *cabangissue, uchar *statuskartu)
{

	uchar szBuff[50+1];

	if( !ChkIfThermalPrinter() )
	{
		return;
	}

	
	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	//TID MID
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
	//CARD TYPE
	MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
	//DATE TIME
	PrnDateTime();

	PrnStr("\n");
	
	//CARD NUMBER
	PrnSetBig();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "%s\n", "INFO KARTU");
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");

	PrnSetNormal();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Issue Date       : %s\n", issuedate);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Cabang Issue     : %s\n", cabangissue);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Status Kartu     : %s\n", statuskartu);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
		
	PrnFooterInfoSaldo();
	PrnCustomerCopy();

	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinterNoLogo();

}

void BrizziPrintCheckBalance(void)
{

	uchar szBuff[50+1];

	ScrCls();
	DispPrinting();
	if( !ChkIfThermalPrinter() )
	{
		return;
	}

	
	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	//TID MID
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
	//CARD TYPE
	MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
	//DATE TIME
	PrnDateTime();

	PrnStr("\n");
	
	//CARD NUMBER
	PrnSetBig();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "%s\n", "INFO SALDO");
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");

	PrnSetNormal();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Saldo            : %s\n", glProcInfo.stTranLog.BRI.Brizzi.PrintAmountAfter);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
		
	PrnFooterInfoSaldo();
	PrnCustomerCopy();

	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

}

void BrizziPrintInfoDeposit(void)
{

	uchar szBuff[50+1];

	ScrCls();
	DispPrinting();
	if( !ChkIfThermalPrinter() )
	{
		return;
	}

	
	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	//TID MID
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
	//CARD TYPE
	MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
	//DATE TIME
	PrnDateTime();
//	PrnInvoiceBatch();
	//REFNUM , APPV CODE
	PrnRefNumApprCode();

	PrnStr("\n");
	
	//CARD NUMBER
	PrnSetBig();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "%s\n", "INFO DEPOSIT");
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");

	PrnSetNormal();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Saldo            : %s\n", glProcInfo.stTranLog.BRI.Brizzi.PrintAmount);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnStr("\n");
		
	PrnFooterInfoSaldo();
	PrnCustomerCopy();

	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

}


void BrizziPrintOfflineBrizzi(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;
	uchar 	szBuff[50+1];
	uchar 	PrintBuff[20+1];

	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return;
	}

	if( !ChkIfThermalPrinter() )
	{
		return;
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
			PrnSetNormal();

			PrnStr("\n");
		}
		
		//TID MID
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		//CARD TYPE
		MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
		//DATE TIME
		PrnDateTime();
		PrnInvoiceBatch();

		PrnStr("\n");
		
		//CARD NUMBER
		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "PEMBAYARAN BRIZZI");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");

		PrnSetNormal();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.BRI.Brizzi.PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Pembayaran       : %s\n", glProcInfo.stTranLog.BRI.Brizzi.PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo            : %s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			PrnFooterInfoSaldo();

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
			if(ucNum==0)
			{
				StartPrinterNoLogo();
				ScrCls();
			}
			else
				StartPrinter();

			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return;
			}
						
		}
		else
		{
			PrnFooterInfoSaldo();
			PrnDuplicateCopy();
			
			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return;
}


void BrizziPrintTopupOnlineBrizzi(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;
	uchar 	szBuff[50+1];
	uchar 	PrintBuff[20+1];


	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return;
	}

	if( !ChkIfThermalPrinter() )
	{
		return;
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
			PrnSetNormal();

			PrnStr("\n");
		}
		//TID MID
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		//CARD TYPE
		MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
		//DATE TIME
		PrnDateTime();
		PrnInvoiceBatch();
		//REFNUM , APPV CODE
		PrnRefNumApprCode();


		PrnStr("\n");
		
		//CARD NUMBER
		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "TOP UP ONLINE");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "Isi Ulang");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "Online Brizzi");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		PrnSetNormal();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnSetNormal();
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MaskPan(glProcInfo.stTranLog.szPan, PrintBuff);
		sprintf((char *)szBuff, "No Kartu Debit   : %16s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "Jumlah isi ulang : %s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo Akhir      : %s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			PrnFooterInfoSaldo();

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
			if(ucNum==0)
			{
				StartPrinterNoLogo();
				ScrCls();
			}
			else
				StartPrinter();

			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return;
			}			
		}
		else
		{
			PrnFooterInfoSaldo();
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return;
}


void BrizziPrintAktivasiDepositBrizzi(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;
	uchar 	szBuff[50+1];
	uchar 	PrintAmount[20+1];


	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return;
	}

	if( !ChkIfThermalPrinter() )
	{
		return;
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
			PrnSetNormal();

			PrnStr("\n");
		}
	
		//TID MID
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		//CARD TYPE
		MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
		//DATE TIME
		PrnDateTime();
		PrnInvoiceBatch();
		//REFNUM , APPV CODE
		PrnRefNumApprCode();

		PrnStr("\n");
		
		//CARD NUMBER
		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "Aktivasi");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "Deposit Brizzi");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");

		PrnSetNormal();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo Awal       : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Aktivasi Deposit : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo Akhir      : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Sisa Deposit     : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			PrnFooterInfoSaldo();

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
			if(ucNum==0)
			{
				StartPrinterNoLogo();
				ScrCls();
			}
			else
				StartPrinter();

			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return;
			}			
			
		}
		else
		{
			PrnFooterInfoSaldo();
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return;
}

void BrizziPrintTopupDeposit(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;
	uchar 	szBuff[50+1];
	uchar 	PrintBuff[20+1];


	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return;
	}

	if( !ChkIfThermalPrinter() )
	{
		return;
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
			PrnSetNormal();

			PrnStr("\n");
		}
	
		//TID MID
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		//CARD TYPE
		MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
		//DATE TIME
		PrnDateTime();
		PrnInvoiceBatch();
		//REFNUM , APPV CODE
		PrnRefNumApprCode();


		PrnStr("\n");
		
		//CARD NUMBER
		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "TOP UP DEPOSIT");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "Isi Ulang");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "Deposit Brizzi");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		PrnSetNormal();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnSetNormal();
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MaskPan(glProcInfo.stTranLog.szPan, PrintBuff);
		sprintf((char *)szBuff, "No Kartu Debit   : %16s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "Jumlah isi ulang : %s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		PrnStr("\n");

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			PrnFooterInfoSaldo();

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
			if(ucNum==0)
			{
				StartPrinter();
				ScrCls();
			}
			else
				StartPrinter();

			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return;
			}			
		}
		else
		{
			PrnFooterInfoSaldo();
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return;
}



void BrizziPrintVoidBrizzi(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;
	uchar 	szBuff[50+1];
	uchar 	PrintBuff[20+1];


	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return;
	}

	if( !ChkIfThermalPrinter() )
	{
		return;
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
			PrnSetNormal();

			PrnStr("\n");
		}
			//TID MID
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		//CARD TYPE
		MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
		//DATE TIME
		PrnDateTime();
		PrnInvoiceBatch();
		//REFNUM , APPV CODE
		PrnRefNumApprCode();


		PrnStr("\n");
		
		//CARD NUMBER
		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "VOID PEMBAYARAN");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		PrnSetNormal();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnSetNormal();
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MaskPan(glProcInfo.stTranLog.szPan, PrintBuff);
		sprintf((char *)szBuff, "TRACE NO#        : %lu\n", glProcInfo.stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "Jumlah Void      : %s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, PrintBuff, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo Akhir      : %s\n", PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			PrnFooterInfoSaldo();

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
			if(ucNum==0)
			{
				StartPrinterNoLogo();
				ScrCls();
			}
			else
				StartPrinter();

			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return;
			}			
		}
		else
		{
			PrnFooterInfoSaldo();
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return;
}



void BrizziPrintRedeem(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;
	uchar 	szBuff[50+1];
	uchar 	PrintAmount[20+1];


	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return;
	}

	if( !ChkIfThermalPrinter() )
	{
		return;
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
			PrnSetNormal();

			PrnStr("\n");
		}
			//TID MID
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		//CARD TYPE
		MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
		//DATE TIME
		PrnDateTime();
		PrnInvoiceBatch();
		//REFNUM , APPV CODE
		PrnRefNumApprCode();

		PrnStr("\n");
		
		//CARD NUMBER
		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "REDEEM BRIZZI");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		PrnSetNormal();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo Kartu      : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo Deposit    : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.Fee, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Fee              : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.szAmount, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Total Redeem     : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "Status Kartu     : CLOSE");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "REDEEM BERHASIL");
		MultiLngPrnStr(szBuff, DISP_LINE_CENTER);

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			PrnFooterInfoSaldo();

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
			if(ucNum==0)
			{
				StartPrinterNoLogo();
				ScrCls();
			}
			else
				StartPrinter();

			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return;
			}			
			
		}
		else
		{
			PrnFooterInfoSaldo();
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return;
}




void BrizziPrintReaktivasi(uchar ucPrnFlag)
{
	uchar	ucNum;
	uchar 	ucMaxPrint;
	uchar 	szBuff[50+1];
	uchar 	PrintAmount[20+1];


	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return;
	}

	if( !ChkIfThermalPrinter() )
	{
		return;
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
			PrnSetNormal();

			PrnStr("\n");
		}
			//TID MID
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnStr("\n");
		//CARD TYPE
		MultiLngPrnStr("BRIZZI CARD\n",DISP_LINE_LEFT);
		//DATE TIME
		PrnDateTime();
		PrnInvoiceBatch();
		//REFNUM , APPV CODE
		PrnRefNumApprCode();

		PrnStr("\n");
		
		//CARD NUMBER
		PrnSetBig();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", "Reaktivasi BRIZZI");
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		PrnSetNormal();
		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo Kartu      : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Saldo Deposit    : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "Lama Pasif       : %s Bulan\n", glProcInfo.stTranLog.BRI.Brizzi.LamaPasif);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintAmount);
		App_ConvAmountTran(glProcInfo.stTranLog.BRI.Brizzi.Fee, PrintAmount, GA_SEPARATOR);
		sprintf((char *)szBuff, "Biaya Admin      : %s\n", PrintAmount);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		if((glProcInfo.stTranLog.BRI.Brizzi.StatusKartu[0]==0x63) && (glProcInfo.stTranLog.BRI.Brizzi.StatusKartu[1]==0x6c))
			sprintf((char *)szBuff, "%s\n", "Status Kartu     : Close");
		else
			sprintf((char *)szBuff, "%s\n", "Status Kartu     : Aktif");
		
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		PrnStr("\n");

		//FOOTER
		if(ucPrnFlag != PRN_REPRINT)
		{
			PrnFooterInfoSaldo();

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
			if(ucNum==0)
			{
				StartPrinterNoLogo();
				ScrCls();
			}
			else
				StartPrinter();

			if(( ucNum==0 ) ||((ucNum==1)&&(ucMaxPrint>2)))
			{
				kbflush();
				ScrClrLine(2,7);
				PubDispString(_T("PRESS ANY KEY"), 4|DISP_LINE_CENTER);
				if(PubWaitKey(USER_OPER_TIMEOUT)==KEYCANCEL)
					return;
			}			
			
		}
		else
		{
			PrnFooterInfoSaldo();
			PrnDuplicateCopy();

			PrnStr("\n\n\n\n\n\n\n\n");
			StartPrinter();

		}
	}

	return;
}

void BrizziPrintSettlement(uchar ucPrnFlag)
{
	uchar 	szBuff[50+1];
	uchar 	PrintBuff[20+1];

#if 0
	if( ucPrnFlag==PRN_NORMAL )
	{
		// save settle information for reprint
		glSysCtrl.stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex]    = TRUE;
		glSysCtrl.stRePrnStlInfo.ulSOC[glCurTlmTmsAcq.ucIndex]     = glSysCtrl.ulInvoiceNo;
		glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex] = glCurTlmTmsAcq.ulCurBatchNo;
		sprintf((char *)glSysCtrl.stRePrnStlInfo.szSettleMsg[glCurTlmTmsAcq.ucIndex], "%s", "CLOSED");
		memcpy(&glSysCtrl.stRePrnStlInfo.stBrizziTotal, &glBrizziTotal, sizeof(BRIZZI_TOTAL_INFO));

		SaveRePrnStlInfo();
	}
#endif

	if( !ChkIfThermalPrinter() )
	{
		return;
	}

	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	if(ucPrnFlag==PRN_REPRINT)
	{
		PrnDuplicateHeader();
		PrnSetNormal();

		PrnStr("\n");
	}
	
	//TID MID
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");

	//DATE TIME
	PrnDateTime();
	
	PrnSetNormal();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "BATCH#: %06lu\n", glCurTlmTmsAcq.ulCurBatchNo);	
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");
	//CARD NUMBER
	PrnSetBig();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "%s\n", "SETTLEMENT REPORT");
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");

	PrnSetNormal();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "Jumlah           : %lu\n", glBrizziTotal.uiSaleCnt);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	MEM_ZERO(PrintBuff);
	App_ConvAmountTran(glBrizziTotal.szSaleAmt, PrintBuff, GA_SEPARATOR);
	sprintf((char *)szBuff, "Total            : %s\n", PrintBuff);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "%s\n", "SETTLEMENT BERHASIL");
	MultiLngPrnStr(szBuff, DISP_LINE_CENTER);

	//FOOTER
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}

#if 1
void PackDataSettlementBrizzi(TRAN_LOG	*stTranLog, uchar *DataOut)
{
	uchar buff[6+1];
	
	memcpy(DataOut, stTranLog->szDateTime+2,12);
	memcpy(DataOut+12, glTranConfig[stTranLog->ucTranType].szProcCode, 6);
	memcpy(DataOut+18, stTranLog->szAmount+2, 10);

	sprintf(buff, "%06lu", stTranLog->ulInvoiceNo);
	memcpy(DataOut+28, buff, 6);

	sprintf(buff, "%06lu", glCurTlmTmsAcq.ulCurBatchNo);
	memcpy(DataOut+34, buff+4, 2);

	memcpy(DataOut+36, stTranLog->BRI.Brizzi.NoKartu, 16);

	memcpy(DataOut+52, stTranLog->BRI.Brizzi.Hash, 8);//ori amt


}
int BrizziSendOffline(void)
{
	int	iRet;
	ushort	uiCnt;
	ushort	lenRec=0;
	uchar flagSend=FALSE;
	uchar bufData[999];
	ushort pos=0;
	
	TRAN_LOG	stTranLog;

	memset(bufData, 0, sizeof(bufData));
	memset(glProcInfo.ucDataSettlement, 0, sizeof(glProcInfo.ucDataSettlement));

	for(uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
	{
		if( glSysCtrl.sBrizziAcqKeyList[uiCnt]==INV_ACQ_KEY )
		{
			continue;
		}

		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadBrizziTranLog(&stTranLog, uiCnt);
		if( iRet!=0 )
		{
			return FALSE;
		}

		if(stTranLog.ucTranType==TRANS_TYPE_BRIZZI_PURCHASE)
		{
			if(lenRec>=14)
			{
				lenRec = 0;
				flagSend=TRUE;
			}
			else
				lenRec++;

			PackDataSettlementBrizzi(&stTranLog , bufData+pos);
			pos+=60;

			if(flagSend)
			{
				//KIRIM
				memcpy(glProcInfo.ucDataSettlement, bufData, strlen(bufData));
				DispTransName();
				setPackBrizziSettlement();

				//SEND MESSAGE TO HOST
				iRet = SendPacket();
				if( iRet!=0 )
				{
					return FALSE ;
				}

				//RECV MESSAGE FROM HOST
				iRet = RecvPacket();
				if( iRet!=0 )
				{
					return FALSE;
				}

				//CHECK RESPONSE FROM HOST
				if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
				{
					DispResult(ERR_HOST_REJ);
					return FALSE;
				}

				memset(glProcInfo.ucDataSettlement, 0, sizeof(glProcInfo.ucDataSettlement));
				memset(bufData, 0, sizeof(bufData));
				pos=0;
				flagSend=FALSE;
			}
		}
	}

	if(lenRec>0)
	{
		//KIRIM
		memcpy(glProcInfo.ucDataSettlement, bufData, strlen(bufData));
		DispTransName();
		setPackBrizziSettlement();

		//SEND MESSAGE TO HOST
		iRet = SendPacket();
		if( iRet!=0 )
		{
			return FALSE ;
		}

		//RECV MESSAGE FROM HOST
		iRet = RecvPacket();
		if( iRet!=0 )
		{
			return FALSE;
		}

		//CHECK RESPONSE FROM HOST
		if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
		{
			DispResult(ERR_HOST_REJ);
			return FALSE;
		}

		memset(glProcInfo.ucDataSettlement, 0, sizeof(glProcInfo.ucDataSettlement));
		memset(bufData, 0, sizeof(bufData));
		pos=0;
	}
	
	return TRUE;
}


// Settle current acquirer.
int BrizziSettleSub(void)
{
	int		iRet;
//	uchar	szBuff[20];
	uchar	ucRet;

	uchar acqUploadStatus=0;
	// don't need to check zero total. it has been check outside.

	TransInit(TRANS_TYPE_BRIZZI_SETTLE);

	if( glCurTlmTmsAcq.acqStlRxTO<150 )	// ???? How about Tcp
	{	// 结算时，timeout加长
		glCurTlmTmsAcq.acqStlRxTO += glCurTlmTmsAcq.acqStlRxTO/2;
	}
	else
	{
		glCurTlmTmsAcq.acqStlRxTO = 250;
	}

	iRet = TranReversal();
	if( iRet!=0 )
	{
		return iRet;
	}

	if(BrizziSendOffline()==FALSE)
		return ERR_NO_DISP;

	if( memcmp(glRecvPack.szBit39, "00", 2)==0 )
	{
		glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex] = S_CLR_LOG;

		SaveSysCtrlBase();
		CommOnHook(FALSE);

		ClearBrizziRecord(glCurTlmTmsAcq.acqID);
		GetNewInvoiceNo();

		// save settle information for reprint
		glSysCtrl.stRePrnStlInfo.bValid[glCurTlmTmsAcq.ucIndex]    = TRUE;
		glSysCtrl.stRePrnStlInfo.ulSOC[glCurTlmTmsAcq.ucIndex]     = glSysCtrl.ulInvoiceNo;
		glSysCtrl.stRePrnStlInfo.ulBatchNo[glCurTlmTmsAcq.ucIndex] = glCurTlmTmsAcq.ulCurBatchNo;
		sprintf((char *)glSysCtrl.stRePrnStlInfo.szSettleMsg[glCurTlmTmsAcq.ucIndex], "%s", "CLOSED");
		memcpy(&glSysCtrl.stRePrnStlInfo.stBrizziTotal, &glBrizziTotal, sizeof(BRIZZI_TOTAL_INFO));

		SaveRePrnStlInfo();

		ucRet = FindAcqIdxByName("BRIZZI_C", FALSE);
		if(ucRet!=MAX_ACQ)
		{
			SetCurAcq(ucRet);

			glSysCtrl.sAcqStatus[glCurTlmTmsAcq.ucIndex] = S_CLR_LOG;
			SaveSysCtrlBase();

			ClearBrizziRecord(glCurTlmTmsAcq.acqID);
		}

		DispPrinting();
		BrizziPrintSettlement(PRN_NORMAL);
		
		return iRet;
	}

	return 0;
}

int BrizziSettle(void)
{
	uchar	ucRet;
	int		iRet;

	TransInit(TRANS_TYPE_BRIZZI_SETTLE);

	ScrCls();
	DispTransName();
	if( !ChkEdcOption(EDC_NOT_SETTLE_PWD) )
	{
		if( PasswordSettle()!=0 )
		{
			return ERR_NO_DISP;
		}
	}

	ucRet = FindAcqIdxByName("BRIZZI_D", FALSE);
	if(ucRet==MAX_ACQ)
		return ERR_NO_DISP;

	SetCurAcq(ucRet);

	ScrCls();
	DispTransName();
	DispProcess();

	CalcBrizziTotal(glCurTlmTmsAcq.acqID, NULL, FALSE);

	if( glBrizziTotal.uiSaleCnt==0)
	{
		ScrCls();
		DispTransName();
		PubDispString(_T("BATCH EMPTY"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return ERR_NO_DISP;
		
	}

	iRet = DispTransBrizziTotal(FALSE);
	if( iRet!=0 )
	{
		return ERR_NO_DISP;
	}

	return BrizziSettleSub();
}
#endif


void BrizziSummaryReport(uchar FlagDate)
{
	int	iRet;
	uchar	ucRet;
	uchar date[6+1];

	memset(date, 0, sizeof(date));

	iRet = TransInit(TRANS_TYPE_BRIZZI_SUMMARY);
	if( iRet!=0 )
	{
		return;
	}

	ScrCls();
	DispTransName();

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	ucRet = FindAcqIdxByName("BRIZZI", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	CalcBrizziTotal(glCurTlmTmsAcq.acqID, date, FlagDate);

	if(( glBrizziTotal.uiSaleCnt==0)&&
		( glBrizziTotal.uiTopupOnlineCnt==0)&&
		( glBrizziTotal.uiTopupDepositCnt==0)&&
		( glBrizziTotal.uiAktivasiDepositCnt==0)&&
		( glBrizziTotal.uiRedeemCnt==0)&&
		( glBrizziTotal.uiReaktivasiCnt==0)&&
		( glBrizziTotal.uiVoidBrizziCnt==0))
	{
		ScrCls();
		DispTransName();
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}

	BrizziPrintSummary();
}

void BrizziDetailReport(uchar FlagDate)
{
	int	iRet;
	uchar	ucRet;
	uchar date[6+1];

	memset(date, 0, sizeof(date));

	iRet = TransInit(TRANS_TYPE_BRIZZI_DETAIL);
	if( iRet!=0 )
	{
		return;
	}

	ScrCls();
	DispTransName();

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	ucRet = FindAcqIdxByName("BRIZZI", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	CalcBrizziTotal(glCurTlmTmsAcq.acqID, date, FlagDate);

	if(( glBrizziTotal.uiSaleCnt==0)&&
		( glBrizziTotal.uiTopupOnlineCnt==0)&&
		( glBrizziTotal.uiTopupDepositCnt==0)&&
		( glBrizziTotal.uiAktivasiDepositCnt==0)&&
		( glBrizziTotal.uiRedeemCnt==0)&&
		( glBrizziTotal.uiReaktivasiCnt==0)&&
		( glBrizziTotal.uiVoidBrizziCnt==0))
	{
		ScrCls();
		DispTransName();
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}

	BrizziPrintDetail(date, FlagDate);
}


void BrizziParseLog(uchar *Data)
{	
	int ret=0;
	char bufPrint[49+1];
	char	buffData[32+1];
	char	buffData2[32+1];
	unsigned long numAmt=0;

	memset(bufPrint, 0, sizeof(bufPrint));
	memset(bufPrint, 0x20, sizeof(bufPrint)-2);
	memset(buffData, 0, sizeof(buffData));
	memset(buffData2, 0, sizeof(buffData2));

	bin2hex(buffData2, Data+16, 6);
	memcpy(buffData,buffData2,2);
	memcpy(buffData+2,"/",1);
	memcpy(buffData+3,buffData2+2,2);
	memcpy(buffData+5,"/",1);
	memcpy(buffData+6,buffData2+4,2);
	memcpy(bufPrint,buffData,strlen(buffData));

	memset(buffData, 0, sizeof(buffData));
	memcpy(buffData,buffData2+6,2);
	memcpy(buffData+2,":",1);
	memcpy(buffData+3,buffData2+8,2);
	memcpy(buffData+5,":",1);
	memcpy(buffData+6,buffData2+10,2);
	memcpy(bufPrint+10,buffData,strlen(buffData));

	memcpy(bufPrint+22,Data+8, 8); 

	
	MEM_ZERO(buffData);
	MEM_ZERO(buffData2);
	if(memcmp(Data+22,"\xec",1)==0)
		memcpy(buffData,"TOPUP",5);
	else if(memcmp(Data+22,"\xeb",1)==0)	
		memcpy(buffData,"PEMBAYARAN",10);		
	else if(memcmp(Data+22,"\xef",1)==0)	
		memcpy(buffData,"AKTIVASI",8);		
	else if(memcmp(Data+22,"\x5f",1)==0)	
		memcpy(buffData,"REAKTIVASI",10);
	else if(memcmp(Data+22,"\xed",1)==0)
		memcpy(buffData,"VOID",4);
	else {
		  memcpy(buffData2,"UNKNOWN",7);	
		  bin2hex(buffData2+7, Data+22, 1);
		  memcpy(buffData,buffData2,strlen(buffData2));	
	}
	
	memcpy(bufPrint+34, buffData, strlen(buffData));
	MultiLngPrnStr(bufPrint, DISP_LINE_LEFT);
	PrnStr("\n");
	
	memset(bufPrint, 0, sizeof(bufPrint));
	MEM_ZERO(buffData);
	MEM_ZERO(buffData2);
	buffData2[0]=Data[25];
	buffData2[1]=Data[24];
	buffData2[2]=Data[23];	
	bin2num(&numAmt, buffData2, 3);
	ret = num2dec(buffData, numAmt, 0);
	
	MEM_ZERO(buffData2);
	fmtAmt(buffData2, buffData, 0, ",.");
	
	MEM_ZERO(buffData);
	memcpy(buffData,"Rp. ",4);
	fmtPad(buffData2, -10, ' ');
	memcpy(buffData+4,buffData2,strlen(buffData2));
	memcpy(bufPrint,buffData,strlen(buffData));
	MultiLngPrnStr(bufPrint, DISP_LINE_RIGHT);
	PrnStr("\n");
	
}

void BrizziPrintLogBrizzi(uchar ucPrnFlag, uchar *Data, ushort maxdata)
{
	uchar 	szBuff[50+1];
	uint   	iLoop;


	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return; 
	}

	if( !ChkIfThermalPrinter() )
	{
		return;
	}

	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	//TID MID
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");

	//DATE TIME
	PrnDateTime();
	PrnStr("\n");
		
	//CARD NUMBER
	PrnSetBig();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "%s\n", "PRINT LAST LOG");
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");

	PrnSetNormal();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "No Kartu BRIZZI  : %16s\n", glProcInfo.stTranLog.BRI.Brizzi.NoKartu);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	PrnSetNormal();
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	MultiLngPrnStr("  DATE      TIME     TERMINAL ID  TRANSACTION\n", DISP_LINE_LEFT);
	MultiLngPrnStr("                                          AMOUNT\n", DISP_LINE_LEFT);
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	for(iLoop=0;iLoop<maxdata;iLoop+=32)
	{
		BrizziParseLog(Data+iLoop);
	}
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);

	//FOOTER
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}



void BrizziParseSummary(uchar *TrxName, uint Count, uchar *Amt)
{	
	int ret=0;
	char bufPrint[49+1];
	char	buffData[32+1];
	char	buffData2[32+1];
	unsigned long numAmt=0;

	memset(bufPrint, 0, sizeof(bufPrint));
	memset(bufPrint, 0x20, sizeof(bufPrint)-2);
	memset(buffData, 0, sizeof(buffData));
	memset(buffData2, 0, sizeof(buffData2));

	memcpy(bufPrint,TrxName,strlen(TrxName));

	sprintf(buffData, "%3lu", Count);
	memcpy(bufPrint+25,buffData,strlen(buffData));

	MEM_ZERO(buffData2);
	MEM_ZERO(buffData);
	memcpy(buffData, Amt, 10);
	fmtAmt(buffData2, buffData, 0, ",.");
	
	MEM_ZERO(buffData);
	memcpy(buffData,"Rp. ",4);
	fmtPad(buffData2, -10, ' ');
	memcpy(buffData+4,buffData2,strlen(buffData2));
	memcpy(bufPrint+34,buffData,strlen(buffData));

	MultiLngPrnStr(bufPrint, DISP_LINE_LEFT);
	PrnStr("\n");
	
}

void BrizziPrintSummary(void)
{
	uchar 	szBuff[50+1];
	uint count=0;
	uchar bufAmt[12+1];


	memset(bufAmt, 0, sizeof(bufAmt));

	if( !ChkIfThermalPrinter() )
	{
		return;
	}

	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	//TID MID
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");

	PrnDateTime();
	
	PrnSetNormal();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "BATCH#: %06lu\n", glCurTlmTmsAcq.ulCurBatchNo);	
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");
	//CARD NUMBER
	PrnSetBig();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "%s\n", "SUMMARY REPORT");
	MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
	PrnStr("\n");

	PrnSetNormal();
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	MultiLngPrnStr("TRANSAKSI               COUNT             AMOUNT\n", DISP_LINE_LEFT);
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);

	if(glBrizziTotal.uiSaleCnt>0)
	{
		count+=glBrizziTotal.uiSaleCnt;
		BrizziParseSummary("PEMBAYARAN", glBrizziTotal.uiSaleCnt, glBrizziTotal.szSaleAmt);
		SafeAscAdd(bufAmt, glBrizziTotal.szSaleAmt, 12);
	}
	
	if(glBrizziTotal.uiTopupOnlineCnt>0)
	{
		count+=glBrizziTotal.uiTopupOnlineCnt;
		BrizziParseSummary("TOPUP ONLINE", glBrizziTotal.uiTopupOnlineCnt, glBrizziTotal.szTopupOnlineAmt);
		SafeAscAdd(bufAmt, glBrizziTotal.szTopupOnlineAmt, 12);
	}
	if(glBrizziTotal.uiTopupDepositCnt>0)
	{
		count+=glBrizziTotal.uiTopupDepositCnt;
		BrizziParseSummary("TOPUP DEPOSIT", glBrizziTotal.uiTopupDepositCnt, glBrizziTotal.szTopupDepositAmt);
		SafeAscAdd(bufAmt, glBrizziTotal.szTopupDepositAmt, 12);
	}

	if(glBrizziTotal.uiAktivasiDepositCnt>0)
	{
		count+=glBrizziTotal.uiAktivasiDepositCnt;
		BrizziParseSummary("AKTIVASI DEPOSIT", glBrizziTotal.uiAktivasiDepositCnt, glBrizziTotal.szAktivasiDepositAmt);
		SafeAscAdd(bufAmt, glBrizziTotal.szAktivasiDepositAmt, 12);
	}

	if(glBrizziTotal.uiRedeemCnt>0)
	{
		count+=glBrizziTotal.uiRedeemCnt;
		BrizziParseSummary("REDEEM", glBrizziTotal.uiRedeemCnt, glBrizziTotal.szRedeemAmt);
		SafeAscAdd(bufAmt, glBrizziTotal.szRedeemAmt, 12);
	}

	if(glBrizziTotal.uiReaktivasiCnt>0)
	{
		count+=glBrizziTotal.uiReaktivasiCnt;
		BrizziParseSummary("REAKTIVASI", glBrizziTotal.uiReaktivasiCnt, glBrizziTotal.szReaktivasiAmt);
		SafeAscAdd(bufAmt, glBrizziTotal.szReaktivasiAmt, 12);
	}

	if(glBrizziTotal.uiVoidBrizziCnt>0)
	{
		count+=glBrizziTotal.uiVoidBrizziCnt;
		BrizziParseSummary("VOID", glBrizziTotal.uiVoidBrizziCnt, glBrizziTotal.szVoidBrizziAmt);
		SafeAscAdd(bufAmt, glBrizziTotal.szVoidBrizziAmt, 12);
	}

	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	BrizziParseSummary("TOTAL", count, bufAmt);	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);


	//FOOTER
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}


void BrizziParseDetail(uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	uiCnt;
	ushort	lenRec=0;
	ushort pos=0;
	TRAN_LOG	stTranLog;

	uchar szBuff[50+1];
	uchar szTransType[24];
	uchar PrintBuff[24];

	
	for(uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
	{
		
		if( glSysCtrl.sBrizziAcqKeyList[uiCnt]==INV_ACQ_KEY )
		{
			continue;
		}
		
		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadBrizziTranLog(&stTranLog, uiCnt);
		if( iRet!=0 )
		{
			return ;
		}

		if(flagDate)
		{
			if(memcmp(stTranLog.szDateTime+2, date+4, 2)!=0)
				continue;
							
			if(memcmp(stTranLog.szDateTime+4, date+2, 2)!=0)
				continue;

			if(memcmp(stTranLog.szDateTime+6, date, 2)!=0)
				continue;
		}

		PrnInit();
		PrnSetNormal();
		MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
		PrnDateTime();

		MEM_ZERO(szBuff);
		PrnSetNormal();
		sprintf((char *)szBuff, "%16s                TRACE NO: %06lu\n", stTranLog.BRI.Brizzi.NoKartu, stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_BRIZZI_PURCHASE:
				sprintf(szTransType, "PEMBAYARAN      ");
				break;
			case TRANS_TYPE_BRIZZI_TOPUP_ONLINE:
				sprintf(szTransType, "TOP UP ONLINE   ");
				break;
			case TRANS_TYPE_BRIZZI_TOPUP_DEP:
				sprintf(szTransType, "TOP UP DEPOSIT  ");
				break;
			case TRANS_TYPE_BRIZZI_AKTV_DEPOS:
				sprintf(szTransType, "AKTIVASI DEPOSIT");
				break;
			case TRANS_TYPE_BRIZZI_VOID:
				sprintf(szTransType, "VOID            ");
				break;
			case TRANS_TYPE_BRIZZI_REDEEM:
				sprintf(szTransType, "REDEEM          ");
				break;
			case TRANS_TYPE_BRIZZI_REAKTIVASI:
				sprintf(szTransType, "REAKTIVASI      ");
				break;
			default:
				sprintf(szTransType, "UNKNOWN         ");
				break;
		}

		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(stTranLog.szAmount, PrintBuff, GA_SEPARATOR);

		MEM_ZERO(szBuff);
		PrnSetNormal();
		sprintf((char *)szBuff, "%-31.16s %16s\n", szTransType, PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		StartPrinter();
		
	}
	return;
}


void BrizziPrintDetail(uchar *date, uchar flagDate)
{
	uchar 	szBuff[50+1];
	uint count=0;
	uchar bufAmt[12+1];


	memset(bufAmt, 0, sizeof(bufAmt));

	if( !ChkIfThermalPrinter() )
	{
		return;
	}

	PrnInit();
	PrnSetNormal();

	//LOGO
	PrnCustomLogo_T();

	//SITE NAME & ADDRESS
	PrnHeader();

	//TID MID
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "TERMINAL ID : %34s\n", glCurTlmTmsAcq.acqTID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "MERCHANT ID : %34s\n", glCurTlmTmsAcq.acqMID);
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");

	PrnDateTime();
	
	PrnSetNormal();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "BATCH#: %06lu\n", glCurTlmTmsAcq.ulCurBatchNo);	
	MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
	PrnStr("\n");
	//CARD NUMBER
	PrnSetBig();
	MEM_ZERO(szBuff);
	sprintf((char *)szBuff, "%s\n", "DETAIL REPORT");
	MultiLngPrnStr(szBuff, DISP_LINE_CENTER);
	PrnStr("\n");

	StartPrinter();
	BrizziParseDetail(date, flagDate);

	//FOOTER
	PrnInit();
	PrnSetNormal();
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}

void BrizziPrnLastTrans(void)
{
	int			iRet;
	ushort		RecNo=0;
	uchar stop;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( glSysCtrl.uiBrizziLastRecNo>=MAX_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	RecNo = glSysCtrl.uiBrizziLastRecNo;

	do
	{	
		stop  = FALSE;
		iRet = LoadBrizziTranLog(&glProcInfo.stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}
		switch(glProcInfo.stTranLog.ucTranType)
		{
			case TRANS_TYPE_BRIZZI_PURCHASE:
			case TRANS_TYPE_BRIZZI_TOPUP_ONLINE:
			case TRANS_TYPE_BRIZZI_AKTV_DEPOS:
			case TRANS_TYPE_BRIZZI_TOPUP_DEP:
			case TRANS_TYPE_BRIZZI_VOID:
			case TRANS_TYPE_BRIZZI_REDEEM:
			case TRANS_TYPE_BRIZZI_REAKTIVASI:
				stop = TRUE;
				break;
		}
		if(stop)
			break;

		if(RecNo==0)
			break;

		RecNo--;
		
	}while(RecNo>0);

	if(!stop)
	{
		PubDispString(_T("NO TRANSACTION"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}
	
	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	switch(glProcInfo.stTranLog.ucTranType)
		{
			case TRANS_TYPE_BRIZZI_PURCHASE:
				BrizziPrintOfflineBrizzi(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_TOPUP_ONLINE:
				BrizziPrintTopupOnlineBrizzi(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_AKTV_DEPOS:
				BrizziPrintAktivasiDepositBrizzi(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_TOPUP_DEP:
				BrizziPrintTopupDeposit(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_VOID:
				BrizziPrintVoidBrizzi(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_REDEEM:
				BrizziPrintRedeem(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_REAKTIVASI:
				BrizziPrintReaktivasi(PRN_REPRINT);
				break;
		}
}


void BrizziRePrnSpecTrans(void)
{
	int			iRet;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( GetBrizziTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetBrizziRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	switch(glProcInfo.stTranLog.ucTranType)
		{
			case TRANS_TYPE_BRIZZI_PURCHASE:
				BrizziPrintOfflineBrizzi(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_TOPUP_ONLINE:
				BrizziPrintTopupOnlineBrizzi(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_AKTV_DEPOS:
				BrizziPrintAktivasiDepositBrizzi(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_TOPUP_DEP:
				BrizziPrintTopupDeposit(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_VOID:
				BrizziPrintVoidBrizzi(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_REDEEM:
				BrizziPrintRedeem(PRN_REPRINT);
				break;
			case TRANS_TYPE_BRIZZI_REAKTIVASI:
				BrizziPrintReaktivasi(PRN_REPRINT);
				break;
			default:
				PubDispString(_T("NO TRANSACTION"), 4|DISP_LINE_LEFT);
				PubBeepErr();
				PubWaitKey(5);
				break;
		}
}
#endif

