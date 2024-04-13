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
	CamelIMAPXMessageInfo *info;
	GHashTable *changed_meanwhile;
	gulong changed_meanwhile_handler_id;
	guint32 permanentflags;
	struct _uidset_state uidset;
	gint unread_change = 0;
	gboolean use_real_junk_path = FALSE;
	gboolean use_real_trash_path = FALSE;
	gboolean remove_deleted_flags = FALSE;
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
	changed_uids = camel_folder_summary_get_changed (folder->summary);

	if (changed_uids->len == 0) {
		camel_folder_free_uids (folder, changed_uids);
		g_object_unref (folder);
		return TRUE;
	}

	changed_meanwhile = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) camel_pstring_free, NULL);
	changed_meanwhile_handler_id = g_signal_connect (folder->summary, "info-changed",
		G_CALLBACK (imapx_server_info_changed_cb), changed_meanwhile);

	if (can_influence_flags) {
		CamelIMAPXSettings *settings;

		settings = camel_imapx_server_ref_settings (is);
		use_real_junk_path = camel_imapx_settings_get_use_real_junk_path (settings);
		use_real_trash_path = camel_imapx_settings_get_use_real_trash_path (settings);
		if (use_real_trash_path) {
			CamelFolder *trash_folder = NULL;
			gchar *real_trash_path;

			real_trash_path = camel_imapx_settings_dup_real_trash_path (settings);
			if (real_trash_path)
				trash_folder = camel_store_get_folder_sync (
					camel_folder_get_parent_store (folder),
					real_trash_path, 0, cancellable, NULL);

			/* Remove deleted flags in all but the trash folder itself */
			remove_deleted_flags = !trash_folder || trash_folder != folder;

			use_real_trash_path = trash_folder != NULL;

			g_clear_object (&trash_folder);
			g_free (real_trash_path);
		}
		g_object_unref (settings);
	}

	if (changed_uids->len > 20)
		camel_folder_summary_prepare_fetch_all (folder->summary, NULL);

	off_orset = on_orset = 0;
	for (i = 0; i < changed_uids->len; i++) {
		guint32 flags, sflags;
		CamelFlag *uflags, *suflags;
		const gchar *uid;
		guint j = 0;

		uid = g_ptr_array_index (changed_uids, i);

		info = (CamelIMAPXMessageInfo *)
			camel_folder_summary_get (folder->summary, uid);

		if (info == NULL)
			continue;

		if (!(info->info.flags & CAMEL_MESSAGE_FOLDER_FLAGGED)) {
			camel_message_info_unref (info);
			continue;
		}

		flags = info->info.flags & CAMEL_IMAPX_SERVER_FLAGS;
		sflags = info->server_flags & CAMEL_IMAPX_SERVER_FLAGS;

		if (can_influence_flags) {
			gboolean move_to_real_junk;
			gboolean move_to_real_trash;

			move_to_real_junk =
				use_real_junk_path &&
				(flags & CAMEL_MESSAGE_JUNK);

			move_to_real_trash =
				use_real_trash_path && remove_deleted_flags &&
				(flags & CAMEL_MESSAGE_DELETED);

			if (move_to_real_junk)
				camel_imapx_folder_add_move_to_real_junk (
					CAMEL_IMAPX_FOLDER (folder), uid);

			if (move_to_real_trash)
				camel_imapx_folder_add_move_to_real_trash (
					CAMEL_IMAPX_FOLDER (folder), uid);
		}

		if (flags != sflags) {
			off_orset |= (flags ^ sflags) & ~flags;
			on_orset |= (flags ^ sflags) & flags;
		}

		uflags = info->info.user_flags;
		suflags = info->server_user_flags;
		while (uflags || suflags) {
			gint res;

			if (uflags) {
				if (suflags)
					res = strcmp (uflags->name, suflags->name);
				else if (*uflags->name)
					res = -1;
				else {
					uflags = uflags->next;
					continue;
				}
			} else {
				res = 1;
			}

			if (res == 0) {
				uflags = uflags->next;
				suflags = suflags->next;
			} else {
				GArray *user_set;
				CamelFlag *user_flag;
				struct _imapx_flag_change *change = NULL, add = { 0 };

				if (res < 0) {
					if (on_user == NULL)
						on_user = g_array_new (FALSE, FALSE, sizeof (struct _imapx_flag_change));
					user_set = on_user;
					user_flag = uflags;
					uflags = uflags->next;
				} else {
					if (off_user == NULL)
						off_user = g_array_new (FALSE, FALSE, sizeof (struct _imapx_flag_change));
					user_set = off_user;
					user_flag = suflags;
					suflags = suflags->next;
				}

				/* Could sort this and binary search */
				for (j = 0; j < user_set->len; j++) {
					change = &g_array_index (user_set, struct _imapx_flag_change, j);
					if (strcmp (change->name, user_flag->name) == 0)
						goto found;
				}
				add.name = g_strdup (user_flag->name);
				add.infos = g_ptr_array_new ();
				g_array_append_val (user_set, add);
				change = &add;
			found:
				camel_message_info_ref (info);
				g_ptr_array_add (change->infos, info);
			}
		}

		camel_message_info_unref (info);
	}

	nothing_to_do =
		(on_orset == 0) &&
		(off_orset == 0) &&
		(on_user == NULL) &&
		(off_user == NULL);

	if (nothing_to_do) {
		g_signal_handler_disconnect (folder->summary, changed_meanwhile_handler_id);

		imapx_sync_free_user (on_user);
		imapx_sync_free_user (off_user);
		imapx_unset_folder_flagged_flag (folder->summary, changed_uids, remove_deleted_flags);
		camel_folder_free_uids (folder, changed_uids);
		g_hash_table_destroy (changed_meanwhile);
		g_object_unref (folder);
		return TRUE;
	}

	if (!camel_imapx_server_ensure_selected_sync (is, mailbox, cancellable, error)) {
		g_signal_handler_disconnect (folder->summary, changed_meanwhile_handler_id);

		imapx_sync_free_user (on_user);
		imapx_sync_free_user (off_user);
		camel_folder_free_uids (folder, changed_uids);
		g_hash_table_destroy (changed_meanwhile);
		g_object_unref (folder);
		return FALSE;
	}

	permanentflags = camel_imapx_mailbox_get_permanentflags (mailbox);

	success = TRUE;
	for (on = 0; on < 2 && success; on++) {
		guint32 orset = on ? on_orset : off_orset;
		GArray *user_set = on ? on_user : off_user;

		for (jj = 0; jj < G_N_ELEMENTS (flags_table) && success; jj++) {
			guint32 flag = flags_table[jj].flag;
			CamelIMAPXCommand *ic = NULL;

			if ((orset & flag) == 0)
				continue;

			c (is->priv->tagprefix, "checking/storing %s flags '%s'\n", on ? "on" : "off", flags_table[jj].name);
			imapx_uidset_init (&uidset, 0, 100);
			for (i = 0; i < changed_uids->len && success; i++) {
				CamelIMAPXMessageInfo *info;
				gboolean remove_deleted_flag;
				guint32 flags;
				guint32 sflags;
				gint send;

				info = (CamelIMAPXMessageInfo *)
					camel_folder_summary_get (
						folder->summary,
						changed_uids->pdata[i]);

				if (info == NULL)
					continue;

				flags = (info->info.flags & CAMEL_IMAPX_SERVER_FLAGS) & permanentflags;
				sflags = (info->server_flags & CAMEL_IMAPX_SERVER_FLAGS) & permanentflags;
				send = 0;

				remove_deleted_flag =
					remove_deleted_flags &&
					(flags & CAMEL_MESSAGE_DELETED);

				if (remove_deleted_flag) {
					/* Remove the DELETED flag so the
					 * message appears normally in the
					 * real Trash folder when copied. */
					flags &= ~CAMEL_MESSAGE_DELETED;
				}

				if ( (on && (((flags ^ sflags) & flags) & flag))
				     || (!on && (((flags ^ sflags) & ~flags) & flag))) {
					if (ic == NULL) {
						ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_SYNC_CHANGES, "UID STORE ");
					}
					send = imapx_uidset_add (&uidset, ic, camel_message_info_uid (info));
				}
				if (send == 1 || (i == changed_uids->len - 1 && ic && imapx_uidset_done (&uidset, ic))) {
					camel_imapx_command_add (ic, " %tFLAGS.SILENT (%t)", on ? "+" : "-", flags_table[jj].name);

					success = camel_imapx_server_process_command_sync (is, ic, _("Error syncing changes"), cancellable, error);

					camel_imapx_command_unref (ic);
					ic = NULL;

					if (!success)
						break;
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
					camel_flag_list_copy (&info->server_user_flags, &info->info.user_flags);
				}

				camel_message_info_unref (info);
			}

			g_warn_if_fail (ic == NULL);
		}

		if (user_set && (permanentflags & CAMEL_MESSAGE_USER) != 0 && success) {
			CamelIMAPXCommand *ic = NULL;

			for (jj = 0; jj < user_set->len && success; jj++) {
				struct _imapx_flag_change *c = &g_array_index (user_set, struct _imapx_flag_change, jj);

				imapx_uidset_init (&uidset, 0, 100);
				for (i = 0; i < c->infos->len; i++) {
					CamelIMAPXMessageInfo *info = c->infos->pdata[i];

					if (ic == NULL)
						ic = camel_imapx_command_new (is, CAMEL_IMAPX_JOB_SYNC_CHANGES, "UID STORE ");

					if (imapx_uidset_add (&uidset, ic, camel_message_info_uid (info)) == 1
					    || (i == c->infos->len - 1 && imapx_uidset_done (&uidset, ic))) {
						gchar *utf7;

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
		}
	}

	g_signal_handler_disconnect (folder->summary, changed_meanwhile_handler_id);

	if (success) {
		CamelStore *parent_store;
		guint32 unseen;

		parent_store = camel_folder_get_parent_store (folder);

		camel_folder_summary_lock (folder->summary);

		for (i = 0; i < changed_uids->len; i++) {
			CamelIMAPXMessageInfo *xinfo = (CamelIMAPXMessageInfo *) camel_folder_summary_get (folder->summary,
					changed_uids->pdata[i]);

			if (!xinfo)
				continue;

			xinfo->server_flags = xinfo->info.flags & CAMEL_IMAPX_SERVER_FLAGS;
			if (!remove_deleted_flags ||
			    !(xinfo->info.flags & CAMEL_MESSAGE_DELETED)) {
				xinfo->info.flags &= ~CAMEL_MESSAGE_FOLDER_FLAGGED;
			} else {
				/* to stare back the \Deleted flag */
				xinfo->server_flags &= ~CAMEL_MESSAGE_DELETED;
				xinfo->info.flags |= CAMEL_MESSAGE_FOLDER_FLAGGED;
			}
			xinfo->info.dirty = TRUE;
			if ((permanentflags & CAMEL_MESSAGE_USER) != 0 ||
			    camel_flag_list_size (&xinfo->server_user_flags) == 0)
				camel_flag_list_copy (&xinfo->server_user_flags, &xinfo->info.user_flags);

			if (g_hash_table_lookup (changed_meanwhile, changed_uids->pdata[i]))
				xinfo->info.flags |= CAMEL_MESSAGE_FOLDER_FLAGGED;

			camel_folder_summary_touch (folder->summary);
			camel_message_info_unref (xinfo);
		}

		camel_folder_summary_unlock (folder->summary);

		/* Apply the changes to server-side unread count; it won't tell
		 * us of these changes, of course. */
		unseen = camel_imapx_mailbox_get_unseen (mailbox);
		unseen += unread_change;
		camel_imapx_mailbox_set_unseen (mailbox, unseen);

		if (folder->summary && (folder->summary->flags & CAMEL_FOLDER_SUMMARY_DIRTY) != 0) {
			CamelStoreInfo *si;

			/* ... and store's summary when folder's summary is dirty */
			si = camel_store_summary_path (CAMEL_IMAPX_STORE (parent_store)->summary, camel_folder_get_full_name (folder));
			if (si) {
				if (si->total != camel_folder_summary_get_saved_count (folder->summary) ||
				    si->unread != camel_folder_summary_get_unread_count (folder->summary)) {
					si->total = camel_folder_summary_get_saved_count (folder->summary);
					si->unread = camel_folder_summary_get_unread_count (folder->summary);
					camel_store_summary_touch (CAMEL_IMAPX_STORE (parent_store)->summary);
				}

				camel_store_summary_info_unref (CAMEL_IMAPX_STORE (parent_store)->summary, si);
			}
		}

		camel_folder_summary_save_to_db (folder->summary, NULL);
		camel_store_summary_save (CAMEL_IMAPX_STORE (parent_store)->summary);
	}

	imapx_sync_free_user (on_user);
	imapx_sync_free_user (off_user);
	camel_folder_free_uids (folder, changed_uids);
	g_hash_table_destroy (changed_meanwhile);
	g_object_unref (folder);

	return success;
}