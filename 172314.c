camel_imapx_server_ref_idle_mailbox (CamelIMAPXServer *is)
{
	CamelIMAPXMailbox *mailbox = NULL;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);

	g_mutex_lock (&is->priv->idle_lock);

	if (is->priv->idle_state != IMAPX_IDLE_STATE_OFF) {
		if (is->priv->idle_mailbox)
			mailbox = g_object_ref (is->priv->idle_mailbox);
		else
			mailbox = camel_imapx_server_ref_selected (is);
	}

	g_mutex_unlock (&is->priv->idle_lock);

	return mailbox;
}