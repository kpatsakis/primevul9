void textview_cursor_normal(TextView *textview)
{
	GdkWindow *window = gtk_text_view_get_window(
			GTK_TEXT_VIEW(textview->text),
			GTK_TEXT_WINDOW_TEXT);
	textview_set_cursor(window, text_cursor);
}