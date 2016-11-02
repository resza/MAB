
/****************************************************************************
NAME
    MultiApp.h - ���嵱��Ӧ�ù�����Эͬ����ʱ����������Ϊ��Ӧ����ʹ�õĹ��ܣ�ʵ�ֹ���������Ӧ�õ���Ϣ���ݽӿ�

DESCRIPTION
	�������������3��ģʽ
	1.��������ģʽ��������ͨ��main���룬�Լ�ά����ʹ�����в�����û���������Ӧ�õĻ��������߳�������ǹ�������
		��ģʽ�£�Ӧ�ò������������κγ��򼴿��������С�
	2.��Ҫ��Ӧ��ģʽ��������Ӧ�ù�������ͬʱҲ���ܴ���������Ӧ�á�����ȿ�����main���룬Ҳ������event_main���롣
		�Լ�ά����ʹ�����в�����������Ҫ�͹�����ͨ�ţ��������������������Ӧ���ṩ�������������չ����������úõĲ�����
		��ģʽ�£�Ӧ�ñ����ڹ������Ĺ��������У�����Ҫ��Ӧ���Ƿ�������޹ؽ�Ҫ��
	3.��Ҫ��Ӧ��ģʽ��������Ӧ�ù�����������Ҳ�Ѿ�����Ψһ����Ҫ��Ӧ�á�����ȿ�����main���룬Ҳ������event_main���롣
		�����Լ�ά��һ���ֲ���������һ���ֲ�����ӹ�����������Ҫ��Ӧ�ã���ú����á�
		��ģʽ�£�����������Ҫ��Ӧ�ñ�����ڡ�

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    Gary Ho     2009.06.18      - created
****************************************************************************/

#ifndef MULTI_AS_SUBAPP_H
#define MULTI_AS_SUBAPP_H

#define FILE_SYNC_PARAM	"SYNC_PARAM"

#define LEN_APPMSG_TAG		16		// tag��󳤶�Ϊ16�ֽ�
#define LEN_APPMSG_VALUE	128		// value��󳤶�Ϊ128�ֽ�
#define SIZE_APPMSG_BUFF	2048	// װ��"tag=value"�Ļ�������С��

// ����ֵ
#define ERR_APPMSG_BUFF		-1
#define ERR_APPMSG_END		-2
#define ERR_APPMSG_TAGLEN	-3
#define ERR_APPMSG_VALUE	-4

// tag
#define MMSG_TAG_LANG		"LANG"
#define MMSG_TAG_PEDMODE	"PED_MODE"
#define MMSG_TAG_EMV		"EMV"
#define MMSG_TAG_FUNC		"FUNC"
#define MMSG_TAG_PABX		"PABX"
#define MMSG_TAG_APN		"WL_APN"
#define MMSG_TAG_DHCP		"LAN_DHCP"
#define MMSG_TAG_PWDBANK	"PWD_BANK"
#define MMSG_TAG_PWDTERM	"PWD_TERM"
#define MMSG_TAG_PWDMERCH	"PWD_MERCHANT"
#define MMSG_TAG_PWDVOID	"PWD_VOID"
#define MMSG_TAG_PWDREFUND	"PWD_REFUND"
#define MMSG_TAG_PWDADJ		"PWD_ADJUST"
#define MMSG_TAG_PWDSETT	"PWD_SETTLE"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// define the data type of value in "tag=value" message
typedef enum{
	MMSG_TYPE_STRING,
	MMSG_TYPE_UCHAR,	
	MMSG_TYPE_INT,
	MMSG_TYPE_ULONG,
}mmsgtype_t;

int InitMultiAppInfo(void);
uchar ChkCurAppMode(uchar ucAppMode);
int LoadSyncParaFile(void);
int ProcManagerAdminMsg(void);
int ProcSendAppMsg(void *pvBuff, int iBuffSize, uchar ucUpdateAll);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

// end of file
