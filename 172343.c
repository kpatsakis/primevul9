camel_imapx_server_stop_idle_sync (CamelIMAPXServer *is,
				   GCancellable *cancellable,
				   GError **error)
{
	GCancellable *idle_cancellable;
	gulong handler_id = 0;
	gint64 wait_end_time;
	gboolean success = TRUE;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	g_mutex_lock (&is->priv->idle_lock);

	if (is->priv->idle_state == IMAPX_IDLE_STATE_OFF) {
		g_mutex_unlock (&is->priv->idle_lock);
		return TRUE;
	} else if (is->priv->idle_state == IMAPX_IDLE_STATE_SCHEDULED) {
		if (is->priv->idle_pending) {
			g_source_destroy (is->priv->idle_pending);
			g_source_unref (is->priv->idle_pending);
			is->priv->idle_pending = NULL;
		}

		is->priv->idle_state = IMAPX_IDLE_STATE_OFF;
		g_cond_broadcast (&is->priv->idle_cond);
	}

	idle_cancellable = is->priv->idle_cancellable ? g_object_ref (is->priv->idle_cancellable) : NULL;

	g_clear_object (&is->priv->idle_cancellable);
	g_clear_object (&is->priv->idle_mailbox);
	is->priv->idle_stamp++;

	if (cancellable) {
		g_mutex_unlock (&is->priv->idle_lock);

		/* Do not hold the idle_lock here, because the callback can be called
		   immediately, which leads to a deadlock inside it. */
		handler_id = g_cancellable_connect (cancellable, G_CALLBACK (imapx_server_wait_idle_stop_cancelled_cb), is, NULL);

		g_mutex_lock (&is->priv->idle_lock);
	}

	while (is->priv->idle_state == IMAPX_IDLE_STATE_PREPARING &&
	       !g_cancellable_is_cancelled (cancellable)) {
		g_cond_wait (&is->priv->idle_cond, &is->priv->idle_lock);
	}

	if (is->priv->idle_state == IMAPX_IDLE_STATE_RUNNING &&
	    !g_cancellable_is_cancelled (cancellable)) {
		is->priv->idle_state = IMAPX_IDLE_STATE_STOPPING;
		g_cond_broadcast (&is->priv->idle_cond);
		g_mutex_unlock (&is->priv->idle_lock);

		g_mutex_lock (&is->priv->stream_lock);
		if (is->priv->output_stream) {
			gint previous_timeout = -1;

			/* Set the connection timeout to some short time, no need to wait for it for too long */
			if (is->priv->connection)
				previous_timeout = imapx_server_set_connection_timeout (is->priv->connection, 5);

			success = g_output_stream_flush (is->priv->output_stream, cancellable, error);
			success = success && g_output_stream_write_all (is->priv->output_stream, "DONE\r\n", 6, NULL, cancellable, error);
			success = success && g_output_stream_flush (is->priv->output_stream, cancellable, error);

			if (previous_timeout >= 0 && is->priv->connection)
				imapx_server_set_connection_timeout (is->priv->connection, previous_timeout);
		} else {
			success = FALSE;

			/* This message won't get into UI. */
			g_set_error_literal (error, CAMEL_IMAPX_SERVER_ERROR, CAMEL_IMAPX_SERVER_ERROR_TRY_RECONNECT,
				"Reconnect after couldn't issue DONE command");
		}
		g_mutex_unlock (&is->priv->stream_lock);
		g_mutex_lock (&is->priv->idle_lock);
	}

	/* Give server 10 seconds to process the DONE command, if it fails, then give up and reconnect */
	wait_end_time = g_get_monotonic_time () + 10 * G_TIME_SPAN_SECOND;

	while (success && is->priv->idle_state != IMAPX_IDLE_STATE_OFF &&
	       !g_cancellable_is_cancelled (cancellable)) {
		success = g_cond_wait_until (&is->priv->idle_cond, &is->priv->idle_lock, wait_end_time);
	}

	g_mutex_unlock (&is->priv->idle_lock);

	if (cancellable && handler_id)
		g_cancellable_disconnect (cancellable, handler_id);

	if (success && g_cancellable_is_cancelled (cancellable)) {
		g_clear_error (error);

		success = FALSE;

		/* This message won't get into UI. */
		g_set_error_literal (error, CAMEL_IMAPX_SERVER_ERROR, CAMEL_IMAPX_SERVER_ERROR_TRY_RECONNECT,
			"Reconnect after cancelled IDLE stop command");
	}

	if (!success) {
		if (idle_cancellable)
			g_cancellable_cancel (idle_cancellable);

		g_mutex_lock (&is->priv->idle_lock);
		is->priv->idle_state = IMAPX_IDLE_STATE_OFF;
		g_mutex_unlock (&is->priv->idle_lock);

		imapx_disconnect (is);
	}

	g_clear_object (&idle_cancellable);

	return success;
}