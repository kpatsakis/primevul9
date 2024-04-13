imapx_untagged_fetch (CamelIMAPXServer *is,
                      GInputStream *input_stream,
                      GCancellable *cancellable,
                      GError **error)
{
	struct _fetch_info *finfo;
	gboolean got_body_header;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	finfo = imapx_parse_fetch (
		CAMEL_IMAPX_INPUT_STREAM (input_stream), cancellable, error);
	if (finfo == NULL) {
		imapx_free_fetch (finfo);
		return FALSE;
	}

	/* Some IMAP servers respond with BODY[HEADER] when
	 * asked for RFC822.HEADER.  Treat them equivalently. */
	got_body_header =
		((finfo->got & FETCH_HEADER) == 0) &&
		(finfo->header == NULL) &&
		((finfo->got & FETCH_BODY) != 0) &&
		(g_strcmp0 (finfo->section, "HEADER") == 0);

	if (got_body_header) {
		finfo->got |= FETCH_HEADER;
		finfo->got &= ~FETCH_BODY;
		finfo->header = finfo->body;
		finfo->body = NULL;
	}

	if ((finfo->got & (FETCH_BODY | FETCH_UID)) == (FETCH_BODY | FETCH_UID)) {
		GOutputStream *output_stream;
		gconstpointer body_data;
		gsize body_size;

		if (!is->priv->get_message_stream) {
			g_warn_if_fail (is->priv->get_message_stream != NULL);
			imapx_free_fetch (finfo);
			return FALSE;
		}

		/* Fill out the body stream, in the right spot. */

		g_seekable_seek (
			G_SEEKABLE (is->priv->get_message_stream),
			finfo->offset, G_SEEK_SET,
			NULL, NULL);

		output_stream = g_io_stream_get_output_stream (is->priv->get_message_stream);

		body_data = g_bytes_get_data (finfo->body, &body_size);

		/* Sometimes the server, like Microsoft Exchange, reports larger message
		   size than it actually is, which results in no data being read from
		   the server for that particular offset. */
		if (body_size) {
			g_mutex_lock (&is->priv->stream_lock);
			if (!g_output_stream_write_all (
				output_stream, body_data, body_size,
				NULL, cancellable, error)) {
				g_mutex_unlock (&is->priv->stream_lock);
				g_prefix_error (
					error, "%s: ",
					_("Error writing to cache stream"));
				imapx_free_fetch (finfo);
				return FALSE;
			}
			g_mutex_unlock (&is->priv->stream_lock);
		}
	}

	if ((finfo->got & FETCH_FLAGS) && !(finfo->got & FETCH_HEADER)) {
		CamelIMAPXMailbox *select_mailbox;
		CamelIMAPXMailbox *select_pending;

		if (is->priv->fetch_changes_mailbox) {
			if (!is->priv->fetch_changes_mailbox ||
			    !is->priv->fetch_changes_folder ||
			    !is->priv->fetch_changes_infos) {
				g_warn_if_fail (is->priv->fetch_changes_mailbox != NULL);
				g_warn_if_fail (is->priv->fetch_changes_folder != NULL);
				g_warn_if_fail (is->priv->fetch_changes_infos != NULL);
				imapx_free_fetch (finfo);
				return FALSE;
			}
		}

		g_mutex_lock (&is->priv->select_lock);
		select_mailbox = g_weak_ref_get (&is->priv->select_mailbox);
		select_pending = g_weak_ref_get (&is->priv->select_pending);
		g_mutex_unlock (&is->priv->select_lock);

		/* This is either a refresh_info job, check to see if it is
		 * and update if so, otherwise it must've been an unsolicited
		 * response, so update the summary to match. */
		if ((finfo->got & FETCH_UID) != 0 && is->priv->fetch_changes_folder && is->priv->fetch_changes_infos) {
			FetchChangesInfo *nfo;
			gint64 monotonic_time;
			gint n_messages;

			nfo = g_hash_table_lookup (is->priv->fetch_changes_infos, finfo->uid);
			if (!nfo) {
				nfo = g_slice_new0 (FetchChangesInfo);

				g_hash_table_insert (is->priv->fetch_changes_infos, (gpointer) camel_pstring_strdup (finfo->uid), nfo);
			}

			nfo->server_flags = finfo->flags;
			nfo->server_user_flags = finfo->user_flags;
			finfo->user_flags = NULL;

			monotonic_time = g_get_monotonic_time ();
			n_messages = camel_imapx_mailbox_get_messages (is->priv->fetch_changes_mailbox);

			if (n_messages > 0 && is->priv->fetch_changes_last_progress + G_USEC_PER_SEC / 2 < monotonic_time &&
			    is->priv->context && is->priv->context->id <= n_messages) {
				COMMAND_LOCK (is);

				if (is->priv->current_command) {
					guint32 n_messages;

					COMMAND_UNLOCK (is);

					is->priv->fetch_changes_last_progress = monotonic_time;

					n_messages = camel_imapx_mailbox_get_messages (is->priv->fetch_changes_mailbox);
					if (n_messages > 0)
						camel_operation_progress (cancellable, 100 * is->priv->context->id / n_messages);
				} else {
					COMMAND_UNLOCK (is);
				}
			}
		} else if (select_mailbox != NULL) {
			CamelFolder *select_folder;
			CamelMessageInfo *mi = NULL;
			gboolean changed = FALSE;
			gchar *uid = NULL;

			c (is->priv->tagprefix, "flag changed: %lu\n", is->priv->context->id);

			if (select_pending)
				select_folder = imapx_server_ref_folder (is, select_pending);
			else
				select_folder = imapx_server_ref_folder (is, select_mailbox);
			if (!select_folder) {
				g_warn_if_fail (select_folder != NULL);

				g_clear_object (&select_mailbox);
				g_clear_object (&select_pending);
				imapx_free_fetch (finfo);

				return FALSE;
			}

			if (finfo->got & FETCH_UID) {
				uid = finfo->uid;
				finfo->uid = NULL;
			} else {
				uid = camel_imapx_dup_uid_from_summary_index (
					select_folder,
					is->priv->context->id - 1);
			}

			if (uid) {
				mi = camel_folder_summary_get (camel_folder_get_folder_summary (select_folder), uid);
				if (mi) {
					/* It's unsolicited _unless_ select_pending (i.e. during
					 * a QRESYNC SELECT */
					changed = imapx_update_message_info_flags (
						mi, finfo->flags,
						finfo->user_flags,
						camel_imapx_mailbox_get_permanentflags (select_mailbox),
						select_folder,
						(select_pending == NULL));
					c (is->priv->tagprefix, "found uid %s in '%s', changed:%d\n", uid,
						camel_folder_get_full_name (select_folder), changed);
				} else {
					/* This (UID + FLAGS for previously unknown message) might
					 * happen during a SELECT (QRESYNC). We should use it. */
					c (is->priv->tagprefix, "flags changed for unknown uid %s in '%s'\n", uid,
						camel_folder_get_full_name (select_folder));
				}
			}

			if (changed) {
				CamelIMAPXSummary *imapx_summary;

				imapx_summary = CAMEL_IMAPX_SUMMARY (camel_folder_get_folder_summary (select_folder));

				if (imapx_summary && (finfo->got & FETCH_MODSEQ) != 0 &&
				    imapx_summary->modseq < finfo->modseq) {
					c (is->priv->tagprefix, "updating summary modseq %" G_GUINT64_FORMAT "~>%" G_GUINT64_FORMAT " in '%s'\n",
						imapx_summary->modseq, finfo->modseq, camel_folder_get_full_name (select_folder));
					imapx_summary->modseq = finfo->modseq;

					camel_folder_summary_touch (CAMEL_FOLDER_SUMMARY (imapx_summary));
				}

				g_mutex_lock (&is->priv->changes_lock);
				if (is->priv->changes)
					camel_folder_change_info_change_uid (is->priv->changes, uid);
				else
					g_warn_if_fail (is->priv->changes != NULL);
				g_mutex_unlock (&is->priv->changes_lock);
			}
			g_free (uid);

			if (changed && camel_imapx_server_is_in_idle (is)) {
				camel_folder_summary_save (camel_folder_get_folder_summary (select_folder), NULL);
				imapx_update_store_summary (select_folder);

				g_mutex_lock (&is->priv->changes_lock);

				camel_folder_changed (select_folder, is->priv->changes);
				camel_folder_change_info_clear (is->priv->changes);

				g_mutex_unlock (&is->priv->changes_lock);
			}

			g_clear_object (&mi);

			g_object_unref (select_folder);
		}

		g_clear_object (&select_mailbox);
		g_clear_object (&select_pending);
	}

	if ((finfo->got & (FETCH_HEADER | FETCH_UID)) == (FETCH_HEADER | FETCH_UID)) {
		CamelIMAPXMailbox *mailbox;
		CamelFolder *folder;
		CamelMimeParser *mp;
		CamelMessageInfo *mi;
		guint32 messages;
		guint32 unseen;
		guint32 uidnext;

		/* This must be a refresh info job as well, but it has
		 * asked for new messages to be added to the index. */

		if (is->priv->fetch_changes_mailbox) {
			if (!is->priv->fetch_changes_mailbox ||
			    !is->priv->fetch_changes_folder ||
			    !is->priv->fetch_changes_infos) {
				g_warn_if_fail (is->priv->fetch_changes_mailbox != NULL);
				g_warn_if_fail (is->priv->fetch_changes_folder != NULL);
				g_warn_if_fail (is->priv->fetch_changes_infos != NULL);
				imapx_free_fetch (finfo);
				return FALSE;
			}

			folder = g_object_ref (is->priv->fetch_changes_folder);
			mailbox = g_object_ref (is->priv->fetch_changes_mailbox);
		} else {
			mailbox = camel_imapx_server_ref_selected (is);
			folder = mailbox ? imapx_server_ref_folder (is, mailbox) : NULL;
		}

		if (!mailbox || !folder || (!(finfo->got & FETCH_FLAGS) && !is->priv->fetch_changes_infos)) {
			g_clear_object (&mailbox);
			g_clear_object (&folder);
			imapx_free_fetch (finfo);

			return TRUE;
		}

		messages = camel_imapx_mailbox_get_messages (mailbox);
		unseen = camel_imapx_mailbox_get_unseen (mailbox);
		uidnext = camel_imapx_mailbox_get_uidnext (mailbox);

		/* Do we want to save these headers for later too?  Do we care? */

		mp = camel_mime_parser_new ();
		camel_mime_parser_init_with_bytes (mp, finfo->header);
		mi = camel_folder_summary_info_new_from_parser (camel_folder_get_folder_summary (folder), mp);
		g_object_unref (mp);

		if (mi != NULL) {
			guint32 server_flags;
			CamelNamedFlags *server_user_flags;
			gboolean free_user_flags = FALSE;

			camel_message_info_set_abort_notifications (mi, TRUE);

			camel_message_info_set_uid (mi, finfo->uid);

			if ((finfo->got & FETCH_CINFO) && finfo->cinfo) {
				gboolean has_attachment = FALSE;

				camel_message_content_info_traverse (finfo->cinfo, imapx_server_cinfo_has_attachment_cb, &has_attachment);

				camel_message_info_set_flags (mi, CAMEL_MESSAGE_ATTACHMENTS, has_attachment ? CAMEL_MESSAGE_ATTACHMENTS : 0);
			}

			if (!(finfo->got & FETCH_FLAGS) && is->priv->fetch_changes_infos) {
				FetchChangesInfo *nfo;

				nfo = g_hash_table_lookup (is->priv->fetch_changes_infos, finfo->uid);
				if (!nfo) {
					g_warn_if_fail (nfo != NULL);

					camel_message_info_set_abort_notifications (mi, FALSE);
					g_clear_object (&mi);
					g_clear_object (&mailbox);
					g_clear_object (&folder);
					imapx_free_fetch (finfo);

					return FALSE;
				}

				server_flags = nfo->server_flags;
				server_user_flags = nfo->server_user_flags;
			} else {
				server_flags = finfo->flags;
				server_user_flags = finfo->user_flags;
				/* free user_flags ? */
				finfo->user_flags = NULL;
				free_user_flags = TRUE;
			}

			/* If the message is a really new one -- equal or higher than what
			 * we know as UIDNEXT for the folder, then it came in since we last
			 * fetched UIDNEXT and UNREAD count. We'll update UIDNEXT in the
			 * command completion, but update UNREAD count now according to the
			 * message SEEN flag */
			if (!(server_flags & CAMEL_MESSAGE_SEEN)) {
				guint64 uidl;

				uidl = strtoull (finfo->uid, NULL, 10);

				if (uidl >= uidnext) {
					c (is->priv->tagprefix, "Updating unseen count for new message %s\n", finfo->uid);
					camel_imapx_mailbox_set_unseen (mailbox, unseen + 1);
				} else {
					c (is->priv->tagprefix, "Not updating unseen count for new message %s\n", finfo->uid);
				}
			}

			camel_message_info_set_size (mi, finfo->size);
			camel_message_info_set_abort_notifications (mi, FALSE);

			camel_folder_summary_lock (camel_folder_get_folder_summary (folder));

			if (!camel_folder_summary_check_uid (camel_folder_get_folder_summary (folder), finfo->uid)) {
				imapx_set_message_info_flags_for_new_message (mi, server_flags, server_user_flags, FALSE, NULL, camel_imapx_mailbox_get_permanentflags (mailbox));
				camel_folder_summary_add (camel_folder_get_folder_summary (folder), mi, TRUE);

				g_mutex_lock (&is->priv->changes_lock);

				camel_folder_change_info_add_uid (is->priv->changes, finfo->uid);
				camel_folder_change_info_recent_uid (is->priv->changes, finfo->uid);

				g_mutex_unlock (&is->priv->changes_lock);

				if (messages > 0) {
					gint cnt = (camel_folder_summary_count (camel_folder_get_folder_summary (folder)) * 100) / messages;

					camel_operation_progress (cancellable, cnt ? cnt : 1);
				}

				if (camel_imapx_server_is_in_idle (is) && !camel_folder_is_frozen (folder))
					camel_folder_summary_save (camel_folder_get_folder_summary (folder), NULL);
			}

			g_clear_object (&mi);
			camel_folder_summary_unlock (camel_folder_get_folder_summary (folder));

			if (free_user_flags)
				camel_named_flags_free (server_user_flags);

			if (camel_imapx_server_is_in_idle (is) && !camel_folder_is_frozen (folder)) {
				CamelFolderChangeInfo *changes = NULL;

				g_mutex_lock (&is->priv->changes_lock);

				if (camel_folder_change_info_changed (is->priv->changes)) {
					changes = is->priv->changes;
					is->priv->changes = camel_folder_change_info_new ();
				}

				g_mutex_unlock (&is->priv->changes_lock);

				if (changes) {
					imapx_update_store_summary (folder);
					camel_folder_changed (folder, changes);

					camel_folder_change_info_free (changes);
				}
			}
		}

		g_clear_object (&mailbox);
		g_clear_object (&folder);
	}

	imapx_free_fetch (finfo);

	return TRUE;
}