/*
blowfish.h:  Header file for blowfish.c

Copyright (C) 1997 by Paul Kocher

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


See blowfish.c for more information about this file.
*/

#include "sys.h"
  
typedef struct {
  unsigned long P[16 + 2];
  unsigned long S[4][256];
} BLOWFISH_CTX;

void Blowfish_Init(BLOWFISH_CTX *ctx, unsigned char *key, int keyLen);
void Blowfish_Encrypt(BLOWFISH_CTX *ctx, unsigned long *xl, unsigned long *xr);
void Blowfish_EncryptChar(BLOWFISH_CTX *ctx, unsigned char *xx);
word Blowfish_EncryptMsg(unsigned char *keys, const byte * msg, word lenmsg, int channel);
void Blowfish_Decrypt(BLOWFISH_CTX *ctx, unsigned long *xl, unsigned long *xr);
word Blowfish_DecryptMsg(unsigned char *keys, const byte * msg, word lenmsg,int channel);
void Blowfish_DecryptChar(BLOWFISH_CTX *ctx, unsigned char *xx);
void XORall(unsigned char *data1, unsigned char *data2, unsigned char *out);

char TurnBlowfish[1];
byte BlowKey[16];           // added

