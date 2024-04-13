static gboolean textview_visibility_notify(GtkWidget *widget,
					   GdkEventVisibility *event,
					   TextView *textview)
{
	gint wx, wy;
	GdkWindow *window;

	if (textview->loading)
		return FALSE;

	window = gtk_text_view_get_window(GTK_TEXT_VIEW(widget),
					  GTK_TEXT_WINDOW_TEXT);

	/* check if occurred for the text window part */
	if (window != event->window)
		return FALSE;
	
	gdk_window_get_pointer(gtk_widget_get_window(widget), &wx, &wy, NULL);
	textview_uri_update(textview, wx, wy);

	return FALSE;
}