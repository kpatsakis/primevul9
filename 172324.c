imapx_server_wait_idle_stop_cancelled_cb (GCancellable *cancellable,
					  gpointer user_data)
{
	CamelIMAPXServer *is = user_data;

	g_return_if_fail (CAMEL_IS_IMAPX_SERVER (is));

	g_mutex_lock (&is->priv->idle_lock);
	g_cond_broadcast (&is->priv->idle_cond);
	g_mutex_unlock (&is->priv->idle_lock);
}