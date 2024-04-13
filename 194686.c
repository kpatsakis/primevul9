gboolean textview_search_string(TextView *textview, const gchar *str,
				gboolean case_sens)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);

	return gtkut_text_view_search_string(text, str, case_sens);
}