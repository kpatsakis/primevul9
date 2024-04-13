static u32 tw5864_vlc_checksum(u32 *data, int len)
{
	u32 val, count_len = len;

	val = *data++;
	while (((count_len >> 2) - 1) > 0) {
		val ^= *data++;
		count_len -= 4;
	}
	val ^= htonl((len >> 2));
	return val;
}