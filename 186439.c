int php_libxml_xmlCheckUTF8(const unsigned char *s)
{
	int i;
	unsigned char c;

	for (i = 0; (c = s[i++]);) {
		if ((c & 0x80) == 0) {
		} else if ((c & 0xe0) == 0xc0) {
			if ((s[i++] & 0xc0) != 0x80) {
				return 0;
			}
		} else if ((c & 0xf0) == 0xe0) {
			if ((s[i++] & 0xc0) != 0x80 || (s[i++] & 0xc0) != 0x80) {
				return 0;
			}
		} else if ((c & 0xf8) == 0xf0) {
			if ((s[i++] & 0xc0) != 0x80 || (s[i++] & 0xc0) != 0x80 || (s[i++] & 0xc0) != 0x80) {
				return 0;
			}
		} else {
			return 0;
		}
	}
	return 1;
}