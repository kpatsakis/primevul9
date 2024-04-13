static void set_string(unsigned char *item, size_t max,
				const unsigned char *data, size_t len)
{
	if (len >= max)
		len = max - 1;

	memcpy(item, data, len);
	item[len] = '\0';

	blkid_rtrim_whitespace(item);
}