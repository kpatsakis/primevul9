imapx_unset_folder_flagged_flag (CamelFolderSummary *summary,
				 GPtrArray *changed_uids,
				 gboolean except_deleted_messages)
{
	CamelMessageInfo *info;
	gboolean changed = FALSE;
	gint ii;

	g_return_if_fail (CAMEL_IS_FOLDER_SUMMARY (summary));
	g_return_if_fail (changed_uids != NULL);

	for (ii = 0; ii < changed_uids->len; ii++) {
		info = camel_folder_summary_get (summary, changed_uids->pdata[ii]);

		if (info) {
			/* some infos could be only 'dirty' (needed to save into summary) */
			if (camel_message_info_get_folder_flagged (info) &&
			   (!except_deleted_messages || (camel_message_info_get_flags (info) & CAMEL_MESSAGE_DELETED) == 0)) {
				camel_message_info_set_folder_flagged (info, FALSE);
				changed = TRUE;
			}

			g_clear_object (&info);
		}
	}

	if (changed) {
		camel_folder_summary_touch (summary);
		camel_folder_summary_save (summary, NULL);
	}
}