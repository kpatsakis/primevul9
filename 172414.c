imapx_can_refresh_mailbox_in_idle (CamelIMAPXServer *imapx_server,
				   CamelIMAPXStore *imapx_store,
				   CamelIMAPXMailbox *mailbox)
{
	CamelIMAPXSettings *imapx_settings;
	gboolean can_refresh = FALSE;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (imapx_server), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_STORE (imapx_store), FALSE);
	g_return_val_if_fail (CAMEL_IS_IMAPX_MAILBOX (mailbox), FALSE);

	if (camel_imapx_mailbox_is_inbox (camel_imapx_mailbox_get_name (mailbox)))
		return TRUE;

	imapx_settings = camel_imapx_server_ref_settings (imapx_server);

	if (camel_imapx_settings_get_use_subscriptions (imapx_settings)) {
		can_refresh = camel_imapx_mailbox_has_attribute (mailbox, CAMEL_IMAPX_LIST_ATTR_SUBSCRIBED);
	} else if (camel_imapx_settings_get_check_all (imapx_settings)) {
		can_refresh = TRUE;
	} else if (camel_imapx_settings_get_check_subscribed (imapx_settings)) {
		can_refresh = camel_imapx_mailbox_has_attribute (mailbox, CAMEL_IMAPX_LIST_ATTR_SUBSCRIBED);
	}

	if (!can_refresh &&
	    !camel_imapx_settings_get_use_subscriptions (imapx_settings)) {
		/* Refresh opened folders when viewing both subscribed and unsubscribed,
		   even if they would not be refreshed otherwise. */
		gchar *folder_path;

		can_refresh = FALSE;

		folder_path = camel_imapx_mailbox_dup_folder_path (mailbox);
		if (folder_path) {
			GPtrArray *opened_folders = camel_store_dup_opened_folders (CAMEL_STORE (imapx_store));

			if (opened_folders) {
				gint ii;

				for (ii = 0; !can_refresh && ii < opened_folders->len; ii++) {
					CamelFolder *folder = g_ptr_array_index (opened_folders, ii);

					can_refresh = g_strcmp0 (camel_folder_get_full_name (folder), folder_path) == 0;
				}

				g_ptr_array_foreach (opened_folders, (GFunc) g_object_unref, NULL);
				g_ptr_array_free (opened_folders, TRUE);
			}
		}

		g_free (folder_path);
	}

	g_clear_object (&imapx_settings);

	return can_refresh;
}