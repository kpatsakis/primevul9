camel_imapx_server_create_mailbox_sync (CamelIMAPXServer *is,
					const gchar *mailbox_name,
					GCancellable *cancellable,
					GError **error)
{
	CamelIMAPXCommand *ic;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (mailbox_name != NULL, FALSE);

	ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_CREATE_MAILBOX, "CREATE %m", mailbox_name);

	success = camel_imapx_server_process_command_sync (is, ic, _("Error creating folder"), cancellable, error);

	camel_imapx_command_unref (ic);

	if (success) {
		/* List the new mailbox so we trigger our untagged
		 * LIST handler.  This simulates being notified of
		 * a newly-created mailbox, so we can just let the
		 * callback functions handle the bookkeeping. */
		success = camel_imapx_server_list_sync (is, mailbox_name, 0, cancellable, error);
	}

	return success;
}