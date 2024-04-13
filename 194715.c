static void open_uri_cb (GtkAction *action, TextView *textview)
{
	ClickableText *uri = g_object_get_data(G_OBJECT(textview->link_popup_menu),
					   "menu_button");
	const gchar *raw_url = g_object_get_data(G_OBJECT(textview->link_popup_menu),
					   "raw_url");

	if (uri) {
		if (textview_uri_security_check(textview, uri) == TRUE) 
			open_uri(uri->uri,
				 prefs_common_get_uri_cmd());
		g_object_set_data(G_OBJECT(textview->link_popup_menu), "menu_button",
				  NULL);
	}
	if (raw_url) {
		open_uri(raw_url, prefs_common_get_uri_cmd());
		g_object_set_data(G_OBJECT(textview->link_popup_menu), "raw_url",
				  NULL);
	}
}