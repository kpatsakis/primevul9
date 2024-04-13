camel_imapx_server_refresh_info_sync (CamelIMAPXServer *is,
				      CamelIMAPXMailbox *mailbox,
				      GCancellable *cancellable,
				      GError **error)
{
	CamelIMAPXCommand *ic;
	CamelIMAPXMailbox *selected_mailbox;
	CamelIMAPXSummary *imapx_summary;
	CamelFolder *folder;
	GHashTable *known_uids;
	guint32 messages;
	guint32 unseen;
	guint32 uidnext;
	guint32 uidvalidity;
	guint64 highestmodseq;
	guint32 total;
	guint64 uidl;
	gboolean need_rescan;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	selected_mailbox = camel_imapx_server_ref_pending_or_selected (is);
	if (selected_mailbox == mailbox) {
		success = camel_imapx_server_noop_sync (is, mailbox, cancellable, error);
	} else {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_STATUS, "STATUS %M (%t)", mailbox, is->priv->status_data_items);

		success = camel_imapx_server_process_command_sync (is, ic, _("Error running STATUS"), cancellable, error);

		camel_imapx_command_unref (ic);
	}
	g_clear_object (&selected_mailbox);

	if (!success)
		return FALSE;

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_val_if_fail (folder != NULL, FALSE);

	imapx_summary = CAMEL_IMAPX_SUMMARY (folder->summary);

	messages = camel_imapx_mailbox_get_messages (mailbox);
	unseen = camel_imapx_mailbox_get_unseen (mailbox);
	uidnext = camel_imapx_mailbox_get_uidnext (mailbox);
	uidvalidity = camel_imapx_mailbox_get_uidvalidity (mailbox);
	highestmodseq = camel_imapx_mailbox_get_highestmodseq (mailbox);
	total = camel_folder_summary_count (folder->summary);

	need_rescan =
		(uidvalidity > 0 && uidvalidity != imapx_summary->validity) ||
		total != messages ||
		imapx_summary->uidnext != uidnext ||
		camel_folder_summary_get_unread_count (folder->summary) != unseen ||
		imapx_summary->modseq != highestmodseq;

	if (!need_rescan) {
		g_object_unref (folder);
		return TRUE;
	}

	if (!camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error)) {
		g_object_unref (folder);
		return FALSE;
	}

	if (is->priv->use_qresync && imapx_summary->modseq > 0 && uidvalidity > 0) {
		imapx_summary->modseq = highestmodseq;
		if (total != messages ||
		    camel_folder_summary_get_unread_count (folder->summary) != unseen ||
		    imapx_summary->modseq != highestmodseq) {
			c (
				is->priv->tagprefix,
				"Eep, after QRESYNC we're out of sync. "
				"total %u / %u, unread %u / %u, modseq %"
				G_GUINT64_FORMAT " / %" G_GUINT64_FORMAT "\n",
				total, messages,
				camel_folder_summary_get_unread_count (folder->summary),
				unseen,
				imapx_summary->modseq,
				highestmodseq);
		} else {
			c (
				is->priv->tagprefix,
				"OK, after QRESYNC we're still in sync. "
				"total %u / %u, unread %u / %u, modseq %"
				G_GUINT64_FORMAT " / %" G_GUINT64_FORMAT "\n",
				total, messages,
				camel_folder_summary_get_unread_count (folder->summary),
				unseen,
				imapx_summary->modseq,
				highestmodseq);
			g_object_unref (folder);
			return TRUE;
		}
	}

	if (total > 0) {
		gchar *uid = camel_imapx_dup_uid_from_summary_index (folder, total - 1);
		if (uid) {
			uidl = g_ascii_strtoull (uid, NULL, 10);
			g_free (uid);
			uidl++;
		} else {
			uidl = 1;
		}
	} else {
		uidl = 1;
	}

	camel_folder_summary_prepare_fetch_all (folder->summary, NULL);

	known_uids = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) camel_pstring_free, NULL);

	success = imapx_server_fetch_changes (is, mailbox, folder, known_uids, uidl, 0, cancellable, error);
	if (success && uidl != 1)
		success = imapx_server_fetch_changes (is, mailbox, folder, known_uids, 0, uidl, cancellable, error);

	if (success) {
		CamelFolderChangeInfo *changes;
		GList *removed = NULL;
		GPtrArray *array;
		gint ii;

		camel_folder_summary_lock (folder->summary);

		changes = camel_folder_change_info_new ();

		array = camel_folder_summary_get_array (folder->summary);
		for (ii = 0; array && ii < array->len; ii++) {
			const gchar *uid = array->pdata[ii];

			if (!uid)
				continue;

			if (!g_hash_table_contains (known_uids, uid)) {
				removed = g_list_prepend (removed, (gpointer) uid);
				camel_folder_change_info_remove_uid (changes, uid);
			}
		}

		camel_folder_summary_unlock (folder->summary);

		if (removed != NULL) {
			camel_folder_summary_remove_uids (folder->summary, removed);
			camel_folder_summary_touch (folder->summary);

			/* Shares UIDs with the 'array'. */
			g_list_free (removed);
		}

		if (camel_folder_change_info_changed (changes)) {
			camel_folder_summary_save_to_db (folder->summary, NULL);
			imapx_update_store_summary (folder);
			camel_folder_changed (folder, changes);
		}

		camel_folder_change_info_free (changes);
		camel_folder_summary_free_array (array);
	}

	g_hash_table_destroy (known_uids);
	g_object_unref (folder);

	return success;
}