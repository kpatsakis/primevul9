hex64_to_str_back(char *ptr, int len, guint64 value)
{
	do {
		*(--ptr) = low_nibble_of_octet_to_hex(value & 0xF);
		value >>= 4;
		len--;
	} while (value);

	/* pad */
	while (len > 0) {
		*(--ptr) = '0';
		len--;
	}

	*(--ptr) = 'x';
	*(--ptr) = '0';

	return ptr;
}