camel_imapx_server_get_capability_info (CamelIMAPXServer *is)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);

	return is->priv->cinfo;
}