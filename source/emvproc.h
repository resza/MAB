
/****************************************************************************
NAME
	emvproc.h - 实现EMV交易处理

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
	MODIFIED   (YYYY.MM.DD)
	shengjx     2006.09.05      - created
****************************************************************************/

#ifndef _EMVPROC_H
#define _EMVPROC_H

// EMV最后一次交易的状态
typedef struct _tagEMV_STATUS
{
	uchar		sgTSIOld[2];
	uchar		sgTVROld[5];		// TVR/TSI before send tho host
	uchar		sgARQCOld[32];
	int			sgARQCLenOld;

	uchar		sgTSINew[2];
	uchar		sgTVRNew[5];

	uchar		sgIACDeinal[5];
	uchar		sgIACOnline[5];
	uchar		sgIACDefault[5];

	uchar		sgTACDeinal[5];
	uchar		sgTACOnline[5];
	uchar		sgTACDefault[5];

	//-------------------------------
	uchar		sAppCryptoFirst[32];
	uchar		sAppCryptoSecond[32];
	uchar		sTLV[512+2];

	uchar		GenACCID1st;
	uchar		GenACCID2nd;

	uchar		ucSW1;
	uchar		ucSW2;
	
}EMV_STATUS;

// EMV错误日志文件记录
typedef struct _tagEMV_ERR_LOG
{
	uchar	bValid;
	uchar	ucAidLen;
	uchar	sAID[16];
	uchar	szPAN[19+1];
	uchar	ucPANSeqNo;
	uchar	szAmount[12+1];
	uchar	szTipAmt[12+1];
	uchar	szRspCode[2+1];
	uchar	szDateTime[14+1];
	uchar	szRRN[12+1];
	uchar	szAuthCode[6+1];
	uchar	sTVR[5];
	uchar	sTSI[2];
	ushort	uiReqICCDataLen;
	uchar	sReqICCData[260+2];
	ushort	uiRspICCDataLen;
	uchar	sRspICCData[260+2];
	ulong	ulSTAN;
}EMV_ERR_LOG;

typedef enum{
	DE55_SALE,
	DE55_UPLOAD,
} DE55_TYPE;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void AppSetMckParam(uchar bEnablePinBypass);
void InitTransEMVCfg(void);
int  FinishEmvTran(void);
void ViewTVR_TSI(void);
int  SetDE55(DE55_TYPE ucType, uchar *psOutData, int *piOutLen);
int  SetTCDE55(void *pstTranLog, uchar *psOutData, int *piOutLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
