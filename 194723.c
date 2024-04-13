static void copy_mail_to_uri_cb	(GtkAction *action, TextView *textview)
{
	ClickableText *uri = g_object_get_data(G_OBJECT(textview->mail_popup_menu),
					   "menu_button");
	if (uri == NULL)
		return;

	gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_PRIMARY), uri->uri +7, -1);
	gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), uri->uri +7, -1);
	g_object_set_data(G_OBJECT(textview->mail_popup_menu), "menu_button",
			  NULL);
}