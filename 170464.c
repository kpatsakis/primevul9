static char *readString(ut8 *p, int off, int len) {
	if (off < 0 || off >= len) {
		return NULL;
	}
	return rz_str_ndup((const char *)p + off, len - off);
}