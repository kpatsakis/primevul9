camel_imapx_server_ref_output_stream (CamelIMAPXServer *is)
{
	GOutputStream *output_stream = NULL;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);

	g_mutex_lock (&is->priv->stream_lock);

	if (is->priv->output_stream != NULL)
		output_stream = g_object_ref (is->priv->output_stream);

	g_mutex_unlock (&is->priv->stream_lock);

	return output_stream;
}