
#include "global.h"

/********************** Internal macros declaration ************************/

// PED function for P8010 not implement [11/20/2006 Tommy]

/********************** Internal structure declaration *********************/
typedef struct _tagPASSWD_INFO
{
	uchar	szMsg[16+1];
	uchar	ucLen;
}PASSWD_INFO;

/********************** Internal functions declaration *********************/
static uchar PasswordSub(uchar ucPwdID);
static uchar PasswordNew(uchar *psOutPwd, uchar ucLen);
static void  ModifyPasswordSub(uchar ucPwdID);

/********************** Internal variables declaration *********************/
static PASSWD_INFO sgPwdInfo[] =
{
	// 顺序必须与PWD_xxx的定义顺序一致
	{_T_NOOP("BANK PASSWORD   "), 6},
	{_T_NOOP("ENT TERMINAL PWD"), 4},
	{_T_NOOP("ENT MERCHANT PWD"), 4},
	{_T_NOOP("VOID PASSWORD   "), 4},
	{_T_NOOP("REFUND PASSWORD "), 4},
	{_T_NOOP("ADJUST PASSWORD "), 4},
	//{_T_NOOP("SETTLEMENT PWD  "), 4},
	{_T_NOOP("PASSWORD"), 4},
};

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

void ResetPwdAll(void)
{
	int	iCnt;
	
	for (iCnt=0; iCnt<PWD_MAX; iCnt++)
	{
#ifdef	__TELIUM_PARAM__
		//sprintf((char *)&glTlmSysParam.sPassword[iCnt][0], "%0*d", sgPwdInfo[iCnt].ucLen, 0);
		sprintf((char *)&glTlmSysParam.sPassword[iCnt][0], "%0*d", sgPwdInfo[iCnt].ucLen, 135246);
#else
		sprintf((char *)&glSysParam.sPassword[iCnt][0], "%0*d", sgPwdInfo[iCnt].ucLen, 0);
#endif
	}
}

uchar PasswordBank(void)
{
	return PasswordSub(PWD_BANK);
}

uchar PasswordTerm(void)
{
	return PasswordSub(PWD_TERM);
}

uchar PasswordMerchant(void)
{
	return PasswordSub(PWD_MERCHANT);
}

uchar PasswordVoid(void)
{
	return PasswordSub(PWD_VOID);
}

uchar PasswordRefund(void)
{
	return PasswordSub(PWD_REFUND);
}

uchar PasswordAdjust(void)
{
	return PasswordSub(PWD_ADJUST);
}

uchar PasswordSettle(void)
{
	return PasswordSub(PWD_SETTLE);
}

void ModifyPasswordBank(void)
{
	ModifyPasswordSub(PWD_BANK);
}

void ModifyPasswordTerm(void)
{
	ModifyPasswordSub(PWD_TERM);
}

void ModifyPasswordMerchant(void)
{
	ModifyPasswordSub(PWD_MERCHANT);
}

void ModifyPasswordVoid(void)
{
	ModifyPasswordSub(PWD_VOID);
}

void ModifyPasswordRefund(void)
{
	ModifyPasswordSub(PWD_REFUND);
}

void ModifyPasswordAdjust(void)
{
	ModifyPasswordSub(PWD_ADJUST);
}

void ModifyPasswordSettle(void)
{
	ModifyPasswordSub(PWD_SETTLE);
}

uchar PasswordSub(uchar ucPwdID)
{
	uchar	ucRet, szBuff[20];

	if( ucPwdID>sizeof(sgPwdInfo)/sizeof(sgPwdInfo[0]) )
	{
		return 1;
	}

	while( 1 )
	{
		ScrClrLine(2, 7);
		PubDispString(_T(sgPwdInfo[ucPwdID].szMsg), 2|DISP_LINE_LEFT);
		memset(szBuff, 0, sizeof(szBuff));
		ucRet = PubGetString(PASS_IN, sgPwdInfo[ucPwdID].ucLen, sgPwdInfo[ucPwdID].ucLen, szBuff, USER_OPER_TIMEOUT);	// 取消了用PED输入的做法
//		ucRet = PubGetString(PASS_IN, sgPwdInfo[ucPwdID].ucLen, 6, szBuff, USER_OPER_TIMEOUT);	// 取消了用PED输入的做法
		if( ucRet!=0 )
		{
			return 1;
		}

		// 取消了用PED输入的做法
#ifdef	__TELIUM_PARAM__
		if( memcmp(szBuff, glTlmSysParam.sPassword[ucPwdID], sgPwdInfo[ucPwdID].ucLen)==0 &&
			strlen((char *)szBuff)==(int)sgPwdInfo[ucPwdID].ucLen )
		{
			return 0;
		}
/*		not use for telium
		if( memcmp(szBuff, glTlmSysParam.sPassword[PWD_BANK], sgPwdInfo[PWD_BANK].ucLen)==0 &&
			strlen((char *)szBuff)==(int)sgPwdInfo[PWD_BANK].ucLen )
		{
			return 0;
		}
*/		
#else
		if( memcmp(szBuff, glSysParam.sPassword[ucPwdID], sgPwdInfo[ucPwdID].ucLen)==0 &&
			strlen((char *)szBuff)==(int)sgPwdInfo[ucPwdID].ucLen )
		{
			return 0;
		}

		if( memcmp(szBuff, glSysParam.sPassword[PWD_BANK], sgPwdInfo[PWD_BANK].ucLen)==0 &&
			strlen((char *)szBuff)==(int)sgPwdInfo[PWD_BANK].ucLen )
		{
			return 0;
		}
#endif

		ScrClrLine(4, 7);
		PubDispString(_T("PWD ERROR!"), 4|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}
}

void ModifyPasswordSub(uchar ucPwdID)
{
	if( PasswordSub(ucPwdID)!=0 )
	{
		return;
	}

#ifdef	__TELIUM_PARAM__
	if( PasswordNew(glTlmSysParam.sPassword[ucPwdID], sgPwdInfo[ucPwdID].ucLen)!=0 )
	{
		return;
	}

	SaveTlmPassword();

#else
	if( PasswordNew(glSysParam.sPassword[ucPwdID], sgPwdInfo[ucPwdID].ucLen)!=0 )
	{
		return;
	}

	SavePassword();

#endif

	PubShowTitle(TRUE, (uchar *)"CHANGE PWD");
	PubDispString(_T("PWD CHANGED!"), 4|DISP_LINE_LEFT);
	PubBeepOk();
	PubWaitKey(3);
}

uchar PasswordNew(uchar *psOutPwd, uchar ucLen)
{
	uchar	ucRet, sBuff1[20], sBuff2[20];

	while( 1 )
	{
		ScrClrLine(4, 7);
		PubDispString(_T("ENTER NEW PWD"), 4|DISP_LINE_LEFT);
		ucRet = PubGetString(PASS_IN, ucLen, ucLen, sBuff1, USER_OPER_TIMEOUT);		// 取消了用PED输入的做法
		if( ucRet!=0 )
		{
			return 1;
		}

		ScrClrLine(4, 7);
		PubDispString(_T("RE-ENTER NEW PWD"), 4|DISP_LINE_LEFT);
		ucRet = PubGetString(PASS_IN, ucLen, ucLen, sBuff2, USER_OPER_TIMEOUT);		// 取消了用PED输入的做法
		if( ucRet!=0 )
		{
			return 1;
		}

		// 取消了用PED输入的做法
		if( memcmp(sBuff1, sBuff2, ucLen)==0 )
		{
			memcpy(psOutPwd, sBuff1, ucLen);
			return 0;
		}

		ScrClrLine(4, 7);
		PubDispString(_T("NOT CONSISTENT"), 6|DISP_LINE_LEFT);
		PubBeepErr();
		PubWaitKey(3);
	}
}

// 取消了用PED输入的做法
// uchar PasswordCheck(uchar *password6, uchar *password4, uchar *password_chk)

// end of file
