imapx_untagged_exists (CamelIMAPXServer *is,
                       GInputStream *input_stream,
                       GCancellable *cancellable,
                       GError **error)
{
	CamelFolder *folder;
	CamelIMAPXMailbox *mailbox;
	guint32 exists;
	gboolean success = TRUE;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	mailbox = camel_imapx_server_ref_pending_or_selected (is);

	if (mailbox == NULL) {
		g_warning ("%s: No mailbox available", G_STRFUNC);
		return TRUE;
	}

	exists = (guint32) is->priv->context->id;

	c (is->priv->tagprefix, "%s: updating mailbox '%s' messages: %d ~> %d\n", G_STRFUNC,
		camel_imapx_mailbox_get_name (mailbox),
		camel_imapx_mailbox_get_messages (mailbox),
		exists);

	camel_imapx_mailbox_set_messages (mailbox, exists);

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_val_if_fail (folder != NULL, FALSE);

	if (camel_imapx_server_is_in_idle (is)) {
		guint count;

		count = camel_folder_summary_count (folder->summary);
		if (count < exists)
			g_signal_emit (is, signals[REFRESH_MAILBOX], 0, mailbox);
	}

	g_object_unref (folder);
	g_object_unref (mailbox);

	return success;
}