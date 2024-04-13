imapx_gather_existing_uids_cb (guint32 uid,
			       gpointer user_data)
{
	GatherExistingUidsData *geud = user_data;
	gchar *uid_str;

	g_return_val_if_fail (geud != NULL, FALSE);
	g_return_val_if_fail (geud->is != NULL, FALSE);
	g_return_val_if_fail (geud->summary != NULL, FALSE);

	geud->n_uids++;

	uid_str = g_strdup_printf ("%u", uid);

	if (camel_folder_summary_check_uid (geud->summary, uid_str)) {
		e (geud->is->priv->tagprefix, "vanished known UID: %u\n", uid);
		if (!geud->uid_list)
			g_mutex_lock (&geud->is->priv->changes_lock);

		geud->uid_list = g_list_prepend (geud->uid_list, uid_str);
		camel_folder_change_info_remove_uid (geud->is->priv->changes, uid_str);
	} else {
		e (geud->is->priv->tagprefix, "vanished unknown UID: %u\n", uid);
		g_free (uid_str);
	}

	return TRUE;
}