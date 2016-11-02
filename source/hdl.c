//Subversion reference
//$HeadURL: https://svn.ingenico.com/UTAPROJECTS/sea-partner/5W/trunk/ISO5WPH/log/hdl.c $
//$Id: hdl.c 1920 2009-04-23 01:58:29Z ajloreto $

#include "global.h"


//forward declarations
static void hdlSubmenu(word msg1, word msg2);


typedef struct 
{
	char mnuName[1+lenMnu];
	word mnu;
	byte mnuFlag;
} TenorData;


 //display current menu state
 // mnu: menu array
 // upp: index of the first menu item to be displayed
 // cur: current menu item infex to be dispalyed in inverted mode
 // dim: number of menu items
static int mnuShow(const Pchar * mnu, byte upp, byte cur, byte dim) {
    char tmp[255];
    byte idx;
    uchar ucRever=0;

    //current menu item should be on the screen
    memset(tmp, 0, 255);
    for (idx = 0; idx < dim; idx++) {   //dim lines to display
        memcpy(tmp, mnu[upp + idx], 16);
        fmtPad(tmp, 16, ' ');    //pad by spaces to have a good menu bar

	ucRever = (upp+idx==cur) ? REVER: 0;
	ScrPrint(0, (uchar)((idx*2)+2), (uchar)(CFONT|ucRever), "%.*s", 16, tmp);
//        PubDispString(tmp, (uchar)(DISP_LINE_LEFT|((upp + idx == cur) ? (DISP_LINE_REVER|(idx*2)) : (idx*2))));
//        ret = dspLS((upp + idx == cur) ? INV(idx + 1) : (idx + 1), tmp);    //the current menu item is inverted
//        CHK;
    }
    return 0;
}


/** Propose a menu of strings to the user. Allow user to browse the menu and to select an item or cancel.
 *
 * The menu can contain no more than MNUMAX items.
 * The cursor keys can be used to browse the menu.
 * Green key ENTER is used to make the selection, and red key CLEAR is used to abort.
 * The numeric keys '1'..’9’ are used to access directly to a menu item.
 *
 * The state of the menu consists of two parts:
 *  - (sta % MNUMAX) is the current position of the selector
 *  - (sta / MNUMAX) is the number of the upper item displayed on the screen
 * 
 * The initial state is given by the second parameter;
 * the function returns the resulting stated if an item is selected.
 * 
 * Timeout of dly seconds is set.
 * Timer 0 is used.
 * If it is zero, the waiting state is infinite.
 *
 * \param mnu (I) An array of strings containing menu items.
 *        The element number zero is the menu name displayed at the top of the screen.
 *        The last element of this array should be zero pointer
 * \param sta (I) Initial menu state defining the topmost menu item displayed
 *        and the current item selected; if it is zero the first menu item will be at the top
 *        and it will be selected
 * \param dly (I) Timeout in seconds; zero value means infinite loop
 * \pre 
 *    - mnu!=0
 *    - the initial current item should be displayed on the screen
 *    - the number of items in the menu should not be zero
 * \return  
 *    - final state if an item is selected.
 *    - zero if it is cancelled. 
 *    - negative in case of error.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dlg.c
 * \test tcab0025.c
*/
int mnuFunction(const Pchar * mnu, int sta, int dly) {
//mnu: array of strings; mnu[0]= name of menu; other elements - menu items
//sta: menu state, contains upp and cur indexes, see below
//dly: timeout in seconds; zero means infinite
    int ret;                    //return value
    byte dim;                   //the mnu dimension including menu header
    byte upp;                   //number of menu item to be displayed first
    byte cur;                   //number of menu item selected just now
    byte bnd;                   //number of items to be displayed on the screen (band)
    char key;                   //key pressed

    if(!sta)                    //the initial state is not important
        sta = 1 * 30 + 1;

    cur = sta % 30;
    upp = sta / 30;
    
    dim = 0;
    while(mnu[dim]) {           //calculate dim: number of elements in the menu array
        dim++;
        ret = 30;
        if(dim >= ret)
            break;              //no more than MNUMAX elements are supported
    }
    
    //calculate bnd: number of items to be displayed on the screen
    bnd = 4;            //if there is enough elements it is the screen height
    if(dim < bnd)
        bnd = dim;              //otherwise, it is the number of items in the menu
    bnd--;                      //don't forget that the first element of the mnu array is the header string

//header
    	dspClear();
	ScrPrint(3, 0, (uchar)(CFONT), "%.*s", 16, (char*)mnu[0]);
	ScrDrawRect(0, 0, 127, 15);

//menu
    while(1) {
        ret = mnuShow(mnu, upp, cur, bnd);  //display current menu state
        CHK;

        //wait for user input
	 key = 0;
	 key = PubWaitKey((short)dly);
	 if(key == NOKEY)
	 	key = 0;

        switch (key) {          //analyse the key pressed
          case 0:
              return 0;         //timeout
          case KEYENTER:         //green key ENTER - current menu item selected
              return upp * 30 + cur;    //return the menu state
          case KEYCANCEL:
	   case KEYCLEAR:
              return 0;         //red key CLEAR - menu selection aborted
          case KEYDOWN:          //DOWN arrow key - move cursor down
              cur++;
              if(cur >= dim) {  //wrapping from the end to the beginning if needed
                  cur = 1;
                  upp = 1;
              } else if(upp + bnd == cur)
                  upp++;        //follow upp if cur is at the bottom
              break;
          case KEYUP:          //UP arrow key - move cursor up
              cur--;
              if(!cur) {        //wrapping from the beginning to the end if needed
                  cur = dim - 1;
                  if(upp + bnd <= cur)
                      upp = cur - bnd + 1;
              } else if(cur == upp - 1) //follow upp if cur is at the top
                  upp = cur;
              break;
          case KEY1:
          case KEY2:
          case KEY3:
          case KEY4:
          case KEY5:
          case KEY6:
          case KEY7:
          case KEY8:
          case KEY9:            //numeric key: direct selecting
              key -= KEY0;
              if(key > dim - 1) {   //if there is not enough menu items - beep
                  Beep();
                  break;
              }
              cur = (byte) key; //adjust screen to put current item on the top
              upp = cur;
              if(upp + bnd > dim)
                  upp = dim - bnd;  //adjust upp to display as many items as possible
              break;
		  //case KEYATM1:
		  case KEYATM2:
		  case KEYATM3:
		  case KEYATM4:
		  	 key -= KEYATM1; //4212
			 return (upp * 30 + upp + key - 1);
			 break;
          default:
              Beep();
              break;            //key not accepted
        }
    }
  lblKO:return -1;
}

/** Propose a menu of strings to the user. Allow user to browse the menu and to select an item or cancel.
 *
 * The menu can contain no more than MNUMAX items.
 * The cursor keys can be used to browse the menu.
 * Green key ENTER is used to make the selection, and red key CLEAR is used to abort.
 * The numeric keys '1'..’9’ are used to access directly to a menu item.
 *
 * The state of the menu consists of two parts:
 *  - (sta % MNUMAX) is the current position of the selector
 *  - (sta / MNUMAX) is the number of the upper item displayed on the screen
 * 
 * The initial state is given by the second parameter;
 * the function returns the resulting stated if an item is selected.
 * 
 * Timeout of dly seconds is set.
 * Timer 0 is used.
 * If it is zero, the waiting state is infinite.
 *
 * \param mnu (I) An array of strings containing menu items.
 *        The element number zero is the menu name displayed at the top of the screen.
 *        The last element of this array should be zero pointer
 * \param sta (I) Initial menu state defining the topmost menu item displayed
 *        and the current item selected; if it is zero the first menu item will be at the top
 *        and it will be selected
 * \param dly (I) Timeout in seconds; zero value means infinite loop
 * \pre 
 *    - mnu!=0
 *    - the initial current item should be displayed on the screen
 *    - the number of items in the menu should not be zero
 * \return  
 *    - final state if an item is selected.
 *    - zero if it is cancelled. 
 *    - negative in case of error.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\dlg.c
 * \test tcab0025.c
*/

int mnuSelect(const Pchar * mnu, int sta, int dly) {
//mnu: array of strings; mnu[0]= name of menu; other elements - menu items
//sta: menu state, contains upp and cur indexes, see below
//dly: timeout in seconds; zero means infinite
    int ret;                    //return value
    byte dim;                   //the mnu dimension including menu header
    byte upp;                   //number of menu item to be displayed first
    byte cur;                   //number of menu item selected just now
    byte bnd;                   //number of items to be displayed on the screen (band)
    char key;                   //key pressed

    if(!sta)                    //the initial state is not important
        sta = 1 * MNUMAX + 1;

    cur = sta % MNUMAX;
    upp = sta / MNUMAX;
    dim = 0;
    while(mnu[dim]) {           //calculate dim: number of elements in the menu array
        dim++;
        ret = MNUMAX;
        if(dim >= ret)
            break;              //no more than MNUMAX elements are supported
    }
    
    //calculate bnd: number of items to be displayed on the screen
    bnd = 4;            //if there is enough elements it is the screen height
    if(dim < bnd)
        bnd = dim;              //otherwise, it is the number of items in the menu
    bnd--;                      //don't forget that the first element of the mnu array is the header string

//header
    	dspClear();
	ScrPrint(3, 0, (uchar)(CFONT), "%.*s", 16, (char*)mnu[0]);
	ScrDrawRect(0, 0, 127, 15);

//menu
    while(1) {
        
        ret = mnuShow(mnu, upp, cur, bnd);  //display current menu state
        CHK;

        //wait for user input
	 key = 0;
	 kbflush();
	 key = PubWaitKey((short)dly);
	 if(key == NOKEY)
	 	key = 0;

        switch (key) {          //analyse the key pressed
          case 0:
              return 0;         //timeout
          case KEYENTER:         //green key ENTER - current menu item selected
              return upp * MNUMAX + cur;    //return the menu state
          case KEYCANCEL:
	   case KEYCLEAR:
              return 0;         //red key CLEAR - menu selection aborted
          case KEYDOWN:          //DOWN arrow key - move cursor down
              cur++;
              if(cur >= dim) {  //wrapping from the end to the beginning if needed
                  cur = 1;
                  upp = 1;
              } else if(upp + bnd == cur)
                  upp++;        //follow upp if cur is at the bottom
              break;
          case KEYUP:          //UP arrow key - move cursor up
              cur--;
              if(!cur) {        //wrapping from the beginning to the end if needed
                  cur = dim - 1;
                  if(upp + bnd <= cur)
                      upp = cur - bnd + 1;
              } else if(cur == upp - 1) //follow upp if cur is at the top
                  upp = cur;
              break;
          case KEY1:
          case KEY2:
          case KEY3:
          case KEY4:
          case KEY5:
          case KEY6:
          case KEY7:
          case KEY8:
          case KEY9:            //numeric key: direct selecting
              key -= KEY0;
              if(key > dim - 1) {   //if there is not enough menu items - beep
                  Beep();
                  break;
              }
              cur = (byte) key; //adjust screen to put current item on the top
              upp = cur;
              if(upp + bnd > dim)
                  upp = dim - bnd;  //adjust upp to display as many items as possible
              break;
		//case KEYATM1:
		  case KEYATM2:
		  case KEYATM3:
		  case KEYATM4:
		  	 key -= KEYATM1; //4212
			 return (upp * MNUMAX + upp + key - 1);
			 break;
          default:
              Beep();
              break;            //key not accepted
        }
    }
  lblKO:return -1;
}

#ifdef	BANK_MANDIRI
static void hdlSelect(word MnuItm)
{
	int ret;
	char buf[lenMnu + 1];

	traReset();
	InitTransInfo();

	MAPPUTWORD(traMnuItm, MnuItm, lblKO);
	MAPGET(MnuItm, buf, lblKO);
	MAPPUTSTR(traCtx, buf + 2, lblKO);

	switch (MnuItm)
	{
			// *** First level submenu(s) ***
		case mnuCustomer:
			hdlSubmenu(MnuItm, mnuMerchant);
			break;
		case mnuMerchant:
			hdlSubmenu(MnuItm, mnuAdmin);
			break;
		case mnuAdmin:
			hdlSubmenu(MnuItm, mnuEnd);
			break;

		case mnuNext:
			hdlSubmenu(MnuItm, mnuEchoTest);
			break;

		case mnuReprint:
			hdlSubmenu(MnuItm, mnuReport);
			break;

			//START - Handle prepaid feature
		case mnuPrepaid:
			hdlSubmenu(MnuItm, mnuMiniATM);
			break;
		case mnuCheckSaldo:
			PrepaidCheckBalance();
			break;
		case mnuPurchase:
			PrepaidPurchase();
			break;
		case mnuTopUp:
			break;
		case mnuUpdateBalance:
			break;
		case mnuHistoryTransaksi:
			break;
		case mnuKartuAktifasi:
			break;
			//END - Handle prepaid feature
			
			//MiniATM
		case mnuMiniATM:
			TransMiniAtm();
			CommOnHook(FALSE);
			break;
		case mnuTarikTunai:
			break;

			// *** Items regarding customer ***			
		case mnuSale:
			TransSale();
			CommOnHook(FALSE);	//disconnect
			break;
		case mnuRefund:
			TransRefund();
			CommOnHook(FALSE);	//disconnect
			break;
		case mnuManual:
			TransSale();
			break;
		case mnuVoid:
			TransVoid();
			CommOnHook(FALSE);	//disconnect
			break;
		case mnuOffline:
			TransOffSale();
			break;
		case mnuPreaut:
			TransAuth(TRANS_TYPE_PREAUTH);
			CommOnHook(FALSE);	//disconnect
			break;
		case mnuCardVer:
			break;
		case mnuLogon:
			TransLogonAllAcq();
			CommOnHook(FALSE);
			break;
		case mnuPowerBuy:
			hdlSubmenu(MnuItm, mnuTarikTunai);
			break;
		case mnuTenor03:
		case mnuTenor06:
		case mnuTenor12:	
		case mnuTenor18:			
		case mnuTenor24:	
		case mnuTenor36:	
			TransPowerBuy();
			CommOnHook(FALSE);	//disconnect
			break;

		case mnuDownloadLTWK:
			LTWKKeyDownload();
			CommOnHook(FALSE);
			break;
			// *** Items regarding merchant ***
			// *** Second level submenu(s) ***
		case mnuMrcInfo:
			hdlSubmenu(MnuItm, mnuMrcBatStan);
			break;
		case mnuMrcBatStan:
			hdlSubmenu(MnuItm, mnuMrcLog);
			break;
		case mnuMrcLog:
			hdlSubmenu(MnuItm, mnuMrcChgPwd);
			break;
		case mnuSettlement:
			TransSettle();
			break;
		case mnuMrcChgPwd:
			break;
		case mnuMrcReset:
			DoClear();
			break;
		case mnuMrcResetRev:
			ClearReversal();
			break;
		case mnuMonitoring:
			//procMonitoring();
			break;
		case mnuLogError:
			break;
			// *** Items regarding Info ***
		case mnuMrcInitRpt:
			break;
		case mnuMrcViewTot:
			break;
		case mnuMrcSWver:
			break;

			// *** Items regarding Batch Stan ***
		case mnuMrcViewRoc:
			break;
		case mnuMrcSetRoc:
			break;
		case mnuMrcViewBatNum:
			break;
		case mnuMrcViewSTAN:
			break;
		case mnuMrcSetSTAN:
			break;
		case mnuMrcViewBat:
			break;
		case mnuMrcViewTxn:
			break;

			// *** Items regarding Log ***
		case mnuReport:
			hdlSubmenu(MnuItm, mnuSettlement);
			break;
	   	case mnuAnyTrx:	
		case mnuMrcPrnTra:
			RePrnSpecTrans();
			break;
	   	case mnuLastTrx:	
		case mnuMrcReprint:
			PrnLastTrans();
			break;
		case mnuMrcSetDup:
		case mnuMrcSetDup2:
			RePrnSettle();
			break;
		case mnuMrcHostTot:
		case mnuMrcHostTot2:	//summary report
		
			break;
		case mnuMrcDetails://audit report
		case mnuMrcDetails2:	
			break;
		case mnuMrcBatTot2:	
			break;

		// *** Items regarding TLE ***
		case mnuLTMK:
			LTMKKeyDownload();
			break;
		case mnuLTWK:
			LTWKKeyDownload();
			break;


			// *** Items regarding administrator ***
			// *** Second level submenu(s) ***
		case mnuTMS:
			hdlSubmenu(MnuItm, mnuTerm);
			break;
		case mnuTerm:
			hdlSubmenu(MnuItm, mnuCmm);
			break;
		case mnuCmm:
			hdlSubmenu(MnuItm, mnuTLE);
			break;
		case mnuTLE:
			hdlSubmenu(MnuItm, mnuEnd);
			break;

			// *** Items regarding TMS ***
		case mnuTMSLP:
			break;
		case mnuTMSVC:
			break;
		case mnuTMSPC:
			break;
		case mnuTMSConfig:
			break;

			// *** Items regarding Terminal ***
		case mnuTDT:
			break;
		case mnuTermAppli:
			break;
		case mnuTMKey:
			break;
		case mnuEDCFunction:
		break;	


			// *** Items regarding Communication ***
		case mnuCmmVS:
			break;
		case mnuCmmPS:
			break;
		case mnuCmmLS:
			break;
		case mnuCmmGS:
			break;
		case mnuCmmCS:
			break;

		default:
			break;
	}
	goto lblEnd;
lblKO:
lblEnd:
	;
}
#endif

#ifdef BANK_BRI
//static void hdlSelect(word MnuItm)
int hdlSelect(word MnuItm)	//irf,  02 Jan 2014
{
	int ret;
	int ret2=0;	//irf,  02 Jan 2014
	char buf[lenMnu + 1];

	traReset();
	InitTransInfo();
	
	MAPPUTWORD(traMnuItm, MnuItm, lblKO);
	MAPGET(MnuItm, buf, lblKO);
	MAPPUTSTR(traCtx, buf + 2, lblKO);

	switch (MnuItm)
	{
// *************** START PURCHASE *****************
		case mnuPurchase:
			hdlSubmenu(MnuItm, mnuBrizzi);
			break;

			case mnuSale:
				TransSale();
				CommOnHook(FALSE);	//disconnect
				break;
			case mnuRedeemption:
				TransSaleRedeemption();
				CommOnHook(FALSE);
				break;
			case mnuPrivilege:
				TransPrivilage();
				CommOnHook(FALSE);
				break;
			case mnuCicilanBRI:
				hdlSubmenu(MnuItm, mnuRefund);
				break;
				
				case mnuPlan1:
					TransCicilanBring();
					CommOnHook(FALSE);
					MAPPUTWORD(traMnuItm, mnuCicilanBRI, lblKO);	//irf,  02 Jan 2014
					ret2=-1;	//back  to mnuCicilanBri 
					break;
				case mnuPlan2:
					ret2=TransCicilanBring();
					CommOnHook(FALSE);
					MAPPUTWORD(traMnuItm, mnuCicilanBRI, lblKO);	//irf,  02 Jan 2014
					ret2=-1;	//back  to mnuCicilanBri 
					break;
				case mnuPlan3:
					ret2=TransCicilanBring();
					CommOnHook(FALSE);
					MAPPUTWORD(traMnuItm, mnuCicilanBRI, lblKO);	//irf,  02 Jan 2014
					ret2=-1;	//back  to mnuCicilanBri 
					break;
			case mnuRefund:
				TransRefund();
				CommOnHook(FALSE);	//disconnect
				break;
			case mnuPreaut: //CARDVER
				TransAuth(TRANS_TYPE_PREAUTH);
				CommOnHook(FALSE);	//disconnect
				break;
			case mnuVoid:
				TransVoid();
				CommOnHook(FALSE);	//disconnect
				break;
			case mnuSettlement:
				TransSettleNew();
				CommOnHook(FALSE);	//disconnect
				break;
			case mnuReprint:
				hdlSubmenu(MnuItm, mnuReport);
				break;
			
				case mnuLastTrx:
	   			PrnLastTrans();
					break;
	   			case mnuAnyTrx:
	   			RePrnSpecTrans();
					break;
				
			case mnuReport:
				hdlSubmenu(MnuItm, mnuBrizzi);
				break;
				case mnuDetailReport:
					PrintDetailReport();
					break;
				case mnuSummaryReport:
					PrintSummaryReport();
					break;
	
// *************** END PURCHASE *****************

// *************** START BRIZZI ******************
		case mnuBrizzi:
			hdlSubmenu(MnuItm, mnuAbsensi);
			break;

			case mnuInitSAM:
				SAMPrepair(TRUE);
				break;
			case mnuInfoSaldo:
				BrizziCheckBalance();
				break;
			case mnuInfoDeposit:
				BrizziInfoDeposit();
				CommOnHook(FALSE);
				break;
			case mnuSaleBrizzi:
				hdlSubmenu(MnuItm, mnuTopup);			
				break;
				case mnuSaleNormal:
					BrizziOfflinePayment(FALSE);
					break;
				case mnuSaleDiskon:
					BrizziOfflinePayment(TRUE);
					break;
			case mnuTopup:
				BrizziTopUpOnLine();
				CommOnHook(FALSE);
				break;
			case mnuTopupDeposit:
				BrizziTopUpDeposit();
				CommOnHook(FALSE);
				break;
			case mnuAktivasi:
				BrizziAktivasiDeposit();
				CommOnHook(FALSE);
				break;
			case mnuVoidBrizzi:
				BrizziVoid();
				CommOnHook(FALSE);
				break;
			case mnuSettlementBrizzi:
				BrizziSettle();
				CommOnHook(FALSE);
				break;
			case mnuRedeem:
				BrizziRedeem();
				CommOnHook(FALSE);
				break;
			case mnuPrintLastLog:
				BrizziPrintLog();
				break;
			case mnuInfoStatusKartu:
				BrizziInfoKartu();
				break;
			case mnuReaktivasi:
				BrizziReAktivasi();
				CommOnHook(FALSE);
				break;
			case mnuReprintBrizzi:
				hdlSubmenu(MnuItm, mnuReportBrizzi);			
				break;
				case mnuLastTrxBrizzi:
					BrizziPrnLastTrans();
					break;
				case mnuAnyTrxBrizzi:
					BrizziRePrnSpecTrans();
					break;
			case mnuReportBrizzi:
				hdlSubmenu(MnuItm, mnuAbsensi);			
				break;
				case mnuSummaryAllBrizzi:
					BrizziSummaryReport(FALSE);
					break;
				case mnuSummaryDateBrizzi:
					BrizziSummaryReport(TRUE);
					break;
				case mnuDetailAllBrizzi:
					BrizziDetailReport(FALSE);
					break;
				case mnuDetailDateBrizzi:
					BrizziDetailReport(TRUE);
					break;
// *************** END BRIZZI ******************	


// *************** START ABSENSI ******************	
		case mnuAbsensi:
			hdlSubmenu(MnuItm, mnuCardServices);
			break;
			case mnuDatang:
				TransAbsensi();
				CommOnHook(FALSE);
				break;
			case mnuPulang:
				TransAbsensi();
				CommOnHook(FALSE);
				break;
			case mnuIstrhtMulai:
				TransAbsensi();
				CommOnHook(FALSE);
				break;
			case mnuIstrhtSelesai:
				TransAbsensi();
				CommOnHook(FALSE);
				break;
			case mnuLemburDatang:
				TransAbsensi();
				CommOnHook(FALSE);
				break;
			case mnuLemburPulang:
				TransAbsensi();
				CommOnHook(FALSE);
				break;
			case mnuShif:
				hdlSubmenu(MnuItm, mnuCardServices);
				break;
				case mnuShif1:
					hdlSubmenu(MnuItm, mnuShif2);
					break;
					case mnuShif1Datang:
						TransAbsensi();
						CommOnHook(FALSE);
						break;	
					case mnuShif1Pulang:
						TransAbsensi();
						CommOnHook(FALSE);
						break;			
				case mnuShif2:
					hdlSubmenu(MnuItm, mnuShif3);
					break;
					case mnuShif2Datang:
						TransAbsensi();
						CommOnHook(FALSE);
						break;
					case mnuShif2Pulang:
						TransAbsensi();
						CommOnHook(FALSE);
						break;			
				case mnuShif3:
					hdlSubmenu(MnuItm, mnuCardServices);
					break;
					case mnuShif3Datang:
						TransAbsensi();
						CommOnHook(FALSE);
						break;
					case mnuShif3Pulang:
						TransAbsensi();
						CommOnHook(FALSE);			
						break;	
// *************** END ABSENSI ******************	


// *************** START CARD SERVICES *****************
		case mnuCardServices:
			hdlSubmenu(MnuItm, mnuMiniATM);
			break;

			case mnuAktivasiKartu:
				TransAktivasiKartu();
				CommOnHook(FALSE);
				break;
			case mnuRenewalKartu:
				TransRenewalKartu();
				CommOnHook(FALSE);
				break;
			case mnuReissuePin:
				TransReissuePin();
				CommOnHook(FALSE);
				break;
			case mnuUbahPinCS:
				TransUbahPin();
				CommOnHook(FALSE);
				break;
			case mnuUbahPwdSVC:
				TransUbahPwdSPV();
				CommOnHook(FALSE);
				break;
				
			case mnuReprintCS:
				hdlSubmenu(MnuItm, mnuReportCS);
				break;

				case mnuRepLastTransCS:
						CardServicesPrnLastTrans();
						break;
					case mnuRepAnyTransCS:
						CardServicesRePrnSpecTrans();
						break;
				
			case mnuReportCS:
				hdlSubmenu(MnuItm, mnuMiniATM);
				break;
		
					case mnuRepTodaySum:
						CardServicesSummaryReport(FALSE);
						//
						break;
					case mnuRepTodayDet:
						CardServicesDetailReport(FALSE);
						//
						break;
					case mnuRepPastSum:
						CardServicesSummaryReport(TRUE);
						//
						break;
					case mnuRepPastDet:
						CardServicesDetailReport(TRUE);
						//
						break;
// *************** END CARD SERVICES *****************						

// *************** START MINI ATM *****************	
		//Level 1
		case mnuMiniATM:
			hdlSubmenu(MnuItm, mnuTunai);
			break;	
			//Level 2
			case mnuRegistrasi:
   				hdlSubmenu(MnuItm, mnuInformasi);
				break;
				//Level 3
				case mnuSMSBanking:
					TransSMSBanking();
					CommOnHook(FALSE);
					break;
				case mnuPhoneBanking:
					TransPhoneBanking();
					CommOnHook(FALSE);
					break;
				case mnuIBanking:
					TransIBanking();
					CommOnHook(FALSE);
					break;
				case MnuTrxPB:
					hdlSubmenu(MnuItm, mnuInformasi);
					break;	
					case MnuTrxPBTransfer:
					TransTrxPBTransfer();
					CommOnHook(FALSE);
					break;
					case MnuTrxPBNoHP:
					TransTrxPBNoHP();
					CommOnHook(FALSE);
					break;

			case mnuInformasi:
   				hdlSubmenu(MnuItm, mnuTransfer);
				break;

				case mnuInformasiSaldo:
					TransInfoSaldo();
					CommOnHook(FALSE);
					break;
				case mnuInfoSaldoBankLain:
					TransInfoSaldoBankLain();
					CommOnHook(FALSE);
					break;
				case mnuMiniStatement:
					TransMiniStatement();
					CommOnHook(FALSE);
					break;
				case mnuMutasiRek:
					TransMutasiRekening();
					CommOnHook(FALSE);
					break;
			
   			case mnuTransfer:
   				hdlSubmenu(MnuItm, mnuPembayaran);
				break;	

				case mnuTransferBRI:
					TransTrfSesamaBRI();
					CommOnHook(FALSE);
					break;
				case mnuTransAntarBank:
					TransTrfAntarBank();
					CommOnHook(FALSE);
					break;
				case mnuInfoKodeBank:
					TransInfoKodeBank();
					break;
// PEMBAYARAN			
			case mnuPembayaran:
   				hdlSubmenu(MnuItm, mnuIsiUlang);
				break;

					case mnuTelkom:
	   				TransPembayaranTelkom();
					CommOnHook(FALSE);
					break;
					
					case mnuPascaBayar:
   					hdlSubmenu(MnuItm, mnuPLN);
						break;

						case mnuHalo:
							TransPembayaranHalo();
							CommOnHook(FALSE);
							break;
						case mnuMatrix:
							TransPembayaranMatrix();
							CommOnHook(FALSE);
							break;
							
    					case mnuPLN:
   	       			hdlSubmenu(MnuItm, mnuKK_KTA);
						break;

						case mnuPascaPLN:
							TransPembayaranPLNPascaBayar();
							CommOnHook(FALSE);
							break;

						case mnuPraPLN:
							TransPembayaranPLNPraBayar();
							CommOnHook(FALSE);
							break;

						case mnuTokenPLN:
							TransPembayaranPLNToken();
							CommOnHook(FALSE);
							break;
						
					case mnuKK_KTA:
					hdlSubmenu(MnuItm, mnuCicilan);
						break;

						case mnuKKBRI:
							TransPembayaranKKBRI();
							CommOnHook(FALSE);
							break;

						case mnuCitibank:
						hdlSubmenu(MnuItm, mnuStandChat);
							break;

							case mnuKKCitibank:
								TransPembayaranKK_KTA(TRANS_TYPE_KKCITIBANK);
//								TransPembayaranKKCitibank();
								CommOnHook(FALSE);
								break;
    							case mnuKTACitibank:
								TransPembayaranKK_KTA(TRANS_TYPE_KTACITIBANK);
//								TransPembayaranKTACitibank();
								CommOnHook(FALSE);
								break;

						case mnuStandChat:
						hdlSubmenu(MnuItm, mnuHSBC);
							break;

							case mnuKKStandChat:
								TransPembayaranKK_KTA(TRANS_TYPE_KKSCB);
								//TransPembayaranKKSCB();
								CommOnHook(FALSE);
								break;
    							case mnuKTAStandChat:
								TransPembayaranKK_KTA(TRANS_TYPE_KTASCB);
								//TransPembayaranKTASCB();
								CommOnHook(FALSE);
								break;

						case mnuHSBC:
						hdlSubmenu(MnuItm, mnuAMRO);
							break;

							case mnuKKHSBC:
								TransPembayaranKK_KTA(TRANS_TYPE_KKHSBC);
								//TransPembayaranKKHSBC();
								CommOnHook(FALSE);
								break;
    							case mnuKTAHSBC:
								TransPembayaranKK_KTA(TRANS_TYPE_KTAHSBC);	
								//TransPembayaranKTAHSBC();
								CommOnHook(FALSE);
								break;

						case mnuAMRO:
							hdlSubmenu(MnuItm, mnuKKANZ);
							break;	

							case mnuKKAMRO:
								TransPembayaranKK_KTA(TRANS_TYPE_KKRBS);
								//TransPembayaranKKRBS();
								CommOnHook(FALSE);
								break;
    							case mnuKTAAMRO:
								TransPembayaranKK_KTA(TRANS_TYPE_KTARBS);	
								//TransPembayaranKTARBS();
								CommOnHook(FALSE);
								break;
								
						case mnuKKANZ:
							hdlSubmenu(MnuItm, mnuCicilan);
							TransPembayaranKK_KTA(TRANS_TYPE_KKANZ);
							//TransPembayaranKKANZ();
							CommOnHook(FALSE);
							break;

			case mnuCicilan:
				hdlSubmenu(MnuItm, mnuZakat);
				break;		

				case mnuFIF:
					TransPembayaranCICILAN();
					CommOnHook(FALSE);
					break;
    				case mnuBAF:
					TransPembayaranCICILAN();
					CommOnHook(FALSE);
					break;
    				case mnuOTO:
					TransPembayaranCICILAN();
					CommOnHook(FALSE);
					break;
    				case mnuFinansia:
					TransPembayaranCICILAN();
					CommOnHook(FALSE);
					break;
    				case mnuVerena:
					TransPembayaranCICILAN();
					CommOnHook(FALSE);
					break;
    				case mnuWOM:
  					TransPembayaranCICILAN();
					CommOnHook(FALSE);
					break;

			case mnuZakat:
				hdlSubmenu(MnuItm, mnuInfaq);
				break;

				case mnuDmptDuafaZ:
					TransPembayaranZakat();
					CommOnHook(FALSE);
					break;
    				case mnuYBMZ:
					TransPembayaranZakat();
					CommOnHook(FALSE);
					break;

			case mnuInfaq:
				hdlSubmenu(MnuItm, mnuDPLK);
				break;

				case mnuDmptDuafaI:
					TransPembayaranInfaq();
					CommOnHook(FALSE);
					break;
    				case mnuYBMI:
					TransPembayaranInfaq();
					CommOnHook(FALSE);
					break;
		
			case mnuDPLK:
				TransPembayaranDPLK();
				CommOnHook(FALSE);
				break;


			case mnuTiketPswt:
				hdlSubmenu(MnuItm, mnuBriva);
				break;

				case mnuGaruda:
					TransPembayaranTiketPesawat();
					CommOnHook(FALSE);
					break;
    				case mnuLion:
					TransPembayaranTiketPesawat();
					CommOnHook(FALSE);
					break;
    				case mnuSriwjy:
					TransPembayaranTiketPesawat();
					CommOnHook(FALSE);
					break;
    				case mnuMandala:
  					//ditutup
					break;
				case mnuMerpati:
					TransPembayaranTiketPesawat();
					CommOnHook(FALSE);
					break;
			case mnuBriva:
				TransBriva();
				CommOnHook(FALSE);
			break;
			
			case mnuPDAM:
				hdlSubmenu(MnuItm, mnuPendidikan);
				break;
				
				case mnuPDAMSBY:
					//ditutup
					break;
					
			case mnuPendidikan:
				hdlSubmenu(MnuItm, mnuTvLangganan);
				break;

				case mnuByrSPP:
					TransPembayaranKuliah();
					CommOnHook(FALSE);
					break;
    				case mnuKdKuliah:
					TransInfoKodeKuliah();
					break;
				
			case mnuTvLangganan:
				hdlSubmenu(MnuItm, mnuIsiUlang);
				break;

				case mnuIndovision:
					//ditutup
					break;
    				case mnuOkTv:
					//ditutup
					break;
    				case mnuTopTv:
					//ditutup
					break;
  
			case mnuIsiUlang:
				hdlSubmenu(MnuItm, mnuSetorPasti);
				break;

				case mnuAxis:
  					hdlSubmenu(MnuItm, mnuEsia);
					break;

					case mnuAxis25rb:
   					case mnuAxis50rb:
   					case mnuAxis75rb:
   					case mnuAxis100rb:
   					case mnuAxis150rb:
   					case mnuAxis200rb:
   					case mnuAxis300rb:
   						TransIsiUlang();
						CommOnHook(FALSE);
						break;

				case mnuEsia:
  					hdlSubmenu(MnuItm, mnuImThree);
					break;

					case mnuEsia25rb:
    					case mnuEsia50rb:
    					case mnuEsia100rb:
    					case mnuEsia150rb:
    					case mnuEsia250rb:
   						TransIsiUlang();
						CommOnHook(FALSE);
						break;

				case mnuImThree:
  					hdlSubmenu(MnuItm, mnuMentari);
					break;

					case mnuImThree25rb:
    					case mnuImThree50rb:
    					case mnuImThree75rb:
    					case mnuImThree100rb:
    					case mnuImThree150rb:
    					case mnuImThree200rb:
    					case mnuImThree500rb:
    					case mnuImThree1jt:
   						TransIsiUlang();
						CommOnHook(FALSE);
						break;
					
				case mnuMentari:
   					hdlSubmenu(MnuItm, mnuSimpati);
					break;

					case mnuMentari25rb:
    					case mnuMentari50rb:
    					case mnuMentari100rb:
					case mnuMentari150rb:
    					case mnuMentari250rb:
    					case mnuMentari500rb:
    					case mnuMentari1jt:
    						TransIsiUlang();
						CommOnHook(FALSE);
						break;
					
				case mnuSimpati:
  					hdlSubmenu(MnuItm, mnuSmartFren);
					break;
					
					case mnuSimpati10rb:
					case mnuSimpati20rb:
					case mnuSimpati25rb:
					case mnuSimpati50rb:
    					case mnuSimpati100rb:
    					case mnuSimpati150rb:
    					case mnuSimpati200rb:
    					case mnuSimpati300rb:
    					case mnuSimpati500rb:
    					case mnuSimpati1jt:
   	    					TransIsiUlang();
						CommOnHook(FALSE);
   	    					break;
					
				case mnuSmartFren:
  					hdlSubmenu(MnuItm, mnuThree);
					break;

					case mnuSmartFren20rb:
					case mnuSmartFren25rb:
    					case mnuSmartFren50rb:
    					case mnuSmartFren100rb:
					case mnuSmartFren150rb:
    					case mnuSmartFren200rb:
					case mnuSmartFren300rb:
					case mnuSmartFren500rb:
   						TransIsiUlang();
						CommOnHook(FALSE);
						break;
				
    				case mnuThree:
  					hdlSubmenu(MnuItm, mnuXL);
					break;

					case mnuThree20rb:
   					case mnuThree50rb:
   					case mnuThree75rb:
   					case mnuThree100rb:
   					case mnuThree150rb:
   					case mnuThree300rb:
   					case mnuThree500rb:
   						TransIsiUlang();
						CommOnHook(FALSE);
						break;
						
    				case mnuXL:
  					hdlSubmenu(MnuItm, mnuSetorPasti);
					break;
					case mnuXLV20rb:
					case mnuXLV25rb:
    					case mnuXLV50rb:
    					case mnuXLV100rb:
    					case mnuXLV200rb:
					case mnuXLV500rb:
    						TransIsiUlang();
						CommOnHook(FALSE);
						break;

			case mnuSetorPasti:
				hdlSubmenu(MnuItm, mnuUbahPin);
				TransSetorPasti();
				break;

			case mnuUbahPin:
				hdlSubmenu(MnuItm, mnuReprintReviewMini);
				TransUbahPin();
				CommOnHook(FALSE);
				break;

			case mnuReprintReviewMini:
				hdlSubmenu(MnuItm, mnuReportMini);
				break;
				
				case mnuReprintMini:
					hdlSubmenu(MnuItm, mnuReviewMini);
					break;
				
					case mnuRepLastTransMini:
						MiniAtmPrnLastTrans();
						break;
					case mnuRepAnyTransMini:
						MiniAtmRePrnSpecTrans();
						break;
						
				case mnuReviewMini:
					hdlSubmenu(MnuItm, mnuReportMini);
					break;
					
					case mnuReviewLast:
						MiniAtmReviewLastTrans();
						break;
					case mnuReviewAny:
						MiniAtmReviewAnyTrans();
						break;

			case mnuReportMini:
				hdlSubmenu(MnuItm, mnuTunai);
				break;

    			case mnuTodaySummary:
				MiniATMSummaryReport(FALSE);
				break;
   				case mnuTodayDetail:
				MiniATMDetailReport(FALSE);
				break;
   				case mnuPastSummary:
				MiniATMSummaryReport(TRUE);
				break;
   				case mnuPastDetail:
				MiniATMDetailReport(TRUE);
				break;
				
  // *************** END MINI ATM *****************	

  // *************** START TUNAI  *****************	

  
  		case mnuTunai:
			hdlSubmenu(MnuItm, mnuTBank);
			break;

			case mnuTarikTunai:
				TransTarikTunai();
				CommOnHook(FALSE);
				break;
   			case mnuPemPinjaman:
				TransSetorPinjaman();
				CommOnHook(FALSE);
   				break;
   			case mnuSetorSimpanan:
				TransSetorSimpanan();
				CommOnHook(FALSE);
				break;
			case mnuVoidTarikTunai:
				TransVoidTarikTunai();
				CommOnHook(FALSE);
				break;
			
			case mnuReprintReviewTunai:
			hdlSubmenu(MnuItm, mnuReportTunai);
			break;
			
					case mnuRepLastTransTunai:
						TunaiPrnLastTrans();
						break;
					case mnuRepAnyTransTunai:
						TunaiRePrnSpecTrans();
						break;
			
			case mnuReportTunai:
				hdlSubmenu(MnuItm, mnuTBank);
				break;
					
    				case mnuTodaySummaryTunai:
					TunaiSummaryReport(FALSE);
					break;
   				case mnuTodayDetailTunai:
					TunaiDetailReport(FALSE);
					break;
   				case mnuPastSummaryTunai:
					TunaiSummaryReport(TRUE);
					break;
   				case mnuPastDetailTunai:
   					TunaiDetailReport(TRUE);
					break;

  // *************** END TUNAI  *****************	

  // ********************** T-BANK *************************** 
  
 		case mnuTBank:
			hdlSubmenu(MnuItm, mnuSSB);
			break;
		case mnuCashIn:
			TransCashIn();
			break;
		case mnuCashOut:
			TransCashOut();
			break;
		case mnuReprintTBank:
			hdlSubmenu(MnuItm, mnuReportTBank);
			break;
			
			case mnuRepLastTransTBank:
				TBankPrnLastTrans();
				break;
			case mnuRepAnyTransTBank:
				TBankRePrnSpecTrans();
				break;		
				
		case mnuReportTBank:
			hdlSubmenu(MnuItm, mnuSSB);
			break;
			
			case mnuRepTodaySumTBank:
				TBankSummaryReport(FALSE);
				break;
			case mnuRepTodayDetTBank:
				TBankDetailReport(FALSE);
				break;
   			case mnuRepPastSumTBank:
				TBankSummaryReport(TRUE);
				break;
   			case mnuRepPastDetTBank:
 				TBankDetailReport(TRUE);
				break;

  // ********************** SSB ***************************


		case mnuSSB:
			hdlSubmenu(MnuItm, mnuLogOn);
			break;
		case mnuSIM:
			hdlSubmenu(MnuItm, mnuUjiSimulator);
			break;
		case mnuSIMBaru:
			hdlSubmenu(MnuItm, mnuSIMPerpanjangan);
			break;
		case mnuSIMPerpanjangan:
			hdlSubmenu(MnuItm, mnuSIMPeningkatan);
			break;
		case mnuSIMPeningkatan:
			hdlSubmenu(MnuItm, mnuSIMPenurunan);
			break;
		case mnuSIMPenurunan:
			hdlSubmenu(MnuItm, mnuMutasi);
			break;
		case mnuMutasi:
			hdlSubmenu(MnuItm, mnuHilangRusak);
			break;
		case mnuMutGol:
			hdlSubmenu(MnuItm, mnuMutPeningkat);
			break;
		case mnuMutPeningkat:
			hdlSubmenu(MnuItm, mnuMutPenurun);
			break;
		case mnuMutPenurun:
			hdlSubmenu(MnuItm, mnuHilangRusak);
			break;
		case mnuHilangRusak:
			hdlSubmenu(MnuItm, mnuUjiSimulator);
			break;
		case mnuHilGol:
			hdlSubmenu(MnuItm, mnuHilPeningkat);
			break;
		case mnuHilPeningkat:
			hdlSubmenu(MnuItm, mnuHilPenurun);
			break;
		case mnuHilPenurun:
			hdlSubmenu(MnuItm, mnuUjiSimulator);
			break;
		case mnuBPKB:
			hdlSubmenu(MnuItm, mnuSTCK);
			break;
		case mnuBPKBBaru:
			hdlSubmenu(MnuItm, mnuBPKBGanti);
			break;
		case mnuBPKBGanti:
			hdlSubmenu(MnuItm, mnuSTCK);
			break;
		case mnuMutasiRanMor:
			hdlSubmenu(MnuItm, mnuLogOn);
			break;		
		case mnuSIMBaruA:
		case mnuSIMBaruC:
		case mnuSIMBaruD:
		case mnuSIMPerpanjangA:
		case mnuSIMPerpanjangAum:
		case mnuSIMPerpanjangB1:
		case mnuSIMPerpanjangB1um:
		case mnuSIMPerpanjangB2:
		case mnuSIMPerpanjangB2um:
		case mnuSIMPerpanjangC:
		case mnuSIMPerpanjangD:
		case mnuSIMPeningkatAum:
		case mnuSIMPeningkatB1:
		case mnuSIMPeningkatB1um:
		case mnuSIMPeningkatB2:
		case mnuSIMPeningkatB2um:
		case mnuSIMPenurunA:
		case mnuSIMPenurunAum:
		case mnuSIMPenurunB1:
		case mnuSIMPenurunB1um:
		case mnuSIMPenurunB2:
		case mnuMutGolA:
		case mnuMutGolAum:
		case mnuMutGolB1:
		case mnuMutGolB1um:
		case mnuMutGolB2:
		case mnuMutGolB2um:
		case mnuMutGolC:
		case mnuMutGolD:
		case mnuMutPeningkatAum:
		case mnuMutPeningkatB1:
		case mnuMutPeningkatB1um:
		case mnuMutPeningkatB2:
		case mnuMutPeningkatB2um:
		case mnuMutPenurunA:
		case mnuMutPenurunAum:
		case mnuMutPenurunB1:
		case mnuMutPenurunB1um:
		case mnuMutPenurunB2:
		case mnuHilGolA:
		case mnuHilGolAum:
		case mnuHilGolB1:
		case mnuHilGolB1um:
		case mnuHilGolB2:
		case mnuHilGolB2um:
		case mnuHilGolC:
		case mnuHilGolD:
		case mnuHilPeningkatAum:
		case mnuHilPeningkatB1:
		case mnuHilPeningkatB1um:
		case mnuHilPeningkatB2:
		case mnuHilPeningkatB2um:
		case mnuHilPenurunA:
		case mnuHilPenurunAum:
		case mnuHilPenurunB1:
		case mnuHilPenurunB1um:
		case mnuHilPenurunB2:
		case mnuUjiSimulator:
		case mnuRoda23Baru:
		case mnuRoda4orBaru:
		case mnuRoda23Ganti:
		case mnuRoda4orGanti:
		case mnuSTCK:
		case mnuRoda23Mutasi:
		case mnuRoda4orMutasi:
			TransSSB();
			CommOnHook(FALSE);
			break;			
		
// ********************** SSB ***************************

  //LOGON
		case mnuLogOn:
			TransLogon();
			CommOnHook(FALSE);
			break;
  // END LOGON
  
		default:
			break;
	}
	goto lblEnd;
lblKO:
lblEnd:
	return ret2;
	;
}
#endif

#ifdef BANK_BUKOPIN
//static void hdlSelect(word MnuItm)
int hdlSelect(word MnuItm)	//irf,  02 Jan 2014
{
	int ret;
	int ret2=0;	//irf,  02 Jan 2014
	char buf[lenMnu + 1];

	// cek menu ATMB
	//MenuAtmbMerchantNasabah= FALSE;

	traReset();
	InitTransInfo();
	
	MAPPUTWORD(traMnuItm, MnuItm, lblKO);
	MAPGET(MnuItm, buf, lblKO);
	MAPPUTSTR(traCtx, buf + 2, lblKO);

	switch (MnuItm)
	{
// *************** START PURCHASE *****************
#ifdef BANK_ARTHA_JASA
		case mnuPurchase:
			TransPurchase();
			CommOnHook(FALSE);	//disconnect
			//hdlSubmenu(MnuItm, mnuVoid);
			break;
#endif



		case mnuUtamaKartuKredit:
				hdlSubmenu(mnuUtamaKartuKreditAwal,mnuEnd);
				break;

				case mnuUtamaKartuKreditAwal:
					hdlSubmenu(MnuItm,mnuEnd);
					break;

		case mnuUtamaMiniAtm:
				hdlSubmenu(mnuMiniAtmBkpn,mnuSetupBkpn);
				break;

		case mnuSale:
			TransSale();
			CommOnHook(FALSE);	//disconnect
			break;
/////		case mnuCash:
			/////----- TransCashAdvance();
/////			CommOnHook(FALSE);	//disconnect
/////			break;
#if 1


/*
		case mnuTransferBank:
				TransTrfAntarBankTes();
				CommOnHook(FALSE);	//disconnect
				break;
				*/

					

		
/*
		case mnuCardVer: //CARDVER
			TransAuth(TRANS_TYPE_PREAUTH);
			CommOnHook(FALSE);	//disconnect
			break;
		case mnuOffline:
			TransOffSale();
			break;
			*/
			///

		/*
		case mnuRefund:
			TransRefund();
			CommOnHook(FALSE);	//disconnect
			break;
			*/
			
#endif
		case mnuVoid:
			TransVoid();
			CommOnHook(FALSE);	//disconnect
			break;
		case mnuSettlement:
			TransSettleNew();
			CommOnHook(FALSE);	//disconnect
			break;
		case mnuFunctions:
			hdlSubmenu(MnuItm,mnuEnd);
			break;
			//LOGON
			case mnuHostLogon:
				//#ifdef BANK_ARTHA_JASA
				//TransLogonAJ();
				//#else
				TransLogonBukopin();
				//#endif
				CommOnHook(FALSE);
				break;    
			case mnuEchoTest:
				TransEchoTest();
				CommOnHook(FALSE);
				break;
			case mnuReprint:
				hdlSubmenu(MnuItm, mnuMrcBatTot);
				break;
		
				case mnuLastTrx:
	  				PrnLastTrans();
					CommOnHook(FALSE);
					break;
	  			case mnuAnyTrx:
	  				RePrnSpecTrans();
					CommOnHook(FALSE);
					break;
				case mnuMrcSetDup:
					RePrnSettleNew();
					CommOnHook(FALSE);
					break;		
			case mnuMrcBatTot:
				if(PasswordMerchant()!=0)
					return;
				ViewTotal();
				CommOnHook(FALSE);
				break;
			case mnuMrcViewBat:
				if(PasswordMerchant()!=0)
					return;
		  		ViewSpecList();
				CommOnHook(FALSE);
				break;
			case mnuMrcReset:
				//(PasswordMerchant()!=0)
				if(PasswordTerm()!=0)
					return;
				ClearAllRecord();
				CommOnHook(FALSE);
				break;
			case mnuMrcResetRev:
				//(PasswordMerchant()!=0)
				if(PasswordTerm()!=0)
					return;
				ClearReversal();
				CommOnHook(FALSE);
				break;
			case mnuReport:
				if(PasswordSettle()!=0)
					return;
				hdlSubmenu(MnuItm, mnuEnd);
				break;
				case mnuDetailReport:
					PrintDetailReport();
					break;
				case mnuSummaryReport:
					PrintSummaryReport();
					break;
			case mnuKoneksi:
				if(PasswordSettle()!=0)
					return;
#ifdef FUN2_READ_ONLY
				SetSysCommParam(PM_LOW);
#else
				SetSysCommParam(PM_MEDIUM);
#endif
				break;
				
	
// *************** END PURCHASE *****************

// ************ ADD FOR MINI ATM BUKOPIN (DIKI) ********
///********************************************************
		case mnuMiniAtmBkpn:
				hdlSubmenu(MnuItm,mnuSetorBkpn);
				break;

		case mnuMiniAtmBkpnMer:
			//MenuAtmbMerchantNasabah = TRUE;
				TransAtmbMerchantNasabahMenuTrue();
				hdlSubmenu(MnuItm,mnuVoid);
				break;

		case mnuMiniAtmBkpnNas:
			////MenuAtmbMerchantNasabah= FALSE;
				TransAtmbMerchantNasabahMenuFalse();
				hdlSubmenu(MnuItm,mnuVoid);
				break;

			case mnuSetorBkpn:
				TransSetorTunaiBkpn();
				CommOnHook(FALSE);
				break;

			case mnuTarikBkpn:
				TransTarikTunaiBkpn();
				CommOnHook(FALSE);
				break;					
		
			case mnuTransferBkpn:
	  			TransTrfAntarBankBkpn();
				CommOnHook(FALSE);
				break;

			case mnuPembelianBkpn:
				hdlSubmenu(MnuItm,mnuPln2Bkpn);
				break;
		
				case mnuEvoucherBkpn:
	  				hdlSubmenu(MnuItm,mnuPln2Bkpn);
					break;

					case mnuEvoucherBkpnXl:
	  				TransIsiUlangBkpn();
					CommOnHook(FALSE);
					break;

					case mnuEvoucherBkpnTelkomsel:
	  				TransIsiUlangBkpn();
					CommOnHook(FALSE);
					break;

					case mnuEvoucherBkpnIndosat:
	  				TransIsiUlangBkpn();
					CommOnHook(FALSE);
					break;

					case mnuEvoucherBkpnEsia:
	  				TransIsiUlangBkpn();
					CommOnHook(FALSE);
					break;

					case mnuEvoucherBkpnSmartfren:
	  				TransIsiUlangBkpn();
					CommOnHook(FALSE);
					break;


					

			case mnuPembayaranBkpn:
				hdlSubmenu(MnuItm,mnuPembelianBkpn);
				break;
		
				case mnuPln1Bkpn:
	  				//TransMultifinanceBkpn();
					CommOnHook(FALSE);
					break;

				case mnuTeleponBkpn:
	  				TransPembayaranTelkomBkpn();
					CommOnHook(FALSE);
					break;

				case mnuSelularBkpn:

					hdlSubmenu(MnuItm,mnuPamPdamBkpn);
					break;
		
					case mnuSelularBkpnHalo:
					memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Provider, "TELKOMSEL HALO", 14);
	  				TransPembayaranSelulerBkpn();
					CommOnHook(FALSE);
					break;

					case mnuSelularBkpnMatrix:
					memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Provider, "INDOSAT OOREDOO", 14);
	  				TransPembayaranSelulerBkpn();
					CommOnHook(FALSE);
					break;
					
	  				case mnuSelularBkpnXplore:
					memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Provider, "XL XPLORE", 9);
	  				TransPembayaranSelulerBkpn();
					CommOnHook(FALSE);
					break;

					case mnuSelularBkpnSmartfren:
					memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Provider, "SMARTFREN", 9);
	  				TransPembayaranSelulerBkpn();
					CommOnHook(FALSE);
					break;

					case mnuSelularBkpnEsia:
					memcpy(glProcInfo.stTranLog.BRI.PEMB_KARTUMATRIX.Provider, "ESIA", 4);
	  				TransPembayaranSelulerBkpn();
					CommOnHook(FALSE);
					break;
					

				case mnuPamPdamBkpn:
	  				hdlSubmenu(MnuItm,mnuMultifinanceBkpn);
					break;
		
					case mnuAetraBkpn:
	  				TransPembayaranPdamAetraBkpn();
					CommOnHook(FALSE);
					break;

					case mnuPalyjaBkpn:
	  				TransPembayaranPdamPalyjaBkpn();
					CommOnHook(FALSE);
					break;

				case mnuMultifinanceBkpn:
	  				hdlSubmenu(MnuItm,mnuKartuKreditBkpn);
					break;
		
					case mnuMafBkpn:
	  				TransMultifinanceBkpn();
					CommOnHook(FALSE);
					break;

					case mnuMcfBkpn:
	  				TransMultifinanceBkpn();
					CommOnHook(FALSE);
					break;

					case mnuWomBkpn:
	  				TransMultifinanceBkpn();
					CommOnHook(FALSE);
					break;

					case mnuBafBkpn:
	  				TransMultifinanceBkpn();
					CommOnHook(FALSE);
					break;

				case mnuKartuKreditBkpn:
					hdlSubmenu(MnuItm, mnuPembelianBkpn);
							break;

							case mnuKartuKreditBkpnAnz:
								TransPembayaranKK_Bkpn(TRANS_TYPE_KK_ANZ_BKPN_INQ);
//								TransPembayaranKKCitibank();
								CommOnHook(FALSE);
								break;
    						case mnuKartuKreditBkpnBkpn:
								TransPembayaranKK_Bkpn(TRANS_TYPE_KK_BUKOPIN_BKPN_INQ);
//								TransPembayaranKTACitibank();
								CommOnHook(FALSE);
								break;
							case mnuKartuKreditBkpnBni:
								TransPembayaranKK_Bkpn(TRANS_TYPE_KK_BNI_BKPN_INQ);
//								TransPembayaranKTACitibank();
								CommOnHook(FALSE);
								break;
							case mnuKartuKreditBkpnHsbc:
								TransPembayaranKK_Bkpn(TRANS_TYPE_KK_HSBC_BKPN_INQ);
//								TransPembayaranKTACitibank();
								CommOnHook(FALSE);
								break;


			case mnuCetakUlangBkpn:
				hdlSubmenu(MnuItm,mnuLaporanBkpn);
				break;
		
				case mnuTransaksiTerakhirBkpn:
	  				MiniAtmPrnLastTransBkpn();
					break;

				case mnuTransaksiNoBkpn:
	  				MiniAtmRePrnSpecTransBkpn();
					break;


			case mnuLaporanBkpn:
				if(MenuAtmbMerchantNasabah)
				{
				}
				else {
					ScrCls();
					PubBeepErr();
					PubDispStringASCII("TRX NOT AVAILABLE", DISP_LINE_CENTER|2);
					PubWaitKey(3);
					return 0;
					break;
					}
				//break;
				hdlSubmenu(MnuItm, mnuLainlainBkpn);
				break;

    			case mnuRekapHarianBkpn:
					MiniATMSummaryReport(FALSE);
					break;
				
   				case mnuDetailHarianBkpn:
					MiniATMDetailReport(FALSE);
					break;

				case mnuFeetransaksiBkpn:
					
					break;


			case mnuInformasiBkpn:
				hdlSubmenu(MnuItm,mnuCetakUlangBkpn);
				break;
		
				case mnuInfoSaldoBkpn:
	  				TransInfoSaldoBkpn();
					CommOnHook(FALSE);
					break;

				case mnuKodeBankBkpn:
	  				//TransInfoKodeBankBkpn();
					TransInfoKodeBankBkpn2();
					break;

				case mnuInfoTransaksiBkpn:
	  				///TransMultifinanceBkpn();
					break;


			
			case mnuLainlainBkpn:
				hdlSubmenu(MnuItm,mnuSetupBkpn);
				break;
					
				case mnuUbahPinBkpn:
					TransUbahPinBkpn();
					CommOnHook(FALSE);
					break;
		
				case mnuUbahBahasaBkpn:
					//TransInfoKodeBankBkpn();
					break;
					

			case mnuSetupBkpn:
				if(MenuAtmbMerchantNasabah)
				{
				}
				else {
					ScrCls();
					PubBeepErr();
					PubDispStringASCII("TRX NOT AVAILABLE", DISP_LINE_CENTER|2);
					PubWaitKey(3);
					return 0;
					break;
					}
				hdlSubmenu(MnuItm,mnuUtamaKartuKreditAwal);
				break;
					
				case mnuInitKartuBukopin:
					TransInitKartuBkpn();
					CommOnHook(FALSE);
					break;
					


				/*
	  		case mnuPlnBkpn:
	  			
				CommOnHook(FALSE);
				break;
			case mnuPdamBkpn:
				
				//CommOnHook(FALSE);
				MiniAtmPrnLastTransBkpn();
				break;	
				*/


/*
			case mnuSettingMBkpn:
				
				//CommOnHook(FALSE);
				MiniAtmRePrnSpecTransBkpn();
				break;	

				

			case mnuTelkomBkpn:
	  			//TransTrfAntarBankBkpn();
				CommOnHook(FALSE);
				break;

			case mnuSelPosBkpn:
	  			//TransTrfAntarBankBkpn();
				CommOnHook(FALSE);
				break;

			case mnuSelPreBkpn:
	  			//TransTrfAntarBankBkpn();
				CommOnHook(FALSE);
				break;

			case mnuPlnBkpn:
	  			hdlSubmenu(MnuItm,mnuPdamBkpn);
				break;

				case mnuPlnBkpn1:
	  				//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

				case mnuPlnBkpn2:
	  				//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

			case mnuPdamBkpn:
	  			hdlSubmenu(MnuItm,mnuMultiBkpn);
				break;
	
				case mnuPdamBkpn1:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;
				
				case mnuPdamBkpn2:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

				case mnuPdamBkpn3:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

			case mnuMultiBkpn:
	  			hdlSubmenu(MnuItm,mnuCCBkpn);
				break;

				case mnuMultiBkpn1:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

				case mnuMultiBkpn2:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

				case mnuMultiBkpn3:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

				case mnuMultiBkpn4:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

			case mnuCCBkpn:
	  			//TransTrfAntarBankBkpn();
				CommOnHook(FALSE);
				break;

			case mnuFBBkpn:
	  			hdlSubmenu(MnuItm,mnuTarikBkpn);
				break;

				case mnuFBBkpn1:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

			case mnuTarikBkpn:
	  			hdlSubmenu(MnuItm,mnuSetorBkpn);
				break;

				case mnuTarikBkpn1:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

				case mnuTarikBkpn2:
					//TransTrfAntarBankBkpn();
					CommOnHook(FALSE);
					break;

			case mnuSetorBkpn:
	  			//TransTrfAntarBankBkpn();
				CommOnHook(FALSE);
				break;
				*/

				

		/*
			case mnuTopupBkpn:
	  			//TransTrfAntarBankBkpn();
				CommOnHook(FALSE);
				break;

				*/












// ************ END FOR MINI ATM BUKOPIN (DIKI) ********





// ************* add for private label bkpn ****************
/*
		case mnuPrvtLbl:
				/////----- TransSalePrvtLbl();
				CommOnHook(FALSE);	//disconnect
				break;

		case mnuPrvtLblTes:
				/////----- TransSalePrvtLblTes();
				CommOnHook(FALSE);	//disconnect
				break;
	*/
					

// ************* end for private label bkpn ****************

					

		default:
			break;
	}
	goto lblEnd;
lblKO:
lblEnd:
	return ret2;
	;
}

#endif


static void hdlSubmenu(word msg1, word msg2)
{
	int ret;
	byte sta;                   //menu state, it is (upper item)*10 + (current item)
	char lvl;                   //menu level
	char mnu[MNUMAX][dspW + 1]; //the final menu array prepared to mnuSelect
	char *ptr[MNUMAX];          //array of pointers to mnu items
	word idx[MNUMAX];           //the correspondent indices
	byte idxP;                  //index within ptr array
	word idxM;                  //take values from msg1..msg2
	char itm[lenMnu + 1];       //(menu level)+(access type)+(item name)+(ending zero)

	//    byte MnuPop;

	memset(ptr, 0, sizeof(ptr));
	MAPGET(msg1, itm, lblKO);   //it is the menu name
	strcpy(mnu[0], itm + 2);    //omit menu level and access type
	lvl = itm[0] + 1;

	idxP = 0;
	ptr[idxP++] = mnu[0];       //it is the menu name
	ret = nvmHold(0);

	for (idxM = msg1 + 1; idxM < msg2; idxM++)
	{    //build mnu and ptr arrays
		MAPGET(idxM, itm, lblKO);   //retrieve menu item
		if(itm[0] != lvl)
			continue;           //only items of level lvl are included

		if(itm[1]=='?')			//modified
			continue;
		
		strcpy(mnu[idxP], itm + 2); //the first and second characters are menu level and access type, it is not showed
		ptr[idxP] = mnu[idxP];  //fill the pointers array
		idx[idxP] = idxM;       //save the index to be returned if this item will be selected
		if(++idxP >= MNUMAX)    //no more than MNUMAX items in menu
			break;
	}
	nvmRelease(0);
	
	if(idxP == 1)
		goto lblEnd;            //empty menu - nothing to do

	sta = 0;                    //it is started with the current item 1 at the top of the screen
	do
	{
		ret = mnuSelect((Pchar *) ptr, sta, 60);    //perform user dialog
		if(ret <= 0)            //timeout or aborted - nothing to do
			break;
		sta = ret;
		idxM = idx[sta % MNUMAX];   //sta%MNUMAX is the current item selected
		ret = hdlSelect((word) (idxM));
		if(ret<0)	//irf,  02 Jan 2014
			break;
	}
	while(sta);
	goto lblEnd;

lblKO:
lblEnd:
	;
}

#ifdef	BANK_MANDIRI
void PublicMenuPrepaid(int idx)
{
	int ret;
	
	char MenuTopUp[1+lenMnu];
	char MenuCekSaldo[1+lenMnu];
	char MenuUpdBal[1+lenMnu];
	char MenuHistory[1+lenMnu];

	MAPGET(mnuCheckSaldo, MenuCekSaldo, lblKO);
	MAPGET(mnuTopUp, MenuTopUp, lblKO);
	MAPGET(mnuUpdateBalance, MenuUpdBal, lblKO);
	MAPGET(mnuHistoryTransaksi, MenuHistory, lblKO);
	
	if(idx == 1){
		MenuCekSaldo[0] = '2';
		MenuUpdBal[0] = '2';
		MenuTopUp[0] = '2';
		MenuHistory[0] = '2';
	} else {
		MenuCekSaldo[0] = '?';
		MenuUpdBal[0] = '?';
		MenuTopUp[0] = '?';
		MenuHistory[0] = '?';
	}
		
	MAPPUTSTR(mnuCheckSaldo, MenuCekSaldo, lblKO);
	MAPPUTSTR(mnuUpdateBalance, MenuUpdBal, lblKO);
	MAPPUTSTR(mnuTopUp, MenuTopUp, lblKO);
	MAPPUTSTR(mnuHistoryTransaksi, MenuHistory, lblKO);

	lblKO:
		;
}
#endif


#ifdef	BANK_MANDIRI
void CekMenuAllowed()
{
	int ret;
	byte EMOffline;
	byte EMPreAuth;
	byte EMSale;
	byte EMPwrBuy;
	byte EMPrepaid;
	char MenuOffline[1+lenMnu];
	char MenuPreAuth[1+lenMnu];
	char MenuManual[1+lenMnu];
	char MenuPwrBuy[1+lenMnu];
	char MenuPrepaid[1+lenMnu];
	char MenuPurchase[1+lenMnu];
	char MenuAktifKartu[1+lenMnu];
	char MenuVoid[1+lenMnu];
	byte EMMiniATM;
	char MenuMiniATM[1+lenMnu];
	char MenuTunai[1+lenMnu];
	char MenuIdleTransfer[1+lenMnu];
	int idxBin;
	char BinName[1+lenName];
	byte Bin[lenBinPrefix + 1];
	char hexBin[2+1];
	TenorData mnuTenor[] = {
		{"", mnuTenor03, '?'},
		{"", mnuTenor06, '?'},
		{"", mnuTenor12, '?'},
		{"", mnuTenor18, '?'},
		{"", mnuTenor24, '?'},
		{"", mnuTenor36, '?'}   
	};

	EMOffline = glTlmSysParam.Con.appEMOffline;
	EMPreAuth = glTlmSysParam.Con.appEMPreAuth;
	EMSale = glTlmSysParam.Con.appEMSale;
	EMPwrBuy = glTlmSysParam.Con.appEMCrdVrfy;
	EMPrepaid = glTlmSysParam.Con.appEMBalance;
	EMMiniATM = glTlmSysParam.Con.appEMCash;

	MAPGET(mnuOffline, MenuOffline, lblKO);
	MAPGET(mnuPreaut, MenuPreAuth, lblKO);
	MAPGET(mnuManual, MenuManual, lblKO);
	MAPGET(mnuPowerBuy, MenuPwrBuy, lblKO);
	MAPGET(mnuPrepaid, MenuPrepaid, lblKO);
	MAPGET(mnuPurchase, MenuPurchase, lblKO);
	MAPGET(mnuKartuAktifasi, MenuAktifKartu, lblKO);
	MAPGET(mnuVoid, MenuVoid,lblKO);
	MAPGET(mnuTarikTunai, 	MenuTunai,	lblKO);
	MAPGET(mnuMiniATM,MenuMiniATM,lblKO);
	MAPGET(mnuTransATMB, MenuIdleTransfer, lblKO);


	//MiniATM
	if(EMMiniATM & 0x01){
	MenuMiniATM[0] = '1';
	MAPPUTSTR(mnuMiniATM, MenuMiniATM, lblKO);
	}
	else {
	MenuMiniATM[0] = '?';
	MAPPUTSTR(mnuMiniATM, MenuMiniATM, lblKO);
	}

	//TARIK TUNAI
	if(EMMiniATM & 0x02){
	MenuTunai[0] = '1';
	MAPPUTSTR(mnuTarikTunai, MenuTunai, lblKO);
	}
	else {
	MenuTunai[0] = '?';
	MAPPUTSTR(mnuTarikTunai, MenuTunai, lblKO);
	}

	//Transfer ATM Bersama
	if(EMMiniATM & 0x80){
	MenuIdleTransfer[0] = '1';
	MAPPUTSTR(mnuTransATMB, MenuIdleTransfer, lblKO);
	}
	else {
	MenuIdleTransfer[0] = '?';
	MAPPUTSTR(mnuTransATMB, MenuIdleTransfer, lblKO);
	}
	
	//handle prepaid feature, start		
	if(EMPrepaid & 0x01) {
		MenuPrepaid[0] = '1';
		MAPPUTSTR(mnuPrepaid, MenuPrepaid, lblKO);
		PublicMenuPrepaid(1);
	}
	else {
		MenuPrepaid[0] = '?';
		MAPPUTSTR(mnuPrepaid, MenuPrepaid, lblKO);
		PublicMenuPrepaid(0);
	}

	if(EMPrepaid & 0x02) {//merchant type
		MenuPurchase[0] = '2';
		MAPPUTSTR(mnuPurchase, MenuPurchase, lblKO);		
		
	}
	else {
		MenuPurchase[0] = '?';
		MAPPUTSTR(mnuPurchase, MenuPurchase, lblKO);	
		
		}
	

	if(EMPrepaid & 0x04) {//public area type	(3)
		MenuAktifKartu[0] = '2';
		MAPPUTSTR(mnuKartuAktifasi, MenuAktifKartu, lblKO);
		}
	else {
		MenuAktifKartu[0] = '?';
		MAPPUTSTR(mnuKartuAktifasi, MenuAktifKartu, lblKO);
		}

	//handle prepaid feature, end

	if(EMOffline & 0x01) {
		MenuOffline[0] = '1';
		MAPPUTSTR(mnuOffline, MenuOffline, lblKO);
	}
	else {
		MenuOffline[0] = '?';
		MAPPUTSTR(mnuOffline, MenuOffline, lblKO);
	}
	
	if(EMPreAuth & 0x01) {
		MenuPreAuth[0] = '1';
		MAPPUTSTR(mnuPreaut, MenuPreAuth, lblKO);
	}
	else {
		MenuPreAuth[0] = '?';
		MAPPUTSTR(mnuPreaut, MenuPreAuth, lblKO);
	}

	if(EMSale & 0x04) {
		MenuManual[0] = '1';
		MAPPUTSTR(mnuManual, MenuManual, lblKO);
	}
	else {
		MenuManual[0] = '?';
		MAPPUTSTR(mnuManual, MenuManual, lblKO);
	}

	if(EMPwrBuy & 0xFF) {
		MenuPwrBuy[0] = '1';
		MAPPUTSTR(mnuPowerBuy, MenuPwrBuy, lblKO);
	}
	else {
		MenuPwrBuy[0] = '?';
		MAPPUTSTR(mnuPowerBuy, MenuPwrBuy, lblKO);
	}	

	if((EMSale & 0x01)||(EMSale & 0x02)||(EMPwrBuy & 0x01)){
		MenuVoid[0]= '1';
		MAPPUTSTR(mnuVoid, MenuVoid, lblKO);		
		}
	else	{
		MenuVoid[0]= '?';
		MAPPUTSTR(mnuVoid, MenuVoid, lblKO);		
		}
	
	for(idxBin = 0; idxBin < 6; idxBin++)	{
		MAPGET(mnuTenor[idxBin].mnu, mnuTenor[idxBin].mnuName, lblKO);
		mnuTenor[idxBin].mnuName[0] = mnuTenor[idxBin].mnuFlag;
	  }
	
	for(idxBin = 0; idxBin < glTlmSysParam.ucBinNum; idxBin++)
	{
		memcpy(BinName, glTlmSysParam.Bin[idxBin].binName, strlen(BinName));
		if(*BinName == 0)
			break;

		memcpy(Bin, glTlmSysParam.Bin[idxBin].binLo, lenBinPrefix+1);
		bin2hex(hexBin, &Bin[1], 2);
		if(memcmp(hexBin, "91", 2) == 0)
			mnuTenor[0].mnuFlag = '2';
		if(memcmp(hexBin, "92", 2) == 0)
			mnuTenor[1].mnuFlag = '2';
		if(memcmp(hexBin, "94", 2) == 0)
			mnuTenor[2].mnuFlag = '2';
		if(memcmp(hexBin, "93", 2) == 0)
			mnuTenor[3].mnuFlag = '2';
		if(memcmp(hexBin, "95", 2) == 0)
			mnuTenor[4].mnuFlag = '2';
		if(memcmp(hexBin, "96", 2) == 0)
			mnuTenor[5].mnuFlag = '2';
	}
	
	for(idxBin = 0; idxBin < 6; idxBin++)	{
		mnuTenor[idxBin].mnuName[0] = mnuTenor[idxBin].mnuFlag;
//		MAPPUTSTR(mnuTenor[idxBin].mnu, mnuTenor[idxBin].mnuName, lblKO);
		mapPut(mnuTenor[idxBin].mnu, mnuTenor[idxBin].mnuName,  sizeof(mnuTenor[idxBin].mnuName));
//		hex_dump_char("mnuPowerBuy tenor", mnuTenor[idxBin].mnuName, sizeof(mnuTenor[idxBin].mnuName));
	}

	
	lblKO:
		;
}
#endif

/** Event received when an application has been selected from the Menu.
 * \header log\\log.h
 * \source log\\hdl.c 
 */
void hdlMainMenu(void)
{
	int ret;

	trcS("hdlMainMenu: Beg\n");	
	MAPPUTWORD(traMnuItm, mnuMainMenu, lblKO);
	hdlSubmenu(mnuMainMenu, mnuEnd);
	trcS("hdlMainMenu: End\n");
lblKO:
	;
}


