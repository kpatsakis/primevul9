static void textview_uri_update(TextView *textview, gint x, gint y)
{
	GtkTextBuffer *buffer;
	GtkTextIter start_iter, end_iter;
	ClickableText *uri = NULL;
	
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview->text));

	if (x != -1 && y != -1) {
		gint bx, by;
		GtkTextIter iter;
		GSList *tags;
		GSList *cur;
		
		gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(textview->text), 
						      GTK_TEXT_WINDOW_WIDGET,
						      x, y, &bx, &by);
		gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(textview->text),
						   &iter, bx, by);

		tags = gtk_text_iter_get_tags(&iter);
		for (cur = tags; cur != NULL; cur = cur->next) {
			GtkTextTag *tag = cur->data;
			char *name;

			g_object_get(G_OBJECT(tag), "name", &name, NULL);

			if ((!strcmp(name, "link") || !strcmp(name, "hlink"))
			    && textview_get_uri_range(textview, &iter, tag,
						      &start_iter, &end_iter)) {

				uri = textview_get_uri_from_range(textview,
								  &iter, tag,
								  &start_iter,
								  &end_iter);
			}
			g_free(name);
			if (uri)
				break;
		}
		g_slist_free(tags);
	}
	
	if (uri != textview->uri_hover) {
		GdkWindow *window;

		if (textview->uri_hover)
			gtk_text_buffer_remove_tag_by_name(buffer,
							   "link-hover",
							   &textview->uri_hover_start_iter,
							   &textview->uri_hover_end_iter);
		    
		textview->uri_hover = uri;
		if (uri) {
			textview->uri_hover_start_iter = start_iter;
			textview->uri_hover_end_iter = end_iter;
		}
		
		window = gtk_text_view_get_window(GTK_TEXT_VIEW(textview->text),
						  GTK_TEXT_WINDOW_TEXT);
		if (textview->messageview->mainwin->cursor_count == 0) {
			textview_set_cursor(window, uri ? hand_cursor : text_cursor);
		} else {
			textview_set_cursor(window, watch_cursor);
		}

		TEXTVIEW_STATUSBAR_POP(textview);

		if (uri) {
			if (!uri->is_quote)
				gtk_text_buffer_apply_tag_by_name(buffer,
							  "link-hover",
							  &start_iter,
							  &end_iter);
			TEXTVIEW_STATUSBAR_PUSH(textview, uri->uri);
		}
	}
}