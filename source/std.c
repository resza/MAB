/** \file
 * Various standard algorythms
 *
 * Subversion reference
 *
 * $HeadURL: https://svn.ingenico.com/UTA/core/utaYYMMDD/pri/std.c $
 *
 * $Id: std.c 2478 2009-10-28 16:39:49Z abarantsev $
 */

#include "global.h"

#ifdef __TEST__
#undef trcOpt
#define trcOpt (TRCOPT & BIT(trcSTD))
#endif

/** Returns 1 if a<b<c and 0 otherwise.
 * \param  a,b,c (I) Some numbers
 * \return
 *    - 1 if a<b<c and 0 otherwise.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
byte isSorted(word a, word b, word c) {
    if(b < a)
        return 0;
    if(c < b)
        return 0;
    return 1;
}

static void negStr(char *str) { //convert str into it supplementary
    byte i, p;
    int n;
    char tmp[16 + 1];
    char dst[16 + 1];

    strcpy(tmp, str);
    fmtPad(tmp, -16, '0');      //left pad by zeroes to length 16

    i = 16;
    p = 0;
    memset(dst, 0, 16 + 1);
    while(i--) {
        n = tmp[i] - '0';

        n = -n;
        n -= p;
        if(n < 0) {
            n += 10;
            p = 1;
        } else
            p = 0;

        dst[i] = n + '0';
    }

    strcpy(str, dst);
}

static void neg2sup(char *str) {    //if negative, convert it to supplementary
    if(str[0] != '-')
        return;                 //non-negative - nothing to do
    *str = '0';
    negStr(str);
}

static void sup2neg(char *str) {    //if supplementary, add '-' sign
    byte i;
    char tmp[16 + 1];

    if(strlen(str) < 16)
        return;                 //non-negative - nothing to do
    if(str[0] < '5')
        return;                 //non-negative - nothing to do
    negStr(str);

    i = 0;
    while(str[i]) {             //the leading zeroes are to be skipped
        if(str[i] != '0')
            break;
        i++;
    }
    strcpy(tmp, str + i);
    strcpy(str, tmp);

    fmtPad(str, -(int) (strlen(str) + 1), '-'); //add leading '-'
}

static byte addSup(char *dst, const char *arg1, const char *arg2) { //add two values converted into supplementaries if necessary
    byte i, n, p, l1, l2;
    char tmp[16 + 1];
    char tmp1[16 + 1];
    char tmp2[16 + 1];

    l1 = strlen(arg1);
    l2 = strlen(arg2);
    
    strcpy(tmp1, arg1);
    fmtPad(tmp1, -16, '0');     //left pad by zeroes to length 16

    strcpy(tmp2, arg2);
    fmtPad(tmp2, -16, '0');     //left pad by zeroes to length 16   

    i = 16;
    p = 0;
    memset(tmp, 0, 16 + 1);
    while(i--) {                //perform the addition
        n = tmp1[i] - '0';

        n += tmp2[i] - '0';

        n += p;
        if(n >= 10) {
            n -= 10;
            p = 1;
        } else
            p = 0;

        tmp[i] = n + '0';
    }

    i = 0;
    while(tmp[i]) {             //the leading zeroes are to be skipped
        if(tmp[i] != '0')
            break;
        i++;
    }

    if(!*(tmp + i))             //full zero string 
        strcpy(dst, "0");
    else
        strcpy(dst, tmp + i);
//  if(!(*dst)) *dst= '0';
    return strlen(dst);
}

/** Add the content of the arg1 to the content of arg2 as strings of decimal digits and write the result into dst.
 * All the strings should be no more than 16 characters length.
 * The negative value strings should start by ‘-‘.
 *
 * \param dst (O) Destination string 
 * \param       arg1 (I) First argument
 * \param       arg2 (I) Second argument
 *
 * \pre
 *    - dst!=0
 *    - arg1!=0
 *    - arg2!=0
 *    - The destination buffer should be large enough to accept 16-character long string (16+1)
 *
 * \return
 *    - strlen(dst)
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
byte addStr(char *dst, const char *arg1, const char *arg2) {
    byte i;
    char tmp1[16 + 1];
    char tmp2[16 + 1];

    strcpy(tmp1, arg1);
    neg2sup(tmp1);

    strcpy(tmp2, arg2);
    neg2sup(tmp2);

    i = addSup(dst, tmp1, tmp2);
    sup2neg(dst);
    return i;
}

/**
 * Subtract the content of the arg2 from the content of arg1 as strings of decimal digits and write the result into dst.
 * All the strings should be no more than 16 characters length.
 * The negative value strings should start by ‘-‘.
 *
 * \param dst (O) Destination string 
 * \param       arg1 (I) First argument
 * \param       arg2 (I) Second argument
 *
 * \pre 
 *    - dst!=0
 *    - arg1!=0
 *    - arg2!=0
 *    - The destination buffer should be large enough to accept 16-character long string (16+1)
 *
 * \return  
 *    - strlen(dst)
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
byte subStr(char *dst, const char *arg1, const char *arg2) {
    byte i;
    char tmp1[16 + 1];
    char tmp2[16 + 1];

    strcpy(tmp1, arg1);
    neg2sup(tmp1);

    strcpy(tmp2, arg2);
    neg2sup(tmp2);
    negStr(tmp2);

    i = addSup(dst, tmp1, tmp2);
    sup2neg(dst);
    return i;
}

/**
 * \todo To be documented
 */
card stdPow(int num, int exp) {
    int i;
    card c = 1;

    for (i = 1; i <= exp; i++, c *= num);
    return c;
}

/** Calculate checksum lrc for the buffer buf of length len.
 * LRC is the result of XORing of all the bytes in buf.
 * This checksum is used in BASE24-like messages.
 * \param buf (O) String on which the LRC should be calculated
 * \param       len (I) Number of bytes in the buf
 * \pre 
 *    - buf!=0
 * \return  
 *    - lrc checksum calculated
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
byte stdLrc(const byte * buf, int len) {
    byte lrc;

    lrc = 0;
    while(len--)
        lrc ^= *buf++;
    return lrc;
}

/** Calculate checksum lrc for the buffer buf of length len.Buf is track2 data including STX and ETX.
 * LRC is the result of XORing of all the bytes in buf.
 * bit 7,6,and 5 must be kept 011 according to APACS 30 common attachment and bit 8 set to zero to ignore parity. 
 * \param buf (I) String on which the LRC should be calculated
 * \param len (I) Number of bytes in the buf
 * \pre 
 *    - buf!=0
 * \return  
 *    - lrc checksum calculated
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcnl0028.c
 */
byte trk2Lrc(const byte * buf, int len) {
    byte lrc;
    byte a = 0;

    lrc = 0;
    while(len--) {
        a = *buf;
        lrc ^= a;
        buf++;
    }
    lrc = lrc & 0x0F;
    lrc = lrc + 0x30;
    return lrc;

}

/** Extract account number from pan for ANSIX 9.8 pin calculation.
 * It is the last 12 pan digits not including Luhn code preceded by 0000 account
 * is converted into binary.
 * \param       acc (O) Account to be calculated
 * \param       pan (I) Original pan number
 * \pre
 *    - acc!=0
 *    - pan!=0
 *    - the length of pan should be greater than or equal to 12
 * \return
 *    - 8 if OK.
 *    - negative otherwise (short pan).
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
#define lenAcc 12
byte stdGetAcc(byte * acc, const char *pan) {
    int ret;
    byte idx;
    char asc[lenAcc + 1];       //account in ASCII

    memset(acc, 0, 8);

    idx = strlen(pan) - 1;      //non include Luhn code
    if(idx < lenAcc)
        return 0;               //pan too short

    idx -= lenAcc;
    ret = fmtSbs(asc, pan, idx, lenAcc);
    
    ret = hex2bin(acc + 2, asc, lenAcc / 2);
    
    return (byte) ret;
}

/** Verify PAN according to the Luhn check digit, algorithm ANSI  X4.13
 * \param pan (I) pan number
 * \pre
 *    - pan!=0
 * \return
 *    - 1 if OK.
 *    - 0 otherwise.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcab0053.c
 */
byte stdLuhnCheck(const char *pan) {
    byte idx;                   //current character index
    byte len;                   //pan length
    byte sum;                   //check sum
    byte odd;                   //odd/even flag
    byte tmp;

    
    len = strlen(pan);
    if(!len)
        return 0;               //empty pan
    idx = len;
    sum = 0;
    odd = 1;
    while(idx--) {
        if(pan[idx] < '0')
            return 0;           //invalid character
        if(pan[idx] > '9')
            return 0;           //invalid character
        tmp = pan[idx] - '0';
        if(!odd) {
            tmp += tmp;
            tmp = (tmp % 10) + (tmp / 10);
        }
        sum += tmp;
        odd = !odd;
    }
    if(sum % 10 == 0)
        return 1;
    return 0;
}

/** Crc16(Xmodem) calculation according to CCITT-1021
 * \todo To be documented
 * \test tcnl0015.C
 */
word stdCrc16(const byte * buf, word len, byte augment_message_for_crc) {

    word crc_val = 0x0000;
    word i = 0;
    word shifter, flag;

    crc_val = 0;
    do {
        for (shifter = 0x80; shifter; shifter >>= 1) {
            flag = ((crc_val) & 0x8000);
            (crc_val) <<= 1;
            (crc_val) |= ((shifter & buf[i]) ? 1 : 0);
            if(flag)
                (crc_val) ^= 0x1021;
        }

    } while(++i < len);
    if(augment_message_for_crc) {
        stdArgMsgCrc16(&crc_val);
    }

    return crc_val;
}

/**
 * \todo To be documented
*/
void stdArgMsgCrc16(word * crc_val) {
    word i = 0, flag;

    for (i = 0; i < 16; i++) {
        flag = (crc_val[0] & 0x8000);
        crc_val[0] <<= 1;
        if(flag)
            crc_val[0] ^= 0x1021;
    }
}

/** XOR len byte of b1 and b2 to obtain dst.
 * \param b1 (I) Binary buffer 1
 * \param b2 (I) Binary buffer 2
 * \param dst (O) Destination buffer containing b1 XOR b2               
 * \param len (I) Len byte to XOR
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\tbx.c
 * \test tcab0057.c
 */
void xor(const byte * b1, const byte * b2, byte * dst, word len) {
    while(len--)
        *dst++ = *b1++ ^ *b2++;
}

/** Set parity bits; par=1..8 is number of parity bit.
 * \param buf (I) Binary buffer to be set
 * \param dim (I) Buffer size
 * \param par (I) Position of the parity bit 1 or 8
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
void stdParity(byte * buf, word dim, byte par) {    //set parity bits
    byte cnt, idx;

    while(dim--) {
        cnt = 0;
        for (idx = 1; idx < 8; idx++) {
            if(idx == par)
                continue;
            if(bitTest(buf, idx))
                cnt++;
        }
        if(cnt % 2)
            bitOff(buf, par);
        else
            bitOn(buf, par);
        buf++;
    }
}

static void justdes2(byte * dst, const byte * src, const byte * key, int ctl) {
    card x, c, d, r, l;
    int i;
    static const card s[64] = { /* S-tables and permutation E combined */
        0xD8D8DBBC, 0xD737D1C1, 0x8B047441, 0x35A9E2FE,
        0x146E9560, 0x8A420CFB, 0xF8FBAF1F, 0xC7B4DD10,
        0x7A97A497, 0x4CFCFA1C, 0x456ADA86, 0xFAC710E9,
        0xE52149EF, 0x338D2004, 0x1E5580F1, 0xE04A2F3D,
        0x870A4E20, 0x28BE9C1F, 0x74D5E339, 0x8240BD00,
        0x6AA1ABC3, 0x3F55E2A8, 0xAF1F56BC, 0x51BB11CF,
        0xB7FC035E, 0xE00307B0, 0x08A3B44B, 0x3F786D67,
        0x09967CBC, 0x45EB7B47, 0xF3683962, 0x9C14C6D2,
        0x16452B42, 0xADDACEBA, 0x58F91ABC, 0x8B68B547,
        0xFAA36659, 0x47BF8901, 0x671AEBA9, 0x30C452AB,
        0x493893E1, 0x72C16866, 0xB7C78574, 0xCD1E6B9A,
        0xB6DCD49C, 0x9822B7FB, 0x89B07E43, 0x77B78644,
        0xA566F5DF, 0xD22D6AC3, 0xAF9A0423, 0x77B71BBC,
        0x81DC043E, 0xC8837314, 0x78659153, 0xAF782C7D,
        0x8C0F78A0, 0x0D3095EF, 0x7A506B8E, 0x8445D610,
        0x5223AB47, 0x724C0C34, 0x45AF54BC, 0x38DBF9CB
    };

/* initial permutations IP and PC1 */
    l = r = c = d = 0;
    i = 7;
    do {
        x = src[i];
        l = l << 1 | (x & 1 << 0) | (x & 1 << 2) << 6 | (x & 1 << 4) << 12 | (x
                                                                              &
                                                                              1
                                                                              <<
                                                                              6)
            << 18;
        r = r << 1 | (x & 1 << 1) >> 1 | (x & 1 << 3) << 5 | (x & 1 << 5) << 11
            | (x & 1 << 7) << 17;
        x = key[i];
        c = c << 1 | (x & 1 << 7) << 17 | (x & 1 << 6) << 10 | (x & 1 << 5) << 3
            | (x & 1 << 4) >> 4;
        d = d << 1 | (x & 1 << 1) << 19 | (x & 1 << 2) << 10 | (x & 1 << 3) <<
            1;
    } while(--i >= 0);
    d |= c & 0x0F;
    c >>= 4;
    i = 24;
/* first round is special: one left shift on encipher, no shift en decipher */
    if(ctl != 0)
        goto startround;
  leftby1:
    c = c << 1 | ((c >> 27) & 1);
    d = d << 1 | ((d >> 27) & 1);
  startround:
/* a round - apply PC2, the S-boxes and permutation E */
    x = (s[((r >> 26 & 62) | (r & 1)) ^
           ((c >> 6 & 32) | (c >> 13 & 16) | (c >> 1 & 8) | (c >> 25 & 4) |
            (c >> 22 & 2) | (c >> 14 & 1))] & 0x00808202) ^ l;
    x ^= s[(r >> 23 & 63) ^
           ((c >> 20 & 32) | (c << 4 & 16) | (c >> 10 & 8) | (c >> 20 & 4) |
            (c >> 6 & 2)
            | (c >> 18 & 1))] & 0x40084010;
    x ^= s[(r >> 19 & 63) ^
           ((c & 32) | (c >> 5 & 16) | (c >> 13 & 8) | (c >> 22 & 4) |
            (c >> 1 & 2) | (c >> 20 & 1))] & 0x04010104;
    x ^= s[(r >> 15 & 63) ^
           ((c >> 7 & 32) | (c >> 17 & 16) | (c << 2 & 8) | (c >> 6 & 4) |
            (c >> 14 & 2) | (c >> 26 & 1))] & 0x80401040;
    x ^= s[(r >> 11 & 63) ^
           ((d >> 10 & 32) | (d & 16) | (d >> 22 & 8) | (d >> 17 & 4) |
            (d >> 8 & 2) | (d >> 1 & 1))] & 0x21040080;
    x ^= s[(r >> 7 & 63) ^
           ((d >> 21 & 32) | (d >> 12 & 16) | (d >> 2 & 8) | (d >> 9 & 4) |
            (d >> 22 & 2)
            | (d >> 8 & 1))] & 0x10202008;
    x ^= s[(r >> 3 & 63) ^
           ((d >> 7 & 32) | (d >> 3 & 16) | (d >> 14 & 8) | (d << 2 & 4) |
            (d >> 21 & 2) | (d >> 3 & 1))] & 0x02100401;
    x ^= s[((r & 31) | (r >> 26 & 32)) ^
           ((d >> 19 & 32) | (d >> 6 & 16) | (d >> 11 & 8) | (d >> 4 & 4) |
            (d >> 19 & 2)
            | (d >> 27 & 1))] & 0x08020820;
    l = r;
    r = x;
/* decide/perform key shifts */
    if((i & 7) == 0) {
        i -= 2;
        if(ctl == 0)
            goto leftby1;
        c = c >> 1 | (c & 1) << 27;
        d = d >> 1 | (d & 1) << 27;
        goto startround;
    }
    if(i != 6) {
        --i;
        if(ctl == 0) {
            c = c << 2 | (c >> 26 & 3);
            d = d << 2 | (d >> 26 & 3);
            goto startround;
        } else {
            c = c >> 2 | (c & 3) << 26;
            d = d >> 2 | (d & 3) << 26;
            goto startround;
        }
    }
/* final swap and permutations IP` */
    i = 7;
    do {
        *dst++ =
            (byte) ((r & 1 << 0) | (r >> 6 & 1 << 2) | (r >> 12 & 1 << 4) |
                    (r >> 18 & 1 << 6) | (l << 1 & 1 << 1) | (l >> 5 & 1 << 3) |
                    (l >> 11 & 1 << 5) | (l >> 17 & 1 << 7));
        l >>= 1;
        r >>= 1;
    } while(--i >= 0);
}

/** Encrypt the binary array src using the key and put the result into the buffer dst.
 * The standard DES algorithm is used for encryption.
 * \param dst (O) Destination binary buffer.
 * \param src (I) Binary buffer to be encrypted.
 * \param key (I) DES encryption key.
 * \pre 
 *    - dst!=0
 *    - src!=0
 *    - key!=0
 * \remark all the buffer are 8 bytes long.
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
void stdDES(byte * dst, byte * src, byte * key) {
    justdes2(dst, src, key, 0);
}

/** Decrypt the binary array src using the key key and put the result into the buffer dst.
 * The standard DES algorithm is used for encryption.
 * \param dst (O) Destination binary buffer.
 * \param       src (I) Binary buffer to be encrypted.
 * \param       key (I) DES decryption key.
 * \pre 
 *    - dst!=0
 *    - src!=0
 *    - key!=0
 * \remark all the buffer are 8 bytes long.
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
void stdSED(byte * dst, byte * src, byte * key) {
    justdes2(dst, src, key, 1);
}

/** Triple DES encryption of  binary array src by double length key;
 * the result is saved into dst.
 * \param       dst (O) Destination binary buffer.
 * \param       src (I) Binary buffer to be encrypted.
 * \param       key (I) Double length DES encryption key.
 * \pre
 *    - dst!=0
 *    - src!=0
 *    - key!=0
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
*/
void stdEDE(byte * dst, byte * src, byte * key) {
    byte tmp[8];

    stdDES(dst, src, key);
    stdSED(tmp, dst, key + 8);
    stdDES(dst, tmp, key);
}

/** Triple DES decryption of  binary array src by double length key;
 * the result is saved into dst.
 * \param dst (O) Destination binary buffer.
 * \param       src (I) Binary buffer to be decrypted.
 * \param       key (I) Double length DES decryption key.
 * \pre
 *    - dst!=0
 *    - src!=0
 *    - key!=0
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
*/
void stdDED(byte * dst, byte * src, byte * key) {
    byte tmp[8];

    stdSED(dst, src, key);
    stdDES(tmp, dst, key + 8);
    stdSED(dst, tmp, key);
}

//Integer hash function for 32 bits based on Robert Jerkins' 32 bit Mix Function.
//http://www.concentric.net/~Ttwang/tech/inthash.htm
static card hashJerkins(card key) {
    key += (key << 12);
    key ^= (key >> 22);
    key += (key << 4);
    key ^= (key >> 9);
    key += (key << 10);
    key ^= (key >> 2);
    key += (key << 7);
    key ^= (key >> 12);
    return key;
}

/** Hash function calculated on a binary buffer
 * \param buf binary buffer to calculate hash
 * \param len buffer length
 * \return  
 *    - output integer hash value.
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcts0002.c
 */
card hashJerkinsBin(const byte * buf, byte len) {
    card ret;
    card *ptr;
    card tmp[256 / sizeof(card)];

    memset(tmp, 0xFF, sizeof(tmp));
    memcpy(tmp, buf, len);

    ret = 0;
    ptr = tmp;
    len += 3;
    len /= 4;
    while(len) {
        ret ^= hashJerkins(*ptr);
        ptr++;
        len--;
    }

    return ret;
}

enum {                          //bmp header structure
    ofsBmpHdrId1,               //Production header ID1
    ofsBmpHdrId2,               //Production header ID2
    ofsBmpHdrId3,               //Production header ID3
    ofsBmpChkSum,               //Checksum of header
    ofsBmpHdrLen,               //Length of header
    ofsBmpInvLen,               //Inverted HdrLen
    ofsBmpHdrVer,               //Header version number
    ofsBmpLen1,                 //BMP len 1 (MSB)
    ofsBmpLen2,                 //BMP len 2
    ofsBmpLen3,                 //BMP len 3
    ofsBmpLen4,                 //BMP len 4 (LSB)
    ofsBmpWdtMsb,               //MSB of Width
    ofsBmpWdtLsb,               //LSB of Width
    ofsBmpHgtMsb,               //MSB of Height
    ofsBmpHgtLsb,               //LSB of Height
    ofsBmpBpp,                  //Bits per pixel
    ofsBmpDatChk,               //BMP Data checksum
    ofsBmpDat                   //data follow
};

word bmpWdt(const byte * bmp) {
    return WORDHL(bmp[ofsBmpWdtMsb], bmp[ofsBmpWdtLsb]);
}
word bmpHgt(const byte * bmp) {
    return WORDHL(bmp[ofsBmpHgtMsb], bmp[ofsBmpHgtLsb]);
}
const byte *bmpDat(const byte * bmp) {
    return bmp + ofsBmpDat;
}

/** Converting three bytes to four characters in Base64 representation.
 * \param inBuf (I) The Data that will be encoded
 * \param inLen (I) The length of the data  
 * \param outBuf (O)The Encoded output buffer
 * \return  
 *    - Length of the Encoded output buffer. 
 * 
 * For the Binary data/printable string conversion, there are several usable methods. One of the 
 * most frequently used is the Base64 algorithm described in the RCF1341 standard. Base64 is 
 * sometimes called Radix64 coding, because it uses the 4 byte transformation system to four "6 bit" 
 * characters. These characters are transformed to a printable form using a special translation table. 
 * 
 * The BASE64 coding system uses the following table:
 * code_table[0..63] = "ABCD...XYZabcd...xyz0123...789+/"
 * 
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcfa0004.c
 */
int Radix64Encode(byte * inBuf, word inLen, byte * outBuf) {
    byte bintoasc[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
    };

    int i;
    unsigned char *p = outBuf;

    // Transform the 3x8 bits to 4x6 bits, as required by base64.
    for (i = 0; i < inLen; i += 3) {
        *p++ = bintoasc[inBuf[0] >> 2];
        *p++ = bintoasc[((inBuf[0] & 3) << 4) + (inBuf[1] >> 4)];
        *p++ = bintoasc[((inBuf[1] & 0xf) << 2) + (inBuf[2] >> 6)];
        *p++ = bintoasc[inBuf[2] & 0x3f];
        inBuf += 3;
    }
    // Pad the result if necessary... 
    if(i == inLen + 1)
        *(p - 1) = '=';
    else if(i == inLen + 2)
        *(p - 1) = *(p - 2) = '=';

    // ...and zero-terminate it.
    *p = '\0';
    return strlen((char *) outBuf);
}

/** Converting four characters in Base64 format to three byte representation. 
 * \param inBuf (I) The encoded input buffer
 * \param outBuf (O)The decoded output buffer
 * \param outLength (O)The length of decoded buffer
 * \return 1 if OK, otherwise negative 
 * 
 * For the Binary data/printable string conversion, there are several usable methods. One of the 
 * most frequently used is the Base64 algorithm described in the RCF1341 standard. Base64 is 
 * sometimes called Radix64 coding, because it uses the 4 byte transformation system to four "6 bit" 
 * characters. These characters are transformed to a printable form using a special translation table. 
 * 
 * The BASE64 coding system uses the following table:
 * code_table[0..63] = "ABCD...XYZabcd...xyz0123...789+/" 
 * 
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcfa0004.c
 */
int Radix64Decode(byte * inBuf, byte * outBuf, word * outLength) {
    byte asctobin[256] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
        0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
        0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
        0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff
    };

    int idx = 0;
    unsigned char c, c2, val = '\0';
    int n = 0;

    for (idx = 0, c = *inBuf; c; c = *(++inBuf), idx = (idx + 1) % 4) {
        if(c == '\n' || c == ' ' || c == '\r' || c == '\t')
            continue;

        if(c == '=') {          // pad character: stop
            if(idx == 1)
                outBuf[n++] = val;
            break;
        }

        if((c = asctobin[(c2 = c)]) == 255)
            return -1;
        //RadixError;

        switch (idx) {
          case 0:
              val = c << 2;
              break;
          case 1:
              val |= (c >> 4) & 3;
              outBuf[n++] = val;
              val = (c << 4) & 0xf0;
              break;
          case 2:
              val |= (c >> 2) & 15;
              outBuf[n++] = val;
              val = (c << 6) & 0xc0;
              break;
          case 3:
              val |= c & 0x3f;
              outBuf[n++] = val;
              break;
        }

    }
    *outLength = n;
    return 1;
}

/** Returns the index of a particular character(if it exists) in an array
 * used by BaseToDec to find face values
 * \param pString (I) The array where in the input character 'search'  is searched
 * \param search (O) The character to be searched
 * \returns the last character to be searched
 * \sa Source: pri\\std.c
 */
static int GetIndex(char *pString, char search) {
    int index = 0;

    while(*pString != (char) 0) //Loop will finish at null character if no match is found
    {
        if(*pString == search)
            break;
        pString++;
        index++;
    }
    return index;
}

/** Returns the decimal equivalent of a number with base less than or equal to 36
 * \param number (I) The number to be converted to decimal
 * \param base (O) The base of the number
 * \returns the last character to be searched
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcma0020.c
 */
int BaseToDec(char *number, card base) {
    int NumLength;
    int PlaceValue = 1, total = 0;
    int i;

    //Symbols used to display a number correctly
    //Numbers over base 10 use letters to represent values over and equal to 10
    //It is able to increase the max no. of bases by adding other symbols

    char symbols[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int MAX_BASE = 36;    //Highest base allowed (make sure there are enough symbols first!)

    if(base < 2 || base > (card) MAX_BASE)
        return 0;               //Failed

    NumLength = strlen(number);

    //Work out the place value of the first digit (base^length-1)
    for (i = 1; i < NumLength; i++) {
        PlaceValue *= base;
    }

    //For each digit, multiply by its place value and add to total
    for (i = 0; i < NumLength; i++) {
        total += GetIndex(symbols, *number) * PlaceValue;
        number++;
        PlaceValue /= base;     //Next digit's place value (previous/base)
    }
    return total;
}

/** One Way Function of binary array dat by simple length key;
 * the result is saved into dst.
 * \param dst (O) Destination binary buffer.
 * \param dat (I) Binary butter to be OWF.
 * \param key (I) Simple length DES encryption key.
 * \pre
 *    - dst!=0
 *    - dat!=0
 *    - key!=0
 * \return non-negative if OK. 
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
*/
int stdOWF(byte * dst, const byte * dat, const byte * key) {
    byte tmpK[8];
    byte tmpD[8];
    byte *owfX = (byte *) "\xA5\xC7\xB2\x82\x84\x76\xA8\x29";
    byte *owfY = (byte *) "\xB5\xE3\x7F\xC5\xD4\xF7\xA3\x93";

    xor(dat, owfX, tmpD, 8);
    xor(key, owfY, tmpK, 8);
    stdParity(tmpK, 8, 8);
    stdDES(dst, tmpD, tmpK);
    xor(dst, tmpD, dst, 8);
    return 1;
}

//Info regarding timestamp processing:
//timestamp definition: http://en.wikipedia.org/wiki/Timestamp
//UNIX time: the number of seconds since 00:00:00 UTC on January 1, 1970 
//The Unix Time "Apocalypse": January 19, 2038 03:14:07 GMT
//Unix Timestamp Converter: http://www.4webhelp.net/us/timestamp.php
//Julian Day Converter: http://www.iasfbo.inaf.it/~mauro/JD/
//Julian Day http://en.wikipedia.org/wiki/Julian_day
//Calendar Algorithms: http://www.tondering.dk/claus/algo.html

/* See http://www.tondering.dk/claus/algo.html
 * date_to_jdn:
 * Calculates the Julian Day Number for a given date.
 * Input parameters:
 *     Calendar style ('j' for JULIAN or 'g' for GREGORIAN)
 *     Year (must be > -4800). The year 1 BC must be given as 0, the
 *         year 2 BC must be given as -1, etc.
 *     Month (1..12)
 *     Day (1..31)
 * Returns:
 *     Julian Day Number
 *
 * Reference: Section 2.16.1 of version 2.9 of the FAQ.
 */
static int date_to_jdn(int year, int month, int day) {
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;

    return day + (153 * m + 2) / 5 + y * 365 + y / 4 - y / 100 + y / 400 -
        32045;
}

/* See http://www.tondering.dk/claus/algo.html
 * jdn_to_date:
 * Calculates the date for a given Julian Day Number.
 * Input parameter:
 *     Calendar style ('j' for JULIAN or 'g' for GREGORIAN)
 *     Julian Day Number
 * Output parameters:
 *     Address of year. The year 1 BC will be stored as 0, the year
 *         2 BC will be stored as -1, etc.
 *     Address of month (1..12)
 *     Address of day (1..31)
 *
 * Reference: Section 2.16.1 of version 2.9 of the FAQ.
 */
static void jdn_to_date(int JD, int *year, int *month, int *day) {
    int a, b, c, d, e, m;

    a = JD + 32044;
    b = (4 * a + 3) / 146097;
    c = a - (b * 146097) / 4;

    d = (4 * c + 3) / 1461;
    e = c - (1461 * d) / 4;
    m = (5 * e + 2) / 153;

    *day = e - (153 * m + 2) / 5 + 1;
    *month = m + 3 - 12 * (m / 10);
    *year = b * 100 + d - 4800 + m / 10;
}

/** Convert Timestamp to DateTime.
 * \param ts (I) Timestamp value to be converted
 * \param YYMMDDhhmmss (O) Buffer to capture the returned DateTime 
 * \header sys\\pri.h
 * \source sys\\std.c
 * \test tcmp0000.c
 *
 */
void ts2dt(card ts, char *YYMMDDhhmmss) {
    card jdnUnix = 2440588;     //julian day number for 01/01/1970
    card spm = 60;              //seconds per minute
    card mph = 60;              //minutes per hour
    card hpd = 24;              //hours per day
    card sph = spm * mph;       //seconds per hour
    card spd = sph * hpd;       //seconds per day
    card days, secs;
    int Y, M, D, h, m, s;

    days = ts / spd;            //number of days since 01/01/1970
    secs = ts % spd;            //number of seconds since midnight

    jdn_to_date(jdnUnix + days, &Y, &M, &D);
    h = secs / sph;
    secs %= sph;
    m = secs / spm;
    s = secs % spm;

    sprintf(YYMMDDhhmmss, "%02d%02d%02d%02d%02d%02d", Y % 100, M, D, h, m, s);
    trcFN("ts2dt: %ul", ts);
    trcFS(" -> %s\n", YYMMDDhhmmss);
}

/** Convert DateTime to Timestamp.
 * \param YYMMDDhhmmss (I) Buffer to capture the returned DateTime 
 * \return timestamp /the number of seconds since first call or since 01/01/1970
 * \header sys\\pri.h
 * \source sys\\std.c
 * \test tcmp0000.c
 *
*/
card dt2ts(const char *YYMMDDhhmmss) {
    int ret;
    card Y, M, D, h, m, s;
    card days, secs;
    const char *ptr;
    card jdnUnix = 2440588;     //julian day number for 01/01/1970
    card spm = 60;              //seconds per minute
    card mph = 60;              //minutes per hour
    card hpd = 24;              //hours per day
    card sph = spm * mph;       //seconds per hour
    card spd = sph * hpd;       //seconds per day

    
    ptr = YYMMDDhhmmss;
    dec2num(&Y, ptr, 2);
    ptr += 2;
    dec2num(&M, ptr, 2);
    ptr += 2;
    dec2num(&D, ptr, 2);
    ptr += 2;
    dec2num(&h, ptr, 2);
    ptr += 2;
    dec2num(&m, ptr, 2);
    ptr += 2;
    dec2num(&s, ptr, 2);

    if(Y > 70)
        Y += 1900;
    else
        Y += 2000;
    days = date_to_jdn(Y, M, D);
    days -= jdnUnix;
    secs = s + m * spm + h * sph;
    ret = days * spd + secs;

    trcFS("dt2ts: %s", YYMMDDhhmmss);
    trcFN(" -> %ul\n", ret);
    return ret;
}
