imapx_untagged_quotaroot (CamelIMAPXServer *is,
                          GInputStream *input_stream,
                          GCancellable *cancellable,
                          GError **error)
{
	CamelIMAPXStore *imapx_store;
	CamelIMAPXMailbox *mailbox;
	gchar *mailbox_name = NULL;
	gchar **quota_roots = NULL;
	gboolean success;

	success = camel_imapx_parse_quotaroot (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		cancellable, &mailbox_name, &quota_roots, error);

	/* Sanity check */
	g_return_val_if_fail (
		(success && (mailbox_name != NULL)) ||
		(!success && (mailbox_name == NULL)), FALSE);

	if (!success)
		return FALSE;

	imapx_store = camel_imapx_server_ref_store (is);
	mailbox = camel_imapx_store_ref_mailbox (imapx_store, mailbox_name);
	g_clear_object (&imapx_store);

	if (mailbox != NULL) {
		camel_imapx_mailbox_set_quota_roots (
			mailbox, (const gchar **) quota_roots);
		g_object_unref (mailbox);
	} else {
		g_warning (
			"%s: Unknown mailbox '%s'",
			G_STRFUNC, mailbox_name);
	}

	g_free (mailbox_name);
	g_strfreev (quota_roots);

	return TRUE;
}