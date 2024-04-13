imapx_server_finalize (GObject *object)
{
	CamelIMAPXServer *is = CAMEL_IMAPX_SERVER (object);

	g_mutex_clear (&is->priv->stream_lock);
	g_mutex_clear (&is->priv->select_lock);
	g_mutex_clear (&is->priv->changes_lock);

	camel_folder_change_info_free (is->priv->changes);
	imapx_free_status (is->priv->copyuid_status);

	g_slice_free (CamelIMAPXServerUntaggedContext, is->priv->context);
	g_hash_table_destroy (is->priv->untagged_handlers);

	if (is->priv->inactivity_timeout != NULL)
		g_source_unref (is->priv->inactivity_timeout);
	g_mutex_clear (&is->priv->inactivity_timeout_lock);

	g_free (is->priv->status_data_items);
	g_free (is->priv->list_return_opts);

	if (is->priv->search_results != NULL)
		g_array_unref (is->priv->search_results);
	g_mutex_clear (&is->priv->search_results_lock);

	g_hash_table_destroy (is->priv->known_alerts);
	g_mutex_clear (&is->priv->known_alerts_lock);

	g_mutex_clear (&is->priv->idle_lock);
	g_cond_clear (&is->priv->idle_cond);

	g_rec_mutex_clear (&is->priv->command_lock);

	g_weak_ref_clear (&is->priv->store);
	g_weak_ref_clear (&is->priv->select_mailbox);
	g_weak_ref_clear (&is->priv->select_pending);
	g_clear_object (&is->priv->cancellable);

	/* Chain up to parent's finalize() method. */
	G_OBJECT_CLASS (camel_imapx_server_parent_class)->finalize (object);
}