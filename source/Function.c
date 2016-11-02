#include "global.h"

int Echo_Test(void)
{
	int		iRet;
	uchar	szTitle[16+1], ucIndex;

	iRet = TransInit(TRANS_TYPE_ECHO_TEST);
	if( iRet!=0 )
	{
		return iRet;
	}
	
	PubShowTitle(TRUE, (uchar *)_T("ECHO TEST     "));
	sprintf((char *)szTitle, _T("ECHO TEST     "));
	iRet = SelectAcq(FALSE, szTitle, &ucIndex);
	if( iRet!=0 )
	{
		return iRet;
	}

//INIT ISO8583
	//==================================================================================
	memset(&glSendPack, 0, sizeof(STISO8583));
	//==================================================================================

//MTI
	//==================================================================================
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);
	//==================================================================================

//BIT 3
	//==================================================================================
	sprintf((char *)glSendPack.szBit3,   "%.*s", LEN_PROC_CODE,
			glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	//==================================================================================

//BIT 11
	//==================================================================================
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	glProcInfo.stTranLog.ulSTAN = glSysCtrl.ulSTAN;
	//==================================================================================
	
//BIT 24
	//==================================================================================
	sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurTlmTmsAcq.acqNII);
	//sprintf((char *)glSendPack.szBit24,        "%.*s", LEN_NII,         glCurAcq.szNii);
	//==================================================================================

//BIT 41
	//==================================================================================
	sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurTlmTmsAcq.acqTID);
	//sprintf((char *)glSendPack.szBit41,     "%.*s", LEN_TERM_ID,     glCurAcq.szTermID);
	//==================================================================================

	//SEND MESSAGE TO HOST
	iRet = SendPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	//RECV MESSAGE FROM HOST
	iRet = RecvPacket();
	if( iRet!=0 )
	{
		return iRet;
	}

	ScrPrint(0, 3, ASCII, "%s", "       COMPLETE    ");
	PubWaitKey(2);

	return iRet;
}



