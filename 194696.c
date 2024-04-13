static void textview_toggle_quote(TextView *textview, GSList *start_list, ClickableText *uri, gboolean expand_only)
{
	GtkTextIter start, end;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview->text));
	
	if (!uri->is_quote)
		return;
	
	if (uri->q_expanded && expand_only)
		return;

	gtk_text_buffer_get_iter_at_offset(buffer, &start, uri->start);
	gtk_text_buffer_get_iter_at_offset(buffer, &end,   uri->end);
	if (textview->uri_hover)
		gtk_text_buffer_remove_tag_by_name(buffer,
						   "link-hover",
						   &textview->uri_hover_start_iter,
						   &textview->uri_hover_end_iter);
	textview->uri_hover = NULL;
	gtk_text_buffer_remove_tag_by_name(buffer,
					   "qlink",
					   &start,
					   &end);
	/* when shifting URIs start and end, we have to do it per-UTF8-char
	 * so use g_utf8_strlen(). OTOH, when inserting in the text buffer, 
	 * we have to pass a number of bytes, so use strlen(). disturbing. */
	 
	if (!uri->q_expanded) {
		gtk_text_buffer_get_iter_at_offset(buffer, &start, uri->start);
		gtk_text_buffer_get_iter_at_offset(buffer, &end,   uri->end);
		textview_shift_uris_after(textview, start_list, uri->start, 
			g_utf8_strlen((gchar *)uri->data, -1)-strlen(" [...]\n"));
		gtk_text_buffer_delete(buffer, &start, &end);
		gtk_text_buffer_get_iter_at_offset(buffer, &start, uri->start);
		gtk_text_buffer_insert_with_tags_by_name
				(buffer, &start, (gchar *)uri->data, 
				 strlen((gchar *)uri->data)-1,
				 "qlink", (gchar *)uri->fg_color, NULL);
		uri->end = gtk_text_iter_get_offset(&start);
		textview_make_clickable_parts_later(textview,
					  uri->start, uri->end);
		uri->q_expanded = TRUE;
	} else {
		gtk_text_buffer_get_iter_at_offset(buffer, &start, uri->start);
		gtk_text_buffer_get_iter_at_offset(buffer, &end,   uri->end);
		textview_remove_uris_in(textview, uri->start, uri->end);
		textview_shift_uris_after(textview, start_list, uri->start, 
			strlen(" [...]\n")-g_utf8_strlen((gchar *)uri->data, -1));
		gtk_text_buffer_delete(buffer, &start, &end);
		gtk_text_buffer_get_iter_at_offset(buffer, &start, uri->start);
		gtk_text_buffer_insert_with_tags_by_name
				(buffer, &start, " [...]", -1,
				 "qlink", (gchar *)uri->fg_color, NULL);
		uri->end = gtk_text_iter_get_offset(&start);
		uri->q_expanded = FALSE;
	}
	if (textview->messageview->mainwin->cursor_count == 0) {
		textview_cursor_normal(textview);
	} else {
		textview_cursor_wait(textview);
	}
}