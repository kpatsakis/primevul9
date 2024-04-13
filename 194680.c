static void textview_show_html(TextView *textview, FILE *fp,
			       CodeConverter *conv)
{
	SC_HTMLParser *parser;
	gchar *str;
	gint lines = 0;

	parser = sc_html_parser_new(fp, conv);
	cm_return_if_fail(parser != NULL);

	account_sigsep_matchlist_create();

	while ((str = sc_html_parse(parser)) != NULL) {
	        if (parser->state == SC_HTML_HREF) {
		        /* first time : get and copy the URL */
		        if (parser->href == NULL) {
				/* ALF - the claws html parser returns an empty string,
				 * if still inside an <a>, but already parsed past HREF */
				str = strtok(str, " ");
				if (str) {
					while (str && *str && g_ascii_isspace(*str))
						str++; 
					parser->href = g_strdup(str);
					/* the URL may (or not) be followed by the
					 * referenced text */
					str = strtok(NULL, "");
				}	
		        }
		        if (str != NULL)
			        textview_write_link(textview, str, parser->href, NULL);
	        } else
		        textview_write_line(textview, str, NULL, FALSE);
		lines++;
		if (lines % 500 == 0)
			GTK_EVENTS_FLUSH();
		if (textview->stop_loading) {
			account_sigsep_matchlist_delete();
			return;
		}
	}
	textview_write_line(textview, "\n", NULL, FALSE);

	account_sigsep_matchlist_delete();

	sc_html_parser_destroy(parser);
}