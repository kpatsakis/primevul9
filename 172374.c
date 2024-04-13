imapx_server_dispose (GObject *object)
{
	CamelIMAPXServer *server = CAMEL_IMAPX_SERVER (object);

	g_cancellable_cancel (server->priv->cancellable);

	imapx_disconnect (server);

	g_weak_ref_set (&server->priv->store, NULL);

	g_clear_object (&server->priv->subprocess);

	g_mutex_lock (&server->priv->idle_lock);
	g_clear_object (&server->priv->idle_cancellable);
	g_clear_object (&server->priv->idle_mailbox);
	if (server->priv->idle_pending) {
		g_source_destroy (server->priv->idle_pending);
		g_source_unref (server->priv->idle_pending);
		server->priv->idle_pending = NULL;
	}
	g_mutex_unlock (&server->priv->idle_lock);

	g_clear_object (&server->priv->subprocess);

	/* Chain up to parent's dispose() method. */
	G_OBJECT_CLASS (camel_imapx_server_parent_class)->dispose (object);
}