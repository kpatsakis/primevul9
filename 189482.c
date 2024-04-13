e_util_utf8_strstrcase (const gchar *haystack,
                        const gchar *needle)
{
	gunichar *nuni, unival;
	gint nlen;
	const gchar *o, *p;

	if (haystack == NULL)
		return NULL;

	if (needle == NULL)
		return NULL;

	if (strlen (needle) == 0)
		return haystack;

	if (strlen (haystack) == 0)
		return NULL;

	nuni = g_alloca (sizeof (gunichar) * strlen (needle));

	nlen = 0;
	for (p = e_util_unicode_get_utf8 (needle, &unival);
	     p && unival;
	     p = e_util_unicode_get_utf8 (p, &unival)) {
		nuni[nlen++] = g_unichar_tolower (unival);
	}
	/* NULL means there was illegal utf-8 sequence */
	if (!p || !nlen)
		return NULL;

	o = haystack;
	for (p = e_util_unicode_get_utf8 (o, &unival);
	     p && unival;
	     p = e_util_unicode_get_utf8 (p, &unival)) {
		gunichar sc;
		sc = g_unichar_tolower (unival);
		/* We have valid stripped gchar */
		if (sc == nuni[0]) {
			const gchar *q = p;
			gint npos = 1;
			while (npos < nlen) {
				q = e_util_unicode_get_utf8 (q, &unival);
				if (!q || !unival) return NULL;
				sc = g_unichar_tolower (unival);
				if (sc != nuni[npos]) break;
				npos++;
			}
			if (npos == nlen) {
				return o;
			}
		}
		o = p;
	}

	return NULL;
}