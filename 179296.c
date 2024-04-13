camel_imapx_server_append_message_sync (CamelIMAPXServer *is,
					CamelIMAPXMailbox *mailbox,
					CamelFolderSummary *summary,
					CamelDataCache *message_cache,
					CamelMimeMessage *message,
					const CamelMessageInfo *mi,
					gchar **appended_uid,
					GCancellable *cancellable,
					GError **error)
{
	gchar *uid = NULL, *path = NULL;
	CamelMimeFilter *filter;
	CamelIMAPXCommand *ic;
	CamelMessageInfo *info;
	GIOStream *base_stream;
	GOutputStream *output_stream;
	GOutputStream *filter_stream;
	gint res;
	time_t date_time;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);
	g_return_val_if_fail (CAMEL_IS_FOLDER_SUMMARY (summary), FALSE);
	g_return_val_if_fail (CAMEL_IS_DATA_CACHE (message_cache), FALSE);
	g_return_val_if_fail (CAMEL_IS_MIME_MESSAGE (message), FALSE);
	/* CamelMessageInfo can be NULL. */

	/* That's okay if the "SELECT" fails here, as it can be due to
	   the folder being write-only; just ignore the error and continue. */
	camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, NULL);

	if (g_cancellable_set_error_if_cancelled (cancellable, error))
		return FALSE;

	/* Append just assumes we have no/a dodgy connection.  We dump
	 * stuff into the 'new' directory, and let the summary know it's
	 * there.  Then we fire off a no-reply job which will asynchronously
	 * upload the message at some point in the future, and fix up the
	 * summary to match */

	/* chen cleanup this later */
	uid = imapx_get_temp_uid ();
	base_stream = camel_data_cache_add (message_cache, "new", uid, error);
	if (base_stream == NULL) {
		g_prefix_error (error, _("Cannot create spool file: "));
		g_free (uid);
		return FALSE;
	}

	output_stream = g_io_stream_get_output_stream (base_stream);
	filter = camel_mime_filter_canon_new (CAMEL_MIME_FILTER_CANON_CRLF);
	filter_stream = camel_filter_output_stream_new (output_stream, filter);

	g_filter_output_stream_set_close_base_stream (
		G_FILTER_OUTPUT_STREAM (filter_stream), FALSE);

	res = camel_data_wrapper_write_to_output_stream_sync (
		CAMEL_DATA_WRAPPER (message),
		filter_stream, cancellable, error);

	g_object_unref (base_stream);
	g_object_unref (filter_stream);
	g_object_unref (filter);

	if (res == -1) {
		g_prefix_error (error, _("Cannot create spool file: "));
		camel_data_cache_remove (message_cache, "new", uid, NULL);
		g_free (uid);
		return FALSE;
	}

	date_time = camel_mime_message_get_date (message, NULL);
	path = camel_data_cache_get_filename (message_cache, "new", uid);
	info = camel_folder_summary_info_new_from_message (summary, message, NULL);
	info->uid = camel_pstring_strdup (uid);

	if (mi != NULL) {
		struct icaltimetype icaltime;
		CamelMessageInfoBase *base_info = (CamelMessageInfoBase *) info;
		const CamelFlag *flag;
		const CamelTag *tag;

		base_info->flags = camel_message_info_flags (mi);
		base_info->size = camel_message_info_size (mi);

		flag = camel_message_info_user_flags (mi);
		while (flag != NULL) {
			if (*flag->name != '\0')
				camel_flag_set (
					&base_info->user_flags,
					flag->name, TRUE);
			flag = flag->next;
		}

		tag = camel_message_info_user_tags (mi);
		while (tag != NULL) {
			if (*tag->name != '\0')
				camel_tag_set (
					&base_info->user_tags,
					tag->name, tag->value);
			tag = tag->next;
		}

		if (date_time > 0) {
			icaltime = icaltime_from_timet (date_time, FALSE);
			if (!icaltime_is_valid_time (icaltime))
				date_time = -1;
		}

		if (date_time <= 0)
			date_time = camel_message_info_date_received (mi);

		if (date_time > 0) {
			icaltime = icaltime_from_timet (date_time, FALSE);
			if (!icaltime_is_valid_time (icaltime))
				date_time = -1;
		}
	}

	g_free (uid);

	if (camel_mime_message_has_attachment (message))
		((CamelMessageInfoBase *) info)->flags |= CAMEL_MESSAGE_ATTACHMENTS;

	if (date_time > 0) {
		gchar *date_time_str;
		struct tm stm;

		gmtime_r (&date_time, &stm);

		/* Store always in UTC */
		date_time_str = g_strdup_printf (
			"\"%02d-%s-%04d %02d:%02d:%02d +0000\"",
			stm.tm_mday,
			get_month_str (stm.tm_mon + 1),
			stm.tm_year + 1900,
			stm.tm_hour,
			stm.tm_min,
			stm.tm_sec);

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_APPEND_MESSAGE, "APPEND %M %F %t %P",
			mailbox,
			((CamelMessageInfoBase *) info)->flags,
			((CamelMessageInfoBase *) info)->user_flags,
			date_time_str,
			path);

		g_free (date_time_str);
	} else {
		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_APPEND_MESSAGE, "APPEND %M %F %P",
			mailbox,
			((CamelMessageInfoBase *) info)->flags,
			((CamelMessageInfoBase *) info)->user_flags,
			path);
	}

	success = camel_imapx_server_process_command_sync (is, ic, _("Error appending message"), cancellable, error);

	if (success) {
		CamelIMAPXFolder *imapx_folder;
		CamelFolder *folder;
		CamelMessageInfo *mi;
		gchar *cur, *old_uid;
		guint32 uidvalidity;

		folder = imapx_server_ref_folder (is, mailbox);
		g_return_val_if_fail (folder != NULL, FALSE);

		uidvalidity = camel_imapx_mailbox_get_uidvalidity (mailbox);

		imapx_folder = CAMEL_IMAPX_FOLDER (folder);

		/* Append done.  If we the server supports UIDPLUS we will get
		 * an APPENDUID response with the new uid.  This lets us move the
		 * message we have directly to the cache and also create a correctly
		 * numbered MessageInfo, without losing any information.  Otherwise
		 * we have to wait for the server to let us know it was appended. */

		mi = camel_message_info_clone (info);
		old_uid = g_strdup (info->uid);

		if (ic->status && ic->status->condition == IMAPX_APPENDUID) {
			c (is->priv->tagprefix, "Got appenduid %d %d\n", (gint) ic->status->u.appenduid.uidvalidity, (gint) ic->status->u.appenduid.uid);
			if (ic->status->u.appenduid.uidvalidity == uidvalidity) {
				if (appended_uid)
					*appended_uid = g_strdup_printf ("%u", (guint) ic->status->u.appenduid.uid);
				mi->uid = camel_pstring_add (g_strdup_printf ("%u", (guint) ic->status->u.appenduid.uid), TRUE);

				cur = camel_data_cache_get_filename  (imapx_folder->cache, "cur", mi->uid);
				if (g_rename (path, cur) == -1 && errno != ENOENT) {
					g_warning ("%s: Failed to rename '%s' to '%s': %s", G_STRFUNC, path, cur, g_strerror (errno));
				}

				imapx_set_message_info_flags_for_new_message (
					mi,
					((CamelMessageInfoBase *) info)->flags,
					((CamelMessageInfoBase *) info)->user_flags,
					TRUE,
					((CamelMessageInfoBase *) info)->user_tags,
					camel_imapx_mailbox_get_permanentflags (mailbox));

				camel_folder_summary_add (folder->summary, mi);

				g_mutex_lock (&is->priv->changes_lock);
				camel_folder_change_info_add_uid (is->priv->changes, mi->uid);
				g_mutex_unlock (&is->priv->changes_lock);

				mi = NULL;

				g_free (cur);
			} else {
				c (is->priv->tagprefix, "but uidvalidity changed \n");
			}
		}

		camel_data_cache_remove (imapx_folder->cache, "new", old_uid, NULL);
		g_free (old_uid);

		camel_imapx_command_unref (ic);
		if (mi)
			camel_message_info_unref (mi);
		g_object_unref (folder);
	}

	g_free (path);

	return success;
}