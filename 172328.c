camel_imapx_server_ref_input_stream (CamelIMAPXServer *is)
{
	GInputStream *input_stream = NULL;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);

	g_mutex_lock (&is->priv->stream_lock);

	if (is->priv->input_stream != NULL)
		input_stream = g_object_ref (is->priv->input_stream);

	g_mutex_unlock (&is->priv->stream_lock);

	return input_stream;
}