/*****************************************************/
/*            all APIs for all pos type              */
/*****************************************************/
#ifndef  _POSAPI_ALL_H_
#define  _POSAPI_ALL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define POS_API __declspec(dllexport)
#else
#define POS_API
#endif

//#ifdef WIN32		// used for propay. propay supports all apis

//=================================================
//               System basic functions
//=================================================
void POS_API Reboot();
uchar POS_API SystemInit(void);
void  POS_API Beef(uchar mode,ushort DlyTime);
uchar POS_API SetTime(uchar *time);
void  POS_API GetTime(uchar *time);
void  POS_API DelayMs(ushort Ms);
void  POS_API TimerSet(uchar TimerNo, ushort Cnts);
unsigned int POS_API GetTimerCount(void);
ushort POS_API TimerCheck(uchar TimerNo);
void  POS_API ReadSN(uchar *SerialNo);
void  POS_API EXReadSN(uchar *SN);
uchar POS_API ReadVerInfo(uchar *VerInfo);
int   POS_API GetTermInfo(uchar *out_info);
#ifndef _PROPAY_
int   POS_API GetLastError(void);		// donot declare it, its name conflicts with system function.
void  POS_API Beep(void);		// donot declare it, its name conflicts with system function.
#endif
void POS_API BeepF(uchar mode,ushort DlyTime);
int POS_API EnumBaseFont(ST_FONT *Fonts, int MaxFontNums);
uchar POS_API HangOff(void);
int POS_API GetTermInfoExt(uchar *InfoOut, int MaxBufLen);
int POS_API SysConfig(uchar *ConfigInfoIn, int InfoInLen);
uchar POS_API ReadDllVer(void);
int POS_API PrnTemperature();
int POS_API	GetVolt(void);
void POS_API SysIdle(void);

//------------------------------------
// nicm add for s90 2008-08-04 begin
//------------------------------------
int POS_API SysSleep(uchar *down_ctrl);

//------------------------------------
// nicm add for s90 2008-08-04 end
//------------------------------------

//------------------------------------
// nicm add for s90 2009-03-04 begin
//------------------------------------
void POS_API PowerOff(void);

//------------------------------------
// nicm add for s90 2009-03-04 end
//------------------------------------

//lijq add at 20091104
uchar POS_API BatteryCheck(void);



//------------------------------------
// nicm add for s60 2008-10-27 begin
//------------------------------------
uchar POS_API OnBase(void);
unsigned int POS_API PowerSave(unsigned int Event, unsigned int OverTime);
void POS_API AutoPowerSave(unsigned char mode);


#define LCD_DEVICE 0x01
#define LCD_CACHE 0x02
void POS_API ScrSetOutput(int device);

#define ECHO_CLOSE 0x00
#define ECHO_OPEN  0x01
#define ECHO_LOCK  0x02
void POS_API ScrSetEcho(int mode);

//------------------------------------
// nicm add for s60 2008-10-27 end
//------------------------------------

//for T52 2012-07-04
int POS_API GetHardwareConfig(const char *Keyword, char *ContextOut, int ContextOutSize);



//=================================================
//		    Encrypt/Decrypt functions
//=================================================
void POS_API des(uchar *input, uchar *output,uchar *deskey, int mode);
void POS_API Hash(uchar* DataIn, uint DataInLen, uchar* DataOut);
int  POS_API RSARecover(uchar *pbyModule,uint dwModuleLen,uchar *pbyExp,
			   uint dwExpLen,uchar *pbyDataIn,uchar *pbyDataOut);
int POS_API RSAKeyPairGen(R_RSA_PUBLIC_KEY *pPublicKeyOut,
                  R_RSA_PRIVATE_KEY *pPrivateKeyOut,
                  int iProtoKeyBit,
                  int iPubEType);
int POS_API RSAKeyPairVerify(R_RSA_PUBLIC_KEY PublicKey,
					 R_RSA_PRIVATE_KEY PrivateKey);


//================================================
//                 Keyboard functions
//================================================
uchar POS_API kbhit(void);
void  POS_API kbflush(void);
uchar POS_API getkey(void);
uchar POS_API _getkey(void);
void  POS_API kbmute(uchar flag);
void  POS_API kbsound(uchar mode,ushort dlytime);
uchar POS_API GetString(uchar *str,uchar mode,uchar minlen,uchar maxlen);
uchar POS_API GetHzString(uchar *outstr, uchar max, ushort TimeOut);
void POS_API kblight(unsigned char mode);
void POS_API SetLightTime(unsigned short LightTime);
//for D-Serial (D200/D210/D300)
void POS_API  KbLock(uchar mode);
int  POS_API   KbCheck(int cmd);

//==================================================
//					LCD functions
//==================================================
void  POS_API ScrCls(void);
void  POS_API ScrClrLine(uchar startline, uchar endline);
void  POS_API ScrGray(uchar mode);
void  POS_API ScrBackLight(uchar mode);
void  POS_API ScrGotoxy (uchar x,uchar y);
uchar POS_API ScrFontSet(uchar fontsize);
uchar POS_API ScrAttrSet(uchar Attr);
int   POS_API Lcdprintf(const char *fmt,...);
void  POS_API ScrPrint(uchar col,uchar row,uchar mode, char *str,...);
void  POS_API ScrPlot(uchar X, uchar Y, uchar Color);
void  POS_API ScrDrLogo(uchar *logo);
#if (defined(_POS_P78) || defined(_POS_P80) || defined(_POS_P90))
void  POS_API ScrDrLogoxy(char x,char y,uchar *logo);			//PXX
#else
void  POS_API ScrDrLogoxy(int x,int y,uchar *logo);				//SXX
#endif
uchar POS_API ScrRestore(uchar mode);
void  POS_API ScrSetIcon(uchar icon_no,uchar mode);
void POS_API ScrDrawBox(uchar y1,uchar x1,uchar y2,uchar x2);	// p90

// S serials POS.
void POS_API ScrGotoxyEx (int x, int y);
void POS_API ScrGetxyEx(int *px, int *py);
void POS_API ScrDrawRect(int left, int top, int right, int bottom);
void POS_API ScrClrRect(int left,int top, int right,int bottom);
void POS_API ScrSpaceSet(int char_extra, int line_extra);
void POS_API ScrGetLcdSize(int *pwidth, int *pheight);
void POS_API ScrTextOut(int x, int y, unsigned char *txt);
int POS_API EnumFont (ST_FONT *pfonts, int max_font_nums);
int  POS_API ScrSelectFont(ST_FONT *psingle_code_font, ST_FONT *pmulti_code_font);

//typedef int COLORREF;
//#define RGB(r,g,b)  ((COLORREF)(((BYTE)(r&0xFC)|((WORD)((BYTE)(g&0xFC))<<8))|(( (BYTE)(b&0xFC))<<16)))
//for s78
int POS_API ScrSetFgColor (/*COLORREF*/int fgColor);
int POS_API ScrSetBgColor (/*COLORREF*/int bgColor);
void POS_API ScrDrawPixel(int X, int Y, /*COLORREF*/int Color);
int POS_API ScrDrawBitmap(unsigned char * pBitmap);
void POS_API ScrRect(int X1, int Y1, int X2, int Y2, COLORREF BorderColor, COLORREF FillColor);

//for T80 monitor
void POS_API LedDisplay(unsigned char type, unsigned char* str);

//=================================================
//			   Magcard reader functions
//=================================================
void  POS_API MagOpen(void);
void  POS_API MagClose(void);
void  POS_API MagReset(void);
uchar POS_API MagSwiped(void);
uchar POS_API MagRead(uchar *Track1, uchar *Track2, uchar *Track3);
uchar POS_API MagGetMagnePrint(uchar *pucMagnePrint);

//=================================================
//			   Smart card reader functions
//=================================================
uchar POS_API IccInit(uchar slot,uchar *ATR);
void  POS_API IccClose(uchar slot);
void  POS_API IccAutoResp(uchar slot, uchar autoresp);
uchar POS_API IccIsoCommand(uchar slot, APDU_SEND *ApduSend, APDU_RESP *ApduRecv);
uchar POS_API IccDetect(uchar slot);
//Added by xiejt on 20100628
void POS_API Mc_Clk_Enable(uchar slot, uchar mode);
void  POS_API Mc_Io_Dir(uchar slot,uchar mode);
uchar POS_API Mc_Io_Read(uchar slot);
void POS_API Mc_Io_Write(uchar slot, uchar mode);
void  POS_API Mc_Vcc(uchar slot,uchar mode);
void  POS_API Mc_Reset(uchar slot,uchar mode);
void  POS_API Mc_Clk(uchar slot,uchar mode);
void  POS_API Mc_C4_Write(uchar slot,uchar mode);
void  POS_API Mc_C8_Write(uchar slot,uchar mode);
uchar POS_API Mc_C4_Read(uchar slot);
uchar POS_API Mc_C8_Read(uchar slot);
void  POS_API Mc_Cs(uchar mode);
void  POS_API Mc_RdyMod(uchar mode);
uchar POS_API Read_CardSlotInfo(uchar slot);
void  POS_API Write_CardSlotInfo(uchar slot,uchar slotopen);

//=================================================
//			   RF card functions
//=================================================
uchar POS_API PiccOpen(void);
uchar POS_API PiccSetup(uchar mode, PICC_PARA *picc_para);	// mich_temp: protype different
uchar POS_API PiccDetect(uchar Mode,uchar *CardType,uchar *SerialInfo,uchar *CID,uchar *Other);
uchar POS_API PiccIsoCommand(uchar cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv);
uchar POS_API PiccRemove(uchar mode,uchar cid);
void POS_API PiccClose(void);
uchar POS_API M1Authority(uchar Type,uchar BlkNo,uchar *Pwd,uchar *SerialNo);
uchar POS_API M1ReadBlock(uchar BlkNo,uchar *BlkValue);
uchar POS_API M1WriteBlock(uchar BlkNo,uchar *BlkValue);
uchar POS_API M1Operate(uchar Type,uchar BlkNo,uchar *Value,uchar UpdateBlkNo);
uchar POS_API PiccInitFelica(uchar ucRate, uchar ucPol);
uchar POS_API PiccManageReg(uchar ucMode, uchar ucRegAddr, uchar *pucOutData);



//-----------------------------------------------------------------
// nicm add for R50 ÉäÆµ¿¨Ä£¿éÔö¼Ó Desfire ¹¦ÄÜÖ§³Ö 2008-09-23 begin
//-----------------------------------------------------------------

// DesFire7816Card
uchar POS_API PiccCmdExchange(uint uiSendLen, uchar* paucInData, uint* puiRecLen, uchar* paucOutData);

//-----------------------------------------------------------------
// nicm add for R50 ÉäÆµ¿¨Ä£¿éÔö¼Ó Desfire ¹¦ÄÜÖ§³Ö 2008-09-23 end
//-----------------------------------------------------------------


//================================================
//                Printer functions
//================================================
uchar POS_API PrnInit(void);
void  POS_API PrnFontSet(uchar Ascii, uchar CFont);
void  POS_API PrnSpaceSet(uchar x, uchar y);
void  POS_API PrnStep(short pixel);
uchar POS_API PrnStr(char *str,...);
uchar POS_API PrnLogo(uchar *logo);					// mich_temp: protype different
uchar POS_API PrnStart(void);
uchar POS_API PrnStatus(void);
void  POS_API PrnLeftIndent(ushort Indent);
int POS_API PrnGetDotLine(void);
void POS_API PrnSetGray(int Level);
int  POS_API PrnGetFontDot(int FontSize, uchar *str, uchar *OutDot);
// new api for S-serials pos
int  POS_API PrnSelectFont(ST_FONT *psingle_code_font, ST_FONT *pmulti_code_font);
void POS_API PrnDoubleWidth(int AscDoubleWidth, int LocalDoubleWidth);
void POS_API PrnDoubleHeight(int AscDoubleHeight, int LocalDoubleHeight);
void POS_API PrnAttrSet(int reverse);
void POS_API SetSlipFW(unsigned char mode);

//================================================
//                PCI PED functions
//================================================
void POS_API PEDInputTimeOut(ulong timeoutsec);
uchar POS_API PEDInput(uchar *str, uchar min, uchar max, uchar mode);
uchar POS_API PEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *cardno, uchar *pin,uchar TwiceInput);
uchar POS_API PEDGetPwd_3Des(uchar PinKeyID, uchar min, uchar max, uchar *cardno, uchar *bPinLen,uchar *pin);
uchar POS_API PEDWriteMKey(uchar KeyID, uchar mode, uchar *Key);
uchar POS_API PEDWriteDKey(uchar DESKeyID, uchar mode, uchar *Key);
uchar POS_API PEDWriteWKey(uchar MKeyID, uchar WKeyID, uchar mode, uchar *Key);
uchar POS_API PEDDeriveKey(uchar MKeyID, uchar WKeyID1, uchar WKeyID2, uchar mode);
uchar POS_API PEDDes(uchar DESKeyID, uchar mode, uchar *datain, uchar *dataout);
uchar POS_API PEDMac(uchar WKeyID, uchar mode, uchar *datain, ushort inLen, uchar *macout, uchar flag);
uchar POS_API EPSPEDAmount(uchar point_mode, uchar prompt_mode, uchar min, uchar max, uchar *Amt);
uchar POS_API EPSPEDLoadTMK(uchar WkeyID1, uchar *TmpKey, uchar *KTMK, uchar *ReferCode);
uchar POS_API EPSPEDLoadKey(uchar WKeyID1, uchar WKeyID2, uchar *KKEY);
uchar POS_API EPSPEDGetMAC1(uchar MKeyID, uchar *Random);
uchar POS_API EPSPEDUpdateTPK(uchar WKeyID1,uchar WKeyID2, uchar *pin_block, uchar *ISN, uchar *new_TPK);
uchar POS_API EPSPEDSetMAC2(uchar WKeyID1,uchar WKeyID2,uchar *MAC2, uchar *rsp_data, uchar rsp_data_len, uchar *rsp_MAC);
uchar POS_API EPSPEDVerifyTMK(uchar MKeyID, uchar *VerifyData);
uchar POS_API EPSPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *ISN, uchar *pin);
uchar POS_API PED_PassWordEncrypt(uchar deskeyid,uchar *oldpin,uchar *despin);
uchar POS_API PED_PassWordInput(uchar deskeyid,uchar min,uchar max,uchar *inpin);
uchar POS_API SYLPEDGetPwd(uchar PinKeyID, uchar min, uchar max, uchar *bPinLen,uchar *pin_block);
uchar POS_API SYLPEDVerifyDerive(uchar MKeyID, uchar WKeyID, uchar *new_WKey, uchar *KeyCv, uchar mode);
uchar POS_API SYLPEDCalcPinBlock(uchar MKeyID,uchar PinKeyID, uchar *pin_block,uchar *new_TPK,uchar mode);
uchar POS_API SYLPPLoadTMK(uchar MkeyID1, uchar *TmpKey, uchar *KTMK, uchar *ReferCode);
uchar POS_API PEDManage(uchar mode,uchar *pwdcode);
uchar POS_API MemCardGetMac(uchar KeyID, uchar *CardNo, uchar *TransData,uchar* MacOut);
uchar POS_API MemCardGetPin(uchar KeyID,uchar *CardNo,uchar *CardSN,uchar *TransData,uchar *PWDOut);

uchar POS_API PEDBeep(void);
uchar POS_API PEDLight(uchar OperMode, uchar Index);
uchar POS_API PEDInvoice(uchar OperMode);
uchar POS_API PEDKbmute(uchar OperMode);
uchar POS_API PEDBackLight(uchar OperMode);
uchar POS_API PEDScrCls(void);
uchar POS_API PEDScrClrLine(uchar line);
uchar POS_API PEDScrPrint(uchar line, uchar col, uchar *str);
uchar POS_API PEDScrWrData(uchar line, uchar col, uchar *Data, uchar DataLen);
uchar POS_API PEDVerInfo(uchar *ver);
uchar POS_API PEDUpdLogo(uchar line, uchar *Data);
uchar POS_API PEDClearKey(uchar keytype);
uchar POS_API EPSPEDDispMenu(uchar StartNo, uchar EndNo, uchar AllLines, uchar detail[][16], uchar *item);
int POS_API PedWriteRsaKey(uchar RSAKeyIndex, ST_RSA_KEY* pstRsakeyIn);
int POS_API PedRsaRecover (uchar RSAKeyIndex, uchar *pucDataIn, uchar * pucDataOut, uchar* pucKeyInfoOut);
int POS_API	PedDukptDes(uchar GroupIdx, uchar KeyVarType,uchar *pucIV, ushort DataInLen, uchar *DataIn, uchar * DataOut, uchar*KsnOut ,uchar Mode);
int POS_API PedGetDukptKSN(uchar GroupIdx, uchar * KsnOut);
int POS_API PedDukptIncreaseKsn(uchar GroupIdx);

//----------------------------
// nicm add 2008-07-25 begin
//----------------------------

int POS_API PedWriteKey(ST_KEY_INFO * KeyInfoIn, ST_KCV_INFO * KcvInfoIn);
int POS_API PedWriteTIK(uchar GroupIdx,uchar SrcKeyIdx,uchar KeyLen,uchar * KeyValueIn,uchar * KsnIn, ST_KCV_INFO * KcvInfoIn);
int POS_API PedGetPinBlock(uchar KeyIdx, uchar *ExpPinLenIn, uchar * DataIn, uchar *PinBlockOut, uchar Mode, ulong TimeoutMs);
int POS_API PedGetMac(uchar KeyIdx, uchar *DataIn, ushort DataInLen, uchar *MacOut, uchar Mode);
int POS_API PedCalcDES(uchar KeyIdx, uchar * DataIn, ushort DataInLen, uchar * DataOut, uchar Mode);
int POS_API PedGetPinDukpt(uchar GroupIdx,uchar *ExpPinLenIn, uchar * DataIn,uchar* KsnOut,uchar * PinBlockOut, uchar Mode,ulong TimeoutMs);
int POS_API PedGetMacDukpt(uchar GroupIdx,uchar *DataIn, ushort DataInLen, uchar *MacOut, uchar * KsnOut, uchar Mode);
int POS_API PedReGenPinBlock(uchar UpdateFlag,ST_KEY_INFO *KeyInfoIn, ST_KCV_INFO *KcvInfoIn,uchar *DataIn,uchar *PinBlock, uchar Mode);
int POS_API PedVerifyPlainPin(uchar IccSlot, uchar *ExpPinLenIn, uchar *IccRespOut, uchar Mode,ulong TimeoutMs);
int POS_API PedVerifyCipherPin(uchar IccSlot, uchar *ExpPinLenIn, RSA_PINKEY *RsaPinKeyIn, uchar *IccRespOut, uchar Mode,ulong TimeoutMs);
int POS_API PedGetKcv(uchar KeyType, uchar KeyIdx,ST_KCV_INFO *KcvInfo);
int POS_API PedWriteKeyVar(uchar KeyType,uchar SrcKeyIdx, uchar DstKeyIdx,uchar * XorVarIn,ST_KCV_INFO * KcvInfoIn);
int POS_API PedGetVer(uchar * VerInfoOut);
int POS_API PedErase(void);
int POS_API PedSetIntervaltime(ulong TPKIntervalTimeMs, ulong TAKIntervalTimeMs);
int POS_API PedSetKeyTag(uchar * KeyTagIn);
int POS_API PedEraseKey(uchar KeyType, uchar KeyIdx);
int POS_API PedWriteKeyEx(ST_KEY_INFO* KeyInfoIn, void* KeyVarInfo, ST_KCV_INFO* KcvInfoIn);


//--------------------------
// nicm add 2008-07-25 end
//--------------------------
int POS_API PedSetFunctionKey(uchar  ucKey);
//================================================
//                PCI PED functions(S serials pos)
//================================================

uchar POS_API PciGetRandom(uchar *random);
//----------------------------------------------------------
// nicm comment because they are replaced by ped api begin
//----------------------------------------------------------
/*

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
*/
//----------------------------------------------------------
// nicm comment because they are replaced by ped api begin
//----------------------------------------------------------


//----------------------------------------------------------
// nicm add for new wireless module begin
//----------------------------------------------------------

int POS_API WlInit(const uchar *SimPinIn);
int POS_API WlGetSignal(uchar *SingnalLevelOut);
int POS_API WlPppLogin(uchar *APNIn, uchar *UidIn, uchar *PwdIn, long Auth, int TimeOut, int AliveInterval);
void POS_API WlPppLogout(void);
int POS_API WlPppCheck(void);
int POS_API WlOpenPort(void);
int POS_API WlClosePort(void);
int POS_API WlSendCmd(const uchar *ATstrIn, uchar *RspOut, ushort Rsplen, ushort TimeOut, ushort Mode);
void POS_API WlSwitchPower(uchar Onoff);
int	 POS_API WlSelSim(uchar simno);
int POS_API WlAutoStart(unsigned char *pin, unsigned  char *APNIn, unsigned  char * UidIn, unsigned  char * PwdIn,  long Auth, int TimeOut, int AliveInterval);
int POS_API WlAutoCheck(void);

//----------------------------------------------------------
// nicm add for new wireless module end
//----------------------------------------------------------


//================================================
//       Multiple apps management functions
//================================================
int   POS_API ReadAppInfo(uchar AppNo, APPINFO* ai);
uchar POS_API ReadAppState(uchar AppNo);
int   POS_API SetAppActive(uchar AppNo, uchar flag);
int   POS_API RunApp(uchar AppNo);
int   POS_API DoEvent(uchar AppNo, ST_EVENT_MSG *msg);

//================================================
//       File functions
//================================================
int   POS_API open(char *filename, uchar mode);
int   POS_API ex_open(char *filename, uchar mode,uchar* attr);
int   POS_API read(int fid, uchar *dat, int len);
int   POS_API write(int fid, uchar *dat, int len);
int   POS_API close(int fid);
int   POS_API seek(int fid, long offset, uchar fromwhere);
long  POS_API tell(int fid);
long  POS_API filesize(char *filename);
#ifndef _PROPAY_
int   POS_API remove(const char *filename);
#endif
long  POS_API freesize(void);
void POS_API InitFileSys(void);
int   POS_API truncate(int fid,long len);
int   POS_API fexist(char *filename);
int   POS_API GetFileInfo(FILE_INFO* finfo);
int   POS_API FileToApp(uchar *strFileName);
int   POS_API FileToParam(uchar *strSrcFileName, uchar *strAppName, int mode);
int   POS_API FileToMonitor(uchar *strFileName);
int   POS_API ReadFontLib(ulong Offset,uchar *FontData, int ReadLen);
//<-----------------------------
// lijq add 2009-10-20
//<-----------------------------
int POS_API FileToPuk(int PukType, int PukIdx,uchar *FileName);
//<-----------------------------
// lijq add 2009-10-20
//<-----------------------------


//<-----------------------------
// nicm add 2009-05-31
//<-----------------------------
int POS_API DelAppFile(uchar *AppName);
//------------------------------>
// nicm add 2009-05-31
//------------------------------>

//================================================
//       Enviromental variable functions
//================================================
uchar POS_API GetEnv(char *name, uchar *value);
uchar POS_API PutEnv(char *name, uchar *value);

//================================================
//     Asynchronism communication functions
//================================================
uchar POS_API PortOpen(uchar channel, uchar *para);
uchar POS_API PortClose(uchar channel);
uchar POS_API PortSend(uchar channel, uchar ch);
uchar POS_API PortRecv(uchar channel, uchar *ch, ushort ms);
uchar POS_API PortReset(uchar channel);
uchar POS_API PortSends(uchar channel,uchar *str,ushort str_len);
uchar POS_API PortTxPoolCheck(uchar channel);
int POS_API PortPeep(uchar channel,uchar *buf,ushort want_len);
int POS_API PortRecvs(uchar Channel,uchar *pszBuf,ushort usBufLen,ushort usTimeoutMs);

//================================================
//		Modem communication functions
//================================================
uchar POS_API ModemReset(void);
uchar POS_API ModemDial(COMM_PARA *MPara, uchar *TelNo, uchar mode);
uchar POS_API ModemCheck(void);
uchar POS_API ModemTxd(uchar *SendData, ushort len);
uchar POS_API ModemRxd(uchar *RecvData, ushort *len);
uchar POS_API ModemAsyncGet(uchar *ch);
uchar POS_API OnHook(void);
ushort POS_API ModemExCommand(uchar *CmdStr, uchar *RespData,
					  ushort *Dlen,ushort Timeout10ms);

//================================================
//	     Wireless lan functions
//================================================
uchar POS_API WNetInit(void);
uchar POS_API WNetCheckSignal(uchar *pSignalLevel);
uchar POS_API WNetCheckSim(void);
uchar POS_API WNetSimPin(uchar *pin);
uchar POS_API WNetUidPwd(uchar *Uid, uchar *Pwd);
uchar POS_API WNetDial(uchar *DialNum, uchar * reserved1, uchar * reserved2);
uchar POS_API WNetCheck(void);
uchar POS_API WNetTcpConnect(uchar *DestIP, uchar *DestPort);
uchar POS_API WNetTcpTxd(uchar *TxData, ushort txlen);
uchar POS_API WNetTcpRxd(uchar *RxData, ushort *prxlen, ushort ms);
uchar POS_API WNetTcpClose(void);
uchar POS_API WNetClose(void);
uchar POS_API WNetLinkCheck(void);
uchar POS_API WNetTcpCheck(void);
uchar POS_API WNetReset(void);
uchar POS_API WNetSendCmd(uchar *cmd, uchar *rsp, ushort rsplen, ushort ms);
uchar POS_API WNetMTcpConnect(int *psocket, uchar *DestIP, uchar *DestPort);
uchar POS_API WNetMTcpTxd(int socket, uchar *TxData,  ushort txlen);
uchar POS_API WNetMTcpRxd(int socket, uchar *RxData, ushort maxlen, ushort *prxlen, ushort ms);
uchar POS_API WNetMTcpClose(int socket);
uchar POS_API WNetMTcpCheck(int socket);

// s80

//-----------------------------------------------------------------
// nicm comment because they are replaced by new wireless fun begin
//-----------------------------------------------------------------
/*
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
*/
//-----------------------------------------------------------------
// nicm comment because they are replaced by new wireless fun end
//-----------------------------------------------------------------

//-----------------------
// nicm add 2009-08-26
//-----------------------
int POS_API PppoeLogin(char *name, char *passwd, int timeout);
void POS_API PppoeLogout(void);
int POS_API PppoeCheck(void);


//================================================
//	     PPP connection functions
//================================================
int   POS_API NpppLogin(char *user_name,char *user_password);
int   POS_API NpppOpen(uchar *remote_addr,ushort remote_port, ushort local_port,uchar mode);
int   POS_API NpppWrite(int connect_handle,uchar *buf,ushort len);
int   POS_API NpppRead(int connect_handle,uchar *out_data,ushort len);
void  POS_API NpppCheck(int connect_handle,PPP_NET_INFO *pi);
int   POS_API NpppClose(int connect_handle);
void  POS_API NpppProcess(void);
int   POS_API GsmCallModem(GSM_CALL *CallPara, uchar *TelNo, uchar *ErrMsg);
int   POS_API GsmCheck(void);
int   POS_API GsmOnHook(void);
int   POS_API NpppSetAuth(ulong authent);

//================================================
//	     TCP/IP functions
//================================================
int POS_API NetSocket(int domain, int type, int protocol);
int POS_API NetBind(int socket, struct net_sockaddr *addr, socklen_t addrlen);
int POS_API NetConnect(int socket, struct net_sockaddr *addr, socklen_t addrlen);
int POS_API NetListen(int socket, int backlog);
int POS_API NetAccept(int socket, struct net_sockaddr *addr, socklen_t *addrlen);
int POS_API NetSend(int socket, void *buf, int size, int flags);
int POS_API NetSendto(int socket, void *buf, int size, int flags,
		struct net_sockaddr *addr, socklen_t addrlen);
int POS_API NetRecv(int socket, void *buf, int size, int flags);
int POS_API NetRecvfrom(int socket, void *buf, int size, int flags,
		struct net_sockaddr *addr, socklen_t *addrlen);
int POS_API NetCloseSocket(int socket);
int POS_API Netioctl(int socket, int cmd, int arg);

//<------------------------
// nicm add 2009-05-15
//<------------------------
/*
** NetAddStaticArp Ôö¼Ó¾²Ì¬ARP±íÏî
** ip   ipaddress
** mac  the host's mac address
** if success, return 0; otherwise, return < 0; **/
int POS_API NetAddStaticArp(char *ip_str, unsigned char mac[6]);

/*
**  NetSetIcmp ÉèÖÃICMP¿ª¹Ø
**  flag   bit0Îª1Ê±  ÄÜ¹»ÏìÓ¦ÆäËû»úÆ÷µÄpingÇëÇó
**         bit0Îª0Ê±  ¹Ø±ÕÏìÓ¦ÆäËû»úÆ÷µÄpingÇëÇó
**/
void POS_API NetSetIcmp(unsigned long flag);

//------------------------>
// nicm add 2009-05-15
//------------------------>

int POS_API SockAddrSet(struct net_sockaddr *addr, char *ip_str, short port);
int POS_API SockAddrGet(struct net_sockaddr *addr, char *ip_str, short *port);
int POS_API PPPLogin(char *name, char *passwd, long auth , int timeout);
void POS_API PPPLogout(void);
int POS_API PPPCheck(void);
int POS_API EthSet(
		char *host_ip, char *host_mask,
		char *gw_ip,
		char *dns_ip
		);
int POS_API EthGet(
		char *host_ip, char *host_mask,
		char *gw_ip,
		char *dns_ip,
		long *state
		);

//-----------------------
// nicm add 2008-12-15
// »ñÈ¡ÖÕ¶ËMACµØÖ·
//-----------------------
int POS_API EthMacGet(unsigned char mac[6]);



//-----------------------
// nicm add 2009-08-10
// ssl api
//-----------------------
int POS_API SslSaveCerPrivkey(uint ServIp,
					  ushort ServPort,
					  ST_SSL_BUF CertChains[],
					  int CertChainsNum,
					  ST_SSL_BUF *Crl,
					  ST_SSL_BUF LocalCerts[],
					  int LocalCertsNum,
					  uchar KeyIdx,//des key ÃÜÔ¿ºÅ
					  ST_SSL_BUF * LocalPrivatekey,/* OUT */
					  uchar FormatType
					  );
//delete privkey file
int POS_API SslDelCerPrivkey(unsigned int ServIp, unsigned short ServPort);


int POS_API DnsResolve(char *name, char *result, int len);
int POS_API NetPing(char *dst_ip_str, long timeout, int size);
int POS_API RouteSetDefault(int ifIndex);
int POS_API RouteGetDefault(void);
int POS_API DhcpStart(void);
void POS_API DhcpStop(void);
int POS_API DhcpCheck(void);

//=============================================================================
// ¶ÁÈ¡ÍøÂçÉè±¸µÄIPÅäÖÃÐÅÏ¢
// ²ÎÊý£º
// Dev	ÍøÂçÉè±¸±àºÅ(10½øÖÆÖÆ)
// 0     ÒÔÌ«ÍøÉè±¸
// 10    Modem PPP
// 11    ÎÞÏßÄ£¿é£¨°üÀ¨GPRSºÍCDMA£©
//
// HostIp	ÓÃÓÚ´æ·ÅIPµØÖ·ÐÅÏ¢£»
// Ó¦ÓÃ³ÌÐò·ÖÅäµÄ¿Õ¼ä²»ÉÙÓÚ20¸ö×Ö½Ú£»
// Èç¹ûÓ¦ÓÃ³ÌÐò²»ÐèÒª¸ÃÐÅÏ¢£¬Ö±½Ó¸øNULL
//
// Mask	ÓÃÓÚ´æ·ÅÑÚÂëÐÅÏ¢£»
// Ó¦ÓÃ³ÌÐò·ÖÅäµÄ¿Õ¼ä²»ÉÙÓÚ20¸ö×Ö½Ú£»
// Èç¹ûÓ¦ÓÃ³ÌÐò²»ÐèÒª¸ÃÐÅÏ¢£¬Ö±½Ó¸øNULL
//
// GateWay	ÓÃÓÚ´æ·ÅÍø¹ØµØÖ·£»
// Ó¦ÓÃ³ÌÐò·ÖÅäµÄ¿Õ¼ä²»ÉÙÓÚ20¸ö×Ö½Ú£»
// Èç¹ûÓ¦ÓÃ³ÌÐò²»ÐèÒª¸ÃÐÅÏ¢£¬Ö±½Ó¸øNULL
//
// Dns	DNS·þÎñÆ÷µØÖ·£»
// Ó¦ÓÃ³ÌÐò·ÖÅäµÄ¿Õ¼ä²»ÉÙÓÚ20¸ö×Ö½Ú£»
// Èç¹ûÓ¦ÓÃ³ÌÐò²»ÐèÒª¸ÃÐÅÏ¢£¬Ö±½Ó¸øNULL

// ·µ»Ø£º
// =0    ¶ÁÈ¡³É¹¦£¬HostIp¡¢Mask¡¢GateWayºÍDnsÄÚÈÝÓÐÐ§
// <0     ´íÎó´úÂë£»
//=============================================================================
int POS_API NetDevGet(int Dev,char *HostIp,char *Mask, char *GateWay,char *Dns);


//================================================
//       FAT file system functions
//================================================
int   POS_API FsOpen(FS_W_STR *name, int attr);
int   POS_API FsClose(int fd);
int   POS_API FsDelete(FS_W_STR *name_in);
int   POS_API FsGetInfo(int fd, FS_INODE_INFO *fs_inode);
int   POS_API FsRename(FS_W_STR * name, FS_W_STR *new_name);
int   POS_API FsDirRead(int fd, FS_INODE_INFO *fs_inode, int num);
int   POS_API FsDirSeek(int fd, int num, int flag);
int   POS_API FsFileRead(int fd, char *buf, int len);
int   POS_API FsFileWrite(int fd, char *buf, int len);
int   POS_API FsFileSeek(int fd, int offset, int flag);
int  POS_API  FsFileTell(int fd);
int   POS_API FsFileTruncate(int fd, int size, int reserve_space);
int   POS_API FsSetWorkDir(FS_W_STR *name);
int   POS_API FsGetWorkDir(FS_W_STR *name);
int   POS_API FsUdiskIn(void);
int   POS_API FsGetDiskInfo(int disk_num, FS_DISK_INFO *disk_info);
void POS_API FsRecycle(int NeedSize);


//================================================
//	     USB functions
//================================================
int POS_API UsbDevStart(void);
int POS_API UsbDevStop(void);
int POS_API UsbDevCheck(void);
int POS_API UsbDevSend(char *buf, int size, int timeout_ms);
int POS_API UsbDevRecv(char *buf, int size, int timeout_ms);

//=================================================
//Tilt Sensor fun
//=================================================
void POS_API GetLeanAngle(int *piXLeanAngle,int *piYLeanAngle,int *piZLeanAngle);

//=================================================
//WPhone fun
//=================================================
uchar POS_API WPhoneCall(uchar *PhoneNum);
uchar POS_API WPhoneHangUp();
uchar POS_API WPhoneStatus();
uchar POS_API WPhoneAnswer();
uchar POS_API WPhoneMicGain(uchar level);
uchar POS_API WPhoneSpkGain(uchar level);
uchar POS_API WPhoneSendDTMF(uchar DTMFNum, ushort DurationMs);

//=================================================
//remote download fun
//=================================================
int POS_API RemoteLoadApp(const T_INCOMMPARA *ptCommPara);
int POS_API QueryLoadTask(T_INCOMMPARA *ptCommPara, uchar *psQueryTask);
int POS_API RemoteLoadManage(uchar mode,uchar *appname,LOADLOG_INFO *loadloginfo);
int POS_API GetLoadedAppStatus(uchar *psAppName,T_LOADSTATUS *ptStat);

// Touch screen fun
int POS_API TsScrCheck(int *X, int *Y);
void POS_API TsSignatureStart (int  x ,  int  y, int w, int h,int Timeout, const char *ImageFile, int ImageType, ulong BkColor, ulong LineColor,int LineWidth);
int POS_API TsSignatureCapture (void);
void POS_API TsSignatureStop(void);
void POS_API TsSignatureClear (void);
void POS_API TsDisable (void);
void POS_API TsEnable (void);
int POS_API TsOpen(void);
int POS_API TsClose(void);

//================================================
//            Bluetooth functions
//================================================
int POS_API BtGetStatus(uchar *BtStatus);
int POS_API BtIoCtrl(int iCmd, void * pArgIn, int iSizeIn, void * pArgOut, int iSizeOut);
void POS_API ModuleUpdate(void);


int POS_API ScrProcessImage(char* FileName, int FileType, int cmd, int x, int y);

//================================================
//            Audio functions
//================================================
int POS_API SoundPlay(char*filename, char type); // just for T60

//================================================
//            WIFI functions
//================================================
int POS_API WifiOpen(void);
int POS_API WifiClose(void);
int POS_API WifiScanAps(ST_WIFI_AP *outAps, int ApCount);
int POS_API WifiConnectAp(ST_WIFI_AP *Ap,ST_WIFI_PARAM *WifiParam);
int POS_API WifiDisconAp(void);
int POS_API WifiCheck(ST_WIFI_AP * Ap);
int POS_API WifiIoCtrl(int iCmd, void * pArgIn, int iSizeIn, void * pArgOut, int iSizeOut);

int POS_API MfiGetStatus(void);

// Ò»Î¬ÌõÂëÍ¨ÓÃ·µ»ØÂë
#define SCAN1D_OK                    0     // ²Ù×÷³É¹¦
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
int POS_API Scan1dOpen(void);

// ¹¦ÄÜ: ¹Ø±ÕÒ»Î¬ÌõÂëÄ£¿é
void POS_API Scan1dClose(void);

// ¹¦ÄÜ: ¿ªÆôÒ»Î¬ÌõÂë£¬²¢¿ªÊ¼É¨Ãè
int POS_API Scan1dStartDecode(void);

/*
¹¦ÄÜ: »ñÈ¡É¨ÃèÊý¾Ý
²ÎÊý:
BarType: Êä³öµÄÌõÂëÀàÐÍ, 1×Ö½Ú
BarData: Êä³öµÄÌõÂëÊý¾Ý, 1-256×Ö½Ú, ÒÔ'\0'Îª½áÊø·û
TimeOutMs: ½ÓÊÕ³¬Ê±
*/
int POS_API Scan1dGetDecodeData(uchar *BarType, uchar *BarData, ushort TimeOutMs);

/*
¹¦ÄÜ: »ñÈ¡Ä£¿é°æ±¾ÐÅÏ¢
²ÎÊý:
Version:  Êä³öµÄ°æ±¾×Ö·û´®, 1-256×Ö½Ú, ÒÔ'\0'Îª½áÊø·û
*/
int POS_API Scan1dReadModuleVer(uchar *Version);

/*
¹¦ÄÜ: ÉèÖÃÄ£¿éÉ¨ÃèÄ£Ê½
²ÎÊý:
Mode:  0 - µ¥´ÎÉ¨Ãè, 1 - Á¬ÐøÉ¨Ãè
ËµÃ÷: Ä£¿éÄ¬ÈÏÊÇµ¥´ÎÉ¨Ãè
*/
int POS_API Scan1dSetScanMode(int Mode);

//#endif  // ifdef WIN32

#ifdef __cplusplus
};
#endif

#endif	// #ifndef  _POSAPI_ALL_H_
