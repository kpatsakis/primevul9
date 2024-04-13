static gboolean textview_leave_notify(GtkWidget *widget,
				      GdkEventCrossing *event,
				      TextView *textview)
{
	if (textview->loading)
		return FALSE;
	textview_uri_update(textview, -1, -1);

	return FALSE;
}