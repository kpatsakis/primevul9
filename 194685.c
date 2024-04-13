void textview_show_info(TextView *textview, const gchar *info_str)
{
	GtkTextView *text;
	GtkTextBuffer *buffer;
	GtkTextIter iter;

	textview_set_font(textview, NULL);
	textview_clear(textview);

	text = GTK_TEXT_VIEW(textview->text);
	buffer = gtk_text_view_get_buffer(text);
	gtk_text_buffer_get_start_iter(buffer, &iter);

	TEXTVIEW_INSERT(info_str);
	textview_show_icon(textview, GTK_STOCK_DIALOG_INFO);
	textview_cursor_normal(textview);
}