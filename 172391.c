imapx_server_process_fetch_changes_infos (CamelIMAPXServer *is,
					  CamelIMAPXMailbox *mailbox,
					  CamelFolder *folder,
					  GHashTable *infos,
					  GHashTable *known_uids,
					  GSList **out_fetch_summary_uids,
					  guint64 from_uidl,
					  guint64 to_uidl)
{
	GHashTableIter iter;
	gpointer key, value;
	CamelFolderSummary *summary;

	g_return_if_fail (CAMEL_IS_IMAPX_SERVER (is));
	g_return_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox));
	g_return_if_fail (CAMEL_IS_FOLDER (folder));
	g_return_if_fail (infos != NULL);

	if (out_fetch_summary_uids)
		g_return_if_fail (*out_fetch_summary_uids == NULL);

	summary = camel_folder_get_folder_summary (folder);

	g_hash_table_iter_init (&iter, infos);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		const gchar *uid = key;
		FetchChangesInfo *nfo = value;
		CamelMessageInfo *minfo;

		if (!uid || !nfo)
			continue;

		if (known_uids)
			g_hash_table_insert (known_uids, (gpointer) camel_pstring_strdup (uid), GINT_TO_POINTER (1));

		if (!camel_folder_summary_check_uid (summary, uid) ||
		    !(minfo = camel_folder_summary_get (summary, uid))) {
			if (out_fetch_summary_uids) {
				*out_fetch_summary_uids = g_slist_prepend (*out_fetch_summary_uids,
					(gpointer) camel_pstring_strdup (uid));
			}

			continue;
		}

		if (imapx_update_message_info_flags (
			minfo,
			nfo->server_flags,
			nfo->server_user_flags,
			camel_imapx_mailbox_get_permanentflags (mailbox),
			folder, FALSE)) {
			g_mutex_lock (&is->priv->changes_lock);
			camel_folder_change_info_change_uid (is->priv->changes, camel_message_info_get_uid (minfo));
			g_mutex_unlock (&is->priv->changes_lock);
		}

		g_clear_object (&minfo);
	}
}