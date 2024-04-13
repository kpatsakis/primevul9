camel_imapx_server_ref_store (CamelIMAPXServer *server)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (server), NULL);

	return g_weak_ref_get (&server->priv->store);
}