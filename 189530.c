e_util_strv_to_slist (const gchar * const *strv)
{
	GSList *slist = NULL;
	gint ii;

	if (!strv)
		return NULL;

	for (ii = 0; strv[ii]; ii++) {
		slist = g_slist_prepend (slist, g_strdup (strv[ii]));
	}

	return g_slist_reverse (slist);
}