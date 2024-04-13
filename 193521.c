soup_filter_input_stream_class_init (SoupFilterInputStreamClass *stream_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (stream_class);
	GInputStreamClass *input_stream_class = G_INPUT_STREAM_CLASS (stream_class);

	object_class->finalize = soup_filter_input_stream_finalize;

	input_stream_class->read_fn = soup_filter_input_stream_read_fn;
}