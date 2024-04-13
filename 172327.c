imapx_server_reset_inactivity_timer (CamelIMAPXServer *is)
{
	g_mutex_lock (&is->priv->inactivity_timeout_lock);

	if (is->priv->inactivity_timeout != NULL) {
		g_source_destroy (is->priv->inactivity_timeout);
		g_source_unref (is->priv->inactivity_timeout);
	}

	is->priv->inactivity_timeout =
		g_timeout_source_new_seconds (INACTIVITY_TIMEOUT_SECONDS);
	g_source_set_callback (
		is->priv->inactivity_timeout,
		imapx_server_inactivity_timeout_cb,
		imapx_weak_ref_new (is),
		(GDestroyNotify) imapx_weak_ref_free);
	g_source_attach (is->priv->inactivity_timeout, NULL);

	g_mutex_unlock (&is->priv->inactivity_timeout_lock);
}