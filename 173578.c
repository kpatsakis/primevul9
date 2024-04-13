static SC_HTMLState sc_html_parse_tag(SC_HTMLParser *parser)
{
	gchar buf[SC_HTMLBUFSIZE];
	SC_HTMLTag *tag;

	sc_html_get_parenthesis(parser, buf, sizeof(buf));

	tag = sc_html_get_tag(buf);

	parser->state = SC_HTML_UNKNOWN;
	if (!tag) return SC_HTML_UNKNOWN;

	if (!strcmp(tag->name, "br")) {
		parser->space = FALSE;
		sc_html_append_char(parser, '\n');
		parser->state = SC_HTML_BR;
	} else if (!strcmp(tag->name, "a")) {
		GList *cur;
		for (cur = tag->attr; cur != NULL; cur = cur->next) {
			if (cur->data && !strcmp(((SC_HTMLAttr *)cur->data)->name, "href")) {
				g_free(parser->href);
				parser->href = g_strdup(((SC_HTMLAttr *)cur->data)->value);
				decode_href(parser);
				parser->state = SC_HTML_HREF_BEG;
				break;
			}
		}
	} else if (!strcmp(tag->name, "/a")) {
		parser->state = SC_HTML_HREF;
	} else if (!strcmp(tag->name, "p")) {
		parser->space = FALSE;
		if (!parser->empty_line) {
			parser->space = FALSE;
			if (!parser->newline) sc_html_append_char(parser, '\n');
			sc_html_append_char(parser, '\n');
		}
		parser->state = SC_HTML_PAR;
	} else if (!strcmp(tag->name, "pre")) {
		parser->pre = TRUE;
		parser->state = SC_HTML_PRE;
	} else if (!strcmp(tag->name, "/pre")) {
		parser->pre = FALSE;
		parser->state = SC_HTML_NORMAL;
	} else if (!strcmp(tag->name, "hr")) {
		if (!parser->newline) {
			parser->space = FALSE;
			sc_html_append_char(parser, '\n');
		}
		sc_html_append_str(parser, HR_STR "\n", -1);
		parser->state = SC_HTML_HR;
	} else if (!strcmp(tag->name, "div")    ||
		   !strcmp(tag->name, "ul")     ||
		   !strcmp(tag->name, "li")     ||
		   !strcmp(tag->name, "table")  ||
		   !strcmp(tag->name, "tr")     ||
		   (tag->name[0] == 'h' && g_ascii_isdigit(tag->name[1]))) {
		if (!parser->newline) {
			parser->space = FALSE;
			sc_html_append_char(parser, '\n');
		}
		parser->state = SC_HTML_NORMAL;
	} else if (!strcmp(tag->name, "/table") ||
		   (tag->name[0] == '/' &&
		    tag->name[1] == 'h' &&
		    g_ascii_isdigit(tag->name[1]))) {
		if (!parser->empty_line) {
			parser->space = FALSE;
			if (!parser->newline) sc_html_append_char(parser, '\n');
			sc_html_append_char(parser, '\n');
		}
		parser->state = SC_HTML_NORMAL;
	} else if (!strcmp(tag->name, "/div")   ||
		   !strcmp(tag->name, "/ul")    ||
		   !strcmp(tag->name, "/li")) {
		if (!parser->newline) {
			parser->space = FALSE;
			sc_html_append_char(parser, '\n');
		}
		parser->state = SC_HTML_NORMAL;
			}

	sc_html_free_tag(tag);

	return parser->state;
}