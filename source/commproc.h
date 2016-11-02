
/****************************************************************************
NAME
    commproc.h - 实现通讯控制

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.14      - created
****************************************************************************/

#ifndef _COMMPROC_H
#define _COMMPROC_H

#define _TCP_BCD_LEN_BYTE	// TCP通讯,BCD 格式的长度字节

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int TranProcess(void);
int SendRecvPacket(void);
int SendPacket(void);
int RecvPacket(void);
int PreDial(void);
int ConnectHost(void);
int AfterTranProc(void);
int ReferralDial(uchar *pszPhoneNo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _COMMPROC_H

// end of file
