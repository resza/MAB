#ifndef _FARSI_H_
#define _FARSI_H_

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif
#ifndef ushort
#define ushort unsigned short
#endif

#define     SHIFT_RIGHT             0x00
#define     SHIFT_CENTER            0x01
#define     SHIFT_LEFT              0x02

#define     EFT_REVERSE             0x80

#define     AR_SUCCESS              0x00
#define     AR_OPENFILE_ERROR       0xFF
#define     AR_READFILE_ERROR       0xFE
#define     AR_PARA_ERROR           0xFD



uchar ArInit(uchar PrnFontID);
uchar ArStop();
uchar ArShowString(uchar lineno, uchar Mode, uchar *str);
uchar ArPrintString(uchar Mode, uchar *str);
uchar ArChgNumType(uchar type);

#endif
