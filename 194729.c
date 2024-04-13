static void textview_show_ertf(TextView *textview, FILE *fp,
			       CodeConverter *conv)
{
	ERTFParser *parser;
	gchar *str;
	gint lines = 0;

	parser = ertf_parser_new(fp, conv);
	cm_return_if_fail(parser != NULL);

	account_sigsep_matchlist_create();

	while ((str = ertf_parse(parser)) != NULL) {
		textview_write_line(textview, str, NULL, FALSE);
		lines++;
		if (lines % 500 == 0)
			GTK_EVENTS_FLUSH();
		if (textview->stop_loading) {
			account_sigsep_matchlist_delete();
			return;
		}
	}
	
	account_sigsep_matchlist_delete();

	ertf_parser_destroy(parser);
}