camel_imapx_server_is_in_idle (CamelIMAPXServer *is)
{
	gboolean in_idle;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	g_mutex_lock (&is->priv->idle_lock);
	in_idle = is->priv->idle_state != IMAPX_IDLE_STATE_OFF;
	g_mutex_unlock (&is->priv->idle_lock);

	return in_idle;
}