
/****************************************************************************
NAME
    fileoper.h - 定义所有文件操作的函数

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
****************************************************************************/

#ifndef _FILEOPER_H
#define _FILEOPER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int InitTranLogFile(void);
int	InitBrizziTranLogFile(void);
int	InitMiniAtmCSTranLogFile(void);
int	InitTunaiTranLogFile(void);

int LoadSysParam(void);
int SaveSysParam(void);
int SaveEdcParam(void);
int SavePassword(void);
int ExistSysFiles(void);
int ValidSysFiles(void);
void RemoveSysFiles(void);
int LoadSysCtrlAll(void);
int SaveSysCtrlAll(void);
int SaveSysCtrlBase(void);
int SaveSysCtrlNormal(void);
int SaveRevInfo(uchar bSaveRevInfo);
int SaveRePrnStlInfo(void);
int LoadTranLog(void *pstLog, ushort uiIndex);
int LoadBrizziTranLog(void *pstLog, ushort uiIndex);
int LoadMiniAtmCSTranLog(void *pstLog, ushort uiIndex);
int LoadTunaiTBankTranLog(void *pstLog, ushort uiIndex);
int UpdateTranLog(void *pstLog, ushort uiIndex);
int UpdateBrizziTranLog(void *pstLog, ushort uiIndex);
int UpdateMiniAtmCSTranLog(void *pstLog, ushort uiIndex);
int UpdateTunaiTranLog(void *pstLog, ushort uiIndex);
int SaveTranLog(void *pstLog);

int SaveBrizziTranLog(void *pstLog);
int SaveMiniAtmTranLog(void *pstLog);
int SaveCardServicesTranLog(void *pstLog);
int SaveTunaiTranLog(void *pstLog);
int SaveTBankTranLog(void *pstLog);

int RecoverTranLog(void);
int RecoverBrizziTranLog(void);
int RecoverMiniAtmTranLog(void);
int RecoverCardServicesTranLog(void);
int RecoverTunaiTranLog(void);
int RecoverTBankTransLog(void);
ushort GetTranLogNum(uchar ucAcqKey);
ushort GetBrizziTranLogNum(uchar ucAcqKey);
ushort GetMiniAtmTranLogNum(uchar ucAcqKey);
ushort GetCardServicesTranLogNum(uchar ucAcqKey);
ushort GetTunaiTranLogNum(uchar ucAcqKey);
ushort GetTBankTranLogNum(uchar ucAcqKey);
uchar AllowDuplicateTran(void);
int GetRecord(ushort uiStatus, void *pstOutTranLog);
int GetBrizziRecord(ushort uiStatus, void *pstOutTranLog);
int GetMiniAtmRecord(ushort uiStatus, void *pstOutTranLog);
int GetCardServicesRecord(ushort uiStatus, void *pstOutTranLog);
int GetTunaiRecord(ushort uiStatus, void *pstOutTranLog);
int GetTBankRecord(ushort uiStatus, void *pstOutTranLog);
void CalcTotal(uchar ucAcqKey);
void CalcBrizziTotal(uchar ucAcqKey, uchar * date, uchar flagDate);
void CalcMiniAtmTotal(void *TotalInfo, ushort *TotalCnt, uchar *TotalAmt, uchar * date, uchar flagDate);
void CalcCardServicesTotal(void *TotalInfo, ushort *TotalCnt,  uchar * date, uchar flagDate);
void CalcTunaiTotal(void *TotalInfo, ushort *TotalCnt, uchar *TotalAmt, uchar * date, uchar flagDate);
void CalcTBankTotal(void *TotalInfo, ushort *TotalCnt,  uchar * date, uchar flagDate);



int SaveEmvStatus(void);
int LoadEmvStatus(void);
int SaveEmvErrLog(void);
int LoadErrLog(ushort uiRecNo, void *pOutErrLog);
int SyncPassword(void);
int LastRecordIsFallback(void);

//irf, 20121128
int SaveTMK(uchar * TMK);
int SaveTWK(uchar * TWK);
int LoadTMK(uchar * TMK);
int LoadTWK(uchar * TWK);

int CheckFileTMK();
int CheckFileTWK();
//end, irf, 20121128

int SaveTlmTmsParam(void);
int LoadTlmTmsParam(void);
int SaveTLMTMScon(void);
int LoadTLMTMScon(void);
int SaveTLMbin(void);
int LoadTLMbin(void);
int SaveTLMiss(void);
int LoadTLMiss(void);
int SaveTLMacq(void);
int LoadTLMacq(void);
int SaveTLMaid(void);
int LoadTLMaid(void);
int SaveTLMpub(void);
int LoadTLMpub(void);
int SaveTLMreg(void);
int LoadTLMreg(void);

int LoadTLMTMS(void);
int SaveTLMTMS(void);
int SaveTlmEdcParam(void);

int LoadTlmSysParam(void);
int SaveTlmSysParam(void);
int SaveTlmPassword(void);
int SyncTlmPassword(void);

int LoadAcqUploadStatus(uchar *acqUploadStatus, uchar ucIndex);
int SaveAcqUploadStatus(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _FILEOPER_H

// end of file
