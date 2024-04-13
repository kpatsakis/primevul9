is_string_valid_atr(const char *atr_str)
{
	unsigned char atr[SC_MAX_ATR_SIZE];
	size_t atr_len = sizeof(atr);

	if (sc_hex_to_bin(atr_str, atr, &atr_len))
		return 0;
	if (atr_len < 2)
		return 0;
	if (atr[0] != 0x3B && atr[0] != 0x3F)
		return 0;
	return 1;
}