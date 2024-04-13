camel_imapx_server_ref_settings (CamelIMAPXServer *server)
{
	CamelIMAPXStore *store;
	CamelSettings *settings;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (server), NULL);

	store = camel_imapx_server_ref_store (server);
	settings = camel_service_ref_settings (CAMEL_SERVICE (store));
	g_object_unref (store);

	return CAMEL_IMAPX_SETTINGS (settings);
}