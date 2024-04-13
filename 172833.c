static char *trim(char *s) {
	char *o;
	for (o = s; *o; o++) {
		if (is_space (*o)) {
			*o = 0;
		}
	}
	return s;
}