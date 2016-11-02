/*****************************************************/
/*              API for P78                          */
/*****************************************************/
#ifndef  _POSAPI_P78_H_
#define  _POSAPI_P78_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _POS_P78		// only for p78 pos

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
void  kbsound(uchar mode,ushort dlytime);
uchar GetString(uchar *str,uchar mode,uchar minlen,uchar maxlen);
uchar GetHzString(uchar *outstr, uchar max, ushort TimeOut);
void  PEDInputTimeOut(ulong timeoutsec);
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


//================================================
//       Multiple apps management functions
//================================================
int   ReadAppInfo(uchar AppNo, APPINFO* ai);
uchar ReadAppState(uchar AppNo);
int   SetAppActive(uchar AppNo, uchar flag);
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
void InitFileSys(void);
int   truncate(int fid,long len);
int   fexist(char *filename);
int   GetFileInfo(FILE_INFO* finfo);
int   FileToApp(uchar *strFileName);
int   FileToParam(uchar *strSrcFileName, uchar *strAppName, int mode);

//================================================
//       Enviromental variable functions
//================================================
uchar GetEnv(char *name, uchar *value);
uchar PutEnv(char *name, uchar *value);

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
//	     PPP connection functions
//================================================
int   NpppLogin(char *user_name,char *user_password);
int   NpppOpen(uchar *remote_addr,ushort remote_port, ushort local_port,uchar mode);
int   NpppWrite(int connect_handle,uchar *buf,ushort len);
int   NpppRead(int connect_handle,uchar *out_data,ushort len);
void  NpppCheck(int connect_handle,PPP_NET_INFO *pi);
int   NpppClose(int connect_handle);
void  NpppProcess(void);

#endif  // ifdef _POS_P78

#ifdef __cplusplus
};
#endif

#endif	// #ifndef  _POSAPI_P78_H_

