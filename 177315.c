static CharSet conv_get_locale_charset_no_utf8(void)
{
	static CharSet cur_charset = -1;
	const gchar *cur_locale;
	const gchar *p;
	gint i;

	if (prefs_common.broken_are_utf8) {
		cur_charset = C_UTF_8;
		return cur_charset;
	}

	cur_locale = conv_get_current_locale();
	if (!cur_locale) {
		cur_charset = C_US_ASCII;
		return cur_charset;
	}

	if (strcasestr(cur_locale, "UTF-8") ||
	    strcasestr(cur_locale, "utf8")) {
		cur_charset = C_UTF_8;
		return cur_charset;
	}

	if ((p = strcasestr(cur_locale, "@euro")) && p[5] == '\0') {
		cur_charset = C_ISO_8859_15;
		return cur_charset;
	}

	for (i = 0; i < sizeof(locale_table) / sizeof(locale_table[0]); i++) {
		const gchar *p;

		/* "ja_JP.EUC" matches with "ja_JP.eucJP", "ja_JP.EUC" and
		   "ja_JP". "ja_JP" matches with "ja_JP.xxxx" and "ja" */
		if (!g_ascii_strncasecmp(cur_locale, locale_table[i].locale,
				 strlen(locale_table[i].locale))) {
			cur_charset = locale_table[i].charset;
			return cur_charset;
		} else if ((p = strchr(locale_table[i].locale, '_')) &&
			 !strchr(p + 1, '.')) {
			if (strlen(cur_locale) == 2 &&
			    !g_ascii_strncasecmp(cur_locale, locale_table[i].locale, 2)) {
				cur_charset = locale_table[i].charset;
				return cur_charset;
			}
		}
	}

	cur_charset = C_AUTO;
	return cur_charset;
}