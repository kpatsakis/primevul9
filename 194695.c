gchar *textview_get_visible_uri		(TextView 	*textview, 
					 ClickableText 	*uri)
{
	GtkTextBuffer *buffer;
	GtkTextIter start, end;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview->text));

	gtk_text_buffer_get_iter_at_offset(buffer, &start, uri->start);
	gtk_text_buffer_get_iter_at_offset(buffer, &end,   uri->end);

	return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}