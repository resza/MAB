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
	unsigned char auth_id;        /* ��֤�� */
	unsigned char identify_code;  /* ʶ���� */
	unsigned char mainkey_id;     /* ����Կ���� */
	unsigned char workkey_id;     /* ������Կ���� */
	unsigned char key_mode;       /* ��Կģʽ */
	unsigned char work_key[16];   /* ��Կ  */
	unsigned char process_mode;   /* �ӽ���ģʽ */
} PIN_KEY;   

typedef struct
{
	unsigned char auth_id;        /* ��֤��  */
	unsigned char identify_code;  /* ʶ����  */
	unsigned char input_mode;     /* ���뷽ʽ  */
	unsigned char key_id;         /* PIN��Կ����  */
	unsigned char encrypt_mode;   /* ����ģʽ */
	unsigned char min_len;        /* PIN��С����  */
	unsigned char max_len;        /* PIN��󳤶�  */
	unsigned char card_no[16];    /* ����   */
	
} PIN_ANSIX98;     

typedef struct
{
	unsigned char auth_id;       /* ��֤��  */
	unsigned char identify_code; /* ʶ����  */
	unsigned char input_mode;    /* ���뷽ʽ  */
	unsigned char key_id;        /* PIN��Կ����  */
	unsigned char encrypt_mode;  /* ����ģʽ  */
	unsigned char min_len;       /* PIN��С����  */
	unsigned char max_len;       /* PIN��󳤶�  */
	unsigned char ISN[6];        /* ISN  */
	
} PIN_ANSIX392;   


/**********************************************/
/*                                            */
/*������������������PED���ܽӿ�               */
/*                                            */
/**********************************************/

/********************************************
 ���ܣ������ӿڣ�ͬBeep()��
 ********************************************/
uchar PEDBeep(void);

/***********************************************
  OperMode=O_OPEN  ��PEDָʾ��
  OperMode=O_CLOSE �ر�PEDָʾ��
  Index          ����Ϊ����ֵ���������:
       RED       ���
       GREEN     �̵�
       YELLOW    �Ƶ�
��ע���ýӿ���Ϊ�պ�����δ��PEDָʾ�ƣ�       
************************************************/
uchar PEDLight(uchar OperMode, uchar Index);

/***********************************************
  OperMode=O_OPEN  ��PED������ʾ
  OperMode=O_CLOSE �ر�PED������ʾ
  ע���ýӿ���Ϊ�պ���
************************************************/
uchar PEDInvoice(uchar OperMode);

/***********************************************
  OperMode=O_OPEN  ��PED������
  OperMode=O_CLOSE �ر�PED������
  ע��ͬKbmute()
************************************************/
uchar PEDKbmute(uchar OperMode);

/***********************************************
  OperMode=O_OPEN  �򿪱���
  OperMode=O_CLOSE �رձ���
  ע��ͬScrBackLight()
************************************************/
uchar PEDBackLight(uchar OperMode);

/***********************************************
  ��������  �����ַ���
  str       ������ַ�������'\0'��β
  min       ��������С����
  max       ��������󳤶ȣ����40�ֽڣ�
  mode=0    ��������
  mode=1    ��������,��ʾ'*'
************************************************/
uchar PEDInput(uchar *str, uchar min, uchar max, uchar mode);

/***********************************************
  ���ܣ�������������ʱ����λΪ���룩
  ϵͳĬ��Ϊ��0���޳�ʱ
************************************************/
void PEDInputTimeOut(unsigned long timeoutsec);

/***********************************************
  ���ܣ�������ͬScrCls();
************************************************/
uchar PEDScrCls(void);

/***********************************************
  ���ܣ�����У�ͬScrClrLine();
************************************************/
uchar PEDScrClrLine(uchar line);

/***********************************************
  ��������  ��ʾ�ַ���(���ֺ�Ӣ�ģ���ͬScrPrint
  line      ��(0 - 7)
  col       ��(0 - 1127)
  str       ��ʾ���ַ���
************************************************/
uchar PEDScrPrint(uchar line, uchar col, uchar *str);

/***********************************************
  ��������  ��ʾ����
  line      ��(0 - 1)
  col       ��(0 - 121)
  Data      ��ʾ����(���ݸ�ʽ�ο�Э��)
  len       ��ʾ���ݳ���
************************************************/
uchar PEDScrWrData(uchar line, uchar col, uchar *Data, uchar len);

/************************************************
  ��������  д������Կ��Կ
  KeyID     ��Կ����(����Կ����
  Key       ��Կֵ
  mode=0x01 DES��Կ
  mode=0x03 3DES��SEED��Կ
*************************************************/
uchar PEDWriteMKey(uchar KeyID, uchar mode, uchar *Key);

/************************************************
  ��������  д��DES(3DES)�������Կ
  DESKeyID  ��Կ����(DES��Կ����
  Key       ��Կֵ
  mode=0x01 DES��Կ
  mode=0x03 3DES��SEED��Կ
************************************************/
uchar PEDWriteDKey(uchar DESKeyID, uchar mode, uchar *Key);

/************************************************
  ��������  д�빤����Կ
  MKeyID    �ø�����ָ��������Կ��Key����mode��ʽ
  �����������ļ������д��,MKeyID=0,�������κμ���,
  ����������ֱ��д��
  WKeyID    д��ĵ�ַ(������Կ��)
  Key       ��Կ
  mode=0x01 DES����
  mode=0x03 д��SEED��Կ��3DES����
  mode=0x81 DES����
  mode=0x83 3DES����
************************************************/
uchar PEDWriteWKey(uchar MKeyID, uchar WKeyID, uchar mode, uchar *Key);

/************************************************
  ��������  ��ɢ��Կ
  MKeyID    ����з�ɢ����Կ,��λΪ1����0x80)ָ����
  ����������������Կ����Կ������Ϊ����Կ����Կ
  WKeyID1   ��ɢ����(������Կ��)
  WKeyID2   ��ɢ���(������Կ��)
  mode=0x01 DES����
  mode=0x03 3DES����
  mode=0x81 DES����
  mode=0x83 3DES����
************************************************/
uchar PEDDeriveKey(uchar MKeyID, uchar WKeyID1, uchar WKeyID2, uchar mode);

/************************************************
  ��������     ��ȡANSI9.8���ܵ�����
  TwiceInput=1 PED������ʾ����2��
  TwiceInput=0 PED������ʾ����1��
  PinKeyID     ����PIN�Ĺ�����Կ(������Կ��)
  min          �����������С����
  max          �����������󳤶�
  pin          ���ܵ��������
  cardno       ������λ�����16λ���ţ�ASCII�룩
************************************************/
uchar PEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *cardno, uchar *pin,uchar TwiceInput);

/*---------------------------------------------------------------------
��ȡANSI9.8���ܵ�����[����3DES����]
ԭ�ͣ�uchar PPGetPwd_3Des(uchar PinKeyID, uchar min, uchar max, uchar *cardno, 
uchar *bPinLen,uchar *pin);
���ܣ�
      ��Ļ�����볤��Ϊ[min��max]������
������
      PinKeyID   ����PIN�Ĺ�����Կ(������Կ��)
      min       �����������С����[0=<min<=14]
      max       �����������󳤶�[0=<max<=14 && max>=min]
      cardno   	������λ�����16λ���ţ�ASCII�룩
      bPinLen	���볤��[������ݣ�min=<bPinLen <=max]��
      pin       ���ܵ�����[�������]
 ----------------------------------------------------------------*/
uchar PEDGetPwd_3Des(uchar PinKeyID, uchar min, uchar max, uchar *cardno,uchar *bPinLen,uchar *pin);


/************************************************
  ��������  DES(3DES)����
  DESKeyID  ����DES����Կ(DES��Կ��)
  datain    ��������
  dataout   �������
  mode=0x01 DES����
  mode=0x03 3DES����
  mode=0x81 DES����
  mode=0x83 3DES����
**************************************************/
uchar PEDDes(uchar DESKeyID, uchar mode, uchar *datain, uchar *dataout);

/************************************************
  ��������  MAC����
  flag=0    �㷨1
  flag=1    �㷨2
  WKeyID    ����MAC�Ĺ�����Կ(������Կ��)
  inLen     ����MAC�����ݳ���
  datain    ��������
  macout    �������(��MAC)
  mode=0x01 DES����
  mode=0x03 3DES����
**************************************************/
//uchar PEDMac(uchar WKeyID, uchar mode, uchar *datain, uchar inLen, uchar *macout, uchar flag);
uchar PEDMac(uchar WKeyID,uchar mode, uchar *datain, ushort inLen, uchar *macout,uchar flag);

/************************************************
  ��������  ��ȡPED�汾��Ϣ
  
  ver:   �汾��Ϣ[�����8�ֽ�]��
  ��ʽ���£�"HvXXSvXX"��
  HvXX������Hv����Ӳ���汾��XXΪ�汾�š�
  SvXX������Sv��������汾��XXΪ�汾�š�
**************************************************/
uchar PEDVerInfo(uchar *ver);

/************************************************
  ��������  ��ʾͼ�ε���
  ע���ú�����Ϊ�սӿڣ���ʹ��ScrDrLogoʵ�ָù���
**************************************************/
uchar PEDUpdLogo(uchar line, uchar *Data);

/***********************************************
���ܣ����PED����Ӧ���͵�KEY

������keytype=0   ����Կ
������keytype=1   ������Կ
������keytype=2   DES��Կ
���أ�
������0���ɹ�������0��ʧ��
*************************************************/
uchar PEDClearKey(uchar keytype);

/***********************************************
���ܣ�PED�������ܹ�����

mode��0   У�鳬��������Կ�����е�PWDCODEΪ
����������8�ֽڵ�У��ֵ����У��ɹ���ϵͳ�Զ�
������������������̱�־���ɸ��³�����Կ���
��������������Կ��
mode��1�� У�������Կ�����е�PWDCODEΪ8�ֽ�
������������У��ֵ����У��ɹ���ϵͳ�Զ����
���������������̱�־�����ɸ��½�����Կ��
mode��2���������̲���

mode��0X80  �޸ĳ���������Կ�����е�PWDCODE
����������  ���¹�����Կ��8�ֽڣ���ɡ���
mode��0X81���޸Ľ�����Կ�����е�PWDCODE����
����������  �½�����Կ��8�ֽڣ���ɡ�
���أ�
������0X00�����ɹ���
������0XF0����У����Կʧ��
������0XF1�����޸�Ȩ�޴���������ȷУ����Կ�ſɸ��£�
*************************************************/
uchar PEDManage(uchar mode,uchar *pwdcode);



/**********************************************/
/*                                            */
/*��������HONG KONG EPS ר�ù��ܽӿ�          */
/*                                            */
/**********************************************/

/***********************************************
ע�ͣ��˵���ʾ��������Ϊ�սӿڣ�
************************************************/
uchar EPSPEDDispMenu(uchar StartNo, uchar EndNo, uchar AllLines, uchar detail[][16], uchar *item);


/**************************************************
���ܣ�������

min��       �ַ�����С����
max��       �ַ�����󳤶�
Point_mode��0X00�� С���������λС��
������������0X01�� С���������λС��
������������0X02�� ��С����
prompt_mode:���ò�����ʱ��Ч[��������ʾ��Ϣ��
����������������Ϊ����ֵ]
Amt:	      ����Ľ��
****************************************************/
uchar EPSPEDAmount(uchar point_mode,uchar prompt_mode, uchar min, uchar max,uchar *Amt);

/***************************************************
���ܣ�װ�س�ʼTMK

MkeyID1��  TMK��д���λ�����(1-100)
TmpKey��   POS������ʱKEY[8 Bytes]
KTMK,��    ������ȡ������[8 Bytes]
ReferCode��������KTMK�����ļ���"\x12\x34\x56\x78\x90
������������\x12\x34\x56"�󣬷��ص�ֵ[8 Bytes]
*****************************************************/
uchar EPSPEDLoadTMK(uchar WkeyID1, uchar *TmpKey, uchar *KTMK, uchar *ReferCode);
uchar EX_EPSPEDLoadTMK(uchar WkeyID1, uchar *TmpKey, uchar *KTMK, uchar *ReferCode,uchar mode);

/***************************************************
���ܣ�EPSװ����Կ

WkeyID1���������ܻ򱻸��µ�TMK��λ�����(1-100)
WkeyID2����װ����Կ��λ�����(1-100)
KKEY��   �����´�������[8 Bytes]
*****************************************************/
uchar EPSPEDLoadKey(uchar WKeyID1,uchar WKeyID2, uchar *KKEY);
uchar EX_EPSPEDLoadKey(uchar WKeyID1,uchar WKeyID2, uchar *KKEY,uchar mode);

/******************************************************
���ܣ�EPS����"���MAK"

MkeyID�� �������ܵ�TMK��λ�����(1-100)
Random�� �����[��������]�򷵻صļ��ܽ�� [�������] [8bytes]
*******************************************************/
uchar EPSPEDGetMAC1(uchar WKeyID, uchar *Random);
uchar EX_EPSPEDGetMAC1(uchar WKeyID, uchar *Random,uchar mode);

/******************************************************
���ܣ�EPS����TPK[Pin_block]

WKeyID1����TMK / TPK��λ�����(1-100)
WKeyID2����TPK��λ�����(1-100)
pin_block��PIN���ܰ�[8bytes]�����º��������ܰ�Ҳ��������
ISN��	   �������к�[6bytes��ASCII��]
new_TPK:   �����´��ĸ��µ�TPK����[8bytes]
********************************************************/
uchar EPSPEDUpdateTPK(uchar WKeyID1,uchar WKeyID2, uchar *pin_block, uchar *ISN, uchar *new_TPK);
uchar EX_EPSPEDUpdateTPK(uchar WKeyID1,uchar WKeyID2, uchar *pin_block, uchar *ISN, uchar *new_TPK,uchar mode);


/******************************************************
���ܣ�EPS����"����MAK"

WKeyID1��      �������ܵ�TMK��λ�����(1-100)
WKeyID2��      MAC2��λ�����(1-100)
MAC2��	       �����´����µ�MAC2����[8bytes]
rsp_data��      rsp_data_len Bytes�����´�������
rsp_data_len:  �´�����rsp_data�ĳ���[rsp_data_len<=233]
rsp_MAC	��     �����´��������Ƚϵ�����[8�ֽڣ�ASCII�봮]
******************************************************/
uchar EPSPEDSetMAC2(uchar WKeyID1,uchar WKeyID2,uchar *MAC2, uchar *rsp_data, uchar rsp_data_len, uchar *rsp_MAC);
uchar EX_EPSPEDSetMAC2(uchar WKeyID1,uchar WKeyID2,uchar *MAC2, uchar *rsp_data, uchar rsp_data_len, uchar *rsp_MAC,uchar mode);



/******************************************************
���ܣ���ȡANSI3.92���ܵ�����

PinKeyID  ����PIN�Ĺ�����Կ(1-100)
min       �����������С����[0=<min<=6]
max       �����������󳤶�[0=<max<=6&&min<=max]
ISN       ������ˮ��ISN [6 Bytes��ASCII�봮]
pin       ���ܵ��������[������ݣ�8bytes]
******************************************************/
uchar EPSPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *ISN, uchar *pin);
uchar EX_EPSPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *ISN, uchar *pin,uchar mode);


/******************************************************
���ܣ� EPSУ��TMK

MkeyID��     �������ܵ�TMK��λ�����(1-100)
VerifyData�� ���ܺ�Ľ����8byte��[�������]
******************************************************/
uchar EPSPEDVerifyTMK(uchar WKeyID, uchar *VerifyData);
uchar EX_EPSPEDVerifyTMK(uchar WKeyID, uchar *VerifyData,uchar mode);



/*********************************************************
 ����ԭ�ͣ�uchar PED_PassWordEncrypt(uchar deskeyid,
 ������������������������������uchar *oldpin,uchar *despin)
 ����˵������DESKEYIDָ����DES��Կ��PED�й̶���KEY��OLDPIN����
 ����������DES���ܣ������������DESPIN�С�
 ����˵����DESKEYID��PED��DES��Կ������
 ����������OLDPIN����û�м��ܵ�PWD����ASCII�룬����<=16��
 ����������DESPIN�������ܵ�PWD����8�ֽڣ�
 ����ֵ����02����Կ�����Ŵ���
		   05������(PWD��)�Ƿ�
 ����������00���ɹ�
 **********************************************************/
uchar PED_PassWordEncrypt(uchar deskeyid,uchar *oldpin,uchar *despin);

/************************************************************
 ����ԭ�ͣ�uchar PED_PassWordInput(uchar deskeyid,uchar min,
                                    uchar max,uchar *inpin)

 ����˵������DESKEYIDָ����DES��Կ��PED�й̶���KEY�������PWD����
 ����������DES���ܣ������������INPIN�С�
 ����˵����DESKEYID��PED��DES��Կ������
 ����������MIN������ָ�������PWD����С�ֽ�����
 ����������MAX������ָ�������PWD������ֽ�����<=16��
 ����������INPIN�������ܵ�PWD����8�ֽڣ�

 ����ֵ����02����Կ�����Ŵ���
           05�������Ƿ�
		   0A��������Կ����Ϊ0
		   06������Աȡ������
   ��������07�����볬ʱ
 ����������00���ɹ�
 ************************************************************/
uchar PED_PassWordInput(uchar deskeyid,uchar min,uchar max,uchar *inpin);



/**********************************************/
/*                                            */
/*��������������������ר�ù��ܽӿ�            */
/*                                            */
/**********************************************/

/***************************************************
  ��������   ��ȡ���ܵ�����[�������Զ�����㷨:����
             �Ҷ��룬�á�F'��䣬��ʽ��FFFFFFFFFFPPPPPP��]
  PinKeyID   ����PIN�Ĺ�����Կ(������Կ��)
  min        �����������С����[<=6, <=max]
  max        �����������󳤶�[<=6]
  bPwdLen    �����PIN����
  pin        ���������[����]
*****************************************************/
uchar SYLPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *bPinLen,uchar *pin_block);

/******************************************************
��������    ��MKeyIDָ��������Կ��new_WKey����mode��ʽ��
            �ӽ��ܣ��õ�����Կ���ô���Կ��
            "\x00\x00\x00\x00\x00\x00\x00\x00"��DES���ܣ�
            ���������ԿУ�����ݱȽϣ������ͬ������Կ
            д��WkeyIDָ���Ĺ�����Կ����
MKeyID	    �洢����Կ������
WKeyID	    ��ɢ�����Կ�洢����
new_WKey    ������֤�ͷ�ɢ����Կ[8Bytes������]
KeyCv	    ��ԿУ������[8Bytes]��
mode        �ӽ���ģʽ�����嶨�����£�
      	    0x01  DES����
      	    0x81  DES����
******************************************************/
uchar SYLPEDVerifyDerive(uchar MKeyID, uchar WKeyID, uchar *new_WKey, uchar *KeyCv, uchar mode);

/***************************************************
��������    ����PinKeyIDָ���Ĺ�����Կ������pin_block
            ���н��ܣ�Ȼ����MKeyIDָ��������Կ��new_TPK
            ����mode��ʽ�ļӽ��ܣ��õ�����Կ���ô���Կ
            ������pin_block���ܡ�
MKeyID	    �������Կ[TMK]������
PinKeyID    ���PIN��Կ[TPK]������
pin_block   PIN��������[8Bytes������]
new_TPK     �µ�PIN��Կ[8Bytes������]
mode        �ӽ���ģʽ�����嶨�����£�
      	    0x01  DES����
      	    0x81  DES����
***************************************************/
uchar SYLPEDCalcPinBlock(uchar MKeyID,uchar PinKeyID,uchar *pin_block,uchar *new_TPK,uchar mode);






/**********************************************/
/*                                            */
/*��������VISA��PED��֤���ܽӿ�               */
/*                                            */
/**********************************************/

/***************************************************
 ���ܣ�����8�ֽڵ������
 ����������
 ���أ���
***************************************************/
void  PEDGetRandom(uchar *random);

/***************************************************
���ܣ���ʼ���ⲿ��֤��Կ

���룺       
      Ӧ������:     32byte����־��Ϣ����'\x0'��β
      �ⲿ��֤��Կ: 8byte�� ���ģ���Ӧ�ó������������ʹ��
                            ������֤��Կ���ܶ���
�������
***************************************************/
uchar PEDInitAuthKey(uchar *app_name,uchar *auth_key);

/***************************************************
���ܣ��ⲿ��֤
���룺       
     Ӧ������: 32byte����־��Ϣ����'\x0'��β
     ��֤����: 8byte���������ݣ����ⲿ��֤��Կ�Գ�����
               "\x98\x87\x76\x65\x12\x23\x34\x45"����
               ES���ܶ���
�����	    
    ��֤�루1BYTE��
***************************************************/
uchar PEDExternalAuth(uchar *app_name,uchar *auth_data,uchar *auth_id);

/***************************************************
���ܣ������ⲿ��֤
���룺       
     ��֤����:   8byte�����ģ���Ӧ�ó����������
�����	    
     ��֤�루1BYTE��
***************************************************/
uchar PEDSuperExternalAuth(uchar *auth_data,uchar *auth_id);

/***************************************************
���ܣ�����PIN����ANSI X3.92����
���룺       
      ��֤��:    1byte���ⲿ��֤ʱ��õķ���ֵ
      ʶ����:    1byte���ⲿ��֤��֤���ݵ����һ���ֽ�
      ���뷽ʽ:  1byte��
		 0x00������PIN
		 0x01������PIN
      PIN��Կ����: 1byte����ΧΪ[1��100]
      ����ģʽ:    1byte����PIN BLOCK�ļ��ܷ���
	    	   0x01��DES����
                   0x03��3DES����
      PIN��С����: 1byte��(Ϊ0ʱ����ENTER�����أ���ʱ
                   Ӧ�����ݳ���Ϊ0����ʾ�û�û����������) 
      PIN��󳤶�: 1byte��<=6
      ISN :        6byte��
�����	    
      ���ܽ��[8 bytes] ��0x00
***************************************************/
uchar PEDPinEncrypt_AnsiX392(PIN_ANSIX392 *Pin_X392,uchar *ret_data);

/***************************************************
���ܣ�����PIN����ANSI X9.8����.
���룺       
      ��֤��:   1byte���ⲿ��֤ʱ��õķ���ֵ
      ʶ����:   1byte���ⲿ��֤��֤���ݵ����һ���ֽ�
      ���뷽ʽ: 1byte��
		0x00������PIN
		0x01������PIN
      PIN��Կ����:   1byte����ΧΪ[1��100]
      ����ģʽ:      1byte����PIN BLOCK�ļ��ܷ���
	    	     0x01��DES����
                     0x03��3DES����
      PIN��С����:   1byte��(Ϊ0ʱ����ENTER�����أ���ʱ
                     Ӧ�����ݳ���Ϊ0����ʾ�û�û����������)
      PIN��󳤶�:   1byte��<=14
      ����:          16byte��
�����	    
      ���ܽ��[8 bytes] ��0x00
***************************************************/
uchar PEDPinEncrypt_AnsiX98(PIN_ANSIX98 *Pin_X98,uchar *ret_data);

/***************************************************
���ܣ�������֤��Կ.
���룺       
      ��֤��: 1byte���ⲿ��֤ʱ��õķ���ֵ
      ʶ����: 1byte���ⲿ��֤��֤���ݵ����һ���ֽ�
      ����Կ: 8byte�����ģ���Ӧ�ó������������ʹ��
                     ������֤��Կ���ܶ���
�����	    
      ��
***************************************************/
uchar PEDUpdateAuthKey(uchar auth_id,uchar identify_code,uchar *new_auth_key);

/***************************************************
���ܣ����PIN��Կ���ⲿ��֤��Կ
���룺       
      ��֤��:  1byte���ⲿ��֤ʱ��õķ���ֵ
      ʶ����:  1byte���ⲿ��֤��֤���ݵ����һ���ֽ�
�����	    
      ��
***************************************************/
uchar PEDErasePinKey(uchar auth_id,uchar identify_code);

/***************************************************
���ܣ�дPINר����Կ

���룺       
      ��֤�� :     1byte���ⲿ��֤ʱ��õķ���ֵ
      ʶ���� :     1byte���ⲿ��֤��֤���ݵ����һ���ֽ�
      ����Կ����:  1byte����Χ[1��100]������Կ����
      ������Կ����:1byte����Χ[1��100]����Ž�����Կ
      ��Կģʽ:    1byte��
                   0x01��DES��Կ
                   0x03��3DES��Կ
      ��Կ:        16byte�����ģ�����룬���������'0x00'
      �ӽ���ģʽ:  1byte��
		   0x01��DES����
	           0x03��3DES����
	           0x81��DES����
                   0x83��3DES����
�����	    
	��
***************************************************/
uchar PEDWritePinKey(PIN_KEY *pin_key);


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

#endif
