static gboolean textview_uri_button_pressed(GtkTextTag *tag, GObject *obj,
					    GdkEvent *event, GtkTextIter *iter,
					    TextView *textview)
{
	GdkEventButton *bevent;
	ClickableText *uri = NULL;
	char *tagname;
	gboolean qlink = FALSE;

	if (!event)
		return FALSE;

	if (event->type != GDK_BUTTON_PRESS && event->type != GDK_2BUTTON_PRESS
		&& event->type != GDK_MOTION_NOTIFY)
		return FALSE;

	uri = textview_get_uri(textview, iter, tag);
	if (!uri)
		return FALSE;

	g_object_get(G_OBJECT(tag), "name", &tagname, NULL);
	
	if (!strcmp(tagname, "qlink"))
		qlink = TRUE;

	g_free(tagname);
	
	bevent = (GdkEventButton *) event;
	
	/* doubleclick: open compose / add address / browser */
	if (qlink && event->type == GDK_BUTTON_PRESS && bevent->button != 1) {
		/* pass rightclick through */
		return FALSE;
	} else if ((event->type == (qlink ? GDK_2BUTTON_PRESS:GDK_BUTTON_PRESS) && bevent->button == 1) ||
		bevent->button == 2 || bevent->button == 3) {
		if (uri->filename && !g_ascii_strncasecmp(uri->filename, "sc://", 5)) {
			MimeView *mimeview = 
				(textview->messageview)?
					textview->messageview->mimeview:NULL;
			if (mimeview && bevent->button == 1) {
				mimeview_handle_cmd(mimeview, uri->filename, NULL, uri->data);
			} else if (mimeview && bevent->button == 2 && 
				!g_ascii_strcasecmp(uri->filename, "sc://select_attachment")) {
				mimeview_handle_cmd(mimeview, "sc://open_attachment", NULL, uri->data);
			} else if (mimeview && bevent->button == 3 && 
				!g_ascii_strcasecmp(uri->filename, "sc://select_attachment")) {
				mimeview_handle_cmd(mimeview, "sc://menu_attachment", bevent, uri->data);
			} 
			return TRUE;
		} else if (qlink && bevent->button == 1) {
			if (prefs_common.hide_quoted) {
				textview_toggle_quote(textview, NULL, uri, FALSE);
				return TRUE;
			} else
				return FALSE;
		} else if (!g_ascii_strncasecmp(uri->uri, "mailto:", 7)) {
			if (bevent->button == 3) {
				g_object_set_data(
					G_OBJECT(textview->mail_popup_menu),
					"menu_button", uri);
				gtk_menu_popup(GTK_MENU(textview->mail_popup_menu), 
					       NULL, NULL, NULL, NULL, 
					       bevent->button, bevent->time);
			} else {
				PrefsAccount *account = NULL;
				FolderItem   *folder_item = NULL;
				Compose *compose;
				
				if (textview->messageview && textview->messageview->msginfo &&
				    textview->messageview->msginfo->folder) {
					

					folder_item = textview->messageview->msginfo->folder;
					if (folder_item->prefs && folder_item->prefs->enable_default_account)
						account = account_find_from_id(folder_item->prefs->default_account);
					if (!account)
						account = account_find_from_item(folder_item);
				}
				compose = compose_new_with_folderitem(account,
								folder_item, uri->uri + 7);
				compose_check_for_email_account(compose);
			}
			return TRUE;
		} else if (g_ascii_strncasecmp(uri->uri, "file:", 5)) {
			if (bevent->button == 1 &&
			    textview_uri_security_check(textview, uri) == TRUE) 
					open_uri(uri->uri,
						 prefs_common_get_uri_cmd());
			else if (bevent->button == 3 && !qlink) {
				g_object_set_data(
					G_OBJECT(textview->link_popup_menu),
					"menu_button", uri);
				gtk_menu_popup(GTK_MENU(textview->link_popup_menu), 
					       NULL, NULL, NULL, NULL, 
					       bevent->button, bevent->time);
			}
			return TRUE;
		} else {
			if (bevent->button == 3 && !qlink) {
				g_object_set_data(
					G_OBJECT(textview->file_popup_menu),
					"menu_button", uri);
				gtk_menu_popup(GTK_MENU(textview->file_popup_menu), 
					       NULL, NULL, NULL, NULL, 
					       bevent->button, bevent->time);
				return TRUE;
			}
		}
	}

	return FALSE;
}