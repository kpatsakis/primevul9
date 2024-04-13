imapx_untagged_expunge (CamelIMAPXServer *is,
                        GInputStream *input_stream,
                        GCancellable *cancellable,
                        GError **error)
{
	CamelIMAPXMailbox *mailbox;
	gulong expunge = 0;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	expunge = is->priv->context->id;

	COMMAND_LOCK (is);

	/* Ignore EXPUNGE responses when not running a COPY(MOVE)_MESSAGE job */
	if (!is->priv->current_command || (is->priv->current_command->job_kind != CAMEL_IMAPX_JOB_COPY_MESSAGE &&
	    is->priv->current_command->job_kind != CAMEL_IMAPX_JOB_MOVE_MESSAGE)) {
		COMMAND_UNLOCK (is);

		c (is->priv->tagprefix, "ignoring untagged expunge: %lu\n", expunge);
		return TRUE;
	}

	COMMAND_UNLOCK (is);

	c (is->priv->tagprefix, "expunged: %lu\n", expunge);

	mailbox = camel_imapx_server_ref_pending_or_selected (is);

	if (mailbox != NULL) {
		CamelFolder *folder;
		gchar *uid;

		folder = imapx_server_ref_folder (is, mailbox);
		g_return_val_if_fail (folder != NULL, FALSE);

		uid = camel_imapx_dup_uid_from_summary_index (folder, expunge - 1);

		if (uid != NULL)
			imapx_expunge_uid_from_summary (is, uid, TRUE);

		g_object_unref (folder);
		g_free (uid);
	}

	g_clear_object (&mailbox);

	return TRUE;
}