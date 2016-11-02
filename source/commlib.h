
/****************************************************************************
NAME
	commlib.h - ʵ�ֹ���ͨѶ����

DESCRIPTION
	1. ʵ��POSӦ�ÿ����г���ͨѶ��ʽ�����ӵĽ������Ͽ��������շ��ȹ���.
	2. ��ģ���ڲ�ռ�ö�ʱ��4#(����ģ�������,�˳�ģ����ͷ�)

REFERENCE

MODIFICATION SHEET:
	MODIFIED   (YYYY.MM.DD)
	shengjx     2006.09.05      - created
****************************************************************************/

#ifndef _COMMLIB_H
#define _COMMLIB_H

#define MASK_COMM_TYPE			0xFF00		// ��ȡ�������
#define MASK_ERR_CODE			0x00FF		// ��ȡ�������

// �������ʹ���
#define ERR_COMM_ALL_BASE		0x0000		// �������ʹ���
#define ERR_COMM_INV_PARAM		0x0001		// ��������
#define ERR_COMM_INV_TYPE		0x0002		// ��ЧͨѶ����
#define ERR_COMM_CANCEL			0x0003		// �û�ȡ��
#define ERR_COMM_TIMEOUT		0x0004		// ͨѶ��ʱ
#define ERR_COMM_COMERR			0x0005
#define	ERR_COMM_TOOBIG			0x0006

#define ERR_COMM_RS232_BASE		0x0100		// RS232�����

#define ERR_COMM_MODEM_BASE			0x0200		// Modem�����
#define ERR_COMM_MODEM_OCCUPIED		(ERR_COMM_MODEM_BASE+0x02)
#define ERR_COMM_MODEM_NO_LINE		(ERR_COMM_MODEM_BASE+0x03)
#define ERR_COMM_MODEM_LINE			(ERR_COMM_MODEM_BASE+0x04)
#define ERR_COMM_MODEM_NO_ACK		(ERR_COMM_MODEM_BASE+0x05)
#define ERR_COMM_MODEM_LINE_BUSY	(ERR_COMM_MODEM_BASE+0x0D)
#define ERR_COMM_MODEM_NO_LINE_2	(ERR_COMM_MODEM_BASE+0x33)
#define ERR_COMM_MODEM_NO_PHONE_UP	(ERR_COMM_MODEM_BASE+0x83)

#define ERR_COMM_TCPIP_BASE		0x0300		// TCPIP�����
#define ERR_COMM_TCPIP_OPENPORT	0x0301		// �򿪶˿�ʧ��
#define ERR_COMM_TCPIP_SETLIP	0x0302
#define ERR_COMM_TCPIP_SETRIP	0x0303
#define ERR_COMM_TCPIP_SETRPORT	0x0304
#define ERR_COMM_TCPIP_CONN		0x0305
#define ERR_COMM_TCPIP_TXD		0x0306
#define ERR_COMM_TCPIP_RXD		0x0307
#define ERR_COMM_TCPIP_SETGW	0x0308
#define ERR_COMM_TCPIP_SETMASK	0x0309

#define ERR_COMM_WIRELESS_BASE		0x0400		// GPRS/CDMA�����

// ���ŷ�ʽ
#define DM_PREDIAL			0		// Ԥ����/Ԥ����
#define DM_DIAL				1		// ʵ�ʲ���/����

// #define _WIRELESS_PRE_DIAL		// open for pre-connect of wireless module

// �������ݷ�ʽ
#define CM_RAW				0		// ԭ���ݷ��ͷ�ʽ(�������ֹ�ַ�,For RS232)
#define CM_SYNC				1		// ͬ��
#define CM_ASYNC			2		// �첽

// for RS232 communication
#define STX             0x02
#define ETX             0x03
#define ENQ             0x05
#define ACK             0x06
#define NAK             0x15

// RS232���ò���
typedef struct _tagRS232_PARA
{
	uchar	ucPortNo;			// Port #, COM1, COM2 ....
	uchar	ucSendMode;			// ���ݷ��ͷ�ʽ
	uchar	szAttr[20+1];		// ���ڴ򿪲���, "9600,8,n,1", ....
}RS232_PARA;

// TCP/IP���ò���
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

// PSTN���ò���
// PSTN parameter
typedef struct _tagPSTN_PARA
{
	uchar		ucSendMode;		// ���ݷ��ͷ�ʽ
	uchar		szTelNo[100+1];	// �绰����
	COMM_PARA	stPara;
    uchar       ucSignalLevel;  // dial voltage level, range 1-15. Use 0 for disable.
}PSTN_PARA;

// GPRS/CDMA configurations
typedef struct _tagWIRELESS_PARAM //
{
	uchar		szAPN[64+1];  ///���ź��� CDMA: #777; GPRS: cmnet
	uchar		szUID[64+1];
	uchar		szPwd[16+1];
	uchar		szSimPin[16+1];     // SIM card PIN
	uchar		szDNS[32+1];
	uchar		szRemoteIP_1[20+1];
	uchar		szRemotePort_1[5+1];
	uchar		szRemoteIP_2[20+1];
	uchar		szRemotePort_2[5+1];
}WIRELESS_PARAM;

// ͨѶģ�������Ϣ
typedef struct _tagCOMM_ERR_MSG
{
	uchar		szMsg[16+1];
}COMM_ERR_MSG;


/****************************************************************************
 Function:		����ʱ�����û�����(ÿ�����һ��,RS232 raw��ʽ��֧��)
 Param In:
	uiTimeLeft	ʣ�೬ʱʱ��,��λΪ��
 Param Out:		none
 Return Code:	none
****************************************************************************/
typedef void (*UpdWaitRspUI)(ushort uiTimeLeft);

// ͨѶģ��������Ϣ
typedef struct _tagCOMM_CONFIG
{
#define CT_NONE		0			// ��ʹ��
#define CT_RS232	1			// RS232ͨѶ
#define CT_MODEM	2			// ModemͨѶ
#define CT_TCPIP	3			// TCP/IPͨѶ
#define	CT_CDMA		4			// CDMA(RFU)
#define CT_GPRS		5			// GPRS(RFU)
#define CT_WIFI		6			// WIFI(RFU)
#define CT_DEMO		0xFF		// ��ʾ/����/��ѵʹ�õ�����ģʽ
	uchar			ucCommType;		// ͨѶ����(RS232/modem/tcp...)
	uchar			ucCommTypeBak;	// ��ѡͨѶ����(RS232/modem/tcp...)

	UpdWaitRspUI	pfUpdWaitUI;	// ����ʱ�����û�����(ÿ�����һ��,RS232 raw��ʽ��֧��)

	uchar			ucTCPClass_BCDHeader;	// For all TCPIP class: the length bytes are in BCD format or not

	RS232_PARA		stRS232Para;	// RS232 ���ò���
	PSTN_PARA		stPSTNPara;		// modem ���ò���
	TCPIP_PARA		stTcpIpPara;	// TCP ���ò���
	WIRELESS_PARAM  stWirlessPara;  // GRPS/CDMA����ͨѶ���ò���
}COMM_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/****************************************************************************
 Function:		ͨѶģ���ʼ��(ֻ����������ģ��)
 Param In:
	pstComCfg	������Ϣ
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommInitModule(COMM_CONFIG *pstCfg);

/****************************************************************************
 Function:		����ͨѶģ�����
 Param In:
	pstComCfg	������Ϣ
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommSetCfgParam(COMM_CONFIG *pstCfg);

/****************************************************************************
 Function:		���ָ���ĵ绰�����Ƿ����Ѿ��洢�Ĳ���һ��
 Param In:
	pszTelNo	�绰����
 Param Out:		none
 Return Code:
	TRUE		һ��
	FALSE		��һ��
****************************************************************************/
int CommChkIfSameTelNo(uchar *pszTelNo);

/****************************************************************************
 Function:		�л�ͨ��ģʽ
 Param In:
	ucCommType	ͨ��ģʽ
 Param Out:		none
 Return Code:
	None
****************************************************************************/
void CommSwitchType(uchar ucCommType);

/****************************************************************************
 Function:		ͨѶģ�鲦��(MODEM���Ż���TCP����TCP���ӵ�)
 Param In:
	ucDialMode	���ű�־,DM_PREDIAL:���ź����̷���, DM_DIAL: ��·�����󷵻�
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommDial(uchar ucDialMode);

/****************************************************************************
 Function:		ͨѶģ�鷢������
 Param In:
	psTxdData	�����͵�����
	uiDataLen	���������ݵ�ʵ�ʳ���(���ֽ�Ϊ��λ)
	ulTimeOut	���ͳ�ʱ,�����(RFU)
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec);

/****************************************************************************
 Function:		ͨѶģ���������
 Param In:
	uiExpLen	�������յ�������ݳ���(���ֽ�Ϊ��λ)
	ulTimeOut	�ȴ��������ݵĳ�ʱʱ��,�����
 Param Out:
	psRxdData	���յ�������
	puiOutLen	���յ���ʵ�����ݳ���
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);

/****************************************************************************
 Function:		ͨѶģ��Ͽ���·(MODEM�һ�����TCP�Ͽ�TCP���ӵ�)
 Param In:
	bReleaseAll	�Ƿ���ȫ�Ͽ���·������, TRUE: �Ͽ�, FALSE: �Ͽ�Ӧ��������·
				(RFU for ����ģ��)
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommOnHook(uchar bReleaseAll);

/****************************************************************************
 Function:			��ȡͨѶ������Ϣ
 Param In:
	iErrCode		����ͨѶ�������صĴ������
	pstCommErrMsg	������Ϣ(������Ӣ��)
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
