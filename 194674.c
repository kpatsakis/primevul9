void textview_scroll_max(TextView *textview, gboolean up)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview->text));
	GtkTextIter iter;
	
	if (up) {
		gtk_text_buffer_get_start_iter(buffer, &iter);
		gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(textview->text),
						&iter, 0.0, TRUE, 0.0, 1.0);
	
	} else {
		gtk_text_buffer_get_end_iter(buffer, &iter);
		gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(textview->text),
						&iter, 0.0, TRUE, 0.0, 0.0);
	}
}