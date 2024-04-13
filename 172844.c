static const char *skipspaces(const char *s) {
	while (is_space (*s)) {
		s++;
	}
	return s;
}