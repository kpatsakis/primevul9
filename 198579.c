static void invert_buf(u8 *dest, const u8 *src, size_t c)
{
	size_t i;

	for (i = 0; i < c; i++)
		dest[i] = src[c-1-i];
}