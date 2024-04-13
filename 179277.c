camel_imapx_server_expunge_sync (CamelIMAPXServer *is,
				 CamelIMAPXMailbox *mailbox,
				 GCancellable *cancellable,
				 GError **error)
{
	CamelFolder *folder;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_val_if_fail (folder != NULL, FALSE);

	success = camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error);

	if (success) {
		CamelIMAPXCommand *ic;

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_EXPUNGE, "EXPUNGE");

		success = camel_imapx_server_process_command_sync (is, ic, _("Error expunging message"), cancellable, error);
		if (success) {
			GPtrArray *uids;
			CamelStore *parent_store;
			const gchar *full_name;

			full_name = camel_folder_get_full_name (folder);
			parent_store = camel_folder_get_parent_store (folder);

			camel_folder_summary_lock (folder->summary);

			camel_folder_summary_save_to_db (folder->summary, NULL);
			uids = camel_db_get_folder_deleted_uids (parent_store->cdb_r, full_name, NULL);

			if (uids && uids->len) {
				CamelFolderChangeInfo *changes;
				GList *removed = NULL;
				gint i;

				changes = camel_folder_change_info_new ();
				for (i = 0; i < uids->len; i++) {
					camel_folder_change_info_remove_uid (changes, uids->pdata[i]);
					removed = g_list_prepend (removed, (gpointer) uids->pdata[i]);
				}

				camel_folder_summary_remove_uids (folder->summary, removed);
				camel_folder_summary_save_to_db (folder->summary, NULL);

				camel_folder_changed (folder, changes);
				camel_folder_change_info_free (changes);

				g_list_free (removed);
				g_ptr_array_foreach (uids, (GFunc) camel_pstring_free, NULL);
			}

			if (uids)
				g_ptr_array_free (uids, TRUE);

			camel_folder_summary_unlock (folder->summary);
		}

		camel_imapx_command_unref (ic);
	}

	g_clear_object (&folder);

	return success;
}