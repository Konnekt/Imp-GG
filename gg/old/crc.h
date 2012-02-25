#define CRC_START(crc) 		(crc)=~0
#define CRC_NEXT(crc, byte)  \
	(crc)=((((crc)>>8) & 0xFFFFFF) ^ crcTable[((crc) ^ (byte)) & 0xFF])
#define CRC_VALUE(crc) 	(~(crc))

extern unsigned long crcTable[256];
void crc_init();

/*
 * crc_init();
 * unsigned long tmp;
 * CRC_START(tmp);
 * CRC_NEXT(tmp, 'a');
 * CRC_NEXT(tmp, 'b');
 * CRC_NEXT(tmp, 'c');
 * printf("%08x\n", CRC_VALUE(crc));
 */
