imapx_untagged_status (CamelIMAPXServer *is,
                       GInputStream *input_stream,
                       GCancellable *cancellable,
                       GError **error)
{
	CamelIMAPXStatusResponse *response;
	CamelIMAPXStore *imapx_store;
	CamelIMAPXMailbox *mailbox;
	const gchar *mailbox_name;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	response = camel_imapx_status_response_new (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		is->priv->inbox_separator, cancellable, error);
	if (response == NULL)
		return FALSE;

	mailbox_name = camel_imapx_status_response_get_mailbox_name (response);

	imapx_store = camel_imapx_server_ref_store (is);
	mailbox = camel_imapx_store_ref_mailbox (imapx_store, mailbox_name);

	if (mailbox != NULL) {
		camel_imapx_mailbox_handle_status_response (mailbox, response);
		camel_imapx_store_emit_mailbox_updated (imapx_store, mailbox);

		if (camel_imapx_server_is_in_idle (is) &&
		    imapx_can_refresh_mailbox_in_idle (is, imapx_store, mailbox))
			g_signal_emit (is, signals[REFRESH_MAILBOX], 0, mailbox);

		g_object_unref (mailbox);
	}

	g_clear_object (&imapx_store);
	g_object_unref (response);

	return TRUE;
}