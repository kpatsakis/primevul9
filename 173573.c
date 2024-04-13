static void sc_html_append_char(SC_HTMLParser *parser, gchar ch)
{
	GString *str = parser->str;

	if (!parser->pre && parser->space) {
		g_string_append_c(str, ' ');
		parser->space = FALSE;
	}

	g_string_append_c(str, ch);

	parser->empty_line = FALSE;
	if (ch == '\n') {
		parser->newline = TRUE;
		if (str->len > 1 && str->str[str->len - 2] == '\n')
			parser->empty_line = TRUE;
	} else
		parser->newline = FALSE;
}