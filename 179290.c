imapx_untagged_vanished (CamelIMAPXServer *is,
                         GInputStream *input_stream,
                         GCancellable *cancellable,
                         GError **error)
{
	CamelFolder *folder;
	CamelIMAPXMailbox *mailbox;
	GArray *uids;
	GList *uid_list = NULL;
	gboolean unsolicited = TRUE;
	guint ii = 0;
	guint len = 0;
	guchar *token = NULL;
	gint tok = 0;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	tok = camel_imapx_input_stream_token (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		&token, &len, cancellable, error);
	if (tok < 0)
		return FALSE;
	if (tok == '(') {
		unsolicited = FALSE;
		while (tok != ')') {
			/* We expect this to be 'EARLIER' */
			tok = camel_imapx_input_stream_token (
				CAMEL_IMAPX_INPUT_STREAM (input_stream),
				&token, &len, cancellable, error);
			if (tok < 0)
				return FALSE;
		}
	} else {
		camel_imapx_input_stream_ungettoken (
			CAMEL_IMAPX_INPUT_STREAM (input_stream),
			tok, token, len);
	}

	uids = imapx_parse_uids (
		CAMEL_IMAPX_INPUT_STREAM (input_stream), cancellable, error);
	if (uids == NULL)
		return FALSE;

	mailbox = camel_imapx_server_ref_pending_or_selected (is);

	g_return_val_if_fail (mailbox != NULL, FALSE);

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_val_if_fail (folder != NULL, FALSE);

	if (unsolicited) {
		guint32 messages;

		messages = camel_imapx_mailbox_get_messages (mailbox);

		if (messages < uids->len) {
			c (
				is->priv->tagprefix,
				"Error: mailbox messages (%u) is "
				"fewer than vanished %u\n",
				messages, uids->len);
			messages = 0;
		} else {
			messages -= uids->len;
		}

		camel_imapx_mailbox_set_messages (mailbox, messages);
	}

	g_return_val_if_fail (is->priv->changes != NULL, FALSE);

	g_mutex_lock (&is->priv->changes_lock);

	for (ii = 0; ii < uids->len; ii++) {
		guint32 uid;
		gchar *str;

		uid = g_array_index (uids, guint32, ii);

		e (is->priv->tagprefix, "vanished: %u\n", uid);

		str = g_strdup_printf ("%u", uid);
		uid_list = g_list_prepend (uid_list, str);
		camel_folder_change_info_remove_uid (is->priv->changes, str);
	}

	g_mutex_unlock (&is->priv->changes_lock);

	uid_list = g_list_reverse (uid_list);
	camel_folder_summary_remove_uids (folder->summary, uid_list);

	/* If the response is truly unsolicited (e.g. via NOTIFY)
	 * then go ahead and emit the change notification now. */
	COMMAND_LOCK (is);
	if (!is->priv->current_command) {
		COMMAND_UNLOCK (is);

		g_mutex_lock (&is->priv->changes_lock);
		if (is->priv->changes->uid_removed &&
		    is->priv->changes->uid_removed->len >= 100) {
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
	} else {
		COMMAND_UNLOCK (is);
	}

	g_list_free_full (uid_list, (GDestroyNotify) g_free);
	g_array_free (uids, TRUE);

	g_object_unref (folder);
	g_object_unref (mailbox);

	return TRUE;
}