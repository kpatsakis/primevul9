camel_imapx_server_process_command_sync (CamelIMAPXServer *is,
					 CamelIMAPXCommand *ic,
					 const gchar *error_prefix,
					 GCancellable *cancellable,
					 GError **error)
{
	CamelIMAPXCommandPart *cp;
	GInputStream *input_stream = NULL;
	GOutputStream *output_stream = NULL;
	gboolean cp_literal_plus;
	GList *head;
	gchar *string;
	gboolean success = FALSE;
	GError *local_error = NULL;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_COMMAND (ic), FALSE);

	camel_imapx_command_close (ic);
	if (ic->status) {
		imapx_free_status (ic->status);
		ic->status = NULL;
	}
	ic->completed = FALSE;

	head = g_queue_peek_head_link (&ic->parts);
	g_return_val_if_fail (head != NULL, FALSE);
	cp = (CamelIMAPXCommandPart *) head->data;
	ic->current_part = head;

	if (g_cancellable_set_error_if_cancelled (cancellable, &local_error)) {
		if (error_prefix && local_error)
			g_prefix_error (&local_error, "%s: ", error_prefix);

		if (local_error)
			g_propagate_error (error, local_error);

		return FALSE;
	}

	cp_literal_plus = ((cp->type & CAMEL_IMAPX_COMMAND_LITERAL_PLUS) != 0);

	COMMAND_LOCK (is);

	if (is->priv->current_command != NULL) {
		g_warning ("%s: [%c] %p: Starting command %p (%s) while still processing %p (%s)", G_STRFUNC,
			is->priv->tagprefix, is, ic, camel_imapx_job_get_kind_name (ic->job_kind),
			is->priv->current_command, camel_imapx_job_get_kind_name (is->priv->current_command->job_kind));
	}

	if (g_cancellable_set_error_if_cancelled (cancellable, &local_error)) {
		c (is->priv->tagprefix, "%s: command %p (%s) cancelled\n", G_STRFUNC, ic, camel_imapx_job_get_kind_name (ic->job_kind));

		COMMAND_UNLOCK (is);

		if (error_prefix && local_error)
			g_prefix_error (&local_error, "%s: ", error_prefix);

		if (local_error)
			g_propagate_error (error, local_error);

		return FALSE;
	}

	c (is->priv->tagprefix, "%s: %p (%s) ~> %p (%s)\n", G_STRFUNC, is->priv->current_command,
		is->priv->current_command ? camel_imapx_job_get_kind_name (is->priv->current_command->job_kind) : "",
		ic, camel_imapx_job_get_kind_name (ic->job_kind));

	is->priv->current_command = ic;
	is->priv->continuation_command = ic;

	COMMAND_UNLOCK (is);

	input_stream = camel_imapx_server_ref_input_stream (is);
	output_stream = camel_imapx_server_ref_output_stream (is);

	if (output_stream == NULL) {
		local_error = g_error_new_literal (
			CAMEL_IMAPX_SERVER_ERROR, CAMEL_IMAPX_SERVER_ERROR_TRY_RECONNECT,
			_("Cannot issue command, no stream available"));
		goto exit;
	}

	c (
		is->priv->tagprefix,
		"Starting command (%s) %c%05u %s\r\n",
		is->priv->current_command ? " literal" : "",
		is->priv->tagprefix,
		ic->tag,
		cp->data && g_str_has_prefix (cp->data, "LOGIN") ?
			"LOGIN..." : cp->data);

	if (ic->job_kind == CAMEL_IMAPX_JOB_DONE)
		string = g_strdup_printf ("%s\r\n", cp->data);
	else
		string = g_strdup_printf ("%c%05u %s\r\n", is->priv->tagprefix, ic->tag, cp->data);
	g_mutex_lock (&is->priv->stream_lock);
	success = g_output_stream_write_all (
		output_stream, string, strlen (string),
		NULL, cancellable, &local_error);
	g_mutex_unlock (&is->priv->stream_lock);
	g_free (string);

	if (local_error != NULL || !success)
		goto exit;

	while (is->priv->continuation_command == ic && cp_literal_plus) {
		/* Sent LITERAL+ continuation immediately */
		imapx_continuation (
			is, input_stream, output_stream,
			TRUE, cancellable, &local_error);
		if (local_error != NULL)
			goto exit;
	}

	while (success && !ic->completed)
		success = imapx_step (is, input_stream, output_stream, cancellable, &local_error);

	imapx_server_reset_inactivity_timer (is);

 exit:

	COMMAND_LOCK (is);

	if (is->priv->current_command == ic) {
		c (is->priv->tagprefix, "%s: %p ~> %p; success:%d local-error:%s result:%s status-text:'%s'\n", G_STRFUNC,
			is->priv->current_command, NULL, success, local_error ? local_error->message : "[null]",
			ic->status ? (
				ic->status->result == IMAPX_OK ? "OK" :
				ic->status->result == IMAPX_NO ? "NO" :
				ic->status->result == IMAPX_BAD ? "BAD" :
				ic->status->result == IMAPX_PREAUTH ? "PREAUTH" :
				ic->status->result == IMAPX_BYE ? "BYE" : "???") : "[null]",
			ic->status ? ic->status->text : "[null]");

		is->priv->current_command = NULL;
		is->priv->continuation_command = NULL;
	} else {
		c (is->priv->tagprefix, "%s: current command:%p doesn't match passed-in command:%p success:%d local-error:%s result:%s status-text:'%s'\n", G_STRFUNC,
			is->priv->current_command, ic, success, local_error ? local_error->message : "[null]",
			ic->status ? (
				ic->status->result == IMAPX_OK ? "OK" :
				ic->status->result == IMAPX_NO ? "NO" :
				ic->status->result == IMAPX_BAD ? "BAD" :
				ic->status->result == IMAPX_PREAUTH ? "PREAUTH" :
				ic->status->result == IMAPX_BYE ? "BYE" : "???") : "[null]",
			ic->status ? ic->status->text : "[null]");
	}

	COMMAND_UNLOCK (is);

	/* Server reported error. */
	if (success && ic->status && ic->status->result != IMAPX_OK) {
		g_set_error (
			&local_error, CAMEL_ERROR,
			CAMEL_ERROR_GENERIC,
			"%s", ic->status->text);
	}

	if (local_error) {
		/* Sadly, G_IO_ERROR_FAILED is also used for 'Connection reset by peer' error;
		   since GLib 2.44 is used G_IO_ERROR_CONNECTION_CLOSED, which is the same as G_IO_ERROR_BROKEN_PIPE */
		if (g_error_matches (local_error, G_IO_ERROR, G_IO_ERROR_FAILED) ||
		    g_error_matches (local_error, G_IO_ERROR, G_IO_ERROR_BROKEN_PIPE) ||
		    g_error_matches (local_error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT)) {
			local_error->domain = CAMEL_IMAPX_SERVER_ERROR;
			local_error->code = CAMEL_IMAPX_SERVER_ERROR_TRY_RECONNECT;
		}

		if (error_prefix && local_error)
			g_prefix_error (&local_error, "%s: ", error_prefix);

		g_propagate_error (error, local_error);

		success = FALSE;
	}

	g_clear_object (&input_stream);
	g_clear_object (&output_stream);

	return success;
}