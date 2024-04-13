word_to_hex_npad(char *out, guint16 word)
{
	if (word >= 0x1000)
		*out++ = low_nibble_of_octet_to_hex((guint8)(word >> 12));
	if (word >= 0x0100)
		*out++ = low_nibble_of_octet_to_hex((guint8)(word >> 8));
	if (word >= 0x0010)
		*out++ = low_nibble_of_octet_to_hex((guint8)(word >> 4));
	*out++ = low_nibble_of_octet_to_hex((guint8)(word >> 0));
	return out;
}