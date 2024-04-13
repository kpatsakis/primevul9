gboolean textview_scroll_page(TextView *textview, gboolean up)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	GtkAdjustment *vadj = gtk_text_view_get_vadjustment(text);

	return gtkutils_scroll_page(GTK_WIDGET(text), vadj, up);
}