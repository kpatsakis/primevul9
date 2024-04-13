void textview_write_link(TextView *textview, const gchar *str,
			 const gchar *uri, CodeConverter *conv)
{
	GtkTextView *text;
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	gchar buf[BUFFSIZE];
	gchar *bufp;
	ClickableText *r_uri;

	if (!str || *str == '\0')
		return;
	if (!uri)
		return;

	while (uri && *uri && g_ascii_isspace(*uri))
		uri++;
		
	text = GTK_TEXT_VIEW(textview->text);
	buffer = gtk_text_view_get_buffer(text);
	gtk_text_buffer_get_end_iter(buffer, &iter);

	if (!conv)
		strncpy2(buf, str, sizeof(buf));
	else if (conv_convert(conv, buf, sizeof(buf), str) < 0)
		conv_utf8todisp(buf, sizeof(buf), str);

	if (g_utf8_validate(buf, -1, NULL) == FALSE)
		return;

	strcrchomp(buf);

	gtk_text_buffer_get_end_iter(buffer, &iter);
	for (bufp = buf; *bufp != '\0'; bufp = g_utf8_next_char(bufp)) {
		gunichar ch;

		ch = g_utf8_get_char(bufp);
		if (!g_unichar_isspace(ch))
			break;
	}
	if (bufp > buf)
		gtk_text_buffer_insert(buffer, &iter, buf, bufp - buf);

	r_uri = g_new0(ClickableText, 1);
	r_uri->uri = g_strdup(uri);
	r_uri->start = gtk_text_iter_get_offset(&iter);
	gtk_text_buffer_insert_with_tags_by_name
		(buffer, &iter, bufp, -1, "link", NULL);
	r_uri->end = gtk_text_iter_get_offset(&iter);
	r_uri->filename = NULL;
	textview->uri_list = g_slist_prepend(textview->uri_list, r_uri);
}