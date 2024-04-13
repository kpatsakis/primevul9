soup_filter_input_stream_new (GInputStream *base_stream)
{
	return g_object_new (SOUP_TYPE_FILTER_INPUT_STREAM,
			     "base-stream", base_stream,
			     "close-base-stream", FALSE,
			     NULL);
}