void textview_clear(TextView *textview)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	GtkTextBuffer *buffer;
	GdkWindow *window = gtk_text_view_get_window(text,
				GTK_TEXT_WINDOW_TEXT);

	buffer = gtk_text_view_get_buffer(text);
	gtk_text_buffer_set_text(buffer, "", -1);
	if (gtk_text_buffer_get_mark(buffer, "body_start"))
		gtk_text_buffer_delete_mark_by_name(buffer, "body_start");
	if (gtk_text_buffer_get_mark(buffer, "body_end"))
		gtk_text_buffer_delete_mark_by_name(buffer, "body_end");

	TEXTVIEW_STATUSBAR_POP(textview);
	textview_uri_list_remove_all(textview->uri_list);
	textview->uri_list = NULL;
	textview->uri_hover = NULL;
	textview->prev_quote_level = -1;

	textview->body_pos = 0;
	if (textview->image) 
		gtk_widget_destroy(textview->image);
	textview->image = NULL;
	textview->avatar_type = 0;

	if (textview->messageview->mainwin->cursor_count == 0) {
		textview_set_cursor(window, text_cursor);
	} else {
		textview_set_cursor(window, watch_cursor);
	}
}