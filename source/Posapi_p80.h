/*****************************************************/
/*              API for P80                          */
/*****************************************************/
#ifndef  _POSAPI_P80_H_
#define  _POSAPI_P80_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _POS_P80		// only for p80 pos

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

//=================================================
//			   RF card functions
//=================================================
uchar PiccOpen(void);
uchar PiccSetup (uchar mode, PICC_PARA *picc_para);	// mich_temp: protype different
uchar PiccDetect(uchar Mode,uchar *CardType,uchar *SerialInfo,uchar *CID,uchar *Other);
uchar PiccIsoCommand(uchar cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv);
uchar PiccRemove(uchar mode,uchar cid);
void PiccClose(void);
uchar M1Authority(uchar Type,uchar BlkNo,uchar *Pwd,uchar *SerialNo);
uchar M1ReadBlock(uchar BlkNo,uchar *BlkValue);
uchar M1WriteBlock(uchar BlkNo,uchar *BlkValue);
uchar M1Operate(uchar Type,uchar BlkNo,uchar *Value,uchar UpdateBlkNo);

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

//================================================
//                PCI PED functions
//================================================
void PEDInputTimeOut(ulong timeoutsec);
uchar PEDInput(uchar *str, uchar min, uchar max, uchar mode);
uchar PEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *cardno, uchar *pin,uchar TwiceInput);
uchar PEDGetPwd_3Des(uchar PinKeyID, uchar min, uchar max, uchar *cardno, uchar *bPinLen,uchar *pin);
uchar PEDWriteMKey(uchar KeyID, uchar mode, uchar *Key);
uchar PEDWriteDKey(uchar DESKeyID, uchar mode, uchar *Key);
uchar PEDWriteWKey(uchar MKeyID, uchar WKeyID, uchar mode, uchar *Key);
uchar PEDDeriveKey(uchar MKeyID, uchar WKeyID1, uchar WKeyID2, uchar mode);
uchar PEDDes(uchar DESKeyID, uchar mode, uchar *datain, uchar *dataout);
uchar PEDMac(uchar WKeyID, uchar mode, uchar *datain, ushort inLen, uchar *macout, uchar flag);
uchar EPSPEDAmount(uchar point_mode, uchar prompt_mode, uchar min, uchar max, uchar *Amt);
uchar EPSPEDLoadTMK(uchar WkeyID1, uchar *TmpKey, uchar *KTMK, uchar *ReferCode);
uchar EPSPEDLoadKey(uchar WKeyID1, uchar WKeyID2, uchar *KKEY);
uchar EPSPEDGetMAC1(uchar MKeyID, uchar *Random);
uchar EPSPEDUpdateTPK(uchar WKeyID1,uchar WKeyID2, uchar *pin_block, uchar *ISN, uchar *new_TPK);
uchar EPSPEDSetMAC2(uchar WKeyID1,uchar WKeyID2,uchar *MAC2, uchar *rsp_data, uchar rsp_data_len, uchar *rsp_MAC);
uchar EPSPEDVerifyTMK(uchar MKeyID, uchar *VerifyData);
uchar EPSPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *ISN, uchar *pin);
uchar PED_PassWordEncrypt(uchar deskeyid,uchar *oldpin,uchar *despin);
uchar PED_PassWordInput(uchar deskeyid,uchar min,uchar max,uchar *inpin);
uchar SYLPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *bPinLen,uchar *pin_block);
uchar SYLPEDVerifyDerive(uchar MKeyID, uchar WKeyID, uchar *new_WKey, uchar *KeyCv, uchar mode);
uchar SYLPEDCalcPinBlock(uchar MKeyID,uchar PinKeyID, uchar *pin_block,uchar *new_TPK,uchar mode);
uchar SYLPPLoadTMK(uchar MkeyID1, uchar *TmpKey, uchar *KTMK, uchar *ReferCode);
uchar PEDManage(uchar mode,uchar *pwdcode);

uchar PEDBeep(void);
uchar PEDLight(uchar OperMode, uchar Index);
uchar PEDInvoice(uchar OperMode);
uchar PEDKbmute(uchar OperMode);
uchar PEDBackLight(uchar OperMode);
uchar PEDScrCls(void);
uchar PEDScrClrLine(uchar line);
uchar PEDScrPrint(uchar line, uchar col, uchar *str);
uchar PEDScrWrData(uchar line, uchar col, uchar *Data, uchar DataLen);
uchar PEDVerInfo(uchar *ver);
uchar PEDUpdLogo(uchar line, uchar *Data);
uchar PEDClearKey(uchar keytype);
uchar EPSPEDDispMenu(uchar StartNo, uchar EndNo, uchar AllLines, uchar detail[][16], uchar *item);

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
int   FileToMonitor(uchar *strFileName);
int   ReadFontLib(ulong Offset,uchar *FontData, int ReadLen);

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
//	     Wireless lan functions
//================================================
uchar WNetInit(void);
uchar WNetCheckSignal(uchar *pSignalLevel);
uchar WNetCheckSim(void);
uchar WNetSimPin(uchar *pin);
uchar WNetUidPwd(uchar *Uid, uchar *Pwd);
uchar WNetDial(uchar *DialNum, uchar * reserved1, uchar * reserved2);
uchar WNetCheck(void);
uchar WNetTcpConnect(uchar *DestIP, uchar *DestPort);
uchar WNetTcpTxd(uchar *TxData, ushort txlen);
uchar WNetTcpRxd(uchar *RxData, ushort *prxlen, ushort ms);
uchar WNetTcpClose(void);
uchar WNetClose(void);
uchar WNetLinkCheck(void);
uchar WNetTcpCheck(void);
uchar WNetReset(void);
uchar WNetSendCmd(uchar *cmd, uchar *rsp, ushort rsplen, ushort ms);
uchar WNetMTcpConnect(int *psocket, uchar *DestIP, uchar *DestPort);
uchar WNetMTcpTxd(int socket, uchar *TxData,  ushort txlen);
uchar WNetMTcpRxd(int socket, uchar *RxData, ushort maxlen, ushort *prxlen, ushort ms);
uchar WNetMTcpClose(int socket);
uchar WNetMTcpCheck(int socket);

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
int   GsmCallModem(GSM_CALL *CallPara, uchar *TelNo, uchar *ErrMsg);
int   GsmCheck(void);
int   GsmOnHook(void);
int   NpppSetAuth(ulong authent);

#endif  // ifdef _POS_P80

#ifdef __cplusplus
};
#endif

#endif	// #ifndef  _POSAPI_P80_H_
