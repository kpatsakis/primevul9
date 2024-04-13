imapx_expunge_uid_from_summary (CamelIMAPXServer *is,
                                const gchar *uid,
                                gboolean unsolicited)
{
	CamelFolder *folder;
	CamelIMAPXMailbox *mailbox;
	guint32 messages;

	mailbox = camel_imapx_server_ref_pending_or_selected (is);

	g_return_if_fail (mailbox != NULL);

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_if_fail (folder != NULL);

	messages = camel_imapx_mailbox_get_messages (mailbox);

	if (unsolicited && messages > 0)
		camel_imapx_mailbox_set_messages (mailbox, messages - 1);

	g_return_if_fail (is->priv->changes != NULL);

	camel_folder_summary_remove_uid (folder->summary, uid);

	g_mutex_lock (&is->priv->changes_lock);

	camel_folder_change_info_remove_uid (is->priv->changes, uid);

	if (camel_imapx_server_is_in_idle (is)) {
		CamelFolderChangeInfo *changes;

		changes = is->priv->changes;
		is->priv->changes = camel_folder_change_info_new ();

		g_mutex_unlock (&is->priv->changes_lock);

		camel_folder_summary_save_to_db (folder->summary, NULL);
		imapx_update_store_summary (folder);
		camel_folder_changed (folder, changes);

		camel_folder_change_info_free (changes);
	} else {
		g_mutex_unlock (&is->priv->changes_lock);
	}

	g_object_unref (folder);
	g_object_unref (mailbox);
}