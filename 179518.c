oct_to_str_back(char *ptr, guint32 value)
{
	while (value) {
		*(--ptr) = '0' + (value & 0x7);
		value >>= 3;
	}

	*(--ptr) = '0';
	return ptr;
}