static void textview_show_tags(TextView *textview)
{
	MsgInfo *msginfo = textview->messageview->msginfo;
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(text);
	GtkTextIter iter;
	ClickableText *uri;
	GSList *cur, *orig;
	gboolean found_tag = FALSE;
	
	if (!msginfo->tags)
		return;
	
	cur = orig = g_slist_sort(g_slist_copy(msginfo->tags), textview_tag_cmp_list);

	for (; cur; cur = cur->next) {
		if (tags_get_tag(GPOINTER_TO_INT(cur->data)) != NULL) {
			found_tag = TRUE;
			break;
		}
	}
	if (!found_tag) {
		g_slist_free(orig);
		return;
	}

	gtk_text_buffer_get_end_iter (buffer, &iter);
	gtk_text_buffer_insert_with_tags_by_name(buffer,
		&iter, _("Tags: "), -1,
		"header_title", "header", "tags", NULL);

	for (cur = orig; cur; cur = cur->next) {
		const gchar *cur_tag = tags_get_tag(GPOINTER_TO_INT(cur->data));
		if (!cur_tag)
			continue;
		uri = g_new0(ClickableText, 1);
		uri->uri = g_strdup("");
		uri->start = gtk_text_iter_get_offset(&iter);
		gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, 
			cur_tag, -1,
			"link", "header", "tags", NULL);
		uri->end = gtk_text_iter_get_offset(&iter);
		uri->filename = g_strdup_printf("sc://search_tags:%s", cur_tag);
		uri->data = NULL;
		textview->uri_list =
			g_slist_prepend(textview->uri_list, uri);
		if (cur->next && tags_get_tag(GPOINTER_TO_INT(cur->next->data)))
			gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, ", ", 2,
				"header", "tags", NULL);
		else
			gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, " ", 1,
				"header", "tags", NULL);
	}
	g_slist_free(orig);

	gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "\n", 1,
		"header", "tags", NULL);
}