imapx_untagged_namespace (CamelIMAPXServer *is,
                          GInputStream *input_stream,
                          GCancellable *cancellable,
                          GError **error)
{
	CamelIMAPXNamespaceResponse *response;
	CamelIMAPXStore *imapx_store;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	response = camel_imapx_namespace_response_new (
		CAMEL_IMAPX_INPUT_STREAM (input_stream), cancellable, error);

	if (response == NULL)
		return FALSE;

	imapx_store = camel_imapx_server_ref_store (is);
	camel_imapx_store_set_namespaces (imapx_store, response);

	g_clear_object (&imapx_store);
	g_object_unref (response);

	return TRUE;
}