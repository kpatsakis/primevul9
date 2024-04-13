imapx_untagged_search (CamelIMAPXServer *is,
                       GInputStream *input_stream,
                       GCancellable *cancellable,
                       GError **error)
{
	GArray *search_results;
	gint tok;
	guint len;
	guchar *token;
	guint64 number;
	gboolean success = FALSE;

	search_results = g_array_new (FALSE, FALSE, sizeof (guint64));

	while (TRUE) {
		gboolean success;

		/* Peek at the next token, and break
		 * out of the loop if we get a newline. */
		tok = camel_imapx_input_stream_token (
			CAMEL_IMAPX_INPUT_STREAM (input_stream),
			&token, &len, cancellable, error);
		if (tok == '\n')
			break;
		if (tok == IMAPX_TOK_ERROR)
			goto exit;
		camel_imapx_input_stream_ungettoken (
			CAMEL_IMAPX_INPUT_STREAM (input_stream),
			tok, token, len);

		success = camel_imapx_input_stream_number (
			CAMEL_IMAPX_INPUT_STREAM (input_stream),
			&number, cancellable, error);

		if (!success)
			goto exit;

		g_array_append_val (search_results, number);
	}

	g_mutex_lock (&is->priv->search_results_lock);

	if (is->priv->search_results == NULL)
		is->priv->search_results = g_array_ref (search_results);
	else
		g_warning ("%s: Conflicting search results", G_STRFUNC);

	g_mutex_unlock (&is->priv->search_results_lock);

	success = TRUE;

exit:
	g_array_unref (search_results);

	return success;
}