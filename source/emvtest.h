
#ifndef _EMVTEST_H
#define _EMVTEST_H

void InitTestKeys(void);
void InitTestApps(void);
void InitLiveKeys(void);
void InitLiveApps(void);

uchar getDefaultCheckSumPublicKey(int KeyID, uchar *RID, uchar *CheckSum);

#endif
