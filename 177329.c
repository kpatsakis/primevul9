static gboolean conv_is_ja_locale(void)
{
	static gint is_ja_locale = -1;
	const gchar *cur_locale;

	if (is_ja_locale != -1)
		return is_ja_locale != 0;

	is_ja_locale = 0;
	cur_locale = conv_get_current_locale();
	if (cur_locale) {
		if (g_ascii_strncasecmp(cur_locale, "ja", 2) == 0)
			is_ja_locale = 1;
	}

	return is_ja_locale != 0;
}