/*****************************************************/
/*              API for S80                          */
/*****************************************************/
#ifndef  _POSAPI_S80_H_
#define  _POSAPI_S80_H_

//PICC LEDÏÔÊ¾¿ØÖÆºê
#define PICC_LED_RED    0x01
#define PICC_LED_GREEN  0x02
#define PICC_LED_YELLOW 0x04
#define PICC_LED_BLUE   0x08

//================================================
//         FAT API
//================================================

/*
** ´íÎóÂë
**/
#define FS_OK                0/* no error, everything OK! */
#define FS_ERR_OTHERS		 -1 /*other erros*/
#define FS_ERR_NODEV         -5 /* Ëù²Ù×÷µÄÉè±¸²»´æÔÚ*/
#define FS_ERR_BUSY          -6 /* ½Úµã²»ÄÜÍ¬Ê±´ò¿ªÁ½´Î*/
#define FS_ERR_ARG           -7/* ´íÎó²ÎÊý*/
#define FS_ERR_EOF           -8 /* ÒÑµ½Î²²¿*/
#define FS_ERR_NAME_SIZE     -9/* Ãû×ÖÌ«³¤*/
#define FS_ERR_NAME_EXIST    -10/* Ãû×ÖÒÑ´æÔÚ*/
#define FS_ERR_CHAR          -11/* ´æÔÚ·Ç·¨×Ö·û*/
#define FS_ERR_NAME_SPACE    -12/* short name NUM is full*/
#define FS_ERR_DISK_FULL     -13/* DISK SPACE is full */
#define FS_ERR_ROOT_FULL     -14/* root space is full */
#define FS_ERR_CACHE_FULL    -15/* cache full, please submit */
#define FS_ERR_PATH          -16/* path depth */
#define FS_ERR_NOENT         -17/* no such inode */
#define FS_ERR_NOTDIR        -18/* not dir */
#define FS_ERR_ISDIR         -19/* is dir */
#define FS_ERR_NOMEM         -20/* NO MEMORY */
#define FS_ERR_BADFD         -21/* BAD file descriptor */
#define FS_ERR_NOTNULL       -22/* dir not null */
#define FS_ERR_WO            -23/* write only */
#define FS_ERR_RO            -24/* read only */
#define FS_ERR_SYS           -25/* sys not support */
#define FAT_ERROR_MAX        -60

/*
** Â·¾¶×î´ó³¤¶È
**/
#define PATH_NAME_MAX 2*1024

#define NAME_FMT_UNICODE 0x1/** UNICODE £¬Ä¿Ç°²»Ö§³Ö*/
#define NAME_FMT_ASCII 0x2 /** ASCII ±àÂë£¬ºÏ·¨×Ö·û0x20~0x7d*/

/** File System Width STRing */
typedef struct
{
	char* str;				/* Ö¸Ïò´æ´¢¿Õ¼äµÄÖ¸Õë */
	int  size;				/* ×Ö·û´®´æ´¢¿Õ¼ä */
	int  fmt;		/* ±àÂë¸ñÊ½£¬È¡ÖµÎªNAME_FMT_ASCII */
}FS_W_STR;

#define FS_ATTR_C 0x1 /* Create if not exist*/
#define FS_ATTR_D 0x2 /* inode is DIR */
#define FS_ATTR_E 0x4/* exclusive */
#define FS_ATTR_R 0x8 /* Read */
#define FS_ATTR_W 0x10 /* Write */
#define FS_ATTR_CRWD (FS_ATTR_C|FS_ATTR_R|FS_ATTR_W|FS_ATTR_D)
#define FS_ATTR_RW   (FS_ATTR_R|FS_ATTR_W)
#define FS_ATTR_RWD  (FS_ATTR_R|FS_ATTR_W|FS_ATTR_D)

#define LONG_NAME_MAX (200*2)
#define SHORT_NAME_MAX (11+1)

#define ATTR_RO 0x01  /* read only */
#define ATTR_HID 0x02 /* hidden */
#define ATTR_SYS 0x04 /* system file */
#define ATTR_VOL 0x08 /* volume ID */
#define ATTR_DIR 0x10 /* directory */
#define ATTR_ARC 0x20 /* archive */

typedef struct fs_date_time_s
{
	long seconds;
	long minute;
	long hour;
	long day;
	long month;
	long year;
}FS_DATE_TIME;

typedef struct
{
	FS_DATE_TIME    wrtime;/* lastly write time */
	FS_DATE_TIME    crtime;/* create time */
	int             size;/* file size */
	int             space;/* space size */
	int             name_fmt;
	int             attr;/* ÊôÐÔ*/
	int             name_size;/* nameµÄÓÐÐ§³¤¶È */
	char            name[LONG_NAME_MAX+4];
}FS_INODE_INFO;

typedef enum {
	FS_SEEK_SET=1,
	FS_SEEK_CUR,
	FS_SEEK_END,
}FS_SEEK_FLAG;


#define FS_VER_FAT16 0
#define FS_VER_FAT32 1

typedef struct
{
	int ver;/* °æ±¾ºÅ:FS_VER_FAT16 or FS_VER_FAT32 */
	int free_space;/* ¿ÕÏÐ¿Õ¼ä,µ¥Î»×Ö½Ú*/
	int used_space;/* ÒÑÓÃ¿Õ¼ä,µ¥Î»×Ö½Ú*/
}FS_DISK_INFO;

//================================================
//           Error code for (GPRS & CDMA) API
//================================================
#define WNET_RET_OK			0
//Ä£¿é¿Ú±»Õ¼ÓÃ
#define ERR_WNET_PORTINUSE 	-1
//Ä£¿éÃ»ÓÐÓ¦´ð
#define ERR_WNET_NORSP		-2
//Ä£¿éÓ¦´ð´íÎó
#define ERR_WNET_RSPERR		-3
//Ä£¿é´®¿ÚÃ»ÓÐ´ò¿ª
#define ERR_WNET_PORTNOOPEN -4
//ÐèÒªPINÂë
#define ERR_WNET_NEEDPIN	-5
//ÐèÒªPUK½âPINÂë
#define ERR_WNET_NEEDPUK	-6
//²ÎÊý´íÎó
#define ERR_WNET_PARAMER	-7
//ÃÜÂë´íÎó
#define ERR_WNET_ERRPIN		-8
//Ã»ÓÐ²åÈëSIM¿¨
#define	ERR_WNET_NOSIM      -9
//²»Ö§³ÖµÄÀàÐÍ
#define ERR_WNET_NOTYPE		-10
//ÍøÂçÃ»ÓÐ×¢²á
#define ERR_WNET_NOREG		-11
//Ä£¿éÔø³õÊ¼»¯
#define ERR_WNET_INIT_ONCE	-12
//Ã»ÓÐÁ¬½Ó
#define ERR_WNET_NOCONNECT  -13
//ÏßÂ·¶Ï¿ª
#define ERR_WNET_LINEOFF	-14
//Ã»ÓÐsocket¿ÉÓÃ
#define ERR_WNET_NOSOCKETUSE	-15
//³¬Ê±
#define ERR_WNET_TIMEOUT		-16
//ÕýÔÚ²¦ºÅÖÐ
#define ERR_WNET_CALLING 		-17
//ÖØ¸´µÄsocketÇëÇó
#define ERR_WNET_REPEAT_SOCKET	-18
//socket ÒÑ¾­¶Ï¿ª
#define ERR_WNET_SOCKET_DROP	-19
//socket ÕýÔÚÁ¬½ÓÖÐ
#define ERR_WNET_CONNECTING     -20
//socket ÕýÔÚ¹Ø±Õ
#define ERR_WNET_SOCKET_CLOSING	-21
//ÍøÂç×¢²áÖÐ
#define ERR_WNET_REGING			-22
//¹Ø±Õ´®¿Ú³ö´í
#define ERR_WNET_PORTCLOSE  	-23



//================================================
//         Net API  for (ETH,PPP)
//================================================

//Error Code
#define NET_OK   	 0      /* No error, everything OK. */
#define NET_ERR_MEM  -1      /* Out of memory error.     */
#define NET_ERR_BUF  -2      /* Buffer error.            */
#define NET_ERR_ABRT -3      /* Connection aborted.      */
#define NET_ERR_RST  -4      /* Connection reset.        */
#define NET_ERR_CLSD -5      /* Connection closed.       */
#define NET_ERR_CONN -6      /* Not connected.           */
#define NET_ERR_VAL  -7      /* Illegal value.           */
#define NET_ERR_ARG  -8      /* Illegal argument.        */
#define NET_ERR_RTE  -9      /* Routing problem.         */
#define NET_ERR_USE  -10     /* Address in use.          */
#define NET_ERR_IF   -11     /* Low-level netif error    */
#define NET_ERR_ISCONN -12   /* Already connected.       */
#define NET_ERR_TIMEOUT -13  /* time out */
#define NET_ERR_AGAIN  -14   /*no block, try again */
#define NET_ERR_EXIST  -15   /* exist already */
#define NET_ERR_SYS    -16   /* sys don not support */

/* PPP ERROR code */
#define NET_ERR_PASSWD -17 /* error password */
#define NET_ERR_MODEM  -18 /* modem dial is fail */
#define NET_ERR_LINKDOWN -19 /* ppp link disconnect */

#define NET_AF_INET 1

enum
{
	NET_SOCK_STREAM=1,	/* TCP */
	NET_SOCK_DGRAM		/* UDP */
};


typedef struct  net_in_addr
{
	unsigned long NET_s_addr;
}NET_IN_ADDR;

typedef struct net_sockaddr_in
{
	char  sin_len;
	char  sin_family;
	short sin_port;
	struct net_in_addr sin_addr;
	char sin_zero[8];
}NET_SOCKADDR_IN;

typedef struct net_sockaddr
{
	char sa_len;
	char sa_family;
	char sa_data[14];
}NET_SOCKADDR;

#ifndef socklen_t
#define socklen_t int
#endif


#define NOBLOCK 1
#define BLOCK   0/* ×èÈû±êÖ¾Î» */
#define SOCK_EVENT_READ    (1<<0)
#define SOCK_EVENT_WRITE   (1<<1)
#define SOCK_EVENT_CONN    (1<<2)
#define SOCK_EVENT_ACCEPT  (1<<3)
#define SOCK_EVENT_ERROR   (1<<4)
#define SOCK_EVENT_MASK    (0xff)

enum
{
	CMD_IO_SET=1,
	CMD_IO_GET,
	CMD_TO_SET,/* timeout set */
	CMD_TO_GET,/* timeout get */
	CMD_IF_SET,/* net_socket bind dev IF (interface)*/
	CMD_IF_GET,/* get the dev if net_socket bind */
	CMD_EVENT_GET,/* get event , such as SOCK_EVENT_READ,SOCK_EVENT_WRITE, etc*/
	CMD_BUF_GET,/* get send or recv buffer , only for SOCK_STREAM */
	CMD_FD_GET,/* get free net_socket fd num */
	/*2010-07-21 add*/
	CMD_KEEPALIVE_SET,/*ÅäÖÃsockµÄKeepAlive¹¦ÄÜ£¬¸ÃÃüÁîÖ»¶ÔNET_SOCK_STREAMÓÐÐ§*/
	CMD_KEEPALIVE_GET,/*»ñÈ¡sockµÄKeepAliveÇé¿ö£¬¸ÃÃüÁîÖ»¶ÔNET_SOCK_STREAMÓÐÐ§*/
};

/* ÍøÂçÉè±¸×´Ì¬±êÖ¾Î»*/
#define FLAGS_UP 0x1 /* link state OK */
#define FLAGS_TX 0x2 /* can tx */
#define FLAGS_RX 0x4 /* can rx */


/* arg for CMD_BUF_GET */
#define TCP_SND_BUF_MAX 1
#define TCP_RCV_BUF_MAX 2
#define TCP_SND_BUF_AVAIL 3

/* ÈÏÖ¤Ëã·¨¶¨Òåºê*/
#define PPP_ALG_PAP       0x1 /* PAP */
#define PPP_ALG_CHAP      0x2 /* CHAP */
#define PPP_ALG_MSCHAPV1  0x4 /* MS-CHAPV1 */
#define PPP_ALG_MSCHAPV2  0x8 /* MS-CHAPV2 */
#define PPP_ALG_ALL 	  0xff /*Ö§³ÖËùÓÐµÄÈÏÖ¤Ëã·¨*/


#define ETH_ROUTE  0
#define PPP_ROUTE  10



#define	PCI_OK				0x00	//ÕýÈ·
#define	PCI_ERROR			0x01	//ÆäËü´íÎó
#define	PCI_PEDAAKEY_ERR	0x03	//PED²»´æÔÚ±¾Ó¦ÓÃÓ¦ÓÃÃû
#define	PCI_KEYINDEX_ERR	0x05	//ÃÜÔ¿Ë÷ÒýºÅ·Ç·¨
#define	PCI_NO_KEY			0x06	//ÃÜÔ¿²»´æÔÚ
#define	PCI_KEYAUTH_ERR		0x07    //·Ç±¾Ó¦ÓÃÃÜÔ¿(ÆäËüÓ¦ÓÃ)
#define	PCI_KEYLEN_ERR		0x08	//ÃÜÔ¿³¤¶È´íÎó
#define	PCI_CANCEL			0x09	//ÓÃ»§È¡Ïû
#define	PCI_TIMEOUT			0x0a	//³¬Ê±ÍË³ö
#define	PCI_WAIT_INTERVAL	0x0b	//Á½´Îµ÷ÓÃ±¾º¯ÊýÊ±¼ä¼ä¸ô²»×ã
#define	PCI_ICC_NOCARD		0x0c	//¿¨²»ÔÚÎ»
#define	PCI_ICC_NOTINIT		0x0d	//¿¨Î´ÉÏµç
#define	PCI_COMM_ERR		0x0e	//Í¨Ñ¶´íÎó
#define	PCI_NEED_ADMIN		0x0f	//ÐèÒªÏÈÈ¡µÃ¸ß¼¶È¨ÏÞ²ÅÄÜ²Ù×÷
#define	PCI_LOCKED			0x10	//PEDÒÑ¾­Ëø¶¨£¬ÇëÏÈ½âËø
#define	PCI_NO_PIN			0x11	//PINÊäÈëÊ±ÓÃ»§Ö±½Ó°´È·ÈÏ£¨ÎÞÃÜÂë
#define	PCI_NO_UPK			0x12    //DUKPT OVER FLOW
#define	PCI_PED_NOMORE_BUF	0XFE
#define PCI_ADMIN_ERR		0x14;    //»ñÈ¡ÏµÍ³Ãô¸Ð·þÎñÊ§°Ü
#define	PCI_NOMORE_KEY		0XB0	//key¿Õ¼äÒÑÂú
#define	PCI_DUKPT_OVERFLOW	0xB1	//
#define	PCI_UNSPT_CMD 		0xff	//ÃüÁî²»Ö§³Ö

#define	TYPE_MASTERKEY	0x00
#define	TYPE_PINKEY		0X01
#define	TYPE_MACKEY		0x02
#define	TYPE_DUKPTKEY	0X03

#define	PED_ENCRYPT		0X01//
#define	PED_DECRYPT 	0X00//

typedef struct
{
     uint  modlen;          //PIN¼ÓÃÜ¹«Ô¿Ä£Êý³¤
     uchar mod[256];        //PIN¼ÓÃÜ¹«Ô¿Ä£Êý,²»×ãÎ»Ç°²¹0x00
     uchar exp[4];          //PIN¼ÓÃÜ¹«Ô¿Ö¸Êý,²»×ãÎ»Ç°²¹0x00
     uchar iccrandomlen;    //´Ó¿¨ÐÐÈ¡µÃµÄËæ»úÊý³¤
     uchar iccrandom[8];    //´Ó¿¨ÐÐÈ¡µÃµÄËæ»úÊý
}RSA_PINKEY;

// **********************************************
//		Font
// **********************************************


//×î¶à¿ÉÖ§³ÖµÄ×ÖÌåÊý
#define MAX_FONT_NUMS       32

//¿ÉÖ§³ÖµÄ×î´ó×ÖÌå¿í¶ÈºÍ¸ß¶È
#define MAX_FONT_WIDTH      32
#define MAX_FONT_HEIGHT     32

//×Ö·û¼¯¶¨Òå
#define CHARSET_WEST        0x01      //ÃÀ¹ú¡¢Ó¢¹ú¼°Î÷Å·¹ú¼Ò
#define CHARSET_TAI         0x02      //Ì©¹ú
#define CHARSET_MID_EUROPE  0x03      //ÖÐÅ·
#define CHARSET_VIETNAM     0x04      //Ô½ÄÏ
#define CHARSET_GREEK       0x05      //Ï£À°
#define CHARSET_BALTIC      0x06      //²¨ÂÞµÄº£
#define CHARSET_TURKEY      0x07      //ÍÁ¶úÆä
#define CHARSET_HEBREW      0x08      //Ï£²®À´
#define CHARSET_RUSSIAN     0x09      //¶íÂÞË¹
#define CHARSET_GB2312      0x0A      //¼òÌåÖÐÎÄ
#define CHARSET_GBK         0x0B      //¼òÌåÖÐÎÄ
#define CHARSET_GB18030     0x0C      //¼òÌåÖÐÎÄ
#define CHARSET_BIG5        0x0D      //·±ÌåÖÐÎÄ
#define CHARSET_SHIFT_JIS   0x0E      //ÈÕ±¾
#define CHARSET_KOREAN      0x0F      //º«¹ú
#define CHARSET_ARABIA      0x10      //°¢À­²®
#define CHARSET_DIY         0x11      //×Ô¶¨Òå×ÖÌå

//×ÖÌå½á¹¹¶¨Òå
typedef struct{
	int CharSet;    //×Ö·û¼¯
	int Width;      //×ÖÌå¿í¶È
	int Height;     //×ÖÌå¸ß¶È
	int Bold;       //£¨1£ººÚÌå£¬ 0£ºÕý³££©
	int Italic;     //£¨1£ºÐ±Ìå£¬ 0£ºÕý³££©
}ST_FONT;


// **********************************************
//		USB
// **********************************************

#define USBDEV_READ   0x1
#define USBDEV_WRITE  0x2
/*
** ´íÎóÂë
**/
#define USB_OK             0/* no error, everything OK! */
#define USB_ERR_NOCONF   -10/* Ã»ÓÐÅäÖÃ³É¹¦*/
#define USB_ERR_TIMEOUT  -11/* ³¬Ê±*/
#define USB_ERR_NOINIT   -12/* Ä£¿éÃ»ÓÐ³õÊ¼»¯³É¹¦*/
#define USB_ERR_NOSTART  -13/* Ã»ÓÐÆô¶¯Device¹¦ÄÜ*/
#define USB_ERR_START    -14/* ÒÑÆô¶¯Device¹¦ÄÜ*/
#define USB_ERR_PACKET   -15/* ÊÕµ½µÄ¶Ô·½±¨ÎÄ²»ÍêÕû*/
#define USB_ERR_DATA     -16/* ÊÕµ½µÄ¶Ô·½Êý¾Ý²»ÍêÕû*/
#define USB_ERR_DOING    -17/* */
#define USB_ERR_BUF      -18
#define USB_ERR_DISCONN  -19/* Ã»ÓÐÁ¬½Óµ½Host*/
#define USB_ERR_PARAM    -20/* bad param */



#if (defined _POS_S80)	|| (defined _POS_SP30)	// for S80 pos and SP30

#ifdef __cplusplus
extern "C" {
#endif

//=================================================
//               System basic functions
//=================================================
uchar SystemInit(void);
void  Beep(void);
void  Beef(uchar mode,ushort DlyTime);
uchar SetTime(uchar *time);
void  GetTime(uchar *time);
void  DelayMs(ushort Ms);
void  TimerSet(uchar TimerNo, ushort Cnts);
ushort TimerCheck(uchar TimerNo);
void  ReadSN(uchar *SerialNo);
void  EXReadSN(uchar *SN);
uchar ReadVerInfo(uchar *VerInfo);
int   GetTermInfo(uchar *out_info);
int   GetLastError(void);

//=================================================
//		    Encrypt/Decrypt functions
//=================================================
void des(uchar *input, uchar *output,uchar *deskey, int mode);
void Hash(uchar* DataIn, uint DataInLen, uchar* DataOut);
int  RSARecover(uchar *pbyModule,uint dwModuleLen,uchar *pbyExp,
			   uint dwExpLen,uchar *pbyDataIn,uchar *pbyDataOut);

//================================================
//                 Keyboard functions
//================================================
uchar kbhit(void);
void  kbflush(void);
uchar getkey(void);
void  kbmute(uchar flag);
uchar GetString(uchar *str,uchar mode,uchar minlen,uchar maxlen);
uchar GetHzString(uchar *outstr, uchar max, ushort TimeOut);

//==================================================
//					LCD functions
//==================================================
void  ScrCls(void);
void  ScrClrLine(uchar startline, uchar endline);
void  ScrGray(uchar mode);
void  ScrBackLight(uchar mode);
void  ScrGotoxy (uchar x,uchar y);
uchar ScrFontSet(uchar fontsize);
uchar ScrAttrSet(uchar Attr);
int   Lcdprintf(const char *fmt,...);
void  ScrPrint(uchar col,uchar row,uchar mode, char *str,...);
void  ScrPlot(uchar X, uchar Y, uchar Color);
void  ScrDrLogo(uchar *logo);
void  ScrDrLogoxy(int x,int y,uchar *logo);
uchar ScrRestore(uchar mode);
void  ScrSetIcon(uchar icon_no,uchar mode);

//=================================================
//			   Magcard reader functions
//=================================================
void  MagOpen(void);
void  MagClose(void);
void  MagReset(void);
uchar MagSwiped(void);
uchar MagRead(uchar *Track1, uchar *Track2, uchar *Track3);

//=================================================
//			   Smart card reader functions
//=================================================
uchar IccInit(uchar slot,uchar *ATR);
void  IccClose(uchar slot);
void  IccAutoResp(uchar slot, uchar autoresp);
uchar IccIsoCommand(uchar slot, APDU_SEND *ApduSend, APDU_RESP *ApduRecv);
uchar IccDetect(uchar slot);

//=================================================
//			   RF card functions
//=================================================
uchar PiccOpen(void);
//uchar PiccInfoSetup (uchar mode, PICC_PARA_INFO *picc_para);	// mich_temp: protype different
uchar PiccSetup (uchar mode, PICC_PARA *picc_para);
uchar PiccDetect(uchar Mode,uchar *CardType,uchar *SerialInfo,uchar *CID,uchar *Other);
uchar PiccIsoCommand(uchar cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv);
uchar PiccRemove(uchar mode,uchar cid);
void PiccClose(void);
uchar M1Authority(uchar Type,uchar BlkNo,uchar *Pwd,uchar *SerialNo);
uchar M1ReadBlock(uchar BlkNo,uchar *BlkValue);
uchar M1WriteBlock(uchar BlkNo,uchar *BlkValue);
uchar M1Operate(uchar Type,uchar BlkNo,uchar *Value,uchar UpdateBlkNo);
void PiccLEDCtrl(uchar Mode);

//================================================
//                Printer functions
//================================================
uchar PrnInit(void);
void  PrnFontSet(uchar Ascii, uchar CFont);
void  PrnSpaceSet(uchar x, uchar y);
void  PrnStep(short pixel);
uchar PrnStr(char *str,...);
uchar PrnLogo(uchar *logo);					// mich_temp: protype different
uchar PrnStart(void);
uchar PrnStatus(void);
void  PrnLeftIndent(ushort Indent);
int PrnGetDotLine(void);
void PrnSetGray(int Level);
int  PrnGetFontDot(int FontSize, uchar *str, uchar *OutDot);
// new api for S-serials pos
int  PrnSelectFont(ST_FONT *psingle_code_font, ST_FONT *pmulti_code_font);
void PrnDoubleWidth(int AscDoubleWidth, int LocalDoubleWidth);
void PrnDoubleHeight(int AscDoubleHeight, int LocalDoubleHeight);
void PrnAttrSet(int reverse);

//================================================
//                PCI PED functions
//================================================
uchar PciGetRandom(uchar *random);
uchar PciWritePinKey(ushort  MKeyID, ushort PinKeyID, uchar PinKeyLen, uchar *PinKey);
uchar PciWriteMacKey(ushort  MKeyID, ushort MacKeyID, uchar MacKeyLen, uchar *MacKey);
uchar PciGetPin(ushort PinKeyID, uchar *ExpectPinLen, uchar *CardNo, uchar Mode, uchar *PinBlock);
uchar PciGetMac(ushort MacKeyID, uchar *DataIn, ushort InLen, uchar Mode, uchar *MacOut);
uchar PciGetPinDukpt(uchar *ExpectPinLen, uchar *CardNo, uchar Mode, uchar *PinBlock, uchar *OutKsn);
uchar PciGetMacDukpt(uchar *DataIn, ushort InLen, uchar Mode, uchar *MacOut, uchar *OutKsn);
uchar PciGetVerifyPlainPinOffLine(uchar IccSlot, uchar *ExpectPinLen, uchar *IccCmd, uchar *IccResp);
uchar PciGetVerifyCipherPinOffLine(uchar IccSlot, uchar *ExpectPinLen, RSA_PINKEY *RsaPinKey, uchar *IccCmd, uchar *IccResp);

uchar PciSetTimeOut(ulong ulTimeOutMs,ulong ulGetPinTimeMs,ulong ulGetMacTimeMs);
uchar PciDerivePinKey(uchar MSrcKeyType,ushort MSrcKeyID, ushort  SrcPinKeyID, ushort DstPinKeyID,uchar ucMode);
uchar PciDeriveMacKey(uchar MSrcKeyType,ushort MSrcKeyID, ushort  SrcMacKeyID, ushort DstMacKeyID,uchar ucMode);
uchar PciWriteMasterKey(ushort MKeyID, uchar MKeyLen, uchar *MKey);
uchar PciWriteDukptInitKey(uchar InitKeyLen, uchar *InitKey, uchar* InKsn);
uchar PciWriteDesKey(ushort DKeyID, uchar DKeyLen, uchar *DKey);
uchar PciDesKeyTdes(ushort DKeyID,uchar Mode,uchar* DataIn,uchar* DataOut);
//================================================
//       Multiple apps management functions
//================================================
int   ReadAppInfo(uchar AppNo, APPINFO* ai);
int   RunApp(uchar AppNo);
int   DoEvent(uchar AppNo, ST_EVENT_MSG *msg);

//================================================
//       File functions
//================================================
int   open(char *filename, uchar mode);
int   ex_open(char *filename, uchar mode,uchar* attr);
int   read(int fid, uchar *dat, int len);
int   write(int fid, uchar *dat, int len);
int   close(int fid);
int   seek(int fid, long offset, uchar fromwhere);
long  filesize(char *filename);
int   remove(const char *filename);
long  freesize(void);
int   truncate(int fid,long len);
int   fexist(char *filename);
int   GetFileInfo(FILE_INFO* finfo);

//================================================
//       Enviromental variable functions
//================================================
uchar GetEnv(char *name, uchar *value);
uchar PutEnv(char *name, uchar *value);

//================================================
//       FAT file system functions
//================================================
int   FsOpen(FS_W_STR *name, int attr);
int   FsClose(int fd);
int   FsDelete(FS_W_STR *name_in);
int   FsGetInfo(int fd, FS_INODE_INFO *fs_inode);
int   FsRename(FS_W_STR * name, FS_W_STR *new_name);
int   FsDirRead(int fd, FS_INODE_INFO *fs_inode, int num);
int   FsDirSeek(int fd, int num, int flag);
int   FsFileRead(int fd, char *buf, int len);
int   FsFileWrite(int fd, char *buf, int len);
int   FsFileSeek(int fd, int offset, int flag);
int   FsFileTell(int fd);
int   FsFileTruncate(int fd, int size, int reserve_space);
int   FsSetWorkDir(FS_W_STR *name);
int   FsGetWorkDir(FS_W_STR *name);
int   FsUdiskIn(void);
int   FsGetDiskInfo(int disk_num, FS_DISK_INFO *disk_info);

//================================================
//     Asynchronism communication functions
//================================================
uchar PortOpen(uchar channel, uchar *para);
uchar PortClose(uchar channel);
uchar PortSend(uchar channel, uchar ch);
uchar PortRecv(uchar channel, uchar *ch, ushort ms);
uchar PortReset(uchar channel);
uchar PortSends(uchar channel,uchar *str,ushort str_len);
uchar PortTxPoolCheck(uchar channel);

//================================================
//		Modem communication functions
//================================================
uchar ModemReset(void);
uchar ModemDial(COMM_PARA *MPara, uchar *TelNo, uchar mode);
uchar ModemCheck(void);
uchar ModemTxd(uchar *SendData, ushort len);
uchar ModemRxd(uchar *RecvData, ushort *len);
uchar ModemAsyncGet(uchar *ch);
uchar OnHook(void);
ushort ModemExCommand(uchar *CmdStr, uchar *RespData,
					  ushort *Dlen,ushort Timeout10ms);

//================================================
//	     Wireless network functions
//================================================
int   ExWNetInit(const uchar *Pin);
int   ExWNetOpen();
int   ExWNetCheckSignal(uchar *SignalLevel);
int   ExWNetPPPDial(const uchar *DialNum,const uchar *Uid,
					const uchar *Pwd,void *Reserved);
int   ExWNetPPPClose();
int   ExWNetTcpConnect(int * Socket,const uchar * DestIP,ushort DestPort);
int   ExWNetTcpCheck(int Socket);
int   ExWNetTcpClose(int Socket);
int   ExWNetTcpTxd(int Socket,const uchar * TxData,ushort TxLen);
int   ExWNetTcpRxd(int Socket,uchar * RxData,ushort ExpLen,
				   ushort * RxLen,ushort ms);
int   ExWNetClose();
int   ExWNetPPPCheck();

//================================================
//	     TCP/IP functions
//================================================
int NetSocket(int domain, int type, int protocol);
int NetBind(int socket, struct net_sockaddr *addr, socklen_t addrlen);
int NetConnect(int socket, struct net_sockaddr *addr, socklen_t addrlen);
int NetListen(int socket, int backlog);
int NetAccept(int socket, struct net_sockaddr *addr, socklen_t *addrlen);
int NetSend(int socket, void *buf, int size, int flags);
int NetSendto(int socket, void *buf, int size, int flags,
		struct net_sockaddr *addr, socklen_t addrlen);
int NetRecv(int socket, void *buf, int size, int flags);
int NetRecvfrom(int socket, void *buf, int size, int flags,
		struct net_sockaddr *addr, socklen_t *addrlen);
int NetCloseSocket(int socket);
int Netioctl(int socket, int cmd, int arg);
int SockAddrSet(struct net_sockaddr *addr, char *ip_str, short port);
int SockAddrGet(struct net_sockaddr *addr, char *ip_str, short *port);
int PPPLogin(char *name, char *passwd, long auth , int timeout);
void PPPLogout(void);
int PPPCheck(void);
int EthSet(
		char *host_ip, char *host_mask,
		char *gw_ip,
		char *dns_ip
		);
int EthGet(
		char *host_ip, char *host_mask,
		char *gw_ip,
		char *dns_ip,
		long *state
		);
int DnsResolve(char *name, char *result, int len);
int NetPing(char *dst_ip_str, long timeout, int size);
int RouteSetDefault(int ifIndex);
int RouteGetDefault(void);
int DhcpStart(void);
void DhcpStop(void);
int DhcpCheck(void);


//================================================
//	     USB functions
//================================================
int UsbDevStart(void);
int UsbDevStop(void);
int UsbDevCheck(void);
int UsbDevSend(char *buf, int size, int timeout_ms);
int UsbDevRecv(char *buf, int size, int timeout_ms);

//================================================
//	     Ò»Î¬ÌõÂë functions
//================================================
// Ò»Î¬ÌõÂëÍ¨ÓÃ·µ»ØÂë
#define SCAN1D_ERROR_OK              0     // ²Ù×÷³É¹¦
#define SCAN1D_ERROR_UNKNOWN        -1     // Î´Öª´íÎó
#define SCAN1D_ERROR_OPEN           -2     // ´®¿Ú´ò¿ªÊ§°Ü
#define SCAN1D_ERROR_NOOPEN         -3     // Ã»´ò¿ª
#define SCAN1D_ERROR_NODECODE       -4     // ÎÞÉ¨ÃèÄ£¿é
#define SCAN1D_ERROR_CMDNAK         -5     // Ó¦´ð´íÎó
#define SCAN1D_ERROR_PARAM          -6     // ²ÎÊý´íÎó
#define SCAN1D_ERROR_TIMEOUT        -7     // ³¬Ê±
#define SCAN1D_ERROR_PACKET         -8     // ½ÓÊÕµÄ°ü·Ç·¨£¬Ð£Ñé²»¹ý
#define SCAN1D_ERROR_DENY           -9     // ²»ÔÊÐíµÄ²Ù×÷£¬¿ÉÄÜÊÇ²ÎÊýÉèÖÃ´íÎó
#define SCAN1D_ERROR_PORT           -10    // ´®¿Ú¶ÁÐ´³ö´í
#define SCAN1D_ERROR_HWUNKNOWN      -11    // ²»ÄÜÊ¶±ðµÄÄ£¿é

/* ÌõÂëÀàÐÍ
Not Applicable       .. 0x00  |  Code 128            .. 0x03
Bookland EAN         .. 0x16  |  Coupon Code         .. 0x17
Chinese 2 of 5       .. 0x72  |  Databar Coupon Code .. 0xb4
Codabar              .. 0x02  |  Discrete 2 of 5     .. 0x04
Code 11              .. 0x0C  |  EAN 8               .. 0x0A
Code 39              .. 0x01  |  EAN 8 with 2 Supps  .. 0x4a
Code 39 Full ASCII   .. 0x13  |  EAN 8 with 5 Supps  .. 0x8a
Code 93              .. 0x07  |  EAN 13              .. 0x0B

EAN 13 with 2 Supps  .. 0x4B  |  Trioptic Code 39    .. 0x15
EAN 13 with 5 Supps  .. 0x8B  |  UPC A               .. 0x08
GS1-128              .. 0x0F  |  UPC A with 2 Supps  .. 0x48
GS1 DataBar Limited  .. 0x31  |  UPC A with 5 Supps  .. 0x88
GS1 DataBar-14       .. 0x30  |  UPC E0              .. 0x09
GS1 DataBar Expanded .. 0x32  |  UPC E0 with 2 Supps .. 0x49
IATA 2 of 5          .. 0x05  |  UPC E0 with 5 Supps .. 0x89
ISBT 128             .. 0x19  |  UPC E1              .. 0x10
Matrix 2 of 5        .. 0x39  |  UPC E1 with 2 Supps .. 0x50
MSI                  .. 0x0E  |  UPC E1 with 5 Supps .. 0x90
*/

/***************************
       API ½Ó¿ÚËµÃ÷
***************************/

// ¹¦ÄÜ: ´ò¿ªÒ»Î¬ÌõÂëÄ£¿é
int Scan1dOpen(void);

// ¹¦ÄÜ: ¹Ø±ÕÒ»Î¬ÌõÂëÄ£¿é
void Scan1dClose(void);

// ¹¦ÄÜ: ¿ªÆôÒ»Î¬ÌõÂë£¬²¢¿ªÊ¼É¨Ãé
int Scan1dStartDecode(void);

/*
¹¦ÄÜ: »ñÈ¡É¨ÃèÊý¾Ý
²ÎÊý:
BarType: ÌõÂëÀàÐÍ, 1×Ö½Ú
BarData: ÌõÂëÊý¾Ý, 256×Ö½Ú
TimeOutMs: ½ÓÊÕ³¬Ê±
*/
int Scan1dGetDecodeData(uchar *BarType, uchar *BarData, ushort TimeOutMs);

/*
¹¦ÄÜ: »ñÈ¡Ä£¿é°æ±¾ÐÅÏ¢
²ÎÊý:
Version:  Êä³öµÄ°æ±¾×Ö·û´®, 256×Ö½Ú
*/
int Scan1dReadModuleVer(uchar *Version);

/*
µäÐÍÓÃ·¨¾ÙÀý:
void TestScan1d(void)
{
    int r;
    uchar type,buff[256];

    r = Scan1dOpen();
    if (r != 0){
       ScrPrint(0, 4, 1, "Scan1dOpen()=%d.", r);
       getkey();
    }

    for (;;){
        ScrCls();
        ScrPrint(0, 0, 1, "Scanning...");
        r = Scan1dStartDecode();
        if (r != 0){
           ScrPrint(0, 4, 1, "Scan1dStartDecode()=%d.", r);
           if (getkey()==KEYCANCEL) break;
        }

        r = Scan1dGetDecodeData(&type, buff, 3000);

        ScrCls();
        if (r != 0){
           ScrPrint(0, 4, 1, "Scan1dGetDecodeData()=%d.", r);
           if (getkey()==KEYCANCEL) break;
        }
        else{
           ScrPrint(0, 0, 1, "OK bar:");
           ScrPrint(0, 2, 1, "Type: %02X.", type);
           ScrPrint(0, 4, 1, "%s", buff);
           if (getkey()==KEYCANCEL) break;
        }

    }

    Scan1dClose();
}
*/

#ifdef __cplusplus
};
#endif

#endif  // ifdef _POS_S80


#endif	// #ifndef  _POSAPI_S80_H_


