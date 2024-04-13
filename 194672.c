static void textview_uri_list_remove_all(GSList *uri_list)
{
	GSList *cur;

	for (cur = uri_list; cur != NULL; cur = cur->next) {
		if (cur->data) {
			g_free(((ClickableText *)cur->data)->uri);
			g_free(((ClickableText *)cur->data)->filename);
			if (((ClickableText *)cur->data)->is_quote) {
				g_free(((ClickableText *)cur->data)->fg_color);
				g_free(((ClickableText *)cur->data)->data); 
				/* (only free data in quotes uris) */
			}
			g_free(cur->data);
		}
	}

	g_slist_free(uri_list);
}