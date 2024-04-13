static gchar *sc_html_find_tag(SC_HTMLParser *parser, const gchar *tag)
{
	gchar *cur = parser->bufp;
	gint len = strlen(tag);

	if (cur == NULL)
		return NULL;

	while ((cur = strstr(cur, "<")) != NULL) {
		if (!g_ascii_strncasecmp(cur, tag, len))
			return cur;
		cur += 2;
	}
	return NULL;
}