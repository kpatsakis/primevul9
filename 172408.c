camel_imapx_server_delete_mailbox_sync (CamelIMAPXServer *is,
					CamelIMAPXMailbox *mailbox,
					GCancellable *cancellable,
					GError **error)
{
	CamelIMAPXCommand *ic;
	CamelIMAPXMailbox *inbox;
	CamelIMAPXStore *imapx_store;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	/* Avoid camel_imapx_job_set_mailbox() here.  We
	 * don't want to select the mailbox to be deleted. */

	imapx_store = camel_imapx_server_ref_store (is);
	/* Keep going, even if this returns NULL. */
	inbox = camel_imapx_store_ref_mailbox (imapx_store, "INBOX");

	/* Make sure the to-be-deleted folder is not
	 * selected by selecting INBOX for this operation. */
	success = camel_imapx_server_ensure_selected_sync (is, inbox, cancellable, error);
	if (!success) {
		g_clear_object (&inbox);
		g_clear_object (&imapx_store);
		return FALSE;
	}

	/* Just to make sure it'll not disappeare before the end of this function */
	g_object_ref (mailbox);

	ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_DELETE_MAILBOX, "DELETE %M", mailbox);

	success = camel_imapx_server_process_command_sync (is, ic, _("Error deleting folder"), cancellable, error);

	camel_imapx_command_unref (ic);

	if (success) {
		camel_imapx_mailbox_deleted (mailbox);
		camel_imapx_store_emit_mailbox_updated (imapx_store, mailbox);
	}

	g_clear_object (&inbox);
	g_clear_object (&imapx_store);
	g_clear_object (&mailbox);

	return success;
}