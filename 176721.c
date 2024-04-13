static char *ldb_parse_find_wildcard(char *value)
{
	while (*value) {
		value = strpbrk(value, "\\*");
		if (value == NULL) return NULL;

		if (value[0] == '\\') {
			if (value[1] == '\0') return NULL;
			value += 2;
			continue;
		}

		if (value[0] == '*') return value;
	}

	return NULL;
}