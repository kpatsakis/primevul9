static const char *_php_mb_regex_mbctype2name(OnigEncoding mbctype)
{
	const php_mb_regex_enc_name_map_t *mapping;

	for (mapping = enc_name_map; mapping->names != NULL; mapping++) {
		if (mapping->code == mbctype) {
			return mapping->names;
		}
	}

	return NULL;
}