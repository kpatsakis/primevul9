soup_filter_input_stream_init (SoupFilterInputStream *stream)
{
	stream->priv = soup_filter_input_stream_get_instance_private (stream);
}