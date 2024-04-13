static u16 crc16(u16 crc, const u8 *buf, size_t len)
{
	u16 tmp;

	while (len--) {
		crc ^= *buf++;
		crc ^= (u8)crc >> 4;
		tmp = (u8)crc;
		crc ^= (tmp ^ (tmp << 1)) << 4;
	}
	return crc;
}