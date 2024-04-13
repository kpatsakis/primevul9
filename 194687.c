gboolean textview_search_string_backward(TextView *textview, const gchar *str,
					 gboolean case_sens)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);

	return gtkut_text_view_search_string_backward(text, str, case_sens);
}