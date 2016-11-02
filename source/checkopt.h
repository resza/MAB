
/****************************************************************************
NAME
    checkopt.h - 定义选项检查函数

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
****************************************************************************/

#ifndef _CHECKOPT_H
#define _CHECKOPT_H

#define HW_NONE			0
// offsets in the string returned by GetTermInfo()
#define HWCFG_MODEL		0
#define HWCFG_PRINTER	1
#define HWCFG_MODEM		2
#define HWCFG_M_SYNC	3
#define HWCFG_M_ASYNC	4
#define HWCFG_PCI		5
#define HWCFG_USBHOST	6
#define HWCFG_USBDEV	7
#define HWCFG_LAN		8
#define HWCFG_GPRS		9
#define HWCFG_CDMA		10
#define HWCFG_WIFI		11
#define HWCFG_CONTACT	12
#define HWCFG_CFONT		13
#define HWCFG_FONTVER	14
#define HWCFG_ICCREAD	15
#define HWCFG_MSR		16
// 17--29 reserved
#define HWCFG_END		30

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uchar ChkTerm(uchar ucTermType);	// 判断当前机型
uchar ChkHardware(uchar ucChkType, uchar ucValue);
uchar ChkIfIrDAPrinter(void);
uchar ChkTermPEDMode(uchar ucMode);
uchar ChkIfEmvEnable(void);// 判断EDC是否支持EMV交易
uchar ChkIssuerOption(ushort uiOption);
uchar ChkAcqOption(ushort uiOption);
uchar ChkEdcOption(ushort uiOption);
uchar ChkOptionExt(uchar *psExtOpt, ushort uiOption);
void  SetOptionExt(uchar *psExtOpt, ushort uiOption);
uchar ChkIfNeedPIN(void);
uchar ChkIfPinReqdAllIssuer(void);
uchar ChkIfBRICredit(void);
uchar ChkIfBRIDebit(void);
uchar ChkIfBelowMagFloor(void);
void  CheckCapture(void);
uchar ChkInstalmentAllAcq(void);
uchar ChkIfDispMaskPan2(void);
uchar ChkIfInstalmentPara(void);
uchar ChkIfTransMaskPan(uchar ucCurPage);
uchar ChkCurAcqName(void *pszKeyword, uchar ucPrefix);
uchar ChkIfTranAllow(uchar ucTranType);
uchar ChkIfZeroAmt(uchar *pszIsoAmountStr);
uchar ChkIfBatchEmpty(void);
uchar ChkIfBatchBrizziEmpty(void);
uchar ChkIfBatchMiniAtmCSEmpty(void);
uchar ChkIfBatchTunaiEmpty(void);
uchar ChkIfZeroTotal(void *pstTotal);
uchar ChkSettle(void);
uchar ChkIfNeedTip(void);
uchar ChkIfDccAcquirer(void);
uchar ChkIfIccTran(ushort uiEntryMode);
uchar ChkIfPrnReceipt(void);
uchar ChkIfNeedReversal(void);
uchar ChkIfSaveLog(void);
uchar ChkIfThermalPrinter(void);
uchar ChkIfNeedSecurityCode(void);
uchar ChkIfNeedMac(void);
uchar ChkIfAcqNeedTC(void);
uchar ChkIfAllowExit(void);
uchar ChkIfBatchFull(void);
uchar ChkIfBatchBrizziFull(void);

uchar ChkIfKeysAlreadyLoaded(uchar ucTranType);	//irf, 20121129
#ifdef	__TELIUM_PARAM__
uchar CheckTransAllowed(void);
void ZeroAcqLogonFlag();
#endif

uchar ChkIfNeedTLE(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _CHECKOPT_H

// end of file
