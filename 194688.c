void textview_set_position(TextView *textview, gint pos)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);

	gtkut_text_view_set_position(text, pos);
}