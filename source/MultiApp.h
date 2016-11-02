
/****************************************************************************
NAME
    MultiApp.h - 定义当和应用管理器协同工作时，本程序作为子应用所使用的功能；实现管理器和子应用的消息传递接口

DESCRIPTION
	程序可以运行在3种模式
	1.独立运行模式－－程序通过main进入，自己维护和使用所有参数，没有与管理器应用的互动。或者程序本身就是管理器。
		此模式下，应用不需依靠其它任何程序即可良好运行。
	2.主要子应用模式－－存在应用管理器，同时也可能存在其它子应用。程序既可能由main进入，也可能由event_main进入。
		自己维护和使用所有参数，但是需要和管理器通信，以向管理器或者其它子应用提供共享参数，或接收管理器端设置好的参数。
		此模式下，应用必须在管理器的管理下运行，但次要子应用是否存在是无关紧要的
	3.次要子应用模式－－存在应用管理器，另外也已经存在唯一的主要子应用。程序既可能由main进入，也可能由event_main进入。
		程序自己维护一部分参数，另外一部分参数须从管理器（或主要子应用）获得和设置。
		此模式下，管理器、主要子应用必须存在。

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    Gary Ho     2009.06.18      - created
****************************************************************************/

#ifndef MULTI_AS_SUBAPP_H
#define MULTI_AS_SUBAPP_H

#define FILE_SYNC_PARAM	"SYNC_PARAM"

#define LEN_APPMSG_TAG		16		// tag最大长度为16字节
#define LEN_APPMSG_VALUE	128		// value最大长度为128字节
#define SIZE_APPMSG_BUFF	2048	// 装载"tag=value"的缓冲区大小。

// 返回值
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
