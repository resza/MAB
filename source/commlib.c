
// #include "posapi.h"
// #include "applib.h"
// #include "commlib.h"
#include "global.h"

/********************** Internal macros declaration ************************/
// for TCP/IP module
#define TCP_PORT_ATTR		"115200,8,n,1"
#define MAX_TCP_RETRY		2
#define TCPIP_V7			7
#define TCPIP_V8			8
#define LAN_CMD_TIMEOUT		300

#define LEN_WORKBUF			(1024*4)
#define TIMER_TEMPORARY		4       // 短时间的定时器(其他模块可以共用)

#define TCPMAXSENDLEN 10240
extern ushort ModemExCommand(uchar *CmdStr, uchar *RespData,
					  ushort *Dlen,ushort Timeout10ms);

/********************** Internal structure declaration *********************/
typedef struct _tagERR_INFO
{
	int		iErrCode;
	uchar	szMsg[16+1];
}ERR_INFO;

/********************** Internal functions declaration *********************/
static int CommInitGprsCdma(COMM_CONFIG *pstCfg);
static int CommInitLAN(COMM_CONFIG *pstCfg);
static int CommInitWifi(COMM_CONFIG *pstCfg);

static int RS232Dial(uchar ucDialMode);
static int RS232Txd(uchar *psTxdData, ushort uiDataLen);
static int RS232RawTxd(uchar *psTxdData, ushort uiDataLen);
static int RS232NacTxd(uchar *psTxdData, ushort uiDataLen);
static int RS232AsyncTxd(uchar *psTxdData, ushort uiDataLen);
static int RS232Rxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int RS232RawRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int RS232NacRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int RS232AsyncRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int RS232OnHook(uchar bReleaseAll);

static int  TcpDial(uchar ucDialMode);
static int  TcpTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec);
static int  TcpRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int  TcpOnHook(uchar bReleaseAll);

#ifdef PXX_COMM
static void SendCommand(uchar *str);
static int  WaitRsp(uchar *psRspData, ushort uiExpLen, ushort uiTimeOut, ushort *puiOutLen);
static int  SearchOK(ushort uiTimeOut, uchar twice);
static int  ExchangeCmd(uchar *cmd, ushort uiTimeOut, uchar twice);
static int  SetLocalIP(uchar *LocalIP);
static int  SetRemoteHost(uchar *RemoteIP, uchar *pszRemotePort);
static int  SetGateway(uchar *GatewayIP);
static int  SetNetMask(uchar *pszNetMask);
static void TcpClose(void);
#endif

// PSTN (Dial) functions
static int PSTNDial(uchar ucDialMode);
static int PSTNTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec);
static int PSTNRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int PSTNSyncRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int PSTNAsyncRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int PSTNOnHook(uchar bReleaseAll);

// GPRS/CDMA functions for Pxx model
#if defined(PXX_COMM)
static int   InitWirelessModule(WIRELESS_PARAM *pstParam);
static int	 WirelessDial(uchar ucDialFlag);
static int   WirelessRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
static int   WirelessTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec);
static int   WirelessClose(uchar bRelease);
static uchar WirelessReset(WIRELESS_PARAM *pstParam);
static uchar DoWirelessDial(WIRELESS_PARAM *pstParam);
static void  DispWirelessSignal(void);
#endif

static void  GetAllErrMsg(int iErrCode, ERR_INFO *pstInfo, COMM_ERR_MSG *pstCommErrMsg);

static void  CalcCRC32(uchar *psData, ushort uiLength, uchar sCRC[4]);
static uchar CalcLRC(uchar *psData, ushort uiLength, uchar ucInit);
static int   Conv2AsyncTxd(uchar *psTxdData, ushort uiDataLen);
static int   UpdateTimer(uchar ucTimerNo, ushort *puiTickCnt);

/********************** Internal variables declaration *********************/
static COMM_CONFIG	sg_stCurCfg = {0xFF};
static uchar		sg_ucTcpVer = TCPIP_V7;
static uchar		sg_sWorkBuf[LEN_WORKBUF+50];

// Shared error messages
static ERR_INFO		sg_stCommErrMsg[] =
{
	{ERR_COMM_INV_PARAM, _T_NOOP("INVALID PARAM")},
	{ERR_COMM_INV_TYPE,  _T_NOOP("INV COMM TYPE")},
	{ERR_COMM_CANCEL,    _T_NOOP("USER CANCEL")},
	{ERR_COMM_TIMEOUT,   _T_NOOP("TIMEOUT")},
	{ERR_COMM_COMERR,    _T_NOOP("COMM ERROR")},
	{ERR_COMM_TOOBIG,    _T_NOOP("DATA TOO BIG")},
	{0, ""},
};

// RS232 error messages
static ERR_INFO		sg_stRS232ErrMsg[] =
{
	{0x01, _T_NOOP("PORT ERROR")},
	{0x02, _T_NOOP("INVALID PORT")},
	{0x03, _T_NOOP("PORT CLOSED")},
	{0x04, _T_NOOP("OVERFLOW")},
	{0x05, _T_NOOP("NO PORT AVAIL")},
	{0xF0, _T_NOOP("NO PORT AVAIL")},
	{0xFE, _T_NOOP("INVALID PARAM")},
	{0xFF, _T_NOOP("TIMEOUT")},
	{0, ""},
};

// TCP/IP error messages for Pxx model
static ERR_INFO		sg_stTCPErrMsg[] =
{
	{ERR_COMM_TCPIP_OPENPORT, _T_NOOP("OPEN PORT ERR")},
	{ERR_COMM_TCPIP_SETLIP,   _T_NOOP("LOCAL IP ERR")},
	{ERR_COMM_TCPIP_SETRIP,   _T_NOOP("REMOTE IP ERR")},
	{ERR_COMM_TCPIP_SETRPORT, _T_NOOP("SET PORT ERR")},
	{ERR_COMM_TCPIP_CONN,     _T_NOOP("TCP CONN ERR")},
	{ERR_COMM_TCPIP_TXD,      _T_NOOP("SEND DATA ERR")},
	{ERR_COMM_TCPIP_RXD,      _T_NOOP("RECV DATA ERR")},
	{ERR_COMM_TCPIP_SETGW,    _T_NOOP("SET GW ERR")},
	{ERR_COMM_TCPIP_SETMASK,  _T_NOOP("SET MASK ERR")},
	{0, ""},
};

// modem error messages
static ERR_INFO	sg_stModemErrMsg[] =
{
	{0x01, _T_NOOP("SEND OVERFLOW")},
	{0x02, _T_NOOP("PHONE OCCUPIED")},
	{0x03, _T_NOOP("NO DIAL TONE")},
	{0x04, _T_NOOP("LINE BREAK")},
	{0x05, _T_NOOP("NO ACK")},
	{0x06, _T_NOOP("IN DIALING")},
	{0x07, _T_NOOP("NOT SUPPORT")},
	{0x08, _T_NOOP("RECV NOT EMPTY")},
	{0x09, _T_NOOP("BUFFER NOT EMPTY")},
	{0x0A, _T_NOOP("IN DIALING")},
	{0x0B, _T_NOOP("LINE READY")},
	{0x0C, _T_NOOP("RECV REJECTED")},
	{0x0D, _T_NOOP("LINE BUSY")},
	{0x0F, _T_NOOP("INVALID TEL NO")},
	{0x33, _T_NOOP("LINE READY ?")},
	{0xE9, _T_NOOP("TONE NOT STOP")},
	{0xF0, _T_NOOP("NOT AVAIL PORT")},
	{0xFD, _T_NOOP("CANCEL BY USER")},
	{0xFE, _T_NOOP("INV DATA LENGTH")},
	{0, ""},
};

// GPRS/CDMA error messages for Pxx model
#if defined(_P80_) || defined(_P90_)
static ERR_INFO	sg_stWirelessErrMsg[] =
{
	{RET_NOBOARD,     _T_NOOP("NO MODULE BOARD")},
	{RET_RSPERR,      _T_NOOP("PLS CHECK SIM")},
	{RET_NORSP,       _T_NOOP("CHECK MODULE")},
	{RET_NOSIM,       _T_NOOP("NO SIM CARD")},
	{RET_NEEDPIN,     _T_NOOP("NEED SIM PIN")},
	{RET_NEEDPUK,     _T_NOOP("NEED SIM PUK")},
	{RET_SIMBLOCKED,  _T_NOOP("SIM LOCKED")},
	{RET_SIMERR,      _T_NOOP("NO SIM/NEED PIN")},
	{RET_PINERR,      _T_NOOP("SIM PIN ERROR")},
	{RET_PARAMERR,    _T_NOOP("ADDR OR PORT ERR")},
	{RET_FORMATERR,   _T_NOOP("FORMAT ERROR")},
	{RET_SNLTOOWEAK,  _T_NOOP("SIGNAL TOO WEAK")},
	{RET_LINKCLOSED,  _T_NOOP("NO CARRIER")},
	{RET_LINKOPENED,  _T_NOOP("LINK OK")},
	{RET_LINKOPENING, _T_NOOP("LINKING")},
	{RET_TCPCLOSED,   _T_NOOP("TCP CLOSED")},
	{RET_TCPOPENED,   _T_NOOP("TCP LINKED")},
	{RET_TCPOPENING,  _T_NOOP("TCP OPENING")},
	{RET_ATTACHING,   _T_NOOP("SEEKING NETWORK")},
	{RET_DETTACHED,   _T_NOOP("DETTACH NETWORK")},
	{0, ""},
};
#endif

#ifdef SXX_WIRELESS_MODULE
static ERR_INFO	sg_stSxxErrMsg[] =
{
	// Wireless error
	{WL_RET_ERR_PORTINUSE,	_T_NOOP("INTERNAL ERR")},	// 模块口被占用
	{WL_RET_ERR_NORSP,		_T_NOOP("INTERNAL ERR")},	// 模块没有应答
	{WL_RET_ERR_RSPERR,		_T_NOOP("INTERNAL ERR")},	// 模块应答错误
	{WL_RET_ERR_PORTNOOPEN,	_T_NOOP("INTERNAL ERR")},	// 模块串口没有打开
	{WL_RET_ERR_NEEDPIN,	_T_NOOP("NEED SIM PIN")},	// 需要PIN码
	{WL_RET_ERR_NEEDPUK,	_T_NOOP("NEED SIM PUK")},	// 需要PUK解PIN码
	{WL_RET_ERR_PARAMER,	_T_NOOP("INVALID CONFIG")},	// 参数错误
	{WL_RET_ERR_ERRPIN,		_T_NOOP("INVALID SIMPIN")},	// 密码错误
	{WL_RET_ERR_NOSIM,		_T_NOOP("NO SIM CARD")},	// 没有插入SIM卡
	{WL_RET_ERR_NOTYPE,		_T_NOOP("INVALID TYPE")},	// 不支持的类型
	{WL_RET_ERR_NOREG,		_T_NOOP("SIM NOT REG.")},	// 网络没有注册
//	{WL_RET_ERR_INIT_ONCE,	_T_NOOP("INITED.")},		// 模块已初始化
	{WL_RET_ERR_LINEOFF,	_T_NOOP("LINE OFF")},		// 连接断开
	{WL_RET_ERR_TIMEOUT,	_T_NOOP("TIMEOUT")},		// 超时
	{WL_RET_ERR_REGING,		_T_NOOP("REGING")},			// 网络注册中
	{WL_RET_ERR_PORTCLOSE,	_T_NOOP("INTERNAL ERR")},	// 关闭串口出错
	{WL_RET_ERR_MODEVER,	_T_NOOP("INTERNAL ERR")},	// 错误的模块版本
//	{WL_RET_ERR_DIALING,	_T_NOOP("INTERNAL ERR")},	// 拨号中  
	{WL_RET_ERR_ONHOOK,		_T_NOOP("INTERNAL ERR")},	// 关机中
	{WL_RET_ERR_PPP_BRK,	_T_NOOP("DISCONNECTED")},	// 发现PPP断开
	{WL_RET_ERR_NOSIG,		_T_NOOP("NO SIGNAL")},		// 网络无信号
	{WL_RET_ERR_POWEROFF,	_T_NOOP("INTERNAL ERR")},	// 模块已下电
	{WL_RET_ERR_BUSY,		_T_NOOP("INTERNAL ERR")},	// 模块忙
	{WL_RET_ERR_OTHER,		_T_NOOP("INTERNAL ERR")},	// 其他未知错误
	// TCPIP stack error
	{NET_ERR_MEM,			_T_NOOP("INTERNAL ERR")},	// 内存不够
	{NET_ERR_BUF,			_T_NOOP("INTERNAL ERR")},	// 缓冲区错误
	{NET_ERR_ABRT,			_T_NOOP("CONN. FAIL")},		// 试图建立连接失败
	{NET_ERR_RST,			_T_NOOP("CONN. CLOSED")},	// 连接被对方复位（收到对方的Reset）
	{NET_ERR_CLSD,			_T_NOOP("CONN. CLOSED")},	// 连接已关闭
	{NET_ERR_CONN,			_T_NOOP("CONN. FAIL")},		// 连接未成功建立
	{NET_ERR_VAL,			_T_NOOP("INVALID PARA")},	// 错误变量
	{NET_ERR_ARG,			_T_NOOP("INVALID PARA")},	// 错误参数
	{NET_ERR_RTE,			_T_NOOP("INTERNAL ERR")},	// 错误路由(route)
	{NET_ERR_USE,			_T_NOOP("INTERNAL ERR")},	// 地址、端口使用中
	{NET_ERR_IF,			_T_NOOP("INTERNAL ERR")},	// 底层硬件错误
//	{NET_ERR_ISCONN										// 连接已建立
	{NET_ERR_TIMEOUT,		_T_NOOP("TIMEOUT")},		// 超时
	{NET_ERR_AGAIN,			_T_NOOP("PLS RETRY")},		// 请求资源不存在，请重试
//	{NET_ERR_EXIST										// 已存在
	{NET_ERR_SYS,			_T_NOOP("UNSUPPORTED")},	// 系统不支持
	{NET_ERR_PASSWD,		_T_NOOP("INVALID PWD")},	// 错误密码
	{NET_ERR_MODEM,			_T_NOOP("DIAL FAIL")},		// 拨号失败
	{NET_ERR_LINKDOWN,		_T_NOOP("LINK DOWN")},		// 数据链路已断开，请重新拨号
	{NET_ERR_LOGOUT,		_T_NOOP("LOG OUT")},		// Logout
	{NET_ERR_PPP,			_T_NOOP("CONN. FAIL")},		// PPP断开
	{NET_ERR_STR,			_T_NOOP("INTERNAL ERR")},	// String Too Long, Illeage
	{NET_ERR_DNS,			_T_NOOP("DNS FAIL")},		// DNS Failure: No such Name
	{NET_ERR_INIT,			_T_NOOP("NOT INITED")},		// No Init
	{0, ""},
};
#endif

/********************** external reference declaration *********************/
extern char *strstr(const char *, const char *);

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// 通讯模块初始化
int CommInitGprsCdma(COMM_CONFIG *pstCfg)
{
#if defined(_P80_) || defined(_P90_) || defined(_S_SERIES_)
	CommSetCfgParam(pstCfg);

  #ifdef SXX_WIRELESS_MODULE
	return SXXWlInit(&pstCfg->stWirlessPara);
  #elif defined(PXX_COMM)
	return InitWirelessModule(&pstCfg->stWirlessPara);
  #else
	return 1;
  #endif
#else
	return 0;
#endif
}

int CommInitLAN(COMM_CONFIG *pstCfg)
{
	int 	iRet;

#ifdef SXX_IP_MODULE
	if (pstCfg->stTcpIpPara.ucDhcp)
	{
		return SxxDhcpStart(FALSE, 30);		// If already get DHCP in manager or other EDC, no need to re-start.
	} else {
		// diki add else for TCP IP STATIC
		iRet = EthSet(pstCfg->stTcpIpPara.szLocalIP, pstCfg->stTcpIpPara.szNetMask, pstCfg->stTcpIpPara.szGatewayIP, pstCfg->stTcpIpPara.szDNSIP);
			if (iRet < 0)
			{
			ScrClrLine(2, 7);
			PubDispString("STATIC IP", 2|DISP_LINE_LEFT);
			PubDispString("FAILED !.",       4|DISP_LINE_LEFT);
			PubBeepErr();
			}
		}
#endif


	return 0;
}

int CommInitWifi(COMM_CONFIG *pstCfg)
{
#ifdef SXX_IP_MODULE
#endif
	return 0;	// !!!!
}

int CommInitModule(COMM_CONFIG *pstCfg)
{
	int		ii, iRet;

	switch(pstCfg->ucCommType)
	{
	case CT_GPRS:
	case CT_CDMA:
		for (ii=0; ii<5; ii++)	//ori 3
		{
			iRet = CommInitGprsCdma(pstCfg);
			if ((iRet==0)||(iRet == WL_RET_ERR_NOSIM))
			{
				break;
			}
		}
		return iRet;

	case CT_TCPIP:
		// 非S系列的LAN不需要开机init
		return CommInitLAN(pstCfg);

	case CT_WIFI:
		// 预留
		//return CommInitWifi_Sxx(pstCfg);

	default:
		return 0;
	}
}

// 设置通讯模块参数
// 这个函数的返回码不分P系列和S系列
int CommSetCfgParam(COMM_CONFIG *pstCfg)
{
	int		iLen;

	if( pstCfg==NULL )
	{
		hex_dump_char("pstCfg NuLL", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));
		return ERR_COMM_INV_PARAM;
	}
// 	if (!ChkIfSupportCommType(pstCfg->ucCommType))
// 	{
// 		return ERR_COMM_INV_TYPE;
// 	}

	switch( pstCfg->ucCommType )
	{
	case CT_RS232:
	case CT_DEMO:
		iLen = strlen((char *)pstCfg->stRS232Para.szAttr);
		if( iLen<10 || iLen>20 )	// 简单检查参数
		{
			return ERR_COMM_INV_PARAM;
		}
		break;

	case CT_MODEM:
		if( pstCfg->stPSTNPara.szTelNo[0]==0 )
		{
			return ERR_COMM_INV_PARAM;
		}
		if( pstCfg->stPSTNPara.ucSendMode!=CM_ASYNC &&
			pstCfg->stPSTNPara.ucSendMode!=CM_SYNC )
		{
		hex_dump_char("pstCfg SendMode", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));
			return ERR_COMM_INV_PARAM;
		}
		break;

	case CT_TCPIP:
	case CT_WIFI:
		if( !ChkIfValidIp(pstCfg->stTcpIpPara.szLocalIP) )
		{
			return ERR_COMM_INV_PARAM;
		}
		if( !ChkIfValidIp(pstCfg->stTcpIpPara.szRemoteIP_1) ||
			!ChkIfValidPort(pstCfg->stTcpIpPara.szRemotePort_1))
		{
			return ERR_COMM_INV_PARAM;
		}
	    break;

	case CT_CDMA:
	case CT_GPRS:
		if( strlen((char *)(pstCfg->stWirlessPara.szAPN))==0 )
		{
			return ERR_COMM_INV_PARAM;
		}
		if( !ChkIfValidIp(pstCfg->stWirlessPara.szRemoteIP_1) ||
			!ChkIfValidPort(pstCfg->stWirlessPara.szRemotePort_1) )
		{
			return ERR_COMM_INV_PARAM;
		}
		break;

	default:
		return ERR_COMM_INV_TYPE;
	}

	memcpy(&sg_stCurCfg, pstCfg, sizeof(COMM_CONFIG));

	return 0;
}

// 检查指定的电话号码是否与已经存储的参数一致
int CommChkIfSameTelNo(uchar *pszTelNo)
{
	if( sg_stCurCfg.ucCommType==0xFF )
	{
		return FALSE;
	}

	if( sg_stCurCfg.ucCommType!=CT_MODEM )
	{
		return TRUE;
	}

	if( strcmp((char *)sg_stCurCfg.stPSTNPara.szTelNo, (char *)pszTelNo)==0 )
	{
		return TRUE;
	}

	return FALSE;
}

void CommSwitchType(uchar ucCommType)
{
	sg_stCurCfg.ucCommType = ucCommType;
}

// 通讯模块拨号(MODEM拨号或者TCP建立TCP连接等)
// "Dial" for all kinds of communication.
// for modem it is dialing, for GPRS/CDMA/WIFI/LAN it is connecting TCPIP
int CommDial(uchar ucDialMode)
{
	int		iRet;

	switch( sg_stCurCfg.ucCommType )
	{
	case CT_RS232:
		iRet = RS232Dial(ucDialMode);
		break;

	case CT_MODEM:
		iRet = PSTNDial(ucDialMode);
		break;

	case CT_TCPIP:
#ifdef _S_SERIES_
		iRet = RouteGetDefault();
		if (iRet!=0)
		{
			iRet = RouteSetDefault(0);
		}
#endif
		iRet = TcpDial(ucDialMode);
	    break;

#if defined(_P80_) || defined(_P90_) || defined(_S_SERIES_)
	case CT_CDMA:
	case CT_GPRS:

  #ifdef SXX_WIRELESS_MODULE
		iRet = SXXWlDial(&sg_stCurCfg.stWirlessPara, 60, 3600, (uchar)(ucDialMode=DM_PREDIAL));
  #elif defined(PXX_COMM)
		iRet = WirelessDial(ucDialMode);
  #else
		return 1;
  #endif
		break;
#endif
		
//	case CT_WIFI:
//		break;

	case CT_DEMO:
		iRet = 0;
		break;

	default:
		iRet = ERR_COMM_INV_TYPE;
	}

	return iRet;
}

// 通讯模块发送数据
// send data for all kinds of communication.
int CommTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec)
{
	int		iRet;

	if( psTxdData==NULL )
	{
	
		hex_dump_char("psTxdData NuLL", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));
		return ERR_COMM_INV_PARAM;
	}

	switch( sg_stCurCfg.ucCommType )
	{
	case CT_RS232:
		iRet = RS232Txd(psTxdData, uiDataLen);
		break;

	case CT_MODEM:
		iRet = PSTNTxd(psTxdData, uiDataLen, uiTimeOutSec);
		break;

	case CT_TCPIP:
		iRet = TcpTxd(psTxdData, uiDataLen, uiTimeOutSec);
		break;

#if defined(_P80_) || defined(_P90_) || defined(_S_SERIES_)
	case CT_CDMA:
	case CT_GPRS:

  #ifdef SXX_WIRELESS_MODULE
		iRet = SXXWlSend(psTxdData, uiDataLen, uiTimeOutSec);
  #elif defined(PXX_COMM)
		iRet = WirelessTxd(psTxdData, uiDataLen, uiTimeOutSec);
  #else
		return 1;
  #endif
		break;
#endif
		
//	case CT_WIFI:
//		break;

	case CT_DEMO:
		iRet = 0;
#ifdef _WIN32
		if (!ChkTerm(_TERMINAL_S90_))
		{
			iRet = RS232Txd(psTxdData, uiDataLen);
			iRet = 0;
		}
#endif
		break;

	default:
		iRet = ERR_COMM_INV_TYPE;
	}

	return iRet;
}

// 通讯模块接收数据
// receive for all kinds of communication.
int CommRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	int		iRet;

	if( psRxdData==NULL )
	{
		return ERR_COMM_INV_PARAM;
	}

	switch( sg_stCurCfg.ucCommType )
	{
	case CT_RS232:
		iRet = RS232Rxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
		break;

	case CT_MODEM:
		iRet = PSTNRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
		break;

	case CT_TCPIP:
	    iRet = TcpRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
		break;

#if defined(_P80_) || defined(_P90_) || defined(_S_SERIES_)
	case CT_CDMA:
	case CT_GPRS:

  #ifdef SXX_WIRELESS_MODULE
		iRet = SXXWlRecv(psRxdData, uiExpLen, (ushort)(uiTimeOutSec), puiOutLen);
  #elif defined(PXX_COMM)
		iRet = WirelessRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
  #else
		return 1;
  #endif
		break;
#endif
		
//	case CT_WIFI:
//		break;

	case CT_DEMO:
		PubWaitKey(2);
		iRet = 0;
		break;

	default:
		iRet = ERR_COMM_INV_TYPE;
	}

	return iRet;
}

// 通讯模块断开链路(MODEM挂机或者TCP断开TCP连接等)
// disconnect
// for modem it is OnHook, for GPRS/CDMA/WIFI/LAN it is closing socket.
int CommOnHook(uchar bReleaseAll)
{
	int		iRet;

	switch( sg_stCurCfg.ucCommType )
	{
	case CT_RS232:
		iRet = RS232OnHook(bReleaseAll);
		break;

	case CT_MODEM:
		iRet = PSTNOnHook(bReleaseAll);
		break;

	case CT_TCPIP:
		iRet = TcpOnHook(bReleaseAll);
		break;

#if defined(_P80_) || defined(_P90_) || defined(_S_SERIES_)
	case CT_CDMA:
	case CT_GPRS:

  #ifdef SXX_WIRELESS_MODULE
		iRet = SXXWlCloseTcp();
		if (bReleaseAll)
		{
			SXXWlClosePPP();
		}
  #elif defined(PXX_COMM)
		iRet = WirelessClose(bReleaseAll);
  #else
		return 1;
  #endif
		break;
#endif

//	case CT_WIFI:
//		break;

	case CT_DEMO:
		iRet = 0;
		break;

	default:
		iRet = ERR_COMM_INV_TYPE;
	}

	return iRet;
}
 
// 获取通讯错误信息
// Retrieve error message for specific error code
void CommGetErrMsg(int iErrCode, COMM_ERR_MSG *pstCommErrMsg)
{
	if( pstCommErrMsg==NULL )
	{
		return;
	}

#if defined(SXX_WIRELESS_MODULE) || defined(SXX_IP_MODULE)
	if ((sg_stCurCfg.ucCommType==CT_GPRS) || (sg_stCurCfg.ucCommType==CT_CDMA) ||
		(sg_stCurCfg.ucCommType==CT_TCPIP) || (sg_stCurCfg.ucCommType==CT_WIFI) )
	{
		sprintf((char *)pstCommErrMsg->szMsg, "COMM ERR:%d", iErrCode);
		GetAllErrMsg(iErrCode, sg_stSxxErrMsg, pstCommErrMsg);
		return;
	}
#endif

	sprintf((char *)pstCommErrMsg->szMsg, "COMM ERR:%04X", iErrCode);

	switch( iErrCode & MASK_COMM_TYPE )
	{
	case ERR_COMM_ALL_BASE:
		GetAllErrMsg(iErrCode, sg_stCommErrMsg, pstCommErrMsg);
		break;

	case ERR_COMM_RS232_BASE:
		GetAllErrMsg(iErrCode, sg_stRS232ErrMsg, pstCommErrMsg);
		break;

	case ERR_COMM_MODEM_BASE:
		GetAllErrMsg(iErrCode, sg_stModemErrMsg, pstCommErrMsg);
	    break;

	case ERR_COMM_TCPIP_BASE:
		GetAllErrMsg(iErrCode, sg_stTCPErrMsg, pstCommErrMsg);
	    break;

#if defined(_P80_) || defined(_P90_)
	case ERR_COMM_WIRELESS_BASE:
		GetAllErrMsg(iErrCode, sg_stWirelessErrMsg, pstCommErrMsg);
		break;
#endif
	}
}

void GetAllErrMsg(int iErrCode, ERR_INFO *pstInfo, COMM_ERR_MSG *pstCommErrMsg)
{
	int		iCnt;

	for(iCnt=0; pstInfo[iCnt].iErrCode!=0; iCnt++)
	{
		if( pstInfo[iCnt].iErrCode==iErrCode ||
			pstInfo[iCnt].iErrCode==(iErrCode & MASK_ERR_CODE) )
		{
			sprintf((char *)pstCommErrMsg->szMsg, "%.16s", pstInfo[iCnt].szMsg);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// RS232 通讯模块
// RS232 module
//////////////////////////////////////////////////////////////////////////
// 串口打开
// Open port
int RS232Dial(uchar ucDialMode)
{
	uchar	ucRet;

	if( ucDialMode==DM_PREDIAL )
	{
		return 0;
	}

	ucRet = PortOpen(sg_stCurCfg.stRS232Para.ucPortNo,
					(void *)sg_stCurCfg.stRS232Para.szAttr);
	if( ucRet!=0 )
	{
		return (ERR_COMM_RS232_BASE | ucRet);
	}

	return 0;
}

// 串口发送数据
// send data in different predefined format
int RS232Txd(uchar *psTxdData, ushort uiDataLen)
{
	int		iRet;

	switch( sg_stCurCfg.stRS232Para.ucSendMode )
	{
	case CM_RAW:
		iRet = RS232RawTxd(psTxdData, uiDataLen);
		break;

	case CM_SYNC:
		iRet = RS232NacTxd(psTxdData, uiDataLen);
		break;

	case CM_ASYNC:
		iRet = RS232AsyncTxd(psTxdData, uiDataLen);
		break;

	default:
		iRet = ERR_COMM_INV_PARAM;
	}

	return iRet;
}

// 串口直接发送
// send raw data
int RS232RawTxd(uchar *psTxdData, ushort uiDataLen)
{
	uchar	ucRet;

	while( uiDataLen-->0 )
	{
		ucRet = PortSend(sg_stCurCfg.stRS232Para.ucPortNo, *psTxdData++);
		if( ucRet!=0 )
		{
			return (ERR_COMM_RS232_BASE | ucRet);
		}
	}

	return 0;
}

// send data in NAC format
// STX+Len1+Len2+strings+ETX+CKS, CKS = Len1 -- ETX (^)
int RS232NacTxd(uchar *psTxdData, ushort uiDataLen)
{
	int		iRet;

	if( uiDataLen>LEN_WORKBUF )
	{
		return ERR_COMM_TOOBIG;
	}

	sg_sWorkBuf[0] = STX;
	sg_sWorkBuf[1] = (uiDataLen/1000)<<4    | (uiDataLen/100)%10;	// convert to BCD
	sg_sWorkBuf[2] = ((uiDataLen/10)%10)<<4 | uiDataLen%10;
	memcpy(&sg_sWorkBuf[3], psTxdData, uiDataLen);
	sg_sWorkBuf[3+uiDataLen]   = ETX;
	sg_sWorkBuf[3+uiDataLen+1] = CalcLRC(psTxdData, uiDataLen, (uchar)(sg_sWorkBuf[1] ^ sg_sWorkBuf[2] ^ ETX));

	iRet = RS232RawTxd(sg_sWorkBuf, (ushort)(uiDataLen+5));	// data
	if( iRet!=0 )
	{
		return iRet;
	}

	return 0;
}

// 串口异步发送
// send in async data format
int RS232AsyncTxd(uchar *psTxdData, ushort uiDataLen)
{
	int		iRet;

	// 转换发送数据到工作缓冲区
	// convert format
	iRet = Conv2AsyncTxd(psTxdData, uiDataLen);
	if( iRet!=0 )
	{
		return iRet;
	}

	return RS232RawTxd(sg_sWorkBuf, (ushort)(uiDataLen+8));
}

// 串口接收
// receive data in different predefined format
int RS232Rxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	int		iRet;

#ifdef _WIN32
	uiTimeOutSec = 5;
	DelayMs(500);
#endif

	switch( sg_stCurCfg.stRS232Para.ucSendMode )
	{
	case CM_RAW:
		iRet = RS232RawRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
		break;

	case CM_SYNC:
		iRet = RS232NacRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
		break;

	case CM_ASYNC:
		iRet = RS232AsyncRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
		break;

	default:
		iRet = ERR_COMM_INV_PARAM;
	}

	return iRet;
}

// 串口直接接收
// receive raw data
int RS232RawRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	uchar   ucRet;
	ushort	uiReadCnt, uiTemp;

	uiReadCnt = uiTemp = 0;
	TimerSet(TIMER_TEMPORARY, (ushort)(uiTimeOutSec*10));
	while( uiReadCnt<uiExpLen )
	{
		if( TimerCheck(TIMER_TEMPORARY)==0 )	// 检查定时器
		{
			if( uiReadCnt>0 )	// 已经读取到数据
			{
				break;
			}
			return ERR_COMM_TIMEOUT;
		}

		ucRet = PortRecv(sg_stCurCfg.stRS232Para.ucPortNo, psRxdData, uiTemp);
		if( ucRet==0x00 )
		{	// 接收成功,继续
			uiTemp = 80;
			psRxdData++;
			uiReadCnt++;
		}
		else if( ucRet==0xFF )
		{
			if( uiReadCnt>0 )
			{
				break;
			}
//			return ERR_COMM_TIMEOUT;		//irf, 20121128
			continue;	//irf, 20121128
		}
		else
		{	// 非超时错误,退出
			return (ERR_COMM_RS232_BASE | ucRet);
		}
	}   // end of while( uiReadCnt<uiExpLen

	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiReadCnt;
	}

	return 0;
}

// receive data in NAC format
// STX+Len1+Len2+strings+ETX+CKS, CKS = Len1 -- ETX (^)
int RS232NacRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	int		iResult;
	uchar	ucRet;
	ushort	uiReadCnt, uiLength;

	if( uiExpLen>LEN_WORKBUF )
	{
		return ERR_COMM_TOOBIG;
	}

	uiReadCnt = uiLength = 0;
	memset(sg_sWorkBuf, 0, sizeof(sg_sWorkBuf));

	TimerSet(TIMER_TEMPORARY, 10);
	while( 1 )
	{
		iResult = UpdateTimer(TIMER_TEMPORARY, &uiTimeOutSec);
		if( iResult!=0 )
		{
			return iResult;
		}

		ucRet = PortRecv(sg_stCurCfg.stRS232Para.ucPortNo, &sg_sWorkBuf[uiReadCnt], 0);
		if( ucRet!=0 )
		{
			if( ucRet==0xFF )
			{
				continue;
			}
			return ERR_COMM_COMERR;
		}
		if( sg_sWorkBuf[0]!=STX )
		{
			continue;
		}

		uiReadCnt++;
		if( uiReadCnt==3 )
		{
			uiLength =  ((sg_sWorkBuf[1]>>4) & 0x0F) * 1000 + (sg_sWorkBuf[1] & 0x0F) * 100 +
						((sg_sWorkBuf[2]>>4) & 0x0F) * 10   + (sg_sWorkBuf[2] & 0x0F);
		}
		if( uiReadCnt==uiLength+5 )
		{	// read data ok, verify it ...
			if( sg_sWorkBuf[uiReadCnt-2]==ETX &&
				CalcLRC(&sg_sWorkBuf[1], (ushort)(uiReadCnt-1), 0)==0 )
			{
				break;
			}
			return ERR_COMM_COMERR;
		}
	}

	memcpy(psRxdData, &sg_sWorkBuf[3], uiLength);
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiLength;
	}

	return 0;
}

// 串口异步接收
// receive data in async format
int RS232AsyncRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	int		iResult;
	uchar	ucRet, sCRC[4];
	ushort	uiLength, uiReadCnt, uiRetryCnt;

	if( uiExpLen>LEN_WORKBUF )
	{
		return ERR_COMM_TOOBIG;
	}

	uiReadCnt = uiLength = uiRetryCnt =0;
	memset(sg_sWorkBuf, 0, sizeof(sg_sWorkBuf));

	TimerSet(TIMER_TEMPORARY, 10);
	while( 1 )
	{
		iResult = UpdateTimer(TIMER_TEMPORARY, &uiTimeOutSec);
		if( iResult!=0 )
		{
			return iResult;
		}

		ucRet = PortRecv(sg_stCurCfg.stRS232Para.ucPortNo, &sg_sWorkBuf[uiReadCnt], 0);
		if( ucRet!=0 )
		{
			if( ucRet==0xFF )
			{
				continue;
			}
			return ERR_COMM_COMERR;
		}
		if( sg_sWorkBuf[0]!=STX )
		{
			continue;
		}

		uiReadCnt++;
		if( uiReadCnt==4 )
		{
			uiLength = (ushort)(sg_sWorkBuf[2]<<8 | sg_sWorkBuf[3]);
		}
		if( uiReadCnt!=uiLength+8 )
		{
			continue;
		}

		// 0xF0 is new ProTims command, erase welcome messages!
		if( sg_sWorkBuf[1]==0xF0 )
		{
			uiReadCnt = uiLength = 0;
			memset(sg_sWorkBuf, 0, sizeof(sg_sWorkBuf));
			continue;
		}

		// verify data
		CalcCRC32(&sg_sWorkBuf[1], (ushort)(uiLength+3), sCRC);
		if( sg_sWorkBuf[1]==0x80 && memcmp(sCRC, &sg_sWorkBuf[uiReadCnt-4], 4)==0 )
		{
			PortSend(sg_stCurCfg.stRS232Para.ucPortNo, ACK);
			break;
		}

		if( ++uiRetryCnt>3 )
		{
			return ERR_COMM_COMERR;
		}
		PortSend(sg_stCurCfg.stRS232Para.ucPortNo, NAK);
		uiReadCnt = uiLength = 0;
		memset(sg_sWorkBuf, 0, sizeof(sg_sWorkBuf));
	}

	memcpy(psRxdData, &sg_sWorkBuf[4], uiLength);
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiLength;
	}

	return 0;
}

// 串口关闭
// Close RS232 port
int RS232OnHook(uchar bReleaseAll)
{
	uchar	ucRet;

	ucRet = PortClose(sg_stCurCfg.stRS232Para.ucPortNo);
	if( ucRet==0 )
	{
		return 0;
	}

	return (ERR_COMM_RS232_BASE | ucRet);
}

//////////////////////////////////////////////////////////////////////////
// TCP 通讯模块
// TCP module
//////////////////////////////////////////////////////////////////////////
// 建立TCP连接
// setup TCP connection
int TcpDial(uchar ucDialMode)
{
#ifdef SXX_IP_MODULE
	if( ucDialMode==DM_PREDIAL )
	{
		return 0;
	}
	return SxxLANTcpDial(&sg_stCurCfg.stTcpIpPara);

#elif defined(PXX_COMM)
	uchar	ucRet, ucSecondIP;
	int		iResult;

	if( ucDialMode==DM_PREDIAL )
	{
		return 0;
	}

	// 打开串口
	PortClose(sg_stCurCfg.stTcpIpPara.ucPortNo);
	ucRet = PortOpen(sg_stCurCfg.stTcpIpPara.ucPortNo, (void *)TCP_PORT_ATTR);
	if( ucRet!=0 )
	{
		return ERR_COMM_TCPIP_OPENPORT;
	}
	TcpClose();

	sg_ucTcpVer = TCPIP_V7;		// don't support "E" command
	iResult = ExchangeCmd((uchar *)"E", LAN_CMD_TIMEOUT, 0);
	if( iResult==0 )
	{
		sg_ucTcpVer = TCPIP_V8;	// support "E" command
	}

	iResult = SetLocalIP(sg_stCurCfg.stTcpIpPara.szLocalIP);
	if( iResult )
	{
		return iResult;
	}

	iResult = SetNetMask(sg_stCurCfg.stTcpIpPara.szNetMask);
	if( iResult )
	{
		return iResult;
	}

	ucSecondIP = FALSE;

TAG_CONNECT:
	if (ucSecondIP)
	{
		iResult = SetRemoteHost(sg_stCurCfg.stTcpIpPara.szRemoteIP_2,
								sg_stCurCfg.stTcpIpPara.szRemotePort_2);
	} 
	else
	{
		iResult = SetRemoteHost(sg_stCurCfg.stTcpIpPara.szRemoteIP_1,
								sg_stCurCfg.stTcpIpPara.szRemotePort_1);
	}
	if( iResult )
	{
		return iResult;
	}

	iResult = SetGateway(sg_stCurCfg.stTcpIpPara.szGatewayIP);
	if( iResult )
	{
		return iResult;
	}
	
	// 连接远端目的
	// connect to remote
	iResult = ExchangeCmd((uchar *)"C", 15000, 1);
	if( iResult )
	{
		if (!ucSecondIP)
		{
			ucSecondIP = TRUE;
			goto TAG_CONNECT;
		}

		return ERR_COMM_TCPIP_CONN;
	}

	return 0;
#else
	return 1;
#endif
}



// 发送数据 非S80 TCP/IP 方式
// send data, for Pxx
int TcpTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec)
{
#ifdef SXX_IP_MODULE
	return SxxTcpTxd(psTxdData, uiDataLen, uiTimeOutSec);

#else
	PortReset(sg_stCurCfg.stTcpIpPara.ucPortNo);

	switch( sg_ucTcpVer ) {
	case TCPIP_V8:
		while( uiDataLen-->0 )
		{
			if( *psTxdData=='\\'  )
			{
				if( PortSend(sg_stCurCfg.stTcpIpPara.ucPortNo, '\\') )
				{
					return ERR_COMM_TCPIP_TXD;
				}
			}
			if( PortSend(sg_stCurCfg.stTcpIpPara.ucPortNo, *psTxdData++) )
			{
				return ERR_COMM_TCPIP_TXD;
			}
		}

		// 去掉SendCommand（此函数中又调用了一次PortReset函数），改用直接发送结束符
		if( PortSend(sg_stCurCfg.stTcpIpPara.ucPortNo, '\\') )
		{			
			return ERR_COMM_TCPIP_TXD;
		}
		
		if( PortSend(sg_stCurCfg.stTcpIpPara.ucPortNo, '\x1C') )
		{			
			return ERR_COMM_TCPIP_TXD;
		}
		//end

		break;

	case TCPIP_V7:
	default:
		while( uiDataLen-->0 )
		{
			if( PortSend(sg_stCurCfg.stTcpIpPara.ucPortNo, *psTxdData++) )
			{
				return ERR_COMM_TCPIP_TXD;
			}
		}
		break;
	}

	return 0;
#endif
}

// 接收数据 非S80 TCP/IP 方式
// receive data, for Pxx
int TcpRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
#ifdef SXX_IP_MODULE
	return SxxTcpRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);

#else
	uchar	ucRet;
	ushort	uiLength;
	ulong	ulTemp;

	ulTemp   = 0;
	uiLength = 0;
	TimerSet(TIMER_TEMPORARY, 10);
	while( uiLength<uiExpLen )
	{
		if( TimerCheck(TIMER_TEMPORARY)==0 )
		{
			if( uiTimeOutSec==0 )
			{
				if( uiLength>0 )		// 已经读取了数据
				{
					break;
				}
				return ERR_COMM_TIMEOUT;
			}
			uiTimeOutSec--;
			if( sg_stCurCfg.pfUpdWaitUI!=NULL )
			{
				(*sg_stCurCfg.pfUpdWaitUI)(uiTimeOutSec);
			}
			TimerSet(TIMER_TEMPORARY, 10);
		}

		ucRet = PortRecv(sg_stCurCfg.stTcpIpPara.ucPortNo, psRxdData, (ushort)ulTemp);
		if( ucRet==0 )
		{
			ulTemp = 80;				// timeout between chars!!!
			psRxdData++;
			uiLength++;
		}
		else if( ucRet==0xFF )
		{
			if( uiLength>0 )		// 已经读取了数据
			{
				break;
			}
		}
		else
		{
			return ERR_COMM_COMERR;
		}
	}
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiLength;
	}

	return 0;
#endif
}

// 断开TCP连接  非S80 TCP/IP 方式
// Close socket, for Pxx
int TcpOnHook(uchar bReleaseAll)
{
#ifdef SXX_IP_MODULE
	return SxxTcpOnHook();

#elif defined(PXX_COMM)
	TcpClose();
	PortClose(sg_stCurCfg.stTcpIpPara.ucPortNo);
	return 0;

#else
	return 1;

#endif
}

#ifdef PXX_COMM
// 发送指令
// send command to LAN module, for Pxx
void SendCommand(uchar *pszCmd)
{
	PortReset(sg_stCurCfg.stTcpIpPara.ucPortNo);
	while( *pszCmd )
	{
		PortSend(sg_stCurCfg.stTcpIpPara.ucPortNo, *pszCmd++);
	}
}

// 接收命令响应
// receive response from LAN module, for Pxx
int WaitRsp(uchar *psRspData, ushort uiExpLen, ushort uiTimeOut, ushort *puiOutLen)
{
	uchar	ucRet;
	ushort	uiLength;

	uiLength = 0;
	while( uiLength<uiExpLen )
	{
		ucRet = PortRecv(sg_stCurCfg.stTcpIpPara.ucPortNo, psRspData, uiTimeOut);
		if( ucRet==0 )
		{
			uiTimeOut = 80;				// timeout between chars!!!
			psRspData++;
			uiLength++;
		}
		else if( ucRet==0xFF )
		{
			if( uiLength>0 )		// 已经读取了数据
			{
				break;
			}
			return ERR_COMM_TIMEOUT;
		}
		else
		{
			return ERR_COMM_COMERR;
		}
	}
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiLength;
	}

	return 0;
}

// 检查OK返回
// search "OK" in response string
int SearchOK(ushort uiTimeOut, uchar bTwice)
{
	int		iRet, iCnt, iMaxTime;
	uchar	szRspBuff[256];
	ushort	uiLength;

	iMaxTime = bTwice ? 2 : 1;
	for(iCnt=0; iCnt<iMaxTime; iCnt++)
	{
		memset(szRspBuff, 0, sizeof(szRspBuff));
		uiLength = sizeof(szRspBuff)-1;
		iRet = WaitRsp(szRspBuff, sizeof(szRspBuff)-1, uiTimeOut, &uiLength);
		if( iRet!=0 )
		{
			return iRet;
		}
		if( strstr((char *)szRspBuff, "OK")!=NULL )
		{
			return 0;
		}
	}

	return ERR_COMM_COMERR;
}

// 执行TCP模块指令
// execute LAN module command
int ExchangeCmd(uchar *pszCmd, ushort uiTimeOut, uchar bTwice)
{
	int		iRet, iCnt;

	iRet = ERR_COMM_COMERR;
	for(iCnt=0; iCnt<MAX_TCP_RETRY; iCnt++)
	{
		SendCommand(pszCmd);
		iRet = SearchOK(uiTimeOut, bTwice);
		if( iRet==0 )
		{
			break;
		}
		if( bTwice )	// "C" 命令只要处理一次
		{
			break;
		}
	}

	return iRet;
}

// 设置本地IP地址
// Set local IP, for Pxx
int SetLocalIP(uchar *pszLocalIP)
{
	int		iRet;
	uchar	szCmd[30];

	if( pszLocalIP==NULL || *pszLocalIP==0 )	// using DHCP
	{
		return 0;
	}
	ExchangeCmd((uchar *)"SDHCP\r", LAN_CMD_TIMEOUT, 0);

	sprintf((char *)szCmd, "L%.20s\r", pszLocalIP);
	iRet = ExchangeCmd(szCmd, LAN_CMD_TIMEOUT, 0);
	if( iRet!=0 )
	{
		return ERR_COMM_TCPIP_SETLIP;
	}

	return 0;
}

// 设置目标主机信息
// set remote host, for Pxx
int SetRemoteHost(uchar *pszRemoteIP, uchar *pszRemotePort)
{
	int		iRet;
	uchar	szCmd[30];

	if( pszRemoteIP==NULL || *pszRemoteIP==0 || pszRemotePort[0]==0 )
	{
		return ERR_COMM_INV_PARAM;
	}

	// 设置远端目的IP地址
	// set IP
	sprintf((char *)szCmd, "R%.20s\r", pszRemoteIP);
	iRet = ExchangeCmd(szCmd, LAN_CMD_TIMEOUT, 0);
	if( iRet!=0 )
	{
		return ERR_COMM_TCPIP_SETRIP;
	}

	// 设置远端目的端口号
	// set port
	sprintf((char *)szCmd, "P%s\r", pszRemotePort);
	iRet = ExchangeCmd(szCmd, LAN_CMD_TIMEOUT, 0);
	if( iRet!=0 )
	{
		return ERR_COMM_TCPIP_SETRPORT;
	}

	return 0;
}

// 设置网关IP地址
// set gateway, for Pxx
int SetGateway(uchar *pszGatewayIP)
{
	int		iRet;
	uchar	szCmd[30];

	if( pszGatewayIP==NULL || *pszGatewayIP==0  )
	{
		return 0;
	}

	sprintf((char *)szCmd, "G%.20s\r", pszGatewayIP);
	iRet = ExchangeCmd(szCmd, LAN_CMD_TIMEOUT, 0);
	if( iRet!=0 )
	{
		return ERR_COMM_TCPIP_SETGW;
	}

	return 0;
}

// 设置子网掩码
// set subnet mask, for Pxx
int SetNetMask(uchar *pszNetMask)
{
	int		iRet;
	uchar	szCmd[30];

	if( pszNetMask==NULL || *pszNetMask==0  )
	{
		return 0;
	}

	sprintf((char *)szCmd, "M%.20s\r", pszNetMask);
	iRet = ExchangeCmd(szCmd, LAN_CMD_TIMEOUT, 0);
	if( iRet!=0 )
	{
		return ERR_COMM_TCPIP_SETMASK;
	}

	return 0;
}

// 断开TCP连接
// disconnect, for Pxx
void TcpClose(void)
{
	uchar ucRet, ucCnt;

	switch( sg_ucTcpVer ){
	case TCPIP_V8:
		SendCommand((uchar *)"\\\x1B");
		break;

	case TCPIP_V7:
	default:
		//如果TCP/IP盒子正处在连接状态,则关闭,否则不作为
		for(ucCnt=0; ucCnt<MAX_TCP_RETRY+1; ucCnt++)
		{
			SendCommand((uchar *)"V");
			ucRet = SearchOK(150, 0);
			if( ucRet==0 )
			{
				break;
			}
			SendCommand((uchar *)"2closeport\r");
			DelayMs(LAN_CMD_TIMEOUT);
		}
		break;
	}
}
#endif 

//////////////////////////////////////////////////////////////////////////
// MODEM 通讯模块
// Modem module
//////////////////////////////////////////////////////////////////////////

// Send AT command before every dial action.
static ushort PSTNSendATCmdBeforeDial(void)
{
#ifndef WIN32
#if !defined(_P60_S1_) && !defined(_P70_S_)
    ushort  usRet;
    uchar   szCmd[128];

    usRet = ModemExCommand("AT-STE=0",NULL,NULL,0);
    if (usRet!=0)
    {
        //PubDebugOutput(NULL, "AT-STE ERR\n", 12, DEVICE_COM1, ASC_MODE);
        return usRet;    
    }
    //PubDebugOutput(NULL, "AT-STE OK\n", 11, DEVICE_COM1, ASC_MODE);
    if (sg_stCurCfg.stPSTNPara.ucSignalLevel > 0)
    {
        sprintf((char *)szCmd, "ATS91=%d", (int)sg_stCurCfg.stPSTNPara.ucSignalLevel);
        usRet = ModemExCommand(szCmd,NULL,NULL,0);
        if (usRet!=0)
        {
            //PubDebugOutput(NULL, "ATS91 ERR\n", 11, DEVICE_COM1, ASC_MODE);
            return usRet;    
        }
        //PubDebugOutput(NULL, "ATS91 OK\n", 10, DEVICE_COM1, ASC_MODE);
    }
#endif
#endif

    return 0;
}
// modem 拨号
// modem dial
int PSTNDial(uchar ucDialMode)
{
#ifndef _P90_
	uchar	ucRet;

	// 处理预拨号
	// predial
	if( ucDialMode==DM_PREDIAL )
	{
		OnHook();
        	PSTNSendATCmdBeforeDial();
		ucRet = ModemDial(&sg_stCurCfg.stPSTNPara.stPara,
			sg_stCurCfg.stPSTNPara.szTelNo, 0);
		if( ucRet!=0 )
		{
			return (ERR_COMM_MODEM_BASE | ucRet);
		}
		return 0;
	}

	// 检查预拨号处理结果
	while( 1 )
	{
		ucRet = ModemCheck();
		if( ucRet==0x00 )
		{
			return 0;
		}
		if( ucRet!=0x0A )
		{
			break;
		}
	}

	// 重新拨号
	// redial here.
	OnHook();
    	PSTNSendATCmdBeforeDial();	
	ucRet = ModemDial(&sg_stCurCfg.stPSTNPara.stPara,
					sg_stCurCfg.stPSTNPara.szTelNo, 1);
	if( ucRet==0x00 )
	{
		if( sg_stCurCfg.stPSTNPara.ucSendMode==CM_ASYNC )
		{
			DelayMs(6000);
		}
		return 0;
	}

	return (ERR_COMM_MODEM_BASE | ucRet);
#else
		hex_dump_char("PSTNDial", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));
	return ERR_COMM_INV_PARAM;
#endif
}

// Modem 发送
// modem send data
int PSTNTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec)
{
#ifndef _P90_
	int		iResult;
	uchar	ucRet;

	switch( sg_stCurCfg.stPSTNPara.ucSendMode )
	{
	case CM_ASYNC:
		iResult = Conv2AsyncTxd(psTxdData, uiDataLen);
		if( iResult!=0 )
		{
			return iResult;
		}
		ucRet = ModemTxd(sg_sWorkBuf, (ushort)(uiDataLen+8));
		break;

	case CM_SYNC:
		ucRet = ModemTxd(psTxdData, uiDataLen);
		break;

	default:
		hex_dump_char("PSTNTxd", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));
		return ERR_COMM_INV_PARAM;
	}
	if( ucRet!=0 )
	{
		return (ERR_COMM_MODEM_BASE | ucRet);
	}

	return 0;
#else
		hex_dump_char("p90", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));
	return ERR_COMM_INV_PARAM;
#endif
}

// Modem接收
// Modem receive data
int PSTNRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	int		iRet;

	switch( sg_stCurCfg.stPSTNPara.ucSendMode )
	{
	case CM_ASYNC:
		iRet = PSTNAsyncRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
		break;

	case CM_SYNC:
		iRet = PSTNSyncRxd(psRxdData, uiExpLen, uiTimeOutSec, puiOutLen);
		break;

	default:
		hex_dump_char("PSTNRxd", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));
		iRet = ERR_COMM_INV_PARAM;
	}

	return iRet;
}

// Modem同步接收
// Modem sync receive
int PSTNSyncRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
#ifndef _P90_
	int		iResult;
	uchar	ucRet;
	ushort	uiLength;

	TimerSet(TIMER_TEMPORARY, 10);
	while( 1 )
	{
		iResult = UpdateTimer(TIMER_TEMPORARY, &uiTimeOutSec);
		if( iResult!=0 )
		{
			return iResult;
		}

		ucRet = ModemRxd(psRxdData, &uiLength);
		if( ucRet!=0x0C )
		{
			break;
		}
	}
	if( ucRet!=0 )
	{
		return (ERR_COMM_MODEM_BASE | ucRet);
	}
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiLength;
	}

	return 0;
#else
		hex_dump_char("PSTNSyncRxd", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));
	return ERR_COMM_INV_PARAM;
#endif
}

// Modem异步接收
// Modem async receive
int PSTNAsyncRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
#ifndef _P90_
	int		iResult;
	uchar	ucRet, sCRC[4];
	ushort	uiLength, uiReadCnt, uiRetryCnt;

	if( uiExpLen>LEN_WORKBUF )
	{
		return ERR_COMM_TOOBIG;
	}

	uiReadCnt = uiLength = uiRetryCnt =0;
	memset(sg_sWorkBuf, 0, sizeof(sg_sWorkBuf));

	TimerSet(TIMER_TEMPORARY, 10);
	while( 1 )
	{
		iResult = UpdateTimer(TIMER_TEMPORARY, &uiTimeOutSec);
		if( iResult!=0 )
		{
			return iResult;
		}

		ucRet = ModemAsyncGet(&sg_sWorkBuf[uiReadCnt]);
		if( ucRet!=0 )
		{
			continue;
		}
		if( sg_sWorkBuf[0]!=STX )
		{
			continue;
		}

		uiReadCnt++;
		if( uiReadCnt==4 )
		{
			uiLength = (ushort)(sg_sWorkBuf[2]<<8 | sg_sWorkBuf[3]);
		}
		if( uiReadCnt!=uiLength+8 )
		{
			continue;
		}

		// 0xF0 is new ProTims command, erase welcome messages!
		if( sg_sWorkBuf[1]==0xF0 )
		{
			uiReadCnt = uiLength = 0;
			memset(sg_sWorkBuf, 0, sizeof(sg_sWorkBuf));
			continue;
		}

		// verify data
		CalcCRC32(&sg_sWorkBuf[1], (ushort)(uiLength+3), sCRC);
		if( sg_sWorkBuf[1]==0x80 && memcmp(sCRC, &sg_sWorkBuf[uiReadCnt-4], 4)==0 )
		{
			ModemTxd((uchar *)"\x06", 1);	// ACK
			break;
		}

		if( ++uiRetryCnt>3 )
		{
			return ERR_COMM_COMERR;
		}
		ModemTxd((uchar *)"\x15", 1);		// NAK
		uiReadCnt = uiLength = 0;
		memset(sg_sWorkBuf, 0, sizeof(sg_sWorkBuf));
	}

	memcpy(psRxdData, &sg_sWorkBuf[4], uiLength);
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiLength;
	}

	return 0;
#else
	
	hex_dump_char("PSTNAsyncRxd", "ERR_COMM_INV_PARAM", strlen("ERR_COMM_INV_PARAM"));

	return ERR_COMM_INV_PARAM;
#endif
}

// Modem 挂机
// Modem onhook
int PSTNOnHook(uchar bReleaseAll)
{
	uchar	ucRet, ucCnt;

	for(ucCnt=0; ucCnt<3; ucCnt++)
	{
		ucRet = OnHook();
		if( ucRet==0 )
		{
			return 0;
		}
		DelayMs(50);
	}

	return (ERR_COMM_MODEM_BASE | ucRet);
}


//////////////////////////////////////////////////////////////////////////
// GPRS/CDMA 通讯模块
// GPRS/CDMA module
//////////////////////////////////////////////////////////////////////////

#define WIRELESS_ERR(a)		(ERR_COMM_WIRELESS_BASE|(a))

#ifdef PXX_COMM

// Pxx only
int InitWirelessModule(WIRELESS_PARAM *pstParam)
{
	uchar   ucRet;
	
	ucRet = WNetInit();
	if( ucRet!=RET_OK )
	{
		return WIRELESS_ERR(ucRet);
	}
	
	// check PIN of SIM card
	ucRet = WNetCheckSim();
	switch( ucRet )
	{
	case RET_OK:
		break;
		
	case RET_NEEDPIN:
	case RET_NEEDPUK:
		if( pstParam->szSimPin[0]!=0 )
		{
			ucRet = WNetSimPin(pstParam->szSimPin);
			if( ucRet!=RET_OK )
			{
				return WIRELESS_ERR(ucRet);
			}
		}
		break;
		
	default:
		return WIRELESS_ERR(ucRet);
	}
	
	// set ISP UID & Password
	ucRet = WNetUidPwd(pstParam->szUID, pstParam->szPwd);
	if( ucRet!=RET_OK )
	{
		return WIRELESS_ERR(ucRet);
	}
	DispWirelessSignal();
	
	return 0;
}

// Pxx only
int WirelessDial(uchar ucDialFlag)
{
	uchar			ucRetryConnect, ucSecondIP, ucRet;
	uchar			sBuff[64];
	WIRELESS_PARAM	*pstParam; 

	pstParam = &sg_stCurCfg.stWirlessPara;

	// Pre-dial/ pre-connect
	if( ucDialFlag==0 )
	{
#ifdef _WIRELESS_PRE_DIAL
		if( WNetLinkCheck()==RET_LINKOPENED )
		{
			return 0;
		}
		// pre-dial
		ucRet = DoWirelessDial(pstParam);
		if( ucRet!=RET_OK )
		{
			return WIRELESS_ERR(ucRet);
		}
#endif
		return 0;
	}

	ucSecondIP     = FALSE;
	ucRetryConnect = FALSE;

	// Check last status
	TimerSet(TIMER_TEMPORARY, 15*10);
	while (1)
	{
		ucRet = WNetLinkCheck();
		if (ucRet==RET_LINKOPENED)
		{
			goto TAG_CONNECT_TCP;
		}
		if (TimerCheck(TIMER_TEMPORARY)==0)
		{
			return WIRELESS_ERR(ucRet);
		}
		if (ucRet==RET_LINKOPENING)
		{
			continue;
		}

		break;
	}

TAG_PPP_CONNECT:
	// to resolve PPP detached issue happened in Vietnam
	ucRet = WNetLinkCheck();
	if (ucRet==RET_DETTACHED)
	{
		WNetSendCmd("AT+CGATT=1\r", sBuff, sizeof(sBuff), 10*1000);
	}

	// PPP link
	DoWirelessDial(pstParam);
	TimerSet(TIMER_TEMPORARY, 15*10);
	while (1)
	{
		ucRet = WNetLinkCheck();
		if (ucRet==RET_LINKOPENED)
		{
			goto TAG_CONNECT_TCP;
		}
		if (TimerCheck(TIMER_TEMPORARY)==0)
		{
			return WIRELESS_ERR(ucRet);
		}
		if ((ucRet!=RET_LINKOPENING) && (ucRet!=RET_LINKCLOSED) && (ucRet!=RET_NORSP))
		{
			return WIRELESS_ERR(ucRet);
		}
	}

TAG_CONNECT_TCP:
	ucRet = WNetTcpConnect(pstParam->szRemoteIP_1, pstParam->szRemotePort_1);
	if( ucRet!=RET_TCPOPENED )
	{
		if (!ucRetryConnect)
		{
			ucRetryConnect = TRUE;
			WNetClose();
			DelayMs(3000);
			goto TAG_PPP_CONNECT;
		}
		ucRetryConnect = FALSE;

		if (!ucSecondIP &&
			ChkIfValidIp(pstParam->szRemoteIP_2) && ChkIfValidPort(pstParam->szRemotePort_2) &&
			(strcmp((char *)(pstParam->szRemoteIP_1), (char *)(pstParam->szRemoteIP_2))!=0))
		{
			ucSecondIP = TRUE;
			WNetClose();
			DelayMs(3000);
			goto TAG_PPP_CONNECT;
		}
	}

	if ((ucRet!=RET_TCPOPENED) && (ucRet!=RET_OK))
	{
		return WIRELESS_ERR(ucRet);
	}

	return 0;
}

// Pxx only
uchar WirelessReset(WIRELESS_PARAM *pstParam)
{
    uchar   ucRet;
	
    ucRet = WNetReset();
	if( ucRet!=RET_OK )
	{
		return ucRet;
	}
	DelayMs(6500);
	
	return InitWirelessModule(pstParam);
}

// Pxx only
uchar DoWirelessDial(WIRELESS_PARAM *pstParam)
{
	DispWirelessSignal();
	TimerSet(TIMER_TEMPORARY, 100);
	return WNetDial(pstParam->szAPN, NULL, NULL);
}

//无线通讯接收
// wireless rxd for Pxx
int WirelessRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	uchar	ucRet;

	ucRet = WNetTcpRxd(psRxdData, puiOutLen, (ushort)(uiTimeOutSec*1000));
	if( ucRet!=RET_OK )
	{
		return WIRELESS_ERR(ucRet);
	}

	return 0;
}

// 无线通讯发送
// Wireless send, for Pxx
int WirelessTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec)
{
	uchar   ucRet, *psTemp;
	ushort	uiCnt, uiPackLen;
	
	psTemp    = psTxdData;
	uiPackLen = 1024;
	for(uiCnt=0; uiCnt<uiDataLen; uiCnt+=uiPackLen)
	{
		ucRet = WNetTcpTxd(psTemp+uiCnt, (ushort)(uiCnt+uiPackLen<=uiDataLen ? uiPackLen : uiDataLen-uiCnt));
		if( ucRet!=RET_OK )
		{
			return WIRELESS_ERR(ucRet);
		}
	}

	return 0;
}

// 无线模块挂机
// Wireless close TCP, or also PPP
int WirelessClose(uchar bReleaseAll)
{
	if( WNetTcpCheck()==RET_TCPOPENED )
	{
		WNetTcpClose();
	}
	if( bReleaseAll && WNetLinkCheck()==RET_LINKOPENED )
	{
		WNetClose();
	}
	
	return 0;
}

// 显示信号强度
// signal level display. Pxx only
void DispWirelessSignal(void)
{
	uchar	ucRet, ucLevel;
	
	if( sg_stCurCfg.ucCommType!=CT_GPRS && sg_stCurCfg.ucCommType!=CT_CDMA )
	{
		return;
	}
	
	ucRet = WNetCheckSignal(&ucLevel);
	if( ucRet!=RET_OK )
	{
		ScrSetIcon(ICON_SIGNAL, CLOSEICON);
		return;
	}
	
	ScrSetIcon(ICON_SIGNAL, (uchar)(5-ucLevel));
}

#endif	// if not defined SXX_WIRELESS_MODULE


//////////////////////////////////////////////////////////////////////////
// 公共数据转换模块
// Shared functions for data conversion
//////////////////////////////////////////////////////////////////////////
// 计算CRC32
// Calculate CRC32
void CalcCRC32(uchar *psData, ushort uiLength, uchar sCRC[4])
{
	ulong	ulRSL, tl;
	ushort	ii;
	uchar	ucTemp, k;

	ulRSL = 0xFFFFFFFFL;
	for(ii=0; ii<uiLength; ii++)
	{
		ucTemp = (uchar)ulRSL;
		ucTemp = ucTemp^psData[ii];
		tl = (ulong)ucTemp;
		for(k=0; k<8; k++)
		{
			if( tl&1 )
			{
				tl = 0xedb88320L^(tl>>1);
			}
			else
			{
				tl = tl>>1;
			}
		}
		ulRSL = tl^(ulRSL>>8);
	}
	ulRSL ^= 0xFFFFFFFFL;

	sCRC[0] = (uchar)(ulRSL>>24);
	sCRC[1] = (uchar)(ulRSL>>16);
	sCRC[2] = (uchar)(ulRSL>>8);
	sCRC[3] = (uchar)(ulRSL);
}

// 计算LRC
// Calculate LRC
uchar CalcLRC(uchar *psData, ushort uiLength, uchar ucInit)
{
	while( uiLength>0 )
	{
		ucInit ^= *psData++;
		uiLength--;
	}

	return ucInit;
}

// 转换为异步发送数据格式,并存储在sg_sWorkBuf中
// Convert to async format, then store in sg_sWorkBuf
int Conv2AsyncTxd(uchar *psTxdData, ushort uiDataLen)
{
	if( uiDataLen>LEN_WORKBUF )
	{
		return ERR_COMM_TOOBIG;
	}

	sg_sWorkBuf[0] = STX;
	sg_sWorkBuf[1] = 0x80;
	sg_sWorkBuf[2] = (uchar)(uiDataLen>>8);
	sg_sWorkBuf[3] = (uchar)(uiDataLen & 0xFF);
	memcpy(&sg_sWorkBuf[4], psTxdData, uiDataLen);
	CalcCRC32(&sg_sWorkBuf[1], (ushort)(uiDataLen+3), &sg_sWorkBuf[4+uiDataLen]);

	return 0;
}

int UpdateTimer(uchar ucTimerNo, ushort *puiTickCnt)
{
	if( TimerCheck(ucTimerNo)==0 )
	{
		if( *puiTickCnt==0 )
		{
			return ERR_COMM_TIMEOUT;
		}

		(*puiTickCnt)--;
		if( sg_stCurCfg.pfUpdWaitUI!=NULL )
		{
			(*sg_stCurCfg.pfUpdWaitUI)(*puiTickCnt);
		}
		TimerSet(ucTimerNo, 10);
	}

	return 0;
}



void RefreshGPRS(void)
{
	int iRet;
	
	if( glTlmSysParam.stTxnCommCfg.ucCommType==CT_GPRS  ||	glTlmSysParam.stTxnCommCfg.ucCommType==CT_CDMA )
	{
		if(!flagNoSIM)
		{
			SXXWlDispSignal();
			iRet = WlPppCheck();
			if(iRet<0)
			{
				ScrSetIcon(ICON_PHONE, ONHOOKICON);
//				hex_dump_char("WlPppCheck", &iRet, sizeof(iRet));
				//WlAutoStart(unsigned char * pin, unsigned char * APNIn, unsigned char * UidIn, unsigned char * PwdIn, long Auth, int TimeOut, int AliveInterval);
				if(iRet = WlPppLogin(glTlmSysParam.stTxnCommCfg.stWirlessPara.szAPN, 
					glTlmSysParam.stTxnCommCfg.stWirlessPara.szUID, 
					glTlmSysParam.stTxnCommCfg.stWirlessPara.szPwd, 
						0xFF, 0, 0)<0)
				{
//						hex_dump_char("WlAutoStart", &iRet, sizeof(iRet));
				}		
				/*
				if(iRet = WlAutoStart(glTlmSysParam.stTxnCommCfg.stWirlessPara.szSimPin, 
					glTlmSysParam.stTxnCommCfg.stWirlessPara.szAPN, 
					glTlmSysParam.stTxnCommCfg.stWirlessPara.szUID, 
					glTlmSysParam.stTxnCommCfg.stWirlessPara.szPwd, 
						0xFF, 0, 0)<0)
				{
//						hex_dump_char("WlAutoStart", &iRet, sizeof(iRet));
				}
				*/
			}
			iRet = WlPppCheck();
			if(iRet==0)
				ScrSetIcon(ICON_PHONE, OFFHOOKICON);
			else
				ScrSetIcon(ICON_PHONE, ONHOOKICON);
		}
	}
}


// end of file
