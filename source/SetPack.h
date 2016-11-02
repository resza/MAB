#ifndef _SETPACK_H
#define _SETPACK_H

void CalculatePinBlock(uchar *sPinBlock);
void CalculatePinBlockCS(uchar *sPinBlock);
void SetPackSale(void);
void SetPackPurchase(void);
void SetPackSaleRedeemption(void);
void setPackPrivilage(void);
void SetPackCicilanBukopin(void);
void SetPackCicilanBring(void);

// Card Service
void setPackKartuPetugas(void);
void setPackAktivasiKartuPetugas(void);
void setPackAktivasiKartuNasabah(void);
void setPackAktivasiKartuPrint(void);
void setPackRenewalKartuLama(void);
void setPackRenewalKartuBaru(void);
void setPackReissuePinPetugas(void);
void setPackReissuePinNasabah(void);
void setPackUbahPinCS(void);
void setPackUbahPwdSPV(void);
//Registrasi
void setPackSMSBanking(void);
void setPackPhoneBanking(void);
void setPackIBanking(void);
void setPackTrxPBTransferInq(void);
void setPackTrxPBTransfer(void);
void setPackTrxPB(void);
//Informasi
void setPackInfoSaldo(void);
void setPackInfoSaldoBankLain(void);
void setPackMiniStatement(void);
void setPackMutasiRekening(void);
//Transfer
void setPackInqTransferSesamaBRI(void);
void setPackTransferSesamaBRI(void);
void setPackInqTransferAntarBank(void);
void setPackTransferAntarBank(void);
//Pembayaran
void setPackInqTelkom(void);
void setPackTelkom(void);
void setPackInqPascaBayar(void);
void setPackPascaBayar(void);

void setPackInqPLNPascaBayar(void);
void setPackPLNPascaBayar(void);
void setPackInqPLNPraBayar(void);
void setPackPLNPraBayar(void);
void setPackPLNToken(void);

void setPackInqKK_KTA(void);
void setPackKK_KTA(void);

void setPackInqCicilan(void);
void setPackCicilan(void);

void setPackPembayaranZakat_Infaq(void);
void setPackPembayaranDPLK(void);

void setPackInqTiketPesawat(void);
void setPackTiketPesawat(void);

void setPackInqBriva(void);
void setPackBriva(void);

void setPackInqPembayaranKuliah(void);
void setPackPembayaranKuliah(void);

//Isi Ulang
void setPackIsiUlang(void);

//Absensi
void setPackAbsensi(void);

//Logon
void setPackLogon(void);
void setPackLogonAJ(void);
void setPackLogonBukopin(void);


//BRIZZI
void setPackBrizziTopUpOnline(void);
void setPackBrizziAktvDeposit(void);
void setPackBrizziInfoDeposit(void);
void setPackBrizziTopUpDeposit(void);
void setPackBrizziVoidReq(void);
void setPackBrizziVoid(void);
void setPackBrizziSettlement(void);
void setPackBrizziRedeem(void);
void setPackBrizziReaktivasi(uchar *LastTrxDate);

void setPackTarikTunai(void);
void setPackSetorTunaiInq(void);
void setPackSetorTunai(void);
void setPackVoidTarikTunai(void);

//T-BANK
void setPackCashIn(void);
void setPackInqCashOut(void);
void setPackCashOut(void);

void SetPackSetorPasti(void);
void setPackSSB(void);

// DIKI ADD FOR MINI ATM BUKOPIN
void setPackInqTransferAntarBankBkpn(void);
void setPackTransferAntarBankBkpn(void);

// diki add for private label bukopin
void SetPackSalePrvtLbl(void);
void SetPackCicilanBukopinPrvtLbl(void);

void setPackIsiUlangBkpn(void);

void setPackInqMultifinanceBkpn(void);
void setPackMultifinanceBkpn(void);

void setPackTarikTunaiBkpnInq(void);

void setPackInfoSaldoBkpn(void);

void setPackUbahPinBkpn(void);

void setPackInqKKBkpn(void);

void setPackKKBkpn(void);

void setPackInqTelkomBkpn(void);

void setPackTelkomBkpn(void);

void setPackInqSelularBkpn(void);

void setPackSelularBkpn(void);

void setPackInqPdamBkpn(void);

void setPackPdamBkpn(void);

void setPackInqSetorTunaiBankBkpn(void);
void setPackSetorTunaiBankBkpn(void);


















#endif

