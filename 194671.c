static void textview_remove_uris_in(TextView *textview, gint start, gint end)
{
	GSList *cur;
	for (cur = textview->uri_list; cur; ) {
		ClickableText *uri = (ClickableText *)cur->data;
		if (uri->start > start && uri->end < end) {
			cur = cur->next;
			textview->uri_list = g_slist_remove(textview->uri_list, uri);
			g_free(uri->uri);
			g_free(uri->filename);
			if (uri->is_quote) {
				g_free(uri->fg_color);
				g_free(uri->data); 
				/* (only free data in quotes uris) */
			}
			g_free(uri);
		} else {
			cur = cur->next;
		}
		
	}
}