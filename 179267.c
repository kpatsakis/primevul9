camel_imapx_server_list_sync (CamelIMAPXServer *is,
			      const gchar *pattern,
			      CamelStoreGetFolderInfoFlags flags,
			      GCancellable *cancellable,
			      GError **error)
{
	CamelIMAPXCommand *ic;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (pattern != NULL, FALSE);

	if (is->priv->list_return_opts != NULL) {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_LIST, "LIST \"\" %s RETURN (%t)",
			pattern, is->priv->list_return_opts);
	} else {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_LIST, "LIST \"\" %s",
			pattern);
	}

	success = camel_imapx_server_process_command_sync (is, ic, _("Error fetching folders"), cancellable, error);

	camel_imapx_command_unref (ic);

	if (!success)
		return FALSE;

	if (!is->priv->list_return_opts) {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_LSUB, "LSUB \"\" %s",
			pattern);

		success = camel_imapx_server_process_command_sync (is, ic, _("Error fetching subscribed folders"), cancellable, error);

		camel_imapx_command_unref (ic);
	}

	return success;
}