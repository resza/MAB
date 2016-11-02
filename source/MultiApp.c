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
// �жϵ�ǰӦ�õ�����ģʽ����������/����Ӧ��/����Ӧ��
// ���أ�0��������Ҫ����״̬��״̬�ޱ仯����1������Ҫ���棨״̬�б䣩
int InitMultiAppInfo(void)
{
	return 0;
}

// ���ع�����������Ӧ�õĲ���ͬ���ļ�(��Ӧ�ù��������ڣ���Ӧ�������ڷǶ���ģʽʱ)
// ����������Ҫ��Ӧ��
//int LoadSyncParaFile(void)
//{
//	return 0;
//}

// ����ӹ��������ݽ�������Ϣ�������������������Ӧ�õĲ���ͬ���ļ���
// ���ucCheckBuffSize!=0, ����pszRetStr������С�BUFFER=xxxx;���ı��
int ProcManagerAdminMsg(void)
{
	return 0;
}

// ��Ӧ�ü�ͨ���ִ�����ȡ"TAG1=VALUE1;TAG2=VALUE2"��Ԫ��
// �﷨����һ��"="֮ǰΪtag��"="֮���һ��";"֮ǰΪvalue
// ��"="����ֻ��tagû��value
// �ո��κη��﷨ָ�����ž�������tag��value�����һ����
// ����tag��valueӦ��ֻ�ɿɼ���ASCII�ַ���ɣ�����Ӧ���йؼ�����"="����";"
static int GetMsgElement(char *pszSource, char *pszTag, char *pszValue, int *piLen)
{
	char *pStart, *pEqual, *pEnd;

	if (strlen(pszSource)==0)
	{
		return ERR_APPMSG_END;		// �ַ�������Ϊ0�����ѽ���
	}

	pStart = pszSource;

	pEnd = strstr(pStart, ";");
	if (pEnd==NULL)
	{
		pEnd = pStart+strlen(pStart);// û��";"����,�����ַ����Ľ�βΪ����
		*piLen = pEnd-pStart;
	}
	else
	{
		*piLen = pEnd-pStart+1;
	}

	pEqual = strstr(pStart, "=");
	if ((pEqual==NULL) || (pEqual>pEnd))// û��"="������"="��";"֮�󣬼�ֻ��tagû��value
	{
		if (pEnd-pStart>LEN_APPMSG_TAG)
		{
			return ERR_APPMSG_TAGLEN;	// �﷨����tag�ֶγ���
		}
		sprintf(pszTag, "%.*s", pEnd-pStart, pStart);
		sprintf(pszValue, "");
	}
	else
	{
		if (pEqual-pStart>LEN_APPMSG_TAG)
		{
			return ERR_APPMSG_TAGLEN;	// �﷨����tag�ֶγ���
		}
		if (pEnd-pEqual>LEN_APPMSG_VALUE)
		{
			return ERR_APPMSG_VALUE;	// �﷨����value�ֶγ���
		}
		sprintf(pszTag,   "%.*s", pEqual-pStart, pStart);
		sprintf(pszValue, "%.*s", pEnd-pEqual-1, pEqual+1);
	}

	return 0;
}

// ����ӹ������յ��ġ��ѽ����tag��value
// ���ڴ˺����ڽ��б����ļ�����
static int ProcMsgElement(char *pszTag, char *pszValue)
{
	// ���ﴦ�������������Ӧ�õ���Ϣ
	// һ����˵�������������������Ҫ��Ӧ�������ã�Ȼ��ش��������������ɹ���������������Ӧ��
	// ��Ӧ������Ҫ��Ӧ�ã����ò�����ͬ����������Ӧ�ã�������������������Ӧ�õ�ͬ��
	return 1;
}

// ��"tag=value"���뵽���е�buffer���档���buffer����������ͬtag���ַ��������滻
// ���������������tag��value���ȣ��Լ���֤��������»��������
// ��������У��ԭ���ִ�����ȷ��
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

	// �������tag��value
	if ((pszTag==NULL)|| (strlen(pszTag)>LEN_APPMSG_TAG))
	{
		return ERR_APPMSG_TAGLEN;
	}
	if ((pszValue!=NULL) && strlen(pszValue)>LEN_APPMSG_VALUE)
	{
		return ERR_APPMSG_VALUE;
	}
	// �����������ִ�
	strcpy(szNewStr, pszTag);
	if (pszValue!=NULL)
	{
		strcat(szNewStr, "=");
		strcat(szNewStr, pszValue);
	}
	strcat(szNewStr, ";");
	iInsLen = strlen(szNewStr);

	// ���������������滻���������ӵ�β��
	pszTemp = pszDest;
	while (1)
	{
		iRet = GetMsgElement(pszTemp, szTempTag, szTempVal, &iLen);
		if (iRet==0)
		{
			if (strcmp(szTempTag, pszTag)==0)	// �������
			{
				if (strlen(pszDest)-iLen+iInsLen>SIZE_APPMSG_BUFF)
				{
					// �滻���ܳ���buffer size
					return ERR_APPMSG_BUFF;
				}
			
				// ���滻
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
		// �������ַ������ܳ���buffer size
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

// ��manager���汾Ӧ�������޸ġ�����Ҫ֪ͨ������Ӧ�û���֪ͨ�������ı�����
// ���뱣֤pvBuff�Ŀռ��㹻
// buffer for pvBuff should >= SIZE_APPMSG_BUFF
int ProcSendAppMsg(void *pvBuff, int iBuffSize, uchar ucUpdateAll)
{
	char	szBuff[256];
	
	memset(sg_szMsgBuff, 0, sizeof(sg_szMsgBuff));

	// When ucUpdateAll!=0, collect those manager-use parameters.
	if (ucUpdateAll)
	{
		// ����FUNC��֧�ֵ����й��ܺ�
		memset(szBuff, 0, sizeof(szBuff));
		GetAllSupportFunc(szBuff);
		PackManagerMsg(MMSG_TAG_FUNC, szBuff, MMSG_TYPE_STRING);
		// �����Ƿ�֧��EMV
		if (ChkIfEmvEnable())
		{
			PackManagerMsg(MMSG_TAG_EMV, "1", MMSG_TYPE_STRING);
		}
		else
		{
			PackManagerMsg(MMSG_TAG_EMV, "0", MMSG_TYPE_STRING);
		}
	}

	// �����������������µĲ���
	// ����
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

	// LAN: ��������? Gateway?
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

	// ����
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

	// LAN: ��������? Gateway?
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

	// ����
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

