#ifndef __BRIZZI_H__
#define __BRIZZI_H__

#define MODE_CHECK_LAST_TRX_DATE	0
#define MODE_CHECK_AKUM_DEBIT		1

typedef struct _CI_HEADER 
{
	uchar      	BRI[3];
	uchar	Card_Number[8];
	uchar	Issue_Date[3];
	uchar	Expiry_Date[3];
	uchar	Cabang_Issue[2];
	uchar	Type_Kartu_Bisnis[2];
	uchar	Model_Kartu[2];
}CI_HEADER;


typedef struct _CI_STATUS 
{
	uchar      	Activation_Date[3];
	uchar	Status[2];
	uchar	Interoperability[27];
}CI_STATUS;

typedef struct _LastTrxAkumDebit 
{
	uchar      	LastTrxDate[3];
	uchar	AkumulasiDebit[4];
}LAST_TRX_AKUM_DEBIT;

void BrizziClearCumDebit(void);


#endif
