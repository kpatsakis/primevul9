static const char *find_alias(REgg *egg, const char *str) {
	// do not forget to free return strings to avoid memory leak
	char *p = (char *) str;
	int i;
	if (*str == '"') {
		return strdup (str);
	}
	// strings could not means aliases
	while (*p && !is_space (*p)) {
		p++;
	}
	*p = '\x00';
	for (i = 0; i < egg->lang.nalias; i++) {
		if (!strcmp (str, egg->lang.aliases[i].name)) {
			return strdup (egg->lang.aliases[i].content);
		}
	}
	return NULL;
	// only strings or alias could return valuable data
}