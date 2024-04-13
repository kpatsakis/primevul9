camel_imapx_server_copy_message_sync (CamelIMAPXServer *is,
				      CamelIMAPXMailbox *mailbox,
				      CamelIMAPXMailbox *destination,
				      GPtrArray *uids,
				      gboolean delete_originals,
				      gboolean remove_deleted_flags,
				      GCancellable *cancellable,
				      GError **error)
{
	GPtrArray *data_uids;
	gint ii;
	gboolean use_move_command = FALSE;
	CamelIMAPXCommand *ic;
	CamelFolder *folder;
	GHashTable *source_infos;
	gboolean remove_junk_flags;
	gboolean success = TRUE;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (destination), FALSE);
	g_return_val_if_fail (uids != NULL, FALSE);

	/* To get permanent flags. That's okay if the "SELECT" fails here, as it can be
	   due to the folder being write-only; just ignore the error and continue. */
	camel_imapx_server_ensure_selected_sync (is, destination, cancellable, NULL);

	if (g_cancellable_set_error_if_cancelled (cancellable, error))
		return FALSE;

	if (!camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error))
		return FALSE;

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_val_if_fail (folder != NULL, FALSE);

	remove_deleted_flags = remove_deleted_flags || (folder->folder_flags & CAMEL_FOLDER_IS_TRASH) != 0;
	remove_junk_flags = (folder->folder_flags & CAMEL_FOLDER_IS_JUNK) != 0;

	/* If we're moving messages, prefer "UID MOVE" if supported. */
	if (delete_originals) {
		g_mutex_lock (&is->priv->stream_lock);

		if (CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, MOVE)) {
			delete_originals = FALSE;
			use_move_command = TRUE;
		}

		g_mutex_unlock (&is->priv->stream_lock);
	}

	source_infos = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, camel_message_info_unref);
	data_uids = g_ptr_array_new ();

	for (ii = 0; ii < uids->len; ii++) {
		gchar *uid = (gchar *) camel_pstring_strdup (uids->pdata[ii]);

		g_ptr_array_add (data_uids, uid);
		g_hash_table_insert (source_infos, uid, camel_folder_summary_get (folder->summary, uid));
	}

	g_ptr_array_sort (data_uids, (GCompareFunc) imapx_uids_array_cmp);

	ii = 0;
	while (ii < data_uids->len && success) {
		struct _uidset_state uidset;
		gint last_index = ii;

		imapx_uidset_init (&uidset, 0, MAX_COMMAND_LEN);

		if (use_move_command)
			ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_MOVE_MESSAGE, "UID MOVE ");
		else
			ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_COPY_MESSAGE, "UID COPY ");

		while (ii < data_uids->len) {
			const gchar *uid = (gchar *) g_ptr_array_index (data_uids, ii);

			ii++;

			if (imapx_uidset_add (&uidset, ic, uid) == 1)
				break;
		}

		imapx_uidset_done (&uidset, ic);

		camel_imapx_command_add (ic, " %M", destination);

		imapx_free_status (is->priv->copyuid_status);
		is->priv->copyuid_status = NULL;

		success = camel_imapx_server_process_command_sync (is, ic,
			use_move_command ? _("Error moving messages") : _("Error copying messages"),
			cancellable, error);

		if (success) {
			struct _status_info *copyuid_status = is->priv->copyuid_status;

			if (ic->status && ic->status->condition == IMAPX_COPYUID)
				copyuid_status = ic->status;

			if (copyuid_status && copyuid_status->u.copyuid.uids &&
			    copyuid_status->u.copyuid.copied_uids &&
			    copyuid_status->u.copyuid.uids->len == copyuid_status->u.copyuid.copied_uids->len) {
				CamelFolder *destination_folder;

				destination_folder = imapx_server_ref_folder (is, destination);
				if (destination_folder) {
					CamelMessageInfo *source_info, *destination_info;
					CamelFolderChangeInfo *changes;
					gint ii;

					changes = camel_folder_change_info_new ();

					for (ii = 0; ii < copyuid_status->u.copyuid.uids->len; ii++) {
						gchar *uid;
						gboolean is_new = FALSE;

						uid = g_strdup_printf ("%d", g_array_index (copyuid_status->u.copyuid.uids, guint32, ii));
						source_info = g_hash_table_lookup (source_infos, uid);
						g_free (uid);

						if (!source_info)
							continue;

						uid = g_strdup_printf ("%d", g_array_index (copyuid_status->u.copyuid.copied_uids, guint32, ii));
						destination_info = camel_folder_summary_get (folder->summary, uid);

						if (!destination_info) {
							is_new = TRUE;
							destination_info = camel_message_info_clone (source_info);
							destination_info->summary = destination_folder->summary;
							camel_pstring_free (destination_info->uid);
							destination_info->uid = camel_pstring_strdup (uid);
						}

						g_free (uid);

						imapx_set_message_info_flags_for_new_message (
							destination_info,
							((CamelMessageInfoBase *) source_info)->flags,
							((CamelMessageInfoBase *) source_info)->user_flags,
							TRUE,
							((CamelMessageInfoBase *) source_info)->user_tags,
							camel_imapx_mailbox_get_permanentflags (destination));
						if (remove_deleted_flags)
							camel_message_info_set_flags (destination_info, CAMEL_MESSAGE_DELETED, 0);
						if (remove_junk_flags)
							camel_message_info_set_flags (destination_info, CAMEL_MESSAGE_JUNK, 0);
						if (is_new)
							camel_folder_summary_add (destination_folder->summary, destination_info);
						camel_folder_change_info_add_uid (changes, destination_info->uid);

						if (!is_new)
							camel_message_info_unref (destination_info);
					}

					if (camel_folder_change_info_changed (changes)) {
						camel_folder_summary_touch (destination_folder->summary);
						camel_folder_summary_save_to_db (destination_folder->summary, NULL);
						camel_folder_changed (destination_folder, changes);
					}

					camel_folder_change_info_free (changes);
					g_object_unref (destination_folder);
				}
			}

			if (delete_originals || use_move_command) {
				CamelFolderChangeInfo *changes = NULL;
				gint jj;

				camel_folder_freeze (folder);

				for (jj = last_index; jj < ii; jj++) {
					const gchar *uid = uids->pdata[jj];

					if (delete_originals) {
						camel_folder_delete_message (folder, uid);
					} else {
						if (camel_folder_summary_remove_uid (folder->summary, uid)) {
							if (!changes)
								changes = camel_folder_change_info_new ();

							camel_folder_change_info_remove_uid (changes, uid);
						}
					}
				}

				if (changes && camel_folder_change_info_changed (changes)) {
					camel_folder_summary_touch (folder->summary);
					camel_folder_summary_save_to_db (folder->summary, NULL);
					camel_folder_changed (folder, changes);
				}

				camel_folder_thaw (folder);

				if (changes)
					camel_folder_change_info_free (changes);
			}
		}

		imapx_free_status (is->priv->copyuid_status);
		is->priv->copyuid_status = NULL;

		camel_imapx_command_unref (ic);
	}

	g_hash_table_destroy (source_infos);
	g_ptr_array_foreach (data_uids, (GFunc) camel_pstring_free, NULL);
	g_ptr_array_free (data_uids, TRUE);
	g_object_unref (folder);

	return success;
}