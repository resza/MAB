
#include "global.h"

static int getInputCode(byte * inpCode){
	int ret;
	char serinfo[32+1];
	char appver[4+1];
	char ranNum[4+1];
	char temp[20+1];
	char temp2[4+1];
	char temp3[4+1];
	card temp2Num;
	card temp3Num;
	card tot;

	MEM_ZERO(serinfo);
	ReadSN(temp);
	MAPGET(appVersion, appver,lblKO);

	memset(ranNum,0,sizeof(ranNum));
	memset(temp,0,sizeof(temp));

    //set random number

	MEM_ZERO(temp);
	GetDateTime(temp);
	memcpy(temp2, temp + 8, 4);
	memcpy(temp3, serinfo + 4, 4);
	dec2num(&temp2Num,temp2,4);
	dec2num(&temp3Num,temp3,4);
	tot = (2*temp2Num) + temp3Num;
	num2dec(ranNum,tot,4);

	//copy to inpCode
	memcpy(inpCode,serinfo, 8);
	memcpy(inpCode + 8,appver, 4);
	memcpy(inpCode + 12, ranNum, 4);

	return 1;
	lblKO:
	return -1;

}

static void fmtInputCode(byte * inpfmt){
	//int ret;
	byte resXor[16+1];
	char temp1[16+1];
	char temp2[16+1];
	byte rsp[128+1];

	getInputCode(rsp);

	memset(temp1,0,sizeof(temp1));
	memset(temp2,0,sizeof(temp2));

	//set first temp
	memcpy(temp1,rsp+8,4);       // app version
	memcpy(temp1+4,rsp,8);		 // sernum edc
	memcpy(temp1+12,rsp+12,4);	 // random

	//set second temp
	memcpy(temp2,"2222",4);
	memcpy(temp2+4,rsp+8,4);     // app version
	memcpy(temp2+8,rsp+12,4);    // random
	memcpy(temp2+12,"2222",4);

	memset(resXor,0,sizeof(resXor));
	xor((byte *) temp1, (byte *) temp2, resXor, 16) ;
	memcpy(inpfmt,resXor,16);
	//hex_dump_char("input xor", resXor, 16);

 }

static void fmtkeyCode(byte * inpCode, char *inputText){
	byte resXor[16+1];
	char temp1[16+1];
	char temp2[16+1];
	char rsp[128+1];
	
	getInputCode(rsp);
	memcpy(inputText, rsp, 16);
	
	memset(temp1,0,sizeof(temp1));
	memset(temp2,0,sizeof(temp2));

	//set first temp
	memcpy(temp1,rsp+8,4);			//app version
	memcpy(temp1+4,rsp,8);			//sernum edc
	memcpy(temp1+12,"1111",4);

	//set second temp
	memcpy(temp2,"1111",4);
	memcpy(temp2+4,rsp+12,4);		//random
	memcpy(temp2+8,rsp,8);			//sernum edc

	memset(resXor,0,sizeof(resXor));
	xor((byte *) temp1, (byte *) temp2, resXor, 16) ;
	memcpy(inpCode,resXor,16);
	//hex_dump_char("key xor", resXor, 16);
}

int checkCode(){
	int ret;
	byte input[16+1];
	byte key[16+1];
	byte code[16+1];
	char buf[16+1];
	char buf2[6+1];
	byte check[3+1];
	char display[16+1];
	int numError;
	int iRetError;
	uchar ucRet;

	memset(input,0,sizeof(input));
	fmtInputCode(input);

	memset(key,0,sizeof(key));
	MEM_ZERO(display);
	fmtkeyCode(key,display);

	TDesCBC((byte*) input, 16, code, (byte *) key);
	memcpy(check, code + 7, 3);
	bin2hex(buf2, check, 3);

	//type code from admin calculator
	numError = 0;
	ret = 0;
	iRetError = 0;

	while ((ret != 1) && (ret != -1 )){
		memset(buf, 0, dspW + 1);
		dspClear();
		dspLS(1, display);
		ucRet = PubGetString(ALPHA_IN, 6, 6, buf, TIME_OUT_INPUT);
		if (ucRet == NOKEY) {
			iRetError = iRetError + 1;

			if (iRetError == 3){
				goto lblKO;
			}
			else{
				continue;
			}
		}
		else if(ucRet == KEYCANCEL)
			return ERR_USERCANCEL;

		iRetError = 0;

		if (memcmp(buf2, buf, 6) == 0) {
			dspClear();
			dspLS(1, "EDC UNLOCKED");
			tmrPause(2);
			ret =  1;
			}
		else {
			dspLS(1, "WRONG INPUT");
			numError = numError + 1;

			if (numError == 3){
				tmrPause(2);
				ret =  -1;

			}
			else{
				dspClear();
				dspLS(1, "INPUT AGAIN");
				tmrPause(2);
			}
		}
	}

    return 0;	
    lblKO:
      return -1;

}


#ifdef _DEVELOPMENT_			
static void InputCodeCalc(byte * inpfmt, char *rsp){
	//int ret;
	byte resXor[16+1];
	char temp1[16+1];
	char temp2[16+1];
	
	memset(temp1,0,sizeof(temp1));
	memset(temp2,0,sizeof(temp2));

	//set first temp
	memcpy(temp1,rsp+8,4);       // app version
	memcpy(temp1+4,rsp,8);		 // sernum edc
	memcpy(temp1+12,rsp+12,4);	 // date ddmm

	//set second temp
	memcpy(temp2,"2222",4);
	memcpy(temp2+4,rsp+8,4);     // app version
	memcpy(temp2+8,rsp+12,4);    // date ddmm
	memcpy(temp2+12,"2222",4);

	memset(resXor,0,sizeof(resXor));
	xor((byte *) temp1, (byte *) temp2, resXor, 16) ;
	memcpy(inpfmt,resXor,16);
	//hex_dump_char("input xor", resXor, 16);

 }


static void KeyCodeCalc(byte * inpCode, char *rsp){
	byte resXor[16+1];
	char temp1[16+1];
	char temp2[16+1];
		
	memset(temp1,0,sizeof(temp1));
	memset(temp2,0,sizeof(temp2));

	//set first temp
	memcpy(temp1,rsp+8,4);			//app version
	memcpy(temp1+4,rsp,8);			//sernum edc
	memcpy(temp1+12,"1111",4);

	//set second temp
	memcpy(temp2,"1111",4);
	memcpy(temp2+4,rsp+12,4);		//date ddmm
	memcpy(temp2+8,rsp,8);			//sernum edc

	memset(resXor,0,sizeof(resXor));
	xor((byte *) temp1, (byte *) temp2, resXor, 16) ;
	memcpy(inpCode,resXor,16);
	//hex_dump_char("key xor", resXor, 16);
}


int CheckCodeCalc(char *buffer)
{
	int ret=1;
	byte input[16+1];
	byte key[16+1];
	byte code[16+1];
	byte check[3+1];

	memset(input,0,sizeof(input));
	InputCodeCalc(input, buffer);

	memset(key,0,sizeof(key));	
	KeyCodeCalc(key, buffer);

	TDesCBC((byte*) input, 16, code, (byte *) key);
	memcpy(check, code + 7, 3);
	hex_dump_char("Activation Code", check, 4);	

    return ret;	    

}
#endif
