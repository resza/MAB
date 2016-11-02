#include "global.h"
//#include "key.h"
#include "keyPED.h"

#define MAXRESULTLEN	1024

uchar g_uckeyNum[KEY_NUM];
uchar szPINBlock[PINBLOCK_LEN + 1];		//store the encrypted PINBlock
uchar szMacBlock[MACBLOCK_LEN + 1];		//store the encrypted MacData
uchar szTDKBlock[TDK_DATA_LEN + 1];		//store the encrypted TDKData
uchar szPINBlockDukpt[PINBLOCK_LEN + 1];
uchar szMacBlockDukpt[MACBLOCK_LEN + 1];

uchar szKSNOut[KSN_LEN+1];

int flag;

ST_KEY_INFO	stKeyInfoIn/*[KEY_NUM]*/;
ST_KCV_INFO	stKcvInfoIn/*[KEY_NUM]*/;

char *HexString(unsigned char *mem, int length)
{
    static char result[MAXRESULTLEN];
    result[0] = '\0';

    if (length > MAXRESULTLEN/2)  /* 1 Byte = 2 Chars                 */
        length = MAXRESULTLEN/2;  /* Ignore the rest to protect       */
                                    /* memory. Or increase buffer size. */

    while (length--) {
        sprintf(strchr(result, '\0'), "%02X", *mem);
        mem++;
    }

    return result;
}

int InputKey(int i) //Input TPK, TAK, TDK 
{
    uchar ucRet;
	uchar ucBuf[KEY_LENGTH + 1];

	if (i == 1) // TPK 
	{
		memset(&g_uckeyNum[3], 0, sizeof(&g_uckeyNum[3]));

		ScrCls();
		ScrPrint(0, 2, 1, "Input TPK:");
#ifndef __IRF_TEST__ 		
		ucRet = PubGetString(ALPHA_IN, KEY_LENGTH, KEY_LENGTH, ucBuf, 60);
		if (ucRet != 0)
		{	
			ScrCls();
			ScrPrint(0, 2, 1, "Input Error");
			DelayMs(3000);
			return -1;
		}
#else
	memset(ucBuf, 0, sizeof(ucBuf));
	memcpy(ucBuf, TPK_VALUE_IN, KEY_LENGTH);
#endif		
		ucRet = WriteKey(1, PED_TMK, 1, PED_TPK, ucBuf, KEY_LENGTH, NULL, 3);
		if (ucRet != 0)
		{
			return -1;
		}
	}
	else if(i == 2) // TAK
	{
		memset(&g_uckeyNum[4], 0, sizeof(&g_uckeyNum[4]));
		
		ScrCls();
		ScrPrint(0, 2, 1, "Input TAK:\n");
		ucRet = PubGetString(ALPHA_IN, KEY_LENGTH, KEY_LENGTH, ucBuf, 60);
		if (ucRet != 0)
		{	
			ScrCls();
			ScrPrint(0, 2, 1, "Input Error");
			DelayMs(3000);
			return -1;
		}
		ucRet = WriteKey(1, PED_TMK, 1, PED_TAK, ucBuf, KEY_LENGTH, NULL, 4);
		if (ucRet != 0)
		{
			return -1;
		}
	}
	else if (i == 3) // TDK
	{
		memset(&g_uckeyNum[5], 0, sizeof(&g_uckeyNum[5]));

		ScrCls();
		ScrPrint(0, 2, 1, "Input TDK:\n");
		ucRet = PubGetString(ALPHA_IN, KEY_LENGTH, KEY_LENGTH, ucBuf, 60);
		if (ucRet != 0)
		{	
			ScrCls();
			ScrPrint(0, 2, 1, "Input Error");
			DelayMs(3000);
			return -1;
		}

		ucRet = WriteKey(1, PED_TMK, 1, PED_TDK, ucBuf, KEY_LENGTH, NULL, 5);
		if (ucRet != 0)
		{
			return -1;
		}
	}
	
	return 0;
}

int WriteKey(uchar SourceKeyIndex, uchar SourceKeyType, 
			 uchar DestKeyIndex, uchar DestKeyType, 
			 uchar *pKeyValue, uchar KeyLen,
			 uchar *pKCV, int i)
{
	int iRet;
	
	ScrCls();
	ScrPrint(0, 4, 1, "Wrting...");
	DelayMs(3000);

	stKeyInfoIn.ucSrcKeyIdx  = SourceKeyIndex;
	stKeyInfoIn.ucSrcKeyType = SourceKeyType;
       stKeyInfoIn.ucDstKeyIdx  = DestKeyIndex;
	stKeyInfoIn.ucDstKeyType =  DestKeyType;
   
	stKeyInfoIn.iDstKeyLen = KeyLen;
	memcpy(&stKeyInfoIn.aucDstKeyValue, pKeyValue, KeyLen);
	
	if (pKCV == NULL)
	{
		stKcvInfoIn.iCheckMode = 0;
	}

	iRet = PedWriteKey(&stKeyInfoIn, &stKcvInfoIn);
	if (iRet != 0)
	{
		ScrCls();
		ScrPrint(0, 2, 0, "PedWriteKey:%d", iRet);
		return iRet;
	}
	g_uckeyNum[i] = KeyLen;

	ScrCls();
	ScrPrint(0, 2, 1, "Write Key Success");
	DelayMs(3000);

	return 0;
}



int GetPINBlock()
{
	int iRet;
	
	if (g_uckeyNum[3] == 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "   NO TPK  ");
		DelayMs(3000);
		return -1;
	}
	ScrCls();
	ScrPrint(0, 0, 1, "Input PIN:\n");
	ScrGotoxy(0, 4);
	DelayMs(1000);
	
	iRet = PedGetPinBlock(1, KEY_VAR_LEN, PAN_VALUE_IN, szPINBlock, 0, 15000);
	if (iRet != 0)
	{
		return -1;
	}

	
 	ScrCls();
 	ScrPrint(0, 4, 1, "%s", HexString(szPINBlock, PINBLOCK_LEN));
//	ScrPrint(0, 4, 1, "%s", szPINBlock);

	DelayMs(30000);

	return 0;
}

int Get_MAC()
{
	int iRet;
	uchar szBuf[MAC_DATA_LEN + 1];

	if (g_uckeyNum[4] == 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "   NO TAK  ");
		DelayMs(3000);
		return -1;
	}

	memset(szBuf, 0, sizeof(szBuf));
	memset(szMacBlock, 0, sizeof(szMacBlock));

	ScrCls();
	ScrPrint(0, 0, 1,"Input MacData\n\n  ");
	iRet = PubGetString(ALPHA_IN, MAC_DATA_LEN, MAC_DATA_LEN, szBuf, 60);
	if (iRet != 0)
	{
		return -1;
	}
	
	iRet = PedGetMac(1, szBuf, MAC_DATA_LEN, szMacBlock, 0);
	if (iRet != 0)
	{
		return -1;
	}
	
 	ScrCls();
	ScrPrint(0, 0, 0x80|0x01, "%s", szMacBlock);
	DelayMs(3000);

	return 0;
}

int GetTDKCalculate()
{
	int iRet;
	uchar szBuf[TDK_DATA_LEN + 1];
	
	if (g_uckeyNum[5] == 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "   NO TDK  ");
		DelayMs(3000);
		return -1;
	}
	
	memset(szBuf, 0, sizeof(szBuf));
	memset(szTDKBlock, 0, sizeof(szTDKBlock));
	
	ScrCls();
	ScrPrint(0, 0, 1,"   Input TDKData\n\n  ");
	iRet = PubGetString(ALPHA_IN, MAC_DATA_LEN, MAC_DATA_LEN, szBuf, 60);
	if (iRet != 0)
	{
		return -1;
	}
	
	iRet = PedCalcDES(1, szBuf, TDK_DATA_LEN, szTDKBlock, 1);
	if (iRet != 0)
	{
		return -1;
	}
	
	ScrCls();
	ScrPrint(0, 0, 0x80|0x01, "%s", szMacBlock);
	DelayMs(3000);
	
	return 0;
}

int GetPINBlockDukpt()
{
	int iRet;
	
	if (g_uckeyNum[2] == 0)
	{
		ScrCls();
		ScrPrint(0, 2, 1, "    NO TIK  ");
		DelayMs(3000);
		return -1;
	}
	ScrCls();
	ScrPrint(0, 0, 1, "Input PIN");
	ScrGotoxy(0, 4);
	DelayMs(1000);
	
	iRet = PedGetPinDukpt(1, KEY_VAR_LEN, PAN_VALUE_IN, szKSNOut, szPINBlockDukpt, 0,15000);
	if (iRet != 0)
	{
		return -1;
	}
	
 	ScrCls();
	ScrPrint(0, 0, 01, "Result:");
	ScrPrint(0, 2, 00, "KSN:%s", szKSNOut);
 	ScrPrint(0, 3, 00, "MacBlock:%02x%02x%02x%02x%02x%02x%02x%02x", szPINBlockDukpt[0], szPINBlockDukpt[1],szPINBlockDukpt[2],szPINBlockDukpt[3],szPINBlockDukpt[4],szPINBlockDukpt[5],szPINBlockDukpt[6],szPINBlockDukpt[7]);
	DelayMs(5000);

	return 0;
}

int GetMACDukpt()
{
	int iRet;
	uchar szBuf[MAC_DATA_LEN + 1];
	
	if (g_uckeyNum[2] == 0)
	{
		ScrCls();
		ScrPrint(0, 2, 1, "   NO TIK  ");
		DelayMs(3000);
		return -1;
	}
	
	memset(szBuf, 0, sizeof(szBuf));
	memset(szMacBlockDukpt, 0, sizeof(szMacBlockDukpt));
	
	ScrCls();
	ScrPrint(0, 0, 1,"Input DukptMacData\n\n ");
	iRet = PubGetString(ALPHA_IN, MAC_DATA_LEN, MAC_DATA_LEN, szBuf, 60);
	if (iRet != 0)
	{
		return -1;
	}
	
	iRet = PedGetMacDukpt (1, szBuf, MAC_DATA_LEN, szMacBlockDukpt, szKSNOut, 0);
	if (iRet != 0)
	{
		return -1;
	}

 	ScrCls();
	ScrPrint(0, 0, 01, "Result:");
	ScrPrint(0, 2, 00, "KSN:%s", szKSNOut);
 	ScrPrint(0, 3, 00, "MacBlock:%02x%02x%02x%02x%02x%02x%02x%02x", szMacBlockDukpt[0], szMacBlockDukpt[1],szMacBlockDukpt[2],szMacBlockDukpt[3],szMacBlockDukpt[4],szMacBlockDukpt[5],szMacBlockDukpt[6],szMacBlockDukpt[7]);
	DelayMs(5000);

	return 0;
}

void Encrypt()
{
	uchar uckey;
	int iRet;
	
	while (1)
	{	
		ScrCls();
		kbflush();
		ScrPrint(0, 0, 0x01|0x80, "   Encryption   ");
		ScrPrint(0, 2, 1, "1.DES/3DES Encrypt");
		ScrPrint(0, 4, 1, "2.Dukpt Encrypt");
		
		uckey = getkey();
		if (uckey == KEY1)
		{
 			while (1)
 			{
				ScrCls();
				kbflush();
				ScrPrint(0, 0, 0x01|0x80, "  For DES/3DES  ");
				ScrPrint(0, 2, 1, "1.Encrypt PIN");
				ScrPrint(0, 4, 1, "2.Encrypt MAC");
				ScrPrint(0, 6, 1, "3.Encrypt Data");
	
				uckey = getkey();
 				if (uckey == KEY1)
 				{	
					iRet = GetPINBlock();
					if (iRet != 0)
					{
						ScrCls();
						ScrPrint(0, 4, 1, "Get PINBlock Error");
						DelayMs(3000);
						continue;
					}
					ScrCls();
					ScrPrint(0, 4, 1, "Encrypt PIN Success");
					DelayMs(3000);
					
					return;
 				}
 				else if (uckey == KEY2)
 				{
					iRet = Get_MAC();
					if (iRet != 0)
					{
						ScrCls();
						ScrPrint(0, 4, 1, "Get Mac Error");
						DelayMs(3000);
						continue;
					}
					ScrCls();
					ScrPrint(0, 4, 1, "Encrypt Mac Success");
					DelayMs(3000);
					
					return;
 				}
				else if (uckey == KEY3)
				{
					iRet = GetTDKCalculate();
					if (iRet != 0)
					{
						ScrCls();
						ScrPrint(0, 4, 1, "Get TDK Error");
						DelayMs(3000);
						continue;
					}
					ScrCls();
					ScrPrint(0, 4, 1, "Encrypt TDK Success");
					DelayMs(3000);
					
					return;
				}
  				else if (uckey == KEYCANCEL)
 				{
  					break;
  				}
 			}
 		}
  		else if(uckey == KEY2)
  		{
			while (1)
  			{
 				ScrCls();
 				kbflush();
 				ScrPrint(0, 0, 0x01|0x80, "   For Dukpt   ");
 				ScrPrint(0, 2, 1, "1.PINDukpt");
 				ScrPrint(0, 4, 1, "2.MACDukpt");
				
 				uckey = getkey();
 				if (uckey == KEY1)
 				{
 					iRet = GetPINBlockDukpt();
 					if(iRet != 0)
 					{
						ScrCls();
						ScrPrint(0, 4, 1, "Get PINDukpt Error");
						DelayMs(3000);
						continue;
 					}
					ScrCls();
					ScrPrint(0, 4, 1, "PINDukpt Success");
					DelayMs(3000);
					
					continue;
				}
 				else if(uckey == KEY2)
 				{
 					iRet = GetMACDukpt();
 					if(iRet != 0)
 					{
						ScrCls();
						ScrPrint(0, 2, 1, "Get MacDukpt Error");
						DelayMs(3000);
						continue;
 					}
					ScrCls();
					ScrPrint(0, 2, 1, "MACDukpt Success");
					DelayMs(3000);
					
					continue;
 				}
				else if(uckey == KEYCANCEL)
				{
					break;
				}
 			}
  		}
		else if (uckey == KEYCANCEL)
		{
			break;
		}
 	}

	return;
}

void ShowKeyMenu()
{
	uchar uckey;
	
	memset(g_uckeyNum, 0, sizeof(g_uckeyNum));
	memset(&stKeyInfoIn, 0, sizeof(ST_KEY_INFO));
	memset(&stKcvInfoIn, 0, sizeof(ST_KCV_INFO));

	while (1)
	{
		ScrCls();
		kbflush();
		ScrPrint(0, 0, 0x01|0x80,     "    Key Menu    ");
		ScrPrint(0, 2, 1,             "1. Input TLK");
		ScrPrint(0, 4, 1,             "2. Input TMK");
		ScrPrint(0, 6, 1,             "3. Input TIK");
		
		uckey = getkey();
		if (uckey == KEY1)
		{
			TLKMenuProcess();
		}
		else if (uckey == KEY2)
		{
			TMKProcess();
		}
		else if (uckey == KEY3)
		{
			WriteTIK(NULL);
		}
		else if (uckey == KEYDOWN)
		{
			while (1)
			{
				ScrCls();
				kbflush();
				ScrPrint(0, 0, 0x01|0x80,     "    Key Menu    ");
				ScrPrint(0, 2, 1,             "4. Input TWK");
				ScrPrint(0, 4, 1,             "5. Encrypt Key");

				uckey = getkey();
	
				if (uckey == KEY4)
				{
					ScrCls();
					kbflush();
					ScrPrint(0, 0, 0x01|0x80,     "    TWK Menu    ");
					ScrPrint(0, 2, 1,             "1. Input TPK");
					ScrPrint(0, 4, 1,             "2. Input TAK");
					ScrPrint(0, 6, 1,             "3. Input TDK");
					
					uckey = getkey();
					if (uckey == KEY1 || uckey == KEY2 || uckey == KEY3)
					{
						TWKMeunProcess(uckey - KEY0);
					}
				}
				else if (uckey == KEY5)
				{
					Encrypt();
				}
				else if (uckey == KEYUP)
				{
					break;
				}
				else if (uckey == KEYCANCEL)
				{
					return;
				}
			}
		}
		else if (uckey == KEYCANCEL)
		{
			return;
		}
	}

}

void TLKMenuProcess()
{
	uchar ucRet;
	uchar ucBuf[KEY_LENGTH + 1];

	memset(&g_uckeyNum, 0, sizeof(g_uckeyNum));

	ScrCls();
	ScrPrint(0, 2, 1, "Input TLK:\n");
	ucRet = PubGetString(ALPHA_IN, KEY_LENGTH, KEY_LENGTH, ucBuf, 60);
	if (ucRet != 0)
	{	
		ScrCls();
		ScrPrint(0, 2, 1, "Input Error");
		DelayMs(3000);
		return;
	}

	PedErase();

	ucRet = WriteKey(0, PED_BKLK, 1, PED_BKLK, ucBuf, KEY_LENGTH, NULL, 0);

	if (ucRet != 0)
	{
		ScrPrint(0, 4, 1, "Write TLK Error");
		DelayMs(3000);
	}
}

void TMKProcess()
{
	uchar ucRet;
	uchar ucBuf[KEY_LENGTH + 1];

/*
	if (g_uckeyNum[0] == 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "    No TLK   ");
		DelayMs(3000);
		return;
	}
	
	memset(g_uckeyNum + 1, 0, sizeof(&g_uckeyNum[1]) * 5);
*/
	ScrCls();
	ScrPrint(0, 2, 1, "Input TMK:\n");
#ifndef __IRF_TEST__	
	ucRet = PubGetString(ALPHA_IN, KEY_LENGTH, KEY_LENGTH, ucBuf, 60);
	if (ucRet != 0)
	{	
		ScrCls();
		ScrPrint(0, 2, 1, "Input Error");
		DelayMs(3000);
		return;
	}
#else
	memset(ucBuf, 0, sizeof(ucBuf));
	memcpy(ucBuf, TMK_VALUE_IN, 16);
#endif
	ucRet = WriteKey(1, PED_BKLK, 1, PED_TMK, ucBuf, KEY_LENGTH, NULL, 1);
	
	if (ucRet != 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "Write TMK Error");
		DelayMs(3000);
		return;
	}
}

void WriteTIK(ST_KCV_INFO *stKVCInfo)
{
	uchar ucRet;
	uchar ucBuf[KEY_LENGTH + 1];

	if (g_uckeyNum[0] == 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1,"   No TLK   ");
		DelayMs(3000);
		return;
	}

	memset(&g_uckeyNum[2], 0, sizeof(&g_uckeyNum[2]));
	
	if (stKVCInfo == NULL)
	{
		stKcvInfoIn.iCheckMode = 0;
	}

	ScrCls();
	ScrPrint(0, 2, 1, "Input TIK:\n");
	ucRet = PubGetString(ALPHA_IN, KEY_LENGTH, KEY_LENGTH, ucBuf, 60);
	if (ucRet != 0)
	{	
		ScrCls();
		ScrPrint(0, 2, 1, "Input Error");
		DelayMs(3000);
		return;
	}
	ucRet = PedWriteTIK(1, 1, KEY_LENGTH, ucBuf, KSN_VALUE_IN, &stKcvInfoIn);

	if (ucRet != 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "Write TIK Error");
		return;
	}
	
	g_uckeyNum[2] = KEY_LENGTH;
	ScrCls();
	ScrPrint(0, 4, 1, "Write TIK Success");
	DelayMs(3000);
}

void TWKMeunProcess(int i)
{
	uchar uckey;

	if (g_uckeyNum[1] == 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1,"   No TMK   ");
		DelayMs(3000);
		return;
	}

	uckey = InputKey(i);
	if (uckey != 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "Write TWK Error");
		DelayMs(3000);
	}
	else
	{
		ScrCls();
		ScrPrint(0, 4, 1, "Write TWK Success");
		DelayMs(3000);
	}
}


//irf, 20121128
void InputTMK(void)
{
	uchar ucRet;
	uchar ucBuf[KEY_LENGTH + 1];
	uchar ucBufOut[KEY_LENGTH+1];


	memset(ucBuf, 0, sizeof(ucBuf));
	
	ScrCls();
	ScrPrint(0, 2, 1, "Input TMK:\n");
	ucRet = PubGetString(ALPHA_IN, KEY_LENGTH, KEY_LENGTH, ucBuf, 60);
	if (ucRet != 0)
	{	
		ScrCls();
		ScrPrint(0, 2, 1, "Input Error");
		DelayMs(3000);
		return;
	}
	memset(ucBufOut, 0, sizeof(ucBufOut));
	PubAsc2Bcd(ucBuf, KEY_LENGTH, ucBufOut);
	ucRet = SaveTMK(ucBufOut);
	if (ucRet != 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "Write TMK Error");
		DelayMs(3000);
		return;
	}

	ScrCls();
	ScrPrint(0, 2, 1, "Write TMK");
	ScrPrint(0, 4, 1, "Success");
	DelayMs(1000);
}

//irf, 20121128
void InputTWK(void)
{
	uchar ucRet;
	uchar ucBuf[KEY_LENGTH + 1];
	uchar ucBufOut[KEY_LENGTH + 1];


	memset(ucBuf, 0, sizeof(ucBuf));

	ScrCls();
	ScrPrint(0, 2, 1, "Input TWK:\n");
	ucRet = PubGetString(ALPHA_IN, KEY_LENGTH, KEY_LENGTH, ucBuf, 60);
	if (ucRet != 0)
	{	
		ScrCls();
		ScrPrint(0, 2, 1, "Input Error");
		DelayMs(3000);
		return;
	}
	memset(ucBufOut, 0, sizeof(ucBufOut));
	PubAsc2Bcd(ucBuf, KEY_LENGTH, ucBufOut);
	ucRet = SaveTWK(ucBufOut);
	if (ucRet != 0)
	{
		ScrCls();
		ScrPrint(0, 4, 1, "Write TWK Error");
		DelayMs(3000);
		return;
	}
	ScrCls();
	ScrPrint(0, 2, 1, "Write TWK");
	ScrPrint(0, 4, 1, "Success");
	DelayMs(1000);
}


