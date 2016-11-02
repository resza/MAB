#ifndef _DEMO_KEY_H
#define _DEMO_KEY_H

#include "posapi.h"

#define __IRF_TEST__

#define KEY_LENGTH		16 //��Կ����
#define PINBLOCK_LEN	8
#define MAC_DATA_LEN	16 //����MAC��������ݳ���
#define MACBLOCK_LEN	8
#define TDK_DATA_LEN	16 //����TDK��������ݳ���

#define PAN_LEN			16 //���ų���
#define KEY_VAR_LEN     "4, 5, 6, 7, 8"     //������PIN����
#define KSN_LEN			10
#define KEY_NUM         6                   //��Կ���͸���TLK.TMK.TPK.TAK.TDK.TIK

#define TLK_VALUE_IN	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" //TLK��Կ����
#define TMK_VALUE_IN	"\x11\x11\x11\x11\x11\x11\x11\x11" //TMK��Կ����
#define TIK_VALUE_IN	"3333333333333333" //TIK����
#define TPK_VALUE_IN	"\x01\x23\x45\x67\x89\xAB\xCD\xEF" //TPK����
#define TAK_VALUE_IN	"4222222222222222" //TAK����
#define TDK_VALUE_IN	"4333333333333333" //TDK ciphertext
#define KSN_VALUE_IN	"0000000001"	   //initial KSN

#define PAN_VALUE_IN	"5221361234567890" //16λ���� 

uchar g_uckeyNum[KEY_NUM];//�����Կ����,���ڼ���Ƿ�������Կ
//int flag;

int InputKey(int i);
int WriteKey(uchar SourceKeyIndex, uchar SourceKeyType, 
			 uchar DestKeyIndex, uchar DestKeyType, 
			 uchar *pKeyValueIn, uchar keyLen, 
			 uchar *pKCV, int i);

int GetPINBlock();
int Get_MAC();
int GetTDKCalculate();

void WriteTIK(ST_KCV_INFO *stKVCInfo);
int GetPINBlockDukpt();
int GetMACDukpt();

void Encrypt();

void ShowKeyMenu();
void TLKMenuProcess();
void TMKProcess();
void TWKMeunProcess(int i);

void InputTMK(void);
void InputTWK(void);

#endif
