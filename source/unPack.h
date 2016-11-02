#ifndef _UNPACK_H
#define _UNPACK_H
uchar unPackTransaction(void);
uchar unPackTransfer_SesamaBRI(void);
uchar unPackTransfer_AntarBank(void);
uchar unPackPembayaran_Telkom(void);
uchar unPackPembayaran_KartuHalo(void);
uchar unPackPembayaran_KartuMatrix(void);
uchar unPackPembayaran_PLNPascaBayar(void);
uchar unPackPembayaran_PLNPraBayar(void);
uchar unPackPembayaran_KKBRI(void);
uchar unPackPembayaran_KKKTA(void);
uchar unPackPembayaran_Cicilan(void);
uchar unPackPembayaran_Zakat(void);
uchar unPackPembayaran_DPLK(void);
uchar unPackPembayaran_Pesawat(void);
uchar unPackPembayaran_Briva(void);
uchar unPackPembayaran_Pendidikan(void);
uchar unPackISi_Ulang(void);

uchar unPackTopUpOnLineBrizzi(void);
uchar unPackAktivasiDepositBrizzi(void);
uchar unPackInfoDepositBrizzi(void);
uchar unPackRedeemBrizzi(void);
uchar unPackReaktivasiBrizzi(void);

uchar unPackTarikTunai(void);
uchar unPackSetorTunai(void);

uchar unPackCashIn(void);
uchar unPackCashOut(void);

uchar unPackSetorPasti(void);
uchar unPackSaleRedeemption(void);
uchar unPackCicilanBukopin(void);
uchar unPackCicilanBring(void);
uchar unPackRenewalKartu(void);
uchar unPackRegTrxPBTransfer(void);
uchar unPackSSB(void);

// DIKI ADD FOR MINI ATM BUKOPIN

uchar unPackTransfer_AntarBankBkpn(void);
uchar unPackISi_UlangBkpn(void);

uchar unPackTarikTunaiBkpn(void);

uchar unPackPembayaran_CicilanBkpn(void);

uchar unPackPembayaran_KKBkpn(void);

uchar unPackPembayaran_TelkomBkpn(void);

uchar unPackPembayaran_SelularBkpn(void);

uchar unPackPembayaran_PdamBkpn(void);

uchar unPackSetorTunaiBankBkpn(void);













#endif
