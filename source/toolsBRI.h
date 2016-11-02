#ifndef _TOOLSBRI_H
#define _TOOLSBRI_H

#ifdef BANK_BRI
void Function_11(void);
void Function_22(void);
void InfoMerchant(void);
void BatchStan(void);
int GetPINCardService(void);
int GetCardCS(uchar ucMode);
int InputUang(void);
int InputNominal(void);
int PilihJenisRekening();
int redemEnterMonth(void);
void CheckMenu(void);
void ViewBrizzi();
void SetAkumDebitBrizzi();
void SetMinimumSaldoBrizzi();
void SetBrizziDisc(void);
//void PrintTagEMV(void);
//void PrintTagEMVTes(void);
uchar ViewDiskonBrizzi(char *pszDesignation, uchar *pszAmount);
#endif
void PrintTagEMV(void); //for test 4212
void PrintTagEMVTes(void);
void displayResponse(void);
void DefaultGagal(void);
void SetBrizzi(void);
void MerchantMenu(void);
void OnOffMenu(void);

// diki add for mini atm bukopin
int PilihJenisRekeningBkpn();

int GetPINCardServiceBkpn(void);


#endif


