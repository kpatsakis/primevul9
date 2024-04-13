soup_filter_input_stream_read_nonblocking (GPollableInputStream  *stream,
					   void                  *buffer,
					   gsize                  count,
					   GError               **error)
{
	SoupFilterInputStream *fstream = SOUP_FILTER_INPUT_STREAM (stream);

	if (!fstream->priv->in_read_until)
		fstream->priv->need_more = FALSE;

	if (fstream->priv->buf && !fstream->priv->in_read_until) {
		return read_from_buf (fstream, buffer, count);
	} else {
		return g_pollable_stream_read (G_FILTER_INPUT_STREAM (fstream)->base_stream,
					       buffer, count,
					       FALSE, NULL, error);
	}
}