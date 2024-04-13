camel_imapx_server_ensure_selected_sync (CamelIMAPXServer *is,
					 CamelIMAPXMailbox *mailbox,
					 GCancellable *cancellable,
					 GError **error)
{
	CamelIMAPXCommand *ic;
	CamelIMAPXMailbox *selected_mailbox;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	if (g_cancellable_set_error_if_cancelled (cancellable, error))
		return FALSE;

	g_mutex_lock (&is->priv->select_lock);
	selected_mailbox = g_weak_ref_get (&is->priv->select_mailbox);
	if (selected_mailbox == mailbox) {
		g_mutex_unlock (&is->priv->select_lock);
		g_clear_object (&selected_mailbox);
		return TRUE;
	}

	g_clear_object (&selected_mailbox);

	ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_SELECT, "SELECT %M", mailbox);

	if (is->priv->use_qresync) {
		CamelFolder *folder;

		folder = imapx_server_ref_folder (is, mailbox);
		camel_imapx_command_add_qresync_parameter (ic, folder);
		g_clear_object (&folder);
	}

	g_weak_ref_set (&is->priv->select_pending, mailbox);
	g_mutex_unlock (&is->priv->select_lock);

	success = camel_imapx_server_process_command_sync (is, ic, _("Failed to select mailbox"), cancellable, error);

	camel_imapx_command_unref (ic);

	g_mutex_lock (&is->priv->select_lock);

	g_weak_ref_set (&is->priv->select_pending, NULL);

	if (success) {
		is->priv->state = IMAPX_SELECTED;
		g_weak_ref_set (&is->priv->select_mailbox, mailbox);
	}

	g_mutex_unlock (&is->priv->select_lock);

	return success;
}