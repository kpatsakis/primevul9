static void textview_set_cursor(GdkWindow *window, GdkCursor *cursor)
{
	if (GDK_IS_WINDOW(window))
		gdk_window_set_cursor(window, cursor);
}