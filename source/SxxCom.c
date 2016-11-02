
#include "global.h"
#include "SxxCom.h"

/********************** Internal macros declaration ************************/
#define TCPMAXSENDLEN		10240

/********************** Internal structure declaration *********************/

/********************** Internal functions declaration *********************/
static int SxxTcpConnect(char *pszIP, short sPort, int iTimeout);
static uchar SocketCheck(int sk);

/********************** Internal variables declaration *********************/
static int sg_iSocket;
/********************** external reference declaration *********************/
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

#if defined(SXX_IP_MODULE) || defined(SXX_WIRELESS_MODULE)

// Sxx TCP connection
// Shared by GPRS/CDMA/PPP/LAN/WIFI/...
int SxxTcpConnect(char *pszIP, short sPort, int iTimeoutSec)
{
	int		iRet;
	struct net_sockaddr stServer_addr;

	// Setup socket
	iRet = NetSocket(NET_AF_INET, NET_SOCK_STREAM, 0);
//	if (iRet!=0)
	if (iRet<0)
	{
		return iRet;	
	}
	sg_iSocket = iRet;

	// Bind IP
	iRet = SockAddrSet(&stServer_addr, pszIP, sPort);
	if (iRet!=0)
	{
		return iRet;
	}

	// set connection timeout
	if (iTimeoutSec<3)
	{
		iTimeoutSec = 3;
	}
	iRet = Netioctl(sg_iSocket, CMD_TO_SET, iTimeoutSec*1000);
//	if (iRet!=0)
	if (iRet<0)
	{
		return iRet;
	}

	// Connect to remote IP
	iRet = NetConnect(sg_iSocket, &stServer_addr, sizeof(stServer_addr));
	if (iRet!=0)
	{
		NetCloseSocket(sg_iSocket);
	}

	return iRet;
}

//Sxx TCP/IP send data
// Shared by GPRS/CDMA/PPP/LAN/WIFI/...
int SxxTcpTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec)
{
	int iRet;
	int iSendLen;
	int iSumLen;

	iRet = Netioctl(sg_iSocket, CMD_TO_SET, uiTimeOutSec*1000);
	if (iRet < 0)
	{
		return iRet;
	}

	iSumLen = 0;
	while(1)
	{
		if (uiDataLen > TCPMAXSENDLEN)
		{
			iSendLen = TCPMAXSENDLEN;
			uiDataLen = uiDataLen - TCPMAXSENDLEN;
		}
		else
		{
			iSendLen = uiDataLen;
		}	
		iRet = NetSend(sg_iSocket, psTxdData+iSumLen, iSendLen, 0);
		if (iRet < 0)
		{
			return iRet;
		}
		if (iRet != iSendLen)
		{
			return -1;
		}
		iSumLen = iSumLen + iSendLen;
		if (iSendLen <= TCPMAXSENDLEN)
		{
			break;
		}	
	}
	return 0;
}

//Sxx TCP/IP receive
// Shared by GPRS/CDMA/PPP/LAN/WIFI/...
int SxxTcpRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	int iRet;

	DelayMs(200);

	iRet = 	Netioctl(sg_iSocket, CMD_TO_SET, uiTimeOutSec*1000);
	if (iRet < 0)
	{
		return iRet;
	}

	iRet = NetRecv(sg_iSocket, psRxdData, uiExpLen, 0);
	if (iRet < 0)
	{
		return iRet;
	}
	*puiOutLen = iRet;

	return 0;
}

//Sxx TCP/IP close socket
// Shared by GPRS/CDMA/PPP/LAN/WIFI/...
int SxxTcpOnHook(void)
{
	int iRet;

	iRet = NetCloseSocket(sg_iSocket);
	if (iRet < 0)
	{
		return iRet;
	}
	return 0;
}

uchar SocketCheck(int sk)
{
	int event;
	if(sk<0) return RET_TCPCLOSED;
	
	event = Netioctl(sk, CMD_EVENT_GET, 0);
	if(event<0)
	{
         NetCloseSocket(sk);
         return RET_TCPCLOSED;
	}	
	
	if(event&(SOCK_EVENT_CONN|SOCK_EVENT_WRITE|SOCK_EVENT_READ))
	{
         return RET_TCPOPENED;
	}
	else if(event&(SOCK_EVENT_ERROR))
	{
         NetCloseSocket(sk);
         return RET_TCPCLOSED;
	}

	return RET_TCPOPENING;
}

#endif	 // SXX_WIRELESS_MODULE || SXX_IP_MODULE

#ifdef SXX_IP_MODULE

int SxxDhcpStart(uchar ucForceStart, uchar ucTimeOutSec)
{
	int	iRet;

	if (ucForceStart && (DhcpCheck()==0))
	{
		DhcpStop();
	}

	if (ucForceStart || (DhcpCheck()!=0))
	{
		iRet = DhcpStart();
		if (iRet < 0)
		{
			return iRet;
		}

		TimerSet(TIMER_TEMPORARY, (ushort)(ucTimeOutSec*10));
		while (TimerCheck(TIMER_TEMPORARY)!=0)
		{
			DelayMs(200);
			iRet = DhcpCheck();
			if (iRet==0)
			{
				return 0;
			}
		}

		return iRet;
	}

	return 0;
}

int SxxLANTcpDial(TCPIP_PARA *pstTcpPara)
{
	int		iRet;
	uchar	ucRedoDhcp, ucSecondIP;

	CommOnHook(FALSE);

	if (pstTcpPara->ucDhcp)
	{
		iRet = SxxDhcpStart(FALSE, 30);
		if (iRet!=0)
		{	
			return iRet;
		}
	}

	ucRedoDhcp = FALSE;
	ucSecondIP = FALSE;

TAG_RETRY_IP:

	// Connect to remote IP
	if (ucSecondIP)
	{
		iRet = SxxTcpConnect(pstTcpPara->szRemoteIP_2, (short)atoi(pstTcpPara->szRemotePort_2), 8);
	}
	else
	{
		iRet = SxxTcpConnect(pstTcpPara->szRemoteIP_1, (short)atoi(pstTcpPara->szRemotePort_1), 8);
	}

	if (iRet!=0)
	{
		if (!ucSecondIP)
		{
			if (pstTcpPara->ucDhcp && !ucRedoDhcp)
			{
				// If fail, suspect the DHCP
				iRet = SxxDhcpStart(FALSE, 10);
				if (iRet!=0)
				{
					return iRet;
				}
				ucRedoDhcp = TRUE;
				goto TAG_RETRY_IP;
			}

			if (ChkIfValidIp(pstTcpPara->szRemoteIP_2) && ChkIfValidPort(pstTcpPara->szRemotePort_2) &&
				(strcmp((char *)(pstTcpPara->szRemoteIP_1), (char *)(pstTcpPara->szRemoteIP_2))!=0))
			{
				ucSecondIP = TRUE;
				goto TAG_RETRY_IP;
			}
		}
	}

	return iRet;
}

#endif	// SXX_IP_MODULE

#ifdef SXX_WIRELESS_MODULE

//初始化无线模块
int SXXWlInit(WIRELESS_PARAM *pstWlPara)
{
	int iRet;

	flagNoSIM = FALSE;
	iRet = WlInit(pstWlPara->szSimPin);	
	if (iRet == -212)	//模块已初始化
		iRet = 0;

	else if(iRet == WL_RET_ERR_NOSIM)
		flagNoSIM = TRUE; 	//there is no SIM card inserted

	if(!flagNoSIM)
		SXXWlDispSignal();

	return iRet;
}

//先判断PPP链路是否联通，若没有联通先进行PPP链接，成功后再进行TCP连接；若已联通则直接进行TCP连接
int SXXWlDial(WIRELESS_PARAM *pstWlPara, int iTimeOut, int iAliveInterval, uchar ucPredial)
{
	int		iRet;
	int		iRetryTime;
	uchar	ucSecondIP;

	if(flagNoSIM)
		return WL_RET_ERR_NOSIM;
		
	SXXWlDispSignal();
	
	if (iTimeOut<1)
	{
		iTimeOut = 1;
	}

	// ********** Pre-dial **********
	if (ucPredial)
	{
		iRet = WlPppCheck();
		if ((iRet==0) || (iRet==WL_RET_ERR_DIALING) || (iRet==1))	// ret=1 means module busy
		{
			return 0;
		}

		iRet = WlPppLogin(pstWlPara->szAPN, pstWlPara->szUID, pstWlPara->szPwd, 0xFF, 0, iAliveInterval);
		return 0;
	}

	// ********** Full-dial **********

	// ********** Check PPP connection **********
	TimerSet(TIMER_TEMPORARY, (ushort)(iTimeOut*10));
	while (TimerCheck(TIMER_TEMPORARY)!=0)
	{
		iRet = WlPppCheck();

		if (iRet == 0)
		{
			goto TCPCONNECT;
		}
	}

	// ********** Take PPP dial action **********
	iRetryTime = 2;
	while(iRetryTime--)
	{
		iRet = WlPppLogin(pstWlPara->szAPN, pstWlPara->szUID, pstWlPara->szPwd, 0xFF, iTimeOut*1000, iAliveInterval);
		if (iRet != 0)
		{
			DelayMs(100);
			continue;
		}

		iRet = WlPppCheck();
		if (iRet == 0)
		{
			break;
		}
	}

	if (iRetryTime <= 0 && iRet != 0)
	{
		ScrSetIcon(ICON_PHONE, ONHOOKICON);
		return iRet;
	}

	// ********** Connect IP **********
TCPCONNECT:	
	ScrSetIcon(ICON_PHONE, OFFHOOKICON);
	
	iRet = SocketCheck(sg_iSocket);  //come from R&D, tom
//	ScrPrint(0, 7, ASCII, "tang[SocketCheck(%i)]",iRet); DelayMs(1000);
	if (iRet == RET_TCPOPENED)
	{
		return 0;
	}

	ucSecondIP = FALSE;

_RETRY_SECOND_IP:
	if (ucSecondIP)
	{
		iRet = SxxTcpConnect(pstWlPara->szRemoteIP_2, (short)atoi(pstWlPara->szRemotePort_2), iTimeOut);	//ori iTimeOut = 8
	} 
	else
	{
		iRet = SxxTcpConnect(pstWlPara->szRemoteIP_1, (short)atoi(pstWlPara->szRemotePort_1), iTimeOut);
	}

	if (iRet < 0)
	{
		if (ChkIfValidIp(pstWlPara->szRemoteIP_2) && ChkIfValidPort(pstWlPara->szRemotePort_2) &&
			(strcmp((char *)(pstWlPara->szRemoteIP_1), (char *)(pstWlPara->szRemoteIP_2))!=0))
		{
			ucSecondIP = TRUE;
			goto _RETRY_SECOND_IP;
		}

		return iRet;
	}

	return 0;
}

//发送数据
int SXXWlSend(uchar *psTxdData, ushort usDataLen, ushort uiTimeOutSec)
{
	return SxxTcpTxd(psTxdData, usDataLen, uiTimeOutSec);
}

//接收数据
int SXXWlRecv(uchar *psRxdData, ushort usExpLen, ushort uiTimeOutSec, ushort *pusOutLen)
{
	DelayMs(200);
	return SxxTcpRxd(psRxdData, usExpLen, uiTimeOutSec, pusOutLen);
}

//关闭TCP连接
int SXXWlCloseTcp(void)
{
	int iRet;

	iRet = NetCloseSocket(sg_iSocket);
	if (iRet < 0)
	{
		return iRet;
	}
	return 0;
}

//关闭PPP链路
void SXXWlClosePPP(void)
{
	WlPppLogout(); 
	return;
}

//显示信号强度
void SXXWlDispSignal(void)
{
	uchar	ucLevel;
	int		iRet;
	
	ucLevel = 0;
	iRet = WlGetSignal(&ucLevel);
	if( iRet<RET_OK )
	{
		if(iRet == WL_RET_ERR_POWEROFF)
			WlInit(glTlmSysParam.stTxnCommCfg.stWirlessPara.szSimPin); //logout
			
//		hex_dump_char("Error SXXWlDispSignal", &iRet, sizeof(iRet));
//		hex_dump_char("ucLevel Error SXXWlDispSignal", &ucLevel, sizeof(ucLevel));
		ScrSetIcon(ICON_SIGNAL, CLOSEICON);
//		ScrSetIcon(ICON_PHONE, ONHOOKICON);
		return;
	}

	if(ucLevel==5)
		ucLevel=4;
	
	//hex_dump_char("SXXWlDispSignal", &ucLevel, sizeof(ucLevel));
	ScrSetIcon(ICON_SIGNAL, (uchar)(5-ucLevel));
}


#endif	// SXX_WIRELESS_MODULE

