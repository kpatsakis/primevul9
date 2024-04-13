fetch_changes_info_free (gpointer ptr)
{
	FetchChangesInfo *nfo = ptr;

	if (nfo) {
		camel_flag_list_free (&nfo->server_user_flags);
		g_free (nfo);
	}
}