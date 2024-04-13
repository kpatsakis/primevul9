camel_imapx_server_new (CamelIMAPXStore *store)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_STORE (store), NULL);

	return g_object_new (
		CAMEL_TYPE_IMAPX_SERVER,
		"store", store, NULL);
}