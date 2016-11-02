#include "global.h"


const unsigned char bit_set_menu[] =
    {
        0x80, 	// = 1000 0000
        0x40, 	// = 0100 0000
        0x20, 	// = 0010 0000
        0x10, 	// = 0001 0000
        0x08, 	// = 0000 1000
        0x04, 	// = 0000 0100
        0x02, 	// = 0000 0010
        0x01		// = 0000 0001
    };

static void SelectHdlMenu(word MnuItm);
static void SubHdlMenu(word msg1, word msg2);

static int ExtractPAN(uchar *pszPAN, uchar *pszOutPan)
{
	int		iLength;

	iLength = strlen((char*) pszPAN);
	if( iLength<13 || iLength>19 )
	{
		return ERR_SWIPECARD;
	}

	memset(pszOutPan, '0', 16);
	memcpy(pszOutPan+4, pszPAN+iLength-13, 12);
	pszOutPan[16] = 0;

	return 0;
}

static int DetectCardEvent(uchar ucMode)
{
	//磁头上电、打开、清缓冲
	if( ucMode & CARD_SWIPED )
	{
		MagClose();
		MagOpen();
		MagReset();
	}

	if( ucMode & FALLBACK_SWIPE )
	{
		DispFallBackSwipeCard();
	}
	else if( (ucMode & CARD_SWIPED) && (ucMode & CARD_INSERTED) )
	{
		DispSwipeCard(FALSE);
	}
	else if( (ucMode & CARD_INSERTED)  )
	{
		if( !(ucMode & SKIP_DETECT_ICC) )
		{
			DispInsertOnly();
		}
	}
	else
	{
		DispSwipeOnly();
	}

    	kbflush();
	while( 1 )
	{
		if(!TimerCheck(0))
			return ERR_NO_DISP;

		if( PubKeyPressed() )
		{
			return CARD_KEYIN;		// 有按键事件
		}
		if( (ucMode & CARD_SWIPED) && (MagSwiped()==0) )
		{
			return CARD_SWIPED;		// 有刷卡事件
		}
		if( (ucMode & CARD_INSERTED) && ChkIfEmvEnable() )
		{
			if( ucMode & SKIP_DETECT_ICC )
			{
				return CARD_INSERTED;	// 有插入IC卡事件
			}
			else if( IccDetect(ICC_USER)==0 )
			{
				return CARD_INSERTED;	// 有插入IC卡事件
			}
		}
	}
}


void displayResponse(void){
	char RspCod[lenRspCod + 1];
    	word mnuItm;
	char ResponCode[21+1];
	
	MEM_ZERO(RspCod);
	MEM_ZERO(ResponCode);
	
	mapGetWord(traMnuItm, mnuItm);

	memcpy(RspCod,glProcInfo.stTranLog.szRspCode,2);
	sprintf(ResponCode,"  Response Code : %s ",RspCod);
	
	//untuk menu ECHO test display Response ditiadakan
	//if(txnType == trtEchoTest)
	//	return 1;
	//------------------------------------------

   	ScrCls();
    	if(strcmp(RspCod, "00") == 0)
    	{
    		
		switch(glProcInfo.stTranLog.ucTranType)
    		{
		case TRANS_TYPE_LOGON:
			ScrPrint(0, 4, ASCII, "%21s", " LOGON SUCCESS    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_TEST_KONEKSI:
			ScrPrint(0, 3, ASCII, "%21s", " TEST KONEKSI    ");
			ScrPrint(0, 4, ASCII, "%21s", " BERHASIL      ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		default:
			ScrPrint(0, 3, ASCII, "%21s", " APPROVED      ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		PubWaitKey(1);
		return;
    	}

	else if(strcmp(RspCod, "01") == 0)
	{
		
			ScrPrint(0, 4, ASCII, "%21s", " CALL YOUR BANK  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
		
			
		
		
	}

	else if(strcmp(RspCod, "02") == 0) // pln
	{
		//MEM_ZERO(bit13);
		//MEM_ZERO(bulan);
		//MAPGET(traRspBit13, bit13, lblKO);
		//memcpy(bulan, bit13, 2);
		//bin2hex(bulan, bit13, 1);
		//bln = atoi(bulan);
	
		
			ScrPrint(0, 3, ASCII, "%21s", " PIN SALAH,      ");
			ScrPrint(0, 4, ASCII, "%21s", " REKENING DIBLOKIR     ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			
		
		
		
	}

	else if(strcmp(RspCod, "03") == 0)
	{
		switch(glProcInfo.stTranLog.ucTranType)
		{
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
//		case TRANS_TYPE_FREN_MOBI_HEPI: //ditutup 6 Januari 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:
			ScrPrint(0, 3, ASCII, "%21s", " KONEKSI MITRA ");
			ScrPrint(0, 4, ASCII, "%21s", " KE TELCO PUTUS  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_INQ_PLNPASCABAYAR:
		case TRANS_TYPE_PLNPASCABAYAR:
		case TRANS_TYPE_INQ_PLNPRABAYAR:
		case TRANS_TYPE_PLNPRABAYAR:
			ScrPrint(0, 3, ASCII, "%21s", " TOTAL KWH    ");
			ScrPrint(0, 4, ASCII, "%21s", " MELEBIHI BATAS  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_INQ_TRFANTARBANK:
		case TRANS_TYPE_TRFANTARBANK:
		case TRANS_TYPE_INFOSALDO_BANKLAIN:
			ScrPrint(0, 3, ASCII, "%21s", " INVALID    ");
			ScrPrint(0, 4, ASCII, "%21s", " MERCHANT  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		default:
			ScrPrint(0, 3, ASCII, "%21s", " PANJANG PIN    ");
			ScrPrint(0, 4, ASCII, "%21s", " KURANG DARI       ");
			ScrPrint(0, 6, ASCII, "%21s", " 6DIGIT       ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "04") == 0) 
	{
	
			ScrPrint(0, 3, ASCII, "%21s", " PEMBELIAN    ");
			ScrPrint(0, 4, ASCII, "%21s", " DILUAR RANGE    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
		
		
		
	}

	else if(strcmp(RspCod, "05") == 0)
	{
		switch(mnuItm)
		{
		default:
			//ScrPrint(0, 3, ASCII, "%21s", " DO NOT HONOUR    ");
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI DITOLAK");
			ScrPrint(0, 4, ASCII, "%21s", " OLEH ISSUER");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "06") == 0)
	{
		switch(mnuItm)
		{
	

		default:
			//ScrPrint(0, 3, ASCII, "%21s", " PEMBAYARAN GAGAL    ");
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI ERROR    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "07") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " PEMBAYARAN GAGAL    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "08") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			DefaultGagal();
		}
		  
		
	}

	else if(strcmp(RspCod, "09") == 0) 
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " KARTU PETUGAS    ");
			ScrPrint(0, 4, ASCII, "%21s", " BELUM TERDAFTAR    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "10") == 0)
	{
		switch(glProcInfo.stTranLog.ucTranType)
		{
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
		//case TRANS_TYPE_FREN_MOBI_HEPI: // DITUTUP 6 jANUARI 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR HANDPHONE   ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK TERDAFTAR   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_AKTIFASIKARTU_PETUGAS:
		case TRANS_TYPE_AKTIFASIKARTU_NASABAH:
		case TRANS_TYPE_AKTIFASIKARTU_PRINT:
			ScrPrint(0, 3, ASCII, "%21s", " KARTU CLOSED   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_CASH_IN:
		case TRANS_TYPE_CASHOUT:			
			ScrPrint(0, 3, ASCII, "%21s", " PASSWORD    ");
			ScrPrint(0, 4, ASCII, "%21s", " SALAH    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		default:
			ScrPrint(0, 3, ASCII, "%21s", " KARTU DITUTUP   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "11") == 0)
	{
		switch(glProcInfo.stTranLog.ucTranType)
		{	
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
		//case TRANS_TYPE_FREN_MOBI_HEPI: // ditutup 6 Januari 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR HANDPHONE   ");
			ScrPrint(0, 4, ASCII, "%21s", " DI BLOKIR   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_AKTIFASIKARTU_NASABAH:
		case TRANS_TYPE_AKTIFASIKARTU_PRINT:
		case TRANS_TYPE_REISSUE_PIN_PETUGAS:
		case TRANS_TYPE_REISSUE_PIN_NASABAH:
		//RENEWAL
			ScrPrint(0, 3, ASCII, "%21s", " KARTU TELAH AKTIF  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_INQ_TRFANTARBANK:
		case TRANS_TYPE_TRFANTARBANK:
		case TRANS_TYPE_INFOSALDO_BANKLAIN:
			ScrPrint(0, 3, ASCII, "%21s", " APPROVED   ");
			ScrPrint(0, 4, ASCII, "%21s", " (VIP)     ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
		case TRANS_TYPE_CASH_IN:
		case TRANS_TYPE_CASHOUT:			
			ScrPrint(0, 4, ASCII, "%21s", " PIN SALAH    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR HANDPHONE   ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK ADA      ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "12") == 0)
	{
		/*
		switch(glProcInfo.stTranLog.ucTranType)
		{
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
		//case TRANS_TYPE_FREN_MOBI_HEPI: // ditutup 6 Januari 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR HANDPHONE ");
			ScrPrint(0, 4, ASCII, "%21s", " EXPIRED      ");
			break;
		case TRANS_TYPE_INQ_TRFANTARBANK:
		case TRANS_TYPE_TRFANTARBANK:
		case TRANS_TYPE_INFOSALDO_BANKLAIN:
			ScrPrint(0, 3, ASCII, "%21s", " NVALID TRANSACTION   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR HANDPHONE ");
			ScrPrint(0, 4, ASCII, "%21s", " EXPIRED      ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		*/


		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK VALID");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "13") == 0)
	{	
		switch(mnuItm)
		{
	
		default:
			//DefaultGagal();
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " NOMINAL TRANSAKSI");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK SESUAI");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
		}	
		
		
	}

    	else if(strcmp(RspCod, "14") == 0)
	{
		switch(mnuItm)
		{

	
		/*
		case mnuFren20rb:
		case mnuFren25rb:
		case mnuFren50rb:
		case mnuFren100rb:
		case mnuFren150rb:
		case mnuFren200rb:
		case mnuFren300rb:
		case mnuFren500rb:
			ScrPrint(0, 3, ASCII, "%21s", "   NOMOR HANDPHONE   ");
			ScrPrint(0, 4, ASCII, "%21s", "        SALAH        ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		*/ //ditutup 6 Januari2014
		
		
		default:
			//ScrPrint(0, 3, ASCII, "%21s", " NOMOR           ");
			//ScrPrint(0, 4, ASCII, "%21s", " TIDAK DITEMUKAN   ");
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " KARTU ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK TERDAFTAR ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;		
		}
		
		
	}

	else if(strcmp(RspCod, "15") == 0)
	{
		switch(mnuItm)
		{
		
		
		default:
			ScrPrint(0, 5, ASCII, "%21s", " INVALID CARD NUMBER ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "16") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			DefaultGagal();
		}
		
		
	}

	else if(strcmp(RspCod, "17") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " AMOUNT   ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK VAILD   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "18") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " CUSTOMER   ");
			ScrPrint(0, 4, ASCII, "%21s", " DISPUTE     ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "19") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RE-ENTER   ");
			ScrPrint(0, 4, ASCII, "%21s", " TRANSACTION     ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "20") == 0)
	{
		switch(glProcInfo.stTranLog.ucTranType)
		{
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
	//	case TRANS_TYPE_FREN_MOBI_HEPI: // ditutup 6 Januari 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:
			ScrPrint(0, 3, ASCII, "%21s", " NOMINAL    ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK TERSEDIA      ");
			break;
		case TRANS_TYPE_INQ_TRFANTARBANK:
		case TRANS_TYPE_TRFANTARBANK:
		case TRANS_TYPE_INFOSALDO_BANKLAIN:
			ScrPrint(0, 3, ASCII, "%21s", " NVALID RESPONSE   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NOMINAL    ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK TERSEDIA      ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "21") == 0)
	{
		switch(glProcInfo.stTranLog.ucTranType)
		{
		case TRANS_TYPE_AXIS:
		case TRANS_TYPE_ESIA:
		//case TRANS_TYPE_FREN_MOBI_HEPI: ditutup 6 Januari 2014
		case TRANS_TYPE_IM3:
		case TRANS_TYPE_MENTARI:
		case TRANS_TYPE_SIMPATI_AS:
		case TRANS_TYPE_SMARTFREN:
		case TRANS_TYPE_THREE:
		case TRANS_TYPE_XL:
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI GAGAL   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_INQ_TRFANTARBANK:
		case TRANS_TYPE_TRFANTARBANK:
		case TRANS_TYPE_INFOSALDO_BANKLAIN:
			ScrPrint(0, 3, ASCII, "%21s", " NO ACTION   ");
			ScrPrint(0, 4, ASCII, "%21s", " TAKEN       ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		case TRANS_TYPE_CASH_IN:
		case TRANS_TYPE_CASHOUT:		
			ScrPrint(0, 3, ASCII, "%21s", " CUSTOMER      ");
			ScrPrint(0, 4, ASCII, "%21s", " BELUM REGISTRASI  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		default:
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI GAGAL   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "22") == 0)
	{
		switch(mnuItm)
		{
	
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " SUSPECTED   ");
			ScrPrint(0, 4, ASCII, "%21s", " MALFUNCTION     ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "23") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI   ");
			ScrPrint(0, 4, ASCII, "%21s", " GAGAL     ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
	}

	else if(strcmp(RspCod, "24") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " SUSPECTED   ");
			ScrPrint(0, 4, ASCII, "%21s", " MALFUNCTION     ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
	}	
	else if(strcmp(RspCod, "25") == 0) 
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " DATA KARTU   ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK DITEMUKAN    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "26") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			DefaultGagal();
		}
	}
	else if(strcmp(RspCod, "27") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			DefaultGagal();
		}
		
		
	 }

	else if(strcmp(RspCod, "30") == 0) 
	{
		switch(mnuItm)
		{
		default:
			//ScrPrint(0, 3, ASCII, "%21s", " FORMAT ERROR   ");
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " FORMAT TRANSAKSI");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK SESUAI ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "31") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " BANK NOT SUPPORTED  ");
			ScrPrint(0, 4, ASCII, "%21s", " BY SWITCH  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "32") == 0)
	{
	switch(mnuItm)
		{
	
		default:
			DefaultGagal();
		}
		
		
	}

	else if(strcmp(RspCod, "33") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " EXPIRED CARD  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "34") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " KARTU EXPIRED  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "36") == 0) // trans antar bank
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESTRICTED CARD  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "38") == 0)
	{
		switch(mnuItm)
		{	
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " SALAH PIN 3X ");
			ScrPrint(0, 4, ASCII, "%21s", " KARTU DIBLOKIR  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);		
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "39") == 0)
	{
		switch(mnuItm)
		{
		default:
			//ScrPrint(0, 3, ASCII, "%21s", " NO CREDIT  ");
			ScrPrint(0, 3, ASCII, "%21s", " INVALID  ");
			//ScrPrint(0, 4, ASCII, "%21s", " ACCOUNT  ");
			ScrPrint(0, 4, ASCII, "%21s", " TERMINAL  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "40") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " REQUESTED FUNCTION ");
			ScrPrint(0, 4, ASCII, "%21s", " NOT SUPPORTED  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "41") == 0)
	{
		switch(mnuItm)
		{	
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " KARTU DIBLOKIR ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "43") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " STOLEN CARD ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "47") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " KARTU PETUGAS ");
			ScrPrint(0, 4, ASCII, "%21s", " DIBLOKIR  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "50") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " PIN SALAH ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "51") == 0)
	{
		switch(mnuItm)
		{	
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " SALDO TIDAK CUKUP ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "52") == 0)
	{
		switch(mnuItm)
		{
		default:
			//ScrPrint(0, 3, ASCII, "%21s", " NO CHEQUING ");
			//ScrPrint(0, 4, ASCII, "%21s", " ACCOUNT ");
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " REKENING GIRO ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK DITEMUKAN ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}
	
	else if(strcmp(RspCod, "53") == 0)
	{
		switch(mnuItm)
		{	
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " REKENING ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK DITEMUKAN ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "54") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			DefaultGagal();
		}
		
		
	}

	else if(strcmp(RspCod, "55") == 0)
	{
		switch(mnuItm)
		{	
			
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", "PIN ANDA SALAH");
			ScrPrint(0, 4, ASCII, "%21s", "MOHON CEK KEMBALI");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "56") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NO CARD RECORD ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "57") == 0) 
	{
		switch(mnuItm)
		{	
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI DITOLAK");
			//ScrPrint(0, 4, ASCII, "%21s", " TIDAK DIIZINKAN ");
			ScrPrint(0, 4, ASCII, "%21s", " OLEH ISSUER");
			ScrPrint(0, 5, ASCII, "%21s", " (REK PASIF)");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "58") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 2, ASCII, "%21s", " TRANSACTION NOT ");
			ScrPrint(0, 3, ASCII, "%21s", " PERMITTED ");
			ScrPrint(0, 4, ASCII, "%21s", " TO TERMINAL  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "59") == 0)
	{
		switch(mnuItm)
		{	
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " KARTU TIDAK AKTIF ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "61") == 0)
	{
		switch(mnuItm)
		{	
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", "TRANSAKSI");
			ScrPrint(0, 4, ASCII, "%21s", "MELEBIHI LIMIT HARIAN");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);	
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "62") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " KARTU ANDA");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK AKTIF");
			ScrPrint(0, 5, ASCII, "%21s", " (HOT CARD)");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "63") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " SECURITY VIOLATION");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "64") == 0)
	{
		switch(mnuItm)
		{	
	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR REKENING  ");
			ScrPrint(0, 4, ASCII, "%21s", " SUDAH TERDAFTAR  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "65") == 0)
	{
		switch(mnuItm)
		{	
		
		default:
			//DefaultGagal();
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " TARIK TUNAI");
			ScrPrint(0, 4, ASCII, "%21s", "MELEBIHI LIMIT HARIAN");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
		}
		
		
	}

	else if(strcmp(RspCod, "67") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " HARD CAPTURE    ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "68") == 0)
	{
		switch(mnuItm)
		{
		
	
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			//ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI   ");
			//ScrPrint(0, 4, ASCII, "%21s", " SEDANG DIPROSES  ");
			ScrPrint(0, 3, ASCII, "%21s", " TIME-OUT ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;				
		}
		
		
	}

	else if(strcmp(RspCod, "69") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR   ");
			ScrPrint(0, 4, ASCII, "%21s", " ISO USE   "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "70") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR   ");
			ScrPrint(0, 4, ASCII, "%21s", " ISO USE   "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}
	else if(strcmp(RspCod, "71") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR   ");
			ScrPrint(0, 4, ASCII, "%21s", " ISO USE   "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "72") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR   ");
			ScrPrint(0, 4, ASCII, "%21s", " ISO USE   "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "73") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR   ");
			ScrPrint(0, 4, ASCII, "%21s", " ISO USE   "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "74") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR   ");
			ScrPrint(0, 4, ASCII, "%21s", " ISO USE   "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "75") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 2, ASCII, "%21s", " ALLOWABLE NUMBER  "); 
			ScrPrint(0, 3, ASCII, "%21s", " OF PIN  ");
			ScrPrint(0, 4, ASCII, "%21s", " TRIES EXCEEDED  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}
	
	else if(strcmp(RspCod, "76") == 0)
	{
		switch(mnuItm)
		{	
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			//ScrPrint(0, 3, ASCII, "%21s", " REKENING   ");
			//ScrPrint(0, 4, ASCII, "%21s", " TIDAK DITEMUKAN   "); 
			ScrPrint(0, 3, ASCII, "%21s", " REKENING TUJUAN ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK TERDAFTAR "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "77") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			//ScrPrint(0, 3, ASCII, "%21s", " INVALID  ");
			//ScrPrint(0, 4, ASCII, "%21s", " FROM ACCOUNT  ");
			ScrPrint(0, 3, ASCII, "%21s", "TRANSAKSI PEMBAYARAN");
			ScrPrint(0, 4, ASCII, "%21s", "MELEBIHI LIMIT HARIAN");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "78") == 0)
	{	
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " REKENING DITUTUP  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "79") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED  FOR ");
			ScrPrint(0, 4, ASCII, "%21s", " PRIVATE USE  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "80") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR FLIGHT  ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK VALID "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "81") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR  ");
			ScrPrint(0, 4, ASCII, "%21s", " PRIVATE USE  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}
	else if(strcmp(RspCod, "82") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR  ");
			ScrPrint(0, 4, ASCII, "%21s", " PRIVATE USE  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "83") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR  ");
			ScrPrint(0, 4, ASCII, "%21s", " PRIVATE USE  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "84") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR  ");
			ScrPrint(0, 4, ASCII, "%21s", " PRIVATE USE  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "85") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR  ");
			ScrPrint(0, 4, ASCII, "%21s", " PRIVATE USE  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "86") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR  ");
			ScrPrint(0, 4, ASCII, "%21s", " PRIVATE USE  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "87") == 0)
	{
		switch(mnuItm)
		{	
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			//ScrPrint(0, 3, ASCII, "%21s", " RESERVED FOR  ");
			//ScrPrint(0, 4, ASCII, "%21s", " PRIVATE USE  ");
			ScrPrint(0, 3, ASCII, "%21s", " TAGIHAN ");
			ScrPrint(0, 4, ASCII, "%21s", " BELUM TERSEDIA ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "88") == 0)
	{
		switch(mnuItm)
		{
		
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " TAGIHAN  ");
			ScrPrint(0, 4, ASCII, "%21s", " TELAH DIBAYAR "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;	
		}
		
		
	}

	else if(strcmp(RspCod, "89") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			//ScrPrint(0, 3, ASCII, "%21s", " KONEKSI KE PIHAK TIGA  ");
			//ScrPrint(0, 4, ASCII, "%21s", " PUTUS (BAD TERMINAL ID)");
			ScrPrint(0, 3, ASCII, "%21s", " TANGGAL SETTLEMENT ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK SESUAI ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;	
		}
		
		
	}

	else if(strcmp(RspCod, "90") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL ");
			ScrPrint(0, 4, ASCII, "%21s", " SEDANG PROSES CUTOFF"); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;	
		}
		
		
	}

	else if(strcmp(RspCod, "91") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			//ScrPrint(0, 3, ASCII, "%21s", " ISSUER OR SWITCH  ");
			//ScrPrint(0, 4, ASCII, "%21s", " INOPERATIVE  "); 
			ScrPrint(0, 3, ASCII, "%21s", " HOST TUJUAN ");
			ScrPrint(0, 4, ASCII, "%21s", " SEDANG OFFLINE  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "92") == 0)
	{
		switch(mnuItm)
		{
			
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			//ScrPrint(0, 3, ASCII, "%21s", " UNABLE TO  ");
			//ScrPrint(0, 4, ASCII, "%21s", " ROUTE TRANSACTION  "); 
			ScrPrint(0, 3, ASCII, "%21s", " JARINGAN SEDANG ");
			ScrPrint(0, 4, ASCII, "%21s", " MENGALAMI GANGGUAN ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "93") == 0)
	{
		switch(mnuItm)
		{	
	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " DOUBLE SEQUENCE  ");
			ScrPrint(0, 4, ASCII, "%21s", " NUMBER  "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "94") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " JARINGAN KOMUNIKASI  ");
			ScrPrint(0, 4, ASCII, "%21s", " DOWN      "); 
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "96") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			//ScrPrint(0, 3, ASCII, "%21s", " KONEKSI KE PIHAK TIGA");
			//ScrPrint(0, 4, ASCII, "%21s", " PUTUS     ");
			ScrPrint(0, 3, ASCII, "%21s", " SISTEM DALAM ");
			ScrPrint(0, 4, ASCII, "%21s", " PROSES MAINTENANCE ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "97") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " CRYPTOGRAPHIC ERROR");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "98") == 0) 
	{
		switch(mnuItm)

		{
		
		default:		
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI GAGAL   ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "99") == 0) 
	{
		switch(mnuItm)
		{
		
		default:
			DefaultGagal();
		}
		
		
	}

	else if(strcmp(RspCod, "N6") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " REKENING TUJUAN ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK TERDAFTAR ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "N9") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " REKENING DIBLOKIR ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "Q5") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI MELEBIHI ");
			ScrPrint(0, 4, ASCII, "%21s", " LIMIT HARIAN ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "T4") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 0, ASCII, "%21s", " TRANSAKSI GAGAL");
			ScrPrint(0, 3, ASCII, "%21s", " TIME-OUT");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}


	else if(strcmp(RspCod, "IB") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			DefaultGagal();
		}
		
		
	}

	else if(strcmp(RspCod, "AE") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			DefaultGagal();
		}
		
		
	}

	else if(strcmp(RspCod, "BA") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			DefaultGagal();
		}
		
		
	}

	else if(strcmp(RspCod, "CL") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " STATUS KARTU  ");
			ScrPrint(0, 4, ASCII, "%21s", " CLOSED  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "FE") == 0)
	{
		switch(mnuItm)
		{
	
		default:
			DefaultGagal();
		}
		
		
	}

	else if(strcmp(RspCod, "HP") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " REGISTRASI NOMOR HP ");
			ScrPrint(0, 3, ASCII, "%21s", " SUDAH MENCAPAI ");
			ScrPrint(0, 3, ASCII, "%21s", " MAKSIMAL  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "IB") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NVALID BRANCH  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "IP") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " PASSWORD SALAH  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "NC") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " BERHASIL ");
			ScrPrint(0, 3, ASCII, "%21s", " LAKUKAN REISSUE PIN ");
			ScrPrint(0, 3, ASCII, "%21s", " (PIN GAGAL)  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "NF") == 0)
	{
		switch(mnuItm)
		{	
	
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NO REKENING TUJUAN ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK DITEMUKAN ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "NO") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR KARTU  ");
			ScrPrint(0, 4, ASCII, "%21s", " KURANG DARI 12  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}
	
	else if(strcmp(RspCod, "OT") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " TRANSAKSI MELEBIHI  ");
			ScrPrint(0, 4, ASCII, "%21s", " LIMIT TOPUP BULANAN  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "Q1") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 3, ASCII, "%21s", " KONEKSI TERPUTUS  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "Q2") == 0)
	{	
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " NOMOR KARTU KREDIT ");
			ScrPrint(0, 4, ASCII, "%21s", " SALAH  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "Q4") == 0)
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " TIMEOUT       ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "RP") == 0)
	{
		switch(mnuItm)
		{
		
		default:
			ScrPrint(0, 2, ASCII, "%21s", " REGISTRASI NOMOR ");
			ScrPrint(0, 3, ASCII, "%21s", " REKENING SUDAH ");
			ScrPrint(0, 4, ASCII, "%21s", " MENCAPAI MAKSIMAL ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}
	
	else if(strcmp(RspCod, "SA") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 2, ASCII, "%21s", " KARTU SUDAH ");
			ScrPrint(0, 3, ASCII, "%21s", " DIAKTIFASI DI-HOST ");
			ScrPrint(0, 4, ASCII, "%21s", " TIDAK PERLU ");
			ScrPrint(0, 5, ASCII, "%21s", " AKTIFASI LAGI ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else if(strcmp(RspCod, "ST") == 0) 
	{
		switch(mnuItm)
		{
		default:
			ScrPrint(0, 3, ASCII, "%21s", " STAN IN  ");
			ScrPrint(0, 7, ASCII, "%21s", ResponCode);
			break;
		}
		
		
	}

	else
	{	
		ScrPrint(0, 3, ASCII, "%21s", " RESPON CODE NEW   ");
		ScrPrint(0, 7, ASCII, "%21s", ResponCode);

	}

	PubWaitKey(2);
	PubBeepErr();
	PubWaitKey(5);

}


void DefaultGagal(void)
{
	char RspCod[lenRspCod + 1];
	char ResponCode[21+1];
	
	MEM_ZERO(RspCod);
	MEM_ZERO(ResponCode);

	ScrCls();
	memcpy(RspCod,glProcInfo.stTranLog.szRspCode,2);
	sprintf(ResponCode,"  Response Code : %s ",RspCod);
	PubDispStringASCII("TRANSAKSI GAGAL", DISP_LINE_CENTER|4);
	PubDispStringASCII(ResponCode, DISP_LINE_CENTER|7);
}

#ifdef BANK_BRI

void Function_11(void)
	{
	int ret;
	uchar ucRet;
	char  password[6+1];
	
	char *menu[]={
		"Setting Menu",
		"Load Parameter",
		//"Set Key In",
		//"Download Key",
		//"Line Connection",
		"GPRS Setup",
		"Tes Koneksi",
		"Set Tanggal",
		//"Set Jam",
		"Cetak Acquirer",
		"Info Acquirer",
		"Set Brizzi",
		//"Enable Menu",
		"Cetak Versi",
		//"SPV Menu",
		//"Init SAM",
		//"Set Init SAM",
		"Refresh GPRS",
		"Restart EDC",
		
		0,
	};

	MEM_ZERO(password);
	
	ScrCls();
	PubDispString("ENTER PASSWORD", DISP_LINE_CENTER|4);
	//ScrPrint(0, 3, ASCII, "%21s", "   ENTER PASSWORD   ");
	ucRet = PubGetString(PASS_IN, 0,6 , password, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		goto lblEnd;
	}

	if(memcmp(password,"000000",6)!=0)
	{
		ScrCls();
		ScrPrint(0, 3, CFONT, "%s", " PASSWORD SALAH   ");

		PubBeepErr();
		PubWaitKey(3);
		goto lblEnd;
	}
	
	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch (ret){
		case 1:
			TmsTeliumDownLoad();
			break;
		//case 2:
			//BlowFishSet();
			//break;
		//case 2:
			//SetKeyIn();
			//break;
		case 2:
			//Gprs Setup
			SetTcpIpSharedPara(&glTlmSysParam.stTxnCommCfg);
			SetAPN();
			//SetWirelessParamNew(&glTlmSysParam._TxnWirlessPara);
			SyncWirelessParam(&glTlmSysParam._TmsWirlessPara, &glTlmSysParam._TxnWirlessPara);
			CommOnHook(TRUE);
			DispWait();
			CommInitModule(&glTlmSysParam.stTxnCommCfg);
			SaveTlmSysParam();
		
			ProcUserMsg();
			break;
		case 3:
			TransTestKoneksi();
			break;
		case 4:
			SetSysDate();
			break;
		//case 5:
			//SetSysTimeNew();
			//break;
		case 5:
			PrintParamNew();
			break;
		case 6:
			InfoAcquirers(PM_MEDIUM);
			break;
			
		//case 5:
			//PrintParamNew();
			//SetGetSysTraceNo(PM_MEDIUM);
		//	break;
		//case 6:
			//InfoAcquirers(PM_MEDIUM);
			//ClearAllRecord();
		//	break;
		case 7:
			SetBrizzi();
			//Set Disc Brizzi
			//SetBrizziDisc();
			//ClearReversal();
			break;
		//case 8:
			//OnOffMenu();
			//break;
		case 8:
			PrnVersi();
			//DispVersion();
			//SetAPN();
			break;
		//case 9: //SPV menu
		//SuperviserMenu();
			//break;
		//case 9:
			//SAMPrepair(TRUE);
			//BlowFishSet();
			//break;
		//case 10:
			//InitBrizzi();
			//break;
		case 9:
			ProcUserMsg();
			break;
		case 10:
			Reboot();
			break;
		
		/*
		case 13:
			RefreshGPRS();
			break;
		case 14:
			//OnOffMenu();
			break;
		case 15:
			//PrintMenuNew();
			break;
		case 16:
			TransLogon();
			CommOnHook(FALSE);
			break;
		case 17:
			SAMPrepair(TRUE);
			break;
		case 18:
			//set init sam 
			break;
		case 19:
			//PrintEmvErrLog();
			break;
		case 20:
			//BlowFishSet();
			break;
		case 21:
			//DebugSetNew();
			break;
		case 22:
			//
			break;
		*/
		default:
			break;
	}

lblEnd:
	;
}

void Function_22(void)
	{
	int ret;
	uchar ucRet;

	char  password[6+1];
	
	char *menu[]={
		"Debug Menu",
		"Set Koneksi",
		//"Cetak Acquirer",
		//"Info Acquirer",
		"Debug EMV",
		//"Download Key",
		"Line Encryption",
		//"Logon",
		"Debug Iso Msg",
		//"Debug Prepaid",
		//"Load Parameter",
		"SPV Menu",
		"Menu STAN",
		"Set Menu",
		"Set Key In",
		//"Set TID",
		0,
	};

	MEM_ZERO(password);
	
	ScrCls();
	PubDispString("ENTER PASSWORD", DISP_LINE_CENTER|4);
	//ScrPrint(0, 3, ASCII, "%21s", "   ENTER PASSWORD   ");
	ucRet = PubGetString(PASS_IN, 0,6 , password, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		goto lblEnd;
	}

	if(memcmp(password,"274653",6)!=0)
		{
		ScrCls();
		ScrPrint(0, 3, CFONT, "%s", " PASSWORD SALAH   ");

		PubBeepErr();
		PubWaitKey(3);
		goto lblEnd;
		}
	
	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch(ret){
		case 1:	
			SetSysCommParam(PM_MEDIUM);
			break;
		//case 2:
			//PrintParamNew();
			//break;
		//case 3:
			//InfoAcquirers(PM_MEDIUM);
			//break;
		case 2:
			DebugEMVICCSet();
			break;
		case 3://Download Key
			BlowFishSet();
			break;
		//case 6:
			//TransLogon();
			//break;
		case 4:
			DebugIsoReqSet();
			DebugIsoResSet();
			break;
		//case 8:
			//Debug Prepaid
			//break;
		//case 8:
			//TmsTeliumDownLoad();
			//break;
		case 5: //SPV Menu
			SuperviserMenu(PM_MEDIUM);
			break;
		case 6://Merchant Menu
			MerchantMenu();
			break;
		case 7:
			OnOffMenu();
			break;
		case 8:
			SetKeyIn();
			break;
		//case 10:
		//	SetTIDMID(PM_MEDIUM);
		//	break;
		default:
			break;
	}

lblEnd:
	;
}

/*
void Function_11(void)
	{
	int ret;
	uchar ucRet;
	char  password[6+1];
	
	char *menu[]={
		"Setting Menu",
		"Load Parameter",
		//"Download Key",
		//"Line Connection",
		"GPRS Setup",
		"Tes Koneksi",
		"Set Tanggal",
		"Set Jam",
		//"Cetak Acquirer",
		//"Info Acquirer",
		"Set Disc Brizzi",
		//"Enable Menu",
		"Cetak Versi",
		//"SPV Menu",
		"Init SAM",
		"Set Init SAM",
		"Refresh GPRS",
		"Restart EDC",
		0,
	};

	MEM_ZERO(password);
	
	ScrCls();
	PubDispString("ENTER PASSWORD", DISP_LINE_CENTER|4);
	//ScrPrint(0, 3, ASCII, "%s", "   ENTER PASSWORD   ");
	ucRet = PubGetString(PASS_IN, 0,6 , password, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		goto lblEnd;
	}

	if(memcmp(password,"000000",6)!=0)
	{
		ScrCls();
		ScrPrint(0, 3, CFONT, "%s", " PASSWORD SALAH   ");

		PubBeepErr();
		PubWaitKey(3);
		goto lblEnd;
	}
	
	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch (ret){
		case 1:
			TmsTeliumDownLoad();
			break;
		//case 2:
			//BlowFishSet();
			//break;
		case 2:
			//Gprs Setup
			SetTcpIpSharedPara(&glTlmSysParam.stTxnCommCfg);
			SetAPN();
			//SetWirelessParamNew(&glTlmSysParam._TxnWirlessPara);
			SyncWirelessParam(&glTlmSysParam._TmsWirlessPara, &glTlmSysParam._TxnWirlessPara);
			CommOnHook(TRUE);
			DispWait();
			CommInitModule(&glTlmSysParam.stTxnCommCfg);
			SaveTlmSysParam();
		
			ProcUserMsg();
			break;
		case 3:
			TransTestKoneksi();
			break;
		case 4:
			SetSysDate();
			break;
		case 5:
			SetSysTimeNew();
			break;
		//case 5:
			//PrintParamNew();
			//SetGetSysTraceNo(PM_MEDIUM);
		//	break;
		//case 6:
			//InfoAcquirers(PM_MEDIUM);
			//ClearAllRecord();
		//	break;
		case 6:
			//Set Disc Brizzi
			SetBrizziDisc();
			//ClearReversal();
			break;
		//case 8:
			//OnOffMenu();
			//break;
		case 7:
			PrnVersi();
			//DispVersion();
			//SetAPN();
			break;
		//case 9: //SPV menu
		//SuperviserMenu();
			//break;
		case 8:
			SAMPrepair(TRUE);
			//BlowFishSet();
			break;
		case 9:
			InitBrizzi();
			break;
		case 10:
			ProcUserMsg();
			break;
		case 11:
			Reboot();
			break;
			
		/*
		case 13:
			RefreshGPRS();
			break;
		case 14:
			//OnOffMenu();
			break;
		case 15:
			//PrintMenuNew();
			break;
		case 16:
			TransLogon();
			CommOnHook(FALSE);
			break;
		case 17:
			SAMPrepair(TRUE);
			break;
		case 18:
			//set init sam 
			break;
		case 19:
			//PrintEmvErrLog();
			break;
		case 20:
			//BlowFishSet();
			break;
		case 21:
			//DebugSetNew();
			break;
		case 22:
			//
			break;
		
		default:
			break;
	}

lblEnd:
	;
}

void Function_22(void)
	{
	int ret;
	uchar ucRet;

	char  password[6+1];
	
	char *menu[]={
		"Debug Menu",
		"Set Koneksi",
		"Cetak Acquirer",
		"Info Acquirer",
		"Debug EMV",
		//"Download Key",
		"Line Encryption",
		//"Logon",
		"Debug Iso Msg",
		//"Debug Prepaid",
		//"Load Parameter",
		"SPV Menu",
		"Merchant Menu",
		"Disable Menu",
		"Set TID",
		0,
	};

	MEM_ZERO(password);
	
	ScrCls();
	PubDispString("ENTER PASSWORD", DISP_LINE_CENTER|4);
	//ScrPrint(0, 3, ASCII, "%s", "   ENTER PASSWORD   ");
	ucRet = PubGetString(PASS_IN, 0,6 , password, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		goto lblEnd;
	}

	if(memcmp(password,"274653",6)!=0)
		{
		ScrCls();
		ScrPrint(0, 3, CFONT, "%s", " PASSWORD SALAH   ");

		PubBeepErr();
		PubWaitKey(3);
		goto lblEnd;
		}
	
	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch(ret){
		case 1:	
			SetSysCommParam(PM_MEDIUM);
			break;
		case 2:
			PrintParamNew();
			break;
		case 3:
			InfoAcquirers(PM_MEDIUM);
			break;
		case 4:
			DebugEMVICCSet();
			break;
		case 5://Download Key
			BlowFishSet();
			break;
		//case 6:
			//TransLogon();
			//break;
		case 6:
			DebugIsoReqSet();
			DebugIsoResSet();
			break;
		//case 8:
			//Debug Prepaid
			//break;
		//case 8:
			//TmsTeliumDownLoad();
			//break;
		case 7: //SPV Menu
			SuperviserMenu(PM_MEDIUM);
			break;
		case 8://Merchant Menu
			MerchantMenu();
			break;
		case 9:
			OnOffMenu();
			break;
		case 10:
			SetTIDMID(PM_MEDIUM);
			break;
		default:
			break;
	}

lblEnd:
	;
}

*/
void SetBrizzi(void)
{
	int ret;
	char *menu[]={
		"Set Brizzi>",
		//"INFO",
		//"Info Set Brizzi",
		"Set Disk Brizzi",
		//"Init SAM",
		"Set Akum Debit",
		"Set Saldo Min",
		
		0,
	};

	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch(ret){
		//case 1:	
			//InfoMerchant();
			//break;
		//case 1:
			//ViewBrizzi();
			//break;
#ifdef BANK_BRI
		case 1:
			SetBrizziDisc();
			break;
		//case 3:
			//SAMPrepair(TRUE);
			//break;
		case 2:
			SetAkumDebitBrizzi();
			break;
		case 3:
			SetMinimumSaldoBrizzi();
			break;
#endif
		default:
			break;
	};
}

void MerchantMenu(void)
{
	int ret;
	char *menu[]={
		"MENU STAN>",
		//"INFO",
		"BATCH STAN",
		0,
	};

	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch(ret){
		//case 1:	
			//InfoMerchant();
			//break;
		case 1:  
			BatchStan();
			break;	
		default:
			break;
	};
}

void InfoMerchant(void)
{
	int ret;
	char *menu[]={
		"INFO>",
		"INIT RECEIPT",
		0,
	};

	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch(ret){
		case 1:	
			PrintParamNew();
			break;	
		default:
			break;
	};
}

void BatchStan(void)
{
	int ret;
	char *menu[]={
		"SET STAN>",
		"VIEW STAN",
		 "SET STAN",
		0,
	};

	ret = mnuFunction((Pchar *)menu, 0, 60)%30;

	switch(ret){
		case 1:	
			ViewStanNo();
			break;
		case 2:
			SetStanNo();
			break;	
		default:
			break;
	};
}

void OnOffMenu(void)
{
	int ret;

	MAPPUTWORD(traMnuItm, mnuMainMenu, lblKO);
	MAPPUTWORD(traMnuItmContext, mnuMainMenu, lblKO);
	SubHdlMenu(mnuMainMenu, mnuEnd);
	trcS("hdlMainMenu: End\n");
lblKO:
	;
}


int GetPINCardService(void)
{
	int		iRet;
	uchar	ucRet;
	uchar	szPAN[20+1];
	//uchar	ucPinKeyID;
	
	uchar pin1[6+1];
	uchar pin2[6+1];
	uchar data[2+1];
	uchar blok1[16+1];
	uchar blok2[16+1];
	uchar blok1Byte[8+1];
	uchar blok2Byte[8+1];
	uchar dataXor[8+1];
	uchar TMK[16];
	uchar TWK[16];
	uchar TPK[16];
	int len;
	uchar	maxtry=0;

	word	mnuItm;

	mapGetWord(traMnuItm, mnuItm);
	
	memset(TPK, 0, sizeof(TPK));
	memset(TMK, 0, sizeof(TMK));
	memset(TWK, 0, sizeof(TWK));

	//ucPinKeyID = DEF_PIN_KEY_ID;	

	iRet = ExtractPAN(glProcInfo.stTranLog.szPan, szPAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	
	//-------------- Internal PCI PED --------------
	if (ChkTermPEDMode(PED_INT_PCI))
	{
Ulang:
		memset(pin1, 0, sizeof(pin1));
		memset(pin2,0,sizeof(pin2));
		
		ScrCls();
		DispTransName();

		switch(mnuItm)
		{
			case mnuAktivasiKartu:
			case mnuRenewalKartu:
				PubDispString("KETIK PIN BARU ?", DISP_LINE_LEFT|4);
//				ScrPrint(0, 2, ASCII, "%s", "INPUT PIN");
				memset(glProcInfo.sPinBlock, 0, sizeof(glProcInfo.sPinBlock));
				break;
			case mnuReissuePin:
			case mnuUbahPinCS:
			case mnuUbahPin:
				PubDispString("INPUT PIN BARU ?", DISP_LINE_LEFT|4);
//				ScrPrint(0, 2, ASCII, "%s", "INPUT PIN BARU");
				break;
		}

		ucRet = PubGetString(PASS_IN, 6,6 , pin1, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}

		ScrCls();
		DispTransName();
		switch(mnuItm)
		{
			case mnuAktivasiKartu:
			case mnuReissuePin:
			case mnuUbahPinCS:
			case mnuUbahPin:
			case mnuRenewalKartu:
				PubDispString("ULANGI PIN BARU?", DISP_LINE_LEFT|4);
//				ScrPrint(0, 2, ASCII, "%s", "ULANGI PIN BARU");
				break;
		}

		ucRet = PubGetString(PASS_IN, 6,6 , pin2, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}

		if (memcmp(pin1, pin2,6) !=0) 
		{
			ScrCls();
			DispTransName();
			PubDispString("PIN TIDAK SAMA", DISP_LINE_CENTER|4);
			PubBeepErr();
			PubWaitKey(3);
			maxtry++;
			if(maxtry>2)
				return ERR_USERCANCEL;
			
			goto Ulang;
		}
		
		if(strlen(pin2)==6)
		{
			memset(blok1, 0, sizeof(blok1));
			memset(blok2, 0, sizeof(blok2));
			memset(blok1Byte, 0, sizeof(blok1Byte));
			memset(blok2Byte, 0, sizeof(blok2Byte));
			memset(dataXor, 0, sizeof(dataXor));
			memset(data, 0, sizeof(data));

			LoadTMK(TMK);	
			LoadTWK(TWK);

			PubDes(ONE_DECRYPT, TWK, TMK, TPK);
			
			len=strlen(pin2);

			sprintf(data, "%02d", len); 
			
			memcpy(blok1, data, 2);
			memcpy(blok1 + 2, pin2, len);
			
			fmtPad(blok1, 16, 0x46);

			memcpy(blok2, szPAN, 16);
			PubAsc2Bcd(blok1, 8*2, blok1Byte);
			PubAsc2Bcd(blok2, 8*2, blok2Byte);

			PubXor(blok1Byte, blok2Byte, 8, dataXor);

			switch (mnuItm)
			{
				case mnuAktivasiKartu:
				case mnuRenewalKartu:
					memcpy(glProcInfo.sPlainPinBlock, dataXor, 8); 
					PubDes(ONE_ENCRYPT, dataXor, TPK, glProcInfo.sPinBlock);
					break;
				case mnuReissuePin:
				case mnuUbahPinCS:
					memcpy(glProcInfo.sPlainPinBlock2, dataXor, 8); 
					PubDes(ONE_ENCRYPT, dataXor, TPK, glProcInfo.sPinBlock2);
					break;
			}

			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT; 
			return 0;
		}

		return ERR_NO_DISP;
	}

	return 0;
}


int GetCardCS(uchar ucMode)
{
	int		iRet, iEventID;
	uchar	bCheckICC, ucKey;

/*
	if( (glProcInfo.stTranLog.uiEntryMode & 0x0F)!=MODE_NO_INPUT )
	{
		PrnInit();
		PrnSetNormal();
		PrnStr("Get card");
		PrnStart();
		return 0;
	}
*/

	if( ucMode & FALLBACK_SWIPE )
	{
		ucMode &= ~(SKIP_CHECK_ICC|CARD_INSERTED);	// clear bit 8, force to check service code
	}

	bCheckICC = !(ucMode & SKIP_CHECK_ICC);

	TimerSet(0, TIME_OUT_INPUT*10);
	while( 1 )
	{
		iEventID = DetectCardEvent(ucMode);
		if( iEventID==CARD_KEYIN )
		{
			ucKey = getkey();
			if( ucKey==KEYCANCEL )
			{
				return ERR_USERCANCEL;
			}
			if( (ucMode & CARD_KEYIN) && ucKey>='0' && ucKey<='9' )
			{
				return ManualInputPan(ucKey);
			}
		}
		else if( iEventID==CARD_SWIPED )
		{
			iRet = SwipeCardProc(bCheckICC);
			if( iRet==0 )
			{
				return ValidCard();
			}
			else if( iRet==ERR_SWIPECARD )
			{
				DispMagReadErr();
				PubBeepErr();
				PubWaitKey(3);
			}
			else if( iRet==ERR_NEED_INSERT )	// 是芯片卡
			{
				if( !(ucMode & CARD_INSERTED) )
				{	// 本身交易不允许插卡
					return iRet;
				}
				ucMode &= ~CARD_SWIPED;			// 去掉刷卡检查
			}
			else
			{
				return iRet;
			}
		}
	}
}


int InputUang(void)
{
	uchar	ucRet;
	ScrCls();
	DispTransName();
	PubDispString("JUMLAH :", DISP_LINE_LEFT|4);
	ucRet = PubGetAmount(glTlmSysParam.Con.appCurrSign, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
						0, 11, glProcInfo.stTranLog.szAmount, USER_OPER_TIMEOUT, GA_SEPARATOR);

	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
	//hex_dump("ttttttttttttt",glProcInfo.stTranLog.szAmount, 15);
	
	return 0;
}

int InputNominal(void)
{
	uchar	ucRet;
	ScrCls();
	DispTransName();
	PubDispString("NOMINAL (Rp) :", DISP_LINE_LEFT|4);
	ucRet = PubGetAmount(glTlmSysParam.Con.appCurrSign, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
						0, 11, glProcInfo.stTranLog.szAmount, USER_OPER_TIMEOUT, GA_SEPARATOR);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	AmtConvToBit4Format(glProcInfo.stTranLog.szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
	
	return 0;
}

int PilihJenisRekening()
{
	int ret;

	char *menu[]={"Jenis Rekening","1. Tabungan","2. Giro",0};
	ret = mnuSelect((Pchar *)menu, 0, 60)%MNUMAX;

	switch (ret)
	{
		case 1:
			mapPutByte(traSumberRek, 1);	
			break;
		case 2:
			mapPutByte(traSumberRek, 2);
			break;
		default:
			goto lblKO;
			break;
	}
	goto lblEnd;
	
lblKO:
	ret = -1;
lblEnd:
	return ret;

}

//Untuk memasukkan SSP Month
int redemEnterMonth(void){
	int ret,len;
	uchar ucRet;
	uchar charTemp2[3+1]; // ori 
	uchar TempBuf[3+1];

	if(glProcInfo.stTranLog.BRI.CicilanBring.Bulan[0]!=0)
		return 0;
	
lblStart:
	dspClear();
    	memset(charTemp2, 0, sizeof charTemp2);
	memset(TempBuf, 0, sizeof TempBuf);

	ScrCls();
	DispTransName();
	//ScrPrint(0, 2, ASCII, "%s", "SPP TERM (3-36):");
	PubDispString("SSP TERM (3-36):", DISP_LINE_LEFT|4);
	ucRet = PubGetString(NUM_IN, 1,2 , charTemp2, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}

	//ret = dspLS(2, "SSP TERM (3-36):");
	//CHECK(ret >= 0,lblKO);
	//ret = enterStr(3, charTemp2, 3+1);
	//input = atoi(charTemp2) % 3; // baru di tambah, 07122011
	//hex2num(input, charTemp2, 3);
	len = atoi(charTemp2);
	if(len % 3  == 0)
	{
		if(charTemp2[1] == 0)
		{
			TempBuf[0]= 0x30;
			TempBuf[1]= charTemp2[0];
        		//MAPPUTSTR(traSSPMonth, TempBuf, lblKO);
			memcpy(glProcInfo.szReq61CicilanBring+3,TempBuf,2);
						
        		if(TempBuf[1] == 0)
        		{
        			Beep();
        			goto lblStart;
        		}
		}
		else
			memcpy(glProcInfo.szReq61CicilanBring+3,charTemp2,2);   
					//MAPPUTSTR(traSSPMonth, charTemp2, lblKO);

		memcpy(glProcInfo.stTranLog.BRI.CicilanBring.Bulan, glProcInfo.szReq61CicilanBring+3, 2);

		ret = 0;
	 	goto lblEnd;	

	 			

	}
	else
	{
		dspClear();
		Beep();Beep();Beep();
		dspLS(1, "INPUT HARUS");
		dspLS(2, "KELIPATAN 3");
		tmrPause(3);
		goto lblStart;
	}

  lblEnd:
       return ret;
}

uchar GetFlagMenu(unsigned short pbitno)
{
	return((glSysCtrl.sMenuIndicator[(uchar)((pbitno-1) >> 3)] << (uchar)((pbitno-1) % 8)) & 0x80);
}

void SetFlagMenuDisableOn(unsigned short bitnum)
{
	glSysCtrl.sMenuIndicator[(uchar)((bitnum - 1) >> 3)] |= bit_set_menu[(uchar)((bitnum - 1) % 8)];
}

void SetFlagMenuDisableOff(unsigned short bitnum)
{
	glSysCtrl.sMenuIndicator[(uchar)((bitnum - 1) >> 3)] &=~ bit_set_menu[(uchar)((bitnum - 1) % 8)];
}


void SearchIdxMenu(word Mnu1in, word *Mnu2Out)
{
	word idx;
	char bufMnu[lenMnu+1];
	char mnu1[lenMnu+1];

	mapGet(Mnu1in, mnu1, sizeof(mnu1));
	
	for(idx=(word)(Mnu1in+1); idx<mnuEnd;idx++)
	{
		mapGet(idx, bufMnu, sizeof(bufMnu));
		if(mnu1[0] == bufMnu[0])
			break;
	}
	
	*Mnu2Out=idx;
}

static int mnuShowOnOff(const Pchar * mnu, byte upp, byte cur, byte dim) {
    char tmp[255];
    byte idx;
    uchar ucRever=0;

    //current menu item should be on the screen
    memset(tmp, 0, 255);
    for (idx = 0; idx < dim; idx++) {   //dim lines to display
        memcpy(tmp, mnu[upp + idx], 19);
        fmtPad(tmp, 21, ' ');    //pad by spaces to have a good menu bar

	ucRever = (upp+idx==cur) ? REVER: 0;
	ScrPrint(0, (uchar)(idx+1), (uchar)(ASCII|ucRever), "%.*s", 19, tmp);
    }
    return 0;
}


int mnuSelectOnOff(const Pchar * mnu, int sta, word *idx, int dly) {
//mnu: array of strings; mnu[0]= name of menu; other elements - menu items
//sta: menu state, contains upp and cur indexes, see below
//dly: timeout in seconds; zero means infinite
    int ret;                    //return value
    byte dim;                   //the mnu dimension including menu header
    byte upp;                   //number of menu item to be displayed first
    byte cur;                   //number of menu item selected just now
    byte bnd;                   //number of items to be displayed on the screen (band)
    char key;                   //key pressed

    word currentMnu=0;
    char bufMnu[lenMnu+1];
	
    if(!sta)                    //the initial state is not important
        sta = 1 * MNUMAX + 1;

    cur = sta % MNUMAX;
    upp = sta / MNUMAX;
    dim = 0;
    while(mnu[dim]) {           //calculate dim: number of elements in the menu array
        dim++;
        ret = MNUMAX;
        if(dim >= ret)
            break;              //no more than MNUMAX elements are supported
    }
    
    //calculate bnd: number of items to be displayed on the screen
    bnd = 4;            //if there is enough elements it is the screen height
    if(dim < bnd)
        bnd = dim;              //otherwise, it is the number of items in the menu
    bnd--;                      //don't forget that the first element of the mnu array is the header string

//header
    	dspClear();
//	PubDispStringASCII(void * pszStr, ushort nPosition)
	ScrPrint(5, 0, (uchar)(ASCII), "%.*s", 16, (char*)mnu[0]);
	ScrDrawRect(0, 0, 127, 7);

//menu
    while(1) {
        
        ret = mnuShowOnOff(mnu, upp, cur, bnd);  //display current menu state
        CHK;

	PubDispStringASCII("Disable>", DISP_LINE_RIGHT|5);
	PubDispStringASCII("Enable>", DISP_LINE_RIGHT|7);
	
        //wait for user input
	 key = 0;
	 kbflush();
	 key = PubWaitKey((short)dly);
	 if(key == NOKEY)
	 	key = 0;

        switch (key) {          //analyse the key pressed
          case 0:
              return 0;         //timeout
          case KEYENTER:         //green key ENTER - current menu item selected
              return upp * MNUMAX + cur;    //return the menu state
	//new
	case KEYATM3:
		SetFlagMenuDisableOn(0);//use on off menu
	  	currentMnu = idx[(upp * MNUMAX + cur) % MNUMAX];   //sta%MNUMAX is the current item selected

		mapGet(currentMnu, bufMnu, sizeof(bufMnu));
		SetFlagMenuDisableOn((word)(currentMnu-mnuMainMenu));
		bufMnu[1]='?';
		
		SaveSysCtrlBase();
		mapPut(currentMnu, bufMnu, sizeof(bufMnu));
		CheckMenu();
		return 2500;
		break;
	case KEYATM4:
		SetFlagMenuDisableOn(0);//use on off menu
	  	currentMnu = idx[(upp * MNUMAX + cur) % MNUMAX];   //sta%MNUMAX is the current item selected

		mapGet(currentMnu, bufMnu, sizeof(bufMnu));

		SetFlagMenuDisableOff((word)(currentMnu-mnuMainMenu));
		bufMnu[1]=' ';
	  	
		SaveSysCtrlBase();
		mapPut(currentMnu, bufMnu, sizeof(bufMnu));
		CheckMenu();
		return 2500;
		break;
/*
	  case KEYMENU:
	  	SetFlagMenuDisableOn(0);//use on off menu
	  	currentMnu = idx[(upp * MNUMAX + cur) % MNUMAX];   //sta%MNUMAX is the current item selected

		mapGet(currentMnu, bufMnu, sizeof(bufMnu));

	  	if(GetFlagMenu((word)(currentMnu-mnuMainMenu)))	// 1 = Disable, 0 = enable
		{
			SetFlagMenuDisableOff((word)(currentMnu-mnuMainMenu));
			bufMnu[1]=' ';
	  	}
		else
		{
			SetFlagMenuDisableOn((word)(currentMnu-mnuMainMenu));
			bufMnu[1]='?';
		}
		SaveSysCtrlBase();
		mapPut(currentMnu, bufMnu, sizeof(bufMnu));
		CheckMenu();
		return 0;
		break;
*/		
          case KEYCANCEL:
	   case KEYCLEAR:
              return 0;         //red key CLEAR - menu selection aborted
          case KEYDOWN:          //DOWN arrow key - move cursor down
              cur++;
              if(cur >= dim) {  //wrapping from the end to the beginning if needed
                  cur = 1;
                  upp = 1;
              } else if(upp + bnd == cur)
                  upp++;        //follow upp if cur is at the bottom
              break;
          case KEYUP:          //UP arrow key - move cursor up
              cur--;
              if(!cur) {        //wrapping from the beginning to the end if needed
                  cur = dim - 1;
                  if(upp + bnd <= cur)
                      upp = cur - bnd + 1;
              } else if(cur == upp - 1) //follow upp if cur is at the top
                  upp = cur;
              break;
          case KEY1:
          case KEY2:
          case KEY3:
          case KEY4:
          case KEY5:
          case KEY6:
          case KEY7:
          case KEY8:
          case KEY9:            //numeric key: direct selecting
              key -= KEY0;
              if(key > dim - 1) {   //if there is not enough menu items - beep
                  Beep();
                  break;
              }
              cur = (byte) key; //adjust screen to put current item on the top
              upp = cur;
              if(upp + bnd > dim)
                  upp = dim - bnd;  //adjust upp to display as many items as possible
              break;
          default:
              Beep();
              break;            //key not accepted
        }
    }
  lblKO:return -1;
}

static void SelectHdlMenu(word MnuItm)
{
	int ret;
	word mnu2;
	
//	traReset();
	
	MAPPUTWORD(traMnuItm, MnuItm, lblKO);
	MAPPUTWORD(traMnuItmContext, MnuItm, lblKO);
	//hex_dump_char("SelectHdlMenu", "NOK", 2);
	SearchIdxMenu(MnuItm, &mnu2);
//	hex_dump_char("SelectHdlMenu", &mnu2, sizeof(mnu2));
	SubHdlMenu(MnuItm, mnu2);
//	hex_dump_char("SelectHdlMenu", "OK", 2);
	goto lblEnd;
lblKO:
lblEnd:
	;
}


static void SubHdlMenu(word msg1, word msg2)
{
	int ret;
	byte sta;                   //menu state, it is (upper item)*10 + (current item)
	char lvl;                   //menu level
	char mnu[MNUMAX][lenMnu + 1]; //the final menu array prepared to mnuSelect
	char *ptr[MNUMAX];          //array of pointers to mnu items
	word idx[MNUMAX];           //the correspondent indices
	byte idxP;                  //index within ptr array
	word idxM;                  //take values from msg1..msg2
	char itm[lenMnu + 1];       //(menu level)+(access type)+(item name)+(ending zero)

	//    byte MnuPop;

lblOnOffMenu:
	memset(ptr, 0, sizeof(ptr));
	memset(mnu, 0, sizeof(mnu));
	MAPGET(msg1, itm, lblKO);   //it is the menu name
	strcpy(mnu[0], itm + 2);    //omit menu level and access type
	lvl = itm[0] + 1;

	idxP = 0;
	ptr[idxP++] = mnu[0];       //it is the menu name
	ret = nvmHold(0);

	for (idxM = msg1 + 1; idxM < msg2; idxM++)
	{    //build mnu and ptr arrays
		MAPGET(idxM, itm, lblKO);   //retrieve menu item
		if(itm[0] != lvl)
			continue;           //only items of level lvl are included

		strcpy(mnu[idxP], itm + 2); //the first and second characters are menu level and access type, it is not showed

		if(itm[1]=='?')
		{
			strcat(mnu[idxP], "(X)");
		}
		else
		{
			strcat(mnu[idxP], "   ");
		}
		
		ptr[idxP] = mnu[idxP];  //fill the pointers array
		idx[idxP] = idxM;       //save the index to be returned if this item will be selected
		if(++idxP >= MNUMAX)    //no more than MNUMAX items in menu
			break;
	}
	nvmRelease(0);
	
	if(idxP == 1)
		goto lblEnd;            //empty menu - nothing to do

	sta = 0;                    //it is started with the current item 1 at the top of the screen
	do
	{
		ret = mnuSelectOnOff((Pchar *) ptr, sta, idx, 60);    //perform user dialog
		if(ret <= 0)            //timeout or aborted - nothing to do
			break;

		if(ret == 2500)
			goto lblOnOffMenu;

		sta = ret;
		idxM = idx[sta % MNUMAX];   //sta%MNUMAX is the current item selected

		SelectHdlMenu(idxM);
	}
	while(sta);
	goto lblEnd;

lblKO:
lblEnd:
	;
}

void CheckMenu(void)
{
	word idx;
	char bufMnu[lenMnu+1];

	if(!GetFlagMenu(0))	//indicator use on off menu
	{	
		for(idx=(word)(mnuBeg+2);idx<mnuEnd;idx++)
		{
			mapGet(idx, bufMnu, sizeof(bufMnu));
			if(bufMnu[1]=='?')
				SetFlagMenuDisableOn((word)(idx-mnuMainMenu));
			else
				SetFlagMenuDisableOff((word)(idx-mnuMainMenu));
		}	
		SaveSysCtrlBase();
		return;
	}
	
	for(idx=(word)(mnuBeg+2);idx<mnuEnd;idx++)
	{
		mapGet(idx, bufMnu, sizeof(bufMnu));
		if(GetFlagMenu((word)(idx-mnuMainMenu)))
			bufMnu[1]='?';
		else
			bufMnu[1]=' ';

		mapPut(idx, bufMnu, sizeof(bufMnu));
	}
}

void ViewBrizzi()
{
//	uchar ucRet;
//	uchar	szBuff[12+1];
	uchar	szBuff1[12+1];
	uchar	szBuff2[12+1];
	uchar	szBuff3[12+1];
	
	uchar	ucRet;

	lbl_1:
	ScrCls();
	ScrClrLine(2, 7);
	PubDispString("Nominal Discount", DISP_LINE_LEFT|0);
	DispAmount(2, glSysCtrl.szAmtDiskonBrizzi);
	//sprintf(szBuff, "%lu", glSysCtrl.szAmtDiskonBrizzi);
	//PubDispStringASCII(szBuff, DISP_LINE_RIGHT|6);
	//DispAmount(2, szBuff);

	//ScrPrint(0, 6, CFONT, "%lu", szBuff1); //"Bank Asal:"
	sprintf(szBuff1, "%lu", (glSysCtrl.ulDiskonBrizzi)); // Add Zaki 20 Januari 2013
	//DispAmount(6, szBuff1);
	PubDispString("Persen Disc (%):", DISP_LINE_LEFT|4);
	PubDispString(szBuff1, DISP_LINE_RIGHT|6);
	//PubDispStringASCII(" v", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return;
	else if(ucRet == KEYUP )
		goto lbl_1;

//	lbl_2:
	ScrCls();
	ScrClrLine(2, 7);
	PubDispString("MINIMAL SALDO :", DISP_LINE_LEFT|0);
	sprintf(szBuff2, "%lu", glSysCtrl.ulMinSaldo*100);
	//PubDispStringASCII(szBuff, DISP_LINE_RIGHT|6);
	DispAmount(2, szBuff2);

	sprintf(szBuff3, "%lu", (glSysCtrl.ulMaxAkumDebit*100)); // Add Zaki 20 Januari 2013
	DispAmount(6, szBuff3);
	PubDispString("AKUM DEBIT :", DISP_LINE_LEFT|4);
	//PubDispStringASCII("^", DISP_LINE_CENTER|7);
	ucRet = PubYesNoUpDown(USER_OPER_TIMEOUT);
	if(ucRet == NOKEY)
		return;
	else if(ucRet == KEYUP )
		goto lbl_1;


/*
	ScrCls();
	MEM_ZERO(szAmount);
	sprintf(szAmount, "%lu", (glSysCtrl.ulMinSaldo*100)); // Add Zaki 20 Januari 2013
	DispAmount(1, szAmount);

	MEM_ZERO(szAmount);
	sprintf(szAmount, "AKUM DEBIT : Rp.%s", (glSysCtrl.ulMaxAkumDebit*100)); // Add Zaki 20 Januari 2013
	DispAmount(2, szAmount);

	MEM_ZERO(szAmount);
	sprintf(szAmount, "Diskon : Rp.%s", glSysCtrl.szAmtDiskonBrizzi);
	DispAmount(3, szAmount);

	MEM_ZERO(szAmount); 
	sprintf(szAmount, "Persen Disc (%) : %lu", glSysCtrl.ulDiskonBrizzi);
	DispAmount(4, szAmount);

	ulong	MinSaldo;
	ulong	AkumDebit;
	uchar	Diskon[20];
	ulong	PersenDiskon;
	uchar	FmtMinSaldo[24];
	uchar	FmtAkumDebit[24];
	uchar	FmtDiskon[24];
	
	MEM_ZERO(MinSaldo);
	MEM_ZERO(AkumDebit);
	MEM_ZERO(Diskon);
	MEM_ZERO(PersenDiskon);
	
	memcpy(MinSaldo,glSysCtrl.ulMinSaldo, 12);
	memcpy(AkumDebit,glSysCtrl.ulMaxAkumDebit,12);
	memcpy(Diskon, glSysCtrl.szAmtDiskonBrizzi, 12);
	memcpy(PersenDiskon, glSysCtrl.ulDiskonBrizzi, 12);
	//fmtAmt(tagihanFmt,tagihan,0,",.");

	sprintf(FmtMinSaldo,"Rp. %s",MinSaldo);
	sprintf(FmtAkumDebit,"Rp. %s",AkumDebit);
	sprintf(FmtDiskon,"Rp. %s",Diskon);

	ScrCls();
	PubDispStringASCII(FmtMinSaldo, DISP_LINE_LEFT|2);
	PubDispStringASCII(FmtAkumDebit, DISP_LINE_LEFT|3);
	PubDispStringASCII(FmtDiskon, DISP_LINE_LEFT|5);
	PubDispStringASCII(PersenDiskon, DISP_LINE_LEFT|6);
*/

}

void SetAkumDebitBrizzi()
{
	uchar ucRet;
	uchar szAmount[12+1];

	ScrCls();
	MEM_ZERO(szAmount);
	
	sprintf(szAmount, "%lu", (glSysCtrl.ulMaxAkumDebit*100)); // Add Zaki 20 Januari 2013
	DispAmount(2, szAmount);

	
	
	MEM_ZERO(szAmount);
	PubDispString("AKUM DEBIT :", DISP_LINE_LEFT|4);
	//sprintf(szAmount, "%.s", glSysCtrl.ulMaxAkumDebit); // Add Zaki 20 Januari 2013
	//strcpy(szAmount, "0");	//zaki 2 feb 2014
	//ucRet = PubGetAmount(glTlmSysParam.Con.appCurrSign, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
		//				0, 10, szAmount, USER_OPER_TIMEOUT, GA_SEPARATOR);
	strcpy(szAmount, "-");	//irf,  02 Jan 2014
	
	ucRet = PubGetString(NUM_IN|ECHO_IN, 0,10 , szAmount, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return;
	}

	
	if (memcmp(szAmount, "-", 1)!=0)
		{
			glSysCtrl.ulMaxAkumDebit= PubAsc2Long(szAmount, strlen(szAmount), NULL);
			SaveSysCtrlBase();
		}
	

	
	
}

void SetMinimumSaldoBrizzi()
{
	uchar ucRet;
	uchar szAmount[12+1];

	ScrCls();
	MEM_ZERO(szAmount);
	sprintf(szAmount, "%lu", (glSysCtrl.ulMinSaldo*100)); // Add Zaki 20 Januari 2013
	DispAmount(2, szAmount);

	MEM_ZERO(szAmount);
	PubDispString("MIN SALDO :", DISP_LINE_LEFT|4);
	//strcpy(szAmount, "-");	//zaki 2 feb 2014
	//ucRet = PubGetAmount(glTlmSysParam.Con.appCurrSign, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
		//				0, 7, szAmount, USER_OPER_TIMEOUT, GA_SEPARATOR);
	//if( ucRet!=0 )
	//{
		//return;
	//}

	strcpy(szAmount, "-");	//irf,  02 Jan 2014
	
	ucRet = PubGetString(NUM_IN|ECHO_IN, 0,10 , szAmount, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return;
	}

	
	if (memcmp(szAmount, "-", 1)!=0)
		{
			glSysCtrl.ulMinSaldo = PubAsc2Long(szAmount, strlen(szAmount), NULL);
			SaveSysCtrlBase();
		}
	
		
}



void SetBrizziDisc()
{
	uchar ucRet;
	uchar szAmount[12+1];
//	uchar buff[12+1];

	kbflush();
	ScrCls();
	PubDispString("SET DISCOUNT", DISP_LINE_CENTER|0);
	PubDispString("1. Rupiah (Rp)", DISP_LINE_LEFT|2);
	PubDispString("2. Persen (%)", DISP_LINE_LEFT|4);
	while(TRUE)
	{
		ucRet = PubWaitKey(USER_INPUT_TIMEOUT);
		switch(ucRet)
		{
			case KEY1:
				goto lblDiskonAmt;
				break;
			case KEY2:
				goto lblDiskon;
				break;
			case NOKEY:
				return;
		}
		
	}

lblDiskonAmt:	
	// Add Zaki 20 Januari 2013
	//ScrClrLine(2, 7);
	//PubDispString("Nominal Discount", DISP_LINE_LEFT|2);
	//sprintf(szAmount, "%06ld", glSysCtrl.szAmtDiskonBrizzi);
	//PubDispString(szAmount, DISP_LINE_RIGHT|6);
	//if( PubYesNo(USER_OPER_TIMEOUT) )
	//{
		//return;
	//}	
	//
	/*
	ScrCls();
	sprintf(buff, "%lu", glSysCtrl.ulDiskonBrizzi); 
	AmtConvToBit4Format(buff, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
	memcpy(glSysCtrl.szAmtDiskonBrizzi, buff, sizeof(glSysCtrl.szAmtDiskonBrizzi));
	if(glTlmSysParam.Con.appTermOpt5 & EDC_TLM_TMS_CONFIRM_AMT_ENTRY)
		{
			if( ViewDiskonBrizzi(NULL, buff) )
			{
				return;
			}
		}	
	*/

	ScrCls();
	DispAmount(2, glSysCtrl.szAmtDiskonBrizzi);

	PubDispString("Nominal Discount", DISP_LINE_LEFT|4);
	//sprintf(buff, "%lu", glSysCtrl.ulDiskonBrizzi); 
	//strcpy(szAmount, "-");	//zaki 2 feb 2014
	//PubDispString("-", DISP_LINE_RIGHT|6);
	//memcpy(szAmount,  glSysCtrl.szAmtDiskonBrizzi, 7);
	//sprintf(szAmount, "%lu", glSysCtrl.szAmtDiskonBrizzi);
	//szAmount[0] = '-';
	//ScrCls();
	//ucRet = PubGetAmount(glTlmSysParam.Con.appCurrSign, glProcInfo.stTranLog.stTranCurrency.ucDecimal,
		//				0, 6, szAmount, USER_OPER_TIMEOUT, GA_SEPARATOR);
	MEM_ZERO(szAmount); 
	strcpy(szAmount, "-");	//irf,  02 Jan 2014
	
	ucRet = PubGetString(NUM_IN|ECHO_IN, 0,7 , szAmount, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return;
	}

	if (memcmp(szAmount, "-", 1)!=0)
		{
			AmtConvToBit4Format(szAmount, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
			memcpy(glSysCtrl.szAmtDiskonBrizzi, szAmount, sizeof(glSysCtrl.szAmtDiskonBrizzi));

			glSysCtrl.ulDiskonBrizzi = 0;	//di reset buffer nya (hanya boleh salah satu)

			SaveSysCtrlBase();

		}
	
return;
	
/////////////////////////////////////////////////////////////////////////////////////////////////////
lblDiskon:
// Add Zaki 20 Januari 2013
	//ScrClrLine(2, 7);
	//PubDispString("Persen Disc (%)", DISP_LINE_LEFT|2);
	//sprintf(szAmount, "%016d", glSysCtrl.ulDiskonBrizzi);
	//PubDispString(szAmount, DISP_LINE_RIGHT|6);
	//if( PubYesNo(USER_OPER_TIMEOUT) )
	//{
		//return;
	//}	
	//
	/*
	ScrCls();
	sprintf(buff, "%lu", glSysCtrl.ulDiskonBrizzi); 
	AmtConvToBit4Format(buff, glProcInfo.stTranLog.stTranCurrency.ucIgnoreDigit);
	memcpy(glSysCtrl.szAmtDiskonBrizzi, buff, sizeof(glSysCtrl.szAmtDiskonBrizzi));
	if(glTlmSysParam.Con.appTermOpt5 & EDC_TLM_TMS_CONFIRM_AMT_ENTRY)
		{
			if( ViewDiskonBrizzi(NULL, buff) )
			{
				return;
			}
		}	
*/
	ScrCls();
	MEM_ZERO(szAmount); 
	sprintf(szAmount, "%lu", glSysCtrl.ulDiskonBrizzi);
//	strcpy(szAmount, "0"); 
	PubDispString("Persen Disc (%):", DISP_LINE_LEFT|4);
	//sprintf(szAmount, "%06ld", glSysCtrl.szAmtDiskonBrizzi); // Add Zaki 20 Januari 2013
	ucRet = PubGetString(NUM_IN|ECHO_IN, 1, 2, szAmount, USER_INPUT_TIMEOUT);
	if( ucRet!=0 )
	{
		return;
	}
	
	memset(glSysCtrl.szAmtDiskonBrizzi, 0, sizeof(glSysCtrl.szAmtDiskonBrizzi));	//di reset buffer nya (hanya boleh salah satu)
	glSysCtrl.ulDiskonBrizzi = PubAsc2Long(szAmount, strlen(szAmount), NULL);
	SaveSysCtrlBase();
	
	
	
}


uchar ViewDiskonBrizzi(char *pszDesignation, uchar *pszAmount)
{
	if ((pszDesignation==NULL) || (pszDesignation[0]==0))
	{
		PubDispString(_T("DISKON       "), 2|DISP_LINE_LEFT);
	}
	else
	{
		PubDispString(pszDesignation, 2|DISP_LINE_LEFT);
	}

	DispAmount(4, pszAmount);
	PubDispString(_T("CORRECT ? Y/N"), DISP_LINE_LEFT|6);
	return !PubYesNo(USER_OPER_TIMEOUT);
}



void AutoLogon(void)
{

	
	TransLogon();
}

#endif
	
void PrintTagEMV(void)
{
		uchar DataOut[128];
		int OutLen;
		uchar szBuff[50];
		uchar szBuf1[50];
		uchar	ucSW1=0, ucSW2=0;

		//PrnInit();
		PrnSetNormal();

		//TERM CAP
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F33,DataOut, &OutLen);
		//hex_dump_char(0x9F33, DataOut, OutLen);
		PubBcd2Asc0(DataOut, 8, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "TERM CAP : %.6s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//ADD TERM CAP
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F40,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, 8, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "ADD TERM CAP : %.10s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Txt
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9f12,DataOut, &OutLen);
		//hex_dump_char(0x9F33, DataOut, OutLen);
		//PubBcd2Asc0(glTranConfig[glProcInfo.stTranLog.ucTranType].szLabel, 8, szBuff);
		memcpy(szBuff, DataOut, OutLen);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Txt : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//APPL ID
		/*
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x4F,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, 8, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "APPL ID : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		*/
		PubBcd2Asc0(glProcInfo.stTranLog.sAID, glProcInfo.stTranLog.ucAidLen, szBuff);
		PubTrimTailChars(szBuff, 'F');
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "APPL ID   : %.32s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		
		//Cert AuthPublic Key Index
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x8F,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Cert AuthPublic Key Index : %.2s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		
		//PAN
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x5A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "PAN : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Transaction Date
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Transaction Date : %.6s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		
		//Transaction Type
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9C,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Transaction Type : %.2s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		
		//Trans Curr Code
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x5F2A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Trans Curr Code : %.4s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		
		//Term Country Code
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		//EMVGetTLVData(0x5F28,DataOut, &OutLen);
		EMVGetTLVData(0x9F1A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Term Country Code : %.4s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Amount Authorized
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F02,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Amount Authorized : %.12s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Amount Other
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F03,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, 6, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Amount Other : %.12s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//PAN Sequence No
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x5F34,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "PAN Sequence No : %.2s\n\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

	//Terminal Action Codes
		//DEN
		//memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
	//	EMVGetTLVData(0x9F8129,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		//PubBcd2Asc0(DataOut, 8, szBuff);
		PubBcd2Asc0(glEmvStatus.sgTACDeinal, 5, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		MultiLngPrnStr("Terminal Action Codes :\n", DISP_LINE_LEFT);
		sprintf((char *)szBuf1, "DEN : %.10s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//ONL
		//memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
//		EMVGetTLVData(0x9F812A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		//PubBcd2Asc0(DataOut, 8, szBuff);
		PubBcd2Asc0(glEmvStatus.sgTACOnline, 5, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "ONL : %.10s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//DEF
		//memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
	//	EMVGetTLVData(0x9F8128,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		//PubBcd2Asc0(DataOut, 8, szBuff);
		PubBcd2Asc0(glEmvStatus.sgTACDefault, 5,szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "DEF : %.10s\n\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

	//Issuer Action Codes
		//DEN
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F0E,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		MultiLngPrnStr("Issuer Action Codes : (* = default)\n", DISP_LINE_LEFT);
		sprintf((char *)szBuf1, "DEN : %.10s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//ONL
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F0F,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "ONL : %.10s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//DEF
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F0D,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "DEF : %.10s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//AIP
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x82,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "AIP : %.4s\n\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);


		//TVR VALUE
		if(glProcInfo.stTranLog.sTVR)
		EMVGetTLVData(0x95, glProcInfo.stTranLog.sTVR, &OutLen);
		PubBcd2Asc0(glProcInfo.stTranLog.sTVR, 5, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "TVR VALUE : %.10s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

	
		//CVMR
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F34,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, lenCVMRes, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "CVMR : %.6s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//TSI
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9B,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "TSI : %.4s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Issuer App Data
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F10,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		MultiLngPrnStr("Issuer App Data :\n", DISP_LINE_LEFT);
		sprintf((char *)szBuf1, "%.36s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Auth Response Code
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x8A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
	//	PubBcd2Asc0(DataOut, lenRspCod, szBuff);
		memcpy(szBuff, DataOut, 2);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Auth Response Code : %s\n\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		
		//Scheme spesifics tags
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F53,DataOut, &OutLen); //4212
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		MultiLngPrnStr("Scheme spesifics tags :\n", DISP_LINE_LEFT);
		sprintf((char *)szBuf1, "9F53 : %.2s\n\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//GEN AC Req CID
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		//EMVGetTLVData(0x9F53,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		DataOut[0]= glEmvStatus.GenACCID1st;
		PubBcd2Asc0(DataOut, 1, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		MultiLngPrnStr("GEN AC CID :\n", DISP_LINE_LEFT);
		sprintf((char *)szBuf1, "1st : %.2s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		//EMVGetTLVData(0x9F53,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		DataOut[0]= glEmvStatus.GenACCID2nd;
		PubBcd2Asc0(DataOut, 1, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "2nd : %.2s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//AC
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F26,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, lenIssTrnCrt, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "AC  : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//ATC
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F36,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, lenATC, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "ATC : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Unpredictable Num.
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F37,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, lenUnpNum, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Unpredictable Num. : %s\n\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Risk Management Info
		//Final SW1SW2
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		//EMVGetTLVData(0x9F53,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
//		EMVGetICCStatus(&ucSW1, &ucSW2);
		DataOut[0]=glEmvStatus.ucSW1;
		DataOut[1]=glEmvStatus.ucSW2;
		PubBcd2Asc0(DataOut, 2, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Final SW1SW2 = %s\n\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		MultiLngPrnStr("Risk Management Info :\n", DISP_LINE_LEFT);


		//Amount
		//memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		//OutLen=0;
		//EMVGetTLVData(0x9F53,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		//PubBcd2Asc0(glProcInfo.stTranLog.szAmount, lenAmt, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Amount  = %s\n", glProcInfo.stTranLog.szAmount);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		/*//PrevAmt
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		//EMVGetTLVData(0xDF25,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, lenTrmFlrLim, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "PrevAmt = %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		*/
		MultiLngPrnStr("PrevAmt = 00000000\n", DISP_LINE_LEFT);
		
		//TotAmt
		//memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		//OutLen=0;
		//EMVGetTLVData(0x9F53,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		//PubBcd2Asc0(glProcInfo.stTranLog.szInitialAmount, lenAmt, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "TotAmt  = %s\n", glProcInfo.stTranLog.szInitialAmount);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Floor
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F1B,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, lenTrmFlrLim, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Floor   = %s\n\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		//PrnStr("\n");
		//PrnStart();
}


	
void PrintTagEMVTes(void)
{
		uchar DataOut[128];
		int OutLen;
		uchar szBuff[50];
		uchar szBuf1[50];

		DispPrinting();
		if( !ChkIfThermalPrinter() )
		{
			return;
		}

//		PrnInit();
		PrnSetNormal();

		//Aplication Cryptogram
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F26,DataOut, &OutLen);
		//hex_dump_char(0x9F33, DataOut, OutLen);
		PubBcd2Asc0(DataOut, 8, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Aplication Cryptogram : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Cryptogram Information
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F27,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Cryptogram Information : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Issuer Application Data
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData( 0x9F10,DataOut, &OutLen);
		//hex_dump_char(0x9F33, DataOut, OutLen);
		PubBcd2Asc0(DataOut,OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Issuer Application Data : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Unpredictable Number
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F37,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Unpredictable Number : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Application Transaction Counter
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F36,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Application Transaction Counter : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Terminal Verification Results 
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x95,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Terminal Verification Results : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Transaction Date
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Transaction Date : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Transaction Type
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9C,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Transaction Type : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Amount, Authorized
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F02,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Amount (Authorized) : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Transaction Currency Code
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x5F2A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Trans Currency Code : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Application Interchange Profile
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x82,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "AIP : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//PAN
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x5A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "PAN : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Terminal Country Code
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F1A,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Terminal Country Code : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//CVM Results
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F34,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "CVM Results : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//Amount, Other
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x9F03,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, 6, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "Amount (Other) : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);

		//PAN Squence Number
		memset(DataOut, 0, sizeof(DataOut));
		memset(szBuff, 0, sizeof(szBuff));
		OutLen=0;
		EMVGetTLVData(0x5F34,DataOut, &OutLen);
	//	hex_dump_char(0x9F40, DataOut, OutLen);
		PubBcd2Asc0(DataOut, OutLen, szBuff);
		memset(szBuf1, 0, sizeof(szBuf1));
		sprintf((char *)szBuf1, "PAN Squence Number : %s\n", szBuff);
		MultiLngPrnStr(szBuf1, DISP_LINE_LEFT);
		
//		PrnStart();
}



// DIKI ADD FOR BUKOPIN MINI ATM
int PilihJenisRekeningBkpn()
{
	int ret;

	//char *menu[]={"Jenis Rekening","1. Tabungan","2. Giro",0};
	char *menu[]={"Rekening Transfer","1. Bukopin","2. Bank Lain",0};
	ret = mnuSelect((Pchar *)menu, 0, 60)%MNUMAX;

	switch (ret)
	{
		case 1:
			mapPutByte(traSumberRek, 1);	
			break;
		case 2:
			mapPutByte(traSumberRek, 2);
			break;
		default:
			goto lblKO;
			break;
	}
	goto lblEnd;
	
lblKO:
	ret = -1;
lblEnd:
	return ret;

}




int GetPINCardServiceBkpn(void)
{
	int		iRet;
	uchar	ucRet;
	uchar	szPAN[20+1];
	//uchar	ucPinKeyID;
	
	uchar pin1[6+1];
	uchar pin2[6+1];
	uchar data[2+1];
	uchar blok1[16+1];
	uchar blok2[16+1];
	uchar blok1Byte[8+1];
	uchar blok2Byte[8+1];
	uchar dataXor[8+1];
	uchar TMK[16];
	uchar TWK[16];
	uchar TPK[16];
	int len;
	uchar	maxtry=0;

	word	mnuItm;

	//mapGetWord(traMnuItm, mnuItm);
	
	memset(TPK, 0, sizeof(TPK));
	memset(TMK, 0, sizeof(TMK));
	memset(TWK, 0, sizeof(TWK));

	//ucPinKeyID = DEF_PIN_KEY_ID;	

	iRet = ExtractPAN(glProcInfo.stTranLog.szPan, szPAN);
	if( iRet!=0 )
	{
		return iRet;
	}

	
	//-------------- Internal PCI PED --------------
	if (ChkTermPEDMode(PED_INT_PCI))
	{
Ulang:
		memset(pin1, 0, sizeof(pin1));
		memset(pin2,0,sizeof(pin2));
		
		ScrCls();
		DispTransName();

		//switch(mnuItm)
		//{
		//	case mnuAktivasiKartu:
		//	case mnuRenewalKartu:
		//		PubDispString("KETIK PIN BARU ?", DISP_LINE_LEFT|4);
//				ScrPrint(0, 2, ASCII, "%s", "INPUT PIN");
		//		memset(glProcInfo.sPinBlock, 0, sizeof(glProcInfo.sPinBlock));
		//		break;
		//	case mnuReissuePin:
		//	case mnuUbahPinCS:
		//	case mnuUbahPin:
				PubDispString("INPUT PIN BARU ?", DISP_LINE_LEFT|4);
//				ScrPrint(0, 2, ASCII, "%s", "INPUT PIN BARU");
		//		break;
		//}

		ucRet = PubGetString(PASS_IN, 6,6 , pin1, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}

		ScrCls();
		DispTransName();
		//switch(mnuItm)
		//{
		//	case mnuAktivasiKartu:
		//	case mnuReissuePin:
		//	case mnuUbahPinCS:
		//	case mnuUbahPin:
		//	case mnuRenewalKartu:
				PubDispString("ULANGI PIN BARU?", DISP_LINE_LEFT|4);
//				ScrPrint(0, 2, ASCII, "%s", "ULANGI PIN BARU");
		//		break;
		//}

		ucRet = PubGetString(PASS_IN, 6,6 , pin2, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}

		if (memcmp(pin1, pin2,6) !=0) 
		{
			ScrCls();
			DispTransName();
			PubDispString("PIN TIDAK SAMA", DISP_LINE_CENTER|4);
			PubBeepErr();
			PubWaitKey(3);
			maxtry++;
			if(maxtry>2)
				return ERR_USERCANCEL;
			
			goto Ulang;
		}
		
		if(strlen(pin2)==6)
		{
			memset(blok1, 0, sizeof(blok1));
			memset(blok2, 0, sizeof(blok2));
			memset(blok1Byte, 0, sizeof(blok1Byte));
			memset(blok2Byte, 0, sizeof(blok2Byte));
			memset(dataXor, 0, sizeof(dataXor));
			memset(data, 0, sizeof(data));

			//LoadTMK(TMK);
			//LoadTWK(TWK);

			memcpy(TMK, glCurTlmTmsAcq.acqMasterKey1, lenMKey);
			memcpy(TWK, glSysCtrl.WorkingKey, lenMKey);

			PubDes(ONE_DECRYPT, TWK, TMK, TPK);
			
			len=strlen(pin2);

			sprintf(data, "%02d", len); 
			
			memcpy(blok1, data, 2);
			memcpy(blok1 + 2, pin2, len);
			
			fmtPad(blok1, 16, 0x46);

			memcpy(blok2, szPAN, 16);
			PubAsc2Bcd(blok1, 8*2, blok1Byte);
			PubAsc2Bcd(blok2, 8*2, blok2Byte);

			PubXor(blok1Byte, blok2Byte, 8, dataXor);


			memcpy(glProcInfo.sPlainPinBlock2, dataXor, 8); 
			PubDes(ONE_ENCRYPT, dataXor, TMK, glProcInfo.sPinBlock2);

	/////		hex_dump_char("sPinBlock akhir", glProcInfo.sPinBlock2, sizeof(glProcInfo.sPinBlock2));

			glProcInfo.stTranLog.uiEntryMode |= MODE_PIN_INPUT; 
			//return 0;
			
			GetPinKeyPerAcq();
			return 0;
			
		}

		return ERR_NO_DISP;
	}

	return 0;
}





//#endif //for bukopin 4212

