camel_imapx_server_sync_message_sync (CamelIMAPXServer *is,
				      CamelIMAPXMailbox *mailbox,
				      CamelFolderSummary *summary,
				      CamelDataCache *message_cache,
				      const gchar *message_uid,
				      GCancellable *cancellable,
				      GError **error)
{
	gchar *cache_file = NULL;
	gboolean is_cached;
	struct stat st;
	gboolean success = TRUE;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);
	g_return_val_if_fail (CAMEL_IS_FOLDER_SUMMARY (summary), FALSE);
	g_return_val_if_fail (CAMEL_IS_DATA_CACHE (message_cache), FALSE);
	g_return_val_if_fail (message_uid != NULL, FALSE);

	/* Check if the cache file already exists and is non-empty. */
	cache_file = camel_data_cache_get_filename (message_cache, "cur", message_uid);
	is_cached = (g_stat (cache_file, &st) == 0 && st.st_size > 0);
	g_free (cache_file);

	if (!is_cached) {
		CamelStream *stream;

		stream = camel_imapx_server_get_message_sync (
			is, mailbox, summary,
			message_cache, message_uid,
			cancellable, error);

		success = (stream != NULL);

		g_clear_object (&stream);
	}

	return success;
}