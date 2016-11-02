#include "global.h"

#define CLA_MAN		    0x90                 //SAM MANDIRI
#define INS_SESSION_MAN 0x10

#define P1_DEFAULT		0x00
#define P2_DEFAULT		0x00
#define P3_DEFAULT		0x00

#define SAMMandiri  0x00					 //SAM MANDIRI

ST_RSA_KEY sRsa2=
  {
  	1024,  	//iModulusLen
	"\xA4\xF7\xAF\x33\x78\x45\x24\xAA\xC7\x13\x4F\xBD\x1F\xF0\x7A\xB3\x87\x0F\xCB\xDD\x3D\xBF\x91\x27\x56\xE6\x27\x03\x5F\x7E\x9B\xDF\x45\xEC\xED\x82\x18\xF9\x0C\xC1\x90\xD7\xEF\x3E\x58\xB3\x7D\x82\x5D\x06\x15\xE4\x10\xCF\x85\x41\x00\x0C\x0A\x0B\xD9\x45\x07\xE9\x0A\x12\x91\x08\x86\x68\xDF\xDD\x68\xFE\x1A\x8B\x02\x30\xC3\x86\xD6\x40\xDF\x82\x6F\xCA\x3D\xD8\x8C\x17\xC4\x43\xF4\xE9\xC6\xD4\xE7\xCC\xC7\x05\xB3\x72\x8D\xB2\xB7\xBB\x7E\x90\x8A\x92\x3E\x2B\x15\x88\xF5\x89\xA1\x9C\x37\xB1\x68\x88\x34\x31\x1D\xEB\xB7\xF5", //production  
	1024,	//iExponentLen
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x02",
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  };

byte RSA_mem[20480];
byte rsa_input[128];
byte rsa_output[128];




/*
 *
 */

int DisplayErrorIsoCommand(uchar error)
{
	char buffer[24];

	MEM_ZERO(buffer);

//	sprintf(buffer, "ERROR CODE %02x %02x", ApduResp->SWA, ApduResp->SWB);
	ScrCls();
	ScrPrint(17, 0, ASCII, "READER ERROR = %02x", error);
	switch(error)
	{
		case 0x01:
			PubDispString("Communication", DISP_LINE_CENTER|2);
			PubDispString("Timeout", DISP_LINE_CENTER|4);
			break;
		case 0x02:
			PubDispString("Card removed", DISP_LINE_CENTER|2);
			PubDispString("during transaction", DISP_LINE_CENTER|4);
			break;
		case 0x03:
			PubDispString("Parity error", DISP_LINE_CENTER|2);
			break;
		case 0x04:
			PubDispString("Slot selection", DISP_LINE_CENTER|2);
			PubDispString("error", DISP_LINE_CENTER|4);
			break;
		case 0x05:
			PubDispString("Data sent", DISP_LINE_CENTER|2);
			PubDispString("too long(LC)", DISP_LINE_CENTER|4);
			break;
		case 0x06:
			PubDispString("Card protocol", DISP_LINE_CENTER|2);
			PubDispString("error", DISP_LINE_CENTER|4);
			break;
		case 0x07:
			PubDispString("Card reset", DISP_LINE_CENTER|2);
			PubDispString("failure", DISP_LINE_CENTER|4);
			break;
		case 0xFF:
			PubDispString("Cannot establish", DISP_LINE_CENTER|2);
			PubDispString("communication", DISP_LINE_CENTER|4);
			PubDispString("no power", DISP_LINE_CENTER|6);
			break;
	}
	Beep();
	PubWaitKey(3);
	return -1;
}


/*
 *
 */

int DisplayErrorKekSAM(APDU_RESP *ApduResp)
{
	char buffer[24];

	MEM_ZERO(buffer);

//	sprintf(buffer, "ERROR CODE %02x %02x", ApduResp->SWA, ApduResp->SWB);
	ScrCls();
	ScrPrint(17, 0, ASCII, "ERROR CODE %02x %02x", ApduResp->SWA, ApduResp->SWB);
//	PubDispString(buffer, DISP_LINE_CENTER|0);
	if((ApduResp->SWA==0x63)&&(ApduResp->SWB==0x00))
	{
		
		PubDispString("AUTHENTICATION", DISP_LINE_CENTER|2);
		PubDispString("FAILED", DISP_LINE_CENTER|4);
	}
	else if((ApduResp->SWA==0x6A)&&(ApduResp->SWB==0x82))
	{
		PubDispString("FILE NOT FOUND", DISP_LINE_CENTER|2);
		PubDispString("IN THE SELECTED", DISP_LINE_CENTER|4);
		PubDispString("APPLICATION", DISP_LINE_CENTER|6);
	}		
	else
	{
		PubDispString("", DISP_LINE_CENTER|2);
		PubDispString("ERROR", DISP_LINE_CENTER|4);
	}
	Beep();
	PubWaitKey(3);
	return -1;
}

int SAMMANDIRI_Start_Session(byte *DataIn, byte p1, byte *rsp )
{
	byte cmd[6+1];
	byte dat[136+2]; 			// first byte is length of dat (16 bytes)
	uchar ucRet;

	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);
	MEM_ZERO(cmd);
	memset(dat,0x00,sizeof(dat));

	ApduSend.Command[0]= CLA_MAN;				//STAPDUCMD.CLA
	ApduSend.Command[1]= INS_SESSION_MAN;		//STAPDUCMD.INS
	ApduSend.Command[2]= 1;					//STAPDUCMD.P1
	ApduSend.Command[3]= P2_DEFAULT;			//STAPDUCMD.P2

	ApduSend.Lc = 136;

	memcpy(ApduSend.DataIn, DataIn, 136);

	ApduSend.Le = 8;
		
	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			return 0;
		}
		else
			DisplayErrorKekSAM(&ApduResp);
	}
	

	return -1;
}


int selectAID(byte* DataIn, byte p2, byte* rsp){
	byte cmd[6+1];
	byte dat[24+2]; 			// first byte is length of dat (16 bytes)
	int ucRet;

	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	MEM_ZERO(cmd);
	MEM_ZERO(dat);
	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);
	
	ApduSend.Command[0] = 0x00;				//STAPDUCMD.CLA
	ApduSend.Command[1] = 0xA4;				//STAPDUCMD.INS
	ApduSend.Command[2] = 0x04;		    	//STAPDUCMD.P1
	ApduSend.Command[3] = p2;					//STAPDUCMD.P2

	ApduSend.Lc = 16;  					//5STAPDUCMD.P3

	memcpy(ApduSend.DataIn, DataIn, 16);                 	// 3 bytes of LMTK_ACQ_ID
	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			return 0;
		}
		else
			DisplayErrorKekSAM(&ApduResp);
	}
	

	return -1;

}


/*
 * Until now, only SAM MANDIRI using this function
 * This function is used to verify the TE(terminal Engineer) PIN
 * return code :
 * - 90 00
 * - 63 Cn  > Incorrect PIN, 'n' represent the number of tries left.
 * - 63 00  > Authentification failed
 * - 67 00  > incorrect length
 * - 6B 00  > incorrect P1 or P2 parameter
 * - 6E 00  > CLA not Support
 * - 90 15  > No Secure session establish
 */
int ui_PIN_Authenticate(byte* encryPIN, byte p1, byte* rsp)
{
	byte cmd[6+1];
	byte dat[12+1]; 			// first byte is length of dat (16 bytes)
	uchar ucRet;

	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	memset(cmd, 0x00, sizeof(cmd));
	memset(dat, 0x00, sizeof(dat));
	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);
	
	ApduSend.Command[0]= 0x94;				//STAPDUCMD.CLA
	ApduSend.Command[1]= 0x12;				//STAPDUCMD.INS
	ApduSend.Command[2]= p1;					//STAPDUCMD.P1
	ApduSend.Command[3]= P2_DEFAULT;		//STAPDUCMD.P2

	ApduSend.Lc = 8;
	memcpy(ApduSend.DataIn, encryPIN, 8);

	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			return 0;
		}
		else
			DisplayErrorKekSAM(&ApduResp);
	}
	else
		DisplayErrorIsoCommand(ucRet);


	return -1;
}

/*
 * This function is used to change the TEPIN
 */
int PIN_Change(byte* DataIn, byte p1, byte* rsp)
{
	byte cmd[6+1];
	byte dat[24+2]; 			// first byte is length of dat (16 bytes)
	uchar ucRet;
	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	MEM_ZERO(cmd);
	MEM_ZERO(dat);
	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);

	ApduSend.Command[0]= 0x94;				//STAPDUCMD.CLA
	ApduSend.Command[1]= 0x17;				//STAPDUCMD.INS
	ApduSend.Command[2]= p1;					//STAPDUCMD.P1
	ApduSend.Command[3]= P2_DEFAULT;			//STAPDUCMD.P2
	ApduSend.Lc = 24;								

	memcpy(ApduSend.DataIn, DataIn, 24);

	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			return 0;
		}
		else
			DisplayErrorKekSAM(&ApduResp);
	}
	else
		DisplayErrorIsoCommand(ucRet);


	return -1;
}


/*
 * Until now, only SAM MANDIRI using this function
 * LMTK > Line encryption Terminal Master Key.
 * This function is used to obtain the LMTK token request data from the card.
 * The generated LMTK token request data will be stored in temporary memory during
 *  the card session.
 * it will be used later for the command LMTK token decrypt processing.
 * 56 bytes of KEK LMTK DATA
 * Using 3des cbc
 *
 * Condition :
 * - Card initialization has been done.
 * - KEK initialization has been done.
 * - The command requires that a secure session must already established.
 * - TE PIN authentication completed.
 * - The LTMK_ACQ_ID and VENDOR_ID must be the same as the one in the card.
 *
 * return code :
 * - 90 00
 * - 67 00  > Incorrect Length
 * - 69 82  > Security status
 * - 69 85  > Condition of use not satisfied
 * - 6A 80  > Invalid Data
 * - 6E 00  > CLA not Support
 * - 90 11  > Card not initialized
 * - 90 12  > KEK not initialized
 * - 90 13  > En/Decryption failed
 * - 90 14  > No session or wrong session established
 * - 90 15  > Key generation failed
 */
int LMTK_Token_ReqGeneration(byte* DataIn, byte* rsp)
{
	byte cmd[6+1];
	byte dat[25+1]; 			// first byte is length of dat (16 bytes)
	uchar ucRet;

	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	MEM_ZERO(cmd);
	MEM_ZERO(dat);
	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);

	ApduSend.Command[0]= 0x94;				//STAPDUCMD.CLA
	ApduSend.Command[1]= 0x18;				//STAPDUCMD.INS
	ApduSend.Command[2]= P1_DEFAULT;			//STAPDUCMD.P1
	ApduSend.Command[3]= P2_DEFAULT;			//STAPDUCMD.P2

	ApduSend.Lc = 24;

	memcpy(ApduSend.DataIn, DataIn, 24);

	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			return 0;
		}
		else
			DisplayErrorKekSAM(&ApduResp);
	}
	else
		DisplayErrorIsoCommand(ucRet);


	return -1;
}


/*
 * this function is used to decrypt the LTMK from LTMK token response from
 * of the previous LTMK Token request generation command.
 *
 * Note :
 * A. LTMK Token response data, KMS-LTMK-DATA, is returned from KMS in LTMK download
 *    response message
 * B. Terminal is required to store LTMK ID along with the LTMK.
 *
 * Condition :
 * -  Card Initialization has been done
 * -  KEK Initialization has been done
 * -  The Command requires that a secure session must be already established
 * -  TEPIN authentication completed
 * -  LTMK Token Request generation has been done for this card session.
 */
int LMTK_Token_Decrypt(byte* DataIn, byte* rsp)
{
	byte cmd[6+1];
	byte dat[40+2]; 			// first byte is length of dat (16 bytes)
	uchar ucRet;
	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	MEM_ZERO(cmd);
	MEM_ZERO(dat);
	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);

	ApduSend.Command[0]= 0x94;				//STAPDUCMD.CLA
	ApduSend.Command[1]= 0x19;				//STAPDUCMD.INS
	ApduSend.Command[2]= P1_DEFAULT;			//STAPDUCMD.P1
	ApduSend.Command[3]= P2_DEFAULT;			//STAPDUCMD.P2

	ApduSend.Lc = 40;							//5STAPDUCMD.P3

	memcpy(ApduSend.DataIn, DataIn, 40);                 	// 3 bytes of LMTK_ACQ_ID
	ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

	if(ucRet==0)
	{
		if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
		{
			memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
			return 0;
		}
		else
			DisplayErrorKekSAM(&ApduResp);
	}
	else
		DisplayErrorIsoCommand(ucRet);
	
	return ucRet;
}


/*
 * Until now, only SAM MANDIRI using this function
 * This function is used to terminate a secure channel between the card and the CAD
 * return code :
 * - 90 00
 * - 65 81  > memory failure
 */

int uiEnd_Session(byte ChipType, byte *rsp )
{
	byte cmd[6+1];
	uchar ucRet = 0;

	APDU_SEND	ApduSend;
	APDU_RESP	ApduResp;

	MEM_ZERO(&ApduSend);
	MEM_ZERO(&ApduResp);
	MEM_ZERO(cmd);

	switch(ChipType)
	{
		case SAMMandiri:  //ATR 3B7B1800000031C0......

			ApduSend.Command[0]= CLA_MAN;			//STAPDUCMD.CLA
			ApduSend.Command[1]= 0x11;	//STAPDUCMD.INS
			ApduSend.Command[2]= P1_DEFAULT;		    //STAPDUCMD.P1
			ApduSend.Command[3]= P2_DEFAULT;			//STAPDUCMD.P2

			ApduSend.Lc = 0;
			
			ucRet = IccIsoCommand(0x80, &ApduSend, &ApduResp);

			if(ucRet==0)
			{
				if((ApduResp.SWA==0x90) && (ApduResp.SWB==0x00))
				{
					memcpy(rsp, ApduResp.DataOut, ApduResp.LenOut);
					return 0;
				}
				else
					DisplayErrorKekSAM(&ApduResp);
			}
			else
				DisplayErrorIsoCommand(ucRet);

			break;

		default :
			 break;
	}

	return -1;
}

int LTMKey(byte *LTMK){
		int ret;
    	byte key[16+1];
    	char temp[64+1];
    	byte buf[24+1];
    	byte buf2[24+1];

    	MEM_ZERO(key);
    	MEM_ZERO(temp);
    	memset(buf,0,sizeof(buf));
    	memset(buf2,0,sizeof(buf2));

	MAPGET(appKeySessionTle, key, lblKO);
//	memcpy(key, glSysCtrl.stTlmApp.appKeySessionTle, sizeof(key));
	MAPGET(appLTMK, temp, lblKO);
//	memcpy(temp, glSysCtrl.stTlmApp.appLTMK, sizeof(glSysCtrl.stTlmApp.appLTMK));
	
    	hex2bin(buf, temp, 24);

    	decTDesCBC(buf, 24, buf2, key);
    	memcpy(LTMK, buf2, 24);

    return 1;
lblKO:
    return -1;

}

int LTWKey(byte *LTWK){
		int ret;
    	byte key[16+1];
    	char temp[128+1];
    	byte buf[64+1];
    	byte LTWKDEK[16+1];
    	byte LTWKMAK[16+1];

    	MEM_ZERO(key);
    	MEM_ZERO(temp);
    	MEM_ZERO(LTWKDEK);
    	MEM_ZERO(LTWKMAK);	
	
    	memset(buf, 0, sizeof(buf));
    	LTMKey(buf);
    	memset(key, 0, sizeof(key));
    	memcpy(key, buf + 4, 16);

    	memset(buf, 0, sizeof(buf));
    	memset(temp, 0, sizeof(temp));
	MAPGET(appLTWKTID1, temp, lblKO);
//	memcpy(temp, glSysCtrl.stTlmApp.appLTWKTID1, sizeof(glSysCtrl.stTlmApp.appLTWKTID1));
	
    	hex2bin(buf, temp, 52);
    	memcpy(LTWK, buf, 4);

    	memset(LTWKDEK,0,sizeof(LTWKDEK));
    	decTDesCBC(buf + 4, 16, LTWKDEK, key);
    	memcpy(LTWK + 4, LTWKDEK, 16);

    	memset(LTWKMAK,0,sizeof(LTWKMAK));
    	decTDesCBC(buf + 20, 16, LTWKMAK, key);
    	memcpy(LTWK + 20, LTWKMAK, 16);


    	return 1;
lblKO:
	return -1;
}


static int getBit62LTMK(tBuffer *val) {
     	int ret;
    	char LTMK[112 + 1];
    	byte buf[63+1];
    	char HTLE[4+1];
    	char Version[2+1];
    	char TypeDLoad[1+1];

    	memset(LTMK, 0, sizeof(LTMK));
    	memset(buf, 0, sizeof(buf));
    	memset(HTLE, 0, sizeof(HTLE));
    	memset(Version, 0, sizeof(Version));
    	memset(TypeDLoad, 0, sizeof(TypeDLoad));

//	memcpy(LTMK, glSysCtrl.stTlmApp.appLTMKDATA, sizeof(LTMK));

	MAPGET(appLTMKDATA, LTMK, lblKO);

    	memcpy(HTLE, "HTLE", 4);
    	memcpy(Version, "03", 2);
    	memcpy(TypeDLoad, "0", 1);

    	memcpy(buf, (byte*) HTLE, 4);
    	memcpy(buf + 4, (byte*)Version, 2);
    	memcpy(buf + 6, (byte*)TypeDLoad, 1);

    	hex2bin(buf+7, LTMK, 56);
	ret = bufApp(val, (byte *) buf, 63);
    	CHK;

    	return ret;
  lblKO:
    	return -1;
}


static int getBit62LTWK(tBuffer * val) {
    	int ret;
    	char temp[48 + 1];
    	byte buf[40+1];
    	byte buf2[24+1];
    	char HTLE[4+1];
    	char Version[2+1];
    	char reqType[1+1];
    	char LTMKACQID[3+1];
    	char acqID[3+1];
    	char LTID[8+1];
    	char vendorID[8+1];
    	char LTMKID[4+1];
    	char LTWKID[4+1];
    	byte key[16+1];
    	byte decLTMKDecryptRsp[24+1];

    	memset(temp,0,sizeof(temp));
    	memset(buf,0x20,sizeof(buf));
    	memset(buf2,0,sizeof(buf2));
    	memset(HTLE,0,sizeof(HTLE));
    	memset(Version,0,sizeof(Version));
    	memset(reqType,0,sizeof(reqType));
    	memset(LTMKACQID,0,sizeof(LTMKACQID));
    	memset(acqID,0,sizeof(acqID));
    	memset(LTID,0,sizeof(LTID));
    	memset(vendorID,0,sizeof(vendorID));
    	memset(LTMKID,0,sizeof(LTMKID));
    	memset(LTWKID,0,sizeof(LTWKID));
    	memset(key,0,sizeof(key));
    	memset(decLTMKDecryptRsp,0,sizeof(decLTMKDecryptRsp));

    	memcpy(HTLE,"HTLE",4);
    	memcpy(Version,"03",2);
    	memcpy(reqType,"1",1);
    	memcpy(LTMKACQID,"001",3);
    	memcpy(acqID,"001",3);
    	MAPGET(appTidTle, LTID, lblKO);
//	memcpy(LTID, glSysCtrl.stTlmApp.appTidTle, sizeof(LTID));
	
    	memcpy(vendorID,"00000001",8);

    	memcpy(buf,(byte*) HTLE,4);
    	memcpy(buf+4,(byte*)Version,2);
    	memcpy(buf+6,(byte*)reqType,1);
    	memcpy(buf+7,(byte*)LTMKACQID,3);
    	memcpy(buf+10,(byte*)acqID,3);
    	memcpy(buf+13,(byte*)LTID,8);
    	memcpy(buf+21,(byte*)vendorID,8);

    	ret = LTMKey(buf2);
    	memcpy(LTMKID,buf2,4);
    	memcpy(buf+29,(byte*) LTMKID,4);

    	memcpy(LTWKID,LTMKID,4);
    	memcpy(buf+33,(byte*) LTWKID,4);

    	ret = bufApp(val, (byte *) buf, 37);
    	CHK;

    	return ret;
  lblKO:
    	return -1;
}




// set bit content of ISO8583 TMS TELIUM.
void SetCommReqFieldLTMK(void)
{
	uchar sn[32+1];
	uchar buffer[1024];
	tBuffer val;
	
	memset(sn, 0, sizeof(sn));
	memset(buffer, 0, sizeof(buffer));
	bufInit(&val, buffer, sizeof(buffer));
	
	memset(&glSendPack, 0, sizeof(STISO8583));
	
//MTI
	sprintf((char *)glSendPack.szMsgCode, "%.*s", LEN_MSG_CODE, "0800");

//BIT 3
	sprintf((char *)glSendPack.szBit3,   "%.*s", lenBit3, "970000");

//BIT 11	
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	
//BIT 24
	sprintf((char *)glSendPack.szBit24,    "%.*s", lenBit24,     glCurTlmTmsAcq.acqNII);

//BIT41
	sprintf((char *)glSendPack.szBit41, "%.*s", lenBit41, glCurTlmTmsAcq.acqTID);

//BIT42	(special for TMS)
	sprintf((char *)glSendPack.szBit42, "%.*s", lenBit42, glCurTlmTmsAcq.acqMID);

//BIT 62 
	getBit62LTMK(&val);
	PubLong2Char((ulong)bufLen(&val), 2, glSendPack.szBit62);
	memcpy(&glSendPack.szBit62[2], bufPtr(&val), bufLen(&val));

}


// set bit content of ISO8583.
void SetCommReqFieldLTWK(void)
{
	uchar sn[32+1];
	uchar buffer[1024];
	tBuffer val;
	
	memset(sn, 0, sizeof(sn));
	memset(buffer, 0, sizeof(buffer));
	bufInit(&val, buffer, sizeof(buffer));
	
	memset(&glSendPack, 0, sizeof(STISO8583));
	
//MTI
	sprintf((char *)glSendPack.szMsgCode, "%.*s", LEN_MSG_CODE, "0800");

//BIT 3
	sprintf((char *)glSendPack.szBit3,   "%.*s", lenBit3, "920000");

//BIT 11	
	sprintf((char *)glSendPack.szBit11, "%06lu", glSysCtrl.ulSTAN);  //??
	
//BIT 24
	sprintf((char *)glSendPack.szBit24,    "%.*s", lenBit24,     glCurTlmTmsAcq.acqNII);

//BIT41
	sprintf((char *)glSendPack.szBit41, "%.*s", lenBit41, glCurTlmTmsAcq.acqTID);

//BIT42	(special for TMS)
	sprintf((char *)glSendPack.szBit42, "%.*s", lenBit42, glCurTlmTmsAcq.acqMID);

//BIT 62 
	getBit62LTWK(&val);
	PubLong2Char((ulong)bufLen(&val), 2, glSendPack.szBit62);
	memcpy(&glSendPack.szBit62[2], bufPtr(&val), bufLen(&val));

}


int loadTleKey(char *Modulus, char *exponent)
{
	int ret;
	word idx;
	byte RID[1 + lenRid];
	byte RSAKey[1 + lenRSAKey];
	char Exp[6+1];
	char PinSamCard[8+1];

	MEM_ZERO(RID);
	MEM_ZERO(RSAKey);
	MEM_ZERO(Exp);
	MEM_ZERO(PinSamCard);

	for (idx = 0; idx < glTlmTmsPub.ucPubNum; idx++)
	{    // Check if the AID and the CAPK index from the card is part of the CAPK multi records table of the application
		memcpy(RID, glTlmTmsPub.Pub[idx].pubRid, sizeof(glTlmTmsPub.Pub[idx].pubRid));
		if(memcmp(&RID[1], "SWM1I", lenRid) != 0)   // RID from table == AID from card (5 first digits)?
			continue;           // No, move to the next record
		break;	
	}

	MEM_ZERO(RSAKey);
	memcpy(RSAKey, glTlmTmsPub.Pub[idx].pubkeyData, sizeof(RSAKey));
	memcpy(Modulus, &RSAKey[1], RSAKey[0]);


	memcpy(Exp, glTlmSysParam.Con.appExponent, 6);
	hex2bin(exponent, Exp, 6);

	MEM_ZERO(PinSamCard);
	memcpy(PinSamCard, RSAKey + 129, 8);
	
	ret = 1;
	goto lblEnd;

lblEnd:
	return ret;
}

int doRSA(char *SessionKey, byte *EncSessionKey)
{
    	byte tmp[128];
    	char SSK[16+1];
    	int ret;
    	byte temp2[16+1];
    	byte keyCheckin[8+1];
    	byte keyCheckOut[16+1];
    	char exponent[4+1];	
    	ST_RSA_KEY sRsa;
    	byte rsaModulus[128];
    	byte rsaPublicExp[128];


	memset(&sRsa, 0x00, sizeof(sRsa));
	sRsa.iModulusLen= sRsa2.iModulusLen;
	memcpy(sRsa.aucModulus, sRsa2.aucModulus, 128);
//	memcpy(sRsa.aucModulus+(512-(sRsa.iModulusLen/8)), sRsa2.aucModulus, (sRsa2.iModulusLen/8));
	
	sRsa.iExponentLen= sRsa2.iExponentLen;
	memcpy(sRsa.aucExponent, sRsa2.aucExponent, 128);
//	memcpy(sRsa.aucExponent+(512-(sRsa.iExponentLen/8)), sRsa2.aucExponent, (sRsa2.iExponentLen/8));

// init rsa lib memory
	memset(RSA_mem, 0x00, sizeof(RSA_mem));

 // encrypt using public key
	memset(rsa_input, 0x00, sizeof(rsa_input));
	memset(SSK, 0x00, sizeof(SSK));
	generateSessionKey(SSK);
	memcpy(SessionKey, SSK, 16);

	MAPPUTSTR(appKeySessionTle, SSK, lblKO);
//	memcpy(glSysCtrl.stTlmApp.appKeySessionTle, SSK, 16);
//	SaveSysCtrlBase();


	memcpy(rsa_input + 112, (byte*)SSK, 16);

	ret = loadTleKey(sRsa.aucModulus, exponent);
	CHECK(ret > 0, lblKO);
	memcpy(sRsa.aucExponent+ 125, exponent, 3);

	memcpy(rsaModulus, sRsa.aucModulus, sizeof(rsaModulus));	//1024 bits
	memcpy(rsaPublicExp, sRsa.aucExponent, sizeof(rsaPublicExp));	//1024 bits
	
	ret = RSARecover(rsaModulus, 128, rsaPublicExp, 128, rsa_input, tmp);
	CHECK(ret==0, lblKO);
	
	memcpy(EncSessionKey, (byte*)tmp, 128);

	memset(keyCheckin, 0x00, sizeof(keyCheckin));
	memset(keyCheckOut, 0x00, sizeof(keyCheckOut));

	memset(temp2, 0x00, sizeof(temp2));
	memcpy(temp2, (byte*)SSK, 16);
	TDesCBC(keyCheckin, 8, keyCheckOut, temp2);

	memcpy(EncSessionKey + 128, (byte*) keyCheckOut, 4);

	return ret;
	lblKO:
		return -1;
}


int generateSessionKey(char * SSK){
	int ret;
	char temp[16+1];
	char sessionKey[16+1];

	memset(sessionKey,0,sizeof(sessionKey));
	memset(temp,0,sizeof(temp));

	GetDateTime(temp);
	memcpy(sessionKey,temp,12);
	memcpy(sessionKey+12,temp,4);
	memcpy(SSK,sessionKey,16);

	ret = 1;
	return ret;
}

int Bit62Req_download_LTMK(byte *rsp){
	char data[1024];
	char TLE_Indicator[4+1];
	char Version[2+1];
	char DLoadType[1+1];

	memset(data,0,sizeof data);
	memset(TLE_Indicator,0,sizeof TLE_Indicator);
	memset(Version,0,sizeof Version);
	memset(DLoadType,0,sizeof DLoadType);

	return 1;
}


void Req_PIN_Change(void){
	byte Buf[128 * 3];          // Buffer to accept the input
    	int ret, icc = 0;
    	byte TEPIN[16+1];
    	char key[16+1];
    	char SessionKey[16+1];
    	byte EncSessionKey[136+1];
    	char LTMKACQID[3+1];
    	char VENDORID[8+1];
    	char NEWPIN[8+1];
    	char LTMKReqDataIn[24+1];
    	char LTMKReqEncDataIn[24+1];
    	char temp[16+1];
	uchar ucRet;

	ScrCls();
	PubDispString("INSERT",  DISP_LINE_CENTER|2);
	PubDispString("KEK SAM",  DISP_LINE_CENTER|4);

    	TimerSet(0, 300);       // 300 = 30 s //added by irf, 

	IccClose(0);
	while( 1 )
	{
		if ((kbhit()==0) && (getkey()==KEYCANCEL))
		{
			return;
		}
		if( IccDetect(ICC_USER)==0 )
		{
			break;
		}
		if(!TimerCheck(0)) 	//timeout
       	{           
			return;
       	}
	}

    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
	ucRet = IccInit(0x80, (uchar *) Buf);
	CHECK(ucRet == 0, lblKO);

    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
    	ucRet = selectAID("\xA0\x00\x00\x00\x18\x4b\x65\x6b\x53\x61\x6d\x00\x00\x01\x01\x02", 0, Buf);
	CHECK(ucRet == 0, lblKO);	


    	//---------------------------------------------------------------
    	memset(SessionKey, 0, sizeof(SessionKey));
    	memset(EncSessionKey, 0, sizeof(EncSessionKey));
    	ret = doRSA(SessionKey, EncSessionKey);
	CHECK(ret == 0, lblKO);	
	DelayMs(100);

    	memset(Buf, 0, sizeof(Buf));
    	ret = SAMMANDIRI_Start_Session(EncSessionKey, 1, Buf );
    	if (ret !=0) goto lblKO;

    //---------------------------------------------------------------
	ScrCls();
	PubDispString(_T("INPUT OLD PIN :"), DISP_LINE_LEFT|3);
	
    	memset(temp, 0, sizeof(temp));
	ucRet = PubGetString(NUM_IN|ECHO_IN, 8, 8, temp, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return;
	}
	if (atol((char *)temp)==0)
	{
		goto lblKO;
	}
    
    	memcpy(key,SessionKey,16);
    	TDesCBC((byte*) temp, 8, TEPIN, (byte *) key);

    	memset(Buf, 0, sizeof(Buf));
    	ret = ui_PIN_Authenticate(TEPIN,1, Buf);
	CHECK(ret == 0, lblKO);

    	//---------------------------------------------------------------
    	memset(LTMKACQID, 0, sizeof(LTMKACQID));
    	memset(VENDORID, 0, sizeof(VENDORID));

	memcpy(LTMKACQID, glTlmSysParam.Con.appLtmkAcqID, sizeof(LTMKACQID));
	memcpy(VENDORID, glTlmSysParam.Con.appVendorID, sizeof(VENDORID));
	
lblUlangi:
	ScrCls();
	PubDispString(_T("INPUT NEW PIN :"), DISP_LINE_LEFT|3);
	
    	memset(NEWPIN, 0, sizeof(NEWPIN));
	ucRet = PubGetString(NUM_IN|ECHO_IN, 8, 8, NEWPIN, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return;
	}
	if (atol((char *)NEWPIN)==0)
	{
		goto lblKO;
	}

	ScrCls();
	PubDispString(_T("RE-ENTER NEW PIN :"), DISP_LINE_LEFT|3);
	
    	memset(temp, 0, sizeof(temp));
	ucRet = PubGetString(NUM_IN|ECHO_IN, 8, 8, temp, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return;
	}
	if (atol((char *)temp)==0)
	{
		goto lblKO;
	}

    	if(memcmp(NEWPIN, temp, 8)!=0)
    	{
	    	ScrCls();
		PubDispString("NOT MATCH", DISP_LINE_CENTER|3);
		PubDispString("PLEASE TRY AGAIN", DISP_LINE_CENTER|5);
		PubBeepErr();
		PubWaitKey(3);
		goto lblUlangi;
    	}
    	memset(LTMKReqDataIn, 0, sizeof(LTMKReqDataIn));
    	memcpy(LTMKReqDataIn,LTMKACQID,3);
    	memcpy(LTMKReqDataIn+3,VENDORID,8);
    	memcpy(LTMKReqDataIn+11,NEWPIN,8);

    	memset(LTMKReqEncDataIn, 0, sizeof(LTMKReqEncDataIn));
    	TDesCBC((byte*) LTMKReqDataIn, 24, LTMKReqEncDataIn, (byte *) key);

    	memset(Buf, 0, sizeof(Buf));
    	ret = PIN_Change(LTMKReqEncDataIn, 2, Buf);
	CHECK(ret == 0, lblKO);
    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
    	ret = uiEnd_Session(0x00, Buf );
	CHECK(ret == 0, lblKO);
    
    	goto lblEnd;
lblKO:
      	IccClose(0);
    	ScrCls();
	PubDispString("PIN CHANGE KO", DISP_LINE_CENTER|2);
	PubDispString(_T("PLS REMOVE CARD"), 4|DISP_LINE_CENTER);
	while( IccDetect(ICC_USER)==0 )
	{
		Beep();
		DelayMs(500);
	} 
      	IccClose(0);
      	
      	return;
lblEnd:
      	IccClose(0);
    	ScrCls();
	PubDispString("PIN CHANGE OK", DISP_LINE_CENTER|2);
	PubDispString(_T("PLS REMOVE CARD"), 4|DISP_LINE_CENTER);
	while( IccDetect(ICC_USER)==0 )
	{
		Beep();
		DelayMs(500);
	} 
      	IccClose(0);

      	return;
}

int Req_LTMK_Download(void)
{
    	byte Buf[128 * 3];          // Buffer to accept the input
    	byte Buf2[56+1];
    	int ret, icc = 0;
    	byte TEPIN[16+1];
    	char key[16+1];
    	char SessionKey[16+1];
    	byte EncSessionKey[136+1];
    	char LTMKACQID[3+1];
    	char VENDORID[8+1];
    	char TERMID[8+1];
    	char LTMKReqDataIn[24+1];
    	char LTMKReqEncDataIn[24+1];
    	char LTMKOUT[112+1];
    	char temp[16+1];
    	char temp2[128+1];
    	byte LTMKDecryIn[40+1];
    	char dataIn[16+1];	
	uchar buffer[999];
	uchar ucRet;
//	uchar KMSLTMK[80+1];

	SetCurAcq(0);
	mapGet(appNiiLTMK, glCurTlmTmsAcq.acqNII, sizeof(glCurTlmTmsAcq.acqNII));

	ScrCls();
	PubDispString("INSERT",  DISP_LINE_CENTER|2);
	PubDispString("KEK SAM",  DISP_LINE_CENTER|4);

    	TimerSet(0, 300);       // 300 = 30 s //added by irf, 

	IccClose(0);
	while( 1 )
	{
		if ((kbhit()==0) && (getkey()==KEYCANCEL))
		{
			return -1;
		}
		if( IccDetect(ICC_USER)==0 )
		{
			break;
		}
		if(!TimerCheck(0)) 	//timeout
       	{           
                   return -1;
       	}
	}
    	//---------------------------------------------------------------
    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
	ucRet = IccInit(0x80, (uchar *) Buf);
	CHECK(ucRet == 0, lblKO);

    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
    	memcpy(dataIn, "\xA0\x00\x00\x00\x18\x4b\x65\x6b\x53\x61\x6d\x00\x00\x01\x01\x02", 16);	
    	ret = selectAID(dataIn, 0, Buf);
	CHECK(ret == 0, lblKO);	
    	//---------------------------------------------------------------
    	memset(SessionKey, 0, sizeof(SessionKey));
    	memset(EncSessionKey, 0, sizeof(EncSessionKey));
    	ret = doRSA(SessionKey, EncSessionKey);
	CHECK(ret == 0, lblKO);	
	DelayMs(100);

//    	hex_dump_char("SESSION KEY", SessionKey, 16);


    	memset(Buf, 0, sizeof(Buf));
    	ret = SAMMANDIRI_Start_Session(EncSessionKey, 1, Buf );
    	if (ret !=0) goto lblKO;

//    	hex_dump_char("Start Session Rsp", Buf, 10);

    //---------------------------------------------------------------
    	ScrCls();
	PubDispString(_T("INPUT PIN :"), DISP_LINE_LEFT|3);
	
    	memset(temp, 0, sizeof(temp));
	ucRet = PubGetString(NUM_IN|ECHO_IN, 8, 8, temp, USER_OPER_TIMEOUT);
	if( ucRet!=0 )
	{
		return ERR_USERCANCEL;
	}
	if (atol((char *)temp)==0)
	{
		goto lblKO;
	}

	MEM_ZERO(key);
    	memcpy(key, SessionKey, 16);
    	TDesCBC((byte*) temp, 8, TEPIN, (byte *) key);

    	memset(Buf, 0, sizeof(Buf));
    	ret = ui_PIN_Authenticate(TEPIN,1, Buf);
	CHECK(ret == 0, lblKO);

    //---------------------------------------------------------------
    	memset(LTMKACQID, 0, sizeof(LTMKACQID));
    	memset(VENDORID, 0, sizeof(VENDORID));
    	memset(TERMID, 0, sizeof(TERMID));

	memcpy(LTMKACQID,	glTlmSysParam.Con.appLtmkAcqID, sizeof(LTMKACQID));
	memcpy(VENDORID,	glTlmSysParam.Con.appVendorID, sizeof(VENDORID));
//	memcpy(TERMID,	glCurTlmTmsAcq.acqTID, sizeof(glCurTlmTmsAcq.acqTID));
	memcpy(TERMID,	"93132833", 8);	//for testing only
    	MAPPUTSTR(appTidTle, TERMID, lblKO);
//	memcpy(glSysCtrl.stTlmApp.appTidTle, TERMID, sizeof(TERMID));
//	SaveSysCtrlBase();

    	memset(LTMKReqDataIn, 0, sizeof(LTMKReqDataIn));
	memcpy(LTMKReqDataIn, LTMKACQID, 3);
    	memcpy(LTMKReqDataIn + 3, VENDORID, 8);
    	memcpy(LTMKReqDataIn + 11, TERMID, 8);

    	memset(LTMKReqEncDataIn, 0, sizeof(LTMKReqEncDataIn));
    	TDesCBC((byte*) LTMKReqDataIn, 24, LTMKReqEncDataIn, (byte *) key);
    
    	memset(Buf, 0, sizeof(Buf));
    	memset(Buf2, 0, sizeof(Buf2));
    	ret = LMTK_Token_ReqGeneration(LTMKReqEncDataIn, Buf);
	CHECK(ret == 0, lblKO);

    	decTDesCBC(Buf, 56, Buf2, (byte *) key);
    
    	memset(LTMKOUT, 0, sizeof(LTMKOUT));
    	bin2hex(LTMKOUT, Buf2, 56);
	MAPPUTSTR(appLTMKDATA, LTMKOUT, lblKO);

//	memcpy(glSysCtrl.stTlmApp.appLTMKDATA, LTMKOUT, strlen(LTMKOUT));
//	SaveSysCtrlBase();
    	//---------------------------------------------------------------

//SET ISO 8583
	glProcInfo.stTranLog.ucTranType = TRANS_TYPE_LTMK;
	SetCommReqFieldLTMK();
	ret = SendRecvPacket();
	if( ret!=0 )
	{
		MEM_ZERO(Buf);
		ret = uiEnd_Session(0x00, Buf );
		goto lblKO;
	}
	//---------------------------------------------------------------
	memset(Buf, 0, sizeof(Buf));
	memset(Buf2, 0, sizeof(Buf2));
	memset(temp2, 0, sizeof(temp2));

	MEM_ZERO(buffer);
	memcpy(buffer, glRecvPack.szBit62+2, PubChar2Long(glRecvPack.szBit62, 2, NULL));
	if(debugRx)
		hex_dump_char("bit 62", buffer, PubChar2Long(glRecvPack.szBit62, 2, NULL));

	if(buffer[6] == 0x30){
		memcpy(temp2, buffer + 7, 40);
//		bin2hex(KMSLTMK, temp2, 40);
	}

//	hex2bin(Buf2, KMSLTMK, 40);
//	TDesCBC(Buf2, 40, LTMKDecryIn, (byte *) key);
	MEM_ZERO(LTMKDecryIn);
	if(debugRx)
		hex_dump_char("plain LTMKDecry in :", temp2, 40);
	TDesCBC(temp2, 40, LTMKDecryIn, (byte *) key);
	if(debugRx)
		hex_dump_char("encry LTMKDecry in :", LTMKDecryIn, 40);

//	DelayMs(100);

	ret = LMTK_Token_Decrypt(LTMKDecryIn, Buf);
    	if (ret != 0) 
		goto lblKO;

	if(debugRx)
		hex_dump_char("LTMKDecry out :", Buf, 26);

    	memset(temp2, 0, sizeof(temp2));
    	bin2hex(temp2, Buf, 24);

    	MAPPUTSTR(appLTMK, temp2, lblKO);

//	memcpy(glSysCtrl.stTlmApp.appLTMK, temp2, strlen(temp2));
//	SaveSysCtrlBase();
		
    	//---------------------------------------------------------------
    	memset(Buf, 0, sizeof(Buf));
    	ret = uiEnd_Session(0x00, Buf );
    	if (ret != 0) 
		goto lblKO;
    
goto lblEnd;
lblKO:
     	IccClose(0);
      	ScrCls();
      	PubDispString("UNSUCCESSFUL", DISP_LINE_CENTER|2);
	PubDispString(_T("PLS REMOVE CARD"), 4|DISP_LINE_CENTER);
	while( IccDetect(ICC_USER)==0 )
	{
		Beep();
		DelayMs(500);
	} 
      	IccClose(0);
		
      	return -1;
lblEnd:
      IccClose(0);
//   ScrCls();
      return 1;
}


static int parseBit62LTWK(const byte * rsp, word length) {
	int ret;
	byte buf[128+1];
	char HTLE[4+1];
	char version[2+1];
	char rspType[1+1];
	char LTWKID[4+1];
	byte eLTWKDEK[16+1];
	byte eLTWKMAK[16+1];
	char kcvLTWKDEK[8+1];
	char kcvLTWKMAK[16+1];
	char renewedAcqID[3+1];
	char temp[104+1];

	memset(buf,0,sizeof(buf));
	memcpy(buf,rsp,length);

	memset(HTLE,0,sizeof(HTLE));
	memset(version,0,sizeof(version));
	memset(rspType,0,sizeof(rspType));
	memset(LTWKID,0,sizeof(LTWKID));
	memset(eLTWKDEK,0,sizeof(eLTWKDEK));
	memset(eLTWKMAK,0,sizeof(eLTWKMAK));
	memset(kcvLTWKDEK,0,sizeof(kcvLTWKDEK));
	memset(kcvLTWKMAK,0,sizeof(kcvLTWKMAK));
	memset(renewedAcqID,0,sizeof(renewedAcqID));
	MEM_ZERO(temp);

	memcpy(HTLE,buf,4);
	memcpy(version,buf+4,2);
	memcpy(rspType,buf+6,1);
	memcpy(LTWKID,buf+7,4);
	memcpy(eLTWKDEK,buf+11,16);
	memcpy(eLTWKMAK,buf+37,16);
	memcpy(kcvLTWKDEK,buf+53,8);
	memcpy(kcvLTWKMAK,buf+61,8);
	memcpy(renewedAcqID,buf+69,3);

	bin2hex(temp,buf+7,52);
	MAPPUTSTR(appLTWKTID1,temp,lblKO);

	return 1;
	lblKO:
	return -1;

}

int Req_LTWK_Download(void){
    byte Buf[128 * 3];          // Buffer to accept the input
    int ret;

	glProcInfo.stTranLog.ucTranType = TRANS_TYPE_LTWK;
	SetCommReqFieldLTWK();
	ret = SendRecvPacket();
    	if (ret != 0) {
		memset(Buf, 0, sizeof(Buf));
		//ret = uiEnd_Session(0x00, Buf );
		goto lblKO;
	}
    //---------------------------------------------------------------

	parseBit62LTWK(glRecvPack.szBit62+2, (ushort)PubChar2Long(glRecvPack.szBit62, 2, NULL));
	
    goto lblEnd;
    lblKO:
     	IccClose(0);
      	ScrCls();
      	PubDispString("  UNSUCCESSFUL", DISP_LINE_CENTER|4);
	PubWaitKey(2);
      	return -1;
    lblEnd:
      IccClose(0);
      	ScrCls();
      	PubDispString("SUCCESSFUL", DISP_LINE_CENTER|4);
	PubWaitKey(2);
      return 1;
}

int LTWKKeyDownload(void){
	int ret;

	SetCurAcq(0);
	mapGet(appNiiLTMK, glCurTlmTmsAcq.acqNII, sizeof(glCurTlmTmsAcq.acqNII));

	ret = Req_LTWK_Download();
	CHECK(ret > 0, lblKO);

	return 1;

	lblKO:
		return -1;

}

void LTMKKeyDownload(void){
	int ret;

//	ret = admNewConnSettings();
//	CHECK(ret >= 0, lblKO);
	

//	MAPPUTWORD(traMnuItm, mnuLTMK, lblKO);

//	MAPPUTBYTE(traNeedDownloadLTWK, 1, lblKO);

	SetCurAcq(0);
	
	ret = Req_LTMK_Download();
	CHECK(ret >= 0, lblKO);

	glSysCtrl.TlmReg.regTleCnt = 0;
	SaveSysCtrlBase();
//	MAPPUTWORD(regTleCnt, 0, lblKO);

	ret = LTWKKeyDownload();
	CHECK(ret > 0, lblKO);

	MAPPUTBYTE(appFlagDownloadLTWK, 0, lblKO);
//	glSysCtrl.stTlmApp.appFlagDownloadLTWK=0;
//	glSysCtrl.stTlmApp.appFlagKEKSAM=1;
//	SaveSysCtrlBase();
	
	ScrCls();
	PubDispString("TLE OK", DISP_LINE_CENTER|4);
	PubWaitKey(2);

	lblKO:
		return;
}

//mcy add for Encryption Counter
int getCounterTle(tBuffer *val)
{
	int ret; 
	word TleCount;
	char dec[8+1];

	TleCount = (word)glSysCtrl.TlmReg.regTleCnt;

	num2dec(dec, TleCount, 4);

	ret = bufApp(val, dec, 4);
	CHECK(ret >= 0, lblKO);

	ret = GetNewTleCountNo();
	//ret = incCard(regTleCnt);
	CHECK(ret >= 0, lblKO);

	return ret;
	lblKO:
		return -1;
}



