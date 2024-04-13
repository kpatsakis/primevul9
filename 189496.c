e_util_free_string_slist (GSList *strings)
{
	g_slist_free_full (strings, (GDestroyNotify) g_free);
}