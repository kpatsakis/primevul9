camel_imapx_server_rename_mailbox_sync (CamelIMAPXServer *is,
					CamelIMAPXMailbox *mailbox,
					const gchar *new_mailbox_name,
					GCancellable *cancellable,
					GError **error)
{
	CamelIMAPXCommand *ic;
	CamelIMAPXMailbox *inbox;
	CamelIMAPXStore *imapx_store;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);
	g_return_val_if_fail (new_mailbox_name != NULL, FALSE);

	imapx_store = camel_imapx_server_ref_store (is);
	inbox = camel_imapx_store_ref_mailbox (imapx_store, "INBOX");
	g_return_val_if_fail (inbox != NULL, FALSE);

	/* We don't want to select the mailbox to be renamed. */
	success = camel_imapx_server_ensure_selected_sync (is, inbox, cancellable, error);
	if (!success) {
		g_clear_object (&inbox);
		g_clear_object (&imapx_store);
		return FALSE;
	}

	ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_RENAME_MAILBOX, "RENAME %M %m", mailbox, new_mailbox_name);

	success = camel_imapx_server_process_command_sync (is, ic, _("Error renaming folder"), cancellable, error);

	camel_imapx_command_unref (ic);

	if (success) {
		/* Perform the same processing as imapx_untagged_list()
		 * would if the server notified us of a renamed mailbox. */

		camel_imapx_store_handle_mailbox_rename (imapx_store, mailbox, new_mailbox_name);
	}

	g_clear_object (&inbox);
	g_clear_object (&imapx_store);

	return success;
}