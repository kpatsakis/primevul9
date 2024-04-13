imapx_untagged_expunge (CamelIMAPXServer *is,
                        GInputStream *input_stream,
                        GCancellable *cancellable,
                        GError **error)
{
	gulong expunged_idx;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	expunged_idx = is->priv->context->id;

	COMMAND_LOCK (is);

	/* Ignore EXPUNGE responses when not running a COPY(MOVE)_MESSAGE job */
	if (!is->priv->current_command || (is->priv->current_command->job_kind != CAMEL_IMAPX_JOB_COPY_MESSAGE &&
	    is->priv->current_command->job_kind != CAMEL_IMAPX_JOB_MOVE_MESSAGE)) {
		gboolean ignored = TRUE;
		gboolean is_idle_command = is->priv->current_command && is->priv->current_command->job_kind == CAMEL_IMAPX_JOB_IDLE;

		COMMAND_UNLOCK (is);

		/* Process only untagged EXPUNGE responses within ongoing IDLE command */
		if (is_idle_command) {
			CamelIMAPXMailbox *mailbox;

			mailbox = camel_imapx_server_ref_selected (is);
			if (mailbox) {
				guint32 messages;

				messages = camel_imapx_mailbox_get_messages (mailbox);
				if (messages > 0) {
					camel_imapx_mailbox_set_messages (mailbox, messages - 1);

					ignored = FALSE;
					c (is->priv->tagprefix, "going to refresh mailbox '%s' due to untagged expunge: %lu\n", camel_imapx_mailbox_get_name (mailbox), expunged_idx);

					g_signal_emit (is, signals[REFRESH_MAILBOX], 0, mailbox);
				}

				g_object_unref (mailbox);
			}
		}

		if (ignored)
			c (is->priv->tagprefix, "ignoring untagged expunge: %lu\n", expunged_idx);

		return TRUE;
	}

	c (is->priv->tagprefix, "expunged: %lu\n", expunged_idx);

	is->priv->current_command->copy_move_expunged = g_slist_prepend (
		is->priv->current_command->copy_move_expunged, GUINT_TO_POINTER (expunged_idx));

	COMMAND_UNLOCK (is);

	return TRUE;
}