/** \file
 * EMV tags definitions
 *
 * Subversion reference
 *
 * $HeadURL: https://svn.ingenico.com/UTA/core/utaYYMMDD/sys/emv.h $
 *
 * $Id: emv.h 2616 2010-04-07 11:16:41Z abarantsev $
 */

#ifndef __EMV_H

#ifdef __EMV__
#define __OWNTAGPRE__ 0xDF
#define __FSBUFLEN__ 262

#ifdef __APR__
#undef __OWNTAGPRE__
#define __OWNTAGPRE__ 0x1F
#endif

#endif

#ifdef __EMV__
enum eTag {                     //EMV tag values
    tagBeg,
    tagAid = 0x4F,              //Application Identifier    
    tagAppLbl = 0x50,           //Application Label
    tagCmdPfm = 0x52,           //Command to Perform
    tagTrk2 = 0x57,             //Track 2 Equivalent Data
    tagPAN = 0x5A,              //Application Primary Account Number (PAN)
    tagChdNam = 0x5F20,         //Cardholder Name
    tagExpDat = 0x5F24,         //Application Expiration Date
    tagEffDat = 0x5F25,         //Application Effective Date
    tagIssCntCod = 0x5F28,      //Issuer Country Code
    tagTrnCurCod = 0x5F2A,      //Transaction Currency Code
    tagLanPrf = 0x5F2D,         //Language Preference
    tagSrvCod = 0x5F30,         //Service Code
    tagPANSeq = 0x5F34,         //Application Primary Account Number (PAN) Sequence Number
    tagTrnCurExp = 0x5F36,      //Transaction Currency Exponent
    tagAplTpl = 0x61,           //Application Template
    tagFCITpl = 0x6F,           //File Control Information (FCI) Template
    tagAEFDtaTpl = 0x70,        //Application Elementary File (AEF) Data Template
    tagIssSc1 = 0x71,           //Issuer Script Template 1
    tagIssSc2 = 0x72,           //Issuer Script Template 2
    tagDirDscTpl = 0x73,        //Directory Discretionary Template
    tagRspMsgTpl2 = 0x77,       //Response Message Template Format 2
    tagRspMsgTpl1 = 0x80,       //Response Message Template Format 1
    tagAmtBin = 0x81,           //Amount, Authorised (Binary)
    tagAIP = 0x82,              //Application Interchange Profile
    tagCmdTpl = 0x83,           //Command Template
    tagDFNam = 0x84,            //Dedicated File (DF) Name
    tagIssScrCmd = 0x86,        //Issuer Script Command
    tagAplPrtInd = 0x87,        //Application Priority Indicator
    tagSFI = 0x88,              //Short File Identifier (SFI)
    tagAutCod = 0x89,           //Authorisation Code
    tagRspCod = 0x8A,           //Authorisation Response Code
    tagCDOL1 = 0x8C,            //Card Risk Management Data Object List 1 (CDOL1)
    tagCDOL2 = 0x8D,            //Card Risk Management Data Object List 2 (CDOL2)
    tagCVM = 0x8E,              //Cardholder Verification Method (CVM) List
    tagCapkIdx = 0x8F,          //Certificate Authority Public Key Index ICC
    tagPubKeyCrt = 0x90,        //Issuer Public Key Certificate
    tagIssAutDta = 0x91,        //Issuer Authentication Data
    tagIssPubRem = 0x92,        //Issuer Public Key Remainder
    tagSgnStaApl = 0x93,        //Signed Static Application Data
    tagAFL = 0x94,              //Application File Locator (AFL)
    tagTVR = 0x95,              //Terminal Verification Results
    tagTDOL = 0x97,             //Transaction Certificate Data Object List (TDOL)
    tagTCHshVal = 0x98,         //Transaction Certificate (TC) Hash Value 
    tagTrnPINDta = 0x99,        //Transaction Personal Identification Number (PIN) Data
    tagTrnDat = 0x9A,           //Transaction Date
    tagTSI = 0x9B,              //Transaction Status Information
    tagTrnTyp = 0x9C,           //Transaction Type
    tagDDFNam = 0x9D,           //Directory Definition File (DDF) Name
    tagAcqId = 0x9F01,          //Acquirer Identifier (BIN)
    tagAmtNum = 0x9F02,         //Amount, Authorised (Numeric)
    tagAmtOthNum = 0x9F03,      //Amount, Other (Numeric)
    tagAmtOthBin = 0x9F04,      //Amount, Other (Binary)
    tagAplDscDta = 0x9F05,      //Application Discretionary Data
    tagAIDT = 0x9F06,           //Application Identifier (AID) Terminal
    tagAplUsgCtl = 0x9F07,      //Application Usage Control
    tagCrdAvn = 0x9F08,         //Application Version Number CCard
    tagTrmAvn = 0x9F09,         //Application Version Number Terminal
    tagCrdNamExt = 0x9F0B,      //Cardholder Name Extended
    tagIACDft = 0x9F0D,         //Issuer Action Code Default
    tagIACDnl = 0x9F0E,         //Issuer Action Code Denial
    tagIACOnl = 0x9F0F,         //Issuer Action Code Online
    tagIAD = 0x9F10,            //Issuer Application Data
    tagIssCodIdx = 0x9F11,      //Issuer Code Table Index
    tagAppPrfNam = 0x9F12,      //Application Preferred Name
    tagLstOnlATC = 0x9F13,      //Last Online Application Transaction Counter (ATC Register)
    tagLowConOfl = 0x9F14,      //Lower Consecutive Offline Limit
    tagMrcCatCod = 0x9F15,      //Merchant Category Code
    tagMrcID = 0x9F16,          //Merchant Identifier
    tagPINTryCnt = 0x9F17,      //Personal Identification Number (PIN) Try Counter
    tagIssScrID = 0x9F18,       //Issuer Script Identifier
    tagAccCntCod = 0x9F1A,      //Card Acceptor Country Code
    tagTrmFlrLim = 0x9F1B,      //Terminal Floor Limit
    tagTrmId = 0x9F1C,          //Terminal Identification
    tagTrmRskMng = 0x9F1D,      //Terminal Risk Management Data
    tagIFDSerNum = 0x9F1E,      //Interface Device (IFD) Serial Number
    tagTr1DscDta = 0x9F1F,      //Track 1 Discretionary Data
    tagTr2DscDta = 0x9F20,      //Track 2 Discretionary Data
    tagTrnTim = 0x9F21,         //Transaction Time
    tagCrtAutPubT = 0x9F22,     //Certification Authority Public Key Index Terminal
    tagUppConOfl = 0x9F23,      //Upper Consecutive Offline Limit
    tagIssTrnCrt = 0x9F26,      //Issuer Transaction Certificate
    tagCID = 0x9F27,            //Cryptogram Information Data (CID)
    tagICCPINCrt = 0x9F2D,      //ICC PIN Encipherment Public Key Certificate
    tagICCPINExp = 0x9F2E,      //ICC PIN Encipherment Public Key Exponent
    tagICCPINRmd = 0x9F2F,      //ICC PIN Encipherment Public Key Remainder
    tagIssPubExp = 0x9F32,      //Issuer Public Key Exponent
    tagTrmCap = 0x9F33,         //Terminal Capabilities
    tagCVMRes = 0x9F34,         //Cardholder Verification Method (CVM) Results
    tagTrmTyp = 0x9F35,         //Terminal Type
    tagATC = 0x9F36,            //Application Transaction Counter (ATC)
    tagUnpNum = 0x9F37,         //Unpredictable Number
    tagPDOL = 0x9F38,           //Processing Options Data Object List (PDOL)
    tagPOSEntMod = 0x9F39,      //PointOfService (POS) Entry Mode
    tagAmtRefCur = 0x9F3A,      //Amount, Reference Currency
    tagAplRefCur = 0x9F3B,      //Application Reference Currency
    tagTrnRefCur = 0x9F3C,      //Transaction Reference Currency
    tagTrnRefExp = 0x9F3D,      //Transaction Reference Currency Exponent
    tagAddTrmCap = 0x9F40,      //Additional Terminal Capabilities
    tagTrnSeqCnt = 0x9F41,      //Transaction Sequence Counter
    tagAplCurCod = 0x9F42,      //Application Currency Code
    tagAplRefExp = 0x9F43,      //Application Reference Currency Exponent
    tagAplCurExp = 0x9F44,      //Application Currency Exponent
    tagDtaAutCod = 0x9F45,      //Data Authentication Code
    tagICCPubCrt = 0x9F46,      //ICC Public Key Certificate
    tagICCPubExp = 0x9F47,      //ICC Public Key Exponent
    tagICCPubRmd = 0x9F48,      //ICC Public Key Remainder
    tagDDOL = 0x9F49,           //Dynamic Data Object List (DDOL)
    tagSDALst = 0x9F4A,         //Static Data Authentication Tag List
    tagSgnDynApl = 0x9F4B,      //Signed Dynamic Application Data
    tagICCDynNbr = 0x9F4C,      //ICC Dynamic Number
    tagAplCurCodV = 0x9F51,     //VISA Application Currency Code
    tagADA = 0x9F52,            //Application Default Action (ADA)
    // tagCnsTrnLimV = 0x9F53,     //VISA Consecutive Transaction Limit (international)
    tagTCC = 0x9F53,            //MC Transaction Category Code
    tagCumTotLimV = 0x9F54,     //VISA Cumulative Total Transaction Amount Limit
    tagGeoInfV = 0x9F55,        //VISA Geographic Indicator
    tagIssAutInd = 0x9F56,      //Issuer Authentication Indicator
    tagIssCntCodV = 0x9F57,     //VISA Issuer CountryCode
    tagLowConOflV = 0x9F58,     //VISA Lower Consecutive Offline Limit
    tagUppConOflV = 0x9F59,     //VISA Upper Consecutive Offline Limit
    tagCrdAccNamM = 0x9F5A,     //Card Acceptor Name (MCard)
    tagCrdAccNamT = 0x9F5B,     //Card Acceptor Name (Terminal)
    tagCrdTrcNum = 0x9F5C,      //Ccard Trace Number
    tagMrcTrcNum = 0x9F5D,      //Mcard Trace Number
    tagSpnPwr = 0x9F5E,         //Spending Power
    tagTrnCod = 0x9F5F,         //Transaction Code
    tagMrcPrcCry = 0x9F63,      //Mcard Purchase Cryptogram
    tagVlpSupInd = 0x9F7A,      //VLP Terminal Support Indicator
    tagVlpTrmLim = 0x9F7B,      //VLP Terminal Transaction Limit
    tagOnlSta = WORDHL(__OWNTAGPRE__, 0x39),    //Online process status
#ifdef __TELIUM_PARAM__
    tagLstCmdRsp = 0x9F8102, //TAG_LAST_APDU_CMD_RESPONSE
    tagTACDft = 0x9F8128,       //Terminal Action Code Default
    tagTACDen = 0x9F8129,       //Terminal Action Code Denial
    tagTACOnl = 0x9F812A,       //Terminal Action Code Online
    tagThrVal = 0x9F812B,       //Threshold value
    tagTarPer = 0x9F8127,       //Target percentage
    tagMaxTarPer = 0x9F8126,    //Maximum target percentage
    tagScrRes = 0x9F812D,       //result of script execution
    tagDftValDDOL = 0x9F8124,   //Default value for DDOL
    tagDftValTDOL = 0x9F8125,   //Default value for TDOL
    tagFrcOnl = 0x9F8414,       //Flag transaction forced online TAG_CUST_FORCED_ONLINE_REQUEST     
    tagRSAKey = 0x9F8123,       //RSA key
    tagAmtPrv = 0x9F8112,       /**< Indicates the cumulative transaction amount for the current card */
    tagHotLst = 0x9F841B,       //Flag set to TRUE if the card is in Black List
    tagEMVDCOpt = 0x9f8111,     // * @brief Special options to be applied to the transaction.
    tagTaaResult = 0x9f8119,	//mcy add for Request 1st Gen AC
    tagRSAKeyExp = 0x9F8122,    //RSA key exponent
    tagSchemasKSU = 0x9F813A,
    tagPinPadOK = 0x9F813B,     /**< Boolean on one Byte. True if Pinpad is functioning */
    tagCVMOutRes = WORDHL(__OWNTAGPRE__, 0x38), //CVMOUT Result
    tagAutoMate = 0x9F8132,     //Result of processing of CVM on the application side (i.e : result of pin entry)
    tagStatusCVP = 0x9F8133,    // Current Cardholder Verication Method to be processed
    tagPinCode = 0x9F8134,      /**< Pin value (Fill in with dummy bytes, as Pin value is no more accessible */
    tagRemainPin = 0x9F8135,    //AG_REMAINING_PIN                                
    tagSignature = 0x9F8136,    // Signature
    tagBypassPin = 0x9F8139,    //Pin Bypass True (1) if bypass required - 1 byte
    tagApduResp = 0x9F8197, //mcy add for 2nd Gen AC
    tagResPropMeth = 0x9F8147,  // TAG_RESULT_PROPRIETARY_METHOD
    tagTRMOverpassAIP = 0x9F81A3,   //TAG_TRM_OVERPASS_AIP    
#else
    tagTACDft = WORDHL(__OWNTAGPRE__, 0x03),    //Terminal Action Code Default
    tagTACDen = WORDHL(__OWNTAGPRE__, 0x04),    //Terminal Action Code Denial
    tagTACOnl = WORDHL(__OWNTAGPRE__, 0x05),    //Terminal Action Code Online
    tagThrVal = WORDHL(__OWNTAGPRE__, 0x07),    //Threshold value
    tagTarPer = WORDHL(__OWNTAGPRE__, 0x08),    //Target percentage
    tagMaxTarPer = WORDHL(__OWNTAGPRE__, 0x09), //Maximum target percentage
    tagScrRes = WORDHL(__OWNTAGPRE__, 0x11),    //result of script execution
    tagDftValDDOL = WORDHL(__OWNTAGPRE__, 0x15),    //Default value for DDOL
    tagDftValTDOL = WORDHL(__OWNTAGPRE__, 0x18),    //Default value for TDOL
    tagFrcOnl = WORDHL(__OWNTAGPRE__, 0x1C),    //Flag transaction forced online
    tagRSAKey = WORDHL(__OWNTAGPRE__, 0x1F),    //RSA key
    tagAmtPrv = WORDHL(__OWNTAGPRE__, 0x25),    //Amount found in the transaction file, in a previous transaction performed with the same card
    tagHotLst = WORDHL(__OWNTAGPRE__, 0x26),    //Flag set to TRUE if the card is in Opposition List
    tagCVMOutRes = WORDHL(__OWNTAGPRE__, 0x38), //CVMOUT Result
    tagRSAKeyExp = WORDHL(__OWNTAGPRE__, 0x7F), //RSA key exponent
    tagDftValONL = WORDHL(__OWNTAGPRE__, 0x19), //Online DOL
    tagDftValAMT = WORDHL(__OWNTAGPRE__, 0x1A), //Amount DOL
    tagDftValRSP = WORDHL(__OWNTAGPRE__, 0x1B), //Responce DOL
    tagTraLst = WORDHL(__OWNTAGPRE__, 0x3A),    //Last Tra
    tagDebugInfo = WORDHL(__OWNTAGPRE__, 0x42), //Debug Information
    tagTRMOverpassAIP = WORDHL(__OWNTAGPRE__, 0x0B),    //Debug Information
#endif
    tagODA = 0x9F81A2,          //Offline Data Authentication
    tagCDANevReqARQ1 = 0x9F81A7,    //TAG_CDA_NEVER_REQUEST_ARQC_1GENAC
    tagCDANevReqONL2 = 0x9F81A8,    //TAG_CDA_NEVER_REQUEST_ONLINE_2GENAC 
    tagDetFaitPriTAA = 0x9F81A6,    //TAG_DETECT_FAIL_PRIOR_TAA       
    tagAuthRes = 0x9f811A,      /**< Issuer decision on one byte : 0 declined, 1 accepted */
    tagCommOk = 0x9F8422,       /**< Boolean, In case of on-line authorisation, indicates if the communication succeeded. Length = 2 bytes */
    tagTrmDc1 = WORDHL(__OWNTAGPRE__, 0x29),    //Terminal decision for Generate AC (TC,AAC,ARQC)
    tagTrmDc2 = WORDHL(__OWNTAGPRE__, 0x31),    //Terminal decision after Generate AC 0=Declined, 1= Approved
    tagLstRspCod = WORDHL(__OWNTAGPRE__, 0x16), //List of possible response codes
    tagEnd
};

enum eTsi {                     //Transaction Status Information
    tsiBeg,
    tsiOflDtaAut,               //Offline data authentication was performed
    tsiChdVrf,                  //Cardholder verification was performed
    tsiCrdRskMng,               //Card risk management was performed
    tsiIssAut,                  //Issuer authentication was performed
    tsiTrmRskMng,               //Terminal risk management was performed
    tsiScrPrc,                  //Script processing was performed
    tsiEnd
};

enum eTvr {                     //terminal verification results bits
    tvrBeg,

    tvrOflDtaAut,               //Offline data authentication was not performed
    tvrOflSdtAut,               //Offline static data authentication failed
    tvrICCDtaMis,               //ICC data missing
    tvrCrdExcFil,               //Card appears on terminal exception file
    tvrOflDdtAut,               //Offline dynamic data authentication failed
    tvrRFU13,
    tvrRFU12,
    tvrRFU11,

    tvrICCTrmApl,               //ICC and terminal have different application versions
    tvrExpApl,                  //Expired application
    tvrAplNotEff,               //Application not yet effective
    tvrSrvNotAlw,               //Requested service not allowed for card product
    tvrNewCrd,                  //New card
    tvrMrcNotOpr,               //Mcard not operative
    tvrRFU22,
    tvrRFU21,

    tvrCrdVrf,                  //Cardholder verification was not successful
    tvrUnrCVM,                  //Unrecognised CVM
    tvrPINTryLim,               //PIN Try Limit exceeded
    tvrPPNotPrs,                //PIN entry required and PIN pad not present or not working
    tvrPINNotEnt,               //PIN entry required, PIN pad present, but PIN was not entered
    tvrOnlPINEnt,               //Online PIN entered
    tvrRFU32,
    tvrRFU31,

    tvrFlrLimExc,               //Transaction exceeds floor limit
    tvrLwrOflLim,               //Lower consecutive offline limit exceeded
    tvrUprOflLim,               //Upper consecutive offline limit exceeded
    tvrSelRndOnl,               //Transaction selected randomly for online processing
    tvrTrnFrcOnl,               //Merchant forced transaction online
    tvrRFU43,
    tvrRFU42,
    tvrRFU41,

    tvrDftTDOL,                 //Default TDOL used
    tvrIssAut,                  //Issuer authentication was unsuccessful
    tvrScrBfr,                  //Script processing failed before final GENERATE AC
    tvrScrAft,                  //Script processing failed after final GENERATE AC
    tvrRFU54,
    tvrRFU53,
    tvrRFU52,
    tvrRFU51,

    tvrEnd
};

enum eAuc {                     //application usage control bits
    aucBeg,

    aucDomCsh,                  //Valid for domestic cash transactions
    aucIntCsh,                  //Valid for international cash transactions
    aucDomGds,                  //Valid for domestic goods
    aucIntGds,                  //Valid for international goods
    aucDomSrv,                  //Valid for domestic services
    aucIntSrv,                  //Valid for international services
    aucATM,                     //Valid at ATMs
    aucNonATM,                  //Valid at terminals other than ATMs

    aucDomCbk,                  //Domestic cashback allowed
    aucIntCbk,                  //International cashback allowed
    aucRFU26,
    aucRFU25,
    aucRFU24,
    aucRFU23,
    aucRFU22,
    aucRFU21,

    aucEnd
};

enum eAip {                     //application interchange profile bits
    aipBeg,

    aipIni,                     //Initiate
    aipOflStaAut,               //Offline static data authentication is supported
    aipOflDynAut,               //Offline dynamic data authentication is supported
    aipChdVrf,                  //Cardholder verification is supported
    aipTrmRskMng,               //Terminal risk management is to be performed
    aipIssAut,                  //Issuer authentication is supported
    aipRFU12,
    aipRFU11,

    aipRFU28,
    aipRFU27,
    aipRFU26,
    aipRFU25,
    aipRFU24,
    aipRFU23,
    aipRFU22,
    aipRFU21,

    aipEnd
};

enum eTcp {                     //terminal capabilities bits
    tcpBeg,

    tcpManKeyEnt,               //Manual Key Entry
    tcpMagStr,                  //Magnetic Stripe
    tcpICC,                     //IC with contacts
    tcpRFU15,
    tcpRFU14,
    tcpRFU13,
    tcpRFU12,
    tcpRFU11,

    tcpPlnPinICC,               //Plaintext PIN for ICC verification
    tcpEncPinOnl,               //Enciphered PIN for online verification
    tcpSigPpr,                  //Signature (paper)
    tcpEncPinOff,               //Enciphered PIN for offline verification
    tcpRFU24,
    tcpRFU23,
    tcpRFU22,
    tcpRFU21,

    tcpSDA,                     //Static data authentication
    tcpDDA,                     //Dynamic data authentication
    tcpCrdCap,                  //Card capture
    tcpRFU35,
    tcpRFU34,
    tcpRFU33,
    tcpRFU32,
    tcpRFU31,

    tcpEnd
};

enum eAcp {                     //additional terminal capabilities bits
    acpBeg,

    acpCsh,                     //Cash
    acpGds,                     //Goods
    acpSrv,                     //Services
    acpCbk,                     //Cashback
    acpInq,                     //Inquiry
    acpTrf,                     //Transfer
    acpPmt,                     //Payment
    acpAdm,                     //Administrative

    acpRFU28,
    acpRFU27,
    acpRFU26,
    acpRFU25,
    acpRFU24,
    acpRFU23,
    acpRFU22,
    acpRFU21,

    acpNumKey,                  //Numeric keys
    acpChrKey,                  //Alphabetic and special characters keys
    acpCmdKey,                  //Command keys
    acpFunKey,                  //Function keys
    acpRFU34,
    acpRFU33,
    acpRFU32,
    acpRFU31,

    acpPrtAtt,                  //Print, attendant
    acpPrtChd,                  //Print, cardholder
    acpDspAtt,                  //Display, attendant
    acpDspChd,                  //Display, cardholder
    acpRFU44,
    acpRFU43,
    acpCT0,                     //Code table 10
    acpCT9,                     //Code table 9

    acpCT8,                     //Code table 8
    acpCT7,                     //Code table 7
    acpCT6,                     //Code table 6
    acpCT5,                     //Code table 5
    acpCT4,                     //Code table 4
    acpCT3,                     //Code table 3
    acpCT2,                     //Code table 2
    acpCT1,                     //Code table 1

    acpEnd
};

enum eCvm {                     //CVMOUT result bits
    cvmBeg,

    cvmPinRevocationPossible,
    cvmOfflinePinOK,
    cvmLastAttempt,
    cvmWrongPin,
    cvmOfflinePin,
    cvmOnlinePin,
    cvmSignature,
    cvmEnding,

    cvmRFU28,
    cvmRFU27,
    cvmRFU26,
    cvmRFU25,
    cvmRFU24,
    cvmRFU23,
    cvmRFU22,
    cvmProprietaryCVM,

    cvmTelium,                  ///<Telium specific follows

    cvmNO_REMAINING_PIN = 52,   ///< PIN Try limit Exceeded
    cvmINPUT_PIN_OFF = 53,      ///< Perform PIN Entry for offline verification
    cvmINPUT_PIN_ON = 54,       ///< Perform PIN Entry for online verification
    cvmINPUT_PIN_OK = 55,       ///< PIN entry performed
    cvmNO_INPUT = 56,           ///< CVM End
    cvmPP_HS = 60,              ///< Pinpad Out of Services
    cvmCANCEL_INPUT = 61,       ///< Pin entry cancelled
    cvmTO_INPUT = 62,           ///< Time Out during PIN Entry
    cvmREMOVED_CARD = 63,       ///< Card Removed
    cvmERROR_CARD = 64,         ///< Card Error
    cvmMORE = 65,               ///< Internal use only
    cvmERROR_ENC_DATA = 66,     ///< Error during PIN Encpherment

    cvmEnd
};

#ifdef __CLS__
enum eCMVTag {
    cmvTagBeg,
    cmvTagAppType = 0x9F928201, /*!< Indicates the application type which provided the AID for Entry Point. */
    cmvTagAidPropId = 0x9F928202,   /*!< Indicates the proprietary identifier of a given AID (to differentiate an AID configuration according to specific parameters). */
    cmvTagDataCallMod = 0x9F928203, /*!< Indicates the way to communicate with the application. See \ref ClessGenericTagsCallingMode. */
    cmvTagErrInfo = 0x9F928204, /*!< Contains the error codes information about the processing. */
    cmvTagAppMethod = 0x9F928205,   /*!< Indicates the applied application selection method. See \ref ClessGenericTagsSelectionMethod. */
    cmvTagDetectType = 0x9F928210,  /*!< DETECTION - Card detection type. See \ref ClessGenericTagsDetectionType. */
    cmvTagGlobalTO = 0x9F928212,    /*!< DETECTION - Card detection global timeout. */
    cmvTagLocalTO = 0x9F928213, /*!< DETECTION - Card detection local timeout. */
    cmvTagDetectNBtoDetect = 0x9F928214,    /*!< DETECTION - Maximum number of card that can be detected. */
    cmvTagDetectResult = 0x9F928215,    /*!< DETECTION - Card detection result. See \ref ClessGenericTagsDetectionResults. */
    cmvTagDetectCardType = 0x9F928217,  /*!< DETECTION - Indicates the type of the detected card (ISO, Mifare, Felica, STM or Unknown). */
    cmvTagLvl1Error = 0x9F928219,   /*!< DETECTION - Level 1 error code. */
    cmvTagAppSelectResult = 0x9F92821A, /*!< Application Selection - Indicates the Application selection result (used for application selection post processing). */
    cmvTagApduCmdtoSend = 0x9F928220,   /*!< APDU METHOD - Command to be sent to the contactless card. */
    cmvTagApduExpectedStatusWord = 0x9F928221,  /*!< APDU METHOD - Expected status word from the contactless card to the sent command. */
    cmvTagApduExpectedCardRsp = 0x9F928222, /*!< APDU METHOD - Expected response from the contactless card to the send command. */
    cmvTagApduRspMask = 0x9F928223, /*!< APDU METHOD - Mask applied on the expected response (if only partial response only matches). */

    /*! \brief APDU METHOD - APDU options :
       - Byte 1 :
       - Bit 1 : APDU Before (0) / After (1)
       - Bit 2 : Proprietary application selection allowed or not (currently not used).
       - Bits 3 to 8 : RFU.
       - Byte 2 : Priority Level.
       - Bytes 3 to 4 : RFU.
     */
    cmvTagApduOptions = 0x9F928224,
    cmvTagApduCardRsp = 0x9F928225, /*!< APDU METHOD - Card response according to the sent APDU. */
    cmvTagApduStatusWord = 0x9F928226,  /*!< APDU METHOD - Card response status word according to the sent command. */
    cmvTagApduGUIMode = 0x9F928230, /*!< DETECTION - GUI mode. */
    cmvTagCustSteptoCust = 0x9F928231,  /*!< CUSTOMISATION PROCESSING - Bit field (on 8 bytes) indicating which steps can be customised. */
    cmvTagCustStepId = 0x9F928232,  /*!< CUSTOMISATION PROCESSING - Bit field containing the step to customise. */
    cmvTagMgrAmt = 0x9F928240,  /*!< MANAGER - Tag indicating if the amount has been entered or not (extracted from S_TRANSIN). */
    cmvTagMgrTxnType = 0x9F928241,  /*!< MANAGER - Tag indicating the transaction type (extracted from S_TRANSIN). */
    cmvTagMgrCurLvl = 0x9F928242,   /*!< MANAGER - Tag indicating the currency label (extracted from S_TRANSIN). */
    cmvTagMgrCashRegNum = 0x9F928243,   /*!< MANAGER - Tag indicating the cash register number (extracted from S_TRANSIN). */
    cmvTagApduInfo = 0xBF928200,    /*!< APDU METHOD - APDU structure (After or before). Refer to \ref GenericParametersFormat for more information about the structure content. */
    cmvTagCardInfo = 0xBF928201,    /*!< DETECTION - Detected card information. */
    cmvTagClessAppCapability = 0x9F28,  /*!< ENTRY POINT - Contactless Application Capabilities Type.<br>Identifies the capabilities of the application to the Terminal, for instance whether the application supports the EMV Contactless Application.<br> - Format : b.<br>   - Length : 3-8 bytes.<br>   - Source : Card. */
    cmvTagTrmTxnQualifier = 0x9F66, /*!< ENTRY POINT - Terminal Transaction Capabilities (Visa kernel only).<br>Indicates the Terminal requirements for online and CVM processing. Only applicable to the Visa kernel.<br>  - Format : b.<br>   - Length : 4 bytes.<br> - Source : Terminal. */
    cmvTagAidOpt = 0x9F928100,  /*!< ENTRY POINT - Indicates the AID options :<br>  - Byte 1 :<br>      - Bit 1 : Indicates if partial AID is supported or not (not used by Entry Point).<br>       - Bit 2 : Indicates if status check supported or not.<br>       - Bit 3 : Indicates if zero amount is allowed or not.<br>       - Bits 4 to 8 : RFU (shall be set to 0).<br>    - Byte 2 :<br>      - Bit 1 : Indicates if Entry Point method is allowed or not for this AID.<br>       - Bit 2 : Indicates if the "List Of AID" is allowed or not for this AID.<br>        - Bits 3 to 8 : RFU (shall be set to 0).<br>    - Bytes 3 to 4 : RFU (shall be set to 0). */
    cmvTagKernelToUse = 0x9F928101, /*!< ENTRY POINT - Indicates the default kernel to use for a given AID. Please refer to \ref EntryPointPossibleKernels for possible values.<br> - Format : b.<br>   - Length : 2 bytes.<br> - Source : Terminal. */
    cmvTagAidAddResults = 0x9F928104,   /*!< ENTRY POINT - Contains the pre processing additional results for an AID (Exceeded floor limit, etc). Please refer to \ref T_PRE_PROCESSING_ADD_RESULT_AID structure for more information.<br>  - Format : b.<br>   - Length : 4 bytes.<br> - Source : Terminal. */
    cmvTagPPSECardRsp = 0x9F928105, /*!< ENTRY POINT - PPSE card response.<br>  - Format : b.<br>   - Length : var. up to 257 bytes.<br>    - Source : Terminal. */
    cmvTagPPSEStatusWord = 0x9F928106,  /*!< ENTRY POINT - PPSE card response status word.<br>  - Format : b.<br>   - Length : 2 bytes.<br> - Source : Terminal. */
    cmvTagFinalSelectCmdSent = 0x9F928107,  /*!< ENTRY POINT - Final SELECT command sent to the card.<br>   - Format : b.<br>   - Length : var. up to 261 bytes.<br>    - Source : Terminal. */
    cmvTagFinalSelectRsp = 0x9F928108,  /*!< ENTRY POINT - Final SELECT card response.<br>  - Format : b.<br>   - Length : var. up to 257 bytes.<br>    - Source : Terminal. */
    cmvTagFinalSelectStatusWord = 0x9F928109,   /*!< ENTRY POINT - Final SELECT card response Status Word.<br>  - Format : b.<br>   - Length : 2 bytes.<br> - Source : Terminal. */
    cmvTagNumAllowedAid = 0x9F92810A,   /*!< ENTRY POINT - Number of allowed AID (according to the pre Entry Point processing results).<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    cmvTagUnpNum = 0x9F92810C,  /*!< ENTRY POINT - Unpredictable number (generated by a scheme within Entry Point).<br> - Format : b.<br>   - Length : 4 bytes.<br> - Source : Terminal. */
    cmvTagClessTxnLim = 0x9F92810D, /*!< ENTRY POINT - Indicates the limit for which contactless transaction can be conducted in the terminal relating to the AID.<br>  - Format : n.<br>   - Length : 6 bytes.<br> - Source : Terminal. */
    cmvTagClessCvmReqdLimit = 0x9F92810E,   /*!< ENTRY POINT - Indicates the contactless floor limit from which a CVM is required relating to an AID.<br>   - Format : n.<br>   - Length : 6 bytes.<br> - Source : Terminal. */
    cmvTagClessFlrLim = 0x9F92810F, /*!< ENTRY POINT - Indicates the maximum transaction amount relating to the AID.<br>    - Format : n.<br>   - Length : 6 bytes.<br> - Source : Terminal. */
    cmvTagNumCandidateAid = 0x9F928110, /*!< ENTRY POINT - Indicates the number */
    cmvTagAidPreProcRes = 0xBF928101,   /*!< ENTRY POINT - Constructed tag containing AID pre processing results (AID, Default kernel, proprietary data, application type, copy of TTQ and additional results).<br> - Format : b.<br>   - Length : var. up to 80 bytes.<br> - Source : Terminal. */
    cmvTagAidInfo = 0xBF928102, /*!< ENTRY POINT - Constructed tag containing all the AID information.<br>  - Format : b.<br>   - Length : var. up to 256 bytes.<br>    - Source : Terminal. */
    cmvTagCandidateListElement = 0xBF928103,    /*!< ENTRY POINT - Constructed tag containing a candidate list element.<br> - Format : b.<br>   - Length : var. up to 512 bytes.<br>    - Source : Terminal. */
    //! @}
    cmvTagEnd
};

enum eCMVCallMode {
    cmvcallmodeBeg,
    cmvcallmodenone = 0,        /*!< Way to communicate is not yet determined. */
    cmvcallmodedel = 1,         /*!< Way to communicate is DEL. */
    cmvcallmodesharedtlv = 2,   /*!< Way to communicate is a buffer in shared memory providing the buffer address only. */
    cmvcallmodetlv = 3,         /*!< Way to communicate is a full buffer containing all the data (for simulation only). */
    cmvcallmodend
};

enum eCMVSelectionMethod {
    cmvSelectBeg,
    cmvSelectApduBefore = 1,    /*!< Application Selection method - APDU Before method. */
    cmvSelectPPSE = 2,          /*!< Application Selection method - PPSE method. */
    cmvSelectLOA = 3,           /*!< Application Selection method - List Of AID method. */
    cmvSelectApduAfter = 4,     /*!< Application Selection method - APDU After method. */
    cmvSelectProprietray = 5,   /*!< DO NOT USE - Application Selection method - Proprietary method. */
    cmvSelectEnd
};

enum eCMVDetectionType {
    cmvDetectBeg,
    cmvDetectIsoA = 0x01,       /*!< DETECTION - Byte for bit indicating ISO 14443-4 Type A is supported (first byte). */
    cmvDetectIsoB = 0x02,       /*!< DETECTION - Byte for bit indicating ISO 14443-4 Type B is supported (first byte). */
    cmvDetectIsoBB = 0x04,      /*!< DETECTION - Byte for bit indicating ISO 14443-4 Type BB is supported (first byte). */
    cmvDetectMifareA = 0x08,    /*!< DETECTION - Byte for bit indicating ISO 14443 Mifare Type A is supported (first byte). */
    cmvDetectStmB = 0x10,       /*!< DETECTION - Byte for bit indicating ISO 14443 STM Type B is supported (first byte). */
    cmvDetectFelica = 0x20,     /*!< DETECTION - Byte for bit indicating Felica is supported (first byte). */
    cmvDetectMifareUltraLightA = 0x40,  /*!< DETECTION - Byte for bit indicating ISO 14443 Mifare Ultra Light Type A is supported (first byte). */
    cmvDetectEnd
};

enum eCMVDetectResults {
    cmvDetectResultBeg,
    cmvDetectResultOK = 0xF1,   /*!< DETECTION - Detection result indicating card correctly detected and activated. */
    cmvDetectResultCollision = 0xF2,    /*!< DETECTION - Detection result indicating card collision. */
    cmvDetectResultGlobalTO = 0xF3, /*!< DETECTION - Detection result indicating global timeout has been reached. */
    cmvDetectResultLocalTO = 0xF4,  /*!< DETECTION - Detection result indicating local timeout has been reached. */
    cmvDetectResultCancel = 0xF5,   /*!< DETECTION - Detection result indicating detection has been cancelled. */
    cmvDetectResultError = 0xF6,    /*!< DETECTION - Detection result indicating an error occurred during the card detection. */
    cmvDetectResultEnd
};

enum eCMVGUIMode {
    cmvGuiModeBeg,
    cmvGuiModeNone = (0),       /*!< Contactless GUI mode : No GUI. */
    cmvGuiModeMC = (1),         /*!< Contactless GUI mode : Standard MasterCard GUI mode. */
    cmvGuiVisaEuro = (2),       /*!< Contactless GUI mode : Standard Visa GUI mode. */
    cmvGuiVisaAsia = (3),       /*!< Contactless GUI mode : Standard Visa Asia GUI mode (with coloured LEDs). */
    cmvGuoMCColored = (4),      /*!< Contactless GUI mode : Standard MasterCard GUI mode (with coloured LEDs). */
    cmvGuiModeEnd
};

enum eCMVKernelID {
    cmvKernelIDBeg,
    cmvKernelIDJCB = 0x0001,    /*!< ENTRY POINT - Defines a JCB Kernel as indicated in the <i>Entry Point specification</i>. */
    cmvKernelIDPayPass = 0x0002,    /*!< ENTRY POINT - Defines a PayPass Kernel as indicated in the <i>Entry Point specification</i>. */
    cmvKernelIDVisa = 0x0003,   /*!< ENTRY POINT - Defines a Visa Kernel as indicated in the <i>Entry Point specification</i>. */
    cmvKernelIDAmex = 0x0004,   /*!< ENTRY POINT - Defines an American Express Kernel as indicated in the <i>Entry Point specification</i>. */
    cmvKernelIDEMVCo = 0x0101,  /*!< ENTRY POINT - Defines a EMVCo kernel. */
    cmvKernelIDUnknown = 0xFFFF,    /*!< ENTRY POINT - Unknown kernel. */
    cmvKernelIDEnd
};

enum eCMVKernelStatus {
    cmvKernelStatusBeg,
    cmvKernelStatusOK = 0x0100, /*!< The function is correctly executed. */
    cmvKernelStatusServiceNA = 0x0101,  /*!< The kernel to be accessed is not loaded in the terminal. */
    cmvKernelStatusDBError = 0x0102,    /*!< A database error occurred. */
    cmvKernelStatusInvalidInput = 0x0103,   /*!< One of the provided data is incorrect. */
    cmvKernelStatusNotSupported = 0x0104,   /*!< The called function is not supported by the kernel. */
    cmvKernelStatusLackofMem = 0x0105,  /*!< There is not enough memory to complete the transaction. */
    cmvKernelStatusCommError = 0x0106,  /*!< A communication error occurred with the contactless card. */
    cmvKernelStatusMissingInput = 0x0107,   /*!< A mandatory data is missing to perform the transaction. */
    cmvKernelStatusICCMissingData = 0x0108, /*!< A mandatory ICC data is missing to perform the transaction. */
    cmvKernelStatusICCInvalidData = 0x0109, /*!< A data returned by the card is not correctly formatted. */
    cmvKernelStatusICCRedundantData = 0x010A,   /*!< Card returned redundant data. */
    cmvKernelStatusICCDataFormatError = 0x010B, /*!< The card response is not correctly formatted (parsing error, empty response, etc). */
    cmvKernelStatusTrmMissingData = 0x010C, /*!< A mandatory terminal data is missing to perform the transaction. */
    cmvKernelStatusCardBlock = 0x010D,  /*!< Card is blocked and transaction cannot be performed. */
    cmvKernelStatusAppBlock = 0x010E,   /*!< The application in the card is blocked. */
    cmvKernelStatusRemoveAID = 0x010F,  /*!< The AID shall be removed from the candidate list. */
    cmvKernelStatusUnknownSW = 0x0110,  /*!< Unexpected response code from the ICC. */
    cmvKernelStatusUseCondNotSatisfy = 0x0111,  /*!< The ICC returned “Conditions of use not satisfied” in the GPO response. */
    cmvKernelStatusOfflineTC = 0x0112,  /*!< Transaction succeeded and is offline approved. */
    cmvKernelStatusOfflineAAC = 0x0113, /*!< Transaction is offline declined. */
    cmvKernelStatusARQC = 0x0114,   /*!< Transaction succeeded and needs and online authorisation to be completed. */
    cmvKernelStatusCancel = 0x0115, /*!< Transaction has been cancelled. */
    cmvKernelStatusUseContactInterface = 0x0116,    /*!< The transaction has to be conducted over another interface (chip or swipe for example). */
    cmvKernelStatusNotAllowed = 0x0117, /*!< This function cannot be called as no transaction is in process with this kernel or cancellation is not allowed according to the transaction progress. */
    cmvKernelStatusContinue = 0x0118,   /*!< The transaction flow must continue. */
    cmvKernelStatusSuspend = 0x0119,    /*!< The transaction flow must be stopped. */
    cmvKernelStatusInternalError = 0x011A,  /*!< A kernel internal error occured. */
    cmvKernelStatusLibInterfaceError = 0x011B,  /*!< An error occured into the kernel interface (linked to the custom application). */
    cmvKernelStatusExpiredCert = 0x011C,    /*!< The certificate used for Offline data authentication is expired. */
    cmvKernelStatusRevokedCert = 0x011D,    /*!< The certificate used for Offline data authentication is revoked. */
    cmvKernelStatusUnknown = 0x017F,    /*!< The status code is unknown (because not yet set in the database). */
    cmvKernelStatusEnd
};

enum cmvPayPassTags {
    tagPPassBeg,
    tagPPassTrack1Data = 0x56,  /*!< PAYPASS - Track 1 Data contains the data objects of the track 1 according to [ISO/IEC 7813] Structure B, excluding start sentinel, end sentinel and LRC.<br>   - Format : ans.<br> - Length : var. up to 76 bytes.<br> - Source : Card. */
    tagPPassTxnCatCod = 0x9F53, /*!< PAYPASS - This is a data object defined by MasterCard which indicates the type of transaction being performed, and which may be used in Card Risk Management.<br>  - Format : an.<br>  - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassCVC3Trk1 = 0x9F60,  /*!< PAYPASS - The CVC3TRACK1 is a 2-byte cryptogram returned by the card in the response to the COMPUTE CRYPTOGRAPHIC CHECKSUM command.<br>    - Format : b.<br>   - Length : 2 byte.<br>  - Source : Card. */
    tagPPassCVC3Trk2 = 0x9F61,  /*!< PAYPASS - The CVC3TRACK2 is a 2-byte cryptogram returned by the card in the response to the COMPUTE CRYPTOGRAPHIC CHECKSUM command.<br>    - Format : b.<br>   - Length : 2 bytes.<br> - Source : Card. */
    tagPPassPCVC3Trk3 = 0x9F62, /*!< PAYPASS - PCVC3TRACK1 indicates to the PayPass reader the positions in the discretionary data field of the Track 1 Data where the qTRACK1 CVC3TRACK1 digits have to be copied.<br> - Format : b.<br>   - Length : 6 bytes.<br> - Source : Card. */
    tagPPassPunATC_Trk1 = 0x9F63,   /*!< PAYPASS - PUNATCTRACK1 indicates to the PayPass reader the positions in the discretionary data field of the Track 1 Data where the nUN UN (Numeric) digits and tTRACK1 ATC digits have to be copied.<br>   - Format : b.<br>   - Length : 6 bytes.<br> - Source : Card. */
    tagPPassNATCTrk1 = 0x9F64,  /*!< PAYPASS - The value of NATCTRACK1 represents the number of digits of the ATC to be included in the discretionary data field of the Track 1 Data.<br>   - Format : b.<br>   - Length : 1 byte.<br>  - Source : Card. */
    tagPPassPCVC3Trk2 = 0x9F65, /*!< PAYPASS - PCVC3TRACK2 indicates to the PayPass reader the positions in the discretionary data field of the Track 2 Data where the qTRACK2 CVC3TRACK2 digits have to be copied.<br> - Format : b.<br>   - Length : 2 bytes.<br> - Source : Card. */
    tagPPassPunATCTrk2 = 0x9F66,    /*!< PAYPASS - PUNATCTRACK2 indicates to the PayPass reader the positions in the discretionary data field of the Track 2 Data where the nUN UN (Numeric) digits and tTRACK2 ATC digits have to be copied.<br>   - Format : b.<br>   - Length : 2 bytes.<br> - Source : Card. */
    tagPPassNATCTrk2 = 0x9F67,  /*!< PAYPASS - The value of NATCTRACK2 represents the number of digits of the ATC to be included in the discretionary data field of the Track 2 Data.<br>   - Format : b.<br>   - Length : 1 byte.<br>  - Source : Card. */
    tagPPassUDOL = 0x9F69,      /*!< PAYPASS - The UDOL is the DOL that specifies the data objects to be included in the data field of the COMPUTE CRYPTOGRAPHIC CHECKSUM command. The UDOL must at least include the UN (Numeric). The UDOL is not mandatory for the card. There will always be a Default UDOL, including as its only entry the tag and length of the UN (Numeric) (tag '9F6A').<br>   - Format : b.<br>   - Length : var.<br> - Source : Card. */
    tagPPassMagUnpNum = 0x9F6A, /*!< PAYPASS - Unpredictable number generated by the PayPass reader during a PayPass – Mag Stripe Transaction. The UN (Numeric) is passed to the card in the data field of the COMPUTE CRYPTOGRAPHIC CHECKSUM command. The (8-nUN) most significant digits must be set to zero.<br> - Format : n.<br>   - Length : 8 bytes.<br> - Source : Terminal. */
    tagPPassTrk2Dat = 0x9F6B,   /*!< PAYPASS - Track 2 Data contains the data objects of the track 2 according to [ISO/IEC 7813], excluding start sentinel, end sentinel and LRC.<br>   - Format : b.<br>   - Length : var. up to 19 bytes.<br> - Source : Card. */
    tagPPassMagAppVerNumICC = 0x9F6C,   /*!< PAYPASS - Version number assigned by the payment system for the specific PayPass – Mag Stripe functionality of the application (card value).<br>   - Format : b.<br>   - Length : 2 bytes.<br> - Source : Card. */
    tagPPassMagAppVerNumTrm = 0x9F6D,   /*!< PAYPASS - Version number assigned by the payment system for the specific PayPass – Mag Stripe functionality of the application (terminal value).<br>   - Format : b.<br>   - Length : 2 bytes.<br> - Source : Terminal. */
    tagPPassThirdParty = 0x9F6E,    /*!< PAYPASS - The PayPass Third Party Data contains proprietary information from a third party. If present, the PayPass Third Party Data must be present in a file read using the READ RECORD command.<br> - Format : b.<br>   - Length : 5-32 bytes.<br>  - Source : Card. */
    tagPPassDDCrdTrk1 = 0x9F918500, /*!< PAYPASS - If Track 1 Data is present, then DDCARD,TRACK1 contains a copy of the discretionary data field of Track 1 Data as returned by the card in the file read using the READ RECORD command during a PayPass – Mag Stripe transaction (i.e. without UN (Numeric), ATC, CVC3TRACK1 and nUN included).<br>   - Format : ans.<br> - Length : var. up to 56 bytes.<br> - Source : Terminal. */
    tagPPassDDCrdTrk2 = 0x9F918501, /*!< PAYPASS - DDCARD,TRACK2 contains a copy of the discretionary data field of Track 2 Data as returned by the card in the file read using the READ RECORD command during a PayPass – Mag Stripe transaction (i.e. without UN (Numeric), ATC, CVC3TRACK2 and nUN included).<br>    - Format : ans.<br> - Length : var. up to 8 bytes.<br>  - Source : Terminal. */
    tagPPassDftUDOL = 0x9F918502,   /*!< PAYPASS - The Default UDOL is the UDOL to be used for constructing the value field of the COMPUTE CRYPTOGRAPHIC CHECKSUM command if the UDOL in the card is not present. The Default UDOL must always be present and must contain as its only entry the tag and length of the UN (Numeric). The value of the Default UDOL must be: '9F6A04'.<br>   - Format : b.<br>   - Length : 3 bytes.<br> - Source : Terminal. */
    tagPPassMagIndicator = 0x9F918503,  /*!< PAYPASS - Indicates for each AID whether the PayPass – Mag Stripe profile is supported or not by the PayPass reader. Its value is implementation specific (\a 0 if not supported, \a 1 if supported). <br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassTrmCapCvmReq = 0x9F918504,  /*!< PAYPASS - Indicates the card data input, CVM, and security capabilities of the terminal and PayPass reader when the transaction amount is greater than or equal to the Terminal CVM Required Limit.<br>    - Format : b.<br>   - Length : 3 bytes.<br> - Source : Terminal. */
    tagPPassTrmCapNoCvmReq = 0x9F918505,    /*!< PAYPASS - Indicates the card data input, CVM, and security capabilities of the terminal and PayPass reader when the transaction amount is below the Terminal CVM Required Limit.<br>   - Format : b.<br>   - Length : 3 bytes.<br> - Source : Terminal. */
    tagPPassTxnCVM = 0x9F918506,    /*!< PAYPASS - Data object used to indicate to the terminal the outcome of the CVM Selection function. Possible values are "No CVM", "Signature", "Online PIN". The coding of the value is implementation specific. Refer to \ref PayPassPossibleCvm for more information about the possible values.<br>    - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassTxnOutcome = 0x9F918507,    /*!< PAYPASS - Data object used to indicate to the terminal the outcome of the transaction processing. Possible values are "Approved", "Online Request", "Declined", "Try Another Interface" or "End Application". The coding of the value is implementation specific. Refer to \ref PayPassPossibleOutcomes for more information about the possible values.<br>    - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntMagKTrk2 = 0x9F918508,   /*!< PAYPASS - Internal tag indicating the number of non-zero bits in PUNATC Track 2 (k Track2).<br>    - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntMagQTrk2 = 0x9F918509,   /*!< PAYPASS - Internal tag indicating the number of non-zero bits in PCVC3 Track 2 (q Track2).<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntMagKTrk1 = 0x9F91850A,   /*!< PAYPASS - Internal tag indicating the number of non-zero bits in PUNATC Track 1 (k Track1).<br>    - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntMagQTrk1 = 0x9F91850B,   /*!< PAYPASS - Internal tag indicating the number of non-zero bits in PCVC3 Track 1 (q Track1).<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntMagNUN = 0x9F91850C, /*!< PAYPASS - Internal tag equal to ((k Track2) - (t Track2)).<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntMagTrmAVNList = 0x9F91850D,  /*!< PAYPASS - Internal tag indicating the list of MStripe application version number supported by the terminal. This consiste of a concatenation of AVN (each of them is coded on 2 bytes).<br>    - Format : b.<br>   - Length : up to 128 bytes (i.e. up to 64 application version numbers).<br> - Source : Terminal. */
    tagPPassIntChipODAMeth = 0x9F91850E,    /*!< PAYPASS - Internal tag indicating the MChip ODA selected method. Refer to \ref PayPassPossibleOdaMethods for more information about the possible values.<br>   - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntChipDataPartODA = 0x9F91850F,    /*!< PAYPASS - Internal tag containing the concatenation of data that are part of authentication.<br>   - Format : b.<br>   - Length : up to 3072 bytes.<br>    - Source : Terminal. */
    tagPPassIntChipSFIErr = 0x9F918510, /*!< PAYPASS - Internal tag indicating if a record that is part of ODA is incorrectly coded or not. If there is no error, this tag is not stored.<br>   - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntChipTrmAVNList = 0x9F918511, /*!< PAYPASS - Internal tag indicating the list of MChip application version number supported by the terminal. This consists of a concatenation of AVN (each of them is coded on 2 bytes).<br>  - Format : b.<br>   - Length : up to 128 bytes (i.e. up to 64 application version numbers).<br> - Source : Terminal. */
    tagPPassIntChipGenACRefCtrlPrm = 0x9F918512,    /*!< PAYPASS - Internal tag indicating the Generate AC Reference Control Parameter to be sent on the Generate AC command (result of the Terminal Action Analysis Step). Refer to \ref GenAcRefControlParameters for more information about the possible values. Note the \a EMVLIB_GENAC_RCP_CDA_MASK bit is not set in this value.<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntChipTACIACDftMatch = 0x9F918513, /*!< PAYPASS - Internal tag indicating the TAC IAC default matched or not with the TVR (\a TRUE if it matched, \a FALSE else).<br>  - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntChioGenACRspDat = 0x9F918514,    /*!< PAYPASS - Internal tag containing the Generate AC card response data (content of template 0x77). This tag will be used for CDA signature generation and verification.<br>  - Format : b.<br>   - Length : up to 257 bytes.<br> - Source : Terminal. */
    tagPPassIntSentAPDU = 0x9F918515,   /*!< PAYPASS - Internal tag containing an APDU sent by the reader to the contactless card (several tags can be present).<br>    - Format : b.<br>   - Length : up to 261 bytes.<br> - Source : Terminal. */
    tagPPassIntCardRsp = 0x9F918516,    /*!< PAYPASS - Internal tag containing a card response (several tags can be present).<br>   - Format : b.<br>   - Length : up to 257 bytes.<br> - Source : Terminal. */
    tagPPassIntCAPKIdxList = 0x9F918517,    /*!< PAYPASS - Internal tag containing the list of CA Public Key index list (concatenation of indexes (each one coded on one byte)). This tag can contain up to 64 supported key indexes.<br>   - Format : b.<br>   - Length : up to 64 bytes.<br>  - Source : Terminal. */
    tagPPassIntMrcFrcOnl = 0x9F918518,  /*!< PAYPASS - Internal tag that indicates if the merchant wants to force the tranasction online (coded on 1 byte). If set to 01, TVR Byte 4 bit 4 is set at the begining of the Terminal Action Analysis to indicate merchant has forced the transaction online (\a 1 if merchant wishes to force transaction online, \a not \a present or \a 0 if not forced online).<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntTxnType = 0x9F918701,    /*!< EMV - Internal transaction type.<br>It can be different than 0x9C (value shall be 0x00, 0x01, 0x02, 0x09 or 0x20)).<br>    - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntCAPKChkSum = 0x9F918702, /*!< EMV - Certification Authority Public Key Check Sum.<br>A check value calculated on the concatenation of all parts of the Certification Authority Public Key (RID, Certification Authority Public Key Index, Certification Authority Public Key Modulus, Certification Authority Public Key Exponent) using SHA-1.<br>  - Format : b.<br>   - Length : 20 bytes.<br>    - Source : Terminal. */
    tagPPassIntCAPKExp = 0x9F918703,    /*!< EMV - Certification Authority Public Key Exponent.<br>Value of the exponent part of the Certification Authority Public Key.<br>    - Format : b.<br>   - Length : 1 or 3 bytes.<br>    - Source : Terminal. */
    tagPPassIntCAPKMod = 0x9F918704,    /*!< EMV - Certification Authority Public Key Modulus.<br>Value of the modulus part of the Certification Authority Public Key.<br>  - Format : b.<br>   - Length : NCA (up to 248).<br> - Source : Terminal. */
    tagPPassIntDftDDOL = 0x9F918705,    /*!< EMV - Default DDOL.<br>DDOL to be used for constructing the INTERNAL AUTHENTICATE command if the DDOL in the card is not present.<br>  - Format : b.<br>   - Length : var.<br> - Source : Terminal. */
    tagPPassIntDftTDOL = 0x9F918706,    /*!< EMV - Default TDOL.<br>TDOL to be used for generating the TC Hash Value if the TDOL in the card is not present.<br>    - Format : b.<br>   - Length : var.<br> - Source : Terminal. */
    tagPPassIntMaxTgtBias = 0x9F918707, /*!< EMV - Maximum Target Percentage to be use for Biased Random Selection.<br>Value used in terminal risk management for random transaction selection.<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntTgt = 0x9F918708,    /*!< EMV - Target Percentage to be used for Random Selection.<br>Value used in terminal risk management for random transaction selection.<br>   - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassIntTACDft = 0x9F918709, /*!< EMV - Terminal Action Code Default.<br>Specifies the acquirer‘s conditions that cause a transaction to be rejected if it might have been approved online, but the terminal is unable to process the transaction online.<br>    - Format : b.<br>   - Length : 5 bytes.<br> - Source : Terminal. */
    tagPPassIntTACDnl = 0x9F91870A, /*!< EMV - Terminal Action Code Denial.<br>Specifies the acquirer‘s conditions that cause the denial of a transaction without attempt to go online.<br> - Format : b.<br>   - Length : 5 bytes.<br> - Source : Terminal. */
    tagPPassIntTACOnl = 0x9F91870B, /*!< EMV - Terminal Action Code Online.<br>Specifies the acquirer‘s conditions that cause a transaction to be transmitted online.<br>   - Format : b.<br>   - Length : 5 bytes.<br> - Source : Terminal. */
    tagPPassIntThresholdValBias = 0x9F91870C,   /*!< EMV - Threshold Value for Biased Random Selection.<br>Value used in terminal risk management for random transaction selection.<br> - Format : b.<br>   - Length : 4 bytes.<br> - Source : Terminal. */
    tagPPassIntTxnRefCur = 0x9F91870D,  /*!< EMV - Transaction Reference Currency Conversion.<br>Factor used in the conversion from the Transaction Currency Code to the Transaction Reference Currency Code.<br>   - Format : n 8.<br> - Length : 4 bytes.<br> - Source : Terminal. */
    tagPPassIntIssScrRes = 0x9F91870E,  /*!< EMV - Issuer Script Results.<br>Indicates the result of the terminal script processing.<br>    - Format : b.<br>   - Length : var.<br> - Source : Terminal. */
    tagPPassIntAppSelInd = 0x9F91870F,  /*!< EMV - Application Selection Indicator.<br>For an application in the ICC to be supported by an application in the terminal, the Application Selection Indicator indicates whether the associated AID in the terminal must match the AID in the card exactly, including the length of the AID, or only up to the length of the AID in the terminal There is only one Application Selection Indicator per AID supported by the terminal.<br>  - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagPPassEnd
};

enum cmvVisaWaveTags {
    tagVWaveTermTxnQualifiers = 0x9F66, /*!< VISAWAVE - Indicates reader capabilities, requirements, and preferences to the card.<br> - Format : B.<br> - Length : 4 bytes.<br> - Source : Terminal. */
    tagVWaveVlpIssuerAuthorizationCode = 0x9F74,    /*!< VISAWAVE - VLP issuer authorization code */
    tagVWaveVlpAvailableFunds = 0x9F79, /*!< VISAWAVE - VLP available funds */
    tagVWaveVlpTermSupportIndicator = 0x9F7A,   /*!< VISAWAVE - VLP terminal support indicator */
    tagVWaveReaderClessTRLimit = 0xDF00,    /*!< VISAWAVE - Reader Contactless transaction limit */
    tagVWaveCvmRequiredLimit = 0xDF01,  /*!< VISAWAVE - CVM required limit */
    tagVWaveClessFloorLimit = 0xDF02,   /*!< VISAWAVE - Contactless floor limit */
    tagVWaveSfi11PresentInAfl = 0x9F918901, /*!< VISAWAVE - Indicates if the SFI 11 is present or not in the AFL (offline/online transaction). */
    tagVWaveOnlineOnly = 0x9F918902,    /*!< VISAWAVE - Indicates if an AID allows the Visa Wave 2 Offline Transaction. */
    tagVWaveTransactionCvm = 0x9F918903,    /*!< VISAWAVE - Data object used to indicate to the terminal the outcome of the CVM Selection function. Possible values are "No CVM", "Signature", "Online PIN". The coding of the value is implementation specific. Refer to \ref VisaWavePossibleCvm for more information about the possible values.<br>  - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagVWaveIntGenacRefCtrlParameter = 0x9F918905,  /*!< VISAWAVE - Internal tag indicating the Generate AC Reference Control Parameter to be sent on the Generate AC command (Always ARQC for online transactions). */
    tagVWaveIntDataPartOfOda = 0x9F918906,  /*!< VISAWAVE - Internal tag containing the concatenation of data that are part of authentication.<br>  - Format : b.<br>   - Length : up to 3072 bytes.<br>    - Source : Terminal. */
    tagVWaveIntIsSfiError = 0x9F918907, /*!< VISAWAVE - Internal tag indicating if a record that is part of ODA is incorrectly coded or not. If there is no error, this tag is not stored.<br>  - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    tagVWaveIntGenacResponseData = 0x9F918908,  /*!< VISAWAVE - Internal tag containing the Generate AC card response data (content of template 0x77). This tag will be used for CDA signature generation and verification.<br> - Format : b.<br>   - Length : up to 257 bytes.<br> - Source : Terminal. */
    tagVWaveIntDataPartOfDda = 0x9F918909,  /*!< VISAWAVE - Internal tag containing the concatenation of data that are part of DDA authentication.<br>  - Format : b.<br>   - Length : up to 3072 bytes.<br>    - Source : Terminal. */
    tagVWaveIntSentApdu = 0x9F91890A,   /*!< VISAWAVE - Internal tag containing an APDU sent by the reader to the contactless card (several tags can be present).<br>   - Format : b.<br>   - Length : up to 261 bytes.<br> - Source : Terminal. */
    tagVWaveIntCardResponse = 0x9F91890B,   /*!< VISAWAVE - Internal tag containing a card response (several tags can be present).<br>  - Format : b.<br>   - Length : up to 257 bytes.<br> - Source : Terminal. */
    tagVWaveEnd
};

enum cmvKernelTags {
    cmvKernelBeg,
    cmvKernelAppName = 0x9F918409,  /*!< Indicates the kernel name (when calling the GetInfos kernel API function, if available).<br>  - Format : anp.<br> - Length : up to 32 bytes.<br>  - Source : Terminal. */
    cmvKernelAppID = 0x9F91840A,    /*!< Indicates the kernel identifier (when calling the GetInfos kernel API function, if available).<br>   - Format : n.<br>   - Length : 2-5 bytes.<br>   - Source : Terminal. */
    cmvKernelAppCRC = 0x9F91840B,   /*!< Indicates the kernel CRC (when calling the GetInfos kernel API function, if available).<br>    - Format : b.<br>   - Length : 2 bytes.<br> - Source : Terminal. */
    cmvKernelIPKRecoverData = 0x9F91840C,   /*!< Data Recovered from the Issuer Public Key Certificate, as defined in EMV Specification 4.2, Book 2, Table 6.<br>   - Format : b.<br>   - Length : up to 248 bytes.<br> - Source : Terminal. */
    cmvKernelSDARecoverData = 0x9F91840D,   /*!< Data Recovered from the SDA, as defined in EMV Specifcation 4.2, Book 2, Table 7.<br>  - Format : b.<br>   - Length : up to 248 bytes.<br> - Source : Terminal. */
    cmvKernelICCRecoverData = 0x9F91840E,   /*!< Data Recovered from the ICC Public Key Certificate, as defined in EMV Specification 4.2, Book 2, Table 14.<br> - Format : b.<br>   - Length : up to 248 bytes.<br> - Source : Terminal. */
    cmvKernelDDARecoverData = 0x9F91840F,   /*!< Data Recovered from the DDA, as defined in EMV specification, Book 2, Table 17.<br>    - Format : b.<br>   - Length : up to 248 bytes.<br> - Source : Terminal. */
    cmvKernelIsCARevoked = 0x9F918410,  /*!< Indicates if the Certificate Authority public key is revoked or not (\a 1 if CA is revoked, \a 0 or \a not \a present if not revoked).<br>Note this tag can be managed or not depending on the kernel.<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    cmvKernelLastAPDUandSW = 0x9F918411,    /*!< Indicates the last command sent to the card (CLA/INS/P1/P2) and the status word (SW) provided by the card to this command.<br> - Format : b.<br>   - Length : 6 bytes.<br> - Source : Terminal. */
    cmvKernelCumulatedAmt = 0x9F918412, /*!< Indicates the cumulated amount for the card during the transaction. */
    cmvKernelSignRequest = 0x9F918413,  /*!< Indicates whether a signature is required or not (\a 1 if signature is required, \a 0 or \a not \a present if not required).<br>Note this tag can be managed or not depending on the kernel.<br>   - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    cmvKernelIsPPFunctioning = 0x9F918414,  /*!< Indicates if the PinPad is functioning or not (\a 1 or \a not \a present if pinpad is functionning, \a 0 if not functioning).<br>Note this tag can be managed or not depending on the kernel.<br>  - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    cmvKernelPANinBlackList = 0x9F918415,   /*!< Indicates if the Card PAN is in the black list or not (\a 1 if in exception file, \a 0 or \a not \a present if not in exception file). This tag can be provided to th ekernel to indicate if the PAN is present or not in the exception file.<br>  - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    cmvKernelCardType = 0x9F918416, /*!< Indicates the card type (value depends on the kernel defines).<br> - Format : b.<br>   - Length : 2 bytes.<br> - Source : Terminal. */
    cmvKernelOnlinePINRequest = 0x9F918417, /*!< Indicates if online PIN is requested by the kernel or not (\a 1 if Online PIN is requested, \a 0 or \a not \a present if not).<br>Note this tag can be managed or not depending on the kernel.<br> - Format : b.<br>   - Length : 1 byte.<br>  - Source : Terminal. */
    cmvKernelConstructedTDOLVal = 0x9F918418,   /*!< Value of the constructed TDOL (using terminal data).<br>Note this tag can be managed or not depending on the kernel.<br>   - Format : b.<br>   - Length : up to 256 bytes.<br> - Source : Terminal. */
    cmvKernelConstructedPDOLVal = 0x9F918419,   /*!< Value of the constructed PDOL (using terminal data).<br>Note this tag can be managed or not depending on the kernel.<br>   - Format : b.<br>   - Length : up to 256 bytes.<br> - Source : Terminal. */
    cmvKernelConstructedCDOL1Val = 0x9F91841A,  /*!< Value of the constructed CDOL1 (using terminal data).<br>Note this tag can be managed or not depending on the kernel.<br>  - Format : b.<br>   - Length : up to 256 bytes.<br> - Source : Terminal. */
    cmvKernelConstructedCDOL2Val = 0x9F91841B,  /*!< Value of the constructed CDOL2 (using terminal data).<br>Note this tag can be managed or not depending on the kernel.<br>  - Format : b.<br>   - Length : up to 256 bytes.<br> - Source : Terminal. */
    cmvKernelConstructedDDOLVal = 0x9F91841C,   /*!< Value of the constructed DDOL (using terminal data).<br>Note this tag can be managed or not depending on the kernel.<br>   - Format : b.<br>   - Length : up to 256 bytes.<br> - Source : Terminal. */
    cmvKernelTrmLang = 0x9F91841D,  /*!< List of languages supported by the terminal (concatenation of languages identifiers : for example "enfrde"). From this tag would be determined the common prefered language (if kernel supports multi-language), stored in the \ref TAG_KERNEL_SELECTED_PREFERED_LANGUAGE tag.<br> - Format : a.<br>   - Length : up to 64 bytes (maximum of 32 supported languages).<br>  - Source : Terminal. */
    cmvKernelSelPrfrdLang = 0x9F91841E, /*!< Language identifier matching between the card and the terminal (as determined by the language selection).<br>  - Format : a.<br>   - Length : 2 bytes.<br> - Source : Terminal. */
    cmvKernelRevokedCertList = 0x9F91841F,  /*!< List of revoked certificates. Each revoked certificate is a concatenation of RID (on 5 bytes), Exponent (on 1 byte) and the serial number (on 3 bytes).<br>    - Format : b.<br>   - Length : up to 288 bytes (maximum of 32 revoked certificates).<br>    - Source : Terminal. */
    cmvKernelEnd
};

#endif
#endif

#define __EMV_H
#endif
