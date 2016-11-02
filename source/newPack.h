#ifndef  __NEWPACK_H_
#define  __NEWPACK_H_


unsigned char	pckBitTest(unsigned char pbitno ); 
void	pckBitSet(unsigned char bitnum ); 
void	pckInitPack( unsigned int msg_type , unsigned char *buf ); 
unsigned char	*pckPack( unsigned char bitn , void *buf , unsigned int len ); 
unsigned char 	*pckUnpack(unsigned char *buf);
unsigned char 	*pckGetBitValue(unsigned char BitNumber);
unsigned char 	*pckGetPackPosition(void);
void 	pckIsoDump8583(unsigned char *buf );
#endif 
