imapx_server_ref_folder (CamelIMAPXServer *is,
                         CamelIMAPXMailbox *mailbox)
{
	CamelFolder *folder;
	CamelIMAPXStore *store;
	gchar *folder_path;
	GError *local_error = NULL;

	store = camel_imapx_server_ref_store (is);

	folder_path = camel_imapx_mailbox_dup_folder_path (mailbox);

	folder = camel_store_get_folder_sync (
		CAMEL_STORE (store), folder_path, 0, NULL, &local_error);

	g_free (folder_path);

	g_object_unref (store);

	/* Sanity check. */
	g_warn_if_fail (
		((folder != NULL) && (local_error == NULL)) ||
		((folder == NULL) && (local_error != NULL)));

	if (local_error != NULL) {
		g_warning (
			"%s: Failed to get folder for '%s': %s",
			G_STRFUNC, camel_imapx_mailbox_get_name (mailbox), local_error->message);
		g_error_free (local_error);
	}

	return folder;
}