/*****************************************************/
/*              API for S80                          */
/*****************************************************/
#ifndef  _POSAPI_S78_H_
#define  _POSAPI_S78_H_


#ifdef _POS_S78		// for S78 pos

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
void  ScrDrLogoxy(char x,char y,uchar *logo);
uchar ScrRestore(uchar mode);
void  ScrSetIcon(uchar icon_no,uchar mode);

//typedef int COLORREF;
//#define RGB(r,g,b)  ((COLORREF)(((BYTE)(r&0xFC)|((WORD)((BYTE)(g&0xFC))<<8))|(( (BYTE)(b&0xFC))<<16)))
int ScrSetFgColor (/*COLORREF*/int fgColor);
int ScrSetBgColor (/*COLORREF*/int bgColor);
void ScrDrawPixel(int X, int Y, /*COLORREF*/int Color);
int ScrDrawBitmap(unsigned char * pBitmap);

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


#ifdef __cplusplus
};
#endif

#endif  // ifdef _POS_S78


#endif	// #ifndef  _POSAPI_S78_H_

