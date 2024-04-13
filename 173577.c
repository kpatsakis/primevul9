static void sc_html_get_parenthesis(SC_HTMLParser *parser, gchar *buf, gint len)
{
	gchar *p;

	buf[0] = '\0';
	cm_return_if_fail(*parser->bufp == '<');

	/* ignore comment / CSS / script stuff */
	if (!strncmp(parser->bufp, "<!--", 4)) {
		parser->bufp += 4;
		while ((p = strstr(parser->bufp, "-->")) == NULL)
			if (sc_html_read_line(parser) == SC_HTML_EOF) return;
		parser->bufp = p + 3;
		return;
	}
	if (!g_ascii_strncasecmp(parser->bufp, "<style", 6)) {
		parser->bufp += 6;
		while ((p = sc_html_find_tag(parser, "</style>")) == NULL)
			if (sc_html_read_line(parser) == SC_HTML_EOF) return;
		parser->bufp = p + 8;
		return;
	}
	if (!g_ascii_strncasecmp(parser->bufp, "<script", 7)) {
		parser->bufp += 7;
		while ((p = sc_html_find_tag(parser, "</script>")) == NULL)
			if (sc_html_read_line(parser) == SC_HTML_EOF) return;
		parser->bufp = p + 9;
		return;
	}

	parser->bufp++;
	while ((p = strchr(parser->bufp, '>')) == NULL)
		if (sc_html_read_line(parser) == SC_HTML_EOF) return;

	strncpy2(buf, parser->bufp, MIN(p - parser->bufp + 1, len));
	g_strstrip(buf);
	parser->bufp = p + 1;
}