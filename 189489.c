e_utf8_strftime (gchar *string,
                 gsize max,
                 const gchar *fmt,
                 const struct tm *tm)
{
	gsize sz, ret;
	gchar *locale_fmt, *buf;

	g_return_val_if_fail (string != NULL, 0);
	g_return_val_if_fail (fmt != NULL, 0);
	g_return_val_if_fail (tm != NULL, 0);

	locale_fmt = g_locale_from_utf8 (fmt, -1, NULL, &sz, NULL);
	if (!locale_fmt)
		return 0;

	ret = e_strftime (string, max, locale_fmt, tm);
	if (!ret) {
		g_free (locale_fmt);
		return 0;
	}

	buf = g_locale_to_utf8 (string, ret, NULL, &sz, NULL);
	if (!buf) {
		g_free (locale_fmt);
		return 0;
	}

	if (sz >= max) {
		gchar *tmp = buf + max - 1;
		tmp = g_utf8_find_prev_char (buf, tmp);
		if (tmp)
			sz = tmp - buf;
		else
			sz = 0;
	}

	memcpy (string, buf, sz);
	string[sz] = '\0';

	g_free (locale_fmt);
	g_free (buf);

	return sz;
}