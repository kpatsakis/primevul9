static int match_hist_bytes(sc_card_t *card, const char *str, size_t len)
{
	const char *src = (const char *) card->reader->atr_info.hist_bytes;
	size_t srclen = card->reader->atr_info.hist_bytes_len;
	size_t offset = 0;

	if (len == 0)
		len = strlen(str);
	if (srclen < len)
		return 0;
	while (srclen - offset > len) {
		if (memcmp(src + offset, str, len) == 0) {
			return 1;
		}
		offset++;
	}
	return 0;
}