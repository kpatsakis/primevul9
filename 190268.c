static char *get_internal_encoding(TSRMLS_D) {
	if (ICONVG(internal_encoding) && ICONVG(internal_encoding)[0]) {
		return ICONVG(internal_encoding);
	} else if (PG(internal_encoding) && PG(internal_encoding)[0]) {
		return PG(internal_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}