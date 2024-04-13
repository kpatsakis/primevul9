hex_int (const gchar *spec,
	 gint         len,
	 guint       *c)
{
	const gchar *end;

	*c = 0;
	for (end = spec + len; spec != end; spec++) {
		if (!g_ascii_isxdigit (*spec))
			return FALSE;

		*c = g_ascii_xdigit_value (*spec);
	}

	return TRUE;
}