gchar *sc_html_parse(SC_HTMLParser *parser)
{
	parser->state = SC_HTML_NORMAL;
	g_string_truncate(parser->str, 0);

	if (*parser->bufp == '\0') {
		g_string_truncate(parser->buf, 0);
		parser->bufp = parser->buf->str;
		if (sc_html_read_line(parser) == SC_HTML_EOF)
			return NULL;
	}

	while (*parser->bufp != '\0') {
		switch (*parser->bufp) {
		case '<': {
			SC_HTMLState st;
			st = sc_html_parse_tag(parser);
			/* when we see an href, we need to flush the str
			 * buffer.  Then collect all the chars until we
			 * see the end anchor tag
			 */
			if (SC_HTML_HREF_BEG == st || SC_HTML_HREF == st)
				return parser->str->str;
			} 
			break;
		case '&':
			sc_html_parse_special(parser);
			break;
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			if (parser->bufp[0] == '\r' && parser->bufp[1] == '\n')
				parser->bufp++;

			if (!parser->pre) {
				if (!parser->newline)
					parser->space = TRUE;

				parser->bufp++;
				break;
			}
			/* fallthrough */
		default:
			sc_html_append_char(parser, *parser->bufp++);
		}
	}

	return parser->str->str;
}