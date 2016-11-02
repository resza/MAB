#include "global.h"


#ifdef BANK_BRI
int TransAktivasiKartu(void)
{
	int		iRet;
	uchar 	ucRet;
	ulong 	lenData;
	uchar	ucEntryMode=0;

	char		bit48Req_1[23+1];
	char		bit48Req_3[60+1];

	char		nomorPetugas[16+1];
	char		PswdPetugas[6+1];
	char		bit48Rsp[999+1];

	char		nomor[15+1];
	char		tanggal[10+1];
	char		nama[21+1];

	char		pinblok[16+1];
	

	iRet = TransInit(TRANS_TYPE_AKTIFASIKARTU_PETUGAS);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}

	PubDispString("OTO. PETUGAS", DISP_LINE_CENTER|2);
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(nomorPetugas);
	memcpy(nomorPetugas,glProcInfo.stTranLog.szPan,16);

	ScrCls();
	DispTransName();
	PubDispString("PASSWORD PETUGAS", DISP_LINE_LEFT|4);
	MEM_ZERO(PswdPetugas);
	ucRet = PubGetString(PASS_IN, 0,6 , PswdPetugas, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	MEM_0x20(bit48Req_1);
	bit48Req_1[0] = '0';
	bit48Req_1[1] = '0';
	bit48Req_1[2] = '1';
	
	memcpy(bit48Req_1+3,PswdPetugas,strlen(PswdPetugas));
	strcpy(glProcInfo.szReq48,bit48Req_1);
	

	//SET ISO 8583
	setPackAktivasiKartuPetugas();

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

//	displayResponse();
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(glProcInfo.stTranLog.szPan);
	iRet = TransInit(TRANS_TYPE_AKTIFASIKARTU_NASABAH);
	if( iRet!=0 )
	{
		return iRet;
	}

	glProcInfo.stTranLog.uiEntryMode = MODE_NO_INPUT;
	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;

	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetPINCardService();
		if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackAktivasiKartuNasabah();

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

//	displayResponse();
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}


	MEM_ZERO(bit48Rsp);
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	MEM_ZERO(nomor);
	MEM_ZERO(tanggal);
	MEM_ZERO(nama);

	memcpy(nomor,bit48Rsp,15);
	memcpy(tanggal,bit48Rsp+16,2);
	memcpy(tanggal+2,"-",1);
	memcpy(tanggal+3,bit48Rsp+18,2);
	memcpy(tanggal+5,"-",1);
	memcpy(tanggal+6,bit48Rsp+20,4);
	memcpy(nama,bit48Rsp+60,21);

	memcpy(glProcInfo.stTranLog.BRI.Card_Services.NoRek, nomor, 15);
	memcpy(glProcInfo.stTranLog.BRI.Card_Services.Nama, nama, 15);

	ScrCls();
	PubDispStringASCII("No Kartu Nasabah :", DISP_LINE_LEFT|0);
	PubDispStringASCII(glProcInfo.stTranLog.szPan, DISP_LINE_LEFT|1);
	PubDispStringASCII("No. Rekening :", DISP_LINE_LEFT|2);
	PubDispStringASCII(nomor, DISP_LINE_LEFT|3);
	PubDispStringASCII("Nama Nasabah :", DISP_LINE_LEFT|4);
	PubDispStringASCII(nama, DISP_LINE_LEFT|5);
	MEM_ZERO(nama);
	sprintf(nama, "Tgl Lahir :%s", tanggal);
	PubDispStringASCII(nama, DISP_LINE_LEFT|6);
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	MEM_ZERO(pinblok);
	bin2hex(pinblok,glProcInfo.sPinBlock,8);

	MEM_0x20(bit48Req_3);
	memcpy(bit48Req_3,nomorPetugas,16);
	memcpy(bit48Req_3+16,PswdPetugas,strlen(PswdPetugas));
	memcpy(bit48Req_3+22,pinblok,16);
	memcpy(bit48Req_3+40,nomor,15);
	
	MEM_ZERO(glProcInfo.szReq48);
	strcpy(glProcInfo.szReq48,bit48Req_3);

	iRet = TransInit(TRANS_TYPE_AKTIFASIKARTU_PRINT);
	if( iRet!=0 )
	{
		return iRet;
	}

	setPackAktivasiKartuPrint();

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

		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveCardServicesTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}
		
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
	
	return 0;
}


int TransRenewalKartu(void)
{
	int		iRet;
	uchar 	ucRet;
	ulong 	lenData;
	uchar	ucEntryMode=0;

	char		bit48Req_1[23+1];
	char		bit48Req_3[60+1];

	char		nomorPetugas[16+1];
	char		PswdPetugas[6+1];
	char		bit48Rsp[999+1];

	char		nomor[15+1];
	char		tanggal[10+1];
	char		nama[21+1];

	

	iRet = TransInit(TRANS_TYPE_RENEWALKARTU_PETUGAS);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}

	PubDispString("OTO. PETUGAS", DISP_LINE_CENTER|2);
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(nomorPetugas);
	memcpy(nomorPetugas,glProcInfo.stTranLog.szPan,16);

	ScrCls();
	DispTransName();
	PubDispString("PASSWORD PETUGAS", DISP_LINE_LEFT|4);
	MEM_ZERO(PswdPetugas);
	ucRet = PubGetString(PASS_IN, 0,6 , PswdPetugas, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	MEM_ZERO(bit48Req_1);
	memset(bit48Req_1, 0x20, sizeof(bit48Req_1)-1);
	bit48Req_1[0] = '0';
	bit48Req_1[1] = '0';
	bit48Req_1[2] = '0';

//	memcpy(bit48Req_1+3,PswdPetugas,strlen(PswdPetugas));
	strcpy(glProcInfo.szReq48,bit48Req_1);
	

	//SET ISO 8583
	setPackKartuPetugas();

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
	
//	displayResponse();
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(glProcInfo.stTranLog.szPan);
	iRet = TransInit(TRANS_TYPE_RENEWALKARTU_LAMA);
	if( iRet!=0 )
	{
		return iRet;
	}

	glProcInfo.stTranLog.uiEntryMode = MODE_NO_INPUT;
	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;

	PubDispString("KARTU LAMA", DISP_LINE_CENTER|2);
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetPIN_Manual(FALSE);
		if( iRet!=0 )
	{
		return iRet;
	}

	memcpy(glProcInfo.stTranLog.BRI.Card_Services.NoKartu, glProcInfo.stTranLog.szPan, sizeof(glProcInfo.stTranLog.szPan));
	setPackRenewalKartuLama();

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

//	displayResponse();
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}


	MEM_ZERO(bit48Rsp);
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	MEM_ZERO(nomor);
	MEM_ZERO(tanggal);
	MEM_ZERO(nama);

	memcpy(nomor,bit48Rsp,15);
	memcpy(tanggal,bit48Rsp+16,2);
	memcpy(tanggal+2,"-",1);
	memcpy(tanggal+3,bit48Rsp+18,2);
	memcpy(tanggal+5,"-",1);
	memcpy(tanggal+6,bit48Rsp+20,4);
	memcpy(nama,bit48Rsp+60,21);

	memcpy(glProcInfo.stTranLog.BRI.Card_Services.NoRek, nomor, 15);
	memcpy(glProcInfo.stTranLog.BRI.Card_Services.Nama, nama, 21);
	
	ScrCls();
	PubDispStringASCII("No Kartu Nasabah :", DISP_LINE_LEFT|0);
	PubDispStringASCII(glProcInfo.stTranLog.szPan, DISP_LINE_LEFT|1);
	PubDispStringASCII("No. Rekening :", DISP_LINE_LEFT|2);
	PubDispStringASCII(nomor, DISP_LINE_LEFT|3);
	PubDispStringASCII("Nama Nasabah :", DISP_LINE_LEFT|4);
	PubDispStringASCII(nama, DISP_LINE_LEFT|5);
	MEM_ZERO(nama);
	sprintf(nama, "Tgl Lahir :%s", tanggal);
	PubDispStringASCII(nama, DISP_LINE_LEFT|6);
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	MEM_0x20(bit48Req_3);
	memcpy(bit48Req_3,nomorPetugas,16);
	memcpy(bit48Req_3+16,glProcInfo.stTranLog.BRI.Card_Services.NoKartu,16);
//	memcpy(bit48Req_3+32,"        ",8);
	memcpy(bit48Req_3+40,nomor, 15);
//	memcpy(bit48Req_3+36,nomor,15);
	
	MEM_ZERO(glProcInfo.szReq48);
	strcpy(glProcInfo.szReq48,bit48Req_3);

	MEM_ZERO(glProcInfo.stTranLog.szPan);
	iRet = TransInit(TRANS_TYPE_RENEWALKARTU_BARU);
	if( iRet!=0 )
	{
		return iRet;
	}

	glProcInfo.stTranLog.uiEntryMode = MODE_NO_INPUT;
	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;

	PubDispString("KARTU BARU", DISP_LINE_CENTER|2);
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetPINCardService();
		if( iRet!=0 )
	{
		return iRet;
	}

	setPackRenewalKartuBaru();

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

		return ERR_NO_DISP;
	}

	unPackRenewalKartu();
	
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveCardServicesTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}
		
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
	
	return 0;
}


int TransReissuePin(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req_1[23+1];
	//char		bit48Req_2[60+1];
	char		nomorPetugas[16+1];
	char		PswdPetugas[6+1];
	char		nomorRekening[15+1];


	iRet = TransInit(TRANS_TYPE_REISSUE_PIN_PETUGAS);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}

	PubDispString("OTO. PETUGAS", DISP_LINE_CENTER|2);
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(nomorPetugas);
	memcpy(nomorPetugas,glProcInfo.stTranLog.szPan,16);

	MEM_ZERO(PswdPetugas);
	
	ScrCls();
	DispTransName();
	PubDispString("PASSWORD PETUGAS", DISP_LINE_LEFT|4);
	MEM_ZERO(PswdPetugas);
	ucRet = PubGetString(PASS_IN, 0,6 , PswdPetugas, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	MEM_0x20(bit48Req_1);

	bit48Req_1[0] = '0';
	bit48Req_1[1] = '0';
	bit48Req_1[2] = '1';
	
	memcpy(bit48Req_1+3,PswdPetugas,strlen(PswdPetugas));
	strcpy(glProcInfo.szReq48,bit48Req_1);
	

	//SET ISO 8583
	setPackReissuePinPetugas();

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
	
//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	ScrCls();
	iRet = TransInit(TRANS_TYPE_REISSUE_PIN_NASABAH);
	if( iRet!=0 )
	{
		return iRet;
	}

	glProcInfo.stTranLog.uiEntryMode = MODE_NO_INPUT;
	
	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(nomorRekening);
	
	ScrCls();
	DispTransName();
	PubDispString("NOMOR REKENING:", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 10,15 , nomorRekening, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	ScrCls();
	iRet = GetPINCardService();
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_0x20(glProcInfo.szReq48);

	memcpy(glProcInfo.szReq48,nomorPetugas,16);
	memcpy(glProcInfo.szReq48+16,nomorRekening,15);
	
	//SET ISO 8583
	setPackReissuePinNasabah();

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

		return ERR_NO_DISP;
	}
	
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveCardServicesTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}
		
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}


int TransUbahPin(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	char		bit48Req[16+1];
	word	mnuItm;

//	strcpy(glProcInfo.stTranLog.szTransName,"UBAH PIN");

	mapGetWord(traMnuItm, mnuItm);
	iRet = TransInit(TRANS_TYPE_UBAHPIN_CS);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetPINCardService();
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(bit48Req);
	CalculatePinBlock(glProcInfo.sPinBlock2);
	bin2hex(bit48Req,glProcInfo.sPinBlock2,8);

	memcpy(glProcInfo.szReq48,bit48Req,16);

	//SET ISO 8583
	setPackUbahPinCS();

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

		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		if(mnuItm==mnuUbahPinCS)
			SaveCardServicesTranLog(&glProcInfo.stTranLog);
	}
		
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	GetNewInvoiceNo();
	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}


	return 0;
}


int TransUbahPwdSPV(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[23+1];
	char		PwdLama[6+1];
	char		Pwd1[6+1];
	char		Pwd2[6+1];


	iRet = TransInit(TRANS_TYPE_UBAHPWD_SPV);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(PwdLama);
	
	ScrCls();
	DispTransName();
//	ScrPrint(0, 2, ASCII, "%s", "INPUT PASSWORD LAMA");
	PubDispString("Ketik Password", DISP_LINE_LEFT|2);
	PubDispString("Lama :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(PASS_IN, 1,6 , PwdLama, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	MEM_0x20(bit48Req);

	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '1';
	
	memcpy(bit48Req+3,PwdLama,strlen(PwdLama));

Ulang:
	MEM_ZERO(Pwd1);
	
	ScrCls();
	DispTransName();
	PubDispString("Ketik Password", DISP_LINE_LEFT|2);
	PubDispString("Baru :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(PASS_IN, 1,6 , Pwd1, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	MEM_ZERO(Pwd2);
	
	ScrCls();
	DispTransName();
	PubDispString("Ulangi Password", DISP_LINE_LEFT|2);
	PubDispString("Baru :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(PASS_IN, 6,6 , Pwd2, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	if (memcmp(Pwd1, Pwd2,6) !=0) 
	{
		ScrClrLine(2,7);
		PubDispString("PASSWORD", DISP_LINE_CENTER|3);
		PubDispString("Tidak Sama", DISP_LINE_CENTER|5);

		PubBeepErr();
		PubWaitKey(3);
		goto Ulang;
	}

	memcpy(bit48Req+9,Pwd1,6);
	strcpy(glProcInfo.szReq48,bit48Req);

	//SET ISO 8583
	setPackUbahPwdSPV();

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
		
		return ERR_NO_DISP;
	}
		
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveCardServicesTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}
		
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////REGISTRASI///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int TransSMSBanking(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	uchar 	ucRet;
	char 	bit48Req[35+1];
	char		noHP[13+1];
	char		Pin1[6+1];
	char		Pin2[6+1];
	uchar maxtry;

	iRet = TransInit(TRANS_TYPE_SMSBANKING);
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
		
	MEM_0x20(bit48Req);
	MEM_ZERO(noHP);
	MEM_ZERO(Pin1);
	MEM_ZERO(Pin2);

	memcpy(bit48Req,glProcInfo.stTranLog.szPan,16);

	ScrCls();
	DispTransName();
	PubDispString("NO. HANDPHONE :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 4, 13 , noHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req+16,noHP,strlen(noHP));

	maxtry=0;
lblMasukanPin:
	ScrCls();
	DispTransName();
	PubDispString("KETIK PIN", DISP_LINE_LEFT|2);
	PubDispString("SMS BANKING :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(PASS_IN, 6,6 , Pin1, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	PubDispString("ULANGI PIN", DISP_LINE_LEFT|2);
	ucRet = PubGetString(PASS_IN, 6,6 , Pin2, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	if (memcmp(Pin1, Pin2, 6) !=0) 
	{
		ScrCls();
		DispTransName();
		PubDispString("PIN TIDAK SAMA", DISP_LINE_CENTER|4);
		PubBeepErr();
		PubWaitKey(3);
		maxtry++;
		if(maxtry>2)
			return ERR_USERCANCEL;
		
		goto lblMasukanPin;
	}

	memcpy(bit48Req+29,Pin2,6);
	strcpy(glProcInfo.szReq48,bit48Req);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackSMSBanking();

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

		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}

	return 0;
}


int TransPhoneBanking(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	uchar 	ucRet;

	iRet = TransInit(TRANS_TYPE_PHONEBANKING);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	PubDispStringASCII("Jagalah Kerahasiaan", DISP_LINE_CENTER|0);
	PubDispStringASCII("PIN Kartu Anda", DISP_LINE_CENTER|1);
	PubDispStringASCII("Hati-Hati", DISP_LINE_CENTER|2);
	PubDispStringASCII("Terhadap Penipuan", DISP_LINE_CENTER|3);
	PubDispStringASCII("Lanjutkan Registrasi?", DISP_LINE_CENTER|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	//SET ISO 8583
	setPackPhoneBanking();

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

		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}
	return 0;
}

int TransIBanking(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	uchar 	ucRet;
	char 	bit48Req[35+1];
	char		Pin1[6+1];
	char		Pin2[6+1];
	uchar	maxtry=0;
	
	iRet = TransInit(TRANS_TYPE_IBANKING);
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
		
	ScrCls();
	PubDispStringASCII("Jagalah Kerahasiaan", DISP_LINE_CENTER|3);
	PubDispStringASCII("PIN Internet Banking", DISP_LINE_CENTER|4);
	PubDispStringASCII("Anda", DISP_LINE_CENTER|5);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	MEM_0x20(bit48Req);
	MEM_ZERO(Pin1);
	MEM_ZERO(Pin2);

	memcpy(bit48Req,glProcInfo.stTranLog.szPan,16);

lblMasukanPin:
	ScrCls();
	DispTransName();
	PubDispString("KETIK PIN", DISP_LINE_LEFT|2);
	PubDispString("I-BANKING :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(PASS_IN, 6,6 , Pin1, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	PubDispString("ULANGI PIN", DISP_LINE_LEFT|2);
	ucRet = PubGetString(PASS_IN, 6,6 , Pin2, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	if (memcmp(Pin1, Pin2, 6) !=0) 
	{
		ScrCls();
		DispTransName();
		PubDispString("PIN TIDAK SAMA", DISP_LINE_CENTER|4);
		PubBeepErr();
		PubWaitKey(3);
		maxtry++;
		if(maxtry>2)
			return ERR_USERCANCEL;
		
		goto lblMasukanPin;
	}

	memcpy(bit48Req+16,Pin2,6);
	strcpy(glProcInfo.szReq48,bit48Req);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackIBanking();

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

		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}

	return 0;
}


int TransTrxPBTransfer(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	uchar 	ucRet;

	iRet = TransInit(TRANS_TYPE_TRX_PB_TRANSFER_INQ);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	PubDispString("INPUT NOMOR", DISP_LINE_LEFT|2);
	PubDispString("REKENING:", DISP_LINE_LEFT|4);

	ucRet = PubGetString(NUM_IN, 15, 15 , glProcInfo.stTranLog.BRI.RegTRXPB.NoRek, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}


	//SET ISO 8583
	setPackTrxPBTransferInq();

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

		displayResponse();
		return ERR_NO_DISP;
	}

	unPackRegTrxPBTransfer();

	ScrCls();
	PubDispStringASCII("REG TRANSFER", DISP_LINE_CENTER|0);
	PubDispStringASCII("Nomor Rekening :", DISP_LINE_LEFT|2);
	PubDispStringASCII(glProcInfo.stTranLog.BRI.RegTRXPB.NoRek, DISP_LINE_LEFT|3);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|4);
	PubDispStringASCII(glProcInfo.stTranLog.BRI.RegTRXPB.Nama, DISP_LINE_LEFT|5);

	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_TRX_PB_TRANSFER);
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackTrxPB();

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

		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}
	return 0;
}



int TransTrxPBNoHP(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	uchar 	ucRet;

	iRet = TransInit(TRANS_TYPE_TRX_PB_NO_HP);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	PubDispString("NO. HANDPHONE :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 4, 13 , glProcInfo.stTranLog.BRI.RegTRXPB.NoHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}


	//SET ISO 8583
	setPackTrxPB();

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

		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}
	return 0;
}


int TransInfoSaldo(void)
{
	int		iRet;
	uchar 	ucRet;
	ulong 	lenData;
	uchar	ucEntryMode=0;
	char 	bit48Rsp[lenBit48+1];
	char 	saldo[16+1];
	char 	saldoFmt[20+1];
	char 	nama[30+1];
	char screenJumlah[22+1];

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(saldo);
	MEM_ZERO(saldoFmt);
	MEM_ZERO(nama);
	MEM_ZERO(screenJumlah);

	strcpy(glProcInfo.stTranLog.szTransName,"INFORMASI SALDO");

	iRet = TransInit(TRANS_TYPE_INFOSALDO);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackInfoSaldo();

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

		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
 	CommOnHook(FALSE);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	memcpy(saldo,bit48Rsp,16);
	fmtAmt(saldoFmt,saldo,2,",.");
	memcpy(nama,bit48Rsp+16,30);
	sprintf(screenJumlah,"Rp. %s", saldoFmt);

	ScrCls();
	PubDispStringASCII("INFO SALDO", DISP_LINE_CENTER|0);
	PubDispStringASCII("Nama:", DISP_LINE_LEFT|2);
	PubDispStringASCII(nama, DISP_LINE_LEFT|3);
	PubDispStringASCII("Saldo:", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenJumlah, DISP_LINE_RIGHT|5);
	PubDispStringASCII("Keluar   Cetak", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}

	return 0;
}


int TransInfoSaldoBankLain(void)
{
	int 		ret;
	int		iRet;
	uchar 	ucRet;
	ulong 	lenData;
	uchar	ucEntryMode=0;
	char 	bit48[lenBit48+1];
	char 	saldo[16+1];
	char 	saldoFmt[20+1];
	char 	nama[30+1];
	char screenJumlah[22+1];


	MEM_ZERO(bit48);
	MEM_ZERO(saldo);
	MEM_ZERO(saldoFmt);
	MEM_ZERO(nama);
	MEM_ZERO(screenJumlah);

	strcpy(glProcInfo.stTranLog.szTransName,"INFO. SALDO BANK LAIN");

	iRet = TransInit(TRANS_TYPE_INFOSALDO_BANKLAIN);
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
		
	RUN_FUNCTION(PilihJenisRekening, lblKO);
	
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackInfoSaldoBankLain();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48, glRecvPack.szBit48+2, lenData);

	memcpy(saldo,bit48,16);
	fmtAmt(saldoFmt,saldo,2,",.");
	memcpy(nama,bit48+16,30);
	sprintf(screenJumlah,"Rp. %s", saldoFmt);


	ScrCls();
	PubDispStringASCII("INFO SALDO", DISP_LINE_CENTER|0);
	PubDispStringASCII("Bank/Nasabah :", DISP_LINE_LEFT|1);
	PubDispStringASCII(nama, DISP_LINE_LEFT|2);

	if(ret==1)
		PubDispStringASCII("Jenis Rek : TABUNGAN", DISP_LINE_LEFT|3);
	else
		PubDispStringASCII("Jenis Rek : GIRO", DISP_LINE_LEFT|3);
	
	PubDispStringASCII("Saldo :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenJumlah, DISP_LINE_RIGHT|5);
	PubDispStringASCII("Keluar  Cetak", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	//DISCONNECT FROM HOST
 	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}

	return 0;

	lblKO:
		return FALSE;
}

int TransMiniStatement(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	strcpy(glProcInfo.stTranLog.szTransName,"MINISTATEMENT");

	iRet = TransInit(TRANS_TYPE_MINISTATEMENT);
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
		

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackMiniStatement();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
	
}


int TransMutasiRekening(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	strcpy(glProcInfo.stTranLog.szTransName,"MUTASI REKENING");

	iRet = TransInit(TRANS_TYPE_MUTASIREKENING);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackMutasiRekening();

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
		
		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);

		GetNewInvoiceNo();

	}

	return 0;
}



int TransTrfSesamaBRI(void)
{
	int		iRet;
	uchar 	ucRet;
	char 	bit48Rsp[lenBit48+1];
	char		bit48Req[23];
	char 	nama[30+1];
	ulong 	lenData;
	uchar	ucEntryMode=0;
	char		noRek[15+1];
	char		jumlah[12+1];
	char		jumlahFmt[20+1];

	char		screenNoRek[21+1];
	char		screenJumlah[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"TRANSFER SESAMA BRI");
	
	iRet = TransInit(TRANS_TYPE_INQ_TRFSESAMABRI);
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
	
	MEM_ZERO(noRek);
	MEM_0x20(bit48Req);
	
	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '0';

	ScrCls();
	DispTransName();
	PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
	PubDispString("TUJUAN:", DISP_LINE_LEFT|4);

	ucRet = PubGetString(NUM_IN, 15, 15 , noRek, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req+3,noRek,strlen(noRek));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	iRet = InputUang();
	if( iRet!=0 )
	{
		return iRet;
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
	setPackInqTransferSesamaBRI();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(nama);
	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);
	MEM_ZERO(screenNoRek);
	MEM_ZERO(screenJumlah);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	mapPutStr(traResponseInq,bit48Rsp);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");
	
	memcpy(nama,bit48Rsp,30);

	sprintf(screenJumlah,"Rp.%s",jumlahFmt);

	ScrCls();
	PubDispStringASCII("TRANSFER SESAMA BRI", DISP_LINE_CENTER|0);
	PubDispStringASCII("Rek. Tujuan :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noRek, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Jumlah", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenJumlah, DISP_LINE_RIGHT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	iRet = TransInit(TRANS_TYPE_TRFSESAMABRI);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackTransferSesamaBRI();

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

		return ERR_NO_DISP;
	}

	unPackTransfer_SesamaBRI();
	
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
	return 0;
}


int TransTrfAntarBank(void)
{
	int		ret;
	int		iRet;
	uchar 	ucRet;
	char 	bit48Rsp[lenBit48+1];
	char 	nama[30+1];
	ulong 	lenData;
	uchar	ucEntryMode=0;

	char 	bit48Req[39+1];
	char		KodeBank[3+1];
	char 	noRek[20+1];
	char		noRef[16+1];

	char 	bankAsal[15+1];
	char 	bankTujuan[15+1];
	char 	uang[12+1];
	char		uangFmt[20+1];

	char screenUang[21+1];
	char screenBankAsal[21+1];
	char screenBankTujuan[21+1];
	char screenNoRek[21+1];
	char screenNoRef[21+1];

	MEM_ZERO(screenUang);
	MEM_ZERO(screenBankAsal);
	MEM_ZERO(screenBankTujuan);
	MEM_ZERO(screenNoRek);
	MEM_ZERO(screenNoRef);

	strcpy(glProcInfo.stTranLog.szTransName,"TRANSFER ANTAR BANK");
			
	iRet = TransInit(TRANS_TYPE_INQ_TRFANTARBANK);
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
	
	RUN_FUNCTION(PilihJenisRekening, lblKO);

	MEM_0x20(bit48Req);
	MEM_ZERO(KodeBank);
	MEM_ZERO(noRek);
	MEM_ZERO(noRef);
	
	ScrCls();
	DispTransName();
	PubDispString("KODE BANK :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 3,3 , KodeBank, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req,KodeBank,strlen(KodeBank));

	ScrCls();
	DispTransName();
	PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
	PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req+3,noRek,strlen(noRek));

	iRet = InputUang();
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	PubDispString("CUST REF NUMBER", DISP_LINE_LEFT|2);
	PubDispString("ATAU LANJUT  :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 0, 16, noRef, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req+23,noRef,strlen(noRef));
	strcpy(glProcInfo.szReq48,bit48Req);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackInqTransferAntarBank();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(nama);
	MEM_ZERO(bankAsal);
	MEM_ZERO(bankTujuan);
	
	memcpy(bankAsal,bit48Rsp+30,15);
	memcpy(bankTujuan,bit48Rsp+45,15);
	memcpy(nama,bit48Rsp,30);

	MEM_ZERO(uang);
	MEM_ZERO(uangFmt);
	memcpy(uang,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(uangFmt,uang,0,",.");
	sprintf(screenUang,"Rp. %s",uangFmt);

	ScrCls();
	PubDispStringASCII("TRANSFER ANTAR BANK", DISP_LINE_CENTER|0);
	PubDispStringASCII("Bank Asal :", DISP_LINE_LEFT|1);
	PubDispStringASCII(bankAsal, DISP_LINE_LEFT|2);
	PubDispStringASCII("Bank Tujuan :", DISP_LINE_LEFT|3);
	PubDispStringASCII(bankTujuan, DISP_LINE_LEFT|4);
	PubDispStringASCII("Rek Tujuan", DISP_LINE_LEFT|5);
	PubDispStringASCII(noRek, DISP_LINE_LEFT|6);
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|0);
	PubDispStringASCII(nama, DISP_LINE_LEFT|1);
	PubDispStringASCII("Cust Ref No :", DISP_LINE_LEFT|2);
	PubDispStringASCII(noRef, DISP_LINE_LEFT|3);
	PubDispStringASCII("Jumlah :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenUang, DISP_LINE_RIGHT|5);
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;
	
	iRet = TransInit(TRANS_TYPE_TRFANTARBANK);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackTransferAntarBank();

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

		return ERR_NO_DISP;
	}

	unPackTransfer_AntarBank();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;

	lblKO:
		return FALSE;
}

int TransInfoKodeBank(void)
{
	uchar 	ucRet;

lbl_1:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "002 B R I");
	ScrPrint(0, 1, ASCII, "%s", "008 MANDIRI");
	ScrPrint(0, 2, ASCII, "%s", "009 B N I");
	ScrPrint(0, 3, ASCII, "%s", "011 DANAMON");
	ScrPrint(0, 4, ASCII, "%s", "013 PERMATA");
	ScrPrint(0, 5, ASCII, "%s", "014 B C A");
	PubDispStringASCII(" v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet == KEYUP )
		goto lbl_1;

lbl_2:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "016 B I I");
	ScrPrint(0, 1, ASCII, "%s", "019 PANIN");
	ScrPrint(0, 2, ASCII, "%s", "022 CIMB NIAGA");
	ScrPrint(0, 3, ASCII, "%s", "023 BANK UOB");
	ScrPrint(0, 4, ASCII, "%s", "028 OCBC NISC");
	ScrPrint(0, 5, ASCII, "%s", "031 CITI BANK");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_1;

lbl_3:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "036 WINDU");
	ScrPrint(0, 1, ASCII, "%s", "037 ARTA GRAHA");
	ScrPrint(0, 2, ASCII, "%s", "041 H S B C");
	ScrPrint(0, 3, ASCII, "%s", "042 TOKYO");
	ScrPrint(0, 4, ASCII, "%s", "046 BANK DBS");
	ScrPrint(0, 5, ASCII, "%s", "050 STANCHRT");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_2;

lbl_4:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "052 R B S");
	ScrPrint(0, 1, ASCII, "%s", "054 CAPITAL");
	ScrPrint(0, 2, ASCII, "%s", "061 ANZ INDONESIA");
	ScrPrint(0, 3, ASCII, "%s", "068 WORI INDONESIA");
	ScrPrint(0, 4, ASCII, "%s", "076 BUMI ARTHA");
	ScrPrint(0, 5, ASCII, "%s", "087 EKONOMI");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_3;

lbl_5:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "088 ANTAR DAERAH");
	ScrPrint(0, 1, ASCII, "%s", "089 RABO BANK");
	ScrPrint(0, 2, ASCII, "%s", "095 MUTIARA");
	ScrPrint(0, 3, ASCII, "%s", "097 MAYAPADA");
	ScrPrint(0, 4, ASCII, "%s", "110 B J B");
	ScrPrint(0, 5, ASCII, "%s", "111 D K I");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_4;

lbl_6:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "112 D I Y");
	ScrPrint(0, 1, ASCII, "%s", "113 JATENG");
	ScrPrint(0, 2, ASCII, "%s", "114 JATIM");
	ScrPrint(0, 3, ASCII, "%s", "115 JAMBI");
	ScrPrint(0, 4, ASCII, "%s", "116 ACEH");
	ScrPrint(0, 5, ASCII, "%s", "117 SUMUT");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_5;

lbl_7:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "118 NAGARI");
	ScrPrint(0, 1, ASCII, "%s", "119 RIAU");
	ScrPrint(0, 2, ASCII, "%s", "120 SUMSEL");
	ScrPrint(0, 3, ASCII, "%s", "121 LAMPUNG");
	ScrPrint(0, 4, ASCII, "%s", "122 KALSEL");
	ScrPrint(0, 5, ASCII, "%s", "123 KALBAR");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_6;

lbl_8:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "124 KALTIM");
	ScrPrint(0, 1, ASCII, "%s", "125 KALTENG");
	ScrPrint(0, 2, ASCII, "%s", "126 SULSEL");
	ScrPrint(0, 3, ASCII, "%s", "127 SULUT");
	ScrPrint(0, 4, ASCII, "%s", "128 N T B");
	ScrPrint(0, 5, ASCII, "%s", "129 BALI");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_7;

lbl_9:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "130 N T T");
	ScrPrint(0, 1, ASCII, "%s", "131 MALUKU");
	ScrPrint(0, 2, ASCII, "%s", "132 PAPUA");
	ScrPrint(0, 3, ASCII, "%s", "133 BENGKULU");
	ScrPrint(0, 4, ASCII, "%s", "134 SULTENG");
	ScrPrint(0, 5, ASCII, "%s", "135 SULTRA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_8;

lbl_10:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "145 BNP");
	ScrPrint(0, 1, ASCII, "%s", "146 SWADESI");
	ScrPrint(0, 2, ASCII, "%s", "147 MUAMALAT");
	ScrPrint(0, 3, ASCII, "%s", "151 MESTIKA");
	ScrPrint(0, 4, ASCII, "%s", "153 SINAR MAS");
	ScrPrint(0, 5, ASCII, "%s", "157 MASPION");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_9;

lbl_11:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "161 GANESHA");
	ScrPrint(0, 1, ASCII, "%s", "167 KESAWAN");
	ScrPrint(0, 2, ASCII, "%s", "200 B T N");
	ScrPrint(0, 3, ASCII, "%s", "212 B SAUDARA");
	ScrPrint(0, 4, ASCII, "%s", "213 B T P N");
	ScrPrint(0, 5, ASCII, "%s", "422 BRI SYARIAH");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_10;

lbl_12:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "425 JABARSYA");
	ScrPrint(0, 1, ASCII, "%s", "426 M E G A");
	ScrPrint(0, 2, ASCII, "%s", "441 BUKOPIN");
	ScrPrint(0, 3, ASCII, "%s", "451 B S M");
	ScrPrint(0, 4, ASCII, "%s", "472 JASA JAKARTA");
	ScrPrint(0, 5, ASCII, "%s", "484 BANK HANA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_11;

lbl_13:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "485 BUMI PUTERA");
	ScrPrint(0, 1, ASCII, "%s", "494 BRI AGRO");
	ScrPrint(0, 2, ASCII, "%s", "498 INDOMONEX");
	ScrPrint(0, 3, ASCII, "%s", "501 ROYAL");
	ScrPrint(0, 4, ASCII, "%s", "503 NOBOU BANK");
	ScrPrint(0, 5, ASCII, "%s", "506 B S M I");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_12;

lbl_14:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "513 I N A PERDANA");
	ScrPrint(0, 1, ASCII, "%s", "517 PANIN SYARIAH");
	ScrPrint(0, 2, ASCII, "%s", "523 SAHABAT");
	ScrPrint(0, 3, ASCII, "%s", "535 B K E");
	ScrPrint(0, 4, ASCII, "%s", "542 ARTOS");
	ScrPrint(0, 5, ASCII, "%s", "553 MAYORA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_13;
lbl_15:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "555 INDEX");
	ScrPrint(0, 1, ASCII, "%s", "556 VICTORIA");
	ScrPrint(0, 2, ASCII, "%s", "558 PUNDI");
	ScrPrint(0, 3, ASCII, "%s", "600 B P R/LSB");
	ScrPrint(0, 4, ASCII, "%s", "688 B P R KS");
	ScrPrint(0, 5, ASCII, "%s", "699 BANK EKA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_14;

	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "950 COMMBANK");
	ScrPrint(0, 1, ASCII, "%s", "987 LAINNYA");
	PubDispStringASCII("^ ", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_15;
	
	return 0;
	
}

int TransPembayaranTelkom(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[13+1];
	char 	bit48Rsp[lenBit48+1];	
	char		kodeArea[4+1];
	char		noTelp[9+1];
	char		nomor[13+1];
	char		nama[30+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

	char		screenNoTelp[21+1];
	char		screenNama[21+1];
	char		screenTagihan[21+1];

	byte		len;

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN TELKOM");

	iRet = TransInit(TRANS_TYPE_INQ_TELKOM);
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

	MEM_ZERO(kodeArea);
	MEM_ZERO(nomor);
	
	ScrCls();
	DispTransName();
	PubDispString("KODE AREA :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 3,4 , kodeArea, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
	memcpy(nomor,kodeArea,strlen(kodeArea));
	MEM_0x20(noTelp);
		
	ScrCls();
	DispTransName();
	PubDispString("NO. TELEPON :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 6,9 , noTelp, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
	MEM_0x20(bit48Req);
	strcat(nomor,noTelp);

	//mapPutStr(traNomorTelepon, nomor);
	strcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.NoTelepon,nomor);

	len = strlen(kodeArea);
	if (len == 3)
		{
		bit48Req[0] = '0';
		memcpy(bit48Req+1,nomor,strlen(nomor));
		}
	else
		{
		memcpy(bit48Req,nomor,strlen(nomor));
		}

	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqTelkom();

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
	
//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	
	mapPutStr(traResponseInq, bit48Rsp);

	memcpy(glProcInfo.stTranLog.szAmount, bit48Rsp+33, 10);
	AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);

	MEM_ZERO(screenTagihan);
	
	//memcpy(nama,bit48Rsp+67,21);
	memcpy(nama,bit48Rsp+43,21);
	memcpy(tagihan,bit48Rsp+33,10);
	fmtAmt(tagihanFmt,tagihan,0,",.");

	sprintf(screenNama,"Nama:%s",nama);
	sprintf(screenNoTelp,"No:%s",nomor);
	sprintf(screenTagihan,"Rp. %s",tagihanFmt);

	ScrCls();
	PubDispStringASCII("PEMBAYARAN TELKOM", DISP_LINE_CENTER|0);
	PubDispStringASCII("No. Telepon :", DISP_LINE_LEFT|1);
	PubDispStringASCII(nomor, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTagihan, DISP_LINE_RIGHT|6);

	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_TELKOM);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackTelkom();

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

		return ERR_NO_DISP;
	}

	unPackPembayaran_Telkom();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;
}



int TransPembayaranHalo(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[13+1];
	char 	bit48Rsp[lenBit48+1];	
	char		noHP[13+1];

	char		nama[30+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

//	char		screenHP[21+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN HALO");

	iRet = TransInit(TRANS_TYPE_INQ_KARTUHALO);
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

	MEM_ZERO(noHP);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	PubDispString("HALO :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 4,13 , noHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req,noHP,strlen(noHP));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqPascaBayar();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);

	memcpy(nama,bit48Rsp+24,21);
	memcpy(tagihan,bit48Rsp+54,10);

	memcpy(glProcInfo.stTranLog.szAmount, bit48Rsp+54, 12);

	fmtAmt(tagihanFmt,tagihan,0,",.");

	sprintf(screenTagihan,"Rp. %s",tagihanFmt);

	ScrCls();
	PubDispStringASCII("PEMBAYARAN HALO", DISP_LINE_CENTER|0);
	PubDispStringASCII("No. Telepon :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noHP, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTagihan, DISP_LINE_LEFT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_KARTUHALO);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackPascaBayar();

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
	if (( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0)  && (memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)!=0 ))
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KartuHalo();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranMatrix(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[13+1];
	char 	bit48Rsp[lenBit48+1];	
	char		noHP[13+1];
	char		nama[30+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

//	char		screenHP[21+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN MATRIX");

	iRet = TransInit(TRANS_TYPE_INQ_KARTUMATRIX);
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

	MEM_ZERO(noHP);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MATRIX :", DISP_LINE_LEFT|4);	
	ucRet = PubGetString(NUM_IN, 4,13 , noHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP, noHP, 13);
	memcpy(bit48Req,noHP,strlen(noHP));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqPascaBayar();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	
	memcpy(nama,bit48Rsp+47,21);
	memcpy(tagihan,bit48Rsp+12,10);

	memcpy(glProcInfo.stTranLog.szAmount, bit48Rsp+12, 12);
	fmtAmt(tagihanFmt,tagihan,0,",.");

	sprintf(screenTagihan,"Rp. %s",tagihanFmt);

	ScrCls();
	PubDispStringASCII("PEMBAYARAN MATRIX", DISP_LINE_CENTER|0);
	PubDispStringASCII("No. Telepon :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noHP, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTagihan, DISP_LINE_LEFT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_KARTUMATRIX);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackPascaBayar();

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
	if (( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0)  && (memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)!=0 ))
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KartuMatrix();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;
}

int TransPembayaranPLNPascaBayar(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[12+1];
	char 	bit48Rsp[lenBit48+1];	
	char		noPelanggan[12+1];

	char		nama[30+1];
	char		tunggakan[2+1];
	char		bulantunggakan[23+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];
	char		total[12+1];
	char		totalFmt[20+1];
	char		adminBankFmt[20+1];

	card		numTotal;

	char		screennoPelanggan[21+1];
	char		screenTunggakan[21+1];
	char		screenTagihan[21+1];
	char		screenTotalBayar[21+1];
	char		screenAdminBank[21+1];
	ulong 	adminBank;
	char		admin[12+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN PLN POSTPAID");

	iRet = TransInit(TRANS_TYPE_INQ_PLNPASCABAYAR);
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

	MEM_ZERO(noPelanggan);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	PubDispString("ID PELANGGAN :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 4,12 , noPelanggan, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req,noPelanggan,strlen(noPelanggan));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqPLNPascaBayar();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(nama);
	MEM_ZERO(tunggakan);
	MEM_ZERO(bulantunggakan);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	MEM_ZERO(total);
	MEM_ZERO(totalFmt);
	MEM_ZERO(admin);
	MEM_ZERO(adminBankFmt);
	MEM_ZERO(screenAdminBank);

	memcpy(nama,bit48Rsp+115,21);
	memcpy(tunggakan,bit48Rsp+48,2);
	memcpy(tagihan,bit48Rsp+86,10);
	fmtAmt(tagihanFmt,tagihan,0,",.");
	memcpy(bulantunggakan,bit48Rsp+279,21); //221

	adminBank = 2500;

	dec2num(&numTotal, tagihan,0);
	numTotal+=adminBank;
	num2dec(total, numTotal, 0);
	fmtAmt(totalFmt,total,0,",.");
	num2dec(admin, adminBank, 0);
	fmtAmt(adminBankFmt,admin,0,",.");

	memcpy(glProcInfo.stTranLog.szAmount, tagihan, 12);
	AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);

	sprintf(screennoPelanggan, "Id PLN : %s", noPelanggan);
	sprintf(screenTunggakan,"Tunggakan: %s BULAN",tunggakan);
	sprintf(screenTagihan,"Rp. %s",tagihanFmt);
	sprintf(screenTotalBayar,"Rp. %s",totalFmt);
	sprintf(screenAdminBank,"Rp. %s",adminBankFmt);
	
	

	ScrCls();
	PubDispStringASCII(screennoPelanggan, DISP_LINE_LEFT|0);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|1);
	PubDispStringASCII(nama, DISP_LINE_LEFT|2);
	PubDispStringASCII(screenTunggakan, DISP_LINE_LEFT|3);
	PubDispStringASCII("BL/TA :", DISP_LINE_LEFT|4);
	MEM_ZERO(bulantunggakan);
	memcpy(bulantunggakan,bit48Rsp+279,18); // 3
	PubDispStringASCII(bulantunggakan, DISP_LINE_LEFT|5);

	MEM_ZERO(bulantunggakan);
	memcpy(bulantunggakan,bit48Rsp+297,6); // 1
	PubDispStringASCII(bulantunggakan, DISP_LINE_LEFT|6);
	
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	// Add by zaki Okt 28-2013
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|0);
	PubDispStringASCII(screenTagihan, DISP_LINE_RIGHT|1);
	PubDispStringASCII("Admin Bank :", DISP_LINE_LEFT|2);
	PubDispStringASCII(screenAdminBank, DISP_LINE_RIGHT|3);
	PubDispStringASCII("Total Bayar :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenTotalBayar, DISP_LINE_RIGHT|5);
	
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_PLNPASCABAYAR);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackPLNPascaBayar();

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
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0) 
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_PLNPascaBayar();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranPLNPraBayar(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[12+1];
	char 	bit48Rsp[lenBit48+1];	
	char		noPelanggan[12+1];

	char		IDPel[12+1];
	char		noMeter[11+1];
	char		nama[30+1];
	char		daya[14+1];
	char		total[12+1];
	char		totalFmt[20+1];

	char		screenNoMeter[21+1];
	char		screennoPelanggan[21+1];
	char		screenDaya[21+1];
	char		screenTotalBayar[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN PLN PREPAID");

	iRet = TransInit(TRANS_TYPE_INQ_PLNPRABAYAR);
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

	MEM_ZERO(noPelanggan);
	MEM_0x30(bit48Req);
	
	ScrCls();
	DispTransName();
	PubDispString("NO METER/ID PLN :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 4,12 , noPelanggan, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req,noPelanggan,strlen(noPelanggan));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	iRet = InputUang();
	if( iRet!=0 )
	{
		return iRet;
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
	setPackInqPLNPraBayar();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(IDPel);
	MEM_ZERO(nama);
	MEM_ZERO(noMeter);
	MEM_ZERO(daya);
	MEM_ZERO(total);
	MEM_ZERO(totalFmt);

	memcpy(IDPel,bit48Rsp+3,12);
	memcpy(noMeter,bit48Rsp+48,11);
	memcpy(nama,bit48Rsp+115,21);
	memcpy(daya,bit48Rsp+35,2);
	memcpy(daya+2,"/",1);
	memcpy(daya+3,bit48Rsp+43,5);
	memcpy(daya+8, "VA", 2);
	memcpy(total,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(totalFmt,total,0,",.");

	sprintf(screenNoMeter,"NO METER:%s",noMeter);
	sprintf(screennoPelanggan,"IDPEL:%s",IDPel);
	sprintf(screenDaya,"Tarif/Daya:%10s",daya);
	sprintf(screenTotalBayar,"Rp. %s",totalFmt);

	ScrCls();
	PubDispStringASCII(screenNoMeter, DISP_LINE_LEFT|0);
	PubDispStringASCII(screennoPelanggan, DISP_LINE_LEFT|1);
	PubDispStringASCII("Nama:", DISP_LINE_LEFT|2);
	PubDispStringASCII(nama, DISP_LINE_LEFT|3);
	PubDispStringASCII(screenDaya, DISP_LINE_LEFT|4);
	PubDispStringASCII("Nominal:", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTotalBayar, DISP_LINE_RIGHT|6);
	
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	iRet = TransInit(TRANS_TYPE_PLNPRABAYAR);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackPLNPraBayar();

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
	if (( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0)  && (memcmp(glProcInfo.stTranLog.szRspCode, "02", 2)!=0 ))
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_PLNPraBayar();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranPLNToken(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[164+1];
	char		noMeteran[11+1];
	char		noReferensi[12+1];

	strcpy(glProcInfo.stTranLog.szTransName,"CETAK PLN TOKEN");

	iRet = TransInit(TRANS_TYPE_PLNTOKEN);
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(bit48Req);
	MEM_ZERO(noMeteran);
	
	ScrCls();
	DispTransName();
	PubDispString("No. METERAN :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 11,11 , noMeteran, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '0';
	bit48Req[3] = '1';

	memcpy(bit48Req+4,noMeteran,11);
	MEM_ZERO(noReferensi);

	ScrCls();
	DispTransName();
	PubDispString("REF. NUMBER :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 9,12 , noReferensi, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[15]=0x20;
	memcpy(bit48Req+16,noReferensi,strlen(noReferensi));
	memcpy(glProcInfo.szReq48,bit48Req, 28);
	
	//SET ISO 8583
	setPackPLNToken();

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
		
		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
	return 0;
}

int TransPembayaranKK_KTA(uchar ucTransType)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[32+1];
	char		noKK[16+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	switch(ucTransType)
	{
		case TRANS_TYPE_KKBRI:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT BRI");
			break;
		case TRANS_TYPE_KKCITIBANK:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN KK CITIBANK");
			break;
		case TRANS_TYPE_KTACITIBANK:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN KTA CITIBANK");
			break;
		case TRANS_TYPE_KKSCB:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT SCB");
			break;
		case TRANS_TYPE_KTASCB:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN KTA SCB");
			break;
		case TRANS_TYPE_KKHSBC:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT HSBC");
			break;
		case TRANS_TYPE_KTAHSBC:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN KTA HSBC");
			break;
		case TRANS_TYPE_KKRBS:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT RBS");
			break;
		case TRANS_TYPE_KTARBS:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN KTA RBS");
			break;
		case TRANS_TYPE_KKANZ:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT ANZ");
			break;
	}
	

	iRet = TransInit(ucTransType);
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

	MEM_0x20(noKK);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	switch(ucTransType)
	{
		case TRANS_TYPE_KKBRI:
		case TRANS_TYPE_KKCITIBANK:
		case TRANS_TYPE_KKSCB:
		case TRANS_TYPE_KKANZ:
		case TRANS_TYPE_KKHSBC:
		case TRANS_TYPE_KKRBS:
			PubDispString("KARTU KREDIT :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 16,16 , noKK, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}
			break;
		case TRANS_TYPE_KTACITIBANK:
		case TRANS_TYPE_KTASCB:
		case TRANS_TYPE_KTAHSBC:
		case TRANS_TYPE_KTARBS:
			PubDispString("KTA :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 10,16 , noKK, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}
			break;
	}
	

	if((ucTransType==TRANS_TYPE_KKCITIBANK)||(ucTransType==TRANS_TYPE_KTACITIBANK))
	{
		bit48Req[0] = '0';
		bit48Req[1] = '0';
		bit48Req[2] = '0';
		bit48Req[3] = '0';
		memcpy(bit48Req+4,noKK,16);
	}
	else
	{
		switch(ucTransType)
		{
			case TRANS_TYPE_KKSCB:
				bit48Req[0] = '1';
				break;
			case TRANS_TYPE_KTASCB:
				bit48Req[0] = '2';
				break;
			case TRANS_TYPE_KKANZ:
				bit48Req[0] = '3';
				break;
			case TRANS_TYPE_KKHSBC:
				bit48Req[0] = '4';
				break;
			case TRANS_TYPE_KTAHSBC:
				bit48Req[0] = '5';
				break;
			case TRANS_TYPE_KKRBS:
				bit48Req[0] = '6';
				break;
			case TRANS_TYPE_KTARBS:
				bit48Req[0] = '7';
				break;
		}
		memcpy(bit48Req+1,noKK,strlen(noKK));

	}

	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Rp.%s",jumlahFmt);
	
	ScrCls();
	switch(ucTransType)
	{
		case TRANS_TYPE_KKCITIBANK:
		case TRANS_TYPE_KKSCB:
		case TRANS_TYPE_KKANZ:
		case TRANS_TYPE_KKHSBC:
		case TRANS_TYPE_KKRBS:
			PubDispStringASCII("BAYAR KARTU KREDIT", DISP_LINE_CENTER|0);
			break;
		case TRANS_TYPE_KTACITIBANK:
		case TRANS_TYPE_KTASCB:
		case TRANS_TYPE_KTAHSBC:
		case TRANS_TYPE_KTARBS:
			PubDispStringASCII("BAYAR KTA", DISP_LINE_CENTER|0);
			break;
	}
	
	switch(ucTransType)
	{
		case TRANS_TYPE_KKCITIBANK:
			PubDispStringASCII("No. KK CITIBANK :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KTACITIBANK:
			PubDispStringASCII("No. KTA CITIBANK :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KKSCB:
			PubDispStringASCII("No. KK SCB :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KTASCB:
			PubDispStringASCII("No. KTA SCB :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KKHSBC:
			PubDispStringASCII("No. KK HSBC :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KTAHSBC:
			PubDispStringASCII("No. KTA HSBC :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KKRBS:
			PubDispStringASCII("No. KK RBS :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KTARBS:
			PubDispStringASCII("No. KTA RBS :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KKANZ:
			PubDispStringASCII("No. KK ANZ :", DISP_LINE_LEFT|2);
			break;
	}
	PubDispStringASCII(noKK, DISP_LINE_LEFT|3);
	PubDispStringASCII("Nominal :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenTagihan, DISP_LINE_LEFT|5);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	DispTransName();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		
		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;
}


int TransPembayaranKKBRI(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[16+1];
	char 	bit48Rsp[lenBit48+1];	
	char		noKK[16+1];
	char		nama[30+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];
	char		minBayar[12+1];
	char		minBayarFmt[20+1];

	char		screenMinBayar[21+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
	strcpy(glProcInfo.stTranLog.BRI.PEMB_KKBRI.Tampilan,"KARTU KREDIT BRI");

	iRet = TransInit(TRANS_TYPE_INQ_KKBRI);
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

	MEM_0x20(noKK);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	PubDispString("KARTU KREDIT :", DISP_LINE_LEFT|4);

	ucRet = PubGetString(NUM_IN, 16,16 , noKK, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req,noKK,strlen(noKK));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqKK_KTA();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	
	mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	MEM_ZERO(minBayar);
	MEM_ZERO(minBayarFmt);
	
	memcpy(nama,bit48Rsp,21);
	memcpy(tagihan,bit48Rsp+31,9);
	fmtAmt(tagihanFmt,tagihan,0,",.");
	memcpy(minBayar,bit48Rsp+42,10);
	fmtAmt(minBayarFmt,minBayar,0,",.");

	sprintf(screenTagihan,"Tagihan:%s",tagihanFmt);
	sprintf(screenMinBayar,"Min.Byr:%s",minBayarFmt);

	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "No Kartu Kredit BRI :"); //"Bank Asal:"
	ScrPrint(0, 1, ASCII, "%s", noKK); //"Bank Asal:"
	ScrPrint(0, 2, ASCII, "%s", nama); //"Bank Asal:"
	ScrPrint(0, 4, ASCII, "%s", screenTagihan); //"Bank Tujuan:"
	ScrPrint(0, 5, ASCII, "%s", screenMinBayar);  //"No Rek:"
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;


	iRet = TransInit(TRANS_TYPE_KKBRI);
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet =InputUang();
	if(iRet!=0)
		return iRet;
	
	setPackKK_KTA();

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

		return ERR_NO_DISP;
	}

	unPackPembayaran_KKBRI();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;
}

#if 0
int TransPembayaranKKCitibank(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[32+1];
	char		noKK[16+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KK CITIBANK");

	iRet = TransInit(TRANS_TYPE_KKCITIBANK);
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

	MEM_0x20(noKK);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("KARTU KREDIT :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 16,16 , noKK, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '0';
	bit48Req[3] = '0';
	memcpy(bit48Req+4,noKK,16);
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Rp.%s",jumlahFmt);
	
	ScrCls();
	PubDispStringASCII("BAYAR KARTU KREDIT", DISP_LINE_CENTER|0);
	PubDispStringASCII("No.KK CITIBANK :", DISP_LINE_LEFT|2);
	PubDispStringASCII(noKK, DISP_LINE_LEFT|3);
	PubDispStringASCII("Nominal :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenTagihan, DISP_LINE_LEFT|5);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	DispTransName();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);
		
		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;
}

int TransPembayaranKTACitibank(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[32+1];
	char		noKTA[10+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KTA CITIBANK");

	iRet = TransInit(TRANS_TYPE_KTACITIBANK);
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


	MEM_0x20(noKTA);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("KTA :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 10,10 , noKTA, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '0';
	bit48Req[3] = '0';
	memcpy(bit48Req+4,noKTA,10);
	memcpy(glProcInfo.szReq48,bit48Req,16);
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Rp.%s",jumlahFmt);
	
	ScrCls();
	PubDispStringASCII("PEMBAYARAN KTA", DISP_LINE_CENTER|0);
	PubDispStringASCII("No.KTA CITIBANK :", DISP_LINE_LEFT|2);
	PubDispStringASCII(noKTA, DISP_LINE_LEFT|3);
	PubDispStringASCII("Nominal :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenTagihan, DISP_LINE_LEFT|5);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	DispTransName();

	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();
	
	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranKKSCB(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[17+1];
	char		noKK[16+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KARTU KREDIT SCB");
	
	iRet = TransInit(TRANS_TYPE_KKSCB);
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


	MEM_0x20(noKK);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NOMOR KARTU KREDIT");
	ucRet = PubGetString(NUM_IN, 16,16 , noKK, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '1';
	memcpy(bit48Req+1,noKK,16);
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Jumlah:Rp.%s",jumlahFmt);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "NOMOR KK SCB:");
	ScrPrint(0, 1, ASCII, "%s", noKK);
	ScrPrint(0, 3, ASCII, "%s", screenTagihan); 
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranKTASCB(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[17+1];
	char		noKTA[11+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KTA SCB");

	iRet = TransInit(TRANS_TYPE_KTASCB);
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

	MEM_0x20(noKTA);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NOMOR KTA");
	ucRet = PubGetString(NUM_IN, 11,11 , noKTA, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '2';
	memcpy(bit48Req+1,noKTA,11);
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Jumlah:Rp.%s",jumlahFmt);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "NOMOR KTA SCB:");
	ScrPrint(0, 1, ASCII, "%s", noKTA);
	ScrPrint(0, 3, ASCII, "%s", screenTagihan); 
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranKKANZ(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[17+1];
	char		noKK[16+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KARTU KREDIT ANZ");
	
	iRet = TransInit(TRANS_TYPE_KKANZ);
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

	MEM_0x20(noKK);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NOMOR KARTU KREDIT");
	ucRet = PubGetString(NUM_IN, 16,16 , noKK, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '3';
	memcpy(bit48Req+1,noKK,16);
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Jumlah:Rp.%s",jumlahFmt);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "NOMOR KK ANZ:");
	ScrPrint(0, 1, ASCII, "%s", noKK);
	ScrPrint(0, 3, ASCII, "%s", screenTagihan); 
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
	
	return 0;
}

int TransPembayaranKKHSBC(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[17+1];
	char		noKK[16+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KARTU KREDIT HSBC");

	iRet = TransInit(TRANS_TYPE_KKHSBC);
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

	MEM_0x20(noKK);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NOMOR KARTU KREDIT");
	ucRet = PubGetString(NUM_IN, 16,16 , noKK, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '4';
	memcpy(bit48Req+1,noKK,16);
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Jumlah:Rp.%s",jumlahFmt);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "NOMOR KK HSBC:");
	ScrPrint(0, 1, ASCII, "%s", noKK);
	ScrPrint(0, 3, ASCII, "%s", screenTagihan); 
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranKTAHSBC(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[17+1];
	char		noKTA[12+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KTA HSBC");

	iRet = TransInit(TRANS_TYPE_KTAHSBC);
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


	MEM_0x20(noKTA);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NOMOR KTA");
	ucRet = PubGetString(NUM_IN, 12,16 , noKTA, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '5';
	memcpy(bit48Req+1,noKTA,12);
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Jumlah:Rp.%s",jumlahFmt);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "NOMOR KTA HSBC:");
	ScrPrint(0, 1, ASCII, "%s", noKTA);
	ScrPrint(0, 3, ASCII, "%s", screenTagihan); 
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	
	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranKKRBS(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[17+1];
	char		noKK[16+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KARTU KREDIT RBS");

	iRet = TransInit(TRANS_TYPE_KKRBS);
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

	MEM_0x20(noKK);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NOMOR KARTU KREDIT");
	ucRet = PubGetString(NUM_IN, 16,16 , noKK, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '6';
	memcpy(bit48Req+1,noKK,16);
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Jumlah:Rp.%s",jumlahFmt);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "NOMOR KK RBS:");
	ScrPrint(0, 1, ASCII, "%s", noKK);
	ScrPrint(0, 3, ASCII, "%s", screenTagihan); 
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();
	
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranKTARBS(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[17+1];
	char		noKTA[10+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMB. KTA RBS");

	iRet = TransInit(TRANS_TYPE_KTARBS);
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

	MEM_0x20(noKTA);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NOMOR KTA");
	ucRet = PubGetString(NUM_IN, 10,10 , noKTA, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '7';
	memcpy(bit48Req+1,noKTA,10);
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	
	iRet =InputUang();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Jumlah:Rp.%s",jumlahFmt);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "NOMOR KTA RBS:");
	ScrPrint(0, 1, ASCII, "%s", noKTA);
	ScrPrint(0, 3, ASCII, "%s", screenTagihan); 
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackKK_KTA();

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
		return iRet;
	}

	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackPembayaran_KKKTA();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;
}
#endif
int TransPembayaranCICILAN(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;
	
	char		bit48Req[18+1];
	char		bit48Rsp[lenBit48+1];
	char		noKontrak[16+1];

	char		nama[20+1];
	char		jatuhTempo[10+1];
	char		angsuran[4+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

	char		screenNoKontrak[21+1];
	char		screenJatuhTempo[21+1];
	char		screenAngsuran[21+1];
	char		screenTagihan[21+1];

	word	mnuItm;
	MEM_0x20(bit48Req);
	
	mapGetWord(traMnuItm, mnuItm);

	switch(mnuItm)
		{
		case mnuFIF:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN CICILAN FIF");
			//strcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Tampilan,"CICILAN FIF");
			iRet = TransInit(TRANS_TYPE_INQ_FIF);
			bit48Req[0]='0';
			bit48Req[1]='0';
			break;
		case mnuBAF:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN CICILAN BAF");
			//strcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Tampilan,"CICILAN BAF");
			iRet = TransInit(TRANS_TYPE_INQ_BAF);
			bit48Req[0]='0';
			bit48Req[1]='1';
			break;
		case mnuOTO:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN CICILAN OTO");
			iRet = TransInit(TRANS_TYPE_INQ_OTO);
			bit48Req[0]='0';
			bit48Req[1]='2';
			break;
		case mnuFinansia:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN FINANSIA");
			iRet = TransInit(TRANS_TYPE_INQ_FINANSIA);
			bit48Req[0]='0';
			bit48Req[1]='3';
			break;
		case mnuVerena:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Tampilan,"CICILAN VENERA");
			iRet = TransInit(TRANS_TYPE_INQ_VERENA);
			bit48Req[0]='0';
			bit48Req[1]='4';
			break;
		case mnuWOM:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN CICILAN WOM");
			iRet = TransInit(TRANS_TYPE_INQ_WOM);
			bit48Req[0]='0';
			bit48Req[1]='5';
			break;
		}

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


	MEM_0x20(noKontrak);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	PubDispString("KONTRAK :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 9,16 , noKontrak, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req+2,noKontrak,16);
	memcpy(glProcInfo.szReq48,bit48Req,18);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqCicilan();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(nama);
	MEM_ZERO(angsuran);
	MEM_ZERO(jatuhTempo);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	mapPutStr(traResponseInq, bit48Rsp);

	memcpy(nama,bit48Rsp+4,20);
	memcpy(angsuran,bit48Rsp+73,4);
	memcpy(jatuhTempo,bit48Rsp+77,2);
	memcpy(jatuhTempo+2,"-",1);
	memcpy(jatuhTempo+3,bit48Rsp+79,2);
	memcpy(jatuhTempo+5,"-",1);
	memcpy(jatuhTempo+6,bit48Rsp+81,4);
	memcpy(tagihan,bit48Rsp+85,10);
	fmtAmt(tagihanFmt,tagihan,0,",.");	

	memcpy(glProcInfo.stTranLog.szAmount,bit48Rsp+85,12);

	sprintf(screenNoKontrak,"No:%s",noKontrak);
	sprintf(screenJatuhTempo,"Jth Tempo: %s",jatuhTempo);
	sprintf(screenAngsuran,"Angsuran Ke: %s",angsuran);
	sprintf(screenTagihan,"Rp.%s",tagihanFmt);
	
	ScrCls();
	PubDispStringASCII("No.Kontrak", DISP_LINE_LEFT|0);
	PubDispStringASCII(noKontrak, DISP_LINE_LEFT|1);
	PubDispStringASCII("Nama", DISP_LINE_LEFT|2);
	PubDispStringASCII(nama, DISP_LINE_RIGHT|3);
	PubDispStringASCII(screenJatuhTempo, DISP_LINE_LEFT|4);
	PubDispStringASCII(screenAngsuran, DISP_LINE_LEFT|5);

	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	PubDispStringASCII("Tagihan", DISP_LINE_LEFT|0);
	PubDispStringASCII(screenTagihan, DISP_LINE_RIGHT|1);

	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	DispTransName();

	switch(mnuItm)
		{
		case mnuFIF:
			iRet = TransInit(TRANS_TYPE_FIF);
			break;
		case mnuBAF:
			iRet = TransInit(TRANS_TYPE_BAF);
			break;
		case mnuOTO:
			iRet = TransInit(TRANS_TYPE_OTO);
			break;
		case mnuFinansia:
			iRet = TransInit(TRANS_TYPE_FINANSIA);
			break;
		case mnuVerena:
			iRet = TransInit(TRANS_TYPE_VERENA);
			break;
		case mnuWOM:
			iRet = TransInit(TRANS_TYPE_WOM);
			break;
		}

	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackCicilan();

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

		return ERR_NO_DISP;
	}

	unPackPembayaran_Cicilan();

	//SAVE TRANSACTION
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransPembayaranZakat(void)
{
	int		iRet;
	uchar 	ucRet;
	char		uang[20+1];
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		screenUang[21+1];

	mapGetWord(traMnuItm, mnuItm);

	MEM_ZERO(uang);
	MEM_ZERO(screenUang);

	strcpy(glProcInfo.stTranLog.szTransName,"SETORAN ZAKAT");
	
	iRet = TransInit(TRANS_TYPE_ZAKAT);
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

	iRet = InputUang();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	fmtAmt(uang,glProcInfo.stTranLog.szAmount,2,",.");
	sprintf(screenUang,"Rp.%s",uang);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "Konfirmasi Pengelola:");
	switch(mnuItm)
		{
		case mnuDmptDuafaZ:
			ScrPrint(0, 1, ASCII, "%s", "Dompet Duafa");
			break;
		case mnuYBMZ:
			ScrPrint(0, 1, ASCII, "%s", "YBM");
			break;
		}
	ScrPrint(0, 3, ASCII, "%s", "Jumlah:");
	ScrPrint(0, 4, ASCII, "%s", screenUang);
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackPembayaranZakat_Infaq();

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

		return ERR_NO_DISP;
	}

	unPackPembayaran_Zakat();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}


int TransPembayaranInfaq(void)
{
	int		iRet;
	uchar 	ucRet;
	char		uang[20+1];
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		screenUang[21+1];

	mapGetWord(traMnuItm, mnuItm);

	MEM_ZERO(uang);
	MEM_ZERO(screenUang);

	strcpy(glProcInfo.stTranLog.szTransName,"SETORAN INFAQ");
	
	iRet = TransInit(TRANS_TYPE_INFAQ);
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

	iRet = InputUang();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	fmtAmt(uang,glProcInfo.stTranLog.szAmount,2,",.");
	sprintf(screenUang,"Rp.%s",uang);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "Konfirmasi Pengelola:");
	switch(mnuItm)
		{
		case mnuDmptDuafaI:
			ScrPrint(0, 1, ASCII, "%s", "Dompet Duafa");
			break;
		case mnuYBMI:
			ScrPrint(0, 1, ASCII, "%s", "YBM");
			break;
		}
	ScrPrint(0, 3, ASCII, "%s", "Jumlah:");
	ScrPrint(0, 4, ASCII, "%s", screenUang);
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackPembayaranZakat_Infaq();

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

		return ERR_NO_DISP;
	}

	unPackPembayaran_Zakat();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}


int TransPembayaranDPLK(void)
{
	int		iRet;
	uchar 	ucRet;
	char		bit48Req[23];
	char		noDPLK[7+1];
	char		uang[20+1];
	uchar	ucEntryMode=0;
	char		screenUang[21+1];

	MEM_ZERO(uang);
	MEM_ZERO(screenUang);

	strcpy(glProcInfo.stTranLog.szTransName,"SETORAN DPLK");
	
	iRet = TransInit(TRANS_TYPE_DPLK);
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
	
	MEM_ZERO(noDPLK);
	MEM_0x20(bit48Req);
	
	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '0';

	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NOMOR DPLK");
	ucRet = PubGetString(NUM_IN, 7, 7, noDPLK, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	
	memcpy(bit48Req+3,noDPLK,strlen(noDPLK));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	iRet = InputUang();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	fmtAmt(uang,glProcInfo.stTranLog.szAmount,2,",.");
	sprintf(screenUang,"Rp.%s",uang);
	
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "Konfirmasi No DPLK:");
	ScrPrint(0, 1, ASCII, "%s", noDPLK);
	ScrPrint(0, 3, ASCII, "%s", "JUMLAH:");
	ScrPrint(0, 4, ASCII, "%s", screenUang);
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);
	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackPembayaranDPLK();

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

		return ERR_NO_DISP;
	}

	unPackPembayaran_DPLK();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;
}

int TransPembayaranTiketPesawat(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong	lenData;
	char		bit48Req[13+1];
	char		bit48Rsp[999+1];
	
	char		kodeBayar[13+1];
	char		KodeBooking[6+1];
	char		nama[30+1];
	char		jmlhFlight[1+1];
	char		Bayar[12+1];
	char		BayarFmt[20+1];
	char		Jadwal_1[21+1];
	char		Jadwal_2[21+1];
	char		Jadwal_3[21+1];

	char		screenKodeBayar[21+1];
	char		screenKodeBooking[21+1];
	char		screenJmlhFlight[21+1];
	char		screenBayar[21+1];
	word	mnuItm;
	
	mapGetWord(traMnuItm, mnuItm);

	switch(mnuItm)
		{
		case mnuGaruda:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN TIKET GARUDA");
			iRet = TransInit(TRANS_TYPE_INQ_GARUDA);
			break;
		case mnuLion:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Tampilan,"TIKET LION AIR");
			iRet = TransInit(TRANS_TYPE_INQ_LIONAIR);
			break;
		case mnuSriwjy:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Tampilan,"TIKET SRIWIJAYA AIR");
			iRet = TransInit(TRANS_TYPE_INQ_SRIWIJAYA);
			break;
		case mnuMerpati:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN TIKET MERPATI");
			iRet = TransInit(TRANS_TYPE_INQ_MERPATI);
			break;
		}

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


	MEM_0x20(kodeBayar);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "KODE BAYAR");
	ucRet = PubGetString(NUM_IN, 5,13 , kodeBayar, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	MEM_0x20(bit48Req);

	memcpy(bit48Req,kodeBayar,13);
	memcpy(glProcInfo.szReq48,bit48Req,13);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqTiketPesawat();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(KodeBooking);
	MEM_ZERO(nama);
	MEM_ZERO(jmlhFlight);
	MEM_ZERO(Bayar);
	MEM_ZERO(BayarFmt);
	MEM_ZERO(Jadwal_1);
	MEM_ZERO(Jadwal_2);
	MEM_ZERO(Jadwal_3);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	mapPutStr(traResponseInq, bit48Rsp);

	memcpy(glProcInfo.stTranLog.szAmount,glRecvPack.szBit4,12);
	memcpy(Bayar,glRecvPack.szBit4,10);
	fmtAmt(BayarFmt,Bayar,0,",.");
	
	memcpy(jmlhFlight,bit48Rsp+19,1);

	if ( memcmp(jmlhFlight, "1", 1)==0 )
		{
			memcpy(KodeBooking,bit48Rsp+73,6);
			memcpy(nama,bit48Rsp+43,21);
			memcpy(Jadwal_1,bit48Rsp+25,3);
			memcpy(Jadwal_1+3,"-",1);
			memcpy(Jadwal_1+4,bit48Rsp+28,3);
			memcpy(Jadwal_1+7," ",1);
			memcpy(Jadwal_1+8,bit48Rsp+35,2);
			memcpy(Jadwal_1+10,"/",1);
			memcpy(Jadwal_1+11,bit48Rsp+37,2);
			memcpy(Jadwal_1+13," ",1);
			memcpy(Jadwal_1+14,bit48Rsp+39,2);
			memcpy(Jadwal_1+16,":",1);
			memcpy(Jadwal_1+17,bit48Rsp+41,2);
		}
	if ( memcmp(jmlhFlight, "2", 1)==0 )		
		{
			memcpy(KodeBooking,bit48Rsp+96,6);
			memcpy(nama,bit48Rsp+66,30);
			memcpy(Jadwal_1,bit48Rsp+25,3);
			memcpy(Jadwal_1+3,"-",1);
			memcpy(Jadwal_1+4,bit48Rsp+28,3);
			memcpy(Jadwal_1+7," ",1);
			memcpy(Jadwal_1+8,bit48Rsp+35,2);
			memcpy(Jadwal_1+10,"/",1);
			memcpy(Jadwal_1+11,bit48Rsp+37,2);
			memcpy(Jadwal_1+13," ",1);
			memcpy(Jadwal_1+14,bit48Rsp+39,2);
			memcpy(Jadwal_1+16,":",1);
			memcpy(Jadwal_1+17,bit48Rsp+41,2);

			memcpy(Jadwal_2,bit48Rsp+48,3);
			memcpy(Jadwal_2+3,"-",1);
			memcpy(Jadwal_2+4,bit48Rsp+51,3);
			memcpy(Jadwal_2+7," ",1);
			memcpy(Jadwal_2+8,bit48Rsp+58,2);
			memcpy(Jadwal_2+10,"/",1);
			memcpy(Jadwal_2+11,bit48Rsp+60,2);
			memcpy(Jadwal_2+13," ",1);
			memcpy(Jadwal_2+14,bit48Rsp+62,2);
			memcpy(Jadwal_2+16,":",1);
			memcpy(Jadwal_2+17,bit48Rsp+64,2);
		}
	if ( memcmp(jmlhFlight, "3", 1)==0 )
		{
			memcpy(KodeBooking,bit48Rsp+119,6);
			memcpy(nama,bit48Rsp+89,21);

			memcpy(Jadwal_1,bit48Rsp+25,3);
			memcpy(Jadwal_1+3,"-",1);
			memcpy(Jadwal_1+4,bit48Rsp+28,3);
			memcpy(Jadwal_1+7," ",1);
			memcpy(Jadwal_1+8,bit48Rsp+35,2);
			memcpy(Jadwal_1+10,"/",1);
			memcpy(Jadwal_1+11,bit48Rsp+37,2);
			memcpy(Jadwal_1+13," ",1);
			memcpy(Jadwal_1+14,bit48Rsp+39,2);
			memcpy(Jadwal_1+16,":",1);
			memcpy(Jadwal_1+17,bit48Rsp+41,2);

			memcpy(Jadwal_2,bit48Rsp+48,3);
			memcpy(Jadwal_2+3,"-",1);
			memcpy(Jadwal_2+4,bit48Rsp+51,3);
			memcpy(Jadwal_2+7," ",1);
			memcpy(Jadwal_2+8,bit48Rsp+58,2);
			memcpy(Jadwal_2+10,"/",1);
			memcpy(Jadwal_2+11,bit48Rsp+60,2);
			memcpy(Jadwal_2+13," ",1);
			memcpy(Jadwal_2+14,bit48Rsp+62,2);
			memcpy(Jadwal_2+16,":",1);
			memcpy(Jadwal_2+17,bit48Rsp+64,2);

			memcpy(Jadwal_3,bit48Rsp+71,3);
			memcpy(Jadwal_3+3,"-",1);
			memcpy(Jadwal_3+4,bit48Rsp+74,3);
			memcpy(Jadwal_3+7," ",1);
			memcpy(Jadwal_3+8,bit48Rsp+81,2);
			memcpy(Jadwal_3+10,"/",1);
			memcpy(Jadwal_3+11,bit48Rsp+83,2);
			memcpy(Jadwal_3+13," ",1);
			memcpy(Jadwal_3+14,bit48Rsp+85,2);
			memcpy(Jadwal_3+16,":",1);
			memcpy(Jadwal_3+17,bit48Rsp+87,2);
		}

	MEM_ZERO(screenKodeBayar);
	MEM_ZERO(screenKodeBooking);
	MEM_ZERO(screenJmlhFlight);
	MEM_ZERO(screenBayar);

	sprintf(screenKodeBayar,"Kd Byr:%s",kodeBayar);
	sprintf(screenKodeBooking,"Kd Booking:%s",KodeBooking);
	sprintf(screenJmlhFlight,"Jml Flight:%s",jmlhFlight);
	sprintf(screenBayar,"Jml Byr:Rp.%s",BayarFmt);
Ulang:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", screenKodeBayar);
	ScrPrint(0, 1, ASCII, "%s", screenKodeBooking);
	ScrPrint(0, 3, ASCII, "%s", nama);
	ScrPrint(0, 5, ASCII, "%s", screenJmlhFlight);
	ScrPrint(0, 6, ASCII, "%s", screenBayar);
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);
	if(ucRet!=0)
		return FALSE;

	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "JADWAL PENERBANGAN");
	if ( memcmp(jmlhFlight, "1", 1)==0 )
		{
			ScrPrint(0, 1, ASCII, "%s", Jadwal_1);
		}
	if ( memcmp(jmlhFlight, "2", 1)==0 )
		{
			ScrPrint(0, 1, ASCII, "%s", Jadwal_1);
			ScrPrint(0, 2, ASCII, "%s", Jadwal_2);
		}
	if ( memcmp(jmlhFlight, "3", 1)==0 )
		{
			ScrPrint(0, 1, ASCII, "%s", Jadwal_1);
			ScrPrint(0, 2, ASCII, "%s", Jadwal_2);
			ScrPrint(0, 3, ASCII, "%s", Jadwal_3);
		}
	
	ScrPrint(0, 7, ASCII, "%s", "KEMBALI        LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);
	if(ucRet!=0)
		goto Ulang;

	ScrCls();

	switch(mnuItm)
		{
		case mnuGaruda:
			iRet = TransInit(TRANS_TYPE_GARUDA);
			break;
		case mnuLion:
			iRet = TransInit(TRANS_TYPE_LIONAIR);
			break;
		case mnuSriwjy:
			iRet = TransInit(TRANS_TYPE_SRIWIJAYA);
			break;
		case mnuMerpati:
			iRet = TransInit(TRANS_TYPE_MERPATI);
			break;

		}

	if( iRet!=0 )
	{
		return iRet;
	}

	setPackTiketPesawat();

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

		return ERR_NO_DISP;
	}

	unPackPembayaran_Pesawat();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}

int TransBriva(void)
{
	int		iRet;
	uchar 	ucRet;
	ulong 	lenData;
	uchar	ucEntryMode=0;

	char		bit48Req[20+1];
	char		bit48Rsp[999+1];

	char		noBriva[20+1];
	char		namaInstuisi[20+1];
	char		namaPelanggan[25+1];
	char		keterangan[20+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];
	char		bayar[12+1];
	char		bayarFmt[20+1];
	char		admin[12+1];

	char		screenTagihan[21+1];
	char		screenJumlah[21+1];

	char		statusBayar[1+1];
//	char		duitss[12+1];
//	char		duit2[12+1];
//	char		duit3[12+1];
//	card		numBayar;
//	ulong	duit;

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN BRIVA");
	
	iRet = TransInit(TRANS_TYPE_INQ_BRIVA);
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
		
	MEM_ZERO(noBriva);
//	memset(duit, 0x30, 12);
	
	ScrCls();
	DispTransName();
	PubDispString("KODE BAYAR", DISP_LINE_LEFT|4);
//	ScrPrint(0, 2, ASCII, "%s", "KODE BAYAR");
	ucRet = PubGetString(NUM_IN, 6,20 , noBriva, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	MEM_0x20(bit48Req);

	memcpy(bit48Req,noBriva,20);
	memcpy(glProcInfo.szReq48,bit48Req,20);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackInqBriva();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	mapPutStr(traResponseInq, bit48Rsp);
	
	unPackPembayaran_Briva();

	MEM_ZERO(namaInstuisi);
	MEM_ZERO(namaPelanggan);
	MEM_ZERO(keterangan);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	MEM_ZERO(admin);
	MEM_ZERO(statusBayar);
	

	memcpy(namaInstuisi,bit48Rsp+118,20);
	memcpy(namaPelanggan,bit48Rsp+56,21);
	memcpy(keterangan,bit48Rsp+20,20);

	memcpy(tagihan,bit48Rsp+91,12);
	fmtAmt(tagihanFmt,tagihan,0,",.");
	
	memcpy(admin,bit48Rsp+48,8);
	memcpy(statusBayar,bit48Rsp+138,1);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_BRIVA.Status, statusBayar, 1);
	
	sprintf(screenTagihan,"Tagihan:Rp.%s",tagihanFmt);

	if ( memcmp(statusBayar, "N", 1)==0 )
		{
			sprintf(screenJumlah,"Jml Byr:Rp.%s",tagihanFmt);
	
			ScrCls();
			ScrPrint(0, 0, ASCII, "%s", noBriva);
			ScrPrint(0, 1, ASCII, "%s", namaInstuisi);
			ScrPrint(0, 2, ASCII, "%s", namaPelanggan);
			ScrPrint(0, 3, ASCII, "%s", keterangan);
			ScrPrint(0, 4, ASCII, "%s", screenTagihan);
			ScrPrint(0, 5, ASCII, "%s", screenJumlah);
			ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	//		hex_dump("duit2",glProcInfo.stTranLog.szAmount, 15);


/*			
			memcpy(glProcInfo.stTranLog.szAmount,bit48Rsp+91,12);
			duit = strlen(glProcInfo.stTranLog.szAmount);
			AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, duit+2);

			hex_dump("duit2",glProcInfo.stTranLog.szAmount, 15);
			

			memcpy(glProcInfo.stTranLog.szAmount,bit48Rsp+91,12); //Dibuka lagi
			//memcpy(duit+2,bit48Rsp+91+2,10);
			fmtAmt(duit, glProcInfo.stTranLog.szAmount,0,"");
			memcpy(glProcInfo.stTranLog.szAmount2,duit, 12);
			
			//hex_dump("duit",duit, 15);
			//memcpy(glProcInfo.stTranLog.szAmount,duit,12);
			hex_dump("duit",glProcInfo.stTranLog.szAmount, 15);
			hex_dump("duit2",glProcInfo.stTranLog.szAmount2, 15);

			

			//memcpy(Bayar,glProcInfo.stTranLog.szAmount,10);
			//fmtAmt(BayarFmt,Bayar,0,",.");
			memcpy(duit3, glProcInfo.stTranLog.szAmount,12); //Dibuka lagi
			dec2num(&numBayar,duit3,0);
			numBayar=numBayar*100;
			num2dec(duit3,numBayar,0);
			hex_dump("duit3", duit3, 15);

*/		

			ucRet = PubYesNo(USER_OPER_TIMEOUT);
				if(ucRet!=0)
					return 0;
		}
	
	if ( memcmp(statusBayar, "Y", 1)==0 )
		{		
			ScrCls();
			ScrPrint(0, 0, ASCII, "%s", noBriva);
			ScrPrint(0, 1, ASCII, "%s", namaInstuisi);
			ScrPrint(0, 2, ASCII, "%s", namaPelanggan);
			ScrPrint(0, 3, ASCII, "%s", keterangan);
			ScrPrint(0, 4, ASCII, "%s", screenTagihan);
			ScrPrint(0, 5, ASCII, "%s", "Jml Byr:Rp.0");
			ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");	
			
			ucRet = PubYesNo(USER_OPER_TIMEOUT);
				if(ucRet!=0)
					return 0;
				
			InputUang();

			memset(bayar, 0, sizeof(bayar));
			memset(bayarFmt, 0, sizeof(bayarFmt));
			memset(screenJumlah, 0, sizeof(screenJumlah));
			
			memcpy(bayar,glProcInfo.stTranLog.szAmount,10);
			fmtAmt(bayarFmt,bayar,0,",.");

		//	hex_dump("Mason Berak",glProcInfo.stTranLog.szAmount, 15);
			sprintf(screenJumlah,"Jml Byr:Rp.%s",bayarFmt);
			
			ScrCls();
			ScrPrint(0, 0, ASCII, "%s", noBriva);
			ScrPrint(0, 1, ASCII, "%s", namaInstuisi);
			ScrPrint(0, 2, ASCII, "%s", namaPelanggan);
			ScrPrint(0, 3, ASCII, "%s", keterangan);
			ScrPrint(0, 4, ASCII, "%s", screenTagihan);
			ScrPrint(0, 5, ASCII, "%s", screenJumlah);
			ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");	

			ucRet = PubYesNo(USER_OPER_TIMEOUT);
				if(ucRet!=0)
					return 0;
		}

	ScrCls();

	iRet = TransInit(TRANS_TYPE_BRIVA);
	if( iRet!=0 )
	{
		return iRet;
	}

	setPackBriva();

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

		return ERR_NO_DISP;
	}


	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}


int TransPembayaranKuliah(void)
{
	int		iRet;
	uchar 	ucRet;
	ulong 	lenData;
	uchar	ucEntryMode=0;
	
	char 	bit48Rsp[lenBit48+1];
	char 	bit48Req[25+1];
	char		KodeUniv[3+1];
	char 	noBill[20+1];

	char		noMahasiswa[20+1];
	char 	nama[30+1];
	char		JenisPembayaran[20+1];
	char		Bayar[12+1];
	char		BayarFmt[20+1];

	char 	screenNoMHS[21+1];
	char 	screenNama[21+1];
	char 	screenJenisPembayaran[21+1];
	char 	screenBayar[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN SPP ONLINE");
			
	iRet = TransInit(TRANS_TYPE_INQ_BAYARKULIAH);
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
	
	MEM_0x20(bit48Req);
	MEM_ZERO(KodeUniv);
	MEM_ZERO(noMahasiswa);
	
	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "KODE UNIV");
	ucRet = PubGetString(NUM_IN, 3,3 , KodeUniv, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	bit48Req[0] = '0';
	bit48Req[1] = '0';
	memcpy(bit48Req+2,KodeUniv,strlen(KodeUniv));

	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", "NO BILLING /");
	ScrPrint(0, 3, ASCII, "%s", "NO MAHASISWA");
	ucRet = PubGetString(NUM_IN, 10, 20, noBill, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req+5,noBill,strlen(noBill));

	strcpy(glProcInfo.szReq48,bit48Req);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackInqPembayaranKuliah();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(noMahasiswa);
	MEM_ZERO(nama);
	MEM_ZERO(JenisPembayaran);
	MEM_ZERO(Bayar);
	MEM_ZERO(BayarFmt);
	
	memcpy(noMahasiswa,bit48Rsp+20,20);
	memcpy(nama,bit48Rsp+60,20);
	memcpy(JenisPembayaran,bit48Rsp+80,20);
	memcpy(Bayar,glRecvPack.szBit4,12);
	fmtAmt(BayarFmt,Bayar,0,",.");

	memcpy(glProcInfo.stTranLog.szAmount,glRecvPack.szBit4,12);

	sprintf(screenNoMHS,"%s",noMahasiswa);
	sprintf(screenNama,"%s",nama);
	sprintf(screenJenisPembayaran,"%s",JenisPembayaran);
	sprintf(screenBayar,"Jmlh:Rp.%s",BayarFmt);

	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "No MAHASISWA"); 
	ScrPrint(0, 1, ASCII, "%s", noMahasiswa);
	ScrPrint(0, 2, ASCII, "%s", nama);  
	ScrPrint(0, 3, ASCII, "%s", "JENIS PEMBAYARAN");
	ScrPrint(0, 4, ASCII, "%s", JenisPembayaran);
	ScrPrint(0, 5, ASCII, "%s", screenBayar);  
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	iRet = TransInit(TRANS_TYPE_BAYARKULIAH);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackPembayaranKuliah();

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

		return ERR_NO_DISP;
	}

	unPackPembayaran_Pendidikan();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;

}


int TransInfoKodeKuliah(void)
{
	uchar 	ucRet;

lbl_1:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "001 U I");
	ScrPrint(0, 1, ASCII, "%s", "002 U N P A D");
	ScrPrint(0, 2, ASCII, "%s", "003 U M (MALANG)");
	ScrPrint(0, 3, ASCII, "%s", "004 UNEJ (JEMBER)");
	ScrPrint(0, 4, ASCII, "%s", "005 UIN BANDUNG");
	ScrPrint(0, 5, ASCII, "%s", "006 U T");
	ScrPrint(0, 7, ASCII, "%s", "PREV             NEXT");
	
	ucRet = PubYesNo(USER_OPER_TIMEOUT);
	if(ucRet!=0)
		return 0;

lbl_2:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "007 UIN JAKARTA");
	ScrPrint(0, 1, ASCII, "%s", "008 UNAS JAKARTA");
	ScrPrint(0, 2, ASCII, "%s", "009 ITN MALANG");
	ScrPrint(0, 3, ASCII, "%s", "010 U N A I R");
	ScrPrint(0, 4, ASCII, "%s", "011 I T B");
	ScrPrint(0, 5, ASCII, "%s", "012 UAJY YOGYA");
	ScrPrint(0, 7, ASCII, "%s", "PREV             NEXT");
	
	ucRet = PubYesNo(USER_OPER_TIMEOUT);
	if(ucRet!=0)
		goto lbl_1;

lbl_3:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "013 U I I");
	ScrPrint(0, 1, ASCII, "%s", "014 UNSRAT MANADO");
	ScrPrint(0, 2, ASCII, "%s", "015 UPI PADANG");
	ScrPrint(0, 3, ASCII, "%s", "016 U N D I P");
	ScrPrint(0, 4, ASCII, "%s", "017 U T M (MADURA)");
	ScrPrint(0, 5, ASCII, "%s", "018 U K S W");
	ScrPrint(0, 7, ASCII, "%s", "PREV             NEXT");
	
	ucRet = PubYesNo(USER_OPER_TIMEOUT);
	if(ucRet!=0)
		goto lbl_2;


lbl_4:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "019 U N N E S");
	ScrPrint(0, 1, ASCII, "%s", "020 UNDANA KUPANG");
	ScrPrint(0, 2, ASCII, "%s", "021 AHMAD DAHLAN");
	ScrPrint(0, 3, ASCII, "%s", "022 UNIV HALULEO");
	ScrPrint(0, 4, ASCII, "%s", "023 UNIV JANABADRA");
	ScrPrint(0, 5, ASCII, "%s", "024 UPN VETERAN YOGYA");
	ScrPrint(0, 7, ASCII, "%s", "PREV             NEXT");
	
	ucRet = PubYesNo(USER_OPER_TIMEOUT);
	if(ucRet!=0)
		goto lbl_3;

	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "025 S T P BANDUNG");
	ScrPrint(0, 1, ASCII, "%s", "026 U N S O E D");
	ScrPrint(0, 2, ASCII, "%s", "027 UIN SUMUT");
	ScrPrint(0, 7, ASCII, "%s", "PREV             NEXT");
	
	ucRet = PubYesNo(USER_OPER_TIMEOUT);
	if(ucRet!=0)
		goto lbl_4;

	return 0;
	
}

int TransIsiUlang(void)
{
	int		iRet;
	uchar	ucRet;
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		kodeArea[4+1];
	char		noGSM[13+1];
	char		noCDMA[9+1];
	char		Operator[15+1];
	char		bit48Req[35+1];
	char		nominal[12+1];
	char		labelOperator[21+1];
	char		nominalDsp[21+1];
	char		dspGSM[13+1];
	char		dspCDMA[13+1];

	MEM_ZERO(Operator);
	MEM_0x20(bit48Req);
	MEM_ZERO(dspGSM);
	MEM_ZERO(dspCDMA);
	MEM_ZERO(labelOperator);
	
	mapGetWord(traMnuItm, mnuItm);

	switch(mnuItm)
		{
			case mnuAxis25rb:
   			case mnuAxis50rb:
   			case mnuAxis75rb:
   			case mnuAxis100rb:
   			case mnuAxis150rb:
   			case mnuAxis200rb:
   			case mnuAxis300rb:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG AXIS");
				strcpy(labelOperator,"NO. AXIS :");
				strcpy(Operator,"AXIS");
				iRet = TransInit(TRANS_TYPE_AXIS);
				break;
			case mnuEsia25rb:
    			case mnuEsia50rb:
    			case mnuEsia100rb:
    			case mnuEsia150rb:
    			case mnuEsia250rb:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG ESIA");
				strcpy(labelOperator,"NO. ESIA :");
				strcpy(Operator,"ESIA");
				iRet = TransInit(TRANS_TYPE_ESIA);
				break;
			/*
			case mnuFren20rb:
    			case mnuFren25rb:
    			case mnuFren50rb:
    			case mnuFren100rb:
    			case mnuFren150rb:
    			case mnuFren200rb:
    			case mnuFren300rb:
    			case mnuFren500rb:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG FREN");
				strcpy(labelOperator,"NO. FREN/MOBI/HEPI :");
				strcpy(Operator,"FREN/MOBI/HEPI");
				iRet = TransInit(TRANS_TYPE_FREN_MOBI_HEPI);
				break;
			*/ // Ditutup tgl 6 Januari 2014
			case mnuImThree25rb:
    			case mnuImThree50rb:
    			case mnuImThree75rb:
    			case mnuImThree100rb:
    			case mnuImThree150rb:
    			case mnuImThree200rb:
    			case mnuImThree500rb:
    			case mnuImThree1jt:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG IM3");
				strcpy(labelOperator,"NO. IM3 :");
				strcpy(Operator,"IM3");
				iRet = TransInit(TRANS_TYPE_IM3);
				break;
			case mnuMentari25rb:
    			case mnuMentari50rb:
    			case mnuMentari100rb:
			case mnuMentari150rb:
    			case mnuMentari250rb:
    			case mnuMentari500rb:
    			case mnuMentari1jt:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG MENTARI");
				strcpy(labelOperator,"NO. MENTARI :");
				strcpy(Operator,"MENTARI");
				iRet = TransInit(TRANS_TYPE_MENTARI);
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
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG SIMPATI/AS");
				strcpy(labelOperator,"NO. SIMPATI/AS :");
				strcpy(Operator,"SIMPATI/AS");
				iRet = TransInit(TRANS_TYPE_SIMPATI_AS);
				break;
			case mnuSmartFren20rb:
			case mnuSmartFren25rb:
    			case mnuSmartFren50rb:
    			case mnuSmartFren100rb:
			case mnuSmartFren150rb:
    			case mnuSmartFren200rb:
			case mnuSmartFren300rb:
    			case mnuSmartFren500rb:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG SMARTFREN");
				strcpy(labelOperator,"NO. SMARTFREN :");
				strcpy(Operator,"SMARTFREN");
				iRet = TransInit(TRANS_TYPE_SMARTFREN);
				break;
			case mnuThree20rb:
   			case mnuThree50rb:
   			case mnuThree75rb:
   			case mnuThree100rb:
   			case mnuThree150rb:
   			case mnuThree300rb:
   			case mnuThree500rb:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG THREE");
				strcpy(labelOperator,"NO. THREE :");
				strcpy(Operator,"THREE");
				iRet = TransInit(TRANS_TYPE_THREE);
				break;
			case mnuXLV20rb:
			case mnuXLV25rb:
    			case mnuXLV50rb:
    			case mnuXLV100rb:
    			case mnuXLV200rb:
			case mnuXLV500rb:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG XL");
				strcpy(labelOperator,"NO. XL :");
				strcpy(Operator,"XL");
				iRet = TransInit(TRANS_TYPE_XL);
				break;
		}

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
		
	MEM_ZERO(nominal);
	MEM_ZERO(nominalDsp);

	switch(mnuItm)
		{
		case mnuSimpati10rb:
		memcpy(glProcInfo.stTranLog.szAmount, "000001000000", 12);
		memcpy(nominal, "000000010000", 12);
		memcpy(nominalDsp, "Rp.            10.000", 21);
			break;
		case mnuSimpati20rb:
		case mnuSmartFren20rb:
		case mnuThree20rb:	
		case mnuXLV20rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000002000000", 12);
			memcpy(nominal, "000000020000", 12);
			memcpy(nominalDsp, "Rp.            20.000", 21);
			break;
		case mnuAxis25rb:
		case mnuEsia25rb:
		case mnuSmartFren25rb:
		case mnuImThree25rb:
		case mnuMentari25rb:
		case mnuSimpati25rb:
		case mnuXLV25rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);
			break;
		case mnuAxis50rb:
		case mnuEsia50rb:
		//case mnuFren50rb:
		case mnuImThree50rb:
		case mnuMentari50rb:
		case mnuSimpati50rb:
		case mnuSmartFren50rb:
		case mnuThree50rb:
		case mnuXLV50rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000005000000", 12);
			memcpy(nominal, "000000050000", 12);
			memcpy(nominalDsp, "Rp.            50.000", 21);
			break;
		case mnuAxis75rb:
		case mnuImThree75rb:
		case mnuThree75rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000007500000", 12);
			memcpy(nominal, "000000075000", 12);
			memcpy(nominalDsp, "Rp.            75.000", 21);
			break;
		case mnuAxis100rb:
		case mnuEsia100rb:
		//case mnuFren100rb:
		case mnuImThree100rb:
		case mnuMentari100rb:
		case mnuSimpati100rb:
		case mnuSmartFren100rb:
		case mnuThree100rb:
		case mnuXLV100rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000010000000", 12);
			memcpy(nominal, "000000100000", 12);
			memcpy(nominalDsp, "Rp.           100.000", 21);
			break;
		case mnuAxis150rb:
		case mnuEsia150rb:
		case mnuSmartFren150rb:
		case mnuImThree150rb:
		case mnuMentari150rb:
		case mnuSimpati150rb:
		case mnuThree150rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000015000000", 12);
			memcpy(nominal, "000000150000", 12);
			memcpy(nominalDsp, "Rp.           150.000", 21);
			break;
		case mnuAxis200rb:
		//case mnuFren200rb:
		case mnuImThree200rb:
		case mnuSimpati200rb:
		case mnuSmartFren200rb:
		case mnuXLV200rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000020000000", 12);
			memcpy(nominal, "000000200000", 12);
			memcpy(nominalDsp, "Rp.           200.000", 21);
			break;
		case mnuEsia250rb:
		case mnuMentari250rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000025000000", 12);
			memcpy(nominal, "000000250000", 12);
			memcpy(nominalDsp, "Rp.           250.000", 21);
			break;
		case mnuAxis300rb:
		case mnuSmartFren300rb:
		case mnuSimpati300rb:
		case mnuThree300rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000030000000", 12);
			memcpy(nominal, "000000300000", 12);
			memcpy(nominalDsp, "Rp.           300.000", 21);
			break;
		case mnuSmartFren500rb:
		case mnuImThree500rb:
		case mnuMentari500rb:
		case mnuSimpati500rb:
		case mnuThree500rb:
		case mnuXLV500rb:
			memcpy(glProcInfo.stTranLog.szAmount, "000050000000", 12);
			memcpy(nominal, "000000500000", 12);
			memcpy(nominalDsp, "Rp.           500.000", 21);
			break;
		case mnuImThree1jt:
		case mnuMentari1jt:
		case mnuSimpati1jt:
			memcpy(glProcInfo.stTranLog.szAmount, "000100000000", 12);
			memcpy(nominal, "000001000000", 12);
			memcpy(nominalDsp, "Rp.         1.000.000", 21);
			break;
		}

	switch(mnuItm)
		{

		case mnuAxis25rb:
   		case mnuAxis50rb:
   		case mnuAxis75rb:
   		case mnuAxis100rb:
   		case mnuAxis150rb:
   		case mnuAxis200rb:
   		case mnuAxis300rb:
		case mnuImThree25rb:
    		case mnuImThree50rb:
    		case mnuImThree75rb:
    		case mnuImThree100rb:
    		case mnuImThree150rb:
    		case mnuImThree200rb:
    		case mnuImThree500rb:
    		case mnuImThree1jt:
		case mnuMentari25rb:
    		case mnuMentari50rb:
    		case mnuMentari100rb:
		case mnuMentari150rb:
    		case mnuMentari250rb:
    		case mnuMentari500rb:
    		case mnuMentari1jt:
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
		case mnuSmartFren20rb:
		case mnuSmartFren25rb:
    		case mnuSmartFren50rb:
    		case mnuSmartFren100rb:
		case mnuSmartFren150rb:
    		case mnuSmartFren200rb:	
		case mnuSmartFren300rb:
		case mnuSmartFren500rb:
		case mnuThree20rb:
   		case mnuThree50rb:
   		case mnuThree75rb:
   		case mnuThree100rb:
   		case mnuThree150rb:
   		case mnuThree300rb:
   		case mnuThree500rb:	
		case mnuXLV20rb:	
		case mnuXLV25rb:
    		case mnuXLV50rb:
    		case mnuXLV100rb:
    		case mnuXLV200rb:
		case mnuXLV500rb:
			MEM_ZERO(noGSM);

			ScrCls();
			DispTransName();
			//ScrPrint(0, 2, ASCII, "%s", "ENTER NOMOR");
			ScrPrint(0, 2, ASCII, "%s", "MASUKKAN NOMOR");
			ScrPrint(0, 3, ASCII, "%s", Operator);
			ucRet = PubGetString(NUM_IN, 4,13 , noGSM, USER_OPER_TIMEOUT);
			if(ucRet!=0)
			{
				return ERR_USERCANCEL;
			}

			memcpy(dspGSM,noGSM,strlen(noGSM));
			memcpy(bit48Req,noGSM,strlen(noGSM));
			memcpy(bit48Req+13,nominal,strlen(nominal));
			memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
			break;
		case mnuEsia25rb:
    		case mnuEsia50rb:
    		case mnuEsia100rb:
    		case mnuEsia150rb:
    		case mnuEsia250rb:
			MEM_ZERO(kodeArea);

			ScrCls();
			DispTransName();
			//ScrPrint(0, 2, ASCII, "%s", "ENTER NOMOR");
			ScrPrint(0, 2, ASCII, "%s", "MASUKKAN NOMOR");
			ScrPrint(0, 3, ASCII, "%s", "KODE AREA");
			ucRet = PubGetString(NUM_IN, 3,4 , kodeArea, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}
			memcpy(dspCDMA,kodeArea,strlen(kodeArea));
			memcpy(bit48Req,kodeArea,strlen(kodeArea));
			MEM_0x20(noCDMA);
		
			ScrCls();
			DispTransName();
			//ScrPrint(0, 2, ASCII, "%s", "ENTER NOMOR");
			ScrPrint(0, 2, ASCII, "%s", "MASUKKAN NOMOR");
			ScrPrint(0, 3, ASCII, "%s", Operator);
			ucRet = PubGetString(NUM_IN, 4,9 , noCDMA, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}
			strcat(dspCDMA,noCDMA);
			memcpy(bit48Req+4,noCDMA,strlen(noCDMA));
			memcpy(bit48Req+13,nominal,strlen(nominal));
			memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
			break;
		}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", labelOperator);

	switch(mnuItm)
		{
		case mnuAxis25rb:
   		case mnuAxis50rb:
   		case mnuAxis75rb:
   		case mnuAxis100rb:
   		case mnuAxis150rb:
   		case mnuAxis200rb:
   		case mnuAxis300rb:
		case mnuImThree25rb:
    		case mnuImThree50rb:
    		case mnuImThree75rb:
    		case mnuImThree100rb:
    		case mnuImThree150rb:
    		case mnuImThree200rb:
    		case mnuImThree500rb:
    		case mnuImThree1jt:
		case mnuMentari25rb:
    		case mnuMentari50rb:
    		case mnuMentari100rb:
		case mnuMentari150rb:
    		case mnuMentari250rb:
    		case mnuMentari500rb:
    		case mnuMentari1jt:
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
		case mnuSmartFren20rb:
		case mnuSmartFren25rb:
    		case mnuSmartFren50rb:
    		case mnuSmartFren100rb:
		case mnuSmartFren150rb:
    		case mnuSmartFren200rb:		
		case mnuSmartFren300rb:
		case mnuSmartFren500rb:
		case mnuThree20rb:
   		case mnuThree50rb:
   		case mnuThree75rb:
   		case mnuThree100rb:
   		case mnuThree150rb:
   		case mnuThree300rb:
   		case mnuThree500rb:	
		case mnuXLV20rb:
		case mnuXLV25rb:
    		case mnuXLV50rb:
    		case mnuXLV100rb:
    		case mnuXLV200rb:
		case mnuXLV500rb:
			ScrPrint(0, 3, ASCII, "%s", dspGSM);
			break;
		case mnuEsia25rb:
    		case mnuEsia50rb:
    		case mnuEsia100rb:
    		case mnuEsia150rb:
    		case mnuEsia250rb:
			ScrPrint(0, 3, ASCII, "%s", dspCDMA);
			break;
		}
	ScrPrint(0, 4, ASCII, "%s", "NOMINAL :");
	ScrPrint(0, 5, ASCII, "%s", nominalDsp);
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	//SET ISO 8583
	setPackIsiUlang();


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
		if ( memcmp(glProcInfo.stTranLog.szRspCode, "68", 2 )==0 )
			{
				CommOnHook(FALSE);
				SaveRevInfo(FALSE);
			}
		else
			{
				CommOnHook(FALSE);
				SaveRevInfo(FALSE);

				return ERR_NO_DISP;
			}
	}

	unPackISi_Ulang();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}

	return 0;
}


int TransAbsensi(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	char		bit48Rsp[999+1];
	ulong	lenData;
	char		Info[2+1];
	char		JamSekarang[10+1];
	char		JamCepat_Telat[10+1];

	char		screenJamSekarang[21+1];
	char		screenJamCepat_Telat[21+1];

	iRet = TransInit(TRANS_TYPE_ABSENSI);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackAbsensi();


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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(JamSekarang);
	MEM_ZERO(JamCepat_Telat);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	MEM_ZERO(Info);
	memcpy(Info,bit48Rsp+1,2);

	ScrCls();
	DispTransName();

	if ( memcmp(Info, "00", 2)==0 )
	{
		memcpy(JamSekarang,bit48Rsp+3,10);
		sprintf(screenJamSekarang,"Time: %s",JamSekarang);
		PubDispStringASCII(screenJamSekarang, DISP_LINE_CENTER|4);
//		ScrPrint(0, 3, ASCII, "%s", screenJamSekarang);
	}
	else	if ( memcmp(Info, "TL", 2)==0 )
	{
		memcpy(JamSekarang,bit48Rsp+3,10);
		memcpy(JamCepat_Telat,bit48Rsp+12,10);
		sprintf(screenJamCepat_Telat,"Terlambat: %s",JamCepat_Telat);
		sprintf(screenJamSekarang,"Time: %s",JamSekarang);
		PubDispStringASCII(screenJamSekarang, DISP_LINE_CENTER|4);
		PubDispStringASCII(screenJamCepat_Telat, DISP_LINE_CENTER|5);
	}
	else if ( memcmp(Info, "CP", 2)==0 )
	{
		memcpy(JamSekarang,bit48Rsp+3,10);
		memcpy(JamCepat_Telat,bit48Rsp+12,10);
		sprintf(screenJamCepat_Telat,"Lebih Cepat: %s",JamCepat_Telat);
		sprintf(screenJamSekarang,"Time: %s",JamSekarang);
		PubDispStringASCII(screenJamSekarang, DISP_LINE_CENTER|4);
		PubDispStringASCII(screenJamCepat_Telat, DISP_LINE_CENTER|5);
	}

	PubWaitKey(5);

	return 0;
}

int TransTestKoneksi(void)
{
	int iRet;
	word idx;
//	byte logonFlag;
	char AcqName[lenName + 1];
	uchar bufTWK[999];
	uchar ucEntryMode=0;
//	uchar acqId;


	iRet = TransInit(TRANS_TYPE_TEST_KONEKSI);
	if( iRet!=0 )
	{
		return iRet;
	}
/*
	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		mapMove(acqBeg, idx);
		mapGetByte(acqID, acqId);
		if(acqId==glCurTlmTmsAcq.acqID)
			break;
	}
*/

	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		MEM_ZERO(AcqName);
		mapMove(acqBeg, idx);
		memcpy(AcqName, glTlmSysParam.Acq[idx].acqName, lenName);
		//if ((memcmp(AcqName, "BRIMINIATM",10)==0) || (memcmp(AcqName, "BRI DEBIT",9)==0) )
		if (memcmp(AcqName, "BRIMINIATM",10)==0) 
		break;
	}
	
	FindAcq(glTlmSysParam.Acq[idx].acqID);

	//if(glCurTlmTmsIss.issPinOpt==0x30)
	//	return 0;

	//mapGetByte(acqLogonFlag, logonFlag);

	setPackLogon();

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
	

	if( memcmp(glRecvPack.szBit39, "00", 2)!=0 )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glRecvPack.szBit39);
#ifdef BANK_BRI
		displayResponse();
#endif
		return ERR_NO_DISP;
	}

	mapPutByte(acqLogonFlag, 1);

	MEM_ZERO(bufTWK);
	memcpy(bufTWK, glRecvPack.szBit62+2, PubChar2Long(glRecvPack.szBit62, 2, NULL));

	mapPut(acqWorkingKey, bufTWK, 16);
	PubAsc2Bcd(bufTWK, 16, glSysCtrl.WorkingKey);

/*
	iRet = GetPinKeyPerAcq();
	if(iRet<0)
	{
		return ERR_NO_DISP;
	}
*/
//#ifdef BANK_BRI
	displayResponse();
//#endif
	return 0;
	
}

int TransLogon(void)
{
	int iRet;
	word idx;
//	byte logonFlag;
	char AcqName[lenName + 1];
	uchar bufTWK[999];
	uchar ucEntryMode=0;
//	uchar acqId;
	uchar datetime[14+1];

	iRet = TransInit(TRANS_TYPE_LOGON_BRI);
	if( iRet!=0 )
	{
		return iRet;
	}
/*
	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		mapMove(acqBeg, idx);
		mapGetByte(acqID, acqId);
		if(acqId==glCurTlmTmsAcq.acqID)
			break;
	}
*/

	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		MEM_ZERO(AcqName);
		mapMove(acqBeg, idx);
		memcpy(AcqName, glTlmSysParam.Acq[idx].acqName, lenName);
		//if ((memcmp(AcqName, "BRIMINIATM",10)==0) || (memcmp(AcqName, "BRI DEBIT",9)==0) )
		if (memcmp(AcqName, "BRIMINIATM",10)==0) 
		break;
	}
	
	FindAcq(glTlmSysParam.Acq[idx].acqID);

	//if(glCurTlmTmsIss.issPinOpt==0x30)
	//	return 0;

	//mapGetByte(acqLogonFlag, logonFlag);

	setPackLogon();

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
	

	if( memcmp(glRecvPack.szBit39, "00", 2)!=0 )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glRecvPack.szBit39);
		displayResponse();
		return ERR_NO_DISP;
	}

	mapPutByte(acqLogonFlag, 1);

	MEM_ZERO(bufTWK);
	memcpy(bufTWK, glRecvPack.szBit62+2, PubChar2Long(glRecvPack.szBit62, 2, NULL));

	mapPut(acqWorkingKey, bufTWK, 16);
	PubAsc2Bcd(bufTWK, 16, glSysCtrl.WorkingKey);

	GetDateTime(datetime);
	memcpy(glSysCtrl.stTlmApp.appStartTimerWorkingKey, datetime, 14);
		
/*
	iRet = GetPinKeyPerAcq();
	if(iRet<0)
	{
		return ERR_NO_DISP;
	}
*/
	displayResponse();
	SaveSysCtrlBase();
	return 0;
	
}

void DisplayMiniAtmTransaction(void)
{
	uchar buffer[23];
	uchar Date[10+1];

	memset(buffer, 0, sizeof(buffer));
	ScrCls();
	//TrxName
	PubDispStringASCII(glProcInfo.stTranLog.szTransName, DISP_LINE_LEFT|0);
	//Pan
	PubDispStringASCII(glProcInfo.stTranLog.szPan, DISP_LINE_LEFT|1);
	//Expdate
	memset(Date, 0, sizeof(Date));
	strcpy(Date, "  /");
	memcpy(Date, glProcInfo.stTranLog.szExpDate+2, 2);
	memcpy(Date+3, glProcInfo.stTranLog.szExpDate, 2);
	sprintf(buffer, "EXP. DATE: %10s", Date);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|2);
	//Amount
	PubDispStringASCII("NOMINAL  :", DISP_LINE_LEFT|3);
	memset(buffer, 0, sizeof(buffer));
	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, buffer, GA_SEPARATOR);
	PubDispStringASCII(buffer, DISP_LINE_RIGHT|4);
	//Date
	memset(buffer, 0, sizeof(buffer));
	memset(Date, 0, sizeof(Date));
	strcpy(Date, "  /  /");
	memcpy(Date, glProcInfo.stTranLog.szDateTime+6, 2);
	memcpy(Date+3, glProcInfo.stTranLog.szDateTime+4, 2);
	memcpy(Date+6, glProcInfo.stTranLog.szDateTime, 4);
	sprintf(buffer, "TANGGAL  : %10s", Date);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|5);
	
}

void DisplayMiniAtmDetailTransaction(void)
{
	uchar buffer[23];
	uchar Time[10+1];

	ScrCls();
	//TIME
	memset(buffer, 0, sizeof(buffer));
	memset(Time, 0, sizeof(Time));
	strcpy(Time, "  :  :");
	memcpy(Time, glProcInfo.stTranLog.szDateTime+8, 2);
	memcpy(Time+3, glProcInfo.stTranLog.szDateTime+10, 2);
	memcpy(Time+6, glProcInfo.stTranLog.szDateTime+12, 2);
	sprintf(buffer, "JAM    : %12s", Time);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|0);
	//STAN
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "STAN   :       %06lu", glProcInfo.stTranLog.ulSTAN);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|1);
	//TRACE
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "TRACE  :       %06lu", glProcInfo.stTranLog.ulInvoiceNo);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|2);
	//REFNUM
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "REF    : %12s", glProcInfo.stTranLog.szRRN);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|3);
		
}

void DisplayTunaiTransaction(void)
{
	uchar buffer[23];
	uchar Date[10+1];

	memset(buffer, 0, sizeof(buffer));
	ScrCls();
	//TrxName
	PubDispStringASCII(glProcInfo.stTranLog.szTransName, DISP_LINE_LEFT|0);
	//Pan
	PubDispStringASCII(glProcInfo.stTranLog.szPan, DISP_LINE_LEFT|1);
	//Expdate
	memset(Date, 0, sizeof(Date));
	strcpy(Date, "  /");
	memcpy(Date, glProcInfo.stTranLog.szExpDate+2, 2);
	memcpy(Date+3, glProcInfo.stTranLog.szExpDate, 2);
	sprintf(buffer, "EXP. DATE: %10s", Date);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|2);
	//Amount
	PubDispStringASCII("NOMINAL  :", DISP_LINE_LEFT|3);
	memset(buffer, 0, sizeof(buffer));
	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, buffer, GA_SEPARATOR);
	PubDispStringASCII(buffer, DISP_LINE_RIGHT|4);
	//Date
	memset(buffer, 0, sizeof(buffer));
	memset(Date, 0, sizeof(Date));
	strcpy(Date, "  /  /");
	memcpy(Date, glProcInfo.stTranLog.szDateTime+6, 2);
	memcpy(Date+3, glProcInfo.stTranLog.szDateTime+4, 2);
	memcpy(Date+6, glProcInfo.stTranLog.szDateTime, 4);
	sprintf(buffer, "TANGGAL  : %10s", Date);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|5);
	
}

void DisplayTunaiDetailTransaction(void)
{
	uchar buffer[23];
	uchar Time[10+1];

	ScrCls();
	//TIME
	memset(buffer, 0, sizeof(buffer));
	memset(Time, 0, sizeof(Time));
	strcpy(Time, "  :  :");
	memcpy(Time, glProcInfo.stTranLog.szDateTime+8, 2);
	memcpy(Time+3, glProcInfo.stTranLog.szDateTime+10, 2);
	memcpy(Time+6, glProcInfo.stTranLog.szDateTime+12, 2);
	sprintf(buffer, "JAM    : %12s", Time);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|0);
	//STAN
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "STAN   :       %06lu", glProcInfo.stTranLog.ulSTAN);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|1);
	//TRACE
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "TRACE  :       %06lu", glProcInfo.stTranLog.ulInvoiceNo);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|2);
	//REFNUM
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "REF    : %12s", glProcInfo.stTranLog.szRRN);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|3);
		
}


int CetakStruk(uchar ucPrnFlag)
{
	word 	mnuItm;
	mapGetWord(traMnuItm, mnuItm);
	
	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return 0;
	}

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SSB:
			PrnReceiptSSB(ucPrnFlag);
			return 0;
			break;
	}

	DispPrinting();
	if( !ChkIfThermalPrinter() )
	{
		return 0;
	}

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
/*
		switch(mnuItm)
			{
			case mnuRepLastTransMini:
			case mnuRepAnyTransMini:
				PrnDuplicateHeader();
				PrnStr("\n");
				break;
			}
*/		
		//TID MID
		PrnHeaderTidMid();

		PrnHeaderDateTime();
		
		PrnHeaderCardNumber();

		PrnHeaderTraceApprCode();

		PrnHeaderBatchRefNum();

		PrnStr("\n");
		PrnHeaderTransactionName();

		switch(glProcInfo.stTranLog.ucTranType)
		{
			case TRANS_TYPE_PRIVILAGE:
				PrnPrivilage();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_AKTIFASIKARTU_PRINT:
				PrnAktivasiKartu();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_REISSUE_PIN_NASABAH:
				PrnReissuePin();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_RENEWALKARTU_BARU:
				PrnRenewalKartu();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_UBAHPIN_CS:
				PrnUbahPinCS();
				break;
			case TRANS_TYPE_UBAHPWD_SPV:
				PrnUbahPinSPV();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SMSBANKING:
				PrnRegistrasiSMSBanking();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_PHONEBANKING:
				PrnRegistrasiPhoneBanking();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_IBANKING:
				PrnRegistrasiIBanking();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_TRX_PB_TRANSFER:
				PrnRegistrasiTRXPBTransfer();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_TRX_PB_NO_HP:
				PrnRegistrasiTRXPBNoHP();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_INFOSALDO:
				PrnInfoSaldoBRI();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_INFOSALDO_BANKLAIN:
				PrnInfoSaldoBankLain();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_MINISTATEMENT:
				PrnMiniStatement();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_MUTASIREKENING:
				PrnMutasiRekening();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_TRFSESAMABRI:
				PrnTrfSesamaBRI();
				PrnFooterTrfSesamaBRI();
				break;
			case TRANS_TYPE_TRFANTARBANK:
				PrnTrfAntarBank();
				PrnFooterTrfSesamaBRI();
				break;
			case TRANS_TYPE_TELKOM:
				PrnPembTelkom();
				break;
			case TRANS_TYPE_KARTUHALO:
				PrnPascaBayarHalo();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_KARTUMATRIX:
				PrnPascaBayarMatrix();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_PLNPASCABAYAR:
				PrnPLNPascaBayar();
				break;
			case TRANS_TYPE_PLNPRABAYAR:
			case TRANS_TYPE_PLNTOKEN:
				PrnPLNPraBayar();
				break;
			//case TRANS_TYPE_PLNTOKEN:

			//	break;
			case TRANS_TYPE_KKBRI:
				PrnKK_BRI();
				PrnFooterKK_KTA();
				break;
			case TRANS_TYPE_KKCITIBANK:
			case TRANS_TYPE_KTACITIBANK:
			case TRANS_TYPE_KKSCB:
			case TRANS_TYPE_KTASCB:
			case TRANS_TYPE_KKANZ:
			case TRANS_TYPE_KKHSBC:
			case TRANS_TYPE_KTAHSBC:
			case TRANS_TYPE_KKRBS:
			case TRANS_TYPE_KTARBS:
				PrnKK_KTA();
				PrnFooterKK_KTA();
				break;
			case TRANS_TYPE_FIF:
			case TRANS_TYPE_BAF:
			case TRANS_TYPE_OTO:
			case TRANS_TYPE_FINANSIA:
			case TRANS_TYPE_VERENA:
			case TRANS_TYPE_WOM:
				PrnCicilan();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_ZAKAT:
			case TRANS_TYPE_INFAQ:
				PrnPembZakat();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_DPLK:
				PrnPembDPLK();
				PrnFooterInfoSaldo();
				break;	
			case TRANS_TYPE_GARUDA:
			case TRANS_TYPE_LIONAIR:
			case TRANS_TYPE_SRIWIJAYA:
			case TRANS_TYPE_MERPATI:
				PrnTiketPesawat();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_BRIVA:
				PrnBriva();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_BAYARKULIAH:
				PrnPendidikan();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_AXIS:
			case TRANS_TYPE_ESIA:
		//	case TRANS_TYPE_FREN_MOBI_HEPI: //ditutup 6 Januari 2014
			case TRANS_TYPE_MENTARI:
			case TRANS_TYPE_THREE:
			case TRANS_TYPE_XL:
				PrnIsiUlang();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SMARTFREN:
				PrnIsiUlangSmartFren();   
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_IM3:
				PrnIsiUlangIM3();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SIMPATI_AS:
				PrnIsiUlangSimpati();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_TARIK_TUNAI:
			case TRANS_TYPE_VOID_TARIK_TUNAI:
				PrnTarikTunai();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SETOR_PINJAMAN:
			case TRANS_TYPE_SETOR_SIMPANAN:
				PrnSetorTunai();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SETORPASTI:
				PrnSetorPasti();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SSB:
				PrnSSB();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_UBAH_PIN_BKPN:
				PrnGantiPinBkpn();
				//PrnFooterInfoSaldo();
				break;
//			case TRANS_TYPE_CASH_IN:
//				PrnCasIn();
//				PrnFooterInfoSaldo();
//				break;
//			case TRANS_TYPE_CASHOUT:
//				PrnCasOut();
//				PrnFooterInfoSaldo();
//				break;
		}

		if(ucPrnFlag)
		{
				PrnStr("\n");
				PrnStr("          **********REPRINT**********          ");		
		}
/*
		switch(mnuItm)
			{
			case mnuRepLastTransMini:
			case mnuRepAnyTransMini:
				PrnStr("\n");
				PrnStr("          **********REPRINT**********          ");
				break;
			}
*/		
		PrnStr("\n\n\n\n\n\n\n\n");
		StartPrinter();

	return 0;
}	


void MiniAtmParseSummary(uchar *TrxName, uint Count, uchar *Amt)
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

void MiniAtmPrintSummary(MINIATM_TOTAL_INFO *TotalInfo, ushort TotalCnt, uchar *TotalAmt)
{
	uchar 	szBuff[50+1];
	uint iLoop=0;


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

	for(iLoop=0;iLoop<64;iLoop++)
	{
		if(TotalInfo[iLoop].uiTotalCnt>0)
		{
			MiniAtmParseSummary(TotalInfo[iLoop].szNamaFitur, TotalInfo[iLoop].uiTotalCnt, TotalInfo[iLoop].szTotalAmt);
		}
	}
	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	MiniAtmParseSummary("TOTAL", TotalCnt, TotalAmt);	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);


	//FOOTER
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}


void TunaiParseSummary(uchar *TrxName, uint Count, uchar *Amt)
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


void TunaiPrintSummary(TUNAI_TOTAL_INFO *TotalInfo, ushort TotalCnt, uchar *TotalAmt)
{
	uchar 	szBuff[50+1];
	uint iLoop=0;


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

	for(iLoop=0;iLoop<64;iLoop++)
	{
		if(TotalInfo[iLoop].uiTotalCnt>0)
		{
			TunaiParseSummary(TotalInfo[iLoop].szNamaFitur, TotalInfo[iLoop].uiTotalCnt, TotalInfo[iLoop].szTotalAmt);
		}
	}
	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	TunaiParseSummary("TOTAL", TotalCnt, TotalAmt);	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);


	//FOOTER
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}


void CardServicesParseSummary(uchar *TrxName, uint Count)
{	
	int ret=0;
	char bufPrint[49+1];
	char	buffData[32+1];

	memset(bufPrint, 0, sizeof(bufPrint));
	memset(bufPrint, 0x20, sizeof(bufPrint)-2);
	memset(buffData, 0, sizeof(buffData));

	memcpy(bufPrint,TrxName,strlen(TrxName));

	sprintf(buffData, "%3lu", Count);
	memcpy(bufPrint+25,buffData,strlen(buffData));

	MultiLngPrnStr(bufPrint, DISP_LINE_LEFT);
	PrnStr("\n");
	
}


void CardServicesPrintSummary(CARDSERVICES_TOTAL_INFO *TotalInfo, ushort TotalCnt)
{
	uchar 	szBuff[50+1];
	uint iLoop=0;


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
	MultiLngPrnStr("TRANSAKSI               COUNT             \n", DISP_LINE_LEFT);
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);

	for(iLoop=0;iLoop<64;iLoop++)
	{
		if(TotalInfo[iLoop].uiTotalCnt>0)
		{
			CardServicesParseSummary(TotalInfo[iLoop].szNamaFitur, TotalInfo[iLoop].uiTotalCnt);
		}
	}
	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	CardServicesParseSummary("TOTAL", TotalCnt);	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);


	//FOOTER
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}




void TBankParseSummary(uchar *TrxName, uint Count)
{	
	int ret=0;
	char bufPrint[49+1];
	char	buffData[32+1];

	memset(bufPrint, 0, sizeof(bufPrint));
	memset(bufPrint, 0x20, sizeof(bufPrint)-2);
	memset(buffData, 0, sizeof(buffData));

	memcpy(bufPrint,TrxName,strlen(TrxName));

	sprintf(buffData, "%3lu", Count);
	memcpy(bufPrint+25,buffData,strlen(buffData));

	MultiLngPrnStr(bufPrint, DISP_LINE_LEFT);
	PrnStr("\n");
	
}


void TBankPrintSummary(TBANK_TOTAL_INFO *TotalInfo, ushort TotalCnt)
{
	uchar 	szBuff[50+1];
	uint iLoop=0;


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
	MultiLngPrnStr("TRANSAKSI               COUNT             \n", DISP_LINE_LEFT);
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);

	for(iLoop=0;iLoop<64;iLoop++)
	{
		if(TotalInfo[iLoop].uiTotalCnt>0)
		{
			TBankParseSummary(TotalInfo[iLoop].szNamaFitur, TotalInfo[iLoop].uiTotalCnt);
		}
	}
	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	TBankParseSummary("TOTAL", TotalCnt);	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);


	//FOOTER
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}

void MiniAtmParseDetail(uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	RecNo;
	ushort	lenRec=0;
	ushort pos=0;
	TRAN_LOG	stTranLog;

	uchar szBuff[50+1];
	uchar szTransType[24];
	uchar PrintBuff[24];
	uchar PrintBuff2[24];


	RecNo= glSysCtrl.uiMiniAtmLastRecNo;
	
	for(pos=0;pos<MAX_MINIATM_TRANLOG;pos++)
	{
		if(RecNo==(MAX_MINIATM_TRANLOG-1))
			RecNo=0;
		else
			RecNo++;

		if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]==INV_ACQ_KEY)
			continue;

		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadMiniAtmCSTranLog(&stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}

		FindAcq(stTranLog.ucAcqKey);
		FindIssuer(stTranLog.ucIssuerKey);

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

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_TRFSESAMABRI:
				sprintf(szTransType, "TRANSFER SESAMA BRI  ");
				break;
			case TRANS_TYPE_TRFANTARBANK:
				sprintf(szTransType,  "TRANSFER ANTAR BANK  ");
				break;
			case TRANS_TYPE_TELKOM:
				sprintf(szTransType,  "TELKOM               ");
				break;
			case TRANS_TYPE_KARTUHALO:
				sprintf(szTransType,  "HALO PASCABAYAR      ");
				break;
			case TRANS_TYPE_KARTUMATRIX:
				sprintf(szTransType,  "SELULAR PASCABAYAR    ");
				break;
			case TRANS_TYPE_PLNPASCABAYAR:
				sprintf(szTransType,  "PLN POSTPAID       ");
				break;
			case TRANS_TYPE_PLNPRABAYAR:
				sprintf(szTransType,  "PLN PREPAID         ");
				break;
			case TRANS_TYPE_KKBRI:
				sprintf(szTransType,  "KARTU KREDIT BRI     ");
				break;
			case TRANS_TYPE_KKCITIBANK:
				sprintf(szTransType,  "KARTU KREDIT CITIBANK");
				break;
			case TRANS_TYPE_KTACITIBANK:
				sprintf(szTransType,  "KTA CITIBANK         ");
				break;
			case TRANS_TYPE_KKSCB:
				sprintf(szTransType,  "KARTU KREDIT S. CHART");
				break;
			case TRANS_TYPE_KTASCB:
				sprintf(szTransType,  "KTA S. CHART         ");
				break;
			case TRANS_TYPE_KKHSBC:
				sprintf(szTransType,  "KARTU KREDIT HSBC    ");
				break;
			case TRANS_TYPE_KTAHSBC:
				sprintf(szTransType,  "KTA HSBC             ");
				break;
			case TRANS_TYPE_KTARBS:
				sprintf(szTransType,  "KTA RBS              ");
				break;
			case TRANS_TYPE_KKANZ:
				sprintf(szTransType,  "KARTU KREDIT ANZ     ");
				break;
			case TRANS_TYPE_FIF:
				sprintf(szTransType,  "CICILAN FIF          ");
				break;
			case TRANS_TYPE_BAF:
				sprintf(szTransType,  "CICILAN BAF          ");
				break;
			case TRANS_TYPE_OTO:
				sprintf(szTransType,  "CICILAN OTO          ");
				break;
			case TRANS_TYPE_FINANSIA:
				sprintf(szTransType,  "CICILAN FINANSIA  ");
				break;
			case TRANS_TYPE_VERENA:
				sprintf(szTransType,  "CICILAN VERENA       ");
				break;
			case TRANS_TYPE_WOM:
				sprintf(szTransType,  "PEMB CICILAN WOM     ");
				break;
			case TRANS_TYPE_ZAKAT:
				sprintf(szTransType,  "BAYAR ZAKAT          ");
				break;
			case TRANS_TYPE_INFAQ:
				sprintf(szTransType,  "BAYAR INFAQ          ");
				break;
			case TRANS_TYPE_DPLK:
				sprintf(szTransType,  "BAYAR DPLK           ");
				break;
			case TRANS_TYPE_GARUDA:
				sprintf(szTransType,  "TIKET GARUDA         ");
				break;
			case TRANS_TYPE_LIONAIR:
				sprintf(szTransType,  "TIKET LION           ");
				break;
			case TRANS_TYPE_SRIWIJAYA:
				sprintf(szTransType,  "TIKET SRIWIJAYA      ");
				break;
			case TRANS_TYPE_MERPATI:
				sprintf(szTransType,  "TIKET MERPATI    ");
				break;
			case TRANS_TYPE_BRIVA:
				sprintf(szTransType,  "TIKET BRIVA          ");
				break;
			case TRANS_TYPE_BAYARKULIAH:
				sprintf(szTransType,  "BAYAR SPP            ");
				break;
			case TRANS_TYPE_IM3:
				sprintf(szTransType,  "ISI PULSA IM3        ");
				break;
			case TRANS_TYPE_SIMPATI_AS:
				sprintf(szTransType,  "ISI PULSA SIMPATI/AS ");
				break;
			case TRANS_TYPE_MENTARI:
				sprintf(szTransType,  "ISI PULSA MENTARI    ");
				break;
			case TRANS_TYPE_ESIA:
				sprintf(szTransType,  "ISI PULSA ESIA       ");
				break;
			case TRANS_TYPE_SMARTFREN:
				sprintf(szTransType,  "ISI PULSA SMART      ");
				break;
			//ditutup 6 Januari 2014
			/*
			case TRANS_TYPE_FREN_MOBI_HEPI:
				sprintf(szTransType,  "ISI PULSA FREN       ");
				break;
			*/
			case TRANS_TYPE_THREE:
				sprintf(szTransType,  "ISI PULSA THREE      ");
				break;
			case TRANS_TYPE_AXIS:
				sprintf(szTransType,  "ISI PULSA AXIS       ");
				break;
			case TRANS_TYPE_XL:
				sprintf(szTransType,  "ISI PULSA XL         ");
				break;
			
		}
		
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(stTranLog.szAmount, PrintBuff, GA_SEPARATOR);

		PrnSetNormal();

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%-31.21s %16s\n", szTransType, PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MEM_ZERO(PrintBuff2);
		memcpy(PrintBuff, stTranLog.szPan, strlen(stTranLog.szPan));
		strcpy(PrintBuff2, "  /  ");
		memcpy(PrintBuff2, stTranLog.szExpDate, 2);
		memcpy(PrintBuff2+3, stTranLog.szExpDate+2, 2);
		sprintf((char *)szBuff, "%-42.19s %5s\n", PrintBuff, PrintBuff2);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		//STAN //TRACE
		MEM_ZERO(szBuff);
		sprintf(szBuff, "STAN : %-26.06lu TRACE : %06lu\n", stTranLog.ulSTAN, stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnDateTime();

		//REFNUM
		MEM_ZERO(szBuff);
		sprintf(szBuff, "REF# : %-26.12s APPR  : %6s\n", stTranLog.szRRN, stTranLog.szAuthCode);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		StartPrinter();
		
	}
	return;
}



void MiniAtmPrintDetail(uchar *date, uchar flagDate)
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
	MiniAtmParseDetail(date, flagDate);

	//FOOTER
	PrnInit();
	PrnSetNormal();
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}

void CardServicesParseDetail(uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	RecNo;
	ushort	lenRec=0;
	ushort pos=0;
	TRAN_LOG	stTranLog;

	uchar szBuff[50+1];
	uchar szTransType[24];
	uchar PrintBuff[24];
	uchar PrintBuff2[24];


	RecNo= glSysCtrl.uiCardServicesLastRecNo;
	
	for(pos=0;pos<MAX_MINIATM_TRANLOG;pos++)
	{
		if(RecNo==(MAX_MINIATM_TRANLOG-1))
			RecNo=0;
		else
			RecNo++;

		if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo+MAX_MINIATM_TRANLOG]==INV_ACQ_KEY)
			continue;

		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadMiniAtmCSTranLog(&stTranLog, (ushort)(RecNo+MAX_MINIATM_TRANLOG));
		if( iRet!=0 )
		{
			return;
		}

		FindAcq(stTranLog.ucAcqKey);
		FindIssuer(stTranLog.ucIssuerKey);

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

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_AKTIFASIKARTU_PRINT:
				sprintf(szTransType, "AKTIVASI KARTU");
				break;
			case TRANS_TYPE_RENEWALKARTU_BARU:
				sprintf(szTransType,  "RENEWAL KARTU");
				break;
			case TRANS_TYPE_REISSUE_PIN_NASABAH:
				sprintf(szTransType,  "REISSUE PIN");
				break;
			case TRANS_TYPE_UBAHPWD_SPV:
				sprintf(szTransType,  "UBAH PWD SPV");
				break;
			case TRANS_TYPE_UBAHPIN_CS:
				sprintf(szTransType,  "UBAH PIN");
				break;
		}
		

		PrnSetNormal();

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", szTransType);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MEM_ZERO(PrintBuff2);
		memcpy(PrintBuff, stTranLog.szPan, strlen(stTranLog.szPan));
		strcpy(PrintBuff2, "  /  ");
		memcpy(PrintBuff2, stTranLog.szExpDate, 2);
		memcpy(PrintBuff2+3, stTranLog.szExpDate+2, 2);
		sprintf((char *)szBuff, "%-42.19s %5s\n", PrintBuff, PrintBuff2);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		//STAN //TRACE
		MEM_ZERO(szBuff);
		sprintf(szBuff, "STAN : %-26.06lu TRACE : %06lu\n", stTranLog.ulSTAN, stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnDateTime();

		//REFNUM
		MEM_ZERO(szBuff);
		sprintf(szBuff, "REF# : %-26.12s APPR  : %6s\n", stTranLog.szRRN, stTranLog.szAuthCode);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		StartPrinter();
		
	}
	return;
}

void CardServicesPrintDetail(uchar *date, uchar flagDate)
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
	CardServicesParseDetail(date, flagDate);

	//FOOTER
	PrnInit();
	PrnSetNormal();
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}

void MiniAtmPrnLastTrans(void)
{
	int			iRet;
	ushort		RecNo=0;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	RecNo = glSysCtrl.uiMiniAtmLastRecNo;

	iRet = LoadMiniAtmCSTranLog(&glProcInfo.stTranLog, RecNo);
	if( iRet!=0 )
	{
		return;
	}
		
	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	CetakStruk(PRN_REPRINT);
	
}


void MiniAtmRePrnSpecTrans(void)
{
	int			iRet;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( GetMiniAtmTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetMiniAtmRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	CetakStruk(PRN_REPRINT);
}


void MiniAtmReviewLastTrans(void)
{
	int			iRet;
	ushort		RecNo=0;
	uchar		retKey=0;
	uchar		stop=FALSE;
	ushort 		startRecNo;

	PubShowTitle(TRUE, (uchar *)_T("REVIEW          "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	RecNo = glSysCtrl.uiMiniAtmLastRecNo;
	startRecNo = RecNo;

	iRet = LoadMiniAtmCSTranLog(&glProcInfo.stTranLog, RecNo);
	if( iRet!=0 )
	{
		return;
	}

#if 0
	do
	{
		PubShowTitle(TRUE, (uchar *)_T("REVIEW         "));
		FindAcq(glProcInfo.stTranLog.ucAcqKey);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

		DisplayMiniAtmTransaction();
		PubDispStringASCII("DETAIL  KELUAR", DISP_LINE_RIGHT|7);
		retKey = PubWaitKey(USER_INPUT_TIMEOUT);
		if(retKey == KEYUP || retKey== KEYDOWN)
		{
			do
			{
				DisplayMiniAtmDetailTransaction();
				PubDispStringASCII("DETAIL  KELUAR", DISP_LINE_RIGHT|7);
				retKey = PubWaitKey(USER_INPUT_TIMEOUT);
				if(retKey == KEYUP || retKey== KEYDOWN)
					break;
				else if(retKey==KEYFN || retKey == KEYCANCEL || retKey == NOKEY)
					return;
			}while(TRUE);
		}
		else if(retKey==KEYFN || retKey == KEYCANCEL || retKey == NOKEY)
			return;
		
	}while(TRUE);
#else
	do
		{
		PubShowTitle(TRUE, (uchar *)_T("REVIEW         "));
		FindAcq(glProcInfo.stTranLog.ucAcqKey);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

		DisplayMiniAtmTransaction();
		PubDispStringASCII("^v   DETAIL", DISP_LINE_RIGHT|7);
		retKey = PubWaitKey(USER_INPUT_TIMEOUT);
		if(retKey == KEYFN)
		{
			do
			{
				DisplayMiniAtmDetailTransaction();
				PubDispStringASCII("^v   DETAIL", DISP_LINE_RIGHT|7);
				retKey = PubWaitKey(USER_INPUT_TIMEOUT);
				if(retKey == KEYFN)
					break;
				else if(retKey==KEYUP)
				{
					while(TRUE)
					{
						if(RecNo==0)
							RecNo=MAX_MINIATM_TRANLOG;

						RecNo--;
						
						if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]!= INV_ACQ_KEY)
							break;
						
					}	
					break;
				}
				else if(retKey == KEYDOWN)
				{
					while(TRUE)
					{
						if(RecNo==(MAX_MINIATM_TRANLOG-1))
							RecNo=0;
						else
							RecNo++;
						
						if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]!= INV_ACQ_KEY)
							break;
						
					}	
					break;
				}
				else if(retKey== KEYCANCEL || retKey == NOKEY)
					return;
				else if(retKey ==KEYATM1)
				{
					ScrCls();
					CetakStruk(PRN_REPRINT);
				}
			}while(TRUE);
		}
		else if(retKey==KEYUP)
		{
			while(TRUE)
			{
				if(RecNo==0)
					RecNo=MAX_MINIATM_TRANLOG;

				RecNo--;
				
				if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]!= INV_ACQ_KEY)
					break;
				
			}		
		}
		else if(retKey == KEYDOWN)
		{
			while(TRUE)
			{
				if(RecNo==(MAX_MINIATM_TRANLOG-1))
					RecNo=0;
				else
					RecNo++;
				
				if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]!= INV_ACQ_KEY)
					break;
				
			}	
			
		}
		else if(retKey==KEYCANCEL || retKey == NOKEY)
			return;
		else if(retKey ==KEYATM1)
		{
			ScrCls();
			CetakStruk(PRN_REPRINT);
		}
		
		iRet = LoadMiniAtmCSTranLog(&glProcInfo.stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}
		
	}while(TRUE);
#endif
}


void MiniAtmReviewAnyTrans(void)
{
	int			iRet;
	ushort		RecNo=0;
	uchar		retKey=0;
	uchar		stop=FALSE;
//	ushort 		startRecNo;

	PubShowTitle(TRUE, (uchar *)_T("REVIEW          "));
	if( GetMiniAtmTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetMiniAtmRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	RecNo = glProcInfo.uiRecNo;
	do
	{
		PubShowTitle(TRUE, (uchar *)_T("REVIEW         "));
		FindAcq(glProcInfo.stTranLog.ucAcqKey);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

		DisplayMiniAtmTransaction();
		PubDispStringASCII("^v   DETAIL", DISP_LINE_RIGHT|7);
		retKey = PubWaitKey(USER_INPUT_TIMEOUT);
		if(retKey == KEYFN)
		{
			do
			{
				DisplayMiniAtmDetailTransaction();
				PubDispStringASCII("^v   DETAIL", DISP_LINE_RIGHT|7);
				retKey = PubWaitKey(USER_INPUT_TIMEOUT);
				if(retKey == KEYFN)
					break;
				else if(retKey==KEYUP)
				{
					while(TRUE)
					{
						if(RecNo==0)
							RecNo=MAX_MINIATM_TRANLOG;

						RecNo--;
						
						if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]!= INV_ACQ_KEY)
							break;
						
					}	
					break;
				}
				else if(retKey == KEYDOWN)
				{
					while(TRUE)
					{
						if(RecNo==(MAX_MINIATM_TRANLOG-1))
							RecNo=0;
						else
							RecNo++;
						
						if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]!= INV_ACQ_KEY)
							break;
						
					}	
					break;
				}
				else if(retKey== KEYCANCEL || retKey == NOKEY)
					return;
				else if(retKey ==KEYATM1)
				{
					ScrCls();
					CetakStruk(PRN_REPRINT);
				}			
			}while(TRUE);
		}
		else if(retKey==KEYUP)
		{
			while(TRUE)
			{
				if(RecNo==0)
					RecNo=MAX_MINIATM_TRANLOG;

				RecNo--;
				
				if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]!= INV_ACQ_KEY)
					break;
				
			}		
		}
		else if(retKey == KEYDOWN)
		{
			while(TRUE)
			{
				if(RecNo==(MAX_MINIATM_TRANLOG-1))
					RecNo=0;
				else
					RecNo++;
				
				if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]!= INV_ACQ_KEY)
					break;
				
			}	
			
		}
		else if(retKey==KEYCANCEL || retKey == NOKEY)
			return;
		else if(retKey ==KEYATM1)
		{
			ScrCls();
			CetakStruk(PRN_REPRINT);
		}

		
		iRet = LoadMiniAtmCSTranLog(&glProcInfo.stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}
		
	}while(TRUE);
	
}


void CardServicesPrnLastTrans(void)
{
	int			iRet;
	ushort		RecNo=0;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( glSysCtrl.uiCardServicesLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	RecNo = glSysCtrl.uiCardServicesLastRecNo;

	iRet = LoadMiniAtmCSTranLog(&glProcInfo.stTranLog, (ushort)(RecNo+MAX_MINIATM_TRANLOG));
	if( iRet!=0 )
	{
		return;
	}
		
	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	CetakStruk(PRN_NORMAL);
	
}


void CardServicesRePrnSpecTrans(void)
{
	int			iRet;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( GetCardServicesTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetCardServicesRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	CetakStruk(PRN_NORMAL);
}


/*
void MiniATMSummaryReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	MINIATM_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar TotalAmt[12+1];
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));
	MEM_ZERO(TotalAmt);

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	//ucRet = FindAcqIdxByName("MINIATM", FALSE);
	ucRet = FindAcqIdxByName("BKPN_DUMMY", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcMiniAtmTotal(&TotalInfo, &TotalCnt, TotalAmt, date,  TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	MiniAtmPrintSummary(TotalInfo, TotalCnt, TotalAmt);
}




void MiniATMDetailReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	MINIATM_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar TotalAmt[12+1];
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));
	MEM_ZERO(TotalAmt);

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	//ucRet = FindAcqIdxByName("MINIATM", FALSE);
	ucRet = FindAcqIdxByName("BKPN_DUMMY", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcMiniAtmTotal(&TotalInfo, &TotalCnt, TotalAmt, date, TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	MiniAtmPrintDetail( date, TRUE);
}
*/

void CardServicesSummaryReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	CARDSERVICES_TOTAL_INFO TotalInfo[64];
	ushort TotalCnt=0;
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
	if( glSysCtrl.uiCardServicesLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	ucRet = FindAcqIdxByName("MINIATM", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcCardServicesTotal(&TotalInfo, &TotalCnt, date, TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	CardServicesPrintSummary(TotalInfo, TotalCnt);
}

void CardServicesDetailReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	MINIATM_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
	if( glSysCtrl.uiCardServicesLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	ucRet = FindAcqIdxByName("MINIATM", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcCardServicesTotal(&TotalInfo, &TotalCnt, date, TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	CardServicesPrintDetail( date, TRUE);
}

void TunaiParseDetail(uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	RecNo;
	ushort	lenRec=0;
	ushort pos=0;
	TRAN_LOG	stTranLog;

	uchar szBuff[50+1];
	uchar szTransType[24];
	uchar PrintBuff[24];
	uchar PrintBuff2[24];


	RecNo= glSysCtrl.uiTunaiLastRecNo;
	
	for(pos=0;pos<MAX_TUNAI_TRANLOG;pos++)
	{
		if(RecNo==(MAX_TUNAI_TRANLOG-1))
			RecNo=0;
		else
			RecNo++;

		if(glSysCtrl.sTunaiAcqKeyList[RecNo]==INV_ACQ_KEY)
			continue;

		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadTunaiTBankTranLog(&stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}

		FindAcq(stTranLog.ucAcqKey);
		FindIssuer(stTranLog.ucIssuerKey);

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

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_SETOR_PINJAMAN:
				sprintf(szTransType, "SETOR PINJAMAN       ");
				break;
			case TRANS_TYPE_SETOR_SIMPANAN:
				sprintf(szTransType,  "SETOR SIMPANAN       ");
				break;
			case TRANS_TYPE_TARIK_TUNAI:
				sprintf(szTransType,  "TARIK TUNAI          ");
				break;			
			case TRANS_TYPE_VOID_TARIK_TUNAI:
				sprintf(szTransType,  "VOID TARIK TUNAI     ");
				break;	
		}
		
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(stTranLog.szAmount, PrintBuff, GA_SEPARATOR);

		PrnSetNormal();

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%-31.21s %16s\n", szTransType, PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MEM_ZERO(PrintBuff2);
		memcpy(PrintBuff, stTranLog.szPan, strlen(stTranLog.szPan));
		strcpy(PrintBuff2, "  /  ");
		memcpy(PrintBuff2, stTranLog.szExpDate, 2);
		memcpy(PrintBuff2+3, stTranLog.szExpDate+2, 2);
		sprintf((char *)szBuff, "%-42.19s %5s\n", PrintBuff, PrintBuff2);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		//STAN //TRACE
		MEM_ZERO(szBuff);
		sprintf(szBuff, "STAN : %-26.06lu TRACE : %06lu\n", stTranLog.ulSTAN, stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnDateTime();

		//REFNUM
		MEM_ZERO(szBuff);
		sprintf(szBuff, "REF# : %-26.12s APPR  : %6s\n", stTranLog.szRRN, stTranLog.szAuthCode);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		StartPrinter();
		
	}
	return;
}

void TunaiPrintDetail(uchar *date, uchar flagDate)
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
	TunaiParseDetail(date, flagDate);

	//FOOTER
	PrnInit();
	PrnSetNormal();
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}

void TBankParseDetail(uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	RecNo;
	ushort	lenRec=0;
	ushort pos=0;
	TRAN_LOG	stTranLog;

	uchar szBuff[50+1];
	uchar szTransType[24];
	uchar PrintBuff[24];
	uchar PrintBuff2[24];


	RecNo= glSysCtrl.uiTBankLastRecNo;
	
	for(pos=0;pos<MAX_TUNAI_TRANLOG;pos++)
	{
		if(RecNo==(MAX_TUNAI_TRANLOG-1))
			RecNo=0;
		else
			RecNo++;

		if(glSysCtrl.sTunaiAcqKeyList[RecNo+MAX_TUNAI_TRANLOG]==INV_ACQ_KEY)
			continue;

		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadTunaiTBankTranLog(&stTranLog, (ushort)(RecNo+MAX_TUNAI_TRANLOG));
		if( iRet!=0 )
		{
			return;
		}

		FindAcq(stTranLog.ucAcqKey);
		FindIssuer(stTranLog.ucIssuerKey);

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

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_CASH_IN:
				sprintf(szTransType, "CASH IN");
				break;
			case TRANS_TYPE_CASHOUT:
				sprintf(szTransType,  "CASH OUT");
				break;
		}
		

		PrnSetNormal();

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%s\n", szTransType);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

	if(stTranLog.ucTranType == TRANS_TYPE_CASH_IN)
	{
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MEM_ZERO(PrintBuff2);
		memcpy(PrintBuff, stTranLog.szPan, strlen(stTranLog.szPan));
		strcpy(PrintBuff2, "  /  ");
		memcpy(PrintBuff2, stTranLog.szExpDate, 2);
		memcpy(PrintBuff2+3, stTranLog.szExpDate+2, 2);
		sprintf((char *)szBuff, "%-42.19s %5s\n", PrintBuff, PrintBuff2);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		//STAN //TRACE
		MEM_ZERO(szBuff);
		sprintf(szBuff, "STAN : %-26.06lu TRACE : %06lu\n", stTranLog.ulSTAN, stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnDateTime();

		//REFNUM
		MEM_ZERO(szBuff);
		sprintf(szBuff, "REF# : %-26.12s APPR  : %6s\n", stTranLog.szRRN, stTranLog.szAuthCode);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		StartPrinter();
	}

	else if (stTranLog.ucTranType == TRANS_TYPE_CASHOUT)
	{
		//STAN //TRACE
		MEM_ZERO(szBuff);
		sprintf(szBuff, "STAN : %-26.06lu TRACE : %06lu\n", stTranLog.ulSTAN, stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnDateTime();

		//REFNUM
		MEM_ZERO(szBuff);
		sprintf(szBuff, "REF# : %-26.12s APPR  : %6s\n", stTranLog.szRRN, stTranLog.szAuthCode);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		StartPrinter();
	}
	}
	return;
}

void TBankPrintDetail(uchar *date, uchar flagDate)
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
	TBankParseDetail(date, flagDate);

	//FOOTER
	PrnInit();
	PrnSetNormal();
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}

void TunaiPrnLastTrans(void)
{
	int			iRet;
	ushort		RecNo=0;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( glSysCtrl.uiTunaiLastRecNo>=MAX_TUNAI_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	RecNo = glSysCtrl.uiTunaiLastRecNo;

	iRet = LoadTunaiTBankTranLog(&glProcInfo.stTranLog, RecNo);
	if( iRet!=0 )
	{
		return;
	}
		
	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	CetakStruk(PRN_REPRINT);
	
}

void TunaiRePrnSpecTrans(void)
{
	int			iRet;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( GetTunaiTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetTunaiRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	CetakStruk(PRN_REPRINT);
}


void TunaiReviewLastTrans(void)
{
	int			iRet;
	ushort		RecNo=0;
	uchar		retKey=0;
	uchar		stop=FALSE;
	ushort 		startRecNo;

	PubShowTitle(TRUE, (uchar *)_T("REVIEW          "));
	if( glSysCtrl.uiTunaiLastRecNo>=MAX_TUNAI_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	RecNo = glSysCtrl.uiTunaiLastRecNo;
	startRecNo = RecNo;

	iRet = LoadTunaiTBankTranLog(&glProcInfo.stTranLog, RecNo);
	if( iRet!=0 )
	{
		return;
	}

#if 0
	do
	{
		PubShowTitle(TRUE, (uchar *)_T("REVIEW         "));
		FindAcq(glProcInfo.stTranLog.ucAcqKey);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

		DisplayMiniAtmTransaction();
		PubDispStringASCII("DETAIL  KELUAR", DISP_LINE_RIGHT|7);
		retKey = PubWaitKey(USER_INPUT_TIMEOUT);
		if(retKey == KEYUP || retKey== KEYDOWN)
		{
			do
			{
				DisplayMiniAtmDetailTransaction();
				PubDispStringASCII("DETAIL  KELUAR", DISP_LINE_RIGHT|7);
				retKey = PubWaitKey(USER_INPUT_TIMEOUT);
				if(retKey == KEYUP || retKey== KEYDOWN)
					break;
				else if(retKey==KEYFN || retKey == KEYCANCEL || retKey == NOKEY)
					return;
			}while(TRUE);
		}
		else if(retKey==KEYFN || retKey == KEYCANCEL || retKey == NOKEY)
			return;
		
	}while(TRUE);
#else
	do
		{
		PubShowTitle(TRUE, (uchar *)_T("REVIEW         "));
		FindAcq(glProcInfo.stTranLog.ucAcqKey);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

		DisplayTunaiTransaction();
		PubDispStringASCII("^v   DETAIL", DISP_LINE_RIGHT|7);
		retKey = PubWaitKey(USER_INPUT_TIMEOUT);
		if(retKey == KEYFN)
		{
			do
			{
				DisplayTunaiDetailTransaction();
				PubDispStringASCII("^v   DETAIL", DISP_LINE_RIGHT|7);
				retKey = PubWaitKey(USER_INPUT_TIMEOUT);
				if(retKey == KEYFN)
					break;
				else if(retKey==KEYUP)
				{
					while(TRUE)
					{
						if(RecNo==0)
							RecNo=MAX_TUNAI_TRANLOG;

						RecNo--;
						
						if(glSysCtrl.sTunaiAcqKeyList[RecNo]!= INV_ACQ_KEY)
							break;
						
					}	
					break;
				}
				else if(retKey == KEYDOWN)
				{
					while(TRUE)
					{
						if(RecNo==(MAX_TUNAI_TRANLOG-1))
							RecNo=0;
						else
							RecNo++;
						
						if(glSysCtrl.sTunaiAcqKeyList[RecNo]!= INV_ACQ_KEY)
							break;
						
					}	
					break;
				}
				else if(retKey== KEYCANCEL || retKey == NOKEY)
					return;
				else if(retKey ==KEYATM1)
				{
					ScrCls();
					CetakStruk(PRN_REPRINT);
				}
			}while(TRUE);
		}
		else if(retKey==KEYUP)
		{
			while(TRUE)
			{
				if(RecNo==0)
					RecNo=MAX_TUNAI_TRANLOG;

				RecNo--;
				
				if(glSysCtrl.sTunaiAcqKeyList[RecNo]!= INV_ACQ_KEY)
					break;
				
			}		
		}
		else if(retKey == KEYDOWN)
		{
			while(TRUE)
			{
				if(RecNo==(MAX_TUNAI_TRANLOG-1))
					RecNo=0;
				else
					RecNo++;
				
				if(glSysCtrl.sTunaiAcqKeyList[RecNo]!= INV_ACQ_KEY)
					break;
				
			}	
			
		}
		else if(retKey==KEYCANCEL || retKey == NOKEY)
			return;
		else if(retKey ==KEYATM1)
		{
			ScrCls();
			CetakStruk(PRN_REPRINT);
		}
		
		iRet = LoadTunaiTBankTranLog(&glProcInfo.stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}
		
	}while(TRUE);
#endif
}


void TunaiReviewAnyTrans(void)
{
	int			iRet;
	ushort		RecNo=0;
	uchar		retKey=0;
	uchar		stop=FALSE;
//	ushort 		startRecNo;

	PubShowTitle(TRUE, (uchar *)_T("REVIEW          "));
	if( GetTunaiTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetTunaiRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	RecNo = glProcInfo.uiRecNo;
	do
	{
		PubShowTitle(TRUE, (uchar *)_T("REVIEW         "));
		FindAcq(glProcInfo.stTranLog.ucAcqKey);
		FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

		DisplayTunaiTransaction();
		PubDispStringASCII("^v   DETAIL", DISP_LINE_RIGHT|7);
		retKey = PubWaitKey(USER_INPUT_TIMEOUT);
		if(retKey == KEYFN)
		{
			do
			{
				DisplayTunaiDetailTransaction();
				PubDispStringASCII("^v   DETAIL", DISP_LINE_RIGHT|7);
				retKey = PubWaitKey(USER_INPUT_TIMEOUT);
				if(retKey == KEYFN)
					break;
				else if(retKey==KEYUP)
				{
					while(TRUE)
					{
						if(RecNo==0)
							RecNo=MAX_TUNAI_TRANLOG;

						RecNo--;
						
						if(glSysCtrl.sTunaiAcqKeyList[RecNo]!= INV_ACQ_KEY)
							break;
						
					}	
					break;
				}
				else if(retKey == KEYDOWN)
				{
					while(TRUE)
					{
						if(RecNo==(MAX_TUNAI_TRANLOG-1))
							RecNo=0;
						else
							RecNo++;
						
						if(glSysCtrl.sTunaiAcqKeyList[RecNo]!= INV_ACQ_KEY)
							break;
						
					}	
					break;
				}
				else if(retKey== KEYCANCEL || retKey == NOKEY)
					return;
				else if(retKey ==KEYATM1)
				{
					ScrCls();
					CetakStruk(PRN_REPRINT);
				}			
			}while(TRUE);
		}
		else if(retKey==KEYUP)
		{
			while(TRUE)
			{
				if(RecNo==0)
					RecNo=MAX_TUNAI_TRANLOG;

				RecNo--;
				
				if(glSysCtrl.sTunaiAcqKeyList[RecNo]!= INV_ACQ_KEY)
					break;
				
			}		
		}
		else if(retKey == KEYDOWN)
		{
			while(TRUE)
			{
				if(RecNo==(MAX_TUNAI_TRANLOG-1))
					RecNo=0;
				else
					RecNo++;
				
				if(glSysCtrl.sTunaiAcqKeyList[RecNo]!= INV_ACQ_KEY)
					break;
				
			}	
			
		}
		else if(retKey==KEYCANCEL || retKey == NOKEY)
			return;
		else if(retKey ==KEYATM1)
		{
			ScrCls();
			CetakStruk(PRN_REPRINT);
		}

		
		iRet = LoadTunaiTBankTranLog(&glProcInfo.stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}
		
	}while(TRUE);
	
}


void TBankPrnLastTrans(void)
{
	int			iRet;
	ushort		RecNo=0;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( glSysCtrl.uiTBankLastRecNo>=MAX_TUNAI_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	RecNo = glSysCtrl.uiTBankLastRecNo;

	iRet = LoadTunaiTBankTranLog(&glProcInfo.stTranLog, (ushort)(RecNo+MAX_TUNAI_TRANLOG));
	if( iRet!=0 )
	{
		return;
	}
		
	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	PrnTBank(PRN_REPRINT);
	
}


void TBankRePrnSpecTrans(void)
{
	int			iRet;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( GetTBankTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetTBankRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	PrnTBank(PRN_REPRINT);
}



void TunaiSummaryReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	TUNAI_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar TotalAmt[12+1];
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));
	MEM_ZERO(TotalAmt);

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
	if( glSysCtrl.uiTunaiLastRecNo>=MAX_TUNAI_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	ucRet = FindAcqIdxByName("TUNAI", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	//CalcMiniAtmTotal(&TotalInfo, &TotalCnt, TotalAmt, date,  TRUE);
	CalcTunaiTotal(&TotalInfo, &TotalCnt, TotalAmt, date,  TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	TunaiPrintSummary(TotalInfo, TotalCnt, TotalAmt);
}


void TunaiDetailReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	TUNAI_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar TotalAmt[12+1];
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));
	MEM_ZERO(TotalAmt);

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
	if( glSysCtrl.uiTunaiLastRecNo>=MAX_TUNAI_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	ucRet = FindAcqIdxByName("TUNAI", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcTunaiTotal(&TotalInfo, &TotalCnt, TotalAmt, date, TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	TunaiPrintDetail( date, TRUE);
}

void TBankSummaryReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	TBANK_TOTAL_INFO TotalInfo[64];
	ushort TotalCnt=0;
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
	if( glSysCtrl.uiTBankLastRecNo>=MAX_TUNAI_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	ucRet = FindAcqIdxByName("TUNAI", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcTBankTotal(&TotalInfo, &TotalCnt, date, TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	TBankPrintSummary(TotalInfo, TotalCnt);
}


void TBankDetailReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	TUNAI_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
	if( glSysCtrl.uiTBankLastRecNo>=MAX_TUNAI_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	ucRet = FindAcqIdxByName("TUNAI", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcTBankTotal(&TotalInfo, &TotalCnt, date, TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	TBankPrintDetail( date, TRUE);
}

int TransTarikTunai(void)
{
	int		iRet;
	uchar	ucRet;
	char		uang[20+1];
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		screenUang[21+1];

	mapGetWord(traMnuItm, mnuItm);

	MEM_ZERO(uang);
	MEM_ZERO(screenUang);

	strcpy(glProcInfo.stTranLog.szTransName,"TARIK TUNAI");
	
	iRet = TransInit(TRANS_TYPE_TARIK_TUNAI);
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

	iRet = InputNominal();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, screenUang, GA_SEPARATOR);
	
	ScrCls();
	DispTransName();
	PubDispStringASCII("NO KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(glProcInfo.stTranLog.szPan, DISP_LINE_LEFT|3);
	PubDispStringASCII("NOMINAL :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenUang, DISP_LINE_RIGHT|5);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	
	//SET ISO 8583
	setPackTarikTunai();
	
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

		return ERR_NO_DISP;
	}

	unPackTarikTunai();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTunaiTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}


int TransSetorPinjaman(void)
{
	int		iRet;
	uchar	ucRet;
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		screenUang[21+1];
	char 	bit48Req[39+1];
	char 	noRek[20+1];

	mapGetWord(traMnuItm, mnuItm);

	MEM_ZERO(screenUang);

	strcpy(glProcInfo.stTranLog.szTransName,"SETOR PINJAMAN");
	
	iRet = TransInit(TRANS_TYPE_SETOR_INQ_PINJ);
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

	MEM_ZERO(noRek);
	MEM_0x20(bit48Req);
	
	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '2';

	ScrCls();
	DispTransName();
	PubDispString("Rek Pinjaman:", DISP_LINE_LEFT|4);

	ucRet = PubGetString(NUM_IN, 15, 15 , noRek, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(glProcInfo.stTranLog.BRI.Tunai.NoRek, noRek, strlen(noRek));
	memcpy(bit48Req+3,noRek,strlen(noRek));
	memcpy(glProcInfo.szReq48, bit48Req,strlen(bit48Req));

	iRet = InputNominal();
	if( iRet!=0 )
	{
		return iRet;
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
	setPackSetorTunaiInq();
	
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

		displayResponse();
		
		return ERR_NO_DISP;
	}

	unPackSetorTunai();

	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, screenUang, GA_SEPARATOR);

	ScrCls();
	PubDispStringASCII("SETOR PINJAMAN", DISP_LINE_CENTER|0);
	PubDispStringASCII("Rek. Pinjaman :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noRek, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(glProcInfo.stTranLog.BRI.Tunai.Nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Nominal :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenUang, DISP_LINE_RIGHT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	iRet = TransInit(TRANS_TYPE_SETOR_PINJAMAN);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackSetorTunai();

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

		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTunaiTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}



int TransSetorSimpanan(void)
{
	int		iRet;
	uchar	ucRet;
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		screenUang[21+1];
	char 	bit48Req[39+1];
	char 	noRek[20+1];

	mapGetWord(traMnuItm, mnuItm);

	MEM_ZERO(screenUang);

	strcpy(glProcInfo.stTranLog.szTransName,"SETOR SIMPANAN");
	
	iRet = TransInit(TRANS_TYPE_SETOR_INQ_SIMP);
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

	MEM_ZERO(noRek);
	MEM_0x20(bit48Req);
	
	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '2';

	ScrCls();
	DispTransName();
	PubDispString("Rek Simpanan:", DISP_LINE_LEFT|4);

	ucRet = PubGetString(NUM_IN, 15, 15 , noRek, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(glProcInfo.stTranLog.BRI.Tunai.NoRek, noRek, strlen(noRek));
	memcpy(bit48Req+3,noRek,strlen(noRek));
	memcpy(glProcInfo.szReq48, bit48Req,strlen(bit48Req));

	iRet = InputNominal();
	if( iRet!=0 )
	{
		return iRet;
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
	setPackSetorTunaiInq();
	
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

		displayResponse();
		
		return ERR_NO_DISP;
	}

	unPackSetorTunai();

	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, screenUang, GA_SEPARATOR);

	ScrCls();
	PubDispStringASCII("SETOR SIMPANAN", DISP_LINE_CENTER|0);
	PubDispStringASCII("Rek. Simpanan :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noRek, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(glProcInfo.stTranLog.BRI.Tunai.Nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Nominal :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenUang, DISP_LINE_RIGHT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	iRet = TransInit(TRANS_TYPE_SETOR_SIMPANAN);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackSetorTunai();

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

		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTunaiTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}


int TransVoidTarikTunai(void)
{
	int			iRet;
	uchar		ucRet;
	uchar		ucTranAct;
	uchar		buffer[23+1];

	strcpy(glProcInfo.stTranLog.szTransName,"VOID TARIK TUNAI");

	iRet = TransInit(TRANS_TYPE_VOID_TARIK_TUNAI);
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	if( !ChkEdcOption(EDC_NOT_VOID_PWD) )
	{
		if( PasswordVoid()!=0 )
		{
			return ERR_NO_DISP;
		}
	}

	while( 1 )
	{
		iRet = GetTunaiRecord(TS_OK|TS_NOSEND|TS_ADJ, &glProcInfo.stTranLog);
		if( iRet!=0 )
		{
			return iRet;
		}

		ucTranAct = glTranConfig[glProcInfo.stTranLog.ucTranType].ucTranAct;
		if( ucTranAct & VOID_ALLOW )
		{
			glProcInfo.stTranLog.ucOrgTranType = glProcInfo.stTranLog.ucTranType;
			glProcInfo.stTranLog.ucTranType    = TRANS_TYPE_VOID_TARIK_TUNAI;
			strcpy(glProcInfo.stTranLog.szTransName,"VOID TARIK TUNAI");
			break;
		}

		PubDispString(_T("NOT ALLOW VOID"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	ScrCls();
	DispTransName();

	if(glCurTlmTmsIss.issVoid != TRUE)
	{
		DispBlockFunc();
		return ERR_NO_DISP;
	}

	if( !ChkSettle() )
	{
		return ERR_NO_DISP;
	}

	//always online
	ScrCls();
	DispTransName();
	DispWait();
	PreDial();


	ScrCls();
	DispTransName();
	PubDispStringASCII("NO KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(glProcInfo.stTranLog.szPan, DISP_LINE_LEFT|3);
	sprintf(buffer, "Trace No #     %06lu", glProcInfo.stTranLog.ulInvoiceNo);
	PubDispStringASCII(buffer, DISP_LINE_LEFT|4);
	PubDispStringASCII("Nominal :", DISP_LINE_LEFT|5);
	MEM_ZERO(buffer);
	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, buffer, GA_SEPARATOR);
	PubDispStringASCII(buffer, DISP_LINE_RIGHT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;
	
	ScrCls();
	DispTransName();

	setPackTarikTunai();

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

		return ERR_NO_DISP;
	}

	unPackTarikTunai();

	glProcInfo.stTranLog.uiStatus |= TS_VOID;
	UpdateTunaiTranLog(&glProcInfo.stTranLog, glProcInfo.uiRecNo);

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;

}

int TransCashIn(void)
{
	int		iRet;
	uchar 	ucRet;
	char		uang[20+1];
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		screenUang[21+1];
	char		bit48Req[60+1];
//	char 	bit48Rsp[lenBit48+1];	
	char		noHP[13+1];

	mapGetWord(traMnuItm, mnuItm);

	MEM_ZERO(uang);
	MEM_ZERO(screenUang);

	strcpy(glProcInfo.stTranLog.szTransName,"CASH IN DARI T-BANK");
	
	iRet = TransInit(TRANS_TYPE_CASH_IN);
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

	MEM_ZERO(noHP);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	PubDispString("ENTER NOMOR HP :", DISP_LINE_LEFT|2);
	ucRet = PubGetString(NUM_IN, 4,13 , noHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req,noHP,strlen(noHP));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	iRet = InputUang();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	fmtAmt(uang,glProcInfo.stTranLog.szAmount,2,",.");
	sprintf(screenUang,"Rp.%s",uang);
	
	ScrCls();
	ScrPrint(0, 1, ASCII, "%s", "Konfirmasi");
	ScrPrint(0, 2, ASCII, "%s", "Jumlah :");
	ScrPrint(0, 3, ASCII, "%s", screenUang);
	ScrPrint(0, 4, ASCII, "%s", "No HP :");
	ScrPrint(0, 5, ASCII, "%s", noHP);
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
	
	//SET ISO 8583
	setPackCashIn();

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

		return ERR_NO_DISP;
	}

	unPackCashIn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTBankTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		PrnTBank(PRN_NORMAL);
	}
		
	return 0;
}


int TransCashOut(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
//	char		bit35Req[37+1];
	char		bit48Req[60+1];
	char		KodeAkses[6+1];
	char		noHP[13+1];
	char		uang[20+1];
	char		screenUang[21+1];

	strcpy(glProcInfo.stTranLog.szTransName,"CASH OUT DARI T-BANK");
/*
	iRet = TransInit(TRANS_TYPE_INQ_CASHOUT);
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

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackInqCashOut();

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

//		displayResponse();
		return ERR_NO_DISP;
	}

	
	ScrCls();
	DispTransName();
	PubDispStringASCII("Otorisasi Agen", DISP_LINE_CENTER|2);
	PubDispStringASCII("Berhasil", DISP_LINE_CENTER|3);
	PubDispStringASCII("BATAL    LANJUT", DISP_LINE_CENTER|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;
*/
	iRet = TransInit(TRANS_TYPE_CASHOUT);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	glProcInfo.stTranLog.uiEntryMode = MODE_SWIPE_INPUT;
	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}

	/*iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	*/
	
	//MEM_ZERO(bit35Req);
	//memset(bit35Req, 0x20, sizeof(bit35Req)-1);
	//memcpy(bit35Req, "9999999999999999=10010000000000000000", 37);
	memcpy(glProcInfo.stTranLog.szPan,"9999999999999999=10010000000000000000",19);

	iRet = ValidCardCashOut();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(KodeAkses);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	PubDispString("ENTER KODE AKSES", DISP_LINE_LEFT|2);	
	ucRet = PubGetString(NUM_IN, 0,6 , KodeAkses, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req,KodeAkses,strlen(KodeAkses));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	MEM_ZERO(noHP);
	
	ScrCls();
	DispTransName();
	PubDispString("ENTER NOMOR HP", DISP_LINE_LEFT|2);	
	ucRet = PubGetString(NUM_IN, 4,13 , noHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	MEM_ZERO (glProcInfo.stTranLog.BRI.CashOut.noHP);
	memcpy(glProcInfo.stTranLog.BRI.CashOut.noHP, noHP, strlen(noHP));
	memcpy(bit48Req+6,noHP,strlen(noHP));
	memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));

	iRet = InputNominal();
	if( iRet!=0 )
	{
		return iRet;
	}

	fmtAmt(uang,glProcInfo.stTranLog.szAmount,2,",.");
	sprintf(screenUang,"Rp.%s",uang);
	
	ScrCls();
	ScrPrint(0, 1, ASCII, "%s", "Konfirmasi");
	ScrPrint(0, 2, ASCII, "%s", "Nominal: ");
	ScrPrint(0, 3, ASCII, "%s", screenUang);
	ScrPrint(0, 4, ASCII, "%s", "No HP :");
	ScrPrint(0, 5, ASCII, "%s", noHP);
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	ScrCls();
/*
	MEM_ZERO(bit35Req);
	memset(bit35Req, 0x20, sizeof(bit35Req)-1);
	memcpy(bit35Req, "9999999999999999=10010000000000000000", 37);
	memcpy(glProcInfo.szReq35,bit35Req,strlen(bit35Req));

	iRet = ValidCardCashOut();
	if( iRet!=0 )
	{
		return iRet;
	}
*/	
	setPackCashOut();
	
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
	if (( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0)  && (memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)!=0 ))
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	unPackCashOut();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTBankTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		PrnTBank(PRN_NORMAL);
	}

	return 0;
}

int TransSetorPasti(void)
{
	int		iRet;
	uchar	ucEntryMode=0;

	strcpy(glProcInfo.stTranLog.szTransName,"SETOR PASTI");
	iRet = TransInit(TRANS_TYPE_SETORPASTI);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCard(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}

	MatchBRIDebitCardTable(glProcInfo.stTranLog.szPan);
	
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

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	SetPackSetorPasti();

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

		return ERR_NO_DISP;
	}

	unPackSetorPasti();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}


static int selectTrxSSB(word menu){
	int ret=0;
	
	switch(menu){
		case mnuSIMBaruA:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "11");
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN SIM BARU");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM A");
			break;
		case mnuSIMBaruC:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "17");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM BARU");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM C");
			break;
		case mnuSIMBaruD:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "18");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM BARU");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM D");
			break;
		case mnuSIMPerpanjangA:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "21");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "PERPANJANGAN SIM A");
			break;
		case mnuSIMPerpanjangAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "22");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PERPANJANGAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "SIM A UMUM");
			break;
		case mnuSIMPerpanjangB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "23");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PERPANJANGAN SIM B1");
			break;
		case mnuSIMPerpanjangB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "24");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PERPANJANGAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "SIM B1 UMUM");
			break;
		case mnuSIMPerpanjangB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "25");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PERPANJANGAN SIM B2");
			break;
		case mnuSIMPerpanjangB2um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "26");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "PERPANJANGAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "SIM B2 UMUM");
			break;
		case mnuSIMPerpanjangC:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "27");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PERPANJANGAN SIM C");
			break;
		case mnuSIMPerpanjangD:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "28");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PERPANJANGAN SIM D");
			break;			
		case mnuSIMPeningkatAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "32");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "PENINGKATAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "SIM A UMUM");
			break;			
		case mnuSIMPeningkatB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "33");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENINGKATAN SIM B1");
			break;			
		case mnuSIMPeningkatB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "34");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "PENINGKATAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "SIM B1 UMUM");
			break;
		case mnuSIMPeningkatB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "35");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENINGKATAN SIM B2");
			break;
		case mnuSIMPeningkatB2um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "36");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "PENINGKATAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "SIM B2 UMUM");
			break;
		case mnuSIMPenurunA:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "41");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENURUNAN SIM A");
			break;
		case mnuSIMPenurunAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "42");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "PENURUNAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "SIM A UMUM");
			break;			
		case mnuSIMPenurunB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "43");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENURUNAN SIM B1");
			break;			
		case mnuSIMPenurunB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "44");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "PENURUNAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "SIM B1 UMUM");
			break;
		case mnuSIMPenurunB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "45");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENURUNAN SIM B2");
			break;
		case mnuMutGolA:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "51");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM A");
			break;
		case mnuMutGolAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "52");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM A UMUM");
			break;
		case mnuMutGolB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "53");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM B1");
			break;
		case mnuMutGolB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "54");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM B1 UMUM");
			break;
		case mnuMutGolB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "55");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM B2");
			break;
		case mnuMutGolB2um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "56");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM B2 UMUM");
			break;
		case mnuMutGolC:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "57");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM C");
			break;
		case mnuMutGolD:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "58");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIM D");
			break;
		case mnuMutPeningkatAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5B");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENINGKATAN SIM A UMUM");
			break;
		case mnuMutPeningkatB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5C");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENINGKATAN SIM B1");
			break;
		case mnuMutPeningkatB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5D");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENINGKATAN SIM B1 UMUM");
			break;
		case mnuMutPeningkatB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5E");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENINGKATAN SIM B2");
			break;
		case mnuMutPeningkatB2um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5F");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENINGKATAN SIM B2 UMUM");
			break;
		case mnuMutPenurunA:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5K");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENURUNAN SIM A");
			break;
		case mnuMutPenurunAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5L");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENURUNAN SIM A UMUM");
			break;
		case mnuMutPenurunB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5M");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENURUNAN SIM B1");
			break;
		case mnuMutPenurunB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5N");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENURUNAN SIM B1 UMUM");
			break;
		case mnuMutPenurunB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5O");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PENURUNAN SIM B2");
			break;
		case mnuHilGolA:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "61");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM HILANG/");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RUSAK SIM A");
			break;
		case mnuHilGolAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "62");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM HILANG/");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RUSAK SIM A UMUM");
			break;
		case mnuHilGolB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "63");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM HILANG/");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RUSAK SIM B1");
			break;
		case mnuHilGolB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "64");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM HILANG/");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RUSAK SIM B UMUM");
			break;
		case mnuHilGolB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "65");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM HILANG/");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RUSAK SIM B2");
			break;
		case mnuHilGolB2um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "66");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM HILANG/");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RUSAK SIM B2 UMUM");
			break;
		case mnuHilGolC:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "67");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM HILANG/");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RUSAK SIM C");
			break;
		case mnuHilGolD:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "68");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN SIM HILANG/");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RUSAK SIM D");
			break;
		case mnuHilPeningkatAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6B");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENINGKATAN SIM A UMUM");
			break;
		case mnuHilPeningkatB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6C");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENINGKATAN SIM B1");
			break;
		case mnuHilPeningkatB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6D");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENINGKATAN SIM B1 UMUM");
			break;
		case mnuHilPeningkatB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6E");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENINGKATAN SIM B2");
			break;
		case mnuHilPeningkatB2um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6F");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENINGKATAN SIM B2 UMUM");
			break;
		case mnuHilPenurunA:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6K");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENURUNAN SIM A");
			break;
		case mnuHilPenurunAum:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6L");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENURUNAN SIM A UMUM");
			break;
		case mnuHilPenurunB1:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6M");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENURUNAN SIM B1");
			break;
		case mnuHilPenurunB1um:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6N");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENURUNAN SIM B1 UMUM");
			break;
		case mnuHilPenurunB2:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6O");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "SIM HILANG/RUSAK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "PENURUNAN SIM B2");
			break;
		case mnuUjiSimulator:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "70");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN UJI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "SIMULATOR");
			break;
		case mnuRoda23Baru:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "81");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN BPKB BARU");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "KEND. RODA 2/3");
			break;
		case mnuRoda4orBaru:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "82");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN BPKB BARU");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "KEND. RODA 4 ATAU LEBIH");
			break;		
		case mnuRoda23Ganti:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "91");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN BPKB GANTI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "PEMILIK KEND. RODA 2/3");
			break;
		case mnuRoda4orGanti:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "92");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB, "BPKB GANTI PEMILIK");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB1,  "KEND. RODA 4 ATAU LEBIH");
			break;		
		case mnuSTCK:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "A0");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN STCK");
			break;
		case mnuRoda23Mutasi:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "B1");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RANMOR RODA 2/3");
			break;		
		case mnuRoda4orMutasi:
			strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "B2");
			strcpy(glProcInfo.stTranLog.szTransName, "PEMBAYARAN MUTASI");
			strcpy(glProcInfo.stTranLog.BRI.Ssb.BayarSSB,  "RANMOR RODA 4 ATAU LEBIH");
			break;		
	}

	ret = 1;
	return ret;
}


int TransSSB(void)
{
	int		iRet;
	uchar	ucRet;
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		bit48Req[23+1];
	char		kodeSatpas[4+1];
	char		uangFmt[20+1];
	char		Jumlah[1+1];

	char TampilBuff[22+1];

	mapGetWord(traMnuItm, mnuItm);

	switch(mnuItm)
		{	
				case mnuSIMBaruA:
				case mnuSIMBaruC:
				case mnuSIMBaruD:
//					strcpy(glProcInfo.stTranLog.szTransName,"SIM BARU");
					iRet = TransInit(TRANS_TYPE_SIM_BARU);
					break;
			
				case mnuSIMPerpanjangA:
				case mnuSIMPerpanjangAum:
				case mnuSIMPerpanjangB1:
				case mnuSIMPerpanjangB1um:
				case mnuSIMPerpanjangB2:
				case mnuSIMPerpanjangB2um:
				case mnuSIMPerpanjangC:
				case mnuSIMPerpanjangD:
//					strcpy(glProcInfo.stTranLog.szTransName,"SIM PERPANJANG");
					iRet = TransInit(TRANS_TYPE_SIM_PERPANJANGAN);
					break;
				
				case mnuSIMPeningkatAum:
				case mnuSIMPeningkatB1:
				case mnuSIMPeningkatB1um:
				case mnuSIMPeningkatB2:
				case mnuSIMPeningkatB2um:
//					strcpy(glProcInfo.stTranLog.szTransName,"SIM PENINGKATAN");
					iRet = TransInit(TRANS_TYPE_SIM_PENINGKATAN);
					break;
					
				case mnuSIMPenurunA:
				case mnuSIMPenurunAum:
				case mnuSIMPenurunB1:
				case mnuSIMPenurunB1um:
				case mnuSIMPenurunB2:
//					strcpy(glProcInfo.stTranLog.szTransName,"SIM PENURUNAN");
					iRet = TransInit(TRANS_TYPE_SIM_PENURUNAN);
					break;
			
				case mnuMutGolA:
				case mnuMutGolAum:
				case mnuMutGolB1:
				case mnuMutGolB1um:
				case mnuMutGolB2:				
				case mnuMutGolB2um:
				case mnuMutGolC:
				case mnuMutGolD:
//					strcpy(glProcInfo.stTranLog.szTransName,"SIM MUTASI");
					iRet = TransInit(TRANS_TYPE_SIM_MUTASI_GOL_SAMA);
					break;
		
				case mnuMutPeningkatAum:
				case mnuMutPeningkatB1:
				case mnuMutPeningkatB1um:
				case mnuMutPeningkatB2:
				case mnuMutPeningkatB2um:
//					strcpy(glProcInfo.stTranLog.szTransName,"MUTASI PENINGKATAN");
					iRet = TransInit(TRANS_TYPE_SIM_MUTASI_PENINGKATAN);
					break;
					
				case mnuMutPenurunA:
				case mnuMutPenurunAum:
				case mnuMutPenurunB1:
				case mnuMutPenurunB1um:
				case mnuMutPenurunB2:
//					strcpy(glProcInfo.stTranLog.szTransName,"MUTASI PENURUNAN");
					iRet = TransInit(TRANS_TYPE_SIM_MUTASI_PENURUNAN);
					break;

				case mnuHilGolA:
				case mnuHilGolAum:
				case mnuHilGolB1:
				case mnuHilGolB1um:
				case mnuHilGolB2:
				case mnuHilGolB2um:
				case mnuHilGolC:
				case mnuHilGolD:
//					strcpy(glProcInfo.stTranLog.szTransName,"SIM HILANG/RUSAK");
					iRet = TransInit(TRANS_TYPE_SIM_HILANG_RUSAK_GOLSAMA);
					break;
					
				case mnuHilPeningkatAum:
				case mnuHilPeningkatB1:
				case mnuHilPeningkatB1um:
				case mnuHilPeningkatB2:
				case mnuHilPeningkatB2um:
//					strcpy(glProcInfo.stTranLog.szTransName,"SIM HILANG/RUSAK");
					iRet = TransInit(TRANS_TYPE_SIM_HILANG_RUSAK_PENINGKATAN);
					break;
				
				case mnuHilPenurunA:
				case mnuHilPenurunAum:
				case mnuHilPenurunB1:
				case mnuHilPenurunB1um:
				case mnuHilPenurunB2:
//					strcpy(glProcInfo.stTranLog.szTransName,"SIM HILANG/RUSAK");
					iRet = TransInit(TRANS_TYPE_SIM_HILANG_RUSAK_PENURUNAN);
					break;	
					
			case mnuUjiSimulator:
//				strcpy(glProcInfo.stTranLog.szTransName,"UJI SIMULATOR");
				iRet = TransInit(TRANS_TYPE_UJI_SIMULATOR);
				break;	
				
			case mnuRoda23Baru:
			case mnuRoda4orBaru:
//				strcpy(glProcInfo.stTranLog.szTransName,"BPKB BARU KENDARAAN");
				iRet = TransInit(TRANS_TYPE_BPKB_BARU);
				break;	

			case mnuRoda23Ganti:
			case mnuRoda4orGanti:
//				strcpy(glProcInfo.stTranLog.szTransName,"BPKB GANTI PEMILIK");
				iRet = TransInit(TRANS_TYPE_BPKB_GANTI_PEMILIK);
				break;
					
			case mnuSTCK:
//				strcpy(glProcInfo.stTranLog.szTransName,"STCK");
				iRet = TransInit(TRANS_TYPE_STCK);
				break;
			
				case mnuRoda23Mutasi:
				case mnuRoda4orMutasi:
//					strcpy(glProcInfo.stTranLog.szTransName,"MUTASI RANMOR");
					iRet = TransInit(TRANS_TYPE_MUTASI_RANMOR);
					break;
		}

	
	if( iRet!=0 )
	{
		return iRet;
	}

	glProcInfo.stTranLog.ucOrgTranType = glProcInfo.stTranLog.ucTranType;
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
/*
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
*/
	switch(glProcInfo.stTranLog.ucTranType)
	{	
		case TRANS_TYPE_BPKB_BARU:
		case TRANS_TYPE_BPKB_GANTI_PEMILIK:
			ScrCls();
			DispTransName();
			PubDispString("Jumlah :", DISP_LINE_LEFT|2);
			memset(Jumlah, 0, sizeof(Jumlah));
			ucRet = PubGetString(NUM_IN, 1, 1 , Jumlah, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}
			glProcInfo.stTranLog.BRI.Ssb.Jumlah = atoi(Jumlah);
			break;
		
	}
	
	MEM_ZERO(bit48Req);
	memset(bit48Req, 0x20, sizeof(bit48Req)-1);

	memcpy(bit48Req, "000POLRI", 8);
/*	
	bit48Req[0] = '0';
	bit48Req[1] = '0';
	bit48Req[2] = '0';
	bit48Req[3] = 'P';
	bit48Req[4] = 'O';
	bit48Req[5] = 'L';
	bit48Req[6] = 'R';
	bit48Req[7] = 'I';
*/

	ScrCls();
	DispTransName();
	PubDispString("KODE SATPAS :", DISP_LINE_LEFT|4);
	MEM_ZERO(kodeSatpas);
	ucRet = PubGetString(NUM_IN, 1, 4 , kodeSatpas, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	strcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSatpasSSB, kodeSatpas);
		
	memcpy(bit48Req+8, kodeSatpas, strlen(kodeSatpas));
//	hex_dump_char("bit48Req", bit48Req, 23);
	memcpy(glProcInfo.szReq48, bit48Req, strlen(bit48Req));
//	hex_dump_char("glProcInfo.szReq48", glProcInfo.szReq48, 23);
#if 1
	selectTrxSSB(mnuItm);
#else
	switch(mnuItm)
		{	
			case mnuSIMBaruA:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "11", 2);
				break;
			case mnuSIMBaruC:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "17", 2);
				break;
			case mnuSIMBaruD:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "18", 2);
				break;
			case mnuSIMPerpanjangA:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "21", 2);
				break;
			case mnuSIMPerpanjangAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "22", 2);
				break;
			case mnuSIMPerpanjangB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "23", 2);
				break;
			case mnuSIMPerpanjangB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "24", 2);
				break;
			case mnuSIMPerpanjangB2:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "25", 2);
				break;
			case mnuSIMPerpanjangB2um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "26", 2);
				break;
			case mnuSIMPerpanjangC:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "27", 2);
				break;
			case mnuSIMPerpanjangD:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "28", 2);
				break;
			case mnuSIMPeningkatAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "32", 2);
				break;
			case mnuSIMPeningkatB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "33", 2);
				break;
			case mnuSIMPeningkatB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "34", 2);
				break;
			case mnuSIMPeningkatB2:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "35", 2);
				break;
			case mnuSIMPeningkatB2um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "36", 2);
				break;
			case mnuSIMPenurunA:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "41", 2);
				break;
			case mnuSIMPenurunAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "42", 2);
				break;
			case mnuSIMPenurunB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "43", 2);
				break;
			case mnuSIMPenurunB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "44", 2);
				break;
			case mnuSIMPenurunB2:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "45", 2);
				break;
			case mnuMutGolA:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "51", 2);
				break;
			case mnuMutGolAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "52", 2);
				break;
			case mnuMutGolB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "53", 2);
				break;
			case mnuMutGolB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "54", 2);
				break;
			case mnuMutGolB2:		
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "55", 2);
				break;
			case mnuMutGolB2um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "56", 2);
				break;
			case mnuMutGolC:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "57", 2);
				break;
			case mnuMutGolD:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "58", 2);
				break;
			case mnuMutPeningkatAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5B", 2);
				break;
			case mnuMutPeningkatB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5C", 2);
				break;
			case mnuMutPeningkatB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5D", 2);
				break;
			case mnuMutPeningkatB2:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5E", 2);
				break;
			case mnuMutPeningkatB2um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5F", 2);
				break;
			case mnuMutPenurunA:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5K", 2);
				break;
			case mnuMutPenurunAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5L", 2);
				break;
			case mnuMutPenurunB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5M", 2);
				break;
			case mnuMutPenurunB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5N", 2);
				break;
			case mnuMutPenurunB2:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "5O", 2);
				break;
			case mnuHilGolA:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "61", 2);
				break;
			case mnuHilGolAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "62", 2);
				break;
			case mnuHilGolB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "63", 2);
				break;
			case mnuHilGolB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "64", 2);
				break;
			case mnuHilGolB2:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "65", 2);
				break;
			case mnuHilGolB2um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "66", 2);
				break;
			case mnuHilGolC:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "67", 2);
				break;
			case mnuHilGolD:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "68", 2);
				break;
			case mnuHilPeningkatAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6B", 2);
				break;
			case mnuHilPeningkatB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6C", 2);
				break;
			case mnuHilPeningkatB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6D", 2);
				break;
			case mnuHilPeningkatB2:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6E", 2);
				break;
			case mnuHilPeningkatB2um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6F", 2);
				break;
			case mnuHilPenurunA:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6K", 2);
				break;
			case mnuHilPenurunAum:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6L", 2);
				break;
			case mnuHilPenurunB1:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6M", 2);
				break;
			case mnuHilPenurunB1um:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6N", 2);
				break;
			case mnuHilPenurunB2:		
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "6O", 2);
				break;
			case mnuUjiSimulator:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "70", 2);
				break;
			case mnuRoda23Baru:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "81", 2);
				break;
			case mnuRoda4orBaru:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "82", 2);
				break;
			case mnuRoda23Ganti:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "91", 2);
				break;
			case mnuRoda4orGanti:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "92", 2);
				break;
			case mnuSTCK:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "A0", 2);
				break;
			case mnuRoda23Mutasi:
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "A1", 2);
				break;
			case mnuRoda4orMutasi:	
				memcpy(glProcInfo.stTranLog.BRI.Ssb.KodeSSB, "A2", 2);
				break;
		}
#endif

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackSSB();
	
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

		displayResponse();
		
		return ERR_NO_DISP;
	}

	unPackSSB();

	ScrCls();
	PubDispStringASCII("PEMBAYARAN SSB", DISP_LINE_CENTER|0);
	
	sprintf(TampilBuff, "SATPAS : %s", glProcInfo.stTranLog.BRI.Ssb.KodeSatpasSSB);
	PubDispStringASCII(TampilBuff, DISP_LINE_LEFT|2);

	sprintf(TampilBuff, "REK: %s", glProcInfo.stTranLog.BRI.Ssb.RekSatpasSSB);
	PubDispStringASCII(TampilBuff, DISP_LINE_LEFT|3);

	sprintf(TampilBuff, "NAMA : %s", glProcInfo.stTranLog.BRI.Ssb.NamaSatpasSSB);
	PubDispStringASCII(TampilBuff, DISP_LINE_LEFT|4);

	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, uangFmt, GA_SEPARATOR);
	PubDispStringASCII("TAGIHAN :", DISP_LINE_LEFT|5);
	PubDispStringASCII(uangFmt, DISP_LINE_RIGHT|6);

	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);
	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	iRet = TransInit(TRANS_TYPE_SSB);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackSSB();
	
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

		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStruk(PRN_NORMAL);
	}
		
	return 0;
}
#endif

int TransLogonAJ(void)
{
	int iRet;
	word idx;
//	byte logonFlag;
	char AcqName[lenName + 1];
	uchar bufTWK[999];
	uchar ucEntryMode=0;
//	uchar acqId;
	uchar datetime[14+1];

	iRet = TransInit(TRANS_TYPE_LOGON);
	if( iRet!=0 )
	{
		return iRet;
	}
/*
	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		mapMove(acqBeg, idx);
		mapGetByte(acqID, acqId);
		if(acqId==glCurTlmTmsAcq.acqID)
			break;
	}
*/

	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		MEM_ZERO(AcqName);
		mapMove(acqBeg, idx);
		memcpy(AcqName, glTlmSysParam.Acq[idx].acqName, lenName);
		if (memcmp(AcqName, "MINIATM",10)==0) 
		break;
	}
	
	FindAcq(glTlmSysParam.Acq[idx].acqID);

	//if(glCurTlmTmsIss.issPinOpt==0x30)
	//	return 0;

	//mapGetByte(acqLogonFlag, logonFlag);

	setPackLogonAJ();

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
	

	if( memcmp(glRecvPack.szBit39, "00", 2)!=0 )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glRecvPack.szBit39);
		displayResponse();
		return ERR_NO_DISP;
	}

	mapPutByte(acqLogonFlag, 1);

	MEM_ZERO(bufTWK);
	memcpy(bufTWK, glRecvPack.szBit62+2, PubChar2Long(glRecvPack.szBit62, 2, NULL));

	mapPut(acqWorkingKey, bufTWK, 16);
	PubAsc2Bcd(bufTWK, 16, glSysCtrl.WorkingKey);

	GetDateTime(datetime);
	memcpy(glSysCtrl.stTlmApp.appStartTimerWorkingKey, datetime, 14);
		
/*
	iRet = GetPinKeyPerAcq();
	if(iRet<0)
	{
		return ERR_NO_DISP;
	}
//*/
	displayResponse();
	SaveSysCtrlBase();
	return 0;
	
}

int TransLogonBukopin(void)
{
	int iRet;
	//word idx;
	//char AcqName[lenName + 1];
	uchar bufTWK[999];
	uchar ucEntryMode=0;
	uchar datetime[14+1];
	uchar	ucAcqIndex, szTitle[16+1];

	iRet = TransInit(TRANS_TYPE_LOGON_BUKOPIN);
	if( iRet!=0 )
	{
		return iRet;
	}
#ifdef	__TELIUM_PARAM__
	sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#else
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#endif
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return iRet;
	}

	setPackLogonBukopin();

	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		return iRet;
	}
	

	if( memcmp(glRecvPack.szBit39, "00", 2)!=0 )
	{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glRecvPack.szBit39);
		displayResponse();
		return ERR_NO_DISP;
	}

	mapPutByte(acqLogonFlag, 1);

	MEM_ZERO(bufTWK);
	memcpy(bufTWK, glRecvPack.szBit61+2, PubChar2Long(glRecvPack.szBit61, 2, NULL));

	mapPut(acqWorkingKey, bufTWK, 32);
	//PubAsc2Bcd(bufTWK, 32, glSysCtrl.WorkingKey);
	PubAsc2Bcd(bufTWK, 32, glTlmSysParam.Acq[ucAcqIndex].acqWorkingKey);

	GetDateTime(datetime);
	memcpy(glSysCtrl.stTlmApp.appStartTimerWorkingKey, datetime, 14);
	//*
	#ifdef ENABLE_EMV	
		if(debugEMVCalc)
		{	
			hex_dump_char("acqName_______Logon", glTlmSysParam.Acq[ucAcqIndex].acqName,lenName);
			hex_dump_char("Bit61_________Logon", glRecvPack.szBit61+2,  32);
			hex_dump_char("acqWorkingKey_Logon", glTlmSysParam.Acq[ucAcqIndex].acqWorkingKey,16);
		}
	#endif
	//*/

	displayResponse();
	SaveTlmSysParam();
	//SaveSysCtrlBase();
	return 0;
}
int TransLogonAllAcqBukopin(void)
{
	int iRet;
	word idx;
	uchar bufTWK[999];
	uchar datetime[14+1];


	for(idx=0;idx<glTlmSysParam.ucAcqNum;idx++)
	{
		SetCurAcq((uchar)idx);
		glProcInfo.stTranLog.ucTranType = TRANS_TYPE_LOGON_BUKOPIN;

		ScrCls();
		DispTransName();

		ScrPrint(0, 0, CFONT|REVER, "LOGON %10s", glCurTlmTmsAcq.acqName);

		mapPutByte(acqLogonFlag, 0);

		setPackLogonBukopin();
		iRet = SendRecvPacket();
		if(iRet != 0)
		{
			return ERR_NO_DISP;
		}

		if( memcmp(glRecvPack.szBit39, "00", 2)!=0 )
		{
		sprintf((char *)glProcInfo.stTranLog.szRspCode, "%.2s", glRecvPack.szBit39);
		displayResponse();
		return ERR_NO_DISP;
		}

		mapPutByte(acqLogonFlag, 1);

		MEM_ZERO(bufTWK);
		memcpy(bufTWK, glRecvPack.szBit61+2, PubChar2Long(glRecvPack.szBit61, 2, NULL));

		mapPut(acqWorkingKey, bufTWK, 32);
		PubAsc2Bcd(bufTWK, 32, glSysCtrl.WorkingKey);
		memcpy(glCurTlmTmsAcq.acqWorkingKey,glSysCtrl.WorkingKey,16);
		
		//hex_dump_char("bufTWK",bufTWK,32);
		//hex_dump_char("glCurTlmTmsAcq.acqWorkingKey",glCurTlmTmsAcq.acqWorkingKey,16);

		GetDateTime(datetime);
		memcpy(glSysCtrl.stTlmApp.appStartTimerWorkingKey, datetime, 14);

		iRet = GetPinKeyPerAcq();
		if(iRet<0)
		{
			return ERR_NO_DISP;
		}
	}
	return 0;
}



// ************* DIKI ADD FOR MINI ATM BUKOPIN **************


int TransTrfAntarBankBkpn(void)
{
	int		ret;
	int		iRet;
	uchar 	ucRet;
	char 	bit48Rsp[lenBit48+1];
	char 	nama[30+1];
	ulong 	lenData;
	uchar	ucEntryMode=0;

	char 	bit48Req[148+1];
	char		KodeBank[148+1];
	char 	noRek[28+1];
	char		noRef[16+1];

	char 	bankAsal[20+1];
	char 	bankTujuan[20+1];
	char 	uang[12+1];
	char		uangFmt[20+1];

	char 	norekAsal[28+1];

	char screenUang[21+1];
	char screenBankAsal[21+1];
	char screenBankTujuan[21+1];
	char screenNoRek[21+1];
	char screenNoRef[21+1];

	
	char 	bit103Req[28+1];
	char 	bit102Rsp[28+1];
	char 	bit103Rsp[28+1];

	char 	bankTujuanFinal[28+1];

	char	ListKodeBank[20+1];
	char	ListNamaBank[20+1];

	uchar	ucKey;

	byte jenisRekening;
	//mapGetByte(traSumberRek, jenisRekening);

	MEM_ZERO(screenUang);
	MEM_ZERO(screenBankAsal);
	MEM_ZERO(screenBankTujuan);
	MEM_ZERO(screenNoRek);
	MEM_ZERO(screenNoRef);

	MEM_ZERO(bankTujuanFinal);

	MEM_ZERO(ListKodeBank);
	MEM_ZERO(ListNamaBank);

	strcpy(glProcInfo.stTranLog.szTransName,"TRANSFER");
			
	iRet = TransInit(TRANS_TYPE_INQ_TRFANTARBANK_BKPN);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
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
	
	////// diki add komen   RUN_FUNCTION(PilihJenisRekening, lblKO);
	RUN_FUNCTION(PilihJenisRekeningBkpn, lblKO);

	MEM_ZERO(bit48Req);
	MEM_ZERO(KodeBank);
	MEM_ZERO(noRek);
	MEM_ZERO(noRef);

	MEM_ZERO(bit103Req);
	
	mapGetByte(traSumberRek, jenisRekening);
	switch(jenisRekening)
		{
		case 1:
			ScrCls();
			iRet = GetCard(ucEntryMode);
				if( iRet!=0 )
				{
					return iRet;
				}
	
			ScrCls();
			DispTransName();
			PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
			PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}

			////memcpy(bit48Req+3,noRek,strlen(noRek));
			//memcpy(bit103Req,noRek,strlen(noRek));
			
			memcpy(bankTujuanFinal,"00",2);
			strcat(bankTujuanFinal,noRek);

			memcpy(bit103Req,bankTujuanFinal,strlen(bankTujuanFinal));


			
			break;
		case 2:

			ScrCls();
			iRet = GetCard(ucEntryMode);
				if( iRet!=0 )
				{
					return iRet;
				}
	
			

		while(1)
		{
			ScrCls();
			//DispTransName();
			PubDispString("LIHAT DAFTAR:", DISP_LINE_LEFT|0);
			PubDispString("KODE BANK ?", DISP_LINE_LEFT|2);
			PubDispString("NO       YES", DISP_LINE_LEFT|6);
			
		ucKey = PubWaitKey(30);
			if ((ucKey==KEYENTER) || (ucKey==KEYUP) || (ucKey==KEYDOWN))
			{
			//TransInfoKodeBankBkpn();
			TransInfoKodeBankBkpn2();
			//continue;
			}
			else 
				{
				break;
				}
		}


			ScrCls();
			DispTransName();
			PubDispString("MASUKKAN", DISP_LINE_LEFT|2);
			PubDispString("KODE BANK :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 3,10 , KodeBank, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}

			fmtPad(KodeBank, 20, ' ');

			memcpy(bit48Req,KodeBank,strlen(KodeBank));
			//memcpy(bit48Req+3,"                                                                                                                                                ",144);
			//memcpy(bit103Req,KodeBank,strlen(KodeBank));

			ScrCls();
			DispTransName();
			PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
			PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}

			////memcpy(bit48Req+3,noRek,strlen(noRek));
			///memcpy(bit103Req+3,noRek,strlen(noRek));
			memcpy(bit103Req,noRek,strlen(noRek));
			break;
		}


	/*
	ScrCls();
	DispTransName();
	PubDispString("KODE BANK :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 3,3 , KodeBank, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	///memcpy(bit48Req,KodeBank,strlen(KodeBank));
	memcpy(bit103Req,KodeBank,strlen(KodeBank));

	ScrCls();
	DispTransName();
	PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
	PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	////memcpy(bit48Req+3,noRek,strlen(noRek));
	memcpy(bit103Req+3,noRek,strlen(noRek));
	*/

	iRet = InputUangBkpn();
	if( iRet!=0 )
	{
		return iRet;
	}

	/*
	ScrCls();
	DispTransName();
	PubDispString("CUST REF NUMBER", DISP_LINE_LEFT|2);
	PubDispString("ATAU LANJUT  :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 0, 16, noRef, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req+23,noRef,strlen(noRef));
	*/
	strcpy(glProcInfo.szReq48,bit48Req);
	strcpy(glProcInfo.szReq103,bit103Req);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	////// diki add komen  setPackInqTransferAntarBank();
	setPackInqTransferAntarBankBkpn();

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

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(bit102Rsp);
	MEM_ZERO(bit103Rsp);
	//bit102Rsp

	// Overbook / Fund Transfer BIT 48 Respone
	// Destination bank 	AN 20 	left, pad blank
	// Destination branch	AN 20 	left, pad blank
	// Transfer media 		N 1 		1:llg, 2:rtgs, 3:fund transfer
	// Destination name	AN 30 	left, pad blank
	// Current balance		N-16, 2 	right, pad zerro
	// Transfer messsage	AN 60 	left, pad blank
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	mapPutStr(traResponseInq,bit48Rsp);

	PubChar2Long(glRecvPack.szField102, 2, &lenData);
	memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);

	PubChar2Long(glRecvPack.szField103, 2, &lenData);
	memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);

	MEM_ZERO(nama);
	MEM_ZERO(bankAsal);
	MEM_ZERO(bankTujuan);
	MEM_ZERO(norekAsal);

	memcpy(ListKodeBank,bit48Rsp,3);
	TransInfoKodeBankBkpnPilih(ListNamaBank, ListKodeBank);
	memcpy(bankTujuan,ListNamaBank,strlen(ListNamaBank));
	
	
	////memcpy(bankTujuan,bit48Rsp+20,20);
	//memcpy(bankTujuan,bit48Rsp+45,15);
	//memcpy(nama,bit48Rsp+40,20);

	//memcpy(norekAsal,bit102Rsp,28);
	//memcpy(noRek,bit103Rsp,28);
	switch(jenisRekening)
		{
		case 1:
			memcpy(norekAsal,bit102Rsp+2,26);
			memcpy(noRek,bit103Rsp+2,26);
			memcpy(nama,bit48Rsp+40,20);
			break;
		case 2:
			memcpy(norekAsal,bit102Rsp,28);
			memcpy(noRek,bit103Rsp,28);
			memcpy(nama,bit48Rsp+41,20);
			break;
		}
	

	MEM_ZERO(uang);
	MEM_ZERO(uangFmt);
	memcpy(uang,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(uangFmt,uang,0,",.");
	sprintf(screenUang,"Rp. %s",uangFmt);

	ScrCls();
	PubDispStringASCII("TRANSFER BANK", DISP_LINE_CENTER|0);
	///PubDispStringASCII("TRANSFER ANTAR BANK", DISP_LINE_CENTER|0);
	///PubDispStringASCII("Bank Asal :", DISP_LINE_LEFT|1);
	///PubDispStringASCII(bankAsal, DISP_LINE_LEFT|2);
	PubDispStringASCII("No.Rek Asal :", DISP_LINE_LEFT|1);
	PubDispStringASCII(norekAsal, DISP_LINE_LEFT|2);
	PubDispStringASCII("Bank Tujuan :", DISP_LINE_LEFT|3);
	PubDispStringASCII(bankTujuan, DISP_LINE_LEFT|4);
	PubDispStringASCII("Rek Tujuan  :", DISP_LINE_LEFT|5);
	PubDispStringASCII(noRek, DISP_LINE_LEFT|6);
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|0);
	PubDispStringASCII(nama, DISP_LINE_LEFT|1);
	///PubDispStringASCII("Cust Ref No :", DISP_LINE_LEFT|2);
	///PubDispStringASCII(noRef, DISP_LINE_LEFT|3);
	PubDispStringASCII("Jumlah :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenUang, DISP_LINE_RIGHT|5);
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;
	
	//iRet = TransInit(TRANS_TYPE_INQ_TRFANTARBANK_BKPN);
	// TRANS_TYPE_TRFANTARBANK_BKPN
	iRet = TransInit(TRANS_TYPE_TRFANTARBANK_BKPN);
	if( iRet!=0 )
	{
		return iRet;
	}



	strcpy(glProcInfo.szReq48,bit48Rsp);
	//strcpy(glProcInfo.szReq102,bit102Rsp);
	strcpy(glProcInfo.szReq103,bit103Rsp);


	///// UPDATE MINI ATM BUKOPIN FOR TRANSFER ANTAR BANK!!!  UPDATE!
	switch(jenisRekening)
			{
			case 1:
				strcpy(glProcInfo.szReq102,bit102Rsp);
			
				break;
			case 2:
				strcpy(glProcInfo.szReq102,"10");
				break;
			}
	


	
	////// diki add komen  setPackTransferAntarBank();
	setPackTransferAntarBankBkpn();

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

		return ERR_NO_DISP;
	}

	////// diki add komen unPackTransfer_AntarBank();
	unPackTransfer_AntarBankBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}


	/*
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}
	*/

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    CommOnHook(FALSE);


	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}


	return 0;

	lblKO:
		return FALSE;
}




int InputUangBkpn(void)
{
	uchar	ucRet;
	ScrCls();
	DispTransName();
	PubDispString("JUMLAH :", DISP_LINE_LEFT|4);
	ucRet = PubGetAmount(glTlmSysParam.Con.appCurrSign, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
						0, 9, glProcInfo.stTranLog.szAmount, USER_OPER_TIMEOUT, GA_SEPARATOR);

	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
	//hex_dump("ttttttttttttt",glProcInfo.stTranLog.szAmount, 15);
	
	return 0;
}


int CetakStrukBkpn(uchar ucPrnFlag)
{
	word 	mnuItm;
	int i =0;
	int i2 =0;
	int i3 =1;
	mapGetWord(traMnuItm, mnuItm);
	
	if(glCurTlmTmsIss.issPrintReceipt!=TRUE)
	{
		return 0;
	}

	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SSB:
			PrnReceiptSSB(ucPrnFlag);
			return 0;
			break;
	}

	DispPrinting();
	if( !ChkIfThermalPrinter() )
	{
		return 0;
	}

for (i=0; i<=1; i++)

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
/*
		switch(mnuItm)
			{
			case mnuRepLastTransMini:
			case mnuRepAnyTransMini:
				PrnDuplicateHeader();
				PrnStr("\n");
				break;
			}
*/		
		//TID MID
		PrnHeaderTidMid();

		PrnHeaderDateTime();
		
		//PrnHeaderCardNumber();
		PrnHeaderCardNumberBkpn();

		PrnHeaderTraceApprCode();

		PrnHeaderBatchRefNum();

		PrnStr("\n");
		if(glProcInfo.stTranLog.ucTranType == TRANS_TYPE_TRFANTARBANK_BKPN)
			{
				// gak dicetak 
			}
		else {
		PrnHeaderTransactionName();
		}

		switch(glProcInfo.stTranLog.ucTranType)
		{
			case TRANS_TYPE_PRIVILAGE:
				PrnPrivilage();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_AKTIFASIKARTU_PRINT:
				PrnAktivasiKartu();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_REISSUE_PIN_NASABAH:
				PrnReissuePin();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_RENEWALKARTU_BARU:
				PrnRenewalKartu();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_UBAHPIN_CS:
				PrnUbahPinCS();
				break;
			case TRANS_TYPE_UBAHPWD_SPV:
				PrnUbahPinSPV();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SMSBANKING:
				PrnRegistrasiSMSBanking();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_PHONEBANKING:
				PrnRegistrasiPhoneBanking();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_IBANKING:
				PrnRegistrasiIBanking();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_TRX_PB_TRANSFER:
				PrnRegistrasiTRXPBTransfer();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_TRX_PB_NO_HP:
				PrnRegistrasiTRXPBNoHP();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_INFOSALDO:
				PrnInfoSaldoBRI();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_INFOSALDO_BANKLAIN:
				PrnInfoSaldoBankLain();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_MINISTATEMENT:
				PrnMiniStatement();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_MUTASIREKENING:
				PrnMutasiRekening();
				PrnFooterTerimaKasih();
				break;
			case TRANS_TYPE_TRFSESAMABRI:
				PrnTrfSesamaBRI();
				PrnFooterTrfSesamaBRI();
				break;
			case TRANS_TYPE_TRFANTARBANK:
				PrnTrfAntarBank();
				PrnFooterTrfSesamaBRI();
				break;
			case TRANS_TYPE_TELKOM:
				PrnPembTelkom();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_KARTUHALO:
				PrnPascaBayarHalo();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_KARTUMATRIX:
				PrnPascaBayarMatrix();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_PLNPASCABAYAR:
				PrnPLNPascaBayar();
				break;
			case TRANS_TYPE_PLNPRABAYAR:
			case TRANS_TYPE_PLNTOKEN:
				PrnPLNPraBayar();
				break;
			//case TRANS_TYPE_PLNTOKEN:

			//	break;
			case TRANS_TYPE_KKBRI:
				PrnKK_BRI();
				PrnFooterKK_KTA();
				break;
			case TRANS_TYPE_KKCITIBANK:
			case TRANS_TYPE_KTACITIBANK:
			case TRANS_TYPE_KKSCB:
			case TRANS_TYPE_KTASCB:
			case TRANS_TYPE_KKANZ:
			case TRANS_TYPE_KKHSBC:
			case TRANS_TYPE_KTAHSBC:
			case TRANS_TYPE_KKRBS:
			case TRANS_TYPE_KTARBS:
	case TRANS_TYPE_KK_ANZ_BKPN:
	case TRANS_TYPE_KK_BUKOPIN_BKPN:
	case TRANS_TYPE_KK_BNI_BKPN:
	case TRANS_TYPE_KK_HSBC_BKPN:
				PrnKK_KTA();
				PrnFooterKK_KTA();
				break;
			case TRANS_TYPE_FIF:
			case TRANS_TYPE_BAF:
			case TRANS_TYPE_OTO:
			case TRANS_TYPE_FINANSIA:
			case TRANS_TYPE_VERENA:
			case TRANS_TYPE_WOM:
	case TRANS_TYPE_INQ_MAF_BKPN:
	case TRANS_TYPE_MAF_BKPN:
	//case TRANS_TYPE_INQ_BAF_BKPN:
	//case TRANS_TYPE_BAF_BKPN:
	case TRANS_TYPE_INQ_MCF_BKPN:
	case TRANS_TYPE_MCF_BKPN:
	//case TRANS_TYPE_INQ_WOM_BKPN:
	//case TRANS_TYPE_WOM_BKPN:
				PrnCicilan();
				PrnFooterInfoSaldo();
				break;
	case TRANS_TYPE_INQ_BAF_BKPN:
	case TRANS_TYPE_BAF_BKPN:
				PrnCicilan();
				PrnFooterInfoBafBkpn();
				break;
	case TRANS_TYPE_INQ_WOM_BKPN:
	case TRANS_TYPE_WOM_BKPN:
				PrnCicilan();
				PrnFooterInfoWomBkpn();
				break;
			case TRANS_TYPE_ZAKAT:
			case TRANS_TYPE_INFAQ:
				PrnPembZakat();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_DPLK:
				PrnPembDPLK();
				PrnFooterInfoSaldo();
				break;	
			case TRANS_TYPE_GARUDA:
			case TRANS_TYPE_LIONAIR:
			case TRANS_TYPE_SRIWIJAYA:
			case TRANS_TYPE_MERPATI:
				PrnTiketPesawat();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_BRIVA:
				PrnBriva();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_BAYARKULIAH:
				PrnPendidikan();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_AXIS:
			case TRANS_TYPE_ESIA:
		//	case TRANS_TYPE_FREN_MOBI_HEPI: //ditutup 6 Januari 2014
			case TRANS_TYPE_MENTARI:
			case TRANS_TYPE_THREE:
			case TRANS_TYPE_XL:
			//	PrnIsiUlang();
			//	PrnFooterInfoSaldo();
			//	break;
			case TRANS_TYPE_SMARTFREN:
			//	PrnIsiUlangSmartFren();   
			//	PrnFooterInfoSaldo();
			//	break;
			case TRANS_TYPE_IM3:
			//	PrnIsiUlangIM3();
			//	PrnFooterInfoSaldo();
			//	break;
			case TRANS_TYPE_SIMPATI_AS:
			//	PrnIsiUlangSimpati();
			//	PrnFooterInfoSaldo();
				PrnIsiUlang();
				break;
			case TRANS_TYPE_TARIK_TUNAI:
			case TRANS_TYPE_VOID_TARIK_TUNAI:
				PrnTarikTunai();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SETOR_PINJAMAN:
			case TRANS_TYPE_SETOR_SIMPANAN:
				PrnSetorTunai();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SETORPASTI:
				PrnSetorPasti();
				PrnFooterInfoSaldo();
				break;
			case TRANS_TYPE_SSB:
				PrnSSB();
				PrnFooterInfoSaldo();
				break;
//			case TRANS_TYPE_CASH_IN:
//				PrnCasIn();
//				PrnFooterInfoSaldo();
//				break;
//			case TRANS_TYPE_CASHOUT:
//				PrnCasOut();
//				PrnFooterInfoSaldo();
//				break;


		// diki add for mini atm bukopin
		///case TRANS_TYPE_TRFSESAMABRI:
		///		PrnTrfSesamaBRI();
		///		PrnFooterTrfSesamaBRI();
		//		break;
		case TRANS_TYPE_TRFANTARBANK_BKPN:
				PrnTrfAntarBankBkpn();
				PrnFooterTrfSesamaBkpn();
				break;

		case TRANS_TYPE_PDAM_PALYJA_BKPN:
				PrnPdamPalyjaBkpn();
				PrnFooterInfoSaldo();
				break;

		case TRANS_TYPE_PDAM_AETRA_BKPN:
				PrnPdamAetraBkpn();
				PrnFooterInfoSaldo();
				break;

		case TRANS_TYPE_TARIK_TUNAI_BKPN:
				PrnTarikTunaiBkpn();
				PrnFooterInfoSaldo();
				break;

		case TRANS_TYPE_SETOR_TUNAI_BKPN:
				PrnSetorTunaiBkpn();
				PrnFooterInfoSaldo();
				break;

		case TRANS_TYPE_UBAH_PIN_BKPN:
				PrnGantiPinBkpn();
				//PrnFooterInfoSaldo();
				break;

				
				
		}

		if(i2==i)
		{
			PrnStr("\n");
			PrnStr("       --------- MERCHANT COPY ---------       ");		
		} else if(i3==i)
		{
			PrnStr("\n");
			PrnStr("       --------- CUSTOMER COPY ---------       ");		
		}


		if(ucPrnFlag)
		{
				PrnStr("\n");
				if(ucPrnFlag)
		{
				PrnStr("\n");
				PrnStr("          **********REPRINT**********          ");		
		}		
		}
/*
		switch(mnuItm)
			{
			case mnuRepLastTransMini:
			case mnuRepAnyTransMini:
				PrnStr("\n");
				PrnStr("          **********REPRINT**********          ");
				break;
			}
*/		
		PrnStr("\n\n\n\n\n\n\n\n");
		StartPrinter();

	}

	return 0;
}	


void MiniAtmPrnLastTransBkpn(void)
{
	int			iRet;
	ushort		RecNo=0;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	RecNo = glSysCtrl.uiMiniAtmLastRecNo;

	iRet = LoadMiniAtmCSTranLog(&glProcInfo.stTranLog, RecNo);
	if( iRet!=0 )
	{
		return;
	}
		
	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	CetakStrukBkpn(PRN_REPRINT);
	
}


void MiniAtmRePrnSpecTransBkpn(void)
{
	int			iRet;

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	if( GetMiniAtmTranLogNum(ACQ_ALL)==0 )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	InitTransInfo();
	iRet = GetMiniAtmRecord(TS_ALL_LOG, &glProcInfo.stTranLog);
	if( iRet!=0 )
	{
		return;
	}

	PubShowTitle(TRUE, (uchar *)_T("REPRINT         "));
	FindAcq(glProcInfo.stTranLog.ucAcqKey);
	FindIssuer(glProcInfo.stTranLog.ucIssuerKey);

	CetakStrukBkpn(PRN_REPRINT);
}


int TransInfoKodeBankBkpn(void)
{
	uchar 	ucRet;

lbl_1:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "002 B R I");
	ScrPrint(0, 1, ASCII, "%s", "008 MANDIRI");
	ScrPrint(0, 2, ASCII, "%s", "009 B N I");
	ScrPrint(0, 3, ASCII, "%s", "011 DANAMON");
	ScrPrint(0, 4, ASCII, "%s", "013 PERMATA");
	ScrPrint(0, 5, ASCII, "%s", "014 B C A");
	PubDispStringASCII(" v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet == KEYUP )
		goto lbl_1;

lbl_2:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "016 B I I");
	ScrPrint(0, 1, ASCII, "%s", "019 PANIN");
	ScrPrint(0, 2, ASCII, "%s", "022 CIMB NIAGA");
	ScrPrint(0, 3, ASCII, "%s", "023 BANK UOB");
	ScrPrint(0, 4, ASCII, "%s", "028 OCBC NISC");
	ScrPrint(0, 5, ASCII, "%s", "031 CITI BANK");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_1;

lbl_3:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "036 WINDU");
	ScrPrint(0, 1, ASCII, "%s", "037 ARTA GRAHA");
	ScrPrint(0, 2, ASCII, "%s", "041 H S B C");
	ScrPrint(0, 3, ASCII, "%s", "042 TOKYO");
	ScrPrint(0, 4, ASCII, "%s", "046 BANK DBS");
	ScrPrint(0, 5, ASCII, "%s", "050 STANCHRT");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_2;

lbl_4:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "052 R B S");
	ScrPrint(0, 1, ASCII, "%s", "054 CAPITAL");
	ScrPrint(0, 2, ASCII, "%s", "061 ANZ INDONESIA");
	ScrPrint(0, 3, ASCII, "%s", "068 WORI INDONESIA");
	ScrPrint(0, 4, ASCII, "%s", "076 BUMI ARTHA");
	ScrPrint(0, 5, ASCII, "%s", "087 EKONOMI");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_3;

lbl_5:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "088 ANTAR DAERAH");
	ScrPrint(0, 1, ASCII, "%s", "089 RABO BANK");
	ScrPrint(0, 2, ASCII, "%s", "095 MUTIARA");
	ScrPrint(0, 3, ASCII, "%s", "097 MAYAPADA");
	ScrPrint(0, 4, ASCII, "%s", "110 B J B");
	ScrPrint(0, 5, ASCII, "%s", "111 D K I");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_4;

lbl_6:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "112 D I Y");
	ScrPrint(0, 1, ASCII, "%s", "113 JATENG");
	ScrPrint(0, 2, ASCII, "%s", "114 JATIM");
	ScrPrint(0, 3, ASCII, "%s", "115 JAMBI");
	ScrPrint(0, 4, ASCII, "%s", "116 ACEH");
	ScrPrint(0, 5, ASCII, "%s", "117 SUMUT");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_5;

lbl_7:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "118 NAGARI");
	ScrPrint(0, 1, ASCII, "%s", "119 RIAU");
	ScrPrint(0, 2, ASCII, "%s", "120 SUMSEL");
	ScrPrint(0, 3, ASCII, "%s", "121 LAMPUNG");
	ScrPrint(0, 4, ASCII, "%s", "122 KALSEL");
	ScrPrint(0, 5, ASCII, "%s", "123 KALBAR");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_6;

lbl_8:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "124 KALTIM");
	ScrPrint(0, 1, ASCII, "%s", "125 KALTENG");
	ScrPrint(0, 2, ASCII, "%s", "126 SULSEL");
	ScrPrint(0, 3, ASCII, "%s", "127 SULUT");
	ScrPrint(0, 4, ASCII, "%s", "128 N T B");
	ScrPrint(0, 5, ASCII, "%s", "129 BALI");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_7;

lbl_9:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "130 N T T");
	ScrPrint(0, 1, ASCII, "%s", "131 MALUKU");
	ScrPrint(0, 2, ASCII, "%s", "132 PAPUA");
	ScrPrint(0, 3, ASCII, "%s", "133 BENGKULU");
	ScrPrint(0, 4, ASCII, "%s", "134 SULTENG");
	ScrPrint(0, 5, ASCII, "%s", "135 SULTRA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_8;

lbl_10:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "145 BNP");
	ScrPrint(0, 1, ASCII, "%s", "146 SWADESI");
	ScrPrint(0, 2, ASCII, "%s", "147 MUAMALAT");
	ScrPrint(0, 3, ASCII, "%s", "151 MESTIKA");
	ScrPrint(0, 4, ASCII, "%s", "153 SINAR MAS");
	ScrPrint(0, 5, ASCII, "%s", "157 MASPION");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_9;

lbl_11:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "161 GANESHA");
	ScrPrint(0, 1, ASCII, "%s", "167 KESAWAN");
	ScrPrint(0, 2, ASCII, "%s", "200 B T N");
	ScrPrint(0, 3, ASCII, "%s", "212 B SAUDARA");
	ScrPrint(0, 4, ASCII, "%s", "213 B T P N");
	ScrPrint(0, 5, ASCII, "%s", "422 BRI SYARIAH");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_10;

lbl_12:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "425 JABARSYA");
	ScrPrint(0, 1, ASCII, "%s", "426 M E G A");
	ScrPrint(0, 2, ASCII, "%s", "441 BUKOPIN");
	ScrPrint(0, 3, ASCII, "%s", "451 B S M");
	ScrPrint(0, 4, ASCII, "%s", "472 JASA JAKARTA");
	ScrPrint(0, 5, ASCII, "%s", "484 BANK HANA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_11;

lbl_13:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "485 BUMI PUTERA");
	ScrPrint(0, 1, ASCII, "%s", "494 BRI AGRO");
	ScrPrint(0, 2, ASCII, "%s", "498 INDOMONEX");
	ScrPrint(0, 3, ASCII, "%s", "501 ROYAL");
	ScrPrint(0, 4, ASCII, "%s", "503 NOBOU BANK");
	ScrPrint(0, 5, ASCII, "%s", "506 B S M I");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_12;

lbl_14:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "513 I N A PERDANA");
	ScrPrint(0, 1, ASCII, "%s", "517 PANIN SYARIAH");
	ScrPrint(0, 2, ASCII, "%s", "523 SAHABAT");
	ScrPrint(0, 3, ASCII, "%s", "535 B K E");
	ScrPrint(0, 4, ASCII, "%s", "542 ARTOS");
	ScrPrint(0, 5, ASCII, "%s", "553 MAYORA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_13;
lbl_15:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "555 INDEX");
	ScrPrint(0, 1, ASCII, "%s", "556 VICTORIA");
	ScrPrint(0, 2, ASCII, "%s", "558 PUNDI");
	ScrPrint(0, 3, ASCII, "%s", "600 B P R/LSB");
	ScrPrint(0, 4, ASCII, "%s", "688 B P R KS");
	ScrPrint(0, 5, ASCII, "%s", "699 BANK EKA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_14;

	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "950 COMMBANK");
	ScrPrint(0, 1, ASCII, "%s", "987 LAINNYA");
	PubDispStringASCII("^ ", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_15;
	
	return 0;
	
}



void MiniAtmParseSummaryBkpn(uchar *TrxName, uint Count, uchar *Amt)
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

void MiniAtmPrintSummaryBkpn(MINIATM_TOTAL_INFO *TotalInfo, ushort TotalCnt, uchar *TotalAmt)
{
	uchar 	szBuff[50+1];
	uint iLoop=0;


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

	for(iLoop=0;iLoop<64;iLoop++)
	{
		if(TotalInfo[iLoop].uiTotalCnt>0)
		{
			MiniAtmParseSummaryBkpn(TotalInfo[iLoop].szNamaFitur, TotalInfo[iLoop].uiTotalCnt, TotalInfo[iLoop].szTotalAmt);
		}
	}
	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	MiniAtmParseSummaryBkpn("TOTAL", TotalCnt, TotalAmt);	
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);


	//FOOTER
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}



void MiniATMSummaryReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	MINIATM_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar TotalAmt[12+1];
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));
	MEM_ZERO(TotalAmt);

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	//ucRet = FindAcqIdxByName("MINIATM", FALSE);
	ucRet = FindAcqIdxByName("BKPN_DUMMY", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcMiniAtmTotal(&TotalInfo, &TotalCnt, TotalAmt, date,  TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("REPORT          "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	MiniAtmPrintSummaryBkpn(TotalInfo, TotalCnt, TotalAmt);
}


void MiniAtmParseDetailBkpn(uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	RecNo;
	ushort	lenRec=0;
	ushort pos=0;
	TRAN_LOG	stTranLog;

	uchar szBuff[50+1];
	uchar szTransType[24];
	uchar PrintBuff[24];
	uchar PrintBuff2[24];


	RecNo= glSysCtrl.uiMiniAtmLastRecNo;
	
	for(pos=0;pos<MAX_MINIATM_TRANLOG;pos++)
	{
		if(RecNo==(MAX_MINIATM_TRANLOG-1))
			RecNo=0;
		else
			RecNo++;

		if(glSysCtrl.sMiniAtmCSAcqKeyList[RecNo]==INV_ACQ_KEY)
			continue;

		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadMiniAtmCSTranLog(&stTranLog, RecNo);
		if( iRet!=0 )
		{
			return;
		}

		FindAcq(stTranLog.ucAcqKey);
		FindIssuer(stTranLog.ucIssuerKey);

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

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_TRFSESAMABRI:
				sprintf(szTransType, "TRANSFER SESAMA BRI  ");
				break;
			case TRANS_TYPE_TRFANTARBANK:
				sprintf(szTransType,  "TRANSFER ANTAR BANK  ");
				break;
			case TRANS_TYPE_TELKOM:
				sprintf(szTransType,  "TELKOM               ");
				break;
			case TRANS_TYPE_KARTUHALO:
				sprintf(szTransType,  "HALO PASCABAYAR      ");
				break;
			case TRANS_TYPE_KARTUMATRIX:
				sprintf(szTransType,  "SELULAR PASCABAYAR    ");
				break;
			case TRANS_TYPE_PLNPASCABAYAR:
				sprintf(szTransType,  "PLN POSTPAID       ");
				break;
			case TRANS_TYPE_PLNPRABAYAR:
				sprintf(szTransType,  "PLN PREPAID         ");
				break;
			case TRANS_TYPE_KKBRI:
				sprintf(szTransType,  "KARTU KREDIT BRI     ");
				break;
			case TRANS_TYPE_KKCITIBANK:
				sprintf(szTransType,  "KARTU KREDIT CITIBANK");
				break;
			case TRANS_TYPE_KTACITIBANK:
				sprintf(szTransType,  "KTA CITIBANK         ");
				break;
			case TRANS_TYPE_KKSCB:
				sprintf(szTransType,  "KARTU KREDIT S. CHART");
				break;
			case TRANS_TYPE_KTASCB:
				sprintf(szTransType,  "KTA S. CHART         ");
				break;
			case TRANS_TYPE_KKHSBC:
				sprintf(szTransType,  "KARTU KREDIT HSBC    ");
				break;
			case TRANS_TYPE_KTAHSBC:
				sprintf(szTransType,  "KTA HSBC             ");
				break;
			case TRANS_TYPE_KTARBS:
				sprintf(szTransType,  "KTA RBS              ");
				break;
			case TRANS_TYPE_KKANZ:
				sprintf(szTransType,  "KARTU KREDIT ANZ     ");
				break;
			case TRANS_TYPE_FIF:
				sprintf(szTransType,  "CICILAN FIF          ");
				break;
			case TRANS_TYPE_BAF:
				sprintf(szTransType,  "CICILAN BAF          ");
				break;
			case TRANS_TYPE_OTO:
				sprintf(szTransType,  "CICILAN OTO          ");
				break;
			case TRANS_TYPE_FINANSIA:
				sprintf(szTransType,  "CICILAN FINANSIA  ");
				break;
			case TRANS_TYPE_VERENA:
				sprintf(szTransType,  "CICILAN VERENA       ");
				break;
			case TRANS_TYPE_WOM:
				sprintf(szTransType,  "PEMB CICILAN WOM     ");
				break;
			case TRANS_TYPE_ZAKAT:
				sprintf(szTransType,  "BAYAR ZAKAT          ");
				break;
			case TRANS_TYPE_INFAQ:
				sprintf(szTransType,  "BAYAR INFAQ          ");
				break;
			case TRANS_TYPE_DPLK:
				sprintf(szTransType,  "BAYAR DPLK           ");
				break;
			case TRANS_TYPE_GARUDA:
				sprintf(szTransType,  "TIKET GARUDA         ");
				break;
			case TRANS_TYPE_LIONAIR:
				sprintf(szTransType,  "TIKET LION           ");
				break;
			case TRANS_TYPE_SRIWIJAYA:
				sprintf(szTransType,  "TIKET SRIWIJAYA      ");
				break;
			case TRANS_TYPE_MERPATI:
				sprintf(szTransType,  "TIKET MERPATI    ");
				break;
			case TRANS_TYPE_BRIVA:
				sprintf(szTransType,  "TIKET BRIVA          ");
				break;
			case TRANS_TYPE_BAYARKULIAH:
				sprintf(szTransType,  "BAYAR SPP            ");
				break;
			case TRANS_TYPE_IM3:
				sprintf(szTransType,  "ISI PULSA IM3        ");
				break;
			case TRANS_TYPE_SIMPATI_AS:
				sprintf(szTransType,  "ISI PULSA SIMPATI/AS ");
				break;
			case TRANS_TYPE_MENTARI:
				sprintf(szTransType,  "ISI PULSA MENTARI    ");
				break;
			case TRANS_TYPE_ESIA:
				sprintf(szTransType,  "ISI PULSA ESIA       ");
				break;
			case TRANS_TYPE_SMARTFREN:
				sprintf(szTransType,  "ISI PULSA SMART      ");
				break;
			//ditutup 6 Januari 2014
			/*
			case TRANS_TYPE_FREN_MOBI_HEPI:
				sprintf(szTransType,  "ISI PULSA FREN       ");
				break;
			*/
			case TRANS_TYPE_THREE:
				sprintf(szTransType,  "ISI PULSA THREE      ");
				break;
			case TRANS_TYPE_AXIS:
				sprintf(szTransType,  "ISI PULSA AXIS       ");
				break;
			case TRANS_TYPE_XL:
				sprintf(szTransType,  "ISI PULSA XL         ");
				break;


			// add for mini atm bukopin

			case TRANS_TYPE_TRFANTARBANK_BKPN:
				sprintf(szTransType, "TRANSFER           ");
				break;

			case TRANS_TYPE_MAF_BKPN:
				sprintf(szTransType, "MAF           ");
				break;

			case TRANS_TYPE_BAF_BKPN:
				sprintf(szTransType, "BAF           ");
				break;

			case TRANS_TYPE_MCF_BKPN:
				sprintf(szTransType, "MCF           ");
				break;

			case TRANS_TYPE_WOM_BKPN:
				sprintf(szTransType, "WOM           ");
				break;

			case TRANS_TYPE_TARIK_TUNAI_BKPN:
				sprintf(szTransType, "TARIK TUNAI      ");
				break;

			case TRANS_TYPE_SETOR_TUNAI_BKPN:
				sprintf(szTransType, "SETOR TUNAI      ");
				break;

			case TRANS_TYPE_KK_ANZ_BKPN:
				sprintf(szTransType, "KK ANZ           ");
				break;

			case TRANS_TYPE_KK_BUKOPIN_BKPN:
				sprintf(szTransType, "KK BUKOPIN       ");
				break;

			case TRANS_TYPE_KK_BNI_BKPN:
				sprintf(szTransType, "KK BNI           ");
				break;

			case TRANS_TYPE_KK_HSBC_BKPN:
				sprintf(szTransType, "KK HSBC          ");
				break;

			case TRANS_TYPE_PDAM_PALYJA_BKPN:
				sprintf(szTransType, "PALYJA           ");
				break;

			case TRANS_TYPE_PDAM_AETRA_BKPN:
				sprintf(szTransType, "AETRA            ");
				break;

			
		}
		
		MEM_ZERO(PrintBuff);
		App_ConvAmountTran(stTranLog.szAmount, PrintBuff, GA_SEPARATOR);

		PrnSetNormal();

		MEM_ZERO(szBuff);
		sprintf((char *)szBuff, "%-31.21s %16s\n", szTransType, PrintBuff);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);
		
		MEM_ZERO(szBuff);
		MEM_ZERO(PrintBuff);
		MEM_ZERO(PrintBuff2);
		memcpy(PrintBuff, stTranLog.szPan, strlen(stTranLog.szPan));
		strcpy(PrintBuff2, "  /  ");
		memcpy(PrintBuff2, stTranLog.szExpDate, 2);
		memcpy(PrintBuff2+3, stTranLog.szExpDate+2, 2);
		sprintf((char *)szBuff, "%-42.19s %5s\n", PrintBuff, PrintBuff2);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		//STAN //TRACE
		MEM_ZERO(szBuff);
		sprintf(szBuff, "STAN : %-26.06lu TRACE : %06lu\n", stTranLog.ulSTAN, stTranLog.ulInvoiceNo);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		PrnDateTime();

		//REFNUM
		MEM_ZERO(szBuff);
		sprintf(szBuff, "REF# : %-26.12s APPR  : %6s\n", stTranLog.szRRN, stTranLog.szAuthCode);
		MultiLngPrnStr(szBuff, DISP_LINE_LEFT);

		StartPrinter();
		
	}
	return;
}



void MiniAtmPrintDetailBkpn(uchar *date, uchar flagDate)
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
	MiniAtmParseDetailBkpn(date, flagDate);

	//FOOTER
	PrnInit();
	PrnSetNormal();
	MultiLngPrnStr("------------------------------------------------\n", DISP_LINE_LEFT);
	PrnFooterInfoSaldo();
	PrnStr("\n\n\n\n\n\n\n\n");
	StartPrinter();

	return;
}


void MiniATMDetailReport(uchar FlagDate)
{
	uchar	ucRet;
	uchar date[6+1];
	MINIATM_TOTAL_INFO	TotalInfo[64];
	ushort TotalCnt=0;
	uchar TotalAmt[12+1];
	uchar pszDateTime[14+1];

	memset(date, 0, sizeof(date));
	MEM_ZERO(TotalAmt);

	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
	if( glSysCtrl.uiMiniAtmLastRecNo>=MAX_MINIATM_TRANLOG )
	{
		PubDispString(_T("EMPTY BATCH"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
	}

	if(FlagDate)
	{
		ScrPrint(0, 4, CFONT, "%s", "DDMMYY");
		ucRet = PubGetString(NUM_IN, 6,6 , date, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return;
		}

	}
	else
	{
		GetDateTime(pszDateTime);
		memcpy(date, pszDateTime+6, 2);
		memcpy(date+2, pszDateTime+4, 2);
		memcpy(date+4, pszDateTime+2, 2);
	}
	
	//ucRet = FindAcqIdxByName("MINIATM", FALSE);
	ucRet = FindAcqIdxByName("BKPN_DUMMY", FALSE);
	if(ucRet==MAX_ACQ)
		return;

	SetCurAcq(ucRet);

	DispProcess();

	memset(TotalInfo, 0, sizeof(TotalInfo));
	CalcMiniAtmTotal(&TotalInfo, &TotalCnt, TotalAmt, date, TRUE);

	if(TotalCnt==0)
	{
		ScrCls();
		PubShowTitle(TRUE, (uchar *)_T("DETAIL REPORT   "));
		PubDispString(_T("NO TRANSACTION"),     3|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(5);
		return;
		
	}
	MiniAtmPrintDetailBkpn( date, TRUE);
}




int TransIsiUlangBkpn(void)
{
	int		iRet;
	uchar	ucRet;
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		kodeArea[4+1];
	char		noGSM[13+1];
	char		noCDMA[9+1];
	char		Operator[15+1];
	char		bit103Req[28+1];
	char		nominal[12+1];
	char		labelOperator[21+1];
	char		nominalDsp[21+1];
	char		dspGSM[13+1];
	char		dspCDMA[13+1];

	uchar uckey;

	MEM_ZERO(Operator);
	MEM_ZERO(bit103Req);
	MEM_ZERO(dspGSM);
	MEM_ZERO(dspCDMA);
	MEM_ZERO(labelOperator);
	
	mapGetWord(traMnuItm, mnuItm);

	switch(mnuItm)
		{
			case mnuEvoucherBkpnEsia:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG");
				strcpy(labelOperator,"NO. ESIA :");
				strcpy(Operator,"ESIA");
				strcpy(glProcInfo.stTranLog.BRI.ISIULANG.OperatorProvider, "ESIA"); 
				iRet = TransInit(TRANS_TYPE_ESIA);
				break;
    		case mnuEvoucherBkpnIndosat:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG");
				strcpy(labelOperator,"NO.INDOSAT OOREDOO:");
				strcpy(Operator,"INDOSAT OOREDOO");
				strcpy(glProcInfo.stTranLog.BRI.ISIULANG.OperatorProvider, "INDOSAT OOREDOO"); 
				iRet = TransInit(TRANS_TYPE_IM3);
				break;
			case mnuEvoucherBkpnTelkomsel:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG");
				strcpy(labelOperator,"NO. SIMPATI/AS :");
				strcpy(Operator,"SIMPATI/AS");
				strcpy(glProcInfo.stTranLog.BRI.ISIULANG.OperatorProvider, "SIMPATI/AS"); 
				iRet = TransInit(TRANS_TYPE_SIMPATI_AS);
				break;
			case mnuEvoucherBkpnSmartfren:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG");
				strcpy(labelOperator,"NO. SMARTFREN :");
				strcpy(Operator,"SMARTFREN");
				strcpy(glProcInfo.stTranLog.BRI.ISIULANG.OperatorProvider, "SMARTFREN"); 
				iRet = TransInit(TRANS_TYPE_SMARTFREN);
				break;
			case mnuEvoucherBkpnXl:
				strcpy(glProcInfo.stTranLog.szTransName,"ISI ULANG");
				strcpy(labelOperator,"NO. XL :");
				strcpy(Operator,"XL");
				strcpy(glProcInfo.stTranLog.BRI.ISIULANG.OperatorProvider, "XL"); 
				iRet = TransInit(TRANS_TYPE_XL);
				break;
		}


	

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
		
	MEM_ZERO(nominal);
	MEM_ZERO(nominalDsp);


/*
	while (1)
 		{
 		ScrCls();
 		kbflush();
 		ScrPrint(0, 0, 1, "Pilih Nominal :");

		ScrPrint(0, 3, ASCII, "%s", "1.  25 rb   5. 200 rb");
		ScrPrint(0, 4, ASCII, "%s", "2.  50 rb   6. 300 rb");
		ScrPrint(0, 5, ASCII, "%s", "3. 100 rb   7. 500 rb");
		ScrPrint(0, 6, ASCII, "%s", "4. 150 rb   ");
		//ScrPrint(0, 7, ASCII, "%s", "4. 200 rb   9. 500 rb");
		
 		uckey = getkey();
 		if (uckey == KEY0)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);
			break;
		}
 		else if(uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);

			break;
 		}
		else if (uckey == KEY2)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000005000000", 12);
			memcpy(nominal, "000000050000", 12);
			memcpy(nominalDsp, "Rp.            50.000", 21);
			break;
		}
 		else if(uckey == KEY3)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000010000000", 12);
			memcpy(nominal, "000000100000", 12);
			memcpy(nominalDsp, "Rp.           100.000", 21);

			break;
 		}
		else if (uckey == KEY4)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000015000000", 12);
			memcpy(nominal, "000000150000", 12);
			memcpy(nominalDsp, "Rp.           150.000", 21);
			break;
		}
 		else if(uckey == KEY5)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000020000000", 12);
			memcpy(nominal, "000000200000", 12);
			memcpy(nominalDsp, "Rp.           200.000", 21);

			break;
 		}
		else if (uckey == KEY6)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000030000000", 12);
			memcpy(nominal, "000000300000", 12);
			memcpy(nominalDsp, "Rp.           300.000", 21);
			break;
		}
 		else if(uckey == KEY7)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000050000000", 12);
			memcpy(nominal, "000000500000", 12);
			memcpy(nominalDsp, "Rp.           500.000", 21);

			break;
 		}
		
		else if (uckey == KEY8)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000025000000", 12);
			memcpy(nominal, "000000250000", 12);
			memcpy(nominalDsp, "Rp.           250.000", 21);
			break;
		}
 		else if(uckey == KEY9)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000050000000", 12);
			memcpy(nominal, "000000500000", 12);
			memcpy(nominalDsp, "Rp.           500.000", 21);

			break;
 		}
		
		else if(uckey == KEYCANCEL)
		{
			return 0;
		}
 	}
*/


///////////////////////////////////
	switch(mnuItm)
			{
				case mnuEvoucherBkpnEsia:
					while (1)
 		{
 		ScrCls();
 		kbflush();
 		ScrPrint(0, 0, 1, "Pilih Nominal :");

		ScrPrint(0, 3, ASCII, "%s", "1.  25 rb");
		ScrPrint(0, 4, ASCII, "%s", "2.  50 rb");
		ScrPrint(0, 5, ASCII, "%s", "3. 100 rb");
		
 		uckey = getkey();
 		if (uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);
			break;
		}
 		else if(uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);

			break;
 		}
		else if (uckey == KEY2)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000005000000", 12);
			memcpy(nominal, "000000050000", 12);
			memcpy(nominalDsp, "Rp.            50.000", 21);
			break;
		}
 		else if(uckey == KEY3)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000010000000", 12);
			memcpy(nominal, "000000100000", 12);
			memcpy(nominalDsp, "Rp.           100.000", 21);

			break;
 		}
		else if(uckey == KEYCANCEL)
		{
			return 0;
		}
 	}
					
					break;
				case mnuEvoucherBkpnIndosat:

					while (1)
 		{
 		ScrCls();
 		kbflush();
 		ScrPrint(0, 0, 1, "Pilih Nominal :");

		ScrPrint(0, 3, ASCII, "%s", "1.  25 rb   ");
		ScrPrint(0, 4, ASCII, "%s", "2.  50 rb   ");
		ScrPrint(0, 5, ASCII, "%s", "3. 100 rb   ");
		ScrPrint(0, 6, ASCII, "%s", "4. 150 rb   ");
		//ScrPrint(0, 7, ASCII, "%s", "4. 200 rb   9. 500 rb");
		
 		uckey = getkey();
 		if (uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);
			break;
		}
 		else if(uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);

			break;
 		}
		else if (uckey == KEY2)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000005000000", 12);
			memcpy(nominal, "000000050000", 12);
			memcpy(nominalDsp, "Rp.            50.000", 21);
			break;
		}
 		else if(uckey == KEY3)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000010000000", 12);
			memcpy(nominal, "000000100000", 12);
			memcpy(nominalDsp, "Rp.           100.000", 21);

			break;
 		}
		else if (uckey == KEY4)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000015000000", 12);
			memcpy(nominal, "000000150000", 12);
			memcpy(nominalDsp, "Rp.           150.000", 21);
			break;
		}
		else if(uckey == KEYCANCEL)
		{
			return 0;
		}
 	}
					break;
				case mnuEvoucherBkpnTelkomsel:
					while (1)
 		{
 		ScrCls();
 		kbflush();
 		ScrPrint(0, 0, 1, "Pilih Nominal :");

		ScrPrint(0, 3, ASCII, "%s", "1.  25 rb   5. 200 rb");
		ScrPrint(0, 4, ASCII, "%s", "2.  50 rb   6. 300 rb");
		ScrPrint(0, 5, ASCII, "%s", "3. 100 rb   7. 500 rb");
		ScrPrint(0, 6, ASCII, "%s", "4. 150 rb   8. 1 juta");
		//ScrPrint(0, 7, ASCII, "%s", "4. 200 rb   9. 500 rb");
		
 		uckey = getkey();
 		if (uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);
			break;
		}
 		else if(uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);

			break;
 		}
		else if (uckey == KEY2)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000005000000", 12);
			memcpy(nominal, "000000050000", 12);
			memcpy(nominalDsp, "Rp.            50.000", 21);
			break;
		}
 		else if(uckey == KEY3)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000010000000", 12);
			memcpy(nominal, "000000100000", 12);
			memcpy(nominalDsp, "Rp.           100.000", 21);

			break;
 		}
		else if (uckey == KEY4)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000015000000", 12);
			memcpy(nominal, "000000150000", 12);
			memcpy(nominalDsp, "Rp.           150.000", 21);
			break;
		}
 		else if(uckey == KEY5)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000020000000", 12);
			memcpy(nominal, "000000200000", 12);
			memcpy(nominalDsp, "Rp.           200.000", 21);

			break;
 		}
		else if (uckey == KEY6)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000030000000", 12);
			memcpy(nominal, "000000300000", 12);
			memcpy(nominalDsp, "Rp.           300.000", 21);
			break;
		}
 		else if(uckey == KEY7)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000050000000", 12);
			memcpy(nominal, "000000500000", 12);
			memcpy(nominalDsp, "Rp.           500.000", 21);

			break;
 		}
		
		else if (uckey == KEY8)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000100000000", 12);
			memcpy(nominal, "000001000000", 12);
			memcpy(nominalDsp, "Rp.         1.000.000", 21);
			break;
		}
		else if(uckey == KEYCANCEL)
		{
			return 0;
		}
 	}
					
					break;
				case mnuEvoucherBkpnSmartfren:
					while (1)
 		{
 		ScrCls();
 		kbflush();
 		ScrPrint(0, 0, 1, "Pilih Nominal :");

		ScrPrint(0, 3, ASCII, "%s", "1.  25 rb   5. 150 rb");
		ScrPrint(0, 4, ASCII, "%s", "2.  50 rb   6. 200 rb");
		ScrPrint(0, 5, ASCII, "%s", "3.  60 rb   7. 300 rb");
		ScrPrint(0, 6, ASCII, "%s", "4. 100 rb   8. 500 rb");
		//ScrPrint(0, 7, ASCII, "%s", "4. 200 rb   9. 500 rb");
		
 		uckey = getkey();
 		if (uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);
			break;
		}
 		else if(uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);

			break;
 		}
		else if (uckey == KEY2)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000005000000", 12);
			memcpy(nominal, "000000050000", 12);
			memcpy(nominalDsp, "Rp.            50.000", 21);
			break;
		}
		else if (uckey == KEY3)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000006000000", 12);
			memcpy(nominal, "000000060000", 12);
			memcpy(nominalDsp, "Rp.            60.000", 21);
			break;
		}
 		else if(uckey == KEY4)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000010000000", 12);
			memcpy(nominal, "000000100000", 12);
			memcpy(nominalDsp, "Rp.           100.000", 21);

			break;
 		}
		else if (uckey == KEY5)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000015000000", 12);
			memcpy(nominal, "000000150000", 12);
			memcpy(nominalDsp, "Rp.           150.000", 21);
			break;
		}
 		else if(uckey == KEY6)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000020000000", 12);
			memcpy(nominal, "000000200000", 12);
			memcpy(nominalDsp, "Rp.           200.000", 21);

			break;
 		}
		else if (uckey == KEY7)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000030000000", 12);
			memcpy(nominal, "000000300000", 12);
			memcpy(nominalDsp, "Rp.           300.000", 21);
			break;
		}
 		else if(uckey == KEY8)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000050000000", 12);
			memcpy(nominal, "000000500000", 12);
			memcpy(nominalDsp, "Rp.           500.000", 21);

			break;
 		}
		else if(uckey == KEYCANCEL)
		{
			return 0;
		}
 	}
					
					break;
				case mnuEvoucherBkpnXl:
					while (1)
 		{
 		ScrCls();
 		kbflush();
 		ScrPrint(0, 0, 1, "Pilih Nominal :");

		ScrPrint(0, 3, ASCII, "%s", "1.  25 rb   ");
		ScrPrint(0, 4, ASCII, "%s", "2.  50 rb   ");
		ScrPrint(0, 5, ASCII, "%s", "3. 100 rb   ");
		ScrPrint(0, 6, ASCII, "%s", "4. 200 rb   ");
		//ScrPrint(0, 7, ASCII, "%s", "4. 200 rb   9. 500 rb");
		
 		uckey = getkey();
 		if (uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);
			break;
		}
 		else if(uckey == KEY1)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000002500000", 12);
			memcpy(nominal, "000000025000", 12);
			memcpy(nominalDsp, "Rp.            25.000", 21);

			break;
 		}
		else if (uckey == KEY2)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000005000000", 12);
			memcpy(nominal, "000000050000", 12);
			memcpy(nominalDsp, "Rp.            50.000", 21);
			break;
		}
 		else if(uckey == KEY3)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000010000000", 12);
			memcpy(nominal, "000000100000", 12);
			memcpy(nominalDsp, "Rp.           100.000", 21);

			break;
 		}
		else if (uckey == KEY4)
 		{
 			memcpy(glProcInfo.stTranLog.szAmount, "000020000000", 12);
			memcpy(nominal, "000000200000", 12);
			memcpy(nominalDsp, "Rp.           200.000", 21);
			break;
		}
		else if(uckey == KEYCANCEL)
		{
			return 0;
		}
 	}
					
					break;
			}

////////////////////////////////////////////


	switch(mnuItm)
		{
		
   		case mnuEvoucherBkpnXl:
   		case mnuEvoucherBkpnSmartfren:
   		case mnuEvoucherBkpnTelkomsel:
   		case mnuEvoucherBkpnIndosat:
			MEM_ZERO(noGSM);

			ScrCls();
			DispTransName();
			//ScrPrint(0, 2, ASCII, "%s", "ENTER NOMOR");
			ScrPrint(0, 2, ASCII, "%s", "MASUKKAN NOMOR");
			ScrPrint(0, 3, ASCII, "%s", Operator);
			ucRet = PubGetString(NUM_IN, 4,13 , noGSM, USER_OPER_TIMEOUT);
			if(ucRet!=0)
			{
				return ERR_USERCANCEL;
			}

			memcpy(dspGSM,noGSM,strlen(noGSM));
			memcpy(bit103Req,noGSM,strlen(noGSM));
			//memcpy(bit103Req+13,nominal,strlen(nominal));
			memcpy(glProcInfo.szReq103,bit103Req,strlen(bit103Req));
			break;
			
		case mnuEvoucherBkpnEsia:
			MEM_ZERO(kodeArea);

			ScrCls();
			DispTransName();
			//ScrPrint(0, 2, ASCII, "%s", "ENTER NOMOR");
			ScrPrint(0, 2, ASCII, "%s", "MASUKKAN NOMOR");
			ScrPrint(0, 3, ASCII, "%s", "KODE AREA");
			ucRet = PubGetString(NUM_IN, 3,4 , kodeArea, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}
			memcpy(dspCDMA,kodeArea,strlen(kodeArea));
			memcpy(bit103Req,kodeArea,strlen(kodeArea));
			MEM_0x20(noCDMA);
		
			ScrCls();
			DispTransName();
			//ScrPrint(0, 2, ASCII, "%s", "ENTER NOMOR");
			ScrPrint(0, 2, ASCII, "%s", "MASUKKAN NOMOR");
			ScrPrint(0, 3, ASCII, "%s", Operator);
			ucRet = PubGetString(NUM_IN, 4,9 , noCDMA, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}
			strcat(dspCDMA,noCDMA);
			//memcpy(bit103Req+4,noCDMA,strlen(noCDMA));
			memcpy(bit103Req,dspCDMA,strlen(dspCDMA));
			//memcpy(bit48Req+13,nominal,strlen(nominal));
			memcpy(glProcInfo.szReq103,bit103Req,strlen(bit103Req));
			break;
		}

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrCls();
	DispTransName();
	ScrPrint(0, 2, ASCII, "%s", labelOperator);

	switch(mnuItm)
		{
		case mnuEvoucherBkpnXl:
   		case mnuEvoucherBkpnSmartfren:
   		case mnuEvoucherBkpnTelkomsel:
   		case mnuEvoucherBkpnIndosat:
			ScrPrint(0, 3, ASCII, "%s", dspGSM);
			break;
		case mnuEvoucherBkpnEsia:
			ScrPrint(0, 3, ASCII, "%s", dspCDMA);
			break;
		}
	ScrPrint(0, 4, ASCII, "%s", "NOMINAL :");
	ScrPrint(0, 5, ASCII, "%s", nominalDsp);
	ScrPrint(0, 7, ASCII, "%s", "BATAL          LANJUT");

	ucRet = PubYesNo(USER_OPER_TIMEOUT);

	if(ucRet!=0)
		return FALSE;

	//SET ISO 8583
	setPackIsiUlangBkpn();


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
		if ( memcmp(glProcInfo.stTranLog.szRspCode, "68", 2 )==0 )
			{
				CommOnHook(FALSE);
				SaveRevInfo(FALSE);
				// diki add for mini atm bukopin
				return ERR_NO_DISP;
			}
		else
			{
				CommOnHook(FALSE);
				SaveRevInfo(FALSE);

				return ERR_NO_DISP;
			}
	}

	unPackISi_UlangBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}

	return 0;
}




int TransMultifinanceBkpn(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;
	
	char		bit103Req[28+1];
	char		bit48Rsp[lenBit48+1];
	char		noKontrak[16+1];

	char 	bit102Rsp[28+1];
	char 	bit103Rsp[28+1];


	/*
	char		nama[20+1];
	char		jatuhTempo[10+1];
	char		angsuran[4+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];
*/
/*
	char		screenNoKontrak[21+1];
	char		screenJatuhTempo[21+1];
	char		screenAngsuran[21+1];
	char		screenTagihan[21+1];
*/

	char		screenbillid[30+1];
	char		screenname[30+1];
	char		screenptname[30+1];
	char		screenbranchname[30+1];
	char		screenserialnumber[30+1];
	char		screenmerkkendaraan[30+1];
	char		screentenor[30+1];
	char		screensisaangsuran[30+1];
	char		screenjatuhtempo[30+1];
	char		screennilaitagihan[30+1];
	char		screennilaiangsuran[30+1];
	char		screennilaidenda[30+1];
	char		screennilaiadmin[30+1];
	char		screennilailainnya[30+1];
	char		screentotalPembayaran[30+1];
	

	char		billid[16+1];
	char		name[30+1];
	char		ptname[25+1];
	char		branchname[30+1];
	char		serialnumber[14+1];
	char		norangka[25+1];
	char		merkkendaraan[42+1];
	char		tenor[3+1];
	char		sisaangsuran[12+1];
	char		noangsuran[3+1];
	char		jatuhtempo[9+1];
	char		nilaitagihan[12+1];
	char		nilaiangsuran[12+1];
	char		nilaidenda[12+1];
	char		nilaiadmin[12+1];
	char		nilailainnya[12+1];
	char		referencenumber[32+1];

	char		sisaangsuranFmt[20+1];
	char		nilaitagihanFmt[20+1];
	char		nilaiangsuranFmt[20+1];
	char		nilaidendaFmt[20+1];

	char		nilaiadminFmt[20+1];
	char		nilailainnyaFmt[20+1];

	char		totalPembayaran[20+1];
	char		totalPembayaranFmt[20+1];
	
	

	word	mnuItm;
	MEM_ZERO(bit103Req);
	
	mapGetWord(traMnuItm, mnuItm);

	switch(mnuItm)
		{
		case mnuMafBkpn:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN CICILAN MAF");
			//strcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Tampilan,"CICILAN FIF");
			iRet = TransInit(TRANS_TYPE_INQ_MAF_BKPN);
			bit103Req[0]='4';
			bit103Req[1]='1';
			bit103Req[2]='0';
			bit103Req[3]='4';
			break;
		case mnuMcfBkpn:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN CICILAN MCF");
			//strcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Tampilan,"CICILAN BAF");
			iRet = TransInit(TRANS_TYPE_INQ_MCF_BKPN);
			bit103Req[0]='4';
			bit103Req[1]='1';
			bit103Req[2]='0';
			bit103Req[3]='3';
			break;
		case mnuWomBkpn:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN CICILAN WOM");
			iRet = TransInit(TRANS_TYPE_INQ_WOM_BKPN);
			bit103Req[0]='4';
			bit103Req[1]='1';
			bit103Req[2]='0';
			bit103Req[3]='5';
			break;
		case mnuBafBkpn:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN CICILAN BAF");
			iRet = TransInit(TRANS_TYPE_INQ_BAF_BKPN);
			bit103Req[0]='4';
			bit103Req[1]='1';
			bit103Req[2]='0';
			bit103Req[3]='6';
			break;
		}



					//multi finance mini atm bukopin
					/*
		case TRANS_TYPE_INQ_MAF_BKPN,
		case TRANS_TYPE_MAF_BKPN,
		case TRANS_TYPE_INQ_BAF_BKPN,
		case TRANS_TYPE_BAF_BKPN,
		case TRANS_TYPE_INQ_MCF_BKPN,
		case TRANS_TYPE_MCF_BKPN,
		case TRANS_TYPE_INQ_WOM_BKPN,
		case TRANS_TYPE_WOM_BKPN,
			*/
			

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


	MEM_ZERO(noKontrak);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	PubDispString("KONTRAK :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 5,16 , noKontrak, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit103Req+4,noKontrak,strlen(noKontrak));
	memcpy(glProcInfo.szReq103,bit103Req,strlen(bit103Req));

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqMultifinanceBkpn();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	


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


	/*
	char		billid[16+1];
	char		name[30+1];
	char		ptname[25+1];
	char		branchname[30+1];
	char		serialnumber[14+1];
	char		norangka[25+1];
	char		merkkendaraan[42+1];
	char		tenor[3+1];
	char		sisaangsuran[12+1];
	char		noangsuran[3+1];
	char		jatuhtempo[9+1];
	char		nilaitagihan[12+1];
	char		nilaiangsuran[12+1];
	char		nilaidenda[12+1];
	char		tagihanFmt[20+1];
	char		referencenumber[32+1];
	*/


/*
product id 4 		:4104
Bill id 16 		:1361101150      
Cust Name 30		:RAJA ARDIANSYAH               
PT name 25		:PT MEGA AUTO FINANCE     
Branch name 30		:Pekanbaru MAF                 
Serial number 14	:BM6402NH      
No rangka 25 		:MH350C001BK110458        
Merk/type kendaraan 42 	:YAMAHA JUPITER MX CW 2011                 
Tenor  3 		:019
Sisa angsuran 12 	:000014280000
no angsuran 3 		:005
Jatuh tempo 9 		:20 Nov 11
Nilai tagihan 12 	:000000000000
Nilai angsuran 12	:000000952000
Nilai denda 12		:000000000000
Nilai admin 12 		:            
Nilai lainnya 12 	:000000000000
no ref 32 	
*/
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	//mapPutStr(traResponseInq, bit48Rsp);

	memcpy(billid,bit48Rsp+4,16);
	memcpy(name,bit48Rsp+20,30);
	memcpy(ptname,bit48Rsp+50,25);
	memcpy(branchname,bit48Rsp+75,30);
	memcpy(serialnumber,bit48Rsp+105,14);
	memcpy(norangka,bit48Rsp+119,25);
	memcpy(merkkendaraan,bit48Rsp+144,42);
	memcpy(tenor,bit48Rsp+186,3);
	memcpy(sisaangsuran,bit48Rsp+189,12);
	memcpy(noangsuran,bit48Rsp+201,3);
	memcpy(jatuhtempo,bit48Rsp+204,9);
	memcpy(nilaitagihan,bit48Rsp+213,12);
	memcpy(nilaiangsuran,bit48Rsp+225,12);
	memcpy(nilaidenda,bit48Rsp+237,12);
	memcpy(nilaiadmin,bit48Rsp+249,12);
	memcpy(nilailainnya,bit48Rsp+261,12);
	memcpy(referencenumber,bit48Rsp+273,32);
	
	//memcpy(jatuhTempo,bit48Rsp+77,2);
	//memcpy(jatuhTempo+2,"-",1);
	fmtAmt(sisaangsuranFmt,sisaangsuran,0,",.");
	fmtAmt(nilaitagihanFmt,nilaitagihan,0,",.");	
	fmtAmt(nilaiangsuranFmt,nilaiangsuran,0,",.");
	fmtAmt(nilaidendaFmt,nilaidenda,0,",.");

	fmtAmt(nilaiadminFmt,nilaiadmin,0,",.");
	fmtAmt(nilailainnyaFmt,nilailainnya,0,",.");

	strcpy(totalPembayaran, glRecvPack.szBit4);
	fmtAmt(totalPembayaranFmt,totalPembayaran,2,",.");


	MEM_ZERO(bit102Rsp);
	MEM_ZERO(bit103Rsp);
	PubChar2Long(glRecvPack.szField102, 2, &lenData);
	memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);
	PubChar2Long(glRecvPack.szField103, 2, &lenData);
	memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);
	

	

	//memcpy(glProcInfo.stTranLog.szAmount,bit48Rsp+85,12);
	strcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4);

/*
		char		screenbillid[30+1];
	char		screenname[30+1];
	char		screenptname[30+1];
	char		screenbranchname[30+1];
	char		screenserialnumber[30+1];
	char		screenmerkkendaraan[30+1];
	char		screentenor[30+1];
	char		screensisaangsuran[30+1];
	char		screenjatuhtempo[30+1];
	char		screennilaitagihan[30+1];
	char		screennilaiangsuran[30+1];
	char		screennilaidenda[30+1];
	char		screennilaiadmin[30+1];
	char		screennilailainnya[30+1];
	char		screentotalPembayaran[30+1];
	*/
	
	sprintf(screensisaangsuran, "Rp.%s",sisaangsuranFmt);
	sprintf(screennilaitagihan, "Rp.%s",nilaitagihanFmt);
	sprintf(screennilaiangsuran, "Rp.%s",nilaiangsuranFmt);
	sprintf(screennilaidenda, "Rp.%s",nilaidendaFmt);
	sprintf(screennilaiadmin,"Rp.%s",jatuhtempo);
	sprintf(screennilailainnya,  "Rp.%s",nilailainnyaFmt);
	sprintf(screentotalPembayaran,   "Rp.%s",totalPembayaranFmt);
	
	ScrCls();
	PubDispStringASCII("No.Kontrak:", DISP_LINE_LEFT|0);
	PubDispStringASCII(billid, DISP_LINE_LEFT|1);
	PubDispStringASCII("Nama:", DISP_LINE_LEFT|2);
	PubDispStringASCII(name, DISP_LINE_RIGHT|3);
	PubDispStringASCII("Nama PT:", DISP_LINE_LEFT|4);
	PubDispStringASCII(ptname, DISP_LINE_LEFT|5);

	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	PubDispStringASCII("Cabang:", DISP_LINE_LEFT|0);
	PubDispStringASCII(branchname, DISP_LINE_LEFT|1);
	PubDispStringASCII("No Seri:", DISP_LINE_LEFT|2);
	PubDispStringASCII(serialnumber, DISP_LINE_LEFT|3);
	PubDispStringASCII("Kendaraan:", DISP_LINE_LEFT|4);
	PubDispStringASCII(merkkendaraan, DISP_LINE_LEFT|5);

	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	PubDispStringASCII("Tenor:", DISP_LINE_LEFT|0);
	PubDispStringASCII(tenor, DISP_LINE_RIGHT|1);
	PubDispStringASCII("Sisa angsuran:", DISP_LINE_LEFT|2);
	PubDispStringASCII(screensisaangsuran, DISP_LINE_RIGHT|3);
	PubDispStringASCII("Angsuran ke:", DISP_LINE_LEFT|4);
	PubDispStringASCII(noangsuran, DISP_LINE_RIGHT|5);

	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	PubDispStringASCII("Jatuh tempo:", DISP_LINE_LEFT|0);
	PubDispStringASCII(jatuhtempo, DISP_LINE_RIGHT|1);
	PubDispStringASCII("Total pembayaran:", DISP_LINE_LEFT|2);
	PubDispStringASCII(screentotalPembayaran, DISP_LINE_RIGHT|3);

	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	ScrCls();
	DispTransName();
	


	switch(mnuItm)
		{
		case mnuMafBkpn:
			iRet = TransInit(TRANS_TYPE_MAF_BKPN);
			break;
		case mnuBafBkpn:
			iRet = TransInit(TRANS_TYPE_BAF_BKPN);
			break;
		case mnuMcfBkpn:
			iRet = TransInit(TRANS_TYPE_MCF_BKPN);
			break;
		case mnuWomBkpn:
			iRet = TransInit(TRANS_TYPE_WOM_BKPN);
			break;
		}


	//multi finance mini atm bukopin
					/*
		case TRANS_TYPE_INQ_MAF_BKPN,
		case TRANS_TYPE_MAF_BKPN,
		case TRANS_TYPE_INQ_BAF_BKPN,
		case TRANS_TYPE_BAF_BKPN,
		case TRANS_TYPE_INQ_MCF_BKPN,
		case TRANS_TYPE_MCF_BKPN,
		case TRANS_TYPE_INQ_WOM_BKPN,
		case TRANS_TYPE_WOM_BKPN,
			*/
			

	if( iRet!=0 )
	{
		return iRet;
	}


	strcpy(glProcInfo.szReq48,bit48Rsp);
	strcpy(glProcInfo.szReq102,bit102Rsp);
	strcpy(glProcInfo.szReq103,bit103Rsp);
	

	//SET ISO 8583
	setPackMultifinanceBkpn();

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

		return ERR_NO_DISP;
	}


	MEM_ZERO(bit48Rsp);
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	mapPutStr(traResponseInq,bit48Rsp);
	

	unPackPembayaran_CicilanBkpn();

	//SAVE TRANSACTION
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}
		
	return 0;
}




int TransTrfAntarBankTes(void)
{
	int		ret;
	int		iRet;
	uchar 	ucRet;
	char 	bit48Rsp[lenBit48+1];
	char 	nama[30+1];
	ulong 	lenData;
	uchar	ucEntryMode=0;

	char 	bit48Req[148+1];
	char		KodeBank[148+1];
	char 	noRek[28+1];
	char		noRef[16+1];

	char 	bankAsal[20+1];
	char 	bankTujuan[20+1];
	char 	uang[12+1];
	char		uangFmt[20+1];

	char 	norekAsal[28+1];

	char screenUang[21+1];
	char screenBankAsal[21+1];
	char screenBankTujuan[21+1];
	char screenNoRek[21+1];
	char screenNoRef[21+1];

	
	char 	bit103Req[28+1];
	char 	bit102Rsp[28+1];
	char 	bit103Rsp[28+1];

	uchar	ucKey;

	byte jenisRekening;
	//mapGetByte(traSumberRek, jenisRekening);

	MEM_ZERO(screenUang);
	MEM_ZERO(screenBankAsal);
	MEM_ZERO(screenBankTujuan);
	MEM_ZERO(screenNoRek);
	MEM_ZERO(screenNoRef);

	strcpy(glProcInfo.stTranLog.szTransName,"TRANSFER");
			
	iRet = TransInit(TRANS_TYPE_INQ_TRFANTARBANK_BKPN);
	if( iRet!=0 )
	{
		return iRet;
	}

	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
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
	
	////// diki add komen   RUN_FUNCTION(PilihJenisRekening, lblKO);
	/////RUN_FUNCTION(PilihJenisRekeningBkpn, lblKO);
	mapPutByte(traSumberRek, 2);

	MEM_ZERO(bit48Req);
	MEM_ZERO(KodeBank);
	MEM_ZERO(noRek);
	MEM_ZERO(noRef);

	MEM_ZERO(bit103Req);
	
	mapGetByte(traSumberRek, jenisRekening);
	switch(jenisRekening)
		{
		case 1:
			ScrCls();
			iRet = GetCard(ucEntryMode);
				if( iRet!=0 )
				{
					return iRet;
				}
	
			ScrCls();
			DispTransName();
			PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
			PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}

			////memcpy(bit48Req+3,noRek,strlen(noRek));
			memcpy(bit103Req,noRek,strlen(noRek));
			break;
		case 2:

			ScrCls();
			iRet = GetCard(ucEntryMode);
				if( iRet!=0 )
				{
					return iRet;
				}
	
			

		while(1)
		{
			ScrCls();
			//DispTransName();
			PubDispString("LIHAT DAFTAR:", DISP_LINE_LEFT|0);
			PubDispString("KODE BANK ?", DISP_LINE_LEFT|2);
			PubDispString("NO       YES", DISP_LINE_LEFT|6);
			
		ucKey = PubWaitKey(30);
			if ((ucKey==KEYENTER) || (ucKey==KEYUP) || (ucKey==KEYDOWN))
			{
			TransInfoKodeBankBkpn2();
			//continue;
			}
			else 
				{
				break;
				}
		}


			ScrCls();
			DispTransName();
			PubDispString("MASUKKAN", DISP_LINE_LEFT|2);
			PubDispString("KODE BANK :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 3,10 , KodeBank, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}

			fmtPad(KodeBank, 147, ' ');

			memcpy(bit48Req,KodeBank,strlen(KodeBank));
			//memcpy(bit48Req+3,"                                                                                                                                                ",144);
			//memcpy(bit103Req,KodeBank,strlen(KodeBank));

			ScrCls();
			DispTransName();
			PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
			PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}

			////memcpy(bit48Req+3,noRek,strlen(noRek));
			///memcpy(bit103Req+3,noRek,strlen(noRek));
			memcpy(bit103Req,noRek,strlen(noRek));
			break;
		}


	/*
	ScrCls();
	DispTransName();
	PubDispString("KODE BANK :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 3,3 , KodeBank, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	///memcpy(bit48Req,KodeBank,strlen(KodeBank));
	memcpy(bit103Req,KodeBank,strlen(KodeBank));

	ScrCls();
	DispTransName();
	PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
	PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	////memcpy(bit48Req+3,noRek,strlen(noRek));
	memcpy(bit103Req+3,noRek,strlen(noRek));
	*/

	iRet = InputUangBkpn();
	if( iRet!=0 )
	{
		return iRet;
	}

	/*
	ScrCls();
	DispTransName();
	PubDispString("CUST REF NUMBER", DISP_LINE_LEFT|2);
	PubDispString("ATAU LANJUT  :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 0, 16, noRef, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(bit48Req+23,noRef,strlen(noRef));
	*/
	strcpy(glProcInfo.szReq48,bit48Req);
	strcpy(glProcInfo.szReq103,bit103Req);

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	////// diki add komen  setPackInqTransferAntarBank();
	setPackInqTransferAntarBankBkpn();

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

		displayResponse();
		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(bit102Rsp);
	MEM_ZERO(bit103Rsp);
	//bit102Rsp

	// Overbook / Fund Transfer BIT 48 Respone
	// Destination bank 	AN 20 	left, pad blank
	// Destination branch	AN 20 	left, pad blank
	// Transfer media 		N 1 		1:llg, 2:rtgs, 3:fund transfer
	// Destination name	AN 30 	left, pad blank
	// Current balance		N-16, 2 	right, pad zerro
	// Transfer messsage	AN 60 	left, pad blank
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	mapPutStr(traResponseInq,bit48Rsp);

	PubChar2Long(glRecvPack.szField102, 2, &lenData);
	memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);

	PubChar2Long(glRecvPack.szField103, 2, &lenData);
	memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);

	MEM_ZERO(nama);
	MEM_ZERO(bankAsal);
	MEM_ZERO(bankTujuan);
	MEM_ZERO(norekAsal);
	
	memcpy(bankTujuan,bit48Rsp+20,20);
	//memcpy(bankTujuan,bit48Rsp+45,15);
	memcpy(nama,bit48Rsp+41,30);

	memcpy(norekAsal,bit102Rsp,28);
	memcpy(noRek,bit103Rsp,28);

	MEM_ZERO(uang);
	MEM_ZERO(uangFmt);
	memcpy(uang,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(uangFmt,uang,0,",.");
	sprintf(screenUang,"Rp. %s",uangFmt);

	ScrCls();
	PubDispStringASCII("TRANSFER BANK", DISP_LINE_CENTER|0);
	///PubDispStringASCII("TRANSFER ANTAR BANK", DISP_LINE_CENTER|0);
	///PubDispStringASCII("Bank Asal :", DISP_LINE_LEFT|1);
	///PubDispStringASCII(bankAsal, DISP_LINE_LEFT|2);
	PubDispStringASCII("No.Rek Asal :", DISP_LINE_LEFT|1);
	PubDispStringASCII(norekAsal, DISP_LINE_RIGHT|2);
	PubDispStringASCII("Bank Tujuan :", DISP_LINE_LEFT|3);
	PubDispStringASCII(bankTujuan, DISP_LINE_RIGHT|4);
	PubDispStringASCII("Rek Tujuan  :", DISP_LINE_LEFT|5);
	PubDispStringASCII(noRek, DISP_LINE_RIGHT|6);
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|0);
	PubDispStringASCII(nama, DISP_LINE_LEFT|1);
	///PubDispStringASCII("Cust Ref No :", DISP_LINE_LEFT|2);
	///PubDispStringASCII(noRef, DISP_LINE_LEFT|3);
	PubDispStringASCII("Jumlah :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenUang, DISP_LINE_RIGHT|5);
	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;
	
	//iRet = TransInit(TRANS_TYPE_INQ_TRFANTARBANK_BKPN);
	// TRANS_TYPE_TRFANTARBANK_BKPN
	iRet = TransInit(TRANS_TYPE_TRFANTARBANK_BKPN);
	if( iRet!=0 )
	{
		return iRet;
	}



	strcpy(glProcInfo.szReq48,bit48Rsp);
	strcpy(glProcInfo.szReq102,bit102Rsp);
	strcpy(glProcInfo.szReq103,bit103Rsp);


	
	////// diki add komen  setPackTransferAntarBank();
	setPackTransferAntarBankBkpn();

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

		return ERR_NO_DISP;
	}

	////// diki add komen unPackTransfer_AntarBank();
	unPackTransfer_AntarBankBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}


	/*
	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}
	*/

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    CommOnHook(FALSE);


	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}


	return 0;

	lblKO:
		return FALSE;
}




int TransSetorTunaiBkpn(void)
{
	int		ret;
	int		iRet;
	uchar 	ucRet;
	char 	bit48Rsp[lenBit48+1];
	char 	nama[30+1];
	ulong 	lenData;
	uchar	ucEntryMode=0;

	char 	bit48Req[148+1];
	char		KodeBank[148+1];
	char 	noRek[28+1];
	char		noRef[16+1];

	char 	bankAsal[20+1];
	char 	bankTujuan[20+1];
	char 	uang[12+1];
	char		uangFmt[20+1];

	char 	norekAsal[28+1];

	char screenUang[21+1];
	char screenBankAsal[21+1];
	char screenBankTujuan[21+1];
	char screenNoRek[21+1];
	char screenNoRef[21+1];

	
	char 	bit103Req[28+1];
	char 	bit102Rsp[28+1];
	char 	bit103Rsp[28+1];

	char 	bankTujuanFinal[28+1];

	uchar	ucKey;

	byte jenisRekening;
	//mapGetByte(traSumberRek, jenisRekening);

	MEM_ZERO(screenUang);
	MEM_ZERO(screenBankAsal);
	MEM_ZERO(screenBankTujuan);
	MEM_ZERO(screenNoRek);
	MEM_ZERO(screenNoRef);

	MEM_ZERO(bankTujuanFinal);


	if(MenuAtmbMerchantNasabah)
		{
		//ScrCls();
		//PubDispStringASCII("TRX NOT AVAILABLE", DISP_LINE_CENTER|2);
		}
		else {
			ScrCls();
			PubBeepErr();
			PubDispStringASCII("TRX NOT AVAILABLE", DISP_LINE_CENTER|2);
			PubWaitKey(3);
			return 0;
			}
		

	strcpy(glProcInfo.stTranLog.szTransName,"SETOR TUNAI");
			
	iRet = TransInit(TRANS_TYPE_SETOR_TUNAI_BKPN_INQ);
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

	//iRet = InputNominal();
	//iRet = InputUangBkpn();
	//if( iRet!=0 )
	//{
	//	return iRet;
	//}

	MEM_ZERO(bit48Req);
	MEM_ZERO(KodeBank);
	MEM_ZERO(noRek);
	MEM_ZERO(noRef);
	MEM_ZERO(bit103Req);

	ScrCls();
		DispTransName();
		PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
		PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
		ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
	
				////memcpy(bit48Req+3,noRek,strlen(noRek));
				//memcpy(bit103Req,noRek,strlen(noRek));
				
		memcpy(bankTujuanFinal,"00",2);
		strcat(bankTujuanFinal,noRek);
		memcpy(bit103Req,bankTujuanFinal,strlen(bankTujuanFinal));

	
	
	
		/*
		
		iRet = GetCard(ucEntryMode);
		if( iRet!=0 )
		{
			return iRet;
		}
	
		*/
		
		////// diki add komen	RUN_FUNCTION(PilihJenisRekening, lblKO);
		/*
		RUN_FUNCTION(PilihJenisRekeningBkpn, lblKO);
	
		MEM_ZERO(bit48Req);
		MEM_ZERO(KodeBank);
		MEM_ZERO(noRek);
		MEM_ZERO(noRef);
	
		MEM_ZERO(bit103Req);
		*/


		
				

		/*
		mapGetByte(traSumberRek, jenisRekening);
		switch(jenisRekening)
			{
			case 1:
				ScrCls();
				iRet = GetCard(ucEntryMode);
					if( iRet!=0 )
					{
						return iRet;
					}
		
				ScrCls();
				DispTransName();
				PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
				PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
				ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
				if( ucRet!=0 )
				{
					return ERR_USERCANCEL;
				}
	
				////memcpy(bit48Req+3,noRek,strlen(noRek));
				//memcpy(bit103Req,noRek,strlen(noRek));
				
				memcpy(bankTujuanFinal,"10",2);
				strcat(bankTujuanFinal,noRek);
	
				memcpy(bit103Req,bankTujuanFinal,strlen(bankTujuanFinal));
	
	
				
				break;
			case 2:
	
				ScrCls();
				iRet = GetCard(ucEntryMode);
					if( iRet!=0 )
					{
						return iRet;
					}
		
				
	
			while(1)
			{
				ScrCls();
				//DispTransName();
				PubDispString("LIHAT DAFTAR:", DISP_LINE_LEFT|0);
				PubDispString("KODE BANK ?", DISP_LINE_LEFT|2);
				PubDispString("NO		YES", DISP_LINE_LEFT|6);
				
			ucKey = PubWaitKey(30);
				if ((ucKey==KEYENTER) || (ucKey==KEYUP) || (ucKey==KEYDOWN))
				{
				TransInfoKodeBankBkpn();
				//continue;
				}
				else 
					{
					break;
					}
			}
	
	
				ScrCls();
				DispTransName();
				PubDispString("MASUKKAN", DISP_LINE_LEFT|2);
				PubDispString("KODE BANK :", DISP_LINE_LEFT|4);
				ucRet = PubGetString(NUM_IN, 3,10 , KodeBank, USER_OPER_TIMEOUT);
				if( ucRet!=0 )
				{
					return ERR_USERCANCEL;
				}
	
				fmtPad(KodeBank, 147, ' ');
	
				memcpy(bit48Req,KodeBank,strlen(KodeBank));
				//memcpy(bit48Req+3,"																																				 ",144);
				//memcpy(bit103Req,KodeBank,strlen(KodeBank));
	
				ScrCls();
				DispTransName();
				PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
				PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
				ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
				if( ucRet!=0 )
				{
					return ERR_USERCANCEL;
				}
	
				////memcpy(bit48Req+3,noRek,strlen(noRek));
				///memcpy(bit103Req+3,noRek,strlen(noRek));
				memcpy(bit103Req,noRek,strlen(noRek));
				break;
			}
		*/
			
	
		/*
		ScrCls();
		DispTransName();
		PubDispString("KODE BANK :", DISP_LINE_LEFT|4);
		ucRet = PubGetString(NUM_IN, 3,3 , KodeBank, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
	
		///memcpy(bit48Req,KodeBank,strlen(KodeBank));
		memcpy(bit103Req,KodeBank,strlen(KodeBank));
	
		ScrCls();
		DispTransName();
		PubDispString("ENTER REKENING", DISP_LINE_LEFT|2);
		PubDispString("TUJUAN :", DISP_LINE_LEFT|4);
		ucRet = PubGetString(NUM_IN, 10, 15, noRek, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
	
		////memcpy(bit48Req+3,noRek,strlen(noRek));
		memcpy(bit103Req+3,noRek,strlen(noRek));
		*/
	
		iRet = InputUangBkpn();
		if( iRet!=0 )
		{
			return iRet;
		}
	
		/*
		ScrCls();
		DispTransName();
		PubDispString("CUST REF NUMBER", DISP_LINE_LEFT|2);
		PubDispString("ATAU LANJUT	:", DISP_LINE_LEFT|4);
		ucRet = PubGetString(NUM_IN, 0, 16, noRef, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
	
		memcpy(bit48Req+23,noRef,strlen(noRef));
		*/
		//strcpy(glProcInfo.szReq48,bit48Req);
		strcpy(glProcInfo.szReq103,bit103Req);
	
		//GET PIN
		ScrCls();
		DispTransName();
		iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
		if( iRet!=0 )
		{
			return iRet;
		}
	
		//SET ISO 8583
		////// diki add komen  setPackInqTransferAntarBank();
		setPackInqSetorTunaiBankBkpn();
	
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
	
			displayResponse();
			return ERR_NO_DISP;
		}
	
		MEM_ZERO(bit48Rsp);
		MEM_ZERO(bit102Rsp);
		MEM_ZERO(bit103Rsp);
		//bit102Rsp
	
		// Overbook / Fund Transfer BIT 48 Respone
		// Destination bank 	AN 20	left, pad blank
		// Destination branch	AN 20	left, pad blank
		// Transfer media		N 1 		1:llg, 2:rtgs, 3:fund transfer
		// Destination name AN 30	left, pad blank
		// Current balance		N-16, 2 	right, pad zerro
		// Transfer messsage	AN 60	left, pad blank
		
		PubChar2Long(glRecvPack.szBit48, 2, &lenData);
		memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
		//mapPutStr(traResponseInq,bit48Rsp);
	
		PubChar2Long(glRecvPack.szField102, 2, &lenData);
		memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);
	
		PubChar2Long(glRecvPack.szField103, 2, &lenData);
		memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);
	
		MEM_ZERO(nama);
		MEM_ZERO(bankAsal);
		MEM_ZERO(bankTujuan);
		MEM_ZERO(norekAsal);
		
		////memcpy(bankTujuan,bit48Rsp+20,20);
		memcpy(bankTujuan,"BUKOPIN",7);
		//memcpy(bankTujuan,bit48Rsp+45,15);
		memcpy(nama,bit48Rsp+40,30);
	
		memcpy(norekAsal,bit102Rsp+2,28);
		memcpy(noRek,bit103Rsp+2,28);
	
		MEM_ZERO(uang);
		MEM_ZERO(uangFmt);
		memcpy(uang,glProcInfo.stTranLog.szAmount,10);
		fmtAmt(uangFmt,uang,0,",.");
		sprintf(screenUang,"Rp. %s",uangFmt);
	
		ScrCls();
		PubDispStringASCII("SETOR TUNAI", DISP_LINE_CENTER|0);
		///PubDispStringASCII("TRANSFER ANTAR BANK", DISP_LINE_CENTER|0);
		///PubDispStringASCII("Bank Asal :", DISP_LINE_LEFT|1);
		///PubDispStringASCII(bankAsal, DISP_LINE_LEFT|2);
		PubDispStringASCII("No.Rek Asal :", DISP_LINE_LEFT|1);
		PubDispStringASCII(norekAsal, DISP_LINE_RIGHT|2);
		PubDispStringASCII("Bank Tujuan :", DISP_LINE_LEFT|3);
		PubDispStringASCII(bankTujuan, DISP_LINE_RIGHT|4);
		PubDispStringASCII("Rek Tujuan  :", DISP_LINE_LEFT|5);
		PubDispStringASCII(noRek, DISP_LINE_RIGHT|6);
		PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);
	
		ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
		if(ucRet!=0)
			return 0;
	
		ScrCls();
		PubDispStringASCII("Nama :", DISP_LINE_LEFT|0);
		PubDispStringASCII(nama, DISP_LINE_RIGHT|1);
		///PubDispStringASCII("Cust Ref No :", DISP_LINE_LEFT|2);
		///PubDispStringASCII(noRef, DISP_LINE_LEFT|3);
		PubDispStringASCII("Jumlah :", DISP_LINE_LEFT|4);
		PubDispStringASCII(screenUang, DISP_LINE_RIGHT|5);
		PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);
	
		ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
		if(ucRet!=0)
			return 0;
		
		//iRet = TransInit(TRANS_TYPE_INQ_TRFANTARBANK_BKPN);
		// TRANS_TYPE_TRFANTARBANK_BKPN
		iRet = TransInit(TRANS_TYPE_SETOR_TUNAI_BKPN);
		if( iRet!=0 )
		{
			return iRet;
		}
	
	
	
		strcpy(glProcInfo.szReq48,bit48Rsp);
		strcpy(glProcInfo.szReq102,bit102Rsp);
		strcpy(glProcInfo.szReq103,bit103Rsp);
	
	
		
		////// diki add komen  setPackTransferAntarBank();
		setPackSetorTunaiBankBkpn();
	
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
	
			return ERR_NO_DISP;
		}
	
		////// diki add komen unPackTransfer_AntarBank();
		mapPutStr(traResponseInq,bit48Rsp);
		unPackSetorTunaiBankBkpn();
	
		//SAVE TRANSACTION
		if( ChkIfSaveLog() )
		{
			SaveMiniAtmTranLog(&glProcInfo.stTranLog);
			GetNewInvoiceNo();
		}
	
	
		/*
		//SAVE TRANSACTION
		if( ChkIfSaveLog() )
		{
			SaveTranLog(&glProcInfo.stTranLog);
			GetNewInvoiceNo();
		}
		*/
	
		//CLEAR REVERSAL
		SaveRevInfo(FALSE);
		
		//DISCONNECT FROM HOST
		CommOnHook(FALSE);
	
	
		//PRINT RECEIPT
		if( ChkIfPrnReceipt() )
		{
			DispTransName();
			CetakStrukBkpn(PRN_NORMAL);
		}
	
	
		return 0;
	
		lblKO:
			return FALSE;
}
	


int TransTarikTunaiBkpn(void)
{
	int		iRet;
	uchar	ucRet;
	char	uang[20+1];
	uchar	ucEntryMode=0;
	word	mnuItm;
	char		screenUang[21+1];

	mapGetWord(traMnuItm, mnuItm);

	MEM_ZERO(uang);
	MEM_ZERO(screenUang);


	if(MenuAtmbMerchantNasabah)
		{
		ScrCls();
		PubBeepErr();
		PubDispStringASCII("TRX NOT AVAILABLE", DISP_LINE_CENTER|2);
		PubWaitKey(3);
		return 0;
		}
		else {
			//ScrCls();
			//PubDispStringASCII("TRX NOT AVAILABLE", DISP_LINE_CENTER|2);
			}
				
					

	strcpy(glProcInfo.stTranLog.szTransName,"PENARIKAN TUNAI");
	
	iRet = TransInit(TRANS_TYPE_TARIK_TUNAI_BKPN);
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

	//iRet = InputNominal();
	iRet = InputUangBkpn();
	if( iRet!=0 )
	{
		return iRet;
	}

	//GET PIN
	ScrCls();
	DispTransName();
	//iRet = GetPIN_Manual(FALSE);	//get pin
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	App_ConvAmountTran(glProcInfo.stTranLog.szAmount, screenUang, GA_SEPARATOR);
	
	ScrCls();
	DispTransName();
	PubDispStringASCII("NO KARTU :", DISP_LINE_LEFT|2);
	PubDispStringASCII(glProcInfo.stTranLog.szPan, DISP_LINE_LEFT|3);
	PubDispStringASCII("NOMINAL :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenUang, DISP_LINE_RIGHT|5);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	strcpy(glProcInfo.szReq103,"4202004603");  // static no rek tujuan (merchant)

	
	//SET ISO 8583
	setPackTarikTunaiBkpnInq();
	
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

		return ERR_NO_DISP;
	}

	unPackTarikTunaiBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		//SaveTunaiTranLog(&glProcInfo.stTranLog);
		//GetNewInvoiceNo();
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}
		
	return 0;
}


int TransInfoSaldoBkpn(void)
{
	int		iRet;
	uchar 	ucRet;
	ulong 	lenData;
	uchar	ucEntryMode=0;
	char 	bit48Rsp[lenBit48+1];
	char 	saldo[16+1];
	char 	saldoFmt[20+1];
	char 	nama[20+1];
	char screenJumlah[22+1];

	MEM_ZERO(bit48Rsp);
	MEM_ZERO(saldo);
	MEM_ZERO(saldoFmt);
	MEM_ZERO(nama);
	MEM_ZERO(screenJumlah);

	strcpy(glProcInfo.stTranLog.szTransName,"INFORMASI SALDO");

	iRet = TransInit(TRANS_TYPE_INFO_SALDO_BKPN);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	//iRet = GetPIN_Manual(FALSE);	//get pin
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	//SET ISO 8583
	setPackInfoSaldoBkpn();

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

		return ERR_NO_DISP;
	}

	//DISCONNECT FROM HOST
 	CommOnHook(FALSE);

	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);

	/*
	memcpy(saldo,bit48Rsp,16);
	fmtAmt(saldoFmt,saldo,2,",.");
	memcpy(nama,bit48Rsp+16,30);
	sprintf(screenJumlah,"Rp. %s", saldoFmt);
	*/

	MEM_ZERO(nama);

	memcpy(nama,glRecvPack.szBit48+2, 20);
	memcpy(saldo,bit48Rsp+30,46);
	fmtAmt(saldoFmt,saldo,2,",.");
	//memcpy(nama,bit48Rsp+0,20);
	sprintf(screenJumlah,"Rp. %s", saldoFmt);

	ScrCls();
	PubDispStringASCII("INFO SALDO", DISP_LINE_CENTER|0);
	PubDispStringASCII("Nama:", DISP_LINE_LEFT|2);
	PubDispStringASCII(nama, DISP_LINE_LEFT|3);
	PubDispStringASCII("Saldo:", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenJumlah, DISP_LINE_RIGHT|5);
	PubDispStringASCII("Keluar   ", DISP_LINE_RIGHT|7);

	//ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	ucRet= PubYesNoFNUP(20);
	if(ucRet!=0)
		return 0;

/*
	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		GetNewInvoiceNo();

		CetakStruk(PRN_NORMAL);
	}
	*/

	return 0;
}




int TransUbahPinBkpn(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	char		bit48Req[16+1];
	word	mnuItm;

	strcpy(glProcInfo.stTranLog.szTransName,"UBAH PIN");

	//mapGetWord(traMnuItm, mnuItm);
	iRet = TransInit(TRANS_TYPE_UBAH_PIN_BKPN);
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
		
	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	iRet = GetPINCardServiceBkpn();
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(bit48Req);
	//CalculatePinBlock(glProcInfo.sPinBlock2);
	bin2hex(bit48Req,glProcInfo.sPinBlock2,8);

	memcpy(glProcInfo.szReq48,bit48Req,16);

	//SET ISO 8583
	setPackUbahPinBkpn();

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

		return ERR_NO_DISP;
	}

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		////if(mnuItm==mnuUbahPinCS)
			SaveCardServicesTranLog(&glProcInfo.stTranLog);
	}
		
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	GetNewInvoiceNo();
	//PRINT RECEIPT
	
	//if( ChkIfPrnReceipt() )
	//{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	//}
	


	return 0;
}






int TransPembayaranKK_Bkpn(uchar ucTransType)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;

	char		bit48Req[28+1];
	char		noKK[28+1];

	char		jumlah[12+1];
	char		jumlahFmt[20+1];
	char		screenTagihan[21+1];


	char		bit48Rsp[lenBit48+1];

	char		bit103Req[28+1];
	char 	bit102Rsp[28+1];
	char 	bit103Rsp[28+1];
	ulong 	lenData;

	char		nama[30+1];
	char		noKKres[20+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

	char		screenNama[21+1];
	char		screenNoKK[21+1];
	char		screenTagihan2[21+1];
	


		

	switch(ucTransType)
	{
		case TRANS_TYPE_KK_ANZ_BKPN_INQ:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT ANZ");
			break;
		case TRANS_TYPE_KK_BUKOPIN_BKPN_INQ:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT BUKOPIN");
			break;
		case TRANS_TYPE_KK_BNI_BKPN_INQ:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT BNI");
			break;
		case TRANS_TYPE_KK_HSBC_BKPN_INQ:
			strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN");
			strcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nama,"KARTU KREDIT HSBC");
			break;
	}
	

	iRet = TransInit(ucTransType);
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

	MEM_0x20(noKK);
	MEM_0x20(bit48Req);
	MEM_ZERO(bit103Req);


	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	switch(ucTransType)
	{
		case TRANS_TYPE_KK_ANZ_BKPN_INQ:
		case TRANS_TYPE_KK_BUKOPIN_BKPN_INQ:
		case TRANS_TYPE_KK_BNI_BKPN_INQ:
		case TRANS_TYPE_KK_HSBC_BKPN_INQ:
			PubDispString("KARTU KREDIT :", DISP_LINE_LEFT|4);
			ucRet = PubGetString(NUM_IN, 16,16 , noKK, USER_OPER_TIMEOUT);
			if( ucRet!=0 )
			{
				return ERR_USERCANCEL;
			}
			break;
	}
	
/*
	if((ucTransType==TRANS_TYPE_KKCITIBANK)||(ucTransType==TRANS_TYPE_KTACITIBANK))
	{
		bit48Req[0] = '0';
		bit48Req[1] = '0';
		bit48Req[2] = '0';
		bit48Req[3] = '0';
		memcpy(bit48Req+4,noKK,16);
	}
	else
	{
		switch(ucTransType)
		{
			case TRANS_TYPE_KKSCB:
				bit48Req[0] = '1';
				break;
			case TRANS_TYPE_KTASCB:
				bit48Req[0] = '2';
				break;
			case TRANS_TYPE_KKANZ:
				bit48Req[0] = '3';
				break;
			case TRANS_TYPE_KKHSBC:
				bit48Req[0] = '4';
				break;
			case TRANS_TYPE_KTAHSBC:
				bit48Req[0] = '5';
				break;
			case TRANS_TYPE_KKRBS:
				bit48Req[0] = '6';
				break;
			case TRANS_TYPE_KTARBS:
				bit48Req[0] = '7';
				break;
		}
		memcpy(bit48Req+1,noKK,strlen(noKK));

	}
	*/

	memcpy(bit103Req,noKK,strlen(noKK));
	memcpy(glProcInfo.szReq103,bit103Req,strlen(bit103Req));
	
	iRet =InputUangBkpn();
	if(iRet!=0)
		return iRet;

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}

	MEM_ZERO(jumlah);
	MEM_ZERO(jumlahFmt);

	memcpy(jumlah,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(jumlahFmt,jumlah,0,",.");

	sprintf(screenTagihan,"Rp.%s",jumlahFmt);
	
	ScrCls();
	switch(ucTransType)
	{
		case TRANS_TYPE_KK_ANZ_BKPN_INQ:
		case TRANS_TYPE_KK_BUKOPIN_BKPN_INQ:
		case TRANS_TYPE_KK_BNI_BKPN_INQ:
		case TRANS_TYPE_KK_HSBC_BKPN_INQ:
			PubDispStringASCII("BAYAR KARTU KREDIT", DISP_LINE_CENTER|0);
			break;
		case TRANS_TYPE_KTACITIBANK:
		case TRANS_TYPE_KTASCB:
		case TRANS_TYPE_KTAHSBC:
		case TRANS_TYPE_KTARBS:
			PubDispStringASCII("BAYAR KTA", DISP_LINE_CENTER|0);
			break;
	}
	
	switch(ucTransType)
	{
		case TRANS_TYPE_KK_ANZ_BKPN_INQ:
			PubDispStringASCII("No. KK ANZ :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KK_BUKOPIN_BKPN_INQ:
			PubDispStringASCII("No. KK BUKOPIN :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KK_BNI_BKPN_INQ:
			PubDispStringASCII("No. KK BNI :", DISP_LINE_LEFT|2);
			break;
		case TRANS_TYPE_KK_HSBC_BKPN_INQ:
			PubDispStringASCII("No. KK HSBC :", DISP_LINE_LEFT|2);
			break;
		
	}
	PubDispStringASCII(noKK, DISP_LINE_LEFT|3);
	PubDispStringASCII("Nominal :", DISP_LINE_LEFT|4);
	PubDispStringASCII(screenTagihan, DISP_LINE_LEFT|5);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	ScrCls();
	DispTransName();
	
	//SET ISO 8583
	setPackInqKKBkpn();

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
		
		return ERR_NO_DISP;
	}


	

		MEM_ZERO(bit48Rsp);
		MEM_ZERO(bit102Rsp);
		MEM_ZERO(bit103Rsp);
		//bit102Rsp
	

		PubChar2Long(glRecvPack.szBit48, 2, &lenData);
		memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
		mapPutStr(traResponseInq,bit48Rsp);
	
		PubChar2Long(glRecvPack.szField102, 2, &lenData);
		memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);
	
		PubChar2Long(glRecvPack.szField103, 2, &lenData);
		memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);


	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	MEM_ZERO(noKKres);
	MEM_ZERO(screenNama);
	MEM_ZERO(screenNoKK);
	MEM_ZERO(screenTagihan2);


	memcpy(noKKres, bit103Rsp, 16);
	
	//memcpy(nama,bit48Rsp+67,21);
	memcpy(nama,bit48Rsp+16,20);
	memcpy(tagihan,bit48Rsp+50,10);
	fmtAmt(tagihanFmt,tagihan,0,",.");

	sprintf(screenNama,"Nama:%s",nama);

	memcpy(tagihan,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(tagihanFmt,tagihan,0,",.");
	sprintf(screenTagihan2,"Rp. %s",tagihanFmt);
	

	ScrCls();
	PubDispStringASCII("PEMBAYARAN KK", DISP_LINE_CENTER|0);
	PubDispStringASCII("NO KK :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noKKres, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTagihan2, DISP_LINE_RIGHT|6);

	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;

	
	strcpy(glProcInfo.szReq48,bit48Rsp);
	strcpy(glProcInfo.szReq102,bit102Rsp);
	strcpy(glProcInfo.szReq103,bit103Rsp);



	switch(ucTransType)
		{
			case TRANS_TYPE_KK_ANZ_BKPN_INQ:
				iRet = TransInit(TRANS_TYPE_KK_ANZ_BKPN);
				if( iRet!=0 )
				{
					return iRet;
				}
				break;
			case TRANS_TYPE_KK_BUKOPIN_BKPN_INQ:
				iRet = TransInit(TRANS_TYPE_KK_BUKOPIN_BKPN);
				if( iRet!=0 )
				{
					return iRet;
				}
				break;
			case TRANS_TYPE_KK_BNI_BKPN_INQ:
				iRet = TransInit(TRANS_TYPE_KK_BNI_BKPN);
				if( iRet!=0 )
				{
					return iRet;
				}
				break;
			case TRANS_TYPE_KK_HSBC_BKPN_INQ:
				iRet = TransInit(TRANS_TYPE_KK_HSBC_BKPN);
				if( iRet!=0 )
				{
					return iRet;
				}
				break;
			
		}

	

	//SET ISO 8583
	setPackKKBkpn();

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

		return ERR_NO_DISP;
	}


	unPackPembayaran_KKBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}

	return 0;
}





int TransPembayaranTelkomBkpn(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[13+1];
	char 	bit48Rsp[lenBit48+1];	
	char		kodeArea[4+1];
	char		noTelp[15+1];
	char		nomor[20+1];
	char		nama[30+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

	char		screenNoTelp[21+1];
	char		screenNama[21+1];
	char		screenTagihan[21+1];

	byte		len;


	char		bit103Req[28+1];
	char 	bit102Rsp[28+1];
	char 	bit103Rsp[28+1];

	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN TELKOM");

	iRet = TransInit(TRANS_TYPE_INQ_TELKOM);
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

	MEM_ZERO(kodeArea);
	MEM_ZERO(nomor);
	
	MEM_ZERO(bit103Req);
	
	ScrCls();
	DispTransName();
	/*
	PubDispString("KODE AREA :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 3,4 , kodeArea, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
	memcpy(nomor,kodeArea,strlen(kodeArea));
	MEM_0x20(noTelp);
	*/

	MEM_0x20(noTelp);
	ScrCls();
	DispTransName();
	PubDispString("ID/NO. TELEPON :", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 4,15 , noTelp, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
	MEM_0x20(bit48Req);
	memcpy(nomor,noTelp,strlen(noTelp));

	//mapPutStr(traNomorTelepon, nomor);
	strcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.NoTelepon,nomor);


		memcpy(bit103Req,nomor,strlen(nomor));
		

	memcpy(glProcInfo.szReq103,bit103Req,strlen(bit103Req));

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqTelkomBkpn();

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
	
//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}


		MEM_ZERO(bit102Rsp);
		MEM_ZERO(bit103Rsp);
		
	
		PubChar2Long(glRecvPack.szField102, 2, &lenData);
		memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);
	
		PubChar2Long(glRecvPack.szField103, 2, &lenData);
		memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);




	
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	strcpy(glProcInfo.szReq102,bit102Rsp);
	strcpy(glProcInfo.szReq103,bit103Rsp);



	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	strcpy(glProcInfo.szReq48,bit48Rsp);
	//mapPutStr(traResponseInq,bit48Rsp);
	

	//memcpy(glProcInfo.stTranLog.szAmount, bit48Rsp+50, 10);
	//AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
	strcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4);
	

	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);

	MEM_ZERO(screenTagihan);
	
	//memcpy(nama,bit48Rsp+67,21);
	memcpy(nama,bit48Rsp+15,21);
	//memcpy(tagihan,bit48Rsp+50,10);
	strcpy(tagihan,glRecvPack.szBit4);
	fmtAmt(tagihanFmt,tagihan,2,",.");

	sprintf(screenNama,"Nama:%s",nama);
	sprintf(screenNoTelp,"No:%s",nomor);
	sprintf(screenTagihan,"Rp. %s",tagihanFmt);

	ScrCls();
	PubDispStringASCII("PEMBAYARAN TELKOM", DISP_LINE_CENTER|0);
	PubDispStringASCII("ID/No.Telepon :", DISP_LINE_LEFT|1);
	PubDispStringASCII(nomor, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTagihan, DISP_LINE_RIGHT|6);

	PubDispStringASCII("Keluar  Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_TELKOM);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackTelkomBkpn();

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

		return ERR_NO_DISP;
	}


	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	mapPutStr(traResponseInq,bit48Rsp);
	

	unPackPembayaran_TelkomBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}

	return 0;
}

/*
int TransPembayaranSelulerBkpnPilihProvider(void)
{
	
	char		BuffProvider[20+1];

	return 0;
}

*/



int TransPembayaranSelulerBkpn(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[13+1];
	char 	bit48Rsp[lenBit48+1];	
	char		noHP[20+1];
	char		nama[30+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

//	char		screenHP[21+1];
	char		screenTagihan[21+1];

	char		bit103Req[28+1];
		char	bit102Rsp[28+1];
		char	bit103Rsp[28+1];


	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN SELULAR");

	iRet = TransInit(TRANS_TYPE_INQ_KARTUMATRIX);
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

	MEM_ZERO(noHP);
	MEM_ZERO(bit103Req);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	PubDispString("TELEPON :", DISP_LINE_LEFT|4);	
	ucRet = PubGetString(NUM_IN, 4,13 , noHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP, noHP, 15);
	memcpy(bit103Req,noHP,strlen(noHP));
	//memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	memcpy(glProcInfo.szReq103,bit103Req,strlen(bit103Req));

	

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqSelularBkpn();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}


	MEM_ZERO(bit102Rsp);
		MEM_ZERO(bit103Rsp);

	
		PubChar2Long(glRecvPack.szField102, 2, &lenData);
		memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);
	
		PubChar2Long(glRecvPack.szField103, 2, &lenData);
		memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);
	
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	strcpy(glProcInfo.szReq102,bit102Rsp);
	strcpy(glProcInfo.szReq103,bit103Rsp);
	

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	strcpy(glProcInfo.szReq48,bit48Rsp);
	//mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	
	memcpy(nama,bit48Rsp+15,21);
	//memcpy(tagihan,bit48Rsp+50,10);
	strcpy(tagihan,glRecvPack.szBit4);

	//memcpy(glProcInfo.stTranLog.szAmount, bit48Rsp+50, 12);
	strcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4);
	//memcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4, 12);
	fmtAmt(tagihanFmt,tagihan,2,",.");

	sprintf(screenTagihan,"Rp. %s",tagihanFmt);

	ScrCls();
	PubDispStringASCII("PEMBAYARAN SELULAR", DISP_LINE_CENTER|0);
	PubDispStringASCII("No. Telepon :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noHP, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTagihan, DISP_LINE_RIGHT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_KARTUMATRIX);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackSelularBkpn();

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
	if (( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0)  && (memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)!=0 ))
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	mapPutStr(traResponseInq,bit48Rsp);
	

	unPackPembayaran_SelularBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}

	return 0;
}




int TransPembayaranPdamPalyjaBkpn(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[13+1];
	char 	bit48Rsp[lenBit48+1];	
	char		noHP[20+1];
	char		nama[30+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

//	char		screenHP[21+1];
	char		screenTagihan[21+1];

	char		bit103Req[28+1];
		char	bit102Rsp[28+1];
		char	bit103Rsp[28+1];

		char		noID[20+1];


	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN AIR PALYJA");

	iRet = TransInit(TRANS_TYPE_PDAM_PALYJA_BKPN_INQ);
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

	MEM_ZERO(noHP);
	MEM_ZERO(noID);
	MEM_ZERO(bit48Req);
	MEM_ZERO(bit103Req);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	PubDispString("ID PELANGGAN :", DISP_LINE_LEFT|4);	
	ucRet = PubGetString(NUM_IN, 4,16 , noHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(noID,"4002",4);
	strcat(noID,noHP);

	MEM_ZERO(bit103Req);
	
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP, noHP, 13);
	memcpy(bit103Req,noID,strlen(noID));
	//memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	//memcpy(glProcInfo.szReq103,bit103Req,strlen(bit103Req));
	strcpy(glProcInfo.szReq103,bit103Req);
	
	

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqPdamBkpn();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}


	MEM_ZERO(bit102Rsp);
		MEM_ZERO(bit103Rsp);
	
		PubChar2Long(glRecvPack.szField102, 2, &lenData);
		memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);
	
		PubChar2Long(glRecvPack.szField103, 2, &lenData);
		memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);
	
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	strcpy(glProcInfo.szReq102,bit102Rsp);
	strcpy(glProcInfo.szReq103,bit103Rsp);
	

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	strcpy(glProcInfo.szReq48,bit48Rsp);
	//mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	
	memcpy(nama,bit48Rsp+20,21);
	//memcpy(tagihan,bit48Rsp+55,10);
	strcpy(tagihan,glRecvPack.szBit4);

	//memcpy(glProcInfo.stTranLog.szAmount, bit48Rsp+57, 12);
	strcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4);
	fmtAmt(tagihanFmt,tagihan,2,",.");

	sprintf(screenTagihan,"Rp. %s",tagihanFmt);
	

	ScrCls();
	PubDispStringASCII("PEMBAYARAN PALYJA", DISP_LINE_CENTER|0);
	PubDispStringASCII("No. ID :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noHP, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTagihan, DISP_LINE_LEFT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_PDAM_PALYJA_BKPN);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackPdamBkpn();

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
	if (( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0)  && (memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)!=0 ))
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}


	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	mapPutStr(traResponseInq,bit48Rsp);

	unPackPembayaran_PdamBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}

	return 0;
}




int TransPembayaranPdamAetraBkpn(void)
{
	int		iRet;
	uchar 	ucRet;
	uchar	ucEntryMode=0;
	ulong 	lenData;

	char		bit48Req[13+1];
	char 	bit48Rsp[lenBit48+1];	
	char		noHP[16+1];
	char		nama[30+1];
	char		tagihan[12+1];
	char		tagihanFmt[20+1];

//	char		screenHP[21+1];
	char		screenTagihan[21+1];

	char		bit103Req[28+1];
		char	bit102Rsp[28+1];
		char	bit103Rsp[28+1];

		char		noID[20+1];


	strcpy(glProcInfo.stTranLog.szTransName,"PEMBAYARAN AIR AETRA");

	iRet = TransInit(TRANS_TYPE_PDAM_AETRA_BKPN_INQ);
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

	MEM_ZERO(noHP);
	MEM_ZERO(noID);
	MEM_0x20(bit48Req);
	
	ScrCls();
	DispTransName();
	//PubDispString("ENTER NOMOR", DISP_LINE_LEFT|2);
	PubDispString("MASUKKAN NOMOR", DISP_LINE_LEFT|2);
	PubDispString("ID PELANGGAN :", DISP_LINE_LEFT|4);	
	ucRet = PubGetString(NUM_IN, 4,16 , noHP, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	memcpy(noID,"4001",4);
	strcat(noID,noHP);


	MEM_ZERO(bit103Req);
	
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP, noHP, 13);
	memcpy(bit103Req,noID,strlen(noID));
	//memcpy(glProcInfo.szReq48,bit48Req,strlen(bit48Req));
	//memcpy(glProcInfo.szReq103,bit103Req,strlen(bit103Req));
	strcpy(glProcInfo.szReq103,bit103Req);
	

	//GET PIN
	ScrCls();
	DispTransName();
	iRet = GetPIN_Manual_MiniAtmBkpn(FALSE);	//get pin
	if( iRet!=0 )
	{
		return iRet;
	}
	
	//SET ISO 8583
	setPackInqPdamBkpn();

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
	

//	displayResponse();

	//CHECK RESPONSE FROM HOST
	if ( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0 )
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		displayResponse();
		return ERR_NO_DISP;
	}


		MEM_ZERO(bit102Rsp);
		MEM_ZERO(bit103Rsp);
	
		PubChar2Long(glRecvPack.szField102, 2, &lenData);
		memcpy(bit102Rsp, glRecvPack.szField102+2, lenData);
	
		PubChar2Long(glRecvPack.szField103, 2, &lenData);
		memcpy(bit103Rsp, glRecvPack.szField103+2, lenData);
	
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	strcpy(glProcInfo.szReq102,bit102Rsp);
	strcpy(glProcInfo.szReq103,bit103Rsp);
	

	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	strcpy(glProcInfo.szReq48,bit48Rsp);
	//mapPutStr(traResponseInq,bit48Rsp);

	MEM_ZERO(nama);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	
	memcpy(nama,bit48Rsp+20,21);
	//memcpy(tagihan,bit48Rsp+55,10);
	strcpy(tagihan,glRecvPack.szBit4);

	//memcpy(glProcInfo.stTranLog.szAmount, bit48Rsp+57, 12);
	strcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4);
	fmtAmt(tagihanFmt,tagihan,2,",.");

	sprintf(screenTagihan,"Rp. %s",tagihanFmt);

	ScrCls();
	PubDispStringASCII("PEMBAYARAN AETRA", DISP_LINE_CENTER|0);
	PubDispStringASCII("No. ID :", DISP_LINE_LEFT|1);
	PubDispStringASCII(noHP, DISP_LINE_LEFT|2);
	PubDispStringASCII("Nama :", DISP_LINE_LEFT|3);
	PubDispStringASCII(nama, DISP_LINE_LEFT|4);
	PubDispStringASCII("Tagihan :", DISP_LINE_LEFT|5);
	PubDispStringASCII(screenTagihan, DISP_LINE_LEFT|6);
	PubDispStringASCII("Keluar   Lanjut", DISP_LINE_RIGHT|7);

	ucRet= PubYesNoFNUP(USER_INPUT_TIMEOUT);
	if(ucRet!=0)
		return 0;


	iRet = TransInit(TRANS_TYPE_PDAM_AETRA_BKPN);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	setPackPdamBkpn();

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
	if (( memcmp(glProcInfo.stTranLog.szRspCode, "00", 2)!=0)  && (memcmp(glProcInfo.stTranLog.szRspCode, "68", 2)!=0 ))
	{
		CommOnHook(FALSE);
		SaveRevInfo(FALSE);

		return ERR_NO_DISP;
	}


	MEM_ZERO(bit48Rsp);
	
	PubChar2Long(glRecvPack.szBit48, 2, &lenData);
	memcpy(bit48Rsp, glRecvPack.szBit48+2, lenData);
	//strcpy(glProcInfo.szReq48,bit48Rsp);
	mapPutStr(traResponseInq,bit48Rsp);
	

	unPackPembayaran_PdamBkpn();

	//SAVE TRANSACTION
	if( ChkIfSaveLog() )
	{
		SaveMiniAtmTranLog(&glProcInfo.stTranLog);
		GetNewInvoiceNo();
	}

	//CLEAR REVERSAL
	SaveRevInfo(FALSE);
	
	//DISCONNECT FROM HOST
    	CommOnHook(FALSE);

	//PRINT RECEIPT
	if( ChkIfPrnReceipt() )
	{
		DispTransName();
		CetakStrukBkpn(PRN_NORMAL);
	}

	return 0;
}



int TransInfoKodeBankBkpnPilih(char *dst, const char *src)
	
{
	uchar 	ucRet;
	char	nama[20+1];

	MEM_ZERO(nama);

	//strcpy(nama, "B R I");
	//if (strcmp(pszIsoAmountStr, "0")==0)


	if (strcmp(src, "002")==0)
		{
			strcpy(dst, "BRI");
		}
	else if (strcmp(src, "008")==0)
		{
			strcpy(dst, "MANDIRI");
		}
	else if (strcmp(src, "009")==0)
		{
			strcpy(dst, "BNI");
		}
	else if (strcmp(src, "011")==0)
		{
			strcpy(dst, "DANAMON");
		}
	else if (strcmp(src, "014")==0)
		{
			strcpy(dst, "BCA");
		}
	else if (strcmp(src, "441")==0)
		{
			strcpy(dst, "BUKOPIN");
		}
	else if (strcmp(src, "008")==0)
		{
			strcpy(dst, "MANDIRI");
		}
	else if (strcmp(src, "013")==0)
		{
			strcpy(dst, "PERMATA");
		}
	else if (strcmp(src, "016")==0)
		{
			strcpy(dst, "BII");
		}
	else if (strcmp(src, "019")==0)
		{
			strcpy(dst, "PANIN");
		}
	else if (strcmp(src, "022")==0)
		{
			strcpy(dst, "CIMB NIAGA");
		}
	else if (strcmp(src, "023")==0)
		{
			strcpy(dst, "BANK UOB");
		}
	else if (strcmp(src, "028")==0)
		{
			strcpy(dst, "OCBC NISC");
		}
	else if (strcmp(src, "031")==0)
		{
			strcpy(dst, "CITI BANK");
		}


	else if (strcmp(src, "036")==0)
		{
			strcpy(dst, "WINDU");
		}
	else if (strcmp(src, "037")==0)
		{
			strcpy(dst, "ARTA GRAHA");
		}
	else if (strcmp(src, "041")==0)
		{
			strcpy(dst, "H S B C");
		}
	else if (strcmp(src, "042")==0)
		{
			strcpy(dst, "TOKYO");
		}
	else if (strcmp(src, "046")==0)
		{
			strcpy(dst, "BANK DBS");
		}
	else if (strcmp(src, "095")==0)
		{
			strcpy(dst, "MUTIARA");
		}
	else if (strcmp(src, "097")==0)
		{
			strcpy(dst, "MAYAPADA");
		}
	else if (strcmp(src, "110")==0)
		{
			strcpy(dst, "B J B");
		}
	else if (strcmp(src, "111")==0)
		{
			strcpy(dst, "D K I");
		}
	else if (strcmp(src, "145")==0)
		{
			strcpy(dst, "BNP");
		}
	else if (strcmp(src, "147")==0)
		{
			strcpy(dst, "MUAMALAT");
		}
	else if (strcmp(src, "151")==0)
		{
			strcpy(dst, "MESTIKA");
		}
	else if (strcmp(src, "153")==0)
		{
			strcpy(dst, "SINAR MAS");
		}
	else if (strcmp(src, "157")==0)
		{
			strcpy(dst, "MASPION");
		}
	else if (strcmp(src, "200")==0)
		{
			strcpy(dst, "B T N");
		}
	else if (strcmp(src, "213")==0)
		{
			strcpy(dst, "B T P N");
		}
	else if (strcmp(src, "422")==0)
		{
			strcpy(dst, "BRI SYARIAH");
		}
	else if (strcmp(src, "426")==0)
		{
			strcpy(dst, "M E G A");
		}
	else if (strcmp(src, "451")==0)
		{
			strcpy(dst, "B S M");
		}
	else if (strcmp(src, "517")==0)
		{
			strcpy(dst, "PANIN SYARIAH");
		}
	else if (strcmp(src, "553")==0)
		{
			strcpy(dst, "MAYORA");
		}
	else if (strcmp(src, "556")==0)
		{
			strcpy(dst, "VICTORIA");
		}
	else if (strcmp(src, "688")==0)
		{
			strcpy(dst, "B P R KS");
		}
	else if (strcmp(src, "061")==0)
		{
			strcpy(dst, "ANZ");
		}


	
	else
		{
			strcpy(dst, "BANK LAINNYA");
		}
	


	//ScrPrint(0, 0, ASCII, "%s", "002 B R I");
	//ScrPrint(0, 1, ASCII, "%s", "008 MANDIRI");
	//ScrPrint(0, 2, ASCII, "%s", "009 B N I");
	//ScrPrint(0, 3, ASCII, "%s", "011 DANAMON");
	//ScrPrint(0, 4, ASCII, "%s", "013 PERMATA");
	//ScrPrint(0, 5, ASCII, "%s", "014 B C A");
	//ScrPrint(0, 0, ASCII, "%s", "016 B I I");
	//ScrPrint(0, 1, ASCII, "%s", "019 PANIN");
	//ScrPrint(0, 2, ASCII, "%s", "022 CIMB NIAGA");
	//ScrPrint(0, 3, ASCII, "%s", "023 BANK UOB");
	//ScrPrint(0, 4, ASCII, "%s", "028 OCBC NISC");
	//ScrPrint(0, 5, ASCII, "%s", "031 CITI BANK");
	//ScrPrint(0, 0, ASCII, "%s", "036 WINDU");
	//ScrPrint(0, 1, ASCII, "%s", "037 ARTA GRAHA");
	//ScrPrint(0, 2, ASCII, "%s", "041 H S B C");
	//ScrPrint(0, 3, ASCII, "%s", "042 TOKYO");
	//ScrPrint(0, 4, ASCII, "%s", "046 BANK DBS");
	//ScrPrint(0, 5, ASCII, "%s", "050 STANCHRT");
	//ScrPrint(0, 0, ASCII, "%s", "052 R B S");
	//ScrPrint(0, 1, ASCII, "%s", "054 CAPITAL");
	//ScrPrint(0, 2, ASCII, "%s", "061 ANZ INDONESIA");
	//ScrPrint(0, 3, ASCII, "%s", "068 WORI INDONESIA");
	//ScrPrint(0, 4, ASCII, "%s", "076 BUMI ARTHA");
	//ScrPrint(0, 5, ASCII, "%s", "087 EKONOMI");
	//ScrPrint(0, 0, ASCII, "%s", "088 ANTAR DAERAH");
	//ScrPrint(0, 1, ASCII, "%s", "089 RABO BANK");
	//ScrPrint(0, 2, ASCII, "%s", "095 MUTIARA");
	//ScrPrint(0, 3, ASCII, "%s", "097 MAYAPADA");
	//ScrPrint(0, 4, ASCII, "%s", "110 B J B");
	//ScrPrint(0, 5, ASCII, "%s", "111 D K I");
	//ScrPrint(0, 0, ASCII, "%s", "112 D I Y");
	//ScrPrint(0, 1, ASCII, "%s", "113 JATENG");
	//ScrPrint(0, 2, ASCII, "%s", "114 JATIM");
	//ScrPrint(0, 3, ASCII, "%s", "115 JAMBI");
	//ScrPrint(0, 4, ASCII, "%s", "116 ACEH");
	//ScrPrint(0, 5, ASCII, "%s", "117 SUMUT");
	//ScrPrint(0, 0, ASCII, "%s", "118 NAGARI");
	//ScrPrint(0, 1, ASCII, "%s", "119 RIAU");
	/*
	ScrPrint(0, 2, ASCII, "%s", "120 SUMSEL");
	ScrPrint(0, 3, ASCII, "%s", "121 LAMPUNG");
	ScrPrint(0, 4, ASCII, "%s", "122 KALSEL");
	ScrPrint(0, 5, ASCII, "%s", "123 KALBAR");
	ScrPrint(0, 0, ASCII, "%s", "124 KALTIM");
	ScrPrint(0, 1, ASCII, "%s", "125 KALTENG");
	ScrPrint(0, 2, ASCII, "%s", "126 SULSEL");
	ScrPrint(0, 3, ASCII, "%s", "127 SULUT");
	ScrPrint(0, 4, ASCII, "%s", "128 N T B");
	ScrPrint(0, 5, ASCII, "%s", "129 BALI");
	ScrPrint(0, 0, ASCII, "%s", "130 N T T");
	ScrPrint(0, 1, ASCII, "%s", "131 MALUKU");
	ScrPrint(0, 2, ASCII, "%s", "132 PAPUA");
	ScrPrint(0, 3, ASCII, "%s", "133 BENGKULU");
	ScrPrint(0, 4, ASCII, "%s", "134 SULTENG");
	ScrPrint(0, 5, ASCII, "%s", "135 SULTRA");
	ScrPrint(0, 0, ASCII, "%s", "145 BNP");
	ScrPrint(0, 1, ASCII, "%s", "146 SWADESI");
	ScrPrint(0, 2, ASCII, "%s", "147 MUAMALAT");
	ScrPrint(0, 3, ASCII, "%s", "151 MESTIKA");
	ScrPrint(0, 4, ASCII, "%s", "153 SINAR MAS");
	ScrPrint(0, 5, ASCII, "%s", "157 MASPION");
	ScrPrint(0, 0, ASCII, "%s", "161 GANESHA");
	ScrPrint(0, 1, ASCII, "%s", "167 KESAWAN");
	ScrPrint(0, 2, ASCII, "%s", "200 B T N");
	ScrPrint(0, 3, ASCII, "%s", "212 B SAUDARA");
	ScrPrint(0, 4, ASCII, "%s", "213 B T P N");
	ScrPrint(0, 5, ASCII, "%s", "422 BRI SYARIAH");
	ScrPrint(0, 0, ASCII, "%s", "425 JABARSYA");
	ScrPrint(0, 1, ASCII, "%s", "426 M E G A");
	ScrPrint(0, 2, ASCII, "%s", "441 BUKOPIN");
	ScrPrint(0, 3, ASCII, "%s", "451 B S M");
	ScrPrint(0, 4, ASCII, "%s", "472 JASA JAKARTA");
	ScrPrint(0, 5, ASCII, "%s", "484 BANK HANA");
	ScrPrint(0, 0, ASCII, "%s", "485 BUMI PUTERA");
	ScrPrint(0, 1, ASCII, "%s", "494 BRI AGRO");
	ScrPrint(0, 2, ASCII, "%s", "498 INDOMONEX");
	ScrPrint(0, 3, ASCII, "%s", "501 ROYAL");
	ScrPrint(0, 4, ASCII, "%s", "503 NOBOU BANK");
	ScrPrint(0, 5, ASCII, "%s", "506 B S M I");
	ScrPrint(0, 0, ASCII, "%s", "513 I N A PERDANA");
	ScrPrint(0, 1, ASCII, "%s", "517 PANIN SYARIAH");
	ScrPrint(0, 2, ASCII, "%s", "523 SAHABAT");
	ScrPrint(0, 3, ASCII, "%s", "535 B K E");
	ScrPrint(0, 4, ASCII, "%s", "542 ARTOS");
	ScrPrint(0, 5, ASCII, "%s", "553 MAYORA");
	ScrPrint(0, 0, ASCII, "%s", "555 INDEX");
	ScrPrint(0, 1, ASCII, "%s", "556 VICTORIA");
	ScrPrint(0, 2, ASCII, "%s", "558 PUNDI");
	ScrPrint(0, 3, ASCII, "%s", "600 B P R/LSB");
	ScrPrint(0, 4, ASCII, "%s", "688 B P R KS");
	ScrPrint(0, 5, ASCII, "%s", "699 BANK EKA");
	ScrPrint(0, 0, ASCII, "%s", "950 COMMBANK");
	ScrPrint(0, 1, ASCII, "%s", "987 LAINNYA");
	*/
	
	return 0;
	
}



int TransAtmbMerchantNasabahMenuTrue(void)
{
	//int		iRet;
	MenuAtmbMerchantNasabah = TRUE;
	return 0;
}

int TransAtmbMerchantNasabahMenuFalse(void)
{
	//int		iRet;
	MenuAtmbMerchantNasabah = FALSE;
	return 0;
}


int TransInfoKodeBankBkpn2(void)
{
	uchar 	ucRet;

lbl_1:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "014 BCA");
	ScrPrint(0, 1, ASCII, "%s", "008 MANDIRI");
	ScrPrint(0, 2, ASCII, "%s", "002 BRI");
	ScrPrint(0, 3, ASCII, "%s", "009 BNI");
	ScrPrint(0, 4, ASCII, "%s", "061 ANZ");
	ScrPrint(0, 5, ASCII, "%s", "022 CIMB NIAGA");
	PubDispStringASCII(" v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet == KEYUP )
		goto lbl_1;

lbl_2:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "451 BSM");
	ScrPrint(0, 1, ASCII, "%s", "016 MAYBANK");
	ScrPrint(0, 2, ASCII, "%s", "013 PERMATA");
	ScrPrint(0, 3, ASCII, "%s", "011 DANAMON");
	ScrPrint(0, 4, ASCII, "%s", "426 BANK MEGA");
	ScrPrint(0, 5, ASCII, "%s", "147 MUAMALAT");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_1;

lbl_3:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "200 BTN");
	ScrPrint(0, 1, ASCII, "%s", "019 BANK PANIN");
	ScrPrint(0, 2, ASCII, "%s", "945 BANK AGRIS");
	ScrPrint(0, 3, ASCII, "%s", "770 ALTOPAY");
	ScrPrint(0, 4, ASCII, "%s", "037 ARTA GRAHA");
	ScrPrint(0, 5, ASCII, "%s", "131 BANK MALUKU");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_2;

lbl_4:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "600 BPR SUPRA");
	ScrPrint(0, 1, ASCII, "%s", "987 ATMB PLUS");
	ScrPrint(0, 2, ASCII, "%s", "542 BANK ARTOS");
	ScrPrint(0, 3, ASCII, "%s", "116 BANK ACEH");
	ScrPrint(0, 4, ASCII, "%s", "088 BANK ANDA");
	ScrPrint(0, 5, ASCII, "%s", "485 BANK MNC");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_3;

lbl_5:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "129 BANK BPD BALI");
	ScrPrint(0, 1, ASCII, "%s", "112 BANK BPD DIY");
	ScrPrint(0, 2, ASCII, "%s", "122 BANK BPD KALSEL");
	ScrPrint(0, 3, ASCII, "%s", "124 BANK BPD KALTIM");
	ScrPrint(0, 4, ASCII, "%s", "213 BANK BTPN");
	ScrPrint(0, 5, ASCII, "%s", "076 BANK BUMI ARTA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_4;

lbl_6:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "054 BANK CAPITAL");
	ScrPrint(0, 1, ASCII, "%s", "950 COMMONWEALTH");
	ScrPrint(0, 2, ASCII, "%s", "111 BANK DKI");
	ScrPrint(0, 3, ASCII, "%s", "699 BPR EKA BUMI ARTHA");
	ScrPrint(0, 4, ASCII, "%s", "087 EKONOMI RAHARJA");
	ScrPrint(0, 5, ASCII, "%s", "484 BANK HANA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_5;

lbl_7:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "041 HSBC");
	ScrPrint(0, 1, ASCII, "%s", "513 INA PERDANA");
	ScrPrint(0, 2, ASCII, "%s", "425 JABAR BANTEN SYARIAH");
	ScrPrint(0, 3, ASCII, "%s", "472 JASA JAKARTA");
	ScrPrint(0, 4, ASCII, "%s", "113 BANK JATENG");
	ScrPrint(0, 5, ASCII, "%s", "114 BANK JATIM");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_6;

lbl_8:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "123 BANK KALBAR");
	ScrPrint(0, 1, ASCII, "%s", "125 BANK KALTENG");
	ScrPrint(0, 2, ASCII, "%s", "121 BANK LAMPUNG");
	ScrPrint(0, 3, ASCII, "%s", "097 MAYAPADA INTERNASIONAL");
	ScrPrint(0, 4, ASCII, "%s", "553 MAYORA");
	ScrPrint(0, 5, ASCII, "%s", "506 MEGA SYARIAH");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_7;

lbl_9:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "118 BANK NAGARI");
	ScrPrint(0, 1, ASCII, "%s", "128 BANK NTB");
	ScrPrint(0, 2, ASCII, "%s", "130 BANK NTT");
	ScrPrint(0, 3, ASCII, "%s", "145 NUSANTARA PARAHYANGAN");
	ScrPrint(0, 4, ASCII, "%s", "028 OCBC NISP");
	ScrPrint(0, 5, ASCII, "%s", "132 BANK PAPUA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_8;

lbl_10:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "558 PUNDI INDONESIA");
	ScrPrint(0, 1, ASCII, "%s", "119 RIAU KEPRI");
	ScrPrint(0, 2, ASCII, "%s", "212 WOORI SAUDARA IND");
	ScrPrint(0, 3, ASCII, "%s", "153 SINARMAS");
	ScrPrint(0, 4, ASCII, "%s", "126 BANK SULSELBAR");
	ScrPrint(0, 5, ASCII, "%s", "127 BANK SULUT");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_9;

lbl_11:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "120 SUMSEL BABEL");
	ScrPrint(0, 1, ASCII, "%s", "117 BANK SUMUT");
	ScrPrint(0, 2, ASCII, "%s", "521 SYARIAH BUKOPIN");
	ScrPrint(0, 3, ASCII, "%s", "023 UOB INDONESIA");
	ScrPrint(0, 4, ASCII, "%s", "405 VICYORIA SYARIAH");
	ScrPrint(0, 5, ASCII, "%s", "536 BCA SYARIAH");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_10;

lbl_12:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "113 BANK BENGKULU");
	ScrPrint(0, 1, ASCII, "%s", "110 BJB");
	ScrPrint(0, 2, ASCII, "%s", "535 BKE");
	ScrPrint(0, 3, ASCII, "%s", "095 MUTIARA");
	ScrPrint(0, 4, ASCII, "%s", "145 BNP");
	ScrPrint(0, 5, ASCII, "%s", "069 BANK OF CHINA");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_11;

lbl_13:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "146 BANK OF INDIA");
	ScrPrint(0, 1, ASCII, "%s", "688 BPR KS");
	ScrPrint(0, 2, ASCII, "%s", "494 BRI AGRO");
	ScrPrint(0, 3, ASCII, "%s", "422 BRIS");
	ScrPrint(0, 4, ASCII, "%s", "506 BSMI");
	ScrPrint(0, 5, ASCII, "%s", "031 CITIBANK");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_12;

lbl_14:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "949 CTBC INDO");
	ScrPrint(0, 1, ASCII, "%s", "046 DBS");
	ScrPrint(0, 2, ASCII, "%s", "789 DOMPETKU");
	ScrPrint(0, 3, ASCII, "%s", "161 GANESHA");
	ScrPrint(0, 4, ASCII, "%s", "567 HARDA");
	ScrPrint(0, 5, ASCII, "%s", "555 INDEX SELINDO");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_13;
lbl_15:
	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "115 BANK JAMBI");
	ScrPrint(0, 1, ASCII, "%s", "157 MASPION");
	ScrPrint(0, 2, ASCII, "%s", "151 MESTIKA");
	ScrPrint(0, 3, ASCII, "%s", "503 NATIONAL NOBU");
	ScrPrint(0, 4, ASCII, "%s", "517 PANIN SYARIAH");
	ScrPrint(0, 5, ASCII, "%s", "559 CENTRATAMA NASIONAL");
	PubDispStringASCII("^v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		 goto lbl_14;

lbl_16:
		ScrCls();
		ScrPrint(0, 0, ASCII, "%s", "167 QNB KESAWAN");
		ScrPrint(0, 1, ASCII, "%s", "089 RABOBANK");
		ScrPrint(0, 2, ASCII, "%s", "501 ROYAL IND");
		ScrPrint(0, 3, ASCII, "%s", "523 SAMPOERNA");
		ScrPrint(0, 4, ASCII, "%s", "498 SBI IND");
		ScrPrint(0, 5, ASCII, "%s", "775 SGODPAY");
		PubDispStringASCII("^v", DISP_LINE_CENTER|7);
		ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
		if(ucRet == NOKEY)
			return 0;
		else if(ucRet==KEYUP)
			 goto lbl_15;

lbl_17:
		ScrCls();
		ScrPrint(0, 0, ASCII, "%s", "050 STANDARD CHARTERED");
		ScrPrint(0, 1, ASCII, "%s", "134 SULTENG");
		ScrPrint(0, 2, ASCII, "%s", "135 SULTRA");
		ScrPrint(0, 3, ASCII, "%s", "042 BANK OF TOKYO");
		ScrPrint(0, 4, ASCII, "%s", "566 VICTORIA IND");
		ScrPrint(0, 5, ASCII, "%s", "036 WINDU KENTJANA");
		PubDispStringASCII("^v", DISP_LINE_CENTER|7);
		ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
		if(ucRet == NOKEY)
			return 0;
		else if(ucRet==KEYUP)
			 goto lbl_16;

lbl_18:
		ScrCls();
		ScrPrint(0, 0, ASCII, "%s", "911 TELKOMSEL TCASH");
		ScrPrint(0, 1, ASCII, "%s", "777 FINNET");
		ScrPrint(0, 2, ASCII, "%s", "526 BANK DINAR");
		ScrPrint(0, 3, ASCII, "%s", "547 BTPNS");
		ScrPrint(0, 4, ASCII, "%s", "899 DOKU");
		ScrPrint(0, 5, ASCII, "%s", "441 BANK BUKOPIN");
		PubDispStringASCII("^v", DISP_LINE_CENTER|7);
		ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
		if(ucRet == NOKEY)
			return 0;
		else if(ucRet==KEYUP)
			 goto lbl_17;


	ScrCls();
	ScrPrint(0, 0, ASCII, "%s", "548 MULTIARTA SENTOSA");
	ScrPrint(0, 1, ASCII, "%s", "490 YUDHA BHAKTI");
	ScrPrint(0, 2, ASCII, "%s", "164 ICBC IND");
	PubDispStringASCII("^ ", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return 0;
	else if(ucRet==KEYUP)
		goto lbl_18;
	
	return 0;
	
}




int TransInitKartuBkpn(void)
{
	int		iRet;
	uchar	ucEntryMode=0;
	char		bit48Req[16+1];
	word	mnuItm;
	uchar 	ucRet;

	strcpy(glProcInfo.stTranLog.szTransName,"INIT KARTU");


	ucEntryMode = CARD_SWIPED|SKIP_CHECK_ICC;
	
	if(!ucEntryMode)
	{
		return ERR_NO_DISP;
	}
	
	iRet = GetCardInitKartuBkpn(ucEntryMode);
	if( iRet!=0 )
	{
		return iRet;
	}
		
	ScrCls();
	PubDispStringASCII("INIT KARTU", DISP_LINE_CENTER|0);
	PubDispStringASCII("SUKSES", DISP_LINE_LEFT|3);
	ucRet= PubYesNoFNUP(10);
	if(ucRet!=0)
		return 0;


	return 0;
}









