int addopt(char *optstr, int maxlen, const char *s)
{
	size_t n;
	size_t m;

	n = strlen(optstr);
	m = n + strlen(s) + 1;
	if (m > JAS_CAST(size_t, maxlen)) {
		return 1;
	}
	if (n > 0) {
		strcat(optstr, "\n");
	}
	strcat(optstr, s);
	return 0;
}