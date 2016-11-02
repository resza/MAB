#ifndef _MINIATM_H
#define _MINIATM_H

#ifdef BANK_BRI
int TransAktivasiKartu(void);
int TransRenewalKartu(void);
int TransReissuePin(void);
int TransUbahPin(void);
int TransUbahPwdSPV(void);

int TransSMSBanking(void);
int TransPhoneBanking(void);
int TransIBanking(void);
int TransTrxPBTransfer(void);
int TransTrxPBNoHP(void);


int TransInfoSaldo(void);
int TransInfoSaldoBankLain(void);
int TransMiniStatement(void);
int TransMutasiRekening(void);

int TransTrfSesamaBRI(void);
int TransTrfAntarBank(void);
int TransInfoKodeBank(void);

int TransPembayaranTelkom(void);
int TransPembayaranHalo(void);
int TransPembayaranMatrix(void);

int TransPembayaranPLNPascaBayar(void);
int TransPembayaranPLNPraBayar(void);
int TransPembayaranPLNToken(void);

int TransPembayaranKK_KTA(uchar ucTransType);

int TransPembayaranKKBRI(void);
int TransPembayaranKKCitibank(void);
int TransPembayaranKTACitibank(void);
int TransPembayaranKKSCB(void);
int TransPembayaranKTASCB(void);
int TransPembayaranKKANZ(void);
int TransPembayaranKKHSBC(void);
int TransPembayaranKTAHSBC(void);
int TransPembayaranKKRBS(void);
int TransPembayaranKTARBS(void);

int TransPembayaranCICILAN(void);

int TransPembayaranZakat(void);
int TransPembayaranInfaq(void);
int TransPembayaranDPLK(void);

int TransPembayaranTiketPesawat(void);
int TransBriva(void);

int TransPembayaranKuliah(void);
int TransInfoKodeKuliah(void);


int TransAbsensi(void);

int TransIsiUlang(void);

int TransLogon(void);

int CetakStruk(uchar ucPrnFlag);
void MiniAtmPrnLastTrans(void);
void MiniAtmRePrnSpecTrans(void);
void MiniAtmReviewLastTrans(void);
void MiniAtmReviewAnyTrans(void);
//void MiniATMSummaryReport(uchar FlagDate);
//void MiniATMDetailReport(uchar FlagDate);


void CardServicesPrnLastTrans(void);
void CardServicesRePrnSpecTrans(void);
void CardServicesReviewLastTrans(void);

void CardServicesSummaryReport(uchar FlagDate);
void CardServicesDetailReport(uchar FlagDate);


void TunaiPrnLastTrans(void);
void TunaiRePrnSpecTrans(void);
void TunaiReviewLastTrans(void);
void TunaiReviewAnyTrans(void);
void TunaiSummaryReport(uchar FlagDate);
void TunaiDetailReport(uchar FlagDate);


void TBankPrnLastTrans(void);
void TBankRePrnSpecTrans(void);
void TBankReviewLastTrans(void);

void TBankSummaryReport(uchar FlagDate);
void TBankDetailReport(uchar FlagDate);




//TUNAI
int TransTarikTunai(void);
int TransSetorPinjaman(void);
int TransSetorSimpanan(void);
int TransVoidTarikTunai(void);

//T-BANK
int TransCashIn(void);
int TransCashOut(void);
int TransSetorPasti(void);

int TransSSB(void);
#endif
int TransTestKoneksi(void);



int TransTrfAntarBankBkpn(void);
int InputUangBkpn(void);
int CetakStrukBkpn(uchar ucPrnFlag);

void MiniAtmPrnLastTransBkpn(void);
void MiniAtmRePrnSpecTransBkpn(void);

int TransInfoKodeBankBkpn(void);

void MiniATMSummaryReport(uchar FlagDate);
void MiniATMDetailReport(uchar FlagDate);

int TransIsiUlangBkpn(void);



int TransMultifinanceBkpn(void);



int TransTrfAntarBankTes(void);



int TransSetorTunaiBkpn(void);

//TransTarikTunaiBkpn
int TransTarikTunaiBkpn(void);


int TransInfoSaldoBkpn(void);


int TransUbahPinBkpn(void);

int TransPembayaranKK_Bkpn(uchar ucTransType);

int TransPembayaranTelkomBkpn(void);

int TransPembayaranSelulerBkpnPilihProvider(void);

int TransPembayaranSelulerBkpn(void);

int TransPembayaranPdamPalyjaBkpn(void);

int TransPembayaranPdamAetraBkpn(void);

int TransInfoKodeBankBkpnPilih(char *dst, const char *src);

int TransAtmbMerchantNasabahMenuTrue(void);
int TransAtmbMerchantNasabahMenuFalse(void);

int TransInfoKodeBankBkpn2(void);

int TransInitKartuBkpn(void);




















#endif


