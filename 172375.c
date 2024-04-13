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
		gboolean request_noop;
		gint change_stamp;

		change_stamp = selected_mailbox ? camel_imapx_mailbox_get_change_stamp (selected_mailbox) : 0;
		request_noop = selected_mailbox && is->priv->last_selected_mailbox_change_stamp != change_stamp;

		if (request_noop)
			is->priv->last_selected_mailbox_change_stamp = change_stamp;

		g_mutex_unlock (&is->priv->select_lock);
		g_clear_object (&selected_mailbox);

		if (request_noop) {
			c (is->priv->tagprefix, "%s: Selected mailbox '%s' changed, do NOOP instead\n", G_STRFUNC, camel_imapx_mailbox_get_name (mailbox));

			return camel_imapx_server_noop_sync (is, mailbox, cancellable, error);
		}

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
		is->priv->last_selected_mailbox_change_stamp = camel_imapx_mailbox_get_change_stamp (mailbox);
		g_weak_ref_set (&is->priv->select_mailbox, mailbox);
	} else {
		is->priv->state = IMAPX_INITIALISED;
		is->priv->last_selected_mailbox_change_stamp = 0;
		g_weak_ref_set (&is->priv->select_mailbox, NULL);
	}

	g_mutex_unlock (&is->priv->select_lock);

	return success;
}