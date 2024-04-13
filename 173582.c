static void sc_html_append_str(SC_HTMLParser *parser, const gchar *str, gint len)
{
	GString *string = parser->str;

	if (!parser->pre && parser->space) {
		g_string_append_c(string, ' ');
		parser->space = FALSE;
	}

	if (len == 0) return;
	if (len < 0)
		g_string_append(string, str);
	else {
		gchar *s;
		Xstrndup_a(s, str, len, return);
		g_string_append(string, s);
	}

	parser->empty_line = FALSE;
	if (string->len > 0 && string->str[string->len - 1] == '\n') {
		parser->newline = TRUE;
		if (string->len > 1 && string->str[string->len - 2] == '\n')
			parser->empty_line = TRUE;
	} else
		parser->newline = FALSE;
}