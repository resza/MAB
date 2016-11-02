
#include "SamTest.h"

extern unsigned char IccInit(unsigned char slot, unsigned char *ATR);
extern unsigned char IccClose(unsigned char slot);
extern unsigned char PrnInit(void);
extern unsigned char PrnStr(char *str, ...);
extern unsigned char PrnStart(void);

unsigned char MyIccInit(unsigned char slot, unsigned char *ATR, const char * pszFileName, const char *pszFuncName, int iCallLine)
{
	PrnInit();
	PrnStr("IccInit called!");
	PrnStr("%s, %s, %d", pszFileName, pszFuncName, iCallLine);
	PrnStr(" Slot No is: 0x%02x", slot);
	PrnStr("\n\n\n\n");
	PrnStart();

	return IccInit(slot, ATR);
}

unsigned char MyIccClose(unsigned char slot, const char * pszFileName, const char *pszFuncName, int iCallLine)
{
	PrnInit();
	PrnStr("IccClose called!");
	PrnStr("%s, %s, %d", pszFileName, pszFuncName, iCallLine);
	PrnStr(" Slot No is: 0x%02x", slot);
	PrnStr("\n\n\n\n");
	PrnStart();

	return IccClose(slot);
}


