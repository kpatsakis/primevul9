static void textview_shift_uris_after(TextView *textview, GSList *start_list, gint start, gint shift)
{
	GSList *cur;
	if (!start_list)
		start_list = textview->uri_list;

	for (cur = start_list; cur; cur = cur->next) {
		ClickableText *uri = (ClickableText *)cur->data;
		if (uri->start <= start)
			continue;
		uri->start += shift;
		uri->end += shift;
	}
}