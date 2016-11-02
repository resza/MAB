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


#define ICON_PHONE	   1	// phone 电话
#define ICON_SIGNAL	   2	// wireless signal 信号
#define ICON_PRINTER   3	// printer 打印机
#define ICON_ICCARD	   4	// smart card IC卡
#define ICON_LOCK      5	// lock 锁
#define ICON_SPEAKER   6	// speeker 扬声器
#define ICON_BATTERY   6	// 电池图标
#define ICON_UP        7	// up 向上
#define ICON_DOWN      8	// down 向下

#define CLOSEICON	   0	// 关闭图标[针对所有图标]
#define OPENICON       1   // 显示图标[针对打印机、IC卡、锁、电池、向上、向下]


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
	// 发散该密钥的源密钥的密钥类型，PED_GMK,PED_TMK,PED_TPK,PED_TAK,PED_TDK, 不得低于ucDstKeyType所在的密钥级别
	uchar ucSrcKeyType;
	// 发散该密钥的源密钥索引，索引一般从1开始，如果该变量为0，则表示这个密钥的写入是明文形式
	uchar ucSrcKeyIdx;
	// 目的密钥的密钥类型，PED_GMK,PED_TMK,PED_TPK,PED_TAK,PED_TDK
	uchar ucDstKeyType;
	// 目的密钥索引
	uchar ucDstKeyIdx;
	// 目的密钥长度，8,16,24
	int iDstKeyLen;
	// 写入密钥的内容
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
#define	PED_TPAK  0X07//(用于TPK/TAK混用的模式)


//define密钥下载命令码定义
#define PED_DOWNLOAD_CMD_TYPE 0X00
// 读取版本
#define PED_CMD_GETVER 0x00
// 请求系统敏感服务授权并读取随机数
#define PED_CMD_REQ_SYS_SSA	0X01
// 回送随机数密文并获取
#define PED_CMD_GET_TEK 0X02
// 格式化ped
#define PED_CMD_FORMAT_PED 0X03//
 // 写入MKSK 密钥
#define PED_CMD_WRITEKEY 0x04//
// 写入TIK
#define PED_CMD_WRITETIK 0x05//
// 设置密钥标签
#define PED_CMD_SETKEYTAG 0X06//
// PED下载完成
#define PED_CMD_DOWNLOAD_COMPLETE 0X07
 // 读取终端序列号
#define PED_CMD_READ_SN 0X08

// PEDAPI 错误码起始值
#define PED_RET_ERR_START -300
// PEDAPI 错误码结束值
#define PED_RET_ERR_END	-500
// PED函数返回正确
#define PED_RET_OK 0
// 密钥不存在
#define PED_RET_ERR_NO_KEY (PED_RET_ERR_START-1)
// 密钥索引错，参数索引不在范围内
#define PED_RET_ERR_KEYIDX_ERR     (PED_RET_ERR_START-2)
// 密钥写入时，源密钥的层次比目的密钥低
#define PED_RET_ERR_DERIVE_ERR	   (PED_RET_ERR_START-3)
// 密钥验证失败
#define PED_RET_ERR_CHECK_KEY_FAIL (PED_RET_ERR_START-4)
// 没输入PIN
#define PED_RET_ERR_NO_PIN_INPUT   (PED_RET_ERR_START-5)
// 取消输入PIN
#define PED_RET_ERR_INPUT_CANCEL   (PED_RET_ERR_START-6)
// 函数调用小于最小间隔时间
#define PED_RET_ERR_WAIT_INTERVAL  (PED_RET_ERR_START-7)
 // KCV模式错，不支持
#define PED_RET_ERR_CHECK_MODE_ERR (PED_RET_ERR_START-8)
// 无权使用该密钥，当出现密钥标签不对，或者写入密钥时,
// 源密钥类型的值大于目的密钥类型，都会返回该密钥
#define PED_RET_ERR_NO_RIGHT_USE   (PED_RET_ERR_START-9)
// 密钥类型错
#define PED_RET_ERR_KEY_TYPE_ERR   (PED_RET_ERR_START-10)
// 期望PIN的长度字符串错
#define PED_RET_ERR_EXPLEN_ERR     (PED_RET_ERR_START-11)
// 目的密钥索引错，不在范围内
#define PED_RET_ERR_DSTKEY_IDX_ERR (PED_RET_ERR_START-12)
// 源密钥索引错，不在范围内
#define PED_RET_ERR_SRCKEY_IDX_ERR (PED_RET_ERR_START-13)
// 密钥长度错
#define PED_RET_ERR_KEY_LEN_ERR		(PED_RET_ERR_START-14)
// 输入PIN超时
#define PED_RET_ERR_INPUT_TIMEOUT  (PED_RET_ERR_START-15)
// IC卡不存在
#define PED_RET_ERR_NO_ICC          (PED_RET_ERR_START-16)
// IC卡未初始化
#define PED_RET_ERR_ICC_NO_INIT    (PED_RET_ERR_START-17)
// DUKPT组索引号错
#define PED_RET_ERR_GROUP_IDX_ERR (PED_RET_ERR_START-18)
// 指针参数非法为空
#define PED_RET_ERR_PARAM_PTR_NULL    (PED_RET_ERR_START-19)
// PED已锁
#define PED_RET_ERR_LOCKED (PED_RET_ERR_START-20)
// PED通用错误
#define PED_RET_ERROR (PED_RET_ERR_START-21)
// 没有空闲的缓冲
#define PED_RET_ERR_NOMORE_BUF (PED_RET_ERR_START-22)
// 需要取得高级权限
#define PED_RET_ERR_NEED_ADMIN (PED_RET_ERR_START-23)
// DUKPT已经溢出
#define PED_RET_ERR_DUKPT_OVERFLOW (PED_RET_ERR_START-24)
// KCV 校验失败
#define PED_RET_ERR_KCV_CHECK_FAIL (PED_RET_ERR_START-25)
// 写入密钥时，源密钥id的类型不和源密钥类型不匹配
#define PED_RET_ERR_SRCKEY_TYPE_ERR	(PED_RET_ERR_START-26)
// 命令不支持
#define PED_RET_ERR_UNSPT_CMD  (PED_RET_ERR_START-27)
// 通讯错误
#define PED_RET_ERR_COMM_ERR (PED_RET_ERR_START-28)
// 没有用户认证公钥
#define PED_RET_ERR_NO_UAPUK (PED_RET_ERR_START-29)
// 取系统敏感服务失败
#define PED_RET_ERR_ADMIN_ERR (PED_RET_ERR_START-30)
// PED处于下载非激活状态
#define	PED_RET_ERR_DOWNLOAD_INACTIVE (PED_RET_ERR_START-31)

//KCV 奇校验失败
#define PED_RET_ERR_KCV_ODD_CHECK_FAIL (PED_RET_ERR_START-32)
//读取PED数据失败
#define PED_RET_ERR_PED_DATA_RW_FAIL (PED_RET_ERR_START-33)
//卡操作错误(脱机明文、密文密码验证)
#define PED_RET_ERR_ICC_CMD_ERR      (PED_RET_ERR_START-34)
//用户按CLEAR键退出输入PIN
#define PED_RET_ERR_INPUT_CLEAR		(PED_RET_ERR_START-39)
//PED存储空间不足
#define PED_RET_ERR_NO_FREE_FLASH	(PED_RET_ERR_START-43)
//DUKPT KSN需要先加1
#define PED_RET_ERR_DUKPT_NEED_INC_KSN	(PED_RET_ERR_START-44)
//KCV MODE错误
#define PED_RET_ERR_KCV_MODE_ERR	(PED_RET_ERR_START-45)
//NO KCV
#define PED_RET_ERR_DUKPT_NO_KCV   (PED_RET_ERR_START-46)
//按FN/ATM4键取消PIN输入
#define PED_RET_ERR_PIN_BYPASS_BYFUNKEY		(PED_RET_ERR_START-47)
//数据MAC校验错
#define PED_RET_ERR_MAC_ERR			(PED_RET_ERR_START-48)
//数据CRC校验错
#define PED_RET_ERR_CRC_ERR			(PED_RET_ERR_START-49)

//=============================================================================
// structure for ped operation
// nicm add 2008-07-25 end
//=============================================================================

//----------------------------------------------------------
// nicm add for new wireless module begin
//----------------------------------------------------------

#define WL_RET_ERR_PORTINUSE -201 // 模块口被占用
#define WL_RET_ERR_NORSP -202 // 模块没有应答
#define WL_RET_ERR_RSPERR -203 // 模块应答错误
#define WL_RET_ERR_PORTNOOPEN -204 // 模块串口没有打开
#define WL_RET_ERR_NEEDPIN -205	// 需要PIN码
#define WL_RET_ERR_NEEDPUK -206	// 需要PUK解PIN码
#define WL_RET_ERR_PARAMER -207	// 参数错误
#define WL_RET_ERR_ERRPIN -208 // 密码错误
#define WL_RET_ERR_NOSIM -209 // 没有插入SIM卡
#define WL_RET_ERR_NOTYPE -210 // 不支持的类型
#define WL_RET_ERR_NOREG -211 // 网络没有注册
#define WL_RET_ERR_INIT_ONCE -212 // 模块已初始化
#define WL_RET_ERR_LINEOFF -214	// 连接断开
#define WL_RET_ERR_TIMEOUT -216	// 超时
#define WL_RET_ERR_REGING -222 // 网络注册中
#define WL_RET_ERR_PORTCLOSE -223 // 关闭串口出错
#define WL_RET_ERR_MODEVER -225	// 错误的模块版本
#define WL_RET_ERR_DIALING -226	// 拨号中
#define WL_RET_ERR_ONHOOK -227 // 关机中
#define WL_RET_ERR_PPP_BRK -228	// 发现PPP断开
#define WL_RET_ERR_NOSIG -229 // 网络无信号
#define WL_RET_ERR_POWEROFF -230 // 模块已下电
#define WL_RET_ERR_BUSY -231 // 模块忙
#define WL_RET_ERR_OTHER -300 // 其他未知错误

#define	NET_OK               0  // 无错误，正常
#define	NET_ERR_MEM     	 -1  // 内存不够
#define	NET_ERR_BUF     	 -2  // 缓冲区错误
#define	NET_ERR_ABRT    	 -3  // 试图建立连接失败
#define	NET_ERR_RST     	 -4  // 连接被对方复位（收到对方的Reset）
#define	NET_ERR_CLSD    	 -5  // 连接已关闭
#define	NET_ERR_CONN    	 -6  // 连接未成功建立
#define	NET_ERR_VAL     	 -7  // 错误变量
#define	NET_ERR_ARG     	 -8  // 错误参数
#define	NET_ERR_RTE     	 -9  // 错误路由(route)
#define	NET_ERR_USE     	 -10 // 地址、端口使用中
#define	NET_ERR_IF      	 -11 // 底层硬件错误
#define	NET_ERR_ISCONN  	 -12 // 连接已建立
#define	NET_ERR_TIMEOUT 	 -13 // 超时
#define	NET_ERR_AGAIN   	 -14 // 请求资源不存在，请重试
#define	NET_ERR_EXIST   	 -15 // 已存在
#define	NET_ERR_SYS     	 -16 // 系统不支持
#define	NET_ERR_PASSWD  	 -17 // 错误密码
#define	NET_ERR_MODEM   	 -18 // 拨号失败
#define	NET_ERR_LINKDOWN   	 -19 // 数据链路已断开，请重新拨号
#define	NET_ERR_LOGOUT	   -20 // Logout
#define	NET_ERR_PPP	       -21 // PPP断开
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
	uchar card_type_check_w; // 卡片类型检查写入允许：1--允许，其它值--不允许，主要用于避免因卡片不规范引起的问题

	uchar card_type_check_val; // 0-检查卡片类型，其他－不检查卡片类型(默认为检查卡片类型)

	//uchar reserved[92];  //保留字节，用于将来扩展；写入时应全清零

	//2009-10-30
	uchar card_RxThreshold_w; // 卡片类型检查写入允许：1--允许，其它值--不允许，主要用于避免因卡片不规范引起的问题

	uchar card_RxThreshold_val;//卡片接收灵敏度

	//2009-11-20

     uchar f_modulate_w; // felica调制深度写入允许

     uchar f_modulate_val;//felica调制深度

	 //add by wls 2011.05.17
	 unsigned char a_modulate_w; // A卡调制指数写入允许：1--允许，其它值—不允许
	 unsigned char a_modulate_val; // A卡调制指数控制变量，有效范围0~63,超出时视为63

		//add by wls 2011.05.17
	 unsigned char a_card_RxThreshold_w; //接收灵敏度检查写入允许：1--允许，其它值--不允许
	 unsigned char a_card_RxThreshold_val;//A卡接收灵敏度

	 //add by liubo 2011.10.25, 针对A,B和C卡的天线增益
	 unsigned char a_card_antenna_gain_w;
	 unsigned char a_card_antenna_gain_val;

	 unsigned char b_card_antenna_gain_w;
	 unsigned char b_card_antenna_gain_val;

	 unsigned char f_card_antenna_gain_w;
	 unsigned char f_card_antenna_gain_val;

	 //added by liubo 2011.10.25，针对Felica的接收灵敏度
	 unsigned char f_card_RxThreshold_w;
	 unsigned char f_card_RxThreshold_val;

	 unsigned char reserved[76]; //保留字节，用于将来扩展；写入时应全清零


}PICC_PARA;


//-----------------------------------------------------------------
// nicm add for R50 射频卡模块增加 Desfire 功能支持 2008-09-23 begin
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
	uchar aucLowerLmt[4]; // 有符号整数, 可以为负数
	uchar aucUpperLmt[4]; // 有符号整数, 可以为负数
	uchar aucValue[4]; // 有符号整数, 可以为负数
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
	uchar aucSize[3]; // 仅用于Read Record, 即已知文件记录的大小, 若未知, 则置为0.
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
// 即接口中*pucStatus中回送的数据
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
// unrecoverable error within application, application will be disabled(仅在安全命令中返回)
#define STAT_DESFIRE_APP_INTEG_ERR     0xA1
// current authentication status does not allow the requested command
#define STAT_DESFIRE_AUTH_ERR          0xAE
// additional data frame is expected to be sent
#define STAT_DESFIRE_ADDITIONAL_FRAME  0xAF
// attempt to read/write data from/to beyond the file's/record's limits.
// attempt to exceed the limits of a value file
#define STAT_DESFIRE_BOUNDARY_ERR      0xBE
// unrecoverable error within PICC, PICC will be disabled(仅用于安全命令中)
#define STAT_DESFIRE_PICC_INTEG_ERR    0xC1
// previous command was not fully completed
// not all frames were requested or provided by the PCD
#define STAT_DESFIRE_CMD_ABORTED       0xCA
// PICC was disabled by an unrecoverable error(仅在安全命令中返回)
#define STAT_DESFIRE_PICC_DISABLED_ERR 0xCD
// number of app limited to 28, no additional CreateApplication possible
#define STAT_DESFIRE_COUNT_ERR         0xCE
// Creation of file/app failed because file/app with same number alredy exists
#define STAT_DESFIRE_DUPLICATE_ERR     0xDE
// could not complete NV-write operation due to loss of power,
// internal backup/rollback mechanism activated(仅在安全命令中返回)
#define STAT_DESFIRE_E2PROM_ERR        0xEE
// specified file number does not exist
#define STAT_DESFIRE_FILE_NOT_FOUND    0xF0
// unrecoverable error within file, file will be disabled(仅在安全命令中返回)
#define STAT_DESFIRE_FILE_INTEG_ERR    0xF1

#define MAX_RSA_MODULUS_BITS 2048

#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

//-----------------------------------------------------------------
// nicm add for R50 射频卡模块增加 Desfire 功能支持 2008-09-23 end
//-----------------------------------------------------------------


typedef struct
{
	unsigned char auth_id;			//论证码
	unsigned char identify_code;	// 识别码
	unsigned char masterkey_id;		// 主密钥索引
	unsigned char pinkey_id;		// PIN密钥索引
	unsigned char key_mode;			// 密钥模式
	unsigned char pin_key[24];		// 密钥
	unsigned char process_mode;		// 加解密模式
} PIN_KEY;

typedef struct
{
	unsigned char auth_id;			// 论证码
	unsigned char identify_code;	// 识别码
	unsigned char masterkey_id;		// 主密钥索引
	unsigned char mackey_id;		// MAC密钥索引
	unsigned char key_mode;			// 密钥模式
	unsigned char mac_key[24];		// 密钥
	unsigned char process_mode;		// 加解密模式
} MAC_KEY;


typedef struct
{
	unsigned char auth_id;			//论证码
	unsigned char identify_code;	//识别码
	unsigned char masterkey_id;		// 主密钥索引
	unsigned char key_mode;			// 密钥模式
	unsigned char master_key[24];	// 密钥
} MASTER_KEY;

typedef struct
{
     unsigned char auth_id;			// 论证码
     unsigned char identify_code;	// 识别码
     unsigned char MKeyID;			// 主密钥索引
     unsigned char WKeyID1;			// PIN或MAC密钥索引
     unsigned char WKeyID2;			// PIN或MAC密钥索引
     unsigned char mode;			// 密钥模式
	 unsigned char keytype;			// 密钥类型
}DERIVE_KEY;


typedef struct
{
	unsigned char auth_id;			// 论证码
	unsigned char identify_code;	// 识别码
	unsigned char input_mode;		// 输入方式
	unsigned char key_id;			// PIN密钥索引
	unsigned char encrypt_mode;		// 加密模式
	unsigned char min_len;			// PIN最小长度
	unsigned char max_len;			// PIN最大长度
	unsigned char card_no[16];		// 卡号

} PIN_ANSIX98;

typedef struct
{
	unsigned char auth_id;			// 论证码
	unsigned char identify_code;	// 识别码
	unsigned char input_mode;		// 输入方式
	unsigned char key_id;			// PIN密钥索引
	unsigned char encrypt_mode;		// 加密模式
	unsigned char min_len;			// PIN最小长度
	unsigned char max_len;			// PIN最大长度
	unsigned char ISN[6];			// ISN

} PIN_ANSIX392;

typedef struct
{
     unsigned char auth_id;			// 论证码
     unsigned char identify_code;	// 识别码
     unsigned char mackeyid;		// MAC密钥索引
     unsigned char mode;			// 加密算法
     unsigned short inLen;			// MAC运算的数据包的长度
     unsigned char datain[2048];	// 需进行MAC运算的数据包
     unsigned char flag;			// MAC算法
} MAC_PACKAGE;

// donot support anymore.
// typedef struct
// {
//      unsigned int  modlen;           //PIN加密公钥模数长
//      unsigned char mod[256];        //PIN加密公钥模数
//      unsigned int  explen;           //PIN加密公钥指数长
//      unsigned char exp[4];          //PIN加密公钥指数
//      unsigned char iccrandomlen;    //从卡行取得的随机数长
//      unsigned char iccrandom[8];    //从卡行取得的随机数
//      unsigned int  termrandomlen;   //从终端应用取得的填充数长
//      unsigned char termrandom[256]; //从终端应用取得的填充数
// } RSA_PINKEY;

// 通讯参数设置
typedef struct
{
    unsigned char bCommMode;        // 0:Serial, 1:Modem, 2:LAN, 3:GPRS, 4:CDMA, 5:PPP
                           // 目前远程下载只支持串口、modem、LAN和PPP四种通讯模式，
                           // 而不支持GPRS和CDMA两种通讯模式  2005-12-22

    unsigned char *psAppName;       // 应用名，若为空则下载所有的应用及参数、数据文件
    unsigned char bLoadType;        // 下载类型，bit0:应用，bit1:参数文件，bit2:数据文件
    unsigned char psProtocol;       // 协议信息：0-protims；1-ftims；2-tims
    unsigned char bPedMode;         // 0-外置PED（如PP20）　1－内置PED（P60－S1，P80）
                           // 目前该参数在ProTims和Tims中不使用，只在FTims中使用

    unsigned char bDMK_index;       // 主密钥索引，ProTims中不使用
    unsigned char sTransCode[5];    // 交易码，只在FTims在中使用，ProTims和Tims中不使用
                           // 取值     含义
                           // "2001"：单应用下载交易
                           // "2011"：多应用下载交易
                           // "1000"：单应用POS查询下载任务
                           // "1005"：单应用POS查询和下载在一次通讯内完成
                           // "1010"：多应用POS查询下载任务
                           // "1015"：多应用POS查询和下载在一次通讯内完成

    unsigned char ucCallMode;    // 调用模式，目前该参数在国内版ProTims、FTims和Tims中
                        // 不使用 2005-09-26
    // 在新版海外版ProTims中使用该参数，主要是香港那边开始使用P80 2005-10-17
    // 应用调用本接口的控制。bit0~bit3: 0：操作员主动要求；
    // 1：按计划定时；
    // 2：被远程服务器呼叫，请求monitor接收server的更新通知
    // 消息(接口返回时，monitor不能断线)；
    // 3：(v0.8.0)被远程服务器呼叫，请求monitor做远程下载操作，
    // 如果bit4~bit7=0, P70的monitor需要用内部串口操作
    // modem。
    // bit4~bit7: 0：monitor不需要建立通讯连接；1：monitor
    // 自己建立连接

    unsigned char *psTermID;     // 终端号，目前该参数在海外版本PtoTims和新版本的
                        // 国内ProTims中使用 2006-04-18
    union
    {
        struct
        {
            unsigned char *psPara; //串口通讯参数，格式如"57600,8,n,1"
        }tSerial;  // 串口通讯参数，参数bCommMode=0时该参数才有效
        struct
        {
            unsigned char *psTelNo;         //电话号码，详细参考ModemDial()函数
            COMM_PARA *ptModPara;  //Modem通讯参数
            unsigned char bTimeout;         //拨号成功后的延时[秒]
        }tModem;  // modem通讯参数，参数bCommMode=1时该参数才有效
        struct
        {
            unsigned char *psLocal_IP_Addr;       // 本地IP地址，在TCP/IP方式下必须使用
            unsigned short wLocalPortNo;           // 本地端口号，现在不使用，IP80模块会自动使用默认端口号
            unsigned char *psRemote_IP_Addr;      // 远程IP地址，在TCP/IP方式下必须使用
            unsigned short wRemotePortNo;          // 远程端口号，在TCP/IP方式下必须使用
            unsigned char *psSubnetMask;          //   子网掩码，在TCP/IP方式下必须使用  2006-04-21
            unsigned char *psGatewayAddr;         //       网关，在TCP/IP方式下必须使用      2006-04-21
        }tLAN; // TCP/IP协议通讯参数，参数bCommMode=2时该参数才有效
        struct
        {
            unsigned char *psAPN;          //APN
            unsigned char *psUserName;     //用户名
            unsigned char *psPasswd;       //用户密码
            unsigned char *psIP_Addr;      //IP地址
            unsigned short nPortNo;         //端口号
            unsigned char *psPIN_CODE;     //如果SIM卡有密码，还要输入PIN码
            unsigned char *psAT_CMD;       //如果有AT命令的话
        }tGPRS; // GSM手机通讯参数，参数bCommMode=3时该参数才有效
        struct
        {
            unsigned char *psTelNo;        //电话号码
            unsigned char *psUserName;     //用户名
            unsigned char *psPasswd;       //用户密码
            unsigned char *psIP_Addr;      //IP地址
            unsigned short nPortNo;         //端口号
            unsigned char *psPIN_CODE;     //如果SIM卡有密码，还要输入PIN码
            unsigned char *psAT_CMD;       //如果有AT命令的话
        }tCDMA; // CDMA手机通讯参数，参数bCommMode=4时该参数才有效
        struct
        {
            unsigned char *psTelNo;        // 电话号码
            COMM_PARA *ptModPara; // MODEM通信参数
            unsigned char *psUserName;     // 用户名
            unsigned char *psPasswd;       // 用户密码
            unsigned char *psIP_Addr;      // IP地址
            unsigned short nPortNo;         // 端口号
        }tPPP; // 基于modem的点对点(Point to Point Protocol)远程下载通讯参数，
               // 参数bCommMode=5时该参数才有效

		struct //WIFI 参数bCommMode=6时该参数才有效
		{

			unsigned char *Wifi_SSID;        // SSID
			unsigned char *psPasswd;       // 用户密码

			unsigned char *Local_IP; // 本机IP

			unsigned short Local_PortNo;

			unsigned char *Remote_IP_Addr;      // 远程IP地址
			unsigned short RemotePortNo;          // 远程端口号
			unsigned char *SubnetMask;          //   子网掩码
			unsigned char *GatewayAddr;         //       网关
			unsigned char *Dns1;
			unsigned char *Dns2;
			unsigned char Encryption_Mode; //  1: WEP 64(ASCII); 2 WEP 128 (ASCII);    3  WPA_TKIP;  4 WPA_AES
			unsigned char Encryption_Index; // WEP时用取值范围:1~4
			unsigned char DHCP_Flag; // 1:使用DHCP
        }tWIFI;
    }tUnion;
}T_INCOMMPARA;

typedef struct
{
    uchar app_name[33];    // 应用名称
    uchar app_ver[21];     // 应用版本
    uchar have_param;      // 有无参数文件
    uchar param_name[17];  // 参数文件名
    uchar param_ver[21];   // 参数文件版本
    uchar UpdatedAppMode;  // 0-立即更新 1-保存在临时文件中
    uchar UpdatedParaMode; // 0-立即更新 1-保存在临时文件中
    uchar AppActiveStatus; // 0-未激活　 1-已激活
    uchar reserve[4];      // 保留字节
}LOADLOG_INFO; // 100byte

typedef struct
{
	uchar bAppTotal;           // POS机中的应用总数
	uchar sAppNum[25];        // 每个应用的序号
	uchar sLoadAppStat[25];  // 对应的每个应用的下载状态
	uchar sLoadParaStat[25];  // 对应的每个应用中参数/数据文件的下载状态
	uchar sAppName[25][33];   // 应用名称汇总
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
/*　　　　　函数返回值一览表　　              */
/*                                            */
/**********************************************/
/*
   0x00	  正确
   0x01	  数据包长度非法
   0x02	  密钥索引非法
   0x03	  非法模式
   0x04	  屏幕坐标非法
   0x05	  字符串可输入长度范围非法
   0x06	  操作员取消输入
   0x07	  输入超时
   0x08	  显示点阵长度非法
   0x09	  两次密码不一致
   0x0a	  用户未输入密码（输入密码的长度为0）
   0x0b	  指定的密钥不存在
   0x0c   内存错
   0x0d	  奇校验错
   0x0e	  TMK错
   0x0f	  "输入MAC"错
   0x10	  菜单序号非法
   0x11	  密钥检验不合法

   0x12   应用外部论证密钥已初始化
   0x13   应用数目溢出
   0x14   应用不存在
   0x15   外部论证失败
   0x16   没有进行外部认证
   0x17   PIN专用密钥太多
   0x18   无应用名称(或应用名称超长）
   0x19   外部论证锁定（连续尝试失败超过15次）
   0x1a   输入间隔小于30秒

   0xc6	  用户取消输入
   0xce	  PED已锁
   0xcf	  对PED解锁失败

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
    void * ptr;/* 内存指针*/
	int  size;/* 空间大小*/
} ST_SSL_BUF;

#define SSL_OPER_OK			0
#define SSL_BEYOND_CERF_NUM	-401//证书个数过多
#define SSL_BEYOND_SERV_IP	-404//太多服务端，多于10个
#define SSL_FILE_OPER_FAIL	-405//文件操作失败,
#define SSL_CER_NO_EXIST	-406//证书不存在
#define SSL_CER_NOSPACE		-407//证书空间不够
#define SSL_NULL_ERR		-408//参数为非法空串


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
	int DhcpEnable;	//DHCP使能，0-关闭，1-开启
	uchar Ip[IPLEN];	//设置静态IP
	uchar Mask[IPLEN];	//掩码
	uchar Gate[IPLEN];	//网关
	uchar Dns[IPLEN];	//DNS
	uchar Wep[KEY_WEP_LEN];	//wep密码
	uchar Wpa[KEY_WPA_MAXLEN];	//WPA连接密码
	uchar reserved[256];//预留
}ST_WIFI_PARAM;
typedef struct
{
	uchar Ssid[SSID_MAXLEN];		//搜索到的AP的SSID
	int SecMode;	//安全模式
	int Rssi;	//搜索到的AP的信号强度
}ST_WIFI_AP;


//================================================
//            颜色相关的定义
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
