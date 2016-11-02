#include "global.h"

#define __TMS_INGENICO__
#ifdef __TMS_INGENICO__


static uchar	sgSyncDial, sgNewTMS;
static uchar	sgTempBuf[1024*20];
static uchar	sgEMVDownloaded;

#define MOV(N) if(len<N) return -1; len-= N; rsp+= N
#define PARSE(F) ret = F(rsp, length); CHK; MOV(ret);

//TMS EMV Tags
#define EMV_KEY_EXPONENT 0xDFE91B
#define EMV_KEY_ALGORITHIM 0xDFE91C
#define EMV_DEFAULT_PARAM 0xFFE913
#define EMV_DEF_APPROVAL_LEVEL 0xDFE91D
#define EMV_DEF_TAGS 0xFFE91E
#define EMV_KEY 0xFFE911
#define EMV_KEY_RID 0xDFE918
#define EMV_KEY_INDEX 0xDFE919
#define EMV_KEY_DATA 0xDFE91A
#define EMV_AID_PARAM 0xFFE910
#define EMV_AID 0xDFE914
#define EMV_AID_PRIORITY 0xDFE91F
#define EMV_AID_APP_SEL_INDIC 0xDFE920
#define EMV_AID_TAGS 0xFFE915
#define EMV_TXN_TYPE 0xDFE916
#define EMV_AID_TXN_TAGS 0xFFE917

#define tmsTACDft 0x1f03
#define tmsTACDen 0x1f04
#define tmsTACOnl 0x1f05
#define tmsThrVal 0x1f07
#define tmsTarPer 0x1f08
#define tmsMaxTarPer 0x1f09
#define tmsDftValDDOL 0x1f15
#define tmsDftValTDOL 0x1f18
#define tagTrnTyp	0x9c
#define tagTrmAvn 	0x9F09         //Application Version Number Terminal
#define tagTrmFlrLim  0x9F1B      //Terminal Floor Limit
#define tagTCC 0x9F53            //MC Transaction Category Code

#define mask8 ((byte)0x80)
#define mask54321 ((byte)0x1F)
#define mask7654321 ((byte)0x7F)

static byte emvFile[65535];
static int emvFileIdx = 0;

static byte aidRecNum;
static byte pubRecNum;
static byte binRecNum;
static byte acqRecNum;
static byte issRecNum;

void dec2bin(char * dst, char * src, int len)
{
	card num;
	char hex[2+1], bin[2+1];
	char bufSrc[256];
	int i;
	char bufDst[256];
	char srcLoop[2];

	MEM_ZERO(bufDst);
	MEM_ZERO(bufSrc);
	memcpy(bufSrc, src, len);
	
	for(i = 0; i < len; i++) 
	{			
		MEM_ZERO(hex);
		MEM_ZERO(bin);
		MEM_ZERO(srcLoop);
		num = 0;
	
		srcLoop[0] = bufSrc[i];
		
		bin2hex(hex, srcLoop, 1);
		dec2num(&num, hex, 0);
		num2bin(bin, num, 1);
		memcpy(bufDst + i, bin, 1);
	}
	memcpy(dst, bufDst, strlen(bufDst));
}


static int fillbit60(tBuffer * val) {
	int ret = 0;
    	uchar sta, ReqRecCnt = 1, mreRec = 0;
    	uchar buffer[lenFleNme + 1];
    	ulong offset = 0;
	byte BinReq;


    	sta = glProcInfo.tlmTra.traSta;

	ret = bufApp(val, (uchar *) "\x00\x66\x00\x03\x01\x01\x90\x01", 8);  // initialize buffer with con table req.
    	CHK;
    	switch (sta) {
      		case dnlSendLoginReq:
      		case dnlSendLogOffReq:
	   		bufReset(val);
	   		ret = bufApp(val, (uchar *) "\x00", 1);    // GENERIC_FILE_OPEN
	   		CHK;
          		break;
      		case dnlSendConTblReq:
          		break;
      		case dnlSendBinTblReq:
			ret = bufOwr(val, 3, (byte *) "\x01\x02", 2); //change the apiVersion and table number required, bin table is table 2
			CHK;
			mreRec = glProcInfo.tlmTra.traTmsMreRec;
			BinReq = glProcInfo.tlmTra.traBinReq;
			if(BinReq == 1) {
				if(mreRec)
				{	
					ReqRecCnt = glTlmTmsBin.ucBinNum;
//					ReqRecCnt = glTlmReg.regBinCnt;
//					ReqRecCnt = glSysCtrl.TlmReg.regBinCnt;
					ReqRecCnt += 1;
					ret = bufOwr(val, 5, &ReqRecCnt, 1);  //overwrite the needed record count for request
					CHK;
				}
			}
			break;
      case dnlSendIssTblReq:
          ret = bufOwr(val, 3, (uchar *) "\x02\x03", 2); //change the apiVersion and table number required, iss table is table 1
          CHK;
//       ret = mapGetByte(traTmsMreRec, mreRec);
	    mreRec = glProcInfo.tlmTra.traTmsMreRec;
//       CHK;
	   
          if(mreRec) {
//          	ret = mapGetByte(regIssCnt, ReqRecCnt);
		ReqRecCnt = glTlmTmsIss.ucIssNum;
//		ReqRecCnt = glTlmReg.regIssCnt;
//		ReqRecCnt = glSysCtrl.TlmReg.regIssCnt;
//          	CHK;
		
              ReqRecCnt += 1;
              ret = bufOwr(val, 5, &ReqRecCnt, 1);  //overwrite the needed record count for request
              CHK;
          }
          break;
      case dnlSendAcqTblReq:
          ret = bufOwr(val, 3, (uchar *) "\x02\x04", 2); //change the apiVersion and table number required, acq table is table 1
          CHK;
//          	ret = mapGetByte(traTmsMreRec, mreRec);
		mreRec = glProcInfo.tlmTra.traTmsMreRec;
//          CHK;
          if(mreRec) {
//              ret = mapGetByte(regAcqCnt, ReqRecCnt);
		ReqRecCnt = glTlmTmsAcq.ucAcqNum;
//		ReqRecCnt = glTlmReg.regAcqCnt;
//		ReqRecCnt = glSysCtrl.TlmReg.regAcqCnt;
//              CHK;
              ReqRecCnt += 1;
              ret = bufOwr(val, 5, &ReqRecCnt, 1);  //overwrite the needed record count for request
              CHK;
          }
          break;
      case dnlSendEmvReq:
          bufReset(val);        //change buffer for emv request
          ret = bufApp(val, (uchar *) "\x00\x66\x00\x01\x00\x00\x01", 7);    // initialize buffer for emv request
          CHK;
          break;
      case dnlSendEmvFileReq:
          bufReset(val);        //change buffer for emv request
          CHK;
          ret = bufApp(val, (uchar *) "\x01", 1);    // GENERIC_FILE_OPEN
          CHK;
//          ret = mapGet(traTmsEmvFleNme, buffer, lenFleNme);
	   strcpy(buffer, glProcInfo.tlmTra.traTmsEmvFleNme);
//          CHK;
          ret = bufApp(val, buffer, strlen((char *) buffer) + 1);   // initialize buffer for emv request
          CHK;
          ret = bufApp(val, (uchar *) "\x72\x62\x00", 3);    //fopen mode "rb"
          CHK;
          break;
      case dnlSendEmvFileOpnReq:
          bufReset(val);        //change buffer for emv request
          CHK;
          ret = bufApp(val, (uchar *) "\x02", 1);    // GENERIC_FILE_INFO
          CHK;
//          ret = mapGet(traTmsEmvFleHdl, buffer, sizeof(buffer));
	   memcpy(buffer, glProcInfo.tlmTra.traTmsEmvFleHdl, lenFleHdl);
//          CHK;
          ret = bufApp(val, buffer, lenFleHdl); // initialize buffer for emv file open
          CHK;
          break;
      case dnlSendEmvFileRedReq:
          bufReset(val);        //change buffer for emv request
          CHK;
          ret = bufApp(val, (uchar *) "\x03", 1);    // GENERIC_FILE_READ
          CHK;
//          ret = mapGet(traTmsEmvFleHdl, buffer, sizeof(buffer));
	   memcpy(buffer, glProcInfo.tlmTra.traTmsEmvFleHdl, lenFleHdl);
//          CHK;
          ret = bufApp(val, buffer, lenFleHdl); // initialize buffer for emv file open
          CHK;
//          ret = mapGetCard(traTmsEmvFleOff, offset);
	   offset = glProcInfo.tlmTra.traTmsEmvFleOff;
//          CHK;
          memset(buffer, 0x00, sizeof(buffer));
          ret = num2bin((uchar *) buffer, offset, lenFleHdl);
          CHK;
          ret = bufApp(val, buffer, lenFleHdl); // initialize buffer for emv file read
          CHK;
          ret = bufApp(val, (uchar *) "\x01\xBB", sizeof(word)); // size of chunk request is always 443
          CHK;
          break;
      case dnlSendEmvFileClsReq:
          bufReset(val);        //change buffer for emv request
          CHK;
          ret = bufApp(val, (uchar *) "\x05", 1);    // GENERIC_FILE_CLOSE
          CHK;
//          ret = mapGet(traTmsEmvFleHdl, buffer, sizeof(buffer));
          memcpy(buffer, glProcInfo.tlmTra.traTmsEmvFleHdl, lenFleHdl);
//          CHK;
          ret = bufApp(val, buffer, lenFleHdl); // initialize buffer for emv file open
          CHK;
          break;
      default:
          ret = -1;
          break;
    }

    return ret;
  lblKO:
    return -1;
}


#endif


// Bit 3 definition:
static void ModifyProcessCodeTMS(void)
{
	if( glProcInfo.stTranLog.ucTranType==TRANS_TYPE_TMS_TELIUM)
	{
		switch(glProcInfo.tlmTra.traSta)
		{
			case dnlSendLoginReq:
				strcpy(glTMSTELIUMSend.szBit3, "010000");
				break;
			case dnlSendConTblReq:
    			case dnlSendBinTblReq:
   			case dnlSendIssTblReq:
    			case dnlSendAcqTblReq:
				strcpy(glTMSTELIUMSend.szBit3, "010101");
				break;
			case dnlSendEmvReq:
				strcpy(glTMSTELIUMSend.szBit3, "010102");
				break;
			case dnlSendEmvFileReq:
			case dnlSendEmvFileOpnReq:
			case dnlSendEmvFileRedReq:
			case dnlSendEmvFileClsReq:
				strcpy(glTMSTELIUMSend.szBit3, "010600");
				break;
			case dnlSendLogOffReq:
				strcpy(glTMSTELIUMSend.szBit3, "011000");
				break;
		}
	}

    PubStrUpper(glTMSTELIUMSend.szBit3);
}



// set bit content of ISO8583 TMS TELIUM.
void SetCommReqFieldTMS(void)
{
	uchar sn[32+1];
	uchar buffer[1024];
	tBuffer val;
	
	memset(sn, 0, sizeof(sn));
	memset(buffer, 0, sizeof(buffer));
	bufInit(&val, buffer, sizeof(buffer));
	
	memset(&glTMSTELIUMSend, 0, sizeof(STTMSTELIUM8583));
	
//MTI
	sprintf((char *)glTMSTELIUMSend.szMsgCode, "%.*s", LEN_MSG_CODE, glTranConfig[glProcInfo.stTranLog.ucTranType].szTxMsgID);

//BIT 3
	sprintf((char *)glTMSTELIUMSend.szBit3,   "%.*s", lenBit3, glTranConfig[glProcInfo.stTranLog.ucTranType].szProcCode);
	ModifyProcessCodeTMS();

//BIT 8	
	memcpy((char *)glTMSTELIUMSend.szBit8,"00000500" , lenBit8);

//BIT 24
	sprintf((char *)glCurTlmTmsAcq.acqNII,    "%.3s", glTlmSysParam.stEdcInfo.szTMSNii);
	sprintf((char *)glTMSTELIUMSend.szBit24,    "%.*s", lenBit24,     glCurTlmTmsAcq.acqNII);

//BIT41
	ReadSN(sn);
	sprintf((char *)glTMSTELIUMSend.szBit41, "%.*s", lenBit41, sn);

//BIT42	(special for TMS)
	memcpy(&glTMSTELIUMSend.szBit42, "\x00\x08", 2);
	memcpy(&glTMSTELIUMSend.szBit42[2], glTlmSysParam.stEdcInfo.szDownLoadTID, lenBit41);

//BIT 60 
		fillbit60(&val);
		PubLong2Char((ulong)bufLen(&val), 2, glTMSTELIUMSend.szBit60);
		memcpy(&glTMSTELIUMSend.szBit60[2], bufPtr(&val), bufLen(&val));
}



static int ConvertIpPortSrv(const byte * rsp, char *ip, char *port, word len)
{
	char tmpbuf[10];
	card val;                   //converted values
	byte cnt = 0;
	int ret = 0;
	char bufIp[15];
	char bufPort[6];

	MEM_ZERO(bufIp);
	MEM_ZERO(bufPort)

//ip
	memset(tmpbuf, 0x00, sizeof(tmpbuf));
	for (cnt = 0; cnt < 4; cnt++)   //  is the length of ip address
	{
		val = (byte) * rsp;
		num2dec(tmpbuf, val, 3);
		strcat(bufIp, tmpbuf);
		if(cnt < 3)
			strcat(bufIp, ".");
		memset(tmpbuf, 0x00, sizeof(tmpbuf));
		MOV(1);
	}
	strcpy(ip, bufIp);
	ret = cnt;                  // size of ip in bcd

//port
	ret = ret + bin2num(&val, rsp, 2);  // add size of port in bcd
	num2dec(tmpbuf, val, 4);
	strcat(bufPort, tmpbuf);
	strcpy(port, bufPort);
	MOV(2);

	return ret;
}


static int ConvIpPrtSrv(const byte * rsp, char *buffer, word len)
{
	char tmpbuf[10];
	card val;                   //converted values
	byte cnt = 0;
	int ret = 0;

	memset(tmpbuf, 0x00, sizeof(tmpbuf));
	for (cnt = 0; cnt < 4; cnt++)   //  is the length of ip address
	{
		val = (byte) * rsp;
		num2dec(tmpbuf, val, 3);
		strcat(buffer, tmpbuf);
		if(cnt < 3)
			strcat(buffer, ".");
		memset(tmpbuf, 0x00, sizeof(tmpbuf));
		MOV(1);
	}
	ret = cnt;                  // size of ip in bcd

	strcat(buffer, "|");
	ret = ret + bin2num(&val, rsp, 2);  // add size of port in bcd
	num2dec(tmpbuf, val, 4);
	strcat(buffer, tmpbuf);
	MOV(2);

	return ret;
}


static int emvGetT(byte * tag, const byte * src)
{  //extract tag from BER-TLV encoded buffer
	byte ret;

	*tag = *src;
	ret = 1;
	if(((*tag) & mask54321) != mask54321)
		return 1;               //1-byte tag
	do
	{                        //process multiple-byte tags
		ret++;
		tag++;
		src++;
		*tag = *src;
	}
	while((*tag) & mask8);
	return ret;
}

static int emvGetL(card * len, const byte * src)
{  //extract length from BER-TLV encoded buffer
	byte ret;

	if(((*src) & mask8) != mask8)
	{ //the easiest case : 1-byte length
		*len = *src;
		ret = 1;
		goto lblOK;
	}
	ret = (*src) & mask7654321;
	src++;
	*len = *src++;
	ret--;

	if(ret == 0)
	{              //two-byte length
		ret = 2;
		goto lblOK;
	}
	*len *= 0x100;
	*len += *src++;
	ret--;

	if(ret == 0)
	{              //three-byte length
		ret = 3;
		goto lblOK;
	}
	*len *= 0x100;
	*len += *src++;
	ret--;

	if(ret == 0)
	{              //four-byte length
		ret = 4;
		goto lblOK;
	}
	*len *= 0x100;
	*len += *src++;
	ret--;
	if(ret == 0)
	{              //five-byte length
		ret = 5;
		goto lblOK;
	}

	return -1;                  //very long TLVs are not supported
lblOK:
	return ret;
}

static word mapPutTmsTag(const byte * rsp, word len, uchar *data, uint lendata)
{  //matches tms tag to correct key in dbs for saving
	card respDta;

	bin2num(&respDta, rsp, (byte)len);

	switch (respDta)
	{
		case tmsTACDft:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvTACDft, data, lendata);
//			hex_dump_char("emvTACDft", glTlmTmsAid.Aid[aidRecNum].emvTACDft, lendata);
			break;
		case tmsTACDen:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvTACDen, data, lendata);
//			hex_dump_char("emvTACDen", glTlmTmsAid.Aid[aidRecNum].emvTACDen, lendata);
			break;
		case tmsTACOnl:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvTACOnl, data, lendata);
//			hex_dump_char("emvTACOnl", glTlmTmsAid.Aid[aidRecNum].emvTACOnl, lendata);
			break;
		case tmsThrVal:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvThrVal, data, lendata);
//			hex_dump_char("emvThrVal", glTlmTmsAid.Aid[aidRecNum].emvThrVal, lendata);
			break;
		case tmsTarPer:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvTarPer, data, lendata);
//			hex_dump_char("emvTarPer", glTlmTmsAid.Aid[aidRecNum].emvTarPer, lendata);
			break;
		case tmsMaxTarPer:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvMaxTarPer, data, lendata);
//			hex_dump_char("emvMaxTarPer", glTlmTmsAid.Aid[aidRecNum].emvMaxTarPer, lendata);
			break;
		case tmsDftValDDOL:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvDftValDDOL, data, lendata);
//			hex_dump_char("emvDftValDDOL", glTlmTmsAid.Aid[aidRecNum].emvDftValDDOL, lendata);
			break;
		case tmsDftValTDOL:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvDftValTDOL, data, lendata);
//			hex_dump_char("emvDftValTDOL", glTlmTmsAid.Aid[aidRecNum].emvDftValTDOL, lendata);
			break;
		case tagTrnTyp:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvTrnTyp, data, lendata);
//			hex_dump_char("emvTrnTyp", glTlmTmsAid.Aid[aidRecNum].emvTrnTyp, lendata);
			break;
		case tagTrmAvn:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvTrmAvn, data, lendata);
//			hex_dump_char("emvTrmAvn", glTlmTmsAid.Aid[aidRecNum].emvTrmAvn, lendata);
			break;
		case tagTrmFlrLim:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvTrmFlrLim, data, lendata);
//			hex_dump_char("emvTrmFlrLim", glTlmTmsAid.Aid[aidRecNum].emvTrmFlrLim, lendata);
			break;
		case tagTCC:
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvTCC, data, lendata);
//			hex_dump_char("emvTCC", glTlmTmsAid.Aid[aidRecNum].emvTCC, lendata);
			break;
	}
	return 0;
}



static int parseTerDta(const byte * rsp, word lenDta)
{ //updates the terminal configurations  data with the ones that are obtained from the TMS
	int ret = 0, val = 0;
	card len = 0;
	byte tag[2];
	byte tarPer[2 + 1];
	card respDta;

	while(lenDta > 0)
	{
		memset(tag, 0, sizeof tag);
		ret = emvGetT(tag, rsp);
		CHK;

		rsp += ret;
		lenDta -= ret;

		len = 0;
		val = 0;
		val = emvGetL(&len, rsp);
		CHECK(val >= 0, lblKO);

		bin2num(&respDta, tag, (byte)ret);

		switch (respDta)
		{
			case tmsTarPer:      //convert to one tag length
			case tmsMaxTarPer:
				if(ret == 2)
				{
					memcpy(tarPer, "\x01", 1);
					rsp += 2;
					memcpy(&tarPer[1], rsp, 1);

					mapPutTmsTag(tag, (word)ret, tarPer, 2);
//					keyTag = mapKeyTmsTag(tag, ret);
//					ret = mapPut(keyTag, tarPer, 2);					

					rsp += 1;
				}
				else
				{
					mapPutTmsTag(tag, (word)ret, (uchar*)rsp, len+val);
//					keyTag = mapKeyTmsTag(tag, ret);
//					ret = mapPut(keyTag, rsp, len + val);

					rsp += (len + val);
				}
				break;
			case tagTrmAvn:
				if(len != 0)
				{
					mapPutTmsTag(tag, (word)ret, (uchar*)rsp, len+val);
//					ret = mapPut(mapKeyTmsTag(tag, ret), rsp, len + val);
				}
				rsp += (len + val);
				break;
			default:
				mapPutTmsTag(tag, (word)ret, (uchar*)rsp, len+val);
//				ret = mapPut(mapKeyTmsTag(tag, ret), rsp, len + val);
				
				rsp += (len + val);
				break;
		}
		CHK;

		lenDta -= (word)(len + val);
	}

	return ret;
lblKO:
	return -1;
}

//updates the EMV tag values or settings
// lenDta = is the length of the value to be saved
// rsp = value of data
// len = length of the message from TMS
// tag = emv tag
static word saveTmsEmvDta(const byte * rsp, word lenDta, int tag) {
    	int ret = 1;
    	word retLen = lenDta;
    	char buf[256];

    	MEM_ZERO(buf);
	
    	switch (tag) {              //save data obtained from tms

       /////////////////////PUBLIC KEYS/////////////////////
	   	case tetEmvKey:
//			ret = mapMove(pubBeg, pubRecNum);
			break;
	      	case tetEmvKeyRid:
	          	retLen = lenDta + 1;
//	          	ret = mapPut(pubRid, rsp, retLen);
		 	memcpy(glTlmTmsPub.Pub[pubRecNum].pubRid, rsp, retLen);
//			hex_dump_char("pubRid", glTlmTmsPub.Pub[pubRecNum].pubRid, retLen);
	          	break;
	      case tetEmvKeyIdx:
//	          	CHECK(lenDta == 1, lblKO);
//	          	ret = mapPutByte(pubIdx, *rsp);          
			glTlmTmsPub.Pub[pubRecNum].pubIdx = *rsp;
//			hex_dump_char("pubIdx", &glTlmTmsPub.Pub[pubRecNum].pubIdx, 1);
			break;
	      case tetEmvKeyDta:
			retLen = lenDta + 1;
//			ret = mapPut(pubkeyData, rsp, retLen);
			memcpy(glTlmTmsPub.Pub[pubRecNum].pubkeyData, rsp, retLen);
//			hex_dump_char("pubkeyData", glTlmTmsPub.Pub[pubRecNum].pubkeyData, retLen);
			break;
	      case tetEmvKeyExp:
//	          	CHECK(lenDta == 1, lblKO);
//	          	ret = mapPutByte(pubExp, *rsp);
			glTlmTmsPub.Pub[pubRecNum].pubExp= *rsp;	
//			hex_dump_char("pubExp", &glTlmTmsPub.Pub[pubRecNum].pubExp, sizeof(uchar));
	          	break;
	      case tetEmvKeyAlg:
//	          	CHECK(lenDta == 1, lblKO);
//	          	ret = mapPutByte(pubAlgo, *rsp);
			glTlmTmsPub.Pub[pubRecNum].pubAlgo= *rsp;
//			hex_dump_char("pubAlgo", &glTlmTmsPub.Pub[pubRecNum].pubAlgo, sizeof(uchar));
	            	pubRecNum += 1;
//	          	ret = mapPutByte(regPubCnt, pubRecNum);
			glTlmTmsPub.ucPubNum=pubRecNum;
//			glTlmReg.regPubCnt = pubRecNum;
//			glSysCtrl.TlmReg.regPubCnt = pubRecNum;
	              break;

		////////////////////TERMINAL SETTINGS/////////////////////
		case tetEmvDefPar:
		case tetEmvDefAprLvl:    //todo: parse following tags and save on terminal settings
		case tetEmvDefTag:       //todo: parse following tags and save on terminal settings
			break;

	          ////////////////////AID DATA/////////////////////         
	      	case tetEmvAidPar:
//	          	ret = mapMove(aidBeg, aidRecNum);
		      	break;
	      	case tetEmvAid:
	          	retLen = lenDta + 1;
//	          	ret = mapPut(emvAid, rsp, retLen);	   
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvAid, rsp, retLen);
//			hex_dump_char("emvAid", glTlmTmsAid.Aid[aidRecNum].emvAid, retLen);
	          	break;
	      	case tetEmvAidPri:       //not used
	          	break;
	      	case tetEmvAppSelIdx:    //not used
	          	break;
	      	case tetEmvAidTag:       //do not save this tags yet
//	          	mapGet(binHi, buf, 1 + lenBinPrefix);
			memcpy(buf, glTlmTmsBin.Bin[binRecNum].binHi, 1 + lenBinPrefix);

//	          	ret = mapPut(emvAcqId, buf, 1 + lenBinPrefix);
			memcpy(glTlmTmsAid.Aid[aidRecNum].emvAcqId, buf, 1 + lenBinPrefix);
//			hex_dump_char("emvAcqId", glTlmTmsAid.Aid[aidRecNum].emvAcqId, 1+lenBinPrefix);
	          	break;
	      	case tetEmvTxnType:
//	          	ret = mapPut(emvAidTxnType, rsp, lenDta);
	          	memcpy(glTlmTmsAid.Aid[aidRecNum].emvAidTxnType, rsp, lenDta);
//			hex_dump_char("emvTxnType", glTlmTmsAid.Aid[aidRecNum].emvAidTxnType, lenDta);
	          	break;
	      	case tetEmvAidTxnTag:
	          	parseTerDta(rsp, lenDta);
	          	aidRecNum += 1;
//	          	MAPPUTBYTE(regAidCnt, aidRecNum, lblKO);
			glTlmTmsAid.ucAidNum = aidRecNum;
//			glTlmReg.regAidCnt = aidRecNum;
//			glSysCtrl.TlmReg.regAidCnt = aidRecNum;
	          break;
	      default:
	          break;
	    }

	return retLen;
}

static int parseTmsTag(const byte * rsp) {  // determines the type of the tag to be processed
    int ret = 0;
    card respDta;

    bin2num(&respDta, rsp, 3);
    switch (respDta) {
      case EMV_KEY:
          ret = tetEmvKey;
          break;
      case EMV_KEY_RID:        //pubRid
          ret = tetEmvKeyRid;
          break;
      case EMV_KEY_INDEX:      //pubIdx
          ret = tetEmvKeyIdx;
          break;
      case EMV_KEY_DATA:       //pubkeyData
          ret = tetEmvKeyDta;
          break;
      case EMV_KEY_EXPONENT:   //pubExp
          ret = tetEmvKeyExp;
          break;
      case EMV_KEY_ALGORITHIM: //pubAlgo
          ret = tetEmvKeyAlg;
          break;
      case EMV_DEFAULT_PARAM:
          ret = tetEmvDefPar;
          break;
      case EMV_DEF_APPROVAL_LEVEL:
          ret = tetEmvDefAprLvl;
          break;
      case EMV_DEF_TAGS:
          ret = tetEmvDefTag;
          break;
      case EMV_AID_PARAM:
          ret = tetEmvAidPar;
          break;
      case EMV_AID:            //emvAid
          ret = tetEmvAid;
          break;
      case EMV_AID_PRIORITY:   //emvAidPrior
          ret = tetEmvAidPri;
          break;
      case EMV_AID_APP_SEL_INDIC:  //emvAidAppSelIn
          ret = tetEmvAppSelIdx;
          break;
      case EMV_AID_TAGS:
          ret = tetEmvAidTag;
          break;
      case EMV_TXN_TYPE:       //emvAidTxnType    
          ret = tetEmvTxnType;
          break;
      case EMV_AID_TXN_TAGS:
          ret = tetEmvAidTxnTag;
          break;
      default:
          ret = 0;
          break;
    }
    return ret;
}

static int parseEmvFile(void) {
    int ret=0, idx = 0,  tmp = 0, lenTmp = 0;
    card lenDta = 0;
		
    while(idx  <  emvFileIdx) {
		lenTmp = 1;
        tmp = parseTmsTag(&emvFile[idx]);
        CHECK(tmp > 0, lblKO);
        idx += 3;                 //move after tms tag size == 3
        
        if(memcmp(&emvFile[idx], "\x81", 1) == 0) {   //tag length is greater than or equal to 128   
            idx += 1;                 //move after tms tag size == 2 byte tag
        bin2num(&lenDta, &emvFile[idx], 1);
        } else if (memcmp(&emvFile[idx], "\x82", 1) == 0) {   //tag length is greater than or equal to 128   
            idx += 1;                 //move after tms tag size == 2 byte tag
	        bin2num(&lenDta, &emvFile[idx], 2);
			lenTmp = 2;
        } else {
	        bin2num(&lenDta, &emvFile[idx], 1);
    	}

        switch (tmp) {          //switch if  tag length will also be saved
          case tetEmvKeyRid:
          case tetEmvAid:
          case tetEmvKeyDta:
              break;
          default:
              idx += lenTmp;           //move after tms tag length
              break;
        }

        lenDta = saveTmsEmvDta(&emvFile[idx], (word)lenDta, tmp);
        CHECK(lenDta > 0, lblKO);

        switch (tmp) {          //for trace data obtained from tag
          case tetEmvKey:
          case tetEmvAidPar:
          case tetEmvDefPar:
          case tetEmvAidTag:
              break;
          default:
              idx += lenDta;
              break;
        }
    }

   memset(emvFile, 0, sizeof(emvFile));
    emvFileIdx = 0;

    return ret;
  lblKO:
    return -1;	
}

//Parsing of EMV chunks
static int parseEmvFleRed(const byte * rsp, word len) {
    	int ret = 0;
    	card offset = 0;
		
    	
	CHECK(len <= lenTmsFleChkSze, lblKO);
//	MAPGETCARD(traTmsEmvFleOff, offset, lblKO);
	offset = glProcInfo.tlmTra.traTmsEmvFleOff;
	offset += (card) len - 1;
//	MAPPUTCARD(traTmsEmvFleOff, offset, lblKO);
	glProcInfo.tlmTra.traTmsEmvFleOff=offset;
	

//	MAPGETBYTE(regAidCnt, aidRecNum, lblKO);
	aidRecNum = glTlmTmsAid.ucAidNum;
//	aidRecNum = glTlmReg.regAidCnt;
//	aidRecNum = glSysCtrl.TlmReg.regAidCnt;
	CHECK(aidRecNum < dimAID, lblKO);
//	MAPGETBYTE(regPubCnt, pubRecNum, lblKO);
	pubRecNum = glTlmTmsPub.ucPubNum;
//	pubRecNum = glTlmReg.regPubCnt;
//	pubRecNum = glSysCtrl.TlmReg.regPubCnt;
	CHECK(pubRecNum < dimPub, lblKO);

	MOV(1);                     // 1st byte of the chunk is not used

    	memcpy(&emvFile[emvFileIdx], rsp, len);
	emvFileIdx += len;

    	return ret;
  lblKO:
    	return -1;	
}

static int parseEmvFleOpn(const byte * rsp, word len)
{
	int ret = 0;
	card fleSze = 0;

	
	MOV(1);                     // not used
	MOV(2);                     // file crc dont know what to do with this

	ret = bin2num(&fleSze, rsp, (byte)len);
	CHK;
//	MAPPUTCARD(traTmsEmvFleSze, fleSze, lblKO);
	glProcInfo.tlmTra.traTmsEmvFleSze = fleSze;

	MOV(len);
	CHECK(len == 0, lblKO);

	//reset aid and pub table before saving new aid and pub details
//	ret = mapReset(aidBeg);
	for(ret=0;ret<dimAID;ret++)
	{
		memset(&glTlmTmsAid.Aid[ret], 0, sizeof(TLM_AID));
	}
//	CHECK(ret > 0, lblKO);
//	ret = mapReset(pubBeg);
	for(ret=0;ret<dimPub;ret++)
	{
		memset(&glTlmTmsPub.Pub[ret], 0, sizeof(TLM_PUB));
	}
//	CHECK(ret > 0, lblKO);

	return ret;
lblKO:
	return -1;
}

static int parseEmvFleDnl(const byte * rsp, word len)
{
	int ret = 0;

	MOV(1);                     // not used

//	ret = mapPut(traTmsEmvFleHdl, rsp, lenFleHdl);
	memcpy(glProcInfo.tlmTra.traTmsEmvFleHdl, rsp, lenFleHdl);

	MOV(len);

	CHECK(len == 0, lblKO);

	return ret;
lblKO:
	return -1;
}

static int parseEmvReq(const byte * rsp, word len)
{
	int ret = 0;

	MOV(2);                     //command still dont know what to do with this

	if(*rsp == 0)               // requested file not on TMS server
	{
		return -80;             // just assigned a negative return value for proper processing, (exception error)
	}
//	ret = mapPut(traTmsEmvFleNme, rsp, len);
	memcpy(glProcInfo.tlmTra.traTmsEmvFleNme, rsp, len);
	
	MOV(len);

	CHECK(len == 0, lblKO);

	return ret;
lblKO:
	return -1;
}

static int saveParseByte(const byte * rsp, byte *data, const char *trc)
{
	*data = *rsp;
	return sizeof(byte);
}

static int saveParseWord(const card * rsp, word *data, const char *trc)
{
	*data = (word)*rsp;
	return sizeof(word);

}

static int saveParseStr(const byte * rsp, uchar *data, word n, const char *trc)
{
	memcpy(data, rsp, n);
	return n;
}

static int saveParseStrTlm(const byte * rsp, word key, word n, const char *trc)
{
	int ret;

	ret = mapPut(key, rsp, n);
	CHK;
	return n;
lblKO:
	return -1;
}


static int parseSkip(const byte * rsp, word n, const char *trc)
{
	return n;
}

static int parsePos(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parsePos");
}

static int parseLength(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseLength");
}

static int parseAcqId(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqID, "parseAcqId");
}

static int parseAcqName(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqName, lenName, "parseAcqName");
}

static int parseTrans_Device(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseTrans_Device");
}

static int parseSettle_Device(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseSettle_Device");
}

static int parseAcqPriTxnPhnSrv(const byte * rsp, word length)
{

	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqPriTxnPhnSrv, lenPhone, "parseAcqPriTxnPhnSrv");

}

static int parseAcqPriStlPhnSrv(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqPriStlPhnSrv, lenPhone, "parseAcqPriStlPhnSrv");
}

/*
static int parseAcqPriTxnSrv(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqPriTxnSrv, length, "parseAcqPriTxnSrv");

}
*/

/*
static int parseAcqPriStlSrv(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqPriStlSrv, length, "parseAcqPriStlSrv");
}
*/

static int parseAcqPriTxnAtt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqPriTxnAtt, "parseacqPriTxnAtt");
}

static int parseAcqPriStlAtt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqPriStlAtt, "parseacqPriStlAtt");
}

static int parseAcqSecTxnPhnSrv(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqSecTxnPhnSrv, lenPhone, "parseAcqSecTxnPhnSrv");
}

static int parseAcqSecStlPhnSrv(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqSecStlPhnSrv, lenPhone, "parseAcqSecStlPhnSrv");
}

/*
static int parseAcqSecTxnSrv(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqSecTxnSrv, length, "parseAcqSecTxnSrv");
}
*/

/*
static int parseAcqSecStlSrv(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqSecStlSrv, length, "parseAcqSecStlSrv");
}
*/

static int parseAcqSecTxnAtt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqSecTxnAtt, "parseacqSecTxnAtt");
}

static int parseAcqSecStlAtt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqSecStlAtt, "parseacqSecStlAtt");
}

static int parseAcqTxnMdmMod(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAcqTxnMdmMod");
}

static int parseAcqStlMdmMod(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAcqStlMdmMod");
}

static int parseAcqTxnProt(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAcqStlProt");
}

static int parseAcqStlProt(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAcqStlProt");
}

static int parseAcqTxnAddOfLargeRxBuffer(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqTxnAddOfLargeRxBuffer");
}

static int parseAcqStlAddOfLargeRxBuffer(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqStlAddOfLargeRxBuffer");
}

static int parseAcqTxnAddOfLargeTxBuffer(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqTxnAddOfLargeTxBuffer");
}

static int parseAcqStlAddOfLargeTxBuffer(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqStlAddOfLargeTxBuffer");
}

static int parseAcqTxnRxBufferSize(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqTxnRxBufferSize");
}

static int parseAcqStlRxBufferSize(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqStlRxBufferSize");
}

static int parseAcqTxnTxBufferSize(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqTxnTxBufferSize");
}

static int parseAcqStlTxBufferSize(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqStlTxBufferSize");
}

static int parseAcqTxnDialOpt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqTxnDialOpt, "parseAcqTxnDialOpt");
}

static int parseAcqStlDialOpt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqStlDialOpt, "parseAcqStlDialOpt");
}

static int parseTxnReserved(const byte * rsp, word length)
{
	return parseSkip(rsp, 7, "parseTxnReserved");
}

static int parseStlReserved(const byte * rsp, word length)
{
	return parseSkip(rsp, 7, "parseStlReserved");
}

static int parseAcqTxnRxTO(const byte * rsp, word length)
{
	card val;

	bin2num(&val, rsp, 2);
	return saveParseWord(&val, &glTlmTmsAcq.Acq[acqRecNum].acqTxnRxTO, "parseAcqTxnRxTO");
}

static int parseAcqStlRxTO(const byte * rsp, word length)
{
	card val;

	bin2num(&val, rsp, 2);
	return saveParseWord(&val, &glTlmTmsAcq.Acq[acqRecNum].acqStlRxTO, "parseAcqStlRxTO");
}

static int parseAcqTxnICTO(const byte * rsp, word length)
{
	card val;

	bin2num(&val, rsp, 2);
	return saveParseWord(&val, &glTlmTmsAcq.Acq[acqRecNum].acqTxnICTO, "parseAcqTxnICTO");
}

static int parseAcqStlICTO(const byte * rsp, word length)
{
	card val;

	bin2num(&val, rsp, 2);
	return saveParseWord(&val, &glTlmTmsAcq.Acq[acqRecNum].acqStlICTO, "parseAcqStlICTO");
}

static int parseAcqTxnOpt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqTxnOpt, "parseAcqTxnOpt");
}

static int parseAcqStlOpt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqStlOpt, "parseAcqStlOpt");
}

static int parseAcqPriTxnIPSrv(const byte * rsp, word length)
{
	char buf[lenSrv + 1];
	int ret;
	uchar buffer[512];

	MEM_ZERO(buffer);
	memcpy(buffer, rsp, length);
	
	ret = ConvertIpPortSrv(buffer, glTlmTmsAcq.Acq[acqRecNum].acqPriTxnIP, glTlmTmsAcq.Acq[acqRecNum].acqPriTxnPort, length);
	CHK;
	
	memset(buf, 0, sizeof(buf));
	ret = ConvIpPrtSrv(rsp, buf, length);
	CHK;

	saveParseStr((byte *) buf, glTlmTmsAcq.Acq[acqRecNum].acqPriTxnIPSrv, lenSrv, "parseAcqPriTxnIPSrv");

	return ret;

lblKO:
	return -1;
}

static int parseAcqPriStlIPSv(const byte * rsp, word length)
{
	char buf[lenSrv + 1];
	int ret;
	uchar buffer[512];

	MEM_ZERO(buffer);
	memcpy(buffer, rsp, length);
	
	ret = ConvertIpPortSrv(buffer, glTlmTmsAcq.Acq[acqRecNum].acqPriStlIP, glTlmTmsAcq.Acq[acqRecNum].acqPriStlPort, length);
	CHK;

	memset(buf, 0, sizeof(buf));
	ret = ConvIpPrtSrv(rsp, buf, length);
	CHK;

	saveParseStr((byte *) buf, glTlmTmsAcq.Acq[acqRecNum].acqPriStlIPSrv, lenSrv, "parseAcqPriStlIPSrv");

	return ret;

lblKO:
	return -1;
}

static int parseAcqSecTxnIPSrv(const byte * rsp, word length)
{
	char buf[lenSrv + 1];
	int ret;
	uchar buffer[512];

	MEM_ZERO(buffer);
	memcpy(buffer, rsp, length);
	
	ret = ConvertIpPortSrv(buffer, glTlmTmsAcq.Acq[acqRecNum].acqSecTxnIP, glTlmTmsAcq.Acq[acqRecNum].acqSecTxnPort, length);
	CHK;

	memset(buf, 0, sizeof(buf));
	ret = ConvIpPrtSrv(rsp, buf, length);
	CHK;

	saveParseStr((byte *) buf, glTlmTmsAcq.Acq[acqRecNum].acqSecTxnIPSrv, lenSrv, "parseAcqSecTxnIPSrv");

	return ret;

lblKO:
	return -1;
}

static int parseAcqSecStlIPSrv(const byte * rsp, word length)
{
	char buf[lenSrv + 1];
	int ret;
	uchar buffer[512];

	MEM_ZERO(buffer);
	memcpy(buffer, rsp, length);
	
	ret = ConvertIpPortSrv(buffer, glTlmTmsAcq.Acq[acqRecNum].acqSecStlIP, glTlmTmsAcq.Acq[acqRecNum].acqSecStlPort, length);
	CHK;
	memset(buf, 0, sizeof(buf));
	ret = ConvIpPrtSrv(rsp, buf, length);
	CHK;

	saveParseStr((byte *) buf, glTlmTmsAcq.Acq[acqRecNum].acqSecStlIPSrv, lenSrv, "parseAcqSecStlIPSrv");

	return ret;

lblKO:
	return -1;
}

static int parseAcqTxnConTO(const byte * rsp, word length)
{
	card val;

	bin2num(&val, rsp, 2);
	return saveParseWord(&val, &glTlmTmsAcq.Acq[acqRecNum].acqTxnConTO, "parseAcqTxnConTO");
}

static int parseAcqStlConTO(const byte * rsp, word length)
{
	card val;

	bin2num(&val, rsp, 2);

	return saveParseWord(&val, &glTlmTmsAcq.Acq[acqRecNum].acqStlConTO, "parseAcqStlConTO");
}

static int parseAcqOpt1(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAcqOpt1");
}

static int parseAcqOpt2(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsAcq.Acq[acqRecNum].acqOpt2, "parseAcqOpt2");
}

static int parseAcqOpt3(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAcqOpt3");
}

static int parseAcqOpt4(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAcqOpt4");
}

static int parseAcqNii(const byte * rsp, word length)
{
	card numNii;
	char buf[LEN_NII + 1];
	byte nii[LEN_NII + 1];
	int ret;

	memset(nii, 0x00, sizeof(nii));
	memset(buf, 0x00, sizeof(buf));
	memcpy(nii, rsp, LEN_NII);
	bin2hex(buf, nii, LEN_NII);
	dec2num(&numNii, buf, 0);
	num2dec(buf, numNii, LEN_NII);

	ret = saveParseStr((byte *) buf, glTlmTmsAcq.Acq[acqRecNum].acqNII, LEN_NII, "parseAcqNii");
	CHK;
	ret = sizeof(word);         //nii length
	return ret;

lblKO:
	return -1;
}

static int parseAcqTID(const byte * rsp, word length)
{
    char TID[lenTID + 1];
    int retVal;

   
    //return saveParseStr(rsp, acqTID, lenTID, "parseAcqTID");
    MEM_ZERO(TID);
	
    retVal = saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqTID, lenTID, "parseAcqTID");
//    MAPGET(acqTID, TID, lblKO);
	memcpy(TID, glTlmTmsAcq.Acq[acqRecNum].acqTID, lenTID);
    	if(memcmp(TID, "00000000", 8) == 0)
	{
        retVal = -1;
//        MAPPUTSTR(traRspCod, "DE", lblKO);
	 memcpy(glTMSTELIUMRecv.szBit39, "DE", lenBit39);
    	}
   
    return retVal;
}

static int parseAcqMID(const byte * rsp, word length)
{
    int retVal;
    char MID[lenMid + 1];
    //return saveParseStr(rsp, acqMID, lenMid, "parseAcqTID");                //ori

    MEM_ZERO(MID);
	
    retVal = saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqMID, lenMid, "parseAcqTID");
//    MAPGET(acqMID, MID, lblKO);
	memcpy(MID, glTlmTmsAcq.Acq[acqRecNum].acqMID, lenMid);
    if(memcmp(MID, "000000000000000", 15) == 0){
        retVal = -1;
//        MAPPUTSTR(traRspCod, "DF", lblKO);
	memcpy(glTMSTELIUMRecv.szBit39, "DF", lenBit39);
    }

    return retVal;
}

static int parseAcqCurBat(const byte * rsp, word length)
{
	char bat[lenInvNum + 1];    //batch number
	int ret;
	ulong ulBatNum=0;

	memset(bat, 0x00, sizeof(bat));
	bin2hex(bat, rsp, 3);

	ulBatNum = PubAsc2Long(bat, lenBatNum, NULL);
	if(ulBatNum==0)
	{
		ulBatNum++;
		sprintf(bat, "%06lu", ulBatNum);
	}
	
	//glTlmTmsAcq.Acq[acqRecNum].ulCurBatchNo = PubAsc2Long(bat, lenBatNum, NULL);
	glTlmTmsAcq.Acq[acqRecNum].ulCurBatchNo = ulBatNum;
	ret =  saveParseStr((byte *) bat, glTlmTmsAcq.Acq[acqRecNum].acqCurBat, sizeof(bat) - 1, "parseAcqCurBat");
	CHK;
	ret = 3;                    //batch num size
	return ret;

lblKO:
	return -1;
}

static int parseAcqNxtBat(const byte * rsp, word length)
{
	return parseSkip(rsp, 3, "parseAcqNxtBat");
}

static int parseAcqStlTme(const byte * rsp, word length)
{
	return parseSkip(rsp, 6, "parseAcqStlTme");
}

static int parseAcqStlDte(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(word), "parseAcqStlDte");
}

static int parseAcqEncPinKey(const byte * rsp, word length)
{   //TODO: enc pin key should be processed here already
	//return parseSkip(rsp, 8, "parseAcqEncPinKey");
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqMasterKey1, lenMKey, "parseAcqEncPinKey");//mcy add for get Master Key from TMS, 20100818
}

static int parseAcqEncMacKey(const byte * rsp, word length)
{   //TODO: enc mac key should be processed here already
	//return parseSkip(rsp, 8, "parseAcqEncMacKey");
	return saveParseStr(rsp, glTlmTmsAcq.Acq[acqRecNum].acqMasterKey2, lenMKey, "parseAcqEncPinKey");//mcy add for get Master Key from TMS, 20100818
}

static int parseAcqDunno(const byte * rsp, word length)
{
	return parseSkip(rsp, 23, "parseAcqDunno");
}

static int parseAcqEMVRespCodeList(const byte * rsp, word length)
{
	return parseSkip(rsp, 18, "parseAcqEMVRespCodeList");
}

static int parseAcqTbl(const byte * rsp, word len)
{
	int ret = 0;
	char buf[lenSrv + 1];
	byte recNo;
	char chn[lenChn + 1];
	word length = 1;
	uchar commtype;

	MEM_ZERO(buf);
	MEM_ZERO(chn);

/*	MAPGET(appChn, chn, lblKO);
	
	switch (chn[0])
	{
		case 'T':
		case 'G':
			MAPPUTBYTE(appConnType, 2, lblKO);
			break;
		case 'R':
			MAPPUTBYTE(appConnType, 3, lblKO);
			break;
		case 'M':
		default:
			MAPPUTBYTE(appConnType, 1, lblKO);
			break;
	}
*/	
	commtype = glTlmSysParam.stTxnCommCfg.ucCommType;
	
//	MAPGETBYTE(regAcqCnt, recNo, lblKO);
	recNo = glTlmTmsAcq.ucAcqNum;
//	recNo = glTlmReg.regAcqCnt;
//	recNo = glSysCtrl.TlmReg.regAcqCnt;
	
	CHECK(recNo < dimAcq, lblKO);

	if(recNo == 0)
	{            // make sure that we only reset the table once
//		ret = mapReset(acqBeg); //reset table first before saving new acw details
		for(ret=0;ret<dimAcq;ret++)
		{
			memset(&glTlmTmsAcq.Acq[ret], 0, sizeof(TLM_ACQ));
		}
		
		CHECK(ret > 0, lblKO);
	}
//	mapMove(acqBeg, recNo);
	acqRecNum=recNo;

	PARSE(parseLength);
	PARSE(parsePos);
	PARSE(parseAcqId);
	PARSE(parseAcqName);
	PARSE(parseTrans_Device);
	PARSE(parseSettle_Device);

/*
	//Start: Transaction Part
	if(commtype==CT_MODEM)
	{
		ret = parseAcqPriTxnSrv(rsp, lenPhone);
		CHK;
	}
*/
	PARSE(parseAcqPriTxnPhnSrv);
	PARSE(parseAcqPriTxnAtt);
/*
	if(commtype==CT_MODEM)
	{
		ret = parseAcqSecTxnSrv(rsp, lenPhone);
		CHK;
	}
*/
	PARSE(parseAcqSecTxnPhnSrv);
	PARSE(parseAcqSecTxnAtt);

	PARSE(parseAcqTxnMdmMod);
	PARSE(parseAcqTxnProt);
	PARSE(parseAcqTxnAddOfLargeRxBuffer);
	PARSE(parseAcqTxnAddOfLargeTxBuffer);
	PARSE(parseAcqTxnRxBufferSize);
	PARSE(parseAcqTxnTxBufferSize);
	PARSE(parseAcqTxnDialOpt);
	PARSE(parseTxnReserved);
	PARSE(parseAcqTxnRxTO);
	PARSE(parseAcqTxnICTO);
	PARSE(parseAcqTxnOpt);

/*
	if((commtype == CT_TCPIP)||(commtype == CT_WIFI)||(commtype == CT_GPRS))
	{
		memset(buf, 0, sizeof(buf));
		ret = ConvIpPrtSrv(rsp, buf, len);
		CHK;

		ret = parseAcqPriTxnSrv((byte *) buf, lenSrv);
		CHK;
	}
*/	
	length = len;
	PARSE(parseAcqPriTxnIPSrv);

/*
	if((commtype == CT_TCPIP)||(commtype == CT_WIFI)||(commtype == CT_GPRS))
	{
		memset(buf, 0, sizeof(buf));
		ret = ConvIpPrtSrv(rsp, buf, len);
		CHK;

		ret = parseAcqSecTxnSrv((byte *) buf, lenSrv);
		CHK;
	}
*/
	length = len;
	PARSE(parseAcqSecTxnIPSrv);

	PARSE(parseAcqTxnConTO);
	//End: Transaction Part

	//Start: Settlement Part
/*	
	if(commtype==CT_MODEM)
	{
		ret = parseAcqPriStlSrv(rsp, lenPhone);
		CHK;
	}
*/	
	PARSE(parseAcqPriStlPhnSrv);
	PARSE(parseAcqPriStlAtt);
/*
	if(commtype==CT_MODEM)
	{
		ret = parseAcqSecStlSrv(rsp, lenPhone);
		CHK;
	}
*/	
	PARSE(parseAcqSecStlPhnSrv);
	PARSE(parseAcqSecStlAtt);

	PARSE(parseAcqStlMdmMod);
	PARSE(parseAcqStlProt);
	PARSE(parseAcqStlAddOfLargeRxBuffer);
	PARSE(parseAcqStlAddOfLargeTxBuffer);
	PARSE(parseAcqStlRxBufferSize);
	PARSE(parseAcqStlTxBufferSize);
	PARSE(parseAcqStlDialOpt);
	PARSE(parseStlReserved);
	PARSE(parseAcqStlRxTO);
	PARSE(parseAcqStlICTO);
	PARSE(parseAcqStlOpt);

/*
	if((commtype == CT_TCPIP)||(commtype == CT_WIFI)||(commtype == CT_GPRS))
	{
		memset(buf, 0, sizeof(buf));
		ret = ConvIpPrtSrv(rsp, buf, len);
		CHK;

		ret = parseAcqPriStlSrv((byte *) buf, lenSrv);
		CHK;
	}
*/	
	PARSE(parseAcqPriStlIPSv);
/*
	if((commtype == CT_TCPIP)||(commtype == CT_WIFI)||(commtype == CT_GPRS))
	{
		memset(buf, 0, sizeof(buf));
		ret = ConvIpPrtSrv(rsp, buf, len);
		CHK;

		ret = parseAcqSecStlSrv((byte *) buf, lenSrv);
		CHK;
	}
*/
	PARSE(parseAcqSecStlIPSrv);

	PARSE(parseAcqStlConTO);
	//End: Settlement Part

	PARSE(parseAcqOpt1);
	PARSE(parseAcqOpt2);
	PARSE(parseAcqOpt3);
	PARSE(parseAcqOpt4);
	PARSE(parseAcqNii);
	PARSE(parseAcqTID);
	PARSE(parseAcqMID);
	PARSE(parseAcqCurBat);
	PARSE(parseAcqNxtBat);
	PARSE(parseAcqStlTme);
	PARSE(parseAcqStlDte);
	PARSE(parseAcqEncPinKey);
	//parseAcqEncPinKey(rsp, 16);
	PARSE(parseAcqEncMacKey);
	PARSE(parseAcqDunno);
	PARSE(parseAcqEMVRespCodeList);

	glTlmTmsAcq.Acq[recNo].ucIndex = recNo;
	glSysCtrl.sAcqStatus[recNo] = S_USE;
	

	CHECK(len == 0, lblKO);

	recNo += 1;
//	MAPPUTBYTE(regAcqCnt, recNo, lblKO);
	glTlmTmsAcq.ucAcqNum = recNo;
//	glTlmReg.regAcqCnt = recNo;
//	glSysCtrl.TlmReg.regAcqCnt = recNo;

	return ret;
lblKO:
	return -1;
}

static int parseIssId(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsIss.Iss[issRecNum].issID, "parseIssId");
}

static int parseIssRefNum(const byte * rsp, word length)
{
	char buf[lenIssRef + 1];
	char dec[lenIssRef + 1];
	char name[lenIssRef+1];	
	int ret;
	
	MEM_ZERO(buf);
	MEM_ZERO(dec);
	MEM_ZERO(name);
	
	memcpy(buf, rsp, lenIssRef);
	dec2bin(dec, buf, lenIssRef);
	//hex_dump_char("refNum", dec, lenIssRef);
	
	ret = saveParseStr((byte *) dec, glTlmTmsIss.Iss[issRecNum].issRef, lenIssRef, "parseIssRefNum");
	CHK;

	ret = 12;                   //size of issuer ref num
	return ret;

lblKO:
	return -1;
}
#if 0
static int parseIssOpt1(const byte * rsp, word length)
{
	return saveParseByte(rsp, issOpt1, "parseIssOpt1");
}

static int parseIssOpt2(const byte * rsp, word length)
{
	return saveParseByte(rsp, issOpt2, "parseIssOpt2");
}

static int parseIssOpt3(const byte * rsp, word length)
{
	return saveParseByte(rsp, issOpt3, "parseIssOpt3");
}

static int parseIssOpt4(const byte * rsp, word length)
{
	return saveParseByte(rsp, issOpt4, "parseIssOpt4");
}

static int parseIssOpt5(const byte * rsp, word length)
{
	return saveParseByte(rsp, issOpt5, "parseIssOpt5");
}

#else
static int parseIssOpt1(const byte * rsp, word length)
{
       int ret=0;
       byte opt1;
      
       ret =  saveParseByte(rsp, &glTlmTmsIss.Iss[issRecNum].issOpt1, "parseIssOpt1");
      
//    MAPGETBYTE( issOpt1, opt1, lblKO);
	opt1=glTlmTmsIss.Iss[issRecNum].issOpt1;

       if(opt1 & 0x01){
//        MAPPUTBYTE(issAccSelect, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issAccSelect = 1;
           }
    if(opt1 & 0x02){
//        MAPPUTBYTE(issEMVFallback, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issEMVFallback = 1;
        }
    if(opt1 & 0x04){
//        MAPPUTBYTE(issManual, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issManual = 1;
//		hex_dump_char("issManual","test",4);
        }
    if(opt1 & 0x08){
//        MAPPUTBYTE(issExpiryDatReq, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issExpiryDate =1;
        }
    if(opt1 & 0x10){
//        MAPPUTBYTE(issOffline, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issOffline = 1;
//		hex_dump_char("issOffline","test",4);
        }
    if(opt1 & 0x20){
//        MAPPUTBYTE(issRefferalDial, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issRefferalDial = 1;
        }
    if(opt1 & 0x40){
//        MAPPUTBYTE(issProductCod, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issProductCod = 1;
        }
    if(opt1 & 0x80){
//        MAPPUTBYTE(issAdjust, 1, lblKO);   
		glTlmTmsIss.Iss[issRecNum].issAdjust = 1;
//		hex_dump_char("issAdjust","test",4);
        }
   
        return ret;   
}


static int parseIssOpt2(const byte * rsp, word length)
{
        int ret=0;
       byte opt2;
      
       ret =  saveParseByte(rsp, &glTlmTmsIss.Iss[issRecNum].issOpt2, "parseIssOpt1");
//    MAPGETBYTE(issOpt2, opt2,  lblKO);
	opt2 = glTlmTmsIss.Iss[issRecNum].issOpt2;

       if(opt2 & 0x01){
//        MAPPUTBYTE(issDoModPAN, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issDoModPAN = 1;
           }
    if(opt2 & 0x02){
//        MAPPUTBYTE(issROCNumReq, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issROCNumReq = 1;
        }
    if(opt2 & 0x04){
//        MAPPUTBYTE(issPrintReceipt, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issPrintReceipt = 1;
        }
    if(opt2 & 0x08){
//        MAPPUTBYTE(issCapTxns, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issCapTxns = 1;
        }
    if(opt2 & 0x10){
//        MAPPUTBYTE(issCheckExpOff, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issCheckExpOff = 1;
        }
    if(opt2 & 0x40){
//        MAPPUTBYTE(issRefund, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issRefund = 1;
//		hex_dump_char("issRefund","test",4);
        }
    if(opt2 & 0x80){
//        MAPPUTBYTE(issPreauth, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issPreauth = 1;
//		hex_dump_char("issPreauth","test",4);
        }
	
        return ret;
   
}



static int parseIssOpt3(const byte * rsp, word length)
{
    int ret=0;
       byte opt3;
      
       ret =  saveParseByte(rsp, &glTlmTmsIss.Iss[issRecNum].issOpt3, "parseIssOpt1");
//    MAPGETBYTE(issOpt3, opt3,  lblKO);
	opt3 = glTlmTmsIss.Iss[issRecNum].issOpt3;

       if(opt3 & 0x01){
//        MAPPUTBYTE(issVoid, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issVoid=1;
//		hex_dump_char("issVoid","test",4);
		}
    if(opt3 & 0x02){
//        MAPPUTBYTE(issAdditionPrompts, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issAdditionPrompts = 1;
        }
    if(opt3 & 0x10){
//        MAPPUTBYTE(issCheckExpAll, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issCheckExpAll = 1;
        }
    if(opt3 & 0x40){
//        MAPPUTBYTE(issCashOut, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issCashOut = 1;
//		hex_dump_char("issCashOut","test",4);
        }
    if(opt3 & 0x80){
//        MAPPUTBYTE(issTrack1, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issTrack1;
        }   
   
        return ret;
}



static int parseIssOpt4(const byte * rsp, word length)
{
    int ret=0;
       byte opt4;
      
       ret =  saveParseByte(rsp, &glTlmTmsIss.Iss[issRecNum].issOpt4, "parseIssOpt1");
//    MAPGETBYTE(issOpt4, opt4,  lblKO);
	opt4 = glTlmTmsIss.Iss[issRecNum].issOpt4;

       if(opt4 & 0x01){
//        MAPPUTBYTE(issBalance, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issBalance = 1;
//		hex_dump_char("issBalance","test",4);
           }
    if(opt4 & 0x02){
//        MAPPUTBYTE(issErrorReceipt, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issErrorReceipt = 1;
        }
    if(opt4 & 0x04){
//        MAPPUTBYTE(issOfflineROC, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issOfflineROC = 1;
        }
   
        return ret;
}



static int parseIssOpt5(const byte * rsp, word length)
{
    int ret=0;
       byte opt5;
      
       ret =  saveParseByte(rsp, &glTlmTmsIss.Iss[issRecNum].issOpt5, "parseIssOpt1");
//    MAPGETBYTE(issOpt5, opt5,  lblKO);
	opt5 = glTlmTmsIss.Iss[issRecNum].issOpt5;

       if(opt5 & 0x10){
//        MAPPUTBYTE(issExtServiceCod, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issExtServiceCod = 1;
           }
    if(opt5 & 0x20){
//        MAPPUTBYTE(issFullPAN, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issFullPAN =1;
        }
    if(opt5 & 0x40){
//        MAPPUTBYTE(issExpiryDate, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issExpiryDate = 1;
        }
    if(opt5 & 0x80){
//        MAPPUTBYTE(issManEntryCVV, 1, lblKO);
		glTlmTmsIss.Iss[issRecNum].issManEntryCVV = 1;
        }
   
        return ret;
}

#endif
static int parseIssDefAcct(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsIss.Iss[issRecNum].issDefAcct, "parseIssDefAcct");
}

static int parseIssFlrLimit(const byte * rsp, word length)
{
	card flrlim;
	char buffer[5];
	int ret;

	memset(buffer, 0x00, sizeof(buffer));
	bin2hex(buffer, rsp, 2);
	dec2num(&flrlim, buffer, 4);
	ret = saveParseWord(&flrlim, &glTlmTmsIss.Iss[issRecNum].issFloorLimit, "parseIssFlrLimit");
	CHK;
	ret = sizeof(word);         //size of  floor limit
	return ret;

lblKO:
	return -1;
}

static int parseIssReAuthMargin(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseIssReAuthMargin");
}

static int parseIssPinOpt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsIss.Iss[issRecNum].issPinOpt, "parseIssPinOpt");
}

static int parseIssuer(const byte * rsp, word * rsplen)
{
	int ret = 0;
	word len, length = 1;

	len = *rsplen;

	PARSE(parseLength);
	PARSE(parsePos);
	PARSE(parseIssId);
	PARSE(parseIssRefNum);
	PARSE(parseIssOpt1);
	PARSE(parseIssOpt2);
	PARSE(parseIssOpt3);
	PARSE(parseIssOpt4);
	PARSE(parseIssDefAcct);
	PARSE(parseIssFlrLimit);
	PARSE(parseIssReAuthMargin);
	PARSE(parseIssOpt5);
	PARSE(parseIssPinOpt);

	*rsplen = len;

	return ret;
lblKO:
	return -1;
}

static int parseIssTbl(const byte * rsp, word len)
{
	int ret = 0;
	byte recNo;

	
//	ret = mapReset(issBeg);     //reset issuer table first before saving new issuer details
//	CHECK(ret > 0, lblKO);
	for(ret = 0; ret<dimIss;ret++)
	{
		memset(&glTlmTmsIss.Iss[ret], 0, sizeof(TLM_ISS));
	}

	//lblIssParse:                 // bin records can be multiple inside one chunk of response to tms with fixed length of 26
	while((len > 0) && (!(len % lenTmsIssDatSze)))
	{    //24 fixed card range length plus 2 bytes for the length
		recNo = glTlmTmsIss.ucIssNum;
//		recNo = glTlmReg.regIssCnt;
//		recNo = glSysCtrl.TlmReg.regIssCnt;
		CHECK(recNo < dimIss, lblKO);

//		ret = mapMove(issBeg, recNo);
		issRecNum = recNo;
//		CHK;
		ret = parseIssuer(rsp, &len);
		CHK;
		rsp += lenTmsIssDatSze;

		recNo++;
//		MAPPUTBYTE(regIssCnt, recNo, lblKO);
		glTlmTmsIss.ucIssNum = recNo;
//		glTlmReg.regIssCnt = recNo;
//		glSysCtrl.TlmReg.regIssCnt = recNo;
	}
	CHECK(len == 0, lblKO);

	recNo += 1;
//	MAPPUTBYTE(regIssCnt, recNo, lblKO);
	glTlmTmsIss.ucIssNum = recNo;
//	glTlmReg.regIssCnt = recNo;
//	glSysCtrl.TlmReg.regIssCnt = recNo;

	return ret;
lblKO:
	return -1;
}

static int parseBinRangeName(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsBin.Bin[binRecNum].binName, lenName, "parseBinRangeName");
}

static int parseBinRangeId(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsBin.Bin[binRecNum].binID, "parseBinRangeId");
}

static int parseBinRangeLowPan(const byte * rsp, word length)
{
	char buf[lenBinPrefix + 2];
	int ret;

	memset(buf, 0x00, sizeof(buf));
	buf[0] = lenBinPrefix;
	memcpy(&buf[1], rsp, lenBinPrefix);

	ret = saveParseStr((byte *) buf, glTlmTmsBin.Bin[binRecNum].binLo, 1 + lenBinPrefix, "parseBinRangeLowPan");
	CHK;
	ret = lenBinPrefix;
	return ret;

lblKO:
	return -1;
}

static int parseBinRangeHighPan(const byte * rsp, word length)
{
	char buf[lenBinPrefix + 2];
	int ret;

	memset(buf, 0, sizeof(buf));
	buf[0] = lenBinPrefix;
	memcpy(&buf[1], rsp, lenBinPrefix);

	ret = saveParseStr((byte *) buf, glTlmTmsBin.Bin[binRecNum].binHi, 1 + lenBinPrefix, "parseBinRangeHighPan");
	CHK;
	ret = lenBinPrefix;
	return ret;

lblKO:
	return -1;
}

static int parseBinRangeCrIssId(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsBin.Bin[binRecNum].binCrIssID, "parseBinRangeCrIssId");
}

static int parseBinRangeCrAcqId(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsBin.Bin[binRecNum].binCrAcqID, "parseBinRangeCrAcqId");
}

static int parseBinRangePanLength(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsBin.Bin[binRecNum].binPanLen, "parseBinRangePanLength");
}

static int parseBinRangeDbIssId(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsBin.Bin[binRecNum].binDbIssID, "parseBinRangeDbIssId");
}

static int parseBinRangeDbAcqId(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsBin.Bin[binRecNum].binDbAcqID, "parseBinRangeDbAcqId");
}

static int parseBin(const byte * rsp, word * rsplen)
{
	int ret = 0;
//	char buffer[24 + 1];
	word len, length = 1;
//	byte binLogo;
//	byte Bin[lenBinPrefix + 1];

//	MEM_ZERO(Bin);
	
	len = *rsplen;

	PARSE(parseLength);
	PARSE(parseBinRangeName);
	PARSE(parseBinRangeId);
	PARSE(parseBinRangeLowPan);

/*
	memset(buffer, 0, sizeof(buffer));
	MAPGET(binLo, Bin, lblKO);
	memcpy(Bin, glTlmTmsBin.Bin[binRecNum].binLo, lenBinPrefix);
	bin2hex(buffer, &Bin[1], 5);

	binLogo = valBinLogoAssign(buffer);

	MAPPUTBYTE(binLogoID, binLogo, lblKO);  //binLogoID :  Logo ID
	glTlmTmsBin.Bin[binRecNum].binLogoID=binLogo;
*/
	PARSE(parseBinRangeHighPan);
	PARSE(parseBinRangeCrIssId);
	PARSE(parseBinRangeCrAcqId);
	PARSE(parseBinRangePanLength);
	PARSE(parseBinRangeDbIssId);
	PARSE(parseBinRangeDbAcqId);

	*rsplen = len;

	return ret;
lblKO:
	return -1;
}

static int parseBinTbl(const byte * rsp, word len)
{
	int ret = 0;
	byte recNo;
	byte BinReq = 0;

	
//	ret = mapGetByte(traBinReq, BinReq);
	BinReq = glProcInfo.tlmTra.traBinReq;
//	CHK;
	if(BinReq == 0) {
	//	ret = mapReset(binBeg);     //reset bin first before saving new bin details
	//	CHECK(ret > 0, lblKO);
		for(ret=0; ret<dimBin;ret++)
		{
			memset(&glTlmTmsBin.Bin[ret], 0, sizeof(TLM_BIN));
		}
	}
	//lblBinParse:                 // bin records can be multiple inside one chunk of response to tms with fixed length of 26
	while((len > 0) && (!(len % lenTmsBinDatSze)))
	{    //26 fixed card range lentgth plus 2 bytes for the length
//		MAPGETBYTE(regBinCnt, recNo, lblKO);
		recNo = glTlmTmsBin.ucBinNum;
//		recNo = glTlmReg.regBinCnt;
//		recNo = glSysCtrl.TlmReg.regBinCnt;
		CHECK(recNo < dimBin, lblKO);
//		mapMove(binBeg, recNo);
		binRecNum=recNo;

//		hex_dump_char("parseBinTbl", rsp, len);
		ret = parseBin(rsp, &len);
		CHK;
		rsp += lenTmsBinDatSze;

		recNo++;
//		MAPPUTBYTE(regBinCnt, recNo, lblKO);
		glTlmTmsBin.ucBinNum = recNo;
//		glTlmReg.regBinCnt = recNo;
//		glSysCtrl.TlmReg.regBinCnt = recNo;
	}
	CHECK(len == 0, lblKO);
	
//	MAPPUTBYTE(traBinReq, 1, lblKO);
	glProcInfo.tlmTra.traBinReq = 1;
	
	return ret;
lblKO:
	return -1;
}

static int parseAppDefTra(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsCon.appDefTra, "parseAppDefTra");
}

static int parseAppAmtFmt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsCon.appAmtFmt, "parseAppAmtFmt");
}

static int parseAppDateTime(const byte * rsp, word length)
{
	return parseSkip(rsp, 6, "parseAppDateTime");
}

static int parseAppAdmPwd(const byte * rsp, word length)
{
	char buf[lenPwd + 1];
	int ret;

	memset(buf, 0, sizeof(buf));
	bin2hex(buf, rsp, lenPwd / 2);
	PubBcd2Asc0((uchar*)rsp,  2, glTlmSysParam.sPassword[PWD_TERM]);
	PubBcd2Asc0((uchar*)rsp,  2, glTlmSysParam.sPassword[PWD_MERCHANT]);
	PubBcd2Asc0((uchar*)rsp,  2, glTlmSysParam.sPassword[PWD_ADJUST]);
	ret = saveParseStr((byte *) buf, glTlmTmsCon.appAdmPwd, lenPwd, "parseAppAdmPwd");
	CHK;
	ret = lenPwd / 2;
	return ret;
lblKO:
	return -1;
}

static int parseAppHelpDesk(const byte * rsp, word length)
{
	char rspHex[24+1];
	char VendorId[8+1];
	char AcqId[3+1];
	char exponent[6+1];
	char NiiMntr[1+lenNII];
	
	MEM_ZERO(rspHex);
	MEM_ZERO(VendorId);
	MEM_ZERO(AcqId);
	MEM_ZERO(exponent);
	MEM_ZERO(NiiMntr);
	
	bin2hex(rspHex, rsp, 12);
	//`hex_dump_char("helpdesk",rsp,12);
	fmtSbs(VendorId, rspHex, 0, 8);
	fmtSbs(AcqId, rspHex, 8, 3);
	fmtSbs(exponent, rspHex, 11, 6);
	//hex2bin(exponent, rspHex + 11, 6);
//	MAPPUTSTR(appVendorID, VendorId, lblKO);
	memcpy(glTlmTmsCon.appVendorID, VendorId, strlen(VendorId));
	
//	MAPPUTSTR(appLtmkAcqID, AcqId, lblKO);
	memcpy(glTlmTmsCon.appLtmkAcqID, AcqId, strlen(AcqId));

//	MAPPUTSTR(appExponent, exponent, lblKO);
	memcpy(glTlmTmsCon.appExponent, exponent, strlen(exponent));

	fmtSbs(NiiMntr, rspHex, 17, 3);
//	MAPPUTSTR(appNIIMntr, NiiMntr, lblKO);
	memcpy(glTlmTmsCon.appNIIMntr, NiiMntr, strlen(NiiMntr));
	return 12;
	//return parseSkip(rsp, 12, "parseAppHelpDesk"); //ori
	
}

static int parseAppTermOpt1(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsCon.appTermOpt1, "parseAppTermOpt1");
}

static int parseAppTermOpt2(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsCon.appTermOpt2, "parseAppTermOpt2");
}

static int parseAppTermOpt3(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAppTermOpt3");
}

static int parseAppTermOpt4(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAppTermOpt4");
}

static int parseRptHdrSiteName(const byte * rsp, word length)
{
	memcpy(glTlmSysParam.stEdcInfo.szMerchantName, rsp ,23);
	return saveParseStr(rsp, glTlmTmsCon.rptHdrSiteName, 24, "parseRptHdrSiteName");
}

static int parseRptHdrSiteAddr1(const byte * rsp, word length)
{
	memcpy(glTlmSysParam.stEdcInfo.szMerchantAddr, rsp ,23);
	return saveParseStr(rsp, glTlmTmsCon.rptHdrSiteAddr1, 24, "parseRptHdrSiteAddr1");
}

static int parseRptHdrSiteAddr2(const byte * rsp, word length)
{
	memcpy(glTlmSysParam.stEdcInfo.szMerchantAddr+23, rsp ,23);
	return saveParseStr(rsp, glTlmTmsCon.rptHdrSiteAddr2, 24, "parseRptHdrSiteAddr2");
}

static int parseRptHdrSiteAddr3(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsCon.rptHdrSiteAddr3, 24, "parseRptHdrSiteAddr3");
}

static int parseRptHdrSiteAddr4(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsCon.appPinSamCard, 24, "parseRptHdrSiteAddr4");
}

static int parseAppPointPos(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAppPointPos");
	//return saveParseStr(&rsp[21], appRekMerchant, 95, "parseAppPointPos");
}

static int parseAppLang(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAppLang");
}

static int parseAppLocOpt(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsCon.appLocOpt, "parseAppLocOpt");
}

static int parseAppCurrSign(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsCon.appCurrSign, lenCurrSign, "parseAppCurrSign");
}

static int parseAppVoidPwd(const byte * rsp, word length)
{
	char buf[lenPwd + 1];
	int ret;

	memset(buf, 0, sizeof(buf));
	bin2hex(buf, rsp, lenPwd / 2);
	PubBcd2Asc0((uchar*)rsp,  2, glTlmSysParam.sPassword[PWD_VOID]);
	ret = saveParseStr((byte *) buf, glTlmTmsCon.appVoidPwd, lenPwd, "parseAppVoidPwd");
	CHK;
	ret = lenPwd / 2;
	return ret;
lblKO:
	return -1;
}

static int parseAppRefPwd(const byte * rsp, word length)
{
	char buf[lenPwd + 1];
	int ret;

	memset(buf, 0, sizeof(buf));
	bin2hex(buf, rsp, lenPwd / 2);
	PubBcd2Asc0((uchar*)rsp,  2, glTlmSysParam.sPassword[PWD_REFUND]);
	ret = saveParseStr((byte *) buf, glTlmTmsCon.appRefPwd, lenPwd, "parseAppRefPwd");
	CHK;
	ret = lenPwd / 2;
	return ret;
lblKO:
	return -1;
}

static int parseAppStlPwd(const byte * rsp, word length)
{
	char buf[lenPwd + 1];
	int ret;

	memset(buf, 0x00, sizeof(buf));
	bin2hex(buf, rsp, lenPwd / 2);
	PubBcd2Asc0((uchar*)rsp,  2, glTlmSysParam.sPassword[PWD_SETTLE]);
	ret = saveParseStr((byte *) buf, glTlmTmsCon.appSetlPwd, lenPwd, "parseAppStlPwd");
	CHK;
	ret = lenPwd / 2;
	return ret;
lblKO:
	return -1;
}

static int parseAppTermOpt5(const byte * rsp, word length)
{
	return saveParseByte(rsp, &glTlmTmsCon.appTermOpt5, "parseAppTermOpt5");
}

static int parseAppDefTransaction(const byte * rsp, word length)
{
	return parseSkip(rsp, sizeof(byte), "parseAppDefTrans");
}

static int parseAppEMSale(const byte * rsp, word length)
{
	//char contStr[4];			//closed by smartweb
	char contStr[7];			//smartweb

	memset(contStr, 0x00, sizeof(contStr));
	if(*rsp & 0x01)
		strcat(contStr, "c");
	if(*rsp & 0x02)
		strcat(contStr, "m");
	if(*rsp & 0x04)
		strcat(contStr, "k");
	if(*rsp & 0x08)										//smartweb
	 	{                                          						//smartweb
//			MAPPUTBYTE(appServiceCode, 1, lblKO);		//smartweb
			glTlmTmsCon.appServiceCode = 1;
    		}												//smartweb

//	trcFS("appEMSale: %s\n", contStr);

//	mapMove(rqsBeg, (word) (trtSale - 1));
//	MAPPUTSTR(rqsEntMod, contStr, lblKO);
	return saveParseByte(rsp, &glTlmTmsCon.appEMSale, "parseAppEMSale");
}

static int parseAppEMRefund(const byte * rsp, word length)
{
	char contStr[4];

	memset(contStr, 0x00, sizeof(contStr));
	if(*rsp & 0x01)
		strcat(contStr, "c");
	if(*rsp & 0x02)
		strcat(contStr, "m");
	if(*rsp & 0x04)
		strcat(contStr, "k");

//	trcFS("appEMRefund: %s\n", contStr);

//	mapMove(rqsBeg, (word) (trtRefund - 1));
//	MAPPUTSTR(rqsEntMod, contStr, lblKO);
	return saveParseByte(rsp, &glTlmTmsCon.appEMRefund, "parseAppEMRefund");
}

static int parseAppEMOffline(const byte * rsp, word length)
{
	char contStr[4];

	memset(contStr, 0x00, sizeof(contStr));
	if(*rsp & 0x01)
		strcat(contStr, "c");
	if(*rsp & 0x02)
		strcat(contStr, "m");
	if(*rsp & 0x04)
		strcat(contStr, "k");

//	trcFS("appEMOffline: %s\n", contStr);

//  mapMove(rqsBeg, (word) (trtOffline - 1));
//	MAPPUTSTR(rqsEntMod, contStr, lblKO);
	return saveParseByte(rsp, &glTlmTmsCon.appEMOffline, "parseAppEMOffline");
}

static int parseAppEMCash(const byte * rsp, word length)
{
	char contStr[4];

	memset(contStr, 0x00, sizeof(contStr));
	if(*rsp & 0x01)
		strcat(contStr, "c");
	if(*rsp & 0x02)
		strcat(contStr, "m");
	if(*rsp & 0x04)
		strcat(contStr, "k");

//	trcFS("appEMCash: %s\n", contStr);

//	mapMove(rqsBeg, (word) (trtCash - 1));
//	MAPPUTSTR(rqsEntMod, contStr, lblKO);
	return saveParseByte(rsp, &glTlmTmsCon.appEMCash, "parseAppEMCash");
}

static int parseAppEMPreAuth(const byte * rsp, word length)
{
	char contStr[4];

	memset(contStr, 0x00, sizeof(contStr));
	if(*rsp & 0x01)
		strcat(contStr, "c");
	if(*rsp & 0x02)
		strcat(contStr, "m");
	if(*rsp & 0x04)
		strcat(contStr, "k");

//	trcFS("appEMPreAuth: %s\n", contStr);

//	mapMove(rqsBeg, (word) (trtPreAut - 1));
//	MAPPUTSTR(rqsEntMod, contStr, lblKO);
	return saveParseByte(rsp, &glTlmTmsCon.appEMPreAuth, "parseAppEMPreAuth");
}

static int parseAppEMCrdVrfy(const byte * rsp, word length)
{
	char contStr[4];
//	int ret;

	memset(contStr, 0x00, sizeof(contStr));
	if(*rsp & 0x01)
		strcat(contStr, "c");
	if(*rsp & 0x02)
		strcat(contStr, "m");
	if(*rsp & 0x04)
		strcat(contStr, "k");

//	trcFS("appEMCrdVrfy: %s\n", contStr);

//	mapMove(rqsBeg, (word) (trtPowerBuy - 1));
//	MAPPUTSTR(rqsEntMod, contStr, lblKO);
	return saveParseByte(rsp, &glTlmTmsCon.appEMCrdVrfy, "parseAppEMCrdVrfy");
}
#if 1
static int parseAppEMBalance(const byte * rsp, word length)
{
	char contStr[4];
	//int ret;

	memset(contStr, 0x00, sizeof(contStr));     // Cicilan Add Bukopin

	AllSetMenuCicilanBukopin();// 4212 for Tenor
		if(*rsp & 0x01){
			SetOnBukTenor(MnuTenor1);
			strcat(contStr, "c");
		} else
			SetOffBukTenor(MnuTenor1);
		
		if(*rsp & 0x02){
			strcat(contStr, "m");
			SetOnBukTenor(MnuTenor3);
		} else
			SetOffBukTenor(MnuTenor3);
		
		if(*rsp & 0x04){
			SetOnBukTenor(MnuTenor6);
			strcat(contStr, "k");
		} else
			SetOffBukTenor(MnuTenor6);
		
		if(*rsp & 0x08)
			SetOnBukTenor(MnuTenor9);
		 else
			SetOffBukTenor(MnuTenor9);
	
		if(*rsp & 0x10)
			SetOnBukTenor(MnuTenor12);
		  else
			SetOffBukTenor(MnuTenor12);
	
		if(*rsp & 0x20)
			SetOnBukTenor(MnuTenor18);
		  else
			SetOffBukTenor(MnuTenor18);
		  
		if(*rsp & 0x40)
			SetOnBukTenor(MnuTenor24);
		else
			SetOffBukTenor(MnuTenor24);

	trcFS("appEMBalance: %s\n", contStr);

	//mapMove(rqsBeg, (word) (trtBalance - 1));
	//MAPPUTSTR(rqsEntMod, contStr, lblKO);
	return saveParseByte(rsp, &glTlmTmsCon.appEMBalance, "parseAppEMBalance");
//lblKO:
//	return -1;
}
#else
static int parseAppEMBalance(const byte * rsp, word length)
{
	char contStr[4];
//	int ret;

	memset(contStr, 0x00, sizeof(contStr));
	if(*rsp & 0x01)
		strcat(contStr, "c");
	if(*rsp & 0x02)
		strcat(contStr, "m");
	if(*rsp & 0x04)
		strcat(contStr, "k");

//	trcFS("appEMBalance: %s\n", contStr);

//	mapMove(rqsBeg, (word) (trtBalance - 1));
//	MAPPUTSTR(rqsEntMod, contStr, lblKO);
	return saveParseByte(rsp, &glTlmTmsCon.appEMBalance, "parseAppEMBalance");
}
#endif
static int parseAppRekening(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsCon.appRekMerchant, 76, "parseAppRekeningTransfer");
}

// for SST time settelment
static int parseAppTimeSettle(const byte * rsp, word length)
{
	return saveParseStr(rsp, glTlmTmsCon.appTimeSet, 4, "parseAppTimeSettle");
}


static int parseConTbl(const byte * rsp, word len)
{
	int ret = 0;
	word length = 1;

	
	PARSE(parseLength);
	PARSE(parseAppAmtFmt);
	PARSE(parseAppDefTra);
	PARSE(parseAppDateTime);
	PARSE(parseAppAdmPwd);
	PARSE(parseAppHelpDesk);
	PARSE(parseAppTermOpt1);
	PARSE(parseAppTermOpt2);
	PARSE(parseAppTermOpt3);
	PARSE(parseAppTermOpt4);
	PARSE(parseRptHdrSiteName);
	PARSE(parseRptHdrSiteAddr1);
	PARSE(parseRptHdrSiteAddr2);
	PARSE(parseRptHdrSiteAddr3);
	PARSE(parseRptHdrSiteAddr4);
	PARSE(parseAppPointPos);
	PARSE(parseAppLang);
	PARSE(parseAppLocOpt);
	PARSE(parseAppCurrSign);
	PARSE(parseAppVoidPwd);
	PARSE(parseAppRefPwd);
	PARSE(parseAppStlPwd);
	PARSE(parseAppTermOpt5);
	PARSE(parseAppDefTransaction);
	PARSE(parseAppEMSale);
	PARSE(parseAppEMRefund);
	PARSE(parseAppEMOffline);
	PARSE(parseAppEMCash);
	PARSE(parseAppEMPreAuth);
	PARSE(parseAppEMCrdVrfy);
	PARSE(parseAppEMBalance);
	PARSE(parseAppRekening);
	PARSE(parseAppTimeSettle);	//for sst

	CHECK(len == 0, lblKO);

	return ret;
lblKO:
	return -1;
}

static int parseTMSLP(const byte * val, word len)
{
	int ret = 0;
	byte TmsMsg;
//	long	DataLen;

	//    MAPGETBYTE(traTmsMsg, TmsMsg, lblKO);
   	TmsMsg = glProcInfo.tlmTra.traTmsMsg;
    	switch (TmsMsg) {
      		case tmsDnlConTbl:
//          		MAPPUTBYTE(traTmsMreRec, val[0], lblKO);
//			hex_dump("tmsDnlConTbl", &TmsMsg, sizeof(TmsMsg));
			glProcInfo.tlmTra.traTmsMreRec = val[0];
//		  	hex_dump_char("tmsDnlConTbl", val, len);
		       ret = parseConTbl(val + 1, (word)(len - 1));
          		break;
	      	case tmsDnlBinTbl:
//	          	MAPPUTBYTE(traTmsMreRec, val[0], lblKO);
			glProcInfo.tlmTra.traTmsMreRec = val[0];
//		  	hex_dump_char("tmsDnlBinTbl", val, len);

		   	ret = parseBinTbl(val + 1, (word)(len - 1));
			
	          	break;
	      	case tmsDnlIssTbl:
//	          	MAPPUTBYTE(traTmsMreRec, val[0], lblKO);
	          	glProcInfo.tlmTra.traTmsMreRec = val[0];
//		  	hex_dump_char("tmsDnlIssTbl", val, len);

			ret = parseIssTbl(val + 1, (word)(len - 1));
	          	break;
	      	case tmsDnlAcqTbl:
//	          	MAPPUTBYTE(traTmsMreRec, val[0], lblKO);
			glProcInfo.tlmTra.traTmsMreRec = val[0];
//		  	hex_dump_char("tmsDnlAcqTbl", val, len);

	          	ret = parseAcqTbl(val + 1, (word)(len - 1));
	          	break;
	      	case tmsEmvReq:
//		  	hex_dump_char("tmsEmvReq", val, len);
				
	          	ret = parseEmvReq(val, len);
	          	break;
	      	case tmsEmvFileDnl:			
//		  	hex_dump_char("tmsEmvFileDnl", val, len);

	          	ret = parseEmvFleDnl(val, len);
	          	break;
	      	case tmsEmvFileOpn:
//		  	hex_dump_char("tmsEmvFileOpn", val, len);

	          	ret = parseEmvFleOpn(val, len);
	          	break;
	      	case tmsEmvFileRed:
//		  	hex_dump_char("tmsEmvFileRed", val, len);

	          	ret = parseEmvFleRed(val, len);
	          	break;
	    	default:		 
			break;					
    }
    return ret;
//  lblKO:
    //return -1;
}

static int CheckTMSRspCode(void)
{
	int ret;
	char RespCode[LEN_RSP_CODE+ 1];

	MEM_ZERO(RespCode);
	
	
//	MAPGET(traRspCod, RespCode, lblKO);
	memcpy(RespCode, glTMSTELIUMRecv.szBit39, LEN_RSP_CODE);
	
	CHECK(memcmp(RespCode, "00", LEN_RSP_CODE) == 0, lblKO);

	ret = 0;
	goto lblEnd;
lblKO:
	switch (RespCode[1])
	{
		case '1':
//			usrInfo(infFileMiss); // Cancelled System Error N1: Requested file download does not exist or is missing
			break;
		case '2':
//			usrInfo(infTMSUnAvail);   // Cancelled System Error N2: TMS server unavailable
			break;
		case '3':
//			usrInfo(infTMSQueryFailed);   // Cancelled System Error N3: Database Query Failed
			break;
		case '4':
//			usrInfo(infTMSDLTooLarge);    // Cancelled System Error N4: request download too large
			break;
		case '5':
//			usrInfo(infTMSVerErr);    // Cancelled System Error N5: TMS2 cannot read version file for a software download
			break;
		case '6':
//			usrInfo(infTMSBufOvrFlw); // Cancelled System Error N6: TMS error buffer overflow
			break;
		case '7':
//			usrInfo(infTMSInvDLReq);  // Cancelled System Error N7: Invalid Download request
			break;
		case '8':
//			usrInfo(infTMSRefNumInv); // Cancelled System Error N8: Invalid TMS Reference Number
			break;
		case 'A':
//			usrInfo(infTMSUnknwnReq); // Cancelled System Error NA: Unknown Request
			break;
		case 'B':
//			usrInfo(infTMSOutofBounds);   // Cancelled System Error NB: internal TMS2 error, value out of bounds
			break;
		case 'C':
//			usrInfo(infTMSFileError); // Cancelled System Error NC: internal TMS2 error: file has not been created for download
			break;
		case 'E':
//            		usrInfo(infInvalidTID);   //if TID == 00000000, not continue TMS process
           		break;
        	case 'F':
//		       usrInfo(infInvalidMID);   //if MID == 000000000000000, not continue TMS process
		       break;
		case 'Z':
//			usrInfo(infTMSUndefError);    // Cancelled System Error NZ: undefined error
			break;

	}
	ret = -1;

lblEnd:

	return ret;

}

static byte getNextState(byte sta)
{
	int ret;
	card RspCod, totSze, offset;
	byte mreRec;
//	byte idxBtcUpl;
	//word BinLength;
	char refnum[1+8];
	byte tcUploadFlag=0;	//add, irf, 08062011

	MEM_ZERO(refnum);
	
//	MAPGETCARD(cvtRspCod, RspCod, lblKO);
//	MAPGETBYTE(traTmsMreRec, mreRec, lblKO);
	mreRec = glProcInfo.tlmTra.traTmsMreRec;

	switch (sta)
	{
		case dnlRecvLoginRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					ret = dnlSendConTblReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvConTblRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					ret = dnlSendBinTblReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvBinTblRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					//ret = dnlSendIssTblReq;//ori
					ret = (mreRec > 0) ? dnlSendBinTblReq: dnlSendIssTblReq;//mcy add,.
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
	
		case dnlRecvIssTblRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					ret = (mreRec > 0) ? dnlSendIssTblReq : dnlSendAcqTblReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvAcqTblRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					//ret = (mreRec>0)? dnlSendAcqTblReq: dnlSendLogOffReq;
					ret = (mreRec > 0) ? dnlSendAcqTblReq : dnlSendEmvReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvLogOffRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
				default:
						  
//				  	MAPGET(regTmsRefNum, refnum, lblKO);
//				  	MAPPUTSTR(appTmsRefNum, refnum, lblKO);
//					MAPPUTBYTE(appAfterTMS, 1, lblKO);
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvEmvRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					ret = dnlSendEmvFileReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvEmvFileRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					ret = dnlSendEmvFileOpnReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvEmvFileOpnRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					ret = dnlSendEmvFileRedReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvEmvFileRedRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
//					MAPGETCARD(traTmsEmvFleSze, totSze, lblKO);
					totSze = glProcInfo.tlmTra.traTmsEmvFleSze;
//					MAPGETCARD(traTmsEmvFleOff, offset, lblKO);
					offset = glProcInfo.tlmTra.traTmsEmvFleOff;
					ret = (offset >= totSze) ? dnlSendEmvFileClsReq : dnlSendEmvFileRedReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		case dnlRecvEmvFileClsRsp:
			RspCod = CheckTMSRspCode();
			switch (RspCod)
			{
				case 0:
					ret = dnlSendLogOffReq;
					break;
				default:
					ret = dnlEnd;
					break;
			}
			break;
		default:
			ret = 0xFF;
			break;
	}
	glProcInfo.tlmTra.traSta=ret;
	return ret;
//lblKO:
//	return 0xFF;
}


static int tleAut(byte sta)
{  
	//tle automate
	int ret = 0;
	long DataLen;

	sta = glProcInfo.tlmTra.traSta;
	while(sta < dnlEnd)
	{
//		MAPPUTBYTE(traSta, sta, lblKO);
		MEM_ZERO(sgTempBuf);
		switch (sta)
		{
			case dnlSendLoginReq:
//				MAPPUTBYTE(traTmsMsg, tmsLogin, lblKO);
				glProcInfo.tlmTra.traTmsMsg = tmsLogin;
//				ret = sendReq(par, sta);
				SetCommReqFieldTMS();	
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
//				usrInfo(infTrans);
				ret = sta += 1;
				break;
			case dnlSendConTblReq:
//				MAPPUTBYTE(traTmsMsg, tmsDnlConTbl, lblKO);
				glProcInfo.tlmTra.traTmsMsg = tmsDnlConTbl;
//				ret = sendReq(par, sta);
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//test only				ResetAllPara(FALSE);
//				usrInfo(infEMVCredit);
//				usrInfo(infTrmCon);
				break;
			case dnlSendBinTblReq:
//				MAPPUTBYTE(traTmsMsg, tmsDnlBinTbl, lblKO);
				glProcInfo.tlmTra.traTmsMsg = tmsDnlBinTbl;
//				ret = sendReq(par, sta);
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				ret = sendReq(par, sta);
//				usrInfo(infCrdRng);
				break;
			case dnlSendIssTblReq:
//				MAPPUTBYTE(traTmsMsg, tmsDnlIssTbl, lblKO);
				glProcInfo.tlmTra.traTmsMsg = tmsDnlIssTbl;
//				ret = sendReq(par, sta);
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				usrInfo(infIss);
				break;
			case dnlSendAcqTblReq:
//				MAPPUTBYTE(traTmsMsg, tmsDnlAcqTbl, lblKO);
				glProcInfo.tlmTra.traTmsMsg = tmsDnlAcqTbl;
//				ret = sendReq(par, sta);
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				usrInfo(infAcq);
				break;
			case dnlSendLogOffReq:
//				MAPPUTBYTE(traTmsMsg, tmsDnlConTbl, lblKO);
				glProcInfo.tlmTra.traTmsMsg = tmsLogOff;
//				ret = sendReq(par, sta);
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				MAPPUTBYTE(traTxnType, trtTMSLOGOFF, lblKO);
//				CHK;
				break;
			case dnlRecvLoginRsp:
//				usrInfo(infRcving);
//				ret = recvRsp(par, sta);
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlRecvConTblRsp:
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
//				ret = recvRsp(par, sta);
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlRecvBinTblRsp:
//				ret = recvRsp(par, sta);
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlRecvIssTblRsp:
//				ret = recvRsp(par, sta);
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlRecvAcqTblRsp:
//				ret = recvRsp(par, sta);
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlRecvLogOffRsp:
//				ret = recvRsp(par, sta);
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = getNextState(sta);
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				break;
			case dnlSendEmvReq:
				glProcInfo.tlmTra.traTmsMsg = tmsEmvReq;
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				MAPPUTBYTE(traTmsMsg, tmsEmvReq, lblKO);
//				MAPPUTBYTE(traTxnType, trtTMSEmvReq, lblKO);
//				ret = sendReq(par, sta);
//				usrInfo(infEmvPrm);
				break;
			case dnlRecvEmvRsp:
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
//				ret = recvRsp(par, sta);
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlSendEmvFileReq:
				glProcInfo.tlmTra.traTmsMsg = tmsEmvFileDnl;
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				MAPPUTBYTE(traTmsMsg, tmsEmvFileDnl, lblKO);
//				MAPPUTBYTE(traTxnType, trtTMSEmvFleDnl, lblKO);
//				ret = sendReq(par, sta);
				break;
			case dnlRecvEmvFileRsp:
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
//				ret = recvRsp(par, sta);
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlSendEmvFileOpnReq:
				glProcInfo.tlmTra.traTmsMsg = tmsEmvFileOpn;
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				MAPPUTBYTE(traTmsMsg, tmsEmvFileOpn, lblKO);
//				ret = sendReq(par, sta);
				break;
			case dnlRecvEmvFileOpnRsp:
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
//				ret = recvRsp(par, sta);
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlSendEmvFileRedReq:
				glProcInfo.tlmTra.traTmsMsg = tmsEmvFileRed;
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				MAPPUTBYTE(traTmsMsg, tmsEmvFileRed, lblKO);
//				ret = sendReq(par, sta);
				break;
			case dnlRecvEmvFileRedRsp:
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
//				ret = recvRsp(par, sta);
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);
				ret = getNextState(sta);
				break;
			case dnlSendEmvFileClsReq:
				glProcInfo.tlmTra.traTmsMsg = tmsEmvFileCls;
				SetCommReqFieldTMS();
				ret = SendPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
				ret = sta += 1;
//				MAPPUTBYTE(traTmsMsg, tmsEmvFileCls, lblKO);
//				ret = sendReq(par, sta);
				break;
			case dnlRecvEmvFileClsRsp:
				ret = RecvPacket();
				if( ret!=0 )
				{
					goto lblKO;
				}
//				ret = recvRsp(par, sta);
				PubChar2Long(glTMSTELIUMRecv.szBit60, 2, (ulong *)&DataLen);
				memcpy(sgTempBuf, &glTMSTELIUMRecv.szBit60[2], (int)DataLen);
				parseTMSLP(sgTempBuf, (word)DataLen);

				parseEmvFile();
				
				ret = getNextState(sta);
				break;
			default:
				
				ret = dnlEnd;
				break;
		}
		CHECK(ret >= 0, lblKO);
		sta = ret;
		DelayMs(300);
	}
	ret = 1;
	goto lblEnd;
lblKO:
	ret = -1;
lblEnd:
	return ret;
}

// get TMS download reff num
int GetTmsRefNum(uchar *pszID)
{
	uchar	ucRet, szBuff[8+1];
	#ifdef BANK_BUKOPIN
	ReadSN(szBuff);
	sprintf((char *)pszID, "%.8s", szBuff);
	#endif
	while (1)
	{
		ScrClrLine(2, 7);
		PubDispString(_T("TMS REFERENCE NO"), DISP_LINE_LEFT|3);
		sprintf(szBuff, "%8s", pszID);
		ucRet = PubGetString(NUM_IN|ECHO_IN|ALPHA_IN, 8, 8, szBuff, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		if (atol((char *)szBuff)!=0)
		{
			break;
		}
	}

	sprintf((char *)pszID, "%.8s", szBuff);
	return 0;
}


// get TMS Nii
int GetTmsNii(uchar *pszID)
{
	uchar	ucRet, szBuff[8+1];

	while (1)
	{
		ScrClrLine(2, 7);
		PubDispString(_T("TMS NII :"), DISP_LINE_LEFT|3);
		sprintf((char *)szBuff, "%.8s", pszID);
		ucRet = PubGetString(NUM_IN|ECHO_IN, 3, 3, szBuff, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		if (atol((char *)szBuff)!=0)
		{
			break;
		}
	}

	sprintf((char *)pszID, "%.8s", szBuff);
	return 0;
}

// Choose Connection TMS
int ChooseConnectionTMS(uchar *pszID)
{
	uchar	ucRet, szBuff[8+1];

	while (1)
	{
		ScrClrLine(2, 7);
		PubDispString(_T("TMS NII :"), DISP_LINE_LEFT|3);
		sprintf((char *)szBuff, "%.8s", pszID);
		ucRet = PubGetString(NUM_IN|ECHO_IN, 8, 8, szBuff, USER_OPER_TIMEOUT);
		if( ucRet!=0 )
		{
			return ERR_USERCANCEL;
		}
		if (atol((char *)szBuff)!=0)
		{
			break;
		}
	}

	sprintf((char *)pszID, "%.8s", szBuff);
	return 0;
}

#ifdef ENABLE_EMV
static void tlmRemoveEmvAppCapk(void)
{
	int				iCnt;
	int				iRet;
	EMV_CAPK		stEmvCapk;
	EMV_APPLIST		stEmvApp;

	for(iCnt=0; iCnt<MAX_KEY_NUM; iCnt++)
	{
		memset(&stEmvCapk, 0, sizeof(EMV_CAPK));
		iRet = EMVGetCAPK(iCnt, &stEmvCapk);
		if( iRet==EMV_OK )
		{
			EMVDelCAPK(stEmvCapk.KeyID, stEmvCapk.RID);
		}
	}
	for(iCnt=0; iCnt<MAX_APP_NUM; iCnt++)
	{
		memset(&stEmvApp, 0, sizeof(EMV_APPLIST));
		iRet = EMVGetApp(iCnt, &stEmvApp);
		if( iRet==EMV_OK )
		{
			EMVDelApp(stEmvApp.AID, (int)stEmvApp.AidLen);
		}
	}
}
#endif

void TmsTeliumDownLoad(void)
{
	int		iRet;
//	long	lDataLen;
	uchar sta;

	InitTransInfo();
	TransInit(TRANS_TYPE_TMS_TELIUM);

	ScrCls();
	DispTransName();
	if( PasswordBank()!=0 )
	{
		return;
	}

	ScrCls();
	DispTransName();
	if ((!ChkIfBatchEmpty()) ||
		(!ChkIfBatchBrizziEmpty()))
	{
		PubDispString(_T("PLS SETTLE BATCH"), 4|DISP_LINE_LEFT);
		PubWaitKey(USER_OPER_TIMEOUT);
		return;
	}

	if(GetTmsRefNum(glTlmSysParam.stEdcInfo.szDownLoadTID)!=0)
	{
		return;
	}

	if(GetTmsNii(glTlmSysParam.stEdcInfo.szTMSNii)!=0)
	{
		return;
	}

	if(SetSysCommLoadParam(PM_MEDIUM)!=0) // Add By Zaki 24 April 2013 Set Comm
	{
		return;
	}
	
	// 提取已输入的通信数据
	//memcpy(&glCommCfg, &glTlmSysParam.stTMSCommCfg, sizeof(COMM_CONFIG));
	memcpy(&glCommCfg, &glTlmSysParam.stTMSCommCfg, sizeof(COMM_CONFIG));
	/*/
	if (glTlmSysParam.ucTMSSyncDial)
	{
		memcpy(&glCommCfg.stPSTNPara.stPara, &glTlmSysParam._TxnModemPara, sizeof(COMM_PARA));
	}
	//*/ //by 4212 for modem
	ScrCls();
	DispTransName();
	
	if(ConfirmLoadEDC()!=0)
	{
		return;
	}

	DispWait();
	
//	 glProcInfo.stTranLog.ucTranType=TRANS_TYPE_TMS_TELIUM;
	 
	glCurTlmTmsAcq.acqTxnRxTO= glTlmSysParam.stEdcInfo.ucTMSTimeOut;

	glProcInfo.tlmTra.traSta = dnlSendLoginReq;

	sta = glProcInfo.tlmTra.traSta;


	MEM_ZERO(&glTlmTmsCon);
	MEM_ZERO(&glTlmTmsBin);
	MEM_ZERO(&glTlmTmsIss);
	MEM_ZERO(&glTlmTmsAcq);
	MEM_ZERO(&glTlmTmsAid);
	MEM_ZERO(&glTlmTmsPub);
	MEM_ZERO(&glTlmTmsParam);

	iRet = tleAut(sta);
	CommOnHook(FALSE);
	if(iRet<0)
		return;	//fail


	if(memcmp(glTMSTELIUMRecv.szBit39, "00", 2)!=0)
		return;
	
	DispProcess();

	memcpy(&glTlmSysParam.Con, &glTlmTmsCon, sizeof(TLM_CON));
	glTlmSysParam.ucBinNum = glTlmTmsBin.ucBinNum;
	memcpy(&glTlmSysParam.Bin, &glTlmTmsBin.Bin, sizeof( glTlmTmsBin.Bin));
	glTlmSysParam.ucIssNum = glTlmTmsIss.ucIssNum;
	memcpy(&glTlmSysParam.Iss, &glTlmTmsIss.Iss, sizeof( glTlmTmsIss.Iss));
	glTlmSysParam.ucAcqNum = glTlmTmsAcq.ucAcqNum;
	memcpy(&glTlmSysParam.Acq, &glTlmTmsAcq.Acq, sizeof( glTlmTmsAcq.Acq));
	SaveTlmSysParam();
	SaveTLMaid();
	SaveTLMpub();
	SaveSysCtrlBase();

#if 1
	tlmRemoveEmvAppCapk();

	SetEmvAid();
	SetEmvCapk();
#endif

	AfterLoadParaProc();
//	ClearMiniAtmCSRecord();
//	ClearTunaiTBankRecord();
	ClearRecord(ACQ_ALL);		//irf,  02 Jan 2014
	
	DispResult(0);
	PrintParamNew(); // Add By Zaki 22 April 2013
	if (!ChkTerm(_TERMINAL_S90_))	// S90 do not support soft-reboot
	{
#ifdef _S_SERIES_
		ScrCls();
		PubDispString(_T("REBOOT..."), 5|DISP_LINE_LEFT);
		PubWaitKey(3);
		Reboot();
		return;
#endif
	}
	ScrCls();
	//PubDispString(_T("PLS REBOOT POS."), 5|DISP_LINE_LEFT);
	PubDispString(_T("PLEASE"), 3|DISP_LINE_CENTER);
	PubDispString(_T("RESTART EDC"), 5|DISP_LINE_CENTER);
	PubWaitKey(-1);
	PowerOff();
	//while(1);	
}

void SetEmvAid(void)
{
	int iLoop;
	EMV_APPLIST	emvAid;
	int iRet;
	
	
	for(iLoop=0;iLoop<glTlmTmsAid.ucAidNum;iLoop++)
	{
		memset(&emvAid, 0, sizeof(EMV_APPLIST));
		//AppName[33];       
		memcpy(emvAid.AID, glTlmTmsAid.Aid[iLoop].emvAid+1, glTlmTmsAid.Aid[iLoop].emvAid[0]);
		emvAid.AidLen = glTlmTmsAid.Aid[iLoop].emvAid[0];
		emvAid.SelFlag = PART_MATCH;           
		emvAid.Priority = 0;          
		emvAid.TargetPer = glTlmTmsAid.Aid[iLoop].emvTarPer[1];         
		emvAid.MaxTargetPer = glTlmTmsAid.Aid[iLoop].emvMaxTarPer[1];         
 		emvAid.FloorLimitCheck = 1;   
		emvAid.RandTransSel = 1;     
		emvAid.VelocityCheck = 1;     
		emvAid.FloorLimit = PubBcd2Long(glTlmTmsAid.Aid[iLoop].emvTrmFlrLim+1, glTlmTmsAid.Aid[iLoop].emvTrmFlrLim[0], NULL);        //最低限额
		emvAid.Threshold = 0;         

		memcpy(emvAid.TACDenial, glTlmTmsAid.Aid[iLoop].emvTACDen+1, glTlmTmsAid.Aid[iLoop].emvTACDen[0]);
		memcpy(emvAid.TACOnline, glTlmTmsAid.Aid[iLoop].emvTACOnl+1, glTlmTmsAid.Aid[iLoop].emvTACOnl[0]);
		memcpy(emvAid.TACDefault, glTlmTmsAid.Aid[iLoop].emvTACDft+1, glTlmTmsAid.Aid[iLoop].emvTACDft[0]);

		memcpy(emvAid.AcquierId, glTlmTmsAid.Aid[iLoop].emvAcqId+1, glTlmTmsAid.Aid[iLoop].emvAcqId[0]) ;      

		memcpy(emvAid.dDOL, glTlmTmsAid.Aid[iLoop].emvDftValDDOL, glTlmTmsAid.Aid[iLoop].emvDftValDDOL[0]+1);
		memcpy(emvAid.tDOL, glTlmTmsAid.Aid[iLoop].emvDftValTDOL, glTlmTmsAid.Aid[iLoop].emvDftValTDOL[0]+1);

		memcpy(emvAid.Version,  glTlmTmsAid.Aid[iLoop].emvTrmAvn+1, glTlmTmsAid.Aid[iLoop].emvTrmAvn[0]);
	
		iRet = EMVAddApp(&emvAid);
//		hex_dump_char("EMVAddApp", &iRet, sizeof(iRet));
	}
}

void SetEmvCapk(void)
{
	int iLoop;
	EMV_CAPK emvCapk;
	int iRet;
	
	for(iLoop=0;iLoop<glTlmTmsPub.ucPubNum;iLoop++)
	{
		memset(&emvCapk, 0, sizeof(EMV_CAPK));
		memcpy(emvCapk.RID, glTlmTmsPub.Pub[iLoop].pubRid+1, lenRid);
		emvCapk.KeyID = glTlmTmsPub.Pub[iLoop].pubIdx;
		emvCapk.HashInd = 1;           
		emvCapk.ArithInd = 1;          
		emvCapk.ModulLen = glTlmTmsPub.Pub[iLoop].pubkeyData[0];          
		memcpy(emvCapk.Modul, glTlmTmsPub.Pub[iLoop].pubkeyData+1, glTlmTmsPub.Pub[iLoop].pubkeyData[0]);  
		emvCapk.ExponentLen = 1; 
		emvCapk.Exponent[0] = glTlmTmsPub.Pub[iLoop].pubExp;     
		memcpy(emvCapk.ExpDate, "\x18\x12\x31", 3);      
		CalCAPKCheckSum(&emvCapk, emvCapk.CheckSum);
//		getDefaultCheckSumPublicKey(emvCapk.KeyID, emvCapk.RID, emvCapk.CheckSum);
//		emvCapk.CheckSum[0] =  glTlmTmsPub.Pub[iLoop].pubAlgo;       

		iRet = EMVAddCAPK(&emvCapk);
//		hex_dump_char("EMVAddCAPK", &iRet, sizeof iRet );
	}
}

