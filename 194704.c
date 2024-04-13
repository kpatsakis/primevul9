void textview_set_text(TextView *textview, const gchar *text)
{
	GtkTextView *view;
	GtkTextBuffer *buffer;

	cm_return_if_fail(textview != NULL);
	cm_return_if_fail(text != NULL);

	textview_clear(textview);

	view = GTK_TEXT_VIEW(textview->text);
	buffer = gtk_text_view_get_buffer(view);
	gtk_text_buffer_set_text(buffer, text, strlen(text));
}