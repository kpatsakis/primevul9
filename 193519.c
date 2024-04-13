soup_filter_input_stream_is_readable (GPollableInputStream *stream)
{
	SoupFilterInputStream *fstream = SOUP_FILTER_INPUT_STREAM (stream);

	if (fstream->priv->buf && !fstream->priv->need_more)
		return TRUE;
	else
		return g_pollable_input_stream_is_readable (G_POLLABLE_INPUT_STREAM (G_FILTER_INPUT_STREAM (fstream)->base_stream));
}