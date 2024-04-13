static CharSet conv_get_outgoing_charset(void)
{
	static CharSet out_charset = -1;
	const gchar *cur_locale;
	const gchar *p;
	gint i;

	if (out_charset != -1)
		return out_charset;

	cur_locale = conv_get_current_locale();
	if (!cur_locale) {
		out_charset = C_AUTO;
		return out_charset;
	}

	if (strcasestr(cur_locale, "UTF-8") ||
	    strcasestr(cur_locale, "utf8")) {
		out_charset = C_UTF_8;
		return out_charset;
	}

	if ((p = strcasestr(cur_locale, "@euro")) && p[5] == '\0') {
		out_charset = C_ISO_8859_15;
		return out_charset;
	}

	for (i = 0; i < sizeof(locale_table) / sizeof(locale_table[0]); i++) {
		const gchar *p;

		if (!g_ascii_strncasecmp(cur_locale, locale_table[i].locale,
				 strlen(locale_table[i].locale))) {
			out_charset = locale_table[i].out_charset;
			break;
		} else if ((p = strchr(locale_table[i].locale, '_')) &&
			 !strchr(p + 1, '.')) {
			if (strlen(cur_locale) == 2 &&
			    !g_ascii_strncasecmp(cur_locale, locale_table[i].locale, 2)) {
				out_charset = locale_table[i].out_charset;
				break;
			}
		}
	}

	return out_charset;
}