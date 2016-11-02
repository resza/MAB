
/****************************************************************************
Ӧ�ñ�׼��

�ٸ����������(����)���޹�˾

�޸���ʷ���޸����ݼ�<<�ٸ�Ӧ�ñ�׼��ʹ��ָ��>>
1.0.0 2008��9��12��    ��׼��ά����
1.0.1 2008��10��14��   ��׼��ά����
1.0.2 2009��3��11��   �޸���:���� ����Sabrina

****************************************************************************/

#ifndef _APPLIB_H
#define _APPLIB_H

#ifndef uchar
	typedef unsigned char	uchar;
#endif

#ifndef ushort
	typedef unsigned short	ushort;
#endif

#ifndef uint
	typedef unsigned int	uint;
#endif

#ifndef ulong
	typedef unsigned long	ulong;
#endif

// #define _POS_DEBUG

#ifndef TRUE
	#define TRUE	1
	#define FALSE	0
#endif

#define TIMER_TEMPORARY			4       // ��ʱ��Ķ�ʱ��(����ģ����Թ���)

// macros for debug output
#define DEVICE_SCR   0x01
#define DEVICE_PRN   0x02
#define DEVICE_COM1  0x04
#define DEVICE_COM2  0x08

#define ASC_MODE	0x10
#define HEX_MODE	0x20
#define ISO_MODE	0x40
#define TLV_MODE	0x80

// macros for vDes()
#define ONE_DECRYPT     0           // ��DES����
#define ONE_ENCRYPT     1           // ��DES����
#define TRI_DECRYPT     2           // ��DES����(16 bytes key)
#define TRI_ENCRYPT     3           // ��DES����(16 bytes key)
#define TRI_DECRYPT3    4           // ��DES����(24 bytes key)
#define TRI_ENCRYPT3    5           // ��DES����(24 bytes key)

// macros for vCalcMac()
#define MAC_ANSIX99     0           // ANSI9.9 standard MAC algorithm
#define MAC_FAST        1           // HyperCom fast MAC algorithm
#define MAC_ANSIX919	2			// ANSI9.19

#ifndef REVER
#define REVER           0x80		// Reverse background when display
#endif

// macros for ScrSetIcon()
#ifndef ICON_PHONE
#define ICON_PHONE      1       // phone
#define ICON_SIGNAL     2       // wireless signal
#define ICON_PRINTER    3       // printer
#define ICON_ICCARD     4       // smart card IC
#define ICON_LOCK       5       // lock
#define ICON_SPEAKER    6       // speaker
#define ICON_UP         7       // up
#define ICON_DOWN       8       // down
#define CLOSEICON		0       // �ر�ͼ��[�������ͼ��]
#define OPENICON        1       // ��ʾͼ��[��Դ�ӡ��/IC��/��/������/��/��]
#endif

// �������ժ�����һ�ͼ�꣬��action��ֵ��������
#define OFFHOOKICON     1       // hook off ժ��
#define ONHOOKICON      2       // hook on  �һ�
// ��������ź�ͼ�꣬��action��ֵ����Ϊ��INITSIGNAL+[0��5]
#define INITSIGNAL      1       // ��ʼ�ź�ǿ��[��ǿ�ź�ΪINITSIGNAL+5]

// macros for PubGetString()
#define NUM_IN			0x01	// ��������
#define ALPHA_IN		0x02	// ��ĸ���ֻ������
#define PASS_IN			0x04	// ��������
#define CARRY_IN		0x08	// �д�������
#define ECHO_IN			0x10	// ���Դ�������,���������ԭ����
#define AUTO_FONT		0x80	// RFU

// macros for PubGetStrParam()
#define GS_ECHOCONT         0x80        // �س��˳�(���Գ�ʼֵ?)
#define GS_CFONT            0x40        // ������
#define GS_NUMBER           0x20        // ������
#define GS_ALPHA            0x10        // ���ַ�
#define GS_PASSWD           0x08        // ���뷽ʽ
#define GS_LALIGN           0x04        // ���������
#define GS_FLOAT            0x02        // С����
#define GS_NORMAL           0x01        // ������ʾ

// macros for screen functions
#define NUM_MAXZHCHARS      16          // ÿ������ַ���Ŀ(������)
#define NUM_MAXCOLS         128         // ÿ���������
#define NUM_MAXCOLNO        (NUM_MAXCOLS-1)     // ����к�
#define NUM_MAXLINES        8           // �������
#define NUM_MAXLINENO       (NUM_MAXLINES-1)    // ����к�
#define NUM_MAXZHLINES      4           // �����������
#define NUM_MAXCHARS		21

#define DISP_LINE_LEFT		0x80
#define DISP_LINE_RIGHT		0x40
#define DISP_LINE_CENTER	0x20
#define DISP_SCR_CENTER		0x0100
#define DISP_LINE_REVER		0x0200
#define DISP_PART_REVER		0x0400

// menu display mode
#define MENU_NOSPEC		0x00	// �Զ���ʾ
#define MENU_USESNO		0x01	// ʹ�ò˵��Լ��ı��(������MENU_AUTOSNO)
#define MENU_AUTOSNO	0x02	// �Զ����
#define MENU_CENTER		0x10	// �м������ʾ
#define MENU_PROMPT		0x20	// ǿ����ʾ��Ϣ�ڵײ�
#define MENU_ASCII		0x40	// ǿ����ʾС����
#define MENU_CFONT		0x80	// ǿ����ʾ������(������MENU_ASCII)

// types for PubGetMenu()
typedef void (*MenuFunc)(void);
typedef struct _tagMenuItem{
	uchar		bVisible;						// TRUE: show it, FALSE: hide
    uchar       szMenuName[NUM_MAXCHARS+1];		// menu name
    MenuFunc    pfMenuFunc;						// callback function(if set)
}MenuItem;

//���ܲ˵���ض���

//ÿ�����ɵ��ַ�������Ӣ���ַ����㣬����ռ2��λ�ã�
#define SM_LINE_WIDTH  16
//���˵�����
#define SM_MAX_LINE    32

/*
 * �����˵���
 */
struct SMART_MENU;

typedef struct _tagMENU_ITEM{
    char                *pszText;  /*�˵�������ʾ*/
    int                 iValue;      /*��Ӧ�Ľ��׺�*/    
    struct SMART_MENU * pstSubMenu;   /*�Ӳ˵������ޣ���ΪNULL*/
}MENU_ITEM;

typedef struct SMART_MENU{
    MENU_ITEM       *pstContent;
    int             iNum;        /*�˵����� */
    uchar			sMask[SM_MAX_LINE];
}SMART_MENU;


#define  MENU_ICON_UP       7   /*up ����*/
#define  MENU_ICON_DOWN     8   /*down ����*/
    /*action - ָ������,    1:������0:Ϩ��*/
#define  MENU_CLOSEICON     0   /*�ر�ͼ��[�������ͼ��]*/
#define  MENU_OPENICON      1   /* ��ʾͼ��[��Դ�ӡ����IC�������������������ϡ�����]*/

//ÿ����ʾһ���˵��ȱʡ��ʾ������
#define SM_1ITEM_PER_LINE 0x00000001

//ʹ�����¼�ͷ��ȷ�ϼ�ѡ��ȱʡ�����ּ�ѡ��
#define SM_ARROW_SEL      (0x00000001<<1)

//��0��ʼ����˵��SM_ARROW_SEL��λʱ��Ч��ȱʡ��1��ʼ��
#define SM_ZERO_START     (0x00000001<<2)

//������ʾ����
#define SM_MIDDLE_TITLE   (0x00000001<<3)

//������ʾ�˵���
#define SM_MIDDLE_ITEM    (0x00000001<<4)

//��ȡ����ֱ�ӷ���
#define SM_DIRECT_RETURN  (0x00000001<<5)

#define SM_ON   1
#define SM_OFF  0

// define function pointer
typedef uchar (*FNPTR_ShowStringRightToLeft)(uchar lineno, uchar Mode, uchar *str);

typedef struct _tagAPPLIB_CONFIG 
{
    uchar   bStrFromRightToLeft;
    FNPTR_ShowStringRightToLeft pFnShowLeftRightString;
}APPLIB_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/****************************************************************************
 Function:      Calculate the offset of the struct member
 Param In:
    type        struct type
    member      struct member name
 Param Out:
    none
 Return Code:   the offset of the struct member
****************************************************************************/
/****************************************************************************
 ��    ��:      ����ṹ��Ա��ƫ��
 �������:
    type        �ṹ������
    member      �ṹ���Ա����
 �������:
    none
 �� �� ֵ:      �ýṹ���Ա��ƫ����
****************************************************************************/
#define OFFSET(type, member)    ( (ulong)(&((type *)0)->member) )

/****************************************************************************
 Function:      Calculate the maximum(minimum) of two int number/expression
 Param In:
    a,b         number/expression need to be compare
 Param Out:
    none
 Return Code:   maximum(minimum)
****************************************************************************/
/****************************************************************************
 ��    ��:      ����������������/���ʽ���(С)ֵ
 �������:
    a,b         ���Ƚϵ�����/���ʽ
 �������:
    none
 �� �� ֵ:      ���(С)ֵ
****************************************************************************/
#define MAX(a, b)       ( (a)>=(b) ? (a) : (b) )
#define MIN(a, b)       ( (a)<=(b) ? (a) : (b) )

/****************************************************************************
 Function:      Calculate the absolute value of int number/expression
 Param In:
    a           int number/expression
 Param Out:
    none
 Return Code:   absolute value
****************************************************************************/
/****************************************************************************
 ��    ��:      ������������/���ʽ����ֵ
 �������:
    a           ��������/���ʽ
 �������:
    none
 �� �� ֵ:      ����ֵ
****************************************************************************/
#define ABS(a)      ( (a)>=0 ? (a) : (-1)*(a) )

/****************************************************************************


                                      1.ϵͳ��������
 
   
	 
*****************************************************************************/


/****************************************************************************
Function:   Get current configuration of the applib.
Input Var:  None
Ouput Var:  pstCfg --- pointer to which the parameter will be output to.
return:     None
Memo:       1. This function always succeed.
            2. About the meaning of each member of the structure, read the
               definition of "APPLIB_CONFIG".
***************************************************************************/
void PubGetLibConfig(APPLIB_CONFIG *pstCfg);

/****************************************************************************
Function:   Write customized configuration back to the applib.
Input Var:  pstCfg --- pointer from which the parameter will be get and saved
                       into applib.
Output Var: None
return:     None
Memo:       1. This function always succeed.
            2. About the meaning of each member of the structure, read the
               definition of "APPLIB_CONFIG".
            3. Suggest to be used in pair with PubGetLibConfig.
            4. The configuration will not save after application quit.
***************************************************************************/
void PubSetLibConfig(APPLIB_CONFIG *pstCfg);

  
/****************************************************************************
���ܣ� 	        ��Դ����Ŀ�괮��һ�������������Ŀ�괮,��������2���ַ���ǰ
                uiLength�ֽ��������psOut�� 
�������	psVect1     �����Ĵ�
                psVect2     �����Ĵ�
                uiLength    Ҫ���ĳ���
�������	psOut       �����Ŀ�괮
���أ�  	��
˵����	        psVect1��psOut��Ϊͬһ����ַ
���3��ָ�����ΪNULL��������ֱ�ӷ��أ�
***************************************************************************/
void PubXor(uchar *psVect1, uchar *psVect2, uint uiLength, uchar *psOut);

/****************************************************************************
���ܣ� 	        ��2����Դ���ֽ��˫�����ȿɶ���16���ƴ���BCD ��ת���ɿɼ�ASCII�롣
�������	psIn      Դ�����ƴ�
                uiLength  ��Ҫת����Դ�������ֽ���
�������	psOut     ת�����ASC��
���أ�  	��
****************************************************************************/
void PubBcd2Asc(uchar *psIn, uint uiLength, uchar *psOut);

/****************************************************************************
���ܣ�  	��2���ƴ��ֽ��˫�����ȿɶ���16���ƴ�,ת����Ĵ��ԡ�\0�������� 
                BCD��ת����ASCII���ַ���������������
�������	psIn      Դ��
                uiLength  ��Ҫת����Դ�������ֽ���
�������	psOut    Ŀ�괮
���أ�  	��
****************************************************************************/
void PubBcd2Asc0(uchar *psIn, uint uiLength, uchar *pszOut);

/****************************************************************************
���ܣ� 	        ���ɶ���16���ƴ��ϲ�����һ�볤�ȵĶ����ƴ�,"12AB"-->0x12AB 
�������	psIn      Դ��
                uiLength  ԴAscii������
�������	psOut     Ŀ�괮
���أ�  	��

****************************************************************************/
void PubAsc2Bcd(uchar *psIn, uint uiLength, uchar *psOut);

/****************************************************************************
//�����ִ�ת��ɳ�����,�൱��atol(),���Ǹú�����Ҫ�����ַ�����
//Convert number string to long integer, similar to atol(). This function don't request end with '\0'.
****************************************************************************/
ulong PubAsc2Long(uchar *psString, uint uiLength, ulong *pulOut);

/****************************************************************************
���ܣ� 	        ������ת���ɸ�λ��ǰ���ַ�����ʾ(���������ֽڸ�ʽ���)
�������	ulSource   ��ת���ĳ�����
                uiTCnt    Ŀ�껺�����Ĵ�С(�ֽ���)
�������	psTarget   ת����Ĵ�
���أ�  	��
****************************************************************************/
void PubLong2Char(ulong ulSource, uint uiTCnt, uchar *psTarget);

/****************************************************************************
���ܣ� 	        ��λ��ǰ���ַ�����ʾת��Ϊ������
�������	psSource   ��ת���Ĵ������4���ַ�
                uiSCnt     Դ���ĳ��ȣ����Ϊ4������ʱ��ȡ��psSourceǰ��4����
                           ����������
�������	*pulTarget ת��������
���أ�  	ת��������
****************************************************************************/
ulong PubChar2Long(uchar *psSource, uint uiSCnt, ulong *pulTarget);

/****************************************************************************
���ܣ� 	        ������ת��ΪBCD��ʽ�Ĵ����
�������	ulSource    ��ת��������
uiTCnt          Ŀ�껺�����Ĵ�С(�ֽ���)
�������	psTarget    Ŀ�껺����
���أ�  	��
****************************************************************************/
void PubLong2Bcd(ulong ulSource, uint uiTCnt, uchar *psTarget);

/****************************************************************************
���ܣ� 	        ת��BCD��Ϊ����
�������	psSource    ��ת����BCD���ݸ�ʽ�Ĵ�
                uiSCnt      Դ���Ĵ�ת������
�������	*pulTarget  ת����������
                pulTarget��ַ����ΪNULL�����ΪNULL�������
                atol�ķ�����long���������ȡֵΪ2147483647
���أ�  	ת�����������ֵͬpulTarget
****************************************************************************/
ulong PubBcd2Long(uchar *psSource, uint uiSCnt, ulong *pulTarget);

/****************************************************************************
���ܣ� 	        ���ַ���ת��Ϊ��д
�������	pszString   ��ת�����ַ���,��'\0'����
�������	pszString   ת������ַ���,��'\0'����
���أ�  	��
****************************************************************************/
void PubStrUpper(uchar *pszString);

/****************************************************************************
���ܣ� 	        ���ַ���ת��ΪСд
�������	pszString   ��ת�����ַ���,��'\0'���� 
�������	pszString   ת������ַ���,��'\0'���� 
���أ�  	��
****************************************************************************/
void PubStrLower(uchar *pszString);

/****************************************************************************
���ܣ� 	        ɾ���ַ������˵Ŀհ��ַ�(����,�ո�/�س�/����/TAB�ַ�)
                ���˵Ļ�����Ҳ�ĸ���:��һ�������һ���Ǵ�4���ַ�Ϊ���,�м�
                �Ĳ�ɾ��.
�������	pszString   ��ת�����ַ���,��'\0'����
�������	pszString   ת������ַ���,��'\0'����
���أ�  	��
****************************************************************************/
void PubTrimStr(uchar *pszString);

/****************************************************************************
���ܣ� 	        ɾ���ַ������˵ĵ��ض��ַ�,��ɾ�������Ҳ��������ض��ַ�
                ���˵ĸ���:ͬPubTrimStr.
�������	pszString   ��ת�����ַ���,��'\0'����
                ucSpcChar   �ض��ַ�
�������	pszString   ת������ַ���,��'\0'����
���أ�  	��
****************************************************************************/
void PubTrimSpcStr(uchar *pszString, uchar ucSpcChar);

/****************************************************************************
���ܣ�          ɾ���ַ�������������ض��ַ�
�������	pszStringInOut     ��ת�����ַ���,��'\0'����
                ucRemoveChar  ��ɾ���ַ�
�������	pszString   ת������ַ���,��'\0'����
���أ�  	��

****************************************************************************/
void PubTrimHeadChars(uchar *pszString, uchar ucRemoveChar);

/****************************************************************************
���ܣ� 	        ɾ���ַ����ұ��������ض��ַ�
�������	pszStringInOut     ��ת�����ַ���,��'\0'����
                ucRemoveChar  ��ɾ���ַ�
�������	pszStringInOut    ת������ַ���,��'\0'����
���أ�  	��

****************************************************************************/
void PubTrimTailChars(uchar *pszString, uchar ucRemoveChar);

/****************************************************************************
���ܣ� 	        �����ִ�Сд���ַ����ȽϺ���
�������	pszStr1    ���Ƚϵ��ַ���1
                pszStr2    ���Ƚϵ��ַ���2 
�������	��/NONE
���أ�  	0          �������
                <0         pszStr1 < pszStr2
                >0         pszStr1 > pszStr2
                �������󣬲��������

****************************************************************************/
int PubStrNoCaseCmp(uchar *pszStr1, uchar *pszStr2);

/****************************************************************************
���ܣ� 	���ǰ���ַ�,�Ա�ﵽuiTargetLen����
�������	pszStringInOut    �����ǰ���ַ��Ĵ�
uiTargetLen       Ŀ�괮���� 
ucAddChar         �����ַ�
�������	pszStringInOut    ����Ĵ�
���أ�  	��
****************************************************************************/
void PubAddHeadChars(uchar *pszString, uint uiTargetLen, uchar ucAddChar);

/****************************************************************************
���ܣ� 	        ��ת�ַ���
�������	pszStringInOut   ����ת���ַ���,��'\0'����
�������	pszStringInOut   ��ת����ַ���,��'\0'����
���أ�  	��
****************************************************************************/
void PubStrReverse(uchar *pszStringInOut);

/****************************************************************************
���ܣ� 	        ȡ�ֽڵĸ���λ
�������	ucInChar    ��ȡ����λ���ֽ�
�������	��
���أ�  	����λ���ֽ�
****************************************************************************/
uchar PubHigh4Bit(uchar ucInChar);

/****************************************************************************
���ܣ� 	        ȡ�ֽڵĵ���λ
�������	ucInChar    ��ȡ����λ���ֽ�
�������	��
���أ�  	����λ���ֽ�
****************************************************************************/
uchar PubLow4Bit(uchar ucInChar);

/****************************************************************************
���ܣ�  	����ASC�ַ������
�������	pszFaciend  ������ASC�ַ�������\0Ϊ������
                pszMultiplier����ASC�ַ�������\0Ϊ������
�������	pszProduct��˵Ľ������\0Ϊ������
���أ�  	��
****************************************************************************/
void PubAscMul(uchar *pszFaciend, uchar *pszMultiplier, uchar *pszProduct);

/****************************************************************************
���ܣ� 	        ������ȵ�����ASC���ִ���ӡ�
�������	psAddend1   ������ASC�ַ����������㴮����uiLenҪ���ֶ���ǰ��
                            0��������ͬ��
                psAddend2   ����ASC�ַ���
                uiLen       �ַ����ĳ���
�������	pszSum      ��ӵĽ��
���أ�  	��
****************************************************************************/
void PubAscAdd(uchar *psAddend1, uchar *psAddend2, uint uiLen, uchar *pszSum);
#define SafeAscAdd(a,b,c)	PubAscAdd(a,b,c,a)

/****************************************************************************
���ܣ� 	        ������ȵ�����ASC�ַ��������ǰ��0
�������	psMinuend    ������ASC�����ַ���
                psSubtrahend ����ASC�����ַ���
uiLen           �ַ����ĳ���
�������	pszResult    ���
���أ�  	��
****************************************************************************/
void PubAscSub(uchar *psMinuend, uchar *psSubtrahend, uint uiLen, uchar *pszResult);
#define SafeAscSub(a,b,c)	PubAscSub(a,b,c,a)

/****************************************************************************
���ܣ� 	        ASC�ַ�����1,�������ԭ���� 
�������	psAscStr    ���1��ASC�ַ���
                uiStrLen     psAscStr�ĳ���
�������	psAscStr    ��1���ASC�ַ���
���أ�  	0   �ɹ�    successful
                1   ʧ��    failed
****************************************************************************/
uchar PubAscDec(uchar *psAscStr, uint uiStrLen);

/****************************************************************************
���ܣ� 	        ASC�ַ�����1,�������ԭ���� 
�������	psAscStr    ���1��ASC�ַ���
                uiStrLen     psAscStr�ĳ���
�������	psAscStr    ��1���ASC�ַ���
���أ�  	0   �ɹ�
                1   ʧ��
****************************************************************************/
uchar PubAscInc(uchar *psAscStr, uint uiStrLen);

/****************************************************************************
���ܣ� 	        ������ȵ�����BCD�ַ������
�������	psAddend1   ������BCD�ַ���
                psAddend2   ����BCD�ַ���
                uiLen       �ַ����ĳ���
�������	psResult    ��ӵĽ��
****************************************************************************/
void PubBcdAdd(uchar *psAddend1, uchar *psAddend2, uint uiLen, uchar *psResult);
#define SafeBcdAdd(a,b,c)		PubBcdAdd(a, b, c, a)

/****************************************************************************
���ܣ� 	        ������ȵ�����BCD�ַ��������ǰ��0
�������	psMinuend      ������BCD�ַ��� 
                psSubtrahend   ����BCD�ַ���
                uiLen          �ַ����ĳ���
�������	psResult        ����Ľ��
****************************************************************************/
void PubBcdSub(uchar *psAddend1, uchar *psAddend2, uint uiLen, uchar *psResult);
#define SafeBcdSub(a,b,c)		PubBcdSub(a, b, c, a)

/****************************************************************************
���ܣ� 	        BCD�ַ�����1,�������ԭ����
�������	psBcdStr    ���1��BCD�ַ���
                uiStrLen     psBcDStr�ĳ���
�������	psBcdStr    ��1���BCD�ַ���
���أ�  	0   �ɹ�    successful
                1   ʧ��    failed
****************************************************************************/
uchar PubBcdDec(uchar *psBcdStr, uint uiStrLen);

/****************************************************************************
���ܣ� 	        BCD�ַ�����1,�������ԭ���� 
�������	psBcdStr    ���1��BCD�ַ���
                iStrLen     psBcdStr�ĳ���
�������	psBcdStr    ��1���BCD�ַ���
���أ�  	0   �ɹ�    successful
                1   ʧ��    failed
****************************************************************************/
uchar PubBcdInc(uchar *psBcdStr, uint uiStrLen);

/****************************************************************************
���ܣ� 	        ת�����ִ�������ʽ����HKD$****.**, RMB****.**
�������	pszPrefix  ǰ���ǰ׺���硰HKD 200.00���еġ�HKD������
                          ��HKD$200.00���еġ�HKD.�����������4���ַ�.���Ϊ�գ�
                           �򲻼�ǰ׺��
                pszIn      ��Ҫת���Ĵ����ִ�,���˵�ǰ��ġ�0�����硱00100��
                           �����Ϊ��100��,��󳤶�Ϊ40
                ucDeciPos   С�����Ľ��λ��������С��4
                           ���� ��ucDeciPos==2, "14277" ת��Ϊ "142.77"
                           if ucDeciPos==0, "14277" ת��Ϊ "14,277"�򲻱䡣
                ucMisc      �������� 
                           GA_SEPARATOR: ʹ�ö��ŷָ������"12,345.67"
                           GA_NEGATIVE: ���ǰ�Ӹ��ţ� "SGD-123,45.67"
�������	pszOut      ��ʽ����Ľ�
���أ�  	��
****************************************************************************/
void PubConvAmount(uchar *pszPrefix, uchar *pszIn, uchar ucDeciPos, uchar ucIgnoreDigit, uchar *pszOut, uchar ucMisc);

/****************************************************************************
Ӧ��ִ���쳣�жϺ���
ԭ�ͣ�	void PubHalt(uchar *pszfile, uint uiLine)
���ܣ� 	��ʾһЩ��Ϣ,������ѭ��.
��ʾ��Ϣ:
	File:�ļ���, Line:����
	Ret:ϵͳErrorֵ
	PLS RECORD THEN RESTART POS
�������	pszfile     �ļ���.
			uiLine      ������ʱ�����Ĵ�������
�������	None
���أ�  	None
ʾ��˵����	
****************************************************************************/
void PubHalt(uchar *pszfile, uint uiLine);

/****************************************************************************
 Function:      Beep 800ms at frequency 6.
 Param In:      None.
 Param Out:     None
 Return Code:   None
****************************************************************************/
void PubLongBeep(void);	

/****************************************************************************
 Function:      Beep 60ms at frequency 6 for 3 times, with 80ms interval.
 Param In:      None.
 Param Out:     None
 Return Code:   None
****************************************************************************/
void PubBeepOk(void);		

/****************************************************************************
 Function:      Beep 200ms at frequency 6,then delay 200ms.
 Param In:      None.
 Param Out:     None
 Return Code:   None
****************************************************************************/
void PubBeepErr(void);	

/****************************************************************************
 Function:      �๦��DES����/��������
 Param In:
    ucMode      DES����/����ģʽ:
                ONE_ENCRYPT --> ��DES����
                ONE_DECRYPT --> ��DES����
                TRI_ENCRYPT --> ��DES����
                TRI_DECRYPT --> ��DES����
    psData:     ������/���ܵ�����(8�ֽ�)
    psKey:      DES�������Կ(�������Ȼ�����������,����ucModeȷ��)
 Param Out:
    psResult    DES����Ľ��(8�ֽ�)
 Return Code:   none
****************************************************************************/
void PubDes(uchar ucMode, uchar *psData, uchar *psKey, uchar *psResult);	//

/****************************************************************************
 Function:      ����MAC���㺯��
 Param In:
    ucMode      ����MAC����ģʽ:
                MAC_FAST    --> ����MAC����(HyperCom���ٷ�ʽ)
                MAC_ANSIX99 --> ANSI x9.9 ��ʽ
				MAC_ANSIX919--> ANSI x9.19 ��ʽ
    psKey:      MAC�������Կ(8�ֽ�)
    psMsg:      ����MAC������
    uiLength:   ����MAC�����ݵĳ���
 Param Out:
    psMAC       ���ĵ�MAC(8�ֽ�)
 Return Code:   none
****************************************************************************/
void PubCalcMac(uchar ucMode, uchar *psKey, uchar *psMsg, uint uiLen, uchar *psMAC);
	
/****************************************************************************
���ܣ� 	        ����LRC
�������	psData    ��Ҫ���������
                uiLength  ���ݵĳ���
�������	*pucInit  LRCֵ
���أ�  	��
****************************************************************************/
void PubCalcLRC(uchar *psData, ushort uiLength, uchar *pucInit);

/****************************************************************************
���ܣ� 	        У�鿨���Ƿ�����ģ10У��
�������	pszPan   ����
�������	��
���أ�  	0    ����
                1    ������
****************************************************************************/
uchar PubChkCardNo(uchar *pszPanNo);



/****************************************************************************


                                      2	��ʾ����
 
   
	 
*****************************************************************************/

/****************************************************************************
���ܣ� 	����Ļ��һ���Դ�������ʾTITLE�����ԡ������ݲ���bClrScreen�����Ƿ����
        ��Ļ�·�(2-7��)�����ݡ�
�������	bClrScreen   TRUE: �����Ļ�·�(2-7��)������
                             FALSE: ����ԭ��Ļ(2-7��)������
                pszTitle     �ԡ�\0����������Ҫ��ʾ���ַ�����һ��һ��ֻ����ʾ
                             16�����ַ���8�����֡�
�������	��
���أ�  	��

****************************************************************************/
void PubShowTitle(uchar bClrScreen, uchar *pszTitle);	//

/****************************************************************************
���ܣ� 	��ucLine�д������������ʾ��ϢpszMsg���� pszMsg���ȳ���һ�еĽ��ᱻ�ص�
�������	ucLine      ��ʾ��Ϣ�����������ڲ���ΪucLine & REVER�������Ϣ
                            ��ucLine����ʾ��ͬʱ��Ϊ����״̬��ʾ
                pszMsg      ��Ҫ��ʾ����Ϣ���ԡ�\0��Ϊ������
�������	��
���أ�  	��
 Return Code:   none
****************************************************************************/
void PubShowMsg(uchar ucLine, uchar *pszMsg);			//

/****************************************************************************
���ܣ� 	�����ַ���ģʽ������ʾ2����Ϣ���ֱ���ʾ��ucLine�м�ucLine+2�У�ÿ�г�
        ��һ����ʾ��Ϣ�����ݽ��ᱻ�ص���
�������	ucLine    ��һ����ʾ�����ݡ����ڲ���ΪucLine & REVER�������
                          Ϣ��ucLine����ʾ��ͬʱ��Ϊ����״̬��ʾ������Ҫ��ʾ
                          2�д��ַ������ucLine��ֵӦΪ0-4.
                pszMsg1   ������ʾ����Ϣ���ԡ�\0��Ϊ������
                pszMsg2   �ڶ�����ʾ����Ϣ���ԡ�\0��Ϊ������
�������	��
���أ�  	��
****************************************************************************/
void PubShowTwoMsg(uchar ucLine, uchar *pszMsg1, uchar *pszMsg2);		//


/****************************************************************************
���ܣ� 	�����ַ���ָ������ʾָ��ģʽ���ַ���
�������	pszStr      ��Ҫ��ʾ���ַ������ԡ�\0��Ϊ������
                ucPosition   ��Ҫ����ʾ������|��ʾ�ķ�ʽ
                ��ʾ�ķ�ʽ�� 
                DISP_SCR_CENTER,   ������Ļ
                DISP_LINE_CENTER,  �о���
                DISP_LINE_ RIGHT    �о���
                DISP_LINE_LEFT      �о���
�������	��
���أ�  	��
****************************************************************************/
void PubDispString(void *pszStr, ushort nPosition);

/****************************************************************************


                                      3	ʱ�亯��
 
   
	 
*****************************************************************************/
#define STAR_YEAR 1980
#define STAR_WEEK 6

/****************************************************************************
���ܣ� 	��һ��ulong������ת��Ϊ����+ʱ���ַ�������1980��1��1��0ʱ��ʼ�㣬 ���
        ʱ����Ե�2116��1��1��
�������	ulSecond            ��ת����ulong����,�䵥λ�ĺ�������
�������	pszYYYYMMDDhhmmss   �õ�������+ʱ���ַ�����14���ֽ���00h��β
���أ�  	None
****************************************************************************/
void PubLong2Time(ulong ulSecond,uchar *pszYYYYMMDDhhmmss);

/****************************************************************************
���ܣ� 	��һ��ulong������ת��Ϊ�����ַ�������1980��1��1�տ�ʼ��
�������	ulDay           ��ת����ulong����,�䵥λ�ĺ�������
�������	pszYYYYMMDD     �õ��������ַ�����8���ֽ���00h��β
���أ�  	None
****************************************************************************/ 
void PubLong2Day(ulong ulDay,uchar *pszYYYYMMDD);  

/****************************************************************************
���ܣ� 	��һ������+ʱ���ַ���ת��Ϊulong����������1980��1��1��0ʱ��ʼ�㣬���
        ʱ����Ե�2116��1��1��
�������	psYYYYMMDDhhmmss       ��Ҫ��ת��������+ʱ���ַ���
�������	None
���أ�  	ת����ulong�������䵥λ�ĺ�������
****************************************************************************/ 
ulong PubTime2Long(uchar *psYYYYMMDDhhmmss);

/****************************************************************************
���ܣ� 	��һ�������ַ���ת��Ϊulong����������1980��1��1�տ�ʼ��
�������	psYYYYMMDD       ��Ҫ��ת���������ַ���
�������	None
���أ�  	ת����ulong�������䵥λ�ĺ�������
****************************************************************************/ 
ulong PubDay2Long(uchar *psYYYYMMDD);

/****************************************************************************
���ܣ� 	 ����MASK����ĸ�ʽ���������ʱ���ʽ�Ƿ���Ч
�������	 psDateTime       ����������ʱ���ַ���
                 psMaskYYYYMMDDhhmmss
                 ��ʽ���ƣ����ַ�������Ϊ"YYYYMMDDhhmmss"�������Ӵ���������ż
                 ����������÷��е����ӡ�
                 �������YYYY��ʽ�������Ϊ19YY����20YY��������Ϊ�Ƿ�
                 �������YY��ʽ�����Զ���Ϊ��20YY��
                 �����ʽ����YMMD�ȷ�����ʹ�ã��򲻱�֤�жϽ����ȷ��
�������	 
���أ�  	 0��	��Ч
                 1��2��3��4��5��6���ֱ�����ꡢ�¡��ա�ʱ���֡��롢�Ƿ�
                 9������MASK��ʽ����
****************************************************************************/ 
uchar PubIsValidTime(uchar *psDateTime,uchar *psMaskYYYYMMDDhhmmss);

/****************************************************************************
���ܣ� 	���������ʱ�俪ʼ,�������ʱ����ʱ���,�����괦��
�������	 psDateTime:      ��Ҫ�ۼӵĻ���ʱ��,��ʽΪYYYYMMDDhhmmss
                 pszDateTimeOut:  �ۼӺ��ʱ��,��ʽΪYYYYMMDDhhmmss
                 ulInterval:      ��Ҫ�ۼӵ�ʱ��,��Ϊ��,��,ʱ,��,��,��
                 psCalMode:       ����ۼӵĵ�λ,�ֱ������,��,ʱ,��,��,��;
                                  ��Ϊ��YY�� ��MM�� ��DD����hh����mm����ss����
                                  ���ģʽ�������ΪNULL����Ĭ��Ϊ��ss
�������	 None
���أ�  	 None 
****************************************************************************/ 
void PubCalDateTime(uchar *psDateTimeIn, uchar *pszDateTimeOut, ulong ulInterval, uchar *psCalMode);

/****************************************************************************
���ܣ� 	��������ʱ���֮��ļ��
�������	psTimeBegin:    ��ʼʱ��,��ʽΪYYYYMMDDhhmmss
                psTimeEnd:      ����ʱ��,��ʽΪYYYYMMDDhhmmss
                psCalMode:      ����ĵ�λ,�ֱ������,��,ʱ,��,��,�ꣻ
                                ��Ϊ��YY����MM����DD����hh����mm����ss����
                                ���ģʽ�������ΪNULL����Ĭ��Ϊ��ss��
�������	pucSignal:      ���������, 1:��,0:��
���أ�  	���ʱ��
****************************************************************************/ 
ulong PubCalInterval(uchar *psTimeBegin, uchar *psTimeEnd, uchar *psCalMode, uchar *pucSignal);

/****************************************************************************


                                      4	���뺯��
 
   
	 
*****************************************************************************/
/****************************************************************************
���ܣ� 	�ڳ�ʱʱ���ڵȴ�ȷ�ϼ�ȡ���� 
�������	nWaitTime    >0: �ȴ������ʱ�䣬��λΪ��
                             =0: ��������.
�������	��
���أ�  	0           ����ENTER��
                0xFF        ����CANCEL����ʱ.
****************************************************************************/
uchar PubYesNo(ushort nWaitTime);


uchar PubYesNoFNUP(ushort nWaitTime);
uchar PubYesNoUpDown(ushort nWaitTime);



/****************************************************************************
���ܣ� 	��ȡ����������ֵ
�������	ucLine       ��ʾ��ʾ��Ϣ���У���������ʾ
                pszPrompt    ��Ҫ��ʾ����ʾ��Ϣ��
�������	pucResult    ����������ֵ.
���أ�  	0            ����OK
                0xFF         �û�ȡ�����߳�ʱ����ʱʱ��Ϊ60�룩
****************************************************************************/
uchar PubGetBoolParam(uchar *pszPrompt, uchar ucLine, uchar *pucResult);

/****************************************************************************
���ܣ� 	��ʽ������������õ���Ļ�ĵ�6-7�н��и�ʽ����ʾ
�������	pszPrefix    ���Ҵ����ָ�룬����"HKD", "SGD$"����Χ��[0,4]��
                             �ڣ�����ΪNULL 
                ucDeciPos    С�����λ�ã���Χ��[0,5]���������롱RMB����ʱ��
                             ��2
                ucMinLen     ������С����
                ucMaxLen     ������󳤶�
                ucTimeout    ���볬ʱʱ�䣬��λ�룬0��ʾһֱ�Ⱥ�.
                ucMisc       ���Ʋ�����GA_SEPARATOR:��ʾʹ��',���Ž��н���ʽ
                             ����ʾ����: "HKD$12,345.67"��GA_NEGATIVE:��ʾ���ţ�
                             ��: "SGD-123,45.67"��
�������	pszData      �������ַ�������00H��β������ʾ��HKD$12,345.67��,
                             ���õ���1234567���Ľ����
���أ�  	0            OK.
                0xFE         ��������Ƿ�
                0xFF         ����CANCEL�������߳�ʱ�˳�
****************************************************************************/
#define GA_SEPARATOR	0x80
#define GA_NEGATIVE		0x40
uchar PubGetAmount(uchar *pszPrefix, uchar ucDeciPos,
				  uchar ucMinLen, uchar ucMaxLen, uchar *pszData,
				  uchar ucTimeout, uchar ucMisc);

/****************************************************************************
���ܣ� 	���������ַ�������ĸ�ַ���������
�������	ucMode     �����ģʽ
                  NUM_IN		0x01  // ��������
                  ALPHA_IN		0x02  // ��ĸ���ֻ������
                  PASS_IN		0x04  // ��������
                  CARRY_IN		0x08  // �д�������
                  ECHO_IN		0x10  // ���Դ�������,���������ԭ����
                ucMinlen    �������С����
                ucMaxlen    �������󳤶�
                pszOut      ��CARRY_IN��ECHO_IN����ģʽʱ��������ݣ��ַ�������
                            �ԡ�\0����β
                ucTimeOut   ���볬ʱʱ�䣬��λ�룬0��ʾ��������.
�������	pszOut      ������ַ������ԡ�\0����β
���أ�  	0           ����ENTER��
                0xFF        ����CANCEL����ʱ.
****************************************************************************/
uchar PubGetString(uchar ucMode, uchar ucMinlen, uchar ucMaxlen, uchar *pszOut, uchar ucTimeOut);

/****************************************************************************
���ܣ� 	�ж��Ƿ��а���
�������	��
�������	��
���أ�  	TRUE       �а���
                FALSE      �ް���
****************************************************************************/
uchar PubKeyPressed(void);

/****************************************************************************
���ܣ� 	�ȴ�����.
�������	iWaitTime    >=0: ��ʱʱ��
                             <0:  �޳�ʱʱ��.
�������	��.
���أ�  	����ֵ�����ް�����ʱ�˳�(NOKEY).
****************************************************************************/
uchar PubWaitKey(short iWaitTime);

/****************************************************************************


                                      5	�˵�����
 
   
	 
*****************************************************************************/

/****************************************************************************
���ܣ� 	��ʾ�����ò˵������ܡ��˵������Ϊ99���˵���ÿ�������ʾ����Ϊ19��
        �ֽ�
�������	pszPrompt       �˵�������/��ʾ���ԣ���ѡ������ΪNULL�򡰡���
                                �ַ��������ԡ�\0��������Ĭ����ʾ"PLS INPUT:"
                pstMenu         �Բ˵��ṹ���飬�Կմ������Ĳ˵����ʾΪ�˵�β��
                ucMode          �˵���ʾ��ģʽѡ��: 
                MENU_NOSPEC     �Զ���ʾ
                MENU_AUTOSNO    �Զ����
                MENU_USESNO     ʹ�ò˵��Լ��ı��(������MENU_AUTOSNO)
                MENU_ CENTERR   �м������ʾ
                MENU_PROMPT     ǿ����ʾ��Ϣ�ڵײ�
                MENU_CFONT      ǿ����ʾ������(������MENU_ASCII)
                MENU_ASCII      ǿ����ʾС����
                nTimeOut   >0:  ��ʱʱ�䣨��λΪ�룩
                           =0:  ��������.
�������	��
���أ�  	>=0             ��ѡ���������
                -1              �˵�ָ��Ϊ��.
                -2              ��Ч�Ĳ˵�����
                -3              �޲˵�����
                -4              �û�ȡ����ʱ
****************************************************************************/
int PubGetMenu(uchar *pszPrompt, MenuItem *pstMenu, uchar ucMode, ushort nTimeOut);
int PubGetMenuNew(uchar *pszPrompt, MenuItem *pstMenu, uchar ucMode, ushort nTimeOut);




/********************************************************************************
 * ������: PubSmInit
 * ��  ��: ��ֻ�ܲ˵����ƺ�����
 * ��  ��: pstMenuItem	- ��ʼ�˵�����
 * ��  ��: pstSmartMenu	- ���ܲ˵�ָ��
 * ����ֵ: ��
 ********************************************************************************/
void PubSmInit(SMART_MENU *pstSmartMenu, MENU_ITEM *pstMenuItem);


/************************************************************************
����: ���˵����ƿ��ز˵���Ŀ
�������: pszName		- �˵�������
          iFlag			- ���� SM_ON / SM_OFF
�������: pstSmartMenu	- �˵�ָ��
����:��
************************************************************************/
void PubSmMask(SMART_MENU *pstSmartMenu, const char *pszName, int iFlag);



/********************************************************************************
����: ���ܲ˵�
�������: pstSmartMenu   - ���ܲ˵�ָ��
          uiMode - �˵���ʾģʽ ����Ϊ���²�������ϣ���Ϊ0��ȡ����ȱʡ��ʽ
                 SM_1ITEM_PER_LINE ÿ����ʾһ���˵���
                ��ȱʡ��ʾ����,��SM_ARROW_SELģʽ��ǿ��ÿ��һ����
                 SM_ARROW_SEL  ʹ�����¼�ͷ��ȷ�ϼ�ѡ��ȱʡ�����ּ�ѡ��
                 SM_ZERO_START  ��0��ʼ����˵���(SM_ARROW_SELģʽ����Ч)
                ��ȱʡ��1��ʼ��
                 SM_MIDDLE_TITLE   ������ʾ���� (ȱʡ����)
                 SM_MIDDLE_ITEM  ������ʾ�˵���(ȱʡ����)
        iTimeout - ��ʱʱ�䣨����Ϊ��λ������Ϊ���ʾ�޳�ʱ 
�������: ��
����:    >0   ѡ���Ľ��׺�
         -1   ȡ����������ʱ��
 ********************************************************************************/
int PubSmartMenuEx(SMART_MENU *pstSmartMenu, uint uiMode, int iTimeout);
int PubSmartMenuExNew(SMART_MENU *pstSmartMenu, uint uiMode, int iTimeout);

  
/****************************************************************************


                                      6	�ļ�����
 
   
	 
*****************************************************************************/
/****************************************************************************
 Function:      Read data from file. Should use Seek() to set write pointer first.
 Param In:
    fd          file handle to be read from.
	psData      data pointer to place data read
    iNeedByte   number of bytes to be read out.
 Param Out:
    none.
 Return Code:
    total read out bytes.
****************************************************************************/
int PubFReadN(int fd, void *psData, int iNeedBytes);

/****************************************************************************
���ܣ� 	���ļ�д������
�������	pszFileName   ���ж��������ļ���
                lOffset        ��������ƫ����
                iLen          ���������ݳ���
�������	psData        �����������׵�ַ
���أ�  	0	�ɹ�
                1	���ļ�����
                2	��λƫ�Ƶ�ַ����
                3	�����ݳ���
****************************************************************************/
uchar PubFileRead(uchar *pszFileName, long lOffset, void *psData, int iLen);		//

/****************************************************************************
 Function:      Write data into file. Should use Seek() to set write pointer first.
 Param In:
    fd          file handle to be written into
	psData      pointer of data source to write into file
    iNeedBytes  number of bytes to write
 Param Out:
    none
 Return Code:
    total write bytes.
****************************************************************************/
int PubFWriteN(int fd, void *psData, int iNeedBytes);

/****************************************************************************
���ܣ� 	������д���ļ�
�������	pszFileName    ����д�������ļ���
                lOffset        д������ƫ����
                psData         д��������׵�ַ
                iLen           д������ݳ���
�������	��
���أ�  	0	�ɹ�
                1	���ļ�����
                2	��λƫ�Ƶ�ַ����
                3	д���ݳ���
****************************************************************************/
uchar PubFileWrite(uchar *pszFileName, long lOffset, void *psData, int iLen);	//




/****************************************************************************


                                      7	Debug�������
 
   
	 
*****************************************************************************/


/****************************************************************************
���ܣ� 	�๦��Debug�����������
�������	pszTitle     Debug���ݵı����ַ���������ΪNULL����ΪNULL��Ҫ��
                             0x00��β������32�ֽڵĻᱻ����
                psData       Debug����ָ��
                usDataLen    Debug���ݳ���
                ucDevice     ����豸����Ϊ��
                             ��Ļ���룬DEVICE_SCR(0x01)
                             ��ӡ�����DEVICE_PRN(0x02)
                             ����1���, DEVICE_COM1(0x04)
                             ����2���, DEVICE_COM2(0x08)
                ucMode       ���ݵĽ���ģʽ����Ϊ��
                             ASC��ģʽ��ASC_MODE(0x10) 
                             ʮ������ģʽ��HEX_MODE(0x20) 
                             ISO8583ģʽ,ISO_MODE(0x40) 
                             TLVģʽ,TLV_MODE(0x80) 
�������	��
���أ�  	0x00         �ɹ�
                0xff         ʧ��
****************************************************************************/
uchar PubDebugOutput(uchar *pszTitle, uchar *psData, ushort nDataLen, uchar ucDevice, uchar ucMode);


/****************************************************************************
���ܣ� 	ָ��PubDebugOutput�����н��ʱ��ISO8583���Ķ���ṹ
������	pvDataDef�� ָ��ISO8583������ṹ��ָ��
���أ�  ��
****************************************************************************/
void PubISODefine(void *pvDataDef);

#ifdef _POS_DEBUG

/****************************************************************************
 Function:      Show HEX message on screen.
 Param In:
    pszTitle    Title of the message.
    psMsg       Pointer of message to be displayed.
    uiMsgLen    Bytes of the message.
    nTimeOut    Seconds between user interaction.
 Param Out:
    none
 Return Code:   none
****************************************************************************/
void DispHexMsg(uchar *pszTitle, uchar *psMsg, uint uiMsgLen, short nTimeOut);

// print a line as hexadecimal format
int DispHexLine(uchar *pszBuff, uint uiOffset, uchar *psMsg, uint uiMsgLen);

// For Debug use, disp file name and line

/****************************************************************************
���ܣ� 	��ʾ�ļ���������
���������	pszFileName:       ��ʾ���ļ���
                ulLineNo��         ��ʾ������
���������	��
���أ�  	��
****************************************************************************/
void DispAssert(uchar *pszFileName, ulong ulLineNo);

    // debug macro for boolean expression
	#define _POS_DEBUG_WAIT		15
    #define PubASSERT(expr)                                           \
                if( !(expr) ){                                      \
                    DispAssert((uchar *)__FILE__, (ulong)(__LINE__));       \
                }

    // print string debug information
    #define PubTRACE0(sz)                                             \
            {                                                       \
                ScrCls();                                           \
                ScrPrint(0, 0, CFONT, "%s", sz);                    \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE1(sz, p1)                                         \
            {                                                       \
                ScrCls();                                           \
                ScrPrint(0, 0, CFONT, sz, p1);                      \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE2(sz, p1, p2)                                     \
            {                                                       \
                ScrCls();                                           \
                ScrPrint(0, 0, CFONT, sz, p1, p2);                  \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE3(sz, p1, p2, p3)                                 \
            {                                                       \
                ScrCls();                                           \
                ScrPrint(0, 0, CFONT, sz, p1, p2, p3);              \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACEHEX(t, s, l)    DispHexMsg((t), (s), (l), _POS_DEBUG_WAIT)

#else /* _POS_DEBUG */

    #define PubASSERT(expr)
    #define PubTRACE0(sz)
    #define PubTRACE1(sz, p1)
    #define PubTRACE2(sz, p1, p2)
    #define PubTRACE3(sz, p1, p2, p3)
    #define PubTRACEHEX(t, s, l)

#endif /* _POS_DEBUG */


/****************************************************************************
                           ����ѡ����
 ��������: ����������1��2��2��״̬��ѡ�񣬻��߲�ѡ������������Ӧ��ѡ��
 �������:                 pszTitle:     ��һ����ʾ����
                           bCondition:  ����ֵ(TRUE FALSE)
                           pszTruePrmpt: ����������ʾ
                           pszFalsePrmpt:������������ʾ
                           pszOption1:     ѡ��һ��ʾ
                           pszOption2:     ѡ�����ʾ
                           iTimeOut:   ��ʱʱ�䣬��, 0��ʾ�޳�ʱ
 �������: 
 �� �� ֵ:                 KEY1��ѡ���һ��ѡ��
                           KEY2��ѡ��ڶ���ѡ��
                           KEYCANCEL��ȡ��ѡ��
                           KEYENTER ������ѡ�񣨿ɼ�����һ����Ŀ�Ĵ���
ʹ��ʾ��:   key = OptionsSelect("���ѳ�����������", glb_file.pre_cm_rev_swipe == '1',
                            "״̬:ˢ��", "״̬:��ˢ��", "1.ˢ��  ", "2.��ˢ��", 0);

****************************************************************************/
uchar PubSelectOptions(uchar * pszTitle, uchar bCondition,  uchar* pszTruePrmpt,  uchar* pszFalsePrmpt, 
				 	                 uchar* pszOption1,     uchar* pszOption2,   ushort iTimeOut );

/****************************************************************************
��    ��:  Api_OneTwoSelect()
��    �ܣ� �ṩ2�ַ�ʽѡ��  ������'1''2',ȷ�ϻ���ȡ������
��������� pszItem           ��ʾ�ڵڶ��е��Ƿ�ѯ������
           pszChoice1        ѡ��"1"����ʾ����
           pszChoice2        ѡ��"2"����ʾ����
���������� NONE
����ֵ��   KEY1 KEY2, KEYENTER(��ѡ��) ���� KEYCANCEL(��ѡ��)
���ӣ�
           �ն˽���        //��һ�������ⲿ������ʾ
           �Ƿ��ӡ��ϸ����
           [1].��
           [2].��
****************************************************************************/
uchar PubSelectOneTwo(uchar *pszItem,uchar *pszChoice1,uchar *pszChoice2);

/****************************************************************************
��    ��:  Api_ZeroOneSelect()
��    �ܣ� �ṩ�Ƿ�ʽѡ��  ������'0''1',ȷ�ϻ���ȡ������
��������� 
         pszTitle  ��ʾ�ڵ�һ�еı�������
         pszItem   ��ʾ�ڵڶ��е��Ƿ�ѯ������
         pszChoice1        ѡ��"0"����ʾ����
         pszChoice2        ѡ��"1"����ʾ����
���������� NONE
����ֵ��  KEY0 KEY1, KEYENTER(��ѡ��) ���� KEYCANCEL(��ѡ��)
���ӣ�
         key = Api_ZeroOneSelect("��  ��","��ӡ������ϸ����","��ӡ","����ӡ");
         if(key == KEY0)
                           ......
           ��  ��        
         ��ӡ������ϸ����
         0.��ӡ 1.����ӡ
****************************************************************************/
uchar PubSelectZeroOne(uchar *pszTitle, uchar *pszItem, uchar *pszChoice1, uchar *pszChoice2);

/****************************************************************************
��    ��:   Api_YesNoSelect()
��    �ܣ�  �ṩ�Ƿ�ʽѡ��  ������ȷ�ϻ���ȡ������
���������  pszItem                ��ʾ�ڵڶ��е��Ƿ�ѯ������
            pszYesPrompt           "��"����ʾ����
            pszNoPrompt            "��"����ʾ����
����������  NONE
����ֵ��    KEYENTER "ȷ��" ���� KEYCANCEL "ȡ��"
���ӣ�      �ն˽���        //��һ�������ⲿ������ʾ
            �Ƿ��ӡ��ϸ����
             [ȷ��].��
             [ȡ��].��
****************************************************************************/
uchar PubSelectYesNo(uchar *pszItem,uchar *pszYesPrompt,uchar *pszNoPrompt);

void PubShowTitleASCII(uchar bClrScreen, uchar * pszTitle);

void PubDispStringASCII(void *pszStr, ushort nPosition);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _UTIL_H */
