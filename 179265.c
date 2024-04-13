imapx_server_fetch_changes (CamelIMAPXServer *is,
			    CamelIMAPXMailbox *mailbox,
			    CamelFolder *folder,
			    GHashTable *known_uids,
			    guint64 from_uidl,
			    guint64 to_uidl,
			    GCancellable *cancellable,
			    GError **error)
{
	GSList *fetch_summary_uids = NULL;
	GHashTable *infos; /* uid ~> FetchChangesInfo */
	CamelIMAPXCommand *ic;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	if (g_cancellable_set_error_if_cancelled (cancellable, error))
		return FALSE;

	if (!from_uidl)
		from_uidl = 1;

	if (to_uidl > 0) {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_REFRESH_INFO, "UID FETCH %lld:%lld (UID FLAGS)", from_uidl, to_uidl);
	} else {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_REFRESH_INFO, "UID FETCH %lld:* (UID FLAGS)", from_uidl);
	}

	g_return_val_if_fail (is->priv->fetch_changes_mailbox == NULL, FALSE);
	g_return_val_if_fail (is->priv->fetch_changes_folder == NULL, FALSE);
	g_return_val_if_fail (is->priv->fetch_changes_infos == NULL, FALSE);

	infos = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) camel_pstring_free, fetch_changes_info_free);

	is->priv->fetch_changes_mailbox = mailbox;
	is->priv->fetch_changes_folder = folder;
	is->priv->fetch_changes_infos = infos;
	is->priv->fetch_changes_last_progress = 0;

	camel_operation_push_message (cancellable,
		_("Scanning for changed messages in '%s'"),
		camel_folder_get_display_name (folder));

	success = camel_imapx_server_process_command_sync (is, ic, _("Error scanning changes"), cancellable, error);

	camel_operation_pop_message (cancellable);
	camel_imapx_command_unref (ic);

	/* It can partly succeed. */
	imapx_server_process_fetch_changes_infos (is, mailbox, folder, infos, known_uids, &fetch_summary_uids, from_uidl, to_uidl);

	g_hash_table_remove_all (infos);

	if (success && fetch_summary_uids) {
		struct _uidset_state uidset;
		GSList *link;

		ic = NULL;
		imapx_uidset_init (&uidset, 0, 100);

		camel_operation_push_message (cancellable,
			_("Fetching summary information for new messages in '%s'"),
			camel_folder_get_display_name (folder));

		fetch_summary_uids = g_slist_sort (fetch_summary_uids, imapx_uids_desc_cmp);

		for (link = fetch_summary_uids; link; link = g_slist_next (link)) {
			const gchar *uid = link->data;

			if (!uid)
				continue;

			if (!ic)
				ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_REFRESH_INFO, "UID FETCH ");

			if (imapx_uidset_add (&uidset, ic, uid) == 1 || (!link->next && ic && imapx_uidset_done (&uidset, ic))) {
				camel_imapx_command_add (ic, " (RFC822.SIZE RFC822.HEADER FLAGS)");

				success = camel_imapx_server_process_command_sync (is, ic, _("Error fetching message info"), cancellable, error);

				camel_imapx_command_unref (ic);
				ic = NULL;

				if (!success)
					break;

				imapx_server_process_fetch_changes_infos (is, mailbox, folder, infos, NULL, NULL, 0, 0);
				g_hash_table_remove_all (infos);
			}
		}

		camel_operation_pop_message (cancellable);

		imapx_server_process_fetch_changes_infos (is, mailbox, folder, infos, NULL, NULL, 0, 0);
	}

	g_return_val_if_fail (is->priv->fetch_changes_mailbox == mailbox, FALSE);
	g_return_val_if_fail (is->priv->fetch_changes_folder == folder, FALSE);
	g_return_val_if_fail (is->priv->fetch_changes_infos == infos, FALSE);

	is->priv->fetch_changes_mailbox = NULL;
	is->priv->fetch_changes_folder = NULL;
	is->priv->fetch_changes_infos = NULL;

	g_slist_free_full (fetch_summary_uids, (GDestroyNotify) camel_pstring_free);
	g_hash_table_destroy (infos);

	return success;
}