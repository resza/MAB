
/****************************************************************************
NAME
    print.h - 定义打印模块

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
****************************************************************************/

#ifndef _PRINT_H
#define _PRINT_H

#define PRN_NORMAL		0
#define PRN_REPRINT		1

#ifndef PRN_OK
#define PRN_OK			0
#define PRN_BUSY		1
#endif

#define PRN_NO_PAPER	2
#define PRN_DATA_ERR	3
#define PRN_ERR			4
#define PRN_NO_DOT		0xFC
#define PRN_DATA_BIG	0xFE
#define PRN_CANCEL		0xA1


enum{
	PRN_6x8=0,
	PRN_8x16,
	PRN_16x16,
	PRN_12x24,
	PRN_24x24,
	PRN_6x12,
	PRN_12x12,
	PRN_NULL=0xFF
};

enum{
	NORMAL=0,
	SMALL,
	BIG
};
uchar flagSizeFont;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void MultiLngPrnStr(uchar *str, uchar mode);

int   PrnCurAcqTransList(void);
void  PrnAllList(void);
void  ConvTransName(uchar *pszOrgName, uchar *pszOutName);
void  PrnSetSmall(void);
void  PrnSetNormal(void);
void  PrnSetBig(void);
int   PrintReceipt(uchar ucPrnFlag);
int   PrnTotalIssuer(uchar ucIssuerKey);
int   PrnTotalAcq(void);
int   PrintSettle(uchar ucPrnFlag);
int PrintSettleNew(uchar ucPrnFlag);
void  PrintParam(void);
int   StartPrinter(void);
int StartPrinterNoLogo(void);
void  PrnEngTime(void);

int  PrnFontSetNew(uchar ucEnType, uchar ucSQType);
int PrintReceiptTrx(uchar ucPrnFlag);

//new
int PrnCustomLogo_T(void);
void PrnTidMid(void);
void PrnTid(void);
void PrnMid(void);
void PrnHeader(void);
void PrnCardType(void);
void PrnCardNumber(void);
void PrnCardNumberBig(void);
void PrnCardHolderName(void);
void PrnCardHolderNameBig(void);
void PrnTransactionName(void);
void PrnBatchInvoice(void);
void PrnInvoiceBatch(void);
void PrnDateTime(void);
void PrnRefNumApprCode(void);
void PrnAmount_New(uchar * pszIndent, uchar ucNeedSepLine);
void PrnAppEMV(void);
void PrnAPPLABEL(void);
void PrnAPPPreferName(void);
void PrnAIDEMV(void);
void PrnTCEMV(void);
void PrnTVREMV(void);
void PrnTSI(void);
void PrnPinSignature(void);
void PrnPinNoSignature(void);
void PrnFooterInformation(void);
void PrnAgenCopy(void);
void PrnMerchantCopy(void);
void PrnBankCopy(void);
void PrnCustomerCopy(void);
void PrnDuplicateCopy(void);
void PrnDuplicateHeader(void);

void PrintSummaryReport(void);
void PrintDetailReport(void);

void PrnCardTypeNew(void); // Zaki 12/2/2013
void PrnBulanCicilanBring(void);// Zaki 12/2/2013
void PrnPokokCB(void);// Zaki 12/2/2013
void PrnCicilanBulanan(void);// Zaki 12/2/2013
//int  PrnEmvPara(void);// Zaki 12/2/2013
void PrnInterest(void);
//prn trx
int PrintReceiptPowerBuy(uchar ucPrnFlag);
void PrnTermMonthlyPowerBuy(void);
void  PrintParamNew(void);
void PrnExpDate(void);//4212 14032014
void PrnCardNumberBigPrvtLbl(void); // diki add private label bukopin


#ifdef __cplusplus
}
#endif /* __cplusplus */ 

#endif	// _PRINT_H

// end of file
