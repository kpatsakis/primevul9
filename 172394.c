camel_imapx_server_schedule_idle_sync (CamelIMAPXServer *is,
				       CamelIMAPXMailbox *mailbox,
				       GCancellable *cancellable,
				       GError **error)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	if (mailbox)
		g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	if (!camel_imapx_server_stop_idle_sync (is, cancellable, error))
		return FALSE;

	if (!camel_imapx_server_can_use_idle (is))
		return TRUE;

	g_mutex_lock (&is->priv->idle_lock);

	if (is->priv->idle_state != IMAPX_IDLE_STATE_OFF) {
		g_warn_if_fail (is->priv->idle_state == IMAPX_IDLE_STATE_OFF);

		g_mutex_unlock (&is->priv->idle_lock);

		return FALSE;
	}

	g_warn_if_fail (is->priv->idle_cancellable == NULL);

	is->priv->idle_cancellable = g_cancellable_new ();
	is->priv->idle_stamp++;

	if (is->priv->idle_pending) {
		g_source_destroy (is->priv->idle_pending);
		g_source_unref (is->priv->idle_pending);
	}

	g_clear_object (&is->priv->idle_mailbox);
	if (mailbox)
		is->priv->idle_mailbox = g_object_ref (mailbox);

	is->priv->idle_state = IMAPX_IDLE_STATE_SCHEDULED;
	is->priv->idle_pending = g_timeout_source_new_seconds (IMAPX_IDLE_WAIT_SECONDS);
	g_source_set_callback (
		is->priv->idle_pending, imapx_server_run_idle_thread_cb,
		imapx_weak_ref_new (is), (GDestroyNotify) imapx_weak_ref_free);
	g_source_attach (is->priv->idle_pending, NULL);

	g_mutex_unlock (&is->priv->idle_lock);

	return TRUE;
}