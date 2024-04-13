imapx_untagged (CamelIMAPXServer *is,
                GInputStream *input_stream,
                GCancellable *cancellable,
                GError **error)
{
	CamelIMAPXSettings *settings;
	CamelSortType fetch_order;
	guchar *p = NULL, c;
	const gchar *token = NULL;
	gboolean success = FALSE;

	/* If is->priv->context is not NULL here, it basically means
	 * that imapx_untagged() got called concurrently for the same
	 * CamelIMAPXServer instance. Should this ever happen, then
	 * we will need to protect this data structure with locks
	 */
	g_return_val_if_fail (is->priv->context == NULL, FALSE);
	is->priv->context = g_slice_new0 (CamelIMAPXServerUntaggedContext);

	settings = camel_imapx_server_ref_settings (is);
	fetch_order = camel_imapx_settings_get_fetch_order (settings);
	g_object_unref (settings);

	is->priv->context->lsub = FALSE;
	is->priv->context->fetch_order = fetch_order;

	e (is->priv->tagprefix, "got untagged response\n");
	is->priv->context->id = 0;
	is->priv->context->tok = camel_imapx_input_stream_token (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		&(is->priv->context->token),
		&(is->priv->context->len),
		cancellable, error);
	if (is->priv->context->tok < 0)
		goto exit;

	if (is->priv->context->tok == IMAPX_TOK_INT) {
		is->priv->context->id = strtoul (
			(gchar *) is->priv->context->token, NULL, 10);
		is->priv->context->tok = camel_imapx_input_stream_token (
			CAMEL_IMAPX_INPUT_STREAM (input_stream),
			&(is->priv->context->token),
			&(is->priv->context->len),
			cancellable, error);
		if (is->priv->context->tok < 0)
			goto exit;
	}

	if (is->priv->context->tok == '\n') {
		g_set_error (
			error, CAMEL_IMAPX_ERROR, CAMEL_IMAPX_ERROR_SERVER_RESPONSE_MALFORMED,
			"truncated server response");
		goto exit;
	}

	e (is->priv->tagprefix, "Have token '%s' id %lu\n", is->priv->context->token, is->priv->context->id);
	p = is->priv->context->token;
	while (p && *p) {
		c = *p;
		*p++ = g_ascii_toupper ((gchar) c);
	}

	token = (const gchar *) is->priv->context->token; /* FIXME need 'guchar *token' here */
	while (token != NULL) {
		CamelIMAPXUntaggedRespHandlerDesc *desc = NULL;

		desc = g_hash_table_lookup (is->priv->untagged_handlers, token);
		if (desc == NULL) {
			/* unknown response, just ignore it */
			c (is->priv->tagprefix, "unknown token: %s\n", is->priv->context->token);
			break;
		}
		if (desc->handler == NULL) {
			/* no handler function, ignore token */
			c (is->priv->tagprefix, "no handler for token: %s\n", is->priv->context->token);
			break;
		}

		/* call the handler function */
		success = desc->handler (is, input_stream, cancellable, error);
		if (!success)
			goto exit;

		/* is there another handler next-in-line? */
		token = desc->next_response;
		if (token != NULL) {
			/* TODO do we need to update 'priv->context->token'
			 *      to the value of 'token' here, before
			 *      calling the handler next-in-line for this
			 *      specific run of imapx_untagged()?
			 *      It has not been done in the original code
			 *      in the "fall through" situation in the
			 *      token switch statement, which is what
			 *      we're mimicking here
			 */
			continue;
		}

		if (!desc->skip_stream_when_done)
			goto exit;
	}

	success = camel_imapx_input_stream_skip (
		CAMEL_IMAPX_INPUT_STREAM (input_stream), cancellable, error);

exit:
	g_slice_free (CamelIMAPXServerUntaggedContext, is->priv->context);
	is->priv->context = NULL;

	return success;
}