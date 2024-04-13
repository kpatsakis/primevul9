static void copy_uri_cb	(GtkAction *action, TextView *textview)
{
	ClickableText *uri = g_object_get_data(G_OBJECT(textview->link_popup_menu),
					   "menu_button");
	const gchar *raw_url =  g_object_get_data(G_OBJECT(textview->link_popup_menu),
					   "raw_url");
	if (uri) {
		if (textview_uri_security_check(textview, uri) == TRUE) {
			gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_PRIMARY), uri->uri, -1);
			gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), uri->uri, -1);
			g_object_set_data(G_OBJECT(textview->link_popup_menu), "menu_button", NULL);
		}
	}
	if (raw_url) {
		gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_PRIMARY), raw_url, -1);
		gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), raw_url, -1);
		g_object_set_data(G_OBJECT(textview->link_popup_menu), "raw_url", NULL);
	}
}