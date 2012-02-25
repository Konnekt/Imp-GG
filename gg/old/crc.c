unsigned long crcTable[256];

void crc_init() 
{
	unsigned long crc, poly;
	int i, j;
	poly = 0xEDB88320L;
	for (i = 0; i < 256; i++)
	{
		crc = i;
		for (j = 8; j > 0; j--)
		{
			if (crc & 1)
			{
				crc = (crc >> 1) ^ poly;
			} 
			else
			{
				crc >>= 1;
			}
		}
		crcTable[i] = crc;
	}
}
