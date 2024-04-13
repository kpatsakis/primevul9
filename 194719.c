void textview_reflect_prefs(TextView *textview)
{
	textview_set_font(textview, NULL);
	textview_update_message_colors(textview);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview->text),
					 prefs_common.textview_cursor_visible);
}