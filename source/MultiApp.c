#include "global.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
static int GetMsgElement(char *pszSource, char *pszTag, char *pszValue, int *piLen);
static int SetMsgElement(char *pszDest, char *pszTag, char *pszValue);
static int ProcMsgElement(char *pszTag, char *pszValue);
static int PackManagerMsg(char *pszTag, void *pszValue, int iValueType);

/********************** Internal variables declaration *********************/
char	sg_szMsgBuff[SIZE_APPMSG_BUFF+2];

/********************** external reference declaration *********************/

/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// To determine the running mode of current application.
// return: 0--No need save; 1--Need save
// 判断当前应用的运行模式：独立运行/主子应用/从子应用
// 返回：0－－不需要保存状态（状态无变化）；1－－需要保存（状态有变）
int InitMultiAppInfo(void)
{
	return 0;
}

// 加载管理器传给子应用的参数同步文件(当应用管理器存在，子应用运行在非独立模式时)
// 不适用于主要子应用
//int LoadSyncParaFile(void)
//{
//	return 0;
//}

// 处理从管理器传递进来的信息（或解析管理器传给子应用的参数同步文件）
// 如果ucCheckBuffSize!=0, 则检查pszRetStr必须带有“BUFFER=xxxx;”的标记
int ProcManagerAdminMsg(void)
{
	return 0;
}

// 从应用间通信字串中提取"TAG1=VALUE1;TAG2=VALUE2"的元素
// 语法：第一个"="之前为tag，"="之后第一个";"之前为value
// 无"="代表只有tag没有value
// 空格及任何非语法指定符号均被当作tag或value本身的一部分
// 建议tag和value应该只由可见的ASCII字符组成，但不应含有关键符号"="或者";"
static int GetMsgElement(char *pszSource, char *pszTag, char *pszValue, int *piLen)
{
	char *pStart, *pEqual, *pEnd;

	if (strlen(pszSource)==0)
	{
		return ERR_APPMSG_END;		// 字符串长度为0，即已结束
	}

	pStart = pszSource;

	pEnd = strstr(pStart, ";");
	if (pEnd==NULL)
	{
		pEnd = pStart+strlen(pStart);// 没有";"结束,则以字符串的结尾为结束
		*piLen = pEnd-pStart;
	}
	else
	{
		*piLen = pEnd-pStart+1;
	}

	pEqual = strstr(pStart, "=");
	if ((pEqual==NULL) || (pEqual>pEnd))// 没有"="，或者"="在";"之后，即只有tag没有value
	{
		if (pEnd-pStart>LEN_APPMSG_TAG)
		{
			return ERR_APPMSG_TAGLEN;	// 语法错误：tag字段超长
		}
		sprintf(pszTag, "%.*s", pEnd-pStart, pStart);
		sprintf(pszValue, "");
	}
	else
	{
		if (pEqual-pStart>LEN_APPMSG_TAG)
		{
			return ERR_APPMSG_TAGLEN;	// 语法错误：tag字段超长
		}
		if (pEnd-pEqual>LEN_APPMSG_VALUE)
		{
			return ERR_APPMSG_VALUE;	// 语法错误：value字段超长
		}
		sprintf(pszTag,   "%.*s", pEqual-pStart, pStart);
		sprintf(pszValue, "%.*s", pEnd-pEqual-1, pEqual+1);
	}

	return 0;
}

// 处理从管理器收到的、已解包的tag和value
// 不在此函数内进行保存文件动作
static int ProcMsgElement(char *pszTag, char *pszValue)
{
	// 这里处理管理器传给本应用的消息
	// 一般来说，大多数参数都是在主要子应用里设置，然后回传给管理器，再由管理器传给其它子应用
	// 本应用是主要子应用，设置参数并同步到其它子应用，而不被动接收其它子应用的同步
	return 1;
}

// 把"tag=value"插入到已有的buffer里面。如果buffer本身已有相同tag的字符串，则替换
// 本函数会检查待插入tag和value长度，以及保证插入后不引致缓冲溢出。
// 本函数不校验原有字串的正确性
static int SetMsgElement(char *pszDest, char *pszTag, char *pszValue)
{
	int		iLen, iInsLen, iRet;
	char	*pszTemp;
	char	szTempTag[LEN_APPMSG_TAG+1], szTempVal[LEN_APPMSG_VALUE+1];
	char	szNewStr[LEN_APPMSG_TAG+1+LEN_APPMSG_VALUE+1+1];

	if (pszDest==NULL)
	{
		return 0;
	}

	// 检查输入tag和value
	if ((pszTag==NULL)|| (strlen(pszTag)>LEN_APPMSG_TAG))
	{
		return ERR_APPMSG_TAGLEN;
	}
	if ((pszValue!=NULL) && strlen(pszValue)>LEN_APPMSG_VALUE)
	{
		return ERR_APPMSG_VALUE;
	}
	// 构造待插入的字串
	strcpy(szNewStr, pszTag);
	if (pszValue!=NULL)
	{
		strcat(szNewStr, "=");
		strcat(szNewStr, pszValue);
	}
	strcat(szNewStr, ";");
	iInsLen = strlen(szNewStr);

	// 搜索，若存在则替换，否则连接到尾部
	pszTemp = pszDest;
	while (1)
	{
		iRet = GetMsgElement(pszTemp, szTempTag, szTempVal, &iLen);
		if (iRet==0)
		{
			if (strcmp(szTempTag, pszTag)==0)	// 如果存在
			{
				if (strlen(pszDest)-iLen+iInsLen>SIZE_APPMSG_BUFF)
				{
					// 替换后不能超出buffer size
					return ERR_APPMSG_BUFF;
				}
			
				// 则替换
				memmove(pszTemp+iInsLen, pszTemp+iLen, strlen(pszTemp+iLen)+1);
				memcpy(pszTemp, szNewStr, iInsLen);
				return 0;
			}
			else
			{
				pszTemp += iLen;
				continue;
			}
		}
		else if (iRet==ERR_APPMSG_END)
		{
			break;
		}
		else
		{
			return iRet;
		}
	}

	if (strlen(pszDest)+1+iInsLen+1>SIZE_APPMSG_BUFF)
	{
		// 连接新字符串后不能超出buffer size
		return ERR_APPMSG_BUFF;
	}

	if ((strlen(pszDest)!=0) && (pszDest[strlen(pszDest)-1]!=';'))
	{
		strcat(pszDest, ";");
	}
	strcat(pszDest, szNewStr);
	return 0;
}

static int PackManagerMsg(char *pszTag, void *pszValue, int iValueType)
{
	char	szBuff[LEN_APPMSG_VALUE+2];

	switch(iValueType)
	{
	case MMSG_TYPE_STRING:
		sprintf(szBuff, "%.*s", LEN_APPMSG_VALUE, (char *)pszValue);
		break;
	case MMSG_TYPE_UCHAR:
		sprintf(szBuff, "%d", *(uchar *)pszValue);
		break;
	case MMSG_TYPE_INT:
		sprintf(szBuff, "%d", *(int *)pszValue);
		break;
	case MMSG_TYPE_ULONG:
		sprintf(szBuff, "%lu", *(ulong *)pszValue);
	    break;
	default:
	    return ERR_APPMSG_VALUE;
	}

	return SetMsgElement(sg_szMsgBuff, pszTag, szBuff);
}

// 向manager报告本应用中已修改、并需要通知其它子应用或者通知管理器的变量。
// 必须保证pvBuff的空间足够
// buffer for pvBuff should >= SIZE_APPMSG_BUFF
int ProcSendAppMsg(void *pvBuff, int iBuffSize, uchar ucUpdateAll)
{
	char	szBuff[256];
	
	memset(sg_szMsgBuff, 0, sizeof(sg_szMsgBuff));

	// When ucUpdateAll!=0, collect those manager-use parameters.
	if (ucUpdateAll)
	{
		// 报告FUNC键支持的所有功能号
		memset(szBuff, 0, sizeof(szBuff));
		GetAllSupportFunc(szBuff);
		PackManagerMsg(MMSG_TAG_FUNC, szBuff, MMSG_TYPE_STRING);
		// 报告是否支持EMV
		if (ChkIfEmvEnable())
		{
			PackManagerMsg(MMSG_TAG_EMV, "1", MMSG_TYPE_STRING);
		}
		else
		{
			PackManagerMsg(MMSG_TAG_EMV, "0", MMSG_TYPE_STRING);
		}
	}

	// 向管理器报告最近更新的参数
	// 密码
#ifdef	__TELIUM_PARAM__
	if (strcmp(glTlmSysParam.sPassword[PWD_BANK], glTlmSysParamBak.sPassword[PWD_BANK])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDBANK, glTlmSysParam.sPassword[PWD_BANK], MMSG_TYPE_STRING);
	}
	if (strcmp(glTlmSysParam.sPassword[PWD_TERM], glTlmSysParamBak.sPassword[PWD_TERM])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDTERM, glTlmSysParam.sPassword[PWD_TERM], MMSG_TYPE_STRING);
	}
	if (strcmp(glTlmSysParam.sPassword[PWD_MERCHANT], glTlmSysParamBak.sPassword[PWD_MERCHANT])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDMERCH, glTlmSysParam.sPassword[PWD_MERCHANT], MMSG_TYPE_STRING);
	}
	if (strcmp(glTlmSysParam.sPassword[PWD_VOID], glTlmSysParamBak.sPassword[PWD_VOID])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDVOID, glTlmSysParam.sPassword[PWD_VOID], MMSG_TYPE_STRING);
	}
	if (strcmp(glTlmSysParam.sPassword[PWD_REFUND], glTlmSysParamBak.sPassword[PWD_REFUND])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDREFUND, glTlmSysParam.sPassword[PWD_REFUND], MMSG_TYPE_STRING);
	}
	if (strcmp(glTlmSysParam.sPassword[PWD_ADJUST], glTlmSysParamBak.sPassword[PWD_ADJUST])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDADJ, glTlmSysParam.sPassword[PWD_ADJUST], MMSG_TYPE_STRING);
	}
	if (strcmp(glTlmSysParam.sPassword[PWD_SETTLE], glTlmSysParamBak.sPassword[PWD_SETTLE])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDSETT, glTlmSysParam.sPassword[PWD_SETTLE], MMSG_TYPE_STRING);
	}

	// modem: PABX
	if (strcmp((char *)glTlmSysParam.stEdcInfo.szPabx, (char *)glTlmSysParamBak.stEdcInfo.szPabx)!=0)
	{
		PackManagerMsg(MMSG_TAG_PABX, glTlmSysParam.stEdcInfo.szPabx, MMSG_TYPE_STRING);
	}

	// GPRS: APN/UID/PWD
	if (strcmp((char *)glTlmSysParam.stTxnCommCfg.stWirlessPara.szAPN, (char *)glTlmSysParamBak.stTxnCommCfg.stWirlessPara.szAPN)!=0)
	{
		PackManagerMsg(MMSG_TAG_APN, glTlmSysParam.stTxnCommCfg.stWirlessPara.szAPN, MMSG_TYPE_STRING);
	}
	if (strcmp((char *)glTlmSysParam.stTxnCommCfg.stWirlessPara.szUID, (char *)glTlmSysParamBak.stTxnCommCfg.stWirlessPara.szUID)!=0)
	{
		PackManagerMsg(MMSG_TAG_APN, glTlmSysParam.stTxnCommCfg.stWirlessPara.szUID, MMSG_TYPE_STRING);
	}
	if (strcmp((char *)glTlmSysParam.stTxnCommCfg.stWirlessPara.szPwd, (char *)glTlmSysParamBak.stTxnCommCfg.stWirlessPara.szPwd)!=0)
	{
		PackManagerMsg(MMSG_TAG_APN, glTlmSysParam.stTxnCommCfg.stWirlessPara.szPwd, MMSG_TYPE_STRING);
	}
	if (strcmp((char *)glTlmSysParam.stTxnCommCfg.stWirlessPara.szSimPin, (char *)glTlmSysParamBak.stTxnCommCfg.stWirlessPara.szPwd)!=0)
	{
		PackManagerMsg(MMSG_TAG_APN, glTlmSysParam.stTxnCommCfg.stWirlessPara.szSimPin, MMSG_TYPE_STRING);
	}

	// LAN: 子网掩码? Gateway?
	if (glTlmSysParam.stTxnCommCfg.stTcpIpPara.ucDhcp!=glTlmSysParamBak.stTxnCommCfg.stTcpIpPara.ucDhcp)
	{
		PackManagerMsg(MMSG_TAG_DHCP, &glTlmSysParam.stTxnCommCfg.stTcpIpPara.ucDhcp, MMSG_TYPE_UCHAR);
	}
	if (strcmp((char *)glTlmSysParam.stTxnCommCfg.stTcpIpPara.szLocalIP, (char *)glTlmSysParamBak.stTxnCommCfg.stTcpIpPara.szLocalIP)!=0)
	{
		PackManagerMsg(MMSG_TAG_DHCP, glTlmSysParam.stTxnCommCfg.stTcpIpPara.szLocalIP, MMSG_TYPE_STRING);
	}
	if (strcmp((char *)glTlmSysParam.stTxnCommCfg.stTcpIpPara.szDNSIP, (char *)glTlmSysParamBak.stTxnCommCfg.stTcpIpPara.szDNSIP)!=0)
	{
		PackManagerMsg(MMSG_TAG_DHCP, glTlmSysParam.stTxnCommCfg.stTcpIpPara.szDNSIP, MMSG_TYPE_STRING);
	}

	// 语言
	if (strcmp((char *)glTlmSysParam.stEdcInfo.stLangCfg.szDispName, (char *)glTlmSysParamBak.stEdcInfo.stLangCfg.szDispName)!=0)
	{
		PackManagerMsg(MMSG_TAG_LANG, glTlmSysParam.stEdcInfo.stLangCfg.szDispName, MMSG_TYPE_STRING);
	}

	// PED mode
	if (glTlmSysParam.stEdcInfo.ucPedMode!=glTlmSysParamBak.stEdcInfo.ucPedMode)
	{
		PackManagerMsg(MMSG_TAG_PEDMODE, &glTlmSysParam.stEdcInfo.ucPedMode, MMSG_TYPE_UCHAR);
	}
#else
	if (strcmp(glSysParam.sPassword[PWD_BANK], glSysParamBak.sPassword[PWD_BANK])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDBANK, glSysParam.sPassword[PWD_BANK], MMSG_TYPE_STRING);
	}
	if (strcmp(glSysParam.sPassword[PWD_TERM], glSysParamBak.sPassword[PWD_TERM])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDTERM, glSysParam.sPassword[PWD_TERM], MMSG_TYPE_STRING);
	}
	if (strcmp(glSysParam.sPassword[PWD_MERCHANT], glSysParamBak.sPassword[PWD_MERCHANT])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDMERCH, glSysParam.sPassword[PWD_MERCHANT], MMSG_TYPE_STRING);
	}
	if (strcmp(glSysParam.sPassword[PWD_VOID], glSysParamBak.sPassword[PWD_VOID])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDVOID, glSysParam.sPassword[PWD_VOID], MMSG_TYPE_STRING);
	}
	if (strcmp(glSysParam.sPassword[PWD_REFUND], glSysParamBak.sPassword[PWD_REFUND])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDREFUND, glSysParam.sPassword[PWD_REFUND], MMSG_TYPE_STRING);
	}
	if (strcmp(glSysParam.sPassword[PWD_ADJUST], glSysParamBak.sPassword[PWD_ADJUST])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDADJ, glSysParam.sPassword[PWD_ADJUST], MMSG_TYPE_STRING);
	}
	if (strcmp(glSysParam.sPassword[PWD_SETTLE], glSysParamBak.sPassword[PWD_SETTLE])!=0)
	{
		PackManagerMsg(MMSG_TAG_PWDSETT, glSysParam.sPassword[PWD_SETTLE], MMSG_TYPE_STRING);
	}

	// modem: PABX
	if (strcmp((char *)glSysParam.stEdcInfo.szPabx, (char *)glSysParamBak.stEdcInfo.szPabx)!=0)
	{
		PackManagerMsg(MMSG_TAG_PABX, glSysParam.stEdcInfo.szPabx, MMSG_TYPE_STRING);
	}

	// GPRS: APN/UID/PWD
	if (strcmp((char *)glSysParam.stTxnCommCfg.stWirlessPara.szAPN, (char *)glSysParamBak.stTxnCommCfg.stWirlessPara.szAPN)!=0)
	{
		PackManagerMsg(MMSG_TAG_APN, glSysParam.stTxnCommCfg.stWirlessPara.szAPN, MMSG_TYPE_STRING);
	}
	if (strcmp((char *)glSysParam.stTxnCommCfg.stWirlessPara.szUID, (char *)glSysParamBak.stTxnCommCfg.stWirlessPara.szUID)!=0)
	{
		PackManagerMsg(MMSG_TAG_APN, glSysParam.stTxnCommCfg.stWirlessPara.szUID, MMSG_TYPE_STRING);
	}
	if (strcmp((char *)glSysParam.stTxnCommCfg.stWirlessPara.szPwd, (char *)glSysParamBak.stTxnCommCfg.stWirlessPara.szPwd)!=0)
	{
		PackManagerMsg(MMSG_TAG_APN, glSysParam.stTxnCommCfg.stWirlessPara.szPwd, MMSG_TYPE_STRING);
	}
	if (strcmp((char *)glSysParam.stTxnCommCfg.stWirlessPara.szSimPin, (char *)glSysParamBak.stTxnCommCfg.stWirlessPara.szPwd)!=0)
	{
		PackManagerMsg(MMSG_TAG_APN, glSysParam.stTxnCommCfg.stWirlessPara.szSimPin, MMSG_TYPE_STRING);
	}

	// LAN: 子网掩码? Gateway?
	if (glSysParam.stTxnCommCfg.stTcpIpPara.ucDhcp!=glSysParamBak.stTxnCommCfg.stTcpIpPara.ucDhcp)
	{
		PackManagerMsg(MMSG_TAG_DHCP, &glSysParam.stTxnCommCfg.stTcpIpPara.ucDhcp, MMSG_TYPE_UCHAR);
	}
	if (strcmp((char *)glSysParam.stTxnCommCfg.stTcpIpPara.szLocalIP, (char *)glSysParamBak.stTxnCommCfg.stTcpIpPara.szLocalIP)!=0)
	{
		PackManagerMsg(MMSG_TAG_DHCP, glSysParam.stTxnCommCfg.stTcpIpPara.szLocalIP, MMSG_TYPE_STRING);
	}
	if (strcmp((char *)glSysParam.stTxnCommCfg.stTcpIpPara.szDNSIP, (char *)glSysParamBak.stTxnCommCfg.stTcpIpPara.szDNSIP)!=0)
	{
		PackManagerMsg(MMSG_TAG_DHCP, glSysParam.stTxnCommCfg.stTcpIpPara.szDNSIP, MMSG_TYPE_STRING);
	}

	// 语言
	if (strcmp((char *)glSysParam.stEdcInfo.stLangCfg.szDispName, (char *)glSysParamBak.stEdcInfo.stLangCfg.szDispName)!=0)
	{
		PackManagerMsg(MMSG_TAG_LANG, glSysParam.stEdcInfo.stLangCfg.szDispName, MMSG_TYPE_STRING);
	}

	// PED mode
	if (glSysParam.stEdcInfo.ucPedMode!=glSysParamBak.stEdcInfo.ucPedMode)
	{
		PackManagerMsg(MMSG_TAG_PEDMODE, &glSysParam.stEdcInfo.ucPedMode, MMSG_TYPE_UCHAR);
	}
#endif

	//if (strlen(sg_szMsgBuff)<iBuffSize)	//irf, 20121128
	if (strlen(sg_szMsgBuff)<(uint)iBuffSize)	//irf, 20121128
	{
		strcpy((char *)pvBuff, sg_szMsgBuff);
	}
	return 0;
}

// end of file

