#ifndef _PRINT_MINIATM_BRI_H
#define _PRINT_MINIATM_BRI_H
int PrnTBank(uchar ucPrnFlag);
int PrnReceipt(uchar ucPrnFlag);
int PrnSale(uchar ucPrnFlag);
int PrnSaleRedeemption(uchar ucPrnFlag);
void PrnPrivilage(void);
int PrnCicilanBring(uchar ucPrnFlag);
int PrnCicilanBukopin(uchar ucPrnFlag);
void PrnVersi(void);
void PrnAktivasiKartu(void);
void PrnReissuePin(void);
void PrnRenewalKartu(void);
void PrnUbahPinCS(void);
void PrnUbahPinSPV(void);
void PrnRegistrasiSMSBanking(void);
void PrnRegistrasiPhoneBanking(void);
void PrnRegistrasiIBanking(void);
void PrnRegistrasiTRXPBTransfer(void);
void PrnRegistrasiTRXPBNoHP(void);
void PrnInfoSaldoBRI(void);
void PrnInfoSaldoBankLain(void);
void PrnMiniStatement(void);
void PrnMutasiRekening(void);
void PrnPembTelkom(void);
void PrnPascaBayarHalo(void);
void PrnPascaBayarMatrix(void);
void PrnPLNPascaBayar(void);
void PrnPLNPraBayar(void);
void PrnKK_BRI(void);
void PrnKK_KTA(void);
void PrnCicilan(void);
void PrnTiketPesawat(void);
void PrnBriva(void);
void PrnPendidikan(void);

void PrnFooterTerimaKasih(void);
void PrnFooterInfoSaldo(void);
void PrnFooterSSB(void);
void PrnTrfSesamaBRI(void);
void PrnTrfAntarBank(void);
void PrnFooterTrfSesamaBRI(void);
void PrnIsiUlang(void);
void PrnIsiUlangSmartFren(void);
void PrnIsiUlangIM3(void);
void PrnIsiUlangSimpati(void);
void PrnPembZakat(void);
void PrnPembDPLK(void);

void PrnHeaderTidMid(void);
void PrnHeaderDateTime(void);
void PrnHeaderCardNumber(void);
void PrnHeaderTraceApprCode(void);
void PrnHeaderBatchRefNum(void);
void PrnHeaderTransactionName(void);

void PrnFooterKK_KTA(void);

void PrnTarikTunai(void);
void PrnSetorTunai(void);

void PrnSetorPasti(void);
void PrnSSB(void);
void PrnCasIn(void);
void PrnCasOut(void);

int PrnReceiptSSB(uchar ucPrnFlag);



// DIKI ADD FOR MINI ATM BUKOPIN
void PrnTrfAntarBankBkpn(void);
void PrnFooterTrfSesamaBkpn(void);



// add fiki for private label bukopin
int PrnReceiptPrvtLbl(uchar ucPrnFlag);
int PrnCicilanBukopinPrvtLbl(uchar ucPrnFlag);



void PrnPdamPalyjaBkpn(void);
void PrnPdamAetraBkpn(void);

void PrnTarikTunaiBkpn(void);

void PrnSetorTunaiBkpn(void);

void PrnHeaderCardNumberBkpn(void);

void PrnGantiPinBkpn(void);





void PrnFooterInfoWomBkpn(void);
void PrnFooterInfoBafBkpn(void);








#endif

