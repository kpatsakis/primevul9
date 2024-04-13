imapx_server_idle_thread (gpointer user_data)
{
	IdleThreadData *itd = user_data;
	CamelIMAPXServer *is;
	CamelIMAPXMailbox *mailbox;
	CamelIMAPXCommand *ic;
	CamelIMAPXCommandPart *cp;
	GCancellable *idle_cancellable;
	GError *local_error = NULL;
	gint previous_timeout = -1;
	gboolean success = FALSE;
	gboolean rather_disconnect = FALSE;

	g_return_val_if_fail (itd != NULL, NULL);

	is = itd->is;
	idle_cancellable = itd->idle_cancellable;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);
	g_return_val_if_fail (G_IS_CANCELLABLE (idle_cancellable), NULL);

	g_mutex_lock (&is->priv->idle_lock);

	if (g_cancellable_is_cancelled (idle_cancellable) ||
	    is->priv->idle_stamp != itd->idle_stamp ||
	    is->priv->idle_state != IMAPX_IDLE_STATE_SCHEDULED) {
		g_cond_broadcast (&is->priv->idle_cond);
		g_mutex_unlock (&is->priv->idle_lock);

		idle_thread_data_free (itd);

		return NULL;
	}

	is->priv->idle_state = IMAPX_IDLE_STATE_PREPARING;
	g_cond_broadcast (&is->priv->idle_cond);

	mailbox = is->priv->idle_mailbox;
	if (mailbox)
		g_object_ref (mailbox);

	g_mutex_unlock (&is->priv->idle_lock);

	if (!mailbox)
		mailbox = camel_imapx_server_ref_selected (is);

	if (!mailbox)
		goto exit;

	success = camel_imapx_server_ensure_selected_sync (is, mailbox, idle_cancellable, &local_error);
	if (!success) {
		rather_disconnect = TRUE;
		goto exit;
	}

	ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_IDLE, "IDLE");
	camel_imapx_command_close (ic);

	cp = g_queue_peek_head (&ic->parts);
	cp->type |= CAMEL_IMAPX_COMMAND_CONTINUATION;

	g_mutex_lock (&is->priv->stream_lock);
	/* Set the connection timeout to one minute more than the inactivity timeout */
	if (is->priv->connection)
		previous_timeout = imapx_server_set_connection_timeout (is->priv->connection, INACTIVITY_TIMEOUT_SECONDS + 60);
	g_mutex_unlock (&is->priv->stream_lock);

	g_mutex_lock (&is->priv->idle_lock);
	if (is->priv->idle_stamp == itd->idle_stamp &&
	    is->priv->idle_state == IMAPX_IDLE_STATE_PREPARING) {
		g_mutex_unlock (&is->priv->idle_lock);

		/* Blocks, until the DONE is issued or on inactivity timeout, error, ... */
		success = camel_imapx_server_process_command_sync (is, ic, _("Error running IDLE"), idle_cancellable, &local_error);

		rather_disconnect = rather_disconnect || !success || g_cancellable_is_cancelled (idle_cancellable);
	} else {
		g_mutex_unlock (&is->priv->idle_lock);
	}

	if (previous_timeout >= 0) {
		g_mutex_lock (&is->priv->stream_lock);
		if (is->priv->connection)
			imapx_server_set_connection_timeout (is->priv->connection, previous_timeout);
		g_mutex_unlock (&is->priv->stream_lock);
	}

	camel_imapx_command_unref (ic);

 exit:
	g_mutex_lock (&is->priv->idle_lock);
	g_clear_object (&is->priv->idle_cancellable);
	is->priv->idle_state = IMAPX_IDLE_STATE_OFF;
	g_cond_broadcast (&is->priv->idle_cond);
	g_mutex_unlock (&is->priv->idle_lock);

	if (success)
		c (camel_imapx_server_get_tagprefix (is), "IDLE finished successfully\n");
	else if (local_error)
		c (camel_imapx_server_get_tagprefix (is), "IDLE finished with error: %s%s\n", local_error->message, rather_disconnect ? "; rather disconnect" : "");
	else
		c (camel_imapx_server_get_tagprefix (is), "IDLE finished without error%s\n", rather_disconnect ? "; rather disconnect" : "");

	if (rather_disconnect) {
		imapx_disconnect (is);
	}

	g_clear_object (&mailbox);
	g_clear_error (&local_error);

	idle_thread_data_free (itd);

	return NULL;
}