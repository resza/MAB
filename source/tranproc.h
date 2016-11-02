
/****************************************************************************
NAME
    tranproc.h - 定义交易处理模块

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
****************************************************************************/

#ifndef _TRANPROC_H
#define _TRANPROC_H

/************************************************************************
 * 定义交易类型 查询 批上送 签到 预授权 授权销售 退款 冲正 销售 结算 测试
 * 撤消预授权 撤消
************************************************************************/
// !!!! Every time a new transaction type is added, glTranConfig[] also add a new item accordingly.
enum
{
	TRANS_TYPE_PREAUTH,
	TRANS_TYPE_AUTH,
	TRANS_TYPE_SALE,
	TRANS_TYPE_INSTALMENT,
	TRANS_TYPE_BATCH_UPLOAD,
	TRANS_TYPE_LOGON,
	TRANS_TYPE_REFUND,
	TRANS_TYPE_REVERSAL,
	TRANS_TYPE_SETTLEMENT,
	TRANS_TYPE_LOAD_PARA,
	TRANS_TYPE_VOID,
	TRANS_TYPE_OFFLINE_SEND,
	TRANS_TYPE_OFF_SALE,
	TRANS_TYPE_SALE_COMP,
	TRANS_TYPE_CASH,
	TRANS_TYPE_SALE_OR_AUTH,
	TRANS_TYPE_TC_SEND,
	TRANS_TYPE_ECHO_TEST,
	TRANS_TYPE_PIN_VERIFY,		//irf, 20121129
	TRANS_TYPE_TMS_TELIUM,
	TRANS_TYPE_CARD_VERIFY,
	TRANS_TYPE_BALANCE,
	TRANS_TYPE_ADJUST,

	//LTMK
	TRANS_TYPE_LTMK,

	//LTWK
	TRANS_TYPE_LTWK,

	//PREPAID
	TRANS_TYPE_PREPAID_PURCHASE,
	TRANS_TYPE_PREPAID_INQ_TOPUP,
	TRANS_TYPE_PREPAID_TOPUP,

	//add cung for BRI
	//PURCHASES
	TRANS_TYPE_SALE_REDEEMPTION,
	TRANS_TYPE_PRIVILAGE,
	TRANS_TYPE_CICILAN_BRING,
	TRANS_TYPE_CICILAN_BUKOPIN,
	//ABSENSI
	TRANS_TYPE_ABSENSI,
	//CARD SERViCE
	TRANS_TYPE_AKTIFASIKARTU_PETUGAS,
	TRANS_TYPE_AKTIFASIKARTU_NASABAH,
	TRANS_TYPE_AKTIFASIKARTU_PRINT,

	TRANS_TYPE_RENEWALKARTU_PETUGAS,
	TRANS_TYPE_RENEWALKARTU_LAMA,
	TRANS_TYPE_RENEWALKARTU_BARU,

	TRANS_TYPE_REISSUE_PIN_PETUGAS,
	TRANS_TYPE_REISSUE_PIN_NASABAH,
	
	TRANS_TYPE_UBAHPIN_CS,
	TRANS_TYPE_UBAHPWD_SPV,
	
	//MINI ATM
	TRANS_TYPE_SMSBANKING,
	TRANS_TYPE_PHONEBANKING,
	TRANS_TYPE_IBANKING,
	TRANS_TYPE_TRX_PB_TRANSFER_INQ,
	TRANS_TYPE_TRX_PB_TRANSFER,
	TRANS_TYPE_TRX_PB_NO_HP,
	
	TRANS_TYPE_INFOSALDO,
	TRANS_TYPE_INFOSALDO_BANKLAIN,
	TRANS_TYPE_MINISTATEMENT,
	TRANS_TYPE_MUTASIREKENING,
	
	TRANS_TYPE_INQ_TRFSESAMABRI,
	TRANS_TYPE_TRFSESAMABRI,
	TRANS_TYPE_INQ_TRFANTARBANK,
	TRANS_TYPE_TRFANTARBANK,

	TRANS_TYPE_INQ_TELKOM,
	TRANS_TYPE_TELKOM,
	TRANS_TYPE_INQ_KARTUHALO,
	TRANS_TYPE_KARTUHALO,
	TRANS_TYPE_INQ_KARTUMATRIX,
	TRANS_TYPE_KARTUMATRIX,

	TRANS_TYPE_INQ_PLNPASCABAYAR,
	TRANS_TYPE_PLNPASCABAYAR,
	TRANS_TYPE_INQ_PLNPRABAYAR,
	TRANS_TYPE_PLNPRABAYAR,
	TRANS_TYPE_PLNTOKEN,

	TRANS_TYPE_INQ_KKBRI,
	TRANS_TYPE_KKBRI,
	TRANS_TYPE_KKCITIBANK,
	TRANS_TYPE_KTACITIBANK,
	TRANS_TYPE_KKSCB,
	TRANS_TYPE_KTASCB,
	TRANS_TYPE_KKHSBC,
	TRANS_TYPE_KTAHSBC,
	TRANS_TYPE_KKRBS,
	TRANS_TYPE_KTARBS,
	TRANS_TYPE_KKANZ,

	TRANS_TYPE_INQ_FIF,
	TRANS_TYPE_FIF,
	TRANS_TYPE_INQ_BAF,
	TRANS_TYPE_BAF,
	TRANS_TYPE_INQ_OTO,
	TRANS_TYPE_OTO,
	TRANS_TYPE_INQ_FINANSIA,
	TRANS_TYPE_FINANSIA,
	TRANS_TYPE_INQ_VERENA,
	TRANS_TYPE_VERENA,
	TRANS_TYPE_INQ_WOM,
	TRANS_TYPE_WOM,

	TRANS_TYPE_ZAKAT,
	TRANS_TYPE_INFAQ,
	TRANS_TYPE_DPLK,

	TRANS_TYPE_INQ_GARUDA,
	TRANS_TYPE_GARUDA,
	TRANS_TYPE_INQ_LIONAIR,
	TRANS_TYPE_LIONAIR,
	TRANS_TYPE_INQ_SRIWIJAYA,
	TRANS_TYPE_SRIWIJAYA,
	TRANS_TYPE_INQ_MERPATI,
	TRANS_TYPE_MERPATI,

	TRANS_TYPE_INQ_BRIVA,
	TRANS_TYPE_BRIVA,

	TRANS_TYPE_INQ_BAYARKULIAH,
	TRANS_TYPE_BAYARKULIAH,

	TRANS_TYPE_AXIS,
	TRANS_TYPE_ESIA,
	//TRANS_TYPE_FREN_MOBI_HEPI,
	TRANS_TYPE_IM3,
	TRANS_TYPE_MENTARI,
	TRANS_TYPE_SIMPATI_AS,
	TRANS_TYPE_SMARTFREN,
	TRANS_TYPE_THREE,
	TRANS_TYPE_XL,

	//LOGON
	TRANS_TYPE_LOGON_BRI,
	TRANS_TYPE_TEST_KONEKSI,

	TRANS_TYPE_BRIZZI_INFO_SALDO,
	TRANS_TYPE_BRIZZI_PURCHASE,
	TRANS_TYPE_BRIZZI_TOPUP_ONLINE,
	TRANS_TYPE_BRIZZI_AKTV_DEPOS,
	TRANS_TYPE_BRIZZI_INFO_DEPOS,
	TRANS_TYPE_BRIZZI_TOPUP_DEP,
	TRANS_TYPE_BRIZZI_VOID_REQ,
	TRANS_TYPE_BRIZZI_VOID,
	TRANS_TYPE_BRIZZI_SETTLE,
	TRANS_TYPE_BRIZZI_SUMMARY,
	TRANS_TYPE_BRIZZI_DETAIL,
	TRANS_TYPE_BRIZZI_REDEEM_REQ,
	TRANS_TYPE_BRIZZI_REDEEM,
	TRANS_TYPE_BRIZZI_REAKT_REQ,
	TRANS_TYPE_BRIZZI_REAKTIVASI,

	TRANS_TYPE_TARIK_TUNAI,
	TRANS_TYPE_SETOR_INQ_PINJ,
	TRANS_TYPE_SETOR_PINJAMAN,
	
	TRANS_TYPE_SETOR_INQ_SIMP,
	TRANS_TYPE_SETOR_SIMPANAN,

	TRANS_TYPE_VOID_TARIK_TUNAI,

	//T-BANK PROGRESS,
	TRANS_TYPE_CASH_IN,
	//TRANS_TYPE_INQ_CASHOUT,
	TRANS_TYPE_CASHOUT,

	//SSB PROGRESS
	TRANS_TYPE_SIM_BARU,
	TRANS_TYPE_SIM_PERPANJANGAN,
	TRANS_TYPE_SIM_PENINGKATAN,
	TRANS_TYPE_SIM_PENURUNAN,
	TRANS_TYPE_SIM_MUTASI_GOL_SAMA,
	TRANS_TYPE_SIM_MUTASI_PENINGKATAN,
	TRANS_TYPE_SIM_MUTASI_PENURUNAN,
	TRANS_TYPE_SIM_HILANG_RUSAK_GOLSAMA,
	TRANS_TYPE_SIM_HILANG_RUSAK_PENINGKATAN,
	TRANS_TYPE_SIM_HILANG_RUSAK_PENURUNAN,
	TRANS_TYPE_UJI_SIMULATOR,
	TRANS_TYPE_BPKB_BARU,
	TRANS_TYPE_BPKB_GANTI_PEMILIK,
	TRANS_TYPE_STCK,
	TRANS_TYPE_MUTASI_RANMOR,
	TRANS_TYPE_SSB,

	//SETOR PASTI 
	TRANS_TYPE_SETORPASTI,

	//Artha Jasa
	
	TRANS_TYPE_INQ_PURCHASE,
	TRANS_TYPE_PURCHASE,
	TRANS_TYPE_LOGON_BUKOPIN,
	TRANS_TYPE_CASH_ADVANCE,


	// diki add mini atm bukopin
	TRANS_TYPE_INQ_TRFANTARBANK_BKPN,
	TRANS_TYPE_TRFANTARBANK_BKPN,


	//multi finance mini atm bukopin
	TRANS_TYPE_INQ_MAF_BKPN,
	TRANS_TYPE_MAF_BKPN,
	TRANS_TYPE_INQ_BAF_BKPN,
	TRANS_TYPE_BAF_BKPN,
	TRANS_TYPE_INQ_MCF_BKPN,
	TRANS_TYPE_MCF_BKPN,
	TRANS_TYPE_INQ_WOM_BKPN,
	TRANS_TYPE_WOM_BKPN,


	//setor & tarik tunai bukopin mini atm bukopin
	TRANS_TYPE_TARIK_TUNAI_BKPN_INQ,
	TRANS_TYPE_TARIK_TUNAI_BKPN,
	TRANS_TYPE_SETOR_TUNAI_BKPN_INQ,
	TRANS_TYPE_SETOR_TUNAI_BKPN,


	TRANS_TYPE_INFO_SALDO_BKPN,
	TRANS_TYPE_UBAH_PIN_BKPN,

	TRANS_TYPE_KK_ANZ_BKPN_INQ,
	TRANS_TYPE_KK_ANZ_BKPN,
	TRANS_TYPE_KK_BUKOPIN_BKPN_INQ,
	TRANS_TYPE_KK_BUKOPIN_BKPN,
	TRANS_TYPE_KK_BNI_BKPN_INQ,
	TRANS_TYPE_KK_BNI_BKPN,
	TRANS_TYPE_KK_HSBC_BKPN_INQ,
	TRANS_TYPE_KK_HSBC_BKPN,

	TRANS_TYPE_PDAM_PALYJA_BKPN_INQ,
	TRANS_TYPE_PDAM_PALYJA_BKPN,
	TRANS_TYPE_PDAM_AETRA_BKPN_INQ,
	TRANS_TYPE_PDAM_AETRA_BKPN,
	
	

	// 在此之前加新的交易类型宏名称. ADD NEW TRANS BEFORE MAX_TRANTYPE
	// 必须同步地在glTranConfig增加交易配置. Must sync with glTranConfig
	MAX_TRANTYPE,
};

// 错误代码(具体定义待定!!!!!)
#define ERR_BASE			0x10000
#define ERR_PINPAD			(ERR_BASE+0x01)
#define ERR_NO_TELNO		(ERR_BASE+0x03)
#define ERR_SWIPECARD		(ERR_BASE+0x05)
#define ERR_USERCANCEL		(ERR_BASE+0x06)
#define ERR_TRAN_FAIL		(ERR_BASE+0x07)
#define ERR_UNSUPPORT_CARD	(ERR_BASE+0x08)
#define ERR_SEL_ACQ			(ERR_BASE+0x09)
#define ERR_HOST_REJ		(ERR_BASE+0x0A)

#define ERR_COMM_ERROR		(ERR_BASE+0x50)

#define ERR_FILEOPER		(ERR_BASE+99)
// #define ERR_NOT_EMV_CARD	(ERR_BASE+100)
#define ERR_NEED_INSERT		(ERR_BASE+101)
#define ERR_NEED_FALLBACK	(ERR_BASE+102)
#define ERR_NEED_SWIPE		(ERR_BASE+103)
#define ERR_EXIT_APP		(ERR_BASE+990)
#define ERR_NO_DISP			(ERR_BASE+999)

#define OFFSEND_TC		0x01
#define OFFSEND_TRAN	0x02
#define OFFSEND_VOID	0x04

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void SetCommReqField(void);
int TransInit(uchar ucTranType);
int TransCapture(void);
int InstallmentMenu(void);
//int TransSale(uchar ucInstallment);

//MINI ATM(miniatm.c)
int TransMiniAtm(void);
int TransMiniAtmSub(void);

//POWER BUY
int TransPowerBuy(void);
int TransPowerBuySub(void);


int TransManual(void);
int TransInstallment(void);
int TransCash(void);
int TransAuth(uchar ucTranType);
int FinishOffLine(void);
int TranReversal(void);
int OfflineSend(uchar ucTypeFlag);
int TranSaleComplete(void);
int TransRefund(void);
int TransOffSale(void);
int TransVoid(void);
int TransOther(void);
void TransAdjust(void);
void TransEchoTest(void);
int TransSettle(void);
int TransSettleNew(void);

int TransPinVerify(void);

int TransLogonAllAcq(void);


void InfoSaldoBrizzi(void);
void OffBriz(void);
void BrizziCheckBalance(void);
void BrizziOfflinePayment(uchar Diskon);
void BrizziTopUpOnLine(void);
void BrizziAktivasiDeposit(void);
void BrizziInfoDeposit(void);
void BrizziTopUpDeposit(void);
void BrizziPrintLog(void);
void BrizziVoid(void);
int BrizziSettle(void);
void BrizziSummaryReport(uchar FlagDate);
void BrizziDetailReport(uchar FlagDate);

void BrizziRedeem(void);
void BrizziInfoKartu(void);
void BrizziReAktivasi(void);
void BrizziPrnLastTrans(void);
void BrizziRePrnSpecTrans(void);







void INIT_BRIZZI_EZPROX(void);
void SALE_BRIZZI_EZPROX(void);
void TestBrizziOfflinePayment(void);




uchar SAMPrepair(uchar Display);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _TRANPROC_H

// end of file
