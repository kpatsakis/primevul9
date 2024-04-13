static void decode_href(SC_HTMLParser *parser)
{
	gchar *tmp;
	SC_HTMLParser *tparser = g_new0(SC_HTMLParser, 1);

	tparser->str = g_string_new(NULL);
	tparser->buf = g_string_new(parser->href);
	tparser->bufp = tparser->buf->str;
	tparser->symbol_table = default_symbol_table;
	
	tmp = sc_html_parse(tparser);
	
	g_free(parser->href);
	parser->href = g_strdup(tmp);

	sc_html_parser_destroy(tparser);
}