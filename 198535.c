unsigned long bebytes2ulong(const u8 *buf)
{
	if (buf == NULL)
		return 0UL;
	return (unsigned long) (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]);
}