camel_imapx_server_sync_changes_sync (CamelIMAPXServer *is,
				      CamelIMAPXMailbox *mailbox,
				      gboolean can_influence_flags,
				      GCancellable *cancellable,
				      GError **error)
{
	guint i, jj, on, on_orset, off_orset;
	GPtrArray *changed_uids;
	GArray *on_user = NULL, *off_user = NULL;
	CamelFolder *folder;
	CamelMessageInfo *info;
	CamelFolderChangeInfo *expunged_changes = NULL;
	GList *expunged_removed_list = NULL;
	GHashTable *stamps;
	guint32 permanentflags;
	struct _uidset_state uidset, uidset_expunge;
	gint unread_change = 0;
	gboolean use_real_junk_path = FALSE;
	gboolean use_real_trash_path = FALSE;
	gboolean remove_deleted_flags = FALSE;
	gboolean is_real_trash_folder = FALSE;
	gboolean is_real_junk_folder = FALSE;
	gboolean has_uidplus_capability;
	gboolean expunge_deleted;
	gboolean nothing_to_do;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	folder = imapx_server_ref_folder (is, mailbox);
	g_return_val_if_fail (folder != NULL, FALSE);

	/* We calculate two masks, a mask of all flags which have been
	 * turned off and a mask of all flags which have been turned
	 * on. If either of these aren't 0, then we have work to do,
	 * and we fire off a job to do it.
	 *
	 * User flags are a bit more tricky, we rely on the user
	 * flags being sorted, and then we create a bunch of lists;
	 * one for each flag being turned off, including each
	 * info being turned off, and one for each flag being turned on.
	 */
	changed_uids = camel_folder_summary_get_changed (camel_folder_get_folder_summary (folder));

	if (changed_uids->len == 0) {
		camel_folder_free_uids (folder, changed_uids);
		g_object_unref (folder);
		return TRUE;
	}

	camel_folder_sort_uids (folder, changed_uids);
	stamps = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) camel_pstring_free, NULL);

	if (can_influence_flags) {
		CamelIMAPXSettings *settings;

		settings = camel_imapx_server_ref_settings (is);

		use_real_junk_path = camel_imapx_settings_get_use_real_junk_path (settings);
		if (use_real_junk_path) {
			CamelFolder *junk_folder = NULL;
			gchar *real_junk_path;

			real_junk_path = camel_imapx_settings_dup_real_junk_path (settings);
			if (real_junk_path) {
				junk_folder = camel_store_get_folder_sync (
					camel_folder_get_parent_store (folder),
					real_junk_path, 0, cancellable, NULL);
			}

			is_real_junk_folder = junk_folder == folder;

			use_real_junk_path = junk_folder != NULL;

			g_clear_object (&junk_folder);
			g_free (real_junk_path);
		}

		use_real_trash_path = camel_imapx_settings_get_use_real_trash_path (settings);
		if (use_real_trash_path) {
			CamelFolder *trash_folder = NULL;
			gchar *real_trash_path;

			real_trash_path = camel_imapx_settings_dup_real_trash_path (settings);
			if (real_trash_path)
				trash_folder = camel_store_get_folder_sync (
					camel_folder_get_parent_store (folder),
					real_trash_path, 0, cancellable, NULL);

			is_real_trash_folder = trash_folder == folder;

			/* Remove deleted flags in all but the trash folder itself */
			remove_deleted_flags = !trash_folder || !is_real_trash_folder;

			use_real_trash_path = trash_folder != NULL;

			g_clear_object (&trash_folder);
			g_free (real_trash_path);
		}

		g_object_unref (settings);
	}

	if (changed_uids->len > 20)
		camel_folder_summary_prepare_fetch_all (camel_folder_get_folder_summary (folder), NULL);

	camel_folder_summary_lock (camel_folder_get_folder_summary (folder));

	off_orset = on_orset = 0;
	for (i = 0; i < changed_uids->len; i++) {
		CamelIMAPXMessageInfo *xinfo;
		guint32 flags, sflags;
		const CamelNamedFlags *local_uflags, *server_uflags;
		const gchar *uid;
		guint j = 0;

		uid = g_ptr_array_index (changed_uids, i);

		info = camel_folder_summary_get (camel_folder_get_folder_summary (folder), uid);
		xinfo = info ? CAMEL_IMAPX_MESSAGE_INFO (info) : NULL;

		if (!info || !xinfo) {
			g_clear_object (&info);
			continue;
		}

		if (!camel_message_info_get_folder_flagged (info)) {
			g_clear_object (&info);
			continue;
		}

		camel_message_info_property_lock (info);

		g_hash_table_insert (stamps, (gpointer) camel_message_info_pooldup_uid (info),
			GUINT_TO_POINTER (camel_message_info_get_folder_flagged_stamp (info)));

		flags = camel_message_info_get_flags (info) & CAMEL_IMAPX_SERVER_FLAGS;
		sflags = camel_imapx_message_info_get_server_flags (xinfo) & CAMEL_IMAPX_SERVER_FLAGS;

		if (can_influence_flags) {
			gboolean move_to_real_junk;
			gboolean move_to_real_trash;
			gboolean move_to_inbox;

			move_to_real_junk =
				use_real_junk_path &&
				(flags & CAMEL_MESSAGE_JUNK);

			move_to_real_trash =
				use_real_trash_path && remove_deleted_flags &&
				(flags & CAMEL_MESSAGE_DELETED);

			move_to_inbox = is_real_junk_folder &&
				!move_to_real_junk &&
				!move_to_real_trash &&
				(camel_message_info_get_flags (info) & CAMEL_MESSAGE_NOTJUNK) != 0;

			if (move_to_real_junk)
				camel_imapx_folder_add_move_to_real_junk (
					CAMEL_IMAPX_FOLDER (folder), uid);

			if (move_to_real_trash)
				camel_imapx_folder_add_move_to_real_trash (
					CAMEL_IMAPX_FOLDER (folder), uid);

			if (move_to_inbox)
				camel_imapx_folder_add_move_to_inbox (
					CAMEL_IMAPX_FOLDER (folder), uid);
		}

		if (flags != sflags) {
			off_orset |= (flags ^ sflags) & ~flags;
			on_orset |= (flags ^ sflags) & flags;
		}

		local_uflags = camel_message_info_get_user_flags (info);
		server_uflags = camel_imapx_message_info_get_server_user_flags (xinfo);

		if (!camel_named_flags_equal (local_uflags, server_uflags)) {
			guint ii, jj, llen, slen;

			llen = local_uflags ? camel_named_flags_get_length (local_uflags) : 0;
			slen = server_uflags ? camel_named_flags_get_length (server_uflags) : 0;
			for (ii = 0, jj = 0; ii < llen || jj < slen;) {
				gint res;

				if (ii < llen) {
					const gchar *local_name = camel_named_flags_get (local_uflags, ii);

					if (jj < slen) {
						const gchar *server_name = camel_named_flags_get (server_uflags, jj);

						res = g_strcmp0 (local_name, server_name);
					} else if (local_name && *local_name)
						res = -1;
					else {
						ii++;
						continue;
					}
				} else {
					res = 1;
				}

				if (res == 0) {
					ii++;
					jj++;
				} else {
					GArray *user_set;
					const gchar *user_flag_name;
					struct _imapx_flag_change *change = NULL, add = { 0 };

					if (res < 0) {
						if (on_user == NULL)
							on_user = g_array_new (FALSE, FALSE, sizeof (struct _imapx_flag_change));
						user_set = on_user;
						user_flag_name = camel_named_flags_get (local_uflags, ii);
						ii++;
					} else {
						if (off_user == NULL)
							off_user = g_array_new (FALSE, FALSE, sizeof (struct _imapx_flag_change));
						user_set = off_user;
						user_flag_name = camel_named_flags_get (server_uflags, jj);
						jj++;
					}

					/* Could sort this and binary search */
					for (j = 0; j < user_set->len; j++) {
						change = &g_array_index (user_set, struct _imapx_flag_change, j);
						if (g_strcmp0 (change->name, user_flag_name) == 0)
							goto found;
					}
					add.name = g_strdup (user_flag_name);
					add.infos = g_ptr_array_new ();
					g_array_append_val (user_set, add);
					change = &add;
				found:
					g_object_ref (info);
					g_ptr_array_add (change->infos, info);
				}
			}
		}

		camel_message_info_property_unlock (info);

		g_clear_object (&info);
	}

	camel_folder_summary_unlock (camel_folder_get_folder_summary (folder));

	nothing_to_do =
		(on_orset == 0) &&
		(off_orset == 0) &&
		(on_user == NULL) &&
		(off_user == NULL);

	if (nothing_to_do) {
		imapx_sync_free_user (on_user);
		imapx_sync_free_user (off_user);
		imapx_unset_folder_flagged_flag (camel_folder_get_folder_summary (folder), changed_uids, remove_deleted_flags);
		camel_folder_free_uids (folder, changed_uids);
		g_hash_table_destroy (stamps);
		g_object_unref (folder);
		return TRUE;
	}

	if (!camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error)) {
		imapx_sync_free_user (on_user);
		imapx_sync_free_user (off_user);
		camel_folder_free_uids (folder, changed_uids);
		g_hash_table_destroy (stamps);
		g_object_unref (folder);
		return FALSE;
	}

	imapx_uidset_init (&uidset_expunge, 0, 100);

	has_uidplus_capability = CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, UIDPLUS);
	expunge_deleted = is_real_trash_folder && !remove_deleted_flags;

	permanentflags = camel_imapx_mailbox_get_permanentflags (mailbox);

	success = TRUE;
	for (on = 0; on < 2 && success; on++) {
		guint32 orset = on ? on_orset : off_orset;
		GArray *user_set = on ? on_user : off_user;

		for (jj = 0; jj < G_N_ELEMENTS (flags_table) && success; jj++) {
			guint32 flag = flags_table[jj].flag;
			CamelIMAPXCommand *ic = NULL, *ic_expunge = NULL;

			if ((orset & flag) == 0)
				continue;

			c (is->priv->tagprefix, "checking/storing %s flags '%s'\n", on ? "on" : "off", flags_table[jj].name);
			imapx_uidset_init (&uidset, 0, 100);
			for (i = 0; i < changed_uids->len && success; i++) {
				CamelMessageInfo *info;
				CamelIMAPXMessageInfo *xinfo;
				gboolean remove_deleted_flag;
				guint32 flags;
				guint32 sflags;
				gint send, send_expunge = 0;

				/* the 'stamps' hash table contains only those uid-s,
				   which were also flagged, not only 'dirty' */
				if (!g_hash_table_contains (stamps, changed_uids->pdata[i]))
					continue;

				info = camel_folder_summary_get (camel_folder_get_folder_summary (folder), changed_uids->pdata[i]);
				xinfo = info ? CAMEL_IMAPX_MESSAGE_INFO (info) : NULL;

				if (!info || !xinfo) {
					g_clear_object (&info);
					continue;
				}

				flags = (camel_message_info_get_flags (info) & CAMEL_IMAPX_SERVER_FLAGS) & permanentflags;
				sflags = (camel_imapx_message_info_get_server_flags (xinfo) & CAMEL_IMAPX_SERVER_FLAGS) & permanentflags;
				send = 0;

				remove_deleted_flag =
					remove_deleted_flags &&
					(flags & CAMEL_MESSAGE_DELETED);

				if (remove_deleted_flag) {
					/* Remove the DELETED flag so the
					 * message appears normally in the
					 * real Trash folder when copied. */
					flags &= ~CAMEL_MESSAGE_DELETED;
				} else if (expunge_deleted && (flags & CAMEL_MESSAGE_DELETED) != 0) {
					if (has_uidplus_capability) {
						if (!ic_expunge)
							ic_expunge = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_EXPUNGE, "UID EXPUNGE ");

						send_expunge = imapx_uidset_add (&uidset_expunge, ic_expunge, camel_message_info_get_uid (info));
					}

					if (!expunged_changes)
						expunged_changes = camel_folder_change_info_new ();

					camel_folder_change_info_remove_uid (expunged_changes, camel_message_info_get_uid (info));
					expunged_removed_list = g_list_prepend (expunged_removed_list,
						(gpointer) camel_pstring_strdup (camel_message_info_get_uid (info)));
				}

				if ( (on && (((flags ^ sflags) & flags) & flag))
				     || (!on && (((flags ^ sflags) & ~flags) & flag))) {
					if (ic == NULL) {
						ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_SYNC_CHANGES, "UID STORE ");
					}
					send = imapx_uidset_add (&uidset, ic, camel_message_info_get_uid (info));
				}
				if (send == 1 || (i == changed_uids->len - 1 && ic && imapx_uidset_done (&uidset, ic))) {
					camel_imapx_command_add (ic, " %tFLAGS.SILENT (%t)", on ? "+" : "-", flags_table[jj].name);

					success = camel_imapx_server_process_command_sync (is, ic, _("Error syncing changes"), cancellable, error);

					camel_imapx_command_unref (ic);
					ic = NULL;

					if (!success) {
						g_clear_object (&info);
						break;
					}
				}

				if (has_uidplus_capability && (
				    send_expunge == 1 || (i == changed_uids->len - 1 && ic_expunge && imapx_uidset_done (&uidset_expunge, ic_expunge)))) {
					success = camel_imapx_server_process_command_sync (is, ic_expunge, _("Error expunging message"), cancellable, error);

					camel_imapx_command_unref (ic_expunge);
					ic_expunge = NULL;

					if (!success) {
						g_clear_object (&info);
						break;
					}

					camel_folder_changed (folder, expunged_changes);
					camel_folder_summary_remove_uids (camel_folder_get_folder_summary (folder), expunged_removed_list);

					camel_folder_change_info_free (expunged_changes);
					expunged_changes = NULL;

					g_list_free_full (expunged_removed_list, (GDestroyNotify) camel_pstring_free);
					expunged_removed_list = NULL;
				}

				if (flag == CAMEL_MESSAGE_SEEN) {
					/* Remember how the server's unread count will change if this
					 * command succeeds */
					if (on)
						unread_change--;
					else
						unread_change++;
				}

				/* The second round and the server doesn't support saving user flags,
				   thus store them at least locally */
				if (on && (permanentflags & CAMEL_MESSAGE_USER) == 0) {
					camel_imapx_message_info_take_server_user_flags (xinfo,
						camel_message_info_dup_user_flags (info));
				}

				g_clear_object (&info);
			}

			if (ic && imapx_uidset_done (&uidset, ic)) {
				camel_imapx_command_add (ic, " %tFLAGS.SILENT (%t)", on ? "+" : "-", flags_table[jj].name);

				success = camel_imapx_server_process_command_sync (is, ic, _("Error syncing changes"), cancellable, error);

				camel_imapx_command_unref (ic);
				ic = NULL;

				if (!success)
					break;
			}

			if (has_uidplus_capability && ic_expunge && imapx_uidset_done (&uidset_expunge, ic_expunge)) {
				success = camel_imapx_server_process_command_sync (is, ic_expunge, _("Error expunging message"), cancellable, error);

				camel_imapx_command_unref (ic_expunge);
				ic_expunge = NULL;

				if (!success)
					break;

				camel_folder_changed (folder, expunged_changes);
				camel_folder_summary_remove_uids (camel_folder_get_folder_summary (folder), expunged_removed_list);

				camel_folder_change_info_free (expunged_changes);
				expunged_changes = NULL;

				g_list_free_full (expunged_removed_list, (GDestroyNotify) camel_pstring_free);
				expunged_removed_list = NULL;
			}

			g_warn_if_fail (ic == NULL);
			g_warn_if_fail (ic_expunge == NULL);
		}

		if (user_set && (permanentflags & CAMEL_MESSAGE_USER) != 0 && success) {
			CamelIMAPXCommand *ic = NULL;

			for (jj = 0; jj < user_set->len && success; jj++) {
				struct _imapx_flag_change *c = &g_array_index (user_set, struct _imapx_flag_change, jj);

				imapx_uidset_init (&uidset, 0, 100);
				for (i = 0; i < c->infos->len; i++) {
					CamelMessageInfo *info = c->infos->pdata[i];

					/* When expunging deleted in the real Trash folder, then skip those deleted,
					   because they are gone now. */
					if (expunge_deleted && has_uidplus_capability &&
					    (((camel_message_info_get_flags (info) & CAMEL_IMAPX_SERVER_FLAGS) & permanentflags) & CAMEL_MESSAGE_DELETED) != 0) {
						if (ic && i == c->infos->len - 1 && imapx_uidset_done (&uidset, ic))
							goto store_changes;
						continue;
					}

					if (ic == NULL)
						ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_SYNC_CHANGES, "UID STORE ");

					if (imapx_uidset_add (&uidset, ic, camel_message_info_get_uid (info)) == 1
					    || (i == c->infos->len - 1 && imapx_uidset_done (&uidset, ic))) {
						gchar *utf7;
 store_changes:
						utf7 = camel_utf8_utf7 (c->name);

						camel_imapx_command_add (ic, " %tFLAGS.SILENT (%t)", on ? "+" : "-", utf7 ? utf7 : c->name);

						g_free (utf7);

						success = camel_imapx_server_process_command_sync (is, ic, _("Error syncing changes"), cancellable, error);

						camel_imapx_command_unref (ic);
						ic = NULL;

						if (!success)
							break;
					}
				}
			}

			g_warn_if_fail (ic == NULL);
		}
	}

	if (success && expunged_changes && expunge_deleted && !has_uidplus_capability) {
		CamelIMAPXCommand *ic;

		ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_EXPUNGE, "EXPUNGE");
		success = camel_imapx_server_process_command_sync (is, ic, _("Error expunging message"), cancellable, error);
		camel_imapx_command_unref (ic);
	}

	if (success && expunged_changes) {
		camel_folder_changed (folder, expunged_changes);
		camel_folder_summary_remove_uids (camel_folder_get_folder_summary (folder), expunged_removed_list);
	}

	if (expunged_changes) {
		camel_folder_change_info_free (expunged_changes);
		expunged_changes = NULL;
	}

	if (expunged_removed_list) {
		g_list_free_full (expunged_removed_list, (GDestroyNotify) camel_pstring_free);
		expunged_removed_list = NULL;
	}

	if (success) {
		CamelFolderSummary *folder_summary;
		CamelStore *parent_store;
		guint32 unseen;

		parent_store = camel_folder_get_parent_store (folder);
		folder_summary = camel_folder_get_folder_summary (folder);

		camel_folder_summary_lock (folder_summary);

		for (i = 0; i < changed_uids->len; i++) {
			CamelMessageInfo *info;
			CamelIMAPXMessageInfo *xinfo;
			gboolean set_folder_flagged;
			guint32 has_flags, set_server_flags;
			gboolean changed_meanwhile;
			const gchar *uid;

			uid = g_ptr_array_index (changed_uids, i);

			/* the 'stamps' hash table contains only those uid-s,
			   which were also flagged, not only 'dirty' */
			if (!g_hash_table_contains (stamps, uid))
				continue;

			info = camel_folder_summary_get (folder_summary, uid);
			xinfo = info ? CAMEL_IMAPX_MESSAGE_INFO (info) : NULL;

			if (!info || !xinfo) {
				g_clear_object (&info);
				continue;
			}

			camel_message_info_property_lock (info);

			changed_meanwhile = camel_message_info_get_folder_flagged_stamp (info) !=
				GPOINTER_TO_UINT (g_hash_table_lookup (stamps, uid));

			has_flags = camel_message_info_get_flags (info);
			set_server_flags = has_flags & CAMEL_IMAPX_SERVER_FLAGS;
			if (!remove_deleted_flags ||
			    !(has_flags & CAMEL_MESSAGE_DELETED)) {
				set_folder_flagged = FALSE;
			} else {
				/* to stare back the \Deleted flag */
				set_server_flags &= ~CAMEL_MESSAGE_DELETED;
				set_folder_flagged = TRUE;
			}

			if ((permanentflags & CAMEL_MESSAGE_USER) != 0 ||
			    !camel_named_flags_get_length (camel_imapx_message_info_get_server_user_flags (xinfo))) {
				camel_imapx_message_info_take_server_user_flags (xinfo, camel_message_info_dup_user_flags (info));
			}

			if (changed_meanwhile)
				set_folder_flagged = TRUE;

			camel_imapx_message_info_set_server_flags (xinfo, set_server_flags);
			camel_message_info_set_folder_flagged (info, set_folder_flagged);

			camel_message_info_property_unlock (info);
			camel_folder_summary_touch (folder_summary);
			g_clear_object (&info);
		}

		camel_folder_summary_unlock (folder_summary);

		/* Apply the changes to server-side unread count; it won't tell
		 * us of these changes, of course. */
		unseen = camel_imapx_mailbox_get_unseen (mailbox);
		unseen += unread_change;
		camel_imapx_mailbox_set_unseen (mailbox, unseen);

		if ((camel_folder_summary_get_flags (folder_summary) & CAMEL_FOLDER_SUMMARY_DIRTY) != 0) {
			CamelStoreInfo *si;

			/* ... and store's summary when folder's summary is dirty */
			si = camel_store_summary_path (CAMEL_IMAPX_STORE (parent_store)->summary, camel_folder_get_full_name (folder));
			if (si) {
				if (si->total != camel_folder_summary_get_saved_count (folder_summary) ||
				    si->unread != camel_folder_summary_get_unread_count (folder_summary)) {
					si->total = camel_folder_summary_get_saved_count (folder_summary);
					si->unread = camel_folder_summary_get_unread_count (folder_summary);
					camel_store_summary_touch (CAMEL_IMAPX_STORE (parent_store)->summary);
				}

				camel_store_summary_info_unref (CAMEL_IMAPX_STORE (parent_store)->summary, si);
			}
		}
	}

	camel_folder_summary_save (camel_folder_get_folder_summary (folder), NULL);
	camel_store_summary_save (CAMEL_IMAPX_STORE (camel_folder_get_parent_store (folder))->summary);

	imapx_sync_free_user (on_user);
	imapx_sync_free_user (off_user);
	camel_folder_free_uids (folder, changed_uids);
	g_hash_table_destroy (stamps);
	g_object_unref (folder);

	return success;
}