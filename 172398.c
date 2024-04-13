camel_imapx_server_get_utf8_accept (CamelIMAPXServer *is)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	return is->priv->utf8_accept;
}