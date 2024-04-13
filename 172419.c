fetch_changes_info_free (gpointer ptr)
{
	FetchChangesInfo *nfo = ptr;

	if (nfo) {
		camel_named_flags_free (nfo->server_user_flags);
		g_slice_free (FetchChangesInfo, nfo);
	}
}