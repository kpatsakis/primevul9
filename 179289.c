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
			CamelMessageInfoBase *mi = (CamelMessageInfoBase *) info;

			/* some infos could be only 'dirty' (needed to save into summary) */
			if ((mi->flags & CAMEL_MESSAGE_FOLDER_FLAGGED) != 0 &&
			   (!except_deleted_messages || (mi->flags & CAMEL_MESSAGE_DELETED) == 0)) {
				mi->flags &= ~CAMEL_MESSAGE_FOLDER_FLAGGED;
				mi->dirty = TRUE;
				changed = TRUE;
			}

			camel_message_info_unref (info);
		}
	}

	if (changed) {
		camel_folder_summary_touch (summary);
		camel_folder_summary_save_to_db (summary, NULL);
	}
}