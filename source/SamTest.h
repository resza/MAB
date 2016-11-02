

// Note: In your project, please replace all "IccInit" as "TestIccInit"
//		 and replace all "IccClose" as "TestIccClose"
//		 then check the log printed

#ifndef _SAM_TEST_H
#define _SAM_TEST_H

unsigned char MyIccInit(unsigned char slot, unsigned char *ATR, const char * pszFileName, const char *pszFuncName, int iCallLine);
unsigned char MyIccClose(unsigned char slot, const char * pszFileName, const char *pszFuncName, int iCallLine);

#define TestIccInit(slot, ATR) \
	MyIccInit(slot, ATR, __FILE__, __FUNCTION__, __LINE__);

#define TestIccClose(slot) \
	MyIccClose(slot, __FILE__, __FUNCTION__, __LINE__);


#endif  // _SAM_TEST_H

