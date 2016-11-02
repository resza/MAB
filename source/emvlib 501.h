/*****************************************************/
/* Emvlib.h                                          */
/* Define the Application Program Interface          */
/* of EMV2 for all PAX POS terminals     		     */
/* Created by ZengYun at 20/01/2005                  */
/*****************************************************/


#ifndef _EMV_LIB_H
#define _EMV_LIB_H

#define CLSS_SLOT      0xff
#define CONTACT_SLOT   0x00

//����ͨ���򵥵�ɾ����Ӧ�Ĺ�Կ��������������֤������
// �������ʹ�øù�Կ�����������й�Կ֤���޷�ʹ��
#define  MAX_REVOCLIST_NUM    30      // EMVCOҪ��ÿ��RID����֧��30�������б�

#define MAX_APP_NUM       32         //EMV��Ӧ���б����ɴ洢��Ӧ����
#define MAX_APP_ITEMS     17         //��16�޸�Ϊ17
#ifndef _LINUX_TERM
#define MAX_KEY_NUM       64         //monitorƽ̨��EMV����֤���Ĺ�Կ�����ɴ洢�Ĺ�Կ��
#else
#define MAX_KEY_NUM       6          //prolineƽ̨��EMV����֤���Ĺ�Կ�����ɴ洢�Ĺ�Կ
#endif

#define PART_MATCH        0x00       //Ӧ��ѡ��ƥ���־(����ƥ��)
#define FULL_MATCH        0x01       //Ӧ��ѡ��ƥ���־(��ȫƥ��)

#define EMV_CASH          0x01       //��������(�ֽ�)
#define EMV_GOODS	      0x02       //��������(����)
#define EMV_SERVICE       0x04       //��������(����)
#define EMV_CASHBACK      0x08       //��������(����)

#define EMV_INQUIRY		  0x10		 //��������(��ѯ)
#define EMV_TRANSFER	  0x20		 //��������(ת��)
#define EMV_PAYMENT		  0x40		 //��������(֧��)
#define EMV_ADMIN		  0x80		 //��������(����)
#define EMV_CASHDEPOSIT   0x90		 //��������(���)


//���״����������붨��
#define EMV_OK             0         //�ɹ�

#define ICC_RESET_ERR     -1         //IC����λʧ��
#define ICC_CMD_ERR       -2         //IC����ʧ��
#define ICC_BLOCK         -3         //IC������    
   
#define EMV_RSP_ERR       -4         //IC���������
#define EMV_APP_BLOCK     -5         //Ӧ������
#define EMV_NO_APP        -6         //��Ƭ��û��EMVӦ��
#define EMV_USER_CANCEL   -7         //�û�ȡ����ǰ��������
#define EMV_TIME_OUT      -8         //�û�������ʱ
#define EMV_DATA_ERR      -9         //��Ƭ���ݴ���
#define EMV_NOT_ACCEPT    -10        //���ײ�����
#define EMV_DENIAL        -11        //���ױ��ܾ�
#define EMV_KEY_EXP       -12        //��Կ����

//�ص��������������������붨��
#define EMV_NO_PINPAD     -13        //û��������̻���̲�����
#define EMV_NO_PASSWORD   -14        //û��������û���������������
#define EMV_SUM_ERR       -15        //��֤������ԿУ��ʹ���
#define EMV_NOT_FOUND     -16        //û���ҵ�ָ�������ݻ�Ԫ��
#define EMV_NO_DATA       -17        //ָ��������Ԫ��û������
#define EMV_OVERFLOW      -18        //�ڴ����

//��������־
#define NO_TRANS_LOG      -19
#define RECORD_NOTEXIST   -20
#define LOGITEM_NOTEXIST  -21

#define ICC_RSP_6985      -22        // GAC�п�Ƭ����6985, ��Ӧ�þ����Ƿ�fallback
#define EMV_PARAM_ERR     -30

//��PED�ӿڸ��¶����ӷ���ֵ����
#define EMV_PED_TIMEOUT   0x01   // PED�ӿڷ��� PED_RET_ERR_INPUT_TIMEOUT
#define EMV_PED_WAIT      0x02   // PED�ӿڷ��� PED_RET_ERR_WAIT_INTERVAL
#define EMV_PED_FAIL      0x03   // PED�ӿڷ�����������


#define REFER_APPROVE     0x01       //�ο�������(ѡ����׼)
#define REFER_DENIAL      0x02       //�ο�������(ѡ��ܾ�)

#define ONLINE_APPROVE    0x00       //����������(������׼)		
#define ONLINE_FAILED     0x01       //����������(����ʧ��)	
#define ONLINE_REFER      0x02       //����������(�����ο�)
#define ONLINE_DENIAL     0x03       //����������(�����ܾ�)
#define ONLINE_ABORT      0x04       //����PBOC(������ֹ)


#define AC_AAC       0x00
#define AC_TC        0x01
#define AC_ARQC      0x02
#define AC_AAC_HOST  0x03 //used for VisaOnLine


#ifdef WIN32
#pragma warning(disable:4103)
#pragma pack(1)//�趨Ϊ1�ֽڶ���
// #else
// 	__attribute__((__packed__))
#endif

typedef struct{
	unsigned char AppName[33];       //����Ӧ��������'\x00'��β���ַ���
	unsigned char AID[17];           //Ӧ�ñ�־
	unsigned char AidLen;            //AID�ĳ���
    unsigned char SelFlag;           //ѡ���־(PART_MATCH ����ƥ��  FULL_MATCH ȫƥ��)      
	unsigned char Priority;          //���ȼ���־
	unsigned char TargetPer;         //Ŀ��ٷֱ���
	unsigned char MaxTargetPer;      //���Ŀ��ٷֱ���
 	unsigned char FloorLimitCheck;   //�Ƿ�������޶�
	unsigned char RandTransSel;      //�Ƿ�����������ѡ��
	unsigned char VelocityCheck;     //�Ƿ����Ƶ�ȼ��
    unsigned long FloorLimit;        //����޶�
	unsigned long Threshold;         //��ֵ
 	unsigned char TACDenial[6];      //�ն���Ϊ����(�ܾ�)
	unsigned char TACOnline[6];      //�ն���Ϊ����(����)
	unsigned char TACDefault[6];     //�ն���Ϊ����(ȱʡ)
    unsigned char AcquierId[6];      //�յ��б�־
	unsigned char dDOL[256];         //�ն�ȱʡDDOL
	unsigned char tDOL[256];         //�ն�ȱʡTDOL
	unsigned char Version[3];        //Ӧ�ð汾
	unsigned char RiskManData[10];   //���չ�������
}EMV_APPLIST;

typedef struct{
	unsigned char aucAppPreName[17];  //Ӧ������ѡ�����ƣ���'\0'��β
	unsigned char aucAppLabel[17];  //Ӧ�ñ�ǩ����'\0'��β
	unsigned char aucIssDiscrData[244]; //tag 'BF0C'���ݣ�1���ֽڵĳ����ֽ�+'BF0C'���222���ֽڣ���'\0'��β 
	                                    //tag '73'���ݣ�1���ֽڵĳ����ֽ�+'73'���242���ֽڣ���'\0'��β
	unsigned char aucAID[17]; //��ƬAID����'\0'��β
	unsigned char ucAidLen; //��ƬAID����
}APPLABEL_LIST;

typedef struct {
	unsigned char RID[5];            //Ӧ��ע�������ID
	unsigned char KeyID;             //��Կ����
	unsigned char HashInd;           //HASH�㷨��־
	unsigned char ArithInd;          //RSA�㷨��־
	unsigned char ModulLen;          //ģ����
	unsigned char Modul[248];        //ģ
	unsigned char ExponentLen;       //ָ������
	unsigned char Exponent[3];       //ָ��
	unsigned char ExpDate[3];        //��Ч��(YYMMDD)
	unsigned char CheckSum[20];      //��ԿУ���
}EMV_CAPK;

typedef struct{
	unsigned char MerchName[256];    //�̻���
	unsigned char MerchCateCode[2];  //�̻������(ûҪ��ɲ�����)
	unsigned char MerchId[15];       //�̻���־(�̻���)
	unsigned char TermId[8];         //�ն˱�־(POS��)
	unsigned char TerminalType;      //�ն�����
	unsigned char Capability[3];     //�ն�����
	unsigned char ExCapability[5];   //�ն���չ����
	unsigned char TransCurrExp;      //���׻��Ҵ���ָ��
	unsigned char ReferCurrExp;      //�ο�����ָ��
	unsigned char ReferCurrCode[2];  //�ο����Ҵ���
	unsigned char CountryCode[2];    //�ն˹��Ҵ���
	unsigned char TransCurrCode[2];  //���׻��Ҵ���
	unsigned long ReferCurrCon;      //�ο����Ҵ���ͽ��״����ת��ϵ��(���׻��ҶԲο����ҵĻ���*1000)
	unsigned char TransType;         //��ǰ��������(EMV_CASH EMV_GOODS EMV_SERVICE EMV_GOODS&EMV_CASHBACK EMV_SERVICE&EMV_CASHBACK)
	unsigned char ForceOnline;       //�̻�ǿ������(1 ��ʾ������������)
	unsigned char GetDataPIN;        //������ǰ�Ƿ�����Դ���
	unsigned char SurportPSESel;     //�Ƿ�֧��PSEѡ��ʽ
}EMV_PARAM;

// ����ͨ���򵥵�ɾ����Ӧ�Ĺ�Կ��������������֤������
// �������ʹ�øù�Կ�����������й�Կ֤���޷�ʹ�� 
typedef  struct 
{
	unsigned char   ucRid[5];  		        // RID
	unsigned char   ucIndex;   		        // CA��Կ����
	unsigned char   ucCertSn[3];            // ������֤�����к�
}EMV_REVOCLIST;


typedef struct  
{
	unsigned char ucUseTermAIPFlg;   //�Ƿ���ڿ�ƬAIP���з��չ���,0-�ÿ�ƬAIP,1-���ն�AIP,Ĭ��Ϊ0
	unsigned char aucTermAIP[2];    //�ն��Ƿ�ǿ�ƽ��з��չ���byte1-bit4Ϊ1��ǿ�ƽ��з��չ���byte1-bit4Ϊ0�������з��չ���Ĭ�������ֽ�ȫΪ0��
	unsigned char ucBypassAllFlg;         //whether bypass all other pin when one pin has been bypassed 1-Yes, 0-No
}EMV_EXTMPARAM;

// MCK ���ò����ṹ
typedef struct 
{
	unsigned char  ucBypassPin; //0-��֧�֣�1��֧�֣�Ĭ��֧��
	unsigned char  ucBatchCapture;  // 0-online data capture, 1-batch capture
	void *pvoid;
}EMV_MCKPARAM;

// for PBOC С��֧��
// �����ն˵����ֽ���ز���
typedef struct 
{
	unsigned char ucECTSIFlg;   // TSI����? 1-���� �����ֽ��ն�֧��ָʾ�� ��EC Terminal Support Indicator��
	unsigned char ucECTSIVal;   // �����ֽ��ն�֧��ָʾ�� = 1,֧��
	unsigned char ucECTTLFlg;   // TTL����? 1-���� �����ֽ��ն˽����޶EC Terminal Transaction Limit��
	unsigned long ulECTTLVal;   // �����ֽ��ն˽����޶�
}EMV_TMECPARAM;

// add for clss pboc
typedef struct  
{
    unsigned long  ulAmntAuth;     // ��Ȩ���(unsigned long), ��Ϊ����, ��ý�������ulAmntOther�Ľ��
    unsigned long  ulAmntOther;    // �������(unsigned long) 
    unsigned long  ulTransNo;      // �������м�����(4 BYTE)
	unsigned char  ucTransType;    // ��������'9C', 0-����/���� 1-�ֽ�/����
	unsigned char  aucTransDate[4]; // �������� YYMMDD
	unsigned char  aucTransTime[4]; // ����ʱ�� HHMMSS
}Clss_TransParam;

typedef struct{
    int MaxLen;
    unsigned short Tag;
    unsigned short Attr;
	unsigned short usTemplate[2];// ������ģ�棬û����Ϊ0
	unsigned char ucSource;// ����ȡֵEMV_SRC_TM��EMV_SRC_ICC��EMV_SRC_ISS
}ELEMENT_ATTR;

#ifdef _LINUX_TERM
typedef struct
{
    uchar       Command[4];
    ushort      Lc;
    uchar       DataIn[512];
    ushort      Le;
}APDU_SEND;

typedef struct
{
    ushort      LenOut;
    uchar       DataOut[512];
    uchar       SWA;
    uchar       SWB;
}APDU_RESP;


typedef struct
{
     unsigned int  modlen;           //PIN���ܹ�Կģ����
     unsigned char mod[256];        //PIN���ܹ�Կģ��
     unsigned char exp[4];          //PIN���ܹ�Կָ��
     unsigned char iccrandomlen;    //�ӿ���ȡ�õ��������
     unsigned char iccrandom[8];    //�ӿ���ȡ�õ������
}RSA_PINKEY;


// PEDAPI ��������ʼֵ
#define PED_RET_ERR_START -300
// PED����������ȷ
#define PED_RET_OK 0
// û����PIN
#define PED_RET_ERR_NO_PIN_INPUT   (PED_RET_ERR_START-5) //
// ȡ������PIN
#define PED_RET_ERR_INPUT_CANCEL   (PED_RET_ERR_START-6) //
// ��������С����С���ʱ��
#define PED_RET_ERR_WAIT_INTERVAL  (PED_RET_ERR_START-7) //

// ����PIN��ʱ
#define PED_RET_ERR_INPUT_TIMEOUT  (PED_RET_ERR_START-15) //
// IC��������
#define PED_RET_ERR_NO_ICC          (PED_RET_ERR_START-16)//
// IC��δ��ʼ��
#define PED_RET_ERR_ICC_NO_INIT    (PED_RET_ERR_START-17) //
//wanmin add on 2012.4.02 end
#endif

//��Կ��Ӧ�ù���������
int  EMVCoreInit(void);
void EMVGetParameter(EMV_PARAM *tParam);
void EMVSetParameter(EMV_PARAM *tParam);
int  EMVGetTLVData(unsigned short Tag, unsigned char *DataOut, int *OutLen);
int  EMVSetTLVData(unsigned short Tag, unsigned char *DataIn, int DataLen);
int  EMVGetScriptResult(unsigned char *Result, int *RetLen);

//��Կ��Ӧ�ù���������
int  EMVAddApp(EMV_APPLIST *App);
int  EMVGetApp(int Index, EMV_APPLIST *App);
int  EMVDelApp(unsigned char *AID, int AidLen);
int  EMVModFinalAppPara(EMV_APPLIST *ptEMV_APP);
int  EMVGetFinalAppPara(EMV_APPLIST *ptEMV_APP);
int  EMVGetLabelList(APPLABEL_LIST  *ptAppLabel, int *pnAppNum); //add by nt 20091218

int  EMVAddCAPK(EMV_CAPK  *capk );
int  EMVGetCAPK(int Index, EMV_CAPK  *capk );
int  EMVDelCAPK(unsigned char KeyID, unsigned char *RID);
int  EMVCheckCAPK(unsigned char *KeyID, unsigned char *RID);

// ���޷���Ӧ�ó����ƹ�֤���б���������ӽӿ�
int  EMVAddRevocList(EMV_REVOCLIST *pRevocList);
int  EMVDelRevocList(unsigned char ucIndex, unsigned char *pucRID);
void EMVDelAllRevocList(void);

//���״���������
int  EMVAppSelect(int Slot,unsigned long TransNo);
int  EMVReadAppData(void);
int  EMVCardAuth(void);
int  EMVStartTrans(unsigned long ulAuthAmt, unsigned long ulCashBackAmt, unsigned char *pACType); //for Visa Online
int  EMVCompleteTrans(int nCommuStatus, unsigned char *paucScript, int *pnScriptLen, unsigned char *pACType); //for Visa Online

//����־ʱ����
//���Ӳ��� ucFlg, ����ȷ���Ƿ����������Ӧ��
int  EMVAppSelectForLog(int Slot, unsigned char ucFlg);
int  EMVAddIccTag(ELEMENT_ATTR tEleAttr[], int nAddNum);

//�ص���������
int  cEMVWaitAppSel(int TryCnt, EMV_APPLIST List[], int AppNum);  
int  cEMVInputAmount(unsigned long *AuthAmt, unsigned long *CashBackAmt);
int  cEMVGetHolderPwd(int TryFlag, int RemainCnt, unsigned char *pin);
int  cEMVReferProc(void);
int  cEMVUnknowTLVData(unsigned short Tag, unsigned char *dat, int len);
void cEMVAdviceProc(void);
void cEMVVerifyPINOK(void);
int  cEMVSetParam(void); //��Ӧ���������ӷ���ֵ����
// ��׼�ǽ�PBOC�ص����Ӵ�ʽ�ɽ��˻ص�����ʵ��Ϊ�պ���
unsigned char  cEMVPiccIsoCommand(unsigned char cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv);

//֤����֤(PBOC2.0)
int  cCertVerify(void);

//��������־(PBOC2.0)
int ReadLogRecord(int RecordNo);
int GetLogItem(unsigned short Tag, unsigned char *TagData, int *TagLen);

// ��ȡ������־,���ӡǩ����־
int EMVGetParamFlag(unsigned char ucParam, int *pnFlg);

// �������ֽ����ʱ����
int EMVGetCardECBalance(unsigned long *plBalance);
int EMVSetTmECParam(EMV_TMECPARAM *tParam);

// ����MCK�����еĲ��������ڽ�MCK�������ô����ں�
int EMVSetMCKParam(EMV_MCKPARAM *pMCKParam);
int EMVGetMCKParam(EMV_MCKPARAM *pMCKParam);// ��ȡ�ں˵�MCK����

// ��ȡEMV �ⷢ���汾�ż�����������Ϣ
int EMVReadVerInfo(char *paucVer);

//���������Ƿ�֧��advice�ı�־
void EMVSetConfigFlag(int nConfigflag);

// ��ʼ��EMV����Ԫֵ���ں�����Ĭ��ֵ��
void EMVInitTLVData(void);

// ��׼�ǽ�PBOCר�ýӿ�
// ���÷ǽ�PBOC����ѡ���Լ�GPO���ݵ��ں���
int EMVSwitchClss(Clss_TransParam *ptTransParam,unsigned char *pucSelData, int nSelLen, uchar *pucGPOData, int nGPOLen);

// ���ô���ں�
int EMVSetAmount(unsigned char * szAuthAmount, unsigned char * szCashBackAmount);

#ifndef _LINUX_TERM
int  EMVCoreVersion(void);

int  EMVProcTrans(void);

//����(�ر�)EMV��CORE�ĵ���״̬
void EMVSetDebug(int EnableFlag);
void EMVGetICCStatus(unsigned char *SWA, unsigned char *SWB);
int  cEMVOnlineProc(unsigned char *RspCode, unsigned char *AuthCode, int *AuthCodeLen, unsigned char *IAuthData, int *IAuthDataLen, unsigned char *Script, int *ScriptLen);

// �����Ƿ�ʹ��PCI �ѻ�PIN�����ӿ�
int EMVSetPCIModeParam(unsigned char ucPciMode, unsigned char *pucExpectPinLen,unsigned long ulTimeoutMs);

// ���EMV ���б���Ľ�����־��Ϣ�������ն˷��չ����е�����޶��鹦�ܣ�
int EMVClearTransLog(void);
#else
//ic�������
uchar cEMVIccIsoCommand(uchar ucslot, APDU_SEND *tApduSend, APDU_RESP *tApduRecv);	//IC�������������ú���֧��IC��ͨ�ýӿ�Э��(T=0��T=1)��

//���ġ�����PIN��֤
int cEMVPedVerifyPlainPin(uchar ucIccSlot, uchar *pucExpPinLenIn, uchar *ucIccRespOut, uchar ucMode,ulong ulTimeoutMs);
int cEMVPedVerifyCipherPin(uchar ucIccSlot, uchar *pExpPinLenIn, RSA_PINKEY *tRsaPinKeyIn, uchar *pucIccRespOut, uchar ucMode,ulong ulTimeoutMs);

#endif

#endif
