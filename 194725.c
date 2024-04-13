static void mail_to_uri_cb (GtkAction *action, TextView *textview)
{
	PrefsAccount *account = NULL;
	Compose *compose;
	ClickableText *uri = g_object_get_data(G_OBJECT(textview->mail_popup_menu),
					   "menu_button");
	if (uri == NULL)
		return;

	if (textview->messageview && textview->messageview->msginfo &&
	    textview->messageview->msginfo->folder) {
		FolderItem   *folder_item;

		folder_item = textview->messageview->msginfo->folder;
		if (folder_item->prefs && folder_item->prefs->enable_default_account)
			account = account_find_from_id(folder_item->prefs->default_account);
		
		compose = compose_new_with_folderitem(account, folder_item, uri->uri+7);
	} else {
		compose = compose_new(account, uri->uri + 7, NULL);
	}
	compose_check_for_email_account(compose);
}