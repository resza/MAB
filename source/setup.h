
/****************************************************************************
NAME
    setup.h - 定义终端参数设置、查询模块

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
****************************************************************************/

#ifndef _SETUP_H
#define _SETUP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int GetIpLocalSettings(void *pstParam);
int GetRemoteIp(uchar *pszHalfText, uchar bAllowNull, void *pstIPAddr);
int ChkIfValidIp(uchar *pszIP);
int ChkIfValidPort(uchar *pszPort);

int  SetTcpIpParam(void *pstParam);
void SyncTcpIpParam(void *pstDst, void *pstSrc);
int  SetWirelessParamNew(WIRELESS_PARAM *pstParam);
int SetAPNTelkomsel(WIRELESS_PARAM *pstParam);
int SetAPNIndosat(WIRELESS_PARAM *pstParam);
int SetAPNXL(WIRELESS_PARAM *pstParam);
int  SetWirelessParam(WIRELESS_PARAM *pstParam);
int PrnSetAPN(WIRELESS_PARAM *pstParam);

void SyncWirelessParam(WIRELESS_PARAM *pstDst, WIRELESS_PARAM *pstSrc);

void GetAllSupportFunc(char *pszBuff);
void FunctionExe(uchar bUseInitMenu, int iFuncNo);
void FunctionMenu(void);
void FunctionInit(void);
void SetSystemParamAll(void);
void SetSysLang(uchar ucSelectMode);
void SetEdcLangExt(char *pszDispName);
void DebugSet(void);
void DebugSetNew(void); // Add By Zaki
void TLEDebugSet(void);
void TLESet(void);
void DebugIsoReqSet(void);
void SetKeyIn(void);
void DebugIsoResSet(void);
void DebugEMVICCSet(void);
void DebugTagEmvSet(void);
void BlowFishSet(void);

void SetBIT48(void);


int  SetCommType(uchar ucMode); // Edit By Zaki
void SetSysCommParam(uchar ucPermission);  // Add By Zaki
int SetSysCommLoadParam(uchar ucPermission); // Add By Zaki
int  SetCommTypeTMS(uchar ucMode); // Add By Zaki
int  ConfirmLoadEDC(void); // Add By Zaki
void SetSysTime(void);  // Add By Zaki
void SetSysDate(void);  // Add By Zaki
void SetSysTimeNew(void);  // Add By Zaki
void SetAPN (void);  // Add By Zaki
//int  SetGetSysTraceNo(uchar ucPermission);
void SetTraceNo(void);
void ViewTraceNo(void);
void SetStanNo(void);
void ViewStanNo(void);
//int  SetGetSysInvoiceNo(uchar ucPermission);
void SetTIDMID(uchar ucPermission);
void InfoAcquirers(uchar ucPermission);
//static int  SetTcpIpSharedPara(COMM_CONFIG *pstCommCfg);
int  SetTcpIpSharedPara(COMM_CONFIG *pstCommCfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _SETUP_H

// end of file
