void textview_init(TextView *textview)
{
	if (!hand_cursor)
		hand_cursor = gdk_cursor_new(GDK_HAND2);
	if (!text_cursor)
		text_cursor = gdk_cursor_new(GDK_XTERM);
	if (!watch_cursor)
		watch_cursor = gdk_cursor_new(GDK_WATCH);

	textview_reflect_prefs(textview);
	textview_set_font(textview, NULL);
	textview_create_tags(GTK_TEXT_VIEW(textview->text), textview);
}