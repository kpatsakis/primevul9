is_valid_cls_ident (const char *p)
{
	/*
	 * FIXME: we need the full unicode glib support for this.
	 * Check: http://www.unicode.org/unicode/reports/tr15/Identifier.java
	 * We do the lame thing for now.
	 */
	if (!isalpha (*p))
		return 0;
	++p;
	while (*p) {
		if (!isalnum (*p) && *p != '_')
			return 0;
		++p;
	}
	return 1;
}