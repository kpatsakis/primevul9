camel_imapx_server_unsubscribe_mailbox_sync (CamelIMAPXServer *is,
					     CamelIMAPXMailbox *mailbox,
					     GCancellable *cancellable,
					     GError **error)
{
	CamelIMAPXCommand *ic;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	/* We don't want to select the mailbox to be unsubscribed. */
	ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_UNSUBSCRIBE_MAILBOX, "UNSUBSCRIBE %M", mailbox);

	success = camel_imapx_server_process_command_sync (is, ic, _("Error unsubscribing from folder"), cancellable, error);

	camel_imapx_command_unref (ic);

	if (success) {
		CamelIMAPXStore *imapx_store;

		/* Perform the same processing as imapx_untagged_list()
		 * would if the server notified us of an unsubscription. */

		imapx_store = camel_imapx_server_ref_store (is);

		camel_imapx_mailbox_unsubscribed (mailbox);
		camel_imapx_store_emit_mailbox_updated (imapx_store, mailbox);

		g_clear_object (&imapx_store);
	}

	return success;
}