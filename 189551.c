e_util_free_object_slist (GSList *objects)
{
	g_slist_free_full (objects, (GDestroyNotify) g_object_unref);
}