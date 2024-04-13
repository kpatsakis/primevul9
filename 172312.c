imapx_completion (CamelIMAPXServer *is,
                  GInputStream *input_stream,
                  guchar *token,
                  gint len,
                  GCancellable *cancellable,
                  GError **error)
{
	CamelIMAPXCommand *ic;
	CamelIMAPXMailbox *mailbox;
	gboolean success = FALSE;
	guint tag;

	/* Given "A0001 ...", 'A' = tag prefix, '0001' = tag. */

	if (token[0] != is->priv->tagprefix) {
		g_set_error (
			error, CAMEL_IMAPX_ERROR, CAMEL_IMAPX_ERROR_SERVER_RESPONSE_MALFORMED,
			"Server sent unexpected response: %s", token);
		return FALSE;
	}

	tag = strtoul ((gchar *) token + 1, NULL, 10);

	COMMAND_LOCK (is);

	if (is->priv->current_command != NULL && is->priv->current_command->tag == tag)
		ic = camel_imapx_command_ref (is->priv->current_command);
	else
		ic = NULL;

	COMMAND_UNLOCK (is);

	if (ic == NULL) {
		g_set_error (
			error, CAMEL_IMAPX_ERROR, CAMEL_IMAPX_ERROR_SERVER_RESPONSE_MALFORMED,
			"got response tag unexpectedly: %s", token);
		return FALSE;
	}

	c (is->priv->tagprefix, "Got completion response for command %05u '%s'\n", ic->tag, camel_imapx_job_get_kind_name (ic->job_kind));

	/* The camel_imapx_server_refresh_info_sync() gets any piled change
	   notifications and will emit the signal with all of them at once.
	   Similarly message COPY/MOVE command. */
	if (!is->priv->fetch_changes_mailbox && !is->priv->fetch_changes_folder && !is->priv->fetch_changes_infos &&
	    ic->job_kind != CAMEL_IMAPX_JOB_COPY_MESSAGE && ic->job_kind != CAMEL_IMAPX_JOB_MOVE_MESSAGE) {
		g_mutex_lock (&is->priv->changes_lock);

		if (camel_folder_change_info_changed (is->priv->changes)) {
			CamelFolder *folder = NULL;
			CamelIMAPXMailbox *mailbox;
			CamelFolderChangeInfo *changes;

			changes = is->priv->changes;
			is->priv->changes = camel_folder_change_info_new ();

			g_mutex_unlock (&is->priv->changes_lock);

			mailbox = camel_imapx_server_ref_pending_or_selected (is);

			g_warn_if_fail (mailbox != NULL);

			if (mailbox) {
				folder = imapx_server_ref_folder (is, mailbox);
				g_return_val_if_fail (folder != NULL, FALSE);

				camel_folder_summary_save (camel_folder_get_folder_summary (folder), NULL);

				imapx_update_store_summary (folder);
				camel_folder_changed (folder, changes);
			}

			camel_folder_change_info_free (changes);

			g_clear_object (&folder);
			g_clear_object (&mailbox);
		} else {
			g_mutex_unlock (&is->priv->changes_lock);
		}
	}

	if (g_list_next (ic->current_part) != NULL) {
		g_set_error (
			error, CAMEL_IMAPX_ERROR, CAMEL_IMAPX_ERROR_SERVER_RESPONSE_MALFORMED,
			"command still has unsent parts? %s", camel_imapx_job_get_kind_name (ic->job_kind));
		goto exit;
	}

	mailbox = camel_imapx_server_ref_selected (is);

	ic->status = imapx_parse_status (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		mailbox, FALSE, cancellable, error);

	g_clear_object (&mailbox);

	if (ic->status == NULL)
		goto exit;

	if (ic->status->condition == IMAPX_CAPABILITY) {
		guint32 list_extended = imapx_lookup_capability ("LIST-EXTENDED");

		is->priv->is_broken_cyrus = is->priv->is_broken_cyrus || (ic->status->text && imapx_server_check_is_broken_cyrus (ic->status->text, &is->priv->is_cyrus));
		if (is->priv->is_broken_cyrus && ic->status->u.cinfo && (ic->status->u.cinfo->capa & list_extended) != 0) {
			/* Disable LIST-EXTENDED for cyrus servers */
			c (is->priv->tagprefix, "Disabling LIST-EXTENDED extension for a Cyrus server\n");
			ic->status->u.cinfo->capa &= ~list_extended;
		}
	}

	success = TRUE;

exit:

	ic->completed = TRUE;
	camel_imapx_command_unref (ic);

	return success;
}