static gboolean textview_motion_notify(GtkWidget *widget,
				       GdkEventMotion *event,
				       TextView *textview)
{
	if (textview->loading)
		return FALSE;
	textview_uri_update(textview, event->x, event->y);
	gdk_window_get_pointer(gtk_widget_get_window(widget), NULL, NULL, NULL);

	return FALSE;
}