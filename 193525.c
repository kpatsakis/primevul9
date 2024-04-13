soup_filter_input_stream_finalize (GObject *object)
{
	SoupFilterInputStream *fstream = SOUP_FILTER_INPUT_STREAM (object);

	g_clear_pointer (&fstream->priv->buf, g_byte_array_unref);

	G_OBJECT_CLASS (soup_filter_input_stream_parent_class)->finalize (object);
}