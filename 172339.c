imapx_continuation (CamelIMAPXServer *is,
                    GInputStream *input_stream,
                    GOutputStream *output_stream,
                    gboolean litplus,
                    GCancellable *cancellable,
                    GError **error)
{
	CamelIMAPXCommand *ic, *newic = NULL;
	CamelIMAPXCommandPart *cp;
	GList *link;
	gssize n_bytes_written;
	gboolean success;

	/* The 'literal' pointer is like a write-lock, nothing else
	 * can write while we have it ... so we dont need any
	 * ohter lock here.  All other writes go through
	 * queue-lock */
	if (camel_imapx_server_is_in_idle (is)) {
		success = camel_imapx_input_stream_skip (
			CAMEL_IMAPX_INPUT_STREAM (input_stream),
			cancellable, error);

		if (!success)
			return FALSE;

		c (is->priv->tagprefix, "Got continuation response for IDLE \n");

		g_mutex_lock (&is->priv->idle_lock);
		is->priv->idle_state = IMAPX_IDLE_STATE_RUNNING;
		g_cond_broadcast (&is->priv->idle_cond);
		g_mutex_unlock (&is->priv->idle_lock);

		return TRUE;
	}

	ic = is->priv->continuation_command;
	if (!litplus) {
		if (ic == NULL) {
			c (is->priv->tagprefix, "got continuation response with no outstanding continuation requests?\n");
			return camel_imapx_input_stream_skip (
				CAMEL_IMAPX_INPUT_STREAM (input_stream),
				cancellable, error);
		}
		c (is->priv->tagprefix, "got continuation response for data\n");
	} else {
		c (is->priv->tagprefix, "sending LITERAL+ continuation\n");
		g_return_val_if_fail (ic != NULL, FALSE);
	}

	/* coverity[deadcode] */
	link = ic ? ic->current_part : NULL;
	if (!link) {
		g_warn_if_fail (link != NULL);
		return FALSE;
	}

	cp = (CamelIMAPXCommandPart *) link->data;

	switch (cp->type & CAMEL_IMAPX_COMMAND_MASK) {
	case CAMEL_IMAPX_COMMAND_DATAWRAPPER:
		c (is->priv->tagprefix, "writing data wrapper to literal\n");
		n_bytes_written =
			camel_data_wrapper_write_to_output_stream_sync (
				CAMEL_DATA_WRAPPER (cp->ob),
				output_stream, cancellable, error);
		if (n_bytes_written < 0)
			return FALSE;
		break;
	case CAMEL_IMAPX_COMMAND_AUTH: {
		gchar *resp;
		guchar *token;

		success = camel_imapx_input_stream_text (
			CAMEL_IMAPX_INPUT_STREAM (input_stream),
			&token, cancellable, error);

		if (!success)
			return FALSE;

		resp = camel_sasl_challenge_base64_sync (
			(CamelSasl *) cp->ob, (const gchar *) token,
			cancellable, error);
		g_free (token);
		if (resp == NULL)
			return FALSE;
		c (is->priv->tagprefix, "got auth continuation, feeding token '%s' back to auth mech\n", resp);

		g_mutex_lock (&is->priv->stream_lock);
		n_bytes_written = g_output_stream_write_all (
			output_stream, resp, strlen (resp),
			NULL, cancellable, error);
		g_mutex_unlock (&is->priv->stream_lock);
		g_free (resp);

		if (n_bytes_written < 0)
			return FALSE;

		/* we want to keep getting called until we get a status reponse from the server
		 * ignore what sasl tells us */
		newic = ic;
		/* We already ate the end of the input stream line */
		goto noskip;
		break; }
	case CAMEL_IMAPX_COMMAND_FILE: {
		GFile *file;
		GFileInfo *file_info;
		GFileInputStream *file_input_stream;
		goffset file_size = 0;

		c (is->priv->tagprefix, "writing file '%s' to literal\n", (gchar *) cp->ob);

		file = g_file_new_for_path (cp->ob);
		file_input_stream = g_file_read (file, cancellable, error);
		g_object_unref (file);

		if (file_input_stream == NULL)
			return FALSE;

		file_info = g_file_input_stream_query_info (file_input_stream,
			G_FILE_ATTRIBUTE_STANDARD_SIZE, cancellable, NULL);
		if (file_info) {
			file_size = g_file_info_get_size (file_info);
			g_object_unref (file_info);
		}

		g_mutex_lock (&is->priv->stream_lock);

		n_bytes_written = imapx_splice_with_progress (
			output_stream, G_INPUT_STREAM (file_input_stream),
			file_size, cancellable, error);

		g_mutex_unlock (&is->priv->stream_lock);

		g_input_stream_close (G_INPUT_STREAM (file_input_stream), cancellable, NULL);
		g_object_unref (file_input_stream);

		if (n_bytes_written < 0)
			return FALSE;

		break; }
	case CAMEL_IMAPX_COMMAND_STRING:
		g_mutex_lock (&is->priv->stream_lock);
		n_bytes_written = g_output_stream_write_all (
			output_stream, cp->ob, cp->ob_size,
			NULL, cancellable, error);
		g_mutex_unlock (&is->priv->stream_lock);
		if (n_bytes_written < 0)
			return FALSE;
		break;
	default:
		/* should we just ignore? */
		g_set_error (
			error, CAMEL_IMAPX_ERROR, CAMEL_IMAPX_ERROR_SERVER_RESPONSE_MALFORMED,
			"continuation response for non-continuation request");
		return FALSE;
	}

	if (ic->job_kind == CAMEL_IMAPX_JOB_APPEND_MESSAGE && !cp->ends_with_crlf) {
		g_mutex_lock (&is->priv->stream_lock);
		n_bytes_written = g_output_stream_write_all (
			output_stream, "\r\n", 2, NULL, cancellable, error);
		g_mutex_unlock (&is->priv->stream_lock);
		if (n_bytes_written < 0)
			return FALSE;
	}

	if (!litplus) {
		success = camel_imapx_input_stream_skip (
			CAMEL_IMAPX_INPUT_STREAM (input_stream),
			cancellable, error);

		if (!success)
			return FALSE;
	}

noskip:
	link = g_list_next (link);
	if (link != NULL) {
		ic->current_part = link;
		cp = (CamelIMAPXCommandPart *) link->data;

		c (is->priv->tagprefix, "next part of command \"%c%05u: %s\"\n", is->priv->tagprefix, ic->tag, cp->data);

		g_mutex_lock (&is->priv->stream_lock);
		n_bytes_written = g_output_stream_write_all (
			output_stream, cp->data, strlen (cp->data),
			NULL, cancellable, error);
		g_mutex_unlock (&is->priv->stream_lock);
		if (n_bytes_written < 0)
			return FALSE;

		if (cp->type & (CAMEL_IMAPX_COMMAND_CONTINUATION | CAMEL_IMAPX_COMMAND_LITERAL_PLUS)) {
			newic = ic;
		} else {
			g_warn_if_fail (g_list_next (link) == NULL);
		}
	} else {
		c (is->priv->tagprefix, "%p: queueing continuation\n", ic);
	}

	g_mutex_lock (&is->priv->stream_lock);
	n_bytes_written = g_output_stream_write_all (
		output_stream, "\r\n", 2, NULL, cancellable, error);
	g_mutex_unlock (&is->priv->stream_lock);
	if (n_bytes_written < 0)
		return FALSE;

	is->priv->continuation_command = newic;

	return TRUE;
}