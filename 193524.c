soup_filter_input_stream_create_source (GPollableInputStream *stream,
					GCancellable         *cancellable)
{
	SoupFilterInputStream *fstream = SOUP_FILTER_INPUT_STREAM (stream);
	GSource *base_source, *pollable_source;

	if (fstream->priv->buf && !fstream->priv->need_more)
		base_source = g_timeout_source_new (0);
	else
		base_source = g_pollable_input_stream_create_source (G_POLLABLE_INPUT_STREAM (G_FILTER_INPUT_STREAM (fstream)->base_stream), cancellable);

	g_source_set_dummy_callback (base_source);
	pollable_source = g_pollable_source_new (G_OBJECT (stream));
	g_source_add_child_source (pollable_source, base_source);
	g_source_unref (base_source);

	return pollable_source;
}