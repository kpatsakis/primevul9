camel_imapx_server_lack_capability (CamelIMAPXServer *is,
				    guint32 capability)
{
	gboolean lack;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	g_mutex_lock (&is->priv->stream_lock);
	lack = is->priv->cinfo != NULL && (is->priv->cinfo->capa & capability) == 0;
	g_mutex_unlock (&is->priv->stream_lock);

	return lack;
}