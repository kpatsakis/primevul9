cdf_u16tos8(char *buf, size_t len, const uint16_t *p)
{
	size_t i;
	for (i = 0; i < len && p[i]; i++)
		buf[i] = CAST(char, p[i]);
	buf[i] = '\0';
	return buf;
}