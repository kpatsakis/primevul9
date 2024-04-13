camel_imapx_server_is_connected (CamelIMAPXServer *imapx_server)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (imapx_server), FALSE);

	return imapx_server->priv->state >= IMAPX_CONNECTED;
}