TextView *textview_create(void)
{
	TextView *textview;
	GtkWidget *vbox;
	GtkWidget *scrolledwin;
	GtkWidget *text;
	GtkTextBuffer *buffer;
	GtkClipboard *clipboard;
	GtkAdjustment *adj;

	debug_print("Creating text view...\n");
	textview = g_new0(TextView, 1);

	scrolledwin = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwin),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwin),
					    GTK_SHADOW_IN);
	gtk_widget_set_size_request
		(scrolledwin, prefs_common.mainview_width, -1);

	/* create GtkSText widgets for single-byte and multi-byte character */
	text = gtk_text_view_new();
	gtk_widget_add_events(text, GDK_LEAVE_NOTIFY_MASK);
	gtk_widget_show(text);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text), FALSE);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text), 6);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text), 6);

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
	clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
	gtk_text_buffer_add_selection_clipboard(buffer, clipboard);

	gtk_widget_ensure_style(text);

	g_object_ref(scrolledwin);

	gtk_container_add(GTK_CONTAINER(scrolledwin), text);

	g_signal_connect(G_OBJECT(text), "key-press-event",
			 G_CALLBACK(textview_key_pressed), textview);
	g_signal_connect(G_OBJECT(text), "motion-notify-event",
			 G_CALLBACK(textview_motion_notify), textview);
	g_signal_connect(G_OBJECT(text), "leave-notify-event",
			 G_CALLBACK(textview_leave_notify), textview);
	g_signal_connect(G_OBJECT(text), "visibility-notify-event",
			 G_CALLBACK(textview_visibility_notify), textview);
	adj = gtk_scrolled_window_get_vadjustment(
		GTK_SCROLLED_WINDOW(scrolledwin));
	g_signal_connect(G_OBJECT(adj), "value-changed",
			 G_CALLBACK(scrolled_cb), textview);
	g_signal_connect(G_OBJECT(text), "size_allocate",
			 G_CALLBACK(textview_size_allocate_cb),
			 textview);


	gtk_widget_show(scrolledwin);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwin, TRUE, TRUE, 0);

	gtk_widget_show(vbox);

	
	textview->ui_manager = gtk_ui_manager_new();
	textview->link_action_group = cm_menu_create_action_group_full(textview->ui_manager,
			"TextviewPopupLink",
			textview_link_popup_entries,
			G_N_ELEMENTS(textview_link_popup_entries), (gpointer)textview);
	textview->mail_action_group = cm_menu_create_action_group_full(textview->ui_manager,
			"TextviewPopupMail",
			textview_mail_popup_entries,
			G_N_ELEMENTS(textview_mail_popup_entries), (gpointer)textview);

	MENUITEM_ADDUI_MANAGER(textview->ui_manager, "/", "Menus", "Menus", GTK_UI_MANAGER_MENUBAR)
	MENUITEM_ADDUI_MANAGER(textview->ui_manager, 
			"/Menus", "TextviewPopupLink", "TextviewPopupLink", GTK_UI_MANAGER_MENU)
	MENUITEM_ADDUI_MANAGER(textview->ui_manager, 
			"/Menus", "TextviewPopupMail", "TextviewPopupMail", GTK_UI_MANAGER_MENU)

	MENUITEM_ADDUI_MANAGER(textview->ui_manager, 
			"/Menus/TextviewPopupLink", "Open", "TextviewPopupLink/Open", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(textview->ui_manager, 
			"/Menus/TextviewPopupLink", "Copy", "TextviewPopupLink/Copy", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(textview->ui_manager, 
			"/Menus/TextviewPopupMail", "Compose", "TextviewPopupMail/Compose", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(textview->ui_manager, 
			"/Menus/TextviewPopupMail", "ReplyTo", "TextviewPopupMail/ReplyTo", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(textview->ui_manager, 
			"/Menus/TextviewPopupMail", "AddAB", "TextviewPopupMail/AddAB", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(textview->ui_manager, 
			"/Menus/TextviewPopupMail", "Copy", "TextviewPopupMail/Copy", GTK_UI_MANAGER_MENUITEM)

	textview->link_popup_menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(
				gtk_ui_manager_get_widget(textview->ui_manager, "/Menus/TextviewPopupLink")) );
	textview->mail_popup_menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(
				gtk_ui_manager_get_widget(textview->ui_manager, "/Menus/TextviewPopupMail")) );

	textview->vbox               = vbox;
	textview->scrolledwin        = scrolledwin;
	textview->text               = text;
	textview->uri_list           = NULL;
	textview->body_pos           = 0;
	textview->last_buttonpress   = GDK_NOTHING;
	textview->image		     = NULL;
	return textview;
}