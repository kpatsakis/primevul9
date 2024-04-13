u8 *ushort2bebytes(u8 *buf, unsigned short x)
{
	if (buf != NULL) {
		buf[1] = (u8) (x & 0xff);
		buf[0] = (u8) ((x >> 8) & 0xff);
	}
	return buf;
}