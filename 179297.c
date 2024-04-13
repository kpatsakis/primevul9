imapx_server_write_file_with_progress (GOutputStream *output_stream,
				       GInputStream *input_stream,
				       goffset file_size,
				       GCancellable *cancellable,
				       GError **error)
{
	gssize n_read;
	gsize bytes_copied, n_written;
	gchar buffer[8192];
	goffset file_offset;
	gboolean res;

	g_return_val_if_fail (G_IS_OUTPUT_STREAM (output_stream), -1);
	g_return_val_if_fail (G_IS_INPUT_STREAM (input_stream), -1);

	if (g_cancellable_set_error_if_cancelled (cancellable, error))
		return FALSE;

	file_offset = 0;
	bytes_copied = 0;
	res = TRUE;
	do {
		n_read = g_input_stream_read (input_stream, buffer, sizeof (buffer), cancellable, error);
		if (n_read == -1) {
			res = FALSE;
			break;
		}

		if (n_read == 0)
			break;

		if (!g_output_stream_write_all (output_stream, buffer, n_read, &n_written, cancellable, error) || n_written == -1) {
			res = FALSE;
			break;
		}

		file_offset += n_read;

		if (file_size > 0) {
			gdouble divd = (gdouble) file_offset / (gdouble) file_size;
			camel_operation_progress (cancellable, (gint) (100 * divd));
		}

		bytes_copied += n_written;
		if (bytes_copied > G_MAXSSIZE)
			bytes_copied = G_MAXSSIZE;
	} while (res);

	if (res)
		return bytes_copied;

	return -1;
}