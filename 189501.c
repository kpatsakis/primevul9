e_util_copy_object_slist (GSList *copy_to,
                          const GSList *objects)
{
	GSList *copied_list;

	copied_list = g_slist_copy_deep (
		(GSList *) objects, (GCopyFunc) g_object_ref, NULL);

	return g_slist_concat (copy_to, copied_list);
}