#ifndef __CTX_H

#define __APPID__ 0x0350
#undef __GMA__
#undef __APR__

#define __CHN_USB__			// add TMS-USB
#define __CHN_COM__
#define __CHN_TCP__
#define __CHN_SCK__
#define __CHN_GPRS__

#define __IDLEMSG__ "MAN9QID\nApp Template"
#define __IDLEAPP__ "MANDIRI APPL"

#define __INGEDEV__ 7
#ifndef __TELIUM__
#define __TELIUM__
#endif
#ifdef __UNICAPT__
#undef __UNICAPT__
#endif

#define __FONT__ "/SYSTEM/COUR.SGN"

#define __PREDIALING__
#define __TCUPLOAD__


#define __EMV__
#define __TEST__

#define __RootKeyLoc__ 126
#define __appMKeyLoc__ 3
#define __appPinKeyLoc__ 1

#define SST_DEBUG

#ifdef SST_DEBUG
/*
#if 1
#define VAR_NUMBER_SIGN		0xFA
#define CARD_NUMBER_SIGN	0x0003C009
#define ID_SCR_XXX_BANK		0x00FA2030
#define ID_BANK_XXX			0x80000001
#else
#define VAR_NUMBER_SIGN		0xDB		//smartweb
#define CARD_NUMBER_SIGN	0x00010526	//smartweb
#define ID_SCR_XXX_BANK		0x00DB2030	//smartweb
#define ID_BANK_XXX			0x80000001	//smartweb
#endif
*/
#if 1	//MANDIRI (USE FOR PRODUCTION)
#define VAR_NUMBER_SIGN	0xD6		//mandiri
#define CARD_NUMBER_SIGN	0x10000884	//mandiri
#define ID_SCR_XXX_BANK	0x00D62030	//mandiri
#define ID_BANK_XXX		    	0x80000001	//mandiri
#else	//SMARTWEB (FOR DEVELOPTMENT ONLY)
#define VAR_NUMBER_SIGN		0xDB		//smartweb
#define CARD_NUMBER_SIGN	0x00010526	//smartweb
#define ID_SCR_XXX_BANK		0x00DB2030	//smartweb
#define ID_BANK_XXX			0x80000001	//smartweb
#endif
#endif

#undef __DEBUG_TRC_PATH__
#define __DEBUG_TRC_PATH__ "c:\\DEBUG.TRC"

#define __CTX_H
#endif
