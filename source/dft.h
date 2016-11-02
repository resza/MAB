#ifndef __DFT_H

//this file is included in dft.c only
//it is not visible for other source files

static char *dftMnu[DIM(mnuBeg,mnuEnd)]={
    "0 MENU UTAMA      ", //mnuMainMenu
    "1 KARTU KREDIT", //mnuUtamaKartuKredit
    "1 MINI ATM", //mnuUtamaMiniAtm
    "2? PURCHASE", //mnuPurchase
    //////"2 SALE", //mnuSale

/////	"1 TRANSFER BANK", //mnuTransferBank
///    "1 PRVT LABEL INS", // mnuPrvtLbl
///    "1 PRVT LABEL TAP", // mnuPrvtLblTes
    "2 MINI ATM", //mnuMiniAtmBkpn
    "3 MERCHANT", //mnuMiniAtmBkpnMer
    "3 NASABAH", //mnuMiniAtmBkpnNas
	"4 SETOR TUNAI", //mnuSetorBkpn
	"4 TARIK TUNAI", //mnuTarikBkpn
	"4 TRANSFER", //mnuTransferBkpn
	"5 TRAN BUKOPIN", //mnuTranBukopinBkpn
	"5 TRAN BANK LAIN ", //mnuTranBankLainBkpn
	"4 PEMBAYARAN", //mnuPembayaranBkpn
	"5 PLN", //mnuPln1Bkpn
	"6 PLN POSTPAID", //mnuPlnPostpaidBkpn
	"6 PLN NON TAGLIS", //mnuPlnNonTaglisBkpn
	"5 TELKOM", //mnuTeleponBkpn
	"5 SELULAR", //mnuSelularBkpn
	"6 TELKOMSEL HALO", //mnuSelularBkpnHalo
	"6 INDOSAT OOREDOO", //mnuSelularBkpnMatrix
	"6 XL XPLORE", //mnuSelularBkpnXplore
	"6 SMARTFREN", //mnuSelularBkpnSmartfren
	"6 ESIA", //mnuSelularBkpnEsia	
	"5 PAM/PDAM", //mnuPamPdamBkpn
	"6 AETRA", //mnuAetraBkpn
	"6 PALYJA", //mnuPalyjaBkpn
	"5 MULTIFINANCE", //mnuMultifinanceBkpn
	"6 MAF", //mnuMafBkpn
	"6 MCF", //mnuMcfBkpn
	"6 WOM", //mnuWomBkpn
	"6 BAF", //mnuBafBkpn
	"5 KARTU KREDIT", //mnuKartuKreditBkpn
	"6 KK ANZ", //mnuKartuKreditBkpnAnz,
	"6 KK BUKOPIN", //mnuKartuKreditBkpnBkpn,
	"6 KK BNI", //mnuKartuKreditBkpnBni,
	"6 KK HSBC", //mnuKartuKreditBkpnHsbc
	"4 PEMBELIAN", //mnuPembelianBkpn
	"5 E-VOUCHER", //mnuEvoucherBkpn
	"6 XL", //mnuEvoucherBkpnXl
	"6 TELKOMSEL", //mnuEvoucherBkpnTelkomsel
	"6 INDOSAT OOREDOO", //mnuEvoucherBkpnIndosat
	"6 ESIA", //mnuEvoucherBkpnEsia
	"6 SMARTFREN", //mnuEvoucherBkpnSmartfren
	"5 PLN", //mnuPln2Bkpn
	"6 PLN PREPAID", //mnuPlnPrepaidBkpn
	"6 MANUAL ADVICE", //mnuManualAdviceBkpn
	"4 INFORMASI", //mnuInformasiBkpn
	"5 INFO SALDO", //mnuInfoSaldoBkpn
	"5 INFO KODE BANK", //mnuKodeBankBkpn
	"5 INFO TRANSAKSI", //mnuInfoTransaksiBkpn
	"4 CETAK ULANG", //mnuCetakUlangBkpn
	"5 TRANSAKSI TERAKHIR", //mnuTransaksiTerakhirBkpn
	"5 TRANSAKSI NO", //mnuTransaksiNoBkpn
	"4 LAPORAN", //mnuLaporanBkpn
	"5 REKAP HARIAN", //mnuRekapHarianBkpn
	"5 DETAIL HARIAN", //mnuDetailHarianBkpn
	"5 FEE TRANSAKSI", //mnuFeetransaksiBkpn
	"4 LAIN-LAIN", //mnuLainlainBkpn
	"5 UBAH PIN", //mnuUbahPinBkpn
	"5 UBAH BAHASA", //mnuUbahBahasaBkpn
	"4 SETUP", //mnuSetupBkpn
	"5 INIT KARTU", //mnuInitKartuBukopin
	/*
    "2 TRANSFER  ", //mnuTransferBkpn
	"2 TELKOM  ", //mnuTelkomBkpn
	"2 SELLR POSTPAID  ", //mnuSelPosBkpn
	"2 SELLR PREPAID  ", //mnuSelPreBkpn
	"2 PLN  ", //mnuPlnBkpn
	"3 PLN POSTPAID  ", //mnuPlnBkpn1
	"3 PLN PREPAID  ", //mnuPlnBkpn2
	"2 PDAM  ", //mnuPdamBkpn
	"3 TPJ/AETRA  ", //mnuPdamBkpn1
	"3 PALYJA  ", //mnuPdamBkpn2
	"3 PAM BSD  ", //mnuPdamBkpn3
	"2 MULTIFINANCE  ", //mnuMultiBkpn
	"3 MCF  ", //mnuMultiBkpn1
	"3 MAF  ", //mnuMultiBkpn2
	"3 WOM  ", //mnuMultiBkpn3
	"3 BAF  ", //mnuMultiBkpn4
	"2 KARTU KREDIT  ", //mnuCCBkpn
	"2 FOOD BENEFIT  ", //mnuFBBkpn
	"3 JAMSOSTEK  ", //mnuFBBkpn1
	"2 TARIK TUNAI  ", //mnuTarikBkpn
	"3 BANK BUKOPIN  ", //mnuTarikBkpn1
	"3 BANK LAIN  ", //mnuTarikBkpn2
	"2 SETOR TUNAI  ", //mnuSetorBkpn
	"2 TOP UP  ", //mnuTopupBkpn	
    "2 SETTING  ", //mnuSettingMBkpn
    */


	"2 KARTU KREDIT", //mnuUtamaKartuKreditAwal
	"3 SALE", //mnuSale
//////    "1 CASH ADVANCE", //mnuCash
    "3 VOID", //mnuVoid
////    "1 REFUND", //mnuRefund
////    "1 CARD VER", //mnuCardVer
////    "1 OFFLINE", //mnuOffline
    "3 SETTLEMENT", //mnuSettlement
    "3 FUNCTIONS", //mnuFunctions
    "4 HOST LOGON>", //mnuHostLogon
    "4 ECHO TEST", //mnuEchoTest
    "4 REPRINT>", //mnuReprint
    "5 LAST TRANSACTION", //mnuLastTrx
    "5 ANY TRANSACTION ", //mnuAnyTrx
    "5 SETTLE DUPLICATE", //mnuMrcSetDup
    "4 BATCH TOTALS", //mnuMrcBatTot
    "4 BATCH VIEW", //mnuMrcViewBat
    "4 CLEAR BATCH", //mnuMrcReset
    "4 CLEAR REVERSAL", //mnuMrcResetRev
    "4 REPORT>         ", //mnuReport
    "5 DETAIL REPORT   ", //mnuDetailReport
    "5 SUMMARY REPORT  ", //mnuSummaryReport
    "4 SETCOMM  ", //mnuKoneksi
};

static char *dftMsg[DIM(msgBeg,msgEnd)]={
    "                ", //msgSpaces
    " Are you sure?  ", //msgAreYouSure
    "  Memory reset  ", //msgMemoryReset
    "Processing error", //msgProcessingError
    "  Data Access", //msgDataAccess
    "      Error", //msgError
    "  Crypto. Error ", //msgCryptoPeriphError
    " Out Of Memory ", //msgMemoryError
    "  Not Accepted  ", //msgNotAccepted
    "PIN:           ", //msgLastPin
    "PIN:            ", //msgPinPrompt
    "Invalid PIN code", //msgWrongPin
    "   Valid Pin    ", //msgValidPin
    "  LAST ATTEMPT  ", //msgLastAttempt
    " ENTER PASSWORD ", //msgPwdInput
    "           12.34", //msgPadAmt
    "   Dialing...   ", //msgDialing
    " Please Wait... ", //msgWait
    "  Connecting..  ", //msgConnecting
    "    Sending...  ", //msgSend
    "    RECEIVING   ", //msgReceive
    "ENTER AMOUNT:   ", //msgEnterAmount
    "ENTER CARD:     ", //msgEnterCard
    "   SWIPE CARD   ", //msgSwipeCard
    "  SWIPE/INSERT  ", //msgSwipeInsert
    "      CARD      ", //msgCard
    "FUNCTION ?", //msgEnterFunction
    "    FUNCTION    ", //msgFunction
    "       TMS      ", //msgTMS
    "    TERMINAL    ", //msgTerminal
    "  COMMUNICATION ", //msgCommunication
    "    CONFIGURE   ", //msgConfigure
    " LOAD PARAMETERS", //msgLoadParam
    "  LOAD SOFTWARE ", //msgLoadSoft
    "TMS CONFIG", //msgTMSConfig
    "TMS REFERENCE NO", //msgTMSRefNo
    "TMS NII :", //msgTMSNii
    "LINK SETUP(1-5)?", //msgLinkSetup
    "MODEM PARAMETER", //msgModemParam
    "PRIMARY NO. :", //msgPriNo
    "SECONDARY NO. :", //msgSecNo
    "BAUD RATE (1-7)?", //msgBaudRate
    "BUFFER SIZE(1-3)", //msgBufferSize
    "LOAD PARAMETERS", //msgLoadParamNow
    "NOW ?", //msgNow
    "HOST LINK(1-4)?", //msgHostLink 
    "   EMV CREDIT   ", //msgEMVCredit
    "TERMINAL  CONFIG", //msgTerminalConfig
    "   CARD RANGES  ", //msgCardRanges
    "     ISSUER     ", //msgIssuer
    "    ACQUIRER    ", //msgAcquirer
    "  EMV PARAMETER ", //msgEMVParam
    "    COMPLETED   ", //msgCompleted
    "      MASTERCARD", //msgMasterCard
    "      VISACARDS", //msgVisaCard
    "      SALE", //msgSale
    "5972037641051717", //msgMCardNo
    "4972037641051717", //msgVCardNo
    "AMT?", //msgAmount
    "TIP?", //msgTip
    "PIN?", //msgPin
    "TOTAL", //msgTotal
    "            Y/N?", //msgYesNo
    "VISACARDS   SALE", //msgVisaCardSale
    "   TRANSACTION  ", //msgTransaction
    "   NOT ALLOWED   ", //msgNotAllowed
    "  TRANSMITTING  ", //msgTransmitting
    "    RECEIVING   ", //msgReceiving
    "VERIFY SIGNATURE", //msgVerifySign
    "  MERCHANT COPY ", //msgMerchantCopy
    "  CUSTOMER COPY ", //msgCustomerCopy
    "    BANK COPY   ", //msgBankCopy
    "   PRESS ENTER  ", //msgPressEnter
    "  PRESS CANCEL  ", //msgPressCancel
    "   PRESS STOP   ", //msgPressStop
    "    PRINTING", //msgPrinting
    "", //msgEmpty
    "Sending..", //msgTleGo
    "RS232", //msgRs232
    "PORT (1-2)?", //msgPort
    "TCP/IP PARAMETER", //msgTCPIPprm
    "PRI. HOST IP:", //msgPriIP
    "PRI. HOST PORT:", //msgPriHostPort
    "SEC. HOST IP:", //msgSecIP
    "SEC. HOST PORT:", //msgSecHostPort
    "LINE PARAMETERS", //msgLineParam
    "1-TONE 2-PULSE?", //msgTonePulse
    "PABX NUMBER", //msgPabx
    "TONE DETECT", //msgToneDetect
    "   VIEW BATCH   ", //msgViewBatch
    "LOG:", //msgLog
    "O/SALE", //msgOsale
    "PREV  MORE  NEXT", //msgPrevNext
    "      MORE      ", //msgMore
    "CURRENT TRACE #", //msgCurrentTrace
    "SET TRACE #", //msgSetTrace
    "ENTER TRACE #", //msgEnterTrace
    "  CURRENT BATCH ", //msgCurrentBatchNum
    "   VIEW STAN #  ", //msgViewStan
    "SET STAN #", //msgSetStan
    "VIEW CONFIG", //msgViewConfig
    "TERMINAL CONFIG", //msgTermConfig
    "SELECT ACQUIRER", //msgSelectAcq
    "APPLI. SELECT...", //msgApplicationSelect
    "  NOT ACCEPTED  ", //msgAidListEmpty
    "    Confirm     ", //msgConfirm
    "   YES = ENTER  ", //msgYesVal
    "   NO  = CLEAR  ", //msgNoAnn
    " APP BLOCK...", //msgEmvAppBlock
    " CARD BLOCK...", //msgEmvCrdBlock
    "TO BE PROCESSED ", //msgShouldBeProcessed
    "  AS CHIP CARD  ", //msgEmvCard
    "New password:   ", //msgNewPwdInput
    "Confirm pwd:    ", //msgConfPwdInput
    "INVALID PASSWORD", //msgPwdInvalid
    "LOAD TEST KEY   ", //msgTestKeyLoad
    "IP ADDRESS :    ", //msgIpAdd
    "SUBNET MASK :   ", //msgSubMask
    "GATEWAY :       ", //msgGateway
    "ENABLE EDH      ", //msgEnableEdh
    "GPRS APN?       ", //msgGprsApn
    "GPRS PIN?       ", //msgGprsPin
    "GPRS USERNAME?  ", //msgGprsUsername
    "GPRS PASSWORD?  ", //msgGprsPassword
    "GPRS PHONE?     ", //msgGprsPhone
    "GPRS", //msgGprs
    "1-OFF 2-ON ?", //msgOnOff
    "GPRS PARAMETER", //msgGprsParameter
    "TCP/IP", //msgTCPIP
    "DIAL UP", //msgDIALUP
    "Not defined", //msgNotDef
    "      DONE      ", //msgDone
    " EMV CONTEXT... ", //msgEmvContext
    " 1200bps", //msg1200
    " 2400bps", //msg2400
    " 9600bps", //msg9600
    " 14400bps", //msg14400
    " 19200bps", //msg19200
    " 28800bps", //msg28800
    " 33600bps", //msg33600
    " 38400bps", //msg38400
    " 57600bps", //msg57600
    " 115200bps", //msg115200
    "500", //msg500
    "1000", //msg1000
    "1400", //msg1400
    "   USER CANCEL", //msgUserCancel
    "  INVALID BIN   ", //msgBinNotFound
    "  Card Refused  ", //msgCardRefused
    "  INVALID CARD  ", //msgInvalidPan
    "  INVALID LUHN  ", //msgInvalidLuhn
    "  CARD EXPIRED  ", //msgCardExpired
    " INVALID INPUT  ", //msgInvalidInput
    "Resp. code:  %s", //msgRspCodBgd
    "ABCDEFGHIJKLMabc", //msgRspCodCtl
    "    Connected   ", //msgConnConnected
    "    LINE BUSY   ", //msgConnBusy
    "  NO DIAL TONE  ", //msgConnNoDialTone
    "   NO CARRIER   ", //msgConnNoCarrier
    "     TIMEOUT    ", //msgConnTimeout
    "CONNECTION FAIL", //msgConnFail
    "    REDIALING   ", //msgRedialing
    "  RECONNECTING  ", //msgReConnecting
    " SENDING FAILED ", //msgSendingFailed
    "RECEIVING FAILED", //msgReceivingFailed
    "    REVERSAL   ", //msgReversal
    "    APPROVE", //msgApprove
    "VOID", //msgVoid
    " ALREADY VOIDED", //msgAlreadyVoid
    " REF# NOT FOUND", //msgRefNotFound
    "TRACE# NOT FOUND", //msgTraceNotFound
    "TRAN DATE/  TIME", //msgLogTraDatTim
    "NO TRANSACTIONS", //msgNoTran
    "OFF", //msgOff
    "ON", //msgOn
    "   BATCH EMPTY", //msgLogEmpty
    "    BATCH FULL", //msgLogFull
    "   SETTLEMENT   ", //msgStl
    "  BATCH UPLOAD  ", //msgBatUpl
    "   CARD RANGES  ", //msgCrdRng
    "     ISSUER     ", //msgIss
    "    ACQUIRER    ", //msgAcq
    " EMV PARAMETER ", //msgEmvPrm
    "", //msgVar
    "ENTER DATE:", //msgEntDat
    "ENTER TIME:", //msgEntTim
    "Load Master Key", //msgLodMstKey
    "Load Working Key", //msgLodWrkKey
    "Enter password: ", //msgEntPwd
    "FUNCT PASSWORD ?", //msgFncPwd
    " TERMINAL RESET ", //msgTerReset
    "Offline Txns", //msgOfltxn
    "   Enter PAN    ", //msgPanInput
    "Enter Exp. date ", //msgExpDatInput
    "ENABLE DHCP", //msgEnableDhcp
    " INVALID TRACK  ", //msgInvalidTrack
    "Approval Code?", //msgEnterAppCod
    "  REMOVE CARD  ", //msgRmvCrd
    "EMV AUTHENTICATE", //msgEmvAuthenticate
    "   EMV CVM...   ", //msgEmvAppCvm
    "    DUPLICATE   ", //msgDuplicata
    "APPROVED", //msgApproved
    "    DECLINED    ", //msgDeclined
    "TONE", //msgTone
    "PULSE", //msgPulse
    " MANUAL BARRED", //msgManBarred
    "  CONNECTION:  ", //msgConnection
    "TC  : ", //msgEmvTc
    "TVR: ", //msgEmvTvr
    "TSI : ", //msgEmvTsi
    " INVALID AMOUNT ", //msgInvalidAmount
    "   TMS ERROR    ", //msgTMSError
    "  FILE MISSING  ", //msgFileMiss
    "     SERVER     ", //msgServer
    "  UNAVAILABLE   ", //msgUnavailable
    "  QUERY FAILED  ", //msgQueryFailed
    "  DL TOO LARGE  ", //msgDLLarge
    "   VER. ERROR   ", //msgInvVer
    "   BUF OVRFLW   ", //msgBufOvrFlw
    "   INVALID REQ  ", //msgInvReq
    "   INV REF NUM  ", //msgInvRefNum
    "   UNKNOWN REQ  ", //msgUnknownReq
    " OUT OF BOUNDS  ", //msgOutBounds
    "  ERROR IN REQ  ", //msgErrorReq
    " UNDEFINED ERR  ", //msgUndefError
    "      PRINT", //msgPrint
    "BATCH NOT EMPTY", //msgBatNotEmpty
    "  DO SETTLEMENT", //msgPerformStl
    "DELETE REVERSAL", //msgDeleteRev
    "  DELETE BATCH  ", //msgDeleteBat
    "  BATCH DELETED ", //msgBatDelete
    "   DELETE TC    ", //msgDeleteTC
    "REVERSAL DELETED", //msgRevDelete
    "  TC DELETED   ", //msgTCDelete
    "COM 1", //msgComPort1
    "COM 2", //msgComPort2
    "GPRS PARAMETER", //msgGPRSprm
    "NOT INITIALIZED", //msgNotInit
    "  NO EMV PARAM", //msgNoEmvFile
    " LOADED IN TMS", //msgLoadedTms
    " DEF PARAM USED", //msgDefaultEMVParam
    "ENABLE BLOWFISH?", //msgEnableBlowfish
    "ON BLOWFISH", //msgOnBf
    "OFF BLOWFISH", //msgOffBf
    "1 = OFF ; 2 = ON", //msgSetBf
    "TLE ON", //msgOnTle
    "TLE OFF", //msgOffTle
    "EMV OFFLINE DEC", //msgOfflineDeclined
    "  PRESS ANY KEY  ", //msgPressAnyKey
    "CHIP MALFUNCTION", //msgChipMalfunction
    "    FALLBACK   ", //msgFallback1
    " PLS SWIPE CARD ", //msgFallback2
    "  EMV DETECTED  ", //msgChipDetect1
    "PLS INSERT CARD", //msgChipDetect2
    "  UNKNOWN AID   ", //msgUnknownAID
    " CARD PROC ERR  ", //msgCardErr
    "STARTING GPRS...", //msgStartingGPRS
    "COMSTART KO     ", //msgComStartKO
    "COMSET KO       ", //msgComSetKO
    "COMDIAL KO      ", //msgComDialKO
    "SUCCESSFULLY    ", //msgSuccessfully
    "CONFIGURED    ", //msgConfigured
    " CALL YOUR BANK ", //msg01Declined1
    " INSUFF. FUNDS  ", //msg51Declined1
    "  OVER LIMIT    ", //msg51Declined2
    " USE MAGSTRIPE  ", //msgUseMag
    " DO NOT HONOUR  ", //msgDoNotHonour
    "  LAST PIN TRY  ", //msgLastPrintTry
    "APPLICATION NOT ", //msgAppNot
    "   AVAILABLE    ", //msgAvailable
    "  PIN BLOCKED   ", //msgPinBlocked
    "  EMV OFFLINE   ", //msgEmvOffline
    " CARD PROC ERR  ", //msgCardProcErr
    "  EMV DECLINED  ", //msgEmvDeclined
    "   PROC. ERROR   ", //msgProcErr
    "TERM CAP : ", //msgTermCap
    "ADD TERM CAP : ", //msgAddTermCap
    "Txt : ", //msgTxt
    "Code Table : ", //msgCodeTable
    "APP.: ", //msgEmvAppType
    "AID : ", //msgEmvAID
    "API : ", //msgEmvApi
    "PAN : ", //msgPAN
    "DEN : ", //msgDen
    "ONL : ", //msgOnl
    "DEF : ", //msgDef
    "CVMR : ", //msgEmvCvmr
    "9F53 :", //msg9F53
    "Gen AC", //msgGenAC
    "AC :", //msgAC
    "ATC :", //msgATC
    "TCC : ", //msgTCC
    "Amount =  ", //msgEmvAmount
    "PrevAmt =  ", //msgPrevAmount
    "TotAmt = ", //msgTotAmt
    "Floor  = ", //msgFloor
    "AIP : ", //msgAIP
    "CID : ", //msgCid
    "IFD SER NUM : ", //msgIFDSerNum
    "Term Type  : ", //msgTermType
    "EXP DATE : ", //msgExpDate
    "1st : ", //msgFirst
    "2nd : ", //msgSecond
    " RECV. TIMEOUT  ", //msgRecvTimeout
    "CARD WAS REMOVED", //msgCardRemoved
    "INVALID TERM ID", //msgInvalidTID
    "INVALID MERC ID", //msgInvalidMID
    "USB", //msgUSB
    "WIFI", //msgWIFI
    "   TC UPLOAD   ", //msgTCUpload
    "     FAILED    ", //msgFailed
    " INCORRECT PIN ", //msgIncorrectPIN
    " CARD DECLINED ", //msgCardDeclined
    "RETRY SETTLEMENT", //msgRetrySetl
    "ACCESS CODE ?", //msgAccCode
    "  PLEASE INSERT ", //msgPleaseInsert
    "      CHIP      ", //msgChip
    "   MAGSTRIPE   ", //msgMagstripe
    "     MANUAL     ", //msgManual
    "  ENTRY BARRED  ", //msgEntryBarred
    "   VOID BARRED  ", //msgVoidBarred
    " ADJUST BARRED ", //msgAdjustBarred
    " OFFLINE BARRED ", //msgOfflineBarred
    " PREAUTH BARRED ", //msgPreautBarred
    " REFUND BARRED ", //msgRefundBarred
    "INVALID MERCHANT", //msgInvalidMerchant
    "  TIP TOO MUCH  ", //msgTooMuch
    "  DOWNLOAD KEY ", //msgDownloadKey
    " INVALID TRANS ", //msgInvalidTrx
    "  PICK UP CARD  ", //msgPickUpCard
    "    INVALID    ", //msgInvalid
    " ACCOUNT NUMBER ", //msgAccNumber
    " RE-ENTER TRANS ", //msgReEnterTrans
    "REFER TO SPECIAL", //msgReferTo
    "   CONDITION    ", //msgCondition
    "   HONOR WITH   ", //msgHonorWith
    " IDENTIFICATION ", //msgIdentification
    "REQ IN PROGRESS", //msgReqInProgress
    " NO SUCH ISSUER ", //msgNoSuchIss
    "    CUSTOMER    ", //msgCustomer
    "  CANCELLATION  ", //msgCancellation
    "     DISPUTE    ", //msgDispute
    "INVALID RESPONSE", //msgInvalidRsp
    "NO ACTION TAKEN", //msgNoActionTaken
    "   SUSPECTED   ", //msgSuspect
    "   MALFUNCTION  ", //msgMalFunction
    "  UNACCEPTABLE  ", //msgUnacceptable
    "TRANSACTION FEES", //msgTrxFees
    "  FORMAT ERROR  ", //msgFormatError
    "NOT SUPPORTED", //msgNotSupport
    "   BY SWITCH   ", //msgBySwitch
    "   PARTIALLY   ", //msgPartially
    "SUSPECTED FRAUD", //msgSuspFraud
    "  REF ACQUIRER  ", //msgRefAcq
    "    PICK UP    ", //msgPickUp
    "RESTRICTED CARD", //msgRestCard
    " MERCHANT CALL ", //msgMercCall
    "ACQUIRER SCURITY", //msgAcqSecurity
    "    EXCEEDED   ", //msgExceeded
    " NO CREDIT ACC ", //msgNoCrAcc
    "REQUEST FUNCTION", //msgReqFunct
    "   CARD LOST   ", //msgCardLost
    "  NO UNIV ACC  ", //msgNoUnivAcc
    "  CARD STOLEN  ", //msgCardStolen
    " NO INVEST ACC ", //msgNoInvestAcc
    "NO CHEQUING ACC", //msgNoCheqAcc
    " NO SAVING ACC ", //msgNoSavAcc
    " NO CARD RECORD ", //msgNoCardRecord
    "TRX NOT PRMITTED", //msgTrxNotPermit
    " FOR CARDHOLDER", //msgForCH
    "MERCHANT CONTACT", //msgMercCont
    "WITHDRAWAL LIMIT", //msgWithdrLmt
    "SECRUTY VIOLTION", //msgSecViolation
    "  HARD CAPTURE  ", //msgHardCapture
    "  PIN RETRIES  ", //msgPinRetries
    "INELIGIBLE ACCT", //msgIneligibleAccount
    "   NO SHARING  ", //msgNoSharing
    " REF CARD ISSUE ", //msgRefCardIssue
    " TRX IN WINDOW ", //msgAppvTrxInWind
    " TRX OUT WINDOW", //msgAppvTrxOutWind
    "  TRX BALANCE  ", //msgAppvTrxBal
    "ACC NO STMT INF", //msgAccNoStmtInf
    " STMT INFO NOT ", //msgStmtInfNotAvail
    "   AVAILABLE   ", //msgStmtInfNotAvail2
    "  SYSTEM ERROR  ", //msgSystemError
    "DATABASE PROBLEM", //msgDatabaseProblem
    " CUT OFF IS IN ", //msgCutIsInProg
    "    PROGRESS   ", //msgCutIsInProg2
    " ISSUER/SWITCH ", //msgIssInOperative
    "  INOPERATIVE  ", //msgIssInOperative2
    " TRX INCOMPLETE", //msgTrxNotComplete
    " DUPLICATE TRX ", //msgDuplicateTransaction
    "RECONCILE ERROR", //msgReconcileError
    "     SYSTEM     ", //msgSystem
    "  INSUFFICIENT ", //msgInsufficientBal
    "     BALANCE   ", //msgInsufficientBal2
    " ACC NOT FOUND ", //msgAccNotFound
    "ACC IN ATTENTION", //msgAccIntAttentionl
    "     REJECT    ", //msgReject
    "    PENDING    ", //msgPending
    "   NO PENDING   ", //msgUpdate
    "   OVER LIMIT     ", //msgOvrlimit
    "   KARTU TIDAK     ", //msgKartutdk
    "   TERDAFTAR     ", //msgTerdaftar
    " INVALID AMOUNT ", //msgB1
    "   EXCEED MAX   ", //msgB2_1
    " NUMBER OF BILL ", //msgB2_2
    "   2 BILL FOR   ", //msgB3_1
    " THE SAME MONTH ", //msgB3_2
    "  MOBILE NUMBER  ", //msgB4_1
    " HAS BEEN BLOCK ", //msgB4_2
    "    BILL REF    ", //msgB5_1
    "   NOT FOUND    ", //msgB5_2
    "    MUST PAY    ", //msgB6_1
    " PREVIOUS BILL  ", //msgB6_2
    "PARTIAL PAYMENT ", //msgB7_1
    "  NOT ALLOWED   ", //msgB7_2
    "    BILL HAS    ", //msgB8_1
    "  ALREADY PAID  ", //msgB8_2
    "   INVALID PH/   ", //msgB9_1
    "CUST ID/USER ID", //msgB9_2
    "    UNDEFINED   ", //msg06_1
    "   ERROR CODE   ", //msg06_2
    " MESSAGE FORMAT ", //msg11_1
    "     ERROR      ", //msg11_2
    "  MESSAGE EDIT  ", //msg31_1
    "     ERROR      ", //msg31_2
    "   EXCEED LIMIT    ", //msg61
    "   MORE THAN    ", //msg62_1
    "  3 REFERENCE   ", //msg62_2
    " MESSAGE FORMAT ", //msg67_1
    "     ERROR      ", //msg67_2
    " INVALID PHONE  ", //msg76_1
    "     NUMBER     ", //msg76_2
    "    PROVIDER    ", //msg87_1
    "DATABASE PROBLEM", //msg87_2
    " DUPLICATE KEY  ", //msg88
    "    TIME OUT    ", //msg90
    "     LINK DOWN   ", //msg92
    "INVALID ROUTING ", //msg93
    "UNDEFINED ERROR", //msgC1
    " MESSAGE FORMAT ", //msgC2_1
    "  MESSAGE EDIT  ", //msgC3_1
    "   MORE THAN   ", //msgC4_1
    "     3 REF      ", //msgC4_2
    "    CVV ERROR   ", //msgP1
    "  TRANS LIMIT   ", //msgP2_1
    "    CARD NOT    ", //msgP3_1
    "     ACTIVE     ", //msgP3_2
    " ERROR MISMATCH ", //msgP4_1
    "     TRACK2      ", //msgP4_2
    "  EXCEEDED PIN  ", //msgP5_1
    "   RETRY LIMIT   ", //msgP5_2
    "INVALID SERVICE", //msgP9_1
    "    FOR CARD    ", //msgP9_2
    "    HOT CARD    ", //msgQ1_1
    "   CARD LIMIT   ", //msgQ2_1
    "    EXCEEDED    ", //msgQ2_2
    "  NO CHECKHING  ", //msgQ3_1
    "    ACCOUNT     ", //msgQ3_2
    "   NO SAVINGS   ", //msgQ4_1
    "  EXPIRED CARD  ", //msgQ5
    "  PIN MISMATCH  ", //msgQ6
    "  SERVICE NOT  ", //msgQ7_1
    "    ACTIVED    ", //msgQ7_2
    "   ACCOUNT NOT   ", //msgQ9_1
    "     FOUND     ", //msgQ9_2
    "  SYSTEM ERROR  ", //msgR1_1
    "  TERMINAL HSM  ", //msgR1_2
    "   BATCH RUN   ", //msgR2_2
    "   TRANSACTION   ", //msgS4
    "  PLEASE SWIPE ", //msgPleaseSwipe
    "    MINI ATM    ", //msgMiniATM
    "  TARIK TUNAI    ", //msgTarikTunai
    "INVALID RESPONSE", //msgInvalidResponse
    "PLEASE TRY AGAIN", //msgPlsTryAgain
    "  DOWNLOAD LTWK ", //msgDownloadLTWK
};

static char *dftMon[DIM(monBeg,monEnd)]={
    "-: ", //monSpr
    "JAN", //monJan
    "FEB", //monFeb
    "MAR", //monMar
    "APR", //monApr
    "MAY", //monMay
    "JUN", //monJun
    "JUL", //monJul
    "AUG", //monAug
    "SEP", //monSep
    "OCT", //monOct
    "NOV", //monNov
    "DEC", //monDec
};


typedef struct sCdbRecord{
    char *_cdbBankKode;
    char *_cdbBankName;
}tCdbRecord;

static tCdbRecord dftCdb[]={
    {"002","BRI            ",}, //
};

typedef struct sBinRecord{
    byte _binID;
    char *_binTotAppDbAmt;
    word _binTotAppDbCnt;
    char *_binTotAppCrAmt;
    word _binTotAppCrCnt;
    char *_binTotAppTpAmt;
    word _binTotAppTpCnt;
}tBinRecord;

static tBinRecord dftBin[]={
    {1,"0",0,"0",0,"0",0,}, //
};

typedef struct sAcqRecord{
    byte _acqID;
    char *_acqName;
    char *_acqCurBat;
    char *_logAcqCurBat;
    char *_acqTotAppDbAmt;
    word _acqTotAppDbCnt;
    char *_acqTotAppCrAmt;
    word _acqTotAppCrCnt;
    char *_acqTotAppTpAmt;
    word _acqTotAppTpCnt;
    char *_acqTotAppSaleAmt;
    word _acqTotAppSaleCnt;
    char *_acqTotAppRefundAmt;
    word _acqTotAppRefundCnt;
    char *_acqTotAppOfflineAmt;
    word _acqTotAppOfflineCnt;
    char *_acqTotAppManualAmt;
    word _acqTotAppManualCnt;
    char *_acqTotAppVoidAmt;
    word _acqTotAppVoidCnt;
    char *_acqTotAppPrepaidAmt;
    word _acqTotAppPrepaidCnt;
    char *_acqTotAppPrepaidAmtUpdate;
    word _acqTotAppPrepaidCntUpdate;
    char *_logAcqTotAppAmtPrepaid;
    word _logAcqTotAppCntPrepaid;
    char *_logAcqTotAppAmtPrepaidUpdate;
    word _logAcqTotAppCntPrepaidUpdate;
    char *_acqTotAppTelkomAmt;
    word _acqTotAppTelkomCnt;
    char *_acqTotAppSatelindoAmt;
    word _acqTotAppSatelindoCnt;
    char *_acqTotAppTelkomselAmt;
    word _acqTotAppTelkomselCnt;
    char *_acqTotAppExcelcomAmt;
    word _acqTotAppExcelcomCnt;
    char *_acqTotAppEsiaAmt;
    word _acqTotAppEsiaCnt;
    char *_acqTotAppPLNAmt;
    word _acqTotAppPLNCnt;
    char *_acqTotAppFirstMediaAmt;
    word _acqTotAppFirstMediaCnt;
    char *_acqTotAppIndovisionAmt;
    word _acqTotAppIndovisionCnt;
    char *_acqTotAppOtoMultiAmt;
    word _acqTotAppOtoMultiCnt;
    char *_acqTotAppMAtmAmt;
    word _acqTotAppMAtmCnt;
    char *_acqTotAppMINIATMAmt;
    char *_acqTotAppPrepaidAmtGP;
    word _acqTotAppPrepaidCntGP;
    char *_logAcqTotAppAmtPrepaidGP;
    word _logAcqTotAppCntPrepaidGP;
    char *_acqTotAppAmtTunai;
    word _acqTotAppCntTunai;
    char *_acqTotAppAmtTransfer;
    word _acqTotAppCntTransfer;
    char *_acqTotAppAmtCC;
    word _acqTotAppCntCC;
    char *_acqTotAppAmtTransferUSD;
    word _acqTotAppCntTransferUSD;
    char *_acqTotAppAmtTransferSGD;
    word _acqTotAppCntTransferSGD;
    char *_acqTotAppAmtTransferJPN;
    word _acqTotAppCntTransferJPN;
    char *_logAcqTotAppAmtTunai;
    word _logAcqTotAppCntTunai;
    char *_logAcqTotAppAmtTransfer;
    word _logAcqTotAppCntTransfer;
    char *_logAcqTotAppAmtCC;
    word _logAcqTotAppCntCC;
    char *_logAcqTotAppAmtTransferUSD;
    word _logAcqTotAppCntTransferUSD;
    char *_logAcqTotAppAmtTransferSGD;
    word _logAcqTotAppCntTransferSGD;
    char *_logAcqTotAppAmtTransferJPN;
    word _logAcqTotAppCntTransferJPN;
    word _acqTotAppCommCnt;
    word _logAcqTotAppCntComm;
    char *_logacqTotAppTelkomAmt;
    word _logacqTotAppTelkomCnt;
    char *_logacqTotAppSatelindoAmt;
    word _logacqTotAppSatelindoCnt;
    char *_logacqTotAppTelkomselAmt;
    word _logacqTotAppTelkomselCnt;
    char *_logacqTotAppExcelcomAmt;
    word _logacqTotAppExcelcomCnt;
    char *_logacqTotAppEsiaAmt;
    word _logacqTotAppEsiaCnt;
    char *_logacqTotAppPLNAmt;
    word _logacqTotAppPLNCnt;
    char *_logacqTotAppFirstMediaAmt;
    word _logacqTotAppFirstMediaCnt;
    char *_logacqTotAppIndovisionAmt;
    word _logacqTotAppIndovisionCnt;
    char *_logacqTotAppOtoMultiAmt;
    word _logacqTotAppOtoMultiCnt;
    char *_logacqTotAppMAtmAmt;
    word _logacqTotAppMAtmCnt;
    char *_logacqTotAppMINIATMAmt;
    char *_logAcqTotAppDbAmt;
    word _logAcqTotAppDbCnt;
    char *_logAcqTotAppVoidAmt;
    word _logAcqTotAppVoidCnt;
    char *_logAcqTotAppTipAmt;
    word _logAcqTotAppTipCnt;
    char *_acqPriTxnPhnSrv;
    char *_acqSecTxnPhnSrv;
    char *_acqPriStlPhnSrv;
    char *_acqSecStlPhnSrv;
    char *_acqPriTxnIPSrv;
    char *_acqSecTxnIPSrv;
    char *_acqPriStlIPSrv;
    char *_acqSecStlIPSrv;
    char *_acqMasterKey1;
    char *_acqMasterKey2;
    char *_acqWorkingKey;
    char *_acqPinKey;
    byte _acqPendingSettle;
    byte _acqLogonFlag;
    byte _acqFlagCountTrx;
}tAcqRecord;

static tAcqRecord dftAcq[]={
    {1,"MCChost","000001","000001","0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0","0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,0,0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0","0",0,"0",0,"0",0,"5263768","5263768","5263768","5263768",__DFTSRV__,__DFTSRV__,__DFTSRV__,__DFTSRV__,"\xB9\x3E\xA1\x51\xDA\x79\x13\x31","\x83\x67\x19\x3B\x34\x9E\x04\x31","\x8A\x5A\xE1\xF8\x1A\xB8\xF2\xDD\x8A\x5A\xE1\xF8\x1A\xB8\xF2\xDD","\x8A\x5A\xE1\xF8\x1A\xB8\xF2\xDD\x8A\x5A\xE1\xF8\x1A\xB8\xF2\xDD",0,0,0,}, //
};

typedef struct sIssRecord{
    byte _issID;
    char *_issRef;
    byte _issOpt1;
    byte _issOpt2;
    byte _issOpt3;
    byte _issOpt4;
    byte _issDefAcct;
    word _issFloorLimit;
    byte _issOpt5;
    byte _issPinOpt;
    char *_issTotAppDbAmt;
    word _issTotAppDbCnt;
    char *_issTotAppCrAmt;
    word _issTotAppCrCnt;
    char *_issTotAppTpAmt;
    word _issTotAppTpCnt;
    char *_issTotAppSaleAmt;
    word _issTotAppSaleCnt;
    char *_issTotAppRefundAmt;
    word _issTotAppRefundCnt;
    char *_issTotAppOfflineAmt;
    word _issTotAppOfflineCnt;
    char *_issTotAppManualAmt;
    word _issTotAppManualCnt;
    char *_issTotAppVoidAmt;
    word _issTotAppVoidCnt;
    char *_issTotAppAmtPrepaid;
    word _issTotAppPrepaidCnt;
    char *_issTotAppAmtPrepaidUpdate;
    word _issTotAppPrepaidCntUpdate;
    char *_issTotAppAmtTelkom;
    word _issTotAppCntTelkom;
    char *_issTotAppAmtSatelindo;
    word _issTotAppCntSatelindo;
    char *_issTotAppAmtTelkomsel;
    word _issTotAppCntTelkomsel;
    char *_issTotAppAmtExcelcom;
    word _issTotAppCntExcelcom;
    char *_issTotAppAmtEsia;
    word _issTotAppCntEsia;
    char *_issTotAppAmtPLN;
    word _issTotAppCntPLN;
    char *_issTotAppAmtFirstMedia;
    word _issTotAppCntFirstMedia;
    char *_issTotAppAmtIndovision;
    word _issTotAppCntIndovision;
    char *_issTotAppAmtOtoMulti;
    word _issTotAppCntOtoMulti;
    char *_issTotAppAmtMAtm;
    word _issTotAppCntMAtm;
    char *_issTotAppPrepaidAmtGP;
    word _issTotAppPrepaidCntGP;
    char *_issTotAppAmtMiniAtm;
    char *_issTotAppAmtTunai;
    word _issTotAppCntTunai;
    char *_issTotAppAmtTransfer;
    word _issTotAppCntTransfer;
    char *_issTotAppAmtCC;
    word _issTotAppCntCC;
    char *_issTotAppAmtTransferUSD;
    word _issTotAppCntTransferUSD;
    char *_issTotAppAmtTransferSGD;
    word _issTotAppCntTransferSGD;
    char *_issTotAppAmtTransferJPN;
    word _issTotAppCntTransferJPN;
    word _issTotAppCntComm;
    byte _issAccSelect;
    byte _issEMVFallback;
    byte _issManual;
    byte _issExpiryDatReq;
    byte _issOffline;
    byte _issRefferalDial;
    byte _issProductCod;
    byte _issAdjust;
    byte _issDoModPAN;
    byte _issROCNumReq;
    byte _issPrintReceipt;
    byte _issCapTxns;
    byte _issCheckExpOff;
    byte _issRefund;
    byte _issPreauth;
    byte _issVoid;
    byte _issAdditionPrompts;
    byte _issCheckExpAll;
    byte _issCashOut;
    byte _issTrack1;
    byte _issBalance;
    byte _issErrorReceipt;
    byte _issOfflineROC;
    byte _issExtServiceCod;
    byte _issFullPAN;
    byte _issExpiryDate;
    byte _issManEntryCVV;
    char *_issTotAppAmtMOnUs;
    word _issTotAppCntMOnUs;
    char *_issTotAppAmtVOnUs;
    word _issTotAppCntVOnUs;
    char *_issTotAppAmtMOffUs;
    word _issTotAppCntMOffUs;
    char *_issTotAppAmtVOffUs;
    word _issTotAppCntVOffUs;
    char *_issTotAppVoidAmtMOnUs;
    word _issTotAppVoidCntMOnUs;
    char *_issTotAppVoidAmtVOnUs;
    word _issTotAppVoidCntVOnUs;
    char *_issTotAppVoidAmtMOffUs;
    word _issTotAppVoidCntMOffUs;
    char *_issTotAppVoidAmtVOffUs;
    word _issTotAppVoidCntVOffUs;
    char *_issTotAppTipAmtMOnUs;
    char *_issTotAppTipAmtVOnUs;
    char *_issTotAppTipAmtMOffUs;
    char *_issTotAppTipAmtVOffUs;
}tIssRecord;

static tIssRecord dftIss[]={
    {0,"MINIATM",0x9E,0xDC,0xD1,0x01,0,0,0xF0,0x31,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0","0",0,"0",0,"0",0,"0",0,"0",0,"0",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0",0,"0","0","0","0"}, //issIssuer1
};

#define dftAppEndTime "2000"
#define dftAppGprsApn __DFTAPN__
#define dftAppChn __DFTCHN__
#define dftAppSrv __DFTSRVTMS__
#define dftAppSrvAtt 2
#define dftAppPabx "9"
#define dftAppTonePulse 1
#define dftAppToneDetect 1
#define dftAppPrefix ""
#define dftAppMrcPwd "0000"
#define dftAppAdmPwd "0000"
#define dftAppAccCode "6212"
#define dftAppExp 0
#define dftAppTid "10000001"
#define dftAppMid "000001019100001"
#define dftAppAmtFmt 0x03
#define dftAppDefTra 0
#define dftAppTermOpt1 0x04
#define dftAppTermOpt2 0x22
#define dftAppTermOpt5 0xC0
#define dftAppLocOpt 0x0F
#define dftAppCurrSign "Php"
#define dftAppConnType 1
#define dftAppComType 1
#define dftAppVoidPwd "0000"
#define dftAppRefPwd "0000"
#define dftAppSetlPwd "0000"
#define dftAppEMSale 0xF7
#define dftAppEMRefund 0xF7
#define dftAppEMOffline 0xF7
#define dftAppEMCash 0xF7
#define dftAppEMPreAuth 0xF7
#define dftAppEMCrdVrfy 0xF7
#define dftAppEMBalance 0xF7
#define dftAppGPRSFlag 0
#define dftAppNII "015"
#define dftAppPriSrv __DFTSRVTMS__
#define dftAppPriAtt 2
#define dftAppSecSrv __DFTSRVTMS__
#define dftAppSecAtt 2
#define dftAppDialOpt 2
#define dftAppBaudRate 1
#define dftAppTmsBufSize 1
#define dftAppIpAdd __DFTSRVTMS__
#define dftAppSecIpAdd __DFTSRVTMS__
#define dftAppCrdInpCtl "mck"
#define dftAppPinKeyLoc __appPinKeyLoc__
#define dftAppMKeyLoc __appMKeyLoc__
#define dftAppDefRxTO 30
#define dftAppDefICTO 3
#define dftAppDefConTO 180
#define dftApptraSale "SALE"
#define dftApptraOSale "O/SALE"
#define dftApptraOffline "OFFLINE"
#define dftApptraPreAuth "PREAUTH"
#define dftApptraVoid "VOID"
#define dftApptraRefund "REFUND"
#define dftApptraAdjust "ADJUST"
#define dftApptraManual "MANUAL"
#define dftAppTenor1 "TENOR 1"
#define dftAppTenor3 "TENOR 3"
#define dftAppTenor6 "TENOR 6"
#define dftAppTenor9 "TENOR 9"
#define dftAppTenor12 "TENOR 12"
#define dftAppTenor18 "TENOR 18"
#define dftAppTenor24 "TENOR 24"
#define dftApptpduHead "\x60"
#define dftApptpduModem "\x30\x00"
#define dftApptpduTCPIP "\x30\x00"
#define dftApptpduRS232 "\x06\x65"
#define dftEmvTrnCurCod "\x02\x03\x60"
#define dftEmvTrnCurExp "\x01\x03"
#define dftEmvMrcCatCod "\x02\x53\x11"
#define dftEmvTrmCntCod "\x02\x03\x60"
#define dftEmvTrmId "\x08""00000211"
#define dftEmvTrmCap "\x03\xE0\xB8\xC8"
#define dftEmvTrmTyp "\x01\x22"
#define dftEmvAddTrmCap "\x05\xF0\x00\xF0\xA0\x01"
#define dftEmvVlpSupInd "\x01\x00"
#define dftEmvVlpTrmLim "\x06\x00\x00\x00\x00\x00\x00"
#define dftEmvLstRspCod "\x14""Y1Z1Y2Z2Y3Z300050104"
#define dftEmvIFDSerNum "\x08""28725422"
#define dftEmvTRMOverpassAIP "\x01\x01"
#define dftAppServiceCode 1
#define dftAppFallBackMod 0
#define dftAppPPPNbr __DFTPPP__
#define dftAppBfKey "3333333333333333"
#define dftAppBlowFish 1
#define dftAppAftTMSKey "\x03\x03\x03\x01\x03\x03\x03\x01"
#define dftAppICT220 0
#define dftAppSettleInd 0
#define dftAppBf "0"
#define dftAppPANBlockBegin 12
#define dftAppPANBlockEnd 0
#define dftAppCurrSignSpaces "Rp.  "
#define dftAppPassTel "1210"
#define dftAppLockFlag 0
#define dftAppAfterTMS 0
#define dftAppGetTick 0
#define dftAppTmsRefNum "00000000"
#define dftAppVersion __SoftwareVersion__
#define dftAppSoftName __SoftwareName__
#define dftAppLtmkAcqID "001"
#define dftAppVendorID "00000001"
#define dftAppLTMKDATA "12345678123456781234567812345678123456781234567812345678"
#define dftAppModulusRSA "12345678123456781234567812345678123456781234567812345678"
#define dftAppKeySessionTle "2013022811262013"
#define dftAppNiiLTMK "018"
#define dftAppKMSLTMKDATA "12345678123456781234567812345678123456781234567812345678"
#define dftAppLTMK "F2B10A6B16E17D89D6C1D51F2322B5568B193CACA99800CF"
#define dftAppLTWKTID1 "0000"
#define dftAppTidTle "93132833"
#define dftAppKeySessionSAM ""
#define dftAppTleSet 2
#define dftAppBit58 "0000000000000000"
#define dftAppTxnVoidBfr 0
#define dftAppIdxBtcUpl 0
#define dftAppNbrTxnVoidBfr 0
#define dftAppCntReceipt 2
#define dftAppNIIMntr "110"
#define dftAppto "0000"
#define dftApptoidle "0000"
#define dftApptoTFHr "0000"
#define dftAppPriMonSrv __DFTMONSRV__
#define dftAppPriMonAtt 2
#define dftAppSecMonSrv __DFTMONSRV__
#define dftAppSecMonAtt 2
#define dftAppLastAutCodPrepaid "000000"
#define dftAppLastTCCode "0000000000"
#define dftAppTerminalType "EFT930"
#define dftAppDateMonitoring "000000"
#define dftAppFlagEDCMon 1
#define dftAppDateReInitRetryEDCMon "000000"
#define dftAppTimeEdcMon 0
#define dftAppFlagDownloadLTWK 0
#define dftAppFlagInit 0
#define dftAppCtrlVersion __CtrlSoftwareVersion__

#define __DFT_H
#endif

