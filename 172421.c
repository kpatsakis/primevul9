camel_imapx_server_have_capability (CamelIMAPXServer *is,
				    guint32 capability)
{
	gboolean have;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	g_mutex_lock (&is->priv->stream_lock);
	have = is->priv->cinfo != NULL && (is->priv->cinfo->capa & capability) != 0;
	g_mutex_unlock (&is->priv->stream_lock);

	return have;
}