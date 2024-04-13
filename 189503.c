e_util_copy_string_slist (GSList *copy_to,
                          const GSList *strings)
{
	GSList *copied_list;

	copied_list = g_slist_copy_deep (
		(GSList *) strings, (GCopyFunc) g_strdup, NULL);

	return g_slist_concat (copy_to, copied_list);
}