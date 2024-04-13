e_filename_make_safe (gchar *string)
{
	gchar *p, *ts;
	gunichar c;
#ifdef G_OS_WIN32
	const gchar *unsafe_chars = " /'\"`&();|<>$%{}!\\:*?#";
#else
	const gchar *unsafe_chars = " /'\"`&();|<>$%{}!#";
#endif

	g_return_if_fail (string != NULL);

	p = string;

	while (p && *p) {
		c = g_utf8_get_char (p);
		ts = p;
		p = g_utf8_next_char (p);
		/* I wonder what this code is supposed to actually
		 * achieve, and whether it does that as currently
		 * written?
		 */
		if (!g_unichar_isprint (c) ||
			(c < 0xff && strchr (unsafe_chars, c & 0xff))) {
			while (ts < p)
				*ts++ = '_';
		}
	}
}