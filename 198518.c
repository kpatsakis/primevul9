u8 *ulong2bebytes(u8 *buf, unsigned long x)
{
	if (buf != NULL) {
		buf[3] = (u8) (x & 0xff);
		buf[2] = (u8) ((x >> 8) & 0xff);
		buf[1] = (u8) ((x >> 16) & 0xff);
		buf[0] = (u8) ((x >> 24) & 0xff);
	}
	return buf;
}