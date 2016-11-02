
/****************************************************************************
NAME
    eventmsg.h - 定义管理器事件处理模块

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
****************************************************************************/

#ifndef _EVENTMSG_H
#define _EVENTMSG_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int ProcUserMsg(void);
int AutoDownParam(void);
int GetAutoDownParam(void);
int ProcEcrMsg(void);
int ProcMagMsg(void);
int ProcICCMsg(void);
int ProcFuncKeyMsg(void);
int ProcKeyMsg(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _EVENTMSG_H

// end of file
