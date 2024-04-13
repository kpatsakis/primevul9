imapx_server_set_streams (CamelIMAPXServer *is,
                          GInputStream *input_stream,
                          GOutputStream *output_stream)
{
	GConverter *logger;

	if (input_stream != NULL) {
		GInputStream *temp_stream;

		/* The logger produces debugging output. */
		logger = camel_imapx_logger_new (is->priv->tagprefix);
		input_stream = g_converter_input_stream_new (
			input_stream, logger);
		g_clear_object (&logger);

		/* Buffer the input stream for parsing. */
		temp_stream = camel_imapx_input_stream_new (input_stream);
		camel_binding_bind_property (
			temp_stream, "close-base-stream",
			input_stream, "close-base-stream",
			G_BINDING_SYNC_CREATE);
		g_object_unref (input_stream);
		input_stream = temp_stream;
	}

	if (output_stream != NULL) {
		/* The logger produces debugging output. */
		logger = camel_imapx_logger_new (is->priv->tagprefix);
		output_stream = g_converter_output_stream_new (
			output_stream, logger);
		g_clear_object (&logger);
	}

	g_mutex_lock (&is->priv->stream_lock);

	/* Don't close the base streams so STARTTLS works correctly. */

	if (G_IS_FILTER_INPUT_STREAM (is->priv->input_stream)) {
		g_filter_input_stream_set_close_base_stream (
			G_FILTER_INPUT_STREAM (is->priv->input_stream),
			FALSE);
	}

	if (G_IS_FILTER_OUTPUT_STREAM (is->priv->output_stream)) {
		g_filter_output_stream_set_close_base_stream (
			G_FILTER_OUTPUT_STREAM (is->priv->output_stream),
			FALSE);
	}

	g_clear_object (&is->priv->input_stream);
	is->priv->input_stream = input_stream;

	g_clear_object (&is->priv->output_stream);
	is->priv->output_stream = output_stream;

	g_mutex_unlock (&is->priv->stream_lock);
}