static int phar_hex_str(const char *digest, size_t digest_len, char **signature) /* {{{ */
{
	int pos = -1;
	size_t len = 0;

	*signature = (char*)safe_pemalloc(digest_len, 2, 1, PHAR_G(persist));

	for (; len < digest_len; ++len) {
		(*signature)[++pos] = hexChars[((const unsigned char *)digest)[len] >> 4];
		(*signature)[++pos] = hexChars[((const unsigned char *)digest)[len] & 0x0F];
	}
	(*signature)[++pos] = '\0';
	return pos;
}