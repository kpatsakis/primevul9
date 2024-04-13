camel_imapx_server_mailbox_selected (CamelIMAPXServer *is,
				     CamelIMAPXMailbox *mailbox)
{
	CamelIMAPXMailbox *selected_mailbox;
	gboolean res;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	g_mutex_lock (&is->priv->select_lock);
	selected_mailbox = g_weak_ref_get (&is->priv->select_mailbox);
	res = selected_mailbox == mailbox;
	g_clear_object (&selected_mailbox);
	g_mutex_unlock (&is->priv->select_lock);

	return res;
}