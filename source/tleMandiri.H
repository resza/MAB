/*
 * kek_SAM.h
 *
 *  Created on: Aug 11, 2010
 *      Author: Danny
 */

#ifndef KEK_SAM_H_
#define KEK_SAM_H_

int generateSessionKey(char * SSK);
int doRSA(char *SessionKey, byte *EncSessionKey);
int Des_MAC(byte *DataIn, byte *rsp, byte *MacKey, int len);
int decTDesCBC(byte *dataIn, int lenInput, byte *dataOut, byte *key);
int TDesCBC(byte *dataIn, int lenInput, byte *dataOut, byte *key);
void LTMKKeyDownload(void);
int LTWKKeyDownload(void);
int protectedBit(tBuffer * bitEncryp);
int getCounterTle(tBuffer *val);
int LTWKey(byte *LTWK);
void Req_PIN_Change(void);
#endif /* KEK_SAM_H_ */
