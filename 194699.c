void textview_scroll_one_line(TextView *textview, gboolean up)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	GtkAdjustment *vadj = gtk_text_view_get_vadjustment(text);

	gtkutils_scroll_one_line(GTK_WIDGET(text), vadj, up);
}