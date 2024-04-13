void textview_destroy(TextView *textview)
{
	GtkTextBuffer *buffer;
	GtkClipboard *clipboard;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview->text));
	clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
	gtk_text_buffer_remove_selection_clipboard(buffer, clipboard);

	textview_uri_list_remove_all(textview->uri_list);
	textview->uri_list = NULL;
	textview->prev_quote_level = -1;

	g_free(textview);
}