static void sc_html_parse_special(SC_HTMLParser *parser)
{
	gchar symbol_name[9];
	gint n;
	const gchar *val;

	parser->state = SC_HTML_UNKNOWN;
	cm_return_if_fail(*parser->bufp == '&');

	/* &foo; */
	for (n = 0; parser->bufp[n] != '\0' && parser->bufp[n] != ';'; n++)
		;
	if (n > 7 || parser->bufp[n] != ';') {
		/* output literal `&' */
		sc_html_append_char(parser, *parser->bufp++);
		parser->state = SC_HTML_NORMAL;
		return;
	}
	strncpy2(symbol_name, parser->bufp, n + 2);
	parser->bufp += n + 1;

	if ((val = g_hash_table_lookup(parser->symbol_table, symbol_name))
	    != NULL) {
		sc_html_append_str(parser, val, -1);
		parser->state = SC_HTML_NORMAL;
		return;
	} 

	sc_html_append_str(parser, symbol_name, -1);
}