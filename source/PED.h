#ifndef _PED_H
#define _PED_H

#define PED_InvoiceOPEN         0x01       
#define PED_CLOSE		0x00
#define PED_GREEN		0x01
#define PED_RED			0x02
#define PED_YELLOW		0x04


#ifndef uchar
#define uchar   unsigned char
#endif
#ifndef uint
#define uint    unsigned int
#endif
#ifndef ushort
#define ushort  unsigned short
#endif
#ifndef ulong
#define ulong   unsigned long
#endif

typedef struct
{
	unsigned char auth_id;        /* 论证码 */
	unsigned char identify_code;  /* 识别码 */
	unsigned char mainkey_id;     /* 主密钥索引 */
	unsigned char workkey_id;     /* 工作密钥索引 */
	unsigned char key_mode;       /* 密钥模式 */
	unsigned char work_key[16];   /* 密钥  */
	unsigned char process_mode;   /* 加解密模式 */
} PIN_KEY;   

typedef struct
{
	unsigned char auth_id;        /* 论证码  */
	unsigned char identify_code;  /* 识别码  */
	unsigned char input_mode;     /* 输入方式  */
	unsigned char key_id;         /* PIN密钥索引  */
	unsigned char encrypt_mode;   /* 加密模式 */
	unsigned char min_len;        /* PIN最小长度  */
	unsigned char max_len;        /* PIN最大长度  */
	unsigned char card_no[16];    /* 卡号   */
	
} PIN_ANSIX98;     

typedef struct
{
	unsigned char auth_id;       /* 论证码  */
	unsigned char identify_code; /* 识别码  */
	unsigned char input_mode;    /* 输入方式  */
	unsigned char key_id;        /* PIN密钥索引  */
	unsigned char encrypt_mode;  /* 加密模式  */
	unsigned char min_len;       /* PIN最小长度  */
	unsigned char max_len;       /* PIN最大长度  */
	unsigned char ISN[6];        /* ISN  */
	
} PIN_ANSIX392;   


/**********************************************/
/*                                            */
/*　　　　　　　基本PED功能接口               */
/*                                            */
/**********************************************/

/********************************************
 功能：蜂鸣接口（同Beep()）
 ********************************************/
uchar PEDBeep(void);

/***********************************************
  OperMode=O_OPEN  打开PED指示灯
  OperMode=O_CLOSE 关闭PED指示灯
  Index          可以为以下值的任意组合:
       RED       红灯
       GREEN     绿灯
       YELLOW    黄灯
　注：该接口暂为空函数（未连PED指示灯）       
************************************************/
uchar PEDLight(uchar OperMode, uchar Index);

/***********************************************
  OperMode=O_OPEN  打开PED语音提示
  OperMode=O_CLOSE 关闭PED语音提示
  注：该接口暂为空函数
************************************************/
uchar PEDInvoice(uchar OperMode);

/***********************************************
  OperMode=O_OPEN  打开PED按键音
  OperMode=O_CLOSE 关闭PED按键音
  注：同Kbmute()
************************************************/
uchar PEDKbmute(uchar OperMode);

/***********************************************
  OperMode=O_OPEN  打开背光
  OperMode=O_CLOSE 关闭背光
  注：同ScrBackLight()
************************************************/
uchar PEDBackLight(uchar OperMode);

/***********************************************
  函数功能  输入字符串
  str       输入的字符串，以'\0'结尾
  min       可输入最小长度
  max       可输入最大长度（最大40字节）
  mode=0    明码输入
  mode=1    密码输入,显示'*'
************************************************/
uchar PEDInput(uchar *str, uchar min, uchar max, uchar mode);

/***********************************************
  功能：设置输入间隔超时（单位为：秒）
  系统默认为：0＝无超时
************************************************/
void PEDInputTimeOut(unsigned long timeoutsec);

/***********************************************
  功能：清屏，同ScrCls();
************************************************/
uchar PEDScrCls(void);

/***********************************************
  功能：清除行，同ScrClrLine();
************************************************/
uchar PEDScrClrLine(uchar line);

/***********************************************
  函数功能  显示字符串(汉字和英文），同ScrPrint
  line      行(0 - 7)
  col       列(0 - 1127)
  str       显示的字符串
************************************************/
uchar PEDScrPrint(uchar line, uchar col, uchar *str);

/***********************************************
  函数功能  显示点阵
  line      行(0 - 1)
  col       列(0 - 121)
  Data      显示数据(数据格式参考协议)
  len       显示数据长度
************************************************/
uchar PEDScrWrData(uchar line, uchar col, uchar *Data, uchar len);

/************************************************
  函数功能  写入主密钥密钥
  KeyID     密钥索引(主密钥区）
  Key       密钥值
  mode=0x01 DES密钥
  mode=0x03 3DES或SEED密钥
*************************************************/
uchar PEDWriteMKey(uchar KeyID, uchar mode, uchar *Key);

/************************************************
  函数功能  写入DES(3DES)计算的密钥
  DESKeyID  密钥索引(DES密钥区）
  Key       密钥值
  mode=0x01 DES密钥
  mode=0x03 3DES或SEED密钥
************************************************/
uchar PEDWriteDKey(uchar DESKeyID, uchar mode, uchar *Key);

/************************************************
  函数功能  写入工作密钥
  MKeyID    用该索引指定的主密钥对Key进行mode方式
  　　　　　的计算后再写入,MKeyID=0,不进行任何计算,
  　　　　　直接写入
  WKeyID    写入的地址(工作密钥区)
  Key       密钥
  mode=0x01 DES加密
  mode=0x03 写入SEED密钥或3DES加密
  mode=0x81 DES解密
  mode=0x83 3DES解密
************************************************/
uchar PEDWriteWKey(uchar MKeyID, uchar WKeyID, uchar mode, uchar *Key);

/************************************************
  函数功能  分散密钥
  MKeyID    需进行分散的密钥,高位为1（即0x80)指定的
  　　　　　工作密钥区密钥，否则为主密钥区密钥
  WKeyID1   分散数据(工作密钥区)
  WKeyID2   分散结果(工作密钥区)
  mode=0x01 DES加密
  mode=0x03 3DES加密
  mode=0x81 DES解密
  mode=0x83 3DES解密
************************************************/
uchar PEDDeriveKey(uchar MKeyID, uchar WKeyID1, uchar WKeyID2, uchar mode);

/************************************************
  函数功能     读取ANSI9.8加密的密码
  TwiceInput=1 PED语音提示输入2次
  TwiceInput=0 PED语音提示输入1次
  PinKeyID     计算PIN的工作密钥(工作密钥区)
  min          密码可输入最小长度
  max          密码可输入最大长度
  pin          加密的密码输出
  cardno       经过移位处理的16位卡号（ASCII码）
************************************************/
uchar PEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *cardno, uchar *pin,uchar TwiceInput);

/*---------------------------------------------------------------------
读取ANSI9.8加密的密码[采用3DES加密]
原型：uchar PPGetPwd_3Des(uchar PinKeyID, uchar min, uchar max, uchar *cardno, 
uchar *bPinLen,uchar *pin);
功能：
      屏幕上输入长度为[min，max]的密码
参数：
      PinKeyID   计算PIN的工作密钥(工作密钥区)
      min       密码可输入最小长度[0=<min<=14]
      max       密码可输入最大长度[0=<max<=14 && max>=min]
      cardno   	经过移位处理的16位卡号（ASCII码）
      bPinLen	密码长度[输出数据，min=<bPinLen <=max]。
      pin       加密的密码[输出数据]
 ----------------------------------------------------------------*/
uchar PEDGetPwd_3Des(uchar PinKeyID, uchar min, uchar max, uchar *cardno,uchar *bPinLen,uchar *pin);


/************************************************
  函数功能  DES(3DES)计算
  DESKeyID  计算DES的密钥(DES密钥区)
  datain    输入数据
  dataout   输出数据
  mode=0x01 DES加密
  mode=0x03 3DES加密
  mode=0x81 DES解密
  mode=0x83 3DES解密
**************************************************/
uchar PEDDes(uchar DESKeyID, uchar mode, uchar *datain, uchar *dataout);

/************************************************
  函数功能  MAC计算
  flag=0    算法1
  flag=1    算法2
  WKeyID    计算MAC的工作密钥(工作密钥区)
  inLen     计算MAC的数据长度
  datain    输入数据
  macout    输出数据(即MAC)
  mode=0x01 DES加密
  mode=0x03 3DES加密
**************************************************/
//uchar PEDMac(uchar WKeyID, uchar mode, uchar *datain, uchar inLen, uchar *macout, uchar flag);
uchar PEDMac(uchar WKeyID,uchar mode, uchar *datain, ushort inLen, uchar *macout,uchar flag);

/************************************************
  函数功能  读取PED版本信息
  
  ver:   版本信息[输出：8字节]，
  格式如下："HvXXSvXX"：
  HvXX：其中Hv代表硬件版本、XX为版本号。
  SvXX：其中Sv代表软件版本、XX为版本号。
**************************************************/
uchar PEDVerInfo(uchar *ver);

/************************************************
  函数功能  显示图形点阵
  注：该函数暂为空接口，可使用ScrDrLogo实现该功能
**************************************************/
uchar PEDUpdLogo(uchar line, uchar *Data);

/***********************************************
功能：清除PED中相应类型的KEY

参数：keytype=0   主密钥
　　　keytype=1   工作密钥
　　　keytype=2   DES密钥
返回：
　　　0：成功　　非0：失败
*************************************************/
uchar PEDClearKey(uchar keytype);

/***********************************************
功能：PED其它功能管理函数

mode＝0   校验超级管理密钥，其中的PWDCODE为
　　　　　8字节的校验值；若校验成功则系统自动
　　　　　解除锁键盘标志并可更新超级密钥与解
　　　　　锁密钥。
mode＝1　 校验解锁密钥，其中的PWDCODE为8字节
　　　　　的校验值；若校验成功则系统自动解除
　　　　　锁键盘标志，并可更新解锁密钥。
mode＝2　　锁键盘操作

mode＝0X80  修改超级管理密钥，其中的PWDCODE
　　　　　  由新管理密钥（8字节）组成。　
mode＝0X81　修改解锁密钥，其中的PWDCODE是由
　　　　　  新解锁密钥（8字节）组成。
返回：
　　　0X00：　成功　
　　　0XF0：　校验密钥失败
　　　0XF1：　修改权限错误（须先正确校验密钥才可更新）
*************************************************/
uchar PEDManage(uchar mode,uchar *pwdcode);



/**********************************************/
/*                                            */
/*　　　　HONG KONG EPS 专用功能接口          */
/*                                            */
/**********************************************/

/***********************************************
注释：菜单显示函数（暂为空接口）
************************************************/
uchar EPSPEDDispMenu(uchar StartNo, uchar EndNo, uchar AllLines, uchar detail[][16], uchar *item);


/**************************************************
功能：输入金额

min：       字符串最小长度
max：       字符串最大长度
Point_mode：0X00： 小数点后有两位小数
　　　　　　0X01： 小数点后有三位小数
　　　　　　0X02： 无小数点
prompt_mode:　该参数暂时无效[由于无提示信息，
　　　　　　　可为任意值]
Amt:	      输入的金额
****************************************************/
uchar EPSPEDAmount(uchar point_mode,uchar prompt_mode, uchar min, uchar max,uchar *Amt);

/***************************************************
功能：装载初始TMK

MkeyID1：  TMK将写入的位置序号(1-100)
TmpKey：   POS产生临时KEY[8 Bytes]
KTMK,：    从主机取回密文[8 Bytes]
ReferCode：主机用KTMK的明文加密"\x12\x34\x56\x78\x90
　　　　　　\x12\x34\x56"后，返回的值[8 Bytes]
*****************************************************/
uchar EPSPEDLoadTMK(uchar WkeyID1, uchar *TmpKey, uchar *KTMK, uchar *ReferCode);
uchar EX_EPSPEDLoadTMK(uchar WkeyID1, uchar *TmpKey, uchar *KTMK, uchar *ReferCode,uchar mode);

/***************************************************
功能：EPS装载密钥

WkeyID1：用来解密或被更新的TMK的位置序号(1-100)
WkeyID2：待装载密钥的位置序号(1-100)
KKEY：   主机下传的密文[8 Bytes]
*****************************************************/
uchar EPSPEDLoadKey(uchar WKeyID1,uchar WKeyID2, uchar *KKEY);
uchar EX_EPSPEDLoadKey(uchar WKeyID1,uchar WKeyID2, uchar *KKEY,uchar mode);

/******************************************************
功能：EPS加密"输出MAK"

MkeyID： 用来加密的TMK的位置序号(1-100)
Random： 随机数[输入数据]或返回的加密结果 [输出数据] [8bytes]
*******************************************************/
uchar EPSPEDGetMAC1(uchar WKeyID, uchar *Random);
uchar EX_EPSPEDGetMAC1(uchar WKeyID, uchar *Random,uchar mode);

/******************************************************
功能：EPS更新TPK[Pin_block]

WKeyID1：　TMK / TPK的位置序号(1-100)
WKeyID2：　TPK的位置序号(1-100)
pin_block：PIN加密包[8bytes]，更新后的密码加密包也由它返回
ISN：	   交易序列号[6bytes的ASCII码]
new_TPK:   主机下传的更新的TPK密文[8bytes]
********************************************************/
uchar EPSPEDUpdateTPK(uchar WKeyID1,uchar WKeyID2, uchar *pin_block, uchar *ISN, uchar *new_TPK);
uchar EX_EPSPEDUpdateTPK(uchar WKeyID1,uchar WKeyID2, uchar *pin_block, uchar *ISN, uchar *new_TPK,uchar mode);


/******************************************************
功能：EPS更新"输入MAK"

WKeyID1：      用来解密的TMK的位置序号(1-100)
WKeyID2：      MAC2的位置序号(1-100)
MAC2：	       主机下传的新的MAC2密文[8bytes]
rsp_data：      rsp_data_len Bytes主机下传的数据
rsp_data_len:  下传数据rsp_data的长度[rsp_data_len<=233]
rsp_MAC	：     主机下传的用来比较的数据[8字节，ASCII码串]
******************************************************/
uchar EPSPEDSetMAC2(uchar WKeyID1,uchar WKeyID2,uchar *MAC2, uchar *rsp_data, uchar rsp_data_len, uchar *rsp_MAC);
uchar EX_EPSPEDSetMAC2(uchar WKeyID1,uchar WKeyID2,uchar *MAC2, uchar *rsp_data, uchar rsp_data_len, uchar *rsp_MAC,uchar mode);



/******************************************************
功能：读取ANSI3.92加密的密码

PinKeyID  计算PIN的工作密钥(1-100)
min       密码可输入最小长度[0=<min<=6]
max       密码可输入最大长度[0=<max<=6&&min<=max]
ISN       交易流水号ISN [6 Bytes，ASCII码串]
pin       加密的密码输出[输出数据，8bytes]
******************************************************/
uchar EPSPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *ISN, uchar *pin);
uchar EX_EPSPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *ISN, uchar *pin,uchar mode);


/******************************************************
功能： EPS校验TMK

MkeyID：     用来加密的TMK的位置序号(1-100)
VerifyData： 加密后的结果（8byte）[输出数据]
******************************************************/
uchar EPSPEDVerifyTMK(uchar WKeyID, uchar *VerifyData);
uchar EX_EPSPEDVerifyTMK(uchar WKeyID, uchar *VerifyData,uchar mode);



/*********************************************************
 函数原型：uchar PED_PassWordEncrypt(uchar deskeyid,
 　　　　　　　　　　　　　　　uchar *oldpin,uchar *despin)
 功能说明：用DESKEYID指定的DES密钥与PED中固定的KEY对OLDPIN进行
 　　　　　DES加密，并将结果存在DESPIN中。
 参数说明：DESKEYID：PED中DES密钥索引号
 　　　　　OLDPIN：　没有加密的PWD串（ASCII码，长度<=16）
 　　　　　DESPIN：　加密的PWD串（8字节）
 返回值：　02＝密钥索引号错误
		   05＝参数(PWD串)非法
 　　　　　00＝成功
 **********************************************************/
uchar PED_PassWordEncrypt(uchar deskeyid,uchar *oldpin,uchar *despin);

/************************************************************
 函数原型：uchar PED_PassWordInput(uchar deskeyid,uchar min,
                                    uchar max,uchar *inpin)

 功能说明：用DESKEYID指定的DES密钥与PED中固定的KEY对输入的PWD进行
 　　　　　DES加密，并将结果存在INPIN中。
 参数说明：DESKEYID：PED中DES密钥索引号
 　　　　　MIN：　　指定输入的PWD的最小字节数。
 　　　　　MAX：　　指定输入的PWD的最大字节数（<=16）
 　　　　　INPIN：　加密的PWD串（8字节）

 返回值：　02＝密钥索引号错误
           05＝参数非法
		   0A＝输入密钥长度为0
		   06＝操作员取消输入
   　　　　07＝输入超时
 　　　　　00＝成功
 ************************************************************/
uchar PED_PassWordInput(uchar deskeyid,uchar min,uchar max,uchar *inpin);



/**********************************************/
/*                                            */
/*　　　　　　　深银联专用功能接口            */
/*                                            */
/**********************************************/

/***************************************************
  函数功能   读取加密的密码[深银联自定义的算法:密码
             右对齐，用‘F'填充，格式“FFFFFFFFFFPPPPPP”]
  PinKeyID   计算PIN的工作密钥(工作密钥区)
  min        密码可输入最小长度[<=6, <=max]
  max        密码可输入最大长度[<=6]
  bPwdLen    输出的PIN长度
  pin        输出的密码[密文]
*****************************************************/
uchar SYLPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *bPinLen,uchar *pin_block);

/******************************************************
函数功能    用MKeyID指定的主密钥对new_WKey进行mode方式的
            加解密，得到新密钥，用此密钥对
            "\x00\x00\x00\x00\x00\x00\x00\x00"作DES加密，
            将结果与密钥校验数据比较，如果相同则将新密钥
            写入WkeyID指定的工作密钥区。
MKeyID	    存储主密钥的索引
WKeyID	    发散后的密钥存储索引
new_WKey    用来验证和发散的密钥[8Bytes，密文]
KeyCv	    密钥校验数据[8Bytes]。
mode        加解密模式，具体定义如下：
      	    0x01  DES加密
      	    0x81  DES解密
******************************************************/
uchar SYLPEDVerifyDerive(uchar MKeyID, uchar WKeyID, uchar *new_WKey, uchar *KeyCv, uchar mode);

/***************************************************
函数功能    先用PinKeyID指定的工作密钥对密文pin_block
            进行解密，然后用MKeyID指定的主密钥对new_TPK
            进行mode方式的加解密，得到新密钥，用此密钥
            对明文pin_block加密。
MKeyID	    存放主密钥[TMK]的索引
PinKeyID    存放PIN密钥[TPK]的索引
pin_block   PIN加密数据[8Bytes，密文]
new_TPK     新的PIN密钥[8Bytes，密文]
mode        加解密模式，具体定义如下：
      	    0x01  DES加密
      	    0x81  DES解密
***************************************************/
uchar SYLPEDCalcPinBlock(uchar MKeyID,uchar PinKeyID,uchar *pin_block,uchar *new_TPK,uchar mode);






/**********************************************/
/*                                            */
/*　　　　VISA　PED认证功能接口               */
/*                                            */
/**********************************************/

/***************************************************
 功能；产生8字节的随机数
 输出：随机数
 返回：无
***************************************************/
void  PEDGetRandom(uchar *random);

/***************************************************
功能；初始化外部论证密钥

输入：       
      应用名称:     32byte，标志信息，以'\x0'结尾
      外部论证密钥: 8byte， 密文，由应用程序随机产生并使用
                            超级论证密钥加密而成
输出：无
***************************************************/
uchar PEDInitAuthKey(uchar *app_name,uchar *auth_key);

/***************************************************
功能；外部论证
输入：       
     应用名称: 32byte，标志信息，以'\x0'结尾
     验证数据: 8byte，加密数据，用外部论证密钥对常数串
               "\x98\x87\x76\x65\x12\x23\x34\x45"进行
               ES加密而得
输出：	    
    论证码（1BYTE）
***************************************************/
uchar PEDExternalAuth(uchar *app_name,uchar *auth_data,uchar *auth_id);

/***************************************************
功能；超级外部论证
输入：       
     验证数据:   8byte，明文，由应用程序随机产生
输出：	    
     论证码（1BYTE）
***************************************************/
uchar PEDSuperExternalAuth(uchar *auth_data,uchar *auth_id);

/***************************************************
功能；输入PIN并用ANSI X3.92加密
输入：       
      论证码:    1byte，外部论证时获得的返回值
      识别码:    1byte，外部论证验证数据的最后一个字节
      输入方式:  1byte，
		 0x00：输入PIN
		 0x01：更新PIN
      PIN密钥索引: 1byte，范围为[1，100]
      加密模式:    1byte，对PIN BLOCK的加密方法
	    	   0x01：DES加密
                   0x03：3DES加密
      PIN最小长度: 1byte，(为0时，按ENTER键返回，此时
                   应答数据长度为0，表示用户没有输入密码) 
      PIN最大长度: 1byte，<=6
      ISN :        6byte，
输出：	    
      加密结果[8 bytes] 或0x00
***************************************************/
uchar PEDPinEncrypt_AnsiX392(PIN_ANSIX392 *Pin_X392,uchar *ret_data);

/***************************************************
功能；输入PIN并用ANSI X9.8加密.
输入：       
      论证码:   1byte，外部论证时获得的返回值
      识别码:   1byte，外部论证验证数据的最后一个字节
      输入方式: 1byte，
		0x00：输入PIN
		0x01：更新PIN
      PIN密钥索引:   1byte，范围为[1，100]
      加密模式:      1byte，对PIN BLOCK的加密方法
	    	     0x01：DES加密
                     0x03：3DES加密
      PIN最小长度:   1byte，(为0时，按ENTER键返回，此时
                     应答数据长度为0，表示用户没有输入密码)
      PIN最大长度:   1byte，<=14
      卡号:          16byte，
输出：	    
      加密结果[8 bytes] 或0x00
***************************************************/
uchar PEDPinEncrypt_AnsiX98(PIN_ANSIX98 *Pin_X98,uchar *ret_data);

/***************************************************
功能；更新论证密钥.
输入：       
      论证码: 1byte，外部论证时获得的返回值
      识别码: 1byte，外部论证验证数据的最后一个字节
      新密钥: 8byte，密文，由应用程序随机产生并使用
                     超级论证密钥加密而成
输出：	    
      无
***************************************************/
uchar PEDUpdateAuthKey(uchar auth_id,uchar identify_code,uchar *new_auth_key);

/***************************************************
功能；清除PIN密钥及外部论证密钥
输入：       
      论证码:  1byte，外部论证时获得的返回值
      识别码:  1byte，外部论证验证数据的最后一个字节
输出：	    
      无
***************************************************/
uchar PEDErasePinKey(uchar auth_id,uchar identify_code);

/***************************************************
功能；写PIN专用密钥

输入：       
      论证码 :     1byte，外部论证时获得的返回值
      识别码 :     1byte，外部论证验证数据的最后一个字节
      主密钥索引:  1byte，范围[1，100]，对密钥解密
      工作密钥索引:1byte，范围[1，100]，存放解密密钥
      密钥模式:    1byte，
                   0x01：DES密钥
                   0x03：3DES密钥
      密钥:        16byte，密文，左对齐，不足则填充'0x00'
      加解密模式:  1byte，
		   0x01：DES加密
	           0x03：3DES加密
	           0x81：DES解密
                   0x83：3DES解密
输出：	    
	无
***************************************************/
uchar PEDWritePinKey(PIN_KEY *pin_key);


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

#endif
