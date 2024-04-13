imapx_untagged_bye (CamelIMAPXServer *is,
                    GInputStream *input_stream,
                    GCancellable *cancellable,
                    GError **error)
{
	guchar *token = NULL;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	success = camel_imapx_input_stream_text (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		&token, cancellable, error);

	/* XXX It's weird to be setting an error on success,
	 *     but it's to indicate the server hung up on us. */
	if (success) {
		g_strstrip ((gchar *) token);

		c (is->priv->tagprefix, "BYE: %s\n", token);
		g_set_error (
			error, CAMEL_IMAPX_SERVER_ERROR, CAMEL_IMAPX_SERVER_ERROR_TRY_RECONNECT,
			"IMAP server said BYE: %s", token);
	}

	g_free (token);

	is->priv->state = IMAPX_SHUTDOWN;

	return FALSE;
}