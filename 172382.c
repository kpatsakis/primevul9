camel_imapx_server_get_tagprefix (CamelIMAPXServer *is)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), 0);

	return is->priv->tagprefix;
}