camel_imapx_server_ref_selected (CamelIMAPXServer *is)
{
	CamelIMAPXMailbox *mailbox;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);

	g_mutex_lock (&is->priv->select_lock);

	mailbox = g_weak_ref_get (&is->priv->select_mailbox);
	if (mailbox == NULL)
		mailbox = g_weak_ref_get (&is->priv->select_pending);

	g_mutex_unlock (&is->priv->select_lock);

	return mailbox;
}