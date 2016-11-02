
/****************************************************************************
NAME
    password.h - 定义密码输入、检查模块

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    shengjx     2006.09.12      - created
****************************************************************************/

#ifndef _PASSWORD_H
#define _PASSWORD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ResetPwdAll(void);
uchar PasswordBank(void);
uchar PasswordTerm(void);
uchar PasswordMerchant(void);
uchar PasswordVoid(void);
uchar PasswordRefund(void);
uchar PasswordAdjust(void);
uchar PasswordSettle(void);

void ModifyPasswordBank(void);
void ModifyPasswordTerm(void);
void ModifyPasswordMerchant(void);
void ModifyPasswordVoid(void);
void ModifyPasswordRefund(void);
void ModifyPasswordAdjust(void);
void ModifyPasswordSettle(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _PASSWORD_H

// end of file
