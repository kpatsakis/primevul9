void sc_html_parser_destroy(SC_HTMLParser *parser)
{
	g_string_free(parser->str, TRUE);
	g_string_free(parser->buf, TRUE);
	g_free(parser->href);
	g_free(parser);
}