#include "global.h"

#define CHK CHECK(ret>=0,lblKO)

/** Get compile date.
 * \return pointer to the date of compilation of the file dft.c
 * 
 * The function allows to identify the compilation date of the application.
 * It is used together with the function getCmpTim() to detect whether the current version of the data base
 * is created by the current or by the previous version of the application.
 * If the current version of the application and the database are not the same
 * a date base rebuilding is ussually required.
 * 
 * This method is used while treating POWER ON event.
 * \sa
 *  - getCmpTim()
 * \remark
 * The format of compile date depends on the compiler.
 * It is not the same for Microsoft Visual and ARM.
 */
const char *getCmpDat(void){ return __DATE__;}
/** Get compile time.
 * \return pointer to the time of compilation of the file dft.c
 * 
 * The function allows to identify the compilation time of the application.
 * It is used together with the function getCmpDat() to detect whether the current version of the data base
 * is created by the current or by the previous version of the application.
 * If the current version of the application and the database are not the same
 * a date base rebuilding is ussually required.
 * 
 * This method is used while treating POWER ON event.
 * \sa
 *  - getCmpDat()
 */
const char *getCmpTim(void){ return __TIME__;}

static int fillDftMnu(void){
    int ret;
    word key;

    trcS("fillDftMnu: Beg\n");
    mapCache(mnuBeg); //enable cache
    for(key= mnuBeg+1; key<mnuEnd; key++){
        ret= mapPut(key,dftMnu[key-mnuBeg-1],0);
        CHK;
    }

    ret= mapSave(mnuBeg); CHECK(ret>=0,lblKO);
    ret= 1; goto lblEnd;
lblKO: trcErr(ret); ret= -1;
lblEnd:
    mapCache(-mnuBeg); //disable cache
    trcFN("fillDftMnu: ret=%d\n",ret);
    return ret;
}

static int fillDftMsg(void){
    int ret;
    word key;

    trcS("fillDftMsg: Beg\n");
    mapCache(msgBeg); //enable cache
    for(key= msgBeg+1; key<msgEnd; key++){
        ret= mapPut(key,dftMsg[key-msgBeg-1],0);
        CHK;
    }

    ret= mapSave(msgBeg); CHECK(ret>=0,lblKO);
    ret= 1; goto lblEnd;
lblKO: trcErr(ret); ret= -1;
lblEnd:
    mapCache(-msgBeg); //disable cache
    trcFN("fillDftMsg: ret=%d\n",ret);
    return ret;
}

static int fillDftMon(void){
    int ret;
    word key;

    trcS("fillDftMon: Beg\n");
    mapCache(monBeg); //enable cache
    for(key= monBeg+1; key<monEnd; key++){
        ret= mapPut(key,dftMon[key-monBeg-1],0);
        CHK;
    }

    ret= mapSave(monBeg); CHECK(ret>=0,lblKO);
    ret= 1; goto lblEnd;
lblKO: trcErr(ret); ret= -1;
lblEnd:
    mapCache(-monBeg); //disable cache
    trcFN("fillDftMon: ret=%d\n",ret);
    return ret;
}

static int fillDftTot(void){
    int ret;
    trcS("fillDftTot: Beg\n");


    ret= 1; CHECK(ret>0,lblKO); goto lblEnd;
lblKO: ret= -1;
lblEnd:
    trcFN("fillDftTot: ret=%d\n",ret);
    return ret;
}

static int fillDftCdb(void){
    int ret;
    word i;
    trcS("fillDftCdb: Beg\n");

    for(i= 0; i<200; i++){
        if(i>=sizeof(dftCdb)/sizeof(tCdbRecord)) break;
        ret= mapMove(cdbBeg,i); CHK;
        ret= mapPutStr((word)(cdbBankKode),dftCdb[i]._cdbBankKode); CHK;
        ret= mapPutStr((word)(cdbBankName),dftCdb[i]._cdbBankName); CHK;
    }

    ret= 1; CHECK(ret>0,lblKO); goto lblEnd;
lblKO: ret= -1;
lblEnd:
    trcFN("fillDftCdb: ret=%d\n",ret);
    return ret;
}

static int fillDftBin(void){
    int ret;
    word i;
    trcS("fillDftBin: Beg\n");

    for(i= 0; i<dimBin; i++){
        if(i>=sizeof(dftBin)/sizeof(tBinRecord)) break;
        ret= mapMove(binBeg,i); CHK;
        ret= mapPut((word)(binID),&dftBin[i]._binID,sizeof(byte)); CHK;
        ret= mapPutStr((word)(binTotAppDbAmt),dftBin[i]._binTotAppDbAmt); CHK;
        ret= mapPut((word)(binTotAppDbCnt),&dftBin[i]._binTotAppDbCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(binTotAppCrAmt),dftBin[i]._binTotAppCrAmt); CHK;
        ret= mapPut((word)(binTotAppCrCnt),&dftBin[i]._binTotAppCrCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(binTotAppTpAmt),dftBin[i]._binTotAppTpAmt); CHK;
        ret= mapPut((word)(binTotAppTpCnt),&dftBin[i]._binTotAppTpCnt,sizeof(word)); CHK;
    }

    ret= 1; CHECK(ret>0,lblKO); goto lblEnd;
lblKO: ret= -1;
lblEnd:
    trcFN("fillDftBin: ret=%d\n",ret);
    return ret;
}

static int fillDftAcq(void){
    int ret;
    word i;
    trcS("fillDftAcq: Beg\n");

    for(i= 0; i<dimAcq; i++){
        if(i>=sizeof(dftAcq)/sizeof(tAcqRecord)) break;
        ret= mapMove(acqBeg,i); CHK;
        ret= mapPut((word)(acqID),&dftAcq[i]._acqID,sizeof(byte)); CHK;
        ret= mapPutStr((word)(acqName),dftAcq[i]._acqName); CHK;
        ret= mapPutStr((word)(acqCurBat),dftAcq[i]._acqCurBat); CHK;
        ret= mapPutStr((word)(logAcqCurBat),dftAcq[i]._logAcqCurBat); CHK;
        ret= mapPutStr((word)(acqTotAppDbAmt),dftAcq[i]._acqTotAppDbAmt); CHK;
        ret= mapPut((word)(acqTotAppDbCnt),&dftAcq[i]._acqTotAppDbCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppCrAmt),dftAcq[i]._acqTotAppCrAmt); CHK;
        ret= mapPut((word)(acqTotAppCrCnt),&dftAcq[i]._acqTotAppCrCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppTpAmt),dftAcq[i]._acqTotAppTpAmt); CHK;
        ret= mapPut((word)(acqTotAppTpCnt),&dftAcq[i]._acqTotAppTpCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppSaleAmt),dftAcq[i]._acqTotAppSaleAmt); CHK;
        ret= mapPut((word)(acqTotAppSaleCnt),&dftAcq[i]._acqTotAppSaleCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppRefundAmt),dftAcq[i]._acqTotAppRefundAmt); CHK;
        ret= mapPut((word)(acqTotAppRefundCnt),&dftAcq[i]._acqTotAppRefundCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppOfflineAmt),dftAcq[i]._acqTotAppOfflineAmt); CHK;
        ret= mapPut((word)(acqTotAppOfflineCnt),&dftAcq[i]._acqTotAppOfflineCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppManualAmt),dftAcq[i]._acqTotAppManualAmt); CHK;
        ret= mapPut((word)(acqTotAppManualCnt),&dftAcq[i]._acqTotAppManualCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppVoidAmt),dftAcq[i]._acqTotAppVoidAmt); CHK;
        ret= mapPut((word)(acqTotAppVoidCnt),&dftAcq[i]._acqTotAppVoidCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppPrepaidAmt),dftAcq[i]._acqTotAppPrepaidAmt); CHK;
        ret= mapPut((word)(acqTotAppPrepaidCnt),&dftAcq[i]._acqTotAppPrepaidCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppPrepaidAmtUpdate),dftAcq[i]._acqTotAppPrepaidAmtUpdate); CHK;
        ret= mapPut((word)(acqTotAppPrepaidCntUpdate),&dftAcq[i]._acqTotAppPrepaidCntUpdate,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtPrepaid),dftAcq[i]._logAcqTotAppAmtPrepaid); CHK;
        ret= mapPut((word)(logAcqTotAppCntPrepaid),&dftAcq[i]._logAcqTotAppCntPrepaid,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtPrepaidUpdate),dftAcq[i]._logAcqTotAppAmtPrepaidUpdate); CHK;
        ret= mapPut((word)(logAcqTotAppCntPrepaidUpdate),&dftAcq[i]._logAcqTotAppCntPrepaidUpdate,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppTelkomAmt),dftAcq[i]._acqTotAppTelkomAmt); CHK;
        ret= mapPut((word)(acqTotAppTelkomCnt),&dftAcq[i]._acqTotAppTelkomCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppSatelindoAmt),dftAcq[i]._acqTotAppSatelindoAmt); CHK;
        ret= mapPut((word)(acqTotAppSatelindoCnt),&dftAcq[i]._acqTotAppSatelindoCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppTelkomselAmt),dftAcq[i]._acqTotAppTelkomselAmt); CHK;
        ret= mapPut((word)(acqTotAppTelkomselCnt),&dftAcq[i]._acqTotAppTelkomselCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppExcelcomAmt),dftAcq[i]._acqTotAppExcelcomAmt); CHK;
        ret= mapPut((word)(acqTotAppExcelcomCnt),&dftAcq[i]._acqTotAppExcelcomCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppEsiaAmt),dftAcq[i]._acqTotAppEsiaAmt); CHK;
        ret= mapPut((word)(acqTotAppEsiaCnt),&dftAcq[i]._acqTotAppEsiaCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppPLNAmt),dftAcq[i]._acqTotAppPLNAmt); CHK;
        ret= mapPut((word)(acqTotAppPLNCnt),&dftAcq[i]._acqTotAppPLNCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppFirstMediaAmt),dftAcq[i]._acqTotAppFirstMediaAmt); CHK;
        ret= mapPut((word)(acqTotAppFirstMediaCnt),&dftAcq[i]._acqTotAppFirstMediaCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppIndovisionAmt),dftAcq[i]._acqTotAppIndovisionAmt); CHK;
        ret= mapPut((word)(acqTotAppIndovisionCnt),&dftAcq[i]._acqTotAppIndovisionCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppOtoMultiAmt),dftAcq[i]._acqTotAppOtoMultiAmt); CHK;
        ret= mapPut((word)(acqTotAppOtoMultiCnt),&dftAcq[i]._acqTotAppOtoMultiCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppMAtmAmt),dftAcq[i]._acqTotAppMAtmAmt); CHK;
        ret= mapPut((word)(acqTotAppMAtmCnt),&dftAcq[i]._acqTotAppMAtmCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppMINIATMAmt),dftAcq[i]._acqTotAppMINIATMAmt); CHK;
        ret= mapPutStr((word)(acqTotAppPrepaidAmtGP),dftAcq[i]._acqTotAppPrepaidAmtGP); CHK;
        ret= mapPut((word)(acqTotAppPrepaidCntGP),&dftAcq[i]._acqTotAppPrepaidCntGP,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtPrepaidGP),dftAcq[i]._logAcqTotAppAmtPrepaidGP); CHK;
        ret= mapPut((word)(logAcqTotAppCntPrepaidGP),&dftAcq[i]._logAcqTotAppCntPrepaidGP,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppAmtTunai),dftAcq[i]._acqTotAppAmtTunai); CHK;
        ret= mapPut((word)(acqTotAppCntTunai),&dftAcq[i]._acqTotAppCntTunai,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppAmtTransfer),dftAcq[i]._acqTotAppAmtTransfer); CHK;
        ret= mapPut((word)(acqTotAppCntTransfer),&dftAcq[i]._acqTotAppCntTransfer,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppAmtCC),dftAcq[i]._acqTotAppAmtCC); CHK;
        ret= mapPut((word)(acqTotAppCntCC),&dftAcq[i]._acqTotAppCntCC,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppAmtTransferUSD),dftAcq[i]._acqTotAppAmtTransferUSD); CHK;
        ret= mapPut((word)(acqTotAppCntTransferUSD),&dftAcq[i]._acqTotAppCntTransferUSD,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppAmtTransferSGD),dftAcq[i]._acqTotAppAmtTransferSGD); CHK;
        ret= mapPut((word)(acqTotAppCntTransferSGD),&dftAcq[i]._acqTotAppCntTransferSGD,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqTotAppAmtTransferJPN),dftAcq[i]._acqTotAppAmtTransferJPN); CHK;
        ret= mapPut((word)(acqTotAppCntTransferJPN),&dftAcq[i]._acqTotAppCntTransferJPN,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtTunai),dftAcq[i]._logAcqTotAppAmtTunai); CHK;
        ret= mapPut((word)(logAcqTotAppCntTunai),&dftAcq[i]._logAcqTotAppCntTunai,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtTransfer),dftAcq[i]._logAcqTotAppAmtTransfer); CHK;
        ret= mapPut((word)(logAcqTotAppCntTransfer),&dftAcq[i]._logAcqTotAppCntTransfer,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtCC),dftAcq[i]._logAcqTotAppAmtCC); CHK;
        ret= mapPut((word)(logAcqTotAppCntCC),&dftAcq[i]._logAcqTotAppCntCC,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtTransferUSD),dftAcq[i]._logAcqTotAppAmtTransferUSD); CHK;
        ret= mapPut((word)(logAcqTotAppCntTransferUSD),&dftAcq[i]._logAcqTotAppCntTransferUSD,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtTransferSGD),dftAcq[i]._logAcqTotAppAmtTransferSGD); CHK;
        ret= mapPut((word)(logAcqTotAppCntTransferSGD),&dftAcq[i]._logAcqTotAppCntTransferSGD,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppAmtTransferJPN),dftAcq[i]._logAcqTotAppAmtTransferJPN); CHK;
        ret= mapPut((word)(logAcqTotAppCntTransferJPN),&dftAcq[i]._logAcqTotAppCntTransferJPN,sizeof(word)); CHK;
        ret= mapPut((word)(acqTotAppCommCnt),&dftAcq[i]._acqTotAppCommCnt,sizeof(word)); CHK;
        ret= mapPut((word)(logAcqTotAppCntComm),&dftAcq[i]._logAcqTotAppCntComm,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppTelkomAmt),dftAcq[i]._logacqTotAppTelkomAmt); CHK;
        ret= mapPut((word)(logacqTotAppTelkomCnt),&dftAcq[i]._logacqTotAppTelkomCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppSatelindoAmt),dftAcq[i]._logacqTotAppSatelindoAmt); CHK;
        ret= mapPut((word)(logacqTotAppSatelindoCnt),&dftAcq[i]._logacqTotAppSatelindoCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppTelkomselAmt),dftAcq[i]._logacqTotAppTelkomselAmt); CHK;
        ret= mapPut((word)(logacqTotAppTelkomselCnt),&dftAcq[i]._logacqTotAppTelkomselCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppExcelcomAmt),dftAcq[i]._logacqTotAppExcelcomAmt); CHK;
        ret= mapPut((word)(logacqTotAppExcelcomCnt),&dftAcq[i]._logacqTotAppExcelcomCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppEsiaAmt),dftAcq[i]._logacqTotAppEsiaAmt); CHK;
        ret= mapPut((word)(logacqTotAppEsiaCnt),&dftAcq[i]._logacqTotAppEsiaCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppPLNAmt),dftAcq[i]._logacqTotAppPLNAmt); CHK;
        ret= mapPut((word)(logacqTotAppPLNCnt),&dftAcq[i]._logacqTotAppPLNCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppFirstMediaAmt),dftAcq[i]._logacqTotAppFirstMediaAmt); CHK;
        ret= mapPut((word)(logacqTotAppFirstMediaCnt),&dftAcq[i]._logacqTotAppFirstMediaCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppIndovisionAmt),dftAcq[i]._logacqTotAppIndovisionAmt); CHK;
        ret= mapPut((word)(logacqTotAppIndovisionCnt),&dftAcq[i]._logacqTotAppIndovisionCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppOtoMultiAmt),dftAcq[i]._logacqTotAppOtoMultiAmt); CHK;
        ret= mapPut((word)(logacqTotAppOtoMultiCnt),&dftAcq[i]._logacqTotAppOtoMultiCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppMAtmAmt),dftAcq[i]._logacqTotAppMAtmAmt); CHK;
        ret= mapPut((word)(logacqTotAppMAtmCnt),&dftAcq[i]._logacqTotAppMAtmCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logacqTotAppMINIATMAmt),dftAcq[i]._logacqTotAppMINIATMAmt); CHK;
        ret= mapPutStr((word)(logAcqTotAppDbAmt),dftAcq[i]._logAcqTotAppDbAmt); CHK;
        ret= mapPut((word)(logAcqTotAppDbCnt),&dftAcq[i]._logAcqTotAppDbCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppVoidAmt),dftAcq[i]._logAcqTotAppVoidAmt); CHK;
        ret= mapPut((word)(logAcqTotAppVoidCnt),&dftAcq[i]._logAcqTotAppVoidCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(logAcqTotAppTipAmt),dftAcq[i]._logAcqTotAppTipAmt); CHK;
        ret= mapPut((word)(logAcqTotAppTipCnt),&dftAcq[i]._logAcqTotAppTipCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(acqPriTxnPhnSrv),dftAcq[i]._acqPriTxnPhnSrv); CHK;
        ret= mapPutStr((word)(acqSecTxnPhnSrv),dftAcq[i]._acqSecTxnPhnSrv); CHK;
        ret= mapPutStr((word)(acqPriStlPhnSrv),dftAcq[i]._acqPriStlPhnSrv); CHK;
        ret= mapPutStr((word)(acqSecStlPhnSrv),dftAcq[i]._acqSecStlPhnSrv); CHK;
        ret= mapPutStr((word)(acqPriTxnIPSrv),dftAcq[i]._acqPriTxnIPSrv); CHK;
        ret= mapPutStr((word)(acqSecTxnIPSrv),dftAcq[i]._acqSecTxnIPSrv); CHK;
        ret= mapPutStr((word)(acqPriStlIPSrv),dftAcq[i]._acqPriStlIPSrv); CHK;
        ret= mapPutStr((word)(acqSecStlIPSrv),dftAcq[i]._acqSecStlIPSrv); CHK;
        ret= mapPutStr((word)(acqMasterKey1),dftAcq[i]._acqMasterKey1); CHK;
        ret= mapPutStr((word)(acqMasterKey2),dftAcq[i]._acqMasterKey2); CHK;
        ret= mapPutStr((word)(acqWorkingKey),dftAcq[i]._acqWorkingKey); CHK;
        ret= mapPutStr((word)(acqPinKey),dftAcq[i]._acqPinKey); CHK;
        ret= mapPut((word)(acqPendingSettle),&dftAcq[i]._acqPendingSettle,sizeof(byte)); CHK;
        ret= mapPut((word)(acqLogonFlag),&dftAcq[i]._acqLogonFlag,sizeof(byte)); CHK;
        ret= mapPut((word)(acqFlagCountTrx),&dftAcq[i]._acqFlagCountTrx,sizeof(byte)); CHK;
    }

    ret= 1; CHECK(ret>0,lblKO); goto lblEnd;
lblKO: ret= -1;
lblEnd:
    trcFN("fillDftAcq: ret=%d\n",ret);
    return ret;
}

static int fillDftIss(void){
    int ret;
    word i;
    trcS("fillDftIss: Beg\n");

    for(i= 0; i<dimIss; i++){
        if(i>=sizeof(dftIss)/sizeof(tIssRecord)) break;
        ret= mapMove(issBeg,i); CHK;
        ret= mapPut((word)(issID),&dftIss[i]._issID,sizeof(byte)); CHK;
        ret= mapPutStr((word)(issRef),dftIss[i]._issRef); CHK;
        ret= mapPut((word)(issOpt1),&dftIss[i]._issOpt1,sizeof(byte)); CHK;
        ret= mapPut((word)(issOpt2),&dftIss[i]._issOpt2,sizeof(byte)); CHK;
        ret= mapPut((word)(issOpt3),&dftIss[i]._issOpt3,sizeof(byte)); CHK;
        ret= mapPut((word)(issOpt4),&dftIss[i]._issOpt4,sizeof(byte)); CHK;
        ret= mapPut((word)(issDefAcct),&dftIss[i]._issDefAcct,sizeof(byte)); CHK;
        ret= mapPut((word)(issFloorLimit),&dftIss[i]._issFloorLimit,sizeof(word)); CHK;
        ret= mapPut((word)(issOpt5),&dftIss[i]._issOpt5,sizeof(byte)); CHK;
        ret= mapPut((word)(issPinOpt),&dftIss[i]._issPinOpt,sizeof(byte)); CHK;
        ret= mapPutStr((word)(issTotAppDbAmt),dftIss[i]._issTotAppDbAmt); CHK;
        ret= mapPut((word)(issTotAppDbCnt),&dftIss[i]._issTotAppDbCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppCrAmt),dftIss[i]._issTotAppCrAmt); CHK;
        ret= mapPut((word)(issTotAppCrCnt),&dftIss[i]._issTotAppCrCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppTpAmt),dftIss[i]._issTotAppTpAmt); CHK;
        ret= mapPut((word)(issTotAppTpCnt),&dftIss[i]._issTotAppTpCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppSaleAmt),dftIss[i]._issTotAppSaleAmt); CHK;
        ret= mapPut((word)(issTotAppSaleCnt),&dftIss[i]._issTotAppSaleCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppRefundAmt),dftIss[i]._issTotAppRefundAmt); CHK;
        ret= mapPut((word)(issTotAppRefundCnt),&dftIss[i]._issTotAppRefundCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppOfflineAmt),dftIss[i]._issTotAppOfflineAmt); CHK;
        ret= mapPut((word)(issTotAppOfflineCnt),&dftIss[i]._issTotAppOfflineCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppManualAmt),dftIss[i]._issTotAppManualAmt); CHK;
        ret= mapPut((word)(issTotAppManualCnt),&dftIss[i]._issTotAppManualCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppVoidAmt),dftIss[i]._issTotAppVoidAmt); CHK;
        ret= mapPut((word)(issTotAppVoidCnt),&dftIss[i]._issTotAppVoidCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtPrepaid),dftIss[i]._issTotAppAmtPrepaid); CHK;
        ret= mapPut((word)(issTotAppPrepaidCnt),&dftIss[i]._issTotAppPrepaidCnt,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtPrepaidUpdate),dftIss[i]._issTotAppAmtPrepaidUpdate); CHK;
        ret= mapPut((word)(issTotAppPrepaidCntUpdate),&dftIss[i]._issTotAppPrepaidCntUpdate,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtTelkom),dftIss[i]._issTotAppAmtTelkom); CHK;
        ret= mapPut((word)(issTotAppCntTelkom),&dftIss[i]._issTotAppCntTelkom,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtSatelindo),dftIss[i]._issTotAppAmtSatelindo); CHK;
        ret= mapPut((word)(issTotAppCntSatelindo),&dftIss[i]._issTotAppCntSatelindo,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtTelkomsel),dftIss[i]._issTotAppAmtTelkomsel); CHK;
        ret= mapPut((word)(issTotAppCntTelkomsel),&dftIss[i]._issTotAppCntTelkomsel,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtExcelcom),dftIss[i]._issTotAppAmtExcelcom); CHK;
        ret= mapPut((word)(issTotAppCntExcelcom),&dftIss[i]._issTotAppCntExcelcom,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtEsia),dftIss[i]._issTotAppAmtEsia); CHK;
        ret= mapPut((word)(issTotAppCntEsia),&dftIss[i]._issTotAppCntEsia,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtPLN),dftIss[i]._issTotAppAmtPLN); CHK;
        ret= mapPut((word)(issTotAppCntPLN),&dftIss[i]._issTotAppCntPLN,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtFirstMedia),dftIss[i]._issTotAppAmtFirstMedia); CHK;
        ret= mapPut((word)(issTotAppCntFirstMedia),&dftIss[i]._issTotAppCntFirstMedia,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtIndovision),dftIss[i]._issTotAppAmtIndovision); CHK;
        ret= mapPut((word)(issTotAppCntIndovision),&dftIss[i]._issTotAppCntIndovision,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtOtoMulti),dftIss[i]._issTotAppAmtOtoMulti); CHK;
        ret= mapPut((word)(issTotAppCntOtoMulti),&dftIss[i]._issTotAppCntOtoMulti,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtMAtm),dftIss[i]._issTotAppAmtMAtm); CHK;
        ret= mapPut((word)(issTotAppCntMAtm),&dftIss[i]._issTotAppCntMAtm,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppPrepaidAmtGP),dftIss[i]._issTotAppPrepaidAmtGP); CHK;
        ret= mapPut((word)(issTotAppPrepaidCntGP),&dftIss[i]._issTotAppPrepaidCntGP,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtMiniAtm),dftIss[i]._issTotAppAmtMiniAtm); CHK;
        ret= mapPutStr((word)(issTotAppAmtTunai),dftIss[i]._issTotAppAmtTunai); CHK;
        ret= mapPut((word)(issTotAppCntTunai),&dftIss[i]._issTotAppCntTunai,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtTransfer),dftIss[i]._issTotAppAmtTransfer); CHK;
        ret= mapPut((word)(issTotAppCntTransfer),&dftIss[i]._issTotAppCntTransfer,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtCC),dftIss[i]._issTotAppAmtCC); CHK;
        ret= mapPut((word)(issTotAppCntCC),&dftIss[i]._issTotAppCntCC,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtTransferUSD),dftIss[i]._issTotAppAmtTransferUSD); CHK;
        ret= mapPut((word)(issTotAppCntTransferUSD),&dftIss[i]._issTotAppCntTransferUSD,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtTransferSGD),dftIss[i]._issTotAppAmtTransferSGD); CHK;
        ret= mapPut((word)(issTotAppCntTransferSGD),&dftIss[i]._issTotAppCntTransferSGD,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtTransferJPN),dftIss[i]._issTotAppAmtTransferJPN); CHK;
        ret= mapPut((word)(issTotAppCntTransferJPN),&dftIss[i]._issTotAppCntTransferJPN,sizeof(word)); CHK;
        ret= mapPut((word)(issTotAppCntComm),&dftIss[i]._issTotAppCntComm,sizeof(word)); CHK;
        ret= mapPut((word)(issAccSelect),&dftIss[i]._issAccSelect,sizeof(byte)); CHK;
        ret= mapPut((word)(issEMVFallback),&dftIss[i]._issEMVFallback,sizeof(byte)); CHK;
        ret= mapPut((word)(issManual),&dftIss[i]._issManual,sizeof(byte)); CHK;
        ret= mapPut((word)(issExpiryDatReq),&dftIss[i]._issExpiryDatReq,sizeof(byte)); CHK;
        ret= mapPut((word)(issOffline),&dftIss[i]._issOffline,sizeof(byte)); CHK;
        ret= mapPut((word)(issRefferalDial),&dftIss[i]._issRefferalDial,sizeof(byte)); CHK;
        ret= mapPut((word)(issProductCod),&dftIss[i]._issProductCod,sizeof(byte)); CHK;
        ret= mapPut((word)(issAdjust),&dftIss[i]._issAdjust,sizeof(byte)); CHK;
        ret= mapPut((word)(issDoModPAN),&dftIss[i]._issDoModPAN,sizeof(byte)); CHK;
        ret= mapPut((word)(issROCNumReq),&dftIss[i]._issROCNumReq,sizeof(byte)); CHK;
        ret= mapPut((word)(issPrintReceipt),&dftIss[i]._issPrintReceipt,sizeof(byte)); CHK;
        ret= mapPut((word)(issCapTxns),&dftIss[i]._issCapTxns,sizeof(byte)); CHK;
        ret= mapPut((word)(issCheckExpOff),&dftIss[i]._issCheckExpOff,sizeof(byte)); CHK;
        ret= mapPut((word)(issRefund),&dftIss[i]._issRefund,sizeof(byte)); CHK;
        ret= mapPut((word)(issPreauth),&dftIss[i]._issPreauth,sizeof(byte)); CHK;
        ret= mapPut((word)(issVoid),&dftIss[i]._issVoid,sizeof(byte)); CHK;
        ret= mapPut((word)(issAdditionPrompts),&dftIss[i]._issAdditionPrompts,sizeof(byte)); CHK;
        ret= mapPut((word)(issCheckExpAll),&dftIss[i]._issCheckExpAll,sizeof(byte)); CHK;
        ret= mapPut((word)(issCashOut),&dftIss[i]._issCashOut,sizeof(byte)); CHK;
        ret= mapPut((word)(issTrack1),&dftIss[i]._issTrack1,sizeof(byte)); CHK;
        ret= mapPut((word)(issBalance),&dftIss[i]._issBalance,sizeof(byte)); CHK;
        ret= mapPut((word)(issErrorReceipt),&dftIss[i]._issErrorReceipt,sizeof(byte)); CHK;
        ret= mapPut((word)(issOfflineROC),&dftIss[i]._issOfflineROC,sizeof(byte)); CHK;
        ret= mapPut((word)(issExtServiceCod),&dftIss[i]._issExtServiceCod,sizeof(byte)); CHK;
        ret= mapPut((word)(issFullPAN),&dftIss[i]._issFullPAN,sizeof(byte)); CHK;
        ret= mapPut((word)(issExpiryDate),&dftIss[i]._issExpiryDate,sizeof(byte)); CHK;
        ret= mapPut((word)(issManEntryCVV),&dftIss[i]._issManEntryCVV,sizeof(byte)); CHK;
        ret= mapPutStr((word)(issTotAppAmtMOnUs),dftIss[i]._issTotAppAmtMOnUs); CHK;
        ret= mapPut((word)(issTotAppCntMOnUs),&dftIss[i]._issTotAppCntMOnUs,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtVOnUs),dftIss[i]._issTotAppAmtVOnUs); CHK;
        ret= mapPut((word)(issTotAppCntVOnUs),&dftIss[i]._issTotAppCntVOnUs,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtMOffUs),dftIss[i]._issTotAppAmtMOffUs); CHK;
        ret= mapPut((word)(issTotAppCntMOffUs),&dftIss[i]._issTotAppCntMOffUs,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppAmtVOffUs),dftIss[i]._issTotAppAmtVOffUs); CHK;
        ret= mapPut((word)(issTotAppCntVOffUs),&dftIss[i]._issTotAppCntVOffUs,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppVoidAmtMOnUs),dftIss[i]._issTotAppVoidAmtMOnUs); CHK;
        ret= mapPut((word)(issTotAppVoidCntMOnUs),&dftIss[i]._issTotAppVoidCntMOnUs,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppVoidAmtVOnUs),dftIss[i]._issTotAppVoidAmtVOnUs); CHK;
        ret= mapPut((word)(issTotAppVoidCntVOnUs),&dftIss[i]._issTotAppVoidCntVOnUs,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppVoidAmtMOffUs),dftIss[i]._issTotAppVoidAmtMOffUs); CHK;
        ret= mapPut((word)(issTotAppVoidCntMOffUs),&dftIss[i]._issTotAppVoidCntMOffUs,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppVoidAmtVOffUs),dftIss[i]._issTotAppVoidAmtVOffUs); CHK;
        ret= mapPut((word)(issTotAppVoidCntVOffUs),&dftIss[i]._issTotAppVoidCntVOffUs,sizeof(word)); CHK;
        ret= mapPutStr((word)(issTotAppTipAmtMOnUs),dftIss[i]._issTotAppTipAmtMOnUs); CHK;
        ret= mapPutStr((word)(issTotAppTipAmtVOnUs),dftIss[i]._issTotAppTipAmtVOnUs); CHK;
        ret= mapPutStr((word)(issTotAppTipAmtMOffUs),dftIss[i]._issTotAppTipAmtMOffUs); CHK;
        ret= mapPutStr((word)(issTotAppTipAmtVOffUs),dftIss[i]._issTotAppTipAmtVOffUs); CHK;
    }

    ret= 1; CHECK(ret>0,lblKO); goto lblEnd;
lblKO: ret= -1;
lblEnd:
    trcFN("fillDftIss: ret=%d\n",ret);
    return ret;
}

static int fillDftApp(void){
    int ret;
    trcS("fillDftApp: Beg\n");

    ret= mapPutStr(appEndTime,dftAppEndTime); CHK;
    ret= mapPutStr(appGprsApn,dftAppGprsApn); CHK;
    ret= mapPutStr(appChn,dftAppChn); CHK;
    ret= mapPutStr(appSrv,dftAppSrv); CHK;
    ret= mapPutByte(appSrvAtt,dftAppSrvAtt); CHK;
    ret= mapPutStr(appPabx,dftAppPabx); CHK;
    ret= mapPutByte(appTonePulse,dftAppTonePulse); CHK;
    ret= mapPutByte(appToneDetect,dftAppToneDetect); CHK;
    ret= mapPutStr(appPrefix,dftAppPrefix); CHK;
    ret= mapPutStr(appMrcPwd,dftAppMrcPwd); CHK;
    ret= mapPutStr(appAdmPwd,dftAppAdmPwd); CHK;
    ret= mapPutStr(appAccCode,dftAppAccCode); CHK;
    ret= mapPutByte(appExp,dftAppExp); CHK;
    ret= mapPutStr(appTid,dftAppTid); CHK;
    ret= mapPutStr(appMid,dftAppMid); CHK;
    ret= mapPutByte(appAmtFmt,dftAppAmtFmt); CHK;
    ret= mapPutByte(appDefTra,dftAppDefTra); CHK;
    ret= mapPutByte(appTermOpt1,dftAppTermOpt1); CHK;
    ret= mapPutByte(appTermOpt2,dftAppTermOpt2); CHK;
    ret= mapPutByte(appTermOpt5,dftAppTermOpt5); CHK;
    ret= mapPutByte(appLocOpt,dftAppLocOpt); CHK;
    ret= mapPutStr(appCurrSign,dftAppCurrSign); CHK;
    ret= mapPutByte(appConnType,dftAppConnType); CHK;
    ret= mapPutByte(appComType,dftAppComType); CHK;
    ret= mapPutStr(appVoidPwd,dftAppVoidPwd); CHK;
    ret= mapPutStr(appRefPwd,dftAppRefPwd); CHK;
    ret= mapPutStr(appSetlPwd,dftAppSetlPwd); CHK;
    ret= mapPutByte(appEMSale,dftAppEMSale); CHK;
    ret= mapPutByte(appEMRefund,dftAppEMRefund); CHK;
    ret= mapPutByte(appEMOffline,dftAppEMOffline); CHK;
    ret= mapPutByte(appEMCash,dftAppEMCash); CHK;
    ret= mapPutByte(appEMPreAuth,dftAppEMPreAuth); CHK;
    ret= mapPutByte(appEMCrdVrfy,dftAppEMCrdVrfy); CHK;
    ret= mapPutByte(appEMBalance,dftAppEMBalance); CHK;
    ret= mapPutByte(appGPRSFlag,dftAppGPRSFlag); CHK;
    ret= mapPutStr(appNII,dftAppNII); CHK;
    ret= mapPutStr(appPriSrv,dftAppPriSrv); CHK;
    ret= mapPutByte(appPriAtt,dftAppPriAtt); CHK;
    ret= mapPutStr(appSecSrv,dftAppSecSrv); CHK;
    ret= mapPutByte(appSecAtt,dftAppSecAtt); CHK;
    ret= mapPutByte(appDialOpt,dftAppDialOpt); CHK;
    ret= mapPutByte(appBaudRate,dftAppBaudRate); CHK;
    ret= mapPutByte(appTmsBufSize,dftAppTmsBufSize); CHK;
    ret= mapPutStr(appIpAdd,dftAppIpAdd); CHK;
    ret= mapPutStr(appSecIpAdd,dftAppSecIpAdd); CHK;
    ret= mapPutStr(appCrdInpCtl,dftAppCrdInpCtl); CHK;
    ret= mapPutByte(appPinKeyLoc,dftAppPinKeyLoc); CHK;
    ret= mapPutByte(appMKeyLoc,dftAppMKeyLoc); CHK;
    ret= mapPutWord(appDefRxTO,dftAppDefRxTO); CHK;
    ret= mapPutWord(appDefICTO,dftAppDefICTO); CHK;
    ret= mapPutWord(appDefConTO,dftAppDefConTO); CHK;
    ret= mapPutStr(apptraSale,dftApptraSale); CHK;
    ret= mapPutStr(apptraOSale,dftApptraOSale); CHK;
    ret= mapPutStr(apptraOffline,dftApptraOffline); CHK;
    ret= mapPutStr(apptraPreAuth,dftApptraPreAuth); CHK;
    ret= mapPutStr(apptraVoid,dftApptraVoid); CHK;
    ret= mapPutStr(apptraRefund,dftApptraRefund); CHK;
    ret= mapPutStr(apptraAdjust,dftApptraAdjust); CHK;
    ret= mapPutStr(apptraManual,dftApptraManual); CHK;
    ret= mapPutStr(appTenor1,dftAppTenor1); CHK;
    ret= mapPutStr(appTenor3,dftAppTenor3); CHK;
    ret= mapPutStr(appTenor6,dftAppTenor6); CHK;
    ret= mapPutStr(appTenor9,dftAppTenor9); CHK;
    ret= mapPutStr(appTenor12,dftAppTenor12); CHK;
    ret= mapPutStr(appTenor18,dftAppTenor18); CHK;
    ret= mapPutStr(appTenor24,dftAppTenor24); CHK;
    ret= mapPutStr(apptpduHead,dftApptpduHead); CHK;
    ret= mapPutStr(apptpduModem,dftApptpduModem); CHK;
    ret= mapPutStr(apptpduTCPIP,dftApptpduTCPIP); CHK;
    ret= mapPutStr(apptpduRS232,dftApptpduRS232); CHK;
    ret= mapPut(emvTrnCurCod,dftEmvTrnCurCod,(byte)(1+dftEmvTrnCurCod[0])); CHK;
    ret= mapPut(emvTrnCurExp,dftEmvTrnCurExp,(byte)(1+dftEmvTrnCurExp[0])); CHK;
    ret= mapPut(emvMrcCatCod,dftEmvMrcCatCod,(byte)(1+dftEmvMrcCatCod[0])); CHK;
    ret= mapPut(emvTrmCntCod,dftEmvTrmCntCod,(byte)(1+dftEmvTrmCntCod[0])); CHK;
    ret= mapPut(emvTrmId,dftEmvTrmId,(byte)(1+dftEmvTrmId[0])); CHK;
    ret= mapPut(emvTrmCap,dftEmvTrmCap,(byte)(1+dftEmvTrmCap[0])); CHK;
    ret= mapPut(emvTrmTyp,dftEmvTrmTyp,(byte)(1+dftEmvTrmTyp[0])); CHK;
    ret= mapPut(emvAddTrmCap,dftEmvAddTrmCap,(byte)(1+dftEmvAddTrmCap[0])); CHK;
    ret= mapPut(emvVlpSupInd,dftEmvVlpSupInd,(byte)(1+dftEmvVlpSupInd[0])); CHK;
    ret= mapPut(emvVlpTrmLim,dftEmvVlpTrmLim,(byte)(1+dftEmvVlpTrmLim[0])); CHK;
    ret= mapPut(emvLstRspCod,dftEmvLstRspCod,(byte)(1+dftEmvLstRspCod[0])); CHK;
    ret= mapPut(emvIFDSerNum,dftEmvIFDSerNum,(byte)(1+dftEmvIFDSerNum[0])); CHK;
    ret= mapPut(emvTRMOverpassAIP,dftEmvTRMOverpassAIP,(byte)(1+dftEmvTRMOverpassAIP[0])); CHK;
    ret= mapPutByte(appServiceCode,dftAppServiceCode); CHK;
    ret= mapPutByte(appFallBackMod,dftAppFallBackMod); CHK;
    ret= mapPutStr(appPPPNbr,dftAppPPPNbr); CHK;
    ret= mapPutStr(appBfKey,dftAppBfKey); CHK;
    ret= mapPutByte(appBlowFish,dftAppBlowFish); CHK;
    ret= mapPutStr(appAftTMSKey,dftAppAftTMSKey); CHK;
    ret= mapPutByte(appICT220,dftAppICT220); CHK;
    ret= mapPutByte(appSettleInd,dftAppSettleInd); CHK;
    ret= mapPutStr(appBf,dftAppBf); CHK;
    ret= mapPutWord(appPANBlockBegin,dftAppPANBlockBegin); CHK;
    ret= mapPutWord(appPANBlockEnd,dftAppPANBlockEnd); CHK;
    ret= mapPutStr(appCurrSignSpaces,dftAppCurrSignSpaces); CHK;
    ret= mapPutStr(appPassTel,dftAppPassTel); CHK;
    ret= mapPutByte(appLockFlag,dftAppLockFlag); CHK;
    ret= mapPutByte(appAfterTMS,dftAppAfterTMS); CHK;
    ret= mapPutWord(appGetTick,dftAppGetTick); CHK;
    ret= mapPutStr(appTmsRefNum,dftAppTmsRefNum); CHK;
    ret= mapPutStr(appVersion,dftAppVersion); CHK;
    ret= mapPutStr(appSoftName,dftAppSoftName); CHK;
    ret= mapPutStr(appLtmkAcqID,dftAppLtmkAcqID); CHK;
    ret= mapPutStr(appVendorID,dftAppVendorID); CHK;
    ret= mapPutStr(appLTMKDATA,dftAppLTMKDATA); CHK;
    ret= mapPutStr(appModulusRSA,dftAppModulusRSA); CHK;
    ret= mapPutStr(appKeySessionTle,dftAppKeySessionTle); CHK;
    ret= mapPutStr(appNiiLTMK,dftAppNiiLTMK); CHK;
    ret= mapPutStr(appKMSLTMKDATA,dftAppKMSLTMKDATA); CHK;
    ret= mapPutStr(appLTMK,dftAppLTMK); CHK;
    ret= mapPutStr(appLTWKTID1,dftAppLTWKTID1); CHK;
    ret= mapPutStr(appTidTle,dftAppTidTle); CHK;
    ret= mapPutStr(appKeySessionSAM,dftAppKeySessionSAM); CHK;
    ret= mapPutByte(appTleSet,dftAppTleSet); CHK;
    ret= mapPutStr(appBit58,dftAppBit58); CHK;
    ret= mapPutByte(appTxnVoidBfr,dftAppTxnVoidBfr); CHK;
    ret= mapPutWord(appIdxBtcUpl,dftAppIdxBtcUpl); CHK;
    ret= mapPutWord(appNbrTxnVoidBfr,dftAppNbrTxnVoidBfr); CHK;
    ret= mapPutByte(appCntReceipt,dftAppCntReceipt); CHK;
    ret= mapPutStr(appNIIMntr,dftAppNIIMntr); CHK;
    ret= mapPutStr(appto,dftAppto); CHK;
    ret= mapPutStr(apptoidle,dftApptoidle); CHK;
    ret= mapPutStr(apptoTFHr,dftApptoTFHr); CHK;
    ret= mapPutStr(appPriMonSrv,dftAppPriMonSrv); CHK;
    ret= mapPutByte(appPriMonAtt,dftAppPriMonAtt); CHK;
    ret= mapPutStr(appSecMonSrv,dftAppSecMonSrv); CHK;
    ret= mapPutByte(appSecMonAtt,dftAppSecMonAtt); CHK;
    ret= mapPutStr(appLastAutCodPrepaid,dftAppLastAutCodPrepaid); CHK;
    ret= mapPutStr(appLastTCCode,dftAppLastTCCode); CHK;
    ret= mapPutStr(appTerminalType,dftAppTerminalType); CHK;
    ret= mapPutStr(appDateMonitoring,dftAppDateMonitoring); CHK;
    ret= mapPutByte(appFlagEDCMon,dftAppFlagEDCMon); CHK;
    ret= mapPutStr(appDateReInitRetryEDCMon,dftAppDateReInitRetryEDCMon); CHK;
    ret= mapPutWord(appTimeEdcMon,dftAppTimeEdcMon); CHK;
    ret= mapPutByte(appFlagDownloadLTWK,dftAppFlagDownloadLTWK); CHK;
    ret= mapPutByte(appFlagInit,dftAppFlagInit); CHK;
    ret= mapPutStr(appCtrlVersion,dftAppCtrlVersion); CHK;

    ret= 1; CHECK(ret>0,lblKO); goto lblEnd;
lblKO: ret= -1;
lblEnd:
    trcFN("fillDftApp: ret=%d\n",ret);
    return ret;
}

static void moveSlider(char *sld){
    if(strlen(sld)>=dspW) memset(sld,0,dspW);
    strcat(sld,".");
}

/** Fill the UTA data base by default values.
 * \param  key (I) Starting sentinel of the data structure to be reset to default values.
 * \return
 * - 1 if everything is OK
 * - negative if failure
 * 
 * The default data values are given in uta:dat attributes for fields in single records and table rows.
 * For multirecord fields they are provided in uta:data/uta/instance tags. 
 * If the parameter is keyBeg (zero) all data structures are reset to default values.
 * 
 * It is a time consuming operation.
 * Depending on the number of data elements in the data base it can take up to several minutes.
 * This function is usually called only once just after application download.
 */
int dftReset(word key){
    int ret;
    char sld[dspW+1];
    trcFN("dftReset: key=%d\n",key);

    ret= mapReset(key); CHK;
    if(key==keyBeg){
        memset(sld,0,dspW+1);

        moveSlider(sld); ret= fillDftMnu(); CHK;
        moveSlider(sld); ret= fillDftMsg(); CHK;
        moveSlider(sld); ret= fillDftMon(); CHK;
        moveSlider(sld); ret= fillDftTot(); CHK;
        moveSlider(sld); ret= fillDftCdb(); CHK;
        moveSlider(sld); ret= fillDftBin(); CHK;
        moveSlider(sld); ret= fillDftAcq(); CHK;
        moveSlider(sld); ret= fillDftIss(); CHK;
        moveSlider(sld); ret= fillDftApp(); CHK;

        //reset compiler date and time into reserved fields
        ret= mapPutStr(appCmpDat,getCmpDat()); CHK;
        ret= mapPutStr(appCmpTim,getCmpTim()); CHK;
    }else{
        switch(key){
            case mnuBeg: ret= fillDftMnu(); break;
            case msgBeg: ret= fillDftMsg(); break;
            case monBeg: ret= fillDftMon(); break;
            case totBeg: ret= fillDftTot(); break;
            case cdbBeg: ret= fillDftCdb(); break;
            case binBeg: ret= fillDftBin(); break;
            case acqBeg: ret= fillDftAcq(); break;
            case issBeg: ret= fillDftIss(); break;
            case appBeg: ret= fillDftApp(); break;
            default: ret= -1; break;
        }
        CHK;
    }

    ret= 1;
    goto lblEnd;

lblKO:
    trcErr(ret);
    ret= -1;
lblEnd:
    trcFN("dftReset: ret=%d\n",ret);
    return ret;
}

