
#include "global.h"

/********************** Internal macros declaration ************************/
#define TIMER_TEMPORARY		4
#define TIMERCNT_MAX	48000

/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
static int  main_sub(ST_EVENT_MSG *pstEventMsg);
static int  CheckTermSatus(ST_EVENT_MSG *pstEventMsg);
static void FirstRunProc(ST_EVENT_MSG *pstEventMsg);
static void SetIdleTimer(void);
static uchar ChkIdleTimer(uchar ucMinutes);
static void ShowIdleMainUI(uchar *pbForceUpdate);
static int  CheckInitTerminal(void);
static void MainIdleProc(void);

/********************** Internal variables declaration *********************/

/********************** external reference declaration *********************/
extern int event_main(ST_EVENT_MSG *pstEventMsg);

const APPINFO AppInfo =
{
	APP_NAME,		//AppName
	EDCAPP_AID,		//AID
	EDC_VER_INTERN,		//AppVer
	"PAX TECHNOLOGY",
	"EDC TEMPLATE FOR VISA/MC PROJECTS",
	"",
	0xFF,
	0xFF,
	0x01,
	""
};

int timeEdcLogOn;
int indeks;
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/


int event_main(ST_EVENT_MSG *pstEventMsg)
{
	glEdcMsgPtr = pstEventMsg;	// 保存事件指针
	return main_sub(pstEventMsg);
}


int main(void)
{
	uchar			bUpdateUI;
	int				iRet;
	ST_EVENT_MSG	stEventMsg;

	uchar SerialInfo[11+1];
	uchar CardType;
	uchar mode;
	uchar CID;

	
#ifdef	__TELIUM_PARAM__	
	nvmStart();	//nvm start
	mapInit();
	traReset();
#endif

	timeEdcLogOn = 0;
	indeks = 0;
	
    // Application initialization
	memset(&stEventMsg, 0, sizeof(stEventMsg));
	stEventMsg.MsgType = USER_MSG;
	iRet = event_main(&stEventMsg);
	if (iRet==ERR_EXIT_APP)
	{
		return 0;
	}
#ifdef BANK_BRI
	CheckMenu();
#endif
    // Main loop
	bUpdateUI = TRUE;
	while( 1 )
	{
		// Setup idle timer 设置空闲计时
		if (bUpdateUI)
		{
			SetIdleTimer();
		}
		ShowIdleMainUI(&bUpdateUI);	// bUpdateUI value may change

		//RefreshGPRS();
		
        // When magstripe card swiped
		if( MagSwiped()==0 )
		{
			memset(&stEventMsg, 0, sizeof(ST_EVENT_MSG));
			stEventMsg.MsgType = MAGCARD_MSG;
			stEventMsg.MagMsg.RetCode = MagRead(stEventMsg.MagMsg.track1,
												stEventMsg.MagMsg.track2,
												stEventMsg.MagMsg.track3);
		
			iRet = event_main(&stEventMsg);
			if (iRet==ERR_EXIT_APP)
			{
				return 0;
			}
			bUpdateUI = TRUE;
			CommOnHook(FALSE);
			continue;
		}

        // When chip card inserted
		if( ChkIfEmvEnable() && IccDetect(ICC_USER)==0 )
		{
			memset(&stEventMsg, 0, sizeof(ST_EVENT_MSG));
			stEventMsg.MsgType = ICCARD_MSG;
			iRet = event_main(&stEventMsg);
			if (iRet==ERR_EXIT_APP)
			{
				return 0;
			}
			bUpdateUI = TRUE;
			CommOnHook(FALSE);
			continue;
		}

		// When TAP card detect
		mode = 'A';	
		PiccClose();
		iRet = PiccOpen();
		if(iRet!=0)
		{
		bUpdateUI = TRUE;
		CommOnHook(FALSE);
		continue;
		} else {
		if( PiccDetect(mode, &CardType, SerialInfo, &CID, NULL)==0 )
		{
			memset(&stEventMsg, 0, sizeof(ST_EVENT_MSG));
			stEventMsg.MsgType = TAPCARD_MSG;
			iRet = event_main(&stEventMsg);
			if (iRet==ERR_EXIT_APP)
			{
				return 0;
			}
			bUpdateUI = TRUE;
			CommOnHook(FALSE);
			continue;
		}
			}

		

        // If any key is pressed
		if( PubKeyPressed() )
		{
			memset(&stEventMsg, 0, sizeof(ST_EVENT_MSG));
			stEventMsg.MsgType  = KEYBOARD_MSG;
			stEventMsg.KeyValue = getkey();
			iRet = event_main(&stEventMsg);
			if (iRet==ERR_EXIT_APP)
			{
				return 0;
			}
			bUpdateUI = TRUE;
			CommOnHook(FALSE);
			continue;
		}

		// add more event-handlers here...
		// 仿照上面，在这里添加更多事件判断和处理函数

		// One of idle-processing 空闲事件处理
#ifdef	__TELIUM_PARAM__
		if(glTlmSysParam.stEdcInfo.ucIdleMinute!=0)	//added by irf, 20121203
		{
			if (ChkIdleTimer(glTlmSysParam.stEdcInfo.ucIdleMinute))
			{
				MainIdleProc();
				bUpdateUI = TRUE;
				continue;
			}
		}

#else
		if(glSysParam.stEdcInfo.ucIdleMinute!=0)	//added by irf, 20121203
		{
			if (ChkIdleTimer(glSysParam.stEdcInfo.ucIdleMinute))
			{
				MainIdleProc();
				bUpdateUI = TRUE;
				continue;
			}
		}
#endif
	}	// end of while( 1
}

// EDC  事件处理函数
int main_sub(ST_EVENT_MSG *pstEventMsg)
{
	int		iRet;
	uchar	ucRet;
	
	ucRet = SystemInit();
	if(ucRet==0)	//check after reset
	{
		debugTx=FALSE;
		debugRx=FALSE;
	}

#ifdef ENABLE_EMV
	iRet = EMVCoreInit();
	if( iRet==EMV_KEY_EXP )
	{
//		hex_dump_char("EMVCoreInit", &iRet, sizeof(iRet));
//		EraseExpireCAPK();	//not erase key
	}
#ifdef __EMV_DEBUG__
			EMVSetDebug(TRUE);
#endif
#endif

#if defined(_P60_S1_) || defined(_S_SERIES_)
	SetOffBase(NULL);
#endif

	CheckTermSatus(pstEventMsg);

#ifndef APP_MANAGER_VER
	// Process manager attached administrative message.
	// Not implemented
#endif

	iRet = ERR_NO_DISP;
	switch( pstEventMsg->MsgType )
	{
	case USER_MSG:		//same like hdlPowerOn in Telium
		ProcUserMsg();
		break;
	
	case EDCAPP_AUTOUPDATE:
		AutoDownParam();
	    break;
	
	case EDCAPP_GETPARA:
		GetAutoDownParam();
	    break;

	case EDC_ECR_MSG:
		iRet = ProcEcrMsg();
		break;
		
	
	case MAGCARD_MSG:
		iRet = ProcMagMsg();
		break;
	
	case ICCARD_MSG:
		iRet = ProcICCMsg();
		PromptRemoveICC();
	    break;

	case TAPCARD_MSG:
		iRet = TransSalePrvtLblTes();
		break;
	
	
	case EPSAPP_FUNCKEYMSG:
		ProcFuncKeyMsg();
	    break;

	case KEYBOARD_MSG:
		iRet = ProcKeyMsg();
		break;

	case APPMSG_SYNC:
		break;

	default:
	    break;
	}
	if( iRet!=0 )
	{
		DispResult(iRet);
	}

#if defined(_P60_S1_) || defined(_S_SERIES_)
	SetOffBase(NULL);
#endif

    	kbflush();
	CheckInitTerminal();
	UnLockTerminal();

	if (iRet==ERR_EXIT_APP)
	{
		return ERR_EXIT_APP;
	}

#ifndef APP_MANAGER_VER
	// Response to manager admin msg
#endif

	return 0;
}

// 读取配置信息、检查终端参数/状态(如,是否下载参数、是否锁定等)、
// 恢复冲正标志等
int CheckTermSatus(ST_EVENT_MSG *pstEventMsg)
{
	FirstRunProc(pstEventMsg);  // 使系统运行参数
	LoadEdcLang();	//set language

	//set backup tms param
#ifdef	__TELIUM_PARAM__
	memcpy(&glTlmSysParamBak, &glTlmSysParam, sizeof(glTlmSysParam));
#else
	memcpy(&glSysParamBak, &glSysParam, sizeof(glSysParam));
#endif
	
#ifndef __TELIUM_PARAM__
	LoadSysDownEdcFile();	// 加载PROTIMS下载的参数文件
#endif

	CheckInitTerminal();	//check already TMS or not
	UnLockTerminal();

	RecoverTranLog();	// must called after system initialization
	RecoverBrizziTranLog();	// must called after system initialization
	RecoverMiniAtmTranLog();	// must called after system initialization
	RecoverCardServicesTranLog();	// must called after system initialization
	RecoverTunaiTranLog();	// must called after system initialization
	RecoverTBankTransLog();	//irf, 02 Jan 2014

	
	InitTransInfo();

#ifndef APP_MANAGER_VER
	// 加载管理器传给子应用的参数同步文件(当应用管理器存在)
	// Not implemented
#endif

	return 0;
}

// 第一次运行时候处理(事实上每次main_sub都会运行)
void FirstRunProc(ST_EVENT_MSG *pstEventMsg)
{
	uchar	szEngTime[16+1];
	uchar	ucNeedUpdateParam;
	uchar	idx;
	uchar 	SerialNo[8+1];
	
	if( ExistSysFiles() )	//pengecekan terhadap file-file mandatory(tms, system, log, dll..) sudah dibuat atau belum
	{
		if (ValidSysFiles())	//pengecekan terhadap kapasitas file-file yang sudah dibuat
		{
#ifdef	__TELIUM_PARAM__
			LoadTlmSysParam();		//TMS PAX

			if(LoadTLMaid())
			{
				SetEmvAid();
				if(LoadTLMpub())
					SetEmvCapk();
			}
			
#else
			LoadSysParam();		//TMS PAX
#endif
			LoadSysCtrlAll();

			ucNeedUpdateParam = FALSE;
			if (pstEventMsg->MsgType==USER_MSG)
			{
				if (UpdateTermInfo() || InitMultiAppInfo())
				{
					ucNeedUpdateParam = TRUE;
				}
			}
#ifdef	__TELIUM_PARAM__
			if( glTlmSysParam.stTxnCommCfg.pfUpdWaitUI!=DispWaitRspStatus )
			{
				glTlmSysParam.stTxnCommCfg.pfUpdWaitUI = DispWaitRspStatus;
				glTlmSysParam.stTMSCommCfg.pfUpdWaitUI = DispWaitRspStatus;
				ucNeedUpdateParam = TRUE;
			}
#else
			if( glSysParam.stTxnCommCfg.pfUpdWaitUI!=DispWaitRspStatus )
			{
				glSysParam.stTxnCommCfg.pfUpdWaitUI = DispWaitRspStatus;
				glSysParam.stTMSCommCfg.pfUpdWaitUI = DispWaitRspStatus;
				ucNeedUpdateParam = TRUE;
			}
#endif
			if (ucNeedUpdateParam)
			{
#ifdef	__TELIUM_PARAM__
				SaveTlmSysParam();	//TMS telium
#else
				SaveSysParam();	//TMS PAX
#endif
			}

#ifdef	__TELIUM_PARAM__
//			mapInit();
			traReset();
#endif			

			return;	//keluar setelah selesai load parameter 
		}
		else
		{
			ScrCls();
			PubDispString(_T("APP AND DATA    "), 0|DISP_LINE_LEFT);
			PubDispString(_T("INCONSIST.      "), 2|DISP_LINE_LEFT);
			PubDispString(_T("RESET CONFIG ?  "), 4|DISP_LINE_LEFT);
			if (!AskYesNo())
			{
				SysHaltInfo("PLS REPLACE APP");
			}

			ScrCls();
			PubDispString(_T("WARNING         "), DISP_LINE_REVER|DISP_LINE_LEFT);
			PubDispString(_T("DATA WILL CLEAR "), 2|DISP_LINE_LEFT);
			PubDispString(_T("CONTINUE ?      "), 4|DISP_LINE_LEFT);
			if (!AskYesNo())
			{
				SysHaltInfo(_T("PLS REPLACE APP"));
			}
			RemoveSysFiles();
		}
	}

	GetEngTime(szEngTime);
	PubShowTitle(TRUE, szEngTime);
	DispWait();
	PubDispString(_T("SYSTEM INIT..."), 6|DISP_LINE_LEFT);

#ifdef __TELIUM_PARAM__
	mapInit();            //setup map tables
	dftReset(keyBeg);           //fill database by default values
//	hex_dump_char("dftReset", "OK", 2);
	mapInit();                  //re-initialise map tables
//	hex_dump_char("mapInit", "OK", 2);
	traReset();
//	hex_dump_char("traReset", "OK", 2);
	MEM_ZERO(SerialNo);

	ReadSN(SerialNo);
	mapPutStr(appSoftName, "v2014");
	mapPutStr(appVersion, "1004");
	mapPutStr(appCtrlVersion, " ");
	mapPutStr(appTid, SerialNo);
#endif

	LoadEdcDefault();	// set EDC default values
	InitTranLogFile();	// Init transaction log file
	InitBrizziTranLogFile();
	InitMiniAtmCSTranLogFile();
	InitTunaiTranLogFile();

	for(idx = 0;idx<5;idx++)
	{
		if(TlmFile[idx].ucName[0]!=0)
		{
			strcpy(glSysCtrl.stTlmFile[idx].ucName, TlmFile[idx].ucName);
			glSysCtrl.stTlmFile[idx].ulSize = TlmFile[idx].ulSize;
			/* 4212
			hex_dump_char(glSysCtrl.stTlmFile[idx].ucName, &glSysCtrl.stTlmFile[idx].ulSize, sizeof(long));
			hex_dump_char(TlmFile[idx].ucName, &TlmFile[idx].ulSize, sizeof(long));
			//*/
		}
	}
SaveSysCtrlAll();

#ifdef ENABLE_EMV
	LoadEmvDefault();	// Init EMV kernel
#endif


}

// 设置空闲计时。设置一个比较长的倒计时，以用于不止一种的空闲事件处理
void SetIdleTimer(void)
{
	TimerSet(TIMER_TEMPORARY, TIMERCNT_MAX);
}

// 检查空闲计时，看是否已经流过了指定的分钟数
uchar ChkIdleTimer(uchar ucMinutes)
{
	int	iCnt = TIMERCNT_MAX-TimerCheck(TIMER_TEMPORARY);
	
	PubASSERT(TIMERCNT_MAX > ucMinutes*60*10);	//	ScrPrint(0,7,ASCII,"%d  ", iCnt/10);
	return (iCnt >= ucMinutes*60*10);
}

// 显示空闲时用户界面
void ShowIdleMainUI(uchar *pbForceUpdate)
{
	static	uchar	szLastTime[5+1] = {"00000"};
	uchar	szCurTime[16+1];
	int 		iRet=0;
	uchar	datetime[14+1];
	uchar	lastdatetime[14+1];
	uchar	TID[8+1];
	long		time;
	
	
	GetEngTime(szCurTime);

	// for mini atm bukopin. diki add
	MenuAtmbMerchantNasabah = FALSE;


	//REFRESH GPRS
	if( *pbForceUpdate || memcmp(szLastTime, &szCurTime[11], 5)!=0 )	
	{
		RefreshGPRS();
	}

	//AUTO LOGON
	if( *pbForceUpdate || memcmp(szLastTime, &szCurTime[11], 5)!=0 )	
	{
		indeks++;
		memset(datetime, 0, sizeof(datetime));
		GetDateTime(datetime);
		memcpy(lastdatetime, glSysCtrl.stTlmApp.appStartTimerWorkingKey, 14);
		if(memcmp(lastdatetime, datetime, 8)!= 0){
			MEM_ZERO(TID);
			SetCurAcq(0);
			memcpy(TID, glCurTlmTmsAcq.acqTID, lenTid);
			time = atol(TID);
			timeEdcLogOn = time % 10000;
			if(indeks == timeEdcLogOn){
//#ifdef BANK_BRI
#ifdef BANK_ARTHA_JASA
				TransLogonAJ();
#endif
				indeks = 0;
			}
			else if(indeks > timeEdcLogOn) 
				indeks = 0;
		}

	}


	if( *pbForceUpdate || memcmp(szLastTime, &szCurTime[11], 4)!=0 )	// Reset magstripe reader every 10 minutes
	{
		MagClose();
		MagOpen();
		MagReset();
	}
	if( *pbForceUpdate || memcmp(szLastTime, &szCurTime[11], 5)!=0 )
	{
		// refresh UI
		sprintf((char *)szLastTime, "%.5s", &szCurTime[11]);
		PubShowTitle(TRUE, szCurTime);
//		PubShowTitleASCII(TRUE, szCurTime);
		DispSwipeCard(TRUE);
		*pbForceUpdate = FALSE;
	}
#ifdef _WIN32
	DelayMs(100);
#endif
}

int CheckInitTerminal(void)
{
	uchar	szCurTime[16+1], szLastTime[16+1];
	uchar	ucKey;
	
#ifdef	__TELIUM_PARAM__
	if( !(glTlmSysParam.ucTermStatus & INIT_MODE) )
	{
		return 0;
	}
#else
	if( !(glSysParam.ucTermStatus & INIT_MODE) )
	{
		return 0;
	}
#endif

	TimerSet(0, 0);
	memset(szCurTime,  0, sizeof(szCurTime));
	memset(szLastTime, 0, sizeof(szLastTime));
	
#ifdef	__TELIUM_PARAM__
	while( glTlmSysParam.ucTermStatus & INIT_MODE )
#else
	while( glSysParam.ucTermStatus & INIT_MODE )
#endif		
	{
		if( TimerCheck(0)==0 )
		{
			TimerSet(0, 10);
			GetEngTime(szCurTime);
			if (strcmp(szCurTime, szLastTime)!=0)
			{
				PubShowTitle(TRUE, szCurTime);
				PubDispString(_T("PLEASE INIT"), 4|DISP_LINE_LEFT);
				ScrPrint(0, 6, CFONT, "[%.14s]", AppInfo.AppName);
				memcpy(szLastTime, szCurTime, sizeof(szLastTime));
			}
		}

		ucKey = PubWaitKey(0);
		if( ucKey==KEYFN )
		{
			InitTransInfo();
			FunctionInit();
			TimerSet(0, 0);
			memset(szLastTime, 0, sizeof(szLastTime));
		}
	}

	return 0;
}

void MainIdleProc(void)
{
#if defined(_S_SERIES_) && !defined(_WIN32)
	// should not let POS go into sleep mode when running simulator
	int		iRet;

	if (ChkTerm(_TERMINAL_S90_))
	{
#ifdef	__TELIUM_PARAM__
		if (glTlmSysParam.stEdcInfo.ucIdleShutdown)
		{
			PowerOff();
		}
#else
		if (glSysParam.stEdcInfo.ucIdleShutdown)
		{
			PowerOff();
		}
#endif
		else
		{
			ScrCls();
			PubDispString(_T("POWER SAVING"), DISP_LINE_LEFT|DISP_LINE_REVER);
			PubDispString(_T("PRESS ANY KEY"), 2|DISP_LINE_LEFT);
			PubDispString(_T("TO RECOVER"),    4|DISP_LINE_LEFT);
			do 
			{
				iRet = SysSleep("00");
			} while((iRet==-3) && (kbhit()!=0));
			// 很奇怪的现象：除非在上次SysSleep返回之后调用DelayMs(3000)，否则即使间隔1分钟，调用SysSleep仍会直接返回-3。
			// 因此我在这里加了判断，如果返回-3而且没有按键则继续重复SysSleep
			// 在外部已经保证了进入MainIdleProc的间隔>=1分钟

			DelayMs(100);
			kbflush();
		}
	}
#endif
}

// end of file

