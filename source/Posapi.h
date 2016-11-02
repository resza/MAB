/*****************************************************/
/* Pax.h                                             */
/* Define the Application Program Interface          */
/* for all PAX POS terminals     		     */
/*****************************************************/

#ifndef  _PAX_POS_API_H
#define  _PAX_POS_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma warning( disable : 4005 )
#ifndef _PROPAY_
	#define SEEK_CUR	0
	#define	SEEK_SET	1
	#define	SEEK_END	2
#endif	// #ifdef _PROPAY_
#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
#define ushort unsigned short
#pragma warning( default : 4005 )


//============================================
//      key values
//============================================
#define KEYF1       0x01
#define KEYF2       0x02
#define KEYF3       0x03
#define KEYF4       0x04
#define KEYUP       0x05
#define KEYDOWN     0x06
#define KEYALPHA    0x07
#define KEYCLEAR    0x08
#define KEYF5       0x09
#define KEYBACKSPACE  0x09
#define KEYF6       0x0a
#define KEYENTER    0x0d
#define KEYMENU     0x14
#define KEYFN       0x15
#define KEYSETTLE   0x16
#define KEYVOID     0x17
#define KEYREPRINT  0x18
#define KEYPRNUP    0x19
#define KEYPRNDOWN  0x1a
#define KEYCANCEL   0x1b
#define KEY00       0x3a

#define KEY0        0x30
#define KEY1        0x31
#define KEY2        0x32
#define KEY3        0x33
#define KEY4        0x34
#define KEY5        0x35
#define KEY6        0x36
#define KEY7        0x37
#define KEY8        0x38
#define KEY9        0x39

#define KEYATM1     0x3B
#define KEYATM2     0x3C
#define KEYATM3     0x3D
#define KEYATM4     0x3E

#define NOKEY       0xff

#define FNKEY1      0x91
#define FNKEY2      0x92
#define FNKEY3      0x93
#define FNKEY4      0x94
#define FNKEY5      0x95
#define FNKEY6      0x96
#define FNKEY7      0x97
#define FNKEY8      0x98
#define FNKEY9      0x99
#define FNKEY0      0x90

#define FNKEYCLEAR  0x9a
#define FNKEYALPHA  0x9b
#define FNKEYUP     0x9c
#define FNKEYDOWN   0x9d
#define FNKEYMENU   0x9e
#define FNKEYENTER  0x9f
#define FNKEYCANCEL 0xa0
#define FNKEYF2     0xa5





//================================================
//              Define for the Power Save event
//================================================
#define EVENT_OVERTIME      0X01
#define EVENT_KEYPRESS      0X02
#define EVENT_MAGSWIPED     0X04
#define EVENT_ICCIN         0X08
#define EVENT_UARTRECV      0X10
#define EVENT_WNETRECV      0X20

//==================================================
//		MACROS for LCD functions
//===================================================
#define ASCII		0x00
#define CFONT		0x01
#define REVER		0x80


#define ICON_PHONE	   1	// phone �绰
#define ICON_SIGNAL	   2	// wireless signal �ź�
#define ICON_PRINTER   3	// printer ��ӡ��
#define ICON_ICCARD	   4	// smart card IC��
#define ICON_LOCK      5	// lock ��
#define ICON_SPEAKER   6	// speeker ������
#define ICON_BATTERY   6	// ���ͼ��
#define ICON_UP        7	// up ����
#define ICON_DOWN      8	// down ����

#define CLOSEICON	   0	// �ر�ͼ��[�������ͼ��]
#define OPENICON       1   // ��ʾͼ��[��Դ�ӡ����IC����������ء����ϡ�����]


//===========================================================
//           structure for smart card operation
//===========================================================
typedef struct{
   	unsigned char Command[4];
   	unsigned short Lc;
   	unsigned char  DataIn[512];
   	unsigned short Le;
}APDU_SEND;

typedef struct{
	unsigned short LenOut;
   	unsigned char  DataOut[512];
   	unsigned char  SWA;
   	unsigned char  SWB;
}APDU_RESP;


//================================================
//         structure for modem operation
//=================================================
typedef struct{
	unsigned char DP;
	unsigned char CHDT;
	unsigned char DT1;
	unsigned char DT2;
	unsigned char HT;
	unsigned char WT;
	unsigned char SSETUP;
	unsigned char DTIMES;
	unsigned char TimeOut;
	unsigned char AsMode;
}COMM_PARA;



//=============================================================================
// structure  and macros for ped operation begin
// nicm add 2008-07-25 being
//=============================================================================
typedef struct
{
	// ��ɢ����Կ��Դ��Կ����Կ���ͣ�PED_GMK,PED_TMK,PED_TPK,PED_TAK,PED_TDK, ���õ���ucDstKeyType���ڵ���Կ����
	uchar ucSrcKeyType;
	// ��ɢ����Կ��Դ��Կ����������һ���1��ʼ������ñ���Ϊ0�����ʾ�����Կ��д����������ʽ
	uchar ucSrcKeyIdx;
	// Ŀ����Կ����Կ���ͣ�PED_GMK,PED_TMK,PED_TPK,PED_TAK,PED_TDK
	uchar ucDstKeyType;
	// Ŀ����Կ����
	uchar ucDstKeyIdx;
	// Ŀ����Կ���ȣ�8,16,24
	int iDstKeyLen;
	// д����Կ������
	uchar aucDstKeyValue[24];
}ST_KEY_INFO;

typedef struct
{
  int iCheckMode;
  uchar aucCheckBuf[128];
}ST_KCV_INFO;

#define PED_BKLK  0x01  //BKLK
#define PED_TMK   0x02 //TMK
#define PED_TPK   0x03 //PIN Key
#define PED_TAK	  0x04 //MAC Key
#define PED_TDK   0x05 //DES Key
#define	PED_DUKPT 0X06
#define	PED_TPAK  0X07//(����TPK/TAK���õ�ģʽ)


//define��Կ���������붨��
#define PED_DOWNLOAD_CMD_TYPE 0X00
// ��ȡ�汾
#define PED_CMD_GETVER 0x00
// ����ϵͳ���з�����Ȩ����ȡ�����
#define PED_CMD_REQ_SYS_SSA	0X01
// ������������Ĳ���ȡ
#define PED_CMD_GET_TEK 0X02
// ��ʽ��ped
#define PED_CMD_FORMAT_PED 0X03//
 // д��MKSK ��Կ
#define PED_CMD_WRITEKEY 0x04//
// д��TIK
#define PED_CMD_WRITETIK 0x05//
// ������Կ��ǩ
#define PED_CMD_SETKEYTAG 0X06//
// PED�������
#define PED_CMD_DOWNLOAD_COMPLETE 0X07
 // ��ȡ�ն����к�
#define PED_CMD_READ_SN 0X08

// PEDAPI ��������ʼֵ
#define PED_RET_ERR_START -300
// PEDAPI ���������ֵ
#define PED_RET_ERR_END	-500
// PED����������ȷ
#define PED_RET_OK 0
// ��Կ������
#define PED_RET_ERR_NO_KEY (PED_RET_ERR_START-1)
// ��Կ�����������������ڷ�Χ��
#define PED_RET_ERR_KEYIDX_ERR     (PED_RET_ERR_START-2)
// ��Կд��ʱ��Դ��Կ�Ĳ�α�Ŀ����Կ��
#define PED_RET_ERR_DERIVE_ERR	   (PED_RET_ERR_START-3)
// ��Կ��֤ʧ��
#define PED_RET_ERR_CHECK_KEY_FAIL (PED_RET_ERR_START-4)
// û����PIN
#define PED_RET_ERR_NO_PIN_INPUT   (PED_RET_ERR_START-5)
// ȡ������PIN
#define PED_RET_ERR_INPUT_CANCEL   (PED_RET_ERR_START-6)
// ��������С����С���ʱ��
#define PED_RET_ERR_WAIT_INTERVAL  (PED_RET_ERR_START-7)
 // KCVģʽ����֧��
#define PED_RET_ERR_CHECK_MODE_ERR (PED_RET_ERR_START-8)
// ��Ȩʹ�ø���Կ����������Կ��ǩ���ԣ�����д����Կʱ,
// Դ��Կ���͵�ֵ����Ŀ����Կ���ͣ����᷵�ظ���Կ
#define PED_RET_ERR_NO_RIGHT_USE   (PED_RET_ERR_START-9)
// ��Կ���ʹ�
#define PED_RET_ERR_KEY_TYPE_ERR   (PED_RET_ERR_START-10)
// ����PIN�ĳ����ַ�����
#define PED_RET_ERR_EXPLEN_ERR     (PED_RET_ERR_START-11)
// Ŀ����Կ���������ڷ�Χ��
#define PED_RET_ERR_DSTKEY_IDX_ERR (PED_RET_ERR_START-12)
// Դ��Կ���������ڷ�Χ��
#define PED_RET_ERR_SRCKEY_IDX_ERR (PED_RET_ERR_START-13)
// ��Կ���ȴ�
#define PED_RET_ERR_KEY_LEN_ERR		(PED_RET_ERR_START-14)
// ����PIN��ʱ
#define PED_RET_ERR_INPUT_TIMEOUT  (PED_RET_ERR_START-15)
// IC��������
#define PED_RET_ERR_NO_ICC          (PED_RET_ERR_START-16)
// IC��δ��ʼ��
#define PED_RET_ERR_ICC_NO_INIT    (PED_RET_ERR_START-17)
// DUKPT�������Ŵ�
#define PED_RET_ERR_GROUP_IDX_ERR (PED_RET_ERR_START-18)
// ָ������Ƿ�Ϊ��
#define PED_RET_ERR_PARAM_PTR_NULL    (PED_RET_ERR_START-19)
// PED����
#define PED_RET_ERR_LOCKED (PED_RET_ERR_START-20)
// PEDͨ�ô���
#define PED_RET_ERROR (PED_RET_ERR_START-21)
// û�п��еĻ���
#define PED_RET_ERR_NOMORE_BUF (PED_RET_ERR_START-22)
// ��Ҫȡ�ø߼�Ȩ��
#define PED_RET_ERR_NEED_ADMIN (PED_RET_ERR_START-23)
// DUKPT�Ѿ����
#define PED_RET_ERR_DUKPT_OVERFLOW (PED_RET_ERR_START-24)
// KCV У��ʧ��
#define PED_RET_ERR_KCV_CHECK_FAIL (PED_RET_ERR_START-25)
// д����Կʱ��Դ��Կid�����Ͳ���Դ��Կ���Ͳ�ƥ��
#define PED_RET_ERR_SRCKEY_TYPE_ERR	(PED_RET_ERR_START-26)
// ���֧��
#define PED_RET_ERR_UNSPT_CMD  (PED_RET_ERR_START-27)
// ͨѶ����
#define PED_RET_ERR_COMM_ERR (PED_RET_ERR_START-28)
// û���û���֤��Կ
#define PED_RET_ERR_NO_UAPUK (PED_RET_ERR_START-29)
// ȡϵͳ���з���ʧ��
#define PED_RET_ERR_ADMIN_ERR (PED_RET_ERR_START-30)
// PED�������طǼ���״̬
#define	PED_RET_ERR_DOWNLOAD_INACTIVE (PED_RET_ERR_START-31)

//KCV ��У��ʧ��
#define PED_RET_ERR_KCV_ODD_CHECK_FAIL (PED_RET_ERR_START-32)
//��ȡPED����ʧ��
#define PED_RET_ERR_PED_DATA_RW_FAIL (PED_RET_ERR_START-33)
//����������(�ѻ����ġ�����������֤)
#define PED_RET_ERR_ICC_CMD_ERR      (PED_RET_ERR_START-34)
//�û���CLEAR���˳�����PIN
#define PED_RET_ERR_INPUT_CLEAR		(PED_RET_ERR_START-39)
//PED�洢�ռ䲻��
#define PED_RET_ERR_NO_FREE_FLASH	(PED_RET_ERR_START-43)
//DUKPT KSN��Ҫ�ȼ�1
#define PED_RET_ERR_DUKPT_NEED_INC_KSN	(PED_RET_ERR_START-44)
//KCV MODE����
#define PED_RET_ERR_KCV_MODE_ERR	(PED_RET_ERR_START-45)
//NO KCV
#define PED_RET_ERR_DUKPT_NO_KCV   (PED_RET_ERR_START-46)
//��FN/ATM4��ȡ��PIN����
#define PED_RET_ERR_PIN_BYPASS_BYFUNKEY		(PED_RET_ERR_START-47)
//����MACУ���
#define PED_RET_ERR_MAC_ERR			(PED_RET_ERR_START-48)
//����CRCУ���
#define PED_RET_ERR_CRC_ERR			(PED_RET_ERR_START-49)

//=============================================================================
// structure for ped operation
// nicm add 2008-07-25 end
//=============================================================================

//----------------------------------------------------------
// nicm add for new wireless module begin
//----------------------------------------------------------

#define WL_RET_ERR_PORTINUSE -201 // ģ��ڱ�ռ��
#define WL_RET_ERR_NORSP -202 // ģ��û��Ӧ��
#define WL_RET_ERR_RSPERR -203 // ģ��Ӧ�����
#define WL_RET_ERR_PORTNOOPEN -204 // ģ�鴮��û�д�
#define WL_RET_ERR_NEEDPIN -205	// ��ҪPIN��
#define WL_RET_ERR_NEEDPUK -206	// ��ҪPUK��PIN��
#define WL_RET_ERR_PARAMER -207	// ��������
#define WL_RET_ERR_ERRPIN -208 // �������
#define WL_RET_ERR_NOSIM -209 // û�в���SIM��
#define WL_RET_ERR_NOTYPE -210 // ��֧�ֵ�����
#define WL_RET_ERR_NOREG -211 // ����û��ע��
#define WL_RET_ERR_INIT_ONCE -212 // ģ���ѳ�ʼ��
#define WL_RET_ERR_LINEOFF -214	// ���ӶϿ�
#define WL_RET_ERR_TIMEOUT -216	// ��ʱ
#define WL_RET_ERR_REGING -222 // ����ע����
#define WL_RET_ERR_PORTCLOSE -223 // �رմ��ڳ���
#define WL_RET_ERR_MODEVER -225	// �����ģ��汾
#define WL_RET_ERR_DIALING -226	// ������
#define WL_RET_ERR_ONHOOK -227 // �ػ���
#define WL_RET_ERR_PPP_BRK -228	// ����PPP�Ͽ�
#define WL_RET_ERR_NOSIG -229 // �������ź�
#define WL_RET_ERR_POWEROFF -230 // ģ�����µ�
#define WL_RET_ERR_BUSY -231 // ģ��æ
#define WL_RET_ERR_OTHER -300 // ����δ֪����

#define	NET_OK               0  // �޴�������
#define	NET_ERR_MEM     	 -1  // �ڴ治��
#define	NET_ERR_BUF     	 -2  // ����������
#define	NET_ERR_ABRT    	 -3  // ��ͼ��������ʧ��
#define	NET_ERR_RST     	 -4  // ���ӱ��Է���λ���յ��Է���Reset��
#define	NET_ERR_CLSD    	 -5  // �����ѹر�
#define	NET_ERR_CONN    	 -6  // ����δ�ɹ�����
#define	NET_ERR_VAL     	 -7  // �������
#define	NET_ERR_ARG     	 -8  // �������
#define	NET_ERR_RTE     	 -9  // ����·��(route)
#define	NET_ERR_USE     	 -10 // ��ַ���˿�ʹ����
#define	NET_ERR_IF      	 -11 // �ײ�Ӳ������
#define	NET_ERR_ISCONN  	 -12 // �����ѽ���
#define	NET_ERR_TIMEOUT 	 -13 // ��ʱ
#define	NET_ERR_AGAIN   	 -14 // ������Դ�����ڣ�������
#define	NET_ERR_EXIST   	 -15 // �Ѵ���
#define	NET_ERR_SYS     	 -16 // ϵͳ��֧��
#define	NET_ERR_PASSWD  	 -17 // ��������
#define	NET_ERR_MODEM   	 -18 // ����ʧ��
#define	NET_ERR_LINKDOWN   	 -19 // ������·�ѶϿ��������²���
#define	NET_ERR_LOGOUT	   -20 // Logout
#define	NET_ERR_PPP	       -21 // PPP�Ͽ�
#define NET_ERR_STR        -22 // String Too Long, Illeage
#define NET_ERR_DNS        -23 // DNS Failure: No such Name
#define NET_ERR_INIT       -24 // No Init



//----------------------------------------------------------
// nicm add for new wireless module end
//----------------------------------------------------------

//=============================================
//     Asynchronism communication functions
//=============================================
#define COM1        	0
//#define COM2            2     P90
#define COM2        	1
#define RS485       	2
#define PINPAD      	3
#define IC_COMMPORT		4

#define BASECOM1    	0
#define BASECOM2    	1
#define BASERS485   	2
#define BASEPINPAD  	3
#define HANDSETCOM1 	4

#define RS232A			0
#define RS232B			1
#define LANPORT 		2
#define MODEM			4
#define WNETPORT        1

//==================================================
//     Macros for encription/decription fucntions
//==================================================
#define ENCRYPT 1
#define DECRYPT 0

//===========================================
//        Defined for file system
//============================================
#define	FILE_EXIST		1
#define	FILE_NOEXIST	2
#define	MEM_OVERFLOW	3
#define	TOO_MANY_FILES	4
#define	INVALID_HANDLE	5
#define	INVALID_MODE	6
#define NO_FILESYS		7
#define FILE_NOT_OPENED	8
#define	FILE_OPENED		9
#define END_OVERFLOW	10
#define TOP_OVERFLOW	11
#define NO_PERMISSION	12
#define FS_CORRUPT		13

#define	O_RDWR		0x01
#define	O_CREATE	0x02
#define	O_ENCRYPT   0x04

typedef struct
{
	unsigned char	fid;
	unsigned char	attr;
	unsigned char	type;
	char			name[17];
	unsigned long	length;
} FILE_INFO;

//extern int errno;
extern int _app_para;


//========================================================
//     MultiApplication functions
//========================================================
typedef struct {
unsigned char AppName[32];
unsigned char AID[16];
unsigned char AppVer[16];
unsigned char AppProvider[32];
unsigned char Descript[64];
unsigned char DownloadTime[14];
unsigned long MainAddress;
unsigned long EventAddress;
unsigned char AppNum;
unsigned char RFU[73];
}APPINFO;


#define MAGCARD_MSG   0x01
#define ICCARD_MSG    0x02
#define KEYBOARD_MSG  0x03
#define USER_MSG      0x04
#define TAPCARD_MSG    0x05


typedef struct{
	unsigned char RetCode;
	unsigned char track1[256];
	unsigned char track2[256];
	unsigned char track3[256];
}ST_MAGCARD;

typedef struct{
	int MsgType;		    //MAGCARD_MSG,ICCARD_MSG,KEYBOARD_MSG,USER_MSG
	ST_MAGCARD MagMsg;      //MAGCARD_MSG
	unsigned char KeyValue;  //ICCARD_MSG
	unsigned char IccSlot;   //KEYBOARD_MSG
	void *UserMsg;           //USER_MSG
}ST_EVENT_MSG;

//=================================================
//			  for RF card functions
//=================================================
typedef struct
{
	unsigned char  drv_ver[5];  //e.g. "1.01A", read only
	unsigned char drv_date[12]; //e.g. "2006.08.25",read only

	unsigned char a_conduct_w;  //Type A conduct write enable: 1--enable,else disable
	unsigned char a_conduct_val;//Type A output conduct value,0~63

	unsigned char m_conduct_w;  //M1 conduct write enable: 1--enable,else disable
	unsigned char m_conduct_val;//M1 output conduct value,0~63

	unsigned char b_modulate_w;  //Type B modulate index write enable,1--enable,else disable
	unsigned char b_modulate_val;//Type B modulate index value

	uchar card_buffer_w;//added in V1.00C,20061204
	ushort card_buffer_val;//max_frame_size of PICC

	uchar wait_retry_limit_w;//added in V1.00F,20071212
	ushort wait_retry_limit_val;//max retry count for WTX block requests,default 3
	// 20080617
	uchar card_type_check_w; // ��Ƭ���ͼ��д������1--��������ֵ--��������Ҫ���ڱ�����Ƭ���淶���������

	uchar card_type_check_val; // 0-��鿨Ƭ���ͣ�����������鿨Ƭ����(Ĭ��Ϊ��鿨Ƭ����)

	//uchar reserved[92];  //�����ֽڣ����ڽ�����չ��д��ʱӦȫ����

	//2009-10-30
	uchar card_RxThreshold_w; // ��Ƭ���ͼ��д������1--��������ֵ--��������Ҫ���ڱ�����Ƭ���淶���������

	uchar card_RxThreshold_val;//��Ƭ����������

	//2009-11-20

     uchar f_modulate_w; // felica�������д������

     uchar f_modulate_val;//felica�������

	 //add by wls 2011.05.17
	 unsigned char a_modulate_w; // A������ָ��д������1--��������ֵ��������
	 unsigned char a_modulate_val; // A������ָ�����Ʊ�������Ч��Χ0~63,����ʱ��Ϊ63

		//add by wls 2011.05.17
	 unsigned char a_card_RxThreshold_w; //���������ȼ��д������1--��������ֵ--������
	 unsigned char a_card_RxThreshold_val;//A������������

	 //add by liubo 2011.10.25, ���A,B��C������������
	 unsigned char a_card_antenna_gain_w;
	 unsigned char a_card_antenna_gain_val;

	 unsigned char b_card_antenna_gain_w;
	 unsigned char b_card_antenna_gain_val;

	 unsigned char f_card_antenna_gain_w;
	 unsigned char f_card_antenna_gain_val;

	 //added by liubo 2011.10.25�����Felica�Ľ���������
	 unsigned char f_card_RxThreshold_w;
	 unsigned char f_card_RxThreshold_val;

	 unsigned char reserved[76]; //�����ֽڣ����ڽ�����չ��д��ʱӦȫ����


}PICC_PARA;


//-----------------------------------------------------------------
// nicm add for R50 ��Ƶ��ģ������ Desfire ����֧�� 2008-09-23 begin
//-----------------------------------------------------------------
//  Structure for returning the information supplied by the GetVersion command.
typedef struct
{
    uchar    ucHwVendorID;
    uchar    ucHwType;
    uchar    ucHwSubType;
    uchar    ucHwMajorVersion;
    uchar    ucHwMinorVersion;
    uchar    ucHwStorageSize;
    uchar    ucHwProtocol;

    uchar    ucSwVendorID;
    uchar    ucSwType;
    uchar    ucSwSubType;
    uchar    ucSwMajorVersion;
    uchar    ucSwMinorVersion;
    uchar    ucSwStorageSize;
    uchar    ucSwProtocol;

    uchar    aucUid[ 7 ];
    uchar    aucBatchNo[ 5 ];
    uchar    ucProductionCW;
    uchar    ucProductionYear;
}VERSION_INFO;

typedef struct
{
//	uchar ucFileNo;
	uchar ucCommSetting;
	uchar aucAccRight[2];
	uchar aucFileSize[3];
}STR_DATAFILE;

typedef struct
{
//	uchar ucFileNo;
	uchar ucCommSetting;
	uchar aucAccRight[2];
	uchar aucLowerLmt[4]; // �з�������, ����Ϊ����
	uchar aucUpperLmt[4]; // �з�������, ����Ϊ����
	uchar aucValue[4]; // �з�������, ����Ϊ����
	uchar ucLmtCrdtEnable;
}STR_VALUEFILE;

typedef struct
{
//	uchar ucFileNo;
	uchar ucCommSetting;
	uchar aucAccRight[2];
	uchar aucRecSize[3];
	uchar aucMaxRecNum[3];
	uchar aucCurRecNUm[3];
}STR_RECORDFILE;

typedef struct
{
	uchar ucFileNo; // file ID
	uchar aucOffset[3];
	uchar aucNumToRead[3]; // data num or record num
	uchar aucSize[3]; // ������Read Record, ����֪�ļ���¼�Ĵ�С, ��δ֪, ����Ϊ0.
}STR_READ_DATA_RECORD;

typedef struct {
	int iModulusLen;// the length of modulus bits.
	uchar aucModulus[512]; //Modulus, if the  length of the Modulus is less than 512bytes, It is padded with 0x00 on the left.
	int iExponentLen; //the length of exponent bits
	uchar aucExponent[512];// Modulus, if the  length of the Exponent is less than 512bytes, It is padded with 0x00 on the left.
	uchar aucKeyInfo[128];// RSA key info, defined by application.
}ST_RSA_KEY;


//-----------------------------------------
// DESFIRE CARD status and error codes
// ���ӿ���*pucStatus�л��͵�����
//-----------------------------------------
#define STAT_DESFIRE_NO_CHANGES        0x0c
// Insufficient NV-Memory to complete command
#define STAT_DESFIRE_E2PROM_OVERFL     0x0E
// Command code not supported
#define STAT_DESFIRE_CMD_ERR           0x1C
// CRC/MACdoes not match data, Padding bytes not valid
#define STAT_DESFIRE_INTEG_ERR         0x1E
// Invalid key number specified
#define STAT_DESFIRE_NO_SUCH_KEY       0x40
// length of command string invalid
#define STAT_DESFIRE_LENGTH_ERR        0x7E
// current config / status does not allow the requested command
#define STAT_DESFIRE_PERMISSION_DENIED 0x9D
// value of the param invalid
#define STAT_DESFIRE_PARAM_ERR         0x9E
// requested AID not present on PICC
#define STAT_DESFIRE_APP_NOT_FOUND     0xA0
// unrecoverable error within application, application will be disabled(���ڰ�ȫ�����з���)
#define STAT_DESFIRE_APP_INTEG_ERR     0xA1
// current authentication status does not allow the requested command
#define STAT_DESFIRE_AUTH_ERR          0xAE
// additional data frame is expected to be sent
#define STAT_DESFIRE_ADDITIONAL_FRAME  0xAF
// attempt to read/write data from/to beyond the file's/record's limits.
// attempt to exceed the limits of a value file
#define STAT_DESFIRE_BOUNDARY_ERR      0xBE
// unrecoverable error within PICC, PICC will be disabled(�����ڰ�ȫ������)
#define STAT_DESFIRE_PICC_INTEG_ERR    0xC1
// previous command was not fully completed
// not all frames were requested or provided by the PCD
#define STAT_DESFIRE_CMD_ABORTED       0xCA
// PICC was disabled by an unrecoverable error(���ڰ�ȫ�����з���)
#define STAT_DESFIRE_PICC_DISABLED_ERR 0xCD
// number of app limited to 28, no additional CreateApplication possible
#define STAT_DESFIRE_COUNT_ERR         0xCE
// Creation of file/app failed because file/app with same number alredy exists
#define STAT_DESFIRE_DUPLICATE_ERR     0xDE
// could not complete NV-write operation due to loss of power,
// internal backup/rollback mechanism activated(���ڰ�ȫ�����з���)
#define STAT_DESFIRE_E2PROM_ERR        0xEE
// specified file number does not exist
#define STAT_DESFIRE_FILE_NOT_FOUND    0xF0
// unrecoverable error within file, file will be disabled(���ڰ�ȫ�����з���)
#define STAT_DESFIRE_FILE_INTEG_ERR    0xF1

#define MAX_RSA_MODULUS_BITS 2048

#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

//-----------------------------------------------------------------
// nicm add for R50 ��Ƶ��ģ������ Desfire ����֧�� 2008-09-23 end
//-----------------------------------------------------------------


typedef struct
{
	unsigned char auth_id;			//��֤��
	unsigned char identify_code;	// ʶ����
	unsigned char masterkey_id;		// ����Կ����
	unsigned char pinkey_id;		// PIN��Կ����
	unsigned char key_mode;			// ��Կģʽ
	unsigned char pin_key[24];		// ��Կ
	unsigned char process_mode;		// �ӽ���ģʽ
} PIN_KEY;

typedef struct
{
	unsigned char auth_id;			// ��֤��
	unsigned char identify_code;	// ʶ����
	unsigned char masterkey_id;		// ����Կ����
	unsigned char mackey_id;		// MAC��Կ����
	unsigned char key_mode;			// ��Կģʽ
	unsigned char mac_key[24];		// ��Կ
	unsigned char process_mode;		// �ӽ���ģʽ
} MAC_KEY;


typedef struct
{
	unsigned char auth_id;			//��֤��
	unsigned char identify_code;	//ʶ����
	unsigned char masterkey_id;		// ����Կ����
	unsigned char key_mode;			// ��Կģʽ
	unsigned char master_key[24];	// ��Կ
} MASTER_KEY;

typedef struct
{
     unsigned char auth_id;			// ��֤��
     unsigned char identify_code;	// ʶ����
     unsigned char MKeyID;			// ����Կ����
     unsigned char WKeyID1;			// PIN��MAC��Կ����
     unsigned char WKeyID2;			// PIN��MAC��Կ����
     unsigned char mode;			// ��Կģʽ
	 unsigned char keytype;			// ��Կ����
}DERIVE_KEY;


typedef struct
{
	unsigned char auth_id;			// ��֤��
	unsigned char identify_code;	// ʶ����
	unsigned char input_mode;		// ���뷽ʽ
	unsigned char key_id;			// PIN��Կ����
	unsigned char encrypt_mode;		// ����ģʽ
	unsigned char min_len;			// PIN��С����
	unsigned char max_len;			// PIN��󳤶�
	unsigned char card_no[16];		// ����

} PIN_ANSIX98;

typedef struct
{
	unsigned char auth_id;			// ��֤��
	unsigned char identify_code;	// ʶ����
	unsigned char input_mode;		// ���뷽ʽ
	unsigned char key_id;			// PIN��Կ����
	unsigned char encrypt_mode;		// ����ģʽ
	unsigned char min_len;			// PIN��С����
	unsigned char max_len;			// PIN��󳤶�
	unsigned char ISN[6];			// ISN

} PIN_ANSIX392;

typedef struct
{
     unsigned char auth_id;			// ��֤��
     unsigned char identify_code;	// ʶ����
     unsigned char mackeyid;		// MAC��Կ����
     unsigned char mode;			// �����㷨
     unsigned short inLen;			// MAC��������ݰ��ĳ���
     unsigned char datain[2048];	// �����MAC��������ݰ�
     unsigned char flag;			// MAC�㷨
} MAC_PACKAGE;

// donot support anymore.
// typedef struct
// {
//      unsigned int  modlen;           //PIN���ܹ�Կģ����
//      unsigned char mod[256];        //PIN���ܹ�Կģ��
//      unsigned int  explen;           //PIN���ܹ�Կָ����
//      unsigned char exp[4];          //PIN���ܹ�Կָ��
//      unsigned char iccrandomlen;    //�ӿ���ȡ�õ��������
//      unsigned char iccrandom[8];    //�ӿ���ȡ�õ������
//      unsigned int  termrandomlen;   //���ն�Ӧ��ȡ�õ��������
//      unsigned char termrandom[256]; //���ն�Ӧ��ȡ�õ������
// } RSA_PINKEY;

// ͨѶ��������
typedef struct
{
    unsigned char bCommMode;        // 0:Serial, 1:Modem, 2:LAN, 3:GPRS, 4:CDMA, 5:PPP
                           // ĿǰԶ������ֻ֧�ִ��ڡ�modem��LAN��PPP����ͨѶģʽ��
                           // ����֧��GPRS��CDMA����ͨѶģʽ  2005-12-22

    unsigned char *psAppName;       // Ӧ��������Ϊ�����������е�Ӧ�ü������������ļ�
    unsigned char bLoadType;        // �������ͣ�bit0:Ӧ�ã�bit1:�����ļ���bit2:�����ļ�
    unsigned char psProtocol;       // Э����Ϣ��0-protims��1-ftims��2-tims
    unsigned char bPedMode;         // 0-����PED����PP20����1������PED��P60��S1��P80��
                           // Ŀǰ�ò�����ProTims��Tims�в�ʹ�ã�ֻ��FTims��ʹ��

    unsigned char bDMK_index;       // ����Կ������ProTims�в�ʹ��
    unsigned char sTransCode[5];    // �����룬ֻ��FTims����ʹ�ã�ProTims��Tims�в�ʹ��
                           // ȡֵ     ����
                           // "2001"����Ӧ�����ؽ���
                           // "2011"����Ӧ�����ؽ���
                           // "1000"����Ӧ��POS��ѯ��������
                           // "1005"����Ӧ��POS��ѯ��������һ��ͨѶ�����
                           // "1010"����Ӧ��POS��ѯ��������
                           // "1015"����Ӧ��POS��ѯ��������һ��ͨѶ�����

    unsigned char ucCallMode;    // ����ģʽ��Ŀǰ�ò����ڹ��ڰ�ProTims��FTims��Tims��
                        // ��ʹ�� 2005-09-26
    // ���°溣���ProTims��ʹ�øò�������Ҫ������Ǳ߿�ʼʹ��P80 2005-10-17
    // Ӧ�õ��ñ��ӿڵĿ��ơ�bit0~bit3: 0������Ա����Ҫ��
    // 1�����ƻ���ʱ��
    // 2����Զ�̷��������У�����monitor����server�ĸ���֪ͨ
    // ��Ϣ(�ӿڷ���ʱ��monitor���ܶ���)��
    // 3��(v0.8.0)��Զ�̷��������У�����monitor��Զ�����ز�����
    // ���bit4~bit7=0, P70��monitor��Ҫ���ڲ����ڲ���
    // modem��
    // bit4~bit7: 0��monitor����Ҫ����ͨѶ���ӣ�1��monitor
    // �Լ���������

    unsigned char *psTermID;     // �ն˺ţ�Ŀǰ�ò����ں���汾PtoTims���°汾��
                        // ����ProTims��ʹ�� 2006-04-18
    union
    {
        struct
        {
            unsigned char *psPara; //����ͨѶ��������ʽ��"57600,8,n,1"
        }tSerial;  // ����ͨѶ����������bCommMode=0ʱ�ò�������Ч
        struct
        {
            unsigned char *psTelNo;         //�绰���룬��ϸ�ο�ModemDial()����
            COMM_PARA *ptModPara;  //ModemͨѶ����
            unsigned char bTimeout;         //���ųɹ������ʱ[��]
        }tModem;  // modemͨѶ����������bCommMode=1ʱ�ò�������Ч
        struct
        {
            unsigned char *psLocal_IP_Addr;       // ����IP��ַ����TCP/IP��ʽ�±���ʹ��
            unsigned short wLocalPortNo;           // ���ض˿ںţ����ڲ�ʹ�ã�IP80ģ����Զ�ʹ��Ĭ�϶˿ں�
            unsigned char *psRemote_IP_Addr;      // Զ��IP��ַ����TCP/IP��ʽ�±���ʹ��
            unsigned short wRemotePortNo;          // Զ�̶˿ںţ���TCP/IP��ʽ�±���ʹ��
            unsigned char *psSubnetMask;          //   �������룬��TCP/IP��ʽ�±���ʹ��  2006-04-21
            unsigned char *psGatewayAddr;         //       ���أ���TCP/IP��ʽ�±���ʹ��      2006-04-21
        }tLAN; // TCP/IPЭ��ͨѶ����������bCommMode=2ʱ�ò�������Ч
        struct
        {
            unsigned char *psAPN;          //APN
            unsigned char *psUserName;     //�û���
            unsigned char *psPasswd;       //�û�����
            unsigned char *psIP_Addr;      //IP��ַ
            unsigned short nPortNo;         //�˿ں�
            unsigned char *psPIN_CODE;     //���SIM�������룬��Ҫ����PIN��
            unsigned char *psAT_CMD;       //�����AT����Ļ�
        }tGPRS; // GSM�ֻ�ͨѶ����������bCommMode=3ʱ�ò�������Ч
        struct
        {
            unsigned char *psTelNo;        //�绰����
            unsigned char *psUserName;     //�û���
            unsigned char *psPasswd;       //�û�����
            unsigned char *psIP_Addr;      //IP��ַ
            unsigned short nPortNo;         //�˿ں�
            unsigned char *psPIN_CODE;     //���SIM�������룬��Ҫ����PIN��
            unsigned char *psAT_CMD;       //�����AT����Ļ�
        }tCDMA; // CDMA�ֻ�ͨѶ����������bCommMode=4ʱ�ò�������Ч
        struct
        {
            unsigned char *psTelNo;        // �绰����
            COMM_PARA *ptModPara; // MODEMͨ�Ų���
            unsigned char *psUserName;     // �û���
            unsigned char *psPasswd;       // �û�����
            unsigned char *psIP_Addr;      // IP��ַ
            unsigned short nPortNo;         // �˿ں�
        }tPPP; // ����modem�ĵ�Ե�(Point to Point Protocol)Զ������ͨѶ������
               // ����bCommMode=5ʱ�ò�������Ч

		struct //WIFI ����bCommMode=6ʱ�ò�������Ч
		{

			unsigned char *Wifi_SSID;        // SSID
			unsigned char *psPasswd;       // �û�����

			unsigned char *Local_IP; // ����IP

			unsigned short Local_PortNo;

			unsigned char *Remote_IP_Addr;      // Զ��IP��ַ
			unsigned short RemotePortNo;          // Զ�̶˿ں�
			unsigned char *SubnetMask;          //   ��������
			unsigned char *GatewayAddr;         //       ����
			unsigned char *Dns1;
			unsigned char *Dns2;
			unsigned char Encryption_Mode; //  1: WEP 64(ASCII); 2 WEP 128 (ASCII);    3  WPA_TKIP;  4 WPA_AES
			unsigned char Encryption_Index; // WEPʱ��ȡֵ��Χ:1~4
			unsigned char DHCP_Flag; // 1:ʹ��DHCP
        }tWIFI;
    }tUnion;
}T_INCOMMPARA;

typedef struct
{
    uchar app_name[33];    // Ӧ������
    uchar app_ver[21];     // Ӧ�ð汾
    uchar have_param;      // ���޲����ļ�
    uchar param_name[17];  // �����ļ���
    uchar param_ver[21];   // �����ļ��汾
    uchar UpdatedAppMode;  // 0-�������� 1-��������ʱ�ļ���
    uchar UpdatedParaMode; // 0-�������� 1-��������ʱ�ļ���
    uchar AppActiveStatus; // 0-δ��� 1-�Ѽ���
    uchar reserve[4];      // �����ֽ�
}LOADLOG_INFO; // 100byte

typedef struct
{
	uchar bAppTotal;           // POS���е�Ӧ������
	uchar sAppNum[25];        // ÿ��Ӧ�õ����
	uchar sLoadAppStat[25];  // ��Ӧ��ÿ��Ӧ�õ�����״̬
	uchar sLoadParaStat[25];  // ��Ӧ��ÿ��Ӧ���в���/�����ļ�������״̬
	uchar sAppName[25][33];   // Ӧ�����ƻ���
}T_LOADSTATUS;

typedef struct {
	unsigned short int bits;                     /* length in bits of modulus */
	unsigned char modulus[MAX_RSA_MODULUS_LEN];  /* modulus */
	unsigned char exponent[MAX_RSA_MODULUS_LEN]; /* public exponent */
}R_RSA_PUBLIC_KEY;

typedef struct {
	unsigned short int bits;								/* length in bits of modulus */
	unsigned char modulus[MAX_RSA_MODULUS_LEN];				/* modulus */
	unsigned char publicExponent[MAX_RSA_MODULUS_LEN];		/* public exponent */
	unsigned char exponent[MAX_RSA_MODULUS_LEN];			/* private exponent */
	unsigned char prime[2][MAX_RSA_PRIME_LEN];				/* prime factors */
	unsigned char primeExponent[2][MAX_RSA_PRIME_LEN];		/* exponents for CRT */
	unsigned char coefficient[MAX_RSA_PRIME_LEN];			/* CRT coefficient */
}R_RSA_PRIVATE_KEY;

enum CHANNEL_NO
{
	P_RS232A=0,
	P_RS232B,
	P_WNET,
	P_PINPAD,
	P_MODEM,
	P_WIFI,
	CHANNEL_LIMIT,
    P_USB=10,
    P_USB_DEV,
    P_USB_HOST
};


/**********************************************/
/*                                            */
/*������������������ֵһ������              */
/*                                            */
/**********************************************/
/*
   0x00	  ��ȷ
   0x01	  ���ݰ����ȷǷ�
   0x02	  ��Կ�����Ƿ�
   0x03	  �Ƿ�ģʽ
   0x04	  ��Ļ����Ƿ�
   0x05	  �ַ��������볤�ȷ�Χ�Ƿ�
   0x06	  ����Աȡ������
   0x07	  ���볬ʱ
   0x08	  ��ʾ���󳤶ȷǷ�
   0x09	  �������벻һ��
   0x0a	  �û�δ�������루��������ĳ���Ϊ0��
   0x0b	  ָ������Կ������
   0x0c   �ڴ��
   0x0d	  ��У���
   0x0e	  TMK��
   0x0f	  "����MAC"��
   0x10	  �˵���ŷǷ�
   0x11	  ��Կ���鲻�Ϸ�

   0x12   Ӧ���ⲿ��֤��Կ�ѳ�ʼ��
   0x13   Ӧ����Ŀ���
   0x14   Ӧ�ò�����
   0x15   �ⲿ��֤ʧ��
   0x16   û�н����ⲿ��֤
   0x17   PINר����Կ̫��
   0x18   ��Ӧ������(��Ӧ�����Ƴ�����
   0x19   �ⲿ��֤��������������ʧ�ܳ���15�Σ�
   0x1a   ������С��30��

   0xc6	  �û�ȡ������
   0xce	  PED����
   0xcf	  ��PED����ʧ��

*/



//Module status related macros
#define WNET_UNKNOWN	0x00
#define WNET_CDMA		0x01
#define WNET_GPRS		0x02
//#define WNET_DETACHED	0x00
//#define WNET_ATTACHED	0x01
//Port related macros
#define COMNUM			0x02	//Serial port number, CPU to Module
//#define COMNUM              0x01    //Serial port number, CPU to Module P90

#define PORT_CLOSED		0x00
#define PORT_OPENED		0x01

#define PSEND_OK		0x00	//serial port send successfully
#define PSEND_ERR		0x01	//serial port send error
#define PRECV_UNOPEN	-2
#define PRECV_ERR		-1		//serial port receive error
#define PRECV_TIMEOUT	0		//serial port receive timeout

//General macros
#define RET_OK			0x00	//return successfully
#define RET_ERR			0x01	//error occurred
#define RET_NORSP		0x02	//no response from the module
#define RET_RSPERR		0x03	//"ERROR" is return from the module
#define RET_NOSIM		0x04	//SIM/UIM card is not inserted or not detected
#define RET_NEEDPIN		0x05	//SIM PIN is required
#define RET_NEEDPUK		0x06	//SIM PUK is required
#define RET_SIMBLOCKED	0x07	//SIM Card is permanently blocked
#define RET_SIMERR		0x08	//SIM card does not exist or needs SIM PIN
#define RET_PINERR		0x09	//SIM PIN is incorrect
#define RET_NOTDIALING	0x0A	//the module is not in dialing status
#define RET_PARAMERR	0x0B	//parameter error
#define RET_FORMATERR	0x0C	//Format error
#define RET_SNLTOOWEAK	0x0D	//the signal is too weak, please check the antenna
#define RET_LINKCLOSED	0x0E	//the module is offline
#define RET_LINKOPENED	0x0F	//the module is online
#define RET_LINKOPENING	0x10	//the module is connecting to the network
#define RET_TCPCLOSED	0x11	//tcp socket is closed
#define RET_TCPOPENED	0x12	//tcp socket is opened
#define RET_TCPOPENING	0x13	//the module is trying to open a TCP socket
#define RET_ATTACHED	0x14	//Attached
#define RET_DETTACHED	0x15	//Dettached
#define RET_ATTACHING	0x16	//the module is looking for the base station.
#define RET_NOBOARD		0x17	//no GPRS or CDMA board exist
#define RET_UNKNOWNTYPE     0x18    //unknown type
#define RET_EMERGENCY       0x19    //SIM/UIM is in emergency status

#define RET_RING            0x1A    //  detect ringing
#define RET_BUSY            0x1B    //  detect call in progress
#define RET_DIALING         0x1C    //  dialing

#define RET_PORTERR		0xFD	//serial port error
#define RET_PORTINUSE	0xFE	//serial port is in use by another program
#define RET_ABNORMAL	0xFF	//abnormal error


/**********************************************
            Functions Declaration
**********************************************/

typedef struct
{
	uchar connected;
	uchar remote_closed;
	uchar has_data_in;
	uchar outbuf_emptied;
	ushort left_outbuf_count;
	uchar remote_addr[4];
	uchar reserved1[30];
	long link_state;
	uchar local_addr[4];
	uchar dns1_addr[4];
	uchar dns2_addr[4];
	uchar gateway_addr[4];
	uchar subnet_mask[4];
	uchar reserved2[30];
}PPP_NET_INFO;


//================================================
//	     PPP connection functions
//================================================
typedef struct
{
  uchar IsSync;//0--async connect,1--sync connect
  uchar ConnectTimeout;//connect timeout for a single no.,0--default to 30s
  uchar SpeedType;//0-default to highest of the GSM chip,for g20 async is 9600
							//1-1200,2-2400,3-4800,4-7200,5-9600,6-12000,7-14400
							//8-19.2k,9-24k,10-26.4k,11-28.8k,12-31.2k,13-33.6k
							//14-48k,15-56k
  uchar AsyncFormat;//0-N81,1-E81,2-O81,3-N71,4-E71,5-O71
  uchar IdleTimeout;//unit:10s,0--no auto hangup
  uchar DialTimes;//dial times for a single number,0--default to 1
  uchar DisableXonXoff;//0-enable XonXoff software flow control,1-disable
  uchar ServiceType;//0-PPP,1-normal ASYNC,other--reserved
  uchar Reserved[50];//must all filled with 0
}GSM_CALL;

//---------------------------
// 2009-08-10 add ssl api
//---------------------------

typedef struct
{
    void * ptr;/* �ڴ�ָ��*/
	int  size;/* �ռ��С*/
} ST_SSL_BUF;

#define SSL_OPER_OK			0
#define SSL_BEYOND_CERF_NUM	-401//֤���������
#define SSL_BEYOND_SERV_IP	-404//̫�����ˣ�����10��
#define SSL_FILE_OPER_FAIL	-405//�ļ�����ʧ��,
#define SSL_CER_NO_EXIST	-406//֤�鲻����
#define SSL_CER_NOSPACE		-407//֤��ռ䲻��
#define SSL_NULL_ERR		-408//����Ϊ�Ƿ��մ�


//================================================
//            Printer functions
//================================================
#define     PRN_OK              0x00
#define     PRN_BUSY            0x01
#define     PRN_PAPEROUT        0x02
#define     PRN_WRONG_PACKAGE   0x03
#define     PRN_FAULT           0x04
#define     PRN_TOOHEAT         0x08
#define     PRN_UNFINISHED      0xf0
#define     PRN_NOFONTLIB       0xfc
#define     PRN_OUTOFMEMORY     0xfe

//================================================
//            Bluetooth functions
//================================================
//BT functions
#define STRING_LEN 64

#define COM_BT	7

#define BT_ROLE_MASTER	1
#define BT_ROLE_SLAVER	0
#define BT_STATE_NOTCONNECT 0
#define BT_STATE_CONNECT 1
//=================================================

//================================================
//            Wifi functions
//================================================
//wifi
#define WIFI_ROUTE_NUM 12

#define WLAN_SEC_UNSEC (0)
#define WLAN_SEC_WEP	(1)
#define WLAN_SEC_WPA_WPA2	(2)
#define WLAN_SEC_WPAPSK_WPA2PSK	(3)

#define IPLEN 4
#define KEY_WEP_LEN 5
#define KEY_WPA_MAXLEN 63
#define SSID_MAXLEN 32
typedef struct
{
	int DhcpEnable;	//DHCPʹ�ܣ�0-�رգ�1-����
	uchar Ip[IPLEN];	//���þ�̬IP
	uchar Mask[IPLEN];	//����
	uchar Gate[IPLEN];	//����
	uchar Dns[IPLEN];	//DNS
	uchar Wep[KEY_WEP_LEN];	//wep����
	uchar Wpa[KEY_WPA_MAXLEN];	//WPA��������
	uchar reserved[256];//Ԥ��
}ST_WIFI_PARAM;
typedef struct
{
	uchar Ssid[SSID_MAXLEN];		//��������AP��SSID
	int SecMode;	//��ȫģʽ
	int Rssi;	//��������AP���ź�ǿ��
}ST_WIFI_AP;


//================================================
//            ��ɫ��صĶ���
//================================================
typedef int COLORREF;
#define RGB(r, g, b) ((COLORREF)((((unsigned int)((r&0xF8) >> 3)) << 11) | (((unsigned int)((g&0xFC) >> 2)) << 5) | ((unsigned char)((b&0xFC) >> 3))))
#define RGB_R(rgb) ((unsigned char)(((rgb)>>11) << 3))
#define RGB_G(rgb) ((unsigned char)((rgb)>>5) << 2)
#define RGB_B(rgb) ((unsigned char)((rgb) << 3))
#ifndef NO_FILL_COLOR
#define NO_FILL_COLOR 0xffffffff
#endif

#include "posapi_p78.h"
#include "posapi_p80.h"
#include "posapi_p90.h"
#include "posapi_s80.h"
#include "posapi_all.h"
#include "posapi_s78.h"

#ifdef __cplusplus
extern "C"
{
#endif

	uchar POS_API _getkey(void);
	void POS_API PiccLight(unsigned char ucLedIndex,unsigned char ucOnOff);

	/************************************************************\
	*															*
	* the following functions or variables are not implemented  *
	* or defined in the lib. it must be defined in the project	*
	* which will use the lib.									*
	*															*
	\************************************************************/
	extern const APPINFO AppInfo;
	extern int event_main(ST_EVENT_MSG *msg);
	extern int main();

#ifdef __cplusplus
};
#endif


#endif
