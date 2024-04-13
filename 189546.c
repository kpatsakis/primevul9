e_util_utf8_decompose (const gchar *text)
{
	gunichar unival;
	const gchar *p;
	gchar utf8[12];
	GString *decomp;

	if (!text)
		return NULL;

	decomp = g_string_sized_new (strlen (text) + 1);

	for (p = e_util_unicode_get_utf8 (text, &unival);
	     p && unival;
	     p = e_util_unicode_get_utf8 (p, &unival)) {
		gunichar sc;
		sc = stripped_char (unival);
		if (sc) {
			gint ulen = g_unichar_to_utf8 (sc, utf8);
			g_string_append_len (decomp, utf8, ulen);
		}
	}

	/* NULL means there was illegal utf-8 sequence */
	if (!p || !decomp->len) {
		g_string_free (decomp, TRUE);
		return NULL;
	}

	return g_string_free (decomp, FALSE);
}