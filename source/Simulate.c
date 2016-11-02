#include "global.h"

/* ********************* Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
/********************** Internal variables declaration *********************/
/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// 检查是否处于练习/演示模式
uchar ChkIfTrainMode(void)
{
#ifdef	__TELIUM_PARAM__
	return (glTlmSysParam.stTxnCommCfg.ucCommType==CT_DEMO);
#else
	return (glSysParam.stTxnCommCfg.ucCommType==CT_DEMO);
#endif
}

// 练习/演示模式下用于模拟后台数据
int CreatDummyRecvData(void *pstProcInfo, void *pstRecvPacket)
{
	uchar	szBuff[32];
	STISO8583		*pstRecv;
	SYS_PROC_INFO	*pstProc;

	pstRecv = (STISO8583 *)pstRecvPacket;
	pstProc = (SYS_PROC_INFO *)pstProcInfo;

	GetDateTime(szBuff);
	if ( (strlen(pstProc->stTranLog.szDateTime)==0) || (pstProc->stTranLog.ucTranType==TRANS_TYPE_VOID) )
	{
		sprintf((char *)(pstProc->stTranLog.szDateTime), "%.14s", (char *)szBuff);
	}

	// Authorization code
	sprintf(pstProc->stTranLog.szAuthCode, "      ");
	if ((pstProc->stTranLog.ucTranType!=TRANS_TYPE_VOID) && (strlen(pstProc->stTranLog.szAuthCode)==0))
	{
		sprintf(pstProc->stTranLog.szAuthCode, "*DEMO*");
	}

	// RRN
	sprintf((char *)(pstProc->stTranLog.szRRN), "*DEMO*%6.6s", (char *)szBuff+8);

	// Response code
	sprintf((char *)(pstRecv->szBit39), "00");
	if ((pstProc->stTranLog.ucTranType==TRANS_TYPE_SETTLEMENT) && (glSendPack.szBit3[1]=='2'))
	{
		sprintf((char *)(pstRecv->szBit39), "95");
	}

	sprintf((char *)(pstProc->stTranLog.szRspCode), (char *)(pstRecv->szBit39));
	return 0;
}

// 在POS运行时简单显示debug信息
int PosDebugShow(char *pszStr, ...)
{
#ifdef APP_DEBUG
	uchar		szBuff[1024+1];
	va_list		pVaList;

	if( pszStr==NULL || *(uchar *)pszStr==0 )
	{
		return 0;
	}

	va_start(pVaList, pszStr);
	vsprintf((char*)szBuff, (char*)pszStr, pVaList);
	va_end(pVaList);

	ScrClrLine(0,1);
	ScrPrint(0,0,0,szBuff);
	kbflush();getkey();
#endif
	return 0;
}

// end of file

