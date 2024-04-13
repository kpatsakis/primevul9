imapx_step (CamelIMAPXServer *is,
            GInputStream *input_stream,
	    GOutputStream *output_stream,
            GCancellable *cancellable,
            GError **error)
{
	guint len;
	guchar *token;
	gint tok;
	gboolean success = FALSE;

	tok = camel_imapx_input_stream_token (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		&token, &len, cancellable, error);

	switch (tok) {
		case IMAPX_TOK_ERROR:
			/* GError is already set. */
			break;
		case '*':
			success = imapx_untagged (
				is, input_stream, cancellable, error);
			break;
		case IMAPX_TOK_TOKEN:
			success = imapx_completion (
				is, input_stream,
				token, len, cancellable, error);
			break;
		case '+':
			success = imapx_continuation (
				is, input_stream, output_stream,
				FALSE, cancellable, error);
			break;
		default:
			g_set_error (
				error, CAMEL_IMAPX_ERROR, CAMEL_IMAPX_ERROR_SERVER_RESPONSE_MALFORMED,
				"unexpected server response:");
			break;
	}

	return success;
}