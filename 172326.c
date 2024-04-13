camel_imapx_server_query_auth_types_sync (CamelIMAPXServer *is,
					  GCancellable *cancellable,
					  GError **error)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	return imapx_connect_to_server (is, cancellable, error);
}