
/****************************************************************************
NAME
	commlib.h - 实现公共通讯函数

DESCRIPTION
	1. 实现POS应用开发中常见通讯方式的连接的建立、断开、数据收发等功能.
	2. 本模块内部占用定时器4#(进入模块后申请,退出模块后释放)

REFERENCE

MODIFICATION SHEET:
	MODIFIED   (YYYY.MM.DD)
	shengjx     2006.09.05      - created
****************************************************************************/

#ifndef _COMMLIB_H
#define _COMMLIB_H

#define MASK_COMM_TYPE			0xFF00		// 提取错误类别
#define MASK_ERR_CODE			0x00FF		// 提取错误代码

// 公共类型错误
#define ERR_COMM_ALL_BASE		0x0000		// 公共类型错误
#define ERR_COMM_INV_PARAM		0x0001		// 参数错误
#define ERR_COMM_INV_TYPE		0x0002		// 无效通讯类型
#define ERR_COMM_CANCEL			0x0003		// 用户取消
#define ERR_COMM_TIMEOUT		0x0004		// 通讯超时
#define ERR_COMM_COMERR			0x0005
#define	ERR_COMM_TOOBIG			0x0006

#define ERR_COMM_RS232_BASE		0x0100		// RS232类错误

#define ERR_COMM_MODEM_BASE			0x0200		// Modem类错误
#define ERR_COMM_MODEM_OCCUPIED		(ERR_COMM_MODEM_BASE+0x02)
#define ERR_COMM_MODEM_NO_LINE		(ERR_COMM_MODEM_BASE+0x03)
#define ERR_COMM_MODEM_LINE			(ERR_COMM_MODEM_BASE+0x04)
#define ERR_COMM_MODEM_NO_ACK		(ERR_COMM_MODEM_BASE+0x05)
#define ERR_COMM_MODEM_LINE_BUSY	(ERR_COMM_MODEM_BASE+0x0D)
#define ERR_COMM_MODEM_NO_LINE_2	(ERR_COMM_MODEM_BASE+0x33)
#define ERR_COMM_MODEM_NO_PHONE_UP	(ERR_COMM_MODEM_BASE+0x83)

#define ERR_COMM_TCPIP_BASE		0x0300		// TCPIP类错误
#define ERR_COMM_TCPIP_OPENPORT	0x0301		// 打开端口失败
#define ERR_COMM_TCPIP_SETLIP	0x0302
#define ERR_COMM_TCPIP_SETRIP	0x0303
#define ERR_COMM_TCPIP_SETRPORT	0x0304
#define ERR_COMM_TCPIP_CONN		0x0305
#define ERR_COMM_TCPIP_TXD		0x0306
#define ERR_COMM_TCPIP_RXD		0x0307
#define ERR_COMM_TCPIP_SETGW	0x0308
#define ERR_COMM_TCPIP_SETMASK	0x0309

#define ERR_COMM_WIRELESS_BASE		0x0400		// GPRS/CDMA类错误

// 拨号方式
#define DM_PREDIAL			0		// 预拨号/预连接
#define DM_DIAL				1		// 实际拨号/连接

// #define _WIRELESS_PRE_DIAL		// open for pre-connect of wireless module

// 发送数据方式
#define CM_RAW				0		// 原数据发送方式(不添加起止字符,For RS232)
#define CM_SYNC				1		// 同步
#define CM_ASYNC			2		// 异步

// for RS232 communication
#define STX             0x02
#define ETX             0x03
#define ENQ             0x05
#define ACK             0x06
#define NAK             0x15

// RS232配置参数
typedef struct _tagRS232_PARA
{
	uchar	ucPortNo;			// Port #, COM1, COM2 ....
	uchar	ucSendMode;			// 数据发送方式
	uchar	szAttr[20+1];		// 串口打开参数, "9600,8,n,1", ....
}RS232_PARA;

// TCP/IP配置参数
// TCPIP parameter
typedef struct _tagTCPIP_PARA
{
	uchar	ucDhcp;				// use DHCP : 1--TRUE, 0--FALSE
	uchar	ucPortNo;			// TCP/IP module port, COM1, COM2, TCPIP
	uchar	szNetMask[15+1];
	uchar	szGatewayIP[15+1];
	uchar	szLocalIP[15+1];
	uchar	szRemoteIP_1[15+1];
	uchar	szRemotePort_1[5+1];
	uchar	szRemoteIP_2[15+1];
	uchar	szRemotePort_2[5+1];
	uchar	szDNSIP[15+1];
}TCPIP_PARA;

// PSTN配置参数
// PSTN parameter
typedef struct _tagPSTN_PARA
{
	uchar		ucSendMode;		// 数据发送方式
	uchar		szTelNo[100+1];	// 电话号码
	COMM_PARA	stPara;
    uchar       ucSignalLevel;  // dial voltage level, range 1-15. Use 0 for disable.
}PSTN_PARA;

// GPRS/CDMA configurations
typedef struct _tagWIRELESS_PARAM //
{
	uchar		szAPN[64+1];  ///拨号号码 CDMA: #777; GPRS: cmnet
	uchar		szUID[64+1];
	uchar		szPwd[16+1];
	uchar		szSimPin[16+1];     // SIM card PIN
	uchar		szDNS[32+1];
	uchar		szRemoteIP_1[20+1];
	uchar		szRemotePort_1[5+1];
	uchar		szRemoteIP_2[20+1];
	uchar		szRemotePort_2[5+1];
}WIRELESS_PARAM;

// 通讯模块错误信息
typedef struct _tagCOMM_ERR_MSG
{
	uchar		szMsg[16+1];
}COMM_ERR_MSG;


/****************************************************************************
 Function:		接收时更新用户界面(每秒调用一次,RS232 raw方式不支持)
 Param In:
	uiTimeLeft	剩余超时时间,单位为秒
 Param Out:		none
 Return Code:	none
****************************************************************************/
typedef void (*UpdWaitRspUI)(ushort uiTimeLeft);

// 通讯模块配置信息
typedef struct _tagCOMM_CONFIG
{
#define CT_NONE		0			// 不使用
#define CT_RS232	1			// RS232通讯
#define CT_MODEM	2			// Modem通讯
#define CT_TCPIP	3			// TCP/IP通讯
#define	CT_CDMA		4			// CDMA(RFU)
#define CT_GPRS		5			// GPRS(RFU)
#define CT_WIFI		6			// WIFI(RFU)
#define CT_DEMO		0xFF		// 演示/调试/培训使用的离线模式
	uchar			ucCommType;		// 通讯类型(RS232/modem/tcp...)
	uchar			ucCommTypeBak;	// 备选通讯类型(RS232/modem/tcp...)

	UpdWaitRspUI	pfUpdWaitUI;	// 接收时更新用户界面(每秒调用一次,RS232 raw方式不支持)

	uchar			ucTCPClass_BCDHeader;	// For all TCPIP class: the length bytes are in BCD format or not

	RS232_PARA		stRS232Para;	// RS232 配置参数
	PSTN_PARA		stPSTNPara;		// modem 配置参数
	TCPIP_PARA		stTcpIpPara;	// TCP 配置参数
	WIRELESS_PARAM  stWirlessPara;  // GRPS/CDMA无线通讯配置参数
}COMM_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/****************************************************************************
 Function:		通讯模块初始化(只适用于无线模块)
 Param In:
	pstComCfg	参数信息
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommInitModule(COMM_CONFIG *pstCfg);

/****************************************************************************
 Function:		设置通讯模块参数
 Param In:
	pstComCfg	参数信息
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommSetCfgParam(COMM_CONFIG *pstCfg);

/****************************************************************************
 Function:		检查指定的电话号码是否与已经存储的参数一致
 Param In:
	pszTelNo	电话号码
 Param Out:		none
 Return Code:
	TRUE		一致
	FALSE		不一致
****************************************************************************/
int CommChkIfSameTelNo(uchar *pszTelNo);

/****************************************************************************
 Function:		切换通信模式
 Param In:
	ucCommType	通信模式
 Param Out:		none
 Return Code:
	None
****************************************************************************/
void CommSwitchType(uchar ucCommType);

/****************************************************************************
 Function:		通讯模块拨号(MODEM拨号或者TCP建立TCP连接等)
 Param In:
	ucDialMode	拨号标志,DM_PREDIAL:拨号后立刻返回, DM_DIAL: 链路建立后返回
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommDial(uchar ucDialMode);

/****************************************************************************
 Function:		通讯模块发送数据
 Param In:
	psTxdData	待发送的数据
	uiDataLen	待发送数据的实际长度(以字节为单位)
	ulTimeOut	发送超时,以秒计(RFU)
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec);

/****************************************************************************
 Function:		通讯模块接收数据
 Param In:
	uiExpLen	期望接收的最大数据长度(以字节为单位)
	ulTimeOut	等待接收数据的超时时间,以秒计
 Param Out:
	psRxdData	接收到的数据
	puiOutLen	接收到的实际数据长度
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);

/****************************************************************************
 Function:		通讯模块断开链路(MODEM挂机或者TCP断开TCP连接等)
 Param In:
	bReleaseAll	是否完全断开链路层连接, TRUE: 断开, FALSE: 断开应用数据链路
				(RFU for 无线模块)
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommOnHook(uchar bReleaseAll);

/****************************************************************************
 Function:			获取通讯错误信息
 Param In:
	iErrCode		其他通讯函数返回的错误代码
	pstCommErrMsg	错误信息(包括中英文)
 Param Out:			none
 Return Code:		none
****************************************************************************/
void CommGetErrMsg(int iErrCode, COMM_ERR_MSG *pstCommErrMsg);

void RefreshGPRS(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _CommLIB_H

// end of file
