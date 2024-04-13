imapx_server_run_idle_thread_cb (gpointer user_data)
{
	GWeakRef *is_weakref = user_data;
	CamelIMAPXServer *is;

	g_return_val_if_fail (is_weakref != NULL, FALSE);

	is = g_weak_ref_get (is_weakref);
	if (!is)
		return FALSE;

	g_mutex_lock (&is->priv->idle_lock);

	if (g_main_current_source () == is->priv->idle_pending) {
		if (!g_source_is_destroyed (g_main_current_source ()) &&
		    is->priv->idle_state == IMAPX_IDLE_STATE_SCHEDULED) {
			IdleThreadData *itd;
			GThread *thread;
			GError *local_error = NULL;

			itd = g_new0 (IdleThreadData, 1);
			itd->is = g_object_ref (is);
			itd->idle_cancellable = g_object_ref (is->priv->idle_cancellable);
			itd->idle_stamp = is->priv->idle_stamp;

			thread = g_thread_try_new (NULL, imapx_server_idle_thread, itd, &local_error);
			if (thread) {
				g_thread_unref (thread);
			} else {
				g_warning ("%s: Failed to create IDLE thread: %s", G_STRFUNC, local_error ? local_error->message : "Unknown error");

				g_clear_object (&itd->is);
				g_clear_object (&itd->idle_cancellable);
				g_free (itd);
			}

			g_clear_error (&local_error);
		}

		g_source_unref (is->priv->idle_pending);
		is->priv->idle_pending = NULL;
	}

	g_mutex_unlock (&is->priv->idle_lock);

	return FALSE;
}