#include "global.h"
uchar unPackTransaction(void)
{
	switch(glProcInfo.stTranLog.ucTranType)
	{
		case TRANS_TYPE_SALE_REDEEMPTION:
			unPackSaleRedeemption();
			break;
		case TRANS_TYPE_CICILAN_BRING:
			unPackCicilanBring();
			break;
		case TRANS_TYPE_CICILAN_BUKOPIN:
			unPackCicilanBukopin();
			break;
		default:
			return TRUE;
			break;
	}
	return TRUE;
}

uchar unPackSaleRedeemption(void)
{
	uchar buffer[999];
	uchar bufPoint[12];
	ulong len=0;

	MEM_ZERO(buffer);
	

	len = PubChar2Long(glRecvPack.szBit63, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit63+2, len);

	memcpy(glProcInfo.stTranLog.BRI.SaleRedeemption.SaleAmt, buffer, 12);
	memcpy(glProcInfo.stTranLog.BRI.SaleRedeemption.RedeemAmt, buffer+12, 12);
	memcpy(glProcInfo.stTranLog.BRI.SaleRedeemption.NetSaleAmt, buffer+24, 12);

	//memcpy(glProcInfo.stTranLog.BRI.SaleRedeemption.Buff63, buffer, len);

	MEM_ZERO(bufPoint);
	memcpy(	bufPoint, buffer+36, 9);
	glProcInfo.stTranLog.BRI.SaleRedeemption.PointRedeem=atol(bufPoint);
	MEM_ZERO(bufPoint);
	memcpy(	bufPoint, buffer+48, 9);
	glProcInfo.stTranLog.BRI.SaleRedeemption.PointAward=atol(bufPoint);
	MEM_ZERO(bufPoint);
	memcpy(	bufPoint, buffer+57, 10);
	glProcInfo.stTranLog.BRI.SaleRedeemption.PointBalance=atol(bufPoint);


	//memcpy(glProcInfo.stTranLog.szAmount, buffer+24, 12);
	memcpy(glProcInfo.stTranLog.szAmount, buffer, 12);	

	return TRUE;
}

uchar unPackCicilanBring(void)
{
	uchar buffer[999];

	ulong len=0;

	MEM_ZERO(buffer);
	

	len = PubChar2Long(glRecvPack.szBit61, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit61+2, len);

	memset(glProcInfo.stTranLog.BRI.CicilanBring.PokokAmt, '0', 12);
	memset(glProcInfo.stTranLog.BRI.CicilanBring.BulanAmt, '0', 12);
	memset(glProcInfo.stTranLog.szAmount, '0', 12);

	memcpy(glProcInfo.stTranLog.BRI.CicilanBring.Term, buffer, 2);
	memcpy(glProcInfo.stTranLog.BRI.CicilanBring.Bulan, buffer+3, 2);
	memcpy(glProcInfo.stTranLog.BRI.CicilanBring.BulanAmt+1, buffer+14, 9);
	memcpy(glProcInfo.stTranLog.BRI.CicilanBring.PokokAmt+1, buffer+50, 9);

	memcpy(glProcInfo.stTranLog.szAmount+1, buffer+50, 9);
	return TRUE;
}


uchar unPackCicilanBukopin(void)
{
	uchar buffer[999];
	uchar Amount[12+1];
	ulong len=0;

	MEM_ZERO(buffer);
	MEM_ZERO(Amount);

	len = PubChar2Long(glRecvPack.szBit61, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit61+2, len);
	//hex_dump_char("buffer",buffer,len);
	MEM_ZERO(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.InstallmenTenor);
	MEM_ZERO(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.BulanAmt);
	MEM_ZERO(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.Interest);
	#ifdef BANK_PANIN
	//panin 4212
	MEM_ZERO(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.PokokAmt);	
	sprintf(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.InstallmenTenor,"%3X\0",buffer);
	sprintf(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.BulanAmt,"%12X\0",buffer+2);
	sprintf(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.Interest,"%5X\0",buffer+8);
	sprintf(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.PokokAmt,"%12X\0",buffer+11);
	#else
	//bukopin 4212
	MEM_ZERO(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.Bulan);
	memcpy(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.InstallmenTenor,buffer,3);
	memcpy(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.Bulan,buffer+3,2);
	memcpy(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.Interest,buffer+5,5);
	memcpy(glProcInfo.stTranLog.BUKOPIN.CicilanBukopin.BulanAmt,buffer+10,12);
	#endif
	return TRUE;
}

#ifdef BANK_BRI
uchar unPackTransfer_SesamaBRI(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	mapGet(traResponseInq, buffer, 999);
	
	memcpy(glProcInfo.stTranLog.BRI.TRF_SESAMABRI.NoRek, glProcInfo.szReq48+3, 15);
	memcpy(glProcInfo.stTranLog.BRI.TRF_SESAMABRI.Nama, buffer, 30);

	fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,2,",.");
	memcpy(glProcInfo.stTranLog.BRI.TRF_SESAMABRI.Total, buffAmt, 12);

	return TRUE;
}

uchar unPackTransfer_AntarBank(void)
{
	uchar buffer1[999];
	uchar buffer2[999];
	uchar buffAmt[12+1];
	byte	  jenisRekening;
	char	  kodeJalur[3+1];

	MEM_ZERO(buffer1);
	MEM_ZERO(buffer2);
	MEM_ZERO(buffAmt);
	MEM_ZERO(kodeJalur);

	mapGet(traResponseInq, buffer1, 999);

	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPengirim, buffer1+30, 15);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPenerima, buffer1+45, 15);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPenerima, buffer1, 30);

	mapGetByte(traSumberRek, jenisRekening);
	switch(jenisRekening)
		{
			case 1:
				strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.JenisRekening, "DARI TABUNGAN");
				break;
			case 2:
				strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.JenisRekening, "DARI GIRO");
				break;
		}

	memcpy(buffer2, glRecvPack.szBit48+2, 999);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPengirim, buffer2, 30);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim, buffer2+63, 20);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPenerima, glProcInfo.szReq48+3, 20);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRefPenerima, glProcInfo.szReq48+23, 16);
	memcpy(kodeJalur, buffer2+60, 3);

	if(memcmp(kodeJalur, "00-", 3) == 0)
		strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI BRI");
	else if(memcmp(kodeJalur, "01-", 3) == 0)
		strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI LINK");
	else if(memcmp(kodeJalur, "02-", 3) == 0)
		strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI PRIMA");
	else if(memcmp(kodeJalur, "03-", 3) == 0)
		strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI BERSAMA");

	fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,2,",.");
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Total, buffAmt, 12);

	return TRUE;
}


uchar unPackPembayaran_Telkom(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	mapGet(traResponseInq, buffer, 999);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.Nama, buffer+67, 30);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.Nama, buffer+43, 30); 
	memcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4, 12);

//	fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,0,",.");
//	memcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.Total, buffAmt, 12);

	return TRUE;
}


uchar unPackPembayaran_KartuHalo(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	mapGet(traResponseInq, buffer, 999);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUHALO.NoHP,glProcInfo.szReq48, 13);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUHALO.Nama,buffer+24, 30);

	memcpy(buffAmt,buffer+54,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUHALO.Total, buffAmtFmt, 12);

	return TRUE;
}

uchar unPackPembayaran_KartuMatrix(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	mapGet(traResponseInq, buffer, 999);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP,glProcInfo.szReq48, 13);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Nama,buffer+47, 30);

	memcpy(buffAmt,buffer+12,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Total, buffAmtFmt, 12);

	return TRUE;
}

uchar unPackPembayaran_PLNPascaBayar(void)
{
	uchar buffer1[999];
	uchar buffer2[999];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[12+1];
	uchar regTag[12+1];

	MEM_ZERO(buffer1);
	MEM_ZERO(buffer2);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	MEM_ZERO(regTag);

	mapGet(traResponseInq, buffer1, 999);
	memcpy(buffer2, glRecvPack.szBit48+2, 999);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.NoIDPel,buffer1+3,12);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Nama,buffer1+115,30);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.TarifDaya,buffer2+36,4);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.TarifDaya+4,"/",1);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.TarifDaya+5,buffer2+40,9);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Bulan,buffer1+279,23);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.StandMeter,buffer2+377,17);

	memcpy(regTag,buffer1+86,10);
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.RegTag,regTag,0,",.");

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.MLPO,buffer2+198,32);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Telepon,buffer2+362,15);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Tunggakan,buffer2+49,2); //50

	memcpy(buffAmt,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPASCA.Total, buffAmtFmt, 12);

	return TRUE;
}

uchar unPackPembayaran_PLNPraBayar(void)
{
	uchar buffer1[999];
	uchar buffer2[999];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[12+1];
	uchar regTag[12+1];

	MEM_ZERO(buffer1);
	MEM_ZERO(buffer2);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	MEM_ZERO(regTag);

	mapGet(traResponseInq, buffer1, 999);
	memcpy(buffer2, glRecvPack.szBit48+2, 999);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.NoMeter,buffer1+48,11);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.NoIDPel,buffer1+3,12);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Nama,buffer1+115,30);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.TarifDaya,buffer1+35,4);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.TarifDaya+4,"/",1);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.TarifDaya+5,buffer1+43,5);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.MLPO,buffer1+197,32);

	memcpy(buffAmt,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Bayar, buffAmtFmt, 12);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Materai,buffer2+106,9);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.PPN,buffer2+188,9);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.PPJ,buffer2+270,9);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Angsuran,buffer2+352,9);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.RPToken,buffer2+167,10);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.KWH,buffer2+178,2);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Token,buffer2+279,20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PLNPRA.Telepon,buffer2+361,12);

	return TRUE;
}



uchar unPackPembayaran_KKBRI(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[12+1];
	uchar tagihan[12+1];
	uchar tagihanFmt[12+1];
	uchar minBayar[12+1];
	uchar minBayarFmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	MEM_ZERO(tagihan);
	MEM_ZERO(tagihanFmt);
	MEM_ZERO(minBayar);
	MEM_ZERO(minBayarFmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	mapGet(traResponseInq, buffer, 999);
	//memcpy(buffer, glRecvPack.szBit48+2, len);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KKBRI.Nomor,glProcInfo.szReq48,16);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KKBRI.Nama, buffer, 30);

	memcpy(tagihan,buffer+31,9);
	fmtAmt(tagihanFmt,tagihan,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KKBRI.Tagihan, tagihanFmt, 16);

	memcpy(minBayar,buffer+42,10);
	fmtAmt(minBayarFmt,minBayar,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KKBRI.MinBayar,minBayarFmt,12);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_KKBRI.DueDate,buffer+54,8);

	memcpy(buffAmt,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KKBRI.Total, buffAmtFmt, 12);

	return TRUE;
}

uchar unPackPembayaran_KKKTA(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[12+1];
	word  mnuItm;

	uchar NoKK_KTA[32+1];
	uchar NoKK[16+1];
	uchar NoKTA[10+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	MEM_ZERO(NoKK_KTA);
	MEM_ZERO(NoKK);
	MEM_ZERO(NoKTA);

	mapGetWord(traMnuItm,mnuItm);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	switch(mnuItm)
		{
		case mnuKKCitibank:
			memcpy(NoKK_KTA,glProcInfo.szReq48,32);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nomor,NoKK_KTA+4,16);
			break;
		case mnuKTACitibank:
			memcpy(NoKK_KTA,glProcInfo.szReq48,32);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nomor,NoKK_KTA+4,10);
			break;
		case mnuKKStandChat:
		case mnuKTAStandChat:
		case mnuKKHSBC:
		case mnuKTAHSBC:
		case mnuKKANZ:
		case mnuKKAMRO:
		case mnuKTAAMRO:
			memcpy(NoKK_KTA,glProcInfo.szReq48,17);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nomor,NoKK_KTA+1,16);
			break;
		}

	memcpy(buffAmt,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Tagihan, buffAmtFmt, 12);

	return TRUE;
}

uchar unPackPembayaran_Cicilan(void)
{
	uchar buffer[999];
	uchar buffAmt[12+1];
	
	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	mapGet(traResponseInq, buffer, 999);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.NoKontrak,buffer+24,16);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Nama,buffer+4,20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Angsuran,buffer+73,4);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.JatuhTempo,buffer+77,2);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.JatuhTempo+2,"-",1);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.JatuhTempo+3,buffer+79,2);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.JatuhTempo+5,"-",1);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.JatuhTempo+6,buffer+81,4);
	
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.MLPO,buffer+45,19);
	
	memcpy(buffAmt,buffer+85,10);
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Tagihan,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.Total,glProcInfo.stTranLog.BRI.PEMB_CICILAN.Tagihan,12);

	return TRUE;
}


uchar unPackPembayaran_Zakat(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);
	
	memcpy(buffer, glRecvPack.szBit48+2, len);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_ZAKAT_INFAQ.Nama, buffer+16, 20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_ZAKAT_INFAQ.Nomor, buffer, 16);

	fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,2,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_ZAKAT_INFAQ.Total, buffAmt, 12);

	return TRUE;
}

uchar unPackPembayaran_DPLK(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);
	
	memcpy(buffer, glRecvPack.szBit48+2, len);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_DPLK.NoDPLK, glProcInfo.szReq48+3, 7);

	fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,2,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_DPLK.Total, buffAmt, 12);

	return TRUE;
}

uchar unPackPembayaran_Pesawat(void)
{
	uchar buffer[999];
	uchar buffAmt[12+1];;

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	mapGet(traResponseInq, buffer, 999);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.KodeBayar,buffer,13);
	
	memcpy(buffAmt,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_TIKET.Total,buffAmt,0,",.");

	memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.JmlhFlight,buffer+19,1);

	if ( memcmp(glProcInfo.stTranLog.BRI.PEMB_TIKET.JmlhFlight, "1", 1)==0 )
		{
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.KodeBooking,buffer+73,6);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Nama,buffer+43,30);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1,buffer+22,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+2," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+3,buffer+31,4);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+7," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+8,buffer+24,1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+9," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+10,buffer+25,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+13,"-",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+14,buffer+28,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+17," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+18,buffer+35,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+20,"/",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+21,buffer+37,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+23," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+24,buffer+39,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+26,":",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+27,buffer+41,2);
			
		}
	if ( memcmp(glProcInfo.stTranLog.BRI.PEMB_TIKET.JmlhFlight, "2", 1)==0 )		
		{
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.KodeBooking,buffer+96,6);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Nama,buffer+66,30);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1,buffer+22,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+2," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+3,buffer+31,4);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+7," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+8,buffer+24,1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+9," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+10,buffer+25,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+13,"-",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+14,buffer+28,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+17," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+18,buffer+35,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+20,"/",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+21,buffer+37,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+23," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+24,buffer+39,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+26,":",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+27,buffer+41,2);

			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2,buffer+45,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+2," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+3,buffer+54,4);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+7," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+8,buffer+47,1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+9," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+10,buffer+48,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+13,"-",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+14,buffer+51,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+17," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+18,buffer+58,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+20,"/",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+21,buffer+60,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+23," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+24,buffer+62,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+26,":",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+27,buffer+64,2);
		}
	if ( memcmp(glProcInfo.stTranLog.BRI.PEMB_TIKET.JmlhFlight, "3", 1)==0 )
		{
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.KodeBooking,buffer+119,6);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Nama,buffer+89,21);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1,buffer+22,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+2," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+3,buffer+31,4);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+7," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+8,buffer+24,1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+9," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+10,buffer+25,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+13,"-",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+14,buffer+28,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+17," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+18,buffer+35,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+20,"/",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+21,buffer+37,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+23," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+24,buffer+39,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+26,":",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_1+27,buffer+41,2);

			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2,buffer+45,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+2," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+3,buffer+54,4);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+7," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+8,buffer+47,1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+9," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+10,buffer+48,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+13,"-",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+14,buffer+51,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+17," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+18,buffer+58,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+20,"/",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+21,buffer+60,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+23," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+24,buffer+62,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+26,":",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_2+27,buffer+64,2);

			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3,buffer+68,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+2," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+3,buffer+77,4);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+7," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+8,buffer+70,1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+9," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+10,buffer+71,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+13,"-",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+14,buffer+74,3);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+17," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+18,buffer+81,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+20,"/",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+21,buffer+83,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+23," ",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+24,buffer+85,2);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+26,":",1);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_TIKET.Jadwal_3+27,buffer+87,2);
		}

	return TRUE;
}

uchar unPackPembayaran_Briva(void)
{
	uchar buffer[999];
	uchar tagihan[12+1];
	uchar bayar[12+1];
	uchar status[1+1];

	uchar total[12+1];

	card   numBayar;

	MEM_ZERO(buffer);
	MEM_ZERO(tagihan);
	MEM_ZERO(bayar);

	mapGet(traResponseInq, buffer, 999);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_BRIVA.NoBriva,buffer,20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_BRIVA.NamaInstitusi,buffer+118,20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_BRIVA.NamaPelanggan,buffer+56,30);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_BRIVA.Keterangan,buffer+20,20);

	memcpy(tagihan,buffer+91+2,10);
	memset(glProcInfo.stTranLog.szAmount, 0, sizeof(glProcInfo.stTranLog.szAmount));
	memset(glProcInfo.stTranLog.szAmount, 0x30, sizeof(glProcInfo.stTranLog.szAmount)-1);
	memcpy(glProcInfo.stTranLog.szAmount, tagihan, 10);
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_BRIVA.Tagihan,tagihan,0,",.");

//	hex_dump("Mason Pindah Berak",glProcInfo.stTranLog.szAmount, 15);

	memcpy(bayar,glProcInfo.stTranLog.szAmount,10); 
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_BRIVA.Bayar,bayar,0,",.");

	memcpy(status,buffer+138,1);
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_BRIVA.Status,status,0,",.");

	dec2num(&numBayar,bayar,0);
	numBayar+=2250;
	num2dec(total,numBayar,0);
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_BRIVA.Total,total,0,",.");
//	hex_dump("Mason  Berak",glProcInfo.stTranLog.szAmount, 15);

	return TRUE;
}


uchar unPackPembayaran_Pendidikan(void)
{
	uchar buffer[999];
	uchar buffAmt[12+1];;

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	mapGet(traResponseInq, buffer, 999);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.NoBill,buffer,20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.NamaMahasiswa,buffer+60,20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.NoMahasiswa,buffer+20,20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.Tipe,buffer+80,20);

	memcpy(buffAmt,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_PENDIDIKAN.Tagihan,buffAmt,0,",.");
	
	return TRUE;
}

uchar unPackISi_Ulang(void)
{
	uchar buffer[999];
	uchar buffAmt[12+1];
	uchar periode[8+1];
	uchar kdVoucer[16+1];
	uchar nomerResi[20+1];
	word  mnuItm;

	MEM_ZERO(buffAmt);
	MEM_ZERO(periode);
	MEM_ZERO(kdVoucer);

	mapGetWord(traMnuItm,mnuItm);

	memcpy(buffer, glRecvPack.szBit48+2, 999);

	memcpy(glProcInfo.stTranLog.BRI.ISIULANG.NoHP,glProcInfo.szReq48,13);
	memcpy(buffAmt,glProcInfo.szReq48+13,12);

	fmtAmt(glProcInfo.stTranLog.BRI.ISIULANG.Nominal,buffAmt,0,",.");

	switch(mnuItm)
		{
		case mnuImThree25rb:
    		case mnuImThree50rb:
    		case mnuImThree75rb:
    		case mnuImThree100rb:
    		case mnuImThree150rb:
    		case mnuImThree200rb:
    		case mnuImThree500rb:
    		case mnuImThree1jt:
				memcpy(periode,buffer+25,8);
				memcpy(glProcInfo.stTranLog.BRI.ISIULANG.Periode,periode,8);
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
				memcpy(periode,buffer+25,8);
				memcpy(glProcInfo.stTranLog.BRI.ISIULANG.Periode,periode,8);

				memcpy(kdVoucer,buffer+33,16);
				memcpy(glProcInfo.stTranLog.BRI.ISIULANG.KodeVoucer,kdVoucer,16);
				break;
		case mnuSmartFren20rb:
		case mnuSmartFren25rb:
    		case mnuSmartFren50rb:
    		case mnuSmartFren100rb:
		case mnuSmartFren150rb:
    		case mnuSmartFren200rb:		
		case mnuSmartFren300rb:
		case mnuSmartFren500rb:
				memcpy(nomerResi,buffer+45,20);
				memcpy(glProcInfo.stTranLog.BRI.ISIULANG.NomerResiSF,nomerResi,20);
				break;
				
		}
	return TRUE;
}


uchar unPackTopUpOnLineBrizzi(void)
{
	uchar buffer[999];
	int len;

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

//	hex_dump_char("unPackTopUpOnLineBrizzi", buffer, 48);
	memcpy(glProcInfo.TopUpKeyBrizzi, buffer, 48);

	return TRUE;
}



uchar unPackAktivasiDepositBrizzi(void)
{
	uchar buffer[999];
	int len;

	MEM_ZERO(buffer);

	memcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4, 12);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	
	memcpy(glProcInfo.TopUpKeyBrizzi, buffer, 48);

	memcpy(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, buffer+48, 10);

	return TRUE;
}


uchar unPackInfoDepositBrizzi(void)
{
	uchar buffer[999];
	int len;

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memset(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, 0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit));
	memcpy(buffer, glRecvPack.szBit48+2, len);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, buffer+4, 12);

	return TRUE;
}


uchar unPackRedeemBrizzi(void)
{
	uchar buffer[999];
	int len;

	MEM_ZERO(buffer);

	memcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4, 12);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);


	memset(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, 0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore));
	memset(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, 0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit));
	MEM_ZERO(glProcInfo.stTranLog.szAmount);
	
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.AmountBefore, buffer, 10);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, buffer+10, 10);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.Fee, buffer+20, 10);	//fee
	memcpy(glProcInfo.stTranLog.szAmount, buffer+30, 10);

	return TRUE;
}



uchar unPackReaktivasiBrizzi(void)
{
	uchar buffer[999];
	int len;

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);


	memset(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, 0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter));
	memset(glProcInfo.stTranLog.BRI.Brizzi.Fee, 0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.Fee));
	memset(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, 0, sizeof(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit));

	memcpy(glProcInfo.stTranLog.BRI.Brizzi.AmountAfter, buffer+16, 10);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.Fee, buffer+26, 10);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.SisaDeposit, buffer+36, 10);

	memcpy(glProcInfo.stTranLog.BRI.Brizzi.LamaPasif, buffer+46, 2);
	memcpy(glProcInfo.stTranLog.BRI.Brizzi.StatusKartu, buffer+48, 2);
	return TRUE;
}

uchar unPackTarikTunai(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar iLoop;

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	memset(glProcInfo.stTranLog.BRI.Tunai.Fee, 0x30, 12);
//	memcpy(glProcInfo.stTranLog.BRI.Tunai.Saldo, buffer+4, 12);
	memcpy(glProcInfo.stTranLog.BRI.Tunai.Saldo, buffer, 16);
//	memcpy(glProcInfo.stTranLog.BRI.Tunai.Fee, buffer+16, 12);
	for(iLoop=0;iLoop<12;iLoop++)
	{
		if(buffer[16+iLoop]==0x20)
			break;
	}

	if(iLoop>0)
		memcpy(&glProcInfo.stTranLog.BRI.Tunai.Fee[12-iLoop], buffer+16, iLoop);
		
	return TRUE;
}


uchar unPackSetorTunai(void)
{
	uchar buffer[999];
	ulong len=0;

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	memcpy(glProcInfo.stTranLog.BRI.Tunai.Nama, buffer, 30);

	return TRUE;
}

uchar unPackCashIn(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];
	//uchar bufPoint[12];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);
	memcpy(glProcInfo.stTranLog.BRI.CashIn.noHP,buffer, 20);
	memcpy(glProcInfo.stTranLog.BRI.CashIn.Fee,buffer+20, 12);
	memcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4, 12);

/*
	MEM_ZERO(bufPoint);
	memcpy(	bufPoint, buffer+20, 12);
		glProcInfo.stTranLog.BRI.CashIn.Fee=atol(bufPoint);
*/
	return TRUE;
}

uchar unPackCashOut(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	
	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);
	//memcpy(glProcInfo.stTranLog.BRI.CashOut.noHP,glProcInfo.szReq48+6, 20);
	memcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4, 12);
	memcpy(glProcInfo.stTranLog.BRI.CashOut.Saldo, buffer, 12);
	memcpy(glProcInfo.stTranLog.BRI.CashOut.Fee, buffer+12, 12);

	return TRUE;
}


uchar unPackSetorPasti(void)
{
	uchar buffer[999];
	ulong len=0;

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	if(glProcInfo.stTranLog.BRI.ucflagDebitBri)
	{
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NamaAsal, buffer, 15);
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NoRekAsal, buffer+15, 15);
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NoRekTujuan, buffer+60, 15);
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NamaTujuan, buffer+75, 20);
	}
	else
	{
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NamaAsal, buffer, 14);
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NoRekTujuan, buffer+17, 15);
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.BankAsal, buffer+41, 15);
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.KodeJenisTransfer, buffer+60, 2);
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NoRekAsal, buffer+63, 15);
		memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NamaTujuan, buffer+78, 15);
	}
	return TRUE;
}

uchar unPackRenewalKartu(void)
{
	uchar buffer[999];
	ulong len=0;

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	memcpy(glProcInfo.stTranLog.BRI.Card_Services.NoRek, buffer,15);
	memcpy(glProcInfo.stTranLog.BRI.Card_Services.Nama,buffer+60,21);
	
	return TRUE;
}

uchar unPackRegTrxPBTransfer(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	memcpy(glProcInfo.stTranLog.BRI.RegTRXPB.Nama, buffer, 30);

	return TRUE;
}


uchar unPackSSB(void)
{
	uchar buffer[999];
	ulong len=0;
	ulong ulAmt;

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	memcpy(glProcInfo.stTranLog.BRI.Ssb.RekSatpasSSB, buffer, 15);
	memcpy(glProcInfo.stTranLog.BRI.Ssb.NamaSatpasSSB, buffer+15, 15);

	memcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4, 12);

	switch(glProcInfo.stTranLog.ucTranType)
	{	
		case TRANS_TYPE_BPKB_BARU:
		case TRANS_TYPE_BPKB_GANTI_PEMILIK:
			PubAsc2Long(glProcInfo.stTranLog.szAmount,12, &ulAmt);
			ulAmt=ulAmt/100;
			ulAmt = ulAmt*glProcInfo.stTranLog.BRI.Ssb.Jumlah;
			sprintf(glProcInfo.stTranLog.szAmount, "%lu", ulAmt);
			AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
			break;
	}

	return TRUE;
}
#endif




uchar unPackTransfer_AntarBankBkpn(void)
{
	uchar buffer1[999];
	uchar buffer2[28+1];
	uchar buffer3[28+1];
	uchar buffAmt[14+1];
	uchar buffAmt2[14+1];
	byte	  jenisRekening;
	char	  kodeJalur[3+1];

	char	ListKodeBank[20+1];
	char	ListNamaBank[20+1];

	MEM_ZERO(buffer1);
	MEM_ZERO(buffer2);
	MEM_ZERO(buffer3);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmt2);
	MEM_ZERO(kodeJalur);
	MEM_ZERO(ListKodeBank);
	MEM_ZERO(ListNamaBank);

	mapGet(traResponseInq, buffer1, 999);

	memcpy(ListKodeBank,buffer1,3);
	TransInfoKodeBankBkpnPilih(ListNamaBank, ListKodeBank);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPenerima,ListNamaBank,strlen(ListNamaBank));
	

	//memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPengirim, buffer1+30, 20);
	//memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.BankPenerima, buffer1+20, 20);
	//memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPenerima, buffer1+40, 30);


	memcpy(buffer2, glRecvPack.szField102+2, 28);
	memcpy(buffer3, glRecvPack.szField103+2, 28);
	//memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim, buffer2+2, 26);
	//memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPenerima, buffer3+2, 26);

	mapGetByte(traSumberRek, jenisRekening);
	switch(jenisRekening)
		{
			case 1:
				strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.JenisRekening, "OVERBOOK");
				memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim, buffer2+2, 6);
				strcat(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim, "****");
				memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPenerima, buffer3+2, 26);
				memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPenerima, buffer1+40, 30);
				break;
			case 2:
				strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.JenisRekening, "FUND TRANSFER");
				memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim, buffer2, 6);
				strcat(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim, "****");
				memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPenerima, buffer3, 26);
				memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPenerima, buffer1+41, 30);
				break;
		}

	/*
	memcpy(buffer2, glRecvPack.szField102+2, 28);
	memcpy(buffer3, glRecvPack.szField103+2, 28);
	//memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NamaPengirim, buffer2, 30);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPengirim, buffer2+2, 26);
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRekPenerima, buffer3+2, 26);
	////memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.NoRefPenerima, glProcInfo.szReq48+23, 16);
	*/

	/*
	memcpy(kodeJalur, buffer2+60, 3);

	if(memcmp(kodeJalur, "00-", 3) == 0)
		strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI BRI");
	else if(memcmp(kodeJalur, "01-", 3) == 0)
		strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI LINK");
	else if(memcmp(kodeJalur, "02-", 3) == 0)
		strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI PRIMA");
	else if(memcmp(kodeJalur, "03-", 3) == 0)
		strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI BERSAMA");
	*/
	strcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Jalur,"MELALUI BANK BUKOPIN");  // DIKI ADD FOR MINI ATM BUKOPIN

	//fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,2,",.");
	//memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Total, buffAmt, 14);

	//fmtAmt(buffAmt,glProcInfo.stTranLog.szAmount,0,",.");
	memcpy(buffAmt2,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(buffAmt,buffAmt2,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.TRF_ANTARBANK.Total, buffAmt, 12);
	

	return TRUE;
}



uchar unPackISi_UlangBkpn(void)
{
	uchar buffer[999];
	uchar buffAmt[12+1];
	uchar buffPulsa[12+1];
	uchar periode[8+1];
	uchar kdVoucer[16+1];
	uchar nomerResi[20+1];
	word  mnuItm;
	uchar buffer3[28+1];
	uchar buffer2[28+1];

	MEM_ZERO(buffAmt);
	MEM_ZERO(periode);
	MEM_ZERO(kdVoucer);
	MEM_ZERO(buffer3);
	MEM_ZERO(buffPulsa);

	///mapGetWord(traMnuItm,mnuItm);

	memcpy(buffer3, glRecvPack.szField103+2, 15);
	memcpy(glProcInfo.stTranLog.BRI.ISIULANG.NoHP, buffer3, 15);
	memcpy(buffAmt,glRecvPack.szBit4,10);
	fmtAmt(glProcInfo.stTranLog.BRI.ISIULANG.Nominal,buffAmt,0,",.");
	memcpy(buffPulsa,glRecvPack.szBit4,10);
	fmtAmt(glProcInfo.stTranLog.BRI.ISIULANG.Pulsa,buffAmt,0,",.");

	MEM_ZERO(buffer2);
	memcpy(buffer2, glRecvPack.szField102+2, 28);
	//memcpy(glProcInfo.stTranLog.BRI.ISIULANG.NoRekPengirim, buffer2+2, 26);
	memcpy(glProcInfo.stTranLog.BRI.ISIULANG.NoRekPengirim, buffer2+2, 6);
	strcat(glProcInfo.stTranLog.BRI.ISIULANG.NoRekPengirim, "****");


	/*
	switch(mnuItm)
		{
		case mnuImThree25rb:
    		case mnuImThree50rb:
    		case mnuImThree75rb:
    		case mnuImThree100rb:
    		case mnuImThree150rb:
    		case mnuImThree200rb:
    		case mnuImThree500rb:
    		case mnuImThree1jt:
				memcpy(periode,buffer+25,8);
				memcpy(glProcInfo.stTranLog.BRI.ISIULANG.Periode,periode,8);
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
				memcpy(periode,buffer+25,8);
				memcpy(glProcInfo.stTranLog.BRI.ISIULANG.Periode,periode,8);

				memcpy(kdVoucer,buffer+33,16);
				memcpy(glProcInfo.stTranLog.BRI.ISIULANG.KodeVoucer,kdVoucer,16);
				break;
		case mnuSmartFren20rb:
		case mnuSmartFren25rb:
    		case mnuSmartFren50rb:
    		case mnuSmartFren100rb:
		case mnuSmartFren150rb:
    		case mnuSmartFren200rb:		
		case mnuSmartFren300rb:
		case mnuSmartFren500rb:
				memcpy(nomerResi,buffer+45,20);
				memcpy(glProcInfo.stTranLog.BRI.ISIULANG.NomerResiSF,nomerResi,20);
				break;
				
		}

	*/

	
	return TRUE;
}




uchar unPackTarikTunaiBkpn(void)
{
	uchar buffer[999];
	uchar bufferFee[30];
	uchar bufferNoRek[30];
	uchar bufferAmount[30];
	uchar bufferSaldo[30];
	ulong len=0;
	uchar iLoop;

	MEM_ZERO(buffer);
	MEM_ZERO(bufferFee);
	MEM_ZERO(bufferNoRek);
	MEM_ZERO(bufferAmount);
	MEM_ZERO(bufferSaldo);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	/*
	memset(glProcInfo.stTranLog.BRI.Tunai.Fee, 0x30, 12);
	memcpy(glProcInfo.stTranLog.BRI.Tunai.Saldo, buffer+4, 12);
	memcpy(glProcInfo.stTranLog.BRI.Tunai.Saldo, buffer, 16);
	memcpy(glProcInfo.stTranLog.BRI.Tunai.Fee, buffer+16, 12);
	*/

	
	memcpy(bufferNoRek, glRecvPack.szField102+2, 28);
	//memcpy(glProcInfo.stTranLog.BRI.Tunai.NoRek, bufferNoRek+2, 26);
	memcpy(glProcInfo.stTranLog.BRI.Tunai.NoRek, bufferNoRek+2, 6);
	strcat(glProcInfo.stTranLog.BRI.Tunai.NoRek, "****");

	memcpy(bufferAmount,glRecvPack.szBit4,10);
	fmtAmt(glProcInfo.stTranLog.BRI.Tunai.NominalTarik,bufferAmount,0,",.");

	memcpy(bufferSaldo, buffer, 14);
	fmtAmt(glProcInfo.stTranLog.BRI.Tunai.Saldo, bufferSaldo, 0,",.");

	////memcpy(bufferFee,glRecvPack.szBit8,6);
	////fmtAmt(glProcInfo.stTranLog.BRI.Tunai.Fee,bufferFee,0,",.");

	
	/*	
	for(iLoop=0;iLoop<12;iLoop++)
	{
		if(buffer[16+iLoop]==0x20)
			break;
	}

	if(iLoop>0)
		memcpy(&glProcInfo.stTranLog.BRI.Tunai.Fee[12-iLoop], buffer+16, iLoop);
	*/
		
	return TRUE;
}




uchar unPackPembayaran_CicilanBkpn(void)
{
	uchar buffer[999];
	uchar buffAmt[12+1];

	uchar buffer2[20+1];


	uchar buffAmtsisaangsuran[12+1];
	uchar buffAmtnilaitagihan[12+1];
	uchar buffAmtnilaiangsuran[12+1];
	uchar buffAmtnilaidenda[12+1];
	uchar buffAmtnilaiadmin[12+1];
	uchar buffAmtnilailainnya[12+1];
	uchar buffAmtTotalBayar[12+1];

	uchar buffAmtsisaangsuranFmt[20+1];
	uchar buffAmtnilaitagihanFmt[20+1];
	uchar buffAmtnilaiangsuranFmt[20+1];
	uchar buffAmtnilaidendaFmt[20+1];
	uchar buffAmtnilaiadminFmt[20+1];
	uchar buffAmtnilailainnyaFmt[20+1];
	uchar buffAmtTotalBayarFmt[20+1];
	
	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffer2);

	MEM_ZERO(buffAmtsisaangsuran);
	MEM_ZERO(buffAmtnilaitagihan);
	MEM_ZERO(buffAmtnilaiangsuran);
	MEM_ZERO(buffAmtnilaidenda);
	MEM_ZERO(buffAmtnilaiadmin);
	MEM_ZERO(buffAmtnilailainnya);
	MEM_ZERO(buffAmtTotalBayar);

	MEM_ZERO(buffAmtsisaangsuranFmt);
	MEM_ZERO(buffAmtnilaitagihanFmt);
	MEM_ZERO(buffAmtnilaiangsuranFmt);
	MEM_ZERO(buffAmtnilaidendaFmt);
	MEM_ZERO(buffAmtnilaiadminFmt);
	MEM_ZERO(buffAmtnilailainnyaFmt);
	MEM_ZERO(buffAmtTotalBayarFmt);

	


	////
/*
	
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
	memcpy(referencenumber,bit48Rsp+249,32);
	
	
	//memcpy(jatuhTempo,bit48Rsp+77,2);
	//memcpy(jatuhTempo+2,"-",1);
	fmtAmt(nilaitagihanFmt,nilaitagihan,0,",.");	
	fmtAmt(nilaiangsuranFmt,nilaiangsuran,0,",.");
	fmtAmt(nilaidendaFmt,nilaidenda,0,",.");
	*/
	

	mapGet(traResponseInq, buffer, 999);

	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.billid,buffer+4,16);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.name,buffer+20,30);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.ptname,buffer+50,25);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.branchname,buffer+75,30);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.serialnumber,buffer+105,14);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.norangka,buffer+119,25);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.merkkendaraan,buffer+144,30);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.tenor,buffer+186,3);
	memcpy(buffAmtsisaangsuran,buffer+189,12);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.noangsuran,buffer+201,3);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.jatuhtempo,buffer+204,9);
	memcpy(buffAmtnilaitagihan,buffer+213,12);
	memcpy(buffAmtnilaiangsuran,buffer+225,12);
	memcpy(buffAmtnilaidenda,buffer+237,12);
	memcpy(buffAmtnilaiadmin,buffer+249,12);
	memcpy(buffAmtnilailainnya,buffer+261,12);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.referencenumber,buffer+273,30);

	memcpy(buffAmtTotalBayar, glRecvPack.szBit4, 10);
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_CICILAN.TotalPembayaran,buffAmtTotalBayar,0,",.");

	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_CICILAN.sisaangsuran,buffAmtsisaangsuran,0,",.");
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilaitagihan,buffAmtnilaitagihan,0,",.");
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilaiangsuran,buffAmtnilaiangsuran,0,",.");
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilaidenda,buffAmtnilaidenda,0,",.");
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilaiadmin,buffAmtnilaiadmin,0,",.");
	fmtAmt(glProcInfo.stTranLog.BRI.PEMB_CICILAN.nilailainnya,buffAmtnilailainnya,0,",.");

	
	

	MEM_ZERO(buffer2);
	memcpy(buffer2, glRecvPack.szField102+2, 20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_CICILAN.NoRek, buffer2+2, 6);
	strcat(glProcInfo.stTranLog.BRI.PEMB_CICILAN.NoRek, "****");
	
	return TRUE;
}



uchar unPackPembayaran_KKBkpn(void)
{
	uchar buffer[999];
	uchar bufferNama[25+1];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[12+1];
	word  mnuItm;

	uchar NoKK_KTA[32+1];
	uchar NoKK[16+1];
	uchar NoKTA[10+1];

	uchar buffer2[20+1];

	uchar NoKK_KTA_AWAL[32+1];
	uchar NoKK_KTA_AKHIR[32+1];
	uchar NoKK_KTA_MASKING[32+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	MEM_ZERO(NoKK_KTA);
	MEM_ZERO(NoKK);
	MEM_ZERO(NoKTA);
	MEM_ZERO(bufferNama);

	MEM_ZERO(NoKK_KTA_AWAL);
	MEM_ZERO(NoKK_KTA_AKHIR);
	MEM_ZERO(NoKK_KTA_MASKING);

	mapGetWord(traMnuItm,mnuItm);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	/*
	switch(mnuItm)
		{
		case mnuKKStandChat:
		case mnuKTAStandChat:
		case mnuKKHSBC:
		case mnuKTAHSBC:
		case mnuKKANZ:
		case mnuKKAMRO:
		case mnuKTAAMRO:
			memcpy(NoKK_KTA,glProcInfo.szReq48,17);
			memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nomor,NoKK_KTA+1,16);
			break;
		}
	*/

	memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.NamaCust, buffer+16, 25);

	//memcpy(NoKK_KTA,glProcInfo.szReq103,17);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nomor,NoKK_KTA,16);
	memcpy(NoKK_KTA,glProcInfo.szReq103,20);
	memcpy(NoKK_KTA_AWAL,NoKK_KTA,6);
	memcpy(NoKK_KTA_AKHIR,NoKK_KTA+12,8);	
	memcpy(NoKK_KTA_MASKING,NoKK_KTA,6);
	strcat(NoKK_KTA_MASKING, "******");
	strcat(NoKK_KTA_MASKING, NoKK_KTA_AKHIR);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Nomor,NoKK_KTA_MASKING,20);

	memcpy(buffAmt,glProcInfo.stTranLog.szAmount,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.Tagihan, buffAmtFmt, 12);

	MEM_ZERO(buffer2);
	memcpy(buffer2, glRecvPack.szField102+2, 20);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.NoRek, buffer2+2, 20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.NoRek, buffer2+2, 6);
	strcat(glProcInfo.stTranLog.BRI.PEMB_KK_KTA.NoRek, "****");

	return TRUE;
}


uchar unPackPembayaran_TelkomBkpn(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[14+1];
	uchar buffer2[20+1];

	uchar   PayPeriodBulan[20+1];
	uchar   PayPeriodTahun[20+1];
	uchar   PayPeriodFmt[20+1];
	uchar	PayPeriod[20+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffer2);
	MEM_ZERO(buffAmtFmt);

	MEM_ZERO(PayPeriodBulan);
	MEM_ZERO(PayPeriodTahun);
	MEM_ZERO(PayPeriodFmt);
	MEM_ZERO(PayPeriod);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	mapGet(traResponseInq, buffer, 999);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.Nama, buffer+67, 30);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.Nama, buffer+15, 30); 
	memcpy(buffAmt, glRecvPack.szBit4, 10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.Total, buffAmtFmt, 14);

	MEM_ZERO(buffer2);
	memcpy(buffer2, glRecvPack.szField102+2, 20);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.NoRek, buffer2+2, 20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.NoRek, buffer2+2, 6);
	strcat(glProcInfo.stTranLog.BRI.PEMB_TELKOM.NoRek, "****");


	memcpy(PayPeriod, buffer+62, 6);
	memcpy(PayPeriodBulan, PayPeriod+4, 2);
	memcpy(PayPeriodTahun, PayPeriod, 4);
	memcpy(PayPeriodFmt, PayPeriodBulan, 2);
	strcat(PayPeriodFmt, " - ");
	strcat(PayPeriodFmt, PayPeriodTahun);
	strcpy(glProcInfo.stTranLog.BRI.PEMB_TELKOM.PayPeriod, PayPeriodFmt);

	return TRUE;
}



uchar unPackPembayaran_SelularBkpn(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[12+1];
	uchar buffAmtFmt[12+1];
	uchar buffer2[20+1];

	uchar   PayPeriodBulan[20+1];
	uchar   PayPeriodTahun[20+1];
	uchar   PayPeriodFmt[20+1];
	uchar	PayPeriod[20+1];

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	MEM_ZERO(buffer2);

	MEM_ZERO(PayPeriodBulan);
	MEM_ZERO(PayPeriodTahun);
	MEM_ZERO(PayPeriodFmt);
	MEM_ZERO(PayPeriod);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	mapGet(traResponseInq, buffer, 999);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP,glProcInfo.szReq103, 13);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Nama,buffer+15, 30);

	memcpy(buffAmt,buffer+50,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Total, buffAmtFmt, 12);

	MEM_ZERO(buffer2);
	memcpy(buffer2, glRecvPack.szField102+2, 20);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoRek, buffer2+2, 20);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoRek, buffer2+2, 6);
	strcat(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoRek, "****");


	memcpy(PayPeriod, buffer+62, 6);
	memcpy(PayPeriodBulan, PayPeriod+4, 2);
	memcpy(PayPeriodTahun, PayPeriod, 4);
	memcpy(PayPeriodFmt, PayPeriodBulan, 2);
	strcat(PayPeriodFmt, " - ");
	strcat(PayPeriodFmt, PayPeriodTahun);
	strcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.PayPeriod, PayPeriodFmt);

	return TRUE;
}





uchar unPackPembayaran_PdamBkpn(void)
{
	uchar buffer[999];
	ulong len=0;
	uchar buffAmt[14+1];
	uchar buffAmtFmt[14+1];
	uchar bufferNoRek[30];



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

	uchar   PayPeriodBulan[20+1];
	uchar   PayPeriodTahun[20+1];
	uchar   PayPeriodFmt[20+1];

	

	MEM_ZERO(buffer);
	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	MEM_ZERO(bufferNoRek);

	/*
	MEM_ZERO(ProductId);
	MEM_ZERO(BillId);
	MEM_ZERO(CustName);
	MEM_ZERO(NumOfBill);
	MEM_ZERO(PayBallance);
	MEM_ZERO(PayPeriod);
	MEM_ZERO(PayPenalti);
	MEM_ZERO(PayTax);
	MEM_ZERO(TotalBayar);
	MEM_ZERO(NoRekAsal);
	MEM_ZERO(AdminFee);
	MEM_ZERO(OtherFee);
	MEM_ZERO(Kubikasi);
	*/
	
	MEM_ZERO(PayPeriodBulan);
	MEM_ZERO(PayPeriodTahun);
	MEM_ZERO(PayPeriodFmt);
	

	

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(bufferNoRek, glRecvPack.szField102+2, 28);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.NoRekAsal, bufferNoRek+2, 26);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.NoRekAsal, bufferNoRek+2, 6);
	strcat(glProcInfo.stTranLog.BRI.PEMB_PDAM.NoRekAsal, "****");

	mapGet(traResponseInq, buffer, 999);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.NoHP,glProcInfo.szReq103, 13);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Nama,buffer+20, 30);

	//memcpy(buffAmt,buffer+55,10);
	//fmtAmt(buffAmtFmt,buffAmt,0,",.");
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Total, buffAmtFmt, 12);

/*
		uchar	ProductId[4+1];
	uchar	BillId[16+1];
	uchar	CustName[30+1];
	uchar	NumOfBill[1+1];
	uchar	PayBallance[12+1];
	uchar	PayPeriod[16+1];
	uchar	PayPenalti[6+1]; YYYYMM
	uchar	PayTax[9+1];
	uchar	TotalBayar[12+1];
	*/
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.ProductId, buffer, 4);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.BillId, buffer+4, 16);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.CustName, buffer+20, 30);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.NumOfBill, buffer+50, 1);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.PayBallance, buffer+59, 10);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.PayPeriod, buffer+69, 6);
	memcpy(PayPeriod, buffer+69, 6);
	memcpy(PayPeriodBulan, PayPeriod+4, 2);
	memcpy(PayPeriodTahun, PayPeriod, 4);
	memcpy(PayPeriodFmt, PayPeriodBulan, 2);
	strcat(PayPeriodFmt, " - ");
	strcat(PayPeriodFmt, PayPeriodTahun);
	strcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.PayPeriod, PayPeriodFmt);
	
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.PayPenalti, buffer+75, 9);
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.PayTax, buffer+84, 10);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.Kubikasi, buffer+94, 17);
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.ReffNum, buffer+135, 15);


	
	//memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.TotalBayar, buffer, 12);

	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	memcpy(buffAmt,buffer+75,9);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.PayPenalti, buffAmtFmt, 12);

	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	memcpy(buffAmt,buffer+84,10);
	fmtAmt(buffAmtFmt,buffAmt,0,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.PayTax, buffAmtFmt, 12);

	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	//strcpy(glProcInfo.stTranLog.szAmount, glRecvPack.szBit4);
	strcpy(buffAmt, glRecvPack.szBit4);
	fmtAmt(buffAmtFmt,buffAmt,2,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.TotalBayar, buffAmtFmt, 14);

	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	memcpy(buffAmt,buffer+57,12);
	fmtAmt(buffAmtFmt,buffAmt,2,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.PayBallance, buffAmtFmt, 14);

	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	memcpy(buffAmt,buffer+111,12);
	fmtAmt(buffAmtFmt,buffAmt,2,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.AdminFee, buffAmtFmt, 12);

	MEM_ZERO(buffAmt);
	MEM_ZERO(buffAmtFmt);
	memcpy(buffAmt,buffer+123,12);
	fmtAmt(buffAmtFmt,buffAmt,2,",.");
	memcpy(glProcInfo.stTranLog.BRI.PEMB_PDAM.OtherFee, buffAmtFmt, 12);
	

	return TRUE;
}



uchar unPackSetorTunaiBankBkpn(void)
{
	uchar buffer[999];
	uchar bufferFee[30];
	uchar bufferNoRek[30];
	uchar bufferAmount[30];
	uchar bufferSaldo[30];
	ulong len=0;
	uchar bufferNoRekTujuan[30];

	MEM_ZERO(buffer);
	MEM_ZERO(bufferFee);
	MEM_ZERO(bufferNoRek);
	MEM_ZERO(bufferAmount);
	MEM_ZERO(bufferSaldo);
	MEM_ZERO(bufferNoRekTujuan);

	MEM_ZERO(buffer);

	len = PubChar2Long(glRecvPack.szBit48, 2, NULL);
	if(len == 0)
		return FALSE;

	if(len>sizeof(buffer))
		len = sizeof(buffer);

	memcpy(buffer, glRecvPack.szBit48+2, len);

	memcpy(bufferSaldo, buffer, 14);
	fmtAmt(glProcInfo.stTranLog.BRI.SetorPasti.SaldoAkhir, bufferSaldo, 0,",.");

	memcpy(bufferNoRek, glRecvPack.szField102+2, 28);
	//memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NoRekAsal, bufferNoRek+2, 26);
	memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NoRekAsal, bufferNoRek+2, 6);
	strcat(glProcInfo.stTranLog.BRI.SetorPasti.NoRekAsal, "****");

	memcpy(bufferAmount,glRecvPack.szBit4,10);
	fmtAmt(glProcInfo.stTranLog.BRI.SetorPasti.Setoran,bufferAmount,0,",.");


	memcpy(bufferNoRekTujuan, glRecvPack.szField103+2, 28);
	//memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NoRekAsal, bufferNoRek+2, 26);
	memcpy(glProcInfo.stTranLog.BRI.SetorPasti.NoRekTujuan, bufferNoRekTujuan+2, 10);
	

	return TRUE;
}




