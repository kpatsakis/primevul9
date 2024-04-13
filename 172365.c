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
		/* Translators: The first “%s” is replaced with an account name and the second “%s”
		   is replaced with a full path name. The spaces around “:” are intentional, as
		   the whole “%s : %s” is meant as an absolute identification of the folder. */
		_("Scanning for changed messages in “%s : %s”"),
		camel_service_get_display_name (CAMEL_SERVICE (camel_folder_get_parent_store (folder))),
		camel_folder_get_full_name (folder));

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
			/* Translators: The first “%s” is replaced with an account name and the second “%s”
			   is replaced with a full path name. The spaces around “:” are intentional, as
			   the whole “%s : %s” is meant as an absolute identification of the folder. */
			_("Fetching summary information for new messages in “%s : %s”"),
			camel_service_get_display_name (CAMEL_SERVICE (camel_folder_get_parent_store (folder))),
			camel_folder_get_full_name (folder));

		fetch_summary_uids = g_slist_sort (fetch_summary_uids, imapx_uids_desc_cmp);

		for (link = fetch_summary_uids; link; link = g_slist_next (link)) {
			const gchar *uid = link->data;

			if (!uid)
				continue;

			if (!ic)
				ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_REFRESH_INFO, "UID FETCH ");

			if (imapx_uidset_add (&uidset, ic, uid) == 1 || (!link->next && ic && imapx_uidset_done (&uidset, ic))) {
				GError *local_error = NULL;
				gboolean bodystructure_enabled;
				CamelIMAPXStore *imapx_store;

				imapx_store = camel_imapx_server_ref_store (is);
				bodystructure_enabled = imapx_store && camel_imapx_store_get_bodystructure_enabled (imapx_store);

				if (bodystructure_enabled)
					camel_imapx_command_add (ic, " (RFC822.SIZE RFC822.HEADER BODYSTRUCTURE FLAGS)");
				else
					camel_imapx_command_add (ic, " (RFC822.SIZE RFC822.HEADER FLAGS)");

				success = camel_imapx_server_process_command_sync (is, ic, _("Error fetching message info"), cancellable, &local_error);

				camel_imapx_command_unref (ic);
				ic = NULL;

				/* Some servers can return broken BODYSTRUCTURE response, thus disable it
				   even when it's not 100% sure the BODYSTRUCTURE response was the broken one. */
				if (bodystructure_enabled && !success &&
				    g_error_matches (local_error, CAMEL_IMAPX_ERROR, CAMEL_IMAPX_ERROR_SERVER_RESPONSE_MALFORMED)) {
					camel_imapx_store_set_bodystructure_enabled (imapx_store, FALSE);
					local_error->domain = CAMEL_IMAPX_SERVER_ERROR;
					local_error->code = CAMEL_IMAPX_SERVER_ERROR_TRY_RECONNECT;
				}

				g_clear_object (&imapx_store);

				if (local_error)
					g_propagate_error (error, local_error);

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

	g_mutex_lock (&is->priv->changes_lock);

	/* Notify about new messages, thus they are shown in the UI early. */
	if (camel_folder_change_info_changed (is->priv->changes)) {
		CamelFolderChangeInfo *changes;

		changes = is->priv->changes;
		is->priv->changes = camel_folder_change_info_new ();

		g_mutex_unlock (&is->priv->changes_lock);

		camel_folder_summary_save (camel_folder_get_folder_summary (folder), NULL);
		imapx_update_store_summary (folder);
		camel_folder_changed (folder, changes);
		camel_folder_change_info_free (changes);
	} else {
		g_mutex_unlock (&is->priv->changes_lock);
	}

	return success;
}