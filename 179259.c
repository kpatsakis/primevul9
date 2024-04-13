camel_imapx_server_get_message_sync (CamelIMAPXServer *is,
				     CamelIMAPXMailbox *mailbox,
				     CamelFolderSummary *summary,
				     CamelDataCache *message_cache,
				     const gchar *message_uid,
				     GCancellable *cancellable,
				     GError **error)
{
	CamelMessageInfo *mi;
	CamelStream *result_stream = NULL;
	CamelIMAPXSettings *settings;
	GIOStream *cache_stream;
	gsize data_size;
	gboolean use_multi_fetch;
	gboolean success;
	GError *local_error = NULL;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), NULL);
	g_return_val_if_fail (CAMEL_IS_FOLDER_SUMMARY (summary), NULL);
	g_return_val_if_fail (CAMEL_IS_DATA_CACHE (message_cache), NULL);
	g_return_val_if_fail (message_uid != NULL, NULL);

	if (!camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error))
		return NULL;

	mi = camel_folder_summary_get (summary, message_uid);
	if (mi == NULL) {
		g_set_error (
			error, CAMEL_FOLDER_ERROR,
			CAMEL_FOLDER_ERROR_INVALID_UID,
			_("Cannot get message with message ID %s: %s"),
			message_uid, _("No such message available."));
		return NULL;
	}

	/* This makes sure that if any file is left on the disk, it is not reused.
	   That can happen when the previous message download had been cancelled
	   or finished with an error. */
	camel_data_cache_remove (message_cache, "tmp", message_uid, NULL);

	cache_stream = camel_data_cache_add (message_cache, "tmp", message_uid, error);
	if (cache_stream == NULL) {
		camel_message_info_unref (mi);
		return NULL;
	}

	settings = camel_imapx_server_ref_settings (is);
	data_size = ((CamelMessageInfoBase *) mi)->size;
	use_multi_fetch = data_size > MULTI_SIZE && camel_imapx_settings_get_use_multi_fetch (settings);
	g_object_unref (settings);

	g_warn_if_fail (is->priv->get_message_stream == NULL);

	is->priv->get_message_stream = cache_stream;

	if (use_multi_fetch) {
		CamelIMAPXCommand *ic;
		gsize fetch_offset = 0;

		do {
			camel_operation_progress (cancellable, fetch_offset * 100 / data_size);

			ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_GET_MESSAGE, "UID FETCH %t (BODY.PEEK[]", message_uid);
			camel_imapx_command_add (ic, "<%u.%u>", fetch_offset, MULTI_SIZE);
			camel_imapx_command_add (ic, ")");
			fetch_offset += MULTI_SIZE;

			success = camel_imapx_server_process_command_sync (is, ic, _("Error fetching message"), cancellable, &local_error);

			camel_imapx_command_unref (ic);
			ic = NULL;

			if (success) {
				gsize really_fetched = g_seekable_tell (G_SEEKABLE (is->priv->get_message_stream));

				/* Don't automatically stop when we reach the reported message
				 * size -- some crappy servers (like Microsoft Exchange) have
				 * a tendency to lie about it. Keep going (one request at a
				 * time) until the data actually stop coming. */
				if (fetch_offset < data_size ||
				    fetch_offset == really_fetched) {
					/* just continue */
				} else {
					break;
				}
			}
		} while (success);
	} else {
		CamelIMAPXCommand *ic;

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_GET_MESSAGE, "UID FETCH %t (BODY.PEEK[])", message_uid);

		success = camel_imapx_server_process_command_sync (is, ic, _("Error fetching message"), cancellable, &local_error);

		camel_imapx_command_unref (ic);
	}

	is->priv->get_message_stream = NULL;

	if (success) {
		if (local_error == NULL) {
			g_io_stream_close (cache_stream, cancellable, &local_error);
			g_prefix_error (
				&local_error, "%s: ",
				_("Failed to close the tmp stream"));
		}

		if (local_error == NULL &&
		    g_cancellable_set_error_if_cancelled (cancellable, &local_error)) {
			g_prefix_error (
				&local_error, "%s: ",
				_("Error fetching message"));
		}

		if (local_error == NULL) {
			gchar *cur_filename;
			gchar *tmp_filename;
			gchar *dirname;

			cur_filename = camel_data_cache_get_filename (message_cache, "cur", message_uid);
			tmp_filename = camel_data_cache_get_filename (message_cache, "tmp", message_uid);

			dirname = g_path_get_dirname (cur_filename);
			g_mkdir_with_parents (dirname, 0700);
			g_free (dirname);

			if (g_rename (tmp_filename, cur_filename) == 0) {
				/* Exchange the "tmp" stream for the "cur" stream. */
				g_clear_object (&cache_stream);
				cache_stream = camel_data_cache_get (message_cache, "cur", message_uid, &local_error);
			} else {
				g_set_error (
					&local_error, G_FILE_ERROR,
					g_file_error_from_errno (errno),
					"%s: %s",
					_("Failed to copy the tmp file"),
					g_strerror (errno));
			}

			g_free (cur_filename);
			g_free (tmp_filename);
		}

		/* Delete the 'tmp' file only if the operation succeeded. It's because
		   cancelled operations end before they are properly finished (IMAP-protocol speaking),
		   thus if any other GET_MESSAGE operation was waiting for this job, then it
		   realized that the message was not downloaded and opened its own "tmp" file, but
		   of the same name, thus this remove would drop file which could be used
		   by a different GET_MESSAGE job. */
		if (!local_error && !g_cancellable_is_cancelled (cancellable))
			camel_data_cache_remove (message_cache, "tmp", message_uid, NULL);
	}

	if (!local_error) {
		result_stream = camel_stream_new (cache_stream);
	} else {
		g_propagate_error (error, local_error);
	}

	g_clear_object (&cache_stream);

	return result_stream;
}