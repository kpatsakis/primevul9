unsigned sc_crc32(const unsigned char *value, size_t len)
{
	size_t ii, jj;
	unsigned long crc;
	unsigned long index, long_c;

	if (!sc_CRC_tab32_initialized)   {
		for (ii=0; ii<256; ii++) {
			crc = (unsigned long) ii;
			for (jj=0; jj<8; jj++) {
				if ( crc & 0x00000001L )
					crc = ( crc >> 1 ) ^ 0xEDB88320l;
				else
					crc =   crc >> 1;
			}
			sc_CRC_tab32[ii] = crc;
		}
		sc_CRC_tab32_initialized = 1;
	}

	crc = 0xffffffffL;
	for (ii=0; ii<len; ii++)   {
		long_c = 0x000000ffL & (unsigned long) (*(value + ii));
		index = crc ^ long_c;
		crc = (crc >> 8) ^ sc_CRC_tab32[ index & 0xff ];
	}

	crc ^= 0xffffffff;
	return  crc%0xffff;
}