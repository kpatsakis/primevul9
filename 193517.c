soup_filter_input_stream_read_line (SoupFilterInputStream  *fstream,
				    void                   *buffer,
				    gsize                   length,
				    gboolean                blocking,
				    gboolean               *got_line,
				    GCancellable           *cancellable,
				    GError                **error)
{
	return soup_filter_input_stream_read_until (fstream, buffer, length,
						    "\n", 1, blocking,
						    TRUE, got_line,
						    cancellable, error);
}