#include "global.h"



#define REQ_INQ 0
#define RSP_INQ 1
#define REQ_PCH 2
#define RSP_PCH 3


static int  GetPanFromTrackPrvtLbl(uchar *pszPAN, uchar *pszExpDate);
static void ConvertHolderNamePrvtLbl(uchar *pszOrgName, uchar *pszNormalName);




static int ret; uchar tipeTab;
static int ucTranType;
static uchar Pan[lenPan+1];
static uchar SerialNo[8+1];
CicilanInfo CicilanBukopinDefault[7]={
	{FALSE,	"1",	"P001",	"01"},//tenor 1
	{FALSE,	"2",	"P003", "01"},//tenor 3
	{FALSE,	"3",	"P006", "01"},//tenor 6
	{FALSE,	"4",	"P009", "01"},//tenor 9
	{FALSE,	"5",	"P012", "01"},//tenor 12
	{FALSE,	"6",	"P018", "01"},//tenor 18
	{FALSE,	"7",	"P024", "01"} //tenor 24
};
void SetReqBit61(int tenor){
	#ifdef BANK_HSBC
	uchar Bulan[7]={
		1,3,6,9,12,18,24
		};
	sprintf(glProcInfo.szReq61CicilanBukopin,"%2d",Bulan[tenor]);
	#else
	#ifdef BANK_PANIN
	sprintf(glProcInfo.szReq61CicilanBukopin,"%9s%4s",SerialNo,CB[tenor].InterestCode);
	#else
	sprintf(glProcInfo.szReq61CicilanBukopin,"%1s%2s",CB[tenor].Tenor,CB[tenor].InstallmentCode);
	#endif
	#endif
	//hex_dump_char("szReq61CicilanBukopin",glProcInfo.szReq61CicilanBukopin,strlen(glProcInfo.szReq61CicilanBukopin));
	}
void tenor1(void){
	ucTranType = appTenor1;
	strcpy(Pan, "01");
	SetReqBit61(MnuTenor1);
}
void tenor2(void){
	ucTranType = appTenor3;
	strcpy(Pan, "03");
	SetReqBit61(MnuTenor3);
}
void tenor3(void){
	ucTranType = appTenor6;
	strcpy(Pan, "06");
	SetReqBit61(MnuTenor6);
}
void tenor4(void){
	ucTranType = appTenor9;
	strcpy(Pan, "09");
	SetReqBit61(MnuTenor9);
}
void tenor5(void){
	ucTranType = appTenor12;
	strcpy(Pan, "12");
	SetReqBit61(MnuTenor12);
}
void tenor6(void){
	ucTranType = appTenor18;
	strcpy(Pan, "18");
	SetReqBit61(MnuTenor18);
}
void tenor7(void){
	ucTranType = appTenor24;
	strcpy(Pan, "24");
	SetReqBit61(MnuTenor24);
}

static MenuItem BUKOPIN[]=
{
		{FALSE	, _T_NOOP(" TENOR  1"),tenor1},//1
		{FALSE	, _T_NOOP(" TENOR  3"),tenor2},//2
		{FALSE	, _T_NOOP(" TENOR  6"),tenor3},//3
		{FALSE	, _T_NOOP(" TENOR  9"),tenor4},//4
		{FALSE	, _T_NOOP(" TENOR 12"),tenor5},//5
		{FALSE	, _T_NOOP(" TENOR 18"),tenor6},//6
		{FALSE	, _T_NOOP(" TENOR 24"),tenor7},//7
		{FALSE,	"", NULL}
};
int ScrMenu(MenuItem *ScMn, uchar *ScHeader,int Count)
{
	int i=0; int j=0;
	MenuItem BuffMenu[20];
	MenuItem BlankMenu[]=
		{
		{FALSE,	"", NULL},
		{FALSE,	"", NULL}
		};
	MEM_ZERO(BuffMenu);
	while(1){
		if(ScMn[i].bVisible==TRUE){
			BuffMenu[j]=ScMn[i];
			j++;
		}
		if(ScMn[i+1].bVisible==TRUE || ScMn[i+1].bVisible==FALSE){
			i++;
			}
		else
			break;
		if(i>19 || i>Count)
			break;
	}
	ScrCls();
	PubShowTitle(TRUE, (uchar *)_T(ScHeader));
	return PubGetMenu(ScHeader, BuffMenu, MENU_AUTOSNO|MENU_CFONT, USER_OPER_TIMEOUT);
}

void SaleSub(void){
	ret=TransSaleSub();
	emv_test();
}
void CicilanSub(void){
	MEM_ZERO(SerialNo);
	MEM_ZERO(glProcInfo.szReq61CicilanBukopin);
	ReadSN(SerialNo);
	ret=ScrMenu(BUKOPIN,"INSTALMENT:",7);
	if(ret>=0){
		memcpy(Pan+2, glProcInfo.stTranLog.szPan, strlen(glProcInfo.stTranLog.szPan)-2);
		if(MatchCardTable(Pan) != 0){
			ScrCls();
			ScrPrint(0, 4, ASCII, "%21s", " CARD UNSUPPORT");
			ScrPrint(0, 6, ASCII, "%21s", " INSTALMENT");
			PubBeepErr();
			PubWaitKey(5);		
		}
		ret=TransCicilanBukopin();
		}
}

static MenuItem SelectSale[]=
{
		{TRUE	, _T_NOOP(" REGULAR   "),SaleSub},
		{FALSE	, _T_NOOP(" INSTALMENT"),CicilanSub},
		{TRUE	,	"", NULL}
};
int DisplayMenuSale(void){
	int i;
	#ifdef BANK_HSBC
	SelectSale[0].bVisible=FALSE;
	#else
	SelectSale[0].bVisible=TRUE;
	#endif
	SelectSale[1].bVisible=FALSE;
	BUKOPIN[MnuTenor1].bVisible	  =FALSE;
	for(i=MnuTenor3; i<7; i++){
		BUKOPIN[i].bVisible = CB[i].flagmenuTenor;
		if(CB[i].flagmenuTenor == TRUE)
			SelectSale[1].bVisible=TRUE;
		}
	if(SelectSale[1].bVisible==FALSE)
		SaleSub();
	else if(SelectSale[0].bVisible == FALSE || CB[MnuTenor1].flagmenuTenor == FALSE)
		CicilanSub();
	else
		ScrMenu(SelectSale,"SALE :",2);
	return 0;
}
void SetDefaultMenuCicilan(void){
	MEM_ZERO(CB);
	memcpy(CB,CicilanBukopinDefault,sizeof(CB));
}
int AllSetMenuCicilanBukopin(void){
	SetDefaultMenuCicilan();

	//SaveTlmSysParam();
	return 0;
}

int TransSubSaleNew(void)
{	
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
		
	return DisplayMenuSale();
	
}

int SetOnBukTenor(int tenor){
	BUKOPIN[tenor].bVisible = TRUE;
	CB[tenor].flagmenuTenor = TRUE ;
	return 0;
}
int SetOffBukTenor(int tenor){
	CB[tenor].flagmenuTenor = FALSE;
	return 0;
}
void SetManualMenuCicilan(void){
	int i; uchar buff[15+1];
	uchar tenor[7]={
		1,3,6,9,12,18,24
		};
	SelectSale[1].bVisible = FALSE;
	for(i=0; i<7; i++){
		if(	CB[i].flagmenuTenor==TRUE)
			sprintf(buff, "TENOR %2d| ON",tenor[i]);
		else
			sprintf(buff, "TENOR %2d|OFF",tenor[i]);

		ScrCls();
		switch(PubSelectZeroOne("MENU CICILAN",buff,"OFF","ON")){
		case KEY0:
			SetOffBukTenor(i);
			break;
		case KEY1:
			SetOnBukTenor(i);
			SelectSale[1].bVisible=TRUE;
			#ifdef BANK_PANIN
			ScrCls();
			PubShowTitle(TRUE, "MENU CICILAN");
			sprintf(buff, "INTEREST CODE");
			PubDispString(buff, DISP_LINE_CENTER|2);
			sprintf(buff, "TENOR %2d",tenor[i]);
			PubDispString(buff, DISP_LINE_CENTER|4);
			PubGetString(ALPHA_IN|NUM_IN,1,4,CB[i].InterestCode,USER_OPER_TIMEOUT);
			#else
			#ifndef BANK_HSBC
			ScrCls();
			PubShowTitle(TRUE, "MENU CICILAN");
			sprintf(buff, "INSTALMENT CODE");
			PubDispString(buff, DISP_LINE_CENTER|2);
			sprintf(buff, "TENOR %2d",tenor[i]);
			PubDispString(buff, DISP_LINE_CENTER|4);
			PubGetString(ALPHA_IN|NUM_IN,1,2,CB[i].InstallmentCode,USER_OPER_TIMEOUT);
			#endif
			#endif
			break;
		case KEYCANCEL:
			return;
			break;
		case KEYENTER:
			continue;
			break;
		default:
			break;

			}
	}
		
}
void SwitchMenuCicilan(void){
	MenuItem SelectSet[3]={
		{TRUE	, _T_NOOP(" DEFAULT   "),SetDefaultMenuCicilan},
		{TRUE	, _T_NOOP(" MANUAL SET"),SetManualMenuCicilan},
		{TRUE	,	"", NULL}
	};
	if(PasswordBank()!=0)
		return;
	ScrMenu(SelectSet,"SET CICILAN",2);
	SaveTlmSysParam();
}
int SetBit63_AJ(void){// untuk artha jasa
	int i;
	
	if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PURCHASE){
	 	i=REQ_PCH;
	 	MEM_ZERO(AJ[i].DestName); MEM_ZERO(AJ[i].IssuerID);
	 	sprintf(AJ[i].DestName				,"%-30s\0" ,AJ[RSP_INQ].DestName); //rsp INQ >> req PCH
		sprintf(AJ[i].IssuerID				,"%11s\0" ,AJ[RSP_INQ].IssuerID); //rsp INQ >> req PCH
	} else if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INQ_PURCHASE){
	 	i=REQ_INQ;
    	MEM_ZERO(AJ[i].DestName); MEM_ZERO(AJ[i].IssuerID);
	 	sprintf(AJ[i].DestName				,"%-30s\0" ," \0");  
		sprintf(AJ[i].IssuerID				,"%11s\0" ," \0"); 
	}
	MEM_ZERO(bit63req); 			MEM_ZERO(AJ[i].TableID);			MEM_ZERO(AJ[i].DestID);			MEM_ZERO(AJ[i].DestAccount); 
	MEM_ZERO(AJ[i].CustRefName);	MEM_ZERO(AJ[i].CardHolderAccount);	MEM_ZERO(AJ[i].CardHolderName);	MEM_ZERO(AJ[i].InfData);
	sprintf(AJ[i].TableID				,"%2s\0"  	,"99\0"); //req
	sprintf(AJ[i].DestID				,"%11s\0" 	,"00000000013\0"); //req
	sprintf(AJ[i].DestAccount			,"%-28s\0" 	,"01300013\0"); //req
	sprintf(AJ[i].CustRefName			,"%-16s\0" 	,"PURCHASE%.*s\0", LEN_TERM_ID,  glCurTlmTmsAcq.acqTID); //req
	
		sprintf(AJ[i].CardHolderAccount 	,"%-28s\0" ," \0"); //rsp PCH
		sprintf(AJ[i].CardHolderName 		,"%-100s\0"," \0"); //rsp PCH
		sprintf(AJ[i].InfData 				,"%-s\0",	"GI001\0"); //optional
		
	sprintf(bit63req,"%2s%11s%28s%30s%16s%11s%28s%100s%s\0",
		AJ[i].TableID,AJ[i].DestID,AJ[i].DestAccount,AJ[i].DestName,AJ[i].CustRefName,AJ[i].IssuerID,AJ[i].CardHolderAccount,AJ[i].CardHolderName,
		AJ[i].InfData);
	
	return strlen(bit63req);;
}

void GetBit63_AJ(char *bit63rsp){// untuk artha jasa
	int i; int lnNext=2;
	
	if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_INQ_PURCHASE)
	 	i=RSP_INQ; 	
	else if(glProcInfo.stTranLog.ucTranType==TRANS_TYPE_PURCHASE)
	 	i=RSP_PCH;

	MEM_ZERO(AJ[i].TableID);	MEM_ZERO(AJ[i].DestID);		MEM_ZERO(AJ[i].DestAccount); 		MEM_ZERO(AJ[i].CustRefName);
	MEM_ZERO(AJ[i].DestName);	MEM_ZERO(AJ[i].IssuerID);	MEM_ZERO(AJ[i].CardHolderAccount);	MEM_ZERO(AJ[i].CardHolderName);
	MEM_ZERO(AJ[i].InfData);
	memcpy(AJ[i].TableID,				bit63rsp+lnNext		,2	); lnNext+=2;  //req
	memcpy(AJ[i].DestID,				bit63rsp+lnNext		,11	); lnNext+=11; //req
	memcpy(AJ[i].DestAccount,			bit63rsp+lnNext		,28	); lnNext+=28; //req
	memcpy(AJ[i].DestName,				bit63rsp+lnNext		,30	); lnNext+=30; //req
	memcpy(AJ[i].CustRefName,			bit63rsp+lnNext		,16	); lnNext+=16; //req
	memcpy(AJ[i].IssuerID,				bit63rsp+lnNext		,11	); lnNext+=11; //req
	memcpy(AJ[i].CardHolderAccount,		bit63rsp+lnNext		,28	); lnNext+=28; //req
	memcpy(AJ[i].CardHolderName,		bit63rsp+lnNext		,100); lnNext+=100;//req
	sprintf(AJ[i].InfData,				bit63rsp+lnNext		); 

	return ;
}

void ISENG(void){
	uchar BuffIseng[32]; uchar DispIseng[32]; int indeex;
	ziTimeIsengA=0;	indeex=0;
	sprintf(BuffIseng,"%16.16s %13.13s \0","SWIPE/INSERT ...",EDC_VER_INTERN);
	while(1){
		ziTimeIsengB=31-ziTimeIsengA;
		memcpy(DispIseng,BuffIseng+ziTimeIsengA,ziTimeIsengB);
		memcpy(DispIseng+ziTimeIsengB,BuffIseng,ziTimeIsengA);
	if(ziTimeIsengA==31)
		ziTimeIsengA=0;
	ziTimeIsengA++;
	ScrPrint(0, 7, ASCII,"%21.21s", DispIseng);
	DelayMs(500);
	if( kbhit()==0x00 )
		break;
	}
}

void RollOut(void){
	uchar WKEY[16+1];
	char WKEY1[16+1];
	char WKEY2[16+1];
	uchar MKEY[16+1];
	char MKEY1[16+1];
	char MKEY2[16+1];
	char WKEYSUM[32+1]; int iRet;
	char MKEYSUM[32+1];
	uchar	ucAcqIndex, szTitle[16+1];
	
	if(PasswordBank()!=0)
		return;

	//PrnCustomLogo_T();
	//uchar SerialNo[99];
	//uint ret;
	//ISENG();
	//PrnInit();
	//PrnStr("SerialNo : %s(%d)",SerialNo,ret);
	//PrnStep(3);
	//StartPrinter();
	//PrnStep(3);
	//*/	
	memset(WKEY1,0,sizeof(WKEY1));
	memset(WKEY2,0,sizeof(WKEY2));
	memset(WKEY,0,sizeof(WKEY));
	memset(MKEY1,0,sizeof(MKEY1));
	memset(MKEY2,0,sizeof(MKEY2));
	memset(MKEY,0,sizeof(MKEY));
	do
		{
	#ifdef	__TELIUM_PARAM__
	sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#else
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#endif
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}
	
	ScrCls();
	PubDispString("INPUT WK ?", DISP_LINE_CENTER|2);
	if(PubYesNo(-1)==0){
	ScrCls();
	PubDispString("MASUKAN WK", DISP_LINE_CENTER|2);
	PubGetString(ALPHA_IN|NUM_IN,16,16,WKEY1,USER_OPER_TIMEOUT);
	PubDispString(WKEY1, DISP_LINE_CENTER|4);
	PubGetString(ALPHA_IN|NUM_IN,16,16,WKEY2,USER_OPER_TIMEOUT);
	memcpy(WKEYSUM,WKEY1,16); memcpy(WKEYSUM+16,WKEY2,16);
	hex2bin(WKEY,WKEYSUM,32);
	memset(glTlmSysParam.Acq[ucAcqIndex].acqWorkingKey,0,sizeof(glTlmSysParam.Acq[ucAcqIndex].acqWorkingKey));
	memcpy(glTlmSysParam.Acq[ucAcqIndex].acqWorkingKey,WKEY,16);
	}
	ScrCls();
	PubDispString("INPUT WK ?", DISP_LINE_CENTER|2);
	if(PubYesNo(-1)==0){
	ScrCls();
	PubDispString("MASUKAN MK", DISP_LINE_CENTER|2);
	PubGetString(ALPHA_IN|NUM_IN,16,16,MKEY1,USER_OPER_TIMEOUT);
	PubDispString(MKEY1, DISP_LINE_CENTER|4);
	PubGetString(ALPHA_IN|NUM_IN,16,16,MKEY2,USER_OPER_TIMEOUT);
	memcpy(MKEYSUM,WKEY1,16); memcpy(MKEYSUM+16,MKEY2,16);
	hex2bin(MKEY,MKEYSUM,32); 
	memset(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey1,0,sizeof(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey1));
	memcpy(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey1,MKEY,8);
	memset(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey2,0,sizeof(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey2));
	memcpy(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey2,MKEY+8,8);
	}
	
	SaveSysCtrlAll();
	SaveTlmSysParam();
	PubDispString("INPUT AGAIN ?", DISP_LINE_CENTER|2);
	}while(PubYesNo(-1)==0);
	//*/
}

void emv_test(void){
	#ifdef ENABLE_EMV	// zaki 7 Januari 2014	
				if(debugEMV)
				{	PrnInit(); 
				//#ifdef BANK_BRI //untuk sertifikasi
					PrnCustomLogo_T();
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
	#endif
}
void tabungan(void){
	tipeTab='1';
}
void giro(void){
	tipeTab='2';
}
void JenisRekening(void){
	MenuItem JenisTabungan[]=
	{
		{TRUE	, _T_NOOP(" TABUNGAN  "),tabungan},
		{TRUE	, _T_NOOP(" GIRO      "),giro},
		{TRUE	,	"", NULL}
	};
	ScrMenu(JenisTabungan,"JENIS REKENING",2);
}
void ModifProCode(void){	
	//memcpy((char *)glSendPack.szBit3+2,tipeTab,1);
	glSendPack.szBit3[2]=tipeTab;
}
void INPUT_WK(void){
	uchar WKEY[16+1];
	char WKEY1[16+1];
	char WKEY2[16+1];
	char WKEYSUM[32+1]; int iRet;
	uchar	ucAcqIndex, szTitle[16+1];
	
	if(PasswordBank()!=0)
		return;
	//*/	
	memset(WKEY1,0,sizeof(WKEY1));
	memset(WKEY2,0,sizeof(WKEY2));
	memset(WKEY,0,sizeof(WKEY));
	do
		{
	#ifdef	__TELIUM_PARAM__
	sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#else
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#endif
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}
	
	ScrCls();
	PubDispString("INPUT WK ?", DISP_LINE_CENTER|2);
	if(PubYesNo(-1)==0){
	ScrCls();
	PubDispString("MASUKAN WK", DISP_LINE_CENTER|2);
	PubGetString(ALPHA_IN|NUM_IN,16,16,WKEY1,USER_OPER_TIMEOUT);
	PubDispString(WKEY1, DISP_LINE_CENTER|4);
	PubGetString(ALPHA_IN|NUM_IN,16,16,WKEY2,USER_OPER_TIMEOUT);
	memcpy(WKEYSUM,WKEY1,16); memcpy(WKEYSUM+16,WKEY2,16);
	hex2bin(WKEY,WKEYSUM,32);
	memset(glTlmSysParam.Acq[ucAcqIndex].acqWorkingKey,0,sizeof(glTlmSysParam.Acq[ucAcqIndex].acqWorkingKey));
	memcpy(glTlmSysParam.Acq[ucAcqIndex].acqWorkingKey,WKEY,16);
	}
	SaveSysCtrlAll();
	SaveTlmSysParam();
	PubDispString("INPUT AGAIN ?", DISP_LINE_CENTER|2);
	}while(PubYesNo(-1)==0);
	return;
}

void INPUT_MK(void){
	uchar MKEY[16+1];
	char MKEY1[16+1];
	char MKEY2[16+1];
	int iRet;
	char MKEYSUM[32+1];
	uchar	ucAcqIndex, szTitle[16+1];
	
	if(PasswordBank()!=0)
		return;
	//*/	
	memset(MKEY1,0,sizeof(MKEY1));
	memset(MKEY2,0,sizeof(MKEY2));
	memset(MKEY,0,sizeof(MKEY));
	do
		{
	#ifdef	__TELIUM_PARAM__
	sprintf((char *)szTitle, "%.16s", glTlmSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#else
	sprintf((char *)szTitle, "%.16s", glSysParam.ucAcqNum>8 ? "SELECT ACQ:" : "SELECT ACQUIRER");
#endif
	iRet = SelectAcq(FALSE, szTitle, &ucAcqIndex);
	if( iRet!=0 )
	{
		return;
	}
	
	ScrCls();
	PubDispString("INPUT MK ?", DISP_LINE_CENTER|2);
	if(PubYesNo(-1)==0){
	ScrCls();
	PubDispString("MASUKAN MK", DISP_LINE_CENTER|2);
	PubGetString(ALPHA_IN|NUM_IN,16,16,MKEY1,USER_OPER_TIMEOUT);
	PubDispString(MKEY1, DISP_LINE_CENTER|4);
	PubGetString(ALPHA_IN|NUM_IN,16,16,MKEY2,USER_OPER_TIMEOUT);
	memcpy(MKEYSUM,MKEY1,16); memcpy(MKEYSUM+16,MKEY2,16);
	hex2bin(MKEY,MKEYSUM,32); 
	memset(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey1,0,sizeof(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey1));
	memcpy(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey1,MKEY,8);
	memset(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey2,0,sizeof(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey2));
	memcpy(glTlmSysParam.Acq[ucAcqIndex].acqMasterKey2,MKEY+8,8);
	}
	
	SaveSysCtrlAll();
	SaveTlmSysParam();
	PubDispString("INPUT AGAIN ?", DISP_LINE_CENTER|2);
	}while(PubYesNo(-1)==0);
	return;
}

int CheckExpBypassPin(void)
{
	uchar CurDate[14+1];
	card numCur,numExp;
	
	GetDateTime(CurDate);
	//hex_dump_char("CurDate",CurDate, 8);
	//hex_dump_char("ExpDate",glTlmSysParam.Con.appVendorID, 8);
	
	//verifikasi tahun
	dec2num(&numCur, CurDate, 4); dec2num(&numExp,glTlmSysParam.Con.appVendorID, 4);
	if(numCur<numExp)
		return 0;
	else if(numCur>numExp)
		return 1;
	
	dec2num(&numCur, CurDate+4, 2); dec2num(&numExp,glTlmSysParam.Con.appVendorID+4, 2); //verifikasi bulan 
	if(numCur<numExp)
		return 0;
	else if(numCur>numExp)
		return 1;

	dec2num(&numCur, CurDate+6, 2); dec2num(&numExp,glTlmSysParam.Con.appVendorID+6, 2); //verifikasi hari
	if(numCur>=numExp)
		return 1;
	return 0;
}




int TransSubSaleNewPrvtLbl(void)
{	
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

	//TransSaleSubPrvtLbl();
	return DisplayMenuSalePrvtLbl();
		
	//return DisplayMenuSale();
	//return 0;
	
}


int StartTapPrvtLbl(void)
{	
	uchar UID[11+1];

	uchar Data[100];
	uchar DataBuff1[2];
	uchar DataBuff2[2];
	uchar DataBuff3[40];
	uchar DataBuff3HolderName[40];
	uchar szTempName[40];

	int ilength, i, iRet;

	memset(Data, 0, sizeof(Data));
	memset(DataBuff1, 0, sizeof(DataBuff1));
	memset(DataBuff2, 0, sizeof(DataBuff2));
	memset(DataBuff3, 0, sizeof(DataBuff3));
	memset(DataBuff3HolderName, 0, sizeof(DataBuff3HolderName));
	memset(szTempName, 0, sizeof(szTempName));
	
	memset(UID,0,sizeof(UID));


	glProcInfo.stTranLog.uiEntryMode = MODE_CONTACTLESS;
		

	
	if(GetVersionPrvtLbl(UID)!=TRUE)
			return ;

	if(SelectAIDPrvtLbl()!=TRUE){
		return;
		}

	

	///if(ReadAFL()!=TRUE)
			///return;

	//if(ReadRec1Sfi08()!=TRUE)
	//		return;

	if(ReadRec1Sfi08(Data)==TRUE)
	{
		///ParseCI_Header(Data, &CI_Header);
		memcpy(DataBuff1,Data,1);
		memcpy(DataBuff2,Data+1,1);
		memcpy(DataBuff3,Data+1,40);

		// add for holder name :
		memcpy(DataBuff3HolderName,Data+23,20);




		
		/*
		PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---ReadRec1Sfi08 --");
		PrnStr("\n");
		hex_dump_char("DATA! Data!:", Data, strlen(Data));
		PrnStr("\n");
		PrnStr("databuff1 :\n");
		PrnStr(DataBuff1);
		PrnStr("\n");
		PrnStr("databuff2 :\n");
		PrnStr(DataBuff2);
		PrnStr("\n");
		PrnStr("\n");
		PrnStr("databuff3 :\n");
		PrnStr(DataBuff3);
		PrnStr("\n");
		PrnStr("databuff3 hexdum :\n");
		hex_dump_char("DATA! hexdum buff3!:", DataBuff3, sizeof(DataBuff3));
		PrnStr("\n");
		*/
		




	ilength = 18;	
	PubBcd2Asc0(DataBuff3, ilength, glProcInfo.szTrack2);
		PubTrimTailChars(glProcInfo.szTrack2, 'F');	// erase padded 'F' chars
		for(i=0; glProcInfo.szTrack2[i]!='\0'; i++)		// convert 'D' to '='
		{
			if( glProcInfo.szTrack2[i]=='D' )
			{
				glProcInfo.szTrack2[i] = '=';
				break;
			}
		}

	//memcpy(glProcInfo.stTranLog.szPan, glProcInfo.szTrack2, 16);

		iRet = GetPanFromTrackPrvtLbl(glProcInfo.stTranLog.szPan, glProcInfo.stTranLog.szExpDate);
		if( iRet!=0 )
		{
			DispMagReadErr();
			PubBeepErr();
			PubWaitKey(3);
			return ERR_NO_DISP;
		}


		// add for holder name
		//sprintf((char *)glProcInfo.stTranLog.szHolderName, "%.20s", sTemp);
		ConvertHolderNamePrvtLbl(DataBuff3HolderName, szTempName);
		sprintf((char *)glProcInfo.stTranLog.szHolderName, "%.20s", szTempName);


	/*
	PrnInit();
	PrnSetNormal();
	PrnStr("\n");
	PrnStr("\n");
	PrnStr("---track 2 nya :--");
	PrnStr("\n");
	PrnStr(glProcInfo.szTrack2);
	PrnStr("\n");
	PrnStr("---PAN nya :--");
	PrnStr("\n");
	PrnStr(glProcInfo.stTranLog.szPan);
	PrnStr("\n");
	PrnStr("\n");
	PrnStr("---EXP nya :--");
	PrnStr("\n");
	PrnStr(glProcInfo.stTranLog.szExpDate);
	PrnStr("\n");
	PrnStr("\n");
	PrnStr("---szHolderName nya :--");
	PrnStr("\n");
	PrnStr(glProcInfo.stTranLog.szHolderName);
	PrnStr("\n");
	StartPrinter();
	*/


	
		return 0;
	}
	else
		return -1;


	//	return;
	//ReadAFL();
	//ReadRec1Sfi08();
	///ReadRec1Sfi18();
	///ReadRec2Sfi18();
	


	/*
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
	*/
	
	DispProcess();


	return 0;
	
}


int GetVersionPrvtLbl(byte *UID)
{
	uchar datainfo[50+1];
	memset(datainfo,0,sizeof(datainfo));
	
	dspClear();
	dspLS(1, "TAP CARD");
	dspLS(2, "TO SCREEN");
		
	if(PollForTokenPrvtLbl(UID, datainfo)!=TRUE)
		{

		/*
		PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---bbbbbb--");
		PrnStr("\n");
		MultiLngPrnStr(datainfo, DISP_LINE_CENTER);
		PrnStr("\n");
		PrnStr("---isi bbb--");
		PrnStr("\n");
		hex_dump_char("DATA aaa:", datainfo, sizeof(datainfo));
		PrnStr("\n");
		StartPrinter();
		*/
		
		return FALSE;
		}


	/*
	PrnInit();
	PrnSetNormal();
	//memcpy(unmask,glCurAcq.acqMID,lenMid);
	//sprintf(szBuff,"MERCHANT ID :%30s\n", unmask);
	PrnStr("\n");
	PrnStr("---aaaaaaa--");
	PrnStr("\n");
	MultiLngPrnStr(datainfo, DISP_LINE_CENTER);
	PrnStr("\n");
	PrnStr("---isi aaa--");
	PrnStr("\n");
	hex_dump_char("DATA bbb:", datainfo, sizeof(datainfo));
	PrnStr("\n");
	StartPrinter();
	*/
	
	

	return TRUE;
 
}

int PollForTokenPrvtLbl(char *UID, uchar *infodata)
//uchar PollForTokenPrvtLbl(char *UID)
{
	uchar ucRet;
	uchar SerialInfo[11+1];
	uchar CardType;
	uchar mode;
	uchar CID;

	
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
				memcpy(infodata, SerialInfo, strlen(SerialInfo));
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



int SelectAIDPrvtLbl(void)
{
	uchar CardResponse[100];
	//,00,A4,04,00,07,A0,00,00,00,65,00,00,00
	//0x00,0xA4,0x04,0x00,0x07,0xA0,0x00,0x00,0x00,0x65,0x00,0x00,0x00
	unsigned char selectApp1[13]={0x00,0xA4,0x04,0x00,0x07,0xA0,0x00,0x00,0x00,0x65,0x00,0x00,0x00};
	uchar ucResult;
	uint uiCardResponse;
	
	memset(CardResponse, 0, sizeof(CardResponse));
	ucResult = PiccCmdExchange(13, selectApp1, &uiCardResponse, CardResponse); // Select App 1


		/*
		PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---isiCardResponse --");
		PrnStr("\n");
		hex_dump_char("DATA selectApp1:", selectApp1, sizeof(selectApp1));
		PrnStr("\n");
		PrnStr("\n");
		hex_dump_char("DATA CardResponse:", CardResponse, sizeof(CardResponse));
		PrnStr("\n");
		StartPrinter();
		*/
		
		
		
	if(ucResult!=0)
		return FALSE;

	////if(CardResponse[0]!=0)
	////	return FALSE;
	
	return TRUE;
}


int ReadAFL(void)
{
	uchar nResult;
	int nCardResponse;
	unsigned char CardResponse[100];
	// 0x80,0xA8,0x00,0x00,0x04,0x83,0x02,0x00,0x00,0x00
	unsigned char ReadAFLRec[10]={0x80,0xA8,0x00,0x00,0x04,0x83,0x02,0x00,0x00,0x00};

	nCardResponse = sizeof(CardResponse);

	memset(CardResponse, 0, sizeof(CardResponse));
	nResult = PiccCmdExchange(10,ReadAFLRec,&nCardResponse,CardResponse);

	PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---isiCardResponse --");
		PrnStr("\n");
		hex_dump_char("DATA2 ReadAFLRec:", ReadAFLRec, sizeof(ReadAFLRec));
		PrnStr("\n");
		PrnStr("\n");
		hex_dump_char("DATA2 CardResponse:", CardResponse, sizeof(CardResponse));
		PrnStr("\n");
		StartPrinter();
		
	if(CardResponse[0] != 0x00)
		return FALSE;

	///memcpy(Data,CardResponse+1,23);

	return TRUE;
}


//READ RECORD 1 OF SFI 08>>
int ReadRec1Sfi08(byte *Data)
{
	uchar nResult;
	int nCardResponse;
	unsigned char CardResponse[100];
	//0x00,0xB2,0x02,0x1C,0x00
	// 00 B2 01 0C 00
	unsigned char ReadRec[5]={0x00,0xB2,0x01,0x0C,0x00};

	nCardResponse = sizeof(CardResponse);

	memset(CardResponse, 0, sizeof(CardResponse));
	nResult = PiccCmdExchange(5,ReadRec,&nCardResponse,CardResponse);

	/*
	PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---isiCardResponse --");
		PrnStr("\n");
		hex_dump_char("DATA2 ReadRec1Sfi08:", ReadRec, sizeof(ReadRec));
		PrnStr("\n");
		PrnStr("\n");
		hex_dump_char("DATA2 CardResponse:", CardResponse, sizeof(CardResponse));
		PrnStr("\n");
		StartPrinter();
		*/
		
		
	///if(CardResponse[0] != 0x00)
	///	return FALSE;

	memcpy(Data,CardResponse+3,sizeof(CardResponse));

	return TRUE;
}



//READ RECORD 1 OF SFI 18>
int ReadRec1Sfi18(void)
{
	uchar nResult;
	int nCardResponse;
	unsigned char CardResponse[100];
	//0x00,0xB2,0x02,0x1C,0x00
	// 00 B2 01 1C 00
	unsigned char ReadRec[5]={0x00,0xB2,0x01,0x1C,0x00};

	nCardResponse = sizeof(CardResponse);

	memset(CardResponse, 0, sizeof(CardResponse));
	nResult = PiccCmdExchange(5,ReadRec,&nCardResponse,CardResponse);

	PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---isiCardResponse --");
		PrnStr("\n");
		hex_dump_char("DATA2 ReadRec1Sfi18:", ReadRec, sizeof(ReadRec));
		PrnStr("\n");
		PrnStr("\n");
		hex_dump_char("DATA2 CardResponse:", CardResponse, sizeof(CardResponse));
		PrnStr("\n");
		StartPrinter();
		
	if(CardResponse[0] != 0x00)
		return FALSE;

	///memcpy(Data,CardResponse+1,23);

	return TRUE;
}



// READ RECORD 2 OF SFI 18
int ReadRec2Sfi18(void)
{
	uchar nResult;
	int nCardResponse;
	unsigned char CardResponse[100];
	//0x00,0xB2,0x02,0x1C,0x00
	unsigned char ReadRec[5]={0x00,0xB2,0x02,0x1C,0x00};

	nCardResponse = sizeof(CardResponse);

	memset(CardResponse, 0, sizeof(CardResponse));
	nResult = PiccCmdExchange(5,ReadRec,&nCardResponse,CardResponse);

	PrnInit();
		PrnSetNormal();
		PrnStr("\n");
		PrnStr("---isiCardResponse --");
		PrnStr("\n");
		hex_dump_char("DATA2 ReadRec2Sfi18:", ReadRec, sizeof(ReadRec));
		PrnStr("\n");
		PrnStr("\n");
		hex_dump_char("DATA2 CardResponse:", CardResponse, sizeof(CardResponse));
		PrnStr("\n");
		StartPrinter();
		
	if(CardResponse[0] != 0x00)
		return FALSE;

	///memcpy(Data,CardResponse+1,23);

	return TRUE;
}




int Req_ICC_PrvtLbl(void)
{
    	byte Buf[100];          // Buffer to accept the input
    	byte Buf222[100];          // Buffer to accept the input
    	byte Buf2[56+1];
    	int ret; // icc = 0;

    	char dataIn[16+1];	

		uchar DataBuff1[2];
		uchar DataBuff2[2];
		uchar DataBuff3[40];
		uchar DataBuff3HolderName[40];
		uchar szTempName[40];
	
	uchar buffer[999];
	uchar ucRet;

	int ilength, iRet, i;



	ScrCls();
	PubDispString("INSERT",  DISP_LINE_CENTER|2);
	PubDispString("CARD",  DISP_LINE_CENTER|4);

	glProcInfo.stTranLog.uiEntryMode = MODE_CHIP_INPUT;

    TimerSet(0, 300);       // 300 = 30 s //added by irf, 

	IccClose(0);
	while( 1 )
	{
		if ((kbhit()==0) && (getkey()==KEYCANCEL))
		{
			return -1;
		}
		if( IccDetect(ICC_USER)==0 )
		{
			break;
		}
		if(!TimerCheck(0)) 	//timeout
       	{           
                   return -1;
       	}
	}
    	//---------------------------------------------------------------
    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
	ucRet = IccInit(0x80, (uchar *) Buf);
	CHECK(ucRet == 0, lblKO);

    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
		memset(dataIn, 0, sizeof(dataIn));
    	///memcpy(dataIn, "\xA0\x00\x00\x00\x18\x4b\x65\x6b\x53\x61\x6d\x00\x00\x01\x01\x02", 16);	
    	//unsigned char selectApp1[13]={0x00,0xA4,0x04,0x00,0x07,0xA0,0x00,0x00,0x00,0x65,0x00,0x00,0x00};
		// \x07\xA0\x00\x00\x00\x65\x00\x00\x00
		memcpy(dataIn, "\xA0\x00\x00\x00\x65\x00\x00", 7);	
    	ret = SelectAIDPrvtLblICC(dataIn, 0, Buf);
		////hex_dump_char("DATA2 Buf:", Buf, sizeof(Buf));
	CHECK(ret == 0, lblKO);	
    	//---------------------------------------------------------------
    	//memset(SessionKey, 0, sizeof(SessionKey));
    	//memset(EncSessionKey, 0, sizeof(EncSessionKey));
    	///ret = doRSA(SessionKey, EncSessionKey);
	////CHECK(ret == 0, lblKO);	


		memset(Buf222, 0, sizeof(Buf222));
		memset(dataIn, 0, sizeof(dataIn));
				memcpy(dataIn, "", 0); 
				// \x07\xA0\x00\x00\x00\x65\x00\x00\x00
				// 00\xB2\x01\x0C\x00
				///memcpy(dataIn, " \x07\xA0\x00\x00\x00\x65\x00\x00\x00", 9); 
				ret = SelectRec1PrvtLbl(dataIn, 0, Buf222);
				////hex_dump_char("DATA2 Buf222:", Buf222, sizeof(Buf222));
			CHECK(ret == 0, lblKO); 

		
	////DelayMs(100);


	memset(DataBuff1, 0, sizeof(DataBuff1));
	memset(DataBuff2, 0, sizeof(DataBuff2));
	memset(DataBuff3, 0, sizeof(DataBuff3));
	memset(DataBuff3HolderName, 0, sizeof(DataBuff3HolderName));
	memset(szTempName, 0, sizeof(szTempName));

	memcpy(DataBuff1,Buf222+4,1);
	memcpy(DataBuff2,Buf222+4,1);
	memcpy(DataBuff3,Buf222+4,40);
	// add for holder name :
	memcpy(DataBuff3HolderName,Buf222+26,20);
	

	ilength = 18;	
	PubBcd2Asc0(DataBuff3, ilength, glProcInfo.szTrack2);
		PubTrimTailChars(glProcInfo.szTrack2, 'F');	// erase padded 'F' chars
		for(i=0; glProcInfo.szTrack2[i]!='\0'; i++)		// convert 'D' to '='
		{
			if( glProcInfo.szTrack2[i]=='D' )
			{
				glProcInfo.szTrack2[i] = '=';
				break;
			}
		}

	//memcpy(glProcInfo.stTranLog.szPan, glProcInfo.szTrack2, 16);

		iRet = GetPanFromTrackPrvtLbl(glProcInfo.stTranLog.szPan, glProcInfo.stTranLog.szExpDate);
		if( iRet!=0 )
		{
			DispMagReadErr();
			PubBeepErr();
			PubWaitKey(3);
			return ERR_NO_DISP;
		}



	// add for holder name
	//sprintf((char *)glProcInfo.stTranLog.szHolderName, "%.20s", sTemp);
	ConvertHolderNamePrvtLbl(DataBuff3HolderName, szTempName);
	sprintf((char *)glProcInfo.stTranLog.szHolderName, "%.20s", szTempName);

	/*
	PrnInit();
	PrnSetNormal();
	PrnStr("\n");
	PrnStr("\n");
	PrnStr("---track 2 nya DIP :--");
	PrnStr("\n");
	PrnStr(glProcInfo.szTrack2);
	PrnStr("\n");
	PrnStr("---PAN nya DIP :--");
	PrnStr("\n");
	PrnStr(glProcInfo.stTranLog.szPan);
	PrnStr("\n");
	PrnStr("\n");
	PrnStr("---EXP nya DIP :--");
	PrnStr("\n");
	PrnStr(glProcInfo.stTranLog.szExpDate);
	PrnStr("\n");
	PrnStr("\n");
	PrnStr("---szHolderName nya :--");
	PrnStr("\n");
	PrnStr(glProcInfo.stTranLog.szHolderName);
	PrnStr("\n");
	StartPrinter();
	*/
	


    
goto lblEnd;
lblKO:
     	IccClose(0);
      	ScrCls();
      	PubDispString("UNSUCCESSFUL", DISP_LINE_CENTER|2);
	PubDispString(_T("PLS REMOVE CARD"), 4|DISP_LINE_CENTER);
	while( IccDetect(ICC_USER)==0 )
	{
		Beep();
		DelayMs(500);
	} 
      	IccClose(0);
		
      	return -1;
lblEnd:
      IccClose(0);
//   ScrCls();
      return 0;
}



int SelectAIDPrvtLblICC(byte* DataIn, byte p2, byte* rsp){
	byte cmd[6+1];
	byte dat[24+2]; 			// first byte is length of dat (16 bytes)
	int ucRet;

	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	//unsigned char selectApp1[13]={0x00,0xA4,0x04,0x00,0x07,0xA0,0x00,0x00,0x00,0x65,0x00,0x00,0x00};

	MEM_ZERO(cmd);
	MEM_ZERO(dat);
	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);
	
	ApduSend.Command[0] = 0x00;				//STAPDUCMD.CLA
	ApduSend.Command[1] = 0xA4;				//STAPDUCMD.INS
	ApduSend.Command[2] = 0x04;		    	//STAPDUCMD.P1
	ApduSend.Command[3] = 0x00;				//STAPDUCMD.P2

	ApduSend.Lc = 7;  					//5STAPDUCMD.P3
	///ApduSend.Le = 0;

	memcpy(ApduSend.DataIn, DataIn, 7);                 	// 3 bytes of LMTK_ACQ_ID
	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	////hex_dump_char("\nDATA &ApduSend:", &ApduSend, sizeof(&ApduSend));
	////hex_dump_char("\nDATA &ApduResp:", &ApduResp, sizeof(&ApduResp));

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			return 0;
		}
		else
			DisplayErrorKekSAM(&ApduResp);
	}
	

	return -1;

}


int SelectRec1PrvtLbl(byte* DataIn, byte p2, byte* rsp){
	byte cmd[6+1];
	byte dat[24+2]; 			// first byte is length of dat (16 bytes)
	int ucRet;

	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	MEM_ZERO(cmd);
	MEM_ZERO(dat);
	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);

	// 00\xB2\x01\x0C\x00
	
	ApduSend.Command[0] = 0x00;				//STAPDUCMD.CLA
	ApduSend.Command[1] = 0xB2;				//STAPDUCMD.INS
	ApduSend.Command[2] = 0x01;		    	//STAPDUCMD.P1
	ApduSend.Command[3] = 0x0C;				//STAPDUCMD.P2

	ApduSend.Lc = 0;  					//5STAPDUCMD.P3

	memcpy(ApduSend.DataIn, DataIn, 0);                 	// 3 bytes of LMTK_ACQ_ID
	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			return 0;
		}
		else
			DisplayErrorKekSAM(&ApduResp);
	}
	

	return -1;

}




int GetPanFromTrackPrvtLbl(uchar *pszPAN, uchar *pszExpDate)
{
	int		iPanLen;
	char	*p, *pszTemp;

	// 从2磁道开始到'＝'
	if( strlen((char *)glProcInfo.szTrack2)>0 )
	{
		pszTemp = (char *)glProcInfo.szTrack2;
	}
	else if( strlen((char *)glProcInfo.szTrack3)>0 )
	{
		pszTemp = (char *)&glProcInfo.szTrack3[2];
	}
	else
	{	// 2、3磁道都没有
		return ERR_SWIPECARD;
	}

	p = strchr((char *)pszTemp, '=');
	if( p==NULL )
	{
		return ERR_SWIPECARD;
	}
	iPanLen = p - pszTemp;
	if( iPanLen<13 || iPanLen>19 )
	{
		return ERR_SWIPECARD;
	}

	sprintf((char *)pszPAN, "%.*s", iPanLen, pszTemp);
	if( pszTemp==(char *)glProcInfo.szTrack2 )
	{
		sprintf((char *)pszExpDate, "%.4s", p+1);
	}
	else
	{
		sprintf((char *)pszExpDate, "0000");
	}

	return 0;
}



void ConvertHolderNamePrvtLbl(uchar *pszOrgName, uchar *pszNormalName)
{
	char	*pszTitle, *pszMidName, *pszTemp, szBuff[50];

	sprintf((char *)pszNormalName, "%s", pszOrgName);
	if( *pszOrgName==0 )
	{
		return;
	}
	pszTemp = (char *)pszNormalName;

	pszMidName = strchr((char *)pszOrgName, '/');
	if( pszMidName==NULL )
	{
		return;
	}

	pszTitle = strrchr((char *)pszOrgName, '.');
	if( pszTitle!=NULL )
	{
		sprintf(szBuff, "%s ", pszTitle+1);
		PubTrimStr((uchar *)szBuff);
		pszTemp += sprintf(pszTemp, "%s ", szBuff);

		sprintf(szBuff, "%.*s ", (int)(pszTitle-pszMidName-1), pszMidName+1);
		PubTrimStr((uchar *)szBuff);
		pszTemp += sprintf(pszTemp, "%s ", szBuff);
	}
	else
	{
		sprintf(szBuff, "%s", pszMidName+1);
		PubTrimStr((uchar *)szBuff);
		pszTemp += sprintf(pszTemp, "%s ", szBuff);
	}
	sprintf(pszTemp, "%.*s", (int)(pszMidName-(char *)pszOrgName), pszOrgName);
}



void SaleSubPrvtLbl(void){
	ret=TransSaleSubPrvtLbl();
	//emv_test();
}


void CicilanSubPrvtLbl(void){
	MEM_ZERO(SerialNo);
	MEM_ZERO(glProcInfo.szReq61CicilanBukopin);
	ReadSN(SerialNo);
	ret=ScrMenu(BUKOPIN,"INSTALMENT:",7);
	if(ret>=0){
		memcpy(Pan+2, glProcInfo.stTranLog.szPan, strlen(glProcInfo.stTranLog.szPan)-2);
		if(MatchCardTable(Pan) != 0){
			ScrCls();
			ScrPrint(0, 4, ASCII, "%21s", " CARD UNSUPPORT");
			ScrPrint(0, 6, ASCII, "%21s", " INSTALMENT");
			PubBeepErr();
			PubWaitKey(5);		
		}
		ret=TransCicilanBukopinPrvtLbl();
		}
}



static MenuItem SelectSalePrvtLbl[]=
{
		{TRUE	, _T_NOOP(" REGULAR   "),SaleSubPrvtLbl},
		{FALSE	, _T_NOOP(" INSTALMENT"),CicilanSubPrvtLbl},
		{TRUE	,	"", NULL}
};


int DisplayMenuSalePrvtLbl(void){
	int i;
	#ifdef BANK_HSBC
	SelectSalePrvtLbl[0].bVisible=FALSE;
	#else
	SelectSalePrvtLbl[0].bVisible=TRUE;
	#endif
	SelectSalePrvtLbl[1].bVisible=FALSE;
	BUKOPIN[MnuTenor1].bVisible	  =FALSE;
	for(i=MnuTenor3; i<7; i++){
		BUKOPIN[i].bVisible = CB[i].flagmenuTenor;
		if(CB[i].flagmenuTenor == TRUE)
			SelectSalePrvtLbl[1].bVisible=TRUE;
		}
	if(SelectSalePrvtLbl[1].bVisible==FALSE)
		////SaleSub();
		SaleSubPrvtLbl();
	else if(SelectSalePrvtLbl[0].bVisible == FALSE || CB[MnuTenor1].flagmenuTenor == FALSE)
		////CicilanSub();
		CicilanSubPrvtLbl();
	else
		ScrMenu(SelectSalePrvtLbl,"SALE :",2);
	return 0;
}






