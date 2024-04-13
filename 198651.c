unsigned short bebytes2ushort(const u8 *buf)
{
	if (buf == NULL)
		return 0U;
	return (unsigned short) (buf[0] << 8 | buf[1]);
}