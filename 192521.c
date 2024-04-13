static char * coolkey_cuid_to_string(coolkey_cuid_t *cuid)
{
	char *buf;
	size_t len = sizeof(coolkey_cuid_t)*2 + 1;
	buf = malloc(len);
	if (buf == NULL) {
		return NULL;
	}
	sc_bin_to_hex((u8 *)cuid, sizeof(*cuid), buf, len, 0);
	return buf;
}