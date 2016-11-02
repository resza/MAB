#ifndef _MAIN_H
#define _MAIN_H
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
static int  main_sub(ST_EVENT_MSG *pstEventMsg);
static int  CheckTermSatus(ST_EVENT_MSG *pstEventMsg);
static void FirstRunProc(ST_EVENT_MSG *pstEventMsg);
static void SetIdleTimer(void);
static uchar ChkIdleTimer(uchar ucMinutes);
static void ShowIdleMainUI(uchar *pbForceUpdate);
static int  CheckInitTerminal(void);
static void MainIdleProc(void);


/********************** Internal variables declaration *********************/




#endif
