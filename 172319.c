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
	CamelFolderChangeInfo *changes = NULL;
	GHashTable *source_infos;
	gboolean remove_junk_flags;
	gboolean success = TRUE;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (destination), FALSE);
	g_return_val_if_fail (uids != NULL, FALSE);

	if (camel_imapx_mailbox_get_permanentflags (destination) == ~0) {
		/* To get permanent flags. That's okay if the "SELECT" fails here, as it can be
		   due to the folder being write-only; just ignore the error and continue. */
		if (!camel_imapx_server_ensure_selected_sync (is, destination, cancellable, NULL)) {
			;
		}
	}

	if (g_cancellable_set_error_if_cancelled (cancellable, error))
		return FALSE;

	if (!camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error))
		return FALSE;

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_val_if_fail (folder != NULL, FALSE);

	remove_deleted_flags = remove_deleted_flags || (camel_folder_get_flags (folder) & CAMEL_FOLDER_IS_TRASH) != 0;
	remove_junk_flags = (camel_folder_get_flags (folder) & CAMEL_FOLDER_IS_JUNK) != 0;

	/* If we're moving messages, prefer "UID MOVE" if supported. */
	if (delete_originals) {
		g_mutex_lock (&is->priv->stream_lock);

		if (CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, MOVE)) {
			delete_originals = FALSE;
			use_move_command = TRUE;
		}

		g_mutex_unlock (&is->priv->stream_lock);
	}

	source_infos = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_object_unref);
	data_uids = g_ptr_array_new ();

	for (ii = 0; ii < uids->len; ii++) {
		CamelMessageInfo *source_info;
		gchar *uid = (gchar *) camel_pstring_strdup (uids->pdata[ii]);

		g_ptr_array_add (data_uids, uid);

		source_info = camel_folder_summary_get (camel_folder_get_folder_summary (folder), uid);
		if (source_info)
			g_hash_table_insert (source_infos, uid, source_info);
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

		g_warn_if_fail (imapx_uidset_done (&uidset, ic));

		camel_imapx_command_add (ic, " %M", destination);

		imapx_free_status (is->priv->copyuid_status);
		is->priv->copyuid_status = NULL;

		success = camel_imapx_server_process_command_sync (is, ic,
			use_move_command ? _("Error moving messages") : _("Error copying messages"),
			cancellable, error);

		g_mutex_lock (&is->priv->changes_lock);
		if (camel_folder_change_info_changed (is->priv->changes)) {
			if (!changes) {
				changes = is->priv->changes;
			} else {
				camel_folder_change_info_cat (changes, is->priv->changes);
				camel_folder_change_info_free (is->priv->changes);
			}

			is->priv->changes = camel_folder_change_info_new ();
		}
		g_mutex_unlock (&is->priv->changes_lock);

		if (ic->copy_move_expunged) {
			CamelFolderSummary *summary;

			ic->copy_move_expunged = g_slist_reverse (ic->copy_move_expunged);

			summary = camel_folder_get_folder_summary (folder);
			if (summary) {
				GPtrArray *array;

				array = camel_folder_summary_get_array (summary);
				if (array) {
					GSList *slink;
					GList *removed_uids = NULL, *llink;

					camel_folder_sort_uids (folder, array);

					for (slink = ic->copy_move_expunged; slink; slink = g_slist_next (slink)) {
						guint expunged_idx = GPOINTER_TO_UINT (slink->data) - 1;

						if (expunged_idx < array->len) {
							const gchar *uid = g_ptr_array_index (array, expunged_idx);

							if (uid) {
								removed_uids = g_list_prepend (removed_uids, (gpointer) uid);
								g_ptr_array_remove_index (array, expunged_idx);
							}
						}
					}

					if (removed_uids) {
						CamelFolderSummary *summary;

						summary = camel_folder_get_folder_summary (folder);

						camel_folder_summary_remove_uids (summary, removed_uids);

						for (llink = removed_uids; llink; llink = g_list_next (llink)) {
							const gchar *uid = llink->data;

							if (!changes)
								changes = camel_folder_change_info_new ();

							camel_folder_change_info_remove_uid (changes, uid);

							g_ptr_array_add (array, (gpointer) uid);
						}

						g_list_free (removed_uids);
					}

					camel_folder_summary_free_array (array);
				}
			}
		}

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
					CamelFolderSummary *destination_summary;
					CamelMessageInfo *source_info, *destination_info;
					CamelFolderChangeInfo *dest_changes;
					gint ii;

					destination_summary = camel_folder_get_folder_summary (destination_folder);
					camel_folder_summary_lock (destination_summary);

					dest_changes = camel_folder_change_info_new ();

					for (ii = 0; ii < copyuid_status->u.copyuid.uids->len; ii++) {
						gchar *uid;
						gboolean is_new = FALSE;
						guint32 source_flags;
						CamelNamedFlags *source_user_flags;
						CamelNameValueArray *source_user_tags;

						uid = g_strdup_printf ("%u", g_array_index (copyuid_status->u.copyuid.uids, guint32, ii));
						source_info = g_hash_table_lookup (source_infos, uid);
						g_free (uid);

						if (!source_info)
							continue;

						uid = g_strdup_printf ("%u", g_array_index (copyuid_status->u.copyuid.copied_uids, guint32, ii));
						destination_info = camel_folder_summary_get (destination_summary, uid);

						if (!destination_info) {
							is_new = TRUE;
							destination_info = camel_message_info_clone (source_info, destination_summary);
							camel_message_info_set_uid (destination_info, uid);
						}

						g_free (uid);

						source_flags = camel_message_info_get_flags (source_info);
						source_user_flags = camel_message_info_dup_user_flags (source_info);
						source_user_tags = camel_message_info_dup_user_tags (source_info);

						imapx_set_message_info_flags_for_new_message (
							destination_info,
							source_flags,
							source_user_flags,
							TRUE,
							source_user_tags,
							camel_imapx_mailbox_get_permanentflags (destination));

						camel_named_flags_free (source_user_flags);
						camel_name_value_array_free (source_user_tags);

						if (remove_deleted_flags)
							camel_message_info_set_flags (destination_info, CAMEL_MESSAGE_DELETED, 0);
						if (remove_junk_flags)
							camel_message_info_set_flags (destination_info, CAMEL_MESSAGE_JUNK, 0);
						imapx_copy_move_message_cache (folder, destination_folder, delete_originals || use_move_command,
							camel_message_info_get_uid (source_info),
							camel_message_info_get_uid (destination_info),
							cancellable);
						if (is_new)
							camel_folder_summary_add (destination_summary, destination_info, FALSE);
						camel_folder_change_info_add_uid (dest_changes, camel_message_info_get_uid (destination_info));

						g_clear_object (&destination_info);
					}

					if (camel_folder_change_info_changed (dest_changes)) {
						camel_folder_summary_touch (destination_summary);
						camel_folder_summary_save (destination_summary, NULL);
						imapx_update_store_summary (destination_folder);
						camel_folder_changed (destination_folder, dest_changes);
					}

					camel_folder_summary_unlock (destination_summary);
					camel_folder_change_info_free (dest_changes);
					g_object_unref (destination_folder);
				}
			}

			if (delete_originals || use_move_command) {
				gint jj;

				for (jj = last_index; jj < ii; jj++) {
					const gchar *uid = uids->pdata[jj];

					if (delete_originals) {
						camel_folder_delete_message (folder, uid);
					} else {
						if (camel_folder_summary_remove_uid (camel_folder_get_folder_summary (folder), uid)) {
							if (!changes)
								changes = camel_folder_change_info_new ();

							camel_folder_change_info_remove_uid (changes, uid);
						}
					}
				}
			}
		}

		imapx_free_status (is->priv->copyuid_status);
		is->priv->copyuid_status = NULL;

		camel_imapx_command_unref (ic);

		camel_operation_progress (cancellable, ii * 100 / data_uids->len);
	}

	if (changes) {
		if (camel_folder_change_info_changed (changes)) {
			camel_folder_summary_touch (camel_folder_get_folder_summary (folder));
			camel_folder_summary_save (camel_folder_get_folder_summary (folder), NULL);

			imapx_update_store_summary (folder);

			camel_folder_changed (folder, changes);
		}

		camel_folder_change_info_free (changes);
	}

	g_hash_table_destroy (source_infos);
	g_ptr_array_foreach (data_uids, (GFunc) camel_pstring_free, NULL);
	g_ptr_array_free (data_uids, TRUE);
	g_object_unref (folder);

	return success;
}