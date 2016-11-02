#ifndef _COMMLIBEZ_H
#define _COMMLIBEZ_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//Open Connection
int EZProxStart();

//Send Data
int EZProxTxd(uchar *psTxdData, ushort uiDataLen);

//Received Data
int EZProxRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);

//Close Connection
int EZProxCloseCon(void);

// end of file


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _CommLIB_H

// end of file
