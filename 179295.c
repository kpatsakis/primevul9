imapx_untagged_lsub (CamelIMAPXServer *is,
                     GInputStream *input_stream,
                     GCancellable *cancellable,
                     GError **error)
{
	CamelIMAPXListResponse *response;
	CamelIMAPXStore *imapx_store;
	const gchar *mailbox_name;
	gchar separator;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	/* LSUB response is syntactically compatible with LIST response. */
	response = camel_imapx_list_response_new (
		CAMEL_IMAPX_INPUT_STREAM (input_stream), cancellable, error);
	if (response == NULL)
		return FALSE;

	camel_imapx_list_response_add_attribute (
		response, CAMEL_IMAPX_LIST_ATTR_SUBSCRIBED);

	mailbox_name = camel_imapx_list_response_get_mailbox_name (response);
	separator = camel_imapx_list_response_get_separator (response);

	/* Record the INBOX separator character once we know it. */
	if (camel_imapx_mailbox_is_inbox (mailbox_name))
		is->priv->inbox_separator = separator;

	imapx_store = camel_imapx_server_ref_store (is);
	camel_imapx_store_handle_lsub_response (imapx_store, is, response);

	g_clear_object (&imapx_store);
	g_clear_object (&response);

	return TRUE;
}