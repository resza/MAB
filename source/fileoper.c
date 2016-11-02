
#include "global.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
/********************** Internal variables declaration *********************/
static uchar FILE_SYS_PARAM[]  = "SysParam.dat";		// 系统配置参数文件
static uchar FILE_SYS_CTRL[]   = "SysCtrl.dat";			// 系统运行控制参数文件
static uchar FILE_TRAN_LOG[]   = "TranLog.dat";			// 交易日志文件
static uchar FILE_EMV_STATUS[] = "EmvStatus.dat";		// 最后一次EMV交易状态
static uchar FILE_ERR_LOG[]    = "EmvErrLog.dat";		// 错误日志文件
static uchar FILE_PASSWORD[]   = "PASSWORD";			/* 密码文件 */

static uchar FILE_BRIZZI_TRAN_LOG[]   = "BrizziTranLog.dat";
static uchar FILE_MINIATMCS_TRAN_LOG[]   = "MiniAtmCSTranLog.dat";
static uchar FILE_TUNAI_TRAN_LOG[]   = "TunaiTranLog.dat";


#ifdef __TELIUM_PARAM__
static uchar FILE_TMK[] = "TMK.dat";		// irf, 20121128
static uchar FILE_TWK[] = "TWK.dat";		// irf, 20121128

static uchar FILE_TLM_TMS_PARAM[] = "TlmTmsParam.dat";		// irf, 20121211
static uchar FILE_TLM_APP[] = "TLMapp.dat";		// irf, 20121211
static uchar FILE_TLM_REG[] = "TLMreg.dat";		// irf, 20121211
static uchar FILE_TLM_TMS_CON[] = "TMScon.dat";		// irf, 20121211
static uchar FILE_TLM_TMS_BIN[] = "TLMbin.dat";		// irf, 20121212
static uchar FILE_TLM_TMS_ISS[] = "TLMiss.dat";		// irf, 20121212
static uchar FILE_TLM_TMS_ACQ[] = "TLMacq.dat";		// irf, 20121212
static uchar FILE_TLM_TMS_AID[] = "TLMaid.dat";		// irf, 20121212
static uchar FILE_TLM_TMS_PUB[] = "TLMpub.dat";		// irf, 20121212

static uchar FILE_TLM_SYS_PARAM[] = "TlmSysParam.dat";		// irf, 20121223


#endif

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// 初始化交易日志文件
// Init transaction log file.
// The management of transaction log is quite different with old style.
int InitTranLogFile(void)
{
	int			fd, iWriteBytes, iCnt;
	TRAN_LOG	stTranLog;
#ifdef ENABLE_EMV
	EMV_ERR_LOG	stErrLog;
#endif

	fd = open((char *)FILE_TRAN_LOG, O_CREATE|O_RDWR);
	if( fd<0 )
	{
		PubTRACE0("InitTranLogFile");
		return ERR_FILEOPER;
	}

	memset(&stTranLog, 0, sizeof(stTranLog));
	for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
	{
		iWriteBytes = PubFWriteN(fd, &stTranLog, sizeof(stTranLog));
		if( iWriteBytes!=(int)sizeof(stTranLog) )
		{
			PubTRACE0("InitTranLogFile");
			close(fd);
			return ERR_FILEOPER;
		}
	}
	close(fd);

	// init error log file
	fd = open((char *)FILE_ERR_LOG, O_CREATE|O_RDWR);
	if( fd<0 )
	{
		PubTRACE0("InitTranLogFile");
		return ERR_FILEOPER;
	}

#ifdef ENABLE_EMV
	memset(&stErrLog, 0, sizeof(stErrLog));
	for(iCnt=0; iCnt<MAX_ERR_LOG; iCnt++)
	{
		iWriteBytes = PubFWriteN(fd, &stErrLog, sizeof(stErrLog));
		if( iWriteBytes!=(int)sizeof(stErrLog) )
		{
			PubTRACE0("InitTranLogFile");
			close(fd);
			return ERR_FILEOPER;
		}
	}
	close(fd);
#endif

	return 0;
}


// Init Brizzi transaction log file.
// The management of transaction log is quite different with old style.
int InitBrizziTranLogFile(void)
{
	int			fd, iWriteBytes, iCnt;
	TRAN_LOG	stTranLog;


	fd = open((char *)FILE_BRIZZI_TRAN_LOG, O_CREATE|O_RDWR);
	if( fd<0 )
	{
		PubTRACE0("InitBrizziTranLogFile");
		return ERR_FILEOPER;
	}

	memset(&stTranLog, 0, sizeof(stTranLog));
	for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
	{
		iWriteBytes = PubFWriteN(fd, &stTranLog, sizeof(stTranLog));
		if( iWriteBytes!=(int)sizeof(stTranLog) )
		{
			PubTRACE0("InitBrizziTranLogFile");
			close(fd);
			return ERR_FILEOPER;
		}
	}
	close(fd);

	return 0;
}


// Init MiniAtm CS transaction log file.
// The management of transaction log is quite different with old style.
int InitMiniAtmCSTranLogFile(void)
{
	int			fd, iWriteBytes, iCnt;
	TRAN_LOG	stTranLog;


	fd = open((char *)FILE_MINIATMCS_TRAN_LOG, O_CREATE|O_RDWR);
	if( fd<0 )
	{
		PubTRACE0("InitMiniAtmCSTranLogFile");
		return ERR_FILEOPER;
	}

	memset(&stTranLog, 0, sizeof(stTranLog));
	for(iCnt=0; iCnt<(MAX_MINIATM_TRANLOG*2); iCnt++)
	{
		iWriteBytes = PubFWriteN(fd, &stTranLog, sizeof(stTranLog));
		if( iWriteBytes!=(int)sizeof(stTranLog) )
		{
			PubTRACE0("InitMiniAtmCSTranLogFile");
			close(fd);
			return ERR_FILEOPER;
		}
	}
	close(fd);

	return 0;
}



// Init Tunai transaction log file.
// The management of transaction log is quite different with old style.
int InitTunaiTranLogFile(void)
{
	int			fd, iWriteBytes, iCnt;
	TRAN_LOG	stTranLog;


	fd = open((char *)FILE_TUNAI_TRAN_LOG, O_CREATE|O_RDWR);
	if( fd<0 )
	{
		PubTRACE0("InitTunaiTranLogFile");
		return ERR_FILEOPER;
	}

	memset(&stTranLog, 0, sizeof(stTranLog));
	for(iCnt=0; iCnt<MAX_TUNAI_TRANLOG*2; iCnt++)
	{
		iWriteBytes = PubFWriteN(fd, &stTranLog, sizeof(stTranLog));
		if( iWriteBytes!=(int)sizeof(stTranLog) )
		{
			PubTRACE0("InitTunaiTranLogFile");
			close(fd);
			return ERR_FILEOPER;
		}
	}
	close(fd);

	return 0;
}


// 读取系统参数
//FILE_SYS_PARAM
int LoadSysParam(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_SYS_PARAM, 0L, &glSysParam, sizeof(SYS_PARAM));
	if( iRet!=0 )
	{
		PubTRACE0("LoadSysParam()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// 保存系统参数
//FILE_SYS_PARAM
int SaveSysParam(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_SYS_PARAM, 0L, &glSysParam, sizeof(SYS_PARAM));
	if( iRet!=0 )
	{
		PubTRACE0("SaveSysParam");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// 保持EDC参数
//FILE_SYS_PARAM
int SaveEdcParam(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_SYS_PARAM,
						OFFSET(SYS_PARAM, stEdcInfo),
						&glSysParam.stEdcInfo,
						sizeof(glSysParam.stEdcInfo));
	if( iRet!=0 )
	{
		PubTRACE0("SaveSysParam");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


//FILE_TLM_SYS_PARAM
int SaveTlmEdcParam(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_SYS_PARAM,
						OFFSET(TLM_SYS_PARAM, stEdcInfo),
						&glTlmSysParam.stEdcInfo,
						sizeof(glTlmSysParam.stEdcInfo));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTlmEdcParam");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// 保存系统密码
//FILE_SYS_PARAM
int SavePassword(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_SYS_PARAM,
						OFFSET(SYS_PARAM, sPassword),
						glSysParam.sPassword,
						sizeof(glSysParam.sPassword));
	if( iRet!=0 )
	{
		PubTRACE0("SavePassword");
		SysHalt();
		return ERR_FILEOPER;
	}

	return SyncPassword();
}


//FILE_SYS_PARAM
int SaveTlmPassword(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_SYS_PARAM,
						OFFSET(TLM_SYS_PARAM, sPassword),
						glTlmSysParam.sPassword,
						sizeof(glTlmSysParam.sPassword));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTlmPassword");
		SysHalt();
		return ERR_FILEOPER;
	}

	return SyncTlmPassword();
}

// 判断系统参数文件是否存在
int ExistSysFiles(void)
{
#if 1	
	if (fexist((char *)FILE_TLM_SYS_PARAM)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("FILE_TLM_SYS_PARAM", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}
	else 	if (fexist((char *)TlmFile[0].ucName)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("TlmFile[0]", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}
	else 	if (fexist((char *)TlmFile[1].ucName)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("TlmFile[1]", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}
	else 	if (fexist((char *)TlmFile[2].ucName)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("TlmFile[2]", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}
	else 	if (fexist((char *)FILE_SYS_CTRL)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("FILE_SYS_CTRL", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}
	else 	if (fexist((char *)FILE_TRAN_LOG)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("FILE_TRAN_LOG", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}
	else 	if (fexist((char *)FILE_BRIZZI_TRAN_LOG)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("FILE_BRIZZI_TRAN_LOG", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}
	else 	if (fexist((char *)FILE_MINIATMCS_TRAN_LOG)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("FILE_MINIATMCS_TRAN_LOG", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}

	else 	if (fexist((char *)FILE_TUNAI_TRAN_LOG)<0)
	{		/*
			ScrCls();
			PubDispStringASCII("FILE_TUNAI_TRAN_LOG", DISP_LINE_CENTER|4);
			PubDispStringASCII("NOT EXIST", DISP_LINE_CENTER|6);
			PubWaitKey(30);
			//*/
			return FALSE;
	}


#else
	if (
#ifdef	__TELIUM_PARAM__
		(fexist((char *)FILE_TLM_SYS_PARAM)<0)||	//TMS TELIUM
		(fexist((char *)TlmFile[0].ucName)<0)||	//file mnu, msg, tot TELIUM 	"page0000.nvm"
		(fexist((char *)TlmFile[1].ucName)<0)||	//file bin, iss, acq TELIUM		"page0001.nvm"
		(fexist((char *)TlmFile[2].ucName)<0)||	//file app TELIUM			"page0002.nvm"
#else
		(fexist((char *)FILE_SYS_PARAM)<0) ||	//TMS PAX
#endif
		(fexist((char *)FILE_SYS_CTRL)<0) ||
		(fexist((char *)FILE_TRAN_LOG)<0) ||
		(fexist((char *)FILE_BRIZZI_TRAN_LOG)<0) ||
		(fexist((char *)FILE_MINIATM_TRAN_LOG)<0) ||
		(fexist((char *)FILE_TUNAI_TRAN_LOG)<0) 
		)
	{
		return FALSE;
	}
#endif
	else
	{
		return TRUE;
	}
}

// 判断系统参数文件大小
int ValidSysFiles(void)
{
#ifdef	__TELIUM_PARAM__
	uchar idx=0;
	if ((fexist((char *)FILE_TLM_SYS_PARAM)<0) ||
		(filesize((char *)FILE_TLM_SYS_PARAM)!=sizeof(TLM_SYS_PARAM)))
	{	/* 4212
		ScrCls();
		PubDispStringASCII("FILE_TLM_SYS_PARAM", DISP_LINE_CENTER|4);
		PubDispStringASCII("SIZE FALSE", DISP_LINE_CENTER|6);
		PubWaitKey(30);
		//*/
		return FALSE;
	}
#else
	if ((fexist((char *)FILE_SYS_PARAM)<0) ||
		(filesize((char *)FILE_SYS_PARAM)!=sizeof(SYS_PARAM)) )		//TMS PAX
	{	/* 4212
		ScrCls();
		PubDispStringASCII("FILE_SYS_CTRL", DISP_LINE_CENTER|4);
		PubDispStringASCII("SIZE FALSE", DISP_LINE_CENTER|6);
		PubWaitKey(30);
		//*/
		return FALSE;
	}
#endif
	if ((fexist((char *)FILE_SYS_CTRL)<0) ||
		(filesize((char *)FILE_SYS_CTRL)!=sizeof(SYS_CONTROL)) )
	{	/* 4212
		ScrCls();
		PubDispStringASCII("FILE_SYS_CTRL", DISP_LINE_CENTER|4);
		PubDispStringASCII("SIZE FALSE", DISP_LINE_CENTER|6);
		PubWaitKey(30);
		//*/
		return FALSE;
	}
	if ((fexist((char *)FILE_TRAN_LOG)<0) ||
		(filesize((char *)FILE_TRAN_LOG)!=MAX_TRANLOG*sizeof(TRAN_LOG)) )
	{	/* 4212
		ScrCls();
		PubDispStringASCII("FILE_TRAN_LOG", DISP_LINE_CENTER|4);
		PubDispStringASCII("SIZE FALSE", DISP_LINE_CENTER|6);
		PubWaitKey(30);
		//*/
		return FALSE;
	}
	if ((fexist((char *)FILE_BRIZZI_TRAN_LOG)<0) ||
		(filesize((char *)FILE_BRIZZI_TRAN_LOG)!=MAX_TRANLOG*sizeof(TRAN_LOG)) )
	{	/* 4212
		ScrCls();
		PubDispStringASCII("FILE_BRIZZI_TRAN_LOG", DISP_LINE_CENTER|4);
		PubDispStringASCII("SIZE FALSE", DISP_LINE_CENTER|6);
		PubWaitKey(30);
		//*/
		return FALSE;
	}
	if ((fexist((char *)FILE_MINIATMCS_TRAN_LOG)<0) ||
		(filesize((char *)FILE_MINIATMCS_TRAN_LOG)!=(MAX_MINIATM_TRANLOG*2)*sizeof(TRAN_LOG)) )
	{	/* 4212
		ScrCls();
		PubDispStringASCII("FILE_MINIATMCS_TRAN_LOG", DISP_LINE_CENTER|4);
		PubDispStringASCII("SIZE FALSE", DISP_LINE_CENTER|6);
		PubWaitKey(30);
		//*/
		return FALSE;
	}
	if ((fexist((char *)FILE_TUNAI_TRAN_LOG)<0) ||
		(filesize((char *)FILE_TUNAI_TRAN_LOG)!=(MAX_TUNAI_TRANLOG*2)*sizeof(TRAN_LOG)) )
	{	/* 4212
		ScrCls();
		PubDispStringASCII("FILE_TUNAI_TRAN_LOG", DISP_LINE_CENTER|4);
		PubDispStringASCII("SIZE FALSE", DISP_LINE_CENTER|6);
		PubWaitKey(30);
		//*/
		return FALSE;
	}

#ifdef	__TELIUM_PARAM__

	LoadSysCtrlAll();
	for(idx = 0; idx<5; idx++)
	{
		if(TlmFile[idx].ucName[0] != 0)
		{
			if ((fexist((char *)TlmFile[idx].ucName)<0) ||
				(TlmFile[idx].ulSize!=glSysCtrl.stTlmFile[idx].ulSize))
			{
				/*	
				hex_dump_char(glSysCtrl.stTlmFile[idx].ucName, &glSysCtrl.stTlmFile[idx].ulSize, sizeof(long));
				hex_dump_char(TlmFile[idx].ucName, &TlmFile[idx].ulSize, sizeof(long));
				//*/
				return FALSE;
			}
		}
		else
			break;
	}

#endif

	return TRUE;
}
void RemoveSysFiles(void)
{
	remove((char *)FILE_SYS_PARAM);	//TMS PAX
	remove((char *)FILE_SYS_CTRL);
	remove((char *)FILE_TRAN_LOG);
	remove((char *)FILE_BRIZZI_TRAN_LOG);
	remove((char *)FILE_MINIATMCS_TRAN_LOG);
	remove((char *)FILE_TUNAI_TRAN_LOG);

	remove((char *)FILE_TLM_SYS_PARAM);
	remove((char *)FILE_TLM_TMS_AID);
	remove((char *)FILE_TLM_TMS_PUB);
	
}

// 读取全部运行控制参数
// Load the whole "glSysCtrl"
int LoadSysCtrlAll(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_SYS_CTRL, 0L, &glSysCtrl, sizeof(SYS_CONTROL));
	if( iRet!=0 )
	{
		PubTRACE0("LoadSysCtrlAll");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// 保存全部运行控制参数(谨慎使用,比较耗时)
// Save the whole "glSysCtrl" to file (very time consuming.)
int SaveSysCtrlAll(void)
{
	int		iRet;

	// about 60K bytes
	iRet = PubFileWrite(FILE_SYS_CTRL, 0L, &glSysCtrl, sizeof(SYS_CONTROL));
	if( iRet!=0 )
	{
		PubTRACE0("SaveSysCtrlAll");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// 保存基本运行控制参数(STAN/Invoice No/....)
// Save most basic system-control parameter. part of "glSysCtrl"
int SaveSysCtrlBase(void)
{
	int		iRet;

	// about 1.5K bytes
	iRet = PubFileWrite(FILE_SYS_CTRL, 0L, &glSysCtrl, LEN_SYSCTRL_BASE);
	if( iRet!=0 )
	{
		PubTRACE0("SaveSysCtrlBase");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// save basic information & reversal information
int SaveSysCtrlNormal(void)
{
	int		iRet;

	// about 20K bytes
	iRet = PubFileWrite(FILE_SYS_CTRL, 0L, &glSysCtrl, LEN_SYSCTRL_NORMAL);
	if( iRet!=0 )
	{
		PubTRACE0("SaveSysCtrlNormal");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// 保存冲正信息(只存储当前收单行对应的)
// Save reversal data of current acquirer.
int SaveRevInfo(uchar bSaveRevInfo)
{
	int			iRet;
	STISO8583	*pstRevPack;

	if( bSaveRevInfo )	// 保存冲正信息
	{
		if( !ChkIfNeedReversal() )
		{
			return 0;
		}

#ifdef	__TELIUM_PARAM__
		glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex].bNeedReversal = TRUE;
		pstRevPack = &glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex].stRevPack;
		glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex].uiEntryMode = glProcInfo.stTranLog.uiEntryMode;
#else
		glSysCtrl.stRevInfo[glCurAcq.ucIndex].bNeedReversal = TRUE;
		pstRevPack = &glSysCtrl.stRevInfo[glCurAcq.ucIndex].stRevPack;
		glSysCtrl.stRevInfo[glCurAcq.ucIndex].uiEntryMode = glProcInfo.stTranLog.uiEntryMode;
#endif
		memcpy(pstRevPack, &glSendPack, sizeof(STISO8583));

		// 冲正交易不允许保存磁道信息
		memset(pstRevPack->szBit45, 0, sizeof(pstRevPack->szBit45));
		memset(pstRevPack->szBit35, 0, sizeof(pstRevPack->szBit35));
		memset(pstRevPack->szBit36, 0, sizeof(pstRevPack->szBit36));
		sprintf((char *)pstRevPack->szBit2,     "%.*s", LEN_PAN,      glProcInfo.stTranLog.szPan);
		sprintf((char *)pstRevPack->szBit14, "%.*s", LEN_EXP_DATE, glProcInfo.stTranLog.szExpDate);
#ifdef	__TELIUM_PARAM__
		iRet = PubFileWrite(FILE_SYS_CTRL,
							OFFSET(SYS_CONTROL, stRevInfo[glCurTlmTmsAcq.ucIndex]),
							&glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex],
							sizeof(REVERSAL_INFO));
#else
		iRet = PubFileWrite(FILE_SYS_CTRL,
							OFFSET(SYS_CONTROL, stRevInfo[glCurAcq.ucIndex]),
							&glSysCtrl.stRevInfo[glCurAcq.ucIndex],
							sizeof(REVERSAL_INFO));
#endif
	}
	else	// 清除冲正信息
	{
#ifdef	__TELIUM_PARAM__
		if( !glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex].bNeedReversal )
		{
			return 0;
		}
		glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex].bNeedReversal = FALSE;
		iRet = PubFileWrite(FILE_SYS_CTRL,
							OFFSET(SYS_CONTROL, stRevInfo[glCurTlmTmsAcq.ucIndex].bNeedReversal),
							&glSysCtrl.stRevInfo[glCurTlmTmsAcq.ucIndex].bNeedReversal,
							1);
#else
		if( !glSysCtrl.stRevInfo[glCurAcq.ucIndex].bNeedReversal )
		{
			return 0;
		}
		glSysCtrl.stRevInfo[glCurAcq.ucIndex].bNeedReversal = FALSE;
		iRet = PubFileWrite(FILE_SYS_CTRL,
							OFFSET(SYS_CONTROL, stRevInfo[glCurAcq.ucIndex].bNeedReversal),
							&glSysCtrl.stRevInfo[glCurAcq.ucIndex].bNeedReversal,
							1);

#endif		
	}
	if( iRet!=0 )
	{
		PubTRACE0("SaveRevInfo");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// save reprint settle information
int SaveRePrnStlInfo(void)
{
	int		iRet;

	// about 40K bytes
	iRet = PubFileWrite(FILE_SYS_CTRL,
						OFFSET(SYS_CONTROL, stRePrnStlInfo),
						&glSysCtrl.stRePrnStlInfo,
						sizeof(REPRN_STL_INFO));
	if( iRet!=0 )
	{
		PubTRACE0("SaveStlInfo");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// 读取一个交易日志
// Load one transaction log from file
int LoadTranLog(void *pstLog, ushort uiIndex)
{
	int		iRet;

	PubASSERT( glSysCtrl.sAcqKeyList[uiIndex]!=INV_ACQ_KEY );
	iRet = PubFileRead(FILE_TRAN_LOG, (long)(sizeof(TRAN_LOG)*uiIndex),
						pstLog, sizeof(TRAN_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// Load one Brizzi transaction log from file
int LoadBrizziTranLog(void *pstLog, ushort uiIndex)
{
	int		iRet;

	PubASSERT( glSysCtrl.sBrizziAcqKeyList[uiIndex]!=INV_ACQ_KEY );
	iRet = PubFileRead(FILE_BRIZZI_TRAN_LOG, (long)(sizeof(TRAN_LOG)*uiIndex),
						pstLog, sizeof(TRAN_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("LoadBrizziTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// Load one Mini Atm CS transaction log from file
int LoadMiniAtmCSTranLog(void *pstLog, ushort uiIndex)
{
	int		iRet;

	PubASSERT( glSysCtrl.sMiniAtmCSAcqKeyList[uiIndex]!=INV_ACQ_KEY );
	iRet = PubFileRead(FILE_MINIATMCS_TRAN_LOG, (long)(sizeof(TRAN_LOG)*uiIndex),
						pstLog, sizeof(TRAN_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("LoadMiniAtmCSTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// Load one Tunai transaction log from file
int LoadTunaiTBankTranLog(void *pstLog, ushort uiIndex)
{
	int		iRet;
	
	PubASSERT( glSysCtrl.sTunaiAcqKeyList[uiIndex]!=INV_ACQ_KEY );
	iRet = PubFileRead(FILE_TUNAI_TRAN_LOG, (long)(sizeof(TRAN_LOG)*uiIndex),
						pstLog, sizeof(TRAN_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTunaiTBankTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// 更新一个交易日志
// update one transaction log
int UpdateTranLog(void *pstLog, ushort uiIndex)
{
	int		iRet;

	PubASSERT( glSysCtrl.sAcqKeyList[uiIndex]!=INV_ACQ_KEY );
	iRet = PubFileWrite(FILE_TRAN_LOG, (long)(sizeof(TRAN_LOG)*uiIndex),
						pstLog, sizeof(TRAN_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("UpdateTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// update one Brizzi transaction log
int UpdateBrizziTranLog(void *pstLog, ushort uiIndex)
{
	int		iRet;

	PubASSERT( glSysCtrl.sBrizziAcqKeyList[uiIndex]!=INV_ACQ_KEY );
	iRet = PubFileWrite(FILE_BRIZZI_TRAN_LOG, (long)(sizeof(TRAN_LOG)*uiIndex),
						pstLog, sizeof(TRAN_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("UpdateBrizziTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// update one Mini Atm CS transaction log
int UpdateMiniAtmCSTranLog(void *pstLog, ushort uiIndex)
{
	int		iRet;

	PubASSERT( glSysCtrl.sMiniAtmCSAcqKeyList[uiIndex]!=INV_ACQ_KEY );
	iRet = PubFileWrite(FILE_MINIATMCS_TRAN_LOG, (long)(sizeof(TRAN_LOG)*uiIndex),
						pstLog, sizeof(TRAN_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("UpdateMiniAtmCSTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// update one Mini Atm transaction log
int UpdateTunaiTranLog(void *pstLog, ushort uiIndex)
{
	int		iRet;

	PubASSERT( glSysCtrl.sTunaiAcqKeyList[uiIndex]!=INV_ACQ_KEY );
	iRet = PubFileWrite(FILE_TUNAI_TRAN_LOG, (long)(sizeof(TRAN_LOG)*uiIndex),
						pstLog, sizeof(TRAN_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("UpdateTunaiTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// 存储交易日志算法(避免结算时候的排序操作，删除交易记录也只是修改索引表)
// 交易日志文件的存储包括: 1.存储ACQ/Issuer Key索引表 2.交易记录
// 由于系统需要操作两次文件系统，这里采用了类似于数据库的两次提交方式:
// Step 1: 记录需要记录的信息（索引表信息、交易记录等）
// Step 2: 完成两次实际的日志信息记录
// 保存一个交易日志

// Algorithm of storing log
// There are 3 places to be store/update when save/update/delete a transaction:
//   1. log data            : saved in log file
//   2. Acq key of each log : saved in glSysCtrl.stAcqKeyList[]
//   3. Iss key of each log : saved in glSysCtrl.sIssuerKeyList[]
// These require to modify 2 files. To avoid exception when writing, it will divided to 3 steps:
//   step 1: write data to "glSysCtrl.stWriteInfo", and set "glSysCtrl.stWriteInfo.bNeedSave" to "TRUE"
//   step 2: write data to 2 files accordingly.
//   step 3: reset the "glSysCtrl.stWriteInfo.bNeedSave" to "FALSE".
// Unless the flag is reset, the applicaion will retry the step 2 and step 3 above.
int SaveTranLog(void *pstLog)
{
	int		iRet, iCnt;

	for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
	{
		if( glSysCtrl.sAcqKeyList[iCnt]==INV_ACQ_KEY )
		{
			break;
		}
	}
	if( iCnt>=MAX_TRANLOG )
	{
		PubTRACE0("SaveTranLog");
		return ERR_FILEOPER;
	}

	glSysCtrl.sAcqKeyList[iCnt]      = glCurTlmTmsAcq.acqID;
	glSysCtrl.sIssuerKeyList[iCnt]   = glCurTlmTmsIss.issID;

	glSysCtrl.uiLastRecNoList[glCurTlmTmsAcq.ucIndex] = (ushort)iCnt;

	glSysCtrl.stWriteInfo.bNeedSave  = TRUE;
	glSysCtrl.stWriteInfo.ucAcqIndex = glCurTlmTmsAcq.ucIndex;
	glSysCtrl.stWriteInfo.uiRecNo    = (ushort)iCnt;

	memcpy(&glSysCtrl.stWriteInfo.stTranLog, pstLog, sizeof(TRAN_LOG));

	// Step 1: 记录需要记录的信息（索引表信息、交易记录等）
	// Save necessary data
	iRet = SaveSysCtrlBase();
	if( iRet<0 )
	{
		return iRet;
	}

	// Step 2: 完成两次实际的日志信息记录
	// Finish the record, and reset the flag to indicate finish.
	return RecoverTranLog();
}

int SaveBrizziTranLog(void *pstLog)
{
	int		iRet, iCnt;

	for(iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
	{
		if( glSysCtrl.sBrizziAcqKeyList[iCnt]==INV_ACQ_KEY )
		{
			break;
		}
	}
	if( iCnt>=MAX_TRANLOG )
	{
		PubTRACE0("SaveBrizziTranLog");
		return ERR_FILEOPER;
	}

	glSysCtrl.sBrizziAcqKeyList[iCnt]      = glCurTlmTmsAcq.acqID;
	glSysCtrl.sBrizziIssuerKeyList[iCnt]   = glCurTlmTmsIss.issID;

	glSysCtrl.uiLastRecNoList[glCurTlmTmsAcq.ucIndex] = (ushort)iCnt;

	glSysCtrl.stWriteInfo.bBrizziNeedSave  = TRUE;
	glSysCtrl.stWriteInfo.ucAcqIndex = glCurTlmTmsAcq.ucIndex;
	glSysCtrl.stWriteInfo.uiBrizziRecNo    = (ushort)iCnt;

	memcpy(&glSysCtrl.stWriteInfo.stTranLog, pstLog, sizeof(TRAN_LOG));

	// Step 1: 记录需要记录的信息（索引表信息、交易记录等）
	// Save necessary data
	iRet = SaveSysCtrlBase();
	if( iRet<0 )
	{
		return iRet;
	}

	// Step 2: 完成两次实际的日志信息记录
	// Finish the record, and reset the flag to indicate finish.
	return RecoverBrizziTranLog();
}

int SaveMiniAtmTranLog(void *pstLog)
{
	int		iRet;
	ushort	iCnt;

	iCnt = glSysCtrl.uiMiniAtmLastRecNo;

	if(iCnt == 0xFFFF)
		iCnt = 0;
	else
		iCnt++;

	if(iCnt>=MAX_MINIATM_TRANLOG)
		iCnt = 0;
	
	glSysCtrl.sMiniAtmCSAcqKeyList[iCnt]      = glCurTlmTmsAcq.acqID;
	glSysCtrl.sMiniAtmCSIssuerKeyList[iCnt]   = glCurTlmTmsIss.issID;

	glSysCtrl.uiLastRecNoList[glCurTlmTmsAcq.ucIndex] = iCnt;

	glSysCtrl.stWriteInfo.bMiniAtmNeedSave  = TRUE;
	glSysCtrl.stWriteInfo.ucAcqIndex = glCurTlmTmsAcq.ucIndex;
	glSysCtrl.stWriteInfo.uiMiniAtmRecNo    = iCnt;

	memcpy(&glSysCtrl.stWriteInfo.stTranLog, pstLog, sizeof(TRAN_LOG));

	// Step 1: 记录需要记录的信息（索引表信息、交易记录等）
	// Save necessary data
	iRet = SaveSysCtrlBase();
	if( iRet<0 )
	{
		return iRet;
	}

	// Step 2: 完成两次实际的日志信息记录
	// Finish the record, and reset the flag to indicate finish.
	return RecoverMiniAtmTranLog();
}


int SaveCardServicesTranLog(void *pstLog)
{
	int		iRet;
	ushort	iCnt;

	iCnt = glSysCtrl.uiCardServicesLastRecNo;

	if(iCnt == 0xFFFF)
		iCnt = 0;
	else
		iCnt++;

	if(iCnt>=MAX_MINIATM_TRANLOG)
		iCnt = 0;

	glSysCtrl.sMiniAtmCSAcqKeyList[iCnt + MAX_MINIATM_TRANLOG]      = glCurTlmTmsAcq.acqID;
	glSysCtrl.sMiniAtmCSIssuerKeyList[iCnt + MAX_MINIATM_TRANLOG]   = glCurTlmTmsIss.issID;

	glSysCtrl.uiLastRecNoList[glCurTlmTmsAcq.ucIndex] = iCnt;

	glSysCtrl.stWriteInfo.bCardServicesNeedSave = TRUE;
	glSysCtrl.stWriteInfo.ucAcqIndex = glCurTlmTmsAcq.ucIndex;
	glSysCtrl.stWriteInfo.uiCardServicesRecNo = iCnt;

	memcpy(&glSysCtrl.stWriteInfo.stTranLog, pstLog, sizeof(TRAN_LOG));

	// Step 1: 记录需要记录的信息（索引表信息、交易记录等）
	// Save necessary data
	iRet = SaveSysCtrlBase();
	if( iRet<0 )
	{
		return iRet;
	}

	// Step 2: 完成两次实际的日志信息记录
	// Finish the record, and reset the flag to indicate finish.
	return RecoverCardServicesTranLog();
}

int SaveTunaiTranLog(void *pstLog)
{
	int		iRet;
	ushort	iCnt;

	iCnt = glSysCtrl.uiTunaiLastRecNo;

	if(iCnt == 0xFFFF)
		iCnt = 0;
	else
		iCnt++;

	if(iCnt>=MAX_TUNAI_TRANLOG)
		iCnt = 0;

	glSysCtrl.sTunaiAcqKeyList[iCnt]      = glCurTlmTmsAcq.acqID;
	glSysCtrl.sTunaiIssuerKeyList[iCnt]   = glCurTlmTmsIss.issID;

	glSysCtrl.uiLastRecNoList[glCurTlmTmsAcq.ucIndex] = iCnt;

	glSysCtrl.stWriteInfo.bTunaiNeedSave  = TRUE;
	glSysCtrl.stWriteInfo.ucAcqIndex = glCurTlmTmsAcq.ucIndex;
	glSysCtrl.stWriteInfo.uiTunaiRecNo    = iCnt;

	memcpy(&glSysCtrl.stWriteInfo.stTranLog, pstLog, sizeof(TRAN_LOG));

	// Step 1: 记录需要记录的信息（索引表信息、交易记录等）
	// Save necessary data
	iRet = SaveSysCtrlBase();
	if( iRet<0 )
	{
		return iRet;
	}

	// Step 2: 完成两次实际的日志信息记录
	// Finish the record, and reset the flag to indicate finish.
	return RecoverTunaiTranLog();
}


int SaveTBankTranLog(void *pstLog)
{
	int		iRet;
	ushort	iCnt;

	iCnt = glSysCtrl.uiTBankLastRecNo;

	if(iCnt == 0xFFFF)
		iCnt = 0;
	else
		iCnt++;

	if(iCnt>=MAX_TUNAI_TRANLOG)
		iCnt = 0;

	glSysCtrl.sTunaiAcqKeyList[iCnt + MAX_TUNAI_TRANLOG]      = glCurTlmTmsAcq.acqID;
	glSysCtrl.sTunaiIssuerKeyList[iCnt + MAX_TUNAI_TRANLOG]   = glCurTlmTmsIss.issID;

	glSysCtrl.uiLastRecNoList[glCurTlmTmsAcq.ucIndex] = iCnt;

	glSysCtrl.stWriteInfo.bTBankNeedSave = TRUE;
	glSysCtrl.stWriteInfo.ucAcqIndex = glCurTlmTmsAcq.ucIndex;
	glSysCtrl.stWriteInfo.uiTBankRecNo = iCnt;

	memcpy(&glSysCtrl.stWriteInfo.stTranLog, pstLog, sizeof(TRAN_LOG));

	// Step 1: 记录需要记录的信息（索引表信息、交易记录等）
	// Save necessary data
	iRet = SaveSysCtrlBase();
	if( iRet<0 )
	{
		return iRet;
	}

	// Step 2: 完成两次实际的日志信息记录
	// Finish the record, and reset the flag to indicate finish.
	return RecoverTBankTransLog();
}


// 恢复日志记录的不一致性
// Finish the save steps.
int RecoverTranLog(void)
{
	int		fd, iRet;

	if( !glSysCtrl.stWriteInfo.bNeedSave )
	{
		return 0;
	}

	fd = open((char *)FILE_TRAN_LOG, O_RDWR|O_CREATE);
	if( fd<0 )
	{
		PubTRACE0("RecoverTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = seek(fd, (long)(glSysCtrl.stWriteInfo.uiRecNo*sizeof(TRAN_LOG)), SEEK_SET);
	if( iRet<0 )
	{
		close(fd);
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = PubFWriteN(fd, &glSysCtrl.stWriteInfo.stTranLog, sizeof(TRAN_LOG));
	close(fd);
	if( iRet!=(int)sizeof(TRAN_LOG) )
	{
		PubTRACE0("RecoverTranLog");
		SysHalt();
		return iRet;
	}

#ifdef	__TELIUM_PARAM__
	glCurTlmTmsAcq.ucIndex= glSysCtrl.stWriteInfo.ucAcqIndex;
#else
	glCurAcq.ucIndex = glSysCtrl.stWriteInfo.ucAcqIndex;
#endif
	if( !(glSysCtrl.stWriteInfo.stTranLog.uiStatus & TS_NOSEND) )
	{
		SaveRevInfo(FALSE);
	}

	glSysCtrl.stWriteInfo.bNeedSave = FALSE;
	glSysCtrl.uiLastRecNo = glSysCtrl.stWriteInfo.uiRecNo;

	return SaveSysCtrlBase();
}


// Finish the save steps.
int RecoverBrizziTranLog(void)
{
	int		fd, iRet;

	if( !glSysCtrl.stWriteInfo.bBrizziNeedSave )
	{
		return 0;
	}

	fd = open((char *)FILE_BRIZZI_TRAN_LOG, O_RDWR|O_CREATE);
	if( fd<0 )
	{
		PubTRACE0("RecoverBrizziTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = seek(fd, (long)(glSysCtrl.stWriteInfo.uiBrizziRecNo*sizeof(TRAN_LOG)), SEEK_SET);
	if( iRet<0 )
	{
		close(fd);
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = PubFWriteN(fd, &glSysCtrl.stWriteInfo.stTranLog, sizeof(TRAN_LOG));
	close(fd);
	if( iRet!=(int)sizeof(TRAN_LOG) )
	{
		PubTRACE0("RecoverBrizziTranLog");
		SysHalt();
		return iRet;
	}

	glCurTlmTmsAcq.ucIndex= glSysCtrl.stWriteInfo.ucAcqIndex;
	if( !(glSysCtrl.stWriteInfo.stTranLog.uiStatus & TS_NOSEND) )
	{
		SaveRevInfo(FALSE);
	}

	glSysCtrl.stWriteInfo.bBrizziNeedSave = FALSE;
	glSysCtrl.uiBrizziLastRecNo = glSysCtrl.stWriteInfo.uiBrizziRecNo;

	return SaveSysCtrlBase();
}


// Finish the save steps.
int RecoverMiniAtmTranLog(void)
{
	int		fd, iRet;

	if( !glSysCtrl.stWriteInfo.bMiniAtmNeedSave )
	{
		return 0;
	}

	fd = open((char *)FILE_MINIATMCS_TRAN_LOG, O_RDWR|O_CREATE);
	if( fd<0 )
	{
		PubTRACE0("RecoverMiniAtmTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = seek(fd, (long)(glSysCtrl.stWriteInfo.uiMiniAtmRecNo*sizeof(TRAN_LOG)), SEEK_SET);
	if( iRet<0 )
	{
		close(fd);
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = PubFWriteN(fd, &glSysCtrl.stWriteInfo.stTranLog, sizeof(TRAN_LOG));
	close(fd);
	if( iRet!=(int)sizeof(TRAN_LOG) )
	{
		PubTRACE0("RecoverMiniAtmTranLog");
		SysHalt();
		return iRet;
	}

	glCurTlmTmsAcq.ucIndex= glSysCtrl.stWriteInfo.ucAcqIndex;
	if( !(glSysCtrl.stWriteInfo.stTranLog.uiStatus & TS_NOSEND) )
	{
		SaveRevInfo(FALSE);
	}

	glSysCtrl.stWriteInfo.bMiniAtmNeedSave = FALSE;
	glSysCtrl.uiMiniAtmLastRecNo = glSysCtrl.stWriteInfo.uiMiniAtmRecNo;

	return SaveSysCtrlBase();
}


// Finish the save steps.
int RecoverCardServicesTranLog(void)
{
	int		fd, iRet;

	if( !glSysCtrl.stWriteInfo.bCardServicesNeedSave )
	{
		return 0;
	}

	fd = open((char *)FILE_MINIATMCS_TRAN_LOG, O_RDWR|O_CREATE);
	if( fd<0 )
	{
		PubTRACE0("RecoverCardServicesTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = seek(fd, (long)((glSysCtrl.stWriteInfo.uiCardServicesRecNo+MAX_MINIATM_TRANLOG)*sizeof(TRAN_LOG)), SEEK_SET);
	if( iRet<0 )
	{
		close(fd);
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = PubFWriteN(fd, &glSysCtrl.stWriteInfo.stTranLog, sizeof(TRAN_LOG));
	close(fd);
	if( iRet!=(int)sizeof(TRAN_LOG) )
	{
		PubTRACE0("RecoverCardServicesTranLog");
		SysHalt();
		return iRet;
	}

	glCurTlmTmsAcq.ucIndex= glSysCtrl.stWriteInfo.ucAcqIndex;
	if( !(glSysCtrl.stWriteInfo.stTranLog.uiStatus & TS_NOSEND) )
	{
		SaveRevInfo(FALSE);
	}

	glSysCtrl.stWriteInfo.bCardServicesNeedSave = FALSE;
	glSysCtrl.uiCardServicesLastRecNo = glSysCtrl.stWriteInfo.uiCardServicesRecNo;

	return SaveSysCtrlBase();
}


// Finish the save steps.
int RecoverTunaiTranLog(void)
{
	int		fd, iRet;

	if( !glSysCtrl.stWriteInfo.bTunaiNeedSave )
	{
		return 0;
	}

	fd = open((char *)FILE_TUNAI_TRAN_LOG, O_RDWR|O_CREATE);
	if( fd<0 )
	{
		PubTRACE0("RecoverTunaiTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = seek(fd, (long)(glSysCtrl.stWriteInfo.uiTunaiRecNo*sizeof(TRAN_LOG)), SEEK_SET);
	if( iRet<0 )
	{
		close(fd);
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = PubFWriteN(fd, &glSysCtrl.stWriteInfo.stTranLog, sizeof(TRAN_LOG));
	close(fd);
	if( iRet!=(int)sizeof(TRAN_LOG) )
	{
		PubTRACE0("RecoverTunaiTranLog");
		SysHalt();
		return iRet;
	}

	glCurTlmTmsAcq.ucIndex= glSysCtrl.stWriteInfo.ucAcqIndex;
	if( !(glSysCtrl.stWriteInfo.stTranLog.uiStatus & TS_NOSEND) )
	{
		SaveRevInfo(FALSE);
	}

	glSysCtrl.stWriteInfo.bTunaiNeedSave = FALSE;
	glSysCtrl.uiTunaiLastRecNo = glSysCtrl.stWriteInfo.uiTunaiRecNo;

	return SaveSysCtrlBase();
}


// Finish the save steps.
int RecoverTBankTransLog(void)
{
	int		fd, iRet;

	if( !glSysCtrl.stWriteInfo.bTBankNeedSave )
	{
		return 0;
	}

	fd = open((char *)FILE_TUNAI_TRAN_LOG, O_RDWR|O_CREATE);
	if( fd<0 )
	{
		PubTRACE0("RecoverTBankTranLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = seek(fd, (long)((glSysCtrl.stWriteInfo.uiTBankRecNo+MAX_TUNAI_TRANLOG)*sizeof(TRAN_LOG)), SEEK_SET);
	if( iRet<0 )
	{
		close(fd);
		SysHalt();
		return ERR_FILEOPER;
	}

	iRet = PubFWriteN(fd, &glSysCtrl.stWriteInfo.stTranLog, sizeof(TRAN_LOG));
	close(fd);
	if( iRet!=(int)sizeof(TRAN_LOG) )
	{
		PubTRACE0("RecoverTBankTranLog");
		SysHalt();
		return iRet;
	}

	glCurTlmTmsAcq.ucIndex= glSysCtrl.stWriteInfo.ucAcqIndex;
	if( !(glSysCtrl.stWriteInfo.stTranLog.uiStatus & TS_NOSEND) )
	{
		SaveRevInfo(FALSE);
	}

	glSysCtrl.stWriteInfo.bTBankNeedSave = FALSE;
	glSysCtrl.uiTBankLastRecNo = glSysCtrl.stWriteInfo.uiTBankRecNo;

	return SaveSysCtrlBase();
}


// 获取交易记录总数
// Retrieve total transaction record number.
ushort GetTranLogNum(uchar ucAcqKey)
{
	ushort	uiCnt, uiTranNum;

	if( ucAcqKey==ACQ_ALL )
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sAcqKeyList[uiCnt]!=INV_ACQ_KEY )
			{
				uiTranNum++;
			}
		}
	}
	else
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sAcqKeyList[uiCnt]==ucAcqKey )
			{
				uiTranNum++;
			}
		}
	}

	return uiTranNum;
}

ushort GetBrizziTranLogNum(uchar ucAcqKey)
{
	ushort	uiCnt, uiTranNum;

	if( ucAcqKey==ACQ_ALL )
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sBrizziAcqKeyList[uiCnt]!=INV_ACQ_KEY )
			{
				uiTranNum++;
			}
		}
	}
	else
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sBrizziAcqKeyList[uiCnt]==ucAcqKey )
			{
				uiTranNum++;
			}
		}
	}

	return uiTranNum;
}

ushort GetMiniAtmTranLogNum(uchar ucAcqKey)
{
	ushort	uiCnt, uiTranNum;

	if( ucAcqKey==ACQ_ALL )
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_MINIATM_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sMiniAtmCSAcqKeyList[uiCnt]!=INV_ACQ_KEY )
			{
				uiTranNum++;
			}
		}
	}
	else
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_MINIATM_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sMiniAtmCSAcqKeyList[uiCnt]==ucAcqKey )
			{
				uiTranNum++;
			}
		}
	}

	return uiTranNum;
}


ushort GetCardServicesTranLogNum(uchar ucAcqKey)
{
	ushort	uiCnt, uiTranNum;

	if( ucAcqKey==ACQ_ALL )
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_MINIATM_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sMiniAtmCSAcqKeyList[uiCnt+MAX_MINIATM_TRANLOG]!=INV_ACQ_KEY )
			{
				uiTranNum++;
			}
		}
	}
	else
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_MINIATM_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sMiniAtmCSAcqKeyList[uiCnt+MAX_MINIATM_TRANLOG]==ucAcqKey )
			{
				uiTranNum++;
			}
		}
	}

	return uiTranNum;
}


ushort GetTunaiTranLogNum(uchar ucAcqKey)
{
	ushort	uiCnt, uiTranNum;

	if( ucAcqKey==ACQ_ALL )
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_TUNAI_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sTunaiAcqKeyList[uiCnt]!=INV_ACQ_KEY )
			{
				uiTranNum++;
			}
		}
	}
	else
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_TUNAI_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sTunaiAcqKeyList[uiCnt]==ucAcqKey )
			{
				uiTranNum++;
			}
		}
	}

	return uiTranNum;
}

ushort GetTBankTranLogNum(uchar ucAcqKey)
{
	ushort	uiCnt, uiTranNum;

	if( ucAcqKey==ACQ_ALL )
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_TUNAI_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sTunaiAcqKeyList[uiCnt+MAX_TUNAI_TRANLOG]!=INV_ACQ_KEY )
			{
				uiTranNum++;
			}
		}
	}
	else
	{
		for(uiTranNum=uiCnt=0; uiCnt<MAX_TUNAI_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sTunaiAcqKeyList[uiCnt+MAX_TUNAI_TRANLOG]==ucAcqKey )
			{
				uiTranNum++;
			}
		}
	}

	return uiTranNum;
}

// 检查最后一笔交易是否与当前交易重复,并进行提示
// Check whether the last transaction is duplicated with current going-on one.
uchar AllowDuplicateTran(void)
{
	TRAN_LOG	stTranLog;
	ushort		uiTranNum;

	uiTranNum = GetTranLogNum(ACQ_ALL);
	if( uiTranNum==0 || glSysCtrl.uiLastRecNo>=MAX_TRANLOG )
	{
		return TRUE;
	}
	if( glSysCtrl.sAcqKeyList[glSysCtrl.uiLastRecNo]==INV_ACQ_KEY )
	{
		return TRUE;
	}

	memset(&stTranLog, 0, sizeof(TRAN_LOG));
	LoadTranLog(&stTranLog, glSysCtrl.uiLastRecNo);

	if( stTranLog.ucTranType!=glProcInfo.stTranLog.ucTranType                       ||
		strncmp((char *)stTranLog.szPan, (char *)glProcInfo.stTranLog.szPan, 19)!=0 ||
		memcmp(stTranLog.szAmount,       glProcInfo.stTranLog.szAmount,      12)!=0 ||
		memcmp(stTranLog.szTipAmount,    glProcInfo.stTranLog.szTipAmount,   12)!=0
		)
	{
		return TRUE;
	}

	ScrCls();
	DispTransName();
	PubDispString(_T("DUPLICATE? Y/N"), 4|DISP_LINE_LEFT);

	return !PubYesNo(USER_OPER_TIMEOUT);
}


// 输入交易流水号以获取该交易数据
// 入口： 限制的交易状态
// 出口： 0 获取成功
// Get invoice to retrieve the transaction record
int GetRecord(ushort uiStatus, void *pstOutTranLog)
{
	int			iRet;
	ushort		uiCnt;
	ulong		ulInvoiceNo;
	TRAN_LOG	*pstLog;

	pstLog = (TRAN_LOG *)pstOutTranLog;
	while( 1 )
	{
		iRet = InputInvoiceNo(&ulInvoiceNo);
		if( iRet!=0 )
		{
			return iRet;
		}

		for(uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sAcqKeyList[uiCnt]==INV_ACQ_KEY )
			{
				continue;
			}

			memset(pstLog, 0, sizeof(TRAN_LOG));
			iRet = LoadTranLog(pstLog, uiCnt);
			if( iRet!=0 )
			{
				return iRet;
			}
			if( pstLog->ulInvoiceNo!=ulInvoiceNo )
			{
				continue;
			}
			if( ((pstLog->uiStatus) & 0x0F)<=uiStatus )
			{
				glProcInfo.uiRecNo = uiCnt;
				return 0;
			}
			else
			{
				DispStateText(pstLog->uiStatus);
				break;
			}
		}
		if( uiCnt>=MAX_TRANLOG )
		{
			ScrClrLine(2, 7);
			PubDispString(_T("INVALID TRACE"), 4|DISP_LINE_LEFT);
		}
		PubBeepErr();
		PubWaitKey(2);
	}
}

int GetBrizziRecord(ushort uiStatus, void *pstOutTranLog)
{
	int			iRet;
	ushort		uiCnt;
	ulong		ulInvoiceNo;
	TRAN_LOG	*pstLog;

	pstLog = (TRAN_LOG *)pstOutTranLog;
	while( 1 )
	{
		iRet = InputInvoiceNo(&ulInvoiceNo);
		if( iRet!=0 )
		{
			return iRet;
		}

		for(uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sBrizziAcqKeyList[uiCnt]==INV_ACQ_KEY )
			{
				continue;
			}

			memset(pstLog, 0, sizeof(TRAN_LOG));
			iRet = LoadBrizziTranLog(pstLog, uiCnt);
			if( iRet!=0 )
			{
				return iRet;
			}
			if( pstLog->ulInvoiceNo!=ulInvoiceNo )
			{
				continue;
			}
			if( ((pstLog->uiStatus) & 0x0F)<=uiStatus )
			{
				glProcInfo.uiRecNo = uiCnt;
				return 0;
			}
			else
			{
				DispStateText(pstLog->uiStatus);
				break;
			}
		}
		if( uiCnt>=MAX_TRANLOG )
		{
			ScrClrLine(2, 7);
			PubDispString(_T("INVALID TRACE"), 4|DISP_LINE_LEFT);
		}
		PubBeepErr();
		PubWaitKey(2);
	}
}


int GetMiniAtmRecord(ushort uiStatus, void *pstOutTranLog)
{
	int			iRet;
	ushort		uiCnt;
	ulong		ulInvoiceNo;
	TRAN_LOG	*pstLog;

	pstLog = (TRAN_LOG *)pstOutTranLog;
	while( 1 )
	{
		iRet = InputInvoiceNo(&ulInvoiceNo);
		if( iRet!=0 )
		{
			return iRet;
		}

		for(uiCnt=0; uiCnt<MAX_MINIATM_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sMiniAtmCSAcqKeyList[uiCnt]==INV_ACQ_KEY )
			{
				continue;
			}

			memset(pstLog, 0, sizeof(TRAN_LOG));
			iRet = LoadMiniAtmCSTranLog(pstLog, uiCnt);
			if( iRet!=0 )
			{
				return iRet;
			}
			if( pstLog->ulInvoiceNo!=ulInvoiceNo )
			{
				continue;
			}
			if( ((pstLog->uiStatus) & 0x0F)<=uiStatus )
			{
				glProcInfo.uiRecNo = uiCnt;
				return 0;
			}
			else
			{
				DispStateText(pstLog->uiStatus);
				break;
			}
		}
		if( uiCnt>=MAX_MINIATM_TRANLOG )
		{
			ScrClrLine(2, 7);
			PubDispString(_T("INVALID TRACE"), 4|DISP_LINE_LEFT);
		}
		PubBeepErr();
		PubWaitKey(2);
	}
}


int GetCardServicesRecord(ushort uiStatus, void *pstOutTranLog)
{
	int			iRet;
	ushort		uiCnt;
	ulong		ulInvoiceNo;
	TRAN_LOG	*pstLog;

	pstLog = (TRAN_LOG *)pstOutTranLog;
	while( 1 )
	{
		iRet = InputInvoiceNo(&ulInvoiceNo);
		if( iRet!=0 )
		{
			return iRet;
		}

		for(uiCnt=0; uiCnt<MAX_MINIATM_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sMiniAtmCSAcqKeyList[uiCnt+MAX_MINIATM_TRANLOG]==INV_ACQ_KEY )
			{
				continue;
			}

			memset(pstLog, 0, sizeof(TRAN_LOG));
			iRet = LoadMiniAtmCSTranLog(pstLog, (ushort)(uiCnt+MAX_MINIATM_TRANLOG));
			if( iRet!=0 )
			{
				return iRet;
			}
			if( pstLog->ulInvoiceNo!=ulInvoiceNo )
			{
				continue;
			}
			if( ((pstLog->uiStatus) & 0x0F)<=uiStatus )
			{
				glProcInfo.uiRecNo = uiCnt;
				return 0;
			}
			else
			{
				DispStateText(pstLog->uiStatus);
				break;
			}
		}
		if( uiCnt>=MAX_MINIATM_TRANLOG )
		{
			ScrClrLine(2, 7);
			PubDispString(_T("INVALID TRACE"), 4|DISP_LINE_LEFT);
		}
		PubBeepErr();
		PubWaitKey(2);
	}
}

int GetTunaiRecord(ushort uiStatus, void *pstOutTranLog)
{
	int			iRet;
	ushort		uiCnt;
	ulong		ulInvoiceNo;
	TRAN_LOG	*pstLog;

	pstLog = (TRAN_LOG *)pstOutTranLog;
	while( 1 )
	{
		iRet = InputInvoiceNo(&ulInvoiceNo);
		if( iRet!=0 )
		{
			return iRet;
		}

		for(uiCnt=0; uiCnt<MAX_TUNAI_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sTunaiAcqKeyList[uiCnt]==INV_ACQ_KEY )
			{
				continue;
			}

			memset(pstLog, 0, sizeof(TRAN_LOG));
			iRet = LoadTunaiTBankTranLog(pstLog, uiCnt);
			if( iRet!=0 )
			{
				return iRet;
			}
			if( pstLog->ulInvoiceNo!=ulInvoiceNo )
			{
				continue;
			}
			if( ((pstLog->uiStatus) & 0x0F)<=uiStatus )
			{
				glProcInfo.uiRecNo = uiCnt;
				return 0;
			}
			else
			{
				DispStateText(pstLog->uiStatus);
				break;
			}
		}
		if( uiCnt>=MAX_TRANLOG )
		{
			ScrClrLine(2, 7);
			PubDispString(_T("INVALID TRACE"), 4|DISP_LINE_LEFT);
		}
		PubBeepErr();
		PubWaitKey(2);
	}
}

int GetTBankRecord(ushort uiStatus, void *pstOutTranLog)
{
	int			iRet;
	ushort		uiCnt;
	ulong		ulInvoiceNo;
	TRAN_LOG	*pstLog;

	pstLog = (TRAN_LOG *)pstOutTranLog;
	while( 1 )
	{
		iRet = InputInvoiceNo(&ulInvoiceNo);
		if( iRet!=0 )
		{
			return iRet;
		}

		for(uiCnt=0; uiCnt<MAX_TUNAI_TRANLOG; uiCnt++)
		{
			if( glSysCtrl.sTunaiAcqKeyList[uiCnt+MAX_TUNAI_TRANLOG]==INV_ACQ_KEY )
			{
				continue;
			}

			memset(pstLog, 0, sizeof(TRAN_LOG));
			iRet = LoadTunaiTBankTranLog(pstLog, (ushort)(uiCnt+MAX_TUNAI_TRANLOG));
			if( iRet!=0 )
			{
				return iRet;
			}
			if( pstLog->ulInvoiceNo!=ulInvoiceNo )
			{
				continue;
			}
			if( ((pstLog->uiStatus) & 0x0F)<=uiStatus )
			{
				glProcInfo.uiRecNo = uiCnt;
				return 0;
			}
			else
			{
				DispStateText(pstLog->uiStatus);
				break;
			}
		}
		if( uiCnt>=MAX_TUNAI_TRANLOG )
		{
			ScrClrLine(2, 7);
			PubDispString(_T("INVALID TRACE"), 4|DISP_LINE_LEFT);
		}
		PubBeepErr();
		PubWaitKey(2);
	}
}

void CalcTBankTotal(void *TotalInfo, ushort *TotalCnt,  uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	uiCnt;
	ushort	lenRec=0;
	uchar flagSend=FALSE;
	ushort idx=0;
	TRAN_LOG	stTranLog;
	ushort	uiTotalCnt=0;
	

	for(uiCnt=0; uiCnt<MAX_TUNAI_TRANLOG; uiCnt++)
	{
		
		if( glSysCtrl.sTunaiAcqKeyList[uiCnt+MAX_TUNAI_TRANLOG]==INV_ACQ_KEY )
		{
			continue;
		}
		
		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadTunaiTBankTranLog(&stTranLog, (ushort)(uiCnt+MAX_TUNAI_TRANLOG));
		if( iRet!=0 )
		{
			return ;
		}

		if(flagDate)
		{
			if(memcmp(stTranLog.szDateTime+2, date+4, 2)!=0)
				continue;
							
			if(memcmp(stTranLog.szDateTime+4, date+2, 2)!=0)
				continue;

			if(memcmp(stTranLog.szDateTime+6, date, 2)!=0)
				continue;
		}

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_CASH_IN:
				idx=0;
				if((((TBANK_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((TBANK_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "CASH IN");

				(((TBANK_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				uiTotalCnt++;
				break;
			case TRANS_TYPE_CASHOUT:
				idx=1;
				if((((TBANK_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((TBANK_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "CASH OUT");

				(((TBANK_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				uiTotalCnt++;
				break;
		}
		
	}
	*TotalCnt = uiTotalCnt;
	return;
}

void CalcTunaiTotal(void *TotalInfo, ushort *TotalCnt, uchar *TotalAmt, uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	uiCnt;
	ushort	lenRec=0;
	uchar flagSend=FALSE;
	ushort idx=0;
	TRAN_LOG	stTranLog;
	ushort	uiTotalCnt=0;
	uchar	szTotalAmt[12+1];
	

	MEM_ZERO(szTotalAmt);

	memset(szTotalAmt, '0', 12);
	for(idx=0; idx<64;idx++)
	{
		memset((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt,       '0', 12);
	}
	

	
	for(uiCnt=0; uiCnt<MAX_TUNAI_TRANLOG; uiCnt++)
	{
		
		if( glSysCtrl.sTunaiAcqKeyList[uiCnt]==INV_ACQ_KEY )
		{
			continue;
		}
		
		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadTunaiTBankTranLog(&stTranLog, uiCnt);
		if( iRet!=0 )
		{
			return ;
		}

		if(flagDate)
		{
			if(memcmp(stTranLog.szDateTime+2, date+4, 2)!=0)
				continue;
							
			if(memcmp(stTranLog.szDateTime+4, date+2, 2)!=0)
				continue;

			if(memcmp(stTranLog.szDateTime+6, date, 2)!=0)
				continue;
		}

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_SETOR_PINJAMAN:
				idx=0;
				if((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "SETOR PINJAMAN");

				(((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_SETOR_SIMPANAN:
				idx=1;
				if((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "SETOR SIMPANAN");

				(((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_TARIK_TUNAI:
				idx=2;
				if((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
				strcpy((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TARIK TUNAI");

				(((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_VOID_TARIK_TUNAI:
				idx=3;
				if((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "VOID TARIK TUNAI");

				(((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((TUNAI_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
		}
		
	}
	*TotalCnt = uiTotalCnt;
	memcpy(TotalAmt, szTotalAmt, sizeof(szTotalAmt));
	return;
}

void CalcCardServicesTotal(void *TotalInfo, ushort *TotalCnt,  uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	uiCnt;
	ushort	lenRec=0;
	uchar flagSend=FALSE;
	ushort idx=0;
	TRAN_LOG	stTranLog;
	ushort	uiTotalCnt=0;
	

	for(uiCnt=0; uiCnt<MAX_MINIATM_TRANLOG; uiCnt++)
	{
		
		if( glSysCtrl.sMiniAtmCSAcqKeyList[uiCnt+MAX_MINIATM_TRANLOG]==INV_ACQ_KEY )
		{
			continue;
		}
		
		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadMiniAtmCSTranLog(&stTranLog, (ushort)(uiCnt+MAX_MINIATM_TRANLOG));
		if( iRet!=0 )
		{
			return ;
		}

		if(flagDate)
		{
			if(memcmp(stTranLog.szDateTime+2, date+4, 2)!=0)
				continue;
							
			if(memcmp(stTranLog.szDateTime+4, date+2, 2)!=0)
				continue;

			if(memcmp(stTranLog.szDateTime+6, date, 2)!=0)
				continue;
		}

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_AKTIFASIKARTU_PRINT:
				idx=0;
				if((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "AKTIVASI KARTU");

				(((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				uiTotalCnt++;
				break;
			case TRANS_TYPE_RENEWALKARTU_BARU:
				idx=1;
				if((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "RENEWAL KARTU");

				(((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				uiTotalCnt++;
				break;
			case TRANS_TYPE_REISSUE_PIN_NASABAH:
				idx=2;
				if((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "REISSUE PIN");

				(((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				uiTotalCnt++;
				break;
			case TRANS_TYPE_UBAHPWD_SPV:
				idx=3;
				if((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "UBAH PASSWORD PETUGAS");

				(((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				uiTotalCnt++;
				break;
			case TRANS_TYPE_UBAHPIN_CS:
				idx=4;
				if((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "UBAH PIN");

				(((CARDSERVICES_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				uiTotalCnt++;
				break;
		}
		
	}
	*TotalCnt = uiTotalCnt;
	return;
}

void CalcMiniAtmTotal(void *TotalInfo, ushort *TotalCnt, uchar *TotalAmt, uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	uiCnt;
	ushort	lenRec=0;
	uchar flagSend=FALSE;
	ushort idx=0;
	TRAN_LOG	stTranLog;
	ushort	uiTotalCnt=0;
	uchar	szTotalAmt[12+1];
	

	MEM_ZERO(szTotalAmt);

	memset(szTotalAmt, '0', 12);
	for(idx=0; idx<64;idx++)
	{
		memset((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt,       '0', 12);
	}
	

	
	for(uiCnt=0; uiCnt<MAX_MINIATM_TRANLOG; uiCnt++)
	{
		
		if( glSysCtrl.sMiniAtmCSAcqKeyList[uiCnt]==INV_ACQ_KEY )
		{
			continue;
		}
		
		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadMiniAtmCSTranLog(&stTranLog, uiCnt);
		if( iRet!=0 )
		{
			return ;
		}

		if(flagDate)
		{
			if(memcmp(stTranLog.szDateTime+2, date+4, 2)!=0)
				continue;
							
			if(memcmp(stTranLog.szDateTime+4, date+2, 2)!=0)
				continue;

			if(memcmp(stTranLog.szDateTime+6, date, 2)!=0)
				continue;
		}

		switch(stTranLog.ucTranType)
		{
			case TRANS_TYPE_TRFSESAMABRI:
				idx=0;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TRANSFER SESAMA BRI");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_TRFANTARBANK:
				idx=1;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TRANSFER ANTAR BANK");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_TELKOM:
				idx=2;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TELKOM");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KARTUHALO:
				idx=3;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "HALO PASCABAYAR");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KARTUMATRIX:
				idx=4;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "SELULAR PASCABAYAR");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_PLNPASCABAYAR:
				idx=5;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "PLN POSTPAID");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_PLNPRABAYAR:
				idx=6;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "PLN PREPAID");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KKBRI:
				idx=7;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KARTU KREDIT BRI");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KKCITIBANK:
				idx=8;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KARTU KREDIT CITIBANK");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KTACITIBANK:
				idx=9;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KTA CITIBANK");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KKSCB:
				idx=10;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KARTU KREDIT S. CHART");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KTASCB:
				idx=11;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KTA S. CHART");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KKHSBC:
				idx=13;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KARTU KREDIT HSBC");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KTAHSBC:
				idx=14;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KTA HSBC");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KTARBS:
				idx=15;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KTA RBS");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_KKANZ:
				idx=12;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KARTU KREDIT ANZ");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_FIF:
				idx=16;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "CICILAN FIF");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_BAF:
				idx=17;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "CICILAN BAF");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_OTO:
				idx=18;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "CICILAN OTO");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_FINANSIA:
				idx=19;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "CICILAN FINANSIA");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_VERENA:
				idx=20;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "CICILAN VERENA");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_WOM:
				idx=21;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "PEMB CICILAN WOM");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_ZAKAT:
				idx=22;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "BAYAR ZAKAT");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_INFAQ:
				idx=23;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "BAYAR INFAQ");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_DPLK:
				idx=24;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "BAYAR DPLK");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_GARUDA:
				idx=25;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TIKET GARUDA");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_LIONAIR:
				idx=26;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TIKET LION");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_SRIWIJAYA:
				idx=27;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TIKET SRIWIJAYA");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_MERPATI:
				idx=28;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TIKET MERPATI");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_BRIVA:
				idx=29;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TIKET BRIVA");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_BAYARKULIAH:
				idx=30;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "BAYAR SPP");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_IM3:
				idx=31;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA IM3");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_SIMPATI_AS:
				idx=32;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA SIMPATI/AS");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_MENTARI:
				idx=33;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA MENTARI");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_ESIA:
				idx=34;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA ESIA");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_SMARTFREN:
				idx=35;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA SMART");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			//ditutup 6 Januari 2014
			/*
			case TRANS_TYPE_FREN_MOBI_HEPI:
				idx=35;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA FREN");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			*/
			case TRANS_TYPE_THREE:
				idx=36;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA THREE");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_AXIS:
				idx=37;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA AXIS");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
			case TRANS_TYPE_XL:
				idx=38;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "ISI PULSA XL");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;




				// ADD FOR MINI ATM BUKOPIN
			
			case TRANS_TYPE_TRFANTARBANK_BKPN:
				idx=39;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TRANSFER");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;

			case TRANS_TYPE_SETOR_TUNAI_BKPN:
				idx=40;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "SETOR TUNAI");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;

			case TRANS_TYPE_TARIK_TUNAI_BKPN:
				idx=41;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "TARIK TUNAI");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;

			case TRANS_TYPE_PDAM_PALYJA_BKPN:
				idx=42;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "PDAM PALYJA");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;

			case TRANS_TYPE_PDAM_AETRA_BKPN:
				idx=43;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "PDAM AETRA");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;

			case TRANS_TYPE_KK_ANZ_BKPN:
				idx=43;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KK ANZ");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;

			case TRANS_TYPE_KK_BUKOPIN_BKPN:
				idx=44;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KK BUKOPIN");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;

			case TRANS_TYPE_KK_BNI_BKPN:
				idx=45;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KK BNI");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;

			case TRANS_TYPE_KK_HSBC_BKPN:
				idx=46;
				if((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt==0)
					strcpy((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szNamaFitur, "KK HSBC");

				(((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).uiTotalCnt++;
				SafeAscAdd((((MINIATM_TOTAL_INFO*)TotalInfo)[idx]).szTotalAmt, stTranLog.szAmount, 12);

				uiTotalCnt++;
				SafeAscAdd(szTotalAmt, stTranLog.szAmount, 12);
				break;
	
			
		}
		
	}
	*TotalCnt = uiTotalCnt;
	memcpy(TotalAmt, szTotalAmt, sizeof(szTotalAmt));
	return;
}


void CalcBrizziTotal(uchar ucAcqKey, uchar * date, uchar flagDate)
{
	int	iRet;
	ushort	uiCnt;
	ushort	lenRec=0;
	uchar flagSend=FALSE;
	ushort pos=0;
	uchar ucTranAct;
	TRAN_LOG	stTranLog;

	ClearBrizziTotalInfo(&glBrizziTotal);
	
	for(uiCnt=0; uiCnt<MAX_TRANLOG; uiCnt++)
	{
		
		if( glSysCtrl.sBrizziAcqKeyList[uiCnt]==INV_ACQ_KEY )
		{
			continue;
		}
		
		memset(&stTranLog, 0, sizeof(TRAN_LOG));
		iRet = LoadBrizziTranLog(&stTranLog, uiCnt);
		if( iRet!=0 )
		{
			return ;
		}

		if(flagDate)
		{
			if(memcmp(stTranLog.szDateTime+2, date+4, 2)!=0)
				continue;
							
			if(memcmp(stTranLog.szDateTime+4, date+2, 2)!=0)
				continue;

			if(memcmp(stTranLog.szDateTime+6, date, 2)!=0)
				continue;
		}

		ucTranAct = glTranConfig[stTranLog.ucTranType].ucTranAct2;
		if( ucTranAct & IN_BRIZZI_OFFLINE_SALE_TOTAL )
		{
			if( stTranLog.uiStatus & TS_VOID )
			{
				glBrizziTotal.uiVoidBrizziCnt++;
				SafeAscAdd(glBrizziTotal.szVoidBrizziAmt, stTranLog.szAmount, 12);
			}
			else
			{
				glBrizziTotal.uiSaleCnt++;
				SafeAscAdd(glBrizziTotal.szSaleAmt, stTranLog.szAmount, 12);

			}
		}
		else if( ucTranAct & IN_BRIZZI_TOPUP_ONL_TOTAL )
		{
			glBrizziTotal.uiTopupOnlineCnt++;
				SafeAscAdd(glBrizziTotal.szTopupOnlineAmt, stTranLog.szAmount, 12);

		}
		else if( ucTranAct & IN_BRIZZI_TOPUP_DEP_TOTAL )
		{
			glBrizziTotal.uiTopupDepositCnt++;
			SafeAscAdd(glBrizziTotal.szTopupDepositAmt, stTranLog.szAmount, 12);
		}
		else if( ucTranAct & IN_BRIZZI_AKT_DEPOSIT_TOTAL )
		{
			glBrizziTotal.uiAktivasiDepositCnt++;
			SafeAscAdd(glBrizziTotal.szAktivasiDepositAmt, stTranLog.szAmount, 12);
		}
		else if( ucTranAct & IN_BRIZZI_REAKTIVASI_TOTAL )
		{
			glBrizziTotal.uiReaktivasiCnt++;
			SafeAscAdd(glBrizziTotal.szReaktivasiAmt, stTranLog.szAmount, 12);
		}
		else if( ucTranAct & IN_BRIZZI_REDEEM_TOTAL )
		{
			glBrizziTotal.uiRedeemCnt++;
			SafeAscAdd(glBrizziTotal.szRedeemAmt, stTranLog.szAmount, 12);
		}
	}
	return;
}


void CalcTotal(uchar ucAcqKey)
{
	uchar		ucAcqIndex, ucIssIndex, ucTranAct;
	uchar		szTotalAmt[12+1];
	ushort		uiIndex;
	TRAN_LOG	stLog;
	uchar		ucBinIndex;

	// clear all total information
	ClearTotalInfo(&glTransTotal);
	ClearTotalInfo(&glEdcTotal);
	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		ClearTotalInfo(&glAcqTotal[ucAcqIndex]);
	}
	for(ucIssIndex=0; ucIssIndex<glTlmSysParam.ucIssNum; ucIssIndex++)
	{
		ClearTotalInfo(&glIssuerTotal[ucIssIndex]);
	}
	for(ucBinIndex=0; ucBinIndex<glTlmSysParam.ucBinNum; ucBinIndex++)
	{
		ClearTotalInfo(&glCardTotal[ucBinIndex]);
	}		

	// 计算出所有acquirer和issuer,以及edc的统计
	// Calculate totals in terms of EDC, acquirer ans issuer.
	for(uiIndex=0; uiIndex<MAX_TRANLOG; uiIndex++)
	{
		if( glSysCtrl.sAcqKeyList[uiIndex]==INV_ACQ_KEY )
		{
			continue;
		}

		memset(&stLog, 0, sizeof(TRAN_LOG));
		LoadTranLog(&stLog, uiIndex);

		for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
		{
			if( glTlmSysParam.Acq[ucAcqIndex].acqID==stLog.ucAcqKey )
			{
				break;
			}
		}
		PubASSERT( ucAcqIndex<glTlmSysParam.ucAcqNum );

		for(ucIssIndex=0; ucIssIndex<glTlmSysParam.ucIssNum; ucIssIndex++)
		{
			if( glTlmSysParam.Iss[ucIssIndex].issID==stLog.ucIssuerKey )
			{
				break;
			}
		}
		PubASSERT( ucIssIndex<glTlmSysParam.ucIssNum );

		for(ucBinIndex=0; ucBinIndex<glTlmSysParam.ucBinNum; ucBinIndex++)
		{
			if(strcmp(glTlmSysParam.Bin[ucBinIndex].binName, stLog.szCardType)==0 ) 
			{
				break;
			}
		}		
		PubASSERT( ucIssIndex<glTlmSysParam.ucBinNum );
		

		PubAscAdd(stLog.szAmount, stLog.szTipAmount, 12, szTotalAmt);
		//PubAddHeadChars(szTotalAmt, 12, '0');		no need: already 12 digits

		if( stLog.ucTranType==TRANS_TYPE_VOID || (stLog.uiStatus & TS_VOID) )
		{
			ucTranAct = glTranConfig[stLog.ucOrgTranType].ucTranAct;
		}
		else
		{
			ucTranAct = glTranConfig[stLog.ucTranType].ucTranAct;
		}
		
		if( ucTranAct & IN_SALE_TOTAL )
		{
			if( stLog.uiStatus & TS_VOID )
			{
				if(stLog.uiEntryMode==MODE_MANUAL_INPUT)
				{
					glEdcTotal.uiVoidSaleManualCnt++;
					SafeAscAdd(glEdcTotal.szVoidSaleManualAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiVoidSaleManualCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szVoidSaleManualAmt, szTotalAmt, 12);

					glIssuerTotal[ucIssIndex].uiVoidSaleManualCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szVoidSaleManualAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiVoidSaleManualCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szVoidSaleManualAmt, szTotalAmt, 12);

				}
				else
				{
					glEdcTotal.uiVoidSaleNonManualCnt++;
					SafeAscAdd(glEdcTotal.szVoidSaleNonManualAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiVoidSaleNonManualCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szVoidSaleNonManualAmt, szTotalAmt, 12);

					glIssuerTotal[ucIssIndex].uiVoidSaleNonManualCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szVoidSaleNonManualAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiVoidSaleNonManualCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szVoidSaleNonManualAmt, szTotalAmt, 12);
				}
#ifndef BANK_BUKOPIN
				glEdcTotal.uiVoidSaleCnt++;
				SafeAscAdd(glEdcTotal.szVoidSaleAmt, szTotalAmt, 12);

				glAcqTotal[ucAcqIndex].uiVoidSaleCnt++;
				SafeAscAdd(glAcqTotal[ucAcqIndex].szVoidSaleAmt, szTotalAmt, 12);
				
				glIssuerTotal[ucIssIndex].uiVoidSaleCnt++;
				SafeAscAdd(glIssuerTotal[ucIssIndex].szVoidSaleAmt, szTotalAmt, 12);

				glCardTotal[ucBinIndex].uiVoidSaleCnt++;
				SafeAscAdd(glCardTotal[ucBinIndex].szVoidSaleAmt, szTotalAmt, 12);
					
#else
				//if(stLog.ucTranType==TRANS_TYPE_OFF_SALE){ //4212 for printing
				if(stLog.uiStatus & TS_NOSEND){ //4212 for printing	
					glEdcTotal.uiVoidSaleOfflineCnt++;
					SafeAscAdd(glEdcTotal.szVoidSaleOfflineAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiVoidSaleOfflineCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szVoidSaleOfflineAmt, szTotalAmt, 12);
				
					glIssuerTotal[ucIssIndex].uiVoidSaleOfflineCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szVoidSaleOfflineAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiVoidSaleOfflineCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szVoidSaleOfflineAmt, szTotalAmt, 12);
					
				} else {
					glEdcTotal.uiVoidSaleCnt++;
					SafeAscAdd(glEdcTotal.szVoidSaleAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiVoidSaleCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szVoidSaleAmt, szTotalAmt, 12);
				
					glIssuerTotal[ucIssIndex].uiVoidSaleCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szVoidSaleAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiVoidSaleCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szVoidSaleAmt, szTotalAmt, 12);

				///////////////////////////////////////////////////////////////////////////////////////4212
				
					glEdcTotal.uiVoidSaleOnlineCnt++;
					SafeAscAdd(glEdcTotal.szVoidSaleOnlineAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiVoidSaleOnlineCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szVoidSaleOnlineAmt, szTotalAmt, 12);
				
					glIssuerTotal[ucIssIndex].uiVoidSaleOnlineCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szVoidSaleOnlineAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiVoidSaleOnlineCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szVoidSaleOnlineAmt, szTotalAmt, 12);
				}
#endif

			}
			else
			{
				if(stLog.uiEntryMode==MODE_MANUAL_INPUT)
				{
					glEdcTotal.uiSaleManualCnt++;
					SafeAscAdd(glEdcTotal.szSaleManualAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiSaleManualCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szSaleManualAmt, szTotalAmt, 12);

					glIssuerTotal[ucIssIndex].uiSaleManualCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szSaleManualAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiSaleManualCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szSaleManualAmt, szTotalAmt, 12);
				}
				else
				{
					glEdcTotal.uiSaleNonManualCnt++;
					SafeAscAdd(glEdcTotal.szSaleNonManualAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiSaleNonManualCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szSaleNonManualAmt, szTotalAmt, 12);

					glIssuerTotal[ucIssIndex].uiSaleNonManualCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szSaleNonManualAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiSaleNonManualCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szSaleNonManualAmt, szTotalAmt, 12);
				}
				
				glEdcTotal.uiSaleCnt++;
				SafeAscAdd(glEdcTotal.szSaleAmt, szTotalAmt, 12);

				glAcqTotal[ucAcqIndex].uiSaleCnt++;
				SafeAscAdd(glAcqTotal[ucAcqIndex].szSaleAmt, szTotalAmt, 12);

				glIssuerTotal[ucIssIndex].uiSaleCnt++;
				SafeAscAdd(glIssuerTotal[ucIssIndex].szSaleAmt, szTotalAmt, 12);

				glCardTotal[ucBinIndex].uiSaleCnt++;
				SafeAscAdd(glCardTotal[ucBinIndex].szSaleAmt, szTotalAmt, 12);
	
#ifdef BANK_BUKOPIN
				//if(stLog.ucTranType==TRANS_TYPE_OFF_SALE){ //4212 for printing
				if(stLog.uiStatus & TS_NOSEND){ //4212 for printing	
					glEdcTotal.uiSaleOfflineCnt++;
					SafeAscAdd(glEdcTotal.szSaleOfflineAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiSaleOfflineCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szSaleOfflineAmt, szTotalAmt, 12);
				
					glIssuerTotal[ucIssIndex].uiSaleOfflineCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szSaleOfflineAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiSaleOfflineCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szSaleOfflineAmt, szTotalAmt, 12);
					
				} else {
					glEdcTotal.uiSaleOnlineCnt++;
					SafeAscAdd(glEdcTotal.szSaleOnlineAmt, szTotalAmt, 12);

					glAcqTotal[ucAcqIndex].uiSaleOnlineCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szSaleOnlineAmt, szTotalAmt, 12);
				
					glIssuerTotal[ucIssIndex].uiSaleOnlineCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szSaleOnlineAmt, szTotalAmt, 12);

					glCardTotal[ucBinIndex].uiSaleOnlineCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szSaleOnlineAmt, szTotalAmt, 12);
				}
#endif

				if( !ChkIfZeroAmt(stLog.szTipAmount) )
				{
					glEdcTotal.uiTipCnt++;
					SafeAscAdd(glEdcTotal.szTipAmt, stLog.szTipAmount, 12);

					glAcqTotal[ucAcqIndex].uiTipCnt++;
					SafeAscAdd(glAcqTotal[ucAcqIndex].szTipAmt, stLog.szTipAmount, 12);

					glIssuerTotal[ucIssIndex].uiTipCnt++;
					SafeAscAdd(glIssuerTotal[ucIssIndex].szTipAmt, stLog.szTipAmount, 12);

					glCardTotal[ucBinIndex].uiTipCnt++;
					SafeAscAdd(glCardTotal[ucBinIndex].szTipAmt, szTotalAmt, 12);
				}

			}
		}

		if( ucTranAct& IN_REFUND_TOTAL )
		{
			if( stLog.uiStatus & TS_VOID )
			{
				glEdcTotal.uiVoidRefundCnt++;
				SafeAscAdd(glEdcTotal.szVoidRefundAmt, szTotalAmt, 12);

				glAcqTotal[ucAcqIndex].uiVoidRefundCnt++;
				SafeAscAdd(glAcqTotal[ucAcqIndex].szVoidRefundAmt, szTotalAmt, 12);

				glIssuerTotal[ucIssIndex].uiVoidRefundCnt++;
				SafeAscAdd(glIssuerTotal[ucIssIndex].szVoidRefundAmt, szTotalAmt, 12);

				glCardTotal[ucBinIndex].uiVoidRefundCnt++;
				SafeAscAdd(glCardTotal[ucBinIndex].szVoidRefundAmt, szTotalAmt, 12);
			}
			else
			{
				glEdcTotal.uiRefundCnt++;
				SafeAscAdd(glEdcTotal.szRefundAmt, szTotalAmt, 12);

				glAcqTotal[ucAcqIndex].uiRefundCnt++;
				SafeAscAdd(glAcqTotal[ucAcqIndex].szRefundAmt, szTotalAmt, 12);

				glIssuerTotal[ucIssIndex].uiRefundCnt++;
				SafeAscAdd(glIssuerTotal[ucIssIndex].szRefundAmt, szTotalAmt, 12);

				glCardTotal[ucBinIndex].uiRefundCnt++;
				SafeAscAdd(glCardTotal[ucBinIndex].szRefundAmt, szTotalAmt, 12);
			}
		}
	}

	if( ucAcqKey==ACQ_ALL )
	{
		memcpy(&glTransTotal, &glEdcTotal, sizeof(TOTAL_INFO));
		return;
	}

	for(ucAcqIndex=0; ucAcqIndex<glTlmSysParam.ucAcqNum; ucAcqIndex++)
	{
		if( glTlmSysParam.Acq[ucAcqIndex].acqID==ucAcqKey )
		{
			break;
		}
	}

	memcpy(&glTransTotal, &glAcqTotal[ucAcqIndex], sizeof(TOTAL_INFO));
}

#ifdef ENABLE_EMV
// save last emv status
int SaveEmvStatus(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_EMV_STATUS, 0L, &glEmvStatus, sizeof(EMV_STATUS));
	if( iRet!=0 )
	{
		PubTRACE0("SaveEmvStatus");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}
#endif

#ifdef ENABLE_EMV
// save last emv status
int LoadEmvStatus(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_EMV_STATUS, 0L, &glEmvStatus, sizeof(EMV_STATUS));
	if( iRet!=0 )
	{
		PubTRACE0("LoadEmvStatus");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}
#endif

#ifdef ENABLE_EMV
// save EMV error log message
int SaveEmvErrLog(void)
{
	int			iRet, iLength;
	EMV_ERR_LOG	stErrLog;

	// collect message for log
	memset(&stErrLog, 0, sizeof(EMV_ERR_LOG));
	stErrLog.bValid = TRUE;
	stErrLog.ucAidLen = glProcInfo.stTranLog.ucAidLen;
	memcpy(stErrLog.sAID, glProcInfo.stTranLog.sAID, stErrLog.ucAidLen);
	sprintf((char *)stErrLog.szPAN, "%.19s", glProcInfo.stTranLog.szPan);
	stErrLog.ucPANSeqNo = glProcInfo.stTranLog.bPanSeqOK ? glProcInfo.stTranLog.ucPanSeqNo : 0xFF;
	sprintf((char *)stErrLog.szAmount, "%.12s", glSendPack.szBit4);
	sprintf((char *)stErrLog.szTipAmt, "%.12s", glSendPack.szBit54);
	sprintf((char *)stErrLog.szRspCode, "%.2s", glProcInfo.stTranLog.szRspCode);
	GetDateTime(stErrLog.szDateTime);
	sprintf((char *)stErrLog.szRRN, "%.12s",    glProcInfo.stTranLog.szRRN);
	sprintf((char *)stErrLog.szAuthCode, "%.6s", glProcInfo.stTranLog.szAuthCode);

	EMVGetTLVData(0x95, stErrLog.sTVR, &iLength);
	EMVGetTLVData(0x9B, stErrLog.sTSI, &iLength);

	stErrLog.uiReqICCDataLen = (ushort)PubChar2Long(glSendPack.szBit55, 2, NULL);
	memcpy(stErrLog.sReqICCData, &glSendPack.szBit55[2], stErrLog.uiReqICCDataLen);
	if( memcmp(glSendPack.szBit11, glRecvPack.szBit11, 6)==0 )
	{
		stErrLog.uiRspICCDataLen = (ushort)PubChar2Long(glRecvPack.szBit55, 2, NULL);
		memcpy(stErrLog.sRspICCData, &glRecvPack.szBit55[2], stErrLog.uiRspICCDataLen);
	}
	stErrLog.ulSTAN = glProcInfo.stTranLog.ulSTAN;

	PubASSERT( glSysCtrl.uiErrLogNo<MAX_ERR_LOG );
	if( glSysCtrl.uiErrLogNo>=MAX_ERR_LOG )
	{
		glSysCtrl.uiErrLogNo = 0;
	}
	iRet = PubFileWrite(FILE_ERR_LOG, (long)(glSysCtrl.uiErrLogNo * sizeof(EMV_ERR_LOG)),
						&stErrLog, sizeof(EMV_ERR_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("SaveEmvErrLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	glSysCtrl.uiErrLogNo++;
	if( glSysCtrl.uiErrLogNo>=MAX_ERR_LOG )
	{
		glSysCtrl.uiErrLogNo = 0;
	}
	SaveSysCtrlBase();

	return 0;
}
#endif

#ifdef ENABLE_EMV
// load one error log
int LoadErrLog(ushort uiRecNo, void *pOutErrLog)
{
	int		iRet;

	iRet = PubFileRead(FILE_ERR_LOG, (long)(uiRecNo * sizeof(EMV_ERR_LOG)),
						pOutErrLog, sizeof(EMV_ERR_LOG));
	if( iRet!=0 )
	{
		PubTRACE0("LoadEmvErrLog");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}
#endif

// 同步密码文件到manager和EPS
// Sync password to manager application. usually for HongKong
int SyncPassword(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_PASSWORD, 0L, glSysParam.sPassword, sizeof(glSysParam.sPassword));
	if( iRet!=0 )
	{
		PubTRACE0("SyncPassword");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// Sync password to manager application. usually for HongKong
int SyncTlmPassword(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_PASSWORD, 0L, glTlmSysParam.sPassword, sizeof(glTlmSysParam.sPassword));
	if( iRet!=0 )
	{
		PubTRACE0("SyncPassword");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// for BEA fallback process
int LastRecordIsFallback(void)
{
	int			iRet;
	TRAN_LOG	stLog;

#ifdef	__TELIUM_PARAM__
	if( glSysCtrl.uiLastRecNoList[glCurTlmTmsAcq.ucIndex]>=MAX_TRANLOG )
	{
		return FALSE;
	}

	memset(&stLog, 0, sizeof(TRAN_LOG));
	iRet = LoadTranLog(&stLog, glSysCtrl.uiLastRecNoList[glCurTlmTmsAcq.ucIndex]);
	if( iRet!=0 )
	{
		return FALSE;
	}

#else
	if( glSysCtrl.uiLastRecNoList[glCurAcq.ucIndex]>=MAX_TRANLOG )
	{
		return FALSE;
	}

	memset(&stLog, 0, sizeof(TRAN_LOG));
	iRet = LoadTranLog(&stLog, glSysCtrl.uiLastRecNoList[glCurAcq.ucIndex]);
	if( iRet!=0 )
	{
		return FALSE;
	}
#endif	
	if( (stLog.uiEntryMode & MODE_FALLBACK_SWIPE) ||
		(stLog.uiEntryMode & MODE_FALLBACK_MANUAL) )
	{
		return TRUE;
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////TELIUM////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 1	//irf, 20121128
// SaveTMK
int SaveTMK(uchar *TMK)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TMK, 0L, TMK, 8);
	if( iRet!=0 )
	{
		PubTRACE0("SaveTMK");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// irf
int LoadTMK(uchar *TMK)
{
	int		iRet;

	iRet = PubFileRead(FILE_TMK, 0L, TMK, 8);
	if( iRet!=0 )
	{
		PubTRACE0("LoadTMK()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}



//irf
// SaveTWK
int SaveTWK(uchar *TWK)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TWK, 0L, TWK, 8);
	if( iRet!=0 )
	{
		PubTRACE0("SaveTWK");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}



// irf
int LoadTWK(uchar *TWK)
{
	int		iRet;

	iRet = PubFileRead(FILE_TWK, 0L, TWK, 8);
	if( iRet!=0 )
	{
		PubTRACE0("LoadTWK()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// irf
int CheckFileTMK()
{
	int	fd;
	
	fd = open((char *)FILE_TMK, O_RDWR);
	if (fd<0)
	{
		return FALSE;
	}
	
	return TRUE;
}

int CheckFileTWK()
{
	int	fd;
	
	fd = open((char *)FILE_TWK, O_RDWR);
	if (fd<0)
	{
		return FALSE;
	}
	
	return TRUE;
}

#endif	//irf, 20121128

//FILE_TLM_TMS_PARAM
int LoadTlmTmsParam(void)
{
	int		iRet;
	
	iRet = PubFileRead(FILE_TLM_TMS_PARAM, 0L, &glTlmTmsParam, sizeof(TLM_TMS));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTlmTmsParam()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// 保存系统参数
//FILE_TLM_TMS_PARAM
int SaveTlmTmsParam(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_TMS_PARAM, 0L, &glTlmTmsParam, sizeof(TLM_TMS));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTlmTmsParam");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_REG
int LoadTLMreg(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_TLM_REG, 0L, &glTlmReg, sizeof(TLM_REG));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTLMreg()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_REG
int SaveTLMreg(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_REG, 0L, &glTlmReg, sizeof(TLM_REG));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTLMreg");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_TMS_CON
int LoadTLMTMScon(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_TLM_TMS_CON, 0L, &glTlmTmsCon, sizeof(TLM_APP_TMS));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTMScon()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_REG
int SaveTLMTMScon(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_TMS_CON, 0L, &glTlmTmsCon, sizeof(TLM_APP_TMS));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTMScon");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_TMS_BIN
int LoadTLMbin(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_TLM_TMS_BIN, 0L, &glTlmTmsBin, sizeof(TLM_TMS_BIN));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTLMbin()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_TMS_BIN
int SaveTLMbin(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_TMS_BIN, 0L, &glTlmTmsBin, sizeof(TLM_TMS_BIN));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTLMbin");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_TMS_ISS
int LoadTLMiss(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_TLM_TMS_ISS, 0L, &glTlmTmsIss, sizeof(TLM_TMS_ISS));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTLMiss()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_TMS_ISS
int SaveTLMiss(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_TMS_ISS, 0L, &glTlmTmsIss, sizeof(TLM_TMS_ISS));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTLMiss");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_TMS_ACQ
int LoadTLMacq(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_TLM_TMS_ACQ, 0L, &glTlmTmsAcq, sizeof(TLM_TMS_ACQ));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTLMacq()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_TMS_ACQ
int SaveTLMacq(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_TMS_ACQ, 0L, &glTlmTmsAcq, sizeof(TLM_TMS_ACQ));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTLMacq");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

int CheckFileAID()
{
	int	fd;
	
	fd = open((char *)FILE_TLM_TMS_AID, O_RDWR);
	if (fd<0)
	{
		return FALSE;
	}
	
	return TRUE;
}

int CheckFilePUB()
{
	int	fd;
	
	fd = open((char *)FILE_TLM_TMS_PUB, O_RDWR);
	if (fd<0)
	{
		return FALSE;
	}
	
	return TRUE;
}


//FILE_TLM_TMS_AID
int LoadTLMaid(void)
{
	int		iRet;

	if(!CheckFileAID()) 
		return FALSE;

	iRet = PubFileRead(FILE_TLM_TMS_AID, 0L, &glTlmTmsAid, sizeof(TLM_TMS_AID));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTLMaid()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return TRUE;
}

//FILE_TLM_TMS_ACQ
int SaveTLMaid(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_TMS_AID, 0L, &glTlmTmsAid, sizeof(TLM_TMS_AID));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTLMaid");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_TLM_TMS_PUB
int LoadTLMpub(void)
{
	int		iRet;

	if(!CheckFilePUB()) 
		return FALSE;
	
	iRet = PubFileRead(FILE_TLM_TMS_PUB, 0L, &glTlmTmsPub, sizeof(TLM_TMS_PUB));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTLMpub()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return TRUE;
}

//FILE_TLM_TMS_PUB
int SaveTLMpub(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_TMS_PUB, 0L, &glTlmTmsPub, sizeof(TLM_TMS_PUB));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTLMpub");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

int LoadTLMTMS(void)
{
//	int 	iRet = 0;

	MEM_ZERO(&glTlmTmsCon);
	MEM_ZERO(&glTlmTmsBin);
	MEM_ZERO(&glTlmTmsIss);
	MEM_ZERO(&glTlmTmsAcq);
	MEM_ZERO(&glTlmTmsAid);
	MEM_ZERO(&glTlmTmsPub);
	MEM_ZERO(&glTlmTmsParam);

	LoadTLMTMScon();
	LoadTLMbin();
	LoadTLMiss();
	LoadTLMacq();
	LoadTLMaid();
	LoadTLMpub();

	memcpy(&glTlmTmsParam.TMScon, &glTlmTmsCon, sizeof(TLM_APP_TMS));
	memcpy(&glTlmTmsParam.TMSBin, &glTlmTmsBin, sizeof(TLM_TMS_BIN));
	memcpy(&glTlmTmsParam.TMSIss, &glTlmTmsIss, sizeof(TLM_TMS_ISS));
	memcpy(&glTlmTmsParam.TMSAcq, &glTlmTmsAcq, sizeof(TLM_TMS_ACQ));
	memcpy(&glTlmTmsParam.TMSAid, &glTlmTmsAid, sizeof(TLM_TMS_AID));
	memcpy(&glTlmTmsParam.TMSPub, &glTlmTmsPub, sizeof(TLM_TMS_PUB));

	return 0;
}

int SaveTLMTMS(void)
{
	SaveTLMTMScon();
	SaveTLMbin();
	SaveTLMiss();
	SaveTLMacq();

	return 0;
}


//FILE_TLM_SYS_PARAM
int LoadTlmSysParam(void)
{
	int		iRet;

	iRet = PubFileRead(FILE_TLM_SYS_PARAM, 0L, &glTlmSysParam, sizeof(TLM_SYS_PARAM));
	if( iRet!=0 )
	{
		PubTRACE0("LoadTlmSysParam()");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

//FILE_SYS_PARAM
int SaveTlmSysParam(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_TLM_SYS_PARAM, 0L, &glTlmSysParam, sizeof(TLM_SYS_PARAM));
	if( iRet!=0 )
	{
		PubTRACE0("SaveTlmSysParam");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}


// LoadAcqUploadStatus
int LoadAcqUploadStatus(uchar *acqUploadStatus, uchar ucIndex)
{
	int		iRet;

	iRet = PubFileRead(FILE_SYS_CTRL, OFFSET(SYS_CONTROL, sAcqUploadStatus[ucIndex]),
						&acqUploadStatus, sizeof(uchar));
	if( iRet!=0 )
	{
		PubTRACE0("LoadAcqUploadStatus");
		SysHalt();
		return ERR_FILEOPER;
	}
	return 0;
}


// Save AcqUploadStatus
int SaveAcqUploadStatus(void)
{
	int		iRet;

	iRet = PubFileWrite(FILE_SYS_CTRL,
						OFFSET(SYS_CONTROL, sAcqUploadStatus[glCurTlmTmsAcq.ucIndex]),
						&glSysCtrl.sAcqUploadStatus[glCurTlmTmsAcq.ucIndex],
						sizeof(uchar));
	if( iRet!=0 )
	{
		PubTRACE0("SaveAcqUploadStatus");
		SysHalt();
		return ERR_FILEOPER;
	}

	return 0;
}

// end of file
