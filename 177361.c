static OnigEncoding _php_mb_regex_name2mbctype(const char *pname)
{
	const char *p;
	const php_mb_regex_enc_name_map_t *mapping;

	if (pname == NULL || !*pname) {
		return ONIG_ENCODING_UNDEF;
	}

	for (mapping = enc_name_map; mapping->names != NULL; mapping++) {
		for (p = mapping->names; *p != '\0'; p += (strlen(p) + 1)) {
			if (strcasecmp(p, pname) == 0) {
				return mapping->code;
			}
		}
	}

	return ONIG_ENCODING_UNDEF;
}