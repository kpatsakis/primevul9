camel_imapx_server_update_quota_info_sync (CamelIMAPXServer *is,
					   CamelIMAPXMailbox *mailbox,
					   GCancellable *cancellable,
					   GError **error)
{
	CamelIMAPXCommand *ic;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	g_mutex_lock (&is->priv->stream_lock);

	if (CAMEL_IMAPX_LACK_CAPABILITY (is->priv->cinfo, QUOTA)) {
		g_mutex_unlock (&is->priv->stream_lock);

		g_set_error_literal (
			error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
			_("IMAP server does not support quotas"));
		return FALSE;
	} else {
		g_mutex_unlock (&is->priv->stream_lock);
	}

	success = camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error);
	if (!success)
		return FALSE;

	ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_UPDATE_QUOTA_INFO, "GETQUOTAROOT %M", mailbox);

	success = camel_imapx_server_process_command_sync (is, ic, _("Error retrieving quota information"), cancellable, error);

	camel_imapx_command_unref (ic);

	return success;
}