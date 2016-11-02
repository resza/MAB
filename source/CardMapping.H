#ifndef _CARDMAPPING_H
#define _CARDMAPPING_H
#include <miltypes.h>


#define TRUE					1		// TRUE
#define FALSE					0		// FALSE


#define WRITE					1		// Write to non-value block
#define DECREASE				2		// Decrease value block


/*
	Description:
	This structure is used to hold the card information after being parsed.

	
*/
typedef struct CardData {
	char				CardIsoNo[19];		// Numeric	, 18 decimal digits
	char 				BinId[7];			// Numeric	, 6 decimal digits
	char 				ExpDate[9];			// Date		, yyyyMMdd
	int32  				TransNo;			// Numeric	, 0 to 64000
	int32 				PurseBalance;		// Numeric	, 0 to 1000000
	char				IssuerId[3];		// Numeric	, 2 decimal digits
	char				ProductId[4];		// Numeric	, 3 decimal digits
	uint8				BlackListFlag;		// Boolean	, TRUE=1, FALSE=0
	uint8				SoldFlag;			// Boolean	, TRUE=1, FALSE=0
	uint8 				CardType;			// Char
	uint8				UserCategoryFlag;	// Char
	uint8				DataFormatVerNo;	// Char
} CardDataStruct;


/*
	Description:
	This structure is used to hold the information of data to write to card.
	If OpType is equal to WRITE, then EDC must write the RawData to the block as normal block write operation.
	If OpType if equal to DECREASE, then EDC must decrease the value block by ValueData and validates the decreased value to match the NewValueData.

	Usage in functions:
	- PerformSaleTrx
*/
typedef struct WriteData{
	uint8				Sector;				// Sector No.
	uint8				Block;				// Block No.
	uint8				OpType;				// Operation Type	, WRITE=1, DECREASE=2
	uint8				RawData[16];		// Raw Data to Write to Card, 16 bytes in 1 block
	uint32				ValueData;			// Value to DECREASE
	uint32				NewValueData;		// New Value after DECREASE That Should be Checked
} WriteDataStruct;


/*
	Descriptions:
	This function is called to set card manufacturer S/N that is used for the current trx

	Parameters:
	- INPUT		mfgNo					Card manufacturer S/N

	Return Value:
	- TRUE
	- FALSE

*/
uint8 SetManufactureNo(uint32 mfgNo);


/*
	Descriptions:
	This function is called to get the block map that identifies which block to read for each sector of card.

	Parameters:
	- INPUT		blockMap							Flag of blocks to read from card for each sector.

		Block Map is an array of uint8 with length 16 represents 16 sectors in the card.
		Index of the array represents sector no.
		Each item (in bits) in the array represents blocks to read in each sector in card.
		e.g. 	blockMap[1] 			--> represents sector 1.
				blockMap[1] = 0x05 	--> 0x14 = 0000 0101 meaning that block #0 & block #2 must be read

	Return Value:
	- TRUE
	- FALSE

*/
uint8 GetBlockMap(uint8 *blockMap);


/*
	Descriptions:
	This function is called to parse the card blocks' raw data.

	Parameters:
	- OUTPUT 	cardData								Parsed card data, see CardDataStruct
	- INPUT		rawBlockData							Raw block data read from card

		Raw Block Data is an array of uint8 with dimension 64x16.
		64 of the dimension represents 64 blocks in the card (16 sectors * 4 blocks).
		16 of the dimension represents 16 bytes in each block.
		For blocks that has been read from card based-on the block map,
		EDC will fill-in the block into the array. 
		The index is based-on 0 to 63 where index of sector 0 block 0 is 0 and index of sector 15 block 4 is 63.
	
	Return Value:
	- TRUE
	- FALSE

*/
uint8 ParseCardData(CardDataStruct *cardData, uint8 rawBlockData[64][16]);


/*
	Descriptions:
	This function is called if a sale trx is performed in order to:
	- Generates blocks to update to the card
	- Generates the raw data to feed to SAM to calculate trx signature

	Parameters:
	- OUTPUT 	writeBlock								List of blocks to update to card
	- OUTPUT 	writeBlockLength						Number of blocks to update to card
	- OUTPUT	signatureData		X(57)				Raw data to calculate trx signature by SAM
	- INPUT		merchantId			X(15)				Merchant ID
	- INPUT		terminalId			X(8)				Terminal ID
	- INPUT		transDate			yyyyMMddHHmmss		Trx date
	- INPUT		amount									Trx amount (in cents)
	- INPUT		spId				X(2)				Service Provider ID

	Return Value:
	- TRUE
	- FALSE

*/
uint8 PerformSaleTrx(WriteDataStruct *block, uint8 *blockLength ,char *signatureData, char *merchantId, char *terminalId, char *transDate, uint32 amount, char *spId);


#endif	// _CARDMAPPING_H
