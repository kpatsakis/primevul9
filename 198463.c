unsigned short lebytes2ushort(const u8 *buf)
{
	if (buf == NULL)
		return 0U;
	return (unsigned short)buf[1] << 8 | (unsigned short)buf[0];
}