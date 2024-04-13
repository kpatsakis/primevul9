camel_imapx_server_register_untagged_handler (CamelIMAPXServer *is,
                                              const gchar *untagged_response,
                                              const CamelIMAPXUntaggedRespHandlerDesc *desc)
{
	const CamelIMAPXUntaggedRespHandlerDesc *previous = NULL;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);
	g_return_val_if_fail (untagged_response != NULL, NULL);
	/* desc may be NULL */

	previous = replace_untagged_descriptor (
		is->priv->untagged_handlers,
		untagged_response, desc);

	return previous;
}