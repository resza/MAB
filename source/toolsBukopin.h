#ifndef _TOOLSBUKOPIN_H
#define _TOOLSBUKOPIN_H

typedef struct {
	char TableID[2+1];
	char DestID[11+1];
	char DestAccount[28+1];
	char DestName[30+1];
	char CustRefName[16+1];
	char IssuerID[11+1];
	char CardHolderAccount[28+1];
	char CardHolderName[100+1];
	char InfData[100+1];
}PuchaseInfo;

typedef struct {
unsigned char flagmenuTenor; //for flag showing menu
char Tenor[1+1];
char InterestCode[4+1]; //for panin
char InstallmentCode[2+1]; //for bukopin
}CicilanInfo;

#define CB glTlmSysParam.stEdcInfo.CicilanInfoBukopin
#define MnuTenor1 	0
#define MnuTenor3 	1
#define MnuTenor6 	2
#define MnuTenor9 	3
#define MnuTenor12 	4
#define MnuTenor18 	5
#define MnuTenor24 	6

PuchaseInfo 	AJ[4];//untuk Artha Jasa
char 			bit63req[lenBit63+2];
int ziTimeIsengA;
int ziTimeIsengB;

int 	TransSubSaleNew(void);
void 	GetBit63_AJ(char * bit63rsp);
int 	SetBit63_AJ(void);
int 	SetOnBukTenor(int tenor);
int 	SetOffBukTenor(int tenor);
int		AllSetMenuCicilanBukopin(void);
void 	RollOut(void);
void	INPUT_MK(void);
void 	INPUT_WK(void);
void 	SwitchMenuCicilan(void);
void 	emv_test(void);
void	JenisRekening(void);
void	ModifProCode(void);
int 	TransLogonAJ(void);
int 	TransLogonBukopin(void);
int 	CheckExpBypassPin(void);
int 	TransSubSaleNewPrvtLbl(void);
int 	StartTapPrvtLbl(void);

int 	SelectAIDPrvtLblICC(byte* DataIn, byte p2, byte* rsp);
int 	SelectRec1PrvtLbl(byte* DataIn, byte p2, byte* rsp);





#endif

