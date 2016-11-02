#ifndef _TLE_H
#define _TLE_H

int getTLEData(uchar *DataIn);
void getMAC(uchar *DataIn, int lenDataIn, uchar *MAC);
int ParseTLEData(void);



#endif
