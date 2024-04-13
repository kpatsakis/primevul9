static unsigned short coolkey_get_key_id(unsigned long object_id) {
	char char_index = (object_id >> 16) & 0xff;
	if (char_index >= '0' && char_index <= '9') {
		return (u8)(char_index - '0');
	}
	if (char_index >= 'A' && char_index <= 'Z') {
		return (u8)(char_index - 'A' + 10);
	}
	if (char_index >= 'a' && char_index <= 'z') {
		return (u8)(char_index - 'a' + 26 + 10);
	}
	return COOLKEY_INVALID_KEY;
}