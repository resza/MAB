#include "posapi.h"
#include "appLib.h"
#include "newPack.h"	//irf, 20121129
#include "string.h"

 uchar ModeDump2=0;	//0 = Print, 1=RS232

#define MSG_TYPE_LENGTH	2
#define MSG_BITMAP_LENGTH	8

#define PACK_NUMERIC		0x01
#define PACK_TRACKX			0x02
#define PACK_ALPHANUM		0x03
#define PACK_ALPHANUMS		0x04
#define PACK_BITS			0x05

#define PACK_LLVAR			0x01
#define PACK_LLLVAR			0x02
#define PACK_MMDDhhmmss	0x03
#define PACK_MMDD			0x04
#define PACK_YYMM			0x05
#define PACK_hhmmss		0x06

//#define uint8	BYTE
//#define uint	unsigned

typedef struct BITD
{
	unsigned char bitn;				/* bit number */
	unsigned char format;			/* bit format */
	unsigned char attrib;			/* attribute */
	unsigned char max;				/* max width */
}
BITD;

const BITD bittab[] =
    {
        { 1,	0,					PACK_BITS, 		1     },
        { 2,	PACK_LLVAR,			PACK_NUMERIC, 	20 	},
        { 3,	0,					PACK_NUMERIC, 	6  	},
        { 4,	0,					PACK_NUMERIC, 	12 	},
        { 5,	0,					PACK_NUMERIC, 	12 	},
        { 6,	0,					PACK_NUMERIC, 	12 	},
        { 7,	PACK_MMDDhhmmss,	PACK_NUMERIC, 	10 	},
        { 8,	0,					PACK_NUMERIC, 	8  	},
        { 9,	0,					PACK_NUMERIC, 	8  	},
        { 10,	0,					PACK_NUMERIC, 	8  	},
        { 11,	0,					PACK_NUMERIC, 	6  	},
        { 12,	PACK_hhmmss,		PACK_NUMERIC, 	6  	},
        { 13,	PACK_MMDD,			PACK_NUMERIC, 	4  	},
        { 14,	PACK_YYMM,			PACK_NUMERIC, 	4  	},
        { 15,	PACK_MMDD,			PACK_NUMERIC, 	4  	},
        { 16,	PACK_MMDD,			PACK_NUMERIC, 	4  	},
        { 17,	PACK_MMDD,			PACK_NUMERIC, 	4  	},
        { 18,	0,					PACK_NUMERIC, 	4  	},
        { 19,	0,					PACK_NUMERIC, 	3  	},
        { 20,	0,					PACK_NUMERIC, 	3  	},
        { 21,	0,					PACK_NUMERIC, 	3  	},
        { 22,	0,					PACK_NUMERIC, 	3  	},
        { 23,	0,					PACK_NUMERIC, 	3  	},
        { 24,	0,					PACK_NUMERIC, 	3  	},
        { 25,	0,					PACK_NUMERIC, 	2  	},
        { 26,	0,					PACK_NUMERIC, 	2  	},
        { 27,	0,					PACK_NUMERIC, 	1  	},
        { 28,	0,					PACK_NUMERIC, 	8  	},
        { 29,	0,					PACK_NUMERIC, 	8  	},
        { 30,	0,					PACK_NUMERIC, 	8  	},
        { 31,	0,					PACK_NUMERIC, 	8  	},
        { 32,	PACK_LLVAR,			PACK_NUMERIC, 	11 	},
        { 33,	PACK_LLVAR,			PACK_NUMERIC, 	11 	},
        { 34,	PACK_LLVAR,			PACK_NUMERIC, 	28 	},
        { 35,	PACK_LLVAR, 			PACK_TRACKX, 	37 	},
        { 36,	PACK_LLLVAR, 		PACK_NUMERIC, 	104	},
        { 37,	0,					PACK_ALPHANUM, 	12 	},
        { 38,	0,					PACK_ALPHANUM, 	6  	},
        { 39,	0,					PACK_ALPHANUM, 	2  	},
        { 40,	0,					PACK_ALPHANUM, 	3  	},
        { 41,	0,					PACK_ALPHANUMS,	8  	},
        { 42,	0,					PACK_ALPHANUMS, 15 	},
        { 43,	0,					PACK_ALPHANUMS, 40 	},
        { 44,	PACK_LLVAR,			PACK_ALPHANUMS, 0  	},
        { 45,	PACK_LLVAR,			PACK_ALPHANUM, 	76 	},
        { 46,	PACK_LLLVAR,		PACK_ALPHANUM, 	255	},
        { 47,	PACK_LLLVAR,		PACK_ALPHANUM, 	255	},
        { 48,	PACK_LLLVAR,		PACK_ALPHANUM, 	255	},
        { 49,	0,				PACK_ALPHANUM, 	3	},
        { 52,	0,					PACK_BITS, 		64 	},
        { 54,	PACK_LLLVAR,		PACK_ALPHANUM, 	0  	},
        { 55,	PACK_LLLVAR,		PACK_ALPHANUMS, 255	}, // SW 20041026 - EMV DE55
        { 56,	PACK_LLLVAR,		PACK_ALPHANUMS, 0  	},
        { 57,	PACK_LLLVAR,		PACK_ALPHANUMS, 0  	},
	{ 58,	PACK_LLLVAR,		PACK_ALPHANUMS, 0  	},
	{ 59,	PACK_LLLVAR,		PACK_ALPHANUMS, 0  	},
	{ 60,	PACK_LLLVAR,		PACK_ALPHANUMS, 0  	},
        { 61,	PACK_LLLVAR,		PACK_ALPHANUMS, 0  	},
        { 62,	PACK_LLLVAR,		PACK_ALPHANUMS, 0  	},
        { 63,	PACK_LLLVAR,		PACK_ALPHANUMS, 0  	},
        { 64,	0,					PACK_BITS, 		64 	},
        { 255,	0,					0, 				0, 	}
    };

static unsigned char *bptr;
static unsigned char *bitmap;
static unsigned char *BitPointers[129];
const unsigned char bit_set_bits[] =
    {
        0x80, 	// = 1000 0000
        0x40, 	// = 0100 0000
        0x20, 	// = 0010 0000
        0x10, 	// = 0001 0000
        0x08, 	// = 0000 1000
        0x04, 	// = 0000 0100
        0x02, 	// = 0000 0010
        0x01	// = 0000 0001
    };


unsigned char pckBitTest(unsigned char pbitno)
{
	return((bitmap[(unsigned char)((pbitno-1) >> 3)] << (unsigned char)((pbitno-1) % 8)) & 0x80);
}

void pckBitSet(unsigned char bitnum)
{
	bitmap[(unsigned char)((bitnum - 1) >> 3)] |= bit_set_bits[(unsigned char)((bitnum - 1) % 8)];
}


void pckInitPack(unsigned int msg_type, unsigned char *buf)
{
//	bintobcd(msg_type, buf, MSG_TYPE_LENGTH * 2);
	PubLong2Bcd(msg_type, MSG_TYPE_LENGTH, buf);
	bitmap 	= buf + MSG_TYPE_LENGTH;
	memset(bitmap, 0x00, 8);
	bptr 	= bitmap + MSG_BITMAP_LENGTH;
	memset(BitPointers, (int)NULL, sizeof(BitPointers));
}


unsigned char *pckPack(unsigned char bitn, void *buf, unsigned int len)
{
	unsigned char i;

	for(i=0; bittab[i].bitn != bitn && bittab[i].bitn != 255; i++)
		;

	if( bittab[i].bitn == 255 )
		return(NULL);

	pckBitSet(bitn);
	BitPointers[bitn] = bptr;

	switch(bittab[i].attrib)
	{
		case PACK_NUMERIC :
			switch(bittab[i].format)
			{
				case PACK_LLLVAR:
//					bintobcd((ulong)len, bptr, 4);
					PubLong2Bcd((unsigned long)len, 2, bptr);
					bptr += 2;
					break;

				case PACK_LLVAR:
//					bintobcd((ulong)len, bptr, 2);
					PubLong2Bcd((unsigned long)len, 1, bptr);
					++bptr;
					break;
			}

			memcpy(bptr, buf, (len + 1) / 2);
			bptr += (len + 1) / 2;

			if( bitn == 2 && (len % 2))
				*(bptr - 1) |= 0x0f;

			break;

		case PACK_TRACKX :
//			bintobcd((ulong)len, bptr, 2);
			PubLong2Bcd((unsigned long)len, 1, bptr);

			++bptr;
			memcpy(bptr, buf, (len + 1) / 2);
			bptr += (len + 1) / 2;
			break;

		case PACK_ALPHANUM :
		case PACK_ALPHANUMS :
			if (bittab[i].format == PACK_LLLVAR)
			{
//				bintobcd((ulong)len, bptr, 4);
				PubLong2Bcd((unsigned long)len, 2, bptr);
				
				bptr += 2;
			}
			else if (bittab[i].format == PACK_LLVAR)
			{
//				bintobcd((ulong)len, bptr, 2);
				PubLong2Bcd((unsigned long)len, 1, bptr);
				++bptr;
			}

			memcpy(bptr, buf, len);
			bptr += len;
			break;

		case PACK_BITS :
			memcpy(bptr, buf, 8);
			bptr += 8;
			break;
	}

	return(bptr);
}

unsigned char *pckUnpack(unsigned char *buf)
{
	unsigned char i, j, NumberOfFields;
	unsigned int len;

	bitmap 	= buf + MSG_TYPE_LENGTH;
	if(pckBitTest(1))//Check for Extended Bitmap
	{
		bptr 			= bitmap + (MSG_BITMAP_LENGTH * 2);
		NumberOfFields 	= 129;
	}
	else
	{
		bptr 			= bitmap + MSG_BITMAP_LENGTH;
		NumberOfFields 	= 65;
	}

	memset(BitPointers, (int)NULL, sizeof(BitPointers));

	for (i = 1; i < NumberOfFields; i++)
	{
		if (pckBitTest(i))
		{
			for (j=0; bittab[j].bitn != 255; j++)
				if (bittab[j].bitn == i)
					break;

			if (bittab[j].bitn == 255)
				return NULL;

			BitPointers[i] = bptr;

			switch(bittab[j].attrib)
			{
				case PACK_NUMERIC :
					if (bittab[j].format == PACK_LLLVAR)
					{
//						len = (uint)bcdtobin(bptr, 4);
						len = (unsigned int)PubBcd2Long(bptr, 2, NULL);
						bptr += 2;
					}
					else if (bittab[j].format == PACK_LLVAR)
					{
//						len = (uint)bcdtobin(bptr, 2);
						len = (unsigned int)PubBcd2Long(bptr, 1, NULL);
						++bptr;
					}
					else
						len = bittab[j].max;

					bptr += (len + 1) / 2;

					break;

				case PACK_TRACKX :
//					len = (uint)bcdtobin(bptr, 2);
					len = (unsigned int)PubBcd2Long(bptr, 1, NULL);
					++bptr;
					bptr += (len + 1) / 2;
					break;

				case PACK_ALPHANUM :
				case PACK_ALPHANUMS :
					if (bittab[j].format == PACK_LLLVAR)
					{
//						len = (uint)bcdtobin(bptr, 4);
						len = (unsigned int)PubBcd2Long(bptr, 2, NULL);
						bptr += 2;
					}
					else
					{
						if (bittab[j].format == PACK_LLVAR)
						{
//							len = (uint)bcdtobin(bptr, 2);
							len = (unsigned int)PubBcd2Long(bptr, 1, NULL);
							++bptr;
						}
						else
							len = bittab[j].max;

					}
					bptr += len;
					break;

				case PACK_BITS :
					bptr += 8;
					break;
			}
		}
		else
			BitPointers[i] = NULL;
	}
	return(bptr);
}


unsigned char *pckGetBitValue(unsigned char BitNumber)
{
	return BitPointers[BitNumber];
}

unsigned char *pckGetPackPosition(void)
{
	return bptr;
}


void pckDumpPrint(char *txt)
{
	//debugPrint(nvmFlagIsoDebugGet() == 1, txt);
		PrnStr("%s\n", txt);
}

void pckDumpBits(unsigned char bit, unsigned int Length, unsigned char * data)
{
	char tg[1024];
	unsigned int i;
	unsigned char n;
	n = 0;
	memset(tg, 0x00, sizeof(tg) );
	sprintf(tg, "%02d|", bit);
	for(i=0; i<Length; i++)
	{
		sprintf(&tg[n*3+3], "%02X ", *(unsigned char *)&data[i]);
		if ( n == 9)
		{
			pckDumpPrint(tg);
			n = 0;
			memset(tg, 0x00, sizeof(tg) );
			memset(tg, 0x20, 3);
		}
		else
			n++;
	}
	if (n != 0)
		pckDumpPrint(tg);
}

void pckDumpNumeric(unsigned char bit, unsigned int Length, unsigned char * data)
{
	pckDumpBits(bit, (Length + 1) / 2, data);
}

void pckDumpASCII(unsigned char bit, unsigned int Length, unsigned char * data)
{
	char mbstr[50];
	unsigned int i, j, count;
	sprintf(mbstr, "%02d|", bit);
	for (i=0; i < Length; i+=29)
	{
		if (i + 29 < Length)
			count = 29;
		else
			count = Length - i;

		for (j=0; j < count; j++)
		{
			if (data[i+j] == 0x0A)
				mbstr[j+3] = 0x40;
			else
				mbstr[j+3] = data[i+j];
		}
		mbstr[j+3] = 0x00;
		pckDumpPrint(mbstr);
		memset(mbstr, 0x20, 3);
	}
}

void pckIsoDumpMessageType( unsigned int d )
{
	char tmp[24];
	sprintf(tmp, "MESSAGE TYPE: %04d ", d);
	pckDumpPrint(tmp);
}


void pckIsoDump8583(unsigned char *buf )
{
	unsigned char 	i, j, NumberOfFields;
	unsigned int 	len;

	PrnInit();
	PrnFontSet(0, 0);
	PrnSpaceSet(1, 2);

	//pckIsoDumpMessageType((uint)bcdtobin( buf, 4));
	pckIsoDumpMessageType((unsigned int)PubBcd2Long(buf, 2, NULL));

	bitmap 	= buf + MSG_TYPE_LENGTH;
	if(pckBitTest(1))//Check for Extended Bitmap
	{
		bptr 			= bitmap + (MSG_BITMAP_LENGTH * 2);
		NumberOfFields 	= 129;
	}
	else
	{
		bptr 			= bitmap + MSG_BITMAP_LENGTH;
		NumberOfFields 	= 65;
	}

	for (i = 1; i < NumberOfFields; i++)
	{
		if (pckBitTest(i))
		{
			for (j=0; bittab[j].bitn != 255; j++)
				if (bittab[j].bitn == i)
					break;

			if (bittab[j].bitn == 255)
				return;


			switch(bittab[j].attrib)
			{
				case PACK_NUMERIC :
					if (bittab[j].format == PACK_LLLVAR)
					{
//						len = (uint)bcdtobin(bptr, 4);
						len = (unsigned int)PubBcd2Long(bptr, 2, NULL);
						bptr += 2;
					}
					else if (bittab[j].format == PACK_LLVAR)
					{
//						len = (uint)bcdtobin(bptr, 2);
						len = (unsigned int)PubBcd2Long(bptr, 1, NULL);
						++bptr;
					}
					else
						len = bittab[j].max;
					pckDumpNumeric(i, len, bptr);
					bptr += (len + 1) / 2;
					break;

				case PACK_TRACKX :
//					len = (uint)bcdtobin(bptr, 2);
					len = (unsigned int)PubBcd2Long(bptr, 1, NULL);
					++bptr;
					pckDumpNumeric(i, len, bptr);
					bptr += (len + 1) / 2;
					break;

				case PACK_ALPHANUM :
				case PACK_ALPHANUMS :
					if (bittab[j].format == PACK_LLLVAR)
					{
//						len = (uint)bcdtobin(bptr, 4);
						len = (unsigned int)PubBcd2Long(bptr, 2, NULL);
						bptr += 2;
					}
					else
					{
						if (bittab[j].format == PACK_LLVAR)
						{
//							len = (uint)bcdtobin(bptr, 2);
							(unsigned int)PubBcd2Long(bptr, 1, NULL);
							++bptr;
						}
						else
							len = bittab[j].max;

					}
					if ((i == 48) ||
						(bittab[j].bitn == 54) ||
					        (bittab[j].bitn == 55) ||
					        (bittab[j].bitn == 56) ||
					        (bittab[j].bitn == 57) ||
					        (bittab[j].bitn == 58) ||
					        (bittab[j].bitn == 59) ||
					        (bittab[j].bitn == 60) ||
					        (bittab[j].bitn == 61))
						pckDumpBits(i, len, bptr);
					else
						pckDumpASCII(i, len, bptr);
					
					bptr += len;
					break;

				case PACK_BITS :
					pckDumpBits(i, 8, bptr);
					bptr += 8;
					break;
			}
		}
	}
	PrnStart();
}
