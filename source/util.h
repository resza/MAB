
/****************************************************************************
NAME
    util.h - 定义所有实用函数(针对应用封装基本库)

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.14      - created
****************************************************************************/

#ifndef _UITL_H
#define _UITL_H

#define TIME_OUT	30
#define TIME_OUT_INPUT	60

// amount type 
#define AMOUNT		0
#define RFDAMOUNT	1
#define ORGAMOUNT	2
#define TIPAMOUNT	3

#define AMTBRIZZI		4

// options for DispErrMsg()
#define DERR_BEEP		0x0002

typedef struct _tagLangConfig{
	char	szDispName[16+1];	// 显示的语言名称
	char	szFileName[16+1];	// 语言文件名
	uchar	ucCharSet;			// 字符集
}LANG_CONFIG;

typedef struct _tagCURRENCY_CONFIG 
{
	uchar	szName[3+1];	        // 货币简称，如"HKD","USD"
	uchar	sCurrencyCode[2];		// ISO-4217   货币代码，如新台币"\x09\x01"
    uchar   sCountryCode[2];        // ISO-3166-1 国家或地区代码，如台湾"\x01\x58"
	uchar	ucDecimal;		        // 货币小数点位置。JPY为0，USD，HKD等为2，极少数货币为3
	uchar	ucIgnoreDigit;	        // 货币用ISO8583 bit4表示时，转换成普通金额数字串之前先忽略掉的尾数个数
}CURRENCY_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int CustomizeAppLibForArabiaLang(uchar bSetToArabia);

void InitTransInfo(void);
ulong GetNewTraceNo(void);
ulong GetNewBatchNo(ulong ulCurBatchNo);
ulong GetNewInvoiceNo(void);
ulong GetNewTleCountNo(void);
void PromptRemoveICC(void);
void PromptRemoveICCNew(void);
void EraseExpireCAPK(void);
void DispSwipeCard(uchar bShowLogo);
void DispSwipeOnly(void);
void DispSwipeCardOnly(void);
void DispSwipeAgenCard(void); // Add Zaki
void DispSwipeInsertCardOnly(void);
void DispInsertOnly(void);
void DispFallBackSwipeCard(void);
void DispBlockFunc(void);
void DispProcess(void);
void DispWait(void);
void DispDial(void);
void DispConnecting(void);
void DispSend(void);
void DispReceive(void);
void DispPrinting(void);
void DispClearOk(void);
void DispAdjustTitle(void);
void DispVersion(void);
void DispWaitRspStatus(ushort uiLeftTime);
void ShowBoxNoShadow(uchar y1, uchar x1, uchar y2, uchar x2);
int  FunctionInput(void);
void SysHalt(void);
void SysHaltInfo(void *pszDispInfo, ...);
void DispMagReadErr(void);
void apiBeef(uchar mode,ushort DlyTime);
int  SelectTransCurrency(void);
void FindIssuer(uchar ucIssuerKey);
void FindAcq(uchar ucAcqKey);
uchar FindAcqIdxByName(char *pszAcqName, uchar bFullMatch);
void SetCurAcq(uchar ucAcqIndex);
void SetCurIssuer(uchar ucIssuerIndex);
int  ReadMagCardInfo(void);
void DispTransName(void);
void DispAccepted(void);
void DispErrMsg(char *pFirstMsg, char *pSecondMsg, short sTimeOutSec, ushort usOption);
void PrintOne(void);
uchar IsChipCardSvcCode(uchar *pszTrack2);
int  ValidPanNo(uchar *pszPanNo);
int  ValidCardExpiry(void);
void GetDateTime(uchar *pszDateTime);
int UpdateLocalTime(uchar *pszNewYear, uchar *pszNewDate, uchar *pszNewTime);
int  ValidCard(void);
int ValidCardCashOut(void);
void GetCardHolderName(uchar *pszHolderName);
void GetEngTime(uchar *pszCurTime);
void Conv2EngTime(uchar *pszDateTime, uchar *pszEngTime);
int  SwipeCardProc(uchar bCheckICC);
int  InsertCardProc(void);
void ConvIssuerName(uchar *pszOrgName, uchar *pszOutName);
int  ConfirmPanInfo(void);
int  GetCard(uchar ucMode);
int  GetAmount(void);
int  GetInstalPlan(void);
void AmtConvToBit4Format(uchar *pszString, uchar ucIgnoreDigit);
void App_ConvAmountLocal(uchar *pszIn, uchar *pszOut, uchar ucMisc);
void App_ConvAmountTran(uchar *pszIn, uchar *pszOut, uchar ucMisc);
uchar ValidAdjustAmount(uchar *pszBaseAmt, uchar *pszTotalAmt);
uchar ValidBigAmount(uchar *pszAmount);
uchar ConfirmAmount(char *pszDesignation, uchar *pszAmount);
void DispAmount(uchar ucLine, uchar *pszAmount);
void DispAmountASCII(uchar ucLine, uchar *pszAmount);
int  GetManualPanFromMsg(void *pszUsrMsg);
int  ManualInputPan(uchar ucInitChar);
int  GetDescriptor(void);
int  GetAddlPrompt(void);
#define GETPIN_EMV		0x80
#define GETPIN_RETRY	0x01
int  GetPIN(uchar ucFlag);
int  GetMAC(uchar ucMode, uchar *psData, ushort uiDataLen, uchar ucMacKeyID, uchar *psOutMAC);
void DispPPPedErrMsg(uchar ucErrCode);
void DispPciErrMsg(int iErrCode);
void MaskPan(uchar *pszInPan, uchar *pszOutPan);
void Get8583ErrMsg(uchar bPackErr, int iErrCode, uchar *pszErrMsg);
int  GetPreAuthCode(void);
int  InputInvoiceNo(ulong *pulInvoiceNo);
void GetStateText(ushort ucStatus, uchar *pszStaText);
void DispStateText(ushort uiStatus);
void DispOperOk(void *pszMsg);
uchar AskYesNo(void);
int SelectAcqCreditDebitOnly(uchar bAllowSelAll, uchar *pszTitle, uchar *pucAcqIndex);
int  SelectAcq(uchar bAllowSelAll, uchar *pszTitle, uchar *pucAcqIndex);
int  SelectIssuer(uchar *pucIssuerIndex);
void ClearTotalInfo(void *pstTotalInfo);
void ClearBrizziTotalInfo(void *pstTotalInfo);
int  DispTransTotal(uchar bShowVoidTrans);
int DispPurchaseTotal(uchar bShowVoidTrans);
int  DispTransBrizziTotal(uchar bShowVoidTrans);
void DispTranLog(ushort uiActRecNo, void *pTranLog);
//void AdjustLcd(void);
uchar IsNumStr(char *pszStr);
//void GetEngTranLabel(uchar *pszTranTitle, uchar *pszEngLabel);
uchar GetTranAmountInfo(void *pTranLog);
void DispResult(int iErrCode);
int  NumOfReceipt(void);
//void ModifyTermCapForPIN(void);
void UpdateEMVTranType(void);
int FindCurrency(uchar *pszCurrencyNameCode, CURRENCY_CONFIG *pstCurrency);
void SyncEmvCurrency(uchar *psCountryCode, uchar *psCurrencyCode, uchar ucDecimal);

int  UpdateTermInfo(void);
int  CheckSysFont(void);
void EnumSysFonts(void);
int  SxxWriteKey(uchar ucSrcKeyIdx, uchar *psKeyBCD, uchar ucKeyLen, uchar ucDstKeyId, uchar ucDstKeyType, uchar *psKCV);

void SetOffBase(unsigned char (*Handle)());
uchar ChkOnBase(void);
uchar OffBaseDisplay(void);
uchar OffBaseCheckPrint(void);
uchar OffBaseCheckEcr(void);

int GetPIN_Manual(uchar ucFlag);	//irf, 20121128
int GetPIN_Manual2(uchar ucFlag);	//irf, 20121128
int GetPIN_ForcePIN(uchar ucFlag);  //4212 20142309
void	SetPosEntryMode(void);

//int fmtPad(char * str, int len, char chr);	//irf, 20121128

int decTDesCBC(uchar *dataIn, int lenInput, uchar *dataOut, uchar *key);
int TDesCBC(uchar *dataIn, int lenInput, uchar *dataOut, uchar *key);

void hex_dump(char * txt, void * buffer, uint len);
void hex_dump_char(const char * pzTitle, const uchar * buff, long len);
void hex_dump_char_New(const char * pzTitle, const uchar * buff, long len); // Add Zaki 12/4/2013

void TestPrint(void);
void test_picc(void);

int  MatchCardTable(uchar *pszPAN);
int MatchBRIDebitCardTable(uchar *pszPAN);
int MatchBRICreditCardTable(uchar *pszPAN);
void PrintMenu(void);
void PrintMenuNew(void); // Add Zaki 12/4/13

void  _trcS(uchar *Text);
void _trcFN(uchar *Text, int key);
void  _trcFS(uchar *Text, uchar *data);

byte valGetByte(word key);
word valGetWord(word key);

int InputKodeArea(void);
int InputNomorTelepon(void);
int parseTMWK(uchar *buff);
int GetPinKeyPerAcq();
int fmtAppNameAndVersion();
uchar CalCAPKCheckSum(EMV_CAPK* capk, uchar *pszCheckSum);
int CheckResponseCode(uchar *ResponseCode);


int GetAmountBrizzi(void);
int ValidCardBRIZZI(void);
void App_ConvAmount(uchar *pszIn, uchar *pszOut, uchar ucDecimal, uchar ucIgnoreDigit, uchar ucMisc);



// DIKI ADD FOR MINI  ATM BUKOPIN
int GetPIN_Manual_MiniAtmBkpn(uchar ucFlag);	



// DIKI ADD FOR PRIVATE LABEL BUKOPIN
int  GetCardPrvtLbl(uchar ucMode);
int  InsertCardProcPrvtLbl(void);
int  ValidCardPrvtLbl(void);
int GetPIN_ManualPrvtLbl(uchar ucFlag);	


int  ValidTransSettleBkpn(void);


// diki add ATMB
int  SwipeCardProcBkpn(uchar bCheckICC);
int  ReadMagCardInfoBkpn(void);


int  GetCardInitKartuBkpn(uchar ucMode);

int ValidCardInitBkpn(void);













#define MEM_ZERO(VAR) memset(VAR, 0, sizeof(VAR));
#define MEM_0x20(VAR) memset(VAR, 0x20, sizeof(VAR));
#define MEM_0x30(VAR) memset(VAR, 0x30, sizeof(VAR));
//#define RUN_FUNCTION(F, LBL) {ret = F(); CHECK(ret > 0, LBL);} 

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _UITL_H

// end of file
