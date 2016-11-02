#include "global.h"

static uchar TLEData(uchar *DataIn, tBuffer*DataOut)
{
	uchar bitMap[lenBitmap];
	uchar bit;
	uchar lenbit[4+1];
	uchar bcd[128*3+1];
	tBuffer temp;
	tBuffer temp2;
	uchar buf[999];
	uchar buf2[999];
	int ret;
	int len;
	int nByte;
	uint iFieldLen;

	MEM_ZERO(buf);
	MEM_ZERO(buf2);
	MEM_ZERO(bitMap);
	bufInit(&temp, buf, sizeof(buf));
    	bufInit(&temp2, buf2, sizeof(buf2));
	

	ret = pckUnpack(DataIn);

	memcpy(bitMap, DataIn+2, lenBitmap);

	for(bit=2;bit<(8*lenBitmap);bit++)
	{
		if(!bitTest(bitMap, bit))
			continue;

		bufReset(&temp2);
		MEM_ZERO(lenbit);
		MEM_ZERO(bcd);
		MEM_ZERO(buf2);
		nByte = 1;

		switch(bit)
		{
			case 2:
				ret = bufApp(&temp, "\x02", 1);
				CHK;

				ret = PackElement(glEdcDataDef+2, glSendPack.szBit2, buf2, (uint *)&iFieldLen);
				if( ret <=0 )
				{
					goto lblKO;
				}
				
				ret = bufApp(&temp2, buf2, iFieldLen);
				CHK;

				len = bufLen(&temp2);
				num2bin(lenbit,len,1);

				ret = bufApp(&temp, (byte*) lenbit, 1);
				CHK;

				ret = bufApp(&temp, bufPtr(&temp2), bufLen(&temp2));
				CHK;

				MEM_ZERO(&glSendPack.szBit2);
				break;
			case 4:
				ret = bufApp(&temp, "\x04", 1);
				CHK;

				ret = PackElement(glEdcDataDef+4, glSendPack.szBit4, buf2, (uint *)&iFieldLen);
				if( ret <=0 )
				{
					goto lblKO;
				}
				
				ret = bufApp(&temp2, buf2, iFieldLen);
				CHK;

				ret = bufApp(&temp, (byte*) "\x06", nByte);
				CHK;

				ret = bufApp(&temp, bufPtr(&temp2), bufLen(&temp2));
				CHK;

				MEM_ZERO(&glSendPack.szBit4);
				break;
			case 14:
				ret = bufApp(&temp, "\x16", 1);
				CHK;

				ret = PackElement(glEdcDataDef+14, glSendPack.szBit14, buf2, (uint *)&iFieldLen);
				if( ret <=0 )
				{
					goto lblKO;
				}

				ret = bufApp(&temp2, buf2, iFieldLen);
				CHK;   

				ret = bufApp(&temp, (byte*) "\x02", 1);
				CHK;

				ret = bufApp(&temp, bufPtr(&temp2), 2);
				CHK;

				MEM_ZERO(&glSendPack.szBit14);
				break;
			case 35:
				ret = bufApp(&temp, "\x43", 1);
				CHK;

				ret = PackElement(glEdcDataDef+35, glSendPack.szBit35, buf2, (uint *)&iFieldLen);
				if( ret <=0 )
				{
					goto lblKO;
				}

				ret = bufApp(&temp2, buf2, iFieldLen);
				CHK;

			    	len = bufLen(&temp2);

				num2bin(lenbit, len, 1);

				ret = bufApp(&temp, (byte*) lenbit, 1);
				CHK;

				ret = bufApp(&temp, bufPtr(&temp2), bufLen(&temp2));
				CHK;

				MEM_ZERO(&glSendPack.szBit35);
				break;
			case 48:
				ret = bufApp(&temp, "\x60", 1);
				CHK;

				ret = PackElement(glEdcDataDef+48, glSendPack.szBit48, buf2, (uint *)&iFieldLen);
				if( ret <=0 )
				{
					goto lblKO;
				}
				
				ret = bufApp(&temp2, buf2, iFieldLen);
				CHK;

				len = bufLen(&temp2);
				if (len >= 128){
				    	if (len < 256){
				    		memcpy(lenbit,"\x81",1);
				    		num2bin(lenbit + 1, len, 1);
				    		nByte = 2;
				    	}
				    	//setting for max len = 9999 (integer)
				    	else{
				    		memcpy(lenbit,"\x82",1);
				    		num2bin(lenbit + 1, len, 2);
				    		nByte = 3;
				    	}
				    }
				    else{
					    num2bin(lenbit,len,1);
					    nByte = 1;
				    }

				ret = bufApp(&temp, (byte*) lenbit, nByte);
				CHK;

				ret = bufApp(&temp, bufPtr(&temp2), bufLen(&temp2));
				CHK;

				MEM_ZERO(&glSendPack.szBit48);
				break;
			case 55:
				ret = bufApp(&temp, "\x67", 1);
				CHK;

				ret = PackElement(glEdcDataDef+55, glSendPack.szBit55, buf2, (uint *)&iFieldLen);
				if( ret <=0 )
				{
					goto lblKO;
				}

				ret = bufApp(&temp2, buf2, iFieldLen);
				CHK;
				
				len = bufLen(&temp2);
				if (len >= 128){
				    	if (len < 256){
				    		memcpy(lenbit,"\x81",1);
				    		num2bin(lenbit + 1, len, 1);
				    		nByte = 2;
				    	}
				    	//setting for max len = 9999 (integer)
				    	else{
				    		memcpy(lenbit,"\x82",1);
				    		num2bin(lenbit + 1, len, 2);
				    		nByte = 3;
				    	}
				    }
				    else{
					    num2bin(lenbit,len,1);
					    nByte = 1;
				    }

				ret = bufApp(&temp, (byte*) lenbit, nByte);
				CHK;

				ret = bufApp(&temp, bufPtr(&temp2), bufLen(&temp2));
				CHK;

				MEM_ZERO(&glSendPack.szBit55);
				break;
			case 58:
				ret = bufApp(&temp, "\x72", 1);
				CHK;
				
				ret = PackElement(glEdcDataDef+58, glSendPack.szBit58, buf2, (uint *)&iFieldLen);
				if( ret <=0 )
				{
					goto lblKO;
				}

				ret = bufApp(&temp2, buf2, iFieldLen);
				CHK;
				
				len = bufLen(&temp2);
				if (len >= 128){
				    	if (len < 256){
				    		memcpy(lenbit,"\x81",1);
				    		num2bin(lenbit+1,len,1);
				    		nByte = 2;
				    	}
				    	//setting for max len = 9999 (integer)
				    	else{
				    		memcpy(lenbit,"\x82",1);
				    		num2bin(lenbit+1,len,2);
				    		nByte = 3;
				    	}
				    }
				    else{
					    num2bin(lenbit,len,1);
					    nByte = 1;
				    }

				ret = bufApp(&temp, (byte*) lenbit, nByte);
				CHK;	

				ret = bufApp(&temp, bufPtr(&temp2), bufLen(&temp2));
				CHK;

				MEM_ZERO(&glSendPack.szBit58);
				break;

			default:

				break;
		}
		
	}

	bufApp(DataOut, bufPtr(&temp), bufLen(&temp));
//	memcpy(DataOut, bufPtr(&temp), bufLen(&temp));
//	hex_dump_char("DATA TLE", bufPtr(DataOut), bufLen(DataOut));

//	PubLong2Char((ulong) bufLen(&temp), 2, glSendPack.szBit57);
//	memcpy(glSendPack.szBit57+2, bufData, bufLen(&temp));

	return TRUE;
lblKO:
	return FALSE;
}

// In bit 57
int getTLEData(uchar *DataIn) {
    	int ret,len;
    	byte LTWKDEK[16+1];
    	byte buf[1024];
    	char temp[1024+1];
    	char HTLE[4+1];
    	char Version[2+1];
    	char acqID[3+1];
    	char LTID[8+1];
    	char encMod[3+1];
    	char LTWKID[4+1];
    	char encCounter[4+1];
    	char protLength[4+1];
    	byte dBit[1024*3+1];
    	tBuffer bit;
    	char lenbit[3+1];
		tBuffer val;
		uchar bufval[999];

    	memset(temp,0,sizeof(temp));
    	memset(HTLE,0,sizeof(HTLE));
    	memset(Version,0,sizeof(Version));
    	memset(acqID,0,sizeof(acqID));
    	memset(LTID,0,sizeof(LTID));
    	memset(encMod,0,sizeof(encMod));
    	memset(LTWKID,0,sizeof(LTWKID));
    	memset(encCounter,0,sizeof(encCounter));
    	memset(protLength,0,sizeof(protLength));

    	MEM_ZERO(lenbit);
    	MEM_ZERO(LTWKDEK);
    	MEM_ZERO(buf);

    	MEM_ZERO(bufval);
    	bufInit(&val, bufval, sizeof(bufval));

    	memcpy(HTLE, "HTLE", 4);			// Hard code HTLE
    	memcpy(Version, "03", 2);             				// get Version
    	ret = bufApp(&val, (byte*)HTLE, 4);
	CHK;

	ret = bufApp(&val, (byte*)Version, 2);
	CHK;
	
    	MAPGET(appLtmkAcqID,acqID,lblKO);	    // get ACQ ID
	ret = bufApp(&val, (byte*)acqID, 3);
	CHK;
	
    	MAPGET(appTidTle, LTID, lblKO);
	ret = bufApp(&val, (byte*)LTID, 8);
    	CHK;	
	
    	encMod[0]= '2';           	// 1 for des_cbc, 2 for 3des_cbc
    	encMod[1]= '0';				// 0 unique key per terminal, 1 per transaction
    	encMod[2]= '0';				// MAC Algorithm. 0 ANSI X9.9 MAC, 1 MAC Using SHA-1 shasing
    	ret = bufApp(&val, (byte*)encMod, 3);
	CHK;

    	memset(buf,0,sizeof(buf));
    	ret = LTWKey(buf);

	memcpy(LTWKID, buf, 4);
    	memset(LTWKDEK, 0, sizeof(LTWKDEK));
    	memcpy(LTWKDEK, buf + 4, 16);

    	ret = bufApp(&val, (byte*)LTWKID, 4);
    	CHK;	
	
    	ret = getCounterTle(&val);
    	CHK;	

    	memset(dBit,0,sizeof(dBit));
    	bufInit(&bit, dBit, sizeof(dBit));

	ret = TLEData(DataIn, &bit);
    	CHECK(ret == TRUE, lblKO);	
    
    	num2dec(lenbit, bufLen(&bit), 3);
    	bufApp(&val, lenbit, 3);       							// protected Text Length
    	bufApp(&val, (byte*) "\x00\x00\x00\x00\x00", 5);       	// padding 5 bytes null

    	len = bufLen(&bit);
    	if (len % 8 != 0) 
	{
		len = (len + 7) / 8 ;
		len *= 8;
    	}

    	MEM_ZERO(buf);
    	MAPPUTSTR(traLTWKDEK, LTWKDEK, lblKO);
    	TDesCBC(bit.ptr, len, buf, LTWKDEK);
    	ret = bufApp(&val, buf, len);       			    // protected Text
    	CHK;

	PubLong2Char((ulong) bufLen(&val), 2, glSendPack.szBit57);
	memcpy(glSendPack.szBit57+2, bufPtr(&val), bufLen(&val));

    return ret;
  lblKO:
    return -1;
}


//-------------------------------------------------------
void getMAC(uchar *DataIn, int lenDataIn, uchar *MAC)
{
	int ret;
	byte buf[128+1];
	byte LTWKMAK[16+1];

	MEM_ZERO(buf);
	MEM_ZERO(LTWKMAK);
	
	memset(buf,0,sizeof(buf));
	ret = LTWKey(buf);

       	memset(LTWKMAK,0,sizeof(LTWKMAK));
       	memcpy(LTWKMAK,buf+20,16);

	PubCalcMac(MAC_ANSIX99, LTWKMAK, DataIn, lenDataIn, MAC);
}


static int parseBit57Tle(const byte *rsp, word len)
{
	byte tagRsp;
	byte flaglen;
	byte buflen;
	char buf[999+1];
	card length;
	word pos=0;
	byte stop= 0;
	char lenBin[2+1], lenHex[4+1];
//	byte Bitmap[lenBitmap+1];
	char bufhex[lenAmt+1];
	
	MEM_ZERO(bufhex);
	MEM_ZERO(&glRecvPack.szBit57);
	
	while(stop==0)
	{
		MEM_ZERO(buf);
		MEM_ZERO(lenBin);
		MEM_ZERO(lenHex);

		tagRsp=rsp[pos];
		switch(tagRsp)
		{
			case 0x02://bit 2
				buflen= rsp[pos+1];
				memcpy(buf, rsp+pos+2, buflen);
				mapPut(traRspBit_2, buf, buflen);
				mapPutWord(traRspLBit_2, buflen);
				pos+=(2+buflen);

				//				
				if(debugTLE==1)
				{
					mapGet(traRspBit_2, buf, lenBit2);
					mapGetWord(traRspLBit_2, buflen);
					hex_dump_char("traRspBit_2", buf, buflen);
				}
				memcpy(glRecvPack.szBit2, buf, buflen);
				break;
			case 0x04://bit 4
				buflen=rsp[pos+1];
				memcpy(buf, rsp+pos+2, buflen);

				bin2hex(bufhex, buf, buflen);
				mapPut(traRspAmount, bufhex, lenAmt);

				mapPut(traRspBit_4, buf, buflen);
				mapPutWord(traRspLBit_4, buflen);
				pos+=(2+buflen);

				
				if(debugTLE==1)
				{
					mapGet(traRspBit_4, buf, lenBit4);
					mapGetWord(traRspLBit_4, buflen);
					hex_dump_char("traRspBit_4", buf, buflen);
				}

				memcpy(glRecvPack.szBit4, buf, buflen);
				break;
			case 0x16://bit 14
				buflen=rsp[pos+1];
				memcpy(buf, rsp+pos+2, buflen);
				mapPut(traRspBit_14, buf, buflen);
				mapPutWord(traRspLBit_14, buflen);
				pos+=(2+buflen);

				//				
				if(debugTLE==1)
				{
					mapGet(traRspBit_14, buf, lenBit14);
					mapGetWord(traRspLBit_14, buflen);
					hex_dump_char("traRspBit_14", buf, buflen);
				}

				
				break;
			case 0x43://bit 35
				buflen=rsp[pos+1];
				memcpy(buf, rsp+pos+2, buflen);
				mapPut(traRspBit_35, buf, buflen);
				mapPutWord(traRspLBit_35, buflen);
				pos+=(2+buflen);

				//				
				if(debugTLE==1)
				{
					mapGet(traRspBit_35, buf, lenBit35);
					mapGetWord(traRspLBit_35, buflen);
					hex_dump_char("traRspBit_35", buf, buflen);
				}
				
				memcpy(glRecvPack.szBit35, buf, length);
				break;
			case 0x60://bit 48
				flaglen = rsp[pos+1];
				if(flaglen == 0x81)
				{
					length=rsp[pos+2];

					memcpy(buf, &rsp[pos + 3], length);

					pos+=(word)(3+length);
				}
				else if(flaglen == 0x82)
				{
					memcpy(lenBin, &rsp[pos + 2], 2);
					bin2hex(lenHex, lenBin, 2);
					hex2num(&length, lenHex, 4);

					memcpy(buf, &rsp[pos + 4], length);

					pos+=(word)(4+length);
				}
				else
				{
					length= rsp[pos+1];

					memcpy(buf, &rsp[pos + 2], length);

					pos+=(word)(2+length);
				}

				mapPut(traRspBit_48, buf, (word)length);
				mapPutWord(traRspLBit_48, (word)length);

				if(debugTLE==1)
				{
					hex_dump_char("traRspBit_48", buf, length);
					mapGet(traRspBit_48, buf, lenBit48);
					mapGetWord(traRspLBit_48, length);
				}
				memcpy(glRecvPack.szBit48, buf, length);
				break;
			case 0x67://bit 55
				flaglen = rsp[pos+1];
				if(flaglen == 0x81)
				{
					length=rsp[pos+2];

					memcpy(buf, &rsp[pos + 3], length);

					pos+=(word)(3+length);
				}
				else if(flaglen == 0x82)
				{
					memcpy(lenBin, &rsp[pos + 2], 2);
					bin2hex(lenHex, lenBin, 2);
					hex2num(&length, lenHex, 4);

					memcpy(buf, &rsp[pos + 4], length);

					pos+=(word)(4+length);
				}
				else
				{
					length= rsp[pos+1];

					memcpy(buf, &rsp[pos + 2], length);

					pos+=(word)(2+length);
				}

				mapPut(traRspBit_55, buf, (word)length);
				mapPutWord(traRspLBit_55, (word)length);
				
				if(debugTLE==1)
				{
					hex_dump_char("traRspBit_55", buf, length);
					mapGet(traRspBit_55, buf, lenBit55);
					mapGetWord(traRspLBit_55, length);
				}

				memcpy(glRecvPack.szBit55, buf, length);
				break;
			case 0x72:// bit 58
				flaglen = rsp[pos+1];
				if(flaglen == 0x81)
				{
					length=rsp[pos+2];

					memcpy(buf, &rsp[pos + 3], length);

					pos+=(word)(3+length);
				}
				else if(flaglen == 0x82)
				{
					memcpy(lenBin, &rsp[pos + 2], 2);
					bin2hex(lenHex, lenBin, 2);
					hex2num(&length, lenHex, 4);

					memcpy(buf, &rsp[pos + 4], length);

					pos+=(word)(4+length);
				}
				else
				{
					length= rsp[pos+1];

					memcpy(buf, &rsp[pos + 2], length);

					pos+=(word)(2+length);
				}

				mapPut(traRspBit_58, buf, (word)length);
				mapPutWord(traRspLBit_58, (word)length);

				if(debugTLE==1)
				{
					hex_dump_char("traRspBit_58", buf, length);
					mapGet(traRspBit_58, buf, lenBit58);
					mapGetWord(traRspLBit_58, length);
				}

				memcpy(glRecvPack.szBit58, buf, length);
				break;
			default:
				stop =1;
				break;
		}
	}

	return 1;
}


int ParseTLEData(void)
{
	int ret, idx = 0;
	char buffer[1024];
	word lenTleHead = 36;
	char dst[1024];
	char key[16+1];
//	byte TxnType;
	char iccData[lenBit55+1];
	word LenBit=0;
	uchar rsp[1024];
	ulong len=0;

	MEM_ZERO(buffer);
	MEM_ZERO(dst);
	MEM_ZERO(key);
	MEM_ZERO(iccData);
	MEM_ZERO(rsp);

	len=PubChar2Long(glRecvPack.szBit57, 2, NULL);
	if(len==0)
		return 0;
	
	memcpy(rsp, glRecvPack.szBit57+2, len);

	MEM_ZERO(&glRecvPack.szBit57);

	if(debugTLE==1)
		hex_dump_char("Bit 57", rsp, len);
	
	memcpy(buffer, rsp+lenTleHead, len-lenTleHead);

	MAPGET(traLTWKDEK, key, lblKO);
	decTDesCBC(buffer, len - lenTleHead, dst, key); 

	if(debugTLE==1)
		hex_dump_char("Bit57Rsp", dst, len - lenTleHead);


	//parse bit 57 (tle)
	parseBit57Tle(dst, (word)(len-lenTleHead));
	
	return 1;
lblKO:
	return -1;
}
