imapx_server_set_store (CamelIMAPXServer *server,
                        CamelIMAPXStore *store)
{
	g_return_if_fail (CAMEL_IS_IMAPX_STORE (store));

	g_weak_ref_set (&server->priv->store, store);
}