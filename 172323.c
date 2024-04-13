camel_imapx_server_noop_sync (CamelIMAPXServer *is,
			      CamelIMAPXMailbox *mailbox,
			      GCancellable *cancellable,
			      GError **error)
{
	gboolean success = TRUE;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	/* Mailbox may be NULL. */

	if (mailbox)
		success = camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error);

	if (success) {
		CamelIMAPXCommand *ic;

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_NOOP, "NOOP");

		success = camel_imapx_server_process_command_sync (is, ic, _("Error performing NOOP"), cancellable, error);

		camel_imapx_command_unref (ic);
	}

	return success;
}