imapx_untagged_vanished (CamelIMAPXServer *is,
                         GInputStream *input_stream,
                         GCancellable *cancellable,
                         GError **error)
{
	CamelFolder *folder;
	CamelIMAPXMailbox *mailbox;
	GatherExistingUidsData geud;
	gboolean unsolicited = TRUE;
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

	g_return_val_if_fail (is->priv->changes != NULL, FALSE);

	mailbox = camel_imapx_server_ref_pending_or_selected (is);
	g_return_val_if_fail (mailbox != NULL, FALSE);

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_val_if_fail (folder != NULL, FALSE);

	geud.is = is;
	geud.summary = camel_folder_get_folder_summary (folder);
	geud.uid_list = NULL;
	geud.n_uids = 0;

	if (!imapx_parse_uids_with_callback (CAMEL_IMAPX_INPUT_STREAM (input_stream), imapx_gather_existing_uids_cb, &geud, cancellable, error)) {
		g_object_unref (folder);
		g_object_unref (mailbox);
		return FALSE;
	}

	/* It's locked by imapx_gather_existing_uids_cb() when the first known UID is found */
	if (geud.uid_list)
		g_mutex_unlock (&is->priv->changes_lock);

	if (unsolicited) {
		guint32 messages;

		messages = camel_imapx_mailbox_get_messages (mailbox);

		if (messages < geud.n_uids) {
			c (
				is->priv->tagprefix,
				"Error: mailbox messages (%u) is "
				"fewer than vanished %u\n",
				messages, geud.n_uids);
			messages = 0;
		} else {
			messages -= geud.n_uids;
		}

		camel_imapx_mailbox_set_messages (mailbox, messages);
	}

	if (geud.uid_list) {
		geud.uid_list = g_list_reverse (geud.uid_list);
		camel_folder_summary_remove_uids (geud.summary, geud.uid_list);
	}

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

			camel_folder_summary_save (geud.summary, NULL);
			imapx_update_store_summary (folder);

			camel_folder_changed (folder, changes);
			camel_folder_change_info_free (changes);
		} else {
			g_mutex_unlock (&is->priv->changes_lock);
		}
	} else {
		COMMAND_UNLOCK (is);
	}

	g_list_free_full (geud.uid_list, g_free);
	g_object_unref (folder);
	g_object_unref (mailbox);

	return TRUE;
}