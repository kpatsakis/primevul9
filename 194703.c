void textview_show_error(TextView *textview)
{
	GtkTextView *text;
	GtkTextBuffer *buffer;
	GtkTextIter iter;

	textview_set_font(textview, NULL);
	textview_clear(textview);

	text = GTK_TEXT_VIEW(textview->text);
	buffer = gtk_text_view_get_buffer(text);
	gtk_text_buffer_get_start_iter(buffer, &iter);

	TEXTVIEW_INSERT(_("\n"
		      "  This message can't be displayed.\n"
		      "  This is probably due to a network error.\n"
		      "\n"
		      "  Use "));
	TEXTVIEW_INSERT_LINK(_("'Network Log'"), "sc://view_log", NULL);
	TEXTVIEW_INSERT(_(" in the Tools menu for more information."));
	textview_show_icon(textview, GTK_STOCK_DIALOG_ERROR);
}