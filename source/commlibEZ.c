
// #include "posapi.h"
// #include "applib.h"
// #include "commlib.h"
#include "global.h"

/********************** Internal macros declaration ************************/

#define LEN_WORKBUF			(1024*4)
#define TIMER_TEMPORARY		4       // 短时间的定时器(其他模块可以共用)


/********************** Internal structure declaration *********************/

static uchar ucPortNo = COM1;
static uchar szAttr[] = "38400,8,n,1";


// Open port
int EZProxStart()
{
	uchar	ucRet;

	ucRet = PortOpen(ucPortNo, szAttr);
	if( ucRet!=0 )
	{
		return (ERR_COMM_RS232_BASE | ucRet);
	}

	return 0;
}

//Send Data
int EZProxTxd(uchar *psTxdData, ushort uiDataLen)
{
	uchar	ucRet;

	while( uiDataLen-->0 )
	{
		ucRet = PortSend(ucPortNo, *psTxdData++);
		if( ucRet!=0 )
		{
			return (ERR_COMM_RS232_BASE | ucRet);
		}
	}

	return 0;
}

//Received Data
int EZProxRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen)
{
	uchar   ucRet;
	ushort	uiReadCnt, uiTemp;

	uiReadCnt = uiTemp = 0;
	TimerSet(TIMER_TEMPORARY, (ushort)(uiTimeOutSec*10));
	while( uiReadCnt<uiExpLen )
	{
		if( TimerCheck(TIMER_TEMPORARY)==0 )	
		{
			if( uiReadCnt>0 )
			{
				break;
			}
			return ERR_COMM_TIMEOUT;
		}

		ucRet = PortRecv(ucPortNo, psRxdData, uiTemp);
		if( ucRet==0x00 )
		{	
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
			continue;	
		}
		else
		{	
			return (ERR_COMM_RS232_BASE | ucRet);
		}
	}   

	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiReadCnt;
	}

	return 0;
}

//Close Connection
int EZProxCloseCon(void)
{
	uchar	ucRet;

	ucRet = PortClose(ucPortNo);
	if( ucRet==0 )
	{
		return 0;
	}

	return (ERR_COMM_RS232_BASE | ucRet);
}
// end of file
