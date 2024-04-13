camel_imapx_server_init (CamelIMAPXServer *is)
{
	is->priv = CAMEL_IMAPX_SERVER_GET_PRIVATE (is);

	is->priv->untagged_handlers = create_initial_untagged_handler_table ();

	g_mutex_init (&is->priv->stream_lock);
	g_mutex_init (&is->priv->inactivity_timeout_lock);
	g_mutex_init (&is->priv->select_lock);
	g_mutex_init (&is->priv->changes_lock);
	g_mutex_init (&is->priv->search_results_lock);
	g_mutex_init (&is->priv->known_alerts_lock);

	g_weak_ref_init (&is->priv->store, NULL);
	g_weak_ref_init (&is->priv->select_mailbox, NULL);
	g_weak_ref_init (&is->priv->select_pending, NULL);

	is->priv->cancellable = g_cancellable_new ();

	is->priv->state = IMAPX_DISCONNECTED;
	is->priv->is_cyrus = FALSE;
	is->priv->copyuid_status = NULL;

	is->priv->changes = camel_folder_change_info_new ();

	is->priv->known_alerts = g_hash_table_new_full (
		(GHashFunc) g_str_hash,
		(GEqualFunc) g_str_equal,
		(GDestroyNotify) g_free,
		(GDestroyNotify) NULL);

	/* Initialize IDLE members. */
	g_mutex_init (&is->priv->idle_lock);
	g_cond_init (&is->priv->idle_cond);
	is->priv->idle_state = IMAPX_IDLE_STATE_OFF;
	is->priv->idle_stamp = 0;

	g_rec_mutex_init (&is->priv->command_lock);
}