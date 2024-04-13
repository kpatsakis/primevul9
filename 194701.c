static gint textview_tag_cmp_list(gconstpointer a, gconstpointer b)
{
	gint id_a = GPOINTER_TO_INT(a);
	gint id_b = GPOINTER_TO_INT(b);
	const gchar *tag_a = tags_get_tag(id_a);
	const gchar *tag_b = tags_get_tag(id_b);
	
	if (tag_a == NULL)
		return tag_b == NULL ? 0:1;
	
	if (tag_b == NULL)
		return 1;

	return g_utf8_collate(tag_a, tag_b);
}