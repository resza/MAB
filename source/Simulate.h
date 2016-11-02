
/****************************************************************************
NAME
    Simulate.h - 定义模拟、debug或培训模式所使用的功能

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    Gary Ho     2009.05.07      - created
****************************************************************************/

#ifndef SIMULATE_H
#define SIMULATE_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uchar ChkIfTrainMode(void);
int CreatDummyRecvData(void *pstProcInfo, void *pstRecvPacket);
int PosDebugShow(char *pszStr, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

// end of file
